#pragma once
#define IDC_CAMERA_WND			7070
#define IDC_RESULT_VIEW_WND		7071
#define IDC_GRAPH_VIEW_WND		7071

class CTab1;
class CTab2;
class LUTManipulator;

class CVisionTestToolDlg : public CDialogEx
{
public:
	CVisionTestToolDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VISIONTESTTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	cv::Mat			m_showImage;

	CTabCtrl		m_Tab;
	CTab1*			m_pTab1;
	CTab2*			m_pTab2;
	
	CCameraPaneWnd* m_pCamera;
	
	CMatView		m_resultView;
	CMatView		m_histogramView;
	CMatView		m_gammaGraphView;

	CButton			m_checkBoxGetViewImage;
	
	CEdit			m_editClipLimit;
	CEdit			m_editTileSize;
	CEdit			m_editGammaValue;

	CSliderCtrl		m_sliderGamma;

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
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditGammaValue();
};
