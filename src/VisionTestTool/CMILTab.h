#pragma once
#include "MILModule.h"
#include "CTab1.h"

class CMILTab : public CTab1
{
	DECLARE_DYNAMIC(CMILTab)

public:
	CMILTab(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMILTab();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	CMILModule* m_pMIL = nullptr;
	
	CEdit m_editMMFPath;
	CEdit m_editStrPath;
	CEdit m_editMCOPath;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnMmfLoad();
	afx_msg void OnBnClickedBtnModelFind();
	afx_msg void OnBnClickedBtnMsrLoad();
	afx_msg void OnBnClickedBtnStringRead();
	afx_msg void OnBnClickedBtnMcoLoad();
	afx_msg void OnBnClickedBtnCodeRead();
};
