#include "pch.h"
#include "framework.h"
#include "VisionTestTool.h"
#include "VisionTestToolDlg.h"
#include "afxdialogex.h"
#include "CTab1.h"
#include "CTab2.h"
#include "CMILTab.h"
#include "ImageProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CVisionTestToolDlg::CVisionTestToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_VISIONTESTTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVisionTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_CHECK_GET_VIEW_IMAGE,	m_checkBoxGetViewImage);
	DDX_Control(pDX, IDC_EDIT_CLIP_LIMIT,		m_editClipLimit);
	DDX_Control(pDX, IDC_EDIT_TILE_SIZE,		m_editTileSize);
	DDX_Control(pDX, IDC_EDIT_GAMMA_VALUE,		m_editGammaValue);
	DDX_Control(pDX, IDC_SLIDER_GAMMA_VALUE,	m_sliderGamma);
}

BEGIN_MESSAGE_MAP(CVisionTestToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1,			&CVisionTestToolDlg::OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_BTN_SHOW_IMAGE,			&CVisionTestToolDlg::OnBnClickedBtnShowImage)
	ON_BN_CLICKED(IDC_BTN_HIST_EQUALIZATION,	&CVisionTestToolDlg::OnBnClickedBtnHistEqualization)
	ON_BN_CLICKED(IDC_BTN_HIST_STRETCH,			&CVisionTestToolDlg::OnBnClickedBtnHistStretch)
	ON_BN_CLICKED(IDC_BTN_WHITE_BALANCE,		&CVisionTestToolDlg::OnBnClickedBtnWhiteBalance)
	ON_BN_CLICKED(IDC_BTN_CLAHE,				&CVisionTestToolDlg::OnBnClickedBtnClahe)
	ON_BN_CLICKED(IDC_BTN_BGR,					&CVisionTestToolDlg::OnBnClickedBtnBgr)
	ON_BN_CLICKED(IDC_BTN_HSV,					&CVisionTestToolDlg::OnBnClickedBtnHsv)
	ON_BN_CLICKED(IDC_BTN_BAYERTOGRAY,			&CVisionTestToolDlg::OnBnClickedBtnBayertogray)
	ON_BN_CLICKED(IDC_BTN_GAMMA_CORRECTION,		&CVisionTestToolDlg::OnBnClickedBtnGammaCorrection)
	ON_EN_CHANGE(IDC_EDIT_GAMMA_VALUE,			&CVisionTestToolDlg::OnEnChangeEditGammaValue)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CVisionTestToolDlg 메시지 처리기

BOOL CVisionTestToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// Tab Control
	m_Tab.InsertItem(0, _T("Tab1"));
	m_Tab.InsertItem(1, _T("Tab2"));
	m_Tab.InsertItem(2, _T("Matrox"));

	m_Tab.SetCurSel(0);

	CRect rect;
	m_Tab.GetWindowRect(&rect);

	m_pTab1 = new CTab1(this);
	m_pTab1->Create(IDD_DIALOG1, &m_Tab);
	m_pTab1->MoveWindow(0, 20, rect.Width(), rect.Height() - 20);
	m_pTab1->ShowWindow(SW_SHOW);

	m_pTab2 = new CTab2(this);
	m_pTab2->Create(IDD_DIALOG2, &m_Tab);
	m_pTab2->MoveWindow(0, 20, rect.Width(), rect.Height() - 20);
	m_pTab2->ShowWindow(SW_HIDE);

	m_pTabMIL = new CMILTab(this);
	m_pTabMIL->Create(IDD_DIALOG3, &m_Tab);
	m_pTabMIL->MoveWindow(0, 20, rect.Width(), rect.Height() - 20);
	m_pTabMIL->ShowWindow(SW_HIDE);

	// Create Camera window
	m_pCamera = new CCameraPaneWnd();
	if (!m_pCamera->Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_CAMERA_WND)) {
		return FALSE;
	}
	CCameraLayoutItem layout;
	layout.Load(g_ipClient.GetAbsPath(_T("D:/Project/VisionTestTool/cfg/Vision_dummy.camLayout")));
	if (layout.m_child.N() == 1) {
		m_pCamera->InitPane(&layout.m_child[0]);
	}
	else {
		m_pCamera->InitPane(&layout);
	}

	CWnd* pWnd = GetDlgItem(IDC_STATIC_CAMERA_VIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	m_pCamera->MoveWindow(rect);

	if (!m_resultView.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_RESULT_VIEW_WND)) {
		return FALSE;
	}
	pWnd = GetDlgItem(IDC_STATIC_RESULT_VIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	m_resultView.MoveWindow(rect);

	if (!m_gammaGraphView.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_GRAPH_VIEW_WND)) {
		return FALSE;
	}
	pWnd = GetDlgItem(IDC_STATIC_GRAPH_VIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	m_gammaGraphView.MoveWindow(rect);
	m_gammaGraphView.ShowTool(FALSE);

	if (!m_histogramView.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_RESULT_VIEW_WND)) {
		return FALSE;
	}
	pWnd = GetDlgItem(IDC_STATIC_HISTOGRAM_VIEW);
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);
	m_histogramView.MoveWindow(rect);
	m_histogramView.ShowTool(FALSE);

	m_sliderGamma.SetRange(0, 30);
	
	m_editClipLimit.SetWindowTextW(_T("2.0"));
	m_editTileSize.SetWindowTextW(_T("8"));
	m_editGammaValue.SetWindowTextW(_T("0.5"));

	return TRUE;
}


void CVisionTestToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CVisionTestToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CVisionTestToolDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int sel = m_Tab.GetCurSel();

	switch (sel) {
		case 0: {
			m_pTab1->ShowWindow(SW_SHOW);
			m_pTab2->ShowWindow(SW_HIDE);
			m_pTabMIL->ShowWindow(SW_HIDE);
			break;
		}
		case 1: {
			m_pTab1->ShowWindow(SW_HIDE);
			m_pTab2->ShowWindow(SW_SHOW);
			m_pTabMIL->ShowWindow(SW_HIDE);
			break;
        }
		case 2: {
			m_pTab1->ShowWindow(SW_HIDE);
			m_pTab2->ShowWindow(SW_HIDE);
			m_pTabMIL->ShowWindow(SW_SHOW);
            break;
        }
	}
	*pResult = 0;
}

cv::Mat CVisionTestToolDlg::GetImage() {
	cv::Mat image;
	if (m_checkBoxGetViewImage.GetCheck())
		m_resultView.GetImage(image);
	else
		m_pCamera->GetCameraView()->GetImage(image);

	return image;
}

void CVisionTestToolDlg::SetResultImage(cv::Mat image, BOOL bRedraw) {
	m_resultView.SetImage(image, 0.0);
	CalcHist(image);

    if (bRedraw)
        m_showImage = image.clone();

	return;
}

void CVisionTestToolDlg::CalcHist(cv::Mat image) {
	int iHistSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };

	int iWidth = 1024;
	cv::Mat histImage(400, iWidth, CV_8UC3, cv::Scalar(0, 0, 0));

	if (image.channels() == 3) {
		std::vector<cv::Mat> bgr_planes;
		split(image, bgr_planes);

		cv::Mat b_hist, g_hist, r_hist;
		cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &iHistSize, &histRange);
		cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &iHistSize, &histRange);
		cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &iHistSize, &histRange);

		int bin_w = cvRound((double)iWidth / iHistSize);

		cv::normalize(b_hist, b_hist, 0, 400, cv::NORM_MINMAX);
		cv::normalize(g_hist, g_hist, 0, 400, cv::NORM_MINMAX);
		cv::normalize(r_hist, r_hist, 0, 400, cv::NORM_MINMAX);

		for (int i = 1; i < iHistSize; i++)
		{
			line(histImage,
				cv::Point(bin_w * (i - 1), 400 - cvRound(b_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), 400 - cvRound(b_hist.at<float>(i))),
				cv::Scalar(255, 0, 0), 2);
			line(histImage,
				cv::Point(bin_w * (i - 1), 400 - cvRound(g_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), 400 - cvRound(g_hist.at<float>(i))),
				cv::Scalar(0, 255, 0), 2);
			line(histImage,
				cv::Point(bin_w * (i - 1), 400 - cvRound(r_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), 400 - cvRound(r_hist.at<float>(i))),
				cv::Scalar(0, 0, 255), 2);
		}
	}
	else {
		cv::Mat gray_hist;
		cv::calcHist(&image, 1, 0, cv::Mat(), gray_hist, 1, &iHistSize, &histRange);

		int bin_w = cvRound((double)iWidth / iHistSize);

		cv::normalize(gray_hist, gray_hist, 0, 400, cv::NORM_MINMAX);

		for (int i = 1; i < iHistSize; i++)
		{
			line(histImage,
				cv::Point(bin_w * (i - 1), 400 - cvRound(gray_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), 400 - cvRound(gray_hist.at<float>(i))),
				cv::Scalar(255, 255, 255), 2);
		}
	}

	m_histogramView.SetImage(histImage);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnShowImage()
{
	cv::Mat image;
	m_pCamera->GetCameraView()->GetImage(image);

	SetResultImage(image);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnHistEqualization()
{
	cv::Mat image = GetImage();
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	cv::equalizeHist(image, dst);
	
	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnHistStretch()
{
	cv::Mat image = GetImage();
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	cv::normalize(image, dst, 0, 255, cv::NORM_MINMAX);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnWhiteBalance()
{
	cv::Mat image = GetImage();
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Mat dst;
	cv::Ptr<cv::xphoto::WhiteBalancer> wb = cv::xphoto::createSimpleWB();
	wb->balanceWhite(image, dst);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnClahe()
{
	cv::Mat image = GetImage();
	if (image.channels() == 3)
		cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	
	CString str;
	m_editClipLimit.GetWindowTextW(str);
	double dClipLimit = _wtof(str);
	m_editTileSize.GetWindowTextW(str);
	int iTileSize = _ttoi(str);

	clahe->setClipLimit(dClipLimit);
	clahe->setTilesGridSize(cv::Size(iTileSize, iTileSize));

	cv::Mat dst;
	clahe->apply(image, dst);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnBgr()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() != 3)
		return;

	cv::Mat spImage[3];
	cv::split(image, spImage);

	cv::Mat dst;
	cv::hconcat(spImage, 3, dst);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnHsv()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	if (image.channels() != 3)
		return;

	cv::cvtColor(image, image, cv::COLOR_BGR2HSV);

	cv::Mat spImage[3];
	cv::split(image, spImage);

	cv::Mat dst;
	cv::hconcat(spImage, 3, dst);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnBayertogray()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat dst;
	cv::cvtColor(image, dst, cv::COLOR_BGR2GRAY);
	cv::cvtColor(dst, dst, cv::COLOR_BayerBG2GRAY);

	SetResultImage(dst);
	return;
}


void CVisionTestToolDlg::OnBnClickedBtnGammaCorrection()
{
	cv::Mat image = GetImage();
	if (image.empty())
		return;

	cv::Mat lut(1, 256, CV_8UC3);

	CString str;
	m_editGammaValue.GetWindowTextW(str);
	
	double gamma_value = _wtof(str);

	if (gamma_value < 0)
		return;

	for (int i = 0; i < lut.cols; ++i) {
		auto data = lut.data;
        data[i * 3 + 0] = pow(i / 255.0, 1.0 / gamma_value) * 255.0;
		data[i * 3 + 1] = pow(i / 255.0, 1.0 / gamma_value) * 255.0;
		data[i * 3 + 2] = pow(i / 255.0, 1.0 / gamma_value) * 255.0;
	}

	cv::Mat graph = cv::Mat::zeros(cv::Size(256, 256), CV_8UC1);
	for (int i = 0; i < lut.cols; ++i) {
		auto lut_data = lut.data;
		graph.at<uchar>(cv::Point(i, 255 - lut_data[i * 3 + 0])) = 255;
	}

	m_gammaGraphView.SetImage(graph, 0.0);
	cv::Mat dst;
	cv::LUT(image, lut, dst);

	SetResultImage(dst);
	return;
}

void CVisionTestToolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (IDC_SLIDER_GAMMA_VALUE == pScrollBar->GetDlgCtrlID()) {
		double dPos = m_sliderGamma.GetPos();

		CString str = Format(_T("%.1lf"), dPos * 0.1);
		m_editGammaValue.SetWindowTextW(str);
		OnBnClickedBtnGammaCorrection();
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVisionTestToolDlg::OnEnChangeEditGammaValue()
{
	CString str;
	m_editGammaValue.GetWindowTextW(str);
    
	double dPos = _ttof(str);
	m_sliderGamma.SetPos(dPos * 10);

}
