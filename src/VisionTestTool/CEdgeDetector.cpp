#include "pch.h"
#include "CEdgeDetector.h"



CEdgeDetector::CEdgeDetector() {
	m_iCanny1		= 100;
	m_iCanny2		= 200;
	m_iThreshold	= 150;
	m_dScale		= 10.0;
}
CEdgeDetector::~CEdgeDetector() {

}

RESULT_DATA CEdgeDetector::Detect(const cv::Mat& image, cv::Mat mark, cv::Rect roiRect, int iCanny1, int iCanny2, int iAngleLimit) {
	RESULT_DATA result;
	if (image.size() != mark.size())
		return result;

	// 1. 이미지 전처리
	cv::Mat image_edge = image.clone();
	if (image_edge.channels() == 3)
		cvtColor(image_edge, image_edge, COLOR_BGR2GRAY);
	
	if (mark.channels() == 3)
		cvtColor(mark, mark, COLOR_BGR2GRAY);

	// resize
	cv::resize(image_edge, image_edge, cv::Size(image.cols / m_dScale, image.rows / m_dScale));
	cv::resize(mark, mark, cv::Size(mark.cols / m_dScale, mark.rows / m_dScale));

	// clahe
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(5);
	clahe->setTilesGridSize(cv::Size(10, 10));
	clahe->apply(image_edge, image_edge);
	clahe->apply(mark, mark);

	// edge
	image_edge	= CannyImage(image_edge,	iCanny1, iCanny2);
	mark		= CannyImage(mark,			iCanny1, iCanny2);

	roiRect = cv::Rect(roiRect.x / m_dScale, roiRect.y / m_dScale, roiRect.width / m_dScale, roiRect.height / m_dScale);
	mark = mark(roiRect).clone();

	//cv::dilate(image_edge, image_edge, cv::Mat::ones(2, 2, CV_8UC1));

	// 2. mark 생성 (0~360)
	mark = MakeMarks(mark);

	// 3. detect
	uchar* data = image_edge.data;
	Concurrency::concurrent_vector<RESULT_DATA> results;

#pragma omp parallel for
	for (int x = 0; x < image_edge.cols; ++x) {
		for (int y = 0; y < image_edge.rows; ++y) {
			// limit angle 사이의 mark들을 매칭.
			for (int iAngle = -iAngleLimit; iAngle <= iAngleLimit; ++iAngle) {
				int iIndex = iAngle;
				if (iIndex < 0)
					iIndex += 360;

				int iMarkSize = m_markPts[iIndex].size();
				double dScore = 0;

				for (int m = 0; m < iMarkSize; ++m) {
					int ix = m_markPts[iIndex][m].x + x;
					int iy = m_markPts[iIndex][m].y + y;

					if (ix >= 0 && ix < image_edge.cols && iy >= 0 && iy < image_edge.rows) {
						if (data[iy * image_edge.cols + ix] == 255) {
							dScore++;
						}
					}
				}

				RESULT_DATA r;
				r.angle		= iIndex;
				r.pt		= cv::Point(x, y);
				r.score		= dScore / iMarkSize * 100;
				results.push_back(r);
			}
		}
	}

	// 4. find top score
	result.score = 0;
	for each (RESULT_DATA r in results) {
		if (result.score < r.score)
			result = r;
	}

	// 5. draw result
	result.image = image_edge.clone();
	if (result.image.channels() == 1)
		cvtColor(result.image, result.image, cv::COLOR_GRAY2BGR);

	auto rPts = GetMarkPts(result.angle);
	for (int i = 0; i < rPts.size(); ++i) {
		auto pt = rPts[i] + result.pt;

		if ((pt.x < result.image.cols) && (pt.y < result.image.rows)) {
			// 원본과 matching하여 색 표시.
			if (image_edge.at<uchar>(pt) == 255) {
				result.image.at<Vec3b>(pt)[0] = 0;
				result.image.at<Vec3b>(pt)[1] = 255;
				result.image.at<Vec3b>(pt)[2] = 0;
			}
			else {
				result.image.at<Vec3b>(pt)[0] = 0;
				result.image.at<Vec3b>(pt)[1] = 255;
				result.image.at<Vec3b>(pt)[2] = 255;
			}
		}
	}

	cv::resize(result.image, result.image, cv::Size(image_edge.cols * m_dScale, image_edge.rows * m_dScale));
	return result;
}


RESULT_DATA CEdgeDetector::Detect(const cv::Mat& image, cv::Mat mark, int iAngleLimit) {
	RESULT_DATA result;
	if (image.size() != mark.size())
		return result;

	// 1. 이미지 전처리
	cv::Mat image_edge = image.clone();
	if (image_edge.channels() == 3)
		cvtColor(image_edge, image_edge, COLOR_BGR2GRAY);

	// clahe
	cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
	clahe->setClipLimit(10);
	clahe->setTilesGridSize(cv::Size(9, 9));
	clahe->apply(image_edge, image_edge);

	// resize
	cv::resize(image_edge,	image_edge, cv::Size(image.cols / m_dScale, image.rows / m_dScale));
	cv::resize(mark,	mark,			cv::Size(image.cols / m_dScale, image.rows / m_dScale));

	// edge
	image_edge	= CannyImage(image_edge);
	mark		= CannyImage(mark);
	
	//cv::dilate(image_edge, image_edge, cv::Mat::ones(2, 2, CV_8UC1));
	
	int iEdgeCount = cv::countNonZero(image_edge);

	// 2. mark 생성 (0~360)
	mark = MakeMarks(mark);

	// 3. detect
	uchar* data = image_edge.data;
	Concurrency::concurrent_vector<RESULT_DATA> results;

#pragma omp parallel for
	for (int x = -100; x <= 100; ++x) {
		for (int y = -100; y <= 100; ++y) {
			// limit angle 사이의 mark들을 매칭.
			for (int iAngle = -iAngleLimit; iAngle <= iAngleLimit; ++iAngle) {
				int iIndex = iAngle;
				if (iIndex < 0)
					iIndex += 360;

				int iMarkSize = m_markPts[iIndex].size();
				double dScore = 0;

				for (int m = 0; m < iMarkSize; ++m) {
					int ix = m_markPts[iIndex][m].x + x;
					int iy = m_markPts[iIndex][m].y + y;

					if (ix >= 0 && ix < image_edge.cols && iy >= 0 && iy < image_edge.rows) {
						if (data[iy * image_edge.cols + ix] == 255) {
							dScore++;
						}
					}
				}
				//if (iMarkSize > iEdgeCount)
				//	iMarkSize = iEdgeCount;

				RESULT_DATA temp_result;
				temp_result.score = dScore / iMarkSize * 100;
				temp_result.pt = cv::Point(x, 0);
				temp_result.angle = iIndex;

				results.push_back(temp_result);
			}
		}
	}

	// 4. find top score
	result.score = 0;
	for each (RESULT_DATA r in results) {
		if (result.score < r.score)
			result = r;
	}

	// 5. draw result
	result.image = image_edge.clone();
	if (result.image.channels() == 1)
		cvtColor(result.image, result.image, cv::COLOR_GRAY2BGR);

	auto rPts = GetMarkPts(result.angle);
	for (int i = 0; i < rPts.size(); ++i) {
		auto pt = rPts[i] + result.pt;

		if ((pt.x < result.image.cols) && (pt.y < result.image.rows)) {
			// 원본과 matching하여 색 표시.
			if (image_edge.at<uchar>(pt) == 255) {
				result.image.at<Vec3b>(pt)[0] = 0;
				result.image.at<Vec3b>(pt)[1] = 255;
				result.image.at<Vec3b>(pt)[2] = 0;
			}
			else {
				result.image.at<Vec3b>(pt)[0] = 0;
				result.image.at<Vec3b>(pt)[1] = 255;
				result.image.at<Vec3b>(pt)[2] = 255;
			}
		}
	}

	cv::resize(result.image, result.image, cv::Size(image_edge.cols * m_dScale, image_edge.rows * m_dScale));
	return result;
}


cv::Mat CEdgeDetector::CorrectedTilt(const cv::Mat& image, int iAngle) {
	return RotateImage(image, -iAngle);
}


std::vector<cv::Point> CEdgeDetector::GetMarkPts(int iAngle) {
	if (iAngle < 0)
		iAngle += 360;

	return m_markPts[iAngle];
}


cv::Mat	CEdgeDetector::CannyImage(const cv::Mat& image, int iCanny1, int iCanny2) {
	cv::Mat canny;
	if (image.channels() == 3)
		cv::cvtColor(image, canny, cv::COLOR_BGR2GRAY);
	else
		canny = image.clone();

	cv::GaussianBlur(canny, canny, cv::Size(5, 5), 0);
	cv::Canny(canny, canny, iCanny1, iCanny2);

	return canny;
}


cv::Mat CEdgeDetector::RotateImage(const cv::Mat& image, int iAngle) {
	cv::Mat rotateImage;
	cv::Mat R = cv::getRotationMatrix2D(cv::Point2f(image.cols / 2, image.rows / 2), (double)iAngle, 1);
	cv::warpAffine(image, rotateImage, R, image.size(), 1, BORDER_CONSTANT, cv::Scalar::all(0));

	return rotateImage;
}


cv::Mat CEdgeDetector::MakeMarks(const cv::Mat& mark) {
	cv::Mat mark_resize = mark.clone();
	cv::resize(mark_resize, m_markImage, cv::Size(mark_resize.cols * m_dScale, mark_resize.rows * m_dScale));

#pragma omp parallel for
	for (int angle = 0; angle < 360; ++angle) {
		cv::Mat rotImage = RotateImage(mark_resize, angle);
		cv::threshold(rotImage, rotImage, m_iThreshold, 255, cv::THRESH_BINARY);
		cv::findNonZero(rotImage, m_markPts[angle]);
		//m_markImages[angle] = rotImage.clone();
	}

	return mark_resize;
}
