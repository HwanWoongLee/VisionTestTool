//#include "pch.h"
#include "MILModule.h"

#define MODEL_MAX_OCCURRENCES 30L
#define SAVE_PATH MIL_TEXT("../")
static const MIL_INT REFLECTANCE_PROFILE_HEIGHT = 256L;


CMILModule::CMILModule()
{
	m_milApplication	= MappAlloc(M_NULL, M_DEFAULT, M_NULL);												// Application identifier.
	m_milSystem			= MsysAlloc(M_DEFAULT, M_SYSTEM_HOST, M_DEFAULT, M_DEFAULT, M_NULL);				// System identifier.
	m_milDisplay		= MdispAlloc(m_milSystem, M_DEFAULT, MIL_TEXT("M_DEFAULT"), M_DEFAULT, M_NULL);		// Display identifier.
    
    m_contextMod        = M_NULL;
    m_contextStr        = M_NULL;
    m_contextMco        = M_NULL;
}

CMILModule::~CMILModule()
{
	if (M_NULL != m_milDisplay)
		MdispFree(m_milDisplay);

	if (M_NULL != m_milSystem)
		MsysFree(m_milSystem);

	if (M_NULL != m_milApplication)
		MappFree(m_milApplication);

    if (M_NULL != m_contextMod)
        MmodFree(m_contextMod);

    if (M_NULL != m_contextStr)
        MstrFree(m_contextStr);

    if (M_NULL != m_contextMco)
        McodeFree(m_contextMco);
}


MIL_ID CMILModule::MatToMILImage(const cv::Mat& image) {
	MIL_ID milImage;

	MIL_INT64	bufferAtt;
	bufferAtt = M_IMAGE + M_PROC;

	MbufAlloc2d(M_DEFAULT_HOST, image.cols, image.rows, (8 + M_UNSIGNED), bufferAtt, &milImage);
	MbufClear(milImage, 0L);
	MbufPut2d(milImage, 0, 0, image.cols, image.rows, image.data);

	return milImage;
}


bool CMILModule::SetModelFinderContext(std::string strContextPath) {
    if (m_contextMod != M_NULL) {
        MmodFree(m_contextMod);
    }

    std::wstring str;
    str.assign(strContextPath.begin(), strContextPath.end());
    MIL_CONST_TEXT_PTR path = str.c_str();
    
    try {
        MmodRestore(path, m_milSystem, M_DEFAULT, &m_contextMod);
        MmodPreprocess(m_contextMod, M_DEFAULT);
    }
    catch (...) {
        return false;
    }

    return true;
}

bool CMILModule::SetStringReaderContext(std::string strContextPath) {
    if (m_contextStr != M_NULL) {
        MstrFree(m_contextStr);
    }

    std::wstring str;
    str.assign(strContextPath.begin(), strContextPath.end());
    MIL_CONST_TEXT_PTR path = str.c_str();

    try {
        MstrRestore(path, m_milSystem, M_DEFAULT, &m_contextStr);
        MstrPreprocess(m_contextStr, M_DEFAULT);
    }
    catch (...) {
        return false;
    }

    return true;
}

bool CMILModule::SetCodeReaderContext(std::string strContextPath) {
    if (m_contextMco != M_NULL) {
        McodeFree(m_contextMco);
    }

    std::wstring str;
    str.assign(strContextPath.begin(), strContextPath.end());
    MIL_CONST_TEXT_PTR path = str.c_str();

    try {
        McodeRestore(path, m_milSystem, M_DEFAULT, &m_contextMco);
    }
    catch (...) {
        return false;
    }

    return true;
}


bool CMILModule::FindModel(const cv::Mat& image, vector<cv::Point2d>& ptPos, vector<double>& dAngle, vector<double>& dScore) {
    if (m_contextMod == M_NULL)
        return false;

    MIL_ID      MilImage = MatToMILImage(image);
    MIL_ID      MilModResult;

    MIL_INT     Found;                                  // number of models found
    MIL_DOUBLE  PosX[MODEL_MAX_OCCURRENCES],            // model pos x
                PosY[MODEL_MAX_OCCURRENCES],            // model pos y
                Angle[MODEL_MAX_OCCURRENCES],           // model angle
                Score[MODEL_MAX_OCCURRENCES];           // model score


    // alloc result
    MmodAllocResult(m_milSystem, M_DEFAULT, &MilModResult);
   
    // find
    MmodFind(m_contextMod, MilImage, MilModResult);

    // get result
    MmodGetResult(MilModResult, M_DEFAULT, M_NUMBER + M_TYPE_MIL_INT, &Found);

    if ((Found >= 1) && (Found < MODEL_MAX_OCCURRENCES)) {
        MmodGetResult(MilModResult, M_DEFAULT, M_POSITION_X, PosX);
        MmodGetResult(MilModResult, M_DEFAULT, M_POSITION_Y, PosY);
        MmodGetResult(MilModResult, M_DEFAULT, M_ANGLE, Angle);
        MmodGetResult(MilModResult, M_DEFAULT, M_SCORE, Score);

        ptPos.clear();
        dAngle.clear();
        dScore.clear();

        for (int i = 0; i < Found; ++i) {
            ptPos.push_back(cv::Point2d(PosX[i], PosY[i]));
            dAngle.push_back(Angle[i]);
            dScore.push_back(Score[i]);
        }
    }
    else {
        // free
        MbufFree(MilImage);
        MmodFree(MilModResult);
        return false;
    }

    // free
    MbufFree(MilImage);
    MmodFree(MilModResult);
    return true;
}

bool CMILModule::ReadString(const cv::Mat& image, std::string& strResult, double& dScore) {
    if (m_contextStr == M_NULL)
        return false;

    MIL_ID          MilImage = MatToMILImage(image);
    MIL_ID          MilStrResult;
    MIL_INT         NumberOfStringRead;
    MIL_TEXT_CHAR   StringResult[STRING_MAX_SIZE + 1];  /* String of characters read.       */
    MIL_DOUBLE      Score;

    // alloc result
    MstrAllocResult(m_milSystem, M_DEFAULT, &MilStrResult);

    // read
    MstrRead(m_contextStr, MilImage, MilStrResult);

    // get result
    MstrGetResult(MilStrResult, M_GENERAL, M_STRING_NUMBER + M_TYPE_MIL_INT, &NumberOfStringRead);

    if (NumberOfStringRead >= 1) {
        MstrGetResult(MilStrResult, 0, M_STRING + M_TYPE_TEXT_CHAR, StringResult);
        MstrGetResult(MilStrResult, 0, M_STRING_SCORE, &Score);

        std::wstring str = StringResult;
        strResult.assign(str.begin(), str.end());
        dScore = Score;
    }
    else {
        // free
        MbufFree(MilImage);
        MbufFree(MilStrResult);
        return false;
    }

    // free
    MbufFree(MilImage);
    MstrFree(MilStrResult);
    return true;
}

bool CMILModule::ReadCode(const cv::Mat& image, std::string& strResult, cv::Point& ptPos) {
    if (m_contextMco == M_NULL)
        return false;

    MIL_ID          MilImage = MatToMILImage(image);
    MIL_ID          MilMcoResult;
    MIL_ID          CodeStatus;

    MIL_DOUBLE      PosX, PosY;

    MIL_TEXT_CHAR   DataMatrixString[STRING_MAX_SIZE + 1];

    // alloc result
    McodeAllocResult(m_milSystem, M_DEFAULT, &MilMcoResult);

    // read
    McodeRead(m_contextMco, MilImage, MilMcoResult);

    // get result
    McodeGetResult(MilMcoResult, M_STATUS + M_TYPE_MIL_INT, &CodeStatus);

    if (CodeStatus == M_STATUS_OK) {
        McodeGetResult(MilMcoResult, M_POSITION_X, &PosX);
        McodeGetResult(MilMcoResult, M_POSITION_Y, &PosY);

        McodeGetResult(MilMcoResult, M_STRING, DataMatrixString);

        ptPos = cv::Point(PosX, PosY);

        std::wstring str = DataMatrixString;
        strResult.assign(str.begin(), str.end());
    }

    // free
    MbufFree(MilImage);
    McodeFree(MilMcoResult);
    return true;
}