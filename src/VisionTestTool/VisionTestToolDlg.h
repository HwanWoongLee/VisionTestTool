#pragma once
#define IDC_CAMERA_WND			7070
#define IDC_RESULT_VIEW_WND		7071
#define IDC_GRAPH_VIEW_WND		7071

class CTab1;
class CTab2;
class LUTManipulator;

// CVisionTestToolDlg 대화 상자
class CVisionTestToolDlg : public CDialogEx
{
// 생성입니다.
public:
	CVisionTestToolDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISIONTESTTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	cv::Mat			m_showImage;

	CTabCtrl		m_Tab;
	CTab1*			m_pTab1;
	CTab2*			m_pTab2;
	// LUTManipulator* m_pLUT;

	CCameraPaneWnd* m_pCamera;
	
	CMatView		m_resultView;
	CMatView		m_histogramView;
	CMatView		m_gammaGraphView;


	CButton			m_checkBoxGetViewImage;
	
	CEdit			m_editClipLimit;
	CEdit			m_editTileSize;
	CEdit			m_editGammaValue;

public:
	cv::Mat GetImage();
	cv::Mat GetShowImage() { return m_showImage; }

	void SetResultImage(cv::Mat image, BOOL bRedraw = TRUE);
	void CalcHist(cv::Mat image);


public:
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	
	afx_msg void OnBnClickedBtnShowImage();
	afx_msg void OnBnClickedBtnHistEqualization();
	afx_msg void OnBnClickedBtnHistStretch();
	afx_msg void OnBnClickedBtnWhiteBalance();
	afx_msg void OnBnClickedBtnClahe();
	
	afx_msg void OnBnClickedBtnBgr();
	afx_msg void OnBnClickedBtnHsv();
	afx_msg void OnBnClickedBtnBayertogray();
	afx_msg void OnBnClickedBtnGammaCorrection();
	CSliderCtrl m_sliderGamma;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditGammaValue();
};
