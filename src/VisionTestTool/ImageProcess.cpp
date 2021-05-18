#include "pch.h"

#include "ImageProcess.h"
#include "ImageProcessor/ImageProcessor.h"


//CImageProcess::CImageProcess(ICameraView* pView, CIPClient* pIPClient, LPCSTR pszUnitName) : m_pView(pView) {
//}

CImageProcess::CImageProcess() {

}

CImageProcess::~CImageProcess() {
	
}

// return BIG ONE
bool CImageProcess::Labeling(const cv::Mat& src, cv::Mat& dst, cv::Rect& rc)
{
	Mat matGray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, matGray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		matGray = src.clone();
	}

	dst = Mat(matGray.size(), matGray.type(), cv::Scalar(0));

	Mat matTemp = matGray.clone();

	vector<vector<cv::Point>> contoursTemp;

	findContours(matTemp, contoursTemp, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));
	if (contoursTemp.size() < 1) return FALSE;

	int iMaxIndex = 0;
	double dMazAreaSize = 0.;
	for (int i = 0; i < contoursTemp.size(); i++)
	{
		const vector<Point>& contour = contoursTemp[i];
		double area = cv::contourArea(contour) + contoursTemp[i].size() * 2; // 외곽선 내부의 영역 + 외곽선 외부의 영역

		if (area > dMazAreaSize)
		{
			dMazAreaSize = area;
			iMaxIndex = i;
		}
	}
	cv::drawContours(dst, contoursTemp, iMaxIndex, cv::Scalar(255), cv::FILLED);
	rc = cv::boundingRect(contoursTemp[iMaxIndex]);

	return 1;
}

BOOL CImageProcess::Labeling(const cv::Mat& src, cv::Mat& dst, vector<vector<cv::Point>>& contours, const double& dMinArea, const BOOL& bFilled, const BOOL& bExternal)
{
	Mat matGray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, matGray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		matGray = src.clone();
	}

	dst = Mat(matGray.size(), matGray.type(), cv::Scalar(0));

	Mat matTemp = matGray.clone();

	vector<vector<cv::Point>> contoursTemp;

	if (bExternal)
		findContours(matTemp, contoursTemp, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE, Point(0, 0));
	else
		findContours(matTemp, contoursTemp, cv::RETR_TREE, cv::CHAIN_APPROX_NONE, Point(0, 0));

	if (contoursTemp.size() < 1) return FALSE;

	for (int i = 0; i < contoursTemp.size(); i++)
	{
		const vector<Point>& contour = contoursTemp[i];

		double area = cv::contourArea(contour) + (contoursTemp[i].size() * 2); // 외곽선 내부의 영역 + 외곽선 외부의 영역

		if (area > dMinArea)
		{
			if (bFilled)
				cv::drawContours(dst, contoursTemp, i, cv::Scalar(255), cv::FILLED);
			else
				cv::drawContours(dst, contoursTemp, i, cv::Scalar(255), 1);

			contours.push_back(contoursTemp[i]);
		}
	}

	return TRUE;
}

double CImageProcess::GetDistancePointToLine(const cv::Point2d& line1, const cv::Point2d& line2, const cv::Point2d pt) {
	double a = (line2.y - line1.y) / (line2.x - line1.x);
	double c = line1.y - (a * line1.x);
	double b = -1.0;

	double d = abs((a * pt.x) + (b * pt.y) + c) / sqrt((a * a) + (b * b));
	return d;
}

double CImageProcess::GetDistancePointToPoint(const cv::Point2d& pt1, const cv::Point2d& pt2) {
	double x = (pt1.x - pt2.x);
	double y = (pt1.y - pt2.y);
	return sqrt((x * x) + (y * y));
}

std::vector<cv::Point2d> CImageProcess::GetPoint2d(const cv::Mat& src)
{
	Mat matGray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, matGray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		matGray = src.clone();
	}

	std::vector<cv::Point2d> pointPositions;

	for (int y = 0; y < matGray.rows; y++)
	{
		for (int x = 0; x < matGray.cols; x++)
		{
			if (matGray.at<unsigned char>(y, x) == 255)
			{
				pointPositions.push_back(cv::Point2d(x, y));
			}
		}
	}

	return pointPositions;
}



void CImageProcess::stopWatch(double& out_time)
{
	static double time = 0.0;
	static bool bCheckTime = false;
	static int timerCount = 0;
	if (!bCheckTime) {
		time = (double)getTickCount();
		timerCount++;
	}

	if (bCheckTime) {
		time = ((double)getTickCount() - time) / getTickFrequency();
		out_time = time;
	}

	bCheckTime = !bCheckTime;
}

BOOL FindInSamples(vector<cv::Point2d>& samples, const int& sampleNumber, const cv::Point2d& point)
{
	for (int i = 0; i < sampleNumber; ++i) {
		if (samples[i].x == point.x && samples[i].y == point.y) {
			return true;
		}
	}
	return FALSE;
}


void GetSamples(vector<cv::Point2d>& samples, const int& sampleCount, const vector<cv::Point2d>& pointSet, const int& dataCount)
{
	for (int i = 0; i < sampleCount;) {
		int j = rand() % dataCount;

		if (!FindInSamples(samples, i, pointSet[j])) {
			samples[i] = pointSet[j];
			++i;
		}
	};
}

BOOL ComputeModelParameter(const vector<cv::Point2d>& samples, const int& sampleCount, cv::Vec4d& estimatedModel)
{
	double sx = 0, sy = 0;
	double sxx = 0, syy = 0;
	double sxy = 0, sw = 0;

	for (int i = 0; i < sampleCount; ++i)
	{
		double x = samples[i].x;
		double y = samples[i].y;

		sx += x;
		sy += y;
		sxx += x * x;
		sxy += x * y;
		syy += y * y;
		sw += 1;
	}

	double vxx = (sxx - sx * sx / sw) / sw;
	double vxy = (sxy - sx * sy / sw) / sw;
	double vyy = (syy - sy * sy / sw) / sw;

	double theta = atan2(2 * vxy, vxx - vyy) / 2;

	estimatedModel[0] = cos(theta);
	estimatedModel[1] = sin(theta);
	estimatedModel[2] = sx / sw;
	estimatedModel[3] = sy / sw;

	return TRUE;
}

static BOOL ComputeModelParameter(const vector<cv::Point2d>& samples, const int& sampleCount, cv::Vec3d& estimatedModel)
{
	// 중심 (a,b), 반지름 c인 원의 방정식: (x - a)^2 + (y - b)^2 = r^2
	Mat A(sampleCount, 3, DataType<double>::type);
	Mat B(sampleCount, 1, DataType<double>::type);

	for (int i = 0; i < sampleCount; i++)
	{
		double x = samples[i].x;
		double y = samples[i].y;

		A.at<double>(i, 0) = x;
		A.at<double>(i, 1) = y;
		A.at<double>(i, 2) = 1.;
		B.at<double>(i, 0) = -x * x - y * y;
	}

	Mat invA(3, sampleCount, DataType<double>::type);
	cv::invert(A, invA, cv::DECOMP_SVD);	//SVD Inverse
	Mat X = invA * B;

	double cx = -X.at<double>(0, 0) / 2.;
	double cy = -X.at<double>(1, 0) / 2.;

	estimatedModel[0] = cx;
	estimatedModel[1] = cy;
	estimatedModel[2] = sqrt(cx * cx + cy * cy - X.at<double>(2, 0));

	return TRUE;
}

static BOOL ComputeModelParameter(const vector<cv::Point2d>& samples, const int& sampleCount, cv::Vec6d& estimatedModel)
{
	// 타원 방정식
	// ax^2 + bxy + cy^2 + dx + ey + f = 0 (a = 1)
	cv::Mat A(sampleCount, 5, cv::DataType<double>::type);
	cv::Mat B(sampleCount, 1, cv::DataType<double>::type);

	for (int i = 0; i < sampleCount; i++)
	{
		double x = samples[i].x;
		double y = samples[i].y;

		A.at<double>(i, 0) = x * y;
		A.at<double>(i, 1) = y * y;
		A.at<double>(i, 2) = x;
		A.at<double>(i, 3) = y;
		A.at<double>(i, 4) = 1;

		B.at<double>(i, 0) = -x * x;
	}

	cv::Mat invA(5, sampleCount, cv::DataType<double>::type);
	cv::invert(A, invA, cv::DECOMP_SVD);	//SVD Inverse
	cv::Mat X = invA * B;

	double a = 1.0;
	double b = X.at<double>(0, 0);
	double c = X.at<double>(1, 0);
	double d = X.at<double>(2, 0);
	double e = X.at<double>(3, 0);
	double f = X.at<double>(4, 0);

	estimatedModel[0] = a;
	estimatedModel[1] = b;
	estimatedModel[2] = c;
	estimatedModel[3] = d;
	estimatedModel[4] = e;
	estimatedModel[5] = f;


	return TRUE;
}

double computeDistance(const cv::Vec4d& estimatedModel, cv::Point2d point)
{
	return fabs((point.x - estimatedModel[2]) * estimatedModel[1] - (point.y - estimatedModel[3]) * estimatedModel[0]) / sqrt(estimatedModel[0] * estimatedModel[0] + estimatedModel[1] * estimatedModel[1]);
}

static double computeDistance(const cv::Vec3d& estimatedModel, cv::Point2d point)
{
	double dx = estimatedModel[0] - point.x;
	double dy = estimatedModel[1] - point.y;

	return fabs(sqrt(dx * dx + dy * dy) - estimatedModel[2]);
}

static double computeDistance(const cv::Vec6d& estimatedModel, cv::Point2d point)
{
	// 한 점 p에서 타원에 내린 수선의 길이를 계산하기 힘들다.
	// 부정확하지만, 간단히 하기위하여 대수적 거리를 계산한다.
	double x = point.x;
	double y = point.y;

	double e = fabs(estimatedModel[0] * x * x + estimatedModel[1] * x * y + estimatedModel[2] * y * y + estimatedModel[3] * x + estimatedModel[4] * y + estimatedModel[5]);
	return sqrt(e);
}

double ModelVerification(vector<cv::Point2d>& inliers, int& inliersCount, const cv::Vec4d& estimatedModel, const vector<cv::Point2d>& pointSet, const int& dataCount, const double& distance)
{
	inliersCount = 0;
	double cost = 0.;

	for (int i = 0; i < dataCount; i++) {
		double distanceTemp = computeDistance(estimatedModel, pointSet[i]);
		if (distanceTemp < distance) {
			cost += 1.0;
			inliers[inliersCount] = pointSet[i];
			++inliersCount;
		}
	}

	return cost;
}

double ModelVerification(vector<cv::Point2d>& inliers, int& inliersCount, const cv::Vec3d& estimatedModel, const vector<cv::Point2d>& pointSet, const int& dataCount, const double& distance)
{
	inliersCount = 0;
	double cost = 0.;

	for (int i = 0; i < dataCount; i++) {
		double distanceTemp = computeDistance(estimatedModel, pointSet[i]);
		if (distanceTemp < distance) {
			cost += 1.0;
			inliers[inliersCount] = pointSet[i];
			++inliersCount;
		}
	}

	return cost;
}


double ModelVerification(vector<cv::Point2d>& inliers, int& inliersCount, const cv::Vec6d& estimatedModel, const vector<cv::Point2d>& pointSet, const int& dataCount, const double& distance)
{
	inliersCount = 0;
	double cost = 0.;

	for (int i = 0; i < dataCount; i++) {
		double distanceTemp = computeDistance(estimatedModel, pointSet[i]);
		if (distanceTemp < distance) {
			cost += 1.0;
			inliers[inliersCount] = pointSet[i];
			++inliersCount;
		}
	}

	return cost;
}


BOOL CImageProcess::RansacLineFitting(const vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec4d& model)
{
	const int dataCount = (int)pointSet.size();
	const int sampleCount = 2; //Line
	if (dataCount < sampleCount) return FALSE;
	vector<cv::Point2d> samples(sampleCount);

	int inliersCount = 0;
	vector<cv::Point2d> inliers(dataCount);

	cv::Vec4d estimatedModel;
	double maxScore = 0.0;

	int maxIteration = (int)(1 + log(1. - 0.99) / log(1. - pow(0.5, 5)));	// 반복 정밀도 항샹 2017.07.31

	for (int i = 0; i < maxIteration; i++)
	{
		GetSamples(samples, sampleCount, pointSet, dataCount);
		ComputeModelParameter(samples, sampleCount, estimatedModel);
		double scoreTemp = ModelVerification(inliers, inliersCount, estimatedModel, pointSet, dataCount, distance);
		if (maxScore < scoreTemp)
		{
			maxScore = scoreTemp;
			ComputeModelParameter(inliers, inliersCount, model);
		}
	}

	score = maxScore;
	return TRUE;
}

BOOL CImageProcess::CircleFitting(const vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec3d& model)
{
	const int dataCount = (int)pointSet.size();
	const int sampleCount = 3;		// 원이 되려면 최소한 3점이 필요
	if (dataCount < sampleCount) return FALSE;
	vector<cv::Point2d> samples(sampleCount);

	int inliersCount = 0;
	vector<cv::Point2d> inliers(dataCount);

	cv::Vec3d estimatedModel;	// circle cx,cy,r
	double maxScore(0.0);

	int maxIteration = (int)(1 + log(1. - 0.99) / log(1. - pow(0.5, sampleCount)));

	for (int i = 0; i < maxIteration; i++)
	{
		GetSamples(samples, sampleCount, pointSet, dataCount);
		ComputeModelParameter(samples, sampleCount, estimatedModel);
		double scoreTemp = ModelVerification(inliers, inliersCount, estimatedModel, pointSet, dataCount, distance);
		if (maxScore < scoreTemp)
		{
			maxScore = scoreTemp;
			ComputeModelParameter(inliers, inliersCount, model);
		}
	}

	score = maxScore;

	return TRUE;
}

bool ConvertStandForm(cv::Vec6d& model) {
	double a = model[0];
	double b = model[1];
	double c = model[2];
	double d = model[3];
	double e = model[4];
	double f = model[5];

	cv::Vec6d stdModel;

	double cx = ((2 * c * d) - (b * e)) / ((b * b) - (4 * a * c));
	double cy = ((2 * a * e) - (b * d)) / ((b * b) - (4 * a * c));
	double theta = (atan2(b, (a - c))) * 0.5;
	double cu = (a * cx * cx) + (b * cx * cy) + (c * cy * cy) - f;
	double ap = ((a * cos(theta) * cos(theta)) + (b * cos(theta) * sin(theta)) + (c * sin(theta) * sin(theta)));
	double cp = ((a * sin(theta) * sin(theta)) - (b * cos(theta) * sin(theta)) + (c * cos(theta) * cos(theta)));
	
	if (cu / ap <= 0 || cu / cp <= 0)
		return false;

	stdModel[0] = cx;
	stdModel[1] = cy;
	stdModel[2] = sqrt(cu / ap);
	stdModel[3] = sqrt(cu / cp);
	stdModel[4] = theta;
	stdModel[5] = 1;

	model = stdModel;
	return true;
}

BOOL CImageProcess::EllipseFitting(const vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec6d& model) {
	const int dataCount = (int)pointSet.size();
	const int sampleCount = 5;		
	if (dataCount < sampleCount) return FALSE;
	vector<cv::Point2d> samples(sampleCount);

	int inliersCount = 0;
	vector<cv::Point2d> inliers(dataCount);

	cv::Vec6d estimatedModel;	// ellipse cx, cy, w, h, angle, 1
	double maxScore(0.0);

	int maxIteration = (int)(1 + log(1. - 0.99) / log(1. - pow(0.5, sampleCount)));

	for (int i = 0; i < maxIteration; i++)
	{
		GetSamples(samples, sampleCount, pointSet, dataCount);
		ComputeModelParameter(samples, sampleCount, estimatedModel);
		double scoreTemp = ModelVerification(inliers, inliersCount, estimatedModel, pointSet, dataCount, distance);
		if (maxScore < scoreTemp)
		{
			ComputeModelParameter(inliers, inliersCount, estimatedModel);
			if (!ConvertStandForm(estimatedModel)) {
				--i;
				continue;
			}
			maxScore = scoreTemp;
			model = estimatedModel;
		}
	}

	score = maxScore;

	return TRUE;
}



BOOL CImageProcess::FindCrossPointCircleLine(const Point2d& pt1, const Point2d& pt2, const Point2d& ptCenter, const double& dRadius, vector<Point2d>& ptCrosses) {
	Point2d ptStd1 = pt1 - ptCenter;
	Point2d ptStd2 = pt2 - ptCenter;
	Point2d pt2t1 = ptStd2 - ptStd1;

	double a = pt2t1.x * pt2t1.x + pt2t1.y * pt2t1.y;
	double b = 2 * ((pt2t1.x * ptStd1.x) + (pt2t1.y * ptStd1.y));
	double c = (ptStd1.x * ptStd1.x) + (ptStd1.y * ptStd1.y) - (dRadius * dRadius);
	double d = b * b - (4 * a * c);
	if (d < 0)
		return FALSE;
	else if (d == 0) {
		double u = -b / (2 * a);
		Point2d ptResult1 = pt1 + (u * pt2t1);
		ptCrosses.push_back(ptResult1);
		return TRUE;
	}
	else if (d > 0) {
		double dSquareRootDelta = sqrt(d);
		double u1 = (-b + dSquareRootDelta) / (2 * a);
		double u2 = (-b - dSquareRootDelta) / (2 * a);
		Point2d ptResult1 = pt1 + (u1 * pt2t1);
		Point2d ptResult2 = pt1 + (u2 * pt2t1);
		ptCrosses.push_back(ptResult1);
		ptCrosses.push_back(ptResult2);
		return TRUE;
	}
	return FALSE;
}

double AngleOfThreePoints(const Point2d& pt1, const Point2d& pt2, const Point2d& ptCenter)
{
	double a = cv::norm((Point2d)pt1 - (Point2d)ptCenter);
	double b = cv::norm((Point2d)pt2 - (Point2d)ptCenter);
	double cosA = b / a;
	double acosA = acos(cosA);
	if (isnan(acosA))
		return 0;
	return acosA / CV_PI * 180;
}

BOOL CImageProcess::GetLine(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& ptC0, cv::Point2d& ptC1, double& dTheta, const BOOL& bCheckAngle, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img(roi).clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		uchar* data = (uchar*)imgEdge.data;
		vector<Point2d> pts;
		if (ePos == LEFT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = 0; x < imgEdge.cols; x++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = imgEdge.cols - 1; x > 0; x--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = 0; y < imgEdge.rows; y++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = imgEdge.rows - 1; y > 0; y--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

		// 취득한 점들을 캔버스에 그린다음 각도 측정 하여 오차가 발생 한다면 그 직선을 삭제하고 다시 실행 함.
		if (bCheckAngle) {
			imgEdge.setTo(0);
			for (int i = 0; i < pts.size(); i++)
				imgEdge.at<UCHAR>(pts[i].y, pts[i].x) = 255;

			for (int i = 0; i < 3; i++) {
				pts.clear();
				pts = GetPoint2d(imgEdge);
				if (pts.size() > nApertureSize2) {
					cv::Vec4d l;
					double dScore(0.);
					RansacLineFitting(pts, 1, dScore, l);	// 반복 정밀도 항샹 2017.07.31

					ptC0.x = l[2] + 3000 * l[0];
					ptC0.y = l[3] + 3000 * l[1];
					ptC1.x = l[2] - 3000 * l[0];
					ptC1.y = l[3] - 3000 * l[1];

					// 각도 체크
					if (ePos == LEFT_LINE || ePos == RIGHT_LINE) {
						if (fabs(ptC1.x - ptC0.x) < 500) {	// 300Pixel 이면 3.13도
															// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt0.x - pt1.x) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}
					else if (ePos == TOP_LINE || ePos == BOTTOM_LINE) {
						if (fabs(ptC1.y - ptC0.y) < 500) {
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);	//2018.07.31 반복정밀도

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt1.y - pt0.y) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}

					cv::line(imgEdge, ptC0, ptC1, cv::Scalar(0), 5);
				}
			}
		}
		else {
			if (pts.size() > nApertureSize2) {
				cv::Vec4d l;
				double dScore(0.);
				for (int i = 0; i < 10; i++) {
					cv::Vec4d ll;
					double dScoreTemp(0.);
					RansacLineFitting(pts, 2, dScoreTemp, ll);
					if (dScoreTemp > dScore) {
						dScore = dScoreTemp;
						l = ll;
					}
				}

				ptC0.x = l[2] + 3000 * l[0];
				ptC0.y = l[3] + 3000 * l[1];
				ptC1.x = l[2] - 3000 * l[0];
				ptC1.y = l[3] - 3000 * l[1];

				// 0번은 cos
				dTheta = 90. - (acos(l[0]) * 180 / CV_PI);
				if (l[1] > 0) dTheta *= -1.;
				ptC0.x += roi.x;
				ptC0.y += roi.y;

				ptC1.x += roi.x;
				ptC1.y += roi.y;

				return TRUE;
			}
		}
	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetLine_parallel(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& ptC0, cv::Point2d& ptC1, const BOOL& bCheckAngle, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img(roi).clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		uchar* data = (uchar*)imgEdge.data;
		
		concurrent_vector<cv::Point2d> pts;
		//vector<Point2d> pts;
		if (ePos == LEFT_LINE) {
#pragma omp parallel for
			for (int y = 0; y < imgEdge.rows; y++) {
#pragma omp parallel for
				for (int x = 0; x < imgEdge.cols; x++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
#pragma omp parallel for
			for (int y = 0; y < imgEdge.rows; y++) {
#pragma omp parallel for
				for (int x = imgEdge.cols - 1; x > 0; x--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
#pragma omp parallel for
			for (int x = 0; x < imgEdge.cols; x++) {
#pragma omp parallel for
				for (int y = 0; y < imgEdge.rows; y++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
#pragma omp parallel for
			for (int x = 0; x < imgEdge.cols; x++) {
#pragma omp parallel for
				for (int y = imgEdge.rows - 1; y > 0; y--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

        if (pts.size() > nApertureSize2) {
            vector<Point2d> ptss;
            ptss.assign(pts.begin(), pts.end());
            cv::Vec4d l;
            double dScore(0.);
#pragma omp parallel for
            for (int i = 0; i < 10; i++) {
                cv::Vec4d ll;
                double dScoreTemp(0.);
                RansacLineFitting(ptss, 2, dScoreTemp, ll);
                if (dScoreTemp > dScore) {
                    dScore = dScoreTemp;
                    l = ll;
                }
            }

            ptC0.x = l[2] + 30000 * l[0];
            ptC0.y = l[3] + 30000 * l[1];
            ptC1.x = l[2] - 30000 * l[0];
            ptC1.y = l[3] - 30000 * l[1];

            ptC0.x += roi.x;
            ptC0.y += roi.y;

            ptC1.x += roi.x;
            ptC1.y += roi.y;

            return TRUE;
        }
	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetLine(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& ptC0, cv::Point2d& ptC1, const BOOL& bCheckAngle, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img(roi).clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		uchar* data = (uchar*)imgEdge.data;
		vector<Point2d> pts;
		if (ePos == LEFT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = 0; x < imgEdge.cols; x++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = imgEdge.cols - 1; x > 0; x--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = 0; y < imgEdge.rows; y++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = imgEdge.rows - 1; y > 0; y--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

		// 취득한 점들을 캔버스에 그린다음 각도 측정 하여 오차가 발생 한다면 그 직선을 삭제하고 다시 실행 함.
		if (bCheckAngle) {
			imgEdge.setTo(0);
			for (int i = 0; i < pts.size(); i++)
				imgEdge.at<UCHAR>(pts[i].y, pts[i].x) = 255;

			for (int i = 0; i < 3; i++) {
				pts.clear();
				pts = GetPoint2d(imgEdge);
				if (pts.size() > nApertureSize2) {
					cv::Vec4d l;
					double dScore(0.);
					RansacLineFitting(pts, 1, dScore, l);	// 반복 정밀도 항샹 2017.07.31

					ptC0.x = l[2] + 3000 * l[0];
					ptC0.y = l[3] + 3000 * l[1];
					ptC1.x = l[2] - 3000 * l[0];
					ptC1.y = l[3] - 3000 * l[1];

					// 각도 체크
					if (ePos == LEFT_LINE || ePos == RIGHT_LINE) {
						if (fabs(ptC1.x - ptC0.x) < 500) {	// 300Pixel 이면 3.13도
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt0.x - pt1.x) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}
					else if (ePos == TOP_LINE || ePos == BOTTOM_LINE) {
						if (fabs(ptC1.y - ptC0.y) < 500) {
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);	//2018.07.31 반복정밀도

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt1.y - pt0.y) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}

					cv::line(imgEdge, ptC0, ptC1, cv::Scalar(0), 5);
				}
			}
		}
		else {
			if (pts.size() > nApertureSize2) {
				cv::Vec4d l;
				double dScore(0.);
				for (int i = 0; i < 10; i++) {
					cv::Vec4d ll;
					double dScoreTemp(0.);
					RansacLineFitting(pts, 2, dScoreTemp, ll);
					if (dScoreTemp > dScore) {
						dScore = dScoreTemp;
						l = ll;
					}
				}

				ptC0.x = l[2] + 30000 * l[0];
				ptC0.y = l[3] + 30000 * l[1];
				ptC1.x = l[2] - 30000 * l[0];
				ptC1.y = l[3] - 30000 * l[1];

				ptC0.x += roi.x;
				ptC0.y += roi.y;

				ptC1.x += roi.x;
				ptC1.y += roi.y;

				return TRUE;
			}
		}
	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetLineFromCenter(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& ptC0, cv::Point2d& ptC1, const BOOL& bCheckAngle, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img(roi).clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		uchar* data = (uchar*)imgEdge.data;
		vector<Point2d> pts;
		cv::Point2d ptCetner = cv::Point2d(imgEdge.cols / 2, imgEdge.rows / 2);

		if (ePos == LEFT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = ptCetner.x; x > 0; x--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = ptCetner.x; x < imgEdge.cols; x++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = ptCetner.y; y > 0; y--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = ptCetner.y; y < imgEdge.rows; y++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

		// 취득한 점들을 캔버스에 그린다음 각도 측정 하여 오차가 발생 한다면 그 직선을 삭제하고 다시 실행 함.
		if (bCheckAngle) {
			imgEdge.setTo(0);
			for (int i = 0; i < pts.size(); i++)
				imgEdge.at<UCHAR>(pts[i].y, pts[i].x) = 255;

			for (int i = 0; i < 3; i++) {
				pts.clear();
				pts = GetPoint2d(imgEdge);
				if (pts.size() > nApertureSize2) {
					cv::Vec4d l;
					double dScore(0.);
					RansacLineFitting(pts, 1, dScore, l);	// 반복 정밀도 항샹 2017.07.31

					ptC0.x = l[2] + 3000 * l[0];
					ptC0.y = l[3] + 3000 * l[1];
					ptC1.x = l[2] - 3000 * l[0];
					ptC1.y = l[3] - 3000 * l[1];

					// 각도 체크
					if (ePos == LEFT_LINE || ePos == RIGHT_LINE) {
						if (fabs(ptC1.x - ptC0.x) < 500) {	// 300Pixel 이면 3.13도
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt0.x - pt1.x) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}
					else if (ePos == TOP_LINE || ePos == BOTTOM_LINE) {
						if (fabs(ptC1.y - ptC0.y) < 500) {
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);	//2018.07.31 반복정밀도

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt1.y - pt0.y) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x += roi.x;
							ptC0.y += roi.y;
							ptC1.x += roi.x;
							ptC1.y += roi.y;
							return TRUE;
						}
					}

					cv::line(imgEdge, ptC0, ptC1, cv::Scalar(0), 5);
				}
			}
		}
		else {
			if (pts.size() > nApertureSize2) {
				cv::Vec4d l;
				double dScore(0.);
				for (int i = 0; i < 10; i++) {
					cv::Vec4d ll;
					double dScoreTemp(0.);
					RansacLineFitting(pts, 2, dScoreTemp, ll);
					if (dScoreTemp > dScore) {
						dScore = dScoreTemp;
						l = ll;
					}
				}

				ptC0.x = l[2] + 3000 * l[0];
				ptC0.y = l[3] + 3000 * l[1];
				ptC1.x = l[2] - 3000 * l[0];
				ptC1.y = l[3] - 3000 * l[1];

				ptC0.x += roi.x;
				ptC0.y += roi.y;

				ptC1.x += roi.x;
				ptC1.y += roi.y;

				return TRUE;
			}
		}
	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetLineFromCenter(const cv::Mat& img, const cv::Point2d ptCenter, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& ptC0, cv::Point2d& ptC1, const BOOL& bCheckAngle, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img.clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		uchar* data = (uchar*)imgEdge.data;
		vector<Point2d> pts;

		if (ePos == LEFT_LINE) {
			for (int y = ptCenter.y; y < imgEdge.rows; y++) {
				for (int x = ptCenter.x; x > 0; x--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
			for (int y = ptCenter.y; y < imgEdge.rows; y++) {
				for (int x = ptCenter.x; x < imgEdge.cols; x++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
			for (int x = ptCenter.x; x < imgEdge.cols; x++) {
				for (int y = ptCenter.y; y > 0; y--) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
			for (int x = ptCenter.x; x < imgEdge.cols; x++) {
				for (int y = ptCenter.y; y < imgEdge.rows; y++) {
					if (data[y * imgEdge.cols + x] == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

		// 취득한 점들을 캔버스에 그린다음 각도 측정 하여 오차가 발생 한다면 그 직선을 삭제하고 다시 실행 함.
		if (bCheckAngle) {
			imgEdge.setTo(0);
			for (int i = 0; i < pts.size(); i++)
				imgEdge.at<UCHAR>(pts[i].y, pts[i].x) = 255;

			for (int i = 0; i < 3; i++) {
				pts.clear();
				pts = GetPoint2d(imgEdge);
				if (pts.size() > nApertureSize2) {
					cv::Vec4d l;
					double dScore(0.);
					RansacLineFitting(pts, 1, dScore, l);	// 반복 정밀도 항샹 2017.07.31

					ptC0.x = l[2] + 3000 * l[0];
					ptC0.y = l[3] + 3000 * l[1];
					ptC1.x = l[2] - 3000 * l[0];
					ptC1.y = l[3] - 3000 * l[1];

					// 각도 체크
					if (ePos == LEFT_LINE || ePos == RIGHT_LINE) {
						if (fabs(ptC1.x - ptC0.x) < 500) {	// 300Pixel 이면 3.13도
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt0.x - pt1.x) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x ;
							ptC0.y ;
							ptC1.x ;
							ptC1.y ;
							return TRUE;
						}
					}
					else if (ePos == TOP_LINE || ePos == BOTTOM_LINE) {
						if (fabs(ptC1.y - ptC0.y) < 500) {
							// 반복 정밀도 향상 2018.07.31
							double dMaxSrore = 0.;
							for (int j = 0; j < 10; j++) {
								double dScore(0.);
								RansacLineFitting(pts, 2, dScore, l);	//2018.07.31 반복정밀도

								Point2d pt0, pt1;
								pt0.x = l[2] + 3000 * l[0];
								pt0.y = l[3] + 3000 * l[1];
								pt1.x = l[2] - 3000 * l[0];
								pt1.y = l[3] - 3000 * l[1];
								if (fabs(pt1.y - pt0.y) > 500) {
									cv::line(imgEdge, pt0, pt1, cv::Scalar(0), 5);
									pts.clear();
									pts = GetPoint2d(imgEdge);
									continue;
								}

								if (dScore > dMaxSrore) {
									dMaxSrore = dScore;
									ptC0.x = l[2] + 3000 * l[0];
									ptC0.y = l[3] + 3000 * l[1];
									ptC1.x = l[2] - 3000 * l[0];
									ptC1.y = l[3] - 3000 * l[1];
								}
							}
							ptC0.x ;
							ptC0.y ;
							ptC1.x ;
							ptC1.y ;
							return TRUE;
						}
					}

					cv::line(imgEdge, ptC0, ptC1, cv::Scalar(0), 5);
				}
			}
		}
		else {
			if (pts.size() > nApertureSize2) {
				cv::Vec4d l;
				double dScore(0.);
				for (int i = 0; i < 10; i++) {
					cv::Vec4d ll;
					double dScoreTemp(0.);
					RansacLineFitting(pts, 2, dScoreTemp, ll);
					if (dScoreTemp > dScore) {
						dScore = dScoreTemp;
						l = ll;
					}
				}

				ptC0.x = l[2] + 3000 * l[0];
				ptC0.y = l[3] + 3000 * l[1];
				ptC1.x = l[2] - 3000 * l[0];
				ptC1.y = l[3] - 3000 * l[1];

				ptC0.x;
				ptC0.y;
				ptC1.x;
				ptC1.y;

				return TRUE;
			}
		}
	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

typedef struct s_SeedPoint
{
	cv::Point seedPoint;
	int color;
}S_SEEDPOINT;

static BOOL DrawResult(Mat& dst, vector<S_SEEDPOINT>& seedPoint)
{
	for (int i = 0; i < seedPoint.size(); i++)
	{
		dst.at<UCHAR>(seedPoint[i].seedPoint) = 0;
	}

	return TRUE;
}

static BOOL SearchingResions(Mat& src, vector<S_SEEDPOINT>& seedPoint, const int& boundary, const int& iThresholdLow)
{
	vector<S_SEEDPOINT> newSeedPoint;
	for (int i = 0; i < seedPoint.size(); i++)
	{
		if (seedPoint[i].seedPoint.x == 0 || seedPoint[i].seedPoint.y == 0 || seedPoint[i].seedPoint.x == src.cols || seedPoint[i].seedPoint.y == src.rows)
		{
			src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x) = 0;
		}
		else if (seedPoint[i].seedPoint.x == 1 || seedPoint[i].seedPoint.y == 1 || seedPoint[i].seedPoint.x == src.cols - 1 || seedPoint[i].seedPoint.y == src.rows - 1)
		{
			src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x) = 0;
		}
		else
		{
			if (abs(src.at<unsigned char>(seedPoint[i].seedPoint.y + 1, seedPoint[i].seedPoint.x) - seedPoint[i].color) < boundary && src.at<unsigned char>(seedPoint[i].seedPoint.y + 1, seedPoint[i].seedPoint.x) > iThresholdLow)
			{
				S_SEEDPOINT s;
				s.seedPoint = Point(seedPoint[i].seedPoint.x, seedPoint[i].seedPoint.y + 1);
				s.color = src.at<unsigned char>(seedPoint[i].seedPoint.y + 1, seedPoint[i].seedPoint.x);
				newSeedPoint.push_back(s);
				src.at<unsigned char>(seedPoint[i].seedPoint.y + 1, seedPoint[i].seedPoint.x) = 0;
			}
			if (abs(src.at<unsigned char>(seedPoint[i].seedPoint.y - 1, seedPoint[i].seedPoint.x) - seedPoint[i].color) < boundary && src.at<unsigned char>(seedPoint[i].seedPoint.y - 1, seedPoint[i].seedPoint.x) > iThresholdLow)
			{
				S_SEEDPOINT s;
				s.seedPoint = Point(seedPoint[i].seedPoint.x, seedPoint[i].seedPoint.y - 1);
				s.color = src.at<unsigned char>(seedPoint[i].seedPoint.y - 1, seedPoint[i].seedPoint.x);
				newSeedPoint.push_back(s);
				src.at<unsigned char>(seedPoint[i].seedPoint.y - 1, seedPoint[i].seedPoint.x) = 0;
			}
			if (abs(src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x + 1) - seedPoint[i].color) < boundary && src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x + 1) > iThresholdLow)
			{
				S_SEEDPOINT s;
				s.seedPoint = Point(seedPoint[i].seedPoint.x + 1, seedPoint[i].seedPoint.y);
				s.color = src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x + 1);
				newSeedPoint.push_back(s);
				src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x + 1) = 0;
			}
			if (abs(src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x - 1) - seedPoint[i].color) < boundary && src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x - 1) > iThresholdLow)
			{
				S_SEEDPOINT s;
				s.seedPoint = Point(seedPoint[i].seedPoint.x - 1, seedPoint[i].seedPoint.y);
				s.color = src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x - 1);
				newSeedPoint.push_back(s);
				src.at<unsigned char>(seedPoint[i].seedPoint.y, seedPoint[i].seedPoint.x - 1) = 0;
			}
		}
	}
	seedPoint.clear();
	seedPoint = newSeedPoint;

	return TRUE;
}

BOOL CImageProcess::RegionGrowing(const Mat& src, Mat& dst, const cv::Point ptSeed, const int& iFirstSeedValue, const int& iThresholdLow, const int& iBoundary, const int& iMaxSeedCount, const BOOL& bInv)
{
	Mat matGray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, matGray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		matGray = src.clone();
	}

	dst = Mat(matGray.size(), matGray.type(), cv::Scalar(0));
	if (bInv) cv::bitwise_not(matGray, matGray);

	vector<S_SEEDPOINT> seedPoint;
	S_SEEDPOINT sp;
	sp.seedPoint = ptSeed;
	sp.color = iFirstSeedValue;
	seedPoint.push_back(sp);

	int countDepth = 0;
	for (int idx = 0; idx < iMaxSeedCount; idx++)
	{
		if (seedPoint.size() == 0) break;
		countDepth++;
		// searching regions
		SearchingResions(matGray, seedPoint, iBoundary, iThresholdLow);
		for (int i = 0; i < seedPoint.size(); i++)
		{
			dst.at<UCHAR>(seedPoint[i].seedPoint) = 255;
		}
	}

	return TRUE;
}

BOOL CImageProcess::GetCircle(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Vec3d& model, const BOOL& bUseCanny) {
	try {
		Mat imgEdge = img(roi).clone();
		if (imgEdge.channels() == 3) cvtColor(imgEdge, imgEdge, cv::COLOR_BGR2GRAY);
		if (bUseCanny) {
			GaussianBlur(imgEdge, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		vector<Point2d> pts;
		if (ePos == LEFT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = 0; x < imgEdge.cols; x++) {
					if (imgEdge.at<UCHAR>(y, x) == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == RIGHT_LINE) {
			for (int y = 0; y < imgEdge.rows; y++) {
				for (int x = imgEdge.cols - 1; x > 0; x--) {
					if (imgEdge.at<UCHAR>(y, x) == 255) {
						pts.push_back(Point2d(x, y));
						break;
					}
				}
			}
		}
		else if (ePos == TOP_LINE) {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = 0; y < imgEdge.rows; y++) {
					if (imgEdge.at<UCHAR>(y, x) == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}
		else {
			for (int x = 0; x < imgEdge.cols; x++) {
				for (int y = imgEdge.rows - 1; y > 0; y--) {
					if (imgEdge.at<UCHAR>(y, x) == 255) {
						pts.push_back(Point(x, y));
						break;
					}
				}
			}
		}

		if (pts.size() > nApertureSize2) {
			cv::Vec3d l;
			double dScore(0.);
			for (int i = 0; i < 5; i++) {
				cv::Vec3d ll;
				double dScoreTemp(0.);
				CircleFitting(pts, 1, dScoreTemp, ll);
				if (dScoreTemp > dScore) {
					dScore = dScoreTemp;
					l = ll;
				}
			}

			model[0] = l[0] + roi.x;
			model[1] = l[1] + roi.y;
			model[2] = l[2];

			return TRUE;
		}

	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetCircleFromCenter(const cv::Mat& img, const cv::Rect& roi,
	const double& dThreshold1, const double& dThreshole2,
	const double& dApertureSize, const int& nApertureSize2,
	cv::Vec3d& model, const BOOL& bUseCanny)
{
	try {
		cv::Mat imgEdge = img(roi).clone();
		cv::Mat imgGray;
		if (imgEdge.channels() == 3)
			cvtColor(imgEdge, imgGray, cv::COLOR_BGR2GRAY);

		if (bUseCanny) {
			GaussianBlur(imgGray, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		vector<Point2d> pts;

		//이미지 분할
		Mat quadImg[4];
		Rect qRect[4];
		qRect[TOP_LEFT] = Rect(0, 0, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[TOP_RIGHT] = Rect(imgEdge.cols / 2, 0, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[BOTTOM_LEFT] = Rect(0, imgEdge.rows / 2, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[BOTTOM_RIGHT] = Rect(imgEdge.cols / 2, imgEdge.rows / 2, imgEdge.cols / 2, imgEdge.rows / 2);

		for (int i = 0; i < 4; i++) {
			quadImg[i] = imgEdge(qRect[i]).clone();
		}

		//point 찾기
		//top left
		for (int x = quadImg[TOP_LEFT].cols - 1; x > 0; x--) {
			for (int y = quadImg[TOP_LEFT].rows - 1; y > 0; y--) {
				if (quadImg[TOP_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_LEFT].x, y + qRect[TOP_LEFT].y));
					break;
				}
			}
		}
		for (int y = quadImg[TOP_LEFT].rows - 1; y > 0; y--) {
			for (int x = quadImg[TOP_LEFT].cols - 1; x > 0; x--) {
				if (quadImg[TOP_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_LEFT].x, y + qRect[TOP_LEFT].y));
					break;
				}
			}
		}
		//top right
		for (int x = 0; x < quadImg[TOP_RIGHT].cols; x++) {
			for (int y = quadImg[TOP_RIGHT].rows - 1; y > 0; y--) {
				if (quadImg[TOP_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_RIGHT].x, y + qRect[TOP_RIGHT].y));
					break;
				}
			}
		}
		for (int y = quadImg[TOP_RIGHT].rows - 1; y > 0; y--) {
			for (int x = 0; x < quadImg[TOP_RIGHT].cols; x++) {
				if (quadImg[TOP_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_RIGHT].x, y + qRect[TOP_RIGHT].y));
					break;
				}
			}
		}
		//bottom left
		for (int x = quadImg[BOTTOM_LEFT].cols - 1; x > 0; x--) {
			for (int y = 0; y < quadImg[BOTTOM_LEFT].rows; y++) {
				if (quadImg[BOTTOM_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_LEFT].x, y + qRect[BOTTOM_LEFT].y));
					break;
				}
			}
		}
		for (int y = 0; y < quadImg[BOTTOM_LEFT].rows; y++) {
			for (int x = quadImg[BOTTOM_LEFT].cols - 1; x > 0; x--) {
				if (quadImg[BOTTOM_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_LEFT].x, y + qRect[BOTTOM_LEFT].y));
					break;
				}
			}
		}
		//bottom right
		for (int x = 0; x < quadImg[BOTTOM_RIGHT].cols; x++) {
			for (int y = 0; y < quadImg[BOTTOM_RIGHT].rows; y++) {
				if (quadImg[BOTTOM_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_RIGHT].x, y + qRect[BOTTOM_RIGHT].y));
					break;
				}
			}
		}
		for (int y = 0; y < quadImg[BOTTOM_RIGHT].rows; y++) {
			for (int x = 0; x < quadImg[BOTTOM_RIGHT].cols; x++) {
				if (quadImg[BOTTOM_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_RIGHT].x, y + qRect[BOTTOM_RIGHT].y));
					break;
				}
			}
		}


		if (pts.size() > nApertureSize2) {
			cv::Vec3d l;
			double dScore(0.);
			for (int i = 0; i < 5; i++) {
				cv::Vec3d ll;
				double dScoreTemp(0.);
				CircleFitting(pts, 1, dScoreTemp, ll);
				if (dScoreTemp > dScore) {
					dScore = dScoreTemp;
					l = ll;
				}
			}


			model[0] = l[0] + roi.x;
			model[1] = l[1] + roi.y;
			model[2] = l[2];

			return TRUE;
		}

	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}

BOOL CImageProcess::GetEllipse(const cv::Mat& img, const cv::Rect& roi,
	const double& dThreshold1, const double& dThreshole2,
	const double& dApertureSize, const int& nApertureSize2,
	cv::Vec6d& model, const BOOL& bUseCanny)
{
	try {
		cv::Mat imgEdge = img(roi).clone();
		cv::Mat imgGray;
		if (imgEdge.channels() == 3)
			cvtColor(imgEdge, imgGray, cv::COLOR_BGR2GRAY);

		if (bUseCanny) {
			GaussianBlur(imgGray, imgEdge, Size(3, 3), 0.0, 0.0, BORDER_DEFAULT);
			Canny(imgEdge, imgEdge, dThreshold1, dThreshole2, dApertureSize, false);
		}
		vector<Point2d> pts;

		//이미지 분할
		Mat quadImg[4];
		Rect qRect[4];
		qRect[TOP_LEFT] = Rect(0, 0, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[TOP_RIGHT] = Rect(imgEdge.cols / 2, 0, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[BOTTOM_LEFT] = Rect(0, imgEdge.rows / 2, imgEdge.cols / 2, imgEdge.rows / 2);
		qRect[BOTTOM_RIGHT] = Rect(imgEdge.cols / 2, imgEdge.rows / 2, imgEdge.cols / 2, imgEdge.rows / 2);

		for (int i = 0; i < 4; i++) {
			quadImg[i] = imgEdge(qRect[i]).clone();
		}

		//point 찾기
		//top left
		for (int x = quadImg[TOP_LEFT].cols - 1; x > 0; x--) {
			for (int y = quadImg[TOP_LEFT].rows - 1; y > 0; y--) {
				if (quadImg[TOP_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_LEFT].x, y + qRect[TOP_LEFT].y));
					break;
				}
			}
		}
		for (int y = quadImg[TOP_LEFT].rows - 1; y > 0; y--) {
			for (int x = quadImg[TOP_LEFT].cols - 1; x > 0; x--) {
				if (quadImg[TOP_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_LEFT].x, y + qRect[TOP_LEFT].y));
					break;
				}
			}
		}
		//top right
		for (int x = 0; x < quadImg[TOP_RIGHT].cols; x++) {
			for (int y = quadImg[TOP_RIGHT].rows - 1; y > 0; y--) {
				if (quadImg[TOP_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_RIGHT].x, y + qRect[TOP_RIGHT].y));
					break;
				}
			}
		}
		for (int y = quadImg[TOP_RIGHT].rows - 1; y > 0; y--) {
			for (int x = 0; x < quadImg[TOP_RIGHT].cols; x++) {
				if (quadImg[TOP_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[TOP_RIGHT].x, y + qRect[TOP_RIGHT].y));
					break;
				}
			}
		}
		//bottom left
		for (int x = quadImg[BOTTOM_LEFT].cols - 1; x > 0; x--) {
			for (int y = 0; y < quadImg[BOTTOM_LEFT].rows; y++) {
				if (quadImg[BOTTOM_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_LEFT].x, y + qRect[BOTTOM_LEFT].y));
					break;
				}
			}
		}
		for (int y = 0; y < quadImg[BOTTOM_LEFT].rows; y++) {
			for (int x = quadImg[BOTTOM_LEFT].cols - 1; x > 0; x--) {
				if (quadImg[BOTTOM_LEFT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_LEFT].x, y + qRect[BOTTOM_LEFT].y));
					break;
				}
			}
		}
		//bottom right
		for (int x = 0; x < quadImg[BOTTOM_RIGHT].cols; x++) {
			for (int y = 0; y < quadImg[BOTTOM_RIGHT].rows; y++) {
				if (quadImg[BOTTOM_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_RIGHT].x, y + qRect[BOTTOM_RIGHT].y));
					break;
				}
			}
		}
		for (int y = 0; y < quadImg[BOTTOM_RIGHT].rows; y++) {
			for (int x = 0; x < quadImg[BOTTOM_RIGHT].cols; x++) {
				if (quadImg[BOTTOM_RIGHT].at<UCHAR>(y, x) == 255) {
					pts.push_back(Point(x + qRect[BOTTOM_RIGHT].x, y + qRect[BOTTOM_RIGHT].y));
					break;
				}
			}
		}


		if (pts.size() > nApertureSize2) {
			cv::Vec6d l;
			double dScore(0.);
			for (int i = 0; i < 5; i++) {
				cv::Vec6d ll;
				double dScoreTemp(0.);
				EllipseFitting(pts, 1, dScoreTemp, ll);
				if (dScoreTemp > dScore) {
					dScore = dScoreTemp;
					l = ll;
				}
			}

			model[0] = l[0] + roi.x;
			model[1] = l[1] + roi.y;
			model[2] = l[2];
			model[3] = l[3];
			model[4] = l[4];
			model[5] = l[5];

			return TRUE;
		}

	}
	catch (...) {
		return FALSE;
	}

	return FALSE;
}


std::vector<cv::Point2f> GetPoint2f(const cv::Mat& src)
{
	Mat matGray;

	if (src.channels() == 3)
	{
		cv::cvtColor(src, matGray, cv::COLOR_BGR2GRAY);
	}
	else
	{
		matGray = src.clone();
	}

	std::vector<cv::Point2f> pointPositions;
	uchar* data = (uchar*)matGray.data;
	for (int y = 0; y < matGray.rows; y++)
	{
		for (int x = 0; x < matGray.cols; x++)
		{
			if (data[y * matGray.cols + x] == 255)
			{
				pointPositions.push_back(cv::Point2f(x, y));
			}
		}
	}

	return pointPositions;
}

BOOL CImageProcess::Delaunay(Mat& src, const double& dStdRadius, const BOOL& bFilled)
{
	vector<cv::Point2f> edgePositions(GetPoint2f(src));
	src.setTo(0);
	Rect rect(0, 0, src.cols, src.rows);

	Subdiv2D subdiv(rect);
	subdiv.insert(edgePositions);

	bool draw;
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);

	for (size_t i = 0; i < triangleList.size(); ++i)
	{
		Vec6f t = triangleList[i];

		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		// MY PIECE OF CODE
		draw = true;

		for (int i = 0; i < 3; i++) {
			if (pt[i].x > src.cols || pt[i].y > src.rows || pt[i].x < 0 || pt[i].y < 0)
			{
				draw = false;
				break;
			}
		}

		double dist = 0.0;
		double a = pt[0].x - pt[1].x;
		double b = pt[0].y - pt[1].y;
		dist = sqrt(a * a + b * b);
		if (dist > dStdRadius) draw = false;

		a = pt[1].x - pt[2].x;
		b = pt[1].y - pt[2].y;
		dist = sqrt(a * a + b * b);
		if (dist > dStdRadius) draw = false;

		a = pt[2].x - pt[0].x;
		b = pt[2].y - pt[0].y;
		dist = sqrt(a * a + b * b);
		if (dist > dStdRadius) draw = false;

		if (draw) {
			line(src, pt[0], pt[1], cv::Scalar(255), 1, cv::LineTypes::LINE_AA);
			line(src, pt[1], pt[2], cv::Scalar(255), 1, cv::LineTypes::LINE_AA);
			line(src, pt[2], pt[0], cv::Scalar(255), 1, cv::LineTypes::LINE_AA);
		}
	}

	return TRUE;
}

Mat CImageProcess::translateImg(Mat& img, int offsetx, int offsety) {
	Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img, img, trans_mat, img.size());
	return img;
}

Point2d CImageProcess::RotatePTP(const Point2d& ptSrc, const Point2d& ptCenter, const double& dTheta) {
	Point2d ptA = ptSrc - ptCenter;
	Point2d ptDst;

	ptDst.x = ptA.x * cos(dTheta * CV_PI / 180) - ptA.y * sin(dTheta * CV_PI / 180);
	ptDst.y = ptA.x * sin(dTheta * CV_PI / 180) + ptA.y * cos(dTheta * CV_PI / 180);

	ptDst += ptCenter;

	return ptDst;
}


void CImageProcess::fftshift(const Mat& inputImg, Mat& outputImg)
{
	outputImg = inputImg.clone();
	int cx = outputImg.cols / 2;
	int cy = outputImg.rows / 2;
	Mat q0(outputImg, Rect(0, 0, cx, cy));
	Mat q1(outputImg, Rect(cx, 0, cx, cy));
	Mat q2(outputImg, Rect(0, cy, cx, cy));
	Mat q3(outputImg, Rect(cx, cy, cx, cy));
	Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}
void CImageProcess::filter2DFreq(const Mat& inputImg, Mat& outputImg, const Mat& H)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI, DFT_SCALE);
	Mat planesH[2] = { Mat_<float>(H.clone()), Mat::zeros(H.size(), CV_32F) };
	Mat complexH;
	merge(planesH, 2, complexH);
	Mat complexIH;
	mulSpectrums(complexI, complexH, complexIH, 0);
	idft(complexIH, complexIH);
	split(complexIH, planes);
	outputImg = planes[0];
}
void CImageProcess::synthesizeFilterH(Mat& inputOutput_H, Point center, int radius)
{
	Point c2 = center, c3 = center, c4 = center;
	c2.y = inputOutput_H.rows - center.y;
	c3.x = inputOutput_H.cols - center.x;
	c4 = Point(c3.x, c2.y);
	circle(inputOutput_H, center, radius, 0, -1, 8);
	circle(inputOutput_H, c2, radius, 0, -1, 8);
	circle(inputOutput_H, c3, radius, 0, -1, 8);
	circle(inputOutput_H, c4, radius, 0, -1, 8);
}
// Function calculates PSD(Power spectrum density) by fft with two flags
// flag = 0 means to return PSD
// flag = 1 means to return log(PSD)
void CImageProcess::calcPSD(const Mat& inputImg, Mat& outputImg, int flag)
{
	Mat planes[2] = { Mat_<float>(inputImg.clone()), Mat::zeros(inputImg.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);
	dft(complexI, complexI);
	split(complexI, planes);            // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))
	planes[0].at<float>(0) = 0;
	planes[1].at<float>(0) = 0;
	// compute the PSD = sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)^2
	Mat imgPSD;
	magnitude(planes[0], planes[1], imgPSD);        //imgPSD = sqrt(Power spectrum density)
	pow(imgPSD, 2, imgPSD);                         //it needs ^2 in order to get PSD
	outputImg = imgPSD;
	// logPSD = log(1 + PSD)
	if (flag)
	{
		Mat imglogPSD;
		imglogPSD = imgPSD + Scalar::all(1);
		log(imglogPSD, imglogPSD);
		outputImg = imglogPSD;
	}
}

// 점 pts들을 지나는 다항함수 구하기
// f(x) = a0 + a1*x + a2*x + ... + an*x^n-1
void CImageProcess::GetPolynomialParameters(const vector<cv::Point>& pts, vector<float>& parameters) {
	int nCount = pts.size();
    if (nCount < 2)
        return;

	// fx = ax * params
	cv::Mat fx		= cv::Mat::zeros(cv::Size(1, nCount), CV_32FC1);
	cv::Mat params	= cv::Mat::zeros(cv::Size(1, nCount), CV_32FC1);
	cv::Mat ax		= cv::Mat::zeros(cv::Size(nCount, nCount), CV_32FC1);

	for (int i = 0; i < nCount; ++i) {
		fx.at<float>(i, 0) = pts[i].y;

		for (int j = 0; j < nCount; ++j) {
			ax.at<float>(j, i) = std::pow(pts[j].x, i);
		}
	}

	// params = ax^-1 * fx
	cv::Mat inv_ax = cv::Mat::zeros(cv::Size(nCount, nCount), CV_32FC1);
	cv::invert(ax, inv_ax, cv::DECOMP_SVD);

	params = inv_ax * fx;

	parameters.clear();
	for (int i = 0; i < nCount; ++i) {
		parameters.push_back(params.at<float>(i, 0));
	}

	return;
}