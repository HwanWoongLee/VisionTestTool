#pragma once


// CTab1 대화 상자
class CVisionTestToolDlg;
class CImageProcess;
class CMILModule;

class CTab1 : public CDialogEx
{
	DECLARE_DYNAMIC(CTab1)

public:
	CTab1(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTab1();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	CVisionTestToolDlg*		m_pParent = nullptr;
	CImageProcess*			m_pImageProcess = nullptr;

	cv::Mat GetImage();
	cv::Mat RotateImage(cv::Mat image, double dAngle);
	void SetImage(cv::Mat image, BOOL bRedraw = TRUE);

private:
	BOOL m_bFirst = FALSE;

	CComboBox m_comboMorph;
	CComboBox m_comboThreshold;

	CSliderCtrl m_sliderCanny1;
	CSliderCtrl m_sliderCanny2;

	CEdit m_editBlurSize;
	CEdit m_editGaussianSize;
	CEdit m_editMedianSize;
	CEdit m_editBilateralD;
	CEdit m_editBilateralColor;
	CEdit m_editBilateralSpace;
	CEdit m_editMorphSize;
	CEdit m_editThreshold1;
	CEdit m_editThreshold2;
	CEdit m_editBlockSize;
	CEdit m_editC;
	CEdit m_editCanny1;
	CEdit m_editCanny2;
	CEdit m_editSobelSize;
	CEdit m_editHoughCircleDist;
	CEdit m_editHoughCircleParam1;
	CEdit m_editHoughCircleParam2;
	CEdit m_editHoughCircleMin;
	CEdit m_editHoughCircleMax;
	CEdit m_editHoughLineThresh;
	CEdit m_editDenoiseH;
	CEdit m_editDenoiseTemplate;
	CEdit m_editDenoiseSearch;

	int m_radioSobel;

private:
	// >> 점안액 TEST
	CMILModule* m_pMIL = nullptr;

	void SetMILContext();
	bool GetMarkImage(const cv::Mat& image, cv::Mat& dst);
	bool CheckBlack(const cv::Mat& image, cv::Mat& dst, int iThresh, int iMinSize = 0); // true:OK, false:NG
	bool CheckLine(const cv::Mat& image, cv::Mat& dst, int iMin, int iMax);				// true:OK, false:NG
	//<<

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnBlur();
	afx_msg void OnBnClickedBtnGaussianBlur();
	afx_msg void OnBnClickedBtnMedianBlur();
	afx_msg void OnBnClickedBtnBilateralBlur();
	afx_msg void OnBnClickedBtnMorph();
	afx_msg void OnBnClickedBtnThreshold();
	afx_msg void OnBnClickedBtnAdaptiveThresh();
	afx_msg void OnBnClickedBtnCanny();
	afx_msg void OnBnClickedBtnSobel();
	afx_msg void OnBnClickedBtnHoughCircle();
	afx_msg void OnBnClickedBtnHoughLine();
	afx_msg void OnBnClickedBtnDenoise();
	afx_msg void OnBnClickedBtnTest();

public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
