#pragma once
#include "CTab1.h"


class CVisionTestToolDlg;
class CImageProcess;
class LUTManipulator;
class CHazeRemoval;

class CTab3 : public CTab1
{
	DECLARE_DYNAMIC(CTab3)

public:
	CTab3(CWnd* pParent = nullptr); 
	virtual ~CTab3();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

private:
	CHazeRemoval* m_pHazeRemoval;

	CEdit m_editDHRadius;
	CEdit m_editDHOmega;
	CEdit m_editDHT0;
	CEdit m_editDHR;
	CEdit m_editDHEPS;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnDehaze();
	
	afx_msg void OnBnClickedBtnRetina();
	afx_msg void OnBnClickedBtnIllum();
};
