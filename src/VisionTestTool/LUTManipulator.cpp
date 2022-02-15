// LUTManipulator.cpp: 구현 파일
//

#include "pch.h"
#include "VisionTestTool.h"
#include "LUTManipulator.h"
#include "afxdialogex.h"


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
	m_pts.push_back(cv::Point(0, 255));
	m_pts.push_back(cv::Point(255, 0));

	if(IsWindow(m_hWnd))
		Invalidate(FALSE);
}

cv::Mat LUTManipulator::GetLUT() {
	int nCount = m_params.size();
	cv::Mat lut(1, 256, CV_8UC3);

	for (int x = 0; x < 256; ++x) {
		float fy = 0;
		for (int i = 0; i < nCount; ++i) {
			fy += m_params[i] * pow(x, i);
		}
		fy = fabsf(fy - 255);
		if (fy < 0)			fy = 0;
		else if (fy > 255)	fy = 255;

		auto data = lut.data;
		data[x * 3 + 0] = fy;
		data[x * 3 + 1] = fy;
		data[x * 3 + 2] = fy;
	}

	return lut;
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

		if (fy < 0)
			fy = 0;
		else if (fy > 255)
			fy = 255;

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
	
    if (abs(fy - real_pt.y) < 10)
        return true;

	return false;
}

bool LUTManipulator::CheckInPoint(const CPoint& pt, int& index_pt) {
	cv::Point read_pt = cv::Point(pt.x / m_dWidth, pt.y / m_dHeight);

	for (int i = 0; i < m_pts.size(); ++i) {
		if (sqrt(pow(m_pts[i].x - read_pt.x, 2) + pow(m_pts[i].y - read_pt.y, 2)) < 10) {
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
	
	pDC.FillSolidRect(&rect, RGB(20, 20, 20));

	CPen pen, * oldPen;
	pen.CreatePen(PS_DOT, 1, RGB(40, 40, 40));
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
		int x = m_pts[i].x;
		int y = m_pts[i].y;
		
		CRect ptRect = CRect(x * m_dWidth - 3, y * m_dHeight - 3, x * m_dWidth + 3, y * m_dHeight + 3);
		pDC.FillSolidRect(ptRect, RGB(255, 0, 0));
	}

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
