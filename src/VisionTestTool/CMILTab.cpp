// CMILTab.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "CMILTab.h"
#include "afxdialogex.h"


// CMILTab 대화 상자

IMPLEMENT_DYNAMIC(CMILTab, CDialogEx)

CMILTab::CMILTab(CWnd* pParent /*=nullptr*/)
	: CTab1(pParent)
{

}

CMILTab::~CMILTab()
{
}

void CMILTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MIL_GMF_PATH, m_editMMFPath);
	DDX_Control(pDX, IDC_EDIT_MIL_STR_PATH, m_editStrPath);
	DDX_Control(pDX, IDC_EDIT_MIL_MCO_PATH, m_editMCOPath);
}


BEGIN_MESSAGE_MAP(CMILTab, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_MMF_LOAD, &CMILTab::OnBnClickedBtnMmfLoad)
	ON_BN_CLICKED(IDC_BTN_MODEL_FIND, &CMILTab::OnBnClickedBtnModelFind)
	ON_BN_CLICKED(IDC_BTN_MSR_LOAD, &CMILTab::OnBnClickedBtnMsrLoad)
	ON_BN_CLICKED(IDC_BTN_STRING_READ, &CMILTab::OnBnClickedBtnStringRead)
	ON_BN_CLICKED(IDC_BTN_MCO_LOAD, &CMILTab::OnBnClickedBtnMcoLoad)
	ON_BN_CLICKED(IDC_BTN_CODE_READ, &CMILTab::OnBnClickedBtnCodeRead)
END_MESSAGE_MAP()


// CMILTab 메시지 처리기


void CMILTab::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		if (!m_pMIL) {
			m_pMIL = new CMILModule();
		}
	}
}


void CMILTab::OnBnClickedBtnMmfLoad()
{
	static TCHAR BASED_CODE szFilter[] = _T("mmf 파일(*.mmf) | *.mmf; |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.mmf"), _T("mmf"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		if (!pathName.IsEmpty()) {
			m_editMMFPath.SetWindowTextW(pathName);

			if (!m_pMIL->SetModelFinderContext(CONV_A(pathName))) {
				MessageBox(_T("Context Load Error"));
			}
		}
	}
	return;
}

void CMILTab::OnBnClickedBtnMsrLoad()
{
	static TCHAR BASED_CODE szFilter[] = _T("msr 파일(*.msr) | *.msr; |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.msr"), _T("msr"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		if (!pathName.IsEmpty()) {
			m_editStrPath.SetWindowTextW(pathName);

			if (!m_pMIL->SetStringReaderContext(CONV_A(pathName))) {
				MessageBox(_T("Context Load Error"));
			}
		}
	}
	return;
}

void CMILTab::OnBnClickedBtnMcoLoad()
{
	static TCHAR BASED_CODE szFilter[] = _T("mco 파일(*.msr) | *.mco; |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.mco"), _T("mco"), OFN_HIDEREADONLY, szFilter);

	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		if (!pathName.IsEmpty()) {
			m_editMCOPath.SetWindowTextW(pathName);

			if (!m_pMIL->SetCodeReaderContext(CONV_A(pathName))) {
				MessageBox(_T("Context Load Error"));
			}
		}
	}
	return;
}

void CMILTab::OnBnClickedBtnModelFind()
{
	for (int num = 0; num < 20; ++num) {
		//cv::Mat image = GetImage();
		auto sss = cv::format("C:/Users/PC/Desktop/성형불량/ok/%d.bmp", num + 1);
		cv::Mat image = imread(sss);
		if (image.empty())
			return;

		cv::Mat dst = image.clone();
		if (image.channels() == 3)
			cvtColor(image, image, cv::COLOR_BGR2GRAY);

		// test >>>>>
		cv::resize(image, image, cv::Size(image.cols / 2, image.rows / 2));
		cv::resize(dst, dst, cv::Size(dst.cols / 2, dst.rows / 2));
		cv::Mat mark_image = cv::imread("D:/Project/VisionTestTool/images/mark_image.png");
		cv::Size mark_size = mark_image.size();
		// <<<<<

		vector<double> angle, score;
		vector<cv::Point2d> pts;

		if (m_pMIL->FindModel(image, pts, angle, score)) {
			for (int i = 0; i < pts.size(); ++i) {
				cv::drawMarker(dst, pts[i], cv::Scalar(0, 255, 0), 0, 50);
				cv::putText(dst, cv::format("score : %.2lf", score[i]), pts[i], 0, 1, cv::Scalar(0, 255, 0));
				cv::putText(dst, cv::format("angle : %.2lf", angle[i]), pts[i] + cv::Point2d(0, 30), 0, 1, cv::Scalar(0, 255, 0));

				cv::Mat M = cv::getRotationMatrix2D(pts[i], -angle[i], 1.0);
				cv::Mat rotImage;
				cv::warpAffine(image, rotImage, M, image.size());

				cv::Mat save_image = rotImage(cv::Rect(pts[i].x - (mark_size.width / 2), pts[i].y - (mark_size.height / 2), mark_size.width, mark_size.height));
				CTime time = CTime::GetCurrentTime();
				int h = time.GetHour();
				int m = time.GetMinute();
				int s = time.GetSecond();
				cv::imwrite(cv::format("../../temp/%d_%d_%d_%d.png", h, m, s, i), save_image);
			}

			SetImage(dst);
		}
		waitKey(1000);
	}
	return;
}

void CMILTab::OnBnClickedBtnStringRead()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst = image.clone();
	if (image.channels() == 3)
		cvtColor(image, image, cv::COLOR_BGR2GRAY);

	double dScore;
	std::string strResult;

	if (m_pMIL->ReadString(image, strResult, dScore)) {
        cv::putText(dst, strResult, cv::Point(10, 30), 0, 1, cv::Scalar(0, 255, 0));
		cv::putText(dst, cv::format("score : %.2lf", dScore), cv::Point(10, 60), 0, 1, cv::Scalar(0, 255, 0));

		SetImage(dst);
	}
	return;
}


void CMILTab::OnBnClickedBtnCodeRead()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst = image.clone();
	if (image.channels() == 3)
		cvtColor(image, image, cv::COLOR_BGR2GRAY);

	std::string strResult;
	cv::Point ptPos;
	if (m_pMIL->ReadCode(image, strResult, ptPos)) {
		cv::drawMarker(dst, ptPos, cv::Scalar(0, 255, 0), 0, 50);
		cv::putText(dst, strResult, ptPos, 0, 1, cv::Scalar(0, 255, 0));

		SetImage(dst);
	}
	return;
}
