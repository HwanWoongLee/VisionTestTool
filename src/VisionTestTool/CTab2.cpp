// CTab2.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "CTab2.h"
#include "afxdialogex.h"
#include "ImageProcess.h"
#include "VisionTestToolDlg.h"

// CTab2 대화 상자

IMPLEMENT_DYNAMIC(CTab2, CDialogEx)

CTab2::CTab2(CWnd* pParent /*=nullptr*/)
	//: CDialogEx(IDD_DIALOG2, pParent)
	: CTab1(pParent)
{
	m_pParent = (CVisionTestToolDlg*)pParent;
	
}

CTab2::~CTab2()
{

}

void CTab2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RANSAC_THRESH1, m_editRansacThresh1);
	DDX_Control(pDX, IDC_EDIT_RANSAC_THRESH2, m_editRansacThresh2);
	DDX_Control(pDX, IDC_EDIT_RANSAC_SIZE, m_editRansacSize);
	DDX_Control(pDX, IDC_COMBO_RANSAC_POS, m_comboRansacPos);
	DDX_Control(pDX, IDC_CHECK_RANSAC_CENTER, m_checkCenter);
	DDX_Control(pDX, IDC_EDIT_ROTATE, m_editRotate);
	DDX_Control(pDX, IDC_SLIDER_ROTATE, m_sliderRotate);
	DDX_Control(pDX, IDC_EDIT_RESIZE, m_editResize);
	DDX_Control(pDX, IDC_COMBO_CONTOUR_MODE, m_comboContourMode);
	DDX_Control(pDX, IDC_COMBO_CONTOUR_METHOD, m_comboContourMethod);
}


BEGIN_MESSAGE_MAP(CTab2, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_RANSAC_LINE, &CTab2::OnBnClickedBtnRansacLine)
	ON_BN_CLICKED(IDC_BTN_RANSAC_CIRCLE, &CTab2::OnBnClickedBtnRansacCircle)
	ON_BN_CLICKED(IDC_BTN_RANSAC_ELLIPSE, &CTab2::OnBnClickedBtnRansacEllipse)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_ROTATE, &CTab2::OnEnChangeEditRotate)
	ON_BN_CLICKED(IDC_BTN_RESIZE, &CTab2::OnBnClickedBtnResize)
	ON_BN_CLICKED(IDC_BTN_CONTOUR, &CTab2::OnBnClickedBtnContour)
	ON_BN_CLICKED(IDC_BTN_CONVEX_HULL, &CTab2::OnBnClickedBtnConvexHull)
	ON_BN_CLICKED(IDC_BTN_MATCHING_LOAD_MARK, &CTab2::OnBnClickedBtnMatchingLoadMark)
	ON_BN_CLICKED(IDC_BTN_MATCHING_TEMPLATE, &CTab2::OnBnClickedBtnMatchingTemplate)
END_MESSAGE_MAP()


void CTab2::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow && !m_bFirst) {
		m_bFirst = TRUE;

		m_editRansacThresh1.SetWindowTextW(_T("100"));
		m_editRansacThresh2.SetWindowTextW(_T("200"));
		m_editRansacSize.SetWindowTextW(_T("100"));
		m_comboRansacPos.SetCurSel(0);

		m_sliderRotate.SetRange(0, 360);
		m_sliderRotate.SetPos(0);
		m_sliderRotate.SetTicFreq(45);
		m_sliderRotate.SetPageSize(10);

		m_editRotate.SetWindowTextW(_T("0"));
		m_editResize.SetWindowTextW(_T("1.0"));

		m_comboContourMode.AddString(_T("RETR_EXTERNAL"));				// 0 외부 윤곽만 검색
		m_comboContourMode.AddString(_T("RETR_LIST"));					// 1 모든 윤관선 검색
		m_comboContourMode.AddString(_T("RETR_CCOMP"));					// 2 두 수준의 계층 구조
		m_comboContourMode.AddString(_T("RETR_TREE"));					// 3 모든 윤과석 검색 후 계층 구조 재구성
		m_comboContourMode.AddString(_T("RETR_FLOODFILL"));				// 4 

		m_comboContourMethod.AddString(_T("CHAIN_APPROX_NONE"));		// 1 
		m_comboContourMethod.AddString(_T("CHAIN_APPROX_SIMPLE"));		// 2
		m_comboContourMethod.AddString(_T("CHAIN_APPROX_TC89_L1"));		// 3 
		m_comboContourMethod.AddString(_T("CHAIN_APPROX_TC89_KCOS"));	// 4 

		m_comboContourMode.SetCurSel(1);
		m_comboContourMethod.SetCurSel(0);
        
		if (!m_markView.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_STATIC_MATCHING_MARK)) {
			return;
		}
		auto pWnd = GetDlgItem(IDC_STATIC_MATCHING_MARK);
		if (pWnd) {
			CRect rect;
			pWnd->GetWindowRect(&rect);
			ScreenToClient(rect);
			m_markView.MoveWindow(rect);
		}
	}
	return;
}

void CTab2::OnBnClickedBtnRansacLine()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray = image.clone();
	if (gray.channels() == 3)
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

	CString str;
	m_editRansacThresh1.GetWindowTextW(str);
	int iThresh1 = _ttoi(str);
	m_editRansacThresh2.GetWindowTextW(str);
	int iThresh2 = _ttoi(str);
	m_editRansacSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	int iPos = m_comboRansacPos.GetCurSel();

	cv::Mat edge;
	cv::GaussianBlur(gray, edge, cv::Size(3, 3), 0);
	cv::Canny(edge, edge, iThresh1, iThresh2);

	cv::Point2d pt1, pt2;
	BOOL bFind = FALSE;

    if (m_checkCenter.GetCheck()) {
        bFind = m_pImageProcess->GetLineFromCenter(edge, cv::Point(edge.cols / 2, edge.rows / 2),
            0, 0,
            3, iSize,
            (eLINE_POSITION)iPos,
            pt1, pt2, FALSE, FALSE);
    }
    else {
        bFind = m_pImageProcess->GetLine(edge, cv::Rect(0, 0, edge.cols, edge.rows),
            0, 0,
            3, iSize,
            (eLINE_POSITION)iPos,
            pt1, pt2, FALSE, FALSE);
    }

	if (bFind) {
		cv::Mat dst = image.clone();
		if (dst.channels() == 1)
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

		cv::line(dst, pt1, pt2, cv::Scalar(0, 255, 0), 2);
		SetImage(dst);
	}

	return;
}

void CTab2::OnBnClickedBtnRansacCircle()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray = image.clone();
	if (gray.channels() == 3)
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

	CString str;
	m_editRansacThresh1.GetWindowTextW(str);
	int iThresh1 = _ttoi(str);
	m_editRansacThresh2.GetWindowTextW(str);
	int iThresh2 = _ttoi(str);
	m_editRansacSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	int iPos = m_comboRansacPos.GetCurSel();

	cv::Mat edge;
	cv::GaussianBlur(gray, edge, cv::Size(3, 3), 0);
	cv::Canny(edge, edge, iThresh1, iThresh2);

	cv::Vec3d circle;
	BOOL bFind = FALSE;
	if (m_checkCenter.GetCheck()) {
		bFind = m_pImageProcess->GetCircleFromCenter(edge, cv::Rect(0, 0, edge.cols, edge.rows),
														0, 0,
														3, iSize,
														circle, FALSE);
	}
    else {
        bFind = m_pImageProcess->GetCircle(edge, cv::Rect(0, 0, edge.cols, edge.rows),
											0, 0,
											3, iSize,
											(eLINE_POSITION)iPos,
											circle, FALSE);
    }

	if (bFind) {
		cv::Mat dst = image.clone();
		if (dst.channels() == 1)
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

		cv::circle(dst, cv::Point(circle[0], circle[1]), circle[2], cv::Scalar(0, 0, 255), 2);
		SetImage(dst);
	}
	return;
}


void CTab2::OnBnClickedBtnRansacEllipse()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray = image.clone();
	if (gray.channels() == 3)
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

	CString str;
	m_editRansacThresh1.GetWindowTextW(str);
	int iThresh1 = _ttoi(str);
	m_editRansacThresh2.GetWindowTextW(str);
	int iThresh2 = _ttoi(str);
	m_editRansacSize.GetWindowTextW(str);
	int iSize = _ttoi(str);

	cv::Mat edge;
	cv::GaussianBlur(gray, edge, cv::Size(3, 3), 0);
	cv::Canny(edge, edge, iThresh1, iThresh2);

	cv::Vec6d ellipse;
	BOOL bFind = m_pImageProcess->GetEllipse(edge, cv::Rect(0, 0, edge.cols, edge.rows),
		0, 0,
		3, iSize,
		ellipse, FALSE);

	if (bFind) {
		cv::Mat dst = image.clone();
		if (dst.channels() == 1)
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
		
		cv::ellipse(dst, cv::Point(ellipse[0], ellipse[1]), cv::Size(ellipse[2], ellipse[3]), ellipse[4] * 180.0 / _M_PI, 0, 360, cv::Scalar(0, 0, 255), 2);
		SetImage(dst);
	}
	return;
}


void CTab2::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar) {
		if (pScrollBar == (CScrollBar*)&m_sliderRotate)
		{
			int iPos = m_sliderRotate.GetPos();
			m_editRotate.SetWindowTextW(Format(_T("%d"), iPos));

			cv::Mat image = m_pParent->GetShowImage();
            if (!image.empty()) {
                SetImage(RotateImage(image, iPos), FALSE);
            }
		}
	}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CTab2::OnEnChangeEditRotate()
{
	CString str;
	m_editRotate.GetWindowTextW(str);
	int iPos = _ttoi(str);
	m_sliderRotate.SetPos(iPos);

	cv::Mat image = m_pParent->GetShowImage();
	if (!image.empty()) {
		SetImage(RotateImage(image, iPos), FALSE);
	}

	return;
}


void CTab2::OnBnClickedBtnResize()
{
	CString str;
	m_editResize.GetWindowTextW(str);
	float fRate = _tstof(str);

	cv::Mat image = m_pParent->GetShowImage();
	if (image.empty())
		return;

	cv::resize(image, image, cv::Size(image.cols * fRate, image.rows * fRate));
	SetImage(image);

	return;
}


void CTab2::OnBnClickedBtnContour()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray = image.clone();
	if (gray.channels() == 3)
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);
	
	int iMode = m_comboContourMode.GetCurSel(); // cv::RETR_LIST;
	int iMethod = m_comboContourMethod.GetCurSel() + 1; //cv::CHAIN_APPROX_SIMPLE;

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(gray, contours, iMode, iMethod);

	if (contours.size() > 0) {
		cv::Mat dst = image.clone();
		if (dst.channels() == 1)
			cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

		for (int i = 0; i < contours.size(); ++i) {
			drawContours(dst, contours, i, cv::Scalar(rand() % 255, rand() % 255, rand() % 255), -1);
		}

		SetImage(dst);
	}

	return;
}


void CTab2::OnBnClickedBtnConvexHull()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat gray = image.clone();
	if (gray.channels() == 3)
		cv::cvtColor(gray, gray, cv::COLOR_BGR2GRAY);

	int iMode = m_comboContourMode.GetCurSel(); // cv::RETR_LIST;
	int iMethod = m_comboContourMethod.GetCurSel() + 1; //cv::CHAIN_APPROX_SIMPLE;

	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(gray, contours, iMode, iMethod);

	if (contours.size() > 0) {
        vector<vector<Point>>hull(contours.size());
		for (size_t i = 0; i < contours.size(); i++) {
			convexHull(contours[i], hull[i]);
		}

		Mat drawing = Mat::zeros(gray.size(), CV_8UC3);
		for (size_t i = 0; i < contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(rand() % 255, rand() % 255, rand() % 255);
			drawContours(drawing, contours, (int)i, color);
			drawContours(drawing, hull, (int)i, color);
		}
		SetImage(drawing);
	}
	return;
}


void CTab2::OnBnClickedBtnMatchingLoadMark()
{
	static TCHAR BASED_CODE szFilter[] = _T("이미지 파일(*.BMP, *.GIF, *.JPG) | *.BMP;*.GIF;*.JPG;*.bmp;*.jpg;*.gif |모든파일(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T("image"), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		m_markImage = cv::imread(std::string(CT2CA(pathName)));
		if (!m_markImage.empty()) {
			m_markView.SetImage(m_markImage, 0.0);
		}
	}
	return;
}


void CTab2::OnBnClickedBtnMatchingTemplate()
{
	cv::Mat image = GetImage();
	
	int match_method = cv::TM_CCORR_NORMED;
	// 0: TM_SQDIFF
	// 1: TM_SQDIFF_NORMED
	// 2: TM_CCORR
	// 3: TM_CCORR_NORMED
	// 4: TM_COEFF
	// 5: TM_COEFF_NORMED
	
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	if (m_markImage.channels() == 3)
		cv::cvtColor(m_markImage, m_markImage, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	cv::matchTemplate(image, m_markImage, dst, match_method);
	cv::normalize(dst, dst, 0, 1, cv::NORM_MINMAX);
	double minVal, maxVal;
	cv::Point minPos, maxPos;
	cv::minMaxLoc(dst, &minVal, &maxVal, &minPos, &maxPos);

	if (image.channels() == 1)
		cv::cvtColor(image, image, cv::COLOR_GRAY2BGR);

	cv::rectangle(image, cv::Rect(maxPos.x, maxPos.y, m_markImage.cols, m_markImage.rows),
		cv::Scalar(0, 255, 0), 1);

	SetImage(image);

	return;
}
