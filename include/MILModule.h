#pragma once
#include "Mil.h"
#include "opencv2/opencv.hpp"

#pragma comment (lib, "MdispD3D.lib"	)
#pragma comment (lib, "mil.lib"			)
#pragma comment (lib, "Mil3dmap.lib"	)
#pragma comment (lib, "Milbead.lib"		)
#pragma comment (lib, "milblob.lib"		)
#pragma comment (lib, "milcal.lib"		)
#pragma comment (lib, "milcode.lib"		)
#pragma comment (lib, "milcolor.lib"	)
#pragma comment (lib, "miledge.lib"		)
#pragma comment (lib, "milfpga.lib"		)
#pragma comment (lib, "Milim.lib"		)
#pragma comment (lib, "milmeas.lib"		)
#pragma comment (lib, "Milmetrol.lib"	)
#pragma comment (lib, "milmod.lib"		)
#pragma comment (lib, "milocr.lib"		)
#pragma comment (lib, "milpat.lib"		)
#pragma comment (lib, "Milreg.lib"		)
#pragma comment (lib, "MilStr.lib"		)


#define STRING_MAX_SIZE         32L

using namespace std;
using namespace cv;


class CMILModule
{
public:
	CMILModule();
	~CMILModule();

	// Model Finder
	bool SetModelFinderContext(std::string strContextPath);
	bool FindModel(const cv::Mat& image, vector<cv::Point2d>& ptPos, vector<double>& dAngle, vector<double>& dScore);

	// String Reader
	bool SetStringReaderContext(std::string strContextPath);
	bool ReadString(const cv::Mat& image, std::string& strResult, double& dScore);

	// Code Reader
	bool SetCodeReaderContext(std::string strContextPath);
	bool ReadCode(const cv::Mat& image, std::string& strResult, cv::Point& ptPos);

protected:
	MIL_ID MatToMILImage(const cv::Mat& image);

private:
	MIL_ID m_milApplication;
	MIL_ID m_milSystem;
	MIL_ID m_milDisplay;

	MIL_ID m_contextMod;		// Model Finder Context
	MIL_ID m_contextStr;		// String Reader Context
	MIL_ID m_contextMco;		// Code Reader Context
};