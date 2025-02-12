#include "StdAfx.h"
#include "PRIFunc_Insp.h"
#include "../../GlobalDeclare.h"

#include <bitset>
#define R_RESULT_PASS							0	//��ǰ 
#define R_RESULT_FAIL							1	//�ҷ�
#define R_FAIL_NOISE							2
#define R_FAIL_BLACK_SPOT						3
#define R_FAIL_STAIN							4

using namespace ACMISSoftISP;


typedef struct __TShadingSpec
{
	TRelativeUniformitySpec  m_stRelativeUniformitySpec;
	TColorSensitivitySpecN m_stColorSensitivitySpec;
	TRelativeIlluminationSpecX m_stRelativeIlluminationXSpec;
} TShadingSpec;

//gMIUDevice.nWidth
CPRIFunc_Insp::CPRIFunc_Insp(void)
{
	imagePattern = cvCreateImage(cvSize(gMIUDevice.nWidth, gMIUDevice.nHeight), IPL_DEPTH_8U, 3);
}


CPRIFunc_Insp::~CPRIFunc_Insp(void)
{
	cvReleaseImage(&imagePattern);
}

bool CPRIFunc_Insp::func_Insp_Fov_Distortion_Rotate(BYTE* img, int _TYPE, bool bAutoMode)
{
	bool bRes = true;
#if 0
	int nBlackLevel = 0;
	TCHAR szLog[SIZE_OF_1K];
    

    int pitch = MbufInquire(vision.MilProcImageChild[4], M_PITCH, NULL);
    int width = MbufInquire(vision.MilProcImageChild[4], M_SIZE_X, NULL);
    int Height = MbufInquire(vision.MilProcImageChild[4], M_SIZE_Y, NULL);

	//if(getROI_SFR(m_nUnit, g_clVision.m_pImgBuff[m_nUnit][1], nPitch, nSizeX, nSizeY, g_clModelData[m_nUnit].m_clSfrInfo.m_clRectCircle) == false)


    vision.clearOverlay(CCD);
    vision.MilBufferUpdate();
    Task.getROI();

    if(Task._findCirclePos(vision.MilImageBuffer[4], pitch, width, Height, Task.SFR.rcROI)==false)
		return false;

	//theApp.MainDlg->g_CalcImageAlign();		//func_Insp_Fov_Distortion_Rotate

	CRect FovRectTemp[MAX_FOV_COUNT];
	memcpy(FovRectTemp, model.sfrElem.m_clRectFov, sizeof(FovRectTemp));

	if (_TYPE == DOT_TYPE)
	{
		//�����϶��� ������ũ
		//������ũ ã��
	}
	else 
	{   if(Task._findFovPos(vision.MilImageBuffer[4], pitch, width, Height, FovRectTemp) == false)//model.sfrElem.m_clRectFov)==false)

		//if (g_FindFovPos(m_nUnit, g_clVision.m_pImgBuff[m_nUnit][1], nPitch, nSizeX, nSizeY, g_clModelData[m_nUnit].m_clSfrInfo.m_clRectFov) == false)
		{
			//AddLog(_T("[�����˻�] Fov Find ����"), 1, m_nUnit);
			return false;
		}
	}
    //vision.drawOverlay(CCD);
    CString sLog;
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	//dTDATASPEC_n
	std::vector<BYTE> vBmpBuffer(nWidth * nHeight * 3, 0);
	IplImage *cvImg = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImg->imageData = (char*)vBmpBuffer.data();

	//ACMISSoftISP::xMakeBMP((BYTE*)pBuffer, vBmpBuffer.data(), nWidth, nHeight, tDataSpec);

	//RECT rtFiducial[4] = { { 477, 50, 527, 100 },{ 1114, 50, 1164, 100 },{ 475, 1152, 525, 1202 },{ 1111, 1154, 1161, 1204 } };

	//g_clModelData[nUnit].sfrElem.m_clRectCircle
	int specCount = 0;
	TFiducialMarkInfo tFiducialMarkInfo;
    //
    //-----ui�� ������ �׸�

	//tFiducialMarkInfo.pszChartType = "GRID";//�׸�ڽ� ��� ����  //���� = "DOT" , ���� = "GRID"
	tFiducialMarkInfo.dDistanceXFromCenter = 220;//Default ���� // ��񿡼� ������ Fiducial Mark �� Center ���� ���� �Է�
	tFiducialMarkInfo.dDistanceYFromCenter = 160;//Default ���� // ��񿡼� ������ Fiducial Mark �� Center ���� ���� �Է�
	//
    tFiducialMarkInfo.dRealGapX =           model.m_ChartSpec[specCount++];//23.514;			// chart �Լ� �� Fiducial Mark ���Ÿ�(cm) ���� �� �Է�
    tFiducialMarkInfo.dRealGapY =           model.m_ChartSpec[specCount++];//20.47;			// chart �Լ� �� Fiducial Mark ���Ÿ�(cm) ���� �� �Է�
    specCount++;
    tFiducialMarkInfo.nFiducialMarkType = _TYPE;// (int)model.m_ChartSpec[specCount++];//0;		//������ũ�� 4~9
    tFiducialMarkInfo.dModuleChartDistance = model.m_ChartSpec[specCount++];//2.0;	// ���� ��� ���� �Ÿ� �� �Է�
	tFiducialMarkInfo.nDistortionAlrotithmType = (int)model.m_ChartSpec[specCount++];//2;	//EDistortionAlgorithmType ����


	tFiducialMarkInfo.nROIBoxSize = 50;				//��񿡼� ������ Fiducial Mark ��ROI ������ �Է�
	tFiducialMarkInfo.nMaxROIBoxSize = 100;			//��񿡼� ������ Fiducial Mark ��ROI ������ �Է�
	tFiducialMarkInfo.dRadius = 0.7;				
	std::shared_ptr<CACMISFiducialMark> m_pChartProc = std::make_shared<CACMISFiducialMark>();
    bool bFindAlign = false;
    if (_TYPE == GRID_TYPE)
    {
        tFiducialMarkInfo.nFiducialMarkNum = 9;
        tFiducialMarkInfo.pszChartType = "GRID";//������ũ = "DOT" , fov��ũ = "GRID"
        bFindAlign = m_pChartProc->SetFiducialMarkSpec(&tFiducialMarkInfo, Task.m_FindFovRect, nWidth, nHeight);//model.sfrElem.m_clRectFov, nWidth, nHeight);
    }
    else
    {
        tFiducialMarkInfo.nFiducialMarkNum = 4;
        tFiducialMarkInfo.pszChartType = "DOT"; //������ũ = "DOT" , fov��ũ = "GRID"
        bFindAlign = m_pChartProc->SetFiducialMarkSpec(&tFiducialMarkInfo, Task.m_FindCircleRect, nWidth, nHeight);//model.sfrElem.m_CircleRoi, nWidth, nHeight);
    }

	
    //3,4,5 ��ǥ�� 0����
    double dfov = 0.0;
    double hfov = 0.0;
    double vfov = 0.0;

    double rotation = 0.0;
    double distortion = 0.0;
	if (bFindAlign)
	{
		m_pChartProc->CalcTiltAndRotation(nWidth, nHeight);
		m_pChartProc->CalcDistortion();
		m_pChartProc->CalcDFOV(nWidth, nHeight);

        dfov = m_pChartProc->GetDFOV();
        hfov = m_pChartProc->GetHFOV();
        vfov = m_pChartProc->GetVFOV();

        CxDPoint ptTilt = m_pChartProc->GetTilt();
        rotation = m_pChartProc->GetRotation();
        distortion = m_pChartProc->GetDistortion();


        model.m_Log_FOV_H_FOV = hfov;
        model.m_Log_FOV_V_FOV = vfov;
        model.m_Log_FOV_D_FOV = dfov;
        model.m_Log_Rotate = rotation;

		MESCommunication.m_dMesFov[0] = model.m_Log_FOV_H_FOV;
		MESCommunication.m_dMesFov[1] = model.m_Log_FOV_V_FOV;
		MESCommunication.m_dMesFov[2] = model.m_Log_FOV_D_FOV;
		MESCommunication.m_dMesRotate = model.m_Log_Rotate;

		for (int j = 0; j < m_pChartProc->GetMaxFiducialMarkCount(); j++)
		{
			const CxDRect& pt = m_pChartProc->GetFiducialMarkROI(j);
			cvRectangle(cvImg, cvPoint((int)pt.left, (int)pt.top), cvPoint((int)pt.right, (int)pt.bottom), CV_BLUE, 1);

			//printf("(%d) : [Fiducial Mark] Left(%d), Top(%d), Right(%d), Bototom(%d)\n", j, (int)pt.left, (int)pt.top, (int)pt.right, (int)pt.bottom);
            //_stprintf_s(szLog, SIZE_OF_1K, _T("(%d) : [Fiducial Mark] Left(%d), Top(%d), Right(%d), Bototom(%d)\n", j, (int)pt.left, (int)pt.top, (int)pt.right, (int)pt.bottom));
             //AddLog(szLog, 0, m_nUnit);
		}
	}
	else
	{
		//printf("Can't find fiducial mark!\n");
        _stprintf_s(szLog, SIZE_OF_1K, _T("Can't find fiducial mark!"));
        //AddLog(szLog, 0, m_nUnit);
		theApp.MainDlg->putListLog(szLog);
	}
    
	cvLine(cvImg, cvPoint(0, nHeight / 2 - 1), cvPoint(nWidth - 1, nHeight / 2 - 1), CV_GREEN, 1);
	cvLine(cvImg, cvPoint(nWidth / 2 - 1, 0), cvPoint(nWidth / 2 - 1, nHeight - 1), CV_GREEN, 1);

	const CxDPoint& ptCenter = m_pChartProc->GetRealCenter();
	cvLine(cvImg, cvPoint(0, (int)(ptCenter.y)), cvPoint(nWidth - 1, (int)(ptCenter.y)), CV_LAVENDER, 1);
	cvLine(cvImg, cvPoint((int)(ptCenter.x), 0), cvPoint((int)(ptCenter.x), nHeight - 1), CV_LAVENDER, 1);

	CvFont cvfont;
	CvPoint pt;
	int scale = (nWidth > 640 ? 1 : 2);
	double nFontSize = 0.5 / scale;
	char strTmp[1024];

	cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);

	// Display Result Tilt	
	const CxDPoint& ptTilt = m_pChartProc->GetTilt();

	sprintf_s(strTmp, "TILT %.3f,%.3f", ptTilt.x, ptTilt.y);
    sLog.Format(_T("[CCD] TILT %.3f,%.3f"), ptTilt.x, ptTilt.y);
    //AddLog(sLog, 0, m_nUnit);
	theApp.MainDlg->putListLog(sLog);
	pt.x = 60;
	pt.y = 20;

	if (m_pChartProc->InSpecTiltX() && m_pChartProc->InSpecTiltY())
		cvPutText(cvImg, strTmp, pt, &cvfont, CV_BLUE);
	else
		cvPutText(cvImg, strTmp, pt, &cvfont, CV_RED);
    //-------------------------------------------------------------------------
	// Display Rotation
	pt.y += 20;
	sprintf_s(strTmp, "ROTATION %.3f", rotation);
    //_stprintf_s(szLog, SIZE_OF_1K, _T("[CCD] ROTATION %.3f"), rotation);
    sLog.Format(_T("[CCD] ROTATION %.3f"), rotation);
    //AddLog(sLog, 0, m_nUnit);
	theApp.MainDlg->putListLog(sLog);
	if (m_pChartProc->InSpecRotation())
		cvPutText(cvImg, strTmp, pt, &cvfont, CV_BLUE);
	else
		cvPutText(cvImg, strTmp, pt, &cvfont, CV_RED);
    //-------------------------------------------------------------------------.

    if (_TYPE == DOT_TYPE)
    {
        // Display DFOV
        pt.y += 20;
        sprintf_s(strTmp, "DFOV %.3f(%.3f, %.3f)", dfov, hfov, vfov);
        sLog.Format(_T("[CCD] DFOV %.3f(%.3f, %.3f)"), dfov, hfov, vfov);
        //AddLog(sLog, 0, m_nUnit);
        theApp.MainDlg->putListLog(sLog);
        if (m_pChartProc->InSpecDFOV())
            cvPutText(cvImg, strTmp, pt, &cvfont, CV_BLUE);
        else
            cvPutText(cvImg, strTmp, pt, &cvfont, CV_RED);
    }

	CString sTemp;
    //-------------------------------------------------------------------------
    if (_TYPE == GRID_TYPE)
    {
        // Display Distortion
        pt.y += 20;
        sprintf_s(strTmp, "Distortion %.3f", distortion);
        sLog.Format(_T("[CCD] Distortion %.3f"), distortion);
        model.m_LogDistortion = distortion;
		MESCommunication.m_dMesDistortion = model.m_LogDistortion;
        //AddLog(sLog, 0, m_nUnit);
        theApp.MainDlg->putListLog(sLog);
        if (m_pChartProc->InSpecDistortion())
            cvPutText(cvImg, strTmp, pt, &cvfont, CV_BLUE);
        else
            cvPutText(cvImg, strTmp, pt, &cvfont, CV_RED);
		{
			sTemp.Format("Distortion Spec NG: %.3f(Spec:%d)", model.m_LogDistortion, 30);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("Distortion Spec NG: %.3f(Spec:%d)", model.m_LogDistortion, 30);
			MandoInspLog.iNGCnt++;
			bRes = false;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Distortion %.3lf(Spec:%d)]"), model.m_LogDistortion, 30);
			MandoInspLog.sNGList += sTemp;
		}
    }
	//  3.7
    //-------------------------------------------------------------------------
	//cvShowImage("OC_Fiducial", cvImg);
	cvReleaseImage(&cvImg);
	//cvWaitKey(0);
    g_SaveLGITLog(m_nUnit, "Chart", m_pChartProc->GetLogHeader(), m_pChartProc->GetLogData());

#endif
	return bRes; 
}


//-----------------------------------------------------------------------------
//
//	R/C,B/C,Cr/Cb �˻�(ColorSensitivity)
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_ColorSensitivity(BYTE* img, int index, bool bAutoMode)	//R/C,B/C,Cr/Cb
{
#if 0
	bool bRes = false;
	int i = 0;
   // TCHAR	szLog[SIZE_OF_1K];
    CString szLog;
	int nBlackLevel = 0;
    int nWidth = gMIUDevice.nWidth;
    int nHeight = gMIUDevice.nHeight;
    TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
    IplImage *cvImgRI = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImgRI = gMIUDevice.imageItp;

    int specCount = 0;
	TColorSensitivitySpecN m_stColorSensitivitySpec;
	RECT rtRoi;
	double dOffset[3] = { 0.0, 0.0, 0.0 }; // RGOffset, BGOffset, GrGbOffset
	SetRect(&rtRoi, 856, 432, 965, 507);
	m_stColorSensitivitySpec.tROI.eROIType = ROIType_RECT;
	m_stColorSensitivitySpec.tROI.ROICount = 1;
	m_stColorSensitivitySpec.tROI.dOffset = dOffset;
	m_stColorSensitivitySpec.tROI.pROIData = &rtRoi;
	m_stColorSensitivitySpec.dSpecDevMaxRG = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.dSpecDevMinRG = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.dSpecDevMaxBG = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.dSpecDevMinBG = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.dSpecDevMaxGrGb = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.dSpecDevMinGrGb = model.m_ColorSensitivitySpec[specCount++];//1.000000;
    m_stColorSensitivitySpec.nAdjustType = model.m_ColorSensitivitySpec[specCount++];// 0;
    m_stColorSensitivitySpec.nTypicalValueType = model.m_ColorSensitivitySpec[specCount++];// 0;
    std::shared_ptr<CACMISShadingColorSensitivity> m_pColorSensitivity = std::make_shared<CACMISShadingColorSensitivity>();

    if (index == MID_6500K_RAW)
    {
        szLog.Format("	[Color Sensitivity] 6500K Image Insp");
        
    }
    else if (index == MID_2800K_RAW)
    {
        szLog.Format("	[Color Sensitivity] 2800K Image Insp");
    }
    else
    {
        szLog.Format("	[Color Sensitivity] Manusl Image Insp");
        //Manual Insp
    }
    theApp.MainDlg->putListLog(szLog);
    //AddLog(szLog, 0, m_nUnit);
    //std::cout << "[Color Sensitivity] Version = " << m_pColorSensitivity->GetVersion() << std::endl;
   // _stprintf_s(szLog, SIZE_OF_1K, _T("	[Color Sensitivity] Version = %s"), m_pColorSensitivity->GetVersion());
    szLog.Format("	[Color Sensitivity] Version = %s" , m_pColorSensitivity->GetVersion());
    //AddLog(szLog, 0, m_nUnit);
	theApp.MainDlg->putListLog(szLog);
	bool result = m_pColorSensitivity->Inspect(img, 
        nWidth, nHeight,
        m_stColorSensitivitySpec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType,
		nBlackLevel);
	//
    //Result
    int nResult = R_RESULT_PASS;
    std::vector<TColorSensitivityResult> m_stColorSensitivityResult;
    for (i = 0; i < m_stColorSensitivitySpec.tROI.ROICount; i++)
    {
        m_stColorSensitivityResult.push_back(*m_pColorSensitivity->GetInspectionResult(i));

        // Check Color Sensitivity RG
        if ((m_stColorSensitivitySpec.dSpecDevMinRG > m_stColorSensitivityResult[i].tAdjustColorRatio.RG) || (m_stColorSensitivitySpec.dSpecDevMaxRG < m_stColorSensitivityResult[i].tAdjustColorRatio.RG))
            nResult = R_RESULT_FAIL;
        // Check Color Sensitivity BG
        if ((m_stColorSensitivitySpec.dSpecDevMinBG > m_stColorSensitivityResult[i].tAdjustColorRatio.BG) || (m_stColorSensitivitySpec.dSpecDevMaxBG < m_stColorSensitivityResult[i].tAdjustColorRatio.BG))
            nResult = R_RESULT_FAIL;
        // Check Color Sensitivity GrGb
        if ((m_stColorSensitivitySpec.dSpecDevMinGrGb > m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb) || (m_stColorSensitivitySpec.dSpecDevMaxGrGb < m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb))
            nResult = R_RESULT_FAIL;
        if (index == MID_6500K_RAW)
        {
            model.m_Log_CS_RC_6500K = m_stColorSensitivityResult[i].tAdjustColorRatio.RG;
            model.m_Log_CS_BC_6500K = m_stColorSensitivityResult[i].tAdjustColorRatio.BG;
            model.m_Log_CS_CrCb_6500K = m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb;
			MESCommunication.m_nMesColorSensitivity[0] = model.m_Log_CS_RC_6500K;
			MESCommunication.m_nMesColorSensitivity[1] = model.m_Log_CS_BC_6500K;
			MESCommunication.m_nMesColorSensitivity[2] = model.m_Log_CS_CrCb_6500K;
        }
        else
        {
            model.m_Log_CS_RC_2800K = m_stColorSensitivityResult[i].tAdjustColorRatio.RG;
            model.m_Log_CS_BC_2800K = m_stColorSensitivityResult[i].tAdjustColorRatio.BG;
            model.m_Log_CS_CrCb_2800K = m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb;
			MESCommunication.m_nMesColorSensitivity[3] = model.m_Log_CS_RC_2800K;
			MESCommunication.m_nMesColorSensitivity[4] = model.m_Log_CS_BC_2800K;
			MESCommunication.m_nMesColorSensitivity[5] = model.m_Log_CS_CrCb_2800K;
        }
       

        //Logging
        TColorSensitivityResult* pResult = &m_stColorSensitivityResult[i];
        //printf("[Color Sensitivity] RG=%.4f, BG=%.4f, GrGb=%.4f \n", pResult->tColorRatio.RG, pResult->tColorRatio.BG, pResult->tColorRatio.GrGb);
        //_stprintf_s(szLog, SIZE_OF_1K, _T("	[Color Sensitivity] RG=%.4f, BG=%.4f, GrGb=%.4f"), pResult->tColorRatio.RG, pResult->tColorRatio.BG, pResult->tColorRatio.GrGb);
        szLog.Format("	[Color Sensitivity] RG=%.4f, BG=%.4f, GrGb=%.4f" , pResult->tColorRatio.RG, pResult->tColorRatio.BG, pResult->tColorRatio.GrGb);
        theApp.MainDlg->putListLog(szLog);
		theApp.MainDlg->putListLog(szLog);
        //printf("[Color Sensitivity] R=%.4f, B=%.4f, B=%.4f \n", pResult->tColorRatio.R, pResult->tColorRatio.G, pResult->tColorRatio.B);
       // _stprintf_s(szLog, SIZE_OF_1K, _T("	[Color Sensitivity] R=%.4f, B=%.4f, B=%.4f"), pResult->tColorRatio.R, pResult->tColorRatio.G, pResult->tColorRatio.B);
        szLog.Format("	[Color Sensitivity] R=%.4f, B=%.4f, B=%.4f", pResult->tColorRatio.R, pResult->tColorRatio.G, pResult->tColorRatio.B);
        theApp.MainDlg->putListLog(szLog);
		theApp.MainDlg->putListLog(szLog);
        //printf("[Color Sensitivity] Gr=%.4f, Gb=%.4f \n", pResult->tColorRatio.Gr, pResult->tColorRatio.Gb);
       // _stprintf_s(szLog, SIZE_OF_1K, _T("	[Color Sensitivity] Gr=%.4f, Gb=%.4f"), pResult->tColorRatio.Gr, pResult->tColorRatio.Gb);
        szLog.Format("	[Color Sensitivity] Gr=%.4f, Gb=%.4f", pResult->tColorRatio.Gr, pResult->tColorRatio.Gb);
		theApp.MainDlg->putListLog(szLog);
        //Graphic
        const RECT* rt = m_pColorSensitivity->GetInspectionROI((EPos)i);
        cvRectangle(cvImgRI, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(0, 0, 255));
    }
    g_SaveLGITLog(m_nUnit, "ColorSensitivity", m_pColorSensitivity->GetLogHeader(), m_pColorSensitivity->GetLogData());
	return bRes;
#endif
	return true;
}
//-----------------------------------------------------------------------------
//
//	SNR �˻� DSM��
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Snr_Vne(BYTE* ChartRaw, BYTE* ChartSecondRaw, bool bAutoMode)
{
	bool bRes = true;
	int i = 0;
	TCHAR szLog[SIZE_OF_1K];
	Task.getROI();				// ���� ��ũ ��ġ �ν�..

	vision.MilBufferUpdate();

	int pitch = MbufInquire(vision.MilProcImageChild[4], M_PITCH, NULL);
	int width = MbufInquire(vision.MilProcImageChild[4], M_SIZE_X, NULL);
	int Height = MbufInquire(vision.MilProcImageChild[4], M_SIZE_Y, NULL);

	bool bRtn = Task._findCirclePos(vision.MilImageBuffer[4], pitch, width, Height, Task.SFR.rcROI);
	if (bRtn == false)
	{
		return false;
	}

	int nBlackLevel = 0;
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	TSNRBWSpecN tSNRSpec;

	std::shared_ptr< CACMISSignalNoiseRatioVNE > pSNRVNE = std::make_shared< CACMISSignalNoiseRatioVNE >();
	TSNRNormSpecN tSNRVNESpec;
	std::vector<RECT> vROI;
	std::vector<double> vOffset;
	tSNRVNESpec.tROI.ROICount = 1;
	vROI.resize(tSNRVNESpec.tROI.ROICount);
	vOffset.resize(tSNRVNESpec.tROI.ROICount);
	
	tSNRVNESpec.tROI.pROIData = vROI.data();
	tSNRVNESpec.tROI.eROIType = ROIType_RECT;
	tSNRVNESpec.tROI.dOffset = vOffset.data();
	tSNRVNESpec.dSNRThreshold = model.m_SnrSpec[0];



	_stprintf_s(szLog, SIZE_OF_1K, _T("[SNRVNE] dSNRThreshold = %.1lf "), tSNRVNESpec.dSNRThreshold);
	theApp.MainDlg->putListLog(szLog);

	int nROIWidth = 80;
	int nROIHeight = 80;

	double dCenterX = nWidth / 2;
	double dCenterY = nHeight / 2;

	int nShiftX = (dCenterX - (Task.m_CircleP[0].x + Task.m_CircleP[1].x + Task.m_CircleP[2].x + Task.m_CircleP[3].x) / 4 ) * -1;
	int nShiftY = (dCenterY - (Task.m_CircleP[0].y + Task.m_CircleP[1].y + Task.m_CircleP[2].y + Task.m_CircleP[3].y) / 4 ) * -1;

	//iSy = model.sfrElem.m_iOffsetY[i] + nShiftX;
	//iSx = model.sfrElem.m_iOffsetX[i] + nShiftY;

	/* Center */ 
	vROI[0].left = dCenterX + nShiftX - nROIWidth / 2;
	vROI[0].top = dCenterY + nShiftY - nROIHeight / 2;
	vROI[0].right = vROI[0].left + nROIWidth - 1;
	vROI[0].bottom = vROI[0].top + nROIHeight - 1;
	vOffset[0] = 0.0;


	BYTE *pBuffer[2] = { NULL, NULL };
	pBuffer[0] = ChartRaw;		
	pBuffer[1] = ChartSecondRaw;


	bool result = pSNRVNE->InspectM((const BYTE**)pBuffer, nWidth, nHeight, tSNRVNESpec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, false, 2);
	//tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, true, 2);
	for (int i = 0; i < tSNRVNESpec.tROI.ROICount; i++)
	{
		const TSNRResult* pSNRResult = pSNRVNE->GetSNRResult(i);

		_stprintf_s(szLog, SIZE_OF_1K, _T("[SNRVNE] Index =%d "), pSNRResult->nIndex);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[SNRVNE] SNRValue =%.2f "), pSNRResult->dSNRResult);
		theApp.MainDlg->putListLog(szLog);
	}



	const TSNRResult* pSNRResult = pSNRVNE->GetMinSNRResult();
	
	_stprintf_s(szLog, SIZE_OF_1K, _T("[SNR] Min Index =%d "), pSNRResult->nIndex);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[SNR] Min SNRValue=%.2f "), pSNRResult->dSNRResult);
	theApp.MainDlg->putListLog(szLog);

	const TSNRNormResult* pTSNRNormResult = pSNRVNE->GetInspectionResult(0);

	double mSnrSpecMin = model.m_SnrSpec[1];
	double mSnrSpecMax = model.m_SnrSpec[2];

	double mSnrValue = pTSNRNormResult->dSNRResult;
	model.m_LogSnr = mSnrValue;
	CString sTemp = _T("");
	if (mSnrValue < mSnrSpecMin || mSnrValue > mSnrSpecMax)
	{
		MandoInspLog.bInspRes = false;	//SNR VNE
		_stprintf_s(szLog, SIZE_OF_1K, _T("[SNR] dSNRResult = %.2lf NG (Spec:%.3lf ~ %.3lf)"), mSnrValue, mSnrSpecMin , mSnrSpecMax);
		theApp.MainDlg->putListLog(szLog);


		sTemp.Format("[SNR] dSNRResult Spec NG: %.2lf(Spec:%.3lf ~ %.3lf)", mSnrValue, mSnrSpecMin, mSnrSpecMax);
		//! Main Displayȭ�� Overlay NG List
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[SNR] Spec NG: %.2lf(Spec:%.3lf ~ %.3lf)", mSnrValue, mSnrSpecMin, mSnrSpecMax);
		MandoInspLog.iNGCnt++;
		sTemp.Format(_T("[SNR] %.2lf"), mSnrValue);
		MandoInspLog.sNGList += sTemp;
	}
	else
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("[SNR] dSNRResult = %.2f OK (Spec:%.3lf ~ %.3lf) "), mSnrValue, mSnrSpecMin, mSnrSpecMax);
		theApp.MainDlg->putListLog(szLog);

	}
	g_SaveLGITLog(m_nUnit, "SNRVNE", pSNRVNE->GetLogHeader(), pSNRVNE->GetLogData());
	return true;
	
}
//-----------------------------------------------------------------------------
//
//	SNR �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Snr(BYTE* img,  bool bAutoMode)
{
	bool bRes = true;
	int i = 0;
	TCHAR szLog[SIZE_OF_1K];

    int nBlackLevel = 0;
    int nWidth = gMIUDevice.nWidth;
    int nHeight = gMIUDevice.nHeight;
    TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	TSNRBWSpecN tSNRSpec;

	int pitch = MbufInquire(vision.MilProcImageChild[4], M_PITCH, NULL);
	int width = MbufInquire(vision.MilProcImageChild[4], M_SIZE_X, NULL);
	int Height = MbufInquire(vision.MilProcImageChild[4], M_SIZE_Y, NULL);

	//if(getROI_SFR(m_nUnit, g_clVision.m_pImgBuff[m_nUnit][1], nPitch, nSizeX, nSizeY, g_clModelData[m_nUnit].m_clSfrInfo.m_clRectCircle) == false)


	vision.clearOverlay(CCD);
	vision.MilBufferUpdate();
	Task.getROI();

	if (Task._findCirclePos(vision.MilImageBuffer[4], pitch, width, Height, Task.SFR.rcROI) == false)
		return false;

	//theApp.MainDlg->g_CalcImageAlign();		//func_Insp_Snr

	// Patch Center Point
	POINT ptCenter[3];
	std::vector<double> vdOffset(3, 0.0);
	std::vector<TROIPoint> vROI(56);

	//std::cout << std::endl;
	//std::cout << __FUNCTION__ << std::endl;

	//GetImageData(MODEL_IKC, tDataSpec, stImgInfo);

	// Image buffers
	std::vector<BYTE> vFrameBuffer(nHeight * nWidth * 2);

	memset(&tSNRSpec, 0x00, sizeof(tSNRSpec));

	int offsetX = model.m_oc_x*-1;
	int offsetY = model.m_oc_y*-1;


	//int izeX = model.sfrElem.m_clRectSnr[0].right - model.sfrElem.m_clRectSnr[0].left;

	int snrSizeX = model.sfrElem.m_clRectSnr[0].right -  model.sfrElem.m_clRectSnr[0].left;
	int snrSizeY = model.sfrElem.m_clRectSnr[0].bottom - model.sfrElem.m_clRectSnr[0].top;
	
	/* White */
	ptCenter[SNR_BW_IMAGE_WHITE].x = model.sfrElem.m_clRectSnr[0].left + (snrSizeX / 2) + offsetX;
    ptCenter[SNR_BW_IMAGE_WHITE].y = model.sfrElem.m_clRectSnr[0].top + (snrSizeY / 2) + offsetY;
	/* Gray */
	ptCenter[SNR_BW_IMAGE_GRAY].x = model.sfrElem.m_clRectSnr[1].left + (snrSizeX / 2) + offsetX;
    ptCenter[SNR_BW_IMAGE_GRAY].y = model.sfrElem.m_clRectSnr[1].top + (snrSizeY / 2) + offsetY;
    /* Black */
    ptCenter[SNR_BW_IMAGE_BLACK].x = model.sfrElem.m_clRectSnr[2].left + (snrSizeX / 2) + offsetX;
    ptCenter[SNR_BW_IMAGE_BLACK].y = model.sfrElem.m_clRectSnr[2].top + (snrSizeY / 2) + offsetY;
	 
    for (int i = 0; i < 3; i++)
    {
		vision.boxlist[CCD].addList(
			ptCenter[SNR_BW_IMAGE_WHITE + i].x - (snrSizeX / 2),
			ptCenter[SNR_BW_IMAGE_WHITE + i].y - (snrSizeY / 2),
			ptCenter[SNR_BW_IMAGE_WHITE + i].x + (snrSizeX / 2),
			ptCenter[SNR_BW_IMAGE_WHITE + i].y + (snrSizeY / 2),
			PS_SOLID, M_COLOR_MAGENTA);
        vROI[i].ptCenter.x = ptCenter[i].x;
        vROI[i].ptCenter.y = ptCenter[i].y;
		vROI[i].nROIWidth = snrSizeX;// 50;
		vROI[i].nROIHeight = snrSizeY;// 50;
    }
    // Spec
    tSNRSpec.dSNRThreshold = model.m_SnrSpec[0];// 20;		//1.0~100.0 ������� SPEC
    tSNRSpec.dDRThreshold = model.m_SnrSpec[1]; //100;
    tSNRSpec.tROI.eROIType = ROIType_POINT;
    tSNRSpec.tROI.ROICount = 3;
    tSNRSpec.tROI.dOffset = vdOffset.data();
    tSNRSpec.tROI.pROIData = vROI.data();
	// Load image
	//LoadImageData(vFrameBuffer.data(), _IMG_IK_Dynamic_Range_raw);

	// Inspect
	//return InspectSNRBW2(tDataSpec, tSNRSpec, vFrameBuffer.data(), stImgInfo.nDisplaySizeX, stImgInfo.nDisplaySizeY, true, _IMG_IK_Dynamic_Range_raw, false, true);
	if (bAutoMode == false)
	{
		/*
		Task.m_dShift_IMG_X = pFrame->ChartCenterOffsetX*(model.m_dSize_CCD_Cell / 1000.0f)*1.0f;
		Task.m_dShift_IMG_Y = pFrame->ChartCenterOffsetY*(model.m_dSize_CCD_Cell / 1000.0f)*1.0f;
		*/
		_stprintf_s(szLog, SIZE_OF_1K, _T("[pos]offset Test x:%.2lf"), offsetX*(model.m_dSize_CCD_Cell / 1000.0f)*1.0f);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[pos]point1 x:%d"), ptCenter[SNR_BW_IMAGE_WHITE].x);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[pos]point2 x:%d"), ptCenter[SNR_BW_IMAGE_GRAY].x);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[pos]point3 x:%d"), ptCenter[SNR_BW_IMAGE_BLACK].x);
		theApp.MainDlg->putListLog(szLog);
	}
	char title[100];

	// Image buffers
	std::vector<BYTE> vBmpBuffer(nWidth * nHeight * 3, 0);

	// output image
	IplImage *cvImgSNR = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImgSNR->imageData = (char*)vBmpBuffer.data();

	// for display
	/*if (bRaw)
	{
		ACMISSoftISP::xMakeBMP(img, vBmpBuffer.data(), nWidth, nHeight, tDataSpec);
	}
	else
	{
		std::copy(img, img + sizeof(BYTE) * nWidth * nHeight * 3, vBmpBuffer.data());
	}*/

	std::shared_ptr<CACMISSignalNoiseRatioBW> pSNRBW = std::make_shared<CACMISSignalNoiseRatioBW>();
	const TSNRResult* pSNRResult = nullptr;

	//std::cout << "[SNRBW2] Version: " << pSNRBW->GetVersion() << std::endl;
    _stprintf_s(szLog, SIZE_OF_1K, _T("[SNRBW2]Version:%s"), pSNRBW->GetVersion());
    //AddLog(szLog, 0, m_nUnit);
	theApp.MainDlg->putListLog(szLog);
	if (!pSNRBW->InspectM((const BYTE**)&img, nWidth, nHeight, tSNRSpec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, false, 1))
	{
		cvReleaseImage(&cvImgSNR);

		//std::cout << "[SNRBW2] Inspection Fail! " << std::endl;
        _stprintf_s(szLog, SIZE_OF_1K, _T("[SNRBW2] Inspection Fail!"));
        //AddLog(szLog, 0, m_nUnit);
		theApp.MainDlg->putListLog(szLog);
		return false;
	}
	//std::cout << "[SNRBW2] Region Count : " << pSNRBW->GetSNRRegionCount() << std::endl;
	_stprintf_s(szLog, SIZE_OF_1K, _T("[SNRBW2]Region Count Hot: %d"), pSNRBW->GetSNRRegionCount());
	//AddLog(szLog, 1, m_nUnit, true);
	theApp.MainDlg->putListLog(szLog);
	for (int i = 0; i < pSNRBW->GetSNRRegionCount(); i++)
	{
		pSNRResult = pSNRBW->GetSNRResult(i);
		if (pSNRResult)
		{
			//std::cout << "[SNRBW2] Index : " << pSNRResult->nIndex << std::endl;
			//std::cout << "[SNRBW2] Variance Value : " << pSNRResult->dVariance << std::endl;
			//std::cout << "[SNRBW2] Average Value : " << pSNRResult->dAverage << std::endl;
			//std::cout << "[SNRBW2] Region : " << pSNRResult->rtROI.left << ", " << pSNRResult->rtROI.top << ", " << pSNRResult->rtROI.right << ", " << pSNRResult->rtROI.bottom << std::endl;

			DisplaySNRGraphics(cvImgSNR, pSNRResult, nWidth, CV_GREEN);
		}
	}
	CString sTemp;
	pSNRResult = pSNRBW->GetMinSNRResult();
	if (pSNRResult)
	{
		//std::cout << "[SNRBW2] SNRValue : " << pSNRResult->dSNRResult << std::endl;
		//std::cout << "[SNRBW2] DRValue : " << pSNRResult->dDRResult << std::endl;
        _stprintf_s(szLog, SIZE_OF_1K, _T("[SNRBW2] SNRValue: %lf"), pSNRResult->dSNRResult);
        //AddLog(szLog, 1, m_nUnit, true);
		theApp.MainDlg->putListLog(szLog);
        _stprintf_s(szLog, SIZE_OF_1K, _T("[SNRBW2] DRValue: %lf"), pSNRResult->dDRResult);
        //AddLog(szLog, 1, m_nUnit, true);
		theApp.MainDlg->putListLog(szLog);

        model.m_Log_SNR_SNR = pSNRResult->dSNRResult;
        model.m_Log_SNR_DR = pSNRResult->dDRResult;

		MESCommunication.m_dMesSnr = model.m_Log_SNR_SNR;
		MESCommunication.m_dMesDr = model.m_Log_SNR_DR;

		DisplaySNRGraphics(cvImgSNR, pSNRResult, nWidth, CV_RED);

		if (model.m_Log_SNR_SNR < 30)
		{
			//ng
			sTemp.Format("SNR Spec NG: %.3f(Spec:%d)", model.m_Log_SNR_SNR, 30);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("SNR Spec NG: %.3f(Spec:%d)", model.m_Log_SNR_SNR, 30);
			MandoInspLog.iNGCnt++;
			bRes = false;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[SNR %.3lf(Spec:%d)]"), model.m_Log_SNR_SNR, 30);
			MandoInspLog.sNGList += sTemp;
		}
	}

	/*if (ImagePath != nullptr && bSaveResultImage == true)
	{
		char filename[100];
		char *tmp = (char *)strrchr(ImagePath, '.');

		if (tmp != nullptr)
		{
			strncpy_s(filename, ImagePath, (int)(tmp - ImagePath));
		}
		else
		{
			strcpy_s(filename, ImagePath);
		}
		strcat_s(filename, "_SNRBW2_result.bmp");
		printf("Saving Result image: %s\n", filename);
		cvSaveImage(filename, cvImgSNR);
	}*/

	//sprintf(title, "SNRBW2%s%s", ImagePath != nullptr ? " - " : "", ImagePath != nullptr ? ImagePath : "");
	//cvShowImage(title, cvImgSNR);
	cvReleaseImage(&cvImgSNR);
    g_SaveLGITLog(m_nUnit, "SNR", pSNRBW->GetLogHeader(), pSNRBW->GetLogData());
	//cvWaitKey(0);

	return bRes;
}
void CPRIFunc_Insp::DisplaySNRGraphics(IplImage* _cvImgBuf, const TSNRResult* _SNRResult, int _nImageWidth, CvScalar color)
{
	IplImage* cvImgBuf = _cvImgBuf;

	if (cvImgBuf == nullptr || _SNRResult == nullptr)
		return;

	CvFont cvfont;
	CvPoint pt, pt2;
	char strTmp[256], strTmp2[256];
	int scale = (_nImageWidth > 640 ? 1 : 2);
	double nFontSize = 0.5 / scale;
#define __FONTSIZE	9

	cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);
	if (_SNRResult->rtROI.right != 0 && _SNRResult->rtROI.bottom != 0)
	{
		if (_SNRResult->dSNRResult == 0.0 && _SNRResult->dDRResult == 0.0)
		{
			sprintf_s(strTmp, "[%d]Avg : %.3f", _SNRResult->nIndex, _SNRResult->dAverage);
			sprintf_s(strTmp2, "[%d]Var : %.3f", _SNRResult->nIndex, _SNRResult->dVariance);
		}
		else if (_SNRResult->dDRResult != 0.0)
		{
			sprintf_s(strTmp, "[%d]SNR : %2.3f", _SNRResult->nIndex, _SNRResult->dSNRResult);
			sprintf_s(strTmp2, "[%d]DR : %2.3f", _SNRResult->nIndex, _SNRResult->dDRResult);
		}
		else
		{
			sprintf_s(strTmp, "[%d]SNR : %2.3f", _SNRResult->nIndex, _SNRResult->dSNRResult);
			memset(strTmp2, 0, sizeof(strTmp2));
		}

		cvRectangle(cvImgBuf, cvPoint((int)_SNRResult->rtROI.left, (int)_SNRResult->rtROI.top), cvPoint((int)_SNRResult->rtROI.right, (int)_SNRResult->rtROI.bottom), color);

		pt.x = (_SNRResult->rtROI.left + _SNRResult->rtROI.right - 1) / 2 - (int)(strlen(strTmp) * __FONTSIZE / 2 / scale);
		pt.y = (_SNRResult->rtROI.top + _SNRResult->rtROI.bottom - 1) / 2 + __FONTSIZE / 2 / scale;
		pt2.x = (_SNRResult->rtROI.left + _SNRResult->rtROI.right - 1) / 2 - (int)(strlen(strTmp2) * __FONTSIZE / 2 / scale);
		pt2.y = (_SNRResult->rtROI.top + _SNRResult->rtROI.bottom - 1) / 2 + __FONTSIZE / 2 / scale + __FONTSIZE * 2;
	}
	else
	{
		if (_SNRResult->dSNRResult == 0.0 && _SNRResult->dDRResult == 0.0)
		{
			sprintf_s(strTmp, "Avg : %.3f", _SNRResult->dAverage);
			sprintf_s(strTmp2, "VAR : %.3f", _SNRResult->dVariance);
		}
		else if (_SNRResult->dDRResult != 0.0)
		{
			sprintf_s(strTmp, "SNR : %2.3f", _SNRResult->dSNRResult);
			sprintf_s(strTmp2, "DR : %2.3f", _SNRResult->dDRResult);
		}
		else
		{
			sprintf_s(strTmp, "SNR : %2.3f", _SNRResult->dSNRResult);
			memset(strTmp2, 0, sizeof(strTmp2));
		}

		pt.x = _nImageWidth / 2 - (int)(strlen(strTmp) * __FONTSIZE / 2 / scale);
		pt.y = __FONTSIZE * 2;
		pt2.x = _nImageWidth / 2 - (int)(strlen(strTmp2) * __FONTSIZE / 2 / scale);
		pt2.y = __FONTSIZE * 2 + __FONTSIZE * 2;
	}
	cvPutText(cvImgBuf, strTmp, pt, &cvfont, color);
	cvPutText(cvImgBuf, strTmp2, pt2, &cvfont, color);
}

//-----------------------------------------------------------------------------
//
//	Relative Illumination (RI) �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Shm_Illumination(BYTE* rawImage, bool bAutoMode, bool bUse8BitOnly)
{
	//TRelativeIlluminationSpecX& _Spec,
	bool bRes = true;
	int i = 0;
	int nResult = R_RESULT_PASS;
	char strTmp[1024];
	CString szLog;
	//TCHAR szLog[SIZE_OF_1K];
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;

	TRelativeIlluminationSpecN tRelativeIlluminationSpecN;
	// Image buffers
	std::vector<RECT> vROI;
	std::vector<double> vOffset;
	
	memset(&tRelativeIlluminationSpecN, 0x00, sizeof(tRelativeIlluminationSpecN));

	//TROIData tROI;
	tRelativeIlluminationSpecN.dSpecRIcornerMin = 60.0;
	tRelativeIlluminationSpecN.dSpecRIcornerMax = 100.0;
	tRelativeIlluminationSpecN.dSpecRIminMin = 5;
	tRelativeIlluminationSpecN.dSpecRIminMax = 100;
	tRelativeIlluminationSpecN.dCenterIntensity = 2000;
	tRelativeIlluminationSpecN.nSpecRINormalizeIndex = 0;

	tRelativeIlluminationSpecN.tROI.ROICount = 5;
	vROI.resize(tRelativeIlluminationSpecN.tROI.ROICount);
	tRelativeIlluminationSpecN.tROI.pROIData = vROI.data();
	tRelativeIlluminationSpecN.tROI.eROIType = ROIType_RECT;
	tRelativeIlluminationSpecN.tROI.dOffset = vOffset.data();


	//for (i = 0; i < tRelativeIlluminationSpecN.tROI.ROICount; i++)
	//{
	//	vROI[i].left = 10 + (i * 20) ;// g_clModelData[m_nUnit].m_LensShadingRoi[i].left;
	//	vROI[i].top = 10;//g_clModelData[m_nUnit].m_LensShadingRoi[i].top;
	//	vROI[i].right = vROI[i].left + 50;
	//	vROI[i].bottom = vROI[i].top + 50;
	//}
	RECT rtRefEdge;
	int roiSize = 50;
	double nfield = 0.85;
	int fieldGapW = (nWidth * nfield);
	int fieldGapH = (nHeight * nfield);

	vROI[0].left = nWidth / 2 - (roiSize / 2);
	vROI[0].top = nHeight / 2 - (roiSize / 2);
	vROI[0].right = vROI[0].left + roiSize;
	vROI[0].bottom = vROI[0].top + roiSize;
	

	//LT
	vROI[1].left = nWidth - fieldGapW;
	vROI[1].top = nHeight - fieldGapH;
	vROI[1].right = vROI[1].left + roiSize;
	vROI[1].bottom = vROI[1].top + roiSize;

	//RT
	vROI[2].left = fieldGapW;
	vROI[2].top = nHeight - fieldGapH;
	vROI[2].right = vROI[2].left + roiSize;
	vROI[2].bottom = vROI[2].top + roiSize;

	//BL
	vROI[3].left = nWidth - fieldGapW;
	vROI[3].top = fieldGapH;
	vROI[3].right = vROI[3].left + roiSize;
	vROI[3].bottom = vROI[3].top + roiSize;

	//BR
	vROI[4].left = fieldGapW;
	vROI[4].top = fieldGapH;
	vROI[4].right = vROI[4].left + roiSize;
	vROI[4].bottom = vROI[4].top + roiSize;

	
	for (i = 0; i < 5; i++)
	{
		rtRefEdge.left = vROI[i].left;
		rtRefEdge.top = vROI[i].top;
		rtRefEdge.right = vROI[i].right;
		rtRefEdge.bottom = vROI[i].bottom;

		vision.boxlist[CCD].addList(rtRefEdge, PS_SOLID, M_COLOR_BLUE);
	}
	
	bool bRaw = true;
	bool bEnableRelativeIllumination = true;
	bool bEnableRelativeIlluminationX = true;
	bool bEnableRelativeUniformity = true;
	bool bEnableColorSensitivity = true;
	bool bSaveResultImage = false;
	//----------------------------------------------------------------------
	// Inspect Main Camera
	//----------------------------------------------------------------------

	std::shared_ptr<CACMISShadingRelativeIllumination> m_pRelativeIllumination = std::make_shared<CACMISShadingRelativeIllumination>();

	//Inspection
	if (!m_pRelativeIllumination->Inspect(rawImage, nWidth, nHeight, tRelativeIlluminationSpecN, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly))
	{
		return false;
	}
	/*for (i = 0; i < tRelativeIlluminationSpecN.tROI.ROICount; i++)
	{
		const TRelativeIlluminationResult* pResult = m_pRelativeIllumination-> GetInspectionResult();
	}
*/
	TRelativeIlluminationResultN m_stRelativeIlluminationResult;
	m_stRelativeIlluminationResult = *m_pRelativeIllumination->GetInspectionResult(0);  //dRicorner

	MandoInspLog.dRicorner[0] = m_stRelativeIlluminationResult.dRIcornerRatio[1];
	MandoInspLog.dRicorner[1] = m_stRelativeIlluminationResult.dRIcornerRatio[2];
	MandoInspLog.dRicorner[2] = m_stRelativeIlluminationResult.dRIcornerRatio[3];
	MandoInspLog.dRicorner[3] = m_stRelativeIlluminationResult.dRIcornerRatio[4];

	MandoInspLog.dRiDiff = m_pRelativeIllumination->GetMaxDiff();
	CString sTemp;

	sTemp.Format("RIcorner 0: %.6lf", MandoInspLog.dRicorner[0]);
	theApp.MainDlg->putListLog(sTemp);
	sTemp.Format("RIcorner 1: %.6lf", MandoInspLog.dRicorner[1]);
	theApp.MainDlg->putListLog(sTemp);
	sTemp.Format("RIcorner 2: %.6lf", MandoInspLog.dRicorner[2]);
	theApp.MainDlg->putListLog(sTemp);
	sTemp.Format("RIcorner 3: %.6lf", MandoInspLog.dRicorner[3]);
	theApp.MainDlg->putListLog(sTemp);



	sTemp.Format("RI MaxDiff: %.6lf", MandoInspLog.dRiDiff);
	theApp.MainDlg->putListLog(sTemp);

	//if (MandoInspLog.dRiDiff < MandoSfrSpec.INSP_RIDiff_Spec)
	//{
	//	sTemp.Format("RI MaxDiff Spec NG: %.6lf (Spec:%.6lf)", MandoInspLog.dRiDiff, MandoSfrSpec.INSP_RIDiff_Spec);
	//	//! Main Displayȭ�� Overlay NG List
	//	MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("RI MaxDiff fail: %.6lf (Spec:%.6lf)", MandoInspLog.dRiDiff, MandoSfrSpec.INSP_RIDiff_Spec);
	//	MandoInspLog.iNGCnt++;
	//	MandoInspLog.bInspRes = false;//LCB
	//								  //bResult = false;
	//	theApp.MainDlg->putListLog(sTemp);
	//	MandoInspLog.sNGList += sTemp;
	//}
	//else
	//{
	//	sTemp.Format("RI MaxDiff Spec IN: %.6lf (Spec:%.6lf)", MandoInspLog.dRiDiff, MandoSfrSpec.INSP_RIDiff_Spec);
	//	theApp.MainDlg->putListLog(sTemp);
	//}


	g_SaveLGITLog(m_nUnit, "RI", m_pRelativeIllumination->GetLogHeader(), m_pRelativeIllumination->GetLogData());
	return true;
}


//-----------------------------------------------------------------------------
//
//	Color Reproduction �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Color_reproduction(const BYTE* pBuffer, bool bAutoMode)
{
	TCHAR szLog[SIZE_OF_1K];
	int i = 0;
	int nResult = R_RESULT_PASS;
	const DPOINT* dResultPos;
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC tDataSpec;			// = g_clLaonGrabberWrapper[m_nUnit].dColor_Tdataspec;
	tDataSpec.eDataFormat = DATAFORMAT_YUV;
	tDataSpec.eOutMode = OUTMODE_YUV422_CbYCrY;
	tDataSpec.eSensorType = SENSORTYPE_OTHER;
	tDataSpec.eDemosaicMethod = DEMOSAICMETHOD_NONE;
	tDataSpec.nBlackLevel = 0;


	bool bUse8BitOnly = false;

	//g_clVision.ClearOverlay(m_nUnit);
	vision.clearOverlay(CCD);
	CACMISColorReproduction *m_pColorReproduction = new CACMISColorReproduction();

	std::vector<RECT> vROI;
	std::vector<double> vOffset;
	std::vector<double> vThreshold;
	std::vector<TColorLab> vColorLab;

	//TColorReproductionSpecN m_stColorReproductionSpec;
	TColorReproductionSpecN m_stColorReproductionSpec;
	m_stColorReproductionSpec.tROI.ROICount = 4;
	vROI.resize(m_stColorReproductionSpec.tROI.ROICount);
	vOffset.resize(m_stColorReproductionSpec.tROI.ROICount);
	vThreshold.resize(m_stColorReproductionSpec.tROI.ROICount);
	vColorLab.resize(m_stColorReproductionSpec.tROI.ROICount);
	m_stColorReproductionSpec.tROI.pROIData = vROI.data();
	m_stColorReproductionSpec.tROI.dOffset = vOffset.data();
	m_stColorReproductionSpec.tROI.eROIType = ROIType_RECT;
	m_stColorReproductionSpec.pdThresholdDelta = vThreshold.data();
	m_stColorReproductionSpec.ptRefColorLab = vColorLab.data();

	vThreshold[0] = 4.0;//
	vThreshold[1] = 4.0;//
	vThreshold[2] = 4.0;//
	vThreshold[3] = 4.0;//

	vColorLab[0].L = 0.000000;//32.404;
	vColorLab[0].a = -27.172578;//44.222;
	vColorLab[0].b = 2.958152;//37.356;
	vColorLab[1].L = 0.000000;//11.326;
	vColorLab[1].a = -33.255820;//33.245;
	vColorLab[1].b = 32.886746;//-51.351;
	vColorLab[2].L = 0.000000;//2.151;
	vColorLab[2].a = -2.104597;//0.0;
	vColorLab[2].b = 1.595063;//-0.001;
	vColorLab[3].L = 0.000000;//26.191;
	vColorLab[3].a = 15.324899;//-25.316;
	vColorLab[3].b = 26.805437;//28.054;
	
	vROI[0].left = 150.000000;
	vROI[0].top = 250.000000;
	vROI[0].right = 250.000000;
	vROI[0].bottom = 350.000000;
	//
	vROI[1].left = 1700.000000;
	vROI[1].top = 250.000000;
	vROI[1].right = 1800.000000;
	vROI[1].bottom = 350.000000;
	//
	vROI[2].left = 150.000000;
	vROI[2].top = 750.000000;
	vROI[2].right = 250.000000;
	vROI[2].bottom = 850.000000;
	//
	vROI[3].left = 1700.000000;
	vROI[3].top = 750.000000;
	vROI[3].right = 1800.000000;
	vROI[3].bottom = 850.000000;

	for (int i = 0; i < 4; i++)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Roi #%d / Left:%d,Top:%d,Right:%d,Bottom:%d"), i + 1, vROI[i].left, vROI[i].top, vROI[i].right, vROI[i].bottom);
		//AddLog(szLog, 0, m_nUnit);
		theApp.MainDlg->putListLog(szLog);
	}

	char strTmp[1024];
	char szTmp[256];
	int dTextX = 0;
	int dTextY = 0;

	dTextX = vROI[0].left;
	dTextY = vROI[0].top - 60;
	if (dTextY < 0)
	{
		dTextY = 0;
	}
	sprintf_s(strTmp, "#1");
	//g_clVision.DrawMOverlayText(m_nUnit, dTextX, dTextY, strTmp, M_COLOR_CYAN, _T("Arial"), 8, 0);
	vision.textlist[CCD].addList(dTextX, dTextY, strTmp, M_COLOR_GREEN, 17, 7, "Arial");


	dTextX = vROI[1].left;
	dTextY = vROI[1].top - 60;
	if (dTextY < 0)
	{
		dTextY = 0;
	}
	sprintf_s(strTmp, "#2");
	//g_clVision.DrawMOverlayText(m_nUnit, dTextX, dTextY, strTmp, M_COLOR_CYAN, _T("Arial"), 8, 0);
	vision.textlist[CCD].addList(dTextX, dTextY, strTmp, M_COLOR_GREEN, 17, 7, "Arial");
	dTextX = vROI[2].left;
	dTextY = vROI[2].top - 60;
	if (dTextY < 0)
	{
		dTextY = 0;
	}
	sprintf_s(strTmp, "#3");
	//g_clVision.DrawMOverlayText(m_nUnit, dTextX, dTextY, strTmp, M_COLOR_CYAN, _T("Arial"), 8, 0);
	vision.textlist[CCD].addList(dTextX, dTextY, strTmp, M_COLOR_GREEN, 17, 7, "Arial");
	dTextX = vROI[3].left;
	dTextY = vROI[3].top - 60;
	if (dTextY < 0)
	{
		dTextY = 0;
	}
	sprintf_s(strTmp, "#4");
	//g_clVision.DrawMOverlayText(m_nUnit, dTextX, dTextY, strTmp, M_COLOR_CYAN, _T("Arial"), 8, 0);
	vision.textlist[CCD].addList(dTextX, dTextY, strTmp, M_COLOR_GREEN, 17, 7, "Arial");
	//
	//g_clVision.DrawMOverlayBox(m_nUnit, m_nUnit, vROI[0].left, vROI[0].top, vROI[0].right, vROI[0].bottom, M_COLOR_BLUE, 1, FALSE);
	//g_clVision.DrawMOverlayBox(m_nUnit, m_nUnit, vROI[1].left, vROI[1].top, vROI[1].right, vROI[1].bottom, M_COLOR_BLUE, 1, FALSE);
	//g_clVision.DrawMOverlayBox(m_nUnit, m_nUnit, vROI[2].left, vROI[2].top, vROI[2].right, vROI[2].bottom, M_COLOR_BLUE, 1, FALSE);
	//g_clVision.DrawMOverlayBox(m_nUnit, m_nUnit, vROI[3].left, vROI[3].top, vROI[3].right, vROI[3].bottom, M_COLOR_BLUE, 1, FALSE);

	vision.boxlist[CCD].addList(vROI[0], PS_SOLID, M_COLOR_BLUE);
	vision.boxlist[CCD].addList(vROI[1], PS_SOLID, M_COLOR_BLUE);
	vision.boxlist[CCD].addList(vROI[2], PS_SOLID, M_COLOR_BLUE);
	vision.boxlist[CCD].addList(vROI[3], PS_SOLID, M_COLOR_BLUE);
	//ROI VIEW
	vOffset[0] = 0.0;
	vOffset[1] = 0.0;
	vOffset[2] = 0.0;
	vOffset[3] = 0.0;


	double mColorValue = 0.0;
	double mColorSpec = 0.0;
	int mIndex = 0;

	bool dColorRtn = true;
	bool result = m_pColorReproduction->Inspect(pBuffer, nWidth, nHeight, m_stColorReproductionSpec,
		tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly,
		false, tDataSpec.eDemosaicMethod);

	for (i = 0; i < m_pColorReproduction->GetInspectionRegionCount(); i++)
	{
		const TColorReproductionResult* pResult = m_pColorReproduction->GetInspectionResult(i); // ��� ��

		MandoInspLog.dColorReproduction[i] = pResult->dDelta;
		//g_clMesCommunication[m_nUnit].m_nMesColorReproduction[i] = pResult->dDelta;

		_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, ROI(%4d, %4d, %4d %4d), Delta = %.3f, L = %.3f, a = %.3f, b = %.3f"),
			pResult->nIndex, pResult->rtROI.left, pResult->rtROI.top, pResult->rtROI.right, pResult->rtROI.bottom, pResult->dDelta, pResult->tColorLab.L, pResult->tColorLab.a, pResult->tColorLab.b);
		//AddLog(szLog, 0, m_nUnit);
		theApp.MainDlg->putListLog(szLog);
	}

	for (i = 0; i < 4; i++)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta = %.3f"), i + 1, MandoInspLog.dColorReproduction[i]);
		theApp.MainDlg->putListLog(szLog);
	}
	//mIndex = 0;
	//mColorValue = g_clMandoInspLog[m_nUnit].m_dColorReproductionResult[mIndex];
	//mColorSpec = vThreshold[mIndex];
	//if (mColorValue > mColorSpec)
	//{
	//	//NG
	//	g_clMesCommunication[m_nUnit].m_nMesColorReproductionResult[0] = 0;
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta NG = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//	dColorRtn = false;
	//}
	//else
	//{
	//	//OK
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta OK = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//}

	////
	//mIndex = 1;
	//mColorValue = g_clMandoInspLog[m_nUnit].m_dColorReproductionResult[mIndex];
	//mColorSpec = vThreshold[mIndex];
	//if (mColorValue > mColorSpec)
	//{
	//	//NG
	//	g_clMesCommunication[m_nUnit].m_nMesColorReproductionResult[1] = 0;
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta NG = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//	dColorRtn = false;
	//}
	//else
	//{
	//	//OK
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta OK = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//}

	////
	//mIndex = 2;
	//mColorValue = g_clMandoInspLog[m_nUnit].m_dColorReproductionResult[mIndex];
	//mColorSpec = vThreshold[mIndex];
	//if (mColorValue > mColorSpec)
	//{
	//	//NG
	//	g_clMesCommunication[m_nUnit].m_nMesColorReproductionResult[2] = 0;
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta NG = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//	dColorRtn = false;
	//}
	//else
	//{
	//	//OK
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta OK = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//}

	////
	//mIndex = 3;
	//mColorValue = g_clMandoInspLog[m_nUnit].m_dColorReproductionResult[mIndex];
	//mColorSpec = vThreshold[mIndex];
	//if (mColorValue > mColorSpec)
	//{
	//	//NG
	//	g_clMesCommunication[m_nUnit].m_nMesColorReproductionResult[3] = 0;
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta NG = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//	dColorRtn = false;
	//}
	//else
	//{
	//	//OK
	//	_stprintf_s(szLog, SIZE_OF_1K, _T("	[ColorReproduction] Region = %d, Delta OK = %.3f (Spec:%.3lf)"), mIndex + 1, mColorValue, mColorSpec);
	//	AddLog(szLog, 0, m_nUnit);
	//}
	//g_clVision.DrawOverlayAll(m_nUnit);
	vision.drawOverlay(CCD);



	//if (dColorRtn == false)
	//{
	//	g_clMesCommunication[m_nUnit].m_nMesFinalResult = 0;//XX
	//	g_clMandoInspLog[m_nUnit].m_sNGList += _T(" [Color Reproduction NG]");
	//	g_clMandoInspLog[m_nUnit].m_bInspRes = false;
	//	if (g_clMandoInspLog[m_nUnit].m_nNGCnt < 30)
	//	{
	//		g_clMandoInspLog[m_nUnit].m_sDispNG[g_clMandoInspLog[m_nUnit].m_nNGCnt].Format(_T("NG [Color Reproduction]"));
	//		g_clMandoInspLog[m_nUnit].m_nNGCnt++;
	//	}
	//}

	g_SaveLGITLog(m_nUnit, "Color Reproduction", m_pColorReproduction->GetLogHeader(), m_pColorReproduction->GetLogData());// , m_pColorReproduction->GetVersion());

	delete m_pColorReproduction;
	return dColorRtn;
}
bool CPRIFunc_Insp::g_GetIllumination(BYTE* RawImage)
{
	TCHAR	szLog[SIZE_OF_1K];
	bool bRtn = false;
	int nResult = R_RESULT_PASS;
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;

	TOpticalCenterN m_stOCCentroid;

	std::shared_ptr<CACMISOpticalCenterCentroid> m_pOpticalCenter = std::make_shared<CACMISOpticalCenterCentroid>();

	int specCount = 0;
	//TOpticalCenter tOpticalCenter;

	m_stOCCentroid.dOpticalCenterSpecX = 5;// g_clModelData[nUnit].m_ICSpec[specCount++];// 20;
	m_stOCCentroid.dOpticalCenterSpecY = 5;//g_clModelData[nUnit].m_ICSpec[specCount++];//20;
	m_stOCCentroid.dPixelPitch = 3;// g_clModelData[nUnit].m_ICSpec[specCount++];//3.75;  3.75;// 
																			//specCount++;
	m_stOCCentroid.nOCBlockSize = 100;// (int)g_clModelData[nUnit].m_ICSpec[specCount++];//100
	m_stOCCentroid.nEdgeTopMargin = 5;// (int)g_clModelData[nUnit].m_ICSpec[specCount++];//5
	m_stOCCentroid.dThresholdRatio = 0.5;// 0.5;// g_clModelData[nUnit].m_ICSpec[specCount++];//0.5;

	m_stOCCentroid.tROI.eROIType = ROIType_RECT;
	m_stOCCentroid.tROI.ROICount = 0;
	m_stOCCentroid.tROI.pROIData = NULL;
	m_stOCCentroid.tROI.dOffset = NULL;

	m_pOpticalCenter->SetInspectPosOffset(0, 0, 0, 0);
	bool bRet = m_pOpticalCenter->Inspect(RawImage, nWidth, nHeight, m_stOCCentroid,
		tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel,
		0, 0,
		tDataSpec.eDemosaicMethod);
	if (bRet == false)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("Illumination ���� ����"));
		theApp.MainDlg->putListLog(szLog);
		return false;
	}



	const DBPOINT* dResultPos;
	dResultPos = m_pOpticalCenter->GetOpticalCenterResult();
	if (dResultPos)
	{
		MandoInspLog.dOpticalOc[0] = (dResultPos->x - (nWidth / 2));
		MandoInspLog.dOpticalOc[1] = (dResultPos->y - (nHeight / 2));


		_stprintf_s(szLog, SIZE_OF_1K, _T("OpticalCenterCentroid X :%.3lf, Y :%.3lf "), MandoInspLog.dOpticalOc[0], MandoInspLog.dOpticalOc[1]);
		theApp.MainDlg->putListLog(szLog);
	}
	else
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("OpticalCenterCentroid TEST FAIL"));
		theApp.MainDlg->putListLog(szLog);
	}

	


	g_SaveLGITLog(0, _T("OCCentroid"), m_pOpticalCenter->GetLogHeader(), m_pOpticalCenter->GetLogData());

}
//-----------------------------------------------------------------------------
//
//	Relative Illumination (RI) �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Illumination(BYTE* img, bool bAutoMode, bool bUse8BitOnly)
{
	//TRelativeIlluminationSpecX& _Spec,
	bool bRes = true;
	int i = 0;
	int nResult = R_RESULT_PASS;
	char strTmp[1024];
    CString szLog;
	//TCHAR szLog[SIZE_OF_1K];
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;

	// Image information
	//RawImgInfo stImgInfo;
	// Spec information
	TShadingSpec tShadingSpec;

	//std::cout << std::endl;
	//std::cout << __FUNCTION__ << std::endl;

	//GetImageData(MODEL_NIO, tDataSpec, stImgInfo);

	// Image buffers
	std::vector<BYTE> vFrameBuffer(nWidth * nHeight * 2);

	memset(&tShadingSpec, 0x00, sizeof(TShadingSpec));


    //----------------------------------------------------------------------
    // Spec - Relative Illumination
    //----------------------------------------------------------------------

    // Initialize ROI regions and offset values
    std::vector<TROISPoint> vROI_field(100);//���� ��� 5
    std::vector<double> vOffset(100);
    std::vector<double> vCH0_thd(100);
    std::vector<double> vCH1_thd(100);
    std::vector<double> vCH2_thd(100);
    std::vector<double> vCH3_thd(100);

    //vector <CBlobRect> *m_vecBlob;	
    //g_clModelData[m_nUnit].m_RISpec[_x][_y] 11,9
    // Center
    int specCount = 0;
    int _xIndex = 0;
    int _x = 0;
    int _y = 0;

    for (_y = 0; _y < 100; _y++)
    {
        vROI_field[_y].ptStart.x = model.m_RirOI[_y][0]*2;// 0
        vROI_field[_y].ptStart.y = model.m_RirOI[_y][1] * 2;//1
        vROI_field[_y].nROIWidth = model.m_RirOI[_y][2];//2
        vROI_field[_y].nROIHeight = model.m_RirOI[_y][3];//3
                                                                          
        vCH0_thd[_y] = model.m_RirOI[_y][4];//4
        vCH1_thd[_y] = model.m_RirOI[_y][5];//5
        vCH2_thd[_y] = model.m_RirOI[_y][6];//6
        vCH3_thd[_y] = model.m_RirOI[_y][7];//7
                                                             
        vOffset[_y] = model.m_RirOI[_y][8];//8
    }
    //----------------------------------------------------------------------
    // Spec - Relative Illumination X
    //----------------------------------------------------------------------
    tShadingSpec.m_stRelativeIlluminationXSpec.nSpecPixelCntInBlock = model.m_RISpec[0];// 8;
    tShadingSpec.m_stRelativeIlluminationXSpec.nEnableChannel = model.m_RISpec[1];// 8;
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCenterIntensity[0] = model.m_RISpec[2];//200;
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCenterIntensity[1] = model.m_RISpec[3];//200;
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCenterIntensity[2] = model.m_RISpec[4];//200;
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCenterIntensity[3] = model.m_RISpec[5];//200;

    tShadingSpec.m_stRelativeIlluminationXSpec.tROI.eROIType = ROIType_SPOINT;// ROIType_FIELD;
    tShadingSpec.m_stRelativeIlluminationXSpec.tROI.ROICount = 100;
    tShadingSpec.m_stRelativeIlluminationXSpec.tROI.dOffset = vOffset.data();
    tShadingSpec.m_stRelativeIlluminationXSpec.tROI.pROIData = vROI_field.data();
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCH0Threshold = vCH0_thd.data();
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCH1Threshold = vCH1_thd.data();
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCH2Threshold = vCH2_thd.data();
    tShadingSpec.m_stRelativeIlluminationXSpec.dSpecCH3Threshold = vCH3_thd.data();

	bool bRaw = true;
	bool bEnableRelativeIllumination = true;
	bool bEnableRelativeIlluminationX = true;
	bool bEnableRelativeUniformity = true;
	bool bEnableColorSensitivity = true;
	bool bSaveResultImage = false;
	//----------------------------------------------------------------------
	// Inspect Main Camera
	//----------------------------------------------------------------------

	int nResultRI = -1, nResultRU = -1, nResultCS = -1, nResultRI_X;
	char title[100];

	// Image buffers
	std::vector<BYTE> vBmpBuffer(nWidth * nHeight * 3, 0);

	// output image
	IplImage *cvImgShading = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImgShading->imageData = (char*)vBmpBuffer.data();

	// for display
	if (bRaw)
	{
		ACMISSoftISP::xMakeBMP(img, vBmpBuffer.data(), nWidth, nHeight, tDataSpec);
	}
	else
	{
		//std::copy(img, img + sizeof(BYTE) * nWidth * nHeight * 3, vBmpBuffer.data());
	}


	if (bEnableRelativeIlluminationX)
	{
		nResultRI_X = Inspect_RelativeIlluminationX(img, nWidth, nHeight, tShadingSpec.m_stRelativeIlluminationXSpec, tDataSpec, cvImgShading, bUse8BitOnly);
		//std::cout << "nResult Relative Illumination X =" << nResultRI_X << std::endl;
       // _stprintf_s(szLog, SIZE_OF_1K, _T("	nResult Relative Illumination X = %d"), nResultRI_X);
        szLog.Format("	nResult Relative Illumination X = %d", nResultRI_X);
		theApp.MainDlg->putListLog(szLog);
	}


	/*if (ImagePath != nullptr && bSaveResultImage == true)
	{
		char filename[100];
		char *tmp = (char *)strrchr(ImagePath, '.');

		if (tmp != nullptr)
		{
			strncpy_s(filename, ImagePath, (int)(tmp - ImagePath));
		}
		else
		{
			strcpy_s(filename, ImagePath);
		}
		strcat_s(filename, "_Shading_result.bmp");
		printf("Saving Result image: %s\n", filename);
		cvSaveImage(filename, cvImgShading);
	}
*/
	//sprintf(title, "Shading%s%s", ImagePath != nullptr ? " - " : "", ImagePath != nullptr ? ImagePath : "");
	sprintf(title, "RI");
	if (bAutoMode == false)
	{
		cvShowImage(title, cvImgShading);
		cvWaitKey(0);
	}
	
	cvReleaseImage(&cvImgShading);
    
	return bRes;
}
int CPRIFunc_Insp::Inspect_RelativeIllumination(const BYTE* pBuffer, int nImageWidth, int nImageHeight, TRelativeIlluminationSpecN& _Spec, TDATASPEC& tDataSpec, IplImage *cvImgRI, bool bUse8BitOnly)
{
	int nResult = R_RESULT_PASS;

	std::shared_ptr<CACMISShadingRelativeIllumination> m_pRelativeIllumination = std::make_shared<CACMISShadingRelativeIllumination>();

	//std::cout << "[Relative Illumination] Version = " << m_pRelativeIllumination->GetVersion() << std::endl;

	//Inspection
	if (!m_pRelativeIllumination->Inspect(pBuffer, nImageWidth, nImageHeight, _Spec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly))
	{
		std::cout << "[Relative Illumination] Inspection Fail! " << std::endl;
		return FALSE;
	}

	//Result
	TRelativeIlluminationResultN m_stRelativeIlluminationResult;
	m_stRelativeIlluminationResult = *m_pRelativeIllumination->GetInspectionResult(0);
	// Check RI corner
	if ((_Spec.dSpecRIcornerMin > m_stRelativeIlluminationResult.dRIcorner) || (_Spec.dSpecRIcornerMax < m_stRelativeIlluminationResult.dRIcorner))
		nResult = R_RESULT_FAIL;
	// Check min RI corner
	if ((_Spec.dSpecRIminMin > m_stRelativeIlluminationResult.dRImin) || (_Spec.dSpecRIminMax < m_stRelativeIlluminationResult.dRImin))
		nResult = R_RESULT_FAIL;

	//Logging
	std::cout << "[Relative Illumination] Center= " << m_stRelativeIlluminationResult.dRIcenterRAW << std::endl;
	std::cout << "[Relative Illumination] UL=" << m_stRelativeIlluminationResult.dRIcornerRAW[0] << std::endl;
	std::cout << "[Relative Illumination] UR=" << m_stRelativeIlluminationResult.dRIcornerRAW[1] << std::endl;
	std::cout << "[Relative Illumination] LL=" << m_stRelativeIlluminationResult.dRIcornerRAW[2] << std::endl;
	std::cout << "[Relative Illumination] LR=" << m_stRelativeIlluminationResult.dRIcornerRAW[3] << std::endl;
	std::cout << "[Relative Illumination] RIcorner=" << m_stRelativeIlluminationResult.dRIcorner << std::endl;
	std::cout << "[Relative Illumination] RImin=" << m_stRelativeIlluminationResult.dRImin << std::endl;

	//Graphic
	for (int i = 0; i<5; i++)
	{
		const RECT* rt = m_pRelativeIllumination->GetInspectionROI((EPos)i);
		cvRectangle(cvImgRI, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(255, 0, 0));
	}

	return nResult;
}
int CPRIFunc_Insp::Inspect_RelativeIlluminationX(const BYTE* pBuffer, int nImageWidth, int nImageHeight, TRelativeIlluminationSpecX& _Spec, TDATASPEC& tDataSpec, IplImage *cvImgRI, bool bUse8BitOnly)
{
	int nResult = R_RESULT_PASS;
    TCHAR	szLog[SIZE_OF_1K];
	std::shared_ptr<CACMISShadingRelativeIllumination_X> m_pRelativeIlluminationX = std::make_shared<CACMISShadingRelativeIllumination_X>();

	//std::cout << "[Relative Illumination X] Version = " << m_pRelativeIlluminationX->GetVersion() << std::endl;
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Relative Illumination X] Version %s"), m_pRelativeIlluminationX->GetVersion());
	theApp.MainDlg->putListLog(szLog);
	//szLog.Format("[Relative Illumination X] Version %s", m_pRelativeIlluminationX->GetVersion());
	//Inspection
	if (!m_pRelativeIlluminationX->Inspect(pBuffer, nImageWidth, nImageHeight, _Spec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly))
	{
		//std::cout << "[Relative Illumination X] Inspection Fail! " << std::endl;
        _stprintf_s(szLog, SIZE_OF_1K, _T("	[Relative Illumination X] Inspection Fail!"));
        //AddLog(szLog, 0, m_nUnit);
		theApp.MainDlg->putListLog(szLog);
		return FALSE;
	}

	//Result
	TRelativeIlluminationResultX stCenterResult = *m_pRelativeIlluminationX->GetInspectionCenterResult();
	RECT rt;

	rt.left = stCenterResult.rtROI.left * 2;
	rt.top = stCenterResult.rtROI.top * 2;
	rt.right = stCenterResult.rtROI.right * 2;
	rt.bottom = stCenterResult.rtROI.bottom * 2;
	cvRectangle(cvImgRI, cvPoint(rt.left, rt.top), cvPoint(rt.right, rt.bottom), CV_RGB(100, 100, 100));

	//Logging
	std::cout << "[Relative Illumination X] Center= " << stCenterResult.dRIRawResult[0] << "," << stCenterResult.dRIRawResult[1] << "," << stCenterResult.dRIRawResult[2] << "," << stCenterResult.dRIRawResult[3] << "," << std::endl;


    _stprintf_s(szLog, SIZE_OF_1K, _T("	[Relative Illumination X] Center=%lf,%lf,%lf,%lf"),stCenterResult.dRIRawResult[0], stCenterResult.dRIRawResult[1], stCenterResult.dRIRawResult[2], stCenterResult.dRIRawResult[3]);
    //AddLog(szLog, 0, m_nUnit);
	theApp.MainDlg->putListLog(szLog);
	double CornerVarTemp = 0.0;

    model.m_Log_RI_Center_R = stCenterResult.dRIRawResult[1];
    model.m_Log_RI_Center_Cr = stCenterResult.dRIRawResult[0];
    model.m_Log_RI_Center_Cb = stCenterResult.dRIRawResult[3];
    model.m_Log_RI_Center_B = stCenterResult.dRIRawResult[2];

	model.m_Log_RI_CornerVar_R = _finite(m_pRelativeIlluminationX->GetCornerVariation(1)) ? m_pRelativeIlluminationX->GetCornerVariation(1) : 0.0;
	model.m_Log_RI_CornerVar_Cr = _finite(m_pRelativeIlluminationX->GetCornerVariation(0)) ? m_pRelativeIlluminationX->GetCornerVariation(0) : 0.0;
	model.m_Log_RI_CornerVar_Cb = _finite(m_pRelativeIlluminationX->GetCornerVariation(3)) ? m_pRelativeIlluminationX->GetCornerVariation(3) : 0.0;
	model.m_Log_RI_CornerVar_B = _finite(m_pRelativeIlluminationX->GetCornerVariation(2)) ? m_pRelativeIlluminationX->GetCornerVariation(2) : 0.0;
	//if (!_finite(dbXData))

	

	//spec Check
	CornerVarTemp = model.m_Log_RI_CornerVar_R;
	if (CornerVarTemp < model.m_RISpec[6] || CornerVarTemp >= model.m_RISpec[7])
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CornerVar_R Spec Out %lf [%.1lf~%.1lf]"), CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] CornerVar_R Spec Out: %lf [%.1lf~%.1lf])", CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
	}
	CornerVarTemp = model.m_Log_RI_CornerVar_Cr;
	if (CornerVarTemp < model.m_RISpec[6] || CornerVarTemp >= model.m_RISpec[7])
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CornerVar_Cr Spec Out %lf [%.1lf~%.1lf]"), CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] CornerVar_Cr Spec Out: %lf [%.1lf~%.1lf])", CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
	}
	CornerVarTemp = model.m_Log_RI_CornerVar_Cb;
	if (CornerVarTemp < model.m_RISpec[6] || CornerVarTemp >= model.m_RISpec[7])
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CornerVar_Cb Spec Out %lf [%.1lf~%.1lf]"), CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] CornerVar_Cb Spec Out: %lf [%.1lf~%.1lf])", CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
	}
	CornerVarTemp = model.m_Log_RI_CornerVar_B;
	if (CornerVarTemp < model.m_RISpec[6] || CornerVarTemp >= model.m_RISpec[7])
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CornerVar_B Spec Out: %lf [%.1lf~%.1lf]"), CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] CornerVar_B Spec Out: %lf [%.1lf~%.1lf])", CornerVarTemp, model.m_RISpec[6], model.m_RISpec[7]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
	}
	//

    if (model.m_Log_RI_Center_R < _Spec.dSpecCenterIntensity[1])
    {
        _stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] RED Spec Out: %lf [%.1lf]"), model.m_Log_RI_Center_R, _Spec.dSpecCenterIntensity[1]);
        theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] RED Spec Out:%lf [%.1lf])", model.m_Log_RI_Center_R, _Spec.dSpecCenterIntensity[1]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
    }
    if (model.m_Log_RI_Center_Cr < _Spec.dSpecCenterIntensity[0])
    {
        //g_clMesCommunication[m_nUnit].m_nMesFinalResult = 0;
        _stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CLEARR Spec Out: %lf [%.1lf]"), model.m_Log_RI_Center_Cr, _Spec.dSpecCenterIntensity[0]);
        theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] clearr Spec Out:%lf [%.1lf])", model.m_Log_RI_Center_Cr, _Spec.dSpecCenterIntensity[0]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
    }
    if (model.m_Log_RI_Center_Cb < _Spec.dSpecCenterIntensity[3])
    {
        //g_clMesCommunication[m_nUnit].m_nMesFinalResult = 0;
        _stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] CLEARB Spec Out: %lf [%.1lf]"), model.m_Log_RI_Center_Cb, _Spec.dSpecCenterIntensity[3]);
        theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] clearb Spec Out:%lf [%.1lf])", model.m_Log_RI_Center_Cb, _Spec.dSpecCenterIntensity[3]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
    }
    if (model.m_Log_RI_Center_B < _Spec.dSpecCenterIntensity[2])
    {
        //g_clMesCommunication[m_nUnit].m_nMesFinalResult = 0;
        _stprintf_s(szLog, SIZE_OF_1K, _T("	[RI] BLUE Spec Out: %lf [%.1lf]"), model.m_Log_RI_Center_B, _Spec.dSpecCenterIntensity[2]);
        theApp.MainDlg->putListLog(szLog);
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[RI] blue Spec Out:%lf [%.1lf])", model.m_Log_RI_Center_B, _Spec.dSpecCenterIntensity[2]);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//RI
		MandoInspLog.sNGList += szLog;
    }
	int regionCount = (int)m_pRelativeIlluminationX->GetInspectionRegionCount();
	for (int i = 0; i < regionCount; i++)
	{
		bool bSpecIn = m_pRelativeIlluminationX->InSpec(i);
		TRelativeIlluminationResultX result = *m_pRelativeIlluminationX->GetInspectionResult(i);

		rt.left = result.rtROI.left * 2;
		rt.top = result.rtROI.top * 2;
		rt.right = result.rtROI.right * 2;
		rt.bottom = result.rtROI.bottom * 2;

		if (result.bPass)
			cvRectangle(cvImgRI, cvPoint(rt.left, rt.top), cvPoint(rt.right, rt.bottom), CV_RGB(0, 255, 0));
		else
			cvRectangle(cvImgRI, cvPoint(rt.left, rt.top), cvPoint(rt.right, rt.bottom), CV_RGB(255, 0, 0));

		/*printf("[%d] %d(%.1f),%d(%.1f),%d(%.1f),%d(%.1f)\n", i, (int)result.dRIRawResult[0], result.dRINormalResult[0],
			(int)result.dRIRawResult[1], result.dRINormalResult[1], (int)result.dRIRawResult[2], result.dRINormalResult[2],
			(int)result.dRIRawResult[3], result.dRINormalResult[3]);*/

        model.m_Log_RI_RED[i] = result.dRINormalResult[1];
        model.m_Log_RI_CLEARR[i] = result.dRINormalResult[0];
        model.m_Log_RI_CLEARB[i] = result.dRINormalResult[3];
        model.m_Log_RI_BLUE[i] = result.dRINormalResult[2];
	}
	MESCommunication.m_nMesRICorner[0] = model.m_Log_RI_CornerVar_R;
	MESCommunication.m_nMesRICorner[1] = model.m_Log_RI_CornerVar_Cr;
	MESCommunication.m_nMesRICorner[2] = model.m_Log_RI_CornerVar_Cb;
	MESCommunication.m_nMesRICorner[3] = model.m_Log_RI_CornerVar_B;


	MESCommunication.m_nMesRI[0] = model.m_Log_RI_Center_R;
	MESCommunication.m_nMesRI[1] = model.m_Log_RI_Center_Cr;
	MESCommunication.m_nMesRI[2] = model.m_Log_RI_Center_Cb;
	MESCommunication.m_nMesRI[3] = model.m_Log_RI_Center_B;
	//
	MESCommunication.m_nMesRI[4] = model.m_Log_RI_RED[0];
	MESCommunication.m_nMesRI[5] = model.m_Log_RI_CLEARR[0];
	MESCommunication.m_nMesRI[6] = model.m_Log_RI_CLEARB[0];
	MESCommunication.m_nMesRI[7] = model.m_Log_RI_BLUE[0];
	MESCommunication.m_nMesRI[8] = model.m_Log_RI_RED[9];
	MESCommunication.m_nMesRI[9] = model.m_Log_RI_CLEARR[9];
	MESCommunication.m_nMesRI[10] = model.m_Log_RI_CLEARB[9];
	MESCommunication.m_nMesRI[11] = model.m_Log_RI_BLUE[9];
	MESCommunication.m_nMesRI[12] = model.m_Log_RI_RED[90];
	MESCommunication.m_nMesRI[13] = model.m_Log_RI_CLEARR[90];
	MESCommunication.m_nMesRI[14] = model.m_Log_RI_CLEARB[90];
	MESCommunication.m_nMesRI[15] = model.m_Log_RI_BLUE[90];
	MESCommunication.m_nMesRI[16] = model.m_Log_RI_RED[99];
	MESCommunication.m_nMesRI[17] = model.m_Log_RI_CLEARR[99];
	MESCommunication.m_nMesRI[18] = model.m_Log_RI_CLEARB[99];
	MESCommunication.m_nMesRI[19] = model.m_Log_RI_BLUE[99];

    g_SaveLGITLog(m_nUnit, "RI", m_pRelativeIlluminationX->GetLogHeader(), m_pRelativeIlluminationX->GetLogData());
	return nResult;
}
int CPRIFunc_Insp::Inspect_RelativeUniformity(const BYTE* pBuffer, int nImageWidth, int nImageHeight, TRelativeUniformitySpec& _Spec, TDATASPEC& tDataSpec, IplImage *cvImgRU, bool bUse8BitOnly)
{
	int nResult = R_RESULT_PASS;

	std::shared_ptr<CACMISShadingRelativeUniformity> m_pRelativeUniformity = std::make_shared<CACMISShadingRelativeUniformity>();

	//Logging
	//std::cout << "[Relative Uniformity] Version = " << m_pRelativeUniformity->GetVersion() << std::endl;
	TCHAR szLog[SIZE_OF_1K];
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Relative Uniformity X] Version %s"), m_pRelativeUniformity->GetVersion());
	theApp.MainDlg->putListLog(szLog);

	//Inspection
	if (!m_pRelativeUniformity->Inspect(pBuffer, nImageWidth, nImageHeight, _Spec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly))
	{
		std::cout << "[Relative Uniformity] Inspection Fail! " << std::endl;
		return FALSE;
	}

	//Result 
	TRelativeUniformityResult m_stRelativeUniformityResult;

	m_stRelativeUniformityResult = *m_pRelativeUniformity->GetInspectionResult();

	int roiSize = _Spec.nROISize;
	if (_Spec.dSpecMax < m_stRelativeUniformityResult.dResultMax)
		nResult = R_RESULT_FAIL;

	//Logging
	std::cout << "[Relative Uniformity] Max= " << m_stRelativeUniformityResult.dResultMax << std::endl;
	std::cout << "[Relative Uniformity] Block( " << m_stRelativeUniformityResult.nMaxBlockX << " , " << m_stRelativeUniformityResult.nMaxBlockY << " )";
	std::cout << "X= " << (m_stRelativeUniformityResult.nMaxBlockX)*(_Spec.nROISize) << ", ";
	std::cout << "Y= " << (m_stRelativeUniformityResult.nMaxBlockY)*(_Spec.nROISize) << ", ";
	std::cout << "Width= " << _Spec.nROISize << ", " << "Height= " << _Spec.nROISize << std::endl;
	std::cout << "[Relative Uniformity] Ref Block( " << m_stRelativeUniformityResult.nRefBlockX << " , " << m_stRelativeUniformityResult.nRefBlockY << " )";
	std::cout << "X= " << (m_stRelativeUniformityResult.nRefBlockX)*(_Spec.nROISize) << ", ";
	std::cout << "Y= " << (m_stRelativeUniformityResult.nRefBlockY)*(_Spec.nROISize) << ", ";
	std::cout << "Width= " << _Spec.nROISize << ", " << "Height= " << _Spec.nROISize << std::endl;

	//Graphic
	RECT rect, rectRef;

	rect.left = m_stRelativeUniformityResult.nMaxBlockX * roiSize;
	rect.top = m_stRelativeUniformityResult.nMaxBlockY * roiSize;
	rect.right = rect.left + roiSize;
	rect.bottom = rect.top + roiSize;

	rectRef.left = m_stRelativeUniformityResult.nRefBlockX * roiSize;
	rectRef.top = m_stRelativeUniformityResult.nRefBlockY * roiSize;
	rectRef.right = rectRef.left + roiSize;
	rectRef.bottom = rectRef.top + roiSize;

	cvRectangle(cvImgRU, cvPoint(rect.left, rect.top), cvPoint(rect.right, rect.bottom), CV_LIGHTGREEN);
	cvRectangle(cvImgRU, cvPoint(rectRef.left, rectRef.top), cvPoint(rectRef.right, rectRef.bottom), CV_YELLOW);

	return nResult;
}
int CPRIFunc_Insp::Inspect_ColorSensitivity(const BYTE* pBuffer,  bool bUse8BitOnly)
{
    int nWidth = gMIUDevice.nWidth;
    int nHeight = gMIUDevice.nHeight;
    TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;

	int nResult = R_RESULT_PASS;
	int i;
    int nResultRI = -1, nResultRU = -1, nResultCS = -1, nResultRI_X;
    char title[100];

    // Image buffers
    std::vector<BYTE> vBmpBuffer(nWidth * nHeight * 3, 0);

    // output image
    IplImage *cvImgShading = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
    cvImgShading->imageData = (char*)vBmpBuffer.data();

    // for display
    //if (bRaw)
    //{
    //    ACMISSoftISP::xMakeBMP(pBuffer, vBmpBuffer.data(), nWidth, nHeight, tDataSpec);
    //}
    //else
    //{
    //   // std::copy(pBuffer, pBuffer + sizeof(BYTE) * nWidth * nHeight * 3, vBmpBuffer.data());
    //}
    // Spec information
    TShadingSpec tShadingSpec;
    memset(&tShadingSpec, 0x00, sizeof(TShadingSpec));
    //----------------------------------------------------------------------
    // Spec - Color Sensitivity
    //----------------------------------------------------------------------

    // Initialize ROI regions and offset values
    RECT rtRoi;
    double dOffset = 0.0;

    SetRect(&rtRoi, 856, 432, 965, 507);

    tShadingSpec.m_stColorSensitivitySpec.tROI.eROIType = ROIType_RECT;
    tShadingSpec.m_stColorSensitivitySpec.tROI.ROICount = 1;
    tShadingSpec.m_stColorSensitivitySpec.tROI.dOffset = &dOffset;
    tShadingSpec.m_stColorSensitivitySpec.tROI.pROIData = &rtRoi;

    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMaxRG =   1.000000;
    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMinRG =   1.000000;
    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMaxBG =   1.000000;
    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMinBG =   1.000000;
    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMaxGrGb = 1.000000;
    tShadingSpec.m_stColorSensitivitySpec.dSpecDevMinGrGb = 1.000000;
    tShadingSpec.m_stColorSensitivitySpec.nTypicalValueType = 0;

    /*nResultCS = Inspect_ColorSensitivity(pBuffer, nWidth, nHeight, tShadingSpec.m_stColorSensitivitySpec, 
        tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, cvImgShading, bUse8BitOnly);
    std::cout << "nResult Color Sensitivity =" << nResultCS << std::endl;*/

    std::shared_ptr<CACMISShadingColorSensitivity> m_pColorSensitivity = std::make_shared<CACMISShadingColorSensitivity>();

    std::cout << "[Color Sensitivity] Version = " << m_pColorSensitivity->GetVersion() << std::endl;

    //Inspection
    /*if (!m_pColorSensitivity->Inspect(pBuffer, nWidth, nHeight, tShadingSpec.m_stColorSensitivitySpec, 
        tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, cvImgShading, bUse8BitOnly))
    {
        std::cout << "[Color Sensitivity] Inspection Fail! " << std::endl;
        return FALSE;
    }*/

    //Result
    //std::vector<TColorSensitivityResult> m_stColorSensitivityResult;
    //for (i = 0; i < _Spec.tROI.ROICount; i++)
    //{
    //    m_stColorSensitivityResult.push_back(*m_pColorSensitivity->GetInspectionResult(i));

    //    // Check Color Sensitivity RG
    //    if ((_Spec.dSpecDevMinRG > m_stColorSensitivityResult[i].tAdjustColorRatio.RG) || (_Spec.dSpecDevMaxRG < m_stColorSensitivityResult[i].tAdjustColorRatio.RG))
    //        nResult = R_RESULT_FAIL;
    //    // Check Color Sensitivity BG
    //    if ((_Spec.dSpecDevMinBG > m_stColorSensitivityResult[i].tAdjustColorRatio.BG) || (_Spec.dSpecDevMaxBG < m_stColorSensitivityResult[i].tAdjustColorRatio.BG))
    //        nResult = R_RESULT_FAIL;
    //    // Check Color Sensitivity GrGb
    //    if ((_Spec.dSpecDevMinGrGb > m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb) || (_Spec.dSpecDevMaxGrGb < m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb))
    //        nResult = R_RESULT_FAIL;

    //    //Logging
    //    TColorSensitivityResult* pResult = &m_stColorSensitivityResult[i];
    //    //printf("[Color Sensitivity] RG=%.4f, BG=%.4f, GrGb=%.4f \n", pResult->tColorRatio.RG, pResult->tColorRatio.BG, pResult->tColorRatio.GrGb);
    //    //printf("[Color Sensitivity] R=%.4f, B=%.4f, B=%.4f \n", pResult->tColorRatio.R, pResult->tColorRatio.G, pResult->tColorRatio.B);
    //    //printf("[Color Sensitivity] Gr=%.4f, Gb=%.4f \n", pResult->tColorRatio.Gr, pResult->tColorRatio.Gb);

    //    //Graphic
    //    const RECT* rt = m_pColorSensitivity->GetInspectionROI((EPos)i);
    //    cvRectangle(cvImgRI, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(0, 0, 255));
    //}



   /* if (ImagePath != nullptr && bSaveResultImage == true)
    {
        char filename[100];
        char *tmp = (char *)strrchr(ImagePath, '.');

        if (tmp != nullptr)
        {
            strncpy_s(filename, ImagePath, (int)(tmp - ImagePath));
        }
        else
        {
            strcpy_s(filename, ImagePath);
        }
        strcat_s(filename, "_Shading_result.bmp");
        printf("Saving Result image: %s\n", filename);
        cvSaveImage(filename, cvImgShading);
    }*/

    /*sprintf(title, "Shading%s%s", ImagePath != nullptr ? " - " : "", ImagePath != nullptr ? ImagePath : "");
    cvShowImage(title, cvImgShading);
    cvReleaseImage(&cvImgShading);
    cvWaitKey(0);*/
	//std::shared_ptr<CACMISShadingColorSensitivity> m_pColorSensitivity = std::make_shared<CACMISShadingColorSensitivity>();

	//std::cout << "[Color Sensitivity] Version = " << m_pColorSensitivity->GetVersion() << std::endl;

	////Inspection
	//if (!m_pColorSensitivity->Inspect(pBuffer, nWidth, nHeight, _Spec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, bUse8BitOnly))
	//{
	//	std::cout << "[Color Sensitivity] Inspection Fail! " << std::endl;
	//	return FALSE;
	//}

	////Result
	//std::vector<TColorSensitivityResult> m_stColorSensitivityResult;
	//for (i = 0; i < _Spec.tROI.ROICount; i++)
	//{
	//	m_stColorSensitivityResult.push_back(*m_pColorSensitivity->GetInspectionResult(i));

	//	// Check Color Sensitivity RG
	//	if ((_Spec.dSpecDevMinRG > m_stColorSensitivityResult[i].tAdjustColorRatio.RG) || (_Spec.dSpecDevMaxRG < m_stColorSensitivityResult[i].tAdjustColorRatio.RG))
	//		nResult = R_RESULT_FAIL;
	//	// Check Color Sensitivity BG
	//	if ((_Spec.dSpecDevMinBG > m_stColorSensitivityResult[i].tAdjustColorRatio.BG) || (_Spec.dSpecDevMaxBG < m_stColorSensitivityResult[i].tAdjustColorRatio.BG))
	//		nResult = R_RESULT_FAIL;
	//	// Check Color Sensitivity GrGb
	//	if ((_Spec.dSpecDevMinGrGb > m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb) || (_Spec.dSpecDevMaxGrGb < m_stColorSensitivityResult[i].tAdjustColorRatio.GrGb))
	//		nResult = R_RESULT_FAIL;

	//	//Logging
	//	TColorSensitivityResult* pResult = &m_stColorSensitivityResult[i];
	//	printf("[Color Sensitivity] RG=%.4f, BG=%.4f, GrGb=%.4f \n", pResult->tColorRatio.RG, pResult->tColorRatio.BG, pResult->tColorRatio.GrGb);
	//	printf("[Color Sensitivity] R=%.4f, B=%.4f, B=%.4f \n", pResult->tColorRatio.R, pResult->tColorRatio.G, pResult->tColorRatio.B);
	//	printf("[Color Sensitivity] Gr=%.4f, Gb=%.4f \n", pResult->tColorRatio.Gr, pResult->tColorRatio.Gb);

	//	//Graphic
	//	const RECT* rt = m_pColorSensitivity->GetInspectionROI((EPos)i);
	//	cvRectangle(cvImgRI, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(0, 0, 255));
	//}

	return nResult;
}
//-----------------------------------------------------------------------------
//
//	SATURATION �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Saturation(BYTE* ChartRawImage, bool bAutoMode)
{
	vision.drawOverlay(CCD);
	char szTmp[256];
	TCHAR szLog[SIZE_OF_1K];
	double mtfOffset = 0.0;
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
	CString sTemp = _T("");


	sTemp.Format(" -------------SATURATION TEST ");
	theApp.MainDlg->putListLog(sTemp);



	vision.clearOverlay(CCD);
	MandoInspLog.sBarcodeID = Task.ChipID;


	bool bResult = true; 
	double dSFR[36];//[MTF_INSP_CNT];

	CPoint Center;
	int dic = 0;
	bool sfrRt = true;
	int nBlackLevel = 0;
	//Task.getROI();				// ���� ��ũ ��ġ �ν�..
	//vision.MilBufferUpdate();
	//if (Task.getROI_SFR(MTF_INSP) == false)
	//{
	//	return false;
	//}
	//Center.x = (Task.m_CircleP[0].x + Task.m_CircleP[1].x + Task.m_CircleP[2].x + Task.m_CircleP[3].x) / 4;
	//Center.y = (Task.m_CircleP[0].y + Task.m_CircleP[1].y + Task.m_CircleP[2].y + Task.m_CircleP[3].y) / 4;

	//double m_Centerx = (Task.m_CirclePos_x[0] + Task.m_CirclePos_x[1] + Task.m_CirclePos_x[2] + Task.m_CirclePos_x[3]) / 4;
	//double m_Centery = (Task.m_CirclePos_y[0] + Task.m_CirclePos_y[1] + Task.m_CirclePos_y[2] + Task.m_CirclePos_y[3]) / 4;

	//model.m_oc_x = (gMIUDevice.nWidth / 2) - m_Centerx;
	//model.m_oc_y = ((gMIUDevice.nHeight / 2) - m_Centery);
	//model.m_LogOC_X = model.m_oc_x + (gMIUDevice.nWidth / 2);
	//model.m_LogOC_Y = model.m_oc_y + (gMIUDevice.nHeight / 2);
	//model.m_LogOC_DelatX = model.m_oc_x;
	//model.m_LogOC_DelatY = model.m_oc_y;

	////-----------------------------------------------------------------------------------
	////-----------------------------------------------------------------------------------
	int nWidth = gMIUDevice.imageItp->width;
	int nHeight = gMIUDevice.imageItp->height;
	int iSizeX = model.m_iSize_ROI_X + 1;
	int iSizeY = model.m_iSize_ROI_Y + 1;
	//std::shared_ptr<CACMISResolutionSFR> m_pSFRProc = std::make_shared<CACMISResolutionSFR>();
	POINT ptROICenter[36];//[MTF_INSP_CNT];


	TSaturationSpec m_stSaturationSpec;
	std::vector<TROICPoint> vROI_point;
	std::vector<double> vSpecBrightMin;
	std::vector<double> vSpecBrightMax;
	std::vector<double> vBinaryThreshold;
	m_stSaturationSpec.tROI.ROICount = 36;//MTF_INSP_CNT;
	m_stSaturationSpec.tROI.dOffset = NULL;
	m_stSaturationSpec.tROI.eROIType = ROIType_CPOINT;

	vROI_point.resize(m_stSaturationSpec.tROI.ROICount);
	vSpecBrightMin.resize(m_stSaturationSpec.tROI.ROICount);
	vSpecBrightMax.resize(m_stSaturationSpec.tROI.ROICount);
	vBinaryThreshold.resize(m_stSaturationSpec.tROI.ROICount);

	for (int i = 0; i < 36; i++)//for (int i = 0; i < MTF_INSP_CNT; i++)
	{
		ptROICenter[i].x = Task.SFR._64_Sfr_Rect[i].left + (iSizeX / 2);
		ptROICenter[i].y = Task.SFR._64_Sfr_Rect[i].top + (iSizeY / 2);
		Task.m_vROI[i].ptCenter.x = ptROICenter[i].x;
		Task.m_vROI[i].ptCenter.y = ptROICenter[i].y;
		Task.m_vROI[i].nROIWidth = model.m_MTF_Direction[i] == 1 ? iSizeX : iSizeY;
		Task.m_vROI[i].nROIHeight = model.m_MTF_Direction[i] == 0 ? iSizeY : iSizeX;

		//
		vSpecBrightMin[i] = model.m_SaturationMin[i];// 2400.0;
		vSpecBrightMax[i] = model.m_SaturationMax[i];// 4000.0;
		vBinaryThreshold[i] = model.m_SaturationThreshold[i];// 0.0;
		
	}

	
	m_stSaturationSpec.tROI.pROIData = Task.m_vROI.data(); ;// vROI_point.data();
	m_stSaturationSpec.dSpecBrightMin = vSpecBrightMin.data();
	m_stSaturationSpec.dSpecBrightMax = vSpecBrightMax.data();
	m_stSaturationSpec.dBinaryThreshold = vBinaryThreshold.data();


	vision.drawOverlay(CCD);


	std::shared_ptr<CACMISSaturation> m_pSaturation = std::make_shared<CACMISSaturation>();
	bool bRet = m_pSaturation->Inspect(ChartRawImage, nWidth, nHeight, m_stSaturationSpec,
		gMIUDevice.dTDATASPEC_n.eDataFormat, gMIUDevice.dTDATASPEC_n.eOutMode, gMIUDevice.dTDATASPEC_n.eSensorType, nBlackLevel, true);

	const TSaturationResult* pResult = m_pSaturation->GetInspectionResult();

	g_SaveLGITLog(m_nUnit, "Saturation", m_pSaturation->GetLogHeader(), m_pSaturation->GetLogData());

	double mSaturationValue = 0.0;
	double mSaturationMin = 0.0;
	double mSaturationMax = 0.0;
	if (pResult)
	{
		//printf("[Saturation] Bright Avg(%f) Min(%f) Max(%f)\n", pResult->dROIBrightAvg, pResult->dROIBrightMin,pResult->dROIBrightMax);

		_stprintf_s(szLog, SIZE_OF_1K, _T("[Saturation] Min:%f, Max:%f"), pResult->dROIBrightMin, pResult->dROIBrightMax);
		theApp.MainDlg->putListLog(szLog);

		_stprintf_s(szLog, SIZE_OF_1K, _T("[Saturation]Bright Avg(%f) "), pResult->dROIBrightAvg);
		theApp.MainDlg->putListLog(szLog);


		for (int i = 0; i < 36; i++)//for (int i = 0; i < MTF_INSP_CNT; i++)
		{
			mSaturationValue = m_pSaturation->GetInspectionResult(i)->dROIBrightAvg;

			model.m_LogSaturation[i] = mSaturationValue;
			mSaturationMin = model.m_SaturationMin[i];
			mSaturationMax = model.m_SaturationMax[i];
			if (mSaturationValue < mSaturationMin || mSaturationValue > mSaturationMax)
			{
				MandoInspLog.bInspRes = false;	//Saturation


				sTemp.Format("[Saturation]Index:%d Spec NG: %.2lf(Spec:%.3lf ~ %.3lf)", i + 1 , mSaturationValue, mSaturationMin, mSaturationMax);
				//! Main Displayȭ�� Overlay NG List
				MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Saturation]Index:%d Spec NG: %.2lf(Spec:%.3lf ~ %.3lf)", i + 1, mSaturationValue, mSaturationMin, mSaturationMax);
				MandoInspLog.iNGCnt++;
				theApp.MainDlg->putListLog(sTemp);
				sTemp.Format(_T("[Saturation]Index:%d %.2lf"), i + 1, mSaturationValue);
				MandoInspLog.sNGList += sTemp;
			}
			else
			{
				_stprintf_s(szLog, SIZE_OF_1K, _T("[Saturation]Index:%d Spec OK: %.2lf(Spec:%.3lf ~ %.3lf)"), i + 1, mSaturationValue, mSaturationMin, mSaturationMax);
				theApp.MainDlg->putListLog(szLog);
			}
		}
		

	}
	else
	{
		MandoInspLog.bInspRes = false;	//Saturation
		return false;
	}

	
	
	return true;
}
//-----------------------------------------------------------------------------
//
//	DEFECT �˻� (dark , white , hot)
//
//-----------------------------------------------------------------------------
//bool CPRIFunc_Insp::func_Insp_Defect(BYTE* img, bool bAutoMode)
bool CPRIFunc_Insp::func_Insp_Defect(BYTE* midImage, BYTE* lowImage, bool bAutoMode)
{
	int nBlackLevel = 0;
	TCHAR szLog[SIZE_OF_1K];
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;
	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	// Spec information
	
	BYTE *pBuffer[2] = { NULL, NULL };
	int i = 0;
    pBuffer[0] = midImage;// MIU.vDefectMidBuffer_6500K; // mid-level image for detecting dark/bright defect
    pBuffer[1] = lowImage;// MIU.vDefectLowBuffer; // low-level image for detecting hot defect

	// Image buffers
	std::vector<BYTE> vBmpBuffer1(nWidth * nHeight * 3, 0);
	std::vector<BYTE> vBmpBuffer2(nWidth * nHeight * 3, 0);

	// output image
	IplImage *cvImgDefect1 = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImgDefect1->imageData = (char*)vBmpBuffer1.data();

	IplImage *cvImgDefect2 = cvCreateImage(cvSize(nWidth, nHeight), 8, 3);
	cvImgDefect2->imageData = (char*)vBmpBuffer2.data();

	// for display
	ACMISSoftISP::xMakeBMP(pBuffer[0], vBmpBuffer1.data(), nWidth, nHeight, tDataSpec);
	ACMISSoftISP::xMakeBMP(pBuffer[1], vBmpBuffer2.data(), nWidth, nHeight, tDataSpec);

	

	TAllDefectPixel_OV stSpecAllDefectSpec;
	memset(&stSpecAllDefectSpec, 0x00, sizeof(stSpecAllDefectSpec));

	std::shared_ptr<CACMISDefectAllDefectPixel_OV> pACMISDefectAllDefect = std::make_shared<CACMISDefectAllDefectPixel_OV>();

	int m_nStartOffsetXOVDefect = 0;
	int m_nStartOffsetYOVDefect = 0;
	int m_nEndOffsetXOVDefect = 0;
	int m_nEndOffsetYOVDefect = 0;

	pACMISDefectAllDefect->SetInspectPosOffset(m_nStartOffsetXOVDefect, m_nStartOffsetYOVDefect, m_nEndOffsetXOVDefect, m_nEndOffsetYOVDefect);

	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]CACMISDefectAllDefectPixel_OV Version: %s"), pACMISDefectAllDefect->GetVersion());
	theApp.MainDlg->putListLog(szLog);


	// Spec
	//m_DefectSpec
	int specCount = 0;
	stSpecAllDefectSpec.nBlockSizeHot = (int)model.m_DefectSpec[specCount++];// 0;
	stSpecAllDefectSpec.nBlockSizeDark = (int)model.m_DefectSpec[specCount++];//1;
	stSpecAllDefectSpec.nBlockSizeBright = (int)model.m_DefectSpec[specCount++];//2;

	stSpecAllDefectSpec.nThresholdTypeHot = (int)model.m_DefectSpec[specCount++];//3;				// threshold type for hot defect
	stSpecAllDefectSpec.nThresholdTypeDark = (int)model.m_DefectSpec[specCount++];//4;				// threshold type for dark defect
	stSpecAllDefectSpec.nThresholdTypeBright = (int)model.m_DefectSpec[specCount++];//5;			// threshold type for bright defect

	stSpecAllDefectSpec.nThresholdTypeLineDefect = 1;
	stSpecAllDefectSpec.nAlgorithmTypeLineDefect = 2;

	stSpecAllDefectSpec.dDefectThresholdHot = model.m_DefectSpec[specCount++];//400.0;		// threshold for hot defect		(dark image)
	stSpecAllDefectSpec.dDefectThresholdDark = model.m_DefectSpec[specCount++];//10.0;		// threshold for dark defect	(mid-level image)
	stSpecAllDefectSpec.dDefectThresholdBright = model.m_DefectSpec[specCount++];//10.0;		// threshold for bright defect	(mid-level image)

	stSpecAllDefectSpec.dDefectThresholdRowInDark = 2.0;
	stSpecAllDefectSpec.dDefectThresholdRowInBright = 2.0;
	stSpecAllDefectSpec.dDefectThresholdColumnInDark = 2.0;
	stSpecAllDefectSpec.dDefectThresholdColumnInBright = 2.0;

	stSpecAllDefectSpec.nMaxHotDefectNum = (int)model.m_DefectSpec[specCount++];//126;				// specification of the maximum number
	stSpecAllDefectSpec.nMaxDarkDefectNum = (int)model.m_DefectSpec[specCount++];//126;			// specification of the maximum number
	stSpecAllDefectSpec.nMaxBrightDefectNum = (int)model.m_DefectSpec[specCount++];//126;			// specification of the maximum number

	stSpecAllDefectSpec.nMaxHotClusterNum = (int)model.m_DefectSpec[specCount++];//0;				// specification of the maximum number
	stSpecAllDefectSpec.nMaxDarkClusterNum = (int)model.m_DefectSpec[specCount++];//0;				// specification of the maximum number
	stSpecAllDefectSpec.nMaxBrightClusterNum = (int)model.m_DefectSpec[specCount++];//0;			// specification of the maximum number

	stSpecAllDefectSpec.nMaxRowDefectNumInDark = 0;
	stSpecAllDefectSpec.nMaxRowDefectNumInBright = 0;
	stSpecAllDefectSpec.nMaxColumnDefectNumInDark = 0;
	stSpecAllDefectSpec.nMaxColumnDefectNumInBright = 0;

	stSpecAllDefectSpec.nEnableChannel = (int)model.m_DefectSpec[specCount++];//1;
																			  //DefectInCouplet ���� (�ȴϺ�������)
	stSpecAllDefectSpec.nDefectInCluster = (int)model.m_DefectSpec[specCount++];//2;
	stSpecAllDefectSpec.nDefectInClusterChannel = (int)model.m_DefectSpec[specCount++];//2

	stSpecAllDefectSpec.nDefectType = EDefectKind_WhitePixel | EDefectKind_BlackPixel |
		EDefectKind_HotPixel | EDefectKind_WhiteClusterColor |
		EDefectKind_BlackClusterColor | EDefectKind_HotClusterColor;

	stSpecAllDefectSpec.tCircleSpec.bEnableCircle = (int)model.m_DefectSpec[specCount++];//false;
	stSpecAllDefectSpec.tCircleSpec.nPosOffsetX = (int)model.m_DefectSpec[specCount++];// 5;	//int
	stSpecAllDefectSpec.tCircleSpec.nPosOffsetY = (int)model.m_DefectSpec[specCount++];//5;	//int
	stSpecAllDefectSpec.tCircleSpec.dRadiusRatioX = model.m_DefectSpec[specCount++];//0.5;	//double
	stSpecAllDefectSpec.tCircleSpec.dRadiusRatioY = model.m_DefectSpec[specCount++];//0.5;	//double
	stSpecAllDefectSpec.tCircleSpec.dThresholdRatio = model.m_DefectSpec[specCount++];//0.5;	//double
	stSpecAllDefectSpec.tCircleSpec.dROIRange = model.m_DefectSpec[specCount++];//0.5;	//double
	stSpecAllDefectSpec.tCircleSpec.nUsedFixedCircle = (int)model.m_DefectSpec[specCount++];//0;//int

	//
	//=========================================================================================================
	//201113 �߰�
	TDefectOffset tDefectOffset;
	memset(&tDefectOffset, 0, sizeof(TDefectOffset));
	TCHAR szPath[SIZE_OF_1K];
	TCHAR szIniBuff[SIZE_OF_1K];
	_stprintf_s(szPath, SIZE_OF_1K, _T("%s\\%s\\Data\\setup.ini"), AA_MODEL_LOAD_DIR, model.mCurModelName);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_HOT_TYPE"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.nHotOffsetType = _ttoi((TCHAR*)(LPCTSTR)szIniBuff);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_BRIGHT_TYPE"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.nBrightOffsetType = _ttoi((TCHAR*)(LPCTSTR)szIniBuff);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_DARK_TYPE"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.nDarkOffsetType = _ttoi((TCHAR*)(LPCTSTR)szIniBuff);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_HOT"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.dHotDefectOffset = _ttof((TCHAR*)(LPCTSTR)szIniBuff);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_BRIGHT"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.dBrightDefectOffset = _ttof((TCHAR*)(LPCTSTR)szIniBuff);
	GetPrivateProfileString(_T("DEFECT"), _T("OFFSET_DARK"), NULL, szIniBuff, sizeof(szIniBuff), szPath);
	tDefectOffset.dDarkDefectOffset = _ttof((TCHAR*)(LPCTSTR)szIniBuff);


	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_HOT_TYPE : %d"), tDefectOffset.nHotOffsetType);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_BRIGHT_TYPE : %d"), tDefectOffset.nBrightOffsetType);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_DARK_TYPE : %d"), tDefectOffset.nDarkOffsetType);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_HOT : %.3lf"), tDefectOffset.dHotDefectOffset);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_BRIGHT : %.3lf"), tDefectOffset.dBrightDefectOffset);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]OFFSET_DARK : %.3lf"), tDefectOffset.dDarkDefectOffset);
	theApp.MainDlg->putListLog(szLog);
	pACMISDefectAllDefect->SetDefectOffset(tDefectOffset);
	//===============================================================================================================


	_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]pACMISDefectAllDefect Version: %s"), pACMISDefectAllDefect->GetVersion());
	theApp.MainDlg->putListLog(szLog);


	// Inspection
	int nHotDefectCount = 0;
	int nBrightDefectCount = 0;
	int nDarkDefectCount = 0;
	// Cluster
	int nHotClusterCount = 0;
	int nBrightClusterCount = 0;
	int nDarkClusterCount = 0;

	int nHotVeryDefectCount = 0;
	int nBrightVeryDefectCount = 0;
	int nDarkVeryDefectCount = 0;

	int nCFPNcount = 0;
	bool bRet = false;
	bRet = pACMISDefectAllDefect->InspectM((const BYTE**)pBuffer, nWidth, nHeight, stSpecAllDefectSpec,
		tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, tDataSpec.nBlackLevel, true, 2);	//defect�� true����
	if (bRet)
	{
		//Single Defect
		int nHotDefectCount = pACMISDefectAllDefect->GetHotDefectCount();
		int nBrightDefectCount = pACMISDefectAllDefect->GetBrightDefectCount();
		int nDarkDefectCount = pACMISDefectAllDefect->GetDarkDefectCount();
		// Cluster
		int nHotClusterCount = pACMISDefectAllDefect->GetHotDefectClusterCount();
		int nBrightClusterCount = pACMISDefectAllDefect->GetBrightDefectClusterCount();
		int nDarkClusterCount = pACMISDefectAllDefect->GetDarkDefectClusterCount();

		model.m_LogDefectBright = nBrightDefectCount;
		model.m_LogDefectDark = nDarkDefectCount;
		model.m_LogDefectHot = nHotDefectCount;

		model.m_LogDefectBrightCluster = nBrightClusterCount + pACMISDefectAllDefect->GetBrightDefectClusterColorCount();
		model.m_LogDefectDarkCluster = nDarkClusterCount + pACMISDefectAllDefect->GetDarkDefectClusterColorCount();
		model.m_LogDefectHotCluster = nHotClusterCount + pACMISDefectAllDefect->GetHotDefectClusterColorCount();
		model.m_LogDefectDarkHorLine = pACMISDefectAllDefect->GetDefectRowLineCountInDark();
		model.m_LogDefectDarkVerLine = pACMISDefectAllDefect->GetDefectColumnLineCountInDark();
		model.m_LogDefectBrightHorLine = pACMISDefectAllDefect->GetDefectRowLineCountInBright();
		model.m_LogDefectBrightVerLine = pACMISDefectAllDefect->GetDefectColumnLineCountInBright();
		//model.m_LogDefectColumnFPN[0] = pOnsemiRCCBDefect->GetDefectColumnFPNCount();

		int nCFPNcount = pACMISDefectAllDefect->GetDefectColumnFPNCount();
		for (int i = 0; i < nCFPNcount; i++)
		{
#if (____SENSOR_TYPE == S_ONSEMI)
			const TDefectResult* pColumnFPNResult = pACMISDefectAllDefect->GetDefectColumnFPNResult(i);
			model.m_LogDefectColumnFPN[i] = pColumnFPNResult->dValue;
#elif  (____SENSOR_TYPE == S_OMNIVISION)
			model.m_LogDefectColumnFPN[i] = 0.0;
#endif
		}
		MESCommunication.m_nMesDefect[0] = model.m_LogDefectBright;
		MESCommunication.m_nMesDefect[1] = model.m_LogDefectDark;
		MESCommunication.m_nMesDefect[2] = model.m_LogDefectHot;
		MESCommunication.m_nMesDefect[3] = model.m_LogDefectBrightCluster;
		MESCommunication.m_nMesDefect[4] = model.m_LogDefectDarkCluster;
		MESCommunication.m_nMesDefect[5] = model.m_LogDefectHotCluster;
		//
		MESCommunication.m_nMesDefect[6] = model.m_LogDefectDarkHorLine;
		MESCommunication.m_nMesDefect[7] = model.m_LogDefectDarkVerLine;
		MESCommunication.m_nMesDefect[8] = model.m_LogDefectBrightHorLine;
		MESCommunication.m_nMesDefect[9] = model.m_LogDefectBrightVerLine;

		MESCommunication.m_nMesDefect[10] = model.m_LogDefectColumnFPN[1]; //Red
		MESCommunication.m_nMesDefect[11] = model.m_LogDefectColumnFPN[0]; //Cr
		MESCommunication.m_nMesDefect[12] = model.m_LogDefectColumnFPN[3]; //Cb
		MESCommunication.m_nMesDefect[13] = model.m_LogDefectColumnFPN[2]; //Blue

		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Single Count Hot: %d"), nHotDefectCount);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Single Count Birght: %d"), nBrightDefectCount);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Single Count Dark: %d"), nDarkDefectCount);
		theApp.MainDlg->putListLog(szLog);
		//
		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Cluster Count Hot: %d"), model.m_LogDefectHotCluster);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Cluster Count Birght: %d"), model.m_LogDefectBrightCluster);
		theApp.MainDlg->putListLog(szLog);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[Defect]Cluster Count Dark: %d"), model.m_LogDefectDarkCluster);
		theApp.MainDlg->putListLog(szLog);
		//
		CString sTemp;
		for (int i = 0; i < 14; i++)
		{
			MESCommunication.m_nMesDefectResult[i] = 1;	//init
		}
		if (nBrightDefectCount > stSpecAllDefectSpec.nMaxBrightDefectNum)
		{
			MESCommunication.m_nMesDefectResult[0] = 0;
			sTemp.Format("[Defect Bright] Spec NG: %d(Spec:%d)", nBrightDefectCount, stSpecAllDefectSpec.nMaxBrightDefectNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect Bright] Spec NG: %d(Spec:%d)", nBrightDefectCount, stSpecAllDefectSpec.nMaxBrightDefectNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect Bright %d: %d]"), nBrightDefectCount, stSpecAllDefectSpec.nMaxBrightDefectNum);
			MandoInspLog.sNGList += sTemp;
		}
		if (nDarkDefectCount > stSpecAllDefectSpec.nMaxDarkDefectNum)
		{
			MESCommunication.m_nMesDefectResult[1] = 0;
			sTemp.Format("[Defect Dark] Spec NG: %d(Spec:%d)", nDarkDefectCount, stSpecAllDefectSpec.nMaxDarkDefectNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect Dark] Spec NG: %d(Spec:%d)", nDarkDefectCount, stSpecAllDefectSpec.nMaxDarkDefectNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect Dark %d: %d]"), nDarkDefectCount, stSpecAllDefectSpec.nMaxDarkDefectNum);
			MandoInspLog.sNGList += sTemp;
		}
		if (nHotDefectCount > stSpecAllDefectSpec.nMaxHotDefectNum)
		{
			MESCommunication.m_nMesDefectResult[2] = 0;
			sTemp.Format("[Defect Hot] Spec NG: %d(Spec:%d)", nHotDefectCount, stSpecAllDefectSpec.nMaxHotDefectNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect Hot] Spec NG: %d(Spec:%d)", nHotDefectCount, stSpecAllDefectSpec.nMaxHotDefectNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect Hot %d: %d]"), nHotDefectCount, stSpecAllDefectSpec.nMaxHotDefectNum);
			MandoInspLog.sNGList += sTemp;
		}

		if (model.m_LogDefectBrightCluster > stSpecAllDefectSpec.nMaxDarkClusterNum)
		{
			MESCommunication.m_nMesDefectResult[3] = 0;
			sTemp.Format("[Defect BrightCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightCluster, stSpecAllDefectSpec.nMaxDarkClusterNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect BrightCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightCluster, stSpecAllDefectSpec.nMaxDarkClusterNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect BrightCluster %d: %d]"), model.m_LogDefectBrightCluster, stSpecAllDefectSpec.nMaxDarkClusterNum);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_LogDefectDarkCluster >   stSpecAllDefectSpec.nMaxBrightClusterNum)
		{
			MESCommunication.m_nMesDefectResult[4] = 0;
			sTemp.Format("[Defect DarkCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkCluster, stSpecAllDefectSpec.nMaxBrightClusterNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect DarkCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkCluster, stSpecAllDefectSpec.nMaxBrightClusterNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect DarkCluster %d: %d]"), model.m_LogDefectDarkCluster, stSpecAllDefectSpec.nMaxBrightClusterNum);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_LogDefectHotCluster >    stSpecAllDefectSpec.nMaxHotClusterNum)
		{
			MESCommunication.m_nMesDefectResult[5] = 0;
			sTemp.Format("[Defect HotCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectHotCluster, stSpecAllDefectSpec.nMaxHotClusterNum);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect HotCluster] Spec NG: %d(Spec:%d)", model.m_LogDefectHotCluster, stSpecAllDefectSpec.nMaxHotClusterNum);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect HotCluster %d: %d]"), model.m_LogDefectHotCluster, stSpecAllDefectSpec.nMaxHotClusterNum);
			MandoInspLog.sNGList += sTemp;
		}


		//
		if (model.m_LogDefectDarkHorLine > 0)
		{
			MESCommunication.m_nMesDefectResult[6] = 0;
			sTemp.Format("[Defect DarkHLine] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkHorLine, 0);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect DarkHLine] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkHorLine, 0);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect DarkHLine %d: %d]"), model.m_LogDefectDarkHorLine, 0);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_LogDefectDarkVerLine > 0)
		{
			MESCommunication.m_nMesDefectResult[7] = 0;
			sTemp.Format("[Defect DarkVLine] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkVerLine, 0);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect DarkVLine] Spec NG: %d(Spec:%d)", model.m_LogDefectDarkVerLine, 0);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect DarkVLine %d: %d]"), model.m_LogDefectDarkVerLine, 0);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_LogDefectBrightHorLine > 0)
		{
			MESCommunication.m_nMesDefectResult[8] = 0;
			sTemp.Format("[Defect BrightHLine] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightHorLine, 0);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect BrightHLine] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightHorLine, 0);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect BrightHLine %d: %d]"), model.m_LogDefectBrightHorLine, 0);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_LogDefectBrightVerLine > 0)
		{
			MESCommunication.m_nMesDefectResult[9] = 0;
			sTemp.Format("[Defect BrightVLine] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightVerLine, 0);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect BrightVLine] Spec NG: %d(Spec:%d)", model.m_LogDefectBrightVerLine, 0);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;	//DEFECT
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format(_T("[Defect BrightVLine %d: %d]"), model.m_LogDefectBrightVerLine, 0);
			MandoInspLog.sNGList += sTemp;
		}
		//nCFPNcount = pACMISDefectAllDefect->GetDefectColumnFPNCount();
#if (____SENSOR_TYPE == S_ONSEMI)

		//for (int i = 0; i < nCFPNcount; i++)		//���߿� 4���� �߰� �Ǳ����
		//{
		//	if (model.m_LogDefectColumnFPN[i] >  stSpecAllDefectSpec.dDefectThresholdColumnFPN)//stSpecAllDefectSpec.dDefectThresholdColumnFPN)
		//	{
		//		sTemp.Format("[Defect ColumnFpn-%d] Spec NG: %.1lf(Spec:%.1lf)", i, model.m_LogDefectColumnFPN[i], stSpecAllDefectSpec.dDefectThresholdColumnFPN);
		//		//! Main Displayȭ�� Overlay NG List
		//		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("[Defect ColumnFpn-%d] Spec NG: %.1lf(Spec:%.1lf)", i, model.m_LogDefectColumnFPN[i], stSpecAllDefectSpec.dDefectThresholdColumnFPN);
		//		MandoInspLog.iNGCnt++;
		//		MandoInspLog.bInspRes = false;
		//		theApp.MainDlg->putListLog(sTemp);
		//		sTemp.Format(_T("[Defect ColumnFpn-%d %.1lf: %.1lf]"), i, model.m_LogDefectColumnFPN[i], stSpecAllDefectSpec.dDefectThresholdColumnFPN);
		//		MandoInspLog.sNGList += sTemp;
		//	}
		//}

		//if (model.m_LogDefectColumnFPN[1] > stSpecAllDefectSpec.dDefectThresholdColumnFPN)MESCommunication.m_nMesDefectResult[10] = 0;
		//if (model.m_LogDefectColumnFPN[0] > stSpecAllDefectSpec.dDefectThresholdColumnFPN)MESCommunication.m_nMesDefectResult[11] = 0;
		//if (model.m_LogDefectColumnFPN[3] > stSpecAllDefectSpec.dDefectThresholdColumnFPN)MESCommunication.m_nMesDefectResult[12] = 0;
		//if (model.m_LogDefectColumnFPN[2] > stSpecAllDefectSpec.dDefectThresholdColumnFPN)MESCommunication.m_nMesDefectResult[13] = 0;


#elif  (____SENSOR_TYPE == S_OMNIVISION)
		//�ȴϺ����� ColumnFpn �˻� ����
#endif
		CvFont cvfont;
		CvPoint pt;
		int scale = (nWidth > 640 ? 1 : 2);
		double nFontSize = 1 / scale;

		cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);
		sTemp.Format("Defect Mid-Level");
		pt.x = 50;
		pt.y = 50;
		cvPutText(cvImgDefect1, sTemp, pt, &cvfont, CV_YELLOW);
		sTemp.Format("Defect Low-Level");
		pt.x = 50;
		pt.y = 50;
		cvPutText(cvImgDefect2, sTemp, pt, &cvfont, CV_YELLOW);
		int i;
		int count = 0;

		count = pACMISDefectAllDefect->GetDarkDefectCount();
		//std::cout << "[AllDefectOnsemiRCCB] Dark Defect Count=" << count << std::endl;
		sTemp.Format("Dark Defect Count=%d", count);
		pt.x = 50;
		pt.y = 90;
		cvPutText(cvImgDefect1, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TDefectResult* singleDefect = pACMISDefectAllDefect->GetDarkDefectResult(i);		//Dark
			if (singleDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] x=" << singleDefect->ptPos.x << " y=" << singleDefect->ptPos.y << std::endl;
				cvRectangle(cvImgDefect1, cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), CV_RGB(255, 0, 0));
			}
		}
		count = pACMISDefectAllDefect->GetBrightDefectCount();
		//std::cout << "[AllDefectOnsemiRCCB] Bright Defect Count=" << count << std::endl;
		sTemp.Format("Bright Defect Count=%d", count);
		pt.x = 50;
		pt.y = 130;
		cvPutText(cvImgDefect1, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TDefectResult* singleDefect = pACMISDefectAllDefect->GetBrightDefectResult(i);		//Bright
			if (singleDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] x=" << singleDefect->ptPos.x << " y=" << singleDefect->ptPos.y << std::endl;

				cvRectangle(cvImgDefect1, cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), CV_RGB(255, 0, 0));
			}
		}
		count = pACMISDefectAllDefect->GetHotDefectCount();
		//std::cout << "[AllDefectOnsemiRCCB] Hot Defect Count=" << count << std::endl;
		sTemp.Format("Hot Defect Count=%d", count);
		pt.x = 50;
		pt.y = 90;
		cvPutText(cvImgDefect2, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TDefectResult* singleDefect = pACMISDefectAllDefect->GetHotDefectResult(i);		//Hot
			if (singleDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] x=" << singleDefect->ptPos.x << " y=" << singleDefect->ptPos.y << std::endl;

				cvRectangle(cvImgDefect2, cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), cvPoint(singleDefect->ptPos.x, singleDefect->ptPos.y), CV_RGB(255, 0, 0));
			}
		}


		count = pACMISDefectAllDefect->GetDarkDefectClusterCount();
		//std::cout << "[AllDefectOnsemiRCCB] Dark Cluster Count=" << count << std::endl;
		sTemp.Format("Dark Cluster Count=%d", count);
		pt.x = 50;
		pt.y = 170;
		cvPutText(cvImgDefect1, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TBlobResult* clusterDefect = pACMISDefectAllDefect->GetDarkDefectClusterResult(i);

			if (clusterDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] l=" << clusterDefect->rROI.left << " t=" << clusterDefect->rROI.top << " r=" << clusterDefect->rROI.right << " b=" << clusterDefect->rROI.bottom << std::endl;
				cvRectangle(cvImgDefect1, cvPoint(clusterDefect->rROI.left, clusterDefect->rROI.top), cvPoint(clusterDefect->rROI.right, clusterDefect->rROI.bottom), CV_RGB(255, 0, 0));
			}
		}
		count = pACMISDefectAllDefect->GetBrightDefectClusterCount();
		//std::cout << "[AllDefectOnsemiRCCB] Bright Cluster Count=" << count << std::endl;
		sTemp.Format("Bright Cluster Count=%d", count);
		pt.x = 50;
		pt.y = 210;
		cvPutText(cvImgDefect1, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TBlobResult* clusterDefect = pACMISDefectAllDefect->GetBrightDefectClusterResult(i);

			if (clusterDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] l=" << clusterDefect->rROI.left << " t=" << clusterDefect->rROI.top << " r=" << clusterDefect->rROI.right << " b=" << clusterDefect->rROI.bottom << std::endl;

				cvRectangle(cvImgDefect1, cvPoint(clusterDefect->rROI.left, clusterDefect->rROI.top), cvPoint(clusterDefect->rROI.right, clusterDefect->rROI.bottom), CV_RGB(255, 0, 0));
			}
		}

		count = pACMISDefectAllDefect->GetHotDefectClusterCount();
		//std::cout << "[AllDefectOnsemiRCCB] Hot Cluster Count=" << count << std::endl;
		sTemp.Format("Hot Cluster Count=%d", count);
		pt.x = 50;
		pt.y = 130;
		cvPutText(cvImgDefect2, sTemp, pt, &cvfont, CV_BLUE);
		for (i = 0; i < count; i++)
		{
			const TBlobResult* clusterDefect = pACMISDefectAllDefect->GetHotDefectClusterResult(i);

			if (clusterDefect)
			{
				//std::cout << "[AllDefectOnsemiRCCB] l=" << clusterDefect->rROI.left << " t=" << clusterDefect->rROI.top << " r=" << clusterDefect->rROI.right << " b=" << clusterDefect->rROI.bottom << std::endl;

				cvRectangle(cvImgDefect2, cvPoint(clusterDefect->rROI.left, clusterDefect->rROI.top), cvPoint(clusterDefect->rROI.right, clusterDefect->rROI.bottom), CV_RGB(255, 0, 0));
			}
		}
		//
		//
		Jpg_ImageSave(cvImgDefect1, DEFECT_MID_JPG);
		Jpg_ImageSave(cvImgDefect2, DEFECT_LOW_JPG);
		if (bAutoMode == false)
		{
			//�����˻��϶�
			/*cvShowImage("cvImgDefect1", cvImgDefect1);
			cvShowImage("cvImgDefect2", cvImgDefect2);
			cvWaitKey(0);*/
			

		}
	}

	/* Defect���� Log */

	int nDefectKind = stSpecAllDefectSpec.nDefectType & (EDefectKind_HotPixel | EDefectKind_BlackPixel | EDefectKind_WhitePixel);
	if (nDefectKind)
	{
		//pACMISDefectAllDefect->GetLogHeader((EDefectKind)nDefectKind);
		//pACMISDefectAllDefect->GetLogData((EDefectKind)nDefectKind);
		g_SaveLGITLog(m_nUnit, "Defect", pACMISDefectAllDefect->GetLogHeader((EDefectKind)nDefectKind), pACMISDefectAllDefect->GetLogData((EDefectKind)nDefectKind));
	}
	
	/* Cluster���� Log */
	nDefectKind = stSpecAllDefectSpec.nDefectType & (EDefectKind_HotClusterColor | EDefectKind_BlackClusterColor | EDefectKind_WhiteClusterColor);
	if (nDefectKind)
	{
		//pACMISDefectAllDefect->GetLogHeader((EDefectKind)nDefectKind);
		//pACMISDefectAllDefect->GetLogData((EDefectKind)nDefectKind);
		g_SaveLGITLog(m_nUnit, "Defect_Cluster", pACMISDefectAllDefect->GetLogHeader((EDefectKind)nDefectKind), pACMISDefectAllDefect->GetLogData((EDefectKind)nDefectKind));
	}
	
    //g_SaveLGITLog(m_nUnit, "Defect", pACMISDefectAllDefect->GetLogHeader(), pACMISDefectAllDefect->GetLogData());

	cvReleaseImage(&cvImgDefect1);
	cvReleaseImage(&cvImgDefect2);
	return bRet;
}



//-----------------------------------------------------------------------------
//
//	UNIFORMITY �˻�
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_Uniformity(BYTE* img, bool bAutoMode, bool bUse8BitOnly)
{
	
	return false;
}



//-----------------------------------------------------------------------------
//
//	STAIN �˻�(BY PRI)
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_OpenStain(IplImage* img, bool bAutoMode)
{
	bool bRes = false;
	return bRes;

}


CString CPRIFunc_Insp::SetDir_Check(CString sPath)
{
	CString sRtn = _T(""), FolderName = _T("");
	CFileFind finder;
	BOOL IsFind;

	SYSTEMTIME time;
	::GetLocalTime(&time);

	FolderName.Format(_T("%s\\%04d%02d"), (TCHAR*)(LPCTSTR)sPath, time.wYear, time.wMonth);
	IsFind = finder.FindFile(FolderName);
	if (!IsFind)
	{
		CreateDirectory(FolderName, NULL);
	}

	sRtn.Format(_T("%s\\%04d%02d\\%02d"), (TCHAR*)(LPCTSTR)sPath, time.wYear, time.wMonth, time.wDay);
	IsFind = finder.FindFile(sRtn);
	if (!IsFind)
	{
		CreateDirectory(sRtn, NULL);
	}

	sRtn.Format(_T("%s\\%04d%02d\\%02d\\"), (TCHAR*)(LPCTSTR)sPath, time.wYear, time.wMonth, time.wDay);

	return sRtn;
}

//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
bool CPRIFunc_Insp::func_Insp_OpenStainLGIT(unsigned char* pImgBuff, bool bAutoMode/* = false*/)
{
	TDATASPEC tDataSpec;

	// Spec information
	TStainSpec tStainSpec;

	//GetImageData(MODEL_NIO, tDataSpec, stImgInfo);

	tDataSpec.eDataFormat = DATAFORMAT_BAYER_12BIT;
	tDataSpec.eOutMode = OUTMODE_BAYER_BGGR;
	tDataSpec.eSensorType = SENSORTYPE_RCCC;
	tDataSpec.nBlackLevel = 64;

	// Image buffers
	//std::vector<BYTE> vFrameBuffer(stImgInfo.nSensorHeight * stImgInfo.nSensorWidth * 2);

	memset(&tStainSpec, 0x00, sizeof(TStainSpec));

	//----------------------------------------------------------------------
	// Spec - BlackSpot
	//----------------------------------------------------------------------
	tStainSpec.stSpecBlackSpot.nBlockWidth = 32;
	tStainSpec.stSpecBlackSpot.nBlockHeight = 32;
	tStainSpec.stSpecBlackSpot.nClusterSize = 5;
	tStainSpec.stSpecBlackSpot.nDefectInCluster = 5;
	tStainSpec.stSpecBlackSpot.dDefectRatio = 0.45000;
	tStainSpec.stSpecBlackSpot.nMaxSingleDefectNum = 100000;	// noise image
	tStainSpec.stSpecBlackSpot.tCircleSpec.bEnableCircle = false;
	tStainSpec.stSpecBlackSpot.tCircleSpec.nPosOffsetX = 5;
	tStainSpec.stSpecBlackSpot.tCircleSpec.nPosOffsetY = 5;
	tStainSpec.stSpecBlackSpot.tCircleSpec.dRadiusRatioX = 0.45;
	tStainSpec.stSpecBlackSpot.tCircleSpec.dRadiusRatioY = 0.45;

	//----------------------------------------------------------------------
	// Spec - LCB
	//----------------------------------------------------------------------
	// !
	tStainSpec.stSpecLCB.dCenterThreshold = model.m_dStainLsbCenter;//29.0;
	// !
	tStainSpec.stSpecLCB.dCornerThreshold = model.m_dStainLsbCorner;//65.0;
	// !
	tStainSpec.stSpecLCB.dEdgeThreshold = model.m_dStainLsbEdge;//80.0;
	tStainSpec.stSpecLCB.nMaxSingleDefectNum = 50000;
	tStainSpec.stSpecLCB.nMinDefectWidthHeight = 10;
	tStainSpec.stSpecLCB.tCircleSpec.bEnableCircle = false;
	tStainSpec.stSpecLCB.tCircleSpec.nPosOffsetX = 5;
	tStainSpec.stSpecLCB.tCircleSpec.nPosOffsetY = 5;
	tStainSpec.stSpecLCB.tCircleSpec.dRadiusRatioX = 0.45;
	tStainSpec.stSpecLCB.tCircleSpec.dRadiusRatioY = 0.45;

	//----------------------------------------------------------------------
	// Spec - Ymean
	//----------------------------------------------------------------------
	tStainSpec.stSpecYmean.nDefectBlockSize = 32;
	tStainSpec.stSpecYmean.nEdgeSize = 100;
	// !
	tStainSpec.stSpecYmean.fCenterThreshold = model.m_dStainYMeanCenter;//3.0;
	// !
	tStainSpec.stSpecYmean.fEdgeThreshold = model.m_dStainYMeanEdge;//6.0;
	// !
	tStainSpec.stSpecYmean.fCornerThreshold = model.m_dStainYMeanCorner;//6.0;
	tStainSpec.stSpecYmean.nLscBlockSize = 128;
	tStainSpec.stSpecYmean.tCircleSpec.bEnableCircle = false;
	tStainSpec.stSpecYmean.tCircleSpec.nPosOffsetX = 5;
	tStainSpec.stSpecYmean.tCircleSpec.nPosOffsetY = 5;
	tStainSpec.stSpecYmean.tCircleSpec.dRadiusRatioX = 0.45;
	tStainSpec.stSpecYmean.tCircleSpec.dRadiusRatioY = 0.45;

	//----------------------------------------------------------------------
	// Inspect Main Camera
	//----------------------------------------------------------------------

	// Load image
	//LoadImageData(vFrameBuffer.data(), _IMG_NIO_Stain_MAIN_01_raw);

	// Inspect
	if(this->InspectStain(tDataSpec, tStainSpec, pImgBuff, gMIUDevice.nWidth, gMIUDevice.nHeight, true, true, true, bAutoMode) == 0)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
int CPRIFunc_Insp::InspectStain(TDATASPEC &tDataSpec, TStainSpec &tStainSpec, unsigned char *pBuffer, int nWidth, int nHeight, bool bEnableBlackSpot/* = true*/, bool bEnableLCB/* = true*/, bool bEnableRYUmean/* = true*/, bool bAutoMode/* = false*/)
{
	
	return 1;
}

//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
int CPRIFunc_Insp::InspectBlackSpotContrast(const BYTE* pBuffer, int nWidth, int nHeight, TBlackSpotContrast& _Spec, EDATAFORMAT dataFormat, EOUTMODE outMode, ESENSORTYPE sensorType, int nBlackLevel, IplImage *cvImgBlackSpot)
{
	
	return 1;
}
//-----------------------------------------------------------------------------
//
//	�̹�(stain) �˻�
//
//-----------------------------------------------------------------------------

bool CPRIFunc_Insp::func_Insp_Stain(BYTE* rawImg)
{
    int nBlackLevel = 0;
    int nWidth = gMIUDevice.nWidth;
    int nHeight = gMIUDevice.nHeight;
    TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
    //


	//ACMISSoftISP::xMakeBMP(rawImg, (byte*)MIU.m_pFrameBMPBuffer, gMIUDevice.nWidth, gMIUDevice.nHeight, tDataSpec);

	//BlackSpotInsp((BYTE*)rawImg, nWidth, nHeight, tDataSpec);// , cvImg);
    LCBInsp((BYTE*)rawImg, nWidth, nHeight, tDataSpec);//, cvImg);
    Blemish_YmeanInsp((BYTE*)rawImg, nWidth, nHeight, tDataSpec);//, cvImg);
	FDFInsp((BYTE*)rawImg, false);
    return true;
}

int CPRIFunc_Insp::InspectStain(BYTE* stainImg, bool bAutoMode)
{
    //SYSTEMTIME time;
    //::GetLocalTime(&time);

    //CFileFind finder;
    //BOOL IsFind;
    //CString FolderName;
    //CString rawName;
    ////stainImg = Raw �̹���
    //FolderName.Format("%s\\%04d%02d\\%02d", IMG_DIR, time.wYear, time.wMonth, time.wDay);

    //IsFind = finder.FindFile(FolderName);
    //if (!IsFind)
    //{
    //    FolderName.Format("%s", BASE_DIR);
    //    IsFind = finder.FindFile(FolderName);
    //    if (!IsFind)
    //    {
    //        CreateDirectory(FolderName, NULL);
    //    }

    //    FolderName.Format("%s", IMG_DIR);
    //    IsFind = finder.FindFile(FolderName);
    //    if (!IsFind)
    //    {
    //        CreateDirectory(FolderName, NULL);
    //    }

    //    FolderName.Format("%s\\%04d", IMG_DIR, time.wYear, time.wYear);
    //    CreateDirectory(FolderName, NULL);

    //    FolderName.Format("%s\\%04d%02d", IMG_DIR, time.wYear, time.wMonth);
    //    CreateDirectory(FolderName, NULL);

    //    FolderName.Format("%s\\%04d%02d\\%02d", IMG_DIR, time.wYear, time.wMonth, time.wDay);
    //    CreateDirectory(FolderName, NULL);
    //}
    //SYSTEMTIME SysTime;
    //::GetLocalTime(&SysTime);

    //finder.Close();

    //CString		fileName;
    //fileName.Format("%s\\%s_Stain_%04d%02d%02d%02d%02d%02d.bmp", FolderName, g_clMandoInspLog[m_nUnit].m_sBarcodeID, SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    //rawName.Format("%s\\%s_Stain_%04d%02d%02d%02d%02d%02d.raw", FolderName, g_clMandoInspLog[m_nUnit].m_sBarcodeID, SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
    ////if(!bAutoMode && false)
    //{
    //    CFile savefile;
    //    CFileException  e;

    //    if (!savefile.Open(rawName, CFile::modeCreate | CFile::modeWrite, &e))
    //    {
    //        AfxMessageBox("File open fail:" + e.m_cause);
    //    }
    //    savefile.Write(stainImg, g_clLaonGrabberWrapper[m_nUnit].m_stMIUDevice.nFrameImageSize);
    //    savefile.Close();
    //}
    //CString sTemp = "";
    ////========================================================================================
    //char	sIniPath[1000];
    //wsprintf(sIniPath, "%s\\stainSpec.ini", DATA_DIR, g_clSysData.m_szModelName); //D:\SETUP\XPENG_SIDE\Data\stainSpec.ini
    //CMapData spec;
    //GetIni(sIniPath, spec);//bEnableCircle = wide = 1, side = 0
    //                       //=========================================================================================
    //int nBlackLevel = 64; // Eliminate black level of sensor when you grab a frame.
    //IplImage *cvImageObject = cvCreateImage(cvSize(g_clLaonGrabberWrapper[m_nUnit].m_stMIUDevice.nWidth, g_clLaonGrabberWrapper[m_nUnit].m_stMIUDevice.nHeight), 8, 3); // bmp for display
    //cvImageObject->imageData = (char*)g_clLaonGrabberWrapper[m_nUnit].m_pFrameBMPBuffer;

    //// 
    //int nResult = R_RESULT_PASS;
    //int nResultBlackSpot = 0;
    //int nResultLCB = 0;
    //int nResultRUYmean = 0;
    //int totalStainNum = 0;
    //g_clVision.ClearOverlay(m_nUnit);

    //if (nResult == R_RESULT_PASS)
    //{
    //   // totalStainNum += LCBInsp(spec, cvImageObject, stainImg);
    //}
    ////�ڡڡڡ�---------------------------------------------------------------------- Spec - Ymean
    //if (nResult == R_RESULT_PASS)
    //{
    //   // totalStainNum += Blemish_YmeanInsp(spec, cvImageObject, stainImg);
    //}
    //cvSaveImage("D:\\StainImg.bmp", cvImageObject);
    return 1;
}
int CPRIFunc_Insp::FDFInsp(BYTE* img, bool bAutoMode)
{
	int nResult = R_RESULT_PASS;

	int nBlackLevel = 0;
	//CString szLog = "";
	TCHAR szLog[SIZE_OF_1K];
	TFDFSpec stSpecFDF;
	CString sTemp;
	memset(&stSpecFDF, 0x00, sizeof(TFDFSpec));
	int specCount = 0;
	int nWidth = gMIUDevice.nWidth;
	int nHeight = gMIUDevice.nHeight;

	TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	//TDATASPEC& tDataSpec = gMIUDevice.dTDATASPEC_n;
	//ACMISSoftISP::xMakeBMP(img, (byte*)MIU.m_pTestBuffer,nWidth, nHeight, tDataSpec);
	//Mat bmpImg = Mat(nHeight, nWidth, CV_8UC3);

	//bmpImg.data = MIU.m_pTestBuffer;
	//----------------------------------------------------------------------
	// Spec - FDF
	//----------------------------------------------------------------------
	stSpecFDF.dCenterThreshold = model.m_FDFSpec[specCount++];//10.75;
	stSpecFDF.dEdgeThreshold = model.m_FDFSpec[specCount++];//15.0;
	stSpecFDF.dCornerThreshold = model.m_FDFSpec[specCount++];//15.0;
	stSpecFDF.nMedianFilterWidth = (int)model.m_FDFSpec[specCount++];//0;
	stSpecFDF.nMedianFilterHeight = model.m_FDFSpec[specCount++];//1000;
	stSpecFDF.nWidthScaleRatio = model.m_FDFSpec[specCount++];//70.0;
	stSpecFDF.nHeightScaleRatio = model.m_FDFSpec[specCount++];//4.2;
	stSpecFDF.nMinDefectWidthHeight = (int)model.m_FDFSpec[specCount++];//64;
	stSpecFDF.nWindowSize = (int)model.m_FDFSpec[specCount++];// 100;
	stSpecFDF.nEdgeSize = (int)model.m_FDFSpec[specCount++];
	stSpecFDF.bEnableChannel = (bool)model.m_FDFSpec[specCount++];
	stSpecFDF.nMaxSingleDefectNum = model.m_FDFSpec[specCount++];//15;

	stSpecFDF.tCircleSpec.bEnableCircle = model.m_FDFSpec[specCount++];//false;
	stSpecFDF.tCircleSpec.nPosOffsetX = (int)model.m_FDFSpec[specCount++];//5;
	stSpecFDF.tCircleSpec.nPosOffsetY = (int)model.m_FDFSpec[specCount++];//5;
	stSpecFDF.tCircleSpec.dRadiusRatioX = model.m_FDFSpec[specCount++];//0.45;
	stSpecFDF.tCircleSpec.dRadiusRatioY = model.m_FDFSpec[specCount++];//0.45;
	stSpecFDF.tCircleSpec.dThresholdRatio = model.m_FDFSpec[specCount++];//0.5;
	stSpecFDF.tCircleSpec.dROIRange = model.m_FDFSpec[specCount++];//0.5;
	stSpecFDF.tCircleSpec.nUsedFixedCircle = (int)model.m_FDFSpec[specCount++];//0;

	stSpecFDF.tMultiCircleSpec.bEnableMultiCircle = (int)model.m_FDFSpec[specCount++];//false;
	stSpecFDF.tMultiCircleSpec.dZoneSizeRatio[0] = model.m_FDFSpec[specCount++];//0.2;
	stSpecFDF.tMultiCircleSpec.dZoneSizeRatio[1] = model.m_FDFSpec[specCount++];//0.4;
	stSpecFDF.tMultiCircleSpec.dZoneSizeRatio[2] = model.m_FDFSpec[specCount++];//0.52;
	stSpecFDF.tMultiCircleSpec.dThreshold[0] = model.m_FDFSpec[specCount++];//0.45;
	stSpecFDF.tMultiCircleSpec.dThreshold[1] = model.m_FDFSpec[specCount++];//0.45;
	stSpecFDF.tMultiCircleSpec.dThreshold[2] = model.m_FDFSpec[specCount++];//0.8;
	stSpecFDF.tMultiCircleSpec.nBlobSize[0] = model.m_FDFSpec[specCount++];
	stSpecFDF.tMultiCircleSpec.nBlobSize[1] = model.m_FDFSpec[specCount++];
	stSpecFDF.tMultiCircleSpec.nBlobSize[2] = model.m_FDFSpec[specCount++];

	//std::shared_ptr<CACMISImageStainRU_YmeanCommon> pInspectBlemish_Ymean = std::make_shared<CACMISImageStainRU_YmeanCommon>();//delete,x
	std::shared_ptr<CACMISImageFastDifferenceFiltering> m_pFDF = std::make_shared<CACMISImageFastDifferenceFiltering>();

	m_pFDF->Inspect((BYTE*)img, nWidth, nHeight, stSpecFDF, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, nBlackLevel, 0, 0);
	int FdfBlobCount = 0;
	FdfBlobCount = m_pFDF->GetDefectBlobCount();
	MESCommunication.m_nMesBlemish[2] = FdfBlobCount;
	//CvFont cvfont;
	CvPoint pt;
	int scale = (nWidth > 640 ? 1 : 2);
	double nFontSize = 1 / scale;
	//CString sTemp;
	//cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);
	//sTemp.Format("Ymean BlobCount : %d", FdfBlobCount);
	char strTmp[1024];
	sprintf_s(strTmp, "FDF BlobCount : %d", FdfBlobCount);
	pt.x = 50;
	pt.y = 110;
	//cvPutText(bmpImg, sTemp, pt, &cvfont, CV_BLUE);
	//putText(bmpImg, strTmp, pt, 0, 1.5, Scalar(10, 10, 10), 3);


	double dFdfSpecMin = 0.0;
	double dFdfSpecMax = 0.0;

	//dFdfSpecMin = (_ttof(EEpromVerifyData.vMinData[25]));
	//dFdfSpecMax = (_ttof(EEpromVerifyData.vMaxData[25]));
	if (FdfBlobCount > 0)//dFdfSpecMax)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[FDF] Spec Out %d [0]"), FdfBlobCount);
		theApp.MainDlg->putListLog(szLog);
		nResult = R_FAIL_STAIN;// R_FAIL_BLACK_SPOT;
							   //�̹��ҷ�
		sTemp.Format("FDF fail Spec NG: %d (Spec:%d)", m_pFDF->GetDefectBlobCount(), 0);
		//! Main Displayȭ�� Overlay NG List
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("FDF fail: %d (Spec:%d)", m_pFDF->GetDefectBlobCount(), 0);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;//LCB
									  //bResult = false;
		theApp.MainDlg->putListLog(sTemp);
		MandoInspLog.sNGList += sTemp;

		for (int i = 0; i < FdfBlobCount; i++)
		{
			const RECT* rt = m_pFDF->GetDefectBlobRect(i);
			//cvRectangle(bmpImg, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(138, 43, 226), 2);
			//rectangle(bmpImg, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(138, 43, 226), 2);
			if (bAutoMode == false)
			{
				//g_clVision.DrawMOverlayBox(m_nUnit, m_nUnit, rt->left, rt->top, rt->right, rt->bottom, M_COLOR_BLUE, 1, FALSE);
				vision.boxlist[CCD].addList(rt, PS_SOLID, M_COLOR_BLUE);
				//vision.boxlist[CCD].addList(rt, PS_SOLID, M_COLOR_CYAN);
			}
		}

	}
	else
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("	[FDF] Spec In %.1lf [%.1lf]"), FdfBlobCount, dFdfSpecMax);
		theApp.MainDlg->putListLog(szLog);
	}

	//szLog.Format("[FDF] Version %s", m_pFDF->GetVersion());
	_stprintf_s(szLog, SIZE_OF_1K, _T("[FDF] Version %s"), m_pFDF->GetVersion());
	theApp.MainDlg->putListLog(szLog);
	//szLog.Format("[FDF] GetDefectBlobCount %d", FdfBlobCount);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[FDF] GetDefectBlobCount  %d"), FdfBlobCount);
	theApp.MainDlg->putListLog(szLog);

	//g_clMandoInspLog[m_nUnit].m_LogBlemishFPF = FdfBlobCount;

	//g_clMesCommunication[m_nUnit].m_nMesBlemish[1] = g_clMandoInspLog[m_nUnit].m_LogBlemishFPF;
	//if (g_clMesCommunication[m_nUnit].m_nMesBlemish[1] == 0)
	//{
	//	g_clMesCommunication[m_nUnit].m_nMesBlemishResult[1] = 1;
	//}
	//else
	//{
	//	g_clTaskWork[m_nUnit].m_bOutputCheck[3] = false;	//STAIN FDF
	//	g_clMesCommunication[m_nUnit].m_nMesBlemishResult[1] = 0;
	//	g_clMesCommunication[m_nUnit].m_nMesFinalResult = 0;
	//}
	if (stSpecFDF.tCircleSpec.bEnableCircle || stSpecFDF.tMultiCircleSpec.bEnableMultiCircle)
	{
		int ocx = 0, ocy = 0, radx = 0, rady = 0;
		const TDefectResult* pCircleMaxResult = m_pFDF->GetMaxDefectResult(EIMAGEREGION_CIRCLE);


		ocx = (int)pCircleMaxResult->dContrastMaxR;
		ocy = (int)pCircleMaxResult->dContrastMaxGb;
		radx = (int)pCircleMaxResult->dContrastMaxGr* stSpecFDF.tCircleSpec.dRadiusRatioX;
		rady = (int)pCircleMaxResult->dContrastMaxB *stSpecFDF.tCircleSpec.dRadiusRatioY;

		if (stSpecFDF.tCircleSpec.bEnableCircle)
		{
			//std::cout << "[RUYMean] ocx=" << ocx << "ocy = " << ocy << "radx = " << radx << "rady = " << rady << std::endl;
			//cvEllipse(bmpImg, cvPoint(ocx, ocy), cvSize(radx, rady), 0, 0, 360, CV_RGB(0, 255, 255));
			//ellipse(bmpImg, cvPoint(ocx, ocy), cvSize(radx, rady), 0, 0, 360, CV_RGB(0, 255, 255));
			//g_clVision.DrawMOverlayCircle(m_nUnit, ocx - radx, ocy - rady, ocx + radx, ocy + rady, M_COLOR_MAGENTA, 1, FALSE);
			vision.ellipselist[CCD].addList(CPoint(ocx - radx, ocy - rady), CPoint(ocx + radx, ocy + rady), PS_SOLID, M_COLOR_CYAN);
		}
	}
	int nCircleCount = m_pFDF->GetDefectCount(EIMAGEREGION_CIRCLE);

	MESCommunication.m_nMesBlemishMaxDefect[0] = m_pFDF->GetMaxDefectResult(EIMAGEREGION_CENTER)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[1] = m_pFDF->GetMaxDefectResult(EIMAGEREGION_EDGE)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[2] = m_pFDF->GetMaxDefectResult(EIMAGEREGION_CORNER)->dValue;

	_stprintf_s(szLog, SIZE_OF_1K, _T("[FDF] Max Value (Center) %.6lf"), MESCommunication.m_nMesBlemishMaxDefect[0]);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[FDF] Max Value (Edge) %.6lf"), MESCommunication.m_nMesBlemishMaxDefect[1]);
	theApp.MainDlg->putListLog(szLog);
	_stprintf_s(szLog, SIZE_OF_1K, _T("[FDF] Max Value (Corner) %.6lf"), MESCommunication.m_nMesBlemishMaxDefect[2]);
	theApp.MainDlg->putListLog(szLog);


	if (true)//g_clModelData[m_nUnit].m_nNgImageSaveUse == 1)
	{
		if (FdfBlobCount > 0)
		{
			//g_clLaonGrabberWrapper[m_nUnit].JpgMatSave(bmpImg, 0);
		}
	}
	else
	{

		//g_clLaonGrabberWrapper[m_nUnit].JpgMatSave(bmpImg, 0);
	}
	//cvReleaseImage(&bmpImg);
	g_SaveLGITLog(m_nUnit, "FDF", m_pFDF->GetLogHeader(), m_pFDF->GetLogData());
	return FdfBlobCount;
}

int CPRIFunc_Insp::BlackSpotInsp(BYTE* bsimg, int nWidth, int nHeight, TDATASPEC& tDataSpec)//, IplImage* cvImgBlackSpot)
{
#if 0
    CString szLog;
    int nBlackLevel = 0;
    TBlackSpotContrastN tBlackSpotSpec;
    TCircleSpecN tStainSpec;
	int stainCount = 0;
    int specCount = 0;
	//
	IplImage *cvImgBlackSpot = cvCreateImage(cvSize(nWidth, nHeight), 8, 3); // bmp for display
	cvImgBlackSpot->imageData = (char*)MIU.m_pFrameBMPBuffer;
	//
    tBlackSpotSpec.nBlockWidth = model.m_BlemishSpec[specCount++];//0;32;
    tBlackSpotSpec.nBlockHeight = (int)model.m_BlemishSpec[specCount++];//32;
    tBlackSpotSpec.nClusterSize = (int)model.m_BlemishSpec[specCount++];//5;
    tBlackSpotSpec.nDefectInCluster = (int)model.m_BlemishSpec[specCount++];//5;
    tBlackSpotSpec.dDefectRatio = model.m_BlemishSpec[specCount++];//0.45000;
    tBlackSpotSpec.nMaxSingleDefectNum = 100000;// (int)model.m_BlemishSpec[specCount++];//100000;	// noise image
    //
    tBlackSpotSpec.tCircleSpec.bEnableCircle = (bool)model.m_BlemishSpec[specCount++];//false;
    tBlackSpotSpec.tCircleSpec.nPosOffsetX = (int)model.m_BlemishSpec[specCount++];//5;
    tBlackSpotSpec.tCircleSpec.nPosOffsetY = (int)model.m_BlemishSpec[specCount++];//5;
    tBlackSpotSpec.tCircleSpec.dRadiusRatioX = model.m_BlemishSpec[specCount++];//0.45;
    tBlackSpotSpec.tCircleSpec.dRadiusRatioY = model.m_BlemishSpec[specCount++];//0.45;
    tBlackSpotSpec.tCircleSpec.dThresholdRatio = model.m_BlemishSpec[specCount++];//0.5;
    tBlackSpotSpec.tCircleSpec.dROIRange = model.m_BlemishSpec[specCount++];//0.5;
    tBlackSpotSpec.tCircleSpec.nUsedFixedCircle = (int)model.m_BlemishSpec[specCount++];//0;
    //
    tBlackSpotSpec.tMultiCircleSpec.bEnableMultiCircle = (int)model.m_BlemishSpec[specCount++];//0;
    tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[0] = model.m_BlemishSpec[specCount++];//0.2;
    tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[1] = model.m_BlemishSpec[specCount++];//0.4;
    tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[2] = model.m_BlemishSpec[specCount++];//0.52;
    tBlackSpotSpec.tMultiCircleSpec.dThreshold[0] = model.m_BlemishSpec[specCount++];//0.45;
    tBlackSpotSpec.tMultiCircleSpec.dThreshold[1] = model.m_BlemishSpec[specCount++];//0.45;
    tBlackSpotSpec.tMultiCircleSpec.dThreshold[2] = model.m_BlemishSpec[specCount++];//0.8;
    tBlackSpotSpec.tMultiCircleSpec.nBlobSize[0] = 0;//0;
    tBlackSpotSpec.tMultiCircleSpec.nBlobSize[1] = 0;//0;
    tBlackSpotSpec.tMultiCircleSpec.nBlobSize[2] = 0;//0;

    int nResult = R_RESULT_PASS;
    std::shared_ptr<CACMISImageBlackSpotContrastCommon> pInspectBlackSpot = std::make_shared<CACMISImageBlackSpotContrastCommon>();
	pInspectBlackSpot->MakeHistEQImage((const BYTE*)bsimg, (BYTE*)cvImgBlackSpot->imageData, gMIUDevice.nWidth, gMIUDevice.nHeight, gMIUDevice.dTDATASPEC_n, 1);
    // inspection
#if  (____SENSOR_TYPE == S_OMNIVISION)
	pInspectBlackSpot->SetInspectPosOffset(0, 2, 0, 0);	//220630
#endif
    int nDefectCount = pInspectBlackSpot->Inspect((BYTE*)bsimg, nWidth, nHeight, tBlackSpotSpec, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, nBlackLevel);

	stainCount = pInspectBlackSpot->GetDefectBlobCount();
    szLog.Format("[BlackSpot] BlackSpot %s", pInspectBlackSpot->GetVersion());
	theApp.MainDlg->putListLog(szLog);
    szLog.Format("[BlackSpot] DefectCount %d", nDefectCount);
	theApp.MainDlg->putListLog(szLog);
    szLog.Format("[BlackSpot] SingleDefectCount %d", pInspectBlackSpot->GetSingleDefectCount());
	theApp.MainDlg->putListLog(szLog);
    szLog.Format("[BlackSpot] BlobCount %d", stainCount);
	theApp.MainDlg->putListLog(szLog);
   
    CString sTemp;
	CvFont cvfont;
	CvPoint pt;
	int scale = (nWidth > 640 ? 1 : 2);
	double nFontSize = 1 / scale;

	cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);
	sTemp.Format("BlackSpot BlobCount : %d", stainCount);
	pt.x = 50;
	pt.y = 50;
	cvPutText(cvImgBlackSpot, sTemp, pt, &cvfont, CV_BLUE);
	//
    if (pInspectBlackSpot->GetSingleDefectCount() > tBlackSpotSpec.nMaxSingleDefectNum) 
    {
        nResult = R_FAIL_NOISE;
    }
    else if (stainCount > 0)
    {
        nResult = R_FAIL_BLACK_SPOT;
		sTemp.Format("[STAIN BLACK SPOT] Spec NG: %df(Spec:%d)", stainCount, 0);
		//! Main Displayȭ�� Overlay NG List
		//
        sTemp.Format("BlackSpot fail Spec NG: %d (Spec:%d)", pInspectBlackSpot->GetDefectBlobCount(), 0);
        //! Main Displayȭ�� Overlay NG List
        MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("BlackSpot fail: %d (Spec:%d)", pInspectBlackSpot->GetDefectBlobCount(), 0);
        MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;	//BLACKSPOT
        //bResult = false;
        theApp.MainDlg->putListLog(sTemp);
        MandoInspLog.sNGList += sTemp;
    }
    model.m_LogBlemishBlackSpot = pInspectBlackSpot->GetDefectBlobCount();
	MESCommunication.m_nMesBlemish[0] = model.m_LogBlemishBlackSpot;
    if (nResult)
    {
        for (int i = 0; i < (int)pInspectBlackSpot->GetDefectBlobCount(); i++)
        {
            const RECT* rt = pInspectBlackSpot->GetDefectBlobRect(i);
            cvRectangle(cvImgBlackSpot, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(255, 0, 0));
			vision.boxlist[CCD].addList(rt, PS_SOLID, M_COLOR_BLUE);
        }

        const TDefectResult* pMaxResult = pInspectBlackSpot->GetMaxDefectResult();

        int crossSize = 20;
        cvLine(cvImgBlackSpot, cvPoint(pMaxResult->ptPos.x - crossSize, pMaxResult->ptPos.y - crossSize),
            cvPoint(pMaxResult->ptPos.x + crossSize, pMaxResult->ptPos.y + crossSize), CV_RGB(255, 0, 0));
        cvLine(cvImgBlackSpot, cvPoint(pMaxResult->ptPos.x + crossSize, pMaxResult->ptPos.y - crossSize),
            cvPoint(pMaxResult->ptPos.x - crossSize, pMaxResult->ptPos.y + crossSize), CV_RGB(255, 0, 0));

        if (tBlackSpotSpec.tCircleSpec.bEnableCircle)
        {
            int ocx = (int)pMaxResult->dContrastMaxR;
            int ocy = (int)pMaxResult->dContrastMaxGb;
            int radx = (int)pMaxResult->dContrastMaxGr;
            int rady = (int)pMaxResult->dContrastMaxB;

            int nZone_X = (int)(radx * tBlackSpotSpec.tCircleSpec.dRadiusRatioX);
            int nZone_Y = (int)(rady * tBlackSpotSpec.tCircleSpec.dRadiusRatioY);


            std::cout << "[BlackSpot] ocx=" << ocx << "ocy = " << ocy << "radx = " << nZone_X << "rady = " << nZone_Y << std::endl;
            std::cout << "[BlackSpot] posx=" << pMaxResult->ptPos.x << "poxy = " << pMaxResult->ptPos.y << std::endl;

            cvEllipse(cvImgBlackSpot, cvPoint(ocx, ocy), cvSize(nZone_X, nZone_Y), 0, 0, 360, CV_RGB(0, 255, 255));
        }

        if (tBlackSpotSpec.tMultiCircleSpec.bEnableMultiCircle)
        {
            double dRadYRatio = 1.0;
            if (tBlackSpotSpec.tCircleSpec.nUsedFixedCircle)
            {
                dRadYRatio = tBlackSpotSpec.tCircleSpec.dRadiusRatioY / tBlackSpotSpec.tCircleSpec.dRadiusRatioX;
            }

            int ocx = (int)pMaxResult->dContrastMaxR;
            int ocy = (int)pMaxResult->dContrastMaxGb;
            int radx = (int)pMaxResult->dContrastMaxGr;
            int rady = (int)pMaxResult->dContrastMaxB;

            int nZoneA_X = (int)(radx * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[0]);
            int nZoneA_Y = (int)(rady * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[0] * dRadYRatio);
            int nZoneB_X = (int)(radx * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[1]);
            int nZoneB_Y = (int)(rady * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[1] * dRadYRatio);
            int nZoneC_X = (int)(radx * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[2]);
            int nZoneC_Y = (int)(rady * tBlackSpotSpec.tMultiCircleSpec.dZoneSizeRatio[2] * dRadYRatio);

            cvEllipse(cvImgBlackSpot, cvPoint(ocx, ocy), cvSize(nZoneA_X, nZoneA_Y), 0, 0, 360, CV_RGB(0, 255, 255));
            cvEllipse(cvImgBlackSpot, cvPoint(ocx, ocy), cvSize(nZoneB_X, nZoneB_Y), 0, 0, 360, CV_RGB(0, 255, 255));
            cvEllipse(cvImgBlackSpot, cvPoint(ocx, ocy), cvSize(nZoneC_X, nZoneC_Y), 0, 0, 360, CV_RGB(0, 255, 255));

            printf("[BlackSpot Multi Circle] ocX=%d, ocY=%d , dRadYRatio=%f, radZoneA(%d,%d), radZoneB(%d,%d), radZoneC(%d,%d)", ocx, ocy, dRadYRatio, nZoneA_X, nZoneA_Y, nZoneB_X, nZoneB_Y, nZoneC_X, nZoneC_Y);

        }

        if ((tBlackSpotSpec.tMultiCircleSpec.bEnableMultiCircle || tBlackSpotSpec.tCircleSpec.bEnableCircle) && tBlackSpotSpec.tCircleSpec.nUsedFixedCircle)
        {
            RECT rtRefEdge;
            RECT rtCenter;
            //oc threshold test roi - top
            rtRefEdge.left = max(nWidth / 2 - 100 / 2, 0);
            rtRefEdge.top = (LONG)(0.1 * nHeight / 2);
            rtRefEdge.right = min(rtRefEdge.left + 100 - 1, (LONG)nWidth - 1);
            rtRefEdge.bottom = min(rtRefEdge.top + 100 - 1, (LONG)nHeight - 1);

            //oc threshold test roi - center
            rtCenter.left = rtRefEdge.left;
            rtCenter.top = max(nHeight / 2 - 100 / 2, 0);
            rtCenter.right = rtRefEdge.right;
            rtCenter.bottom = min(nHeight / 2 + 100 / 2 - 1, nHeight - 1);

            cvRectangle(cvImgBlackSpot, cvPoint(rtRefEdge.left, rtRefEdge.top), cvPoint(rtRefEdge.right, rtRefEdge.bottom), CV_RGB(255, 86, 12));
            cvRectangle(cvImgBlackSpot, cvPoint(rtCenter.left, rtCenter.top), cvPoint(rtCenter.right, rtCenter.bottom), CV_RGB(255, 86, 12));
        }
    }
	Jpg_ImageSave(cvImgBlackSpot, BLACKSPOT_JPG);
    g_SaveLGITLog(m_nUnit, "BlackSpot", pInspectBlackSpot->GetLogHeader(), pInspectBlackSpot->GetLogData());

	cvReleaseImage(&cvImgBlackSpot);
    //cvSaveImage("D:\\cvImgBlackSpot.bmp", cvImgBlackSpot);

#endif
    return true;
}

int CPRIFunc_Insp::LCBInsp(BYTE* img, int nWidth, int nHeight, TDATASPEC& tDataSpec)//, IplImage* bmpImg)
{
    int nResult = R_RESULT_PASS;
    int nResultLCB = 0;
    int nBlackLevel = 0;
    CString szLog;
    TLCBSpecN tStainSpec;
    memset(&tStainSpec, 0x00, sizeof(TLCBSpecN));
    int specCount = 0;
    //----------------------------------------------------------------------
    // Spec - LCB
    //----------------------------------------------------------------------
	IplImage *cvImgLcb = cvCreateImage(cvSize(nWidth, nHeight), 8, 3); // bmp for display
	//cvImgLcb->imageData = (char*)MIU.m_pFrameBMPBuffer;
	//
	tStainSpec.dCenterThreshold = model.m_LcbSpec[specCount++];// 35.0;
	tStainSpec.dEdgeThreshold = model.m_LcbSpec[specCount++];//13.5;
	tStainSpec.dCornerThreshold = model.m_LcbSpec[specCount++];//40.5;
	tStainSpec.nMaxSingleDefectNum = (int)model.m_LcbSpec[specCount++];//50000;
	tStainSpec.nMinDefectWidthHeight = (int)model.m_LcbSpec[specCount++];//12;
	tStainSpec.dCenterMaxR = model.m_LcbSpec[specCount++];//9;
	tStainSpec.dCenterMaxGr = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCenterMaxGb = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCenterMaxB = model.m_LcbSpec[specCount++];//9;
	tStainSpec.dEdgeMaxR = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dEdgeMaxGr = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dEdgeMaxGb = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dEdgeMaxB = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCornerMaxR = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCornerMaxGr = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCornerMaxGb = model.m_LcbSpec[specCount++];//0;
	tStainSpec.dCornerMaxB = model.m_LcbSpec[specCount++];//0;
	tStainSpec.tCircleSpec.bEnableCircle = model.m_LcbSpec[specCount++];//true;
	tStainSpec.tCircleSpec.nPosOffsetX = (int)model.m_LcbSpec[specCount++];//5;
	tStainSpec.tCircleSpec.nPosOffsetY = (int)model.m_LcbSpec[specCount++];//5;
	tStainSpec.tCircleSpec.dRadiusRatioX = model.m_LcbSpec[specCount++];//0.5;
	tStainSpec.tCircleSpec.dRadiusRatioY = model.m_LcbSpec[specCount++];//0.5;
	tStainSpec.tCircleSpec.dThresholdRatio = model.m_LcbSpec[specCount++];//0.5;
	tStainSpec.tCircleSpec.dROIRange = model.m_LcbSpec[specCount++];//0.5;
	tStainSpec.tCircleSpec.nUsedFixedCircle = model.m_LcbSpec[specCount++];//false;
	tStainSpec.tMultiCircleSpec.bEnableMultiCircle = model.m_LcbSpec[specCount++];//false;
	tStainSpec.tMultiCircleSpec.dZoneSizeRatio[0] = model.m_LcbSpec[specCount++];//0.25;
	tStainSpec.tMultiCircleSpec.dZoneSizeRatio[1] = model.m_LcbSpec[specCount++];//0.35;
	tStainSpec.tMultiCircleSpec.dZoneSizeRatio[2] = model.m_LcbSpec[specCount++];//0.45;
	tStainSpec.tMultiCircleSpec.dThreshold[0] = model.m_LcbSpec[specCount++];//9.0;
	tStainSpec.tMultiCircleSpec.dThreshold[1] = model.m_LcbSpec[specCount++];//15.0;
	tStainSpec.tMultiCircleSpec.dThreshold[2] = model.m_LcbSpec[specCount++];//50.0;
	tStainSpec.tMultiCircleSpec.nBlobSize[0] = model.m_LcbSpec[specCount++];
	tStainSpec.tMultiCircleSpec.nBlobSize[1] = model.m_LcbSpec[specCount++];
	tStainSpec.tMultiCircleSpec.nBlobSize[2] = model.m_LcbSpec[specCount++];


    std::shared_ptr<CACMISImageStainLCBCommon> pInspectLCB = std::make_shared<CACMISImageStainLCBCommon>();
	pInspectLCB->MakeHistEQImage((const BYTE*)img, (BYTE*)cvImgLcb->imageData, gMIUDevice.nWidth, gMIUDevice.nHeight, gMIUDevice.dTDATASPEC_n, 1);
	CvFont cvfont;
	CvPoint pt;
	int scale = (nWidth > 640 ? 1 : 2);
	double nFontSize = 1 / scale;
	cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);

#if  (____SENSOR_TYPE == S_OMNIVISION)
	pInspectLCB->SetInspectPosOffset(0, 2, 0, 0);	//220630
#endif
    nResultLCB = pInspectLCB->Inspect((BYTE*)img, nWidth, nHeight, tStainSpec,tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType,nBlackLevel, 0, 0);

    if (nResultLCB > 0)
    {
        nResult = R_FAIL_STAIN;
    }
    szLog.Format("[LCB] Version %s", pInspectLCB->GetVersion());
	theApp.MainDlg->putListLog(szLog);
    szLog.Format("[LCB] DefectBlobCount %d", pInspectLCB->GetDefectBlobCount());
	theApp.MainDlg->putListLog(szLog);
    int ocx = 0, ocy = 0, radx = 0, rady = 0;
    CString sTemp;
	sTemp.Format("LCB BlobCount : %d", pInspectLCB->GetDefectBlobCount());
	pt.x = 50;
	pt.y = 80;
	cvPutText(cvImgLcb, sTemp, pt, &cvfont, CV_BLUE);
    
    model.m_LogBlemishLcb = pInspectLCB->GetDefectBlobCount();
	MESCommunication.m_nMesBlemish[0] = model.m_LogBlemishLcb;
    if (tStainSpec.tCircleSpec.bEnableCircle || tStainSpec.tMultiCircleSpec.bEnableMultiCircle)
    {
        const TDefectResult* pCircleMaxResult = pInspectLCB->GetMaxDefectResult(EIMAGEREGION_CIRCLE);

        ocx = (int)pCircleMaxResult->dContrastMaxR;
        ocy = (int)pCircleMaxResult->dContrastMaxGb;
        radx = (int)pCircleMaxResult->dContrastMaxGr * tStainSpec.tCircleSpec.dRadiusRatioX;
        rady = (int)pCircleMaxResult->dContrastMaxB  *tStainSpec.tCircleSpec.dRadiusRatioY;
       
        if (tStainSpec.tCircleSpec.bEnableCircle)
        {
            std::cout << "[LCB Circle] ocx=" << ocx << "ocy = " << ocy << "radx = " << radx << "rady = " << rady << std::endl;
            cvEllipse(cvImgLcb, cvPoint(ocx, ocy), cvSize(radx, rady), 0, 0, 360, CV_RGB(0, 255, 255),2);
			vision.ellipselist[CCD].addList(CPoint(ocx - radx, ocy - rady), CPoint(ocx + radx, ocy + rady), PS_SOLID, M_COLOR_CYAN);
        }
        else
        {
            double dRadYRatio = 1.0;
            if (tStainSpec.tCircleSpec.nUsedFixedCircle)
            {
                dRadYRatio = tStainSpec.tCircleSpec.dRadiusRatioY / tStainSpec.tCircleSpec.dRadiusRatioX;
            }
            //������x
            //������y
            int nZoneA_X = (int)(radx * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[0]);
            int nZoneA_Y = (int)(rady * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[0] * dRadYRatio);
            int nZoneB_X = (int)(radx * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[1]);
            int nZoneB_Y = (int)(rady * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[1] * dRadYRatio);
            int nZoneC_X = (int)(radx * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[2]);
            int nZoneC_Y = (int)(rady * tStainSpec.tMultiCircleSpec.dZoneSizeRatio[2] * dRadYRatio);
            cvEllipse(cvImgLcb, cvPoint(ocx, ocy), cvSize(nZoneA_X, nZoneA_Y), 0, 0, 360, CV_RGB(0, 255, 255), 2);
            cvEllipse(cvImgLcb, cvPoint(ocx, ocy), cvSize(nZoneB_X, nZoneB_Y), 0, 0, 360, CV_RGB(0, 255, 255), 2);
            cvEllipse(cvImgLcb, cvPoint(ocx, ocy), cvSize(nZoneC_X, nZoneC_Y), 0, 0, 360, CV_RGB(0, 255, 255), 2);
            //

			vision.ellipselist[CCD].addList(CPoint(ocx - nZoneA_X, ocy - nZoneA_Y), CPoint(ocx + nZoneA_X, ocy + nZoneA_Y), PS_SOLID, M_COLOR_MAGENTA);
			vision.ellipselist[CCD].addList(CPoint(ocx - nZoneB_X, ocy - nZoneB_Y), CPoint(ocx + nZoneB_X, ocy + nZoneB_Y), PS_SOLID, M_COLOR_MAGENTA);
			vision.ellipselist[CCD].addList(CPoint(ocx - nZoneC_X, ocy - nZoneC_Y), CPoint(ocx + nZoneC_X, ocy + nZoneC_Y), PS_SOLID, M_COLOR_MAGENTA);
            //vision.ellipselist[CCD].addList(CPoint(ocx - nZoneA_X, ocy - nZoneA_Y), CPoint(ocx + nZoneA_X, ocy + nZoneA_Y), PS_SOLID, M_COLOR_CYAN);

        }
        if (tStainSpec.tCircleSpec.nUsedFixedCircle)
        {
            RECT rtRefEdge;
            RECT rtCenter;
            //oc threshold test roi - top
            rtRefEdge.left = max(nWidth / 2 - 100 / 2, 0);
            rtRefEdge.top = (LONG)(0.1 * nHeight / 2);
            rtRefEdge.right = min(rtRefEdge.left + 100 - 1, (LONG)nWidth - 1);
            rtRefEdge.bottom = min(rtRefEdge.top + 100 - 1, (LONG)nHeight - 1);

            //oc threshold test roi - center
            rtCenter.left = rtRefEdge.left;
            rtCenter.top = max(nHeight / 2 - 100 / 2, 0);
            rtCenter.right = rtRefEdge.right;
            rtCenter.bottom = min(nHeight / 2 + 100 / 2 - 1, nHeight - 1);

            cvRectangle(cvImgLcb, cvPoint(rtRefEdge.left, rtRefEdge.top), cvPoint(rtRefEdge.right, rtRefEdge.bottom), CV_RGB(255, 86, 12));
            cvRectangle(cvImgLcb, cvPoint(rtCenter.left, rtCenter.top), cvPoint(rtCenter.right, rtCenter.bottom), CV_RGB(255, 86, 12));


			vision.boxlist[CCD].addList(rtRefEdge, PS_SOLID, M_COLOR_BLUE);
			vision.boxlist[CCD].addList(rtCenter, PS_SOLID, M_COLOR_BLUE);
        }
        
    }


	MESCommunication.m_nMesBlemishMaxDefect[3] = pInspectLCB->GetMaxDefectResult(EIMAGEREGION_CENTER)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[4] = pInspectLCB->GetMaxDefectResult(EIMAGEREGION_EDGE)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[5] = pInspectLCB->GetMaxDefectResult(EIMAGEREGION_CORNER)->dValue;

	//_stprintf_s(szLog, SIZE_OF_1K, _T("[LCB] Max Value (Center) %.6lf"), g_clMesCommunication[m_nUnit].m_nMesBlemishMaxDefect[3]);
	szLog.Format("[LCB] Max Value (Center) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[3]);
	theApp.MainDlg->putListLog(szLog);
	//_stprintf_s(szLog, SIZE_OF_1K, _T("[LCB] Max Value (Edge) %.6lf"), g_clMesCommunication[m_nUnit].m_nMesBlemishMaxDefect[4]);
	szLog.Format("[LCB] Max Value (Edge) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[4]);
	theApp.MainDlg->putListLog(szLog);
	//_stprintf_s(szLog, SIZE_OF_1K, _T("[LCB] Max Value (Corner) %.6lf"), g_clMesCommunication[m_nUnit].m_nMesBlemishMaxDefect[5]);
	szLog.Format("[LCB] Max Value (Corner) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[5]);
	theApp.MainDlg->putListLog(szLog);

    if (nResult)
    {
        if (tStainSpec.tCircleSpec.bEnableCircle || tStainSpec.tMultiCircleSpec.bEnableMultiCircle)
        {
            int nCircleCount = pInspectLCB->GetDefectCount(EIMAGEREGION_CIRCLE);
            if (tStainSpec.tCircleSpec.nUsedFixedCircle)
            {
                RECT rtRefEdge;
                RECT rtCenter;
                //oc threshold test roi - top
                rtRefEdge.left = max(gMIUDevice.nWidth / 2 - 100 / 2, 0);
                rtRefEdge.top = (LONG)(0.1 * gMIUDevice.nHeight / 2);
                rtRefEdge.right = min(rtRefEdge.left + 100 - 1, (LONG)gMIUDevice.nWidth - 1);
                rtRefEdge.bottom = min(rtRefEdge.top + 100 - 1, (LONG)gMIUDevice.nHeight - 1);

                //oc threshold test roi - center
                rtCenter.left = rtRefEdge.left;
                rtCenter.top = max(gMIUDevice.nHeight / 2 - 100 / 2, 0);
                rtCenter.right = rtRefEdge.right;
                rtCenter.bottom = min(gMIUDevice.nHeight / 2 + 100 / 2 - 1, gMIUDevice.nHeight - 1);

                cvRectangle(cvImgLcb, cvPoint(rtRefEdge.left, rtRefEdge.top), cvPoint(rtRefEdge.right, rtRefEdge.bottom), CV_RGB(255, 86, 12));
                cvRectangle(cvImgLcb, cvPoint(rtCenter.left, rtCenter.top), cvPoint(rtCenter.right, rtCenter.bottom), CV_RGB(255, 86, 12));

               // g_clVision.DrawMOverlayBox(m_nUnit, rtRefEdge, M_COLOR_RED, 1, FALSE, PS_SOLID);
                //g_clVision.DrawMOverlayBox(m_nUnit, rtCenter, M_COLOR_RED, 1, FALSE, PS_SOLID);
                vision.boxlist[CCD].addList(rtRefEdge, PS_SOLID, M_COLOR_RED);
                vision.boxlist[CCD].addList(rtCenter, PS_SOLID, M_COLOR_RED);

            }
        }

		if (pInspectLCB->GetDefectBlobCount() > 0)
		{
			//�̹��ҷ�
			sTemp.Format("LCB fail Spec NG: %d (Spec:%d)", pInspectLCB->GetDefectBlobCount(), 0);
			//! Main Displayȭ�� Overlay NG List
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("LCB fail: %d (Spec:%d)", pInspectLCB->GetDefectBlobCount(), 0);
			MandoInspLog.iNGCnt++;
			MandoInspLog.bInspRes = false;//LCB
			//bResult = false;
			theApp.MainDlg->putListLog(sTemp);
			MandoInspLog.sNGList += sTemp;
			for (int i = 0; i < pInspectLCB->GetDefectBlobCount(); i++)
			{
				const RECT* rt = pInspectLCB->GetDefectBlobRect(i);
				cvRectangle(cvImgLcb, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(0, 0, 255), 2);
				//g_clVision.DrawMOverlayBox(m_nUnit, rt->left, rt->top, rt->right, rt->bottom, M_COLOR_BLUE, 1, FALSE);
				vision.boxlist[CCD].addList(rt, PS_SOLID, M_COLOR_BLUE);
			}

			//220627
			//double dReduceFactorX = 0.;
			//double dReduceFactorY = 0.;

			/*dReduceFactorX = (double)theApp.MainDlg->autodispDlg->m_iSizeX_Client / gMIUDevice.nWidth;
			dReduceFactorY = (double)theApp.MainDlg->autodispDlg->m_iSizeY_Client / gMIUDevice.nHeight;
			MimResize(vision.MilGrabImage[1], vision.MilDefectImage, dReduceFactorX, dReduceFactorY, M_DEFAULT);*/
			//220627
		}
    }
	Jpg_ImageSave(cvImgLcb, LCBJPG);
    //cvSaveImage("D:\\StainImg.bmp", cvImgLcb);
    //
    g_SaveLGITLog(m_nUnit, "LCB", pInspectLCB->GetLogHeader(), pInspectLCB->GetLogData());

	cvReleaseImage(&cvImgLcb);
    return nResultLCB;
}
//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
int CPRIFunc_Insp::Blemish_YmeanInsp(BYTE* img, int nWidth, int nHeight, TDATASPEC& tDataSpec)
{
    int nResult = R_RESULT_PASS;
    int nResultRUYmean = 0;
    int nBlackLevel = 0;
    CString szLog = "";
    TStainSpec tStainSpec;
    TRU_YmeanSpecN stSpecYmean;
    memset(&stSpecYmean, 0x00, sizeof(TRU_YmeanSpecN));
    int specCount = 0;

	IplImage *cvImgYmean = cvCreateImage(cvSize(nWidth, nHeight), 8, 3); // bmp for display
	//cvImgYmean->imageData = (char*)MIU.m_pFrameBMPBuffer;
	
    //----------------------------------------------------------------------
    // Spec - Ymean
    //----------------------------------------------------------------------
	stSpecYmean.nEdgeSize = (int)model.m_YmeanSpec[specCount++];// 100;
	stSpecYmean.fCenterThreshold = model.m_YmeanSpec[specCount++];//10.75;
	stSpecYmean.fEdgeThreshold = model.m_YmeanSpec[specCount++];//15.0;
	stSpecYmean.fCornerThreshold = model.m_YmeanSpec[specCount++];//15.0;
	stSpecYmean.nDefectBlockSize = model.m_YmeanSpec[specCount++];//15;
	stSpecYmean.nLscBlockSize = (int)model.m_YmeanSpec[specCount++];//64;
	stSpecYmean.nCalcType = (int)model.m_YmeanSpec[specCount++];//0;
	stSpecYmean.nMaxRecursiveCount = model.m_YmeanSpec[specCount++];//1000;
	stSpecYmean.dMaxDefectSize = model.m_YmeanSpec[specCount++];//70.0;
	stSpecYmean.dPixelSize = model.m_YmeanSpec[specCount++];//4.2;

	stSpecYmean.tCircleSpec.bEnableCircle = model.m_YmeanSpec[specCount++];//false;
	stSpecYmean.tCircleSpec.nPosOffsetX = (int)model.m_YmeanSpec[specCount++];//5;
	stSpecYmean.tCircleSpec.nPosOffsetY = (int)model.m_YmeanSpec[specCount++];//5;
	stSpecYmean.tCircleSpec.dRadiusRatioX = model.m_YmeanSpec[specCount++];//0.45;
	stSpecYmean.tCircleSpec.dRadiusRatioY = model.m_YmeanSpec[specCount++];//0.45;
	stSpecYmean.tCircleSpec.dThresholdRatio = model.m_YmeanSpec[specCount++];//0.5;
	stSpecYmean.tCircleSpec.dROIRange = model.m_YmeanSpec[specCount++];//0.5;
	stSpecYmean.tCircleSpec.nUsedFixedCircle = (int)model.m_YmeanSpec[specCount++];//0;
	stSpecYmean.tMultiCircleSpec.bEnableMultiCircle = (int)model.m_YmeanSpec[specCount++];//false;
	stSpecYmean.tMultiCircleSpec.dZoneSizeRatio[0] = model.m_YmeanSpec[specCount++];//0.2;
	stSpecYmean.tMultiCircleSpec.dZoneSizeRatio[1] = model.m_YmeanSpec[specCount++];//0.4;
	stSpecYmean.tMultiCircleSpec.dZoneSizeRatio[2] = model.m_YmeanSpec[specCount++];//0.52;
	stSpecYmean.tMultiCircleSpec.dThreshold[0] = model.m_YmeanSpec[specCount++];//0.45;
	stSpecYmean.tMultiCircleSpec.dThreshold[1] = model.m_YmeanSpec[specCount++];//0.45;
	stSpecYmean.tMultiCircleSpec.dThreshold[2] = model.m_YmeanSpec[specCount++];//0.8;
	stSpecYmean.tMultiCircleSpec.nBlobSize[0] = model.m_YmeanSpec[specCount++];
	stSpecYmean.tMultiCircleSpec.nBlobSize[1] = model.m_YmeanSpec[specCount++];
	stSpecYmean.tMultiCircleSpec.nBlobSize[2] = model.m_YmeanSpec[specCount++];

    std::shared_ptr<CACMISImageStainRU_YmeanCommon> pInspectBlemish_Ymean = std::make_shared<CACMISImageStainRU_YmeanCommon>();//delete,x
	pInspectBlemish_Ymean->MakeHistEQImage((const BYTE*)img, (BYTE*)cvImgYmean->imageData, gMIUDevice.nWidth, gMIUDevice.nHeight, gMIUDevice.dTDATASPEC_n, 1);
	CvFont cvfont;
	CvPoint pt;
	int scale = (nWidth > 640 ? 1 : 2);
	double nFontSize = 1 / scale;
	cvInitFont(&cvfont, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_NORMAL, nFontSize, nFontSize, 0, 1, 10);

#if  (____SENSOR_TYPE == S_OMNIVISION)
	pInspectBlemish_Ymean->SetInspectPosOffset(0, 2, 0, 0);	//220630
#endif
    pInspectBlemish_Ymean->Inspect((BYTE*)img, nWidth, nHeight, stSpecYmean, tDataSpec.eDataFormat, tDataSpec.eOutMode, tDataSpec.eSensorType, nBlackLevel, 0, 0);
    nResultRUYmean = pInspectBlemish_Ymean->GetDefectBlobCount();
	
    CString sTemp;
	sTemp.Format("Ymean BlobCount : %d", pInspectBlemish_Ymean->GetDefectBlobCount());
	pt.x = 50;
	pt.y = 110;
	cvPutText(cvImgYmean, sTemp, pt, &cvfont, CV_BLUE);
    if (nResultRUYmean > 0)
    {
        nResult = R_FAIL_STAIN;// R_FAIL_BLACK_SPOT;
        //�̹��ҷ�

        sTemp.Format("Ymean fail Spec NG: %d (Spec:%d)", pInspectBlemish_Ymean->GetDefectBlobCount(), 0);
        //! Main Displayȭ�� Overlay NG List
        MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("Ymean fail: %d (Spec:%d)", pInspectBlemish_Ymean->GetDefectBlobCount(), 0);
        MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;//YMEAN
        theApp.MainDlg->putListLog(sTemp);
        MandoInspLog.sNGList += sTemp;
    }

    szLog.Format("[RUYMean] Version %s", pInspectBlemish_Ymean->GetVersion());
	theApp.MainDlg->putListLog(szLog);
    szLog.Format("[RUYMean] GetDefectBlobCount %d", nResultRUYmean);
	theApp.MainDlg->putListLog(szLog);
    model.m_LogBlemishRU_Ymean = nResultRUYmean;
	MESCommunication.m_nMesBlemish[1] = model.m_LogBlemishRU_Ymean;
    if (nResult)
    {
        if (stSpecYmean.tCircleSpec.bEnableCircle || stSpecYmean.tMultiCircleSpec.bEnableMultiCircle)
        {
            int ocx = 0, ocy = 0, radx = 0, rady = 0;
            int nCircleCount = pInspectBlemish_Ymean->GetDefectCount(EIMAGEREGION_CIRCLE);
            const TDefectResult* pCircleMaxResult = pInspectBlemish_Ymean->GetMaxDefectResult(EIMAGEREGION_CIRCLE);

            ocx = (int)pCircleMaxResult->dContrastMaxR;
            ocy = (int)pCircleMaxResult->dContrastMaxGb;
            radx = (int)pCircleMaxResult->dContrastMaxGr* stSpecYmean.tCircleSpec.dRadiusRatioX;;
            rady = (int)pCircleMaxResult->dContrastMaxB *stSpecYmean.tCircleSpec.dRadiusRatioY; ;
            if (stSpecYmean.tCircleSpec.bEnableCircle)
            {
                //std::cout << "[RUYMean] ocx=" << ocx << "ocy = " << ocy << "radx = " << radx << "rady = " << rady << std::endl;
                cvEllipse(cvImgYmean, cvPoint(ocx, ocy), cvSize(radx, rady), 0, 0, 360, CV_RGB(0, 255, 255));
                //vision.ellipselist[CCD].addList(CPoint(ocx - radx, ocy - rady), CPoint(ocx + radx, ocy + rady), PS_SOLID, M_COLOR_CYAN);

            }

        }
        for (int i = 0; i < pInspectBlemish_Ymean->GetDefectBlobCount(); i++)
        {
            const RECT* rt = pInspectBlemish_Ymean->GetDefectBlobRect(i);
            cvRectangle(cvImgYmean, cvPoint(rt->left, rt->top), cvPoint(rt->right, rt->bottom), CV_RGB(138, 43, 226), 2);
            vision.boxlist[CCD].addList(rt, PS_SOLID, M_COLOR_CYAN);
        }
    }
	MESCommunication.m_nMesBlemishMaxDefect[6] = pInspectBlemish_Ymean->GetMaxDefectResult(EIMAGEREGION_CENTER)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[7] = pInspectBlemish_Ymean->GetMaxDefectResult(EIMAGEREGION_EDGE)->dValue;
	MESCommunication.m_nMesBlemishMaxDefect[8] = pInspectBlemish_Ymean->GetMaxDefectResult(EIMAGEREGION_CORNER)->dValue;

	szLog.Format("[Ymean] Max Value (Center) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[6]);
	theApp.MainDlg->putListLog(szLog);
	szLog.Format("[Ymean] Max Value (Edge) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[7]);
	theApp.MainDlg->putListLog(szLog);
	szLog.Format("[Ymean] Max Value (Corner) %.6lf", MESCommunication.m_nMesBlemishMaxDefect[8]);
	theApp.MainDlg->putListLog(szLog);


	Jpg_ImageSave(cvImgYmean, YMEAN_JPG);
    g_SaveLGITLog(m_nUnit, "RUYMean", pInspectBlemish_Ymean->GetLogHeader(), pInspectBlemish_Ymean->GetLogData());

	cvReleaseImage(&cvImgYmean);
    return nResultRUYmean;
}
bool CPRIFunc_Insp::func_insp_Voltage()
{
	double cur = 0.0;
	//SHM �輺�� å�Ӹ� 2312115 �߰�
	//Des Port A = 0x40 , B = 0x40 , C = 0x41 , D = 0x41 
	//Register Address A = 0x04 , B = 0x02 , C = 0x04 , D = 0x02 
	bool	bRes = false;
	int errorCode = 0;
	bool bInspRtn = true;

	byte WData[1];
	int i = 0;
	byte pVolData1[8];
	memset(pVolData1, 0x00, 8);

	unsigned int nSlaveAddress = 0x41;
	unsigned int nAddress = 0x02;
	unsigned short kkkk = 0x0000;
	errorCode = MIU.m_pBoard->ReadI2CBurst((unsigned short)nSlaveAddress, (unsigned short)nAddress, 1, pVolData1, (unsigned short)2);	//Temperature1

	//[14-3]bits�� 10������ ��ȯ
	//V_bus = ��ȯ �� * 0.008;

	int decimalValue = (pVolData1[0] << 8) | pVolData1[1];
	int sumData = (decimalValue >> 3) & ~(1 << 12);

	CString sTemp;
	double dVoltageVal = sumData * 0.008;
	double dVoltageSpec = MandoSfrSpec.INSP_Voltage_Spec;;

	MandoInspLog.dVoltage = dVoltageVal;

	if (dVoltageVal < 11.5 || dVoltageVal > 12.5)
	{
		sTemp.Format("Voltage Spec NG: %.6lf (Spec:11.5 ~ 12.5)", dVoltageVal);
		//! Main Displayȭ�� Overlay NG List
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("Voltage fail: %.6lf (Spec:11.5 ~ 12.5)", dVoltageVal);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;//LCB
		theApp.MainDlg->putListLog(sTemp);
		MandoInspLog.sNGList += sTemp;
	}
	else
	{
		sTemp.Format("Voltage Spec IN: %.6lf (Spec:11.5 ~ 12.5)", dVoltageVal);
		theApp.MainDlg->putListLog(sTemp);
	}

	sTemp.Empty();
	return false;
}

//-----------------------------------------------------------------------------
//
//	���� ����
//
//-----------------------------------------------------------------------------
std::bitset<16> calculateTwosComplement(const std::bitset<16>& original) {
	std::bitset<16> onesComplement = ~original;

	// 1�� ���ϱ� ���� unsigned long long�� ��ȯ �� ���� �� �ٽ� std::bitset���� ��ȯ
	return std::bitset<16>(onesComplement.to_ullong() + 1);
}
bool CPRIFunc_Insp::func_Insp_CurrentMeasure(bool bLogDraw, bool bAutoMode)
{
	double dCurrentVal = 0.0;

	bool	bRes = false;
	int errorCode = 0;
	bool bInspRtn = true;

	byte WData[1];
	int i = 0;
	byte pVolData1[8];
	memset(pVolData1, 0x00, 8);

	unsigned int nSlaveAddress = 0x41;
	unsigned int nAddress = 0x01;

	
	errorCode = MIU.m_pBoard->ReadI2CBurst((unsigned short)nSlaveAddress, (unsigned short)nAddress, 1, pVolData1, (unsigned short)2);	//Temperature1

	//SHM �輺�� å�Ӹ� 2312115 �߰�
	//Des Port A = 0x40 , B = 0x40 , C = 0x41 , D = 0x41 
	//Register Address A = 0x03 , B = 0x01 , C = 0x03 , D = 0x01 
	//
	// 16���� 0xA5�� 16��Ʈ 2������ ��ȯ
	//std::bitset<16> binaryNumber(pVolData1[0]);// 0xA5);

	//// 2�� ���� ���
	////std::bitset<16> twosComplement = calculateTwosComplement(binaryNumber);
	//// 2�� ���� ���
	//std::bitset<16> twosComplement = calculateTwosComplement(binaryNumber);

	//int decimalValue = (pVolData1[0] << 8) | pVolData1[1];
	//int sumData = (decimalValue >> 3) & ~(1 << 12);

	//// std::bitset���� unsigned long long�� ��ȯ �� double�� ��ȯ
	//unsigned long long twosComplementValue = twosComplement.to_ullong();
	//double twosComplementDouble = static_cast<double>(twosComplementValue);

	std::bitset<8> binaryByte1(pVolData1[0]);
	std::bitset<8> binaryByte2(pVolData1[1]);
	// 2���� ��ȯ
	std::bitset<16> combinedBinaryValue((binaryByte1.to_ulong() << 8) | binaryByte2.to_ulong());
	//2�� ���� ��ȯ
	std::bitset<16> twosComplement = ~combinedBinaryValue;
	twosComplement = twosComplement.to_ulong() + 1;
	//3 ~ 14 bit�����
	std::bitset<12> maskedValue = (twosComplement.to_ulong() >> 3) & ~(1 << 12);
	//���� ��ȯ
	int decimalValue = static_cast<int>(maskedValue.to_ulong());
	


	//������ 0.21���� �۾ƾ� ok


	//16bit 2�������� 2�� ���� ���
	//[14-3]bits�� 10������ ��ȯ
	//V_shunt = ��ȯ�� * 0.00004
	CString sTemp;
	double dCurrentSpec = MandoSfrSpec.INSP_Current_Spec;
	dCurrentVal = decimalValue * 0.00004;

	MandoInspLog.dCurrent = dCurrentVal;
	if (dCurrentVal > dCurrentSpec)
	{
		sTemp.Format("Current Spec NG: %.6lf (Spec:%.6lf)", dCurrentVal, dCurrentSpec);
		//! Main Displayȭ�� Overlay NG List
		MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("Current fail: %.6lf (Spec:%.6lf)", dCurrentVal, dCurrentSpec);
		MandoInspLog.iNGCnt++;
		MandoInspLog.bInspRes = false;//LCB
		theApp.MainDlg->putListLog(sTemp);
		MandoInspLog.sNGList += sTemp;
	}
	else
	{
		sTemp.Format("Current Spec IN: %.6lf (Spec:%.6lf)", dCurrentVal, dCurrentSpec);
		theApp.MainDlg->putListLog(sTemp);
	}

	sTemp.Empty();


//	TCHAR szLog[SIZE_OF_1K];
//
//	CMapData spec;
//	//GetIni("SPEC.INI", spec);
//	TCHAR	szPath[SIZE_OF_1K];
//
//
//	//g_clVision.ClearOverlay(CAM_CCD);
//
//
//	_stprintf_s(szPath, SIZE_OF_1K, _T("%s\\Model\\%s\\SPEC.INI"), BASE_DATA_PATH, g_clSysData.m_szModelName);
//	GetIni(szPath, spec);
//
//	double m_dCurrentMin = spec["CURRENT_SPEC.current_min.d"].d;
//	double m_dCurrentMax = spec["CURRENT_SPEC.current_max.d"].d;
//
//	//MIU.GetCurrent( &cur ); 
//#ifdef ON_LINE_GRABBER
//	g_clLaonGrabberWrapper[m_nUnit].GetCurrent(&cur);
//#endif
//
//	if ((cur > m_dCurrentMin) && (cur < m_dCurrentMax))
//	{
//		bRes = true;
//	}
//
//	// 20141202 LHC - current ���� �Ͽ� MES ����
//	if (bRes == true)
//	{
//		g_clMesCommunication[m_nUnit].m_dMesCurrent = cur;
//		g_clMesCommunication[m_nUnit].m_nMesCurrentResult = 1;
//	}
//	else
//	{
//		g_clMesCommunication[m_nUnit].m_dMesCurrent = cur;
//		g_clMesCommunication[m_nUnit].m_nMesCurrentResult = 0;
//	}
//
//	g_clMandoInspLog[m_nUnit].m_dCurrent = cur;
//
//	if (bLogDraw == true)
//	{
//		_stprintf_s(szLog, SIZE_OF_1K, _T("	���� ����ġ : %.02lf"), cur);
//		//AddLog(szLog, 0, m_nUnit);
//		theApp.MainDlg->putListLog(szLog);
//
//		g_clMandoInspLog[m_nUnit].m_dCurrent = cur;
//
//		if (bRes == false)
//		{
//			// ����ö�Դϴ�
//			//���� ������Camera �˻� Log ���� Defect_Black
//			g_clMandoInspLog[m_nUnit].m_bInspRes = false;						//�˻� ��� ����
//			g_clMandoInspLog[m_nUnit].m_sNGList += _T("[CURRENT ERROR]");	//NG ����
//
//			if (g_clMandoInspLog[m_nUnit].m_nNGCnt < 20)
//			{
//				//! Main Displayȭ�� Overlay NG List
//				g_clMandoInspLog[m_nUnit].m_sDispNG[g_clMandoInspLog[m_nUnit].m_nNGCnt].Format(_T("CURRENT ERROR : %.02lf"), cur);
//				g_clMandoInspLog[m_nUnit].m_nNGCnt++;
//			}
//
//			if (bAutoMode == true)	g_clTaskWork[m_nUnit].m_nCurrentNgCount++;	//�ڵ��� ��츸 �˻� NG ���� ����
//		}
//	}
//
	return bRes;
}

bool CPRIFunc_Insp::func_insp_CDP800_Current()
{
#if 0
	ViStatus status;
	ViSession defaultRM;
	ViSession power_supply;
	ViString expr = "?*";
	ViPFindList findList = new unsigned long;
	ViPUInt32 retcnt = new unsigned long;
	ViChar instrDesc[1000];
	char reply_string[256];
	char GPIB_address[3];
	char Visa_address[40];
	double voltage = 0.0;
	double current = 0.0;
	TCHAR szLog[SIZE_OF_1K];
	CString strSrc = "";
	CString strInstr = "";
	CString m_strInstrAddr = "";
	unsigned long i = 0;
	bool bFindDP = false;
	m_strInstrAddr = _T("");
	if (false)
	{
		//RS232 ����
	}
	else
	{
		//GPIO ����
		strcpy(GPIB_address, "5");
		strcpy(Visa_address, "GPIB0::");
		strcat(Visa_address, GPIB_address);
	}
	status = viOpenDefaultRM(&defaultRM);
	voltage = 12.0;

	viOpen(defaultRM, Visa_address, 0, 0, &power_supply);

	/* Query the power supply id, read response and print */
	viPrintf(power_supply, "*IDN?\n");
	viScanf(power_supply, "%s", &reply_string);
	//TRACE("Instrument identification string:\n	%s/n/n", reply_string);
	_stprintf_s(szLog, SIZE_OF_1K, _T("Instrument identification string : %s"), reply_string);
	theApp.MainDlg->putListLog(szLog);
	/* Initialize Power Supply */
	//viPrintf(power_supply, "*RST\n"); 
	//viPrintf(power_supply, "Current 2\n");
	viPrintf(power_supply, "Output on\n");
	//viPrintf(power_supply, "Volt %f\n", voltage); 
	//TRACE("Voltage Current/n/n");



	//for (voltage = 0.1; voltage <= 0.2; voltage += 0.02)
	{
		viPrintf(power_supply, "Volt %f\n", voltage);
		Sleep(500);
		//viPrintf(power_supply, "Measure:Current[:DC]?");//"Measure:Current?\n");
		//viPrintf(power_supply, ":MEAS:ALL? CH1");
		viPrintf(power_supply, "Measure:Current?\n");
		Sleep(100);
		viScanf(power_supply, "%s", &reply_string);
		//_stprintf_s(szLog, SIZE_OF_1K, _T("Current string : %s"), reply_string);
		//theApp.MainDlg->putListLog(szLog);
		viScanf(power_supply, "%lf\n", &current);
		model.dGen2Current = current;
		_stprintf_s(szLog, SIZE_OF_1K, _T("CURRENT:%.3lf"), model.dGen2Current);
		theApp.MainDlg->putListLog(szLog);
	}
	//viPrintf(power_supply, "Output off\n");
	//viClose(power_supply);
	//viClose(defaultRM);
	//142Current\n\n");Change GPIB address here *****/%s\n\n", reply_string);/* Set power on condition/* Set Current limit to 2A/* Turn output on/* Print heading/*set voltage/* print power supply setting *//* allow output to settle for 500 msec *//*measure output current/* retrieve readin

#endif
	return false;
}

//-----------------------------------------------------------------------------
//
//	TEST PATTERN �˻�
//
//-----------------------------------------------------------------------------
int CPRIFunc_Insp::func_Insp_TestPattern(bool bAutoMode)
{
	return -1;
}
