#pragma once
#include <cmath>
#include <omp.h>
#include <concurrent_vector.h>

#define PI 3.14159265358979323846
#define Radian2Degree(x) x * 180 / PI
#define Degree2Radian(x) x *  PI / 180;


typedef struct RESULT_DATA {
	int			angle = 0;
	double		score = 0;
	cv::Point	pt = cv::Point(0, 0);
	cv::Rect	rect;
	cv::Mat		image;
} RESULT_DATA;

// Edge base detector
class CEdgeDetector
{
public:
	CEdgeDetector();
	~CEdgeDetector();

	RESULT_DATA				Detect(const cv::Mat& image, cv::Mat mark, int iAngleLimit = 30);
	RESULT_DATA				Detect(const cv::Mat& image, cv::Mat mark, cv::Rect roiRect, int iCanny1, int iCanny2, int iAngleLimit = 30);
	cv::Mat					CorrectedTilt(const cv::Mat& image, int iAngle);

	cv::Mat					GetMark() { return m_markImage; }
	std::vector<cv::Point>	GetMarkPts(int iAngle);


protected:
	cv::Mat					CannyImage(const cv::Mat& image, int iCanny1 = 100, int iCanny2 = 200);
	cv::Mat					RotateImage(const cv::Mat& image, int iAngle);
	cv::Mat					MakeMarks(const cv::Mat& mark);


protected:
	std::vector<cv::Point>	m_markPts[360];
	//cv::Mat					m_markImages[360];

	cv::Mat					m_markImage;

	int						m_iCanny1;
	int						m_iCanny2;
	int						m_iThreshold;
	double					m_dScale;

};
