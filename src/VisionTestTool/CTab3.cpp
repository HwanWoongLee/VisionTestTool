﻿// CTab3.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "CTab3.h"
#include "afxdialogex.h"
#include "ImageProcess.h"
#include "VisionTestToolDlg.h"
#include "hazeremoval.h"
#include <opencv2/bioinspired.hpp>

// CTab3 대화 상자

IMPLEMENT_DYNAMIC(CTab3, CDialogEx)

CTab3::CTab3(CWnd* pParent /*=nullptr*/)
	: CTab1(pParent)
{
	m_pParent = (CVisionTestToolDlg*)pParent;
	
}

CTab3::~CTab3()
{
}

void CTab3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DH_RADIUS, m_editDHRadius);
	DDX_Control(pDX, IDC_EDIT_DH_OMEGA, m_editDHOmega);
	DDX_Control(pDX, IDC_EDIT_DH_T0, m_editDHT0);
	DDX_Control(pDX, IDC_EDIT_DH_R, m_editDHR);
	DDX_Control(pDX, IDC_EDIT_DH_EPS, m_editDHEPS);
}


BEGIN_MESSAGE_MAP(CTab3, CDialogEx)
	ON_WM_HSCROLL()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_DEHAZE, &CTab3::OnBnClickedBtnDehaze)
	ON_BN_CLICKED(IDC_BTN_RETINA, &CTab3::OnBnClickedBtnRetina)
	ON_BN_CLICKED(IDC_BTN_ILLUM, &CTab3::OnBnClickedBtnIllum)
END_MESSAGE_MAP()


void CTab3::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		// Haze Removal
		if (!m_pHazeRemoval) {
			m_pHazeRemoval = new CHazeRemoval();

			m_editDHRadius.SetWindowTextW(_T("7"));
			m_editDHOmega.SetWindowTextW(_T("0.95"));
			m_editDHT0.SetWindowTextW(_T("0.1"));
			m_editDHR.SetWindowTextW(_T("60"));
			m_editDHEPS.SetWindowTextW(_T("0.001"));
		}
    }
	return;
}


void CTab3::OnBnClickedBtnDehaze()
{
	Mat in_img = GetImage();
	if (in_img.empty())
		return;
	
	if (!m_pHazeRemoval)
		return;

	CString str;
	m_editDHRadius.GetWindowTextW(str);
	int radius		= _ttoi(str);
	m_editDHOmega.GetWindowTextW(str);
	double omega	= _ttof(str);
	m_editDHT0.GetWindowTextW(str);
	double t0		= _ttof(str);
	m_editDHR.GetWindowTextW(str);
	int r			= _ttoi(str);
	m_editDHEPS.GetWindowTextW(str);
	double eps		= _ttof(str);


	Mat out_img(in_img.rows, in_img.cols, CV_8UC3);
	unsigned char* indata = in_img.data;
	unsigned char* outdata = out_img.data;

	m_pHazeRemoval->InitProc(in_img.cols, in_img.rows, in_img.channels());
	m_pHazeRemoval->InitPrams(radius, omega, t0, r, eps);
	m_pHazeRemoval->Process(indata, outdata, in_img.cols, in_img.rows, in_img.channels());

	SetImage(out_img);
}


void CTab3::OnBnClickedBtnRetina()
{
	cv::Mat image = GetImage();

	cv::Ptr<cv::bioinspired::Retina> myRetina = cv::bioinspired::Retina::create(image.size());
	myRetina->write("RetinaDefaultParameters.xml");
	myRetina->setup("RetinaDefaultParameters.xml");
	myRetina->clearBuffers();

	cv::Mat retinaOutput_parvo;
	cv::Mat retinaOutput_magno;

	// run retina on the input image
	myRetina->run(image);
	// grab retina outputs
	myRetina->getParvo(retinaOutput_parvo);
	myRetina->getMagno(retinaOutput_magno);

	SetImage(retinaOutput_parvo);
}


std::pair<cv::Mat, cv::Mat> get_illumination_channel(cv::Mat I, float w) {
	int N = I.size[0];
	int M = I.size[1];
	cv::Mat darkch = cv::Mat::zeros(cv::Size(M, N), CV_32FC1);
	cv::Mat brightch = cv::Mat::zeros(cv::Size(M, N), CV_32FC1);

	int padding = int(w / 2);
	// padding for channels
	cv::Mat padded = cv::Mat::zeros(cv::Size(M + 2 * padding, N + 2 * padding), CV_32FC3);

	for (int i = padding; i < padding + M; i++) {
		for (int j = padding; j < padding + N; j++) {
			padded.at<cv::Vec3f>(j, i).val[0] = (float)I.at<cv::Vec3b>(j - padding, i - padding).val[0] / 255;
			padded.at<cv::Vec3f>(j, i).val[1] = (float)I.at<cv::Vec3b>(j - padding, i - padding).val[1] / 255;
			padded.at<cv::Vec3f>(j, i).val[2] = (float)I.at<cv::Vec3b>(j - padding, i - padding).val[2] / 255;
		}
	}

	for (int i = 0; i < darkch.size[1]; i++) {
		int col_up, row_up;

		col_up = int(i + w);

		for (int j = 0; j < darkch.size[0]; j++) {
			double minVal, maxVal;

			row_up = int(j + w);

			cv::minMaxLoc(padded.colRange(i, col_up).rowRange(j, row_up), &minVal, &maxVal);

			darkch.at<float>(j, i) = minVal; //dark channel
			brightch.at<float>(j, i) = maxVal; //bright channel
		}
	}

	return std::make_pair(darkch, brightch);
}
cv::Mat get_atmosphere(cv::Mat I, cv::Mat brightch, float p = 0.1) {
	int N = brightch.size[0];
	int M = brightch.size[1];

	// flattening and reshaping image array
	cv::Mat flatI(cv::Size(1, N*M), CV_8UC3);
	std::vector<std::pair<float, int>> flatBright;

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			int index = i * N + j;
			flatI.at<cv::Vec3b>(index, 0).val[0] = I.at<cv::Vec3b>(j, i).val[0];
			flatI.at<cv::Vec3b>(index, 0).val[1] = I.at<cv::Vec3b>(j, i).val[1];
			flatI.at<cv::Vec3b>(index, 0).val[2] = I.at<cv::Vec3b>(j, i).val[2];

			flatBright.push_back(std::make_pair(-brightch.at<float>(j, i), index));
		}
	}


	// sorting and slicing the array
	sort(flatBright.begin(), flatBright.end());

	cv::Mat A = cv::Mat::zeros(cv::Size(1, 3), CV_32FC1);

	for (int k = 0; k < int(M*N*p); k++) {
		int sindex = flatBright[k].second;
		A.at<float>(0, 0) = A.at<float>(0, 0) + (float)flatI.at<cv::Vec3b>(sindex, 0).val[0];
		A.at<float>(1, 0) = A.at<float>(1, 0) + (float)flatI.at<cv::Vec3b>(sindex, 0).val[1];
		A.at<float>(2, 0) = A.at<float>(2, 0) + (float)flatI.at<cv::Vec3b>(sindex, 0).val[2];
	}

	A = A / int(M*N*p);

	return A / 255;
}
cv::Mat get_initial_transmission(cv::Mat A, cv::Mat brightch) {
	double A_n, A_x, minVal, maxVal;
	cv::minMaxLoc(A, &A_n, &A_x);
	cv::Mat init_t(brightch.size(), CV_32FC1);
	init_t = brightch.clone();
	// finding initial transmission map
	init_t = (init_t - A_x) / (1.0 - A_x);
	cv::minMaxLoc(init_t, &minVal, &maxVal);
	// normalized initial transmission map
	init_t = (init_t - minVal) / (maxVal - minVal);

	return init_t;
}
cv::Mat reduce_init_t(cv::Mat init_t) {
	cv::Mat mod_init_t(init_t.size(), CV_8UC1);

	for (int i = 0; i < init_t.size[1]; i++) {
		for (int j = 0; j < init_t.size[0]; j++) {
			mod_init_t.at<uchar>(j, i) = std::min((int)(init_t.at<float>(j, i) * 255), 255);
		}
	}

	int x[3] = { 0, 32, 255 };
	int f[3] = { 0, 32, 48 };

	// creating array [0,...,255]
	cv::Mat table(cv::Size(1, 256), CV_8UC1);

	//Linear Interpolation
	int l = 0;
	for (int k = 0; k < 256; k++) {
		if (k > x[l + 1]) {
			l = l + 1;
		}

		float m = (float)(f[l + 1] - f[l]) / (x[l + 1] - x[l]);
		table.at<int>(k, 0) = (int)(f[l] + m * (k - x[l]));
	}

	//Lookup table
	cv::LUT(mod_init_t, table, mod_init_t);

	for (int i = 0; i < init_t.size[1]; i++) {
		for (int j = 0; j < init_t.size[0]; j++) {
			// normalizing the transmission map
			init_t.at<float>(j, i) = (float)mod_init_t.at<uchar>(j, i) / 255;
		}
	}

	return init_t;
}
cv::Mat get_corrected_transmission(cv::Mat I, cv::Mat A, cv::Mat darkch, cv::Mat brightch, cv::Mat init_t, float alpha, float omega, int w) {
	cv::Mat im3(I.size(), CV_32FC3);
	//divide pixel values by atmospheric light
	for (int i = 0; i < I.size[1]; i++) {
		for (int j = 0; j < I.size[0]; j++) {
			im3.at<cv::Vec3f>(j, i).val[0] = (float)I.at<cv::Vec3b>(j, i).val[0] / A.at<float>(0, 0);
			im3.at<cv::Vec3f>(j, i).val[1] = (float)I.at<cv::Vec3b>(j, i).val[1] / A.at<float>(1, 0);
			im3.at<cv::Vec3f>(j, i).val[2] = (float)I.at<cv::Vec3b>(j, i).val[2] / A.at<float>(2, 0);
		}
	}

	cv::Mat dark_c, dark_t, diffch;

	std::pair<cv::Mat, cv::Mat> illuminate_channels = get_illumination_channel(im3, w);
	// dark channel transmission map
	dark_c = illuminate_channels.first;
	// corrected dark transmission map
	dark_t = 1 - omega * dark_c;
	cv::Mat corrected_t = init_t;
	diffch = brightch - darkch; //difference between transmission maps

	for (int i = 0; i < diffch.size[1]; i++) {
		for (int j = 0; j < diffch.size[0]; j++) {
			if (diffch.at<float>(j, i) < alpha) {
				// initializing corrected transmission map with initial transmission map
				corrected_t.at<float>(j, i) = abs(dark_t.at<float>(j, i)*init_t.at<float>(j, i));
			}
		}
	}

	return corrected_t;
}
cv::Mat get_final_image(cv::Mat I, cv::Mat A, cv::Mat refined_t, float tmin) {
	cv::Mat J(I.size(), CV_32FC3);

	for (int i = 0; i < refined_t.size[1]; i++) {
		for (int j = 0; j < refined_t.size[0]; j++) {
			float temp = refined_t.at<float>(j, i);

			if (temp < tmin) {
				temp = tmin;
			}
			// finding result
			J.at<cv::Vec3f>(j, i).val[0] = (I.at<cv::Vec3f>(j, i).val[0] - A.at<float>(0, 0)) / temp + A.at<float>(0, 0);
			J.at<cv::Vec3f>(j, i).val[1] = (I.at<cv::Vec3f>(j, i).val[1] - A.at<float>(1, 0)) / temp + A.at<float>(1, 0);
			J.at<cv::Vec3f>(j, i).val[2] = (I.at<cv::Vec3f>(j, i).val[2] - A.at<float>(2, 0)) / temp + A.at<float>(2, 0);
		}
	}

	double minVal, maxVal;
	cv::minMaxLoc(J, &minVal, &maxVal);

	// normalized image
	for (int i = 0; i < J.size[1]; i++) {
		for (int j = 0; j < J.size[0]; j++) {
			J.at<cv::Vec3f>(j, i).val[0] = (J.at<cv::Vec3f>(j, i).val[0] - minVal) / (maxVal - minVal);
			J.at<cv::Vec3f>(j, i).val[1] = (J.at<cv::Vec3f>(j, i).val[1] - minVal) / (maxVal - minVal);
			J.at<cv::Vec3f>(j, i).val[2] = (J.at<cv::Vec3f>(j, i).val[2] - minVal) / (maxVal - minVal);
		}
	}

	return J;
}

void CTab3::OnBnClickedBtnIllum()
{
	cv::Mat img = GetImage();

	float tmin = 0.1;
	int w = 15;
	float alpha = 0.4;
	float omega = 0.75;
	float p = 0.1;
	double eps = 1e-3;
	bool reduce = false;

	std::pair<cv::Mat, cv::Mat> illuminate_channels = get_illumination_channel(img, w);

	cv::Mat Idark = illuminate_channels.first;
	cv::Mat Ibright = illuminate_channels.second;

	cv::Mat A = get_atmosphere(img, Ibright);

	cv::Mat init_t = get_initial_transmission(A, Ibright);

	if (reduce) {
		init_t = reduce_init_t(init_t);
	}

	double minVal, maxVal;
	// Convert the input to a float array
	cv::Mat I(img.size(), CV_32FC3), normI;

	for (int i = 0; i < img.size[1]; i++) {
		for (int j = 0; j < img.size[0]; j++) {
			I.at<cv::Vec3f>(j, i).val[0] = (float)img.at<cv::Vec3b>(j, i).val[0] / 255;
			I.at<cv::Vec3f>(j, i).val[1] = (float)img.at<cv::Vec3b>(j, i).val[1] / 255;
			I.at<cv::Vec3f>(j, i).val[2] = (float)img.at<cv::Vec3b>(j, i).val[2] / 255;
		}
	}

	cv::minMaxLoc(I, &minVal, &maxVal);

	normI = (I - minVal) / (maxVal - minVal);

	cv::Mat corrected_t = get_corrected_transmission(img, A, Idark, Ibright, init_t, alpha, omega, w);

	cv::Mat refined_t(normI.size(), CV_32FC1);
	// applying guided filter
	refined_t = guidedFilter(normI, corrected_t, w, eps);

	cv::Mat J_refined = get_final_image(I, A, refined_t, tmin);

	cv::Mat enhanced(img.size(), CV_8UC3);

	for (int i = 0; i < img.size[1]; i++) {
		for (int j = 0; j < img.size[0]; j++) {
			enhanced.at<cv::Vec3b>(j, i).val[0] = std::min((int)(J_refined.at<cv::Vec3f>(j, i).val[0] * 255), 255);
			enhanced.at<cv::Vec3b>(j, i).val[1] = std::min((int)(J_refined.at<cv::Vec3f>(j, i).val[1] * 255), 255);
			enhanced.at<cv::Vec3b>(j, i).val[2] = std::min((int)(J_refined.at<cv::Vec3f>(j, i).val[2] * 255), 255);
		}
	}

	cv::Mat f_enhanced;

	cv::detailEnhance(enhanced, f_enhanced, 10, 0.15);
	cv::edgePreservingFilter(f_enhanced, f_enhanced, 1, 64, 0.2);

	SetImage(f_enhanced);
}
