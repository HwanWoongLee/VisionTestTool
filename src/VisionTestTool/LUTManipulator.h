#pragma once
#include "ImageProcess.h"

// LUTManipulator 대화 상자
using namespace std;
using namespace cv;

class LUTManipulator : public CDialogEx
{
	DECLARE_DYNAMIC(LUTManipulator)

public:
	LUTManipulator(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~LUTManipulator();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LUT };
#endif

private:
	CImageProcess		m_imgProc;

	vector<cv::Point>	m_pts;
	std::vector<float>	m_params;

	double				m_dWidth, m_dHeight;

	bool				m_bLBDown = false;
	int					m_iSelectIndex = -1;


public:
	void InitPts();
	cv::Mat GetLUT();


protected:
	void DrawGraph(CDC& pDC, double dWidth, double dHeight);
	bool CheckMousePointInLine(const CPoint& pt);
	bool CheckInPoint(const CPoint& pt, int& index_pt);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
