#pragma once
#include "CTab1.h"

class CVisionTestToolDlg;
class CImageProcess;
class CTab2 : public CTab1
{
	DECLARE_DYNAMIC(CTab2)

public:
	CTab2(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTab2();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif
protected:
	CVisionTestToolDlg*		m_pParent = nullptr;

	

private:
	CMatView	m_markView;
	cv::Mat		m_markImage;

	BOOL		m_bFirst = FALSE;
	CEdit		m_editRansacThresh1;
	CEdit		m_editRansacThresh2;
	CEdit		m_editRansacSize;
	CEdit		m_editResize;
	CEdit		m_editRotate;

	CSliderCtrl m_sliderRotate;

	CComboBox	m_comboRansacPos;
	CComboBox	m_comboContourMode;
	CComboBox	m_comboContourMethod;

	CButton		m_checkCenter;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnRansacLine();
	afx_msg void OnBnClickedBtnRansacCircle();
	afx_msg void OnBnClickedBtnRansacEllipse();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditRotate();
	afx_msg void OnBnClickedBtnResize();
	afx_msg void OnBnClickedBtnContour();
	afx_msg void OnBnClickedBtnConvexHull();
	afx_msg void OnBnClickedBtnMatchingLoadMark();
	afx_msg void OnBnClickedBtnMatchingTemplate();
};
