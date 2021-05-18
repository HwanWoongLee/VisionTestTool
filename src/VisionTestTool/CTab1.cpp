// CTab1.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "CTab1.h"
#include "afxdialogex.h"
#include "VisionTestToolDlg.h";
#include "ImageProcess.h"

// CTab1 대화 상자

IMPLEMENT_DYNAMIC(CTab1, CDialogEx)

CTab1::CTab1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_radioSobel(0)
{
	m_pParent = (CVisionTestToolDlg*)pParent;
	m_pImageProcess = new CImageProcess();

	m_test4Pts.push_back(cv::Point(1722, 773));
	m_test4Pts.push_back(cv::Point(2635, 773));
	m_test4Pts.push_back(cv::Point(3802, 773));
	m_test4Pts.push_back(cv::Point(2151, 914));
	m_test4Pts.push_back(cv::Point(2955, 600));
	m_test4Pts.push_back(cv::Point(2361, 573));
	m_test4Pts.push_back(cv::Point(2151, 560));
	m_test4Pts.push_back(cv::Point(2151, 650));

	//m_test5Pts.push_back(cv::Point(1152, 660));
	//m_test5Pts.push_back(cv::Point(3197, 660));
	m_test5Pts.push_back(cv::Point(665, 411));
	m_test5Pts.push_back(cv::Point(2893, 411));

}

CTab1::~CTab1()
{
	SAFE_DELETE(m_pImageProcess);
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

cv::Mat	CTab1::RotateImage(cv::Mat image, double dAngle) {
	cv::Mat rotateImage;
	cv::Mat R = cv::getRotationMatrix2D(cv::Point2f(image.cols / 2, image.rows / 2), dAngle, 1);

	cv::warpAffine(image, rotateImage, R, image.size());

	return rotateImage;
}

BEGIN_MESSAGE_MAP(CTab1, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BLUR, &CTab1::OnBnClickedBtnBlur)
	ON_BN_CLICKED(IDC_BTN_GAUSSIAN_BLUR, &CTab1::OnBnClickedBtnGaussianBlur)
	ON_BN_CLICKED(IDC_BTN_MEDIAN_BLUR, &CTab1::OnBnClickedBtnMedianBlur)
	ON_BN_CLICKED(IDC_BTN_BILATERAL_BLUR, &CTab1::OnBnClickedBtnBilateralBlur)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BTN_MORPH, &CTab1::OnBnClickedBtnMorph)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD, &CTab1::OnBnClickedBtnThreshold)
	ON_BN_CLICKED(IDC_BTN_ADAPTIVE_THRESH, &CTab1::OnBnClickedBtnAdaptiveThresh)
	ON_BN_CLICKED(IDC_BTN_CANNY, &CTab1::OnBnClickedBtnCanny)
	ON_BN_CLICKED(IDC_BTN_SOBEL, &CTab1::OnBnClickedBtnSobel)
	ON_BN_CLICKED(IDC_BTN_HOUGH_CIRCLE, &CTab1::OnBnClickedBtnHoughCircle)
	ON_BN_CLICKED(IDC_BTN_HOUGH_LINE, &CTab1::OnBnClickedBtnHoughLine)
	ON_BN_CLICKED(IDC_BTN_DENOISE, &CTab1::OnBnClickedBtnDenoise)
	ON_BN_CLICKED(IDC_BTN_TEST, &CTab1::OnBnClickedBtnTest)
	ON_WM_HSCROLL()
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
	if (iThreshType == 5) {
		iThreshType = 7;
	}
	else if (iThreshType == 6) {
		if (image.channels() == 3)
			cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

		iThreshType = 8;
	}
	else if (iThreshType == 7) {
		if (image.channels() == 3)
			cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

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
	cv::HoughLines(image, lines, 1, M_PI / 180, iThreshold);

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

void CTab1::OnBnClickedBtnTest()
{
	//cv::Mat image = GetImage();
	//cv::Mat mark = cv::imread("D:/Project/VisionTestTool/mark.bmp");
	//cv::Rect roiRect;
	//
	//TestROI(image, mark, roiRect);
	//
	//SetImage(image);
	cv::Mat image1 = cv::imread("C:/Users/PC/Desktop/test_1.bmp");
	cv::Mat image2 = cv::imread("C:/Users/PC/Desktop/test_2.jpg");
	
	std::thread th1([&]() {
		TestCode4(image1);
		}
	);
	std::thread th2([&]() {
		TestCode5(image2);
		}
	);
	
	th1.join();
	th2.join();
	
	cv::Mat dst;
	cv::vconcat(image1, image2, dst);
	//cv::imwrite("C:/Users/PC/Desktop/result_1.bmp", image1);
	//cv::imwrite("C:/Users/PC/Desktop/result_2.bmp", image2);
	
	SetImage(dst);
	return;
}

void CTab1::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar) {
		int iPos;
		if (pScrollBar == (CScrollBar*)&m_sliderCanny1) {
			iPos = m_sliderCanny1.GetPos();
			m_editCanny1.SetWindowTextW(Format(_T("%d"), iPos));
		}
		else if (pScrollBar == (CScrollBar*)&m_sliderCanny2) {
			iPos = m_sliderCanny2.GetPos();
			m_editCanny2.SetWindowTextW(Format(_T("%d"), iPos));
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

void CTab1::TestCode() {
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	// denoise
	cv::Mat denoise;
	cv::fastNlMeansDenoising(image, denoise, 3.0, 7, 21);

	// edge
	cv::Mat edge;
	cv::Canny(denoise, edge, 10, 40);

	// get line
	cv::Point2d pt1, pt2;
	cv::Point2d ptSub1, ptSub2;

	// FInd Line
	double dAngle;
	if (m_pImageProcess->GetLine(edge, cv::Rect(0, 0, edge.cols, edge.rows),
		0, 0,
		3, 300,
		eLINE_POSITION::LEFT_LINE,
		pt1, pt2,
		dAngle,
		FALSE, FALSE))
	{
		// Rotate
		cv::Point2d ptCenter = (pt1 + pt2) / 2;
		cv::Mat R = cv::getRotationMatrix2D(ptCenter, dAngle, 1.0);
		cv::Mat rotateImage;
		cv::warpAffine(edge, rotateImage, R, edge.size());

		// ROI
		cv::Rect roiRect = cv::Rect(ptCenter.x, 0, 200, rotateImage.rows);
		roiRect = roiRect & cv::Rect(0, 0, rotateImage.cols, rotateImage.rows);
		cv::Mat roiImage = rotateImage(roiRect).clone();
		cv::threshold(roiImage, roiImage, 100, 255, cv::THRESH_BINARY);

		// Get OutLine
		cv::Mat temp = cv::Mat::zeros(roiImage.size(), CV_8UC1);
		auto data = roiImage.data;
		for (int y = 0; y < roiImage.rows; y++) {
			for (int x = roiImage.cols - 1; x > 0; x--) {
				if (data[y * roiImage.cols + x] == 255) {
					temp.at<uchar>(cv::Point(x, y)) = 255;
					break;
				}
			}
		}

		// Find Sub Line
		if (m_pImageProcess->GetLine(roiImage, cv::Rect(0, 0, roiImage.cols, roiImage.rows),
			0, 0,
			3, 300,
			eLINE_POSITION::RIGHT_LINE,
			ptSub1, ptSub2,
			FALSE, FALSE))
		{
			cv::Mat drawImage = image.clone();
			if (drawImage.channels() == 1)
				cv::cvtColor(drawImage, drawImage, cv::COLOR_GRAY2BGR);

			cv::Point2d ptCenterSub = (ptSub1 + ptSub2) / 2;
			cv::Point2d ptRot1 = rotatePoint(ptSub1, ptCenterSub, deg2rad(dAngle));
			cv::Point2d ptRot2 = rotatePoint(ptSub2, ptCenterSub, deg2rad(dAngle));

			cv::line(drawImage, pt1, pt2, cv::Scalar(0, 255, 0), 1);
			cv::line(drawImage, ptRot1 + (Point2d)roiRect.tl(), ptRot2 + (Point2d)roiRect.tl(), cv::Scalar(0, 255, 0), 1);

			ptCenterSub += (Point2d)roiRect.tl();

			double dDistance = abs(ptCenter.x - ptCenterSub.x);
			cv::putText(drawImage, cv::format("dist : %.2lfmm", (dDistance * 11.5) / 1000.0), cv::Point(100, 100), 0, 2, cv::Scalar(0, 255, 0), 2);
			SetImage(drawImage);
		}
	}

	return;
}

void CTab1::TestCode2() {
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray;
	if (image.channels() == 3)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	else
		gray = image.clone();

	// denoise
	//cv::Mat denoise;
	//cv::fastNlMeansDenoising(image, denoise, 3.0, 7, 21);

	// edge
	cv::Mat edge;
	cv::Canny(gray, edge, 10, 40);

	// get line
	cv::Point2d pt1, pt2;
	cv::Point2d ptSub1, ptSub2;

	if (image.channels() == 1)
		cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	vector<cv::Rect> roiRects;
	roiRects.push_back(cv::Rect(557, 300, 320, 120));
	roiRects.push_back(cv::Rect(1165, 300, 320, 120));
	roiRects.push_back(cv::Rect(1616, 300, 320, 120));
	roiRects.push_back(cv::Rect(2300, 300, 320, 120));
	roiRects.push_back(cv::Rect(2800, 300, 320, 120));
	roiRects.push_back(cv::Rect(557, 1800, 320, 120));
	roiRects.push_back(cv::Rect(1165, 1800, 320, 120));
	roiRects.push_back(cv::Rect(1616, 1800, 320, 120));
	roiRects.push_back(cv::Rect(2300, 1800, 320, 120));
	roiRects.push_back(cv::Rect(2800, 1800, 320, 120));

	cv::Mat drawImage = image.clone();;

	// FInd Line
	for (int i = 0; i < roiRects.size(); ++i) {
		eLINE_POSITION eLinePos = LEFT_LINE;
		if (i < 5) {
			eLinePos = TOP_LINE;
		}
		else if (i < 10) {
			eLinePos = BOTTOM_LINE;
		}

		if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
			0, 0,
			3, 100,
			eLinePos,
			pt1, pt2,
			FALSE, FALSE))
		{
			cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
			cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
		}
	}
	// FInd Line
	for (int i = 0; i < roiRects.size(); ++i) {
		eLINE_POSITION eLinePos = LEFT_LINE;
		if (i < 5) {
			eLinePos = TOP_LINE;
		}
		else if (i < 10) {
			eLinePos = BOTTOM_LINE;
		}

		if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
			0, 0,
			3, 100,
			eLinePos,
			pt1, pt2,
			FALSE, FALSE))
		{
			cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
			cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
		}
	}
	// FInd Line
	for (int i = 0; i < roiRects.size(); ++i) {
		eLINE_POSITION eLinePos = LEFT_LINE;
		if (i < 5) {
			eLinePos = TOP_LINE;
		}
		else if (i < 10) {
			eLinePos = BOTTOM_LINE;
		}

		if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
			0, 0,
			3, 100,
			eLinePos,
			pt1, pt2,
			FALSE, FALSE))
		{
			cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
			cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
		}
	}

	drawImage = image.clone();

	std::thread th1([&]() {
		// FInd Line
		for (int i = 0; i < roiRects.size(); ++i) {
			eLINE_POSITION eLinePos = LEFT_LINE;
			if (i < 5) {
				eLinePos = TOP_LINE;
			}
			else if (i < 10) {
				eLinePos = BOTTOM_LINE;
			}

			if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
				0, 0,
				3, 100,
				eLinePos,
				pt1, pt2,
				FALSE, FALSE))
			{
				cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
				cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
			}
		}
		}
	);
	std::thread th2([&]() {
		// FInd Line
		for (int i = 0; i < roiRects.size(); ++i) {
			eLINE_POSITION eLinePos = LEFT_LINE;
			if (i < 5) {
				eLinePos = TOP_LINE;
			}
			else if (i < 10) {
				eLinePos = BOTTOM_LINE;
			}

			if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
				0, 0,
				3, 100,
				eLinePos,
				pt1, pt2,
				FALSE, FALSE))
			{
				cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
				cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
			}
		}
		}
	);
	std::thread th3([&]() {
		// FInd Line
		for (int i = 0; i < roiRects.size(); ++i) {
			eLINE_POSITION eLinePos = LEFT_LINE;
			if (i < 5) {
				eLinePos = TOP_LINE;
			}
			else if (i < 10) {
				eLinePos = BOTTOM_LINE;
			}

			if (m_pImageProcess->GetLine_parallel(edge, roiRects[i],
				0, 0,
				3, 100,
				eLinePos,
				pt1, pt2,
				FALSE, FALSE))
			{
				cv::line(drawImage, pt1, pt2, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), 2);
				cv::rectangle(drawImage, roiRects[i], cv::Scalar(0, 0, 255), 1);
			}
		}
		}
	);

	th1.join();
	th2.join();
	th3.join();
	SetImage(drawImage);
	return;
}

// 재진테크, 내부 홀 검사
void CTab1::TestCode3() {
	cv::Mat image = GetImage();
	cv::Mat gray;
	if (image.channels() == 3)
		cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	else
		gray = image.clone();

	// get mark image
	cv::Mat mark_image = cv::imread("C:/Users/PC/Desktop/Sample/창원쇠샘플/m_mark.bmp");
	if (mark_image.channels() == 3)
		cvtColor(mark_image, mark_image, cv::COLOR_BGR2GRAY);

	// template matching
	cv::Mat dst;
	cv::matchTemplate(gray, mark_image, dst, 3);
	cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);

	double minVal, maxVal;
	cv::Point minPos, maxPos;
	cv::minMaxLoc(dst, &minVal, &maxVal, &minPos, &maxPos);

	if (image.channels() == 1)
		cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	// get rect from find pos
	cv::Rect rect = cv::Rect(maxPos.x, maxPos.y, mark_image.cols, mark_image.rows);
    cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 1);


	// set roi image
	int iSize = 300;
	cv::Point ptCenter = (rect.tl() + rect.br()) / 2;
	cv::Rect roiRect = cv::Rect(ptCenter.x - (iSize / 2), ptCenter.y - (iSize / 2), iSize, iSize);
	cv::Mat roiImage = gray(roiRect).clone();

	cv::GaussianBlur(roiImage, roiImage, cv::Size(3, 3), 0); 
	cv::Canny(roiImage, roiImage, 100, 200);

	std::vector<cv::Point> pts;
	cv::findNonZero(roiImage, pts);

	if (pts.size() > 1000) {
		for (int i = 0; i < pts.size(); ++i) {
			image.at<Vec3b>(pts[i] + roiRect.tl())[0] = 0;
			image.at<Vec3b>(pts[i] + roiRect.tl())[1] = 0;
			image.at<Vec3b>(pts[i] + roiRect.tl())[2] = 255;
		}
		cv::putText(image, "OK", roiRect.tl(), 0, 2, cv::Scalar(0, 255, 0), 2);
	}
	else {
		cv::putText(image, "NG", roiRect.tl(), 0, 2, cv::Scalar(0, 0, 255), 2);
	}

	SetImage(image);
	return;
}  

// 베터리 치수 측정
void CTab1::TestCode4(cv::Mat& image) {
	//cv::Mat image = GetImage();
	//if (image.empty())
	//	return;

	cv::Mat gray;
	if (image.channels() == 3)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	else
		gray = image.clone();

	double dWidth = 150;
	double dHeight = 50;

	eLINE_POSITION pos = eLINE_POSITION::LEFT_LINE;
	int iCanny1 = 30;
	int iCanny2 = 60;

	vector<cv::Point2d> result_pts;
	for (int i = 0; i < m_test4Pts.size(); ++i) {
		cv::Rect roiRect = cv::Rect(m_test4Pts[i].x - (dWidth / 2), m_test4Pts[i].y - (dHeight / 2), dWidth, dHeight);
		cv::rectangle(image, roiRect, cv::Scalar(0, 255, 0), 2);
		
		cv::Point2d pt1, pt2;
		if (i > 2) {
			pos = eLINE_POSITION::BOTTOM_LINE;
		}
		if (i == 6) {
			pos = eLINE_POSITION::TOP_LINE;
			iCanny1 = 0;
			iCanny2 = 30;
		}
		if (m_pImageProcess->GetLine_parallel(gray, roiRect, iCanny1, iCanny2, 3, 30, pos, pt1, pt2, FALSE, TRUE)) {
			result_pts.push_back((pt1 + pt2) / 2);
			cv::putText(image, cv::format("%d", i), roiRect.tl(), 0, 1, cv::Scalar(0, 0, 255), 1);
			cv::drawMarker(image, (pt1 + pt2) / 2, cv::Scalar(0, 255, 0), 0, 50, 1);
		}
	}

	double A	= abs(result_pts[0].x - result_pts[2].x) * 0.049224;
	double F	= abs(result_pts[0].x - result_pts[1].x) * 0.049224;
	double D	= abs(result_pts[3].y - result_pts[4].y) * 0.049224;
	double C1	= abs(result_pts[7].y - result_pts[6].y) * 0.049224;
	double C2	= abs(result_pts[5].y - result_pts[6].y) * 0.049224;

	cv::putText(image, cv::format("A : %.3lf (mm)", A), cv::Point(100, 100), 0, 2, cv::Scalar(0, 255, 0), 2);
	cv::putText(image, cv::format("F : %.3lf (mm)", F), cv::Point(100, 200), 0, 2, cv::Scalar(0, 255, 0), 2);
	cv::putText(image, cv::format("D : %.3lf (mm)", D), cv::Point(100, 300), 0, 2, cv::Scalar(0, 255, 0), 2);
	cv::putText(image, cv::format("C1: %.3lf (mm)", C1), cv::Point(100, 400), 0, 2, cv::Scalar(0, 255, 0), 2);
	cv::putText(image, cv::format("C2: %.3lf (mm)", C2), cv::Point(100, 500), 0, 2, cv::Scalar(0, 255, 0), 2);

	//m_testImage1 = image.clone();
	//SetImage(image);
	return;
}
// 베터리 치수 측정
void CTab1::TestCode5(cv::Mat& image) {
	//cv::Mat image = GetImage();
	//if (image.empty())
	//	return;

	cv::Mat gray;
	if (image.channels() == 3)
		cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	else
		gray = image.clone();

	double dWidth = 500;
	double dHeight = 150;
	int iCanny1 = 100;
	int iCanny2 = 200;
	cv::Point2d pt1, pt2;
	vector<cv::Point> result_pts;

	for (int i = 0; i < m_test5Pts.size(); ++i) {
		result_pts.clear();

		cv::Rect roiRect = cv::Rect(m_test5Pts[i].x - (dWidth / 2), m_test5Pts[i].y - (dHeight / 2), dWidth, dHeight);
		cv::rectangle(image, roiRect, cv::Scalar(0, 255, 0), 2);
		cv::Mat roiImage = gray(roiRect).clone();
		
		cv::GaussianBlur(roiImage, roiImage, cv::Size(3, 3), 0);
		cv::Canny(roiImage, roiImage, 0, 200);

        //if (m_pImageProcess->GetLine(roiImage, cv::Rect(0, 0, roiImage.cols, roiImage.rows), 0, 0, 3, 100, eLINE_POSITION::TOP_LINE, pt1, pt2, FALSE, FALSE)) {
        //    uchar* data = (uchar*)roiImage.data;
        //    for (int x = 0; x < roiImage.cols; ++x) {
        //        for (int y = 0; y < roiImage.rows; ++y) {
		//			if (data[y * roiImage.cols + x] == 255) {
		//				result_pts.push_back(cv::Point(x, y));
		//				break;
		//			}
		//		}
		//	}
		//
		//	vector<cv::Point2d> line_pts;
		//	cv::Mat temp = cv::Mat::zeros(roiImage.size(), CV_8UC1);
		//	for each (auto pt in result_pts)
		//	{
		//		if (m_pImageProcess->GetDistancePointToLine(pt1, pt2, pt) < 0.5) {
		//			line_pts.push_back(pt);
		//			temp.at<uchar>(pt) = 255;
		//		}
		//	}
		//	
		//	cv::Point ptH1, ptH2, ptL1, ptL2;
		//	for (int num = 0; num < line_pts.size() - 1; ++num) {
		//		if (abs(line_pts[num].x - line_pts[num + 1].x) > 15) {
		//			ptH1 = line_pts[num];
		//		}
		//	}
		//	for (int num = line_pts.size() - 1; num > 1; --num) {
		//		if (abs(line_pts[num].x - line_pts[num - 1].x) > 15) {
		//			ptH2 = line_pts[num];
		//		}
		//	}
		//	
		//	continue;
		//}

		cv::Mat temp = cv::Mat::zeros(roiImage.size(), CV_8UC1);

		uchar* data = (uchar*)roiImage.data;
        for (int x = 0; x < roiImage.cols; ++x) {
            for (int y = roiImage.rows - 1; y > 0; --y) {
                if (data[y * roiImage.cols + x] == 255) {
					result_pts.push_back(cv::Point(x, y));
					temp.at<uchar>(cv::Point(x, y)) = 255;
					break;
                }
            }
        }

		
		// 2차 미분
		vector<double> gradients, gradients2;
		int iDist = 20;
		cv::Mat test = cv::Mat::zeros(cv::Size(roiImage.cols, roiImage.rows*5), CV_8UC1);
		cv::Mat test2 = cv::Mat::zeros(cv::Size(roiImage.cols, roiImage.rows * 5), CV_8UC1);

        for (int index = iDist; index < result_pts.size() - iDist; ++index) {
            double dG = abs(result_pts[index - iDist].y - result_pts[index + iDist].y) / double(iDist * 2);

            gradients.push_back(dG * 100);
            test.at<uchar>(cv::Point(result_pts[index].x, dG * 100)) = 255;
        }
        for (int index = iDist; index < gradients.size() - iDist; ++index) {
            double dG = abs(gradients[index - iDist] - gradients[index + iDist]) / double(iDist * 2);

            gradients2.push_back(dG * 100);
            test2.at<uchar>(cv::Point(result_pts[index].x + iDist, dG * 100)) = 255;
        }
		
		// 극점 찾기
		vector<cv::Point> extreme_pts;
		data = test2.data;
        for (int y = test2.rows; y > 0; --y) {
            for (int x = 0; x < test2.cols; ++x) {
                if (extreme_pts.size() >= 4)
                    break;

                if (data[y * test2.cols + x] == 255) {
					// 극점이 여러개인 경우
					int zCount = 1;
					int zIndex = x;
					for (int z = 1; z < 5; ++z) {
						if (data[y * test2.cols + x + z] == 255) {
							zCount++;
							zIndex += (x + z);
						}
					}
					zIndex = zIndex / zCount;
                    extreme_pts.push_back(result_pts[zIndex] + roiRect.tl());

                    cv::rectangle(test2, cv::Rect(zIndex - 15, 0, 30, test2.rows), cv::Scalar::all(0), -1);
					y++;
                    break;
                }
            }
            if (extreme_pts.size() >= 4)
                break;
        }

		std::sort(extreme_pts.begin(), extreme_pts.end(),
			[](cv::Point pt1, cv::Point pt2)->bool {
				return pt1.x < pt2.x;
			}
		);
		
        for (int index = 0; index < extreme_pts.size(); ++index)
        {
            cv::drawMarker(image, extreme_pts[index], cv::Scalar(0, 0, 255), 0, 20);
            cv::putText(image, cv::format("%d", index), extreme_pts[index], 0, .5, cv::Scalar(0, 0, 255));
        }

		double M = m_pImageProcess->GetDistancePointToPoint(extreme_pts[0], extreme_pts[3]);
		double P = m_pImageProcess->GetDistancePointToPoint(extreme_pts[1], extreme_pts[2]);

		cv::putText(image, cv::format("M : %.3lf", M), roiRect.tl() + cv::Point(0, 15), 0, .5, cv::Scalar(0, 0, 255));
		cv::putText(image, cv::format("P : %.3lf", P), roiRect.tl() + cv::Point(0, 30), 0, .5, cv::Scalar(0, 0, 255));

		continue;
    }

	//m_testImage2 = image.clone();

	//SetImage(image);
	return;
}

void CTab1::TestROI(cv::Mat& image, cv::Mat mark, cv::Rect& roiRect) {
	cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
	cv::cvtColor(image, image, cv::COLOR_BayerBG2BGR);

	int match_method = cv::TM_CCORR_NORMED;

	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	if (mark.channels() == 3)
		cv::cvtColor(mark, mark, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	cv::matchTemplate(image, mark, dst, match_method);
	cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
	double minVal, maxVal;
	cv::Point minPos, maxPos;
	cv::minMaxLoc(dst, &minVal, &maxVal, &minPos, &maxPos);

	if (image.channels() == 1)
		cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	cv::rectangle(image, cv::Rect(maxPos.x, maxPos.y, mark.cols, mark.rows),
		cv::Scalar(0, 255, 0), 1);
	cv::rectangle(image, cv::Rect(maxPos.x - 450, maxPos.y - 30, 800, 150), 
		cv::Scalar(0, 255, 255), 2);

	return;
}


