#pragma once
#include "concurrent_vector.h"


using namespace std;
using namespace cv;
using namespace concurrency;

enum eLINE_POSITION {
	LEFT_LINE = 0	,
	RIGHT_LINE		,
	TOP_LINE		,
	BOTTOM_LINE		,
};

class CImageProcess {
protected:
	ICameraView* m_pView = NULL;

	enum {
		TOP_LEFT = 0,
		TOP_RIGHT,
		BOTTOM_LEFT,
		BOTTOM_RIGHT,
	};
	
	
public:
	//CImageProcess(ICameraView* pView, CIPClient* pIPClient = NULL, LPCSTR pszUnitName = NULL);
	CImageProcess();
	~CImageProcess();


public:
	bool Labeling(const cv::Mat& src, cv::Mat& dst, cv::Rect& rc);
	BOOL Labeling(const cv::Mat& src, cv::Mat& dst, vector<vector<cv::Point>>& contours, const double& dMinArea, const BOOL& bFilled, const BOOL& bExternal);
	static BOOL RegionGrowing(const cv::Mat& src, cv::Mat& dst, const cv::Point ptSeed, const int& iFirstSeedValue = 170, const int& iThresholdLow = 50, const int& iBoundary = 1, const int& iMaxSeedCount = 5000, const BOOL& bInv = FALSE); // jsk
	BOOL RansacLineFitting(const std::vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec4d& model);
	BOOL CircleFitting(const vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec3d& model);
	BOOL EllipseFitting(const vector<cv::Point2d>& pointSet, const double& distance, double& score, cv::Vec6d& model);
	BOOL GetLine(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& pt1, cv::Point2d& pt2, const BOOL& bCheckAngle = FALSE, const BOOL& bUseCanny = TRUE);
	BOOL GetLine(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& pt1, cv::Point2d& pt2, double& dTheta, const BOOL& bCheckAngle = FALSE, const BOOL& bUseCanny = TRUE);
	BOOL GetLine_parallel(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& pt1, cv::Point2d& pt2, const BOOL& bCheckAngle = FALSE, const BOOL& bUseCanny = TRUE);
	BOOL GetCircle(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Vec3d& model, const BOOL& bUseCanny = 1);
	void stopWatch(double& out_time);
	BOOL FindCrossPointCircleLine(const cv::Point2d& pt1, const cv::Point2d& pt2, const cv::Point2d& ptCenter, const double& dRadius, vector<cv::Point2d>& ptCrosses); // return 0, 1, 2;
	Mat translateImg(Mat &img, int offsetx, int offsety);
	Point2d RotatePTP(const Point2d& ptSrc, const Point2d& ptCenter, const double& dTheta);
	std::vector<cv::Point2d> GetPoint2d(const cv::Mat& src);

	double GetDistancePointToLine(const cv::Point2d& line1, const cv::Point2d& line2, const cv::Point2d pt);
	double GetDistancePointToPoint(const cv::Point2d& pt1, const cv::Point2d& pt2);

	BOOL Delaunay(Mat& src, const double& dStdRadius, const BOOL& bFilled);
	//BOOL Delaunay(Mat& src, const double& dStdRadius = 100, const BOOL& bFilled = FALSE);
	//BOOL Delaunay(Mat& src, vector<vector<Point>>& pts, const double& dStdRadius = 100, const BOOL& bFilled = FALSE);

	// center
	BOOL GetCircleFromCenter(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, cv::Vec3d& model, const BOOL& bUseCanny = 1);
	BOOL GetEllipse(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, cv::Vec6d& model, const BOOL& bUseCanny = 1);
	BOOL GetLineFromCenter(const cv::Mat& img, const cv::Rect& roi, const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& pt1, cv::Point2d& pt2, const BOOL& bCheckAngle = FALSE, const BOOL& bUseCanny = TRUE);
	BOOL GetLineFromCenter(const cv::Mat& img, const cv::Point2d ptCenter , const double& dThreshold1, const double& dThreshole2, const double& dApertureSize, const int& nApertureSize2, const eLINE_POSITION& ePos, cv::Point2d& pt1, cv::Point2d& pt2, const BOOL& bCheckAngle = FALSE, const BOOL& bUseCanny = TRUE);

	// Periodic Noise Removing Filter
	void fftshift(const cv::Mat& inputImg, cv::Mat& outputImg);
	void filter2DFreq(const cv::Mat& inputImg, cv::Mat& outputImg, const cv::Mat& H);
	void synthesizeFilterH(cv::Mat& inputOutput_H, cv::Point center, int radius);
	void calcPSD(const cv::Mat& inputImg, cv::Mat& outputImg, int flag=0);

	// Polynomial interpolation (다항함수 보간법)
	void GetPolynomialParameters(const vector<cv::Point>& pts, vector<float>& paramters);

};

