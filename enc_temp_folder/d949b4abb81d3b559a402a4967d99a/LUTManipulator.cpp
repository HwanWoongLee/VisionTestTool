// LUTManipulator.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "LUTManipulator.h"
#include "afxdialogex.h"


// LUTManipulator 대화 상자

IMPLEMENT_DYNAMIC(LUTManipulator, CDialogEx)

LUTManipulator::LUTManipulator(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LUT, pParent)
{
	InitPts();
}

LUTManipulator::~LUTManipulator()
{
}

void LUTManipulator::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void LUTManipulator::InitPts() {
	m_pts.clear();
	m_pts.push_back(cv::Point(0, 0));
	m_pts.push_back(cv::Point(255, 255));

}

void LUTManipulator::DrawGraph(CDC& pDC, double dWidth, double dHeight) {
	if (m_pts.size() < 2)
		return;

	m_imgProc.GetPolynomialParameters(m_pts, m_params);

	if (m_params.empty())
		return;

	int nCount = m_params.size();

	vector<float> values;

	for (int x = 0; x < 255; ++x) {
		float fy = 0;

		for (int n = 0; n < nCount; ++n) {
			fy += m_params[n] * pow(x, n);
		}
		values.push_back(fy);
	}

    for (int x = 0; x < 255 - 1; ++x) {
		pDC.MoveTo(x * dWidth, values[x] * dHeight);
		pDC.LineTo((x + 1) * dWidth, values[x + 1] * dHeight);
	}

	return;
}

bool LUTManipulator::CheckMousePointInLine(const CPoint& pt) {
	if (m_params.size() < 2)
		return false;

	int nCount = m_params.size();
	float fy = 0;
	
	CPoint real_pt = CPoint(pt.x / m_dWidth, pt.y / m_dHeight);

	for (int i = 0; i < nCount; ++i) {
		fy += m_params[i] * pow(real_pt.x, i);
	}
	
    if (abs(fy - real_pt.y) > 2)
        return false;

	return true;
}

bool LUTManipulator::CheckInPoint(const CPoint& pt, int& index_pt) {
	cv::Point read_pt = cv::Point(pt.x / m_dWidth, pt.y / m_dHeight);

	for (int i = 0; i < m_pts.size(); ++i) {
		if (sqrt(pow(m_pts[i].x - read_pt.x, 2) + pow(m_pts[i].y - read_pt.y, 2)) < 5) {
			index_pt = i;
			return true;
		}
	}

	return false;
}

BEGIN_MESSAGE_MAP(LUTManipulator, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// LUTManipulator 메시지 처리기


void LUTManipulator::OnPaint()
{
    CPaintDC dc(this);
	CMemDC memDC(dc, this);
	
	CDC& pDC = memDC.GetDC();
	CRect rect;
	GetClientRect(&rect);
	
	pDC.FillSolidRect(&rect, RGB(200, 200, 200));

	CPen pen, * oldPen;
	pen.CreatePen(PS_DOT, 1, RGB(180, 180, 180));
	oldPen = pDC.SelectObject(&pen);

	m_dWidth = rect.Width() / 255.0;
	m_dHeight = rect.Height() / 255.0;

	// draw grid
    for (int i = 0; i < 255; ++i) {
		if (i % 10 == 0) {
			pDC.MoveTo(i * m_dWidth, 0);
			pDC.LineTo(i * m_dWidth, rect.Height());
			pDC.MoveTo(0, i * m_dHeight);
			pDC.LineTo(rect.Width(), i * m_dHeight);
		}
	}
	pDC.SelectObject(oldPen);
	pen.DeleteObject();

	// draw graph
	pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
	oldPen = pDC.SelectObject(&pen);

	DrawGraph(pDC, m_dWidth, m_dHeight);

	pDC.SelectObject(oldPen);
	pen.DeleteObject();

	for (int i = 0; i < m_pts.size(); ++i) {
		CRect ptRect = CRect(m_pts[i].x * m_dWidth - 5, m_pts[i].y * m_dHeight - 5, m_pts[i].x * m_dWidth + 5, m_pts[i].y * m_dHeight + 5);
		pDC.FillSolidRect(ptRect, RGB(255, 0, 0));
	}

	CString str;
	str.Format(_T("button down : %d"), m_bLBDown);
	pDC.TextOutW(20, 20, str);

	return;
}


void LUTManipulator::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (CheckMousePointInLine(point)) {
		m_bLBDown = true;

	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void LUTManipulator::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLBDown = false;
	m_iSelectIndex = -1;

	Invalidate(FALSE);
	CDialogEx::OnLButtonUp(nFlags, point);
}


void LUTManipulator::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bLBDown) {
		if (CheckInPoint(point, m_iSelectIndex)) {
			m_pts[m_iSelectIndex] = cv::Point(point.x / m_dWidth, point.y / m_dHeight);
		}
		else if(m_iSelectIndex != -1) {
			m_pts[m_iSelectIndex] = cv::Point(point.x / m_dWidth, point.y / m_dHeight);
		}
		else {
			m_pts.push_back(cv::Point(point.x / m_dWidth, point.y / m_dHeight));
		}
		Invalidate(FALSE);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}
