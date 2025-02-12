#pragma once

#include "export.h"
#include <LibACMISCommon\ACMISCommon.h>


namespace ACMISSoftISP
{
	ACMISSOFTISP_API int xCalcImageRadius4Byte(int&ocx, int&ocy, int &radx, int &rady, const int* img, int w, int h, TCircleSpecN *Spec, double &angle, EDATAFORMAT eDataFormat = DATAFORMAT_BAYER_PARALLEL_12BIT); // using Y Image Only
	ACMISSOFTISP_API int xCalcImageRadius2Byte(int&ocx, int&ocy, int &radx, int &rady, const unsigned short* img, int w, int h, TCircleSpecN *Spec, double &angle); // using Y Image Only
	ACMISSOFTISP_API int xCalcImageRadius(int&ocx, int&ocy, int &radx, int &rady, const BYTE* img, int w, int h, TCircleSpecN *Spec, double &angle); // using Y Image Only

	ACMISSOFTISP_API int xCalcCenterCircleObject4Byte(int&ocx, int&ocy, const int* img, int w, int h, EDATAFORMAT eDataFormat = DATAFORMAT_BAYER_PARALLEL_12BIT); // using Y Image Only
	ACMISSOFTISP_API int xCalcCenterCircleObject2Byte(int&ocx, int&ocy, const unsigned short* img, int w, int h); // using Y Image Only
	ACMISSOFTISP_API int xCalcCenterCircleObject(int&ocx, int&ocy, const BYTE* img, int w, int h); // using Y Image Only

	ACMISSOFTISP_API void xCalcFixedCircleCenter4Byte(int&ocx, int&ocy, const int* img, int w, int h, EDATAFORMAT eDataFormat = DATAFORMAT_BAYER_PARALLEL_12BIT);
	ACMISSOFTISP_API void xCalcFixedCircleCenter2Byte(int&ocx, int&ocy, const unsigned short* img, int w, int h);
	ACMISSOFTISP_API void xCalcFixedCircleCenter(int&ocx, int&ocy, const BYTE* img, int w, int h);

	ACMISSOFTISP_API int xCalcCircleData4Byte(const int* img, int w, int h, TCircleSpecN *Spec, int &m_nOcx, int &m_nOcy, int &m_nRadx, int &m_nRady, double &m_dAngle, EDATAFORMAT eDataFormat = DATAFORMAT_BAYER_PARALLEL_12BIT);
	ACMISSOFTISP_API int xCalcCircleData2Byte(const unsigned short* img, int w, int h, TCircleSpecN *Spec, int &m_nOcx, int &m_nOcy, int &m_nRadx, int &m_nRady, double &m_dAngle);
	ACMISSOFTISP_API int xCalcCircleData(const BYTE* img, int w, int h, TCircleSpecN *Spec, int &m_nOcx, int &m_nOcy, int &m_nRadx, int &m_nRady, double &m_dAngle);
	ACMISSOFTISP_API int xCalcCircleData4Byte(const int* img, int w, int h, TCircleSpecN *Spec, TCircleData& tCircleData, EDATAFORMAT eDataFormat = DATAFORMAT_BAYER_PARALLEL_12BIT);
	ACMISSOFTISP_API int xCalcCircleData2Byte(const unsigned short* img, int w, int h, TCircleSpecN *Spec, TCircleData& tCircleData);
	ACMISSOFTISP_API int xCalcCircleData(const BYTE* img, int w, int h, TCircleSpecN *Spec, TCircleData& tCircleData);
	ACMISSOFTISP_API int xGetCircleMask(int w, int h, TCircleSpecN& Spec, TCircleData& tCircleData, BYTE* pMask);

	ACMISSOFTISP_API bool DetectSqureVertexs(const BYTE *pBuffer, int nImageWidth, int nImageHeight, RECT* inputROI, int num, POINT* outputPos);
	ACMISSOFTISP_API bool DetectPatchVertexs(const BYTE *pBuffer, int nImageWidth, int nImageHeight, TPatchInfo* tPatchInfo, int num, std::vector< std::vector<THarrisCornerPoint> >& outputPos);
	ACMISSOFTISP_API bool DetectPatchVertexsOpenCV(const BYTE* pBuffer, int nImageWidth, int nImageHeight, std::vector<TPatchInfo> vPatchInfo, std::vector<BYTE>& vMask, std::vector< std::vector<THarrisCornerPoint> >& outputPos, std::vector<TPatchSize>& vPatchSize);
#if 0
	ACMISSOFTISP_API bool DetectPatchVertexsOpenCV(const BYTE *pBuffer, int nImageWidth, int nImageHeight, TPatchInfo tPatchInfo, int nPatchIndex, std::vector<BYTE>& vMask, std::vector<THarrisCornerPoint>& outputPos, TPatchSize& tPatchSize);
#endif
	ACMISSOFTISP_API bool DetectCrossCenter(const BYTE *pBuffer, int nImageWidth, int nImageHeight, RECT* inputROI, int num, double dQualityLevel, POINT* outputPos); // default dQualityLevel = 0.1
	ACMISSOFTISP_API bool GetPatchSize(const BYTE *pBuffer, int nImageWidth, int nImageHeight, double dThreshold, TPatchInfo tPatchInfo, std::vector<BYTE>& vMask, TPatchSize& tMaxPatchSize);

	ACMISSOFTISP_API bool SearchEdgeCenter(const BYTE* pSrc, POINT tStartPoint, int nEdgeDir, int nScanDir, RECT crop_ROI, int nROIWidth, int nROIHeight, RECT& rtROI);
	ACMISSOFTISP_API void MoveROI2EdgeCenter(BYTE* pYBuffer, int nImageWidth, int nImageHeight, int nEdgeDir, RECT& rtROI, RECT crop_ROI, int nIndex);
	ACMISSOFTISP_API bool SearchSFRPatch(const BYTE* pYBuffer, int nImageWidth, int nImageHeight, bool bUsingOpenCV, int nPatchPartialDemosaic, TCircleSpecN& tCircleSpec, std::vector<TPatchInfo>& vSFRPatchInfo, std::vector<TPatchSize>& vSFRPatchSize, std::vector< std::vector<THarrisCornerPoint> >& vVertex);

	ACMISSOFTISP_API void HarrisCorner(const BYTE* img, int w, int h, RECT rtROI, double th, std::vector<THarrisCornerPoint>& vCorners);
	ACMISSOFTISP_API void HarrisCorner(const unsigned short* img, int w, int h, RECT rtROI, double dThreshold, std::vector<THarrisCornerPoint>& vCorners);
	ACMISSOFTISP_API void HarrisCornerOpenCV(const BYTE* img, int w, int h, RECT rtROI, double dThreshold, std::vector<THarrisCornerPoint>& vCorners);
	ACMISSOFTISP_API void HarrisCornerOpenCV(const unsigned short* img, int w, int h, RECT rtROI, double dThreshold, std::vector<THarrisCornerPoint>& vCorners);

	ACMISSOFTISP_API DBPOINT GetCentroidCircleCenter(int* pBuffer, int nImageWidth, int nImageHeight, double dThresholdRatio, double dThresholdMargin, int bEnableChannel, TDATASPEC& tDataSpec, double dCenterAvgBright, double* dCornerAvgBright, double& dThresholdUL, double& dThresholdLL);
	ACMISSOFTISP_API bool FindFiducialMark(const int* pBuffer, int nImageWidth, int nImageHeight, std::vector< POINT> ptROICenter, double dBinThreshold, double dMinCircularity, int nMinSize, double dMaxROIDelta, TInspectRegionOffset tInspectRegionOffset, std::vector<POINT>& vMarkPoint, std::vector<CDRect>& vMarkRadiusBox, std::vector<CDRect>& vMarkROI);
	ACMISSOFTISP_API bool DefectFlareMark(BYTE* pFrameY, int nImageWidth, int nImageHeight, RECT rtROI, double dBinaryThreshold, double dRadius, double dBorderWidth, std::vector<DBox2D>& vBox);
	ACMISSOFTISP_API bool DefectStrayLightPoint(BYTE* p8BitBuffer, int nROIWidth, int nROIHeight, double* dBinaryThreshold, int nMinLEDLightSize, TROICPoint tCenterPoint, POINT& ptStrayLightCenter, int& nMaxBlobSize);
	ACMISSOFTISP_API bool FindFiducialMark(BYTE* pFrameY, int nImageWidth, int nImageHeight, RECT rtFiducialMarkROI, int nROIBoxSize, int nMaxROIBoxSize, POINT ptFiducialMarkPoint, double dBinaryThreshold, double dMaxDeviation, double dRadius, EFiducialMarkShape eFiducialMarkShape, CDPoint& MarkPoint, CDRect& MarkRadiusBox, CDRect& MarkROI);
	ACMISSOFTISP_API bool FindFiducialMark(BYTE* pFrameY, int nImageWidth, int nImageHeight, int nROIBoxSize, int nMaxROIBoxSize, double dBinaryThreshold, double dRadius, std::vector<CDPoint>& vMarkPoint, std::vector<CDRect>& vMarkRadiusBox, std::vector<CDRect>& vMarkROI);
};
