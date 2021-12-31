// CTab1.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "CTab1.h"
#include "afxdialogex.h"
#include "VisionTestToolDlg.h";
#include "ImageProcess.h"
//#include "MILModule.h"

// CTab1 대화 상자

IMPLEMENT_DYNAMIC(CTab1, CDialogEx)

CTab1::CTab1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_radioSobel(0)
{
	m_pParent = (CVisionTestToolDlg*)pParent;
	m_pImageProcess = new CImageProcess();



}

CTab1::~CTab1()
{
	if(m_pImageProcess)
		delete m_pImageProcess;

	m_pImageProcess = nullptr;
}

void CTab1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BLUR_SIZE, m_editBlurSize);
	DDX_Control(pDX, IDC_EDIT_GAUSSIAN_SIZE, m_editGaussianSize);
	DDX_Control(pDX, IDC_EDIT_MEDIAN_SIZE, m_editMedianSize);
	DDX_Control(pDX, IDC_EDIT_BILATERAL_D, m_editBilateralD);
	DDX_Control(pDX, IDC_EDIT_BILATERAL_SC, m_editBilateralColor);
	DDX_Control(pDX, IDC_EDIT_BILATERAL_SS, m_editBilateralSpace);
	DDX_Control(pDX, IDC_EDIT_MORPH_SIZE, m_editMorphSize);
	DDX_Control(pDX, IDC_COMBO_MORPH, m_comboMorph);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD1, m_editThreshold1);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD_2, m_editThreshold2);
	DDX_Control(pDX, IDC_COMBO_THRESHOLD, m_comboThreshold);
	DDX_Control(pDX, IDC_EDIT_ADAP_BLOCK, m_editBlockSize);
	DDX_Control(pDX, IDC_EDIT_ADAP_C, m_editC);
	DDX_Control(pDX, IDC_EDIT_CANNY1, m_editCanny1);
	DDX_Control(pDX, IDC_EDIT_CANNY2, m_editCanny2);
	DDX_Control(pDX, IDC_EDIT_SOBEL_SIZE, m_editSobelSize);
	DDX_Radio(pDX, IDC_RADIO_SOBEL_1, m_radioSobel);
	DDX_Control(pDX, IDC_EDIT_HOUGH_CIRCLE_DIST, m_editHoughCircleDist);
	DDX_Control(pDX, IDC_EDIT_HOUGH_CIRCLE_PARAM1, m_editHoughCircleParam1);
	DDX_Control(pDX, IDC_EDIT_HOUGH_CIRCLE_PARAM2, m_editHoughCircleParam2);
	DDX_Control(pDX, IDC_EDIT_HOUGH_CIRCLE_MIN, m_editHoughCircleMin);
	DDX_Control(pDX, IDC_EDIT_HOUGH_CIRCLE_MAX, m_editHoughCircleMax);
	DDX_Control(pDX, IDC_EDIT_HOUGH_LINE, m_editHoughLineThresh);
	DDX_Control(pDX, IDC_EDIT_DENOISE_H, m_editDenoiseH);
	DDX_Control(pDX, IDC_EDIT_DENOISE_TEMPLATE, m_editDenoiseTemplate);
	DDX_Control(pDX, IDC_EDIT_DENOISE_SEARCH, m_editDenoiseSearch);
	DDX_Control(pDX, IDC_SLIDER_CANNY1, m_sliderCanny1);
	DDX_Control(pDX, IDC_SLIDER_CANNY2, m_sliderCanny2);
}

cv::Mat CTab1::GetImage() {
	return m_pParent->GetImage();
}

void CTab1::SetImage(cv::Mat image, BOOL bRedraw) {
	m_pParent->SetResultImage(image, bRedraw);

	return;
}

cv::Mat	CTab1::RotateImage(const cv::Mat& image, double dAngle) {
	cv::Mat rotateImage;
	cv::Point2d base(image.cols / 2, image.rows / 2);

	cv::Mat R			= cv::getRotationMatrix2D(base, dAngle, 1);

	//cv::Rect bbox		= cv::RotatedRect(base, image.size(), dAngle).boundingRect();
	//R.at<double>(0, 2)	+= bbox.width / 2.0 - base.x;
	//R.at<double>(1, 2)	+= bbox.height / 2.0 - base.y;
	//cv::warpAffine(image, rotateImage, R, bbox.size());

	cv::warpAffine(image, rotateImage, R, image.size(), 1, BORDER_CONSTANT, cv::Scalar::all(0));

	return rotateImage;
}

BEGIN_MESSAGE_MAP(CTab1, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BLUR,				&CTab1::OnBnClickedBtnBlur)
	ON_BN_CLICKED(IDC_BTN_GAUSSIAN_BLUR,	&CTab1::OnBnClickedBtnGaussianBlur)
	ON_BN_CLICKED(IDC_BTN_MEDIAN_BLUR,		&CTab1::OnBnClickedBtnMedianBlur)
	ON_BN_CLICKED(IDC_BTN_BILATERAL_BLUR,	&CTab1::OnBnClickedBtnBilateralBlur)
	ON_BN_CLICKED(IDC_BTN_MORPH,			&CTab1::OnBnClickedBtnMorph)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD,		&CTab1::OnBnClickedBtnThreshold)
	ON_BN_CLICKED(IDC_BTN_ADAPTIVE_THRESH,	&CTab1::OnBnClickedBtnAdaptiveThresh)
	ON_BN_CLICKED(IDC_BTN_CANNY,			&CTab1::OnBnClickedBtnCanny)
	ON_BN_CLICKED(IDC_BTN_SOBEL,			&CTab1::OnBnClickedBtnSobel)
	ON_BN_CLICKED(IDC_BTN_HOUGH_CIRCLE,		&CTab1::OnBnClickedBtnHoughCircle)
	ON_BN_CLICKED(IDC_BTN_HOUGH_LINE,		&CTab1::OnBnClickedBtnHoughLine)
	ON_BN_CLICKED(IDC_BTN_DENOISE,			&CTab1::OnBnClickedBtnDenoise)
	ON_BN_CLICKED(IDC_BTN_TEST,				&CTab1::OnBnClickedBtnTest)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_TEST2, &CTab1::OnBnClickedBtnTest2)
END_MESSAGE_MAP()


void CTab1::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
        if (!m_bFirst) {
            m_editBlurSize.SetWindowTextW(_T("3"));
            m_editGaussianSize.SetWindowTextW(_T("3"));
            m_editMedianSize.SetWindowTextW(_T("3"));
            m_editBilateralD.SetWindowTextW(_T("10"));
            m_editBilateralColor.SetWindowTextW(_T("50"));
            m_editBilateralSpace.SetWindowTextW(_T("50"));
            m_editMorphSize.SetWindowTextW(_T("3"));
            m_editThreshold1.SetWindowTextW(_T("100"));
            m_editThreshold2.SetWindowTextW(_T("255"));
            m_editBlockSize.SetWindowTextW(_T("3"));
            m_editC.SetWindowTextW(_T("2"));
            m_editCanny1.SetWindowTextW(_T("100"));
            m_editCanny2.SetWindowTextW(_T("200"));
            m_editSobelSize.SetWindowTextW(_T("3"));

            m_editHoughCircleDist.SetWindowTextW(_T("150"));;
            m_editHoughCircleParam1.SetWindowTextW(_T("100"));;
            m_editHoughCircleParam2.SetWindowTextW(_T("90"));;
            m_editHoughCircleMin.SetWindowTextW(_T("100"));;
            m_editHoughCircleMax.SetWindowTextW(_T("200"));;
            m_editHoughLineThresh.SetWindowTextW(_T("150"));;

            m_editDenoiseH.SetWindowTextW(_T("3.0"));
            m_editDenoiseTemplate.SetWindowTextW(_T("7"));
            m_editDenoiseSearch.SetWindowTextW(_T("21"));

			m_comboMorph.AddString(_T("ERODE"));	// 0
			m_comboMorph.AddString(_T("DILATE"));	// 1
			m_comboMorph.AddString(_T("OPEN"));	// 2
			m_comboMorph.AddString(_T("CLOSE"));	// 3
			m_comboMorph.AddString(_T("GRADIENT"));	// 4
			m_comboMorph.AddString(_T("TOP HAT"));	// 5
			m_comboMorph.AddString(_T("BLACK HAT"));	// 6
			m_comboMorph.AddString(_T("HIT MISS"));	// 7

			m_comboThreshold.AddString(_T("BINARY    "));
			m_comboThreshold.AddString(_T("BINARY_INV"));
			m_comboThreshold.AddString(_T("TRUNC     "));
			m_comboThreshold.AddString(_T("TOZERO    "));
			m_comboThreshold.AddString(_T("TOZERO_INV"));
			m_comboThreshold.AddString(_T("MASK      "));
			m_comboThreshold.AddString(_T("OTSU      "));
			m_comboThreshold.AddString(_T("TRIANGLE  "));

			m_comboMorph.SetCurSel(0);
			m_comboThreshold.SetCurSel(0);

			m_sliderCanny1.SetRange(0, 255);
			m_sliderCanny2.SetRange(0, 255);

			m_bFirst = TRUE;
		}
	}
}


void CTab1::OnBnClickedBtnBlur()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editBlurSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	cv::blur(image, image, cv::Size(iSize, iSize));
	SetImage(image);
	return;
}


void CTab1::OnBnClickedBtnGaussianBlur()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editGaussianSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	cv::GaussianBlur(image, image, cv::Size(iSize, iSize), 0);
	SetImage(image);
	return;
}


void CTab1::OnBnClickedBtnMedianBlur()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editMedianSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	cv::medianBlur(image, image, iSize);
	SetImage(image);
	return;
}


void CTab1::OnBnClickedBtnBilateralBlur()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	CString str;
	m_editBilateralD.GetWindowTextW(str);
	int iD = _ttoi(str);
	m_editBilateralColor.GetWindowTextW(str);
	int iColor = _ttoi(str);
	m_editBilateralSpace.GetWindowTextW(str);
	int iSpace = _ttoi(str);

	cv::Mat dst;
	cv::bilateralFilter(image, dst, iD, iColor, iSpace);
	SetImage(dst);
	return;
}


void CTab1::OnBnClickedBtnMorph()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editMorphSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(iSize, iSize));
	cv::Mat dst;

	int iMorphType = m_comboMorph.GetCurSel();
	cv::morphologyEx(image, dst, (cv::MorphTypes)iMorphType, kernel);

	SetImage(dst);
}


void CTab1::OnBnClickedBtnThreshold()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	CString str;
	m_editThreshold1.GetWindowTextW(str);
	int iThresh1 = _ttoi(str);
	m_editThreshold2.GetWindowTextW(str);
	int iThresh2 = _ttoi(str);

	int iThreshType = m_comboThreshold.GetCurSel();

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	if (iThreshType == 5) {
		iThreshType = 7;
	}
	else if (iThreshType == 6) {
		iThreshType = 8;
	}
	else if (iThreshType == 7) {
		iThreshType = 16;
	}

	cv::Mat dst;
	cv::threshold(image, dst, iThresh1, iThresh2, (cv::ThresholdTypes)iThreshType);

	SetImage(dst);
	return;
}


void CTab1::OnBnClickedBtnAdaptiveThresh()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	CString str;
	m_editBlockSize.GetWindowTextW(str);
	int iBlockSize = _ttoi(str);
	m_editC.GetWindowTextW(str);
	int iC = _ttoi(str);;

	cv::adaptiveThreshold(image, dst, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, iBlockSize, iC);
	SetImage(dst);
}


void CTab1::OnBnClickedBtnCanny()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	CString str;
	m_editCanny1.GetWindowTextW(str);
	int iCanny1 = _ttoi(str);
	m_editCanny2.GetWindowTextW(str);
	int iCanny2 = _ttoi(str);;

	cv::GaussianBlur(image, image, cv::Size(3, 3), 0);
	cv::Canny(image, dst, iCanny1, iCanny2);

	SetImage(dst);
	return;
}


void CTab1::OnBnClickedBtnSobel()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	CString str;
	m_editSobelSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	int x = 0;
	int y = 0;

	UpdateData(true);
	switch (m_radioSobel) {
		case 0: {
			x = 1;
			y = 0;
			break;
		}
		case 1: {
			x = 0;
			y = 1;
			break;
		}
		case 2: {
			x = 1;
			y = 1;
			break;
		}
	}
	
	cv::Sobel(image, dst, -1, x, y, iSize);

	SetImage(dst);
}


void CTab1::OnBnClickedBtnHoughCircle()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst = image.clone();

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	std::vector<cv::Vec3f> circles;
	CString str;
	m_editHoughCircleDist.GetWindowTextW(str);
	double dMinDist = (double)_ttoi(str);
	m_editHoughCircleParam1.GetWindowTextW(str);
	double dCannyMax = (double)_ttoi(str);
	m_editHoughCircleParam2.GetWindowTextW(str);
	double dParam2 = (double)_ttoi(str);
	m_editHoughCircleMin.GetWindowTextW(str);
	int iMinRadius = _ttoi(str);
	m_editHoughCircleMax.GetWindowTextW(str);
	int iMaxRadius = _ttoi(str);

	cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, 1, dMinDist, dCannyMax, dParam2, iMinRadius, iMaxRadius);

	if (dst.channels() == 1)
		cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

	for (int i = 0; i < circles.size(); ++i) {
		cv::circle(dst, cv::Point(circles[i][0], circles[i][1]), circles[i][2], cv::Scalar(0, 0, 255), 2);
	}

	SetImage(dst);
	return;
}


void CTab1::OnBnClickedBtnHoughLine()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst = image.clone();

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editHoughLineThresh.GetWindowTextW(str);
	int iThreshold = _ttoi(str);

	std::vector<cv::Vec2f> lines;
	cv::HoughLines(image, lines, 1, CV_PI / 180, iThreshold);

	if (dst.channels() == 1)
		cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

	for (int i = 0; i < lines.size(); ++i) {
		float rho = lines[i][0], theta = lines[i][1];
		cv::Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		cv::line(dst, pt1, pt2, cv::Scalar(0, 0, 255), 2);
	}

	SetImage(dst);
	return;
}


void CTab1::OnBnClickedBtnDenoise()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst = image.clone();
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	CString str;
	m_editDenoiseH.GetWindowTextW(str);
    float h = _tstof(str);
	m_editDenoiseTemplate.GetWindowTextW(str);
    int templateWindowSize = _ttoi(str);
	m_editDenoiseSearch.GetWindowTextW(str);
	int searchWindowSize = _ttoi(str);
	
    cv::fastNlMeansDenoising(image, dst, h, templateWindowSize, searchWindowSize);

	SetImage(dst);
	return;
}

cv::Point2d rotate2d(const cv::Point2d& inPoint, const double& angRad)
{
	cv::Point2d outPoint;
	//CW rotation
	outPoint.x = std::cos(angRad) * inPoint.x - std::sin(angRad) * inPoint.y;
	outPoint.y = std::sin(angRad) * inPoint.x + std::cos(angRad) * inPoint.y;
	return outPoint;
}

cv::Point2d rotatePoint(const cv::Point2d& inPoint, const cv::Point2d& center, const double& angRad)
{
	return rotate2d(inPoint - center, angRad) + center;
}

#define MARK_WIDTH 380
#define MARK_HEIGHT 280

//void CTab1::SetMILContext() {
//    if (!m_pMIL) {
//        m_pMIL = new CMILModule();
//        m_pMIL->SetModelFinderContext("D:/Project/VisionTestTool/cfg/model_finder.mmf");
//    }
//}
//bool CTab1::GetMarkImage(const cv::Mat& image, cv::Mat& dst) {
//    cv::Mat resize;
//    cv::resize(image, resize, cv::Size(image.cols * 0.5, image.rows * 0.5));
//
//    //cv::Rect roiRect = cv::Rect(600, 1500, 1300, 800);
//    cv::Rect roiRect = cv::Rect(600, 1200, 1300, 800) & cv::Rect(0, 0, resize.cols, resize.rows);
//    cv::Mat roiImage = resize(roiRect);
//    if (roiImage.channels() == 3)
//        cv::cvtColor(roiImage, roiImage, cv::COLOR_BGR2GRAY);
//
//    vector<Point2d> pts;
//    vector<double> angles;
//    vector<double> scores;
//
//    if (m_pMIL->FindModel(roiImage, pts, angles, scores)) {
//        cv::Mat R = cv::getRotationMatrix2D(pts[0], -angles[0], 1.0);
//
//        cv::Mat rotImage;
//        cv::warpAffine(roiImage, rotImage, R, roiImage.size());
//
//        cv::Rect rect = cv::Rect(pts[0].x - MARK_WIDTH, pts[0].y - MARK_HEIGHT - 100, MARK_WIDTH * 2, MARK_HEIGHT * 2) & cv::Rect(0, 0, rotImage.cols, rotImage.rows);
//        rotImage = rotImage(rect);
//        dst = rotImage.clone();
//
//        return true;
//    }
//    return false;
//}
//bool CTab1::CheckBlack(const cv::Mat& image, cv::Mat& dst, int iThresh, int iMinSize) {
//	cv::threshold(image, dst, iThresh, 255, cv::THRESH_BINARY_INV);
//
//	vector<vector<cv::Point>> contours;
//	findContours(dst, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));
//
//    if (contours.size() < 1)
//        return true;
//
//	cv::Mat resultImage = cv::Mat::zeros(image.size(), CV_8UC1);
//
//	for (int i = 0; i < contours.size(); ++i) {
//		if (contours[i].size() > iMinSize) {
//			cv::drawContours(resultImage, contours, i, cv::Scalar(255), -1);
//		}
//	}
//
//	dst = resultImage.clone();
//	return false;
//}
//bool CTab1::CheckLine(const cv::Mat& image, cv::Mat& dst, int iMin, int iMax) {
//	cv::Mat resultImage = cv::Mat::zeros(image.size(), CV_8UC3);
//
//	double dWidth = image.cols / 5;
//
//	cv::Mat temp, temp2;
//	cv::Rect roiRect = cv::Rect(0, 340, dWidth, 100);
//	bool bResult = true;
//
//	for (int i = 0; i < 5; ++i) {
//		temp = image(cv::Rect(dWidth * i, 0, dWidth, image.rows)).clone();
//		temp2 = temp(roiRect);
//
//		cv::threshold(temp2, temp2, 220, 255, cv::THRESH_BINARY);
//		cv::GaussianBlur(temp2, temp2, cv::Size(3, 3), 0);
//		cv::Canny(temp2, temp2, 50, 100);
//		
//		cv::Point2d pt1, pt2;
//		if (m_pImageProcess->GetLine_parallel(temp2, cv::Rect(0, 0, temp2.cols, temp2.rows), 0, 0, 3, 100, eLINE_POSITION::BOTTOM_LINE, pt1, pt2, FALSE, FALSE)) {
//			double dHeight = (pt1.y + pt2.y) * 0.5;
//			cv::Scalar color;
//			if (dHeight > iMin && dHeight < iMax) {
//				color = cv::Scalar(0, 255, 0);
//			}
//			else {
//				color = cv::Scalar(0, 0, 255);
//				bResult = false;
//			}
//
//			cv::line(resultImage,
//				roiRect.tl() + cv::Point(dWidth * i, dHeight),
//				roiRect.tl() + cv::Point(dWidth * (i + 1), dHeight),
//				color, 1);
//		}
//		else {
//			bResult = false;
//		}
//	}
//
//	dst = resultImage.clone();
//	return bResult;
//}
//
//bool m_bBlack	= true;
//bool m_bLine	= true;
//
//void CTab1::OnBnClickedBtnTest()
//{
//	SetMILContext();
//
//	auto sTime = GetTickCount64();
//
//	cv::Mat image = GetImage();
//	cv::Mat markImage, resultImage;
//	if (GetMarkImage(image, markImage)) {
//		cv::Mat blackImage, lineImage;
//		vector<Point> pts;
//
//		resultImage = markImage.clone();
//		if (resultImage.channels() == 1)
//			cvtColor(resultImage, resultImage, cv::COLOR_GRAY2BGR);
//
//		m_bBlack	= CheckBlack(markImage, blackImage, 100);
//		m_bLine		= CheckLine(markImage, lineImage, 40, 70);
//
//		if (!m_bBlack) {
//			cv::findNonZero(blackImage, pts);
//
//			for each (Point pt in pts) {
//				resultImage.at<Vec3b>(pt)[0] = 0;
//				resultImage.at<Vec3b>(pt)[1] = 0;
//				resultImage.at<Vec3b>(pt)[2] = 255;
//			}
//			cv::putText(resultImage, "Carbide NG", cv::Point(50, 100), 0, 1, cv::Scalar(0, 0, 255));
//		}
//		if (!m_bLine) {
//			cv::putText(resultImage, "Height NG", cv::Point(50, 150), 0, 1, cv::Scalar(0, 0, 255));
//		}
//		for (int y = 0; y < lineImage.rows; ++y) {
//			auto data = lineImage.ptr<uchar>(y);
//			for (int x = 0; x < lineImage.cols; ++x) {
//				if (data[x * 3 + 0] != 0 || data[x * 3 + 1] != 0 || data[x * 3 + 2] != 0) {
//					resultImage.at<Vec3b>(y, x)[0] = data[x * 3 + 0];
//					resultImage.at<Vec3b>(y, x)[1] = data[x * 3 + 1];
//					resultImage.at<Vec3b>(y, x)[2] = data[x * 3 + 2];
//				}
//			}
//		}
//
//		double dTime = (GetTickCount64() - sTime) * 0.001;
//		cv::putText(resultImage, cv::format("time : %.2lf (sec)", dTime), cv::Point(50, 50), 0, 1, cv::Scalar(255, 0, 0));
//		SetImage(resultImage);
//	}
//	else {
//		MessageBox(_T("Mark 찾기 실패"));
//	}
//
//	return;
//}

void CTab1::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar) {
		int iPos;
		if (pScrollBar == (CScrollBar*)&m_sliderCanny1) {
			iPos = m_sliderCanny1.GetPos();
			CString str;
			str.Format(_T("%d"), iPos);
			m_editCanny1.SetWindowTextW(str);
		}
		else if (pScrollBar == (CScrollBar*)&m_sliderCanny2) {
			iPos = m_sliderCanny2.GetPos();
			CString str;
			str.Format(_T("%d"), iPos);
			m_editCanny2.SetWindowTextW(str);
		}
		cv::Mat image = GetImage();

		cv::Mat dst;
		CString str;
		m_editCanny1.GetWindowTextW(str);
		int iCanny1 = _ttoi(str);
		m_editCanny2.GetWindowTextW(str);
		int iCanny2 = _ttoi(str);;

		cv::GaussianBlur(image, image, cv::Size(3, 3), 0);
		cv::Canny(image, dst, iCanny1, iCanny2);

		SetImage(dst, FALSE);
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

#define PI 3.14159265358979
std::thread* pThread = nullptr;

double Degree(double radian) {
	return radian * 180 / PI;
}

double Radian(double degree) {
	return degree * PI / 180;
}

bool CTab1::CalDistance(cv::Mat image, double dTilt, double& dH, double& dDistance) {
	cv::Mat canny;
	
	if (image.channels() == 3) {
		cv::cvtColor(image, canny, cv::COLOR_BGR2GRAY);
	}
	else {
		canny = image.clone();
	}

	cv::GaussianBlur(canny, canny, cv::Size(3, 3), 0);
	cv::Canny(canny, canny, 100, 200);

	cv::Point2d pt1, pt2;

	cv::Mat rotImage;

	if (m_pImageProcess->GetLine(canny, cv::Rect(0, 0, canny.cols, canny.rows), 0, 0, 3, (int)(canny.cols *0.66), eLINE_POSITION::TOP_LINE, pt1, pt2, FALSE, FALSE)) {
		cv::line(image, pt1, pt2, cv::Scalar(0, 0, 255), 2);
	}
	else {
		return false;
	}
	
	dH = (pt1.y + pt2.y) / 2;
	dDistance = dH / tan(Radian(dTilt));

	return true;
}

bool CTab1::CalTilt(cv::Mat image, double dDistance, double& dH, double& dTilt) {
	cv::Mat canny;

	if (image.channels() == 3) {
		cv::cvtColor(image, canny, cv::COLOR_BGR2GRAY);
	}
	else {
		canny = image.clone();
	}

	cv::GaussianBlur(canny, canny, cv::Size(3, 3), 0);
	cv::Canny(canny, canny, 100, 200);

	cv::Point2d pt1, pt2;

	cv::Mat rotImage;

	if (m_pImageProcess->GetLine(canny, cv::Rect(0, 0, canny.cols, canny.rows), 0, 0, 3, (int)(canny.cols *0.66), eLINE_POSITION::TOP_LINE, pt1, pt2, FALSE, FALSE)) {
		cv::line(image, pt1, pt2, cv::Scalar(0, 0, 255), 2);
	}
	else {
		return false;
	}

	dH = (pt1.y + pt2.y) / 2;
	dTilt = atan2(dH, dDistance);
	dTilt = Degree(dTilt);

	return true;
}


void CTab1::OnBnClickedBtnTest() {
	pThread = new std::thread([&]() {
		double dTilt, dHeight = 0;
		int dDistance = 0;

		cv::namedWindow("set distance");
		cv::createTrackbar("distance", "set distance", &dDistance, 10000);

		while (true) {
			cv::Mat image = GetImage();
			cv::Mat gray;

			if (image.channels() == 3)
				cvtColor(image, gray, cv::COLOR_BGR2GRAY);
			else
				gray = image.clone();

			if (CalTilt(image, dDistance, dHeight, dTilt)) {
				cv::putText(image, cv::format("tilt : %.1lf", dTilt), cv::Point(50, 50), 0, 2, cv::Scalar(0, 255, 0), 2);
				cv::putText(image, cv::format("dist : %d", dDistance), cv::Point(50, 150), 0, 2, cv::Scalar(0, 255, 0), 2);
				cv::putText(image, cv::format("height : %.1lf", dHeight), cv::Point(50, 250), 0, 2, cv::Scalar(0, 255, 0), 2);
			}

			cv::resize(image, image, cv::Size(image.cols / 2, image.rows / 2));
			cv::imshow("set distance", image);
			cv::waitKey(10);
		}
	});

	return;
}

void CTab1::OnBnClickedBtnTest2()
{
	// cv::Mat imageA = cv::imread("C:/Project/VisionTestTool-master/images/aa.jpg");
	cv::Mat image = GetImage();

	double dTilt		= 20;	// 현재 카메라의 tilt 값
	double dDistance	= 0;	// tilt값 기준으로 받아올 distance
	double dHeight		= 0;	// 측정한 수평선 높이

	// distance 계산
	if (CalDistance(image, dTilt, dHeight, dDistance)) {
		cv::putText(image, cv::format("tilt : %.1lf", dTilt),		cv::Point(50, 50), 0, 2, cv::Scalar(0, 255, 0), 2);
		cv::putText(image, cv::format("dist : %.1lf", dDistance),	cv::Point(50, 150), 0, 2, cv::Scalar(0, 255, 0), 2);
		cv::putText(image, cv::format("height : %.1lf", dHeight),	cv::Point(50, 250), 0, 2, cv::Scalar(0, 255, 0), 2);
	}

	cv::Mat image2 = cv::imread("C:/Project/VisionTestTool-master/images/bbb.jpg");
	// 가지고 있는 distance로 tilt 계산
	if (CalTilt(image2, dDistance, dHeight, dTilt)) {
		cv::putText(image2, cv::format("tilt : %.1lf", dTilt),		cv::Point(50, 50), 0, 2,	cv::Scalar(0, 0, 255), 2);
		cv::putText(image2, cv::format("dist : %.1lf", dDistance),	cv::Point(50, 150), 0, 2,	cv::Scalar(0, 0, 255), 2);
		cv::putText(image2, cv::format("height : %.1lf", dHeight), cv::Point(50, 250), 0, 2, cv::Scalar(0, 0, 255), 2);
	}

	cv::Mat image3 = cv::imread("C:/Project/VisionTestTool-master/images/ccc.jpg");
	// 가지고 있는 distance로 tilt 계산
	if (CalTilt(image3, dDistance, dHeight, dTilt)) {
		cv::putText(image3, cv::format("tilt : %.1lf", dTilt),		cv::Point(50, 50), 0, 2, cv::Scalar(0, 0, 255), 2);
		cv::putText(image3, cv::format("dist : %.1lf", dDistance),	cv::Point(50, 150), 0, 2, cv::Scalar(0, 0, 255), 2);
		cv::putText(image3, cv::format("height : %.1lf", dHeight), cv::Point(50, 250), 0, 2, cv::Scalar(0, 0, 255), 2);
	}

	cv::resize(image, image, cv::Size(image.cols / 2, image.rows / 2));
	cv::resize(image2, image2, cv::Size(image2.cols / 2, image2.rows / 2));
	cv::resize(image3, image3, cv::Size(image3.cols / 2, image3.rows / 2));

	cv::imshow("cal dist", image);
	cv::imshow("cal tilt", image2);
	cv::imshow("cal tilt2", image3);

	return;
}

