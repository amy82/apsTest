// AA BonderDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "AA Bonder.h"
#include "AA BonderDlg.h"
#include "ImageInsp/Inspection_Alg.h"
#include "InformDlg.h"
#include "InfoDlg.h"
#include <algorithm>
//! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
CInspection_Alg Alg;
//! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <mmsystem.h>				// ��Ƽ �̵�� Ÿ�̸�..
#pragma comment(lib, "winmm.lib")

#define BCR_STX				0x02
#define BCR_ETX				0x03

#define BCR_ACK				0x06
#define BCR_CR				0x0d
#define BCR_NAK				0x15


/* Thread */
/************************************************************************/

//! Modified by LHW (2013/2/25)
//! ���� ��� �ӵ��� �ø��� ���Ͽ� ���� ���� Thread�� ������ ����� �۾��Ѵ�.
//! 4���� Thread�� �׻� ¦�� �Ǿ�, ����/���� �Ǿ�� �Ѵ�.
bool bThreadCcmGrab = false;
bool bThreadCcmGrabRun = false;
bool bFlag_First_Grab_Display = false;
CWinThread* pThread_CCM_Grab     = NULL;
CWinThread* pThread_CCM_CvtColor = NULL;
CWinThread* pThread_CCM_CvtMil   = NULL;
CWinThread* pThread_CCM_Display  = NULL;
CWinThread*	pThread_CheckDate  = NULL;

CWinThread* pThread_TaskOrigin = NULL;
bool bThreadOriginRun = false;
//


bool bThreadServoAlarm = false;
bool bThreadServoAlarmRun = false;
CWinThread* pThread_ServoAlarm = NULL;	//! Added by LHW (2013/4/19)
//! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool bThreadTaskLens = false;
bool bThreadTaskLensRun = false;
CWinThread* pThread_TaskLens = NULL;
bool bThreadTaskLens_Align = false;
bool bThreadTaskLensRun_Align = false;
bool bThreadTaskPcb = false;
bool bThreadTaskPcbRun = false;
CWinThread* pThread_TaskPcb = NULL;

bool bThreadTaskReady= false;
bool bThreadTaskReadyRun = false;
int	 iReadyRunCnt = 0;
CWinThread* pThread_TaskReady = NULL;


bool bThread_MIUCheckRun = false;
CWinThread* pThread_MIUCheck = NULL;

bool bThreadMonitor = false;
bool bThreadMonitorRun = false;
CWinThread* pThread_Monitor = NULL;


bool bThreadClock = false;
bool bThreadClockRun = false;
CWinThread* pThread_Clock = NULL;


bool bThreadGrab = false;
bool bThreadGrabRun = false;
CWinThread* pThread_Grab = NULL;

bool bThreadEpoxyRun = false;
CWinThread* pThread_Epoxy = NULL;
/************************************************************************/


UINT Thread_Epoxy(LPVOID parm)
{
	bThreadEpoxyRun = true;
	g_bMovingflag = true;

	//CRICLE_EPOXY, RECT_EPOXY, POLYGON_EPOXY


	//���� ���ü��� 3������ ������ �ȴ�.
	/*
	motor.func_Epoxy_Draw();				//����
	motor.func_Epoxy_CircleDraw();			//��
	motor.func_Epoxy_Rect_Circle_Draw();	//�ٰ���
	*/
	//
	if (sysData.nEpoxyIndex == CRICLE_EPOXY)
	{
		motor.func_Epoxy_CircleDraw();
	}
	else if (sysData.nEpoxyIndex == RECT_EPOXY)
	{
		motor.func_Epoxy_Draw();
	}
	else if(sysData.nEpoxyIndex == POLYGON_EPOXY)
	{
		motor.func_Epoxy_Rect_Circle_Draw();
	}
	else if (sysData.nEpoxyIndex == POINT_EPOXY)
	{
		//point ��
	}

	motor.PCB_Z_Motor_Move(Wait_Pos);



	g_bMovingflag = false;
	bThreadEpoxyRun = false;
	return 1;
}
UINT Thread_Grab(LPVOID parm)
{
	bThreadGrab = bThreadGrabRun = true;

	while (bThreadGrab)
	{
		if (vision.UserHookData.liveMode == 1)
		{
			MdigGrab(vision.MilDigitizer, vision.MilGrabImage[0]);
			MdigGrabWait(vision.MilDigitizer, M_GRAB_FRAME_END);

			//MimFlip(vision.MilGrabImageChild[CAM2], vision.MilGrabImageChild[CAM2], M_HORIZONTAL, M_DEFAULT);
			//MimRotate(vision.MilGrabImageChild[CAM2], vision.MilGrabImageChild[CAM2], 90,CAM_SIZE_X/2,CAM_SIZE_Y/2,CAM_SIZE_X/2,CAM_SIZE_Y/2,M_DEFAULT);

			MimResize(vision.MilGrabImageChild[0], vision.MilSmallImageChild[0], CAM_REDUCE_FACTOR_X, CAM_REDUCE_FACTOR_Y, M_DEFAULT);
			//MimResize(vision.MilGrabImageChild[1], vision.MilSmallImageChild[1], CAM_REDUCE_FACTOR_X, CAM_REDUCE_FACTOR_Y, M_DEFAULT);
		}

		::Sleep(10);
	}

	return 1;
}


//! Servo ����̹��� Alarm Ȯ��
//! [���� ����] PCI-R1604, PCI-R1604-MLII�� ����� ��������̹� �࿡�� ���
UINT Thread_ServoAlarm(LPVOID parm)
{
	bThreadServoAlarm = true;
	bThreadServoAlarmRun = true;

	int   i = 0;
	long  lAxisNo; 
	DWORD dwRet = 0;

	long  BoardNo, ModulePos;
	DWORD ModuleID;

	DWORD uReturnMode = 0;
	DWORD upAlarmCode;	//! Ȯ�ε� �˶� �ڵ�

	const int iBufSize_GetAlarm = 1024;
	char  szGetAlarm[iBufSize_GetAlarm];
	CString asAlarm[MAX_MOTOR_NO];
	for ( i = 0; i < MAX_MOTOR_NO; i++ )
	{
		asAlarm[i] = _T("");
	}
			
	while (bThreadServoAlarm)
	{
		for ( i = 0; i < MAX_MOTOR_NO; i++ )
		{
			lAxisNo = i;

			//! Servo Alarm ���� Ȯ��
			if ( motor.GetAmpFault(lAxisNo) == false )
			{
				continue;
			}

			//! Servo ����̹��� Alarm�� Ȯ���� �� �ִ� ���� Ȯ��
			dwRet = AxmInfoGetAxis(lAxisNo, &BoardNo, &ModulePos, &ModuleID);
			if ( dwRet != AXT_RT_SUCCESS )
			{
				continue;
			}
			if ( ModuleID != AXT_SMC_R1V04A4 && ModuleID != AXT_SMC_R1V04A5 )
			{
				continue;
			}

			//! �������� �˶� ����Ȯ���� ��û�Ѵ�.
			dwRet = AxmStatusRequestServoAlarm(lAxisNo);

			//! �������� �˶� ����Ȯ�� ��û�� ���� ���� ����� Ȯ���Ѵ�.
			//! uReturnMode : �Լ� ���� ������ ����
			//! - [0] �Լ� ���� �� �ٷ� ��ȯ
			//! - [1] ���������κ��� �˶� �ڵ带 ���� �� ���� ��ȭ���� ����
			//! - [2] ���������κ��� �˶� �ڵ带 ���� �� ���� ��ȭ���� ������ ���α׷� Blocking ���� ����
			uReturnMode = 0;
			dwRet = AxmStatusReadServoAlarm(lAxisNo, uReturnMode, &upAlarmCode);
			if ( dwRet != AXT_RT_SUCCESS )
			{
				//! [4210] AXT_RT_MOTION_READ_ALARM_WAITING : ���������κ��� �˶� �ڵ尡 ���޵��� ����
				//! [4211] AXT_RT_MOTION_READ_ALARM_NO_REQUEST : �˶� �ڵ� �б� ��û ���� ����.
				//! [4212] AXT_RT_MOTION_READ_ALARM_TIMEOUT : �˶� �ڵ� �б��� �ð� �ʰ� (1�� �̻�)
				//! [4213] AXT_RT_MOTION_READ_ALARM_FAILED : �˶� �ڵ� �б� ����(��Ʈ��ũ ���� ������)
				continue;
			}

			//! �������� �˶� �ڵ带 ����Ͽ� �˶� ���뿡 ���� ���ڿ� ���� Ȯ���Ѵ�.
			//! �˶� �ڵ忡 ���� ���ڿ��� �ùٸ��� ǥ���ϱ� ���ؼ��� ���̺귯���� ���� �����Ǵ� �˶� ���ڿ� �϶� ������ 
			//! Axl.dll�� ���� ������ �ְų� OS ��ġ ���丮 ���� System32 �������� �����Ͽ��� �Ѵ�. 
			//! �׷��� ���� ��� �Լ� ���� ����� ��AXT_RT_MOTION_READ_ALARM_FILES���� ���ϵȴ�. 
			//! �˶� �ڵ忡 ���� ���ڿ� ������ EzSoftwre ��ġ �������� ��AXL(Library)\Library\DefFile���� �ִ�.
			//!  (Alarm-A4N.def, Alarm-A5N.def)
			dwRet = AxmStatusGetServoAlarmString(lAxisNo, upAlarmCode, iBufSize_GetAlarm, szGetAlarm);
			if ( dwRet != AXT_RT_SUCCESS )
			{
				//! [4220] AXT_RT_MOTION_READ_ALARM_UNKNOWN : �߸��� �˶� �ڵ�
				//! [4221] AXT_RT_MOTION_READ_ALARM_FILES : �˶� �ڵ忡 �ش��ϴ� ���ڿ� ������ �ε���� ����.
				switch(dwRet)
				{
				case AXT_RT_MOTION_READ_ALARM_UNKNOWN:
					{
						asAlarm[lAxisNo].Format(_T("[%s] Servo Alarm : Unknown Alarm"), MotorName[lAxisNo]);
					}
					break;
				case AXT_RT_MOTION_READ_ALARM_FILES:
					{
						asAlarm[lAxisNo].Format(_T("[%s] Servo Alarm : Def File not exist"), MotorName[lAxisNo]);
					}
					break;
				default:
					{
						asAlarm[lAxisNo].Format(_T("[%s] Servo Alarm : Error"), MotorName[lAxisNo]);
					}
					break;
				}

				continue;
			}

			asAlarm[lAxisNo].Format(_T("[%s] Servo Alarm : %s"), MotorName[lAxisNo], szGetAlarm);
			LogSave(asAlarm[lAxisNo]);

			Sleep(200);
		}//! for ( i = 0; i < AxisCount; i++ )

	}//! while (bThreadServoAlarm)

	bThreadServoAlarmRun = false;

	return true;
}

//! Modified by LHW (2013/2/25)
//! CCM ���� ���� Thread
//! [���� ����] Model�� ����Ǹ�, Thread�� �ݰ�, �ٽ� ������Ѿ� �մϴ�.
UINT Thread_Ccm_Grab(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	if(pFrame == NULL)
	{
		bThreadCcmGrab = false;
		bThreadCcmGrabRun = false;
		return 1;
	}

	CString sMessage;
	if(gMIUDevice.nWidth < 100 || gMIUDevice.nHeight < 100)
	{
		sMessage.Format(_T("MIL ���� ���� ȣ�� ũ�Ⱑ �������Դϴ�."));
		errMsg2(Task.AutoFlag,sMessage);
		return 1;
	}

	vision.MiuBufferFree();

	if(!vision.MiuBufferAlloc(gMIUDevice.nWidth, gMIUDevice.nHeight))
	{
		errMsg2(Task.AutoFlag,"MIL ���� ���� ���� ����.");
		return 1;
	}
	

	int tmpCnt = 0;

	int    iIndexDevice=0;
	int    errorCode =0;
//	INT64	TimeStamp;
	bThreadCcmGrab = true;
	bThreadCcmGrabRun = true;

	bool saveFlag = false;
		
	try
	{
		while(bThreadCcmGrab)
		{
			if ( pFrame == NULL )
			{
				break;
			}

			if ( gMIUDevice.bMIUOpen != 1 )
			{
				Sleep(500);
				continue;
			}

			if(gMIUDevice.CurrentState==0)
			{
				Sleep(10);
				continue;
			}

			if(gMIUDevice.CurrentState==4)
			{
				pFrame->m_labelUsbLive.SetBkColor(M_COLOR_GREEN);
				pFrame->m_labelUsbLive.Invalidate();
			}
			else
			{
				pFrame->m_labelUsbLive.SetBkColor(M_COLOR_RED);
				pFrame->m_labelUsbLive.Invalidate();
			}


			if( gMIUDevice.CurrentState < 3 )
			{
				Sleep(10);
				continue;
			}

			iIndexDevice = MIU.GetIndexDevice();
			if ( iIndexDevice < 0 )
			{
				Sleep(500);
				continue;
			}

			if ( MIU.m_iIndex_Grab_Working < 0 )
			{
				Sleep(500);
				continue;
			}

			if ( MIU.m_iIndex_Grab_Working == MIU.m_iIndex_Grab_Ready )
			{
				//! �ش� ������ ������ ���� ȭ�鿡 ǥ������ ����
				Sleep(5);
				continue;
			}


			//! ���� ��� ������ ���°� �ӽ� ���� 
			//! [���� ����] 1 Frame�� �� ���� ���� Display�ϴ� ��ɿ� �ʿ�, Process�� ���� ������ �߿��ϴ�.
			if ( gMIUDevice.CurrentState == 3)
			{
				gMIUDevice.CurrentState = 2;
			}

			//! ���� ��� ���� Thread�� ��� �ð� ���� ����
			MIU.Init_Grab_Time(0);
			MIU.Start_Grab_Time(0);

			//! ���� ��� �ð��� �˾Ƴ���.
			::GetLocalTime( &(MIU.m_aTemp[ MIU.m_iIndex_Grab_Working ].TimeGrab) );
			try
			{
				//! ���� ���
				if (MIU.m_pBoard->GetFrame(MIU.m_pFrameRawBuffer, MIU.m_pFrameBMPBuffer))
				{
					//gMIUDevice.imageGrayItp->imageData = (char*)MIU.m_pFrameBMPBuffer; 
					gMIUDevice.imageItp->imageData = (char*)MIU.m_pFrameBMPBuffer;
					//cvSaveImage("D:\\m_pFrameBMPBuffer.bmp", gMIUDevice.imageGrayItp);	 
					MIU.Grab_StopImage();

					//! ���� ��� ���� Thread�� ��� �ð� ����
					MIU.Measure_Grab_Time(0);

					//! ���� ��� ���� Thread�� ��� �ð� ���� ����
					MIU.Init_Grab_Time(1);
					MIU.Start_Grab_Time(1);

					//! ���� ����� ��ġ��, �ش� ������ ���� index�� �˷��� �Ŀ�, 
					MIU.m_iIndex_Grab_Ready = MIU.m_iIndex_Grab_Working;
					//! ���� ������ ������ ����� �� �ֵ���, index ����
					(MIU.m_iIndex_Grab_Working)++;
					//! ������ ũ�⸦ �Ѿ��, ���� index�� �ʱ�ȭ
					if (MIU.m_iIndex_Grab_Working >= iBufSize_CCM_Temp)
					{
						MIU.m_iIndex_Grab_Working = 0;
					}
				}
			}
			catch (std::exception& e)//catch (CException *e)//catch (CFileException *e)
			{
				//e->ReportError();
			}

			Sleep(5);
		}
	}
	catch (CException* e)
	{
		TCHAR czCause[255];
		e->GetErrorMessage(czCause, sizeof(czCause));
		TRACE(_T("ThreadFunc_CCM_Grab - CException [%s] \n"), czCause);
		e->Delete();
	}


	gMIUDevice.CurrentState = -1;

	bThreadCcmGrab = false;
	bThreadCcmGrabRun = false;

	return 1;
}

//! Modified by LHW (2013/2/25)
//! CCM�ο��� Color ��ȯ �۾��� Thread
//! [���� ����] Model�� ����Ǹ�, Thread�� �ݰ�, �ٽ� ������Ѿ� �մϴ�.
UINT Thread_Ccm_CvtColor(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	if(pFrame == NULL)
	{
		bThreadCcmGrab = false;
		bThreadCcmGrabRun = false;
		return 1;
	}
	
	bThreadCcmGrab = true;
	bThreadCcmGrabRun = true;

	try
	{
		while(bThreadCcmGrab)
		{
			if ( pFrame == NULL )
			{
				break;
			}

			if ( gMIUDevice.bMIUOpen != 1 )
			{
				Sleep(500);
				continue;
			}

			if(gMIUDevice.CurrentState==0)
			{
				Sleep(10);
				continue;
			}

			if(gMIUDevice.CurrentState<3)
			{
				Sleep(10);
				continue;
			}

			if ( MIU.m_iIndex_Grab_Ready < 0 )
			{
				Sleep(50);
				continue;
			}

			if ( MIU.m_iIndex_Grab_Ready == MIU.m_iIndex_Grab_Used )
			{
				Sleep(5);
				continue;
			}


			//! ���� ��� ������ ���°� �ӽ� ����
			//! [���� ����] 1 Frame�� �� ���� ���� Display�ϴ� ��ɿ� �ʿ�, Process�� ���� ������ �߿��ϴ�.
			//MIU.m_aTemp[ MIU.m_iIndex_Cvt_Clr_Working ].CurrentState = MIU.m_aTemp[ MIU.m_iIndex_Grab_Ready ].CurrentState;

			//! ���� ó������ ����, ���� ������ �����Ƿ�, 

			//! ���� ��� ���� Thread�� ��� �ð� ����
			MIU.Measure_Grab_Time(1);

			//! ���� ��� ���� Thread�� ��� �ð� ���� ����
			MIU.Init_Grab_Time(2);
			MIU.Start_Grab_Time(2);

			MIU.m_iIndex_Cvt_Clr_Working = MIU.m_iIndex_Grab_Ready;

			MIU.Measure_Grab_Time(2);

			//! ���� ��� ���� Thread�� ��� �ð� ���� ����
			MIU.Init_Grab_Time(3);
			MIU.Start_Grab_Time(3);

			MIU.m_iIndex_Grab_Used = MIU.m_iIndex_Grab_Ready;

			MIU.m_iIndex_Cvt_Clr_Used = MIU.m_iIndex_Cvt_Clr_Working;

			Sleep(5);
		}
	}
	catch (CException* e)
	{
		TCHAR czCause[255];
		e->GetErrorMessage(czCause, sizeof(czCause));
		TRACE(_T("Thread_Ccm_CvtColor - CException [%s] \n"), czCause);
		e->Delete();
	}
	
	gMIUDevice.CurrentState = -1;

	bThreadCcmGrab = false;
	bThreadCcmGrabRun = false;

	return 1;
}

//! Modified by LHW (2013/2/25)
//! CCM�ο����� Open CV --> Mil �̹��� ���� ��ȯ�� Thread
//! [���� ����] Model�� ����Ǹ�, Thread�� �ݰ�, �ٽ� ������Ѿ� �մϴ�.
UINT Thread_Ccm_CvtMil(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	//SYSTEMTIME	sysTime;

	if(pFrame == NULL)
	{
		bThreadCcmGrab = false;
		bThreadCcmGrabRun = false;
		return 1;
	}
		
	bThreadCcmGrab = true;
	bThreadCcmGrabRun = true;

	try
	{
		while(bThreadCcmGrab)
		{
			if ( pFrame == NULL )
			{
				break;
			}

			if ( gMIUDevice.bMIUOpen != 1 )
			{
				Sleep(500);
				continue;
			}

			if(gMIUDevice.CurrentState==0)
			{
				Sleep(100);
				continue;
			}

			if(gMIUDevice.CurrentState<3)
			{
				Sleep(10);
				continue;
			}

			if ( MIU.m_iIndex_Cvt_Clr_Used < 0 )
			{
				Sleep(100);
				continue;
			}

			if ( MIU.m_iIndex_Cvt_Clr_Used == MIU.m_iIndex_Cvt_Mil_Working )
			{
				Sleep(5);
				continue;
			}

			//! ���� ��� ������ ���°� �ӽ� ����
			//! [���� ����] 1 Frame�� �� ���� ���� Display�ϴ� ��ɿ� �ʿ�, Process�� ���� ������ �߿��ϴ�.
			//MIU.m_aTemp[ MIU.m_iIndex_Cvt_Mil_Working ].CurrentState = MIU.m_aTemp[ MIU.m_iIndex_Cvt_Clr_Used ].CurrentState;

			MIU.m_iIndex_Cvt_Mil_Working = MIU.m_iIndex_Cvt_Clr_Used;

			//! ���� ��� ���� Thread�� ��� �ð� ����
			MIU.Measure_Grab_Time(3);

			//! ���� ��� ���� Thread�� ��� �ð� ���� ����
			MIU.Init_Grab_Time(4);
			MIU.Start_Grab_Time(4);

			//! RGB ä�� �и�..
			vision.m_acsGrabLock[1].Lock();



			//! ���� ��� �ð��� �����Ѵ�.
			CopySystemTime(MIU.m_aTemp[ MIU.m_iIndex_Cvt_Mil_Working ].TimeGrab, vision.m_aTimeGrab[1]);


			vision.m_acsGrabLock[1].Unlock();

			MIU.Measure_Grab_Time(4);

			//! ���� ��� ���� Thread�� ��� �ð� ���� ����
			MIU.Init_Grab_Time(5);
			MIU.Start_Grab_Time(5);

			/*MbufPut(vision.MilProcImageChild[3], MIU.m_acvChildImage[0]->imageData);
			MbufPut(vision.MilProcImageChild[4], MIU.m_acvChildImage[1]->imageData);
			MbufPut(vision.MilProcImageChild[5], MIU.m_acvChildImage[2]->imageData);*/

			MIU.m_iIndex_Cvt_Mil_Used = MIU.m_iIndex_Cvt_Mil_Working;			
			
			Sleep(5);
		}
	}
	catch (CException* e)
	{
		TCHAR czCause[255];
		e->GetErrorMessage(czCause, sizeof(czCause));
		TRACE(_T("Thread_Ccm_CvtMil - CException [%s] \n"), czCause);
		e->Delete();
	}
	
	gMIUDevice.CurrentState = -1;

	bThreadCcmGrab = false;
	bThreadCcmGrabRun = false;

	return 1;
}


//! Modified by LHW (2013/2/25, 2013/3/27)
//! CCM ���� Display�� Thread
//! [���� ����] Model�� ����Ǹ�, Thread�� �ݰ�, �ٽ� ������Ѿ� �մϴ�.
UINT Thread_Ccm_Display(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	if(pFrame == NULL)
	{
		bThreadCcmGrab = false;
		bThreadCcmGrabRun = false;
		return 1;
	}
	
	bThreadCcmGrab = true;
	bThreadCcmGrabRun = true;

	double dReduceFactorX = 0.;
	double dReduceFactorY = 0.;
	
	try
	{
		while(bThreadCcmGrab)
		{
			if ( pFrame == NULL )
			{
				break;
			}

			if ( gMIUDevice.bMIUOpen != 1 )
			{
				Sleep(500);
				continue;
			}

			if(gMIUDevice.CurrentState==0)
			{
				Sleep(100);
				continue;
			}

			if(gMIUDevice.CurrentState<3)
			{
				Sleep(10);
				continue;
			}

			if ( MIU.m_iIndex_Cvt_Mil_Used < 0 )
			{
				Sleep(100);
				continue;
			}
			
			if ( MIU.m_iIndex_Cvt_Mil_Used == MIU.m_iIndex_Display_Working )
			{
				Sleep(5);
				continue;
			}

			if ( gMIUDevice.nWidth <= 0 || gMIUDevice.nHeight <= 0 )
			{
				Sleep(100);
				continue;
			}

			//! ���� ��� ���� Thread�� ��� �ð� ����
			MIU.Measure_Grab_Time(5);

			MIU.Init_Grab_Time(6);
			MIU.Start_Grab_Time(6);

			//! ���� ��� ������ ���°� �ӽ� ����
			//! [���� ����] 1 Frame�� �� ���� ���� Display�ϴ� ��ɿ� �ʿ�, Process�� ���� ������ �߿��ϴ�.

			//! ���� ó������ ����, ���� ������ �����Ƿ�, 

			dReduceFactorX = (double)SMALL_CCD_SIZE_X / gMIUDevice.nWidth;
			dReduceFactorY = (double)SMALL_CCD_SIZE_Y / gMIUDevice.nHeight;

			MIU.m_iIndex_Display_Working = MIU.m_iIndex_Cvt_Mil_Used;

			pFrame->Update_CCD_Display();

			MIU.Measure_Grab_Time(6);

			//! Display�� Frame Rate ����� ���� ȣ���Ѵ�. 
			MIU.Add_Display_Count();	

			if ( bFlag_First_Grab_Display == false )
			{
				//! Thread�� �����ǰ� ����, ù��°�� ���� ��濡 �����ϴ�.
				bFlag_First_Grab_Display = true;
			}

			Sleep(5);
		}
	}
	catch (CException* e)
	{
		TCHAR czCause[255];
		e->GetErrorMessage(czCause, sizeof(czCause));
		TRACE(_T("Thread_Ccm_Display - CException [%s] \n"), czCause);
		e->Delete();
	}
	
	gMIUDevice.CurrentState = -1;

	bThreadCcmGrab = false;
	bThreadCcmGrabRun = false;

	return 1;
}

UINT Thread_TaskReady(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	CString logStr="";
	CString sTemp="";

	vision.setLiveMode(true);
	int delayTime = 5;


	if( bThreadTaskLensRun == true ||  bThreadTaskPcbRun == true )
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//"�ڵ� ���� �� �Դϴ�."
		delayMsg(sLangChange);
		return 0;
	}

	if( Task.AutoFlag == 2 )
	{
		sLangChange.LoadStringA(IDS_STRING1362);	//"�Ͻ� ���� �� �Դϴ�."
		errMsg2(Task.AutoFlag, sLangChange);
		return 0;
	}

	if ( bThreadTaskReadyRun == true)
	{
		sLangChange.LoadStringA(IDS_STRING1322);	//"���� �غ� ���� �� �Դϴ�."
		errMsg2(Task.AutoFlag, sLangChange);
		return 0;
	}

	if(g_bMovingflag)
	{
		sLangChange.LoadStringA(IDS_STRING1324);	//"���� �غ� ���� - ��� ���� �� �Դϴ�."
		sTemp.Format(sLangChange);
		errMsg2(Task.AutoFlag, sTemp);
		return 0;
	}

	g_bMovingflag =true;

	bThreadTaskReady = true;
	bThreadTaskReadyRun = true;
	iReadyRunCnt = 0;
	Task.m_iStatus_Unit_Epoxy = 1;

	for(int i=0; i<MAX_MOTOR_NO; i++)
	{
		if(motor.m_bOrgFlag[i]==false)
		{
			bThreadTaskReady = false;
			break;
		}
	}
	int i_alarm_flag;

	while(bThreadTaskReady)
	{
		if(Task.AutoFlag != 1)
		{
			sLangChange.LoadStringA(IDS_STRING402);	//"AutoRunFlag OFF ���� . Lens�� step :-1"
			logStr.Format(sLangChange);
			pFrame->putListLog(logStr);
			break;
		}

		i_alarm_flag = pFrame->checkAutoRunLensAlarm(Task.ReadyTask);

		if(i_alarm_flag != 0)
		{
			/*Dio.setAlarm(ALARM_ON);*/
			Task.ReadyTask = -abs(Task.ReadyTask);
			Task.AutoFlag = 0;
			pFrame->AutoRunView(Task.AutoFlag);

			g_bMovingflag = false;
		}


		if(Task.AutoFlag)
		{
			if ( Task.ReadyTask >= 10000 && Task.ReadyTask <19900 )
			{
				Task.ReadyTask = theApp.MainDlg->pcbProcess.Ready_process(Task.ReadyTask);		/* ���� �ε� -> ���� ����� -> ���� ���� ��ġ */	
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING309);	//"[���� �غ�]���� �غ� �Ϸ� �߽��ϴ�."
				pFrame->putListLog(sLangChange);
				break;
			}
		}


		if ( Task.ReadyTask < 0 ){
			break;
		}
		Sleep(5);
	}
	if ( Task.ReadyTask == 19900 )
	{
		sLangChange.LoadStringA(IDS_STRING1321);	//"���� �غ� ���� �Ϸ�."
		logStr.Format(sLangChange);
		pFrame->putListLog(logStr);
	}
	else
	{
		sLangChange.LoadStringA(IDS_STRING1325);	//	"���� �غ� ����."
		logStr.Format(sLangChange);
		pFrame->putListLog(logStr);
	}

	if(Task.ReadyTask == 19900)
	{
		bThreadTaskReadyRun = false;		// �����尡 ���� �ߴ��� Ȯ��..

		pFrame->AutoRunView(3);
	}
	else
	{
		bThreadTaskReadyRun = false;		// �����尡 ���� �ߴ��� Ȯ��..

		pFrame->AutoRunView(0);
	}

	Task.AutoFlag = 0;

	bThreadTaskReady = false;			// ������ ���� ���� ��..
	bThreadTaskReadyRun = false;		// �����尡 ���� �ߴ��� Ȯ��..

	g_bMovingflag = false;
	Task.m_iStatus_Unit_Epoxy = 0;

	return true;
}


UINT Thread_MIUCheck(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
	CString logStr="";

	
	bThread_MIUCheckRun=true;
	if( !pFrame->MIUCheck_process())// || gMIUDevice.CurrentState != 4)
	{
		logStr.Format("CCD ��� ���� �ʱ�ȭ ����.\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.");
		errMsg2(Task.AutoFlag, logStr);
		bThread_MIUCheckRun=false;
		return false;
	}
	Sleep(300);

	///MIU.SwitchHDRToLinearMode();		//zoox �𵨸�

	bThread_MIUCheckRun=false;

	if(Task.PCBTask > 60000 && Task.MUICheckflag == false)
	{
		//sLangChange.LoadStringA(IDS_STRING446);	//"CCD ������ ����."
		//errMsg2(Task.AutoFlag, sLangChange);
		//return false;
	}
	return true;
}

UINT Thread_Monitor(LPVOID parm)
{
#ifndef ON_LINE_MODE
	return true;
#endif
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	bThreadMonitor		= true;
	bThreadMonitorRun	= true;

	int iOldHomeErrorFlag, oldAmpFaultFlag, iOldBuzzerFlag, i_Old_EM_SwitchFlag, iOldDoorOpenFlag, iOldMainAirFlag, iOldLimitFlag ;
	iOldHomeErrorFlag = oldAmpFaultFlag = iOldBuzzerFlag = i_Old_EM_SwitchFlag = iOldDoorOpenFlag = iOldMainAirFlag = iOldLimitFlag = -1;

	bool iOldThread1=false, iOldThread2=false;

	char sLog[100];

	while(bThreadMonitor)
	{
		motor.InDIO(0, curInDio[0]);
		motor.InDIO(2, curInDio[1]);

		sprintf_s(sLog,"Ready %d", Task.ReadyTask);
		pFrame->m_labelThread1.SetText(sLog);

		if ( Task.ReadyTask == 19900 )
		{
			pFrame->m_labelThread1.SetBkColor(M_COLOR_GREEN);
		}
		else
		{
			pFrame->m_labelThread1.SetBkColor(M_COLOR_GRAY);
		}

		iOldThread1 = bThreadTaskLensRun;

		pFrame->m_labelThread1.Invalidate();

		sprintf_s(sLog, "Pcb %d", Task.PCBTask);
		pFrame->m_labelThread2.SetText(sLog);

		if ( iOldThread2 != bThreadTaskPcbRun )
		{
			if ( bThreadTaskPcbRun )
			{
				pFrame->m_labelThread2.SetBkColor(M_COLOR_GREEN);
			}
			else
			{
				pFrame->m_labelThread2.SetBkColor(M_COLOR_GRAY);
			}

			iOldThread2 = bThreadTaskPcbRun;

			pFrame->m_labelThread2.Invalidate();
		}
		
////////////////////////////////////////////////////////////////////////////////
// ���� ���� ���� Ȯ��..

		Task.iHomeErrorFlag = motor.HomeCheck();

		//if(Task.iHomeErrorFlag)
		//{
		//	if(!delayDlg->IsWindowVisible())
		//	{
		//		//sLangChange.LoadStringA(IDS_STRING1399);	//��ü ���� ���͸� �Ϸ����� �� �߽��ϴ�.
		//		//delayMsg(sLangChange.GetBuffer(99), 5000, M_COLOR_RED);
		//	}
		//}

		if(iOldHomeErrorFlag != Task.iHomeErrorFlag)
		{
			if(Task.iHomeErrorFlag && !iOldHomeErrorFlag)
			{
				pFrame->m_labelHom.SetBkColor(M_COLOR_RED);

				pFrame->m_btnOrigin.m_iStateBtn =2;
				pFrame->m_btnOrigin.Invalidate();

				if(Task.AutoFlag)
				{
					Dio.setAlarm(ALARM_ON);
					sLangChange.LoadStringA(IDS_STRING1399);	//��ü ���� ���͸� �Ϸ����� �� �߽��ϴ�.
					pFrame->putListLog(sLangChange);
					pFrame->Invalidate();
				}
			}
			else if(!Task.iHomeErrorFlag && iOldHomeErrorFlag)
			{
				pFrame->m_labelHom.SetBkColor(M_COLOR_GREEN);
				pFrame->Invalidate();

				pFrame->m_btnOrigin.m_iStateBtn = 3;
				pFrame->m_btnOrigin.Invalidate();
			}
			iOldHomeErrorFlag = Task.iHomeErrorFlag;
		}

////////////////////////////////////////////////////////////////////////////////
// ���� ����̺� �˶� Display

		int iAmpfault = 0;
		//for(int i=0;i<MAX_MOTOR_NO;i++)
		//{
		//	//if(motor.GetAmpFault(i))
		//	//{
		//	//	iAmpfault += i+1;
		//	//	if(!delayDlg->IsWindowVisible())
		//	//	{
		//	//		//sLangChange.LoadStringA(IDS_STRING1053);	//Servo Alarm �� ���� �Ǿ� �ֽ��ϴ�.
		//	//		//delayMsg(sLangChange.GetBuffer(99), 5000, M_COLOR_RED);
		//	//	}
		//	//}
		//}

		if(iAmpfault == 0){
			pFrame->m_labelServo.SetBkColor(M_COLOR_GREEN);
		}else{
			pFrame->m_labelServo.SetBkColor(M_COLOR_RED);
		}
		if(iAmpfault != oldAmpFaultFlag){
			pFrame->	m_labelServo.Invalidate();
		}
		Task.iAmpFaultFlag = oldAmpFaultFlag = iAmpfault; 


////////////////////////////////////////////////////////////////////////////////
// Limit ���� ���� Display
#if 1
 		//for(int i=0; i<MAX_MOTOR_NO; i++)
 		//{
			//if ( motor.GetNegaSensor(i) )
 		//	{
			//	if (bThreadTaskReadyRun == true || i == Motor_PCB_Z)// && i==Motor_Lens_Yt)
			//		continue;
 		//		Task.iLimitErrorFlag =1;
			//	break;
 		//	}
			//else 
			//	Task.iLimitErrorFlag =0;
			//
			//if ( motor.GetPosiSensor(i) ) 
 		//	{
			//	if(i == Motor_Lens_Z)continue;
 		//		Task.iLimitErrorFlag =1;
			//	break;
 		//	}
			//else			Task.iLimitErrorFlag =0;
 
			//if (motor.GetAmpFault(i))
			//{
			//	sTempLang.LoadStringA(IDS_STRING152);	//[%s] AMP �˶� ����
			//	sLangChange.Format(sTempLang, MotorName[i]);
			//	sprintf_s(sLog, sLangChange);
			//	if (!delayDlg->IsWindowVisible())				//delayMsg(sLog,3000,M_COLOR_RED);
			//		Task.iLimitErrorFlag = 1;
			//	break;
			//}
			//else			Task.iLimitErrorFlag =0;
			//Sleep(15);
 		//}
#endif

		if(Task.iLimitErrorFlag ==1 && Task.AutoFlag)
		{
			CString sLog2="";
			sLog2.Format("%s",sLog);
			errMsg2(Task.AutoFlag, sLog2);
			Task.AutoFlag = 0;			//	�Ͻ������� ����� 0: �Ͻ����� 1: Run	
			pFrame->AutoRunView(Task.AutoFlag);
			bThreadTaskPcb =false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
			bThreadTaskLens = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
			bThreadTaskLens_Align = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
		}

		if(iOldLimitFlag != Task.iLimitErrorFlag)
		{
			if(Task.iLimitErrorFlag ==1 && Task.AutoFlag)
			{
				Dio.setAlarm(ALARM_ON);
				Task.AutoFlag = 0;			//	�Ͻ������� ����� 0: �Ͻ����� 1: Run	
				pFrame->AutoRunView(Task.AutoFlag);
				bThreadTaskPcb = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
				bThreadTaskLens = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
				bThreadTaskLens_Align = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
			}

			iOldLimitFlag = Task.iLimitErrorFlag;
		}

		////////////////////////////////////////////////////////////////////////////////
		// Door Open ����..
		Task.iDoorFlag = 0x00;
        int doorFlag = 0x00;
		/*if(curInDio[0] & DIO_IN_DOORSENSOR1)			Task.iDoorFlag += 0x0001;
		if(curInDio[0] & DIO_IN_DOORSENSOR2)			Task.iDoorFlag += 0x0002;
		if(curInDio[0] & DIO_IN_DOORSENSOR3)			Task.iDoorFlag += 0x0004;
        if (curInDio[0] & DIO_IN_DOORSENSOR4)			Task.iDoorFlag += 0x0008;
        if (curInDio[0] & DIO_IN_DOORSENSOR5)			Task.iDoorFlag += 0x0010;*/
        doorFlag = 0x0007;
		if(iOldDoorOpenFlag != Task.iDoorFlag) 
		{
			if (Task.iDoorFlag != doorFlag) //0x003F = 63  //0x001F
			{
				pFrame->m_LabelDoor.SetBkColor(M_COLOR_RED);
				Task.iDoorFlag2 = 1;//door ���� �� �ݰ�������
			}
			else
			{
				pFrame->m_LabelDoor.SetBkColor(M_COLOR_GREEN);
				Task.iDoorFlag2 = 0;
			}
			pFrame->m_LabelDoor.Invalidate();	
		}
		iOldDoorOpenFlag = Task.iDoorFlag;


//-- ������� �߿� PCB ������ ���� ������.. ���� �������� �������� ����
		//if(Task.AutoFlag == 0 && gMIUDevice.CurrentState == 4)
		//{
		//	if( !func_Insp_CurrentMeasure(false))
		//	{//������ ����
		//		gMIUDevice.CurrentState = 3;

		//		pFrame->m_csLock_Miu.Lock();
		//		if(!MIU.Stop())
		//		{
		//			pFrame->putListLog(MIU.sLogMessage);
		//			delayMsg(MIU.sLogMessage.GetBuffer(999));
		//		}
		//		else
		//		{
		//			sLangChange.LoadStringA(IDS_STRING1424);	//ī�޶� ��� ��� Close.
		//			pFrame->putListLog(sLangChange);
		//		}

		//		pFrame->m_bMiuRun = false;
		//		pFrame->m_csLock_Miu.Unlock();
		//	}
		//}

//-- ���ڵ� Reading
		CString sBarCode = "";
		if( barcode.func_Barcode_Read(sBarCode) ) 
		{	//���ڵ尡 ���� ������...
			CString sData="";
			int FindR = sBarCode.ReverseFind('\r');	//12
			int FindN = sBarCode.ReverseFind('\n');//13
			int totalSize = sBarCode.GetLength();
			if (FindR > 0 || FindN > 0)
			{ 
				if (totalSize > FindR && FindR > 0)
				{
					totalSize = FindR;
				}

				if (totalSize > FindN && FindN > 0)
				{
					totalSize = FindN;
				}
			}


			sData = sBarCode.Mid(0, totalSize);///// sBarCode.GetLength() - 2);
			//if(len > 12)
			//{
			//	//sData = sBarCode.Mid(0, 12);
			//}
			//else	
			//{
			//	//sData = sBarCode;
			//}
			if( Task.AutoFlag == 1 )
			{
				if( (Task.PCBTask > 10000) &&  (Task.PCBTask < 10180) )
				{//Loading ���� �� �ν� Ȯ�α����� Data Task�� Push
					Task.m_bPBStart = 1;	//���ڵ� ���������� ���� ������.		 
					sprintf_s(Task.ChipID, sData, sizeof(sData) );
					pFrame->func_ChipID_Draw();	
				}
			}
			else if( Task.AutoFlag == 0 )
			{//����(����)���¿����� �׻� Push
				Task.m_bPBStart = 1;	//���ڵ� ���������� ���� ������.		
				sprintf_s(Task.ChipID, sData, sizeof(sData) );
				pFrame->func_ChipID_Draw();	
			}
		}

		//���� �����̵� ����

		if ((Task.AutoFlag == 1) && (motor.bAxdConnect == true))	//������ ���
		{
			if (Dio.StartPBOnCheck(true, false))	Dio.StartPBLampOn(true);
			else									Dio.StartPBLampOn(false);

			if (Dio.DoorPBOnCheck(true, false))
			{
				Dio.DoorPBLampOn(true);
				Dio.DoorLift(true, false);
			}
			//else
			//{
			//	if (pFrame->bLightCurtain == false && Task.AutoFlag == 1)// && sysData.m_iFront==0
			//	{//���� Ŀư ���� �ȵǾ��� ��쿡�� Close
			//		Dio.DoorPBLampOn(false);
			//		Dio.DoorLift(false, false);
			//	}
			//}
		}
		else
		{
			if (Dio.DoorPBOnCheck(true, false))
			{
				Dio.DoorPBLampOn(true);
				Dio.DoorLift(true, false);
				if (sysData.m_iFront == 0)
				{
					Dio.DoorLift(true, false);
				}
			}
		}
		if (Task.AutoFlag == 1 && gMIUDevice.CurrentState == 1)
		{
			if (Dio.LightCurtainOnCheck(true, false))//���� Ŀư ���������� ���� �ɶ�   ���� �۵� �� 250115
			{
				if (!(curInDio[1] & DIO_IN_DOOR_CLOSE))//Door Lift Close�� ���� �ȵ� ��� ������ Lift �ϰ� - �� ���� ���Ͷ�
				{
					pFrame->bLightCurtain = true;
					Dio.DoorLift(true, false);
				}
				else
				{
					Dio.DoorLift(false, false);
					pFrame->bLightCurtain = false;
				}
			}
			else
			{
				Dio.DoorLift(false, false);
				pFrame->bLightCurtain = false;
			}
		}
		if ((Task.PCBTask > 11000) && (Task.PCBTask < 110000))
		{
			if (Dio.LightCurtainOnCheck(true, false))//���� Ŀư ���������� ���� �ɶ�   ���� �۵� �� 250115
			{
				if (!(curInDio[1] & DIO_IN_DOOR_CLOSE))//Door Lift Close�� ���� �ȵ� ��� ������ Lift �ϰ� - �� ���� ���Ͷ�
				{
					pFrame->bLightCurtain = true;
					Dio.DoorLift(true, false);
				}
				else
				{
					Dio.DoorLift(false, false);
					pFrame->bLightCurtain = false;
				}
			}
			else
			{
				Dio.DoorLift(false, false);
				pFrame->bLightCurtain = false;
			}
		}
		

		Sleep(1);
	}

	bThreadMonitorRun = false;

	return true;
}

UINT Thread_TaskOrigin(LPVOID parm)
{
	bThreadOriginRun = true;
	CString sTemp = _T("");
	
	sTemp.Format("��ü ���� ���� �����մϴ�");	//��ü ���� ���� �����մϴ�
	delayMsg(sTemp, 50000, M_COLOR_DARK_GREEN);
	theApp.MainDlg->putListLog(sTemp);
	Sleep(100);
	//
	memset(Task.ChipID, 0x00, 256);
	sprintf_s(Task.ChipID, "EMPTY");
	Task.m_bPBStart = 0;	//���ڵ� ���� �ʱ�ȭ
	theApp.MainDlg->func_ChipID_Draw();



	bool bHomeComplete = false;

	bHomeComplete = motor.HomeProcessAll();

	if (bHomeComplete)
	{
		Dio.setAlarm(ALARM_OFF);
		sTemp.Format("��ü ���� ���� �Ϸ�");
		delayMsg(sTemp.GetBuffer(99), 50000, M_COLOR_GREEN);
	}
	else
	{
		sTemp.Format("��ü ���� ���� ����");
		delayMsg(sTemp.GetBuffer(99), 50000, M_COLOR_RED);
	}
	theApp.MainDlg->putListLog(sTemp);
	g_bMovingflag = false;
	bThreadOriginRun = false;
	return 1;
}
UINT Thread_TaskPcb(LPVOID parm)
{
	CString logStr="";
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	 
	vision.setLiveMode(true);

	pFrame->putListLog(""); 
	pFrame->putListLog("");  


 	pFrame->m_iCurCamNo =  0;
	if(Task.PCBTask >= 60000)
	{
		pFrame->ctrlSubDlg(MAIN_DLG);
		pFrame->changeMainBtnColor(MAIN_DLG);
		pFrame->setCamDisplay(3,1);		
	}
	else
	{
 		pFrame->ctrlSubDlg(MAIN_DLG);
		pFrame->changeMainBtnColor(MAIN_DLG);
	}

	vision.clearOverlay(CAM1);
	vision.drawOverlay(CAM1);


	if ( bThreadTaskPcbRun == true)
	{
		pFrame->putListLog("PCB �����尡 ���� �� �Դϴ�.");
		return 0;
	}

	int i_alarm_flag = pFrame->checkAutoRunPcbAlarm(Task.PCBTask);

	if(i_alarm_flag)
	{
		logStr.Format("[���� ����] %s", pFrame->sz_PCB_Error_Msg);
		errMsg2(Task.AutoFlag,logStr);

		Task.AutoFlag = 0;
		pFrame->AutoRunView(Task.AutoFlag);
		Task.PCBTask = 0;
		bThreadTaskPcb = false;
		bThreadTaskPcbRun = false;
		g_bMovingflag = false;

		return 1;
	}


	bThreadTaskPcb = true;
	bThreadTaskPcbRun = true;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	Task.m_iHour	= sysTime.wHour;
	Task.m_iMin		= sysTime.wMinute;
	Task.m_iSec		= sysTime.wSecond;

	int oldTask = 0;
	pFrame->DisableButton(true);

	while(bThreadTaskPcb && (Task.PCBTask >= Task.m_iStart_Step_PCB) && (Task.PCBTask < Task.m_iEnd_Step_PCB))
	{
		if(Task.AutoFlag != 1)			//0:���� 1: �ڵ� 
		{
			logStr.Format("AutoRunFlag OFF ���� . PCB�� step :-1");	//AutoRunFlag OFF ���� . PCB�� step :-1
			pFrame->putListLog(logStr);
			break;
		}
		
		
		i_alarm_flag = pFrame->checkAutoRunPcbAlarm(Task.PCBTask);

		if(i_alarm_flag != 0)
		{
			Dio.setAlarm(ALARM_ON);
			delayMsg(pFrame->sz_PCB_Error_Msg, 1000000, M_COLOR_RED);

			Task.AutoFlag = 0;
			pFrame->AutoRunView(Task.AutoFlag);
			Task.PCBTask = -abs(Task.PCBTask);
			continue;
		}
		//���� AA����� ���� 241127
		// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////������
		if (sysData.m_iProductHolderBoning == 1)
		{
			//Ȧ�� ����� �κ�   Figure �𵨸� 241127

			if (Task.PCBTask >= 10000 && Task.PCBTask < 20000)
			{
				//Ȧ�� , ���� ����
				Task.PCBTask = theApp.MainDlg->pcbProcess.HolderRun_Loading(Task.PCBTask);	//ok
			}
			else if (Task.PCBTask >= 20000 && Task.PCBTask < 30000)
			{
				//����
				Task.PCBTask = theApp.MainDlg->pcbProcess.HolderRun_Dispensing(Task.PCBTask);	//ok
			}
			else if (Task.PCBTask >= 30000 && Task.PCBTask < 40000)
			{
				//���� �ε�
				Task.PCBTask = theApp.MainDlg->pcbProcess.HolderRun_LensLoading(Task.PCBTask);	//ok
			}
			else if (Task.PCBTask >= 40000 && Task.PCBTask < 50000)
			{
				//���� + uv
				Task.PCBTask = theApp.MainDlg->pcbProcess.HolderRun_BondingUv(Task.PCBTask);	//ok
			}
			else if (Task.PCBTask >= 50000 && Task.PCBTask < 59000)
			{
				//����
				Task.PCBTask = theApp.MainDlg->pcbProcess.HolderRun_FinalOut(Task.PCBTask);
			}
		}
		else
		{
			if (Task.PCBTask >= 10000 && Task.PCBTask < 11000)
			{
				Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_ProductLoading(Task.PCBTask); //! ����� ��ǰ Loading	
			}
			else if (Task.PCBTask >= 11000 && Task.PCBTask < 49000)
			{
				if (sysData.m_iProductComp == 1)
				{
					Task.PCBTask = theApp.MainDlg->pcbProcess.procProductComplete(Task.PCBTask);	// �ϼ�ǰ �˻� �� ���
				}
				else
				{
#if (____AA_WAY == PCB_TILT_AA)
					if (Task.PCBTask >= 11000 && Task.PCBTask < 15000)			Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_PCBOutsideAlign(Task.PCBTask);		//! PCB �ܺ� Align Step		
					else if (Task.PCBTask >= 15000 && Task.PCBTask < 26000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_LensNewPassPickup(Task.PCBTask);		//! Lens Pickup�� ��ǰ �ѱ� Step
					else if (Task.PCBTask >= 26000 && Task.PCBTask < 27000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_SensorAlign(Task.PCBTask);		//! Sensor Align Step + Laser ����
					else if (Task.PCBTask >= 27000 && Task.PCBTask < 29000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_LaserMeasure(Task.PCBTask);			//pcb ������ ����
					else if (Task.PCBTask >= 30000 && Task.PCBTask < 39000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_EpoxyNewResing(Task.PCBTask);		//! Epoxy ���� �� ���� �˻� Step
					else if (Task.PCBTask >= 39000 && Task.PCBTask <= 40000)	Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_InspAAPos(Task.PCBTask);			//! Defect �˻��� ������ġ Step
#elif (____AA_WAY == LENS_TILT_AA)
					if (Task.PCBTask >= 11000 && Task.PCBTask < 15000)			Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_PCBOutsideAlign(Task.PCBTask);					//Ȧ�� �����
					else if (Task.PCBTask >= 26000 && Task.PCBTask < 27000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_SensorAlign(Task.PCBTask);						//���� �����
					else if (Task.PCBTask >= 27000 && Task.PCBTask < 28000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_LaserMeasure(Task.PCBTask);						//pcb ������ ����
					else if (Task.PCBTask >= 30000 && Task.PCBTask < 35000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_EpoxyNewResing(Task.PCBTask);					//����
					else if (Task.PCBTask >= 35000 && Task.PCBTask < 39000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_LensAlign(Task.PCBTask);							//���� �����
					else if (Task.PCBTask >= 39000 && Task.PCBTask < 40000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_LensLoading(Task.PCBTask);						//���� �ε�
					else if (Task.PCBTask >= 40000 && Task.PCBTask < 41000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_InspAAPos(Task.PCBTask);							//AA
#endif
					else if (Task.PCBTask >= 41000 && Task.PCBTask < 49000)		Task.PCBTask = theApp.MainDlg->pcbProcess.RunProc_Bonding_Pos_GO(Task.PCBTask);					//���� �ε�
				}
			}
			else if (Task.PCBTask >= 49000 && Task.PCBTask < 50000)
			{
				Task.PCBTask = 60000;
				Task.iMTFCnt = 0;
			}
			else if (Task.PCBTask >= 60000 && Task.PCBTask <80000)
			{
				Task.PCBTask = theApp.MainDlg->pcbProcess.procAutoFocus(Task.PCBTask);			//Active Align ����
			}
			else if (Task.PCBTask >= 80000 && Task.PCBTask <110000)
			{
				Task.iMTFCnt = 0;
				Task.bFirstAA = true;
				Task.m_iCnt_1Step_End = Task.m_iCnt_Step_AA_Total;
				Task.PCBTask = 60200;	//2��AA
			}
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if (Task.PCBTask >= 110000 && Task.PCBTask <120000)
			{
				Task.PCBTask = theApp.MainDlg->pcbProcess.UV_process(Task.PCBTask);
			}
			else if (Task.PCBTask >= 120000 && Task.PCBTask <130000)
			{
				if (sysData.m_iProductComp == 1)
				{
					Task.PCBTask = theApp.MainDlg->pcbProcess.Complete_FinalInsp(Task.PCBTask);
				}
				else
				{
					Task.PCBTask = theApp.MainDlg->pcbProcess.func_MandoFinalSFR(Task.PCBTask);
				}
			}
		}
		if (Task.PCBTask >= Task.m_iEnd_Step_PCB)
		{
			//if(!bThreadTaskLens)	//Dio.setAlarm(ALARM_OFF);	// Lens  ������ ���߸� Yellow	
			Dio.setAlarm(ALARM_OFF);	// Lens  ������ ���߸� Yellow
			break;
		}
		Sleep(5);
	}

	pFrame->DisableButton(false);
	pFrame->dispGrid();
	if(Task.m_iEnd_Step_PCB < 100000)
	{
		Task.AutoFlag = 0;
		pFrame->AutoRunView(Task.AutoFlag);
	}

	Task.PausePCBStep = abs(Task.PCBTask);

	bThreadTaskPcb = false;
	bThreadTaskPcbRun = false; 

	Task.PcbOnStage = 100;
	logStr.Format("PCB AA-Bonding ���� ������ ����.");
	pFrame->putListLog(logStr);
	
	Task.m_iHour = Task.m_iMin = Task.m_iSec = 0;

	delayMsg(logStr.GetBuffer(999), 3000, M_COLOR_RED);

	g_bMovingflag = false;
	
	if (Task.AutoFlag == 0)
	{
		MIU.Stop();					// 95 ~ 100 msec
	}					// 95 ~ 100 msec

	
	logStr.Empty();
	return 1;
}

UINT Thread_TaskLens(LPVOID parm)
{
	
	CString logStr="";
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;


	vision.setLiveMode(true);

	pFrame->putListLog("");
	pFrame->putListLog("");


 	pFrame->m_iCurCamNo = 0;
	if(Task.LensTask >= 60000)
	{
		pFrame->ctrlSubDlg(MAIN_DLG);
		pFrame->changeMainBtnColor(MAIN_DLG);
		pFrame->setCamDisplay(3,1);		
	}
	else
	{
 		pFrame->ctrlSubDlg(MAIN_DLG);
		pFrame->changeMainBtnColor(MAIN_DLG);
	}

	vision.clearOverlay(CAM2);
	vision.drawOverlay(CAM2);

	if ( bThreadTaskLensRun == true)
	{
//		errMsg2(Task.AutoFlag, "Lens �����尡 ���� �� �Դϴ�.");
		return 0;
	}

	int i_alarm_flag = pFrame->checkAutoRunLensAlarm(Task.LensTask);

	if(i_alarm_flag)
	{
		sLangChange.LoadStringA(IDS_STRING297); //[���� ����]
		logStr.Format(sLangChange, pFrame->sz_LENS_Error_Msg);
		errMsg2(Task.AutoFlag,logStr);

		Task.AutoFlag = 0;
		pFrame->AutoRunView(Task.AutoFlag);
		//Task.LensTask = 0;
		//bThreadTaskLens = false;
		//bThreadTaskLensRun = false;
		//bThreadTaskLens_Align = false;
		g_bMovingflag = false;

		return 1;
	}


	bThreadTaskLens = true;
	bThreadTaskLensRun = true;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	Task.m_iHour	= sysTime.wHour;
	Task.m_iMin		= sysTime.wMinute;
	Task.m_iSec		= sysTime.wSecond;

	int oldTask = 0;
	pFrame->DisableButton(true);

	while(bThreadTaskLens && (Task.LensTask >= Task.m_iStart_Step_LENS) && (Task.LensTask < Task.m_iEnd_Step_LENS))
	{
		if(Task.AutoFlag != 1)			//0:���� 1: �ڵ� 
		{
			sLangChange.LoadStringA(IDS_STRING403);
			logStr.Format(sLangChange);	//AutoRunFlag OFF ���� . Lens�� step :-1
			pFrame->putListLog(logStr);
			break;
		}
		
		i_alarm_flag = pFrame->checkAutoRunLensAlarm(Task.LensTask);
		 
		if(i_alarm_flag != 0)
		{
			//Dio.setAlarm(ALARM_ON);
			delayMsg(pFrame->sz_LENS_Error_Msg, 1000000, M_COLOR_RED);

			Task.AutoFlag = 0;
			pFrame->AutoRunView(Task.AutoFlag);
			Task.LensTask = -abs(Task.LensTask);

			continue;
		}
		
//�ڡڡڡ�///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		if(Task.LensTask >= 30000 && Task.LensTask < 50000)
		{
			Task.LensTask = theApp.MainDlg->pcbProcess.RunProc_LENS_AlignLaserMeasure(Task.LensTask);		//! LENS Align Step


			//Task.LensTask = theApp.MainDlg->pcbProcess.RunProc_LENS_LensLoad(Task.LensTask);		//! LENS Align Step
			//theApp.MainDlg->pcbProcess.RunProc_PCBOutsideAlign(Task.PCBTask);	
			
		}

//�ڡڡڡ�///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		

		if (Task.LensTask >= Task.m_iEnd_Step_LENS)
		{
			break;
		} 
		

		Sleep(5);
	}

	pFrame->DisableButton(false);

	pFrame->dispGrid();

	if(Task.m_iEnd_Step_LENS < 100000)
	{
//		Task.AutoFlag = 0;
		pFrame->AutoRunView(Task.AutoFlag);
	}

	Task.PauseLensStep = abs(Task.LensTask);

	bThreadTaskLens = false;
	bThreadTaskLensRun = false;

	Task.LensOnStage = 100;
	logStr.Format(_T("Lens AA-Bonding ���� ������ ����."));//sLangChange);
	pFrame->putListLog(logStr);
	
	Task.m_iHour = Task.m_iMin = Task.m_iSec = 0;

	g_bMovingflag = false;
	
	return 1;
}

UINT Thread_Clock(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

	WORD wOldDay = 0;
	CString strTime;
	CString strOldTime;
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	wOldDay = sysTime.wDay;

	bThreadClock = true;
	bThreadClockRun = true;

	while (bThreadClock)
	{
		::GetLocalTime(&sysTime);

		strTime.Format("%02d : %02d : %02d", sysTime.wHour,sysTime.wMinute, sysTime.wSecond);
		if(strOldTime != strTime){
			pFrame->m_labelTime.SetText(strTime);
			strOldTime = strTime;
		}

		Sleep(200);
		checkMessage();
	}
	 
	bThreadClockRun = false;

	return true;
}

UINT Thread_CheckDate(LPVOID parm)
{//----- ��¥�� ���÷� Check�Ͽ� ���ʿ��� �����͸� ������Ų��...
	CAABonderDlg *pFrame = (CAABonderDlg *)(AfxGetApp()->m_pMainWnd); 

	int year=0, month=0, day=0,hour=0,minute=0,second=0;
	GetDateAndTime(year, month, day, hour, minute, second);

	while(1)
	{
		///if(!g_chkdateTH_flag)	break;		

		GetDateAndTime(year, month, day, hour, minute, second);

		pFrame->DeleteOldData(year, month, day);


		::Sleep(600000);
	}

	return 0;
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CAABonderDlg ��ȭ ����




CAABonderDlg::CAABonderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAABonderDlg::IDD, pParent)
	, m_iCcd(0)
	, m_bLogView(true)
	, bEpoxyTimeChk(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//! <---------------------------------------------------------------------------
	//! Added by LHW (2013/2/5, 2013/3/6)
	m_iMode_Mouse_Box = 0;	//! Mouse�� �簢 ���� �����ϴ� �۾��� ���� ��, 0 based	
	dwTickStartRun = false;

	pThread_CCM_Grab = NULL; 
	pThread_CCM_Display = NULL;
	pThread_CCM_CvtColor = NULL;
	pThread_CCM_CvtMil = NULL;
	//! <---------------------------------------------------------------------------

	//! <---------------------------------------
	//! Added by LHW (2013/3/27)
	m_bState_CCD_Zoom = false;
	m_bBox_CCD_Zoom = false;
	m_bBox_Acting_CCD_Zoom = false;
	m_bActing_CCD_Zoom = false;
	m_bPan_CCD_Zoom = false;
	m_bActing_Pan_CCD_Zoom = false;
	m_rect_CCD_Zoom.SetRectEmpty();
	//! <---------------------------------------

	//! Added by LHW (2013/5/2)
	m_pos[0] = m_pos[1] = NULL;


	sfrSpecDlg = NULL;
	chartSetDlg = NULL;
	lensDlg = NULL;
	lensEdgeDlg = NULL;
	pcbDlg = NULL;
	ccdDlg = NULL;
	motorDlg = NULL;
	motorDlg2 = NULL;
	motorDlg3 = NULL;
	ioDlg = NULL;
	modelDlg = NULL;
	autodispDlg = NULL;
	InspSpecSet = NULL;
}

void CAABonderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_listLog);
	DDX_Control(pDX, IDC_LABEL_MENU, m_labelMenu);
	DDX_Control(pDX, IDC_LABEL_TITLE, m_labelTitle);
	DDX_Control(pDX, IDC_LABEL_STATUS_HOM, m_labelHom);
	DDX_Control(pDX, IDC_LABEL_STATUS_SERVO, m_labelServo);
	DDX_Control(pDX, IDC_LABEL_STATUS_MES, m_labelMes);

	DDX_Control(pDX, IDC_LABEL_STATUS_USB_MODULE, m_labelUsbModule);
	DDX_Control(pDX, IDC_LABEL_TIME, m_labelTime);

	DDX_Control(pDX, IDC_LABEL_PICKUP_NO_LENS, m_labelPickupNoLensCentering);
	DDX_Control(pDX, IDC_LABEL_PICKUP_NO_LENS_GRIP, m_labelPickupNoLensGrip);
	DDX_Control(pDX, IDC_LABEL_PICKUP_NO_PCB, m_labelPickupNoPcb);

	DDX_Control(pDX, IDC_LABEL_STATUS_THREAD1, m_labelThread1);
	DDX_Control(pDX, IDC_LABEL_STATUS_THREAD2, m_labelThread2);

	DDX_Control(pDX, IDC_LABEL_MODELNAME, m_labelCurModelName);
	DDX_Control(pDX, IDC_LABEL_ID, m_labelCCD_ID);
	DDX_Control(pDX, IDC_LABEL_LOT_NAME, m_labelLotName);
	DDX_Control(pDX, IDC_LABEL_CCD_RETRY, m_labelCcdRetryCnt);
	DDX_Control(pDX, IDC_LABEL_STATUS_USB_LIVE, m_labelUsbLive);

	DDX_Control(pDX, IDC_BUTTON_ORIGIN, m_btnOrigin);
	DDX_Control(pDX, IDC_BUTTON_READY, m_btnReady);
	DDX_Control(pDX, IDC_BUTTON_AUTORUN, m_btnAutorun);
	DDX_Control(pDX, IDC_BUTTON_PAUSE, m_btnPause);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON_NG_OUT, m_btnNgOut);
	DDX_Control(pDX, IDC_AUTORUN_START, m_btnStart);


	DDX_Check(pDX, IDC_CHECK_DIST, m_bMeasureDist);
	DDX_Check(pDX, IDC_CHECK_LOG_VIEW, m_bLogView);
	DDX_Check(pDX, IDC_CHECK_BCR_COUNT, m_bBcrAutoCount);


	DDX_Control(pDX, IDC_LABEL_STATUS_DOOR, m_LabelDoor);
	DDX_Control(pDX, IDC_BUTTON_MAIN, m_bMainBtn_Main);
	DDX_Control(pDX, IDC_BUTTON_MODEL, m_bMainBtn_Model);
	DDX_Control(pDX, IDC_BUTTON_LENS, m_bMainBtn_Align);
	DDX_Control(pDX, IDC_BUTTON_CCD, m_bMainBtn_CCD);
	DDX_Control(pDX, IDC_BUTTON_MOTOR, m_bMainBtn_Motor);
	DDX_Control(pDX, IDC_BUTTON_IO, m_bMainBtn_IO);
	DDX_Control(pDX, IDC_BUTTON_LIGHT, m_bMainBtn_Light);
	DDX_Control(pDX, IDC_BUTTON_ALARM, m_bMainBtn_Alarm);
	DDX_Control(pDX, IDC_BUTTON_PCB_RESULT, m_bPcbFinish);
	DDX_Control(pDX, IDC_BUTTON_DISPENSE_RESULT, m_bDispenseFinish);
	DDX_Control(pDX, IDC_BUTTON_LENS_PASS_RESULT, m_bLensPassFinish);
	DDX_Control(pDX, IDC_BUTTON_TIME_CHECK, m_EpoxyTimeCheck);
	DDX_Control(pDX, IDC_BUTTON_PROCOMP, m_bProCompCheck);
	DDX_Control(pDX, IDC_BUTTON_SMINI_OQMODE, m_bSminiOQCheck);
	DDX_Control(pDX, IDC_BUTTON_SET_DLG, m_bSetDlg);

	DDX_Control(pDX, IDC_BUTTON_PROC_HOLDER, m_bProHolderBondingCheck);
}

BEGIN_MESSAGE_MAP(CAABonderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAABonderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAABonderDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CAABonderDlg::OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_MAIN, &CAABonderDlg::OnBnClickedButtonMain)
	ON_BN_CLICKED(IDC_BUTTON_LENS, &CAABonderDlg::OnBnClickedButtonLens)
	ON_BN_CLICKED(IDC_BUTTON_CCD, &CAABonderDlg::OnBnClickedButtonCcd)
	ON_BN_CLICKED(IDC_BUTTON_MOTOR, &CAABonderDlg::OnBnClickedButtonMotor)
	ON_BN_CLICKED(IDC_BUTTON_IO, &CAABonderDlg::OnBnClickedButtonIo)
	ON_BN_CLICKED(IDC_BUTTON_MODEL, &CAABonderDlg::OnBnClickedButtonModel)
//	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_LENS_SUPPLY, &CAABonderDlg::OnBnClickedButtonLensSupply)
	ON_BN_CLICKED(IDC_BUTTON_PCB_OS_CHECK, &CAABonderDlg::OnBnClickedButtonPcbOsCheck)
	ON_BN_CLICKED(IDC_BUTTON_PCB_SUPPLY, &CAABonderDlg::OnBnClickedButtonPcbSupply)
	ON_BN_CLICKED(IDC_BUTTON_CCD_ALIGN, &CAABonderDlg::OnBnClickedButtonCcdAlign)
	ON_BN_CLICKED(IDC_BUTTON_CCD_INSP, &CAABonderDlg::OnBnClickedButtonCcdInsp)
	ON_BN_CLICKED(IDC_BUTTON_ORIGIN, &CAABonderDlg::OnBnClickedButtonOrigin)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON_LIGHT, &CAABonderDlg::OnBnClickedButtonLight)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CAABonderDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CAABonderDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_AUTORUN, &CAABonderDlg::OnBnClickedButtonAutorun)
	ON_BN_CLICKED(IDC_BUTTON_READY, &CAABonderDlg::OnBnClickedButtonReady)
	ON_BN_CLICKED(IDC_RADIO_ALIGN, &CAABonderDlg::OnBnClickedRadioAlign)
	ON_BN_CLICKED(IDC_RADIO_CCD2, &CAABonderDlg::OnBnClickedRadioCcd2)
	ON_STN_CLICKED(IDC_LABEL_STATUS_USB_LIVE, &CAABonderDlg::OnStnClickedLabelStatusUsbLive)
	ON_BN_CLICKED(IDC_CHECK_DIST, &CAABonderDlg::OnBnClickedCheckDist)
	ON_BN_CLICKED(IDC_BUTTON_ALARM, &CAABonderDlg::OnBnClickedButtonAlarm)
	ON_STN_CLICKED(IDC_LABEL_STATUS_SERVO, &CAABonderDlg::OnStnClickedLabelStatusServo)
	ON_BN_CLICKED(IDC_BUTTON_NG_OUT, &CAABonderDlg::OnBnClickedButtonNgOut)
	ON_BN_CLICKED(IDC_BUTTON_PCB_RESULT, &CAABonderDlg::OnBnClickedButtonPcbResult)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_LABEL_TITLE, &CAABonderDlg::OnClickedLabelTitle)
	ON_STN_CLICKED(IDC_LABEL_CCD_RETRY, &CAABonderDlg::OnStnClickedLabelCcdRetry)
	ON_BN_CLICKED(IDC_BUTTON_DISPENSE_RESULT, &CAABonderDlg::OnBnClickedButtonDispenseResult)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_LENS_PASS_RESULT, &CAABonderDlg::OnBnClickedButtonLensPassResult)
	ON_BN_CLICKED(IDC_BUTTON_MINIMIZE, &CAABonderDlg::OnBnClickedButtonMinimize)
	ON_BN_CLICKED(IDC_BUTTON_TIME_CHECK, &CAABonderDlg::OnBnClickedButtonTimeCheck)
	ON_STN_CLICKED(IDC_LABEL_LOT_NAME, &CAABonderDlg::OnStnClickedLabelLotName)
	ON_BN_CLICKED(IDC_SERVER_START, &CAABonderDlg::OnBnClickedServerStart)
	ON_BN_CLICKED(IDC_SERVER_STOP, &CAABonderDlg::OnBnClickedServerStop)
	ON_BN_CLICKED(IDC_CLIENT_CONNECT, &CAABonderDlg::OnBnClickedClientConnect)
	ON_BN_CLICKED(IDC_CLIENT_DISCONNECT, &CAABonderDlg::OnBnClickedClientDisconnect)
	ON_BN_CLICKED(IDC_CLIENT_SEND, &CAABonderDlg::OnBnClickedClientSend)
	ON_BN_CLICKED(IDC_BUTTON_PROCOMP, &CAABonderDlg::OnBnClickedButtonProcomp)
	ON_STN_CLICKED(IDC_LABEL_ID, &CAABonderDlg::OnStnClickedLabelId)
	ON_NOTIFY(NM_DBLCLK, IDC_STATIC_RESULT_GRID, &CAABonderDlg::OnDBClickedGridResult)
	ON_BN_CLICKED(IDC_AUTORUN_START, &CAABonderDlg::OnBnClickedAutorunStart)
	ON_BN_CLICKED(IDC_DOOR_OPEN, &CAABonderDlg::OnBnClickedDoorOpen)
	ON_BN_CLICKED(IDC_DOOR_CLOSE, &CAABonderDlg::OnBnClickedDoorClose)
	ON_STN_CLICKED(IDC_LABEL_STATUS_MES, &CAABonderDlg::OnStnClickedLabelStatusMes)
	ON_STN_CLICKED(IDC_LABEL_MODELNAME, &CAABonderDlg::OnStnClickedLabelModelname)
	ON_BN_CLICKED(IDC_BUTTON_SMINI_OQMODE, &CAABonderDlg::OnBnClickedButtonSminiOqmode)
	ON_BN_CLICKED(IDC_BUTTON_SET_DLG, &CAABonderDlg::OnBnClickedButtonSetDlg)
	ON_BN_CLICKED(IDC_CHECK_LOG_VIEW, &CAABonderDlg::OnBnClickedCheckLogView)
	ON_BN_CLICKED(IDC_CHECK_BCR_COUNT, &CAABonderDlg::OnBnClickedCheckBcrCount)
	ON_BN_CLICKED(IDC_BUTTON_PROC_HOLDER, &CAABonderDlg::OnBnClickedButtonProcHolder)
END_MESSAGE_MAP()


// CAABonderDlg �޽��� ó����

BOOL CAABonderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadStringA(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	theApp.MainDlg = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;

#if (____AA_WAY == PCB_TILT_AA)
	TITLE_MOTOR_Z = Motor_PCB_Z;
	TITLE_MOTOR_TX = Motor_PCB_Xt;
	TITLE_MOTOR_TY = Motor_PCB_Yt;
	TITLE_MOTOR_X = Motor_PCB_X;
	TITLE_MOTOR_Y = Motor_PCB_Y;
#elif (____AA_WAY == LENS_TILT_AA)
	TITLE_MOTOR_Z = Motor_Lens_Z;
	TITLE_MOTOR_TX = Motor_Lens_Xt;
	TITLE_MOTOR_TY = Motor_Lens_Yt;
	TITLE_MOTOR_X = Motor_Lens_X;
	TITLE_MOTOR_Y = Motor_Lens_Y;
#endif

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.
	Start_Btn_On = false;


	m_clLogThread.StartThread();



	LogSave("���α׷� ����.");


	//1ȣ�� 2ȣ�� �� ���� ���� ���� Ȯ��
	//BASE_AA_MODEL_LOAD_DIR

	CFileFind finder;
	BOOL IsFind;
	CString FolderName;
	FolderName.Format("%s", BASE_AA_MODEL_LOAD_DIR);
	IsFind = finder.FindFile(FolderName);
	if (!IsFind)
	{
		errMsg2(Task.AutoFlag, "1/2 ȣ�� �� Ȯ�ιٶ��ϴ�.");
		OnCancel();
		return FALSE;
	}


	modelList.NewLoad();
	sysData.Load();
	g_AlarmFlag = true;
	g_AlarmCnt = 0;
	output_Mode = ASCII_MODE;


	password.Load();
	MandoSfrSpec.NewSfrLoad();
	//MandoSfrSpec.load();
	myTimer(false);

	m_bMiuRun = false;

#ifdef ON_LINE_VISION
	if(!vision.initVB())
	{
		errMsg2(Task.AutoFlag, "ī�޶� ���� ���� �ʱ�ȭ ����");
		return FALSE;
	}
#endif
	

	MIU.setInterface();
	model.Load();
	work.Load();
	model.PatternLoad();
	
    model.AcmisDataLoad();

    model.RoiLoad();

	bool bRet = false;
	func_Set_SFR_N_Type();

	//! MbufBayer �Լ� ��� ���ο� ����, MIU ����� ���� ���� ����� �����ؾ� �Ѵ�.
	//! Added by LHW (2013/3/12)
	if ( model.m_eGrabMethod == MIL_BAYER_CVT_COLOR )
	{
		MIU.m_bFlag_Color_Covert_Ext = true;
		MIU.m_bFlag_Mil_Convert = true;
	}
	else
	{
		MIU.m_bFlag_Color_Covert_Ext = false;
		MIU.m_bFlag_Mil_Convert = true;
	}

#ifdef ON_LINE_VISION
	vision.initDisplay();
	vision.initMarkDisplay(MARK_SIZE_X, MARK_SIZE_Y);

	CWnd* pWnd;
	pWnd = (CWnd*)GetDlgItem(IDC_DISP_PCB);//IDC_DISP_LENS
	MdispSelectWindow(vision.MilDisplay[0], vision.MilSmallImageChild[0], pWnd->m_hWnd);
	//
	pWnd = (CWnd*)GetDlgItem(IDC_DISP_CCD);
	MdispSelectWindow(vision.MilDisplay[3], vision.MilSmallImage[1], pWnd->m_hWnd);

	vision.enableOverlay();
	
#endif
#ifdef		ON_LINE_MOTOR
	bool initFlag = motor.Axl_Init();
	if(!initFlag)
	{
		//errMsg2(Task.AutoFlag, msg); 

		char logStr[1000];
		sprintf_s(logStr, "���� �ʱ�ȭ�� ���� �Ͽ����ϴ�.");
		errMsg2(Task.AutoFlag, logStr);

		sprintf_s(logStr, "���� �ʱ�ȭ�� ���� �Ͽ����ϴ�.");
		LogSave(logStr);

		OnCancel();
	}

	motor.AmpEnableAll();
	Sleep(500);
	motor.AmpDisableAll();
	//Dio.PCBvaccumOn(VACCUM_OFF, true);
#endif
	

	
	SetInterface();


#ifdef ON_LINE_VISION
	bRet = vision.MiuBufferAlloc(gMIUDevice.nWidth, gMIUDevice.nHeight);
#endif
	//! <---------------------------------------
	//! Added by LHW (2013/3/27)	
	m_rect_CCD_Zoom.left = 0;
	m_rect_CCD_Zoom.top = 0; 
	m_rect_CCD_Zoom.right = gMIUDevice.nWidth;
	m_rect_CCD_Zoom.bottom = gMIUDevice.nHeight;
	//! <---------------------------------------

	m_bisAlignBtn = false;
	m_bisMotorBtn = false;
	m_bisLightBtn = false;

	m_iCurCamNo = 3;
	ctrlSubDlg(MAIN_DLG);
	changeMainBtnColor(MAIN_DLG);
	vision.clearOverlay(CAM1);
	vision.drawOverlay(CAM1);
	vision.clearOverlay(CAM2);
	vision.drawOverlay(CAM2);
	vision.clearOverlay(CCD);
	vision.drawOverlay(CCD);
	 
	//m_mmResult = timeSetEvent(1000, 1, NULL, NULL, TIME_PERIODIC );
	 

#ifdef ON_LINE_MONITOR
	pThread_Monitor    = ::AfxBeginThread(Thread_Monitor, this);
#endif
	pThread_Clock	   = ::AfxBeginThread(Thread_Clock, this);


#ifdef ON_LINE_VISION
	vision.setLiveMode(true);
	pThread_Grab = ::AfxBeginThread(Thread_Grab, this);
#endif

	/* ���� ���� �� DB */
	/*if(!g_ADOData.func_AA_DBConnect())
	{
		putListLog(" [ ACCESS DB ] Open Fail!");
	}*/


	/*���� �ð� Timer*/
	if( work.m_Epoxy_Time_Flag == 0)
	{
		sLangChange.LoadStringA(IDS_STRING538);//EPOXY TIME START
		GetDlgItem(IDC_BUTTON_TIME_CHECK)->SetWindowText(sLangChange);
		m_EpoxyTimeCheck.m_iStateBtn = 0;
	}
	else 
	{
		sLangChange.LoadStringA(IDS_STRING1451);//EPOXY TIME STOP
		GetDlgItem(IDC_BUTTON_TIME_CHECK)->SetWindowText(sLangChange);
		m_EpoxyTimeCheck.m_iStateBtn = 1;
	}
	m_EpoxyTimeCheck.Invalidate();


	m_oldDlg = -1;

	/////pThread_CheckDate = AfxBeginThread(Thread_CheckDate, this);
	
	SetTimer(999, 500, NULL);
	SetTimer(9, 3000, NULL);

#ifdef ON_LINE_VISION
	for(int i = 0; i < MARK_CNT; i++)
	{
		int iCh = 1;
		if(iCh == PCB_Chip_MARK)	iCh = 0;

		for(int j = 0; j < 2; j++){
			vision.geometricMarkPreProc(iCh, i, j);
		}
	}
#endif
	LightDlg.Create(IDD_DIALOG_LIGHT, this);
	LightDlg.ShowWindow(SW_HIDE);


	CString logStr="";
	logStr.Format("%d", work.m_iCoverUpDownCnt);
	m_labelCcdRetryCnt.SetText(logStr);
	m_labelCcdRetryCnt.Invalidate();
	
	Rs232Init();

#ifdef ON_LINE_KEYENCE
	ConnectBarcode();
#endif

#if (____AA_WAY == PCB_TILT_AA)
	putListLog(" [INFO] PCB AA");
#elif (____AA_WAY == LENS_TILT_AA)
	putListLog(" [INFO] LENS AA");
#endif
	

	m_bBarcodeConnect = false;
	Task.bSfrLogView = true;
	logStr.Empty();
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}
void CAABonderDlg::Rs232Init()
{
	SerialPortList.GetList_SerialPort();
	CString logStr= _T("");
	CString sCommPort = _T("");
	bool bRet_Con_RS232C = false;



	sCommPort.Format("COM%d", sysData.iCommPort[COMM_LIGHT1]);
	LightControl.SetReceiveProcPtr(this);
	bRet_Con_RS232C = LightControl.Connect_Device(sCommPort, 0);
	//-----------------------------------------------------------------------------------------------------------------------------------
	//
	//
	//BARCODE 
	//IDC_COMBO_BARCODE_PORT - sysData.iCommPort[COMM_BARCODE]
	//
	if( !barcode.func_Comm_Open(sysData.iCommPort[COMM_BARCODE], sysData.iBaudRate[COMM_BARCODE]) )
	{
		logStr.Format("[HnadBarcode] ��� ���� ���� : COM%d" , sysData.iCommPort[COMM_BARCODE]);
		putListLog(logStr);
		Task.bConnectBarcode = false;
	}
	else
	{
		logStr.Format("[HandBarcode] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_BARCODE]);
		Task.bConnectBarcode = true;
		putListLog(logStr);
	}
	//LIM
	if (!barcode2.func_Comm_Open(sysData.iCommPort[COMM_X100_BCR], sysData.iBaudRate[COMM_X100_BCR]))
	{
		logStr.Format("[X100Barcode] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_X100_BCR]);
		putListLog(logStr);
		Task.bConnectBarcode = false;
	}
	else
	{
		logStr.Format("[X100Barcode] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_X100_BCR]);
		Task.bConnectBarcode = true;
		putListLog(logStr);
	}

	//-----------------------------------------------------------------------------------------------------------------------------------
	//
	//
	//LASER 
	//IDC_COMBO_LASER_PORT - sysData.iCommPort[COMM_LASER]

#ifdef ON_LINE_KEYENCE

	if (!Keyence.func_RS232_CommOpen(sysData.iCommPort[COMM_LASER], 9600))
	{
		logStr.Format("[LASER] ��� ���� ���� : COM%d , RATE%d", sysData.iCommPort[COMM_LASER], 9600);
		putListLog(logStr);
	}
	else
	{
		logStr.Format("[LASER] ��� ���� ���� : RATE%d", 9600);
		putListLog(logStr);
		if (!Keyence.func_LT9030_KeyLock(true))
		{
			logStr.Format("[LASER] ��� �̻� �߻�..Lock ���� Fail..Cable�� Ȯ�� �ϼ���. RATE%d", 9600);
			putListLog(logStr);
		}
		else
		{
			logStr.Format("[LASER] ��� ���� ���� : COM%d, RATE%d", sysData.iCommPort[COMM_LASER], 9600);
			putListLog(logStr);
		}

	}
#endif
	//-----------------------------------------------------------------------------------------------------------------------------------
	//
	//
	//UV 
	//IDC_COMBO_UV_PORT - sysData.iCommPort[COMM_UV]

	
	
	if (!UVCommand.Connect_Device(sysData.iCommPort[COMM_UV]))
	{
		logStr.Format("[UV TRI #1] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV]);
		putListLog(logStr);
	}
	else
	{
		logStr.Format("[UV] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV]);
		putListLog(logStr);
		Sleep(100);
		//UVCommand.UV_Shutter_Open();//
		UVCommand.UV_Shutter_PowerSet(model.UV_Power);// 95);//
		logStr.Format("	#1 UV POWER SET: %d", model.UV_Power);
		putListLog(logStr);
	}

	//���� ���� ���� uv�� io�� �۵� ȣ�� + AltisUv

	/*sCommPort.Format("COM%d", sysData.iCommPort[COMM_UV]);
	LightControlFifth.SetReceiveProcPtr(this);
	bRet_Con_RS232C = UVControl.Connect_Device(sCommPort, 0);*/



	//sCommPort.Format("COM%d", sysData.iCommPort[COMM_UV]);

	//if (!UvAltisControl[0].Connect_Device(sCommPort))
	//{
	//	logStr.Format("[UV Altis #1] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV]);
	//	putListLog(logStr);
	//}
	//else
	//{
	//	logStr.Format("[UV Altis #1] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV]);
	//	putListLog(logStr);
	//	Sleep(100);

	//}

	//sCommPort.Format("COM%d", sysData.iCommPort[COMM_UV2]);
	//if (!UvAltisControl[1].Connect_Device(sCommPort))
	//{
	//	logStr.Format("[UV Altis #2] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV2]);
	//	putListLog(logStr);
	//}
	//else
	//{
	//	logStr.Format("[UV Altis #2] ��� ���� ���� : COM%d", sysData.iCommPort[COMM_UV2]);
	//	putListLog(logStr);
	//	Sleep(100);

	//}
	//sysData.iCommPort[]
	//-----------------------------------------------------------------------------------------------------------------------------------
	//
	//
	//CHART , ALIGN , OC  	( TOP CHART , SIDE CHART , ALIGN , OC
	//IDC_COMBO_LIGHT1_PORT - sysData.iCommPort[COMM_LIGHT1]
	
	

	
	logStr.Empty();
	sCommPort.Empty();
}

void CAABonderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CAABonderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CAABonderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAABonderDlg::OnBnClickedOk(){}
void CAABonderDlg::OnBnClickedCancel(){}


void CAABonderDlg::MainEpoxyRun()
{
	if (bThreadEpoxyRun == true)
	{
		g_bMovingflag = false;
		putListLog("���� �̵����Դϴ�.");
		return;
	}
	pThread_Epoxy = ::AfxBeginThread(Thread_Epoxy, this);
}
void CAABonderDlg::setCamDisplay(int camNo, int mode)
{
	if (camNo<0 || camNo >= MAX_CAM_NO)
	{
		CString errStr;
		errStr.Format("setCamDisplay() Function Call Error. [cam %d, mode %d]", camNo, mode);

		LogSave(errStr);
		camNo = 0;
	}

	int disp1, disp2, disp3, disp4;

	if (mode == 0)
	{
		if (camNo == 0)
		{
			disp1 = IDC_DISP_LENS;
			disp2 = IDC_DISP_PCB;
			disp3 = IDC_DISP_PCB2;
			disp4 = IDC_DISP_CCD;
		}
		else if (camNo == 1)
		{
			disp1 = IDC_DISP_PCB;
			disp2 = IDC_DISP_LENS;
			disp3 = IDC_DISP_CCD;
			disp4 = IDC_DISP_PCB2;
		}
		else if (camNo == 2)
		{
			disp1 = IDC_DISP_CCD;
			disp2 = IDC_DISP_LENS;
			disp3 = IDC_DISP_PCB;
			disp4 = IDC_DISP_PCB2;
		}
		else if (camNo == 3)
		{
			disp1 = IDC_DISP_CCD;
			disp2 = IDC_DISP_LENS;
			disp3 = IDC_DISP_PCB;
			disp4 = IDC_DISP_PCB2;
		}
		// 20151006 ���� ����ī�޶� �߰��� ���� ����
		else if (camNo == 4)
		{
			disp1 = IDC_DISP_PCB;
			disp2 = IDC_DISP_PCB2;
			disp3 = IDC_DISP_LENS;
			disp4 = IDC_DISP_CCD;
		}

		if (camNo == 2 || camNo == 3)
		{
			GetDlgItem(disp1)->MoveWindow(ccdPosLeft, CamPosY, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
		}
		else
		{
			GetDlgItem(disp1)->MoveWindow(m_rectCamDispPos1.left, m_rectCamDispPos1.top, SMALL_CAM_SIZE_X, SMALL_CAM_SIZE_Y);
		}


		GetDlgItem(disp1)->ShowWindow(SW_SHOW);
		GetDlgItem(disp2)->ShowWindow(SW_HIDE);
		GetDlgItem(disp3)->ShowWindow(SW_HIDE);
		GetDlgItem(disp4)->ShowWindow(SW_HIDE);

		//		GetDlgItem(IDC_LIST_LOG)->ShowWindow(SW_HIDE);

		autodispDlg->ShowWindow(false);
	}
	else
	{
		if (camNo == 0)
		{
			disp1 = IDC_DISP_CCD;// IDC_DISP_LENS;
			if (m_iCurCamNo == 3)
			{
				disp2 = IDC_DISP_PCB2;
				disp3 = IDC_DISP_PCB;
			}
			else
			{
				disp2 = IDC_DISP_PCB;
				disp3 = IDC_DISP_PCB2;
			}

			disp4 = IDC_DISP_LENS;// IDC_DISP_CCD;
		}
		else if (camNo == 1)
		{
			disp1 = IDC_DISP_PCB;
			disp2 = IDC_DISP_LENS;
			disp3 = IDC_DISP_PCB2;
			disp4 = IDC_DISP_CCD;
		}
		else if (camNo == 2)
		{
			disp1 = IDC_DISP_LENS;
			disp2 = IDC_DISP_PCB2;
			disp3 = IDC_DISP_PCB;
			disp4 = IDC_DISP_CCD;
		}
		else if (camNo == 3)
		{
			disp1 = IDC_DISP_CCD;
			disp2 = IDC_DISP_PCB;
			disp3 = IDC_DISP_LENS;
			disp4 = IDC_DISP_PCB2;
		}

		if (camNo == 3)
		{
			GetDlgItem(disp1)->MoveWindow(ccdPosLeft, CamPosY, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
		}
		else
		{
			GetDlgItem(disp1)->MoveWindow(m_rectCamDispPos1.left, m_rectCamDispPos1.top, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
			GetDlgItem(disp2)->MoveWindow(m_rectCamDispPos2.left, m_rectCamDispPos2.top, SMALL_CAM_SIZE_X, SMALL_CAM_SIZE_Y);
		}


		GetDlgItem(disp1)->ShowWindow(SW_SHOW);
		GetDlgItem(disp3)->ShowWindow(SW_HIDE);
		GetDlgItem(disp4)->ShowWindow(SW_HIDE);
		if (camNo == 3)
		{
			GetDlgItem(disp2)->ShowWindow(SW_HIDE);

			autodispDlg->ShowWindow(true);
		}
		else
		{
			GetDlgItem(disp2)->ShowWindow(SW_SHOW);
			autodispDlg->ShowWindow(false);
		}
	}

	m_iCurCamNo = camNo;
}

void CAABonderDlg::SetInterface()
{
	WINDOWPLACEMENT wndpl;
	CRect rTemp;
	CString sTemp;
	baseGap = 1;
	//---------------Main Frame-----------------------
	wndpl.rcNormalPosition.left = 0;
	wndpl.rcNormalPosition.top = 0;
	wndpl.rcNormalPosition.right = MAIN_DLG_SIZE_X;
	wndpl.rcNormalPosition.bottom = MAIN_DLG_SIZE_Y;
	this->MoveWindow(&wndpl.rcNormalPosition);
	//	GetDlgItem(IDC_LABEL_RUN_MODE)->GetWindowPlacement(&wndpl);
	GetDlgItem(IDC_LABEL_MODELNAME)->GetWindowPlacement(&wndpl);

	CamPosY = wndpl.rcNormalPosition.top + baseGap;
	int cent = wndpl.rcNormalPosition.right + 1; //(wndpl.rcNormalPosition.right + MAIN_DLG_SIZE_X) / 2;

	

	m_rectCamDispPos1.left = cent;
	m_rectCamDispPos1.right = m_rectCamDispPos1.left + SMALL_CAM_SIZE_X + 2;
	m_rectCamDispPos1.top = wndpl.rcNormalPosition.top;
	m_rectCamDispPos1.bottom = wndpl.rcNormalPosition.top + SMALL_CAM_SIZE_Y;

	//ccd���� ��ǥ ����
	ccdPosLeft = m_rectCamDispPos1.left;// baseGap;
	ccdPosRight = ccdPosLeft + (SMALL_CCD_SIZE_X);

	m_rectCcdDispPos.left = cent;// baseGap;
	m_rectCcdDispPos.right = ccdPosRight;
	m_rectCcdDispPos.top = CamPosY;
	m_rectCcdDispPos.bottom = CamPosY + SMALL_CCD_SIZE_Y;
	
	m_rectCamDispPos2.top = wndpl.rcNormalPosition.top;
	m_rectCamDispPos2.bottom = wndpl.rcNormalPosition.top + SMALL_CAM_SIZE_Y;
	
	m_rectCamDispPos2.left = ccdPosRight;
	m_rectCamDispPos2.right = m_rectCamDispPos2.left + SMALL_CCD_SIZE_X + 1;

	

	g_iCCDCamView = 5;		// ȭ�� ũ�� ����
							/* ListBox */
	wndpl.rcNormalPosition.left = m_rectCamDispPos1.left;
	wndpl.rcNormalPosition.right = m_rectCamDispPos2.left;// -10;// m_rectCamDispPos1.right + ;
	wndpl.rcNormalPosition.top = m_rectCamDispPos1.bottom + 145;
	wndpl.rcNormalPosition.bottom = MAIN_DLG_SIZE_Y - 85;
	GetDlgItem(IDC_LIST_LOG)->MoveWindow(&wndpl.rcNormalPosition);

	/* ���콺 Ŀ�� */
	loadStandardCursor();

	DispCurModelName(model.mCurModelName);// model.name);

	//initGrid();
	//InitGridCtrl_Result();
	initInspResGrid();

	SetInterface_CreateDlg();

	SetInterface_Button();
	SetInterface_Label();
	m_iOldDlgNo = -1;
	m_bDrawFlag = false;
	m_bBoxMoveFlag = false;
	m_bBoxMoveFlag_CCD = false;

	Task.AutoFlag = 0;
	AutoRunView(Task.AutoFlag);

	m_bMainBtn_Main.m_iStateBtn = 1;
	m_bMainBtn_Main.Invalidate();


	m_bPcbFinish.m_iStateBtn = 0;
	m_bPcbFinish.Invalidate();

	m_bDispenseFinish.m_iStateBtn = 0;
	m_bDispenseFinish.Invalidate();

	m_bLensPassFinish.m_iStateBtn = 0;
	m_bLensPassFinish.Invalidate();

	m_EpoxyTimeCheck.m_iStateBtn = 0;
	m_bLensPassFinish.Invalidate();

	m_bProCompCheck.m_iStateBtn = 0;
	m_bProCompCheck.Invalidate();

	m_bSminiOQCheck.m_iStateBtn = 0;
	m_bSminiOQCheck.Invalidate();

	m_bProHolderBondingCheck.m_iStateBtn = 0;
	m_bProHolderBondingCheck.Invalidate();

	font.CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_listLog.SetFont(&font, 1);
	font.DeleteObject();


	m_bIsLensMode = 0;
	m_bIsMotorMode = 0;


}

void	CAABonderDlg::SetInterface_Button()
{
	WINDOWPLACEMENT wndpl;
	sLangChange.LoadStringA(IDS_STRING1328);
	GetDlgItem(IDC_BUTTON_ORIGIN)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1320);
	GetDlgItem(IDC_BUTTON_READY)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1369);
	GetDlgItem(IDC_BUTTON_AUTORUN)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1360);
	GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1400);
	GetDlgItem(IDC_BUTTON_STOP)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING389);
	GetDlgItem(IDC_RADIO_ALIGN)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING431);
	GetDlgItem(IDC_RADIO_CCD2)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1147);
	GetDlgItem(IDC_CHECK_DIST)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1309);
	GetDlgItem(IDC_BUTTON_PROCOMP)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1249 );
	GetDlgItem(IDC_BUTTON_MINIMIZE)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1406);
	GetDlgItem(IDC_BUTTON_LIGHT)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1297);
	GetDlgItem(IDC_BUTTON_ALARM)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1292);
	GetDlgItem(IDC_BUTTON_LENS)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING451);
	GetDlgItem(IDC_BUTTON_CCD)->SetWindowText(sLangChange);
	sLangChange.LoadStringA(IDS_STRING1407);
	GetDlgItem(IDC_BUTTON_EXIT)->SetWindowText(sLangChange);
	//----------------------------------------------------------------------------------------- 
	//-----------------------------------------------------------------------------------------
	/* ��ư ��� */
	wndpl.rcNormalPosition.left		= 0;
	wndpl.rcNormalPosition.right	= MAIN_DLG_SIZE_X - 1;
	wndpl.rcNormalPosition.top		= MAIN_DLG_SIZE_Y - 80;
	wndpl.rcNormalPosition.bottom	= MAIN_DLG_SIZE_Y - 1;
	m_labelMenu.MoveWindow(&wndpl.rcNormalPosition);
	m_labelMenu.SetBkColor(M_COLOR_LIGHT_GREEN);

	//-----------------------------------------------------------------------------------------
	CWnd *pbutton[9] = {NULL,};
	pbutton[0] = &m_bMainBtn_Main;
	pbutton[1] = &m_bMainBtn_Model;
	pbutton[2] = &m_bMainBtn_Align;
	pbutton[3] = &m_bMainBtn_Motor;
	pbutton[4] = &m_bMainBtn_CCD;
	pbutton[5] = &m_bMainBtn_IO;
	pbutton[6] = &m_bMainBtn_Light;
	pbutton[7] = &m_bMainBtn_Alarm;
	pbutton[8] = GetDlgItem(IDC_BUTTON_EXIT);

	int btnSize_w = 142;//213;
	wndpl.rcNormalPosition.left = 6;
	wndpl.rcNormalPosition.right = btnSize_w;//btnSize_w;//140;
	wndpl.rcNormalPosition.top = MAIN_DLG_SIZE_Y - 74;
	wndpl.rcNormalPosition.bottom = MAIN_DLG_SIZE_Y - 7;

	for (int i = 0; i < 9; i++) {
		if (i != 0) {
			wndpl.rcNormalPosition.left = wndpl.rcNormalPosition.right + 3;
			wndpl.rcNormalPosition.right += btnSize_w; ;// btnSize_w;//125; 
		}
		pbutton[i]->MoveWindow(&wndpl.rcNormalPosition);
	}

	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------��� �޴� ����
	CRect posRect;
	
	int baseGap = 1;
	GetDlgItem(IDC_LABEL_TITLE)->GetWindowPlacement(&wndpl);

	

	int topClock_W = 220;
	int topDoor_W = 100;
	int topServo_W = 100;
	int topHome_W = 100;
	int topMes_W = 100;
	int topCCD_W = 130;
	int top_H = 68;
	//=================================================================
	//�ð�
	
	wndpl.rcNormalPosition.right	= MAIN_DLG_SIZE_X-baseGap;
	wndpl.rcNormalPosition.left		= wndpl.rcNormalPosition.right - topClock_W;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	GetDlgItem(IDC_LABEL_TIME)->MoveWindow(&wndpl.rcNormalPosition);

	//Door
	wndpl.rcNormalPosition.right	= wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left		= wndpl.rcNormalPosition.right - topDoor_W;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	m_LabelDoor.MoveWindow(&wndpl.rcNormalPosition);
	//mes
	
	/*wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left = wndpl.rcNormalPosition.right - topMes_W;
	wndpl.rcNormalPosition.top = baseGap;
	wndpl.rcNormalPosition.bottom = top_H;
	m_labelMes.MoveWindow(&wndpl.rcNormalPosition);*/
	//Servo
	wndpl.rcNormalPosition.right	= wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left		= wndpl.rcNormalPosition.right - topServo_W;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	m_labelServo.MoveWindow(&wndpl.rcNormalPosition);

	//Home
	wndpl.rcNormalPosition.right	= wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left		= wndpl.rcNormalPosition.right - topHome_W;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	m_labelHom.MoveWindow(&wndpl.rcNormalPosition);

	//CCD
	wndpl.rcNormalPosition.right	= wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left		= wndpl.rcNormalPosition.right - topCCD_W;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	m_labelUsbLive.MoveWindow(&wndpl.rcNormalPosition);
	
	//Title
	wndpl.rcNormalPosition.right	= wndpl.rcNormalPosition.left - baseGap;
	wndpl.rcNormalPosition.left		= 1;
	wndpl.rcNormalPosition.top		= baseGap;
	wndpl.rcNormalPosition.bottom	= top_H;
	m_labelTitle.MoveWindow(&wndpl.rcNormalPosition);
	//--------------------------------------------------------------------------
	//--------------------------------------------------------------------------
	m_btnStart.m_iStateBtn = 0;
}

void CAABonderDlg::SetInterface_Label()
{
	CString sTemp="";
	CString temp="";
	bool ModelSelect = false;


	if (LGIT_MODEL_INDEX == M2_FF_MODULE)
	{
		temp.Format(_T("M2_FF_MODULE"));
		ModelSelect = true;
	}



	
	if (ModelSelect == false)
	{
		temp.Format(_T("EMPTY"));
		putListLog(" [INFO] BUILD MODEL : �� �� ����");
	}
	else
	{

		sTemp.Format("[INFO] BUILD MODEL : %s", temp);
		putListLog(sTemp);
	}
	sTemp.Format("[%s] Active Alignment.%s", temp, VER_STR);
	
//
	//DEF_VER_DAY
	m_labelTitle
		.SetBkColor(M_COLOR_DARK_GREEN)//M_COLOR_BLUE)
		.SetTextColor(M_COLOR_WHITE)
		.SetFontBold(TRUE)
		.SetText(sTemp)
		.SetFontSize(24);

	

	sLangChange.LoadStringA(IDS_STRING1328);
	m_labelHom
		.SetBkColor(M_COLOR_RED)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange) //��������
		.SetFontSize(12);

	sLangChange.LoadStringA(IDS_STRING509);
	m_LabelDoor
		.SetBkColor(M_COLOR_RED)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(12);
		
	sLangChange.LoadStringA(IDS_STRING1450);
	m_labelThread1
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(12);

	sLangChange.LoadStringA(IDS_STRING1449);
	m_labelThread2
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(12);

	sLangChange.LoadStringA(IDS_STRING1056);
	m_labelServo
		.SetBkColor(M_COLOR_RED)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(12);
	m_labelMes
		.SetBkColor(M_COLOR_RED)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetFontSize(12);

	m_labelUsbModule
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetFontSize(12);


	sLangChange.LoadStringA(IDS_STRING441);//"CCD ������" 
	m_labelUsbLive  
		.SetBkColor(M_COLOR_RED) 
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(12);

	SYSTEMTIME time;
	::GetLocalTime(&time);

	sLangChange.Format("%02d : %02d : %02d", time.wHour, time.wMinute, time.wSecond);

	m_labelTime
		.SetBkColor(M_COLOR_DARK_CYAN)
		.SetTextColor(M_COLOR_GREEN)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(30);

	m_labelPickupNoPcb
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_GREEN)
		.SetFontBold(TRUE)
		.SetText("-")
		.SetFontSize(16);

	m_labelPickupNoLensCentering
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_GREEN)
		.SetFontBold(TRUE)
		.SetText("-")
		.SetFontSize(16);

	m_labelPickupNoLensGrip
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_GREEN)
		.SetFontBold(TRUE)
		.SetText("-")
		.SetFontSize(16);

	m_labelCurModelName
		.SetBkColor(M_COLOR_CYAN)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetFontSize(17);

	m_labelCCD_ID
		.SetBkColor(M_COLOR_WHITE)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetFontSize(12);
	sLangChange.LoadStringA(IDS_STRING756);	//"Lot ��"
	m_labelLotName
		.SetBkColor(M_COLOR_WHITE)
		.SetTextColor(M_COLOR_BLACK)
		.SetFontBold(TRUE)
		.SetText(sLangChange)
		.SetFontSize(14);

	m_labelCcdRetryCnt
		.SetBkColor(M_COLOR_GRAY)
		.SetTextColor(M_COLOR_GREEN)
		.SetFontBold(TRUE)
		.SetText("0")
		.SetFontSize(16);
}

void CAABonderDlg::SetInterface_CreateDlg()
{
	/* Dialog Open */

	if(sfrSpecDlg == NULL){
		sfrSpecDlg = new CSfrSpec;
		sfrSpecDlg->Create(IDD_DIALOG_SFR_SPEC);
		sfrSpecDlg->ShowWindow(SW_HIDE);
	}
	if (chartSetDlg == NULL) {
		chartSetDlg = new CChartSetDlg;
		chartSetDlg->Create(IDD_DIALOG_CHART_SET);
		chartSetDlg->ShowWindow(SW_HIDE);
	}
	
	if(modelDlg == NULL){
		modelDlg = new CModelDlg;
		modelDlg->Create(IDD_DIALOG_MODEL);
		modelDlg->ShowWindow(SW_HIDE);
	}

	if(lensDlg == NULL){
		lensDlg = new CLensDlg;
 		lensDlg->Create(IDD_DIALOG_LENS);
 		lensDlg->ShowWindow(SW_HIDE);
	}

	if(lensEdgeDlg == NULL){
		lensEdgeDlg = new CLensEdgeDlg;
 		lensEdgeDlg->Create(IDD_DIALOG_LENS_EDGE);
 		lensEdgeDlg->ShowWindow(SW_HIDE);
	}

	if(pcbDlg == NULL){
 		pcbDlg = new CPcbDlg;
 		pcbDlg->Create(IDD_DIALOG_PCB);
 		pcbDlg->ShowWindow(SW_HIDE);
	}


	if(ccdDlg == NULL){
 		ccdDlg = new CCcdDlg;
 		ccdDlg->Create(IDD_DIALOG_CCD);
 		ccdDlg->ShowWindow(SW_HIDE);
	}
 
	if(motorDlg == NULL){
 		motorDlg = new CMotorDlg;
 		motorDlg->Create(IDD_DIALOG_LENS_TEACHING);
 		motorDlg->ShowWindow(SW_HIDE);
	}
 
	if(motorDlg2 == NULL){
		motorDlg2 = new CMotorDlg2;
		motorDlg2->Create(IDD_DIALOG_PCB_TEACHING);
		motorDlg2->ShowWindow(SW_HIDE);
	}

	if(motorDlg3 == NULL){
		motorDlg3 = new CMotorDlg3;
		motorDlg3->Create(IDD_DIALOG_PCB2_TEACHING);
		motorDlg3->ShowWindow(SW_HIDE);
	}


	/*if(lightDlg == NULL){
		lightDlg = new CLightDlg;
		lightDlg->Create(IDD_DIALOG_LIGHT);
		lightDlg->ShowWindow(SW_HIDE);
	}*/

	
// 	DataSet = new CDataSet;
// 	DataSet->Create(IDD_DIALOG_DATASET);
// 	DataSet->ShowWindow(SW_HIDE);

	if(ioDlg == NULL){
		ioDlg = new CIoDlg;
 		ioDlg->Create(IDD_DIALOG_IO);
 		ioDlg->ShowWindow(SW_HIDE);
	}

	if(delayDlg == NULL){
		delayDlg = new CDelayMsgDlg;
		delayDlg->Create(IDD_DIALOG_DELAY);
		delayDlg->ShowWindow(SW_HIDE);
	}

	/*if( TiltingManualdlg == NULL )
	{
		TiltingManualdlg = new CTiltingManualDlg;
		TiltingManualdlg->Create(IDD_DIALOG_MANUAL_TILTING);
		TiltingManualdlg->ShowWindow(SW_HIDE);
	}*/

	if(autodispDlg == NULL){
		autodispDlg = new CAutoDispDlg;
		autodispDlg->Create(IDD_AUTODISP_DIALOG);
		autodispDlg->ShowWindow(SW_HIDE);
	}

	if(alarmDlg == NULL){
		alarmDlg = new CAlarmDialog;
		alarmDlg->Create(IDD_DIALOG_ALARM);
		alarmDlg->ShowWindow(SW_HIDE);
	}

	/*if(g_pFoceDlg == NULL){
		g_pFoceDlg = new CForceAlignDlg;
		g_pFoceDlg->Create(IDD_DIALOG_FORCE_ALIGN);
		g_pFoceDlg->ShowWindow(SW_HIDE);
	}*/

	Make_Child_Dialog();		// ����, CCD �ʱ�ȭ
}

bool CAABonderDlg::ConnectBarcode()
{
	TCHAR szLog[SIZE_OF_1K];
	if (m_bBarcodeConnect == true)
	{
		//m_clSerialThread.CloseBcrSerial();
		return true;
	}
	_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BCR ���� �õ�"));
	putListLog(szLog);
	// ���� ����
	m_clBarcodeConnSocket.SetMainDlgPtr(this);
	m_clBarcodeConnSocket.Create();
	//m_szBarcodeIp.Format("192.168.0.1");
	// MES ����
	if (m_clBarcodeConnSocket.Connect(sysData.m_szBarcodeIp, sysData.m_nBarcodePort) == FALSE)					//KEYENCE    SystemData.m_nBarcodePort
	{
		m_clBarcodeConnSocket.Close();
		m_bBarcodeConnect = false;
		//m_clButtonExMes.SetPress(3);
		_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BarCode(%s:%d) ���� ����"), sysData.m_szBarcodeIp, sysData.m_nBarcodePort);
		putListLog(szLog);
		return false;
	}
	else
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BarCode(%s:%d) ���� ����"), sysData.m_szBarcodeIp, sysData.m_nBarcodePort);
		putListLog(szLog);
		return false;
	}

	m_bBarcodeConnect = true;
	_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BarCode ���� ����"));
	putListLog(szLog);

	return true;

}

void CAABonderDlg::DisConnectBarcode()
{
	TCHAR szLog[SIZE_OF_1K];
	CString sSendBuff = _T("");
#if 0
	if (m_bBarcodeConnect == false)
		return;
	m_clBarcodeConnSocket.Close();
	m_bBarcodeConnect = false;
	_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BCR ���� ������"));		//DATALOGIC
	PrintLog(szLog, 20001, 1, false, 999);
#else
	if (m_bBarcodeConnect == false)

		sSendBuff.Format(_T("LON\r"));//KEYENCE
	return;
	m_clBarcodeConnSocket.Close();
	m_bBarcodeConnect = false;
	_stprintf_s(szLog, SIZE_OF_1K, _T("[LAN] BCR ���� ������"));
	putListLog(szLog);
#endif

}

bool CAABonderDlg::SendPacketToBarcode(bool bUse)
{
	TCHAR szLog[SIZE_OF_1K];
	int nSendSize;
	int nRetVal;

	CString sPacket = _T("");

#if 0
	BYTE sSendBuff[SIZE_OF_100BYTE];
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	sSendBuff[0] = BCR_STX;
	if (bUse == true)
	{
		sSendBuff[1] = 'L';
		sSendBuff[2] = 'O';
		sSendBuff[3] = 'N';
		sSendBuff[4] = BCR_ETX;
		nSendSize = 5;
	}
	else
	{
		sSendBuff[1] = 'L';
		sSendBuff[2] = 'O';
		sSendBuff[3] = 'F';
		sSendBuff[4] = 'F';
		sSendBuff[5] = BCR_ETX;
		nSendSize = 6;
	}

#else
	CString sSendBuff = _T("");
	if (bUse == true)
	{
		sSendBuff.Format(_T("LON\r"));
	}
	else
	{
		sSendBuff.Format(_T("LOFF\r"));
	}

	nSendSize = sSendBuff.GetLength() + 0;
#endif

	nRetVal = m_clBarcodeConnSocket.Send(sSendBuff, nSendSize);//nRetVal = m_clBarcodeConnSocket.Send(szLog, nSendSize);

	sPacket.Format(_T("%s"), sSendBuff);
	if (nRetVal != nSendSize)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("[FAIL] %s �۽� ����(Org:%d, Sent:%d)"), (TCHAR*)(LPCTSTR)sPacket, nSendSize, nRetVal);
		putListLog(szLog);
		return false;
	}
	_stprintf_s(szLog, SIZE_OF_1K, _T("[SEND] BCR - %s"), (TCHAR*)(LPCTSTR)sPacket);
	putListLog(szLog);

	sPacket.Empty();
	return true;
}



//! Added by LHW (2013/3/27)
//! �̹� �˻翡�� �̹��� Ȯ�� ���¿����� Pan ��� �߰��� �����ϰ� �ϱ� ���� �Լ��� �и�
int CAABonderDlg::Update_CCD_Display()
{
	double dReduceFactorX = 0.;
	double dReduceFactorY = 0.;

    dReduceFactorX = (double)SMALL_CCD_SIZE_X / gMIUDevice.nWidth; 
	dReduceFactorY = (double)SMALL_CCD_SIZE_Y / gMIUDevice.nHeight;

	//bool   bBox_CCD_Zoom = m_bBox_CCD_Zoom;
	bool   bCCD_Zoom     = m_bActing_CCD_Zoom;

	//CPoint ViewPos;
	CRect  rect_CCD_Zoom;

	if ( bCCD_Zoom == true )
	{
		m_csLock_CCD_Zoom.Lock();
		rect_CCD_Zoom = m_rect_CCD_Zoom;
		m_csLock_CCD_Zoom.Unlock();

		m_ViewPos.x = rect_CCD_Zoom.left;
		m_ViewPos.y = rect_CCD_Zoom.top;

		//! �̹��� ���� ��ġ Ȯ��
		if ( m_ViewPos.x < 0 )
		{
			m_ViewPos.x = 0;
		}
		if ( m_ViewPos.x + SMALL_CCD_SIZE_X > gMIUDevice.nWidth )
		{
			m_ViewPos.x = gMIUDevice.nWidth - SMALL_CCD_SIZE_X;
		}
		if ( m_ViewPos.y < 0 )
		{
			m_ViewPos.y = 0;
		}
		if ( m_ViewPos.y + SMALL_CCD_SIZE_Y > gMIUDevice.nHeight )
		{
			m_ViewPos.y = gMIUDevice.nHeight - SMALL_CCD_SIZE_Y;
		}
	}
	else
	{
		if(g_iCCDCamView == 0)		// �»�
		{
			m_ViewPos.x = 0;
			m_ViewPos.y = 0;
		}
		else if(g_iCCDCamView == 1)	// ���
		{
			m_ViewPos.x = gMIUDevice.nWidth - SMALL_CCD_SIZE_X;
			m_ViewPos.y = 0;
		}
		else if(g_iCCDCamView == 2)		//����
		{
			m_ViewPos.x = 0;
			m_ViewPos.y = gMIUDevice.nHeight - SMALL_CCD_SIZE_Y;
		}
		else if(g_iCCDCamView == 3)		//����
		{
			m_ViewPos.x = gMIUDevice.nWidth - SMALL_CCD_SIZE_X;
			m_ViewPos.y = gMIUDevice.nHeight - SMALL_CCD_SIZE_Y;
		}
		else if(g_iCCDCamView == 4)		// �߽�
		{
			m_ViewPos.x = gMIUDevice.nWidth/2 - SMALL_CCD_SIZE_X /2;
			m_ViewPos.y = gMIUDevice.nHeight/2 - SMALL_CCD_SIZE_Y /2;
		}
	}			


	if(vision.m_iDispMode==1 && !Task.AutoFlag)
	{
		MimBinarize(vision.MilGrabImageChild[3], vision.MilGrabImageChild[3], M_GREATER_OR_EQUAL, vision.m_iThValue, M_NULL);

		MbufCopy(vision.MilGrabImageChild[3], vision.MilGrabImageChild[4]);
		MbufCopy(vision.MilGrabImageChild[3], vision.MilGrabImageChild[5]);

		if ( bCCD_Zoom == true )
		{
			m_bState_CCD_Zoom = true;
			MbufCopyColor2d(vision.MilProcImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
		}
		else
		{
			if(g_iCCDCamView >= 0 && g_iCCDCamView <= 4)	// Ȯ�� ����
			{
				m_bState_CCD_Zoom = true;
				MbufCopyColor2d(vision.MilGrabImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
			}
			else
			{
				m_bState_CCD_Zoom = false;
				MimResize(vision.MilGrabImage[1], vision.MilSmallImage[1], dReduceFactorX, dReduceFactorY, M_DEFAULT);
			}
		}
	}
	else if(vision.m_iDispMode==2 && !Task.AutoFlag)
	{
		MbufCopy(vision.MilGrabImage[1], vision.MilProcImage[1]);

		if ( bCCD_Zoom == true )
		{
			m_bState_CCD_Zoom = true;
			MbufCopyColor2d(vision.MilProcImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
		}
		else
		{
			if(g_iCCDCamView >= 0 && g_iCCDCamView <= 4)	// Ȯ�� ����
			{
				m_bState_CCD_Zoom = true;
				MbufCopyColor2d(vision.MilProcImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
			}
			else
			{
				m_bState_CCD_Zoom = false;
				MimResize(vision.MilProcImage[1], vision.MilSmallImage[1], dReduceFactorX, dReduceFactorY, M_DEFAULT);
			}
		}
	}
	else
	{
		if ( bCCD_Zoom == true )
		{
			m_bState_CCD_Zoom = true;
			MbufCopyColor2d(vision.MilProcImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
		}
		else
		{
			if(g_iCCDCamView >= 0 && g_iCCDCamView <= 4)	// Ȯ�� ����
			{
				m_bState_CCD_Zoom = true;
				MbufCopyColor2d(vision.MilGrabImage[1], vision.MilSmallImage[1], M_ALL_BAND, m_ViewPos.x, m_ViewPos.y, M_ALL_BAND, 0, 0, SMALL_CCD_SIZE_X, SMALL_CCD_SIZE_Y);
			}
			else
			{
				m_bState_CCD_Zoom = false;
				MimResize(vision.MilGrabImage[1], vision.MilSmallImage[1], dReduceFactorX, dReduceFactorY, M_DEFAULT);
			}
		}
	}

	return 1;
}

int CAABonderDlg::WaitThreadEndH(HANDLE _hThread, int _nWaitMilSec, LPCTSTR _sMsg, bool _bClose)
{
	if (!_hThread) return 0;

	DWORD dwExit = 0;
	DWORD dwRetCode = WaitForSingleObject(_hThread, _nWaitMilSec);// _nWaitMilSec);  INFINITE
	if (dwRetCode == WAIT_OBJECT_0) // ��������
	{
	}
	else if (dwRetCode == WAIT_TIMEOUT) // Ÿ�Ӿƿ�
	{
		::TerminateThread(_hThread, 0);
	}
	return 1;
}

void CAABonderDlg::threadExit()
{
	DWORD dwResult;

	bool bAutoDel = false;// pThread_Clock->m_bAutoDelete;
	DWORD dwExit = 0;
	DWORD dwRetCode = 0;

	if (pThread_TaskLens != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_TaskLens->m_hThread, 3000, "pThread_TaskLens", false);//�ڵ��� ������ �ȵȴ�.
		pThread_TaskLens = NULL;
	//	delete pThread_TaskLens;
	}
	if (pThread_TaskPcb != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_TaskPcb->m_hThread, 3000, "pThread_TaskPcb", false);//�ڵ��� ������ �ȵȴ�.
		pThread_TaskPcb = NULL;
		//delete pThread_TaskPcb;
	}
	if (pThread_TaskReady != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_TaskReady->m_hThread, 3000, "pThread_TaskReady", false);//�ڵ��� ������ �ȵȴ�.
		pThread_TaskReady = NULL;
		//delete pThread_TaskReady;
	}
	if (pThread_TaskOrigin != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_TaskOrigin->m_hThread, 3000, "pThread_TaskOrigin", false);//�ڵ��� ������ �ȵȴ�.
		pThread_TaskOrigin = NULL;
		//delete pThread_TaskOrigin;
	}

	//
	if (pThread_CCM_Grab != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_CCM_Grab->m_hThread, 3000, "pThread_CCM_Grab", false);//�ڵ��� ������ �ȵȴ�.
		pThread_CCM_Grab = NULL;
		//delete pThread_CCM_Grab;
	}
	if (pThread_CCM_CvtColor != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_CCM_CvtColor->m_hThread, 3000, "pThread_CCM_CvtColor", false);//�ڵ��� ������ �ȵȴ�.
		pThread_CCM_CvtColor = NULL;
		//delete pThread_CCM_CvtColor;
	}
	if (pThread_CCM_CvtMil != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_CCM_CvtMil->m_hThread, 3000, "pThread_CCM_CvtMil", false);//�ڵ��� ������ �ȵȴ�.
		pThread_CCM_CvtMil = NULL;
		//delete pThread_CCM_CvtMil;
	}
	if (pThread_CCM_Display != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_CCM_Display->m_hThread, 3000, "pThread_CCM_Display", false);//�ڵ��� ������ �ȵȴ�.
		pThread_CCM_Display = NULL;
		//delete pThread_CCM_Display;
	}

	if (pThread_Monitor != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_Monitor->m_hThread, 3000, "pThread_Monitor", false);//�ڵ��� ������ �ȵȴ�.
		pThread_Monitor = NULL;
		//delete pThread_Monitor;
	}
	if (pThread_Clock != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_Clock->m_hThread, 3000, "pThread_Clock", false);//�ڵ��� ������ �ȵȴ�.
		pThread_Clock = NULL;
		//delete pThread_Clock;
	}
	if (pThread_Grab != NULL)
	{
		dwRetCode = WaitThreadEndH(pThread_Grab->m_hThread, 3000, "pThread_Grab", false);//�ڵ��� ������ �ȵȴ�.
		pThread_Grab = NULL;
		//delete pThread_Grab;
	}
	return;
}
void CAABonderDlg::OnBnClickedButtonExit()
{
	MIU.Close();

	if ( askMsg("�����Ͻðڽ��ϱ�?") == IDOK)
	{
		KillTimer(999);

		bThreadMonitor = false;
		bThreadTaskLens = false;
		bThreadTaskPcb = false;
		bThreadCcmGrab = false;
		bThreadClock = false;
		bThreadGrab = false;
		bThreadTaskReady = false;


		Sleep(300);
		threadExit();	//240103

		g_ADOData.func_AA_DBDisConnect();


#ifdef ON_LINE_VISION
		vision.closeVB();
#endif



		LightControl.Close_Device();
		/*LightControlSecond.Close_Device();
		LightControlthird.Close_Device();
		LightControlFourth.Close_Device();
		LightControlFifth.Close_Device();*/
		UVControl.Close_Device();
		Keyence.func_RS232_CommClose();
		/////barcode.func_Comm_Close(); Sleep(10);

		motor.AmpDisableAll();


		// �α� ������ ����
		m_clLogThread.StopLogging();
		m_clLogThread.EndThread();//EndThread();
		if (sfrSpecDlg != NULL)
		{
			sfrSpecDlg->DestroyWindow();
			delete sfrSpecDlg;
			sfrSpecDlg = NULL;
		}
		if (chartSetDlg != NULL)
		{
			chartSetDlg->DestroyWindow();
			delete chartSetDlg;
			chartSetDlg = NULL;
		}
		
		if (modelDlg!=NULL)
		{
			modelDlg->DestroyWindow();
			delete modelDlg;
			modelDlg = NULL;
		}

		if (lensDlg != NULL)
		{
			lensDlg->DestroyWindow();
			delete lensDlg;
			lensDlg = NULL;
		}
		if (lensEdgeDlg != NULL)
		{
			lensEdgeDlg->DestroyWindow();
			delete lensEdgeDlg;
			lensEdgeDlg = NULL;
		}

		if (pcbDlg != NULL)
		{
			pcbDlg->DestroyWindow();
			delete pcbDlg;
			pcbDlg = NULL;
		}

		if (ccdDlg != NULL)
		{
			ccdDlg->DestroyWindow();
			delete ccdDlg;
			ccdDlg = NULL;
		}
		if (motorDlg != NULL)
		{
			motorDlg->DestroyWindow();
			delete motorDlg;
			motorDlg = NULL;
		}
		if (motorDlg2 != NULL)
		{
			motorDlg2->DestroyWindow();
			delete motorDlg2;
			motorDlg2 = NULL;
		}
		if (motorDlg3 != NULL)
		{
			motorDlg3->DestroyWindow();
			delete motorDlg3;
			motorDlg3 = NULL;
		}
		/*if (lightDlg != NULL)
		{
			lightDlg->DestroyWindow();
			delete lightDlg;
			lightDlg = NULL;
		}*/

		if (ioDlg != NULL)
		{
			ioDlg->DestroyWindow();
			delete ioDlg;
			ioDlg = NULL;
		}

		if (delayDlg != NULL)
		{
			delayDlg->DestroyWindow();
			delete delayDlg;
			delayDlg = NULL;
		}

		if (autodispDlg != NULL)
		{
			autodispDlg->DestroyWindow();
			delete autodispDlg;
			autodispDlg = NULL;
		}

		if (alarmDlg != NULL)
		{
			alarmDlg->DestroyWindow();
			delete alarmDlg;
			alarmDlg = NULL;
		}

		
		destoryStandardCursor();
		sTempLang.Empty();
		sLangChange.Empty();
		theApp.MainDlg = NULL;
		

		CDialogEx::OnOK();
	}
}


void CAABonderDlg::ctrlSubDlg(int iDlgNo)
{

	if ((iDlgNo == MOTOR_DLG && motorDlg2->IsWindowVisible()) ||
		(iDlgNo == MOTOR_DLG2 && motorDlg->IsWindowVisible()) ||
		(iDlgNo == MOTOR_DLG3 && motorDlg3->IsWindowVisible())
		)
	{

	}
	else
		password.m_bFlag = false;


	if (modelDlg->IsWindowVisible())
		modelDlg->ShowWindow(SW_HIDE);

	if (lensDlg->IsWindowVisible())
		lensDlg->ShowWindow(SW_HIDE);

	if (lensEdgeDlg->IsWindowVisible())
		lensEdgeDlg->ShowWindow(SW_HIDE);

	if (pcbDlg->IsWindowVisible())
		pcbDlg->ShowWindow(SW_HIDE);

	if (ccdDlg->IsWindowVisible())
	{
		ccdDlg->Hide_All_Child_Dialog();
		ccdDlg->ShowWindow(SW_HIDE);
	}

	if (motorDlg->IsWindowVisible())
		motorDlg->ShowWindow(SW_HIDE);

	if (motorDlg2->IsWindowVisible())
		motorDlg2->ShowWindow(SW_HIDE);

	if (motorDlg3->IsWindowVisible())
		motorDlg3->ShowWindow(SW_HIDE);
	

	//if (lightDlg->IsWindowVisible())
		//lightDlg->ShowWindow(SW_HIDE);


	if (ioDlg->IsWindowVisible())
		ioDlg->ShowWindow(SW_HIDE);

	if (alarmDlg->IsWindowVisible())
		alarmDlg->ShowWindow(SW_HIDE);

	//if (g_pFoceDlg->IsWindowVisible())
	//	g_pFoceDlg->ShowWindow(SW_HIDE);

	if (m_iOldDlgNo == iDlgNo)							//���� ���̾�α� cam ȭ�� 
		iDlgNo = MAIN_DLG;

	/*if (iDlgNo == IDD_DIALOG_LIGHT)						
	{
	if (!m_bisLightBtn)
	{
	iDlgNo = MAIN_DLG;
	}
	}*/

	if (iDlgNo == LENS_DLG || iDlgNo == PCB_DLG)			//���� ���̾�α� cam ȭ�� 
	{
		if (!m_bisAlignBtn)
		{
			iDlgNo = MAIN_DLG;
		}
	}

	if (iDlgNo == MOTOR_DLG ||
		iDlgNo == MOTOR_DLG2 ||
		iDlgNo == MOTOR_DLG3 ||
		iDlgNo == MOTOR_DLG4 ||
		iDlgNo == MOTOR_INSPDLG)
	{
		if (!m_bisMotorBtn)
		{
			iDlgNo = MAIN_DLG;
		}
	}

	if (m_bisAlignBtn && iDlgNo != LENS_DLG && iDlgNo != PCB_DLG && LENS_EDGE_DLG != iDlgNo)
		m_bisAlignBtn = false;

	if (m_bisMotorBtn && iDlgNo != MOTOR_DLG && iDlgNo != MOTOR_DLG2 && iDlgNo != MOTOR_DLG3 && iDlgNo != MOTOR_DLG4 && iDlgNo != MOTOR_INSPDLG)
		m_bisMotorBtn = false;

	/*if(iDlgNo==MAIN_DLG)
	{
	autodispDlg->ShowWindow(true);
	}else
	{
	autodispDlg->ShowWindow(false);
	}*/

	switch (iDlgNo)
	{
	case MAIN_DLG:
		setCamDisplay(m_iCurCamNo, 1);//1); 201012
		//autodispDlg->ShowWindow(SW_SHOW);
		break;
	case MODEL_DLG:
		setCamDisplay(1, 0);//1);
		modelDlg->ShowWindow(SW_SHOW);

		break;
	case LENS_DLG:
		m_iCurCamNo = 1;
		setCamDisplay(1, 0);//PCB//setCamDisplay(0, 1);//PCB
		lensDlg->ShowWindow(SW_SHOW);
		break;
	case LENS_EDGE_DLG:
		m_iCurCamNo = 1;
		setCamDisplay(1, 0);
		lensEdgeDlg->ShowWindow(SW_SHOW);
		break;
	case PCB_DLG:
		m_iCurCamNo = 1;
		setCamDisplay(1, 0);//setCamDisplay(0, 1);//PCB
		pcbDlg->ShowWindow(SW_SHOW);
		break;

	case CCD_DLG:
		m_iCurCamNo = 3;
		setCamDisplay(3, 0);//setCamDisplay(3, 1);
		ccdDlg->ShowWindow(SW_SHOW);

		break;
	case MOTOR_DLG:
		setCamDisplay(m_iCurCamNo, 0);// 1);
		motorDlg->ShowWindow(SW_SHOW);

		break;
	case MOTOR_DLG2:
		setCamDisplay(m_iCurCamNo, 0);// 1);
		motorDlg2->ShowWindow(SW_SHOW);

		break;
	case MOTOR_DLG3:
		setCamDisplay(m_iCurCamNo, 0);// 1);
		motorDlg3->ShowWindow(SW_SHOW);
		break;

	case IDD_DIALOG_LIGHT:
		//setCamDisplay(m_iCurCamNo, 0);//1);
		//lightDlg->ShowWindow(SW_SHOW);

		break;
	case IO_DLG:
		setCamDisplay(m_iCurCamNo, 0);//1);
		ioDlg->ShowWindow(SW_SHOW);

		break;

	case ALARM_DLG:
		setCamDisplay(m_iCurCamNo, 0);//1);

		alarmDlg->m_iAlarmKind = e_AlarmHistory;
		alarmDlg->ShowWindow(SW_SHOW);

		break;
	}

	m_iOldDlgNo = iDlgNo;
}



void CAABonderDlg::OnBnClickedButtonMain()
{
	ctrlSubDlg(MAIN_DLG);
	//setCamDisplay(m_iCurCamNo, 0);
	changeMainBtnColor(MAIN_DLG);
	for(int i=0; i<MARK_CNT; i++)
	{
		//SetDigReference(i);
	}
}


void CAABonderDlg::OnBnClickedButtonLens()
{
	if (m_bisAlignBtn)	m_bisAlignBtn = false;
	else				m_bisAlignBtn = true;

	if (m_bIsLensMode == 0)		ctrlSubDlg(LENS_DLG);
	else if(m_bIsLensMode == 1)	ctrlSubDlg(PCB_DLG);
	else						ctrlSubDlg(LENS_EDGE_DLG);

	changeMainBtnColor(PCB_DLG);			
}


void CAABonderDlg::OnBnClickedButtonCcd()
{
	ctrlSubDlg(CCD_DLG);
	changeMainBtnColor(CCD_DLG);
}


void CAABonderDlg::OnBnClickedButtonMotor()
{ 
	if (m_bisMotorBtn)	m_bisMotorBtn = false;
	else				m_bisMotorBtn = true;

	if( m_bIsMotorMode == 0 )		ctrlSubDlg(MOTOR_DLG);
	else if( m_bIsMotorMode == 1 )	ctrlSubDlg(MOTOR_DLG2);
	else if( m_bIsMotorMode	== 2 )	ctrlSubDlg(MOTOR_DLG3);
	else							ctrlSubDlg(MOTOR_DLG4);
	changeMainBtnColor(MOTOR_DLG2);
}


void CAABonderDlg::OnBnClickedButtonIo()
{
	ctrlSubDlg(IO_DLG);
	changeMainBtnColor(IO_DLG);
}


void CAABonderDlg::OnBnClickedButtonModel()
{
	ctrlSubDlg(MODEL_DLG);
	changeMainBtnColor(MODEL_DLG);
}



void CAABonderDlg::dispStepOnButton(int iCtrlNo, int iState)
{

}

//! ���� ȭ�鿡 ǥ�õǾ� �ִ� �簢 ������ ������ ����
void CAABonderDlg::Change_Mode_Mouse_Box(int iMode_Mouse_Box)
{
	//! ���� ���õ� ���� ���� �簢 ���� ǥ�ø� �ϴ� ������ �����Ѵ�. 
	COLORREF clrBoxArea = GetColor_Mouse_Box(iMode_Mouse_Box);

	vision.ChangeColor_Box(m_iCurCamNo, clrBoxArea);
	vision.drawOverlay(m_iCurCamNo);
}

//! �Էµ� ���� �´� �簢 ������ ������ ��ȯ
COLORREF CAABonderDlg::GetColor_Mouse_Box(int iMode_Mouse_Box)
{
	COLORREF clrRet = RGB(255, 0, 0);

	switch(iMode_Mouse_Box)
	{
	case 0:
		{
//			clrRet = RGB(0, 255, 0);
			clrRet = RGB(255, 0, 0);
		}
		break;
	case 1:
		{
			clrRet = RGB(0, 0, 255);
		}
		break;
	case 2:
		{
			clrRet = RGB(255, 255, 0);
		}
		break;
	case 3:
		{
			clrRet = RGB(0, 255, 255);
		}
		break;
	case 4:
		{
			clrRet = RGB(255, 128, 0);
		}
		break;		
	}

	return clrRet;
}


void CAABonderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bDrawFlag			= false;
	m_bBoxMoveFlag		= false;
	m_bBoxMoveFlag_CCD	= false;
	m_bDrawMeasureLine	= false;
	//! <-------------------------------------
	//! Added by LHW (2013/3/27)
	if ( m_bPan_CCD_Zoom == true )
	{
		::SetCursor(m_hCursor_Standard);		
	}	
	if ( m_bBox_CCD_Zoom == true )
	{
		if ( m_bBox_Acting_CCD_Zoom == true && m_bState_CCD_Zoom == false )
		{
			//! �簢 ������ ���� ��ġ���� Ȯ���Ѵ�.

			m_csLock_CCD_Zoom.Lock();
			m_rect_CCD_Zoom.left = m_rBox.left;
			m_rect_CCD_Zoom.top  = m_rBox.top;
			if ( m_rect_CCD_Zoom.left < 0 )
			{
				m_rect_CCD_Zoom.left = 0;
			}
			if ( m_rect_CCD_Zoom.left + SMALL_CCD_SIZE_X > gMIUDevice.nWidth )
			{
				m_rect_CCD_Zoom.left = gMIUDevice.nWidth - SMALL_CCD_SIZE_X;
			}
			if ( m_rect_CCD_Zoom.top < 0 )
			{
				m_rect_CCD_Zoom.top = 0;
			}
			if ( m_rect_CCD_Zoom.top + SMALL_CCD_SIZE_Y > gMIUDevice.nHeight )
			{
				m_rect_CCD_Zoom.top = gMIUDevice.nHeight - SMALL_CCD_SIZE_Y;
			}
			m_csLock_CCD_Zoom.Unlock();

			m_bActing_CCD_Zoom = true;
			m_bPan_CCD_Zoom = true;

			if ( gMIUDevice.CurrentState < 3 )
			{
				//! CCD OffLine ������ ��
				Update_CCD_Display();
			}
		}

		m_rBox.SetRectEmpty();
		vision.clearOverlay(m_iCurCamNo);
		vision.drawOverlay(m_iCurCamNo);
	}
	m_bActing_Pan_CCD_Zoom = false;
	m_bBox_Acting_CCD_Zoom = false;
	//! <-------------------------------------

	if (m_iCurCamNo==CAM1)
	{
		lensDlg->m_rMarkBox = lensDlg->m_rcFixedBox = m_rBox;		
		pcbDlg->m_rMarkBox = pcbDlg->m_rcFixedBox =m_rBox;

		lensEdgeDlg->m_rMarkBox = lensEdgeDlg->m_rcFixedBox = m_rBox;

	}
	else if (m_iCurCamNo==CAM2)
	{
		lensEdgeDlg->m_rMarkBox = lensEdgeDlg->m_rcFixedBox =m_rBox;
	}
#if 0
	if (point.x>m_rectCamDispPos1.left	&&
		point.x<m_rectCamDispPos1.right &&
		point.y>m_rectCamDispPos1.top	&&
		point.y<m_rectCamDispPos1.bottom)
	{
		if (motorDlg->m_bCalcResol) 
		{
			motorDlg->registPatMark(m_iCurCamNo, m_rBox);
			motorDlg->m_bFindTop = motorDlg->m_bFindBottom = motorDlg->m_bFindLeft = motorDlg->m_bFindRight = false;
			//		vision.clearOverlay(m_iCurCamNo);
			motorDlg->drawResolBackGround();
			vision.drawOverlay(m_iCurCamNo);

			m_rBox.left		= 0;
			m_rBox.top		= 0;
			m_rBox.right	= 0;
			m_rBox.bottom	= 0;
		}else if (motorDlg2->m_bCalcResol)
		{
			motorDlg2->registPatMark(m_iCurCamNo, m_rBox);
			motorDlg2->m_bFindTop = motorDlg2->m_bFindBottom = motorDlg2->m_bFindLeft = motorDlg2->m_bFindRight = false;
			//		vision.clearOverlay(m_iCurCamNo);
			motorDlg2->drawResolBackGround();
			vision.drawOverlay(m_iCurCamNo);

			m_rBox.left		= 0;
			m_rBox.top		= 0;
			m_rBox.right	= 0;
			m_rBox.bottom	= 0;
		}
		else if (motorDlg3->m_bCalcResol)
		{
			motorDlg3->registPatMark(m_iCurCamNo, m_rBox);
			motorDlg3->m_bFindTop = motorDlg3->m_bFindBottom = motorDlg3->m_bFindLeft = motorDlg3->m_bFindRight = false;
			//		vision.clearOverlay(m_iCurCamNo);
			motorDlg3->drawResolBackGround();
			vision.drawOverlay(m_iCurCamNo);

			m_rBox.left		= 0;
			m_rBox.top		= 0;
			m_rBox.right	= 0;
			m_rBox.bottom	= 0;
		}
		else if (motorDlg4->m_bCalcResol)
		{
			motorDlg4->registPatMark(m_iCurCamNo, m_rBox);
			motorDlg4->m_bFindTop = motorDlg4->m_bFindBottom = motorDlg4->m_bFindLeft = motorDlg4->m_bFindRight = false;
			//		vision.clearOverlay(m_iCurCamNo);
			motorDlg4->drawResolBackGround();
			vision.drawOverlay(m_iCurCamNo);

			m_rBox.left = 0;
			m_rBox.top = 0;
			m_rBox.right = 0;
			m_rBox.bottom = 0;
		}
	}
#endif
	ReleaseCapture();

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CAABonderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if((nFlags & 0x01) == false)
	{
		m_bDrawFlag			= false;
		m_bBoxMoveFlag		= false;
		m_bBoxMoveFlag_CCD	= false;
		m_bDrawMeasureLine	= false;
	}

	double dExpandFactorX, dExpandFactorY;
	if (m_bBoxMoveFlag_CCD)
	{
		if (point.x<m_rectCcdDispPos.left ||
			point.x>m_rectCcdDispPos.right ||
			point.y<m_rectCcdDispPos.top ||
			point.y>m_rectCcdDispPos.bottom && !m_bMeasureDist)
		{
			m_iMoveType = checkMousePos(point, m_rBox);
		}
	}
	else
	{
		if (point.x<m_rectCamDispPos1.left ||
			point.x>m_rectCamDispPos1.right ||
			point.y<m_rectCamDispPos1.top ||
			point.y>m_rectCamDispPos1.bottom && !m_bMeasureDist)
		{
			m_iMoveType = checkMousePos(point, m_rBox);
		}
	}

	/*if (point.x<m_rectCamDispPos1.left	||
		point.x>m_rectCamDispPos1.right ||
		point.y<m_rectCamDispPos1.top   ||
		point.y>m_rectCamDispPos1.bottom  && !m_bMeasureDist )
	{
		m_iMoveType = checkMousePos(point, m_rBox);	
	}*/
	
	//! if ( ccdDlg->m_pDefectDlg->IsWindowVisible() )	

	if ( m_bMeasureDist && !m_bDrawMeasureLine )
	{
		m_iType_MeasureLine = changeCursor_MeasureMode(point);
	}
	else
		changeCursor(point, m_rBox);

	if (m_bDrawFlag)
	{
		if (point.x>m_rectCamDispPos1.left	&&
			point.x<m_rectCamDispPos1.right &&
			point.y>m_rectCamDispPos1.top	&&
			point.y<m_rectCamDispPos1.bottom)
		{
			if (m_iCurCamNo<3)
			{
				dExpandFactorX = CAM_EXPAND_FACTOR_X;
				dExpandFactorY = CAM_EXPAND_FACTOR_Y;
			}
			else
			{
				dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
				dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
			}

			point.x -= m_rectCamDispPos1.left;
			point.y -= m_rectCamDispPos1.top;

			if (m_bBoxMoveFlag)
			{
				int iMoveX = (int)(point.x * dExpandFactorX + 0.5) -(int)(m_ClickP.x * dExpandFactorX + 0.5);
				int iMoveY = (int)(point.y * dExpandFactorY + 0.5) - (int)(m_ClickP.y * dExpandFactorY + 0.5);

				/* �̵� */
				if (m_iMoveType==MOVE_ALL)
				{
					m_rBox.left		+= iMoveX;
					m_rBox.top		+= iMoveY;
					m_rBox.right	+= iMoveX;
					m_rBox.bottom	+= iMoveY;
				}
				/* �� ũ�� */
				else if (m_iMoveType==MOVE_WIDTH_LEFT)
				{
					m_rBox.left		+= iMoveX;
				}
				/* �� ũ�� */
				else if (m_iMoveType==MOVE_WIDTH_RIGHT)
				{
					m_rBox.right	+= iMoveX;
				}
				/* �� ũ�� */
				else if (m_iMoveType==MOVE_HEIGHT_TOP)
				{
					m_rBox.top		+= iMoveY;
				}
				/* �� ũ�� */
				else if (m_iMoveType==MOVE_HEIGHT_BOTTOM)
				{
					m_rBox.bottom	+= iMoveY;
				}
				/* �»� ũ�� */
				else if (m_iMoveType==MOVE_NW)
				{
					m_rBox.left		+= iMoveX;
					m_rBox.top		+= iMoveY;
				}
				/* ��� ũ�� */
				else if (m_iMoveType==MOVE_NE)
				{
					m_rBox.top		+= iMoveY;
					m_rBox.right	+= iMoveX;
				}
				/* ���� ũ�� */
				else if (m_iMoveType==MOVE_SW)
				{
					m_rBox.left		+= iMoveX;
					m_rBox.bottom	+= iMoveY;
				}
				/* ���� ũ�� */
				else if (m_iMoveType==MOVE_SE)
				{
					m_rBox.right	+= iMoveX;
					m_rBox.bottom	+= iMoveY;
				}

				m_ClickP = point;
			}
			else
			{
				m_rBox.left		= (int)(m_ClickP.x * dExpandFactorX + 0.5);
				m_rBox.top		= (int)(m_ClickP.y * dExpandFactorY + 0.5);
				m_rBox.right	= (int)(point.x	* dExpandFactorX + 0.5);
				m_rBox.bottom	= (int)(point.y * dExpandFactorY + 0.5);
			}

			if (m_rBox.left>m_rBox.right)
				SWAP(m_rBox.left, m_rBox.right);
			if (m_rBox.top>m_rBox.bottom)
				SWAP(m_rBox.top, m_rBox.bottom);

			m_rcFixedBox = m_rBox;

			if (m_rBox.left<1)
			{
				m_rBox.left		= 1;
				m_rBox.right	= 1 + m_rcFixedBox.Width();
			}
			if (m_rBox.top<1)
			{
				m_rBox.top		= 1;
				m_rBox.bottom	= 1 + m_rcFixedBox.Height();
			}
			if (m_iCurCamNo<3)
			{
				if (m_rBox.right>CAM_SIZE_X-1)
				{
					m_rBox.right	= CAM_SIZE_X - 1;
					m_rBox.left		= m_rBox.right - m_rcFixedBox.Width();
				}
				if (m_rBox.bottom>CAM_SIZE_Y-1)
				{
					m_rBox.bottom	= CAM_SIZE_Y -1;
					m_rBox.top		= m_rBox.bottom - m_rcFixedBox.Height();
				}
			}
			else
			{
				if (m_rBox.right>gMIUDevice.nWidth-1)
				{
					m_rBox.right	= gMIUDevice.nWidth - 1;
					m_rBox.left		= m_rBox.right - m_rcFixedBox.Width();
				}
				if (m_rBox.bottom>gMIUDevice.nHeight-1)
				{
					m_rBox.bottom	= gMIUDevice.nHeight -1;
					m_rBox.top		= m_rBox.bottom - m_rcFixedBox.Height();
				}
			}

			if ( m_iCurCamNo == 3 )
			{
				//! Added by LHW (2013/3/27)

				//! ���� ���õ� ���� ���� �簢 ���� ǥ�ø� �ϴ� ������ �����Ѵ�. 
				COLORREF clrBoxArea = GetColor_Mouse_Box(m_iMode_Mouse_Box);

				//! �簢 ������ �׸���. 
				vision.clearOverlay(m_iCurCamNo);
				vision.boxlist[m_iCurCamNo].addList(m_rBox, PS_SOLID, clrBoxArea);

				vision.drawOverlay(m_iCurCamNo);
			}
			else
			{
				//! <----------------------------------------------------------------------------
				//! Modify by LHW (2013/2/5)
				//vision.clearOverlay(m_iCurCamNo);
				//vision.boxlist[m_iCurCamNo].addList(m_rBox, PS_DOT, M_COLOR_GREEN);
				//vision.drawOverlay(m_iCurCamNo);

				//! ���� ���õ� ���� ���� �簢 ���� ǥ�ø� �ϴ� ������ �����Ѵ�. 
				COLORREF clrBoxArea = GetColor_Mouse_Box(m_iMode_Mouse_Box);

				//! �簢 ������ �׸���. 
				vision.clearOverlay(m_iCurCamNo);
				vision.boxlist[m_iCurCamNo].addList(m_rBox, PS_SOLID, clrBoxArea);

				if (m_iCurCamNo==3)	ccdDlg->m_pSFRDlg->drawROI();

				vision.drawOverlay(m_iCurCamNo);
				//! <---------------------------------------------------------------------------
			}

			GetDlgItem(IDC_DISP_LENS+m_iCurCamNo)->UpdateWindow();
		}

		SetCapture();
	}
	else if (m_bBoxMoveFlag_CCD)
	{
		if (m_iNo_SFR!=-1)
		{
			dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
			dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;

			//point.x -= m_rectCamDispPos1.left;
			//point.y -= m_rectCamDispPos1.top;
			point.x -= m_rectCcdDispPos.left;//m_rectCamDispPos1.left;
			point.y -= m_rectCcdDispPos.top;//m_rectCamDispPos1.top;

			int iMoveX = (int)(point.x * dExpandFactorX + 0.5) -(int)(m_ClickP.x * dExpandFactorX + 0.5);
			int iMoveY = (int)(point.y * dExpandFactorY + 0.5) - (int)(m_ClickP.y * dExpandFactorY + 0.5);

			/* �̵� */
			if (m_iMoveType==MOVE_ALL)
			{
				m_rBox.left		+= iMoveX;
				m_rBox.top		+= iMoveY;
				m_rBox.right	+= iMoveX;
				m_rBox.bottom	+= iMoveY;
			}
			/* �� ũ�� */
			else if (m_iMoveType==MOVE_WIDTH_LEFT)
			{
				m_rBox.left		+= iMoveX;
			}
			/* �� ũ�� */
			else if (m_iMoveType==MOVE_WIDTH_RIGHT)
			{
				m_rBox.right	+= iMoveX;
			}
			/* �� ũ�� */
			else if (m_iMoveType==MOVE_HEIGHT_TOP)
			{
				m_rBox.top		+= iMoveY;
			}
			/* �� ũ�� */
			else if (m_iMoveType==MOVE_HEIGHT_BOTTOM)
			{
				m_rBox.bottom	+= iMoveY;
			}
			/* �»� ũ�� */
			else if (m_iMoveType==MOVE_NW)
			{
				m_rBox.left		+= iMoveX;
				m_rBox.top		+= iMoveY;
			}
			/* ��� ũ�� */
			else if (m_iMoveType==MOVE_NE)
			{
				m_rBox.top		+= iMoveY;
				m_rBox.right	+= iMoveX;
			}
			/* ���� ũ�� */
			else if (m_iMoveType==MOVE_SW)
			{
				m_rBox.left		+= iMoveX;
				m_rBox.bottom	+= iMoveY;
			}
			/* ���� ũ�� */
			else if (m_iMoveType==MOVE_SE)
			{
				m_rBox.right	+= iMoveX;
				m_rBox.bottom	+= iMoveY;
			}

			m_ClickP = point;

			if (m_rBox.left>m_rBox.right)
				SWAP(m_rBox.left, m_rBox.right);
			if (m_rBox.top>m_rBox.bottom)
				SWAP(m_rBox.top, m_rBox.bottom);

			m_rcFixedBox = m_rBox;

			if (m_rBox.left<1)
			{
				m_rBox.left		= 1;
				m_rBox.right	= 1 + m_rcFixedBox.Width();
			}
			if (m_rBox.top<1)
			{
				m_rBox.top		= 1;
				m_rBox.bottom	= 1 + m_rcFixedBox.Height();
			}

			if (m_rBox.right>gMIUDevice.nWidth-1)
			{
				m_rBox.right	= gMIUDevice.nWidth - 1;
				m_rBox.left		= m_rBox.right - m_rcFixedBox.Width();
			}
			if (m_rBox.bottom>gMIUDevice.nHeight-1)
			{
				m_rBox.bottom	= gMIUDevice.nHeight -1;
				m_rBox.top		= m_rBox.bottom - m_rcFixedBox.Height();
			}

			if (m_iNo_SFR < model.mGlobalChartCount)	//if(m_iNo_SFR<LAST_MARK_CNT)					// �簢 ��ũ ��ġ..
			{
				int iTemp;
				if(m_rBox.right < m_rBox.left)
				{
					iTemp=m_rBox.right;
					m_rBox.right=m_rBox.left;
					m_rBox.left=iTemp;
				}
				if(m_rBox.bottom < m_rBox.top)
				{
					iTemp=m_rBox.bottom;
					m_rBox.bottom=m_rBox.top;
					m_rBox.top=iTemp;
				}				
				
				ccdDlg->m_pSFRDlg->m_iOffsetX_SFR[m_iNo_SFR]	= m_rBox.left;
				ccdDlg->m_pSFRDlg->m_iOffsetY_SFR[m_iNo_SFR]	= m_rBox.top;

				if (m_iMoveType!=MOVE_ALL)
				{
					ccdDlg->m_pSFRDlg->m_iSizeX_SFR[m_iNo_SFR]	= m_rBox.right - m_rBox.left;
					ccdDlg->m_pSFRDlg->m_iSizeY_SFR[m_iNo_SFR]	= m_rBox.bottom - m_rBox.top;
				}
				else
				{
					ccdDlg->m_pSFRDlg->m_iSizeX_SFR[m_iNo_SFR]	= m_rBox.right - m_rBox.left;
					ccdDlg->m_pSFRDlg->m_iSizeY_SFR[m_iNo_SFR]	= m_rBox.bottom - m_rBox.top;
				}
			}
			else if (m_iNo_SFR < model.mGlobalChartCount + 4)//else if(m_iNo_SFR<LAST_MARK_CNT+4)			// ���� ��ũ �˻� ��ġ..
			{
				ccdDlg->m_pSFRDlg->m_rcRoiBox[m_iNo_SFR - model.mGlobalChartCount] = m_rBox;
				//ccdDlg->m_pSFRDlg->m_rcRoiBox[m_iNo_SFR - LAST_MARK_CNT] = m_rBox;
			}


			COLORREF clrBoxArea = GetColor_Mouse_Box(m_iMode_Mouse_Box);

			vision.clearOverlay(CCD);
			ccdDlg->m_pSFRDlg->drawRectSFR(m_iNo_SFR);
			vision.drawOverlay(CCD);

			GetDlgItem(IDC_DISP_LENS+m_iCurCamNo)->UpdateWindow();
		}

		SetCapture();
	}
	else if ( point.x>m_rectCamDispPos1.left	&&
			  point.x<m_rectCamDispPos1.right	&&
			  point.y>m_rectCamDispPos1.top		&&
			  point.y<m_rectCamDispPos1.bottom	&& m_bDrawMeasureLine == true )
	{
		if (m_iCurCamNo<3)
		{
			dExpandFactorX = CAM_EXPAND_FACTOR_X;
			dExpandFactorY = CAM_EXPAND_FACTOR_Y;
		}
		else
		{
			dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
			dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
		}

		point.x -= m_rectCamDispPos1.left;
		point.y -= m_rectCamDispPos1.top;

		int iMoveX = (int)(point.x * dExpandFactorX + 0.5) - (int)(m_ClickP.x * dExpandFactorX + 0.5);
		int iMoveY = (int)(point.y * dExpandFactorY + 0.5) - (int)(m_ClickP.y * dExpandFactorY + 0.5);

		m_ClickP = point;

		/* Left Line */
		if ( m_iType_MeasureLine == 1 )
		{
			m_iLine_Left += iMoveX;
		}
		/* Top Line */
		else if ( m_iType_MeasureLine == 2 )
		{
			m_iLine_Top += iMoveY;
		}
		/* Right Line */
		else if ( m_iType_MeasureLine == 3 )
		{
			m_iLine_Right += iMoveX;
		}
		/* Bottom Line */
		else if ( m_iType_MeasureLine == 4 )
		{
			m_iLine_Bottom += iMoveY;
		}

		drawLine_MeasureDist(1);

		SetCapture();
	}


	CDialogEx::OnMouseMove(nFlags, point);
}


void CAABonderDlg::OnBnClickedButtonLensSupply()
{
}



void CAABonderDlg::OnBnClickedButtonPcbSupply()
{
}


void CAABonderDlg::OnBnClickedButtonPcbOsCheck()
{
}


void CAABonderDlg::OnBnClickedButtonCcdAlign()
{
	/* �̵����̸� Return */
	if(g_bMovingflag)
		return;
	g_bMovingflag =true;


	Task.PCBTask	= 60000;
	Task.m_iStart_Step_PCB = 60000;
	Task.m_iEnd_Step_PCB = 100000;///110000;  250107

	pThread_TaskPcb = ::AfxBeginThread(Thread_TaskPcb, this);
}


void CAABonderDlg::OnBnClickedButtonCcdInsp()
{
	/* �̵����̸� Return */
	if(g_bMovingflag)
		return;
	g_bMovingflag =true;

	Task.m_iStart_Step_PCB	= 60000;
	Task.m_iEnd_Step_PCB	= 110000;

	pThread_TaskPcb = ::AfxBeginThread(Thread_TaskPcb, this);
	//pThread_TaskLens = ::AfxBeginThread(Thread_TaskLens, this);
}


void CAABonderDlg::putListLog(CString strLog)
{
	if (!m_listLog)
	{
		return;
	}
	SYSTEMTIME stSysTime;
	//CString sListLog;
	int nCount;

	GetLocalTime(&stSysTime);
	TCHAR sListLog[SIZE_OF_1K];
	//sListLog.Format(_T("[%02d:%02d:%02d.%03d] %s"), stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond, stSysTime.wMilliseconds, (TCHAR*)(LPCTSTR)strLog);

	_stprintf_s(sListLog, SIZE_OF_1K, _T("[%02d:%02d:%02d.%03d] %s"), stSysTime.wHour, stSysTime.wMinute, stSysTime.wSecond, stSysTime.wMilliseconds, (TCHAR*)(LPCTSTR)strLog);

	m_clLogThread.Log(sListLog);



	/*nCount = m_listLog.GetCount();
	if (nCount > 1000)
	{
		m_listLog.DeleteString(0);
	}
	nCount = m_listLog.AddString(sListLog);
	m_listLog.SetCurSel(nCount);
	LogSave(sListLog);*/

	//
	/*if (m_listLog.GetCount()>10000)
		m_listLog.ResetContent();

	m_listLog.InsertString(m_listLog.GetCount(), strLog);
	m_listLog.SetCurSel(m_listLog.GetCount()-1);

	LogSave(strLog);*/
}


void CAABonderDlg::loadStandardCursor()
{
	m_hCursor_Standard	= NULL;
	m_hCursor_Width		= NULL;
	m_hCursor_Height	= NULL;
	m_hCursor_Move		= NULL;
	m_hCursor_NWSE		= NULL;
	m_hCursor_NESW		= NULL;

	m_hCursor_Standard	= AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_hCursor_Width		= AfxGetApp()->LoadStandardCursor(IDC_SIZEWE);
	m_hCursor_Height	= AfxGetApp()->LoadStandardCursor(IDC_SIZENS);
	m_hCursor_Move		= AfxGetApp()->LoadStandardCursor(IDC_SIZEALL);
	m_hCursor_NWSE		= AfxGetApp()->LoadStandardCursor(IDC_SIZENWSE);
	m_hCursor_NESW		= AfxGetApp()->LoadStandardCursor(IDC_SIZENESW);
}

void CAABonderDlg::destoryStandardCursor()
{
	if (m_hCursor_Standard != NULL)
		::DestroyCursor(m_hCursor_Standard);

	if (m_hCursor_Width != NULL)
		::DestroyCursor(m_hCursor_Width);

	if (m_hCursor_Height != NULL)
		::DestroyCursor(m_hCursor_Height);

	if (m_hCursor_Move != NULL)
		::DestroyCursor(m_hCursor_Move);

	if (m_hCursor_NWSE != NULL)
		::DestroyCursor(m_hCursor_NWSE);

	if (m_hCursor_NESW != NULL)
		::DestroyCursor(m_hCursor_NESW);
}

void CAABonderDlg::changeCursor(CPoint p, CRect rcTemp)
{
	double dExpandFactorX;
	double dExpandFactorY;
	int	iGap;

	if (m_iCurCamNo<3)
	{
		dExpandFactorX = CAM_EXPAND_FACTOR_X;
		dExpandFactorY = CAM_EXPAND_FACTOR_Y;
		iGap = 20;
	}
	else
	{
		dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
		dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
		
		// 20140905 Overlay Box ��ġ �̵� �� Box ������ �󸶳� ������ �־�� ���콺 Ŀ�� ��� ���� �Ÿ� Ȯ�� ��..
		//iGap = 50;
		iGap = int(dExpandFactorX * 5);
	}

	CPoint point = p;

	int iRtn = -1;

	point.x -= m_rectCamDispPos1.left;
	point.y -= m_rectCamDispPos1.top;

	p.x = (int)(point.x * dExpandFactorX + 0.5);
	p.y = (int)(point.y * dExpandFactorY + 0.5);

	/* �ڽ� �̵� */
	if (p.x>rcTemp.left+iGap	&&
		p.x<rcTemp.right-iGap	&&
		p.y>rcTemp.top+iGap		&&
		p.y<rcTemp.bottom-iGap)
	{
		::SetCursor(m_hCursor_Move);
	}
	/* �� ũ�� */
	else if (p.y>rcTemp.top+iGap && p.y<rcTemp.bottom-iGap && p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap)
	{
		::SetCursor(m_hCursor_Width);
	}
	/* �� ũ�� */
	else if (p.y>rcTemp.top+iGap && p.y<rcTemp.bottom-iGap && p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap)
	{
		::SetCursor(m_hCursor_Width);
	}
	/* �� ũ�� */
	else if (p.x>rcTemp.left+iGap && p.x<rcTemp.right-iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		::SetCursor(m_hCursor_Height);
	}
	/* �� ũ�� */
	else if (p.x>rcTemp.left+iGap && p.x<rcTemp.right-iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+iGap)
	{
		::SetCursor(m_hCursor_Height);
	}
	/* �»� ũ�� */
	else if (p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		::SetCursor(m_hCursor_NWSE);
	}
	/* ��� ũ�� */
	else if (p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		::SetCursor(m_hCursor_NESW);
	}
	/* ���� ũ�� */
	else if (p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+iGap)
	{
		::SetCursor(m_hCursor_NESW);
	}
	/* ���� ũ�� */
	else if (p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+20)
	{
		::SetCursor(m_hCursor_NWSE);
	}
	else
	{
		::SetCursor(m_hCursor_Standard);
	}
}


int CAABonderDlg::checkMousePos(CPoint p, CRect rcTemp)
{
	double dExpandFactorX;
	double dExpandFactorY;
	int	iGap;

	if (m_iCurCamNo<3)
	{
		dExpandFactorX = CAM_EXPAND_FACTOR_X;
		dExpandFactorY = CAM_EXPAND_FACTOR_Y;
		iGap = 20;
	}
	else
	{
		dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
		dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
		// 20140905 Overlay Box ��ġ �̵� �� Box ������ �󸶳� ������ �־�� ���콺 Ŀ�� ��� ���� �Ÿ� Ȯ�� ��..
		//iGap = 50;
		iGap = int(dExpandFactorX * 5);
	}

	CPoint point = p;

	int iRtn = -1;

	point.x -= m_rectCamDispPos1.left;
	point.y -= m_rectCamDispPos1.top;

	p.x = (int)(point.x * dExpandFactorX + 0.5);
	p.y = (int)(point.y * dExpandFactorY + 0.5);

	/* �ڽ� �̵� */
	if (p.x>rcTemp.left+iGap	&&
		p.x<rcTemp.right-iGap	&&
		p.y>rcTemp.top+iGap		&&
		p.y<rcTemp.bottom-iGap)
	{
		iRtn = MOVE_ALL;
	}
	/* �� ũ�� */
	else if (p.y>rcTemp.top+iGap && p.y<rcTemp.bottom-iGap && p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap)
	{
		iRtn = MOVE_WIDTH_LEFT;
	}
	/* �� ũ�� */
	else if (p.y>rcTemp.top+iGap && p.y<rcTemp.bottom-iGap && p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap)
	{
		iRtn = MOVE_WIDTH_RIGHT;
	}
	/* �� ũ�� */
	else if (p.x>rcTemp.left+iGap && p.x<rcTemp.right-iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		iRtn = MOVE_HEIGHT_TOP;
	}
	/* �� ũ�� */
	else if (p.x>rcTemp.left+iGap && p.x<rcTemp.right-iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+iGap)
	{
		iRtn = MOVE_HEIGHT_BOTTOM;
	}
	/* �»� ũ�� */
	else if (p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		iRtn = MOVE_NW;
	}
	/* ��� ũ�� */
	else if (p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap && p.y>rcTemp.top-iGap && p.y<rcTemp.top+iGap)
	{
		iRtn = MOVE_NE;
	}
	/* ���� ũ�� */
	else if (p.x>rcTemp.left-iGap && p.x<rcTemp.left+iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+iGap)
	{
		iRtn = MOVE_SW;
	}
	/* ���� ũ�� */
	else if (p.x>rcTemp.right-iGap && p.x<rcTemp.right+iGap && p.y>rcTemp.bottom-iGap && p.y<rcTemp.bottom+iGap)
	{
		iRtn = MOVE_SE;
	}
	else
	{
	}

	return iRtn;
}

void CAABonderDlg::OnBnClickedButtonOrigin()
{
	CString sTemp="";

	if(Task.AutoFlag == 1) 
	{
		sLangChange.LoadStringA(IDS_STRING1395);	//"��ü ���� ���� ���� - �ڵ����� ��
		sTemp.Format(sLangChange);
		delayMsg(sTemp.GetBuffer(99), 3000, M_COLOR_DARK_GREEN);
		return;
	}

	if(g_bMovingflag)
	{
		sLangChange.LoadStringA(IDS_STRING1394);	//��ü ���� ���� ���� - ���� �̵� ��
		sTemp.Format(sLangChange);
		delayMsg(sTemp.GetBuffer(99), 3000, M_COLOR_DARK_GREEN);
		return;
	}


	bool home_process_run_flag = true;
	g_bMovingflag =true;
	sLangChange.LoadStringA(IDS_STRING1398);	//��ü ���� ���͸� ���� �Ͻðڽ��ϱ�?
	sTemp.Format("��ü ���� ���͸� ���� �Ͻðڽ��ϱ�?");	//��ü ���� ���͸� ���� �Ͻðڽ��ϱ�?
	if (askMsg(sTemp))
	{
		pThread_TaskOrigin = ::AfxBeginThread(Thread_TaskOrigin, this);
	}

	
}


BOOL CAABonderDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
		else if(pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
		else if (pMsg->wParam == VK_SPACE)
		{
		}
		else if(pMsg->wParam == VK_F2)
		{
		}
		else if(pMsg->wParam == VK_F3)
		{
		}
		else if(pMsg->wParam == VK_F4)
		{
		}
		else if(pMsg->wParam == VK_F5)
		{
		}
		else if(pMsg->wParam == VK_F6)
		{
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAABonderDlg::CcmThreadStart()
{
	pThread_CCM_Grab = NULL; 
	pThread_CCM_Display = NULL;

	pThread_CCM_CvtColor = NULL;
	pThread_CCM_CvtMil = NULL;
	
	bFlag_First_Grab_Display = false;

	pThread_CCM_Grab = ::AfxBeginThread(Thread_Ccm_Grab, this);
	pThread_CCM_CvtColor = ::AfxBeginThread(Thread_Ccm_CvtColor, this);
	pThread_CCM_CvtMil = ::AfxBeginThread(Thread_Ccm_CvtMil, this);
	pThread_CCM_Display = ::AfxBeginThread(Thread_Ccm_Display, this);

}


int CAABonderDlg::AlignLimitCheck(int Insptype, double dOffsetX, double dOffsetY, double dOffsetTh)										// [Insptype] 0:Lens PreAlign 1:PCB PreAlign
{																					// Return	0:NG,  1:Retry,  2:OK
	if(Insptype ==1)
	{
		if(fabs(dOffsetX)  > model.axis[Motor_Lens_X].m_dLimit_Err 
			|| fabs(dOffsetY)  > model.axis[Motor_Lens_Y].m_dLimit_Err)
		{
			putListLog("Lens�� X, Y ������ �̵� Limit�� �ʰ� �Ͽ����ϴ�.");
			return 0;
		}
	}
	else if(Insptype ==0)
	{
		if(fabs(dOffsetX)  > model.axis[Motor_PCB_X].m_dLimit_Err 
			|| fabs(dOffsetY)  > model.axis[Motor_PCB_Y].m_dLimit_Err 
			|| fabs(dOffsetTh)  > model.axis[Motor_PCB_TH].m_dLimit_Err)
			return 0;
	}


	if(Insptype ==1)
	{
		if(fabs(dOffsetX)  > model.axis[Motor_Lens_X].m_dLimit_OK 
			|| fabs(dOffsetY)  > model.axis[Motor_Lens_Y].m_dLimit_OK
			|| fabs(dOffsetTh)  > model.axis[Motor_PCB_TH].m_dLimit_OK)
			return 1;
	}
	else if(Insptype ==0)
	{
		if(fabs(dOffsetX)  > model.axis[Motor_PCB_X].m_dLimit_OK 
			|| fabs(dOffsetY)  > model.axis[Motor_PCB_Y].m_dLimit_OK
			|| fabs(dOffsetTh)  > model.axis[Motor_PCB_TH].m_dLimit_OK)
			return 1;
	}

	return 2;
}



int CAABonderDlg::procCamComAlign(int camNo, int iMarkType, bool liveMode, double &dOffsetX, double &dOffsetY, double &dOffsetTh)
{
	int iMarkNo;
	CString sTemp;
	dOffsetTh = 0.0f;
	
	vision.clearOverlay(camNo);
	double ep1;
	MappTimer(M_TIMER_RESET, &ep1);
#ifndef		NORINDA_MODE
	vision.getSnapImage(camNo);
	Sleep(200);
#endif
	double ep2;
	MappTimer(M_TIMER_READ, &ep2);
#ifdef		NORINDA_MODE
	if(vision.getLiveMode())
	{
		vision.getSnapImage(camNo);
		vision.setLiveMode(true);
	}
#else
	if(liveMode){
		vision.setLiveMode(true);
	}
#endif
	iMarkNo = vision.findComAlignMark(camNo, iMarkType);

	sTemp.Format(_T(" %.0f msec"), (ep2 - ep1)*1000);
	vision.textlist[camNo].addList((CAM_SIZE_X-140), (CAM_SIZE_Y-60), sTemp, M_COLOR_WHITE, 24, 10, _T("arialuni"));

	vision.drawOverlay(camNo);

	if ( iMarkNo!= -1 )
	{
		MmodControl(vision.ModResult[iMarkType][iMarkNo], M_DEFAULT, M_DRAW_SCALE_X, CAM_REDUCE_FACTOR_X);
		MmodControl(vision.ModResult[iMarkType][iMarkNo], M_DEFAULT, M_DRAW_SCALE_Y, CAM_REDUCE_FACTOR_Y);

		MgraColor(M_DEFAULT, M_COLOR_GREEN);
		MmodDraw(M_DEFAULT, vision.ModResult[iMarkType][iMarkNo], vision.MilOverlayImage[camNo], M_DRAW_BOX+M_DRAW_POSITION+M_DRAW_EDGES+M_DRAW_AXIS, M_DEFAULT, M_DEFAULT);

		if (calcAlignData(camNo, iMarkType, dOffsetX, dOffsetY, dOffsetTh) == false)
		{
			sTemp.Format("����ǰ Align �˻� NG");
			putListLog(sTemp);

			return -1;
		}
	}
	return iMarkNo;
}

int CAABonderDlg::procCamAlign(int camNo, int iMarkType, bool liveMode, double &dOffsetX, double &dOffsetY, double &dOffsetTh)
{
	int iMarkNo;
	CString sTemp;
	dOffsetX = dOffsetY = dOffsetTh = 0.0f;

	vision.clearOverlay(camNo);
	double ep1;
	MappTimer(M_TIMER_RESET, &ep1);

	vision.getSnapImage(camNo);
	Sleep(200);

	double ep2;
	MappTimer(M_TIMER_READ, &ep2);

	if(liveMode)
		vision.setLiveMode(true);
	
	iMarkNo = vision.findMark(camNo, iMarkType);
	
	sTemp.Format(_T(" %.0f msec"), (ep2 - ep1)*1000);
	vision.textlist[camNo].addList((CAM_SIZE_X-140), (CAM_SIZE_Y-60), sTemp, M_COLOR_WHITE, 24, 10, _T("arialuni"));

	vision.drawOverlay(camNo);

	if ( iMarkNo!= -1 )
	{
		MmodControl(vision.ModResult[iMarkType][iMarkNo], M_DEFAULT, M_DRAW_SCALE_X, CAM_REDUCE_FACTOR_X);
		MmodControl(vision.ModResult[iMarkType][iMarkNo], M_DEFAULT, M_DRAW_SCALE_Y, CAM_REDUCE_FACTOR_Y);

		MgraColor(M_DEFAULT, M_COLOR_GREEN);
		MmodDraw(M_DEFAULT, vision.ModResult[iMarkType][iMarkNo], vision.MilOverlayImage[camNo], M_DRAW_BOX+M_DRAW_POSITION+M_DRAW_EDGES+M_DRAW_AXIS, M_DEFAULT, M_DEFAULT);

		if (calcAlignData(camNo, iMarkType, dOffsetX, dOffsetY, dOffsetTh) == false)
		{
			if(camNo==1)
			{
				sLangChange.LoadStringA(IDS_STRING656);	//Lens Align �˻� NG.
				sTemp.Format(sLangChange);
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING906);	//PCB Align �˻� NG.
				sTemp.Format(sLangChange);
			}

			putListLog(sTemp.GetBuffer(99));

			return -1;
		}
	}

	if (iMarkNo==-1)
		return -1;

	return 0;
}

void CAABonderDlg::calcCamRotatePos(int iCamNo, double dFindX, double dFindY, double dBaseX, double dBaseY, double& dCx, double& dCy)
{
	double dCosVal,dSinVal;
	double dTheta;

	//--- ȸ������ Degree�� ����ǹǷ�...
	dTheta = (sysData.dCamAngle[iCamNo].x*M_PI) / 180.0;
	dCosVal = cos(dTheta);
	dSinVal = sin(dTheta);

	dCx = (dCosVal*(dFindX-dBaseX))-(dSinVal*(dBaseY-dFindY));
	dCx = dCx + dBaseX;

	//--- ȸ������ Degree�� ����ǹǷ�...
	dTheta = (sysData.dCamAngle[iCamNo].y*M_PI) / 180.0;
	dCosVal = cos(dTheta);
	dSinVal = sin(dTheta);

	dCy = (dSinVal*(dFindX-dBaseX))+(dCosVal*(dBaseY-dFindY));
	dCy = dCy*(-1) + dBaseY;
}


bool CAABonderDlg::calcAlignData(int camNo, int iMarkType, double &dOffsetX, double &dOffsetY, double &dOffsetTh)
{
	dOffsetX = dOffsetY = dOffsetTh = 0;

	double dRadianTh = g_dFindAngle[iMarkType] * M_PI / 180.0;

	double dTempCurX=0.0, dTempCurY=0.0;

	calcCamRotatePos(camNo, g_dFindCenterX[iMarkType], g_dFindCenterY[iMarkType], (CAM_SIZE_X>>1), (CAM_SIZE_Y>>1), dTempCurX, dTempCurY);

	CDPoint curPos;

	curPos.x		=	dTempCurX - (CAM_SIZE_X>>1);
	curPos.y		=	(CAM_SIZE_Y>>1) - dTempCurY;

	CDPoint rotatePos;
	double dCosVal=0.0, dSinVal=0.0;

	dCosVal = cos(dRadianTh);
	dSinVal = sin(dRadianTh);

	rotatePos.x = (curPos.x*dCosVal) - (curPos.y*dSinVal);
	rotatePos.y = (curPos.x*dSinVal) + (curPos.y*dCosVal);

	if (camNo == 1)
	{
		dOffsetX = rotatePos.x;
		dOffsetY = rotatePos.y;
		dOffsetTh = g_dFindAngle[iMarkType];
	}
	else
	{
		dOffsetX = rotatePos.x;
		dOffsetY = rotatePos.y;
		dOffsetTh = g_dFindAngle[iMarkType];
	}

	dOffsetX *= sysData.dCamResol[camNo].x * -1;
	dOffsetY *= sysData.dCamResol[camNo].y * -1;

	CString sTemp="";
	sLangChange.LoadStringA(IDS_STRING1482);	//������ : %.3lf %.3lf %.3lf
	sTemp.Format(sLangChange, dOffsetX, dOffsetY, dOffsetTh);
	vision.textlist[camNo].addList(20, (CAM_SIZE_Y-50), sTemp, M_COLOR_CYAN, 24, 10, "Arial Black");

	return true;
}

/////////////////////// ���� �۾� �� �Դϴ�~~
int CAABonderDlg::checkAutoRunLensAlarm(int fi_step)					// 0:����, 1:���� ���� ���� ���� �� �� �ֵ���, 2:������ ����
{
	////////////////////////////////////////////////////////////////////////////////
	// ���� AmpFault

	motor.AmpFaultCheck();
	if(Task.iAmpFaultFlag)
	{
		m_labelServo.SetBkColor(M_COLOR_RED);
		m_labelServo.Invalidate();
		sLangChange.LoadStringA(IDS_STRING1327);	//���� �� AMP Fault�� ���� �Ǿ����ϴ�.
		_stprintf_s(sz_PCB_Error_Msg, sLangChange);
		return 2;
	}


	////////////////////////////////////////////////////////////////////////////////
	// ���� ���� ���� Ȯ��..

	motor.HomeCheck();

	if(Task.iHomeErrorFlag)
	{
		m_labelHom.SetBkColor(M_COLOR_RED);
		m_labelHom.Invalidate();
		sLangChange.LoadStringA(IDS_STRING1331);	//���� ���� ������ ���� �Ǿ����ϴ�.
		_stprintf_s(sz_PCB_Error_Msg, sLangChange);
		return 2;
	}
	
	//motor.InDIO(0, curInDio[0]);
	return 0;
}


int CAABonderDlg::checkAutoRunPcbAlarm(int fi_step)					// 0:����, 1:���� ���� ���� ���� �� �� �ֵ���, 2:������ ����
{
	////////////////////////////////////////////////////////////////////////////////
	// ���� AmpFault

	bool testFlag = true;

	motor.AmpFaultCheck();
	if(Task.iAmpFaultFlag)
	{
		m_labelServo.SetBkColor(M_COLOR_RED);
		m_labelServo.Invalidate();
		sLangChange.LoadStringA(IDS_STRING1327);	//���� �� AMP Fault�� ���� �Ǿ����ϴ�.
		_stprintf_s(sz_PCB_Error_Msg, sLangChange);
		return 2;
	}


	////////////////////////////////////////////////////////////////////////////////
	// ���� ���� ���� Ȯ��..

	motor.HomeCheck();

	if(Task.iHomeErrorFlag)
	{
		m_labelHom.SetBkColor(M_COLOR_RED);
		m_labelHom.Invalidate();

		sLangChange.LoadStringA(IDS_STRING1331);	//���� ���� ������ ���� �Ǿ����ϴ�.
		_stprintf_s(sz_PCB_Error_Msg, sLangChange);
		return 2;
	}

	//motor.InDIO(0, curInDio[0]);

	return 0;
}

int CAABonderDlg::_checkPcbMotor()
{
	double dMotorPosX  = motor.GetEncoderPos(Motor_PCB_X);
	double dMotorPosY  = motor.GetEncoderPos(Motor_PCB_Y);
	double dMotorPosTH = motor.GetEncoderPos(Motor_PCB_TH);

	double dMotorSavePosX  = model.axis[Motor_PCB_X].pos[Bonding_Pos];
	double dMotorSavePosY  = model.axis[Motor_PCB_Y].pos[Bonding_Pos];
	double dMotorSavePosTH = model.axis[Motor_PCB_TH].pos[Bonding_Pos];

	if ( fabs(dMotorPosX-dMotorSavePosX) < model.axis[Motor_PCB_X].m_dLimit_Err		&&
		 fabs(dMotorPosY-dMotorSavePosY) < model.axis[Motor_PCB_Y].m_dLimit_Err		&&
		 fabs(dMotorPosTH-dMotorSavePosTH) < model.axis[Motor_PCB_TH].m_dLimit_Err )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}


int CAABonderDlg::_moveZMotor(double dDist, double dVel)
{
	double dCurPos = 0.0;
	double dMovePos = 0.0;
#if (____AA_WAY == PCB_TILT_AA)
	dMovePos = motor.GetCommandPos(TITLE_MOTOR_Z) + dDist;
#elif (____AA_WAY == LENS_TILT_AA)
	dMovePos = motor.GetCommandPos(TITLE_MOTOR_Z) - dDist;
#endif
	bool b_move_ok_flag = motor.MoveAxis(TITLE_MOTOR_Z, ABS, dMovePos, dVel, sysData.fMotorAccTime[TITLE_MOTOR_Z]);


	if(!b_move_ok_flag){
		return -1;
	}
	double ep = myTimer(true);

	while (1)
	{

		if ( myTimer(true)-ep< 5000)
		{
			if ( motor.IsStopAxis(TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z) )
			{
				dCurPos = motor.GetCommandPos(TITLE_MOTOR_Z);

				if (fabs(dCurPos-dMovePos)<=0.003)
				{
					break;
				}
			}
		}
		else
		{
			putListLog("Auto Focusing �� Z Motor �̵� �ð� �ʰ�.");
			return -1;
		}
	}

	return 1;
}


int CAABonderDlg::_getMTF(int Mode, bool LogView)
{
	const int mLocalMtfCnt = model.mGlobalSmallChartCount;
	bool bRtn = false;
	int iCnt_Focus = 0; 


	int i = 0;
	int j = 0;
	TCHAR szLog[SIZE_OF_1K];

	dSFR_N_8_PositionX = model.m_Line_Pulse;
	dSFR_N_4_PositionX = model.m_Line_Pulse;
	//
	double fMax_SFR_N4[COMMON_MTF_INSP_CNT] = {0.0, };
	
	bool bSfrMaxTest = true;
	
	//gMIUDevice.CurrentState = 3;	//��������
	vision.clearOverlay(CCD);
	vision.MilBufferUpdate();	//ù�忡�� ������ Image�� ����� ���� �߻����� �ʱ� �ѹ� ���� ������


	Task.getROI();				// ���� ��ũ ��ġ �ν�..
	for(int i=0; i<COMMON_MTF_INSP_CNT; i++)
	{ 
		fMax_SFR_N4[i] = 0.0;
		Task.SFR.dMaxSfr_StepLog[i] = 0.0;
	}
	int nWidth = gMIUDevice.imageItp->width;
	int nHeight = gMIUDevice.imageItp->height;
	int iSizeX = model.m_iSize_ROI_X + 1;
	int iSizeY = model.m_iSize_ROI_Y + 1;
	vYImgBuffer.resize(nHeight * nWidth, 0);
	vYImgBuffer.clear();
	int dic = 0;
	CString strView = "";
	bool sfrRt = false;

	//double dFrequency[1] = {dSFR_N_8_PositionX}; // Spatial frequency for SFR calculation  
	//double dSfrFinalResult[COMMON_MTF_INSP_CNT][1] = {{0.0},}; // SFR result

	if (Task.bFirstAA == true)
	{
		iCnt_Focus = model.strInfo_CCM.m_iCnt_Average_Fine_Focus;		//2��, ����ǰ
	}
	else
	{
		iCnt_Focus = model.strInfo_CCM.m_iCnt_Average_Thru_Focus;		//1��
	}
	//if(Mode==SFR_FINAL)
	//{
	//	iCnt_Focus=model.strInfo_CCM.m_iCnt_Average_Fine_Focus;		//2��, ����ǰ
	//}
	//else
	//{
	//	iCnt_Focus=model.strInfo_CCM.m_iCnt_Average_Thru_Focus;		//1��
	//}
	//==============================================================================================================================================
	//==============================================================================================================================================
	//new
	double _offset = 0.0;
	int nBlackLevel = 0;
	std::shared_ptr<CACMISResolutionSFR> m_pSFRProc = std::make_shared<CACMISResolutionSFR>();
	POINT ptROICenter[COMMON_MTF_INSP_CNT];
	TSFRSpec m_stSFRSpec;





	//iCnt_Focus = model.strInfo_CCM.m_iCnt_Average_Thru_Focus;

	if (iCnt_Focus < 1)
	{
		iCnt_Focus = 1;
	}
	if(iCnt_Focus > 30)
	{
		iCnt_Focus = 30;
	}
	for (j = 0; j < iCnt_Focus; j++)		//for (j = 0; j < 1; j++)//
    {
        if (j == 0)  
		{
            bRtn = Task.getROI_SFR(Mode);				// �簢�� Box ��ġ �ν�..
			if (bRtn == false)
			{
				_stprintf_s(szLog, SIZE_OF_1K, _T("[SFR ����] ���� ��ũ �ν� ����"));
				errMsg2(Task.AutoFlag, szLog);
				return -1;
			}
			for (i = 0; i < model.mGlobalSmallChartCount; i++)	//for (int i = 0; i < MTF_INSP_CNT; i++)
			{
				ptROICenter[i].x = Task.SFR._64_Sfr_Rect[i].left + (iSizeX / 2);
				ptROICenter[i].y = Task.SFR._64_Sfr_Rect[i].top + (iSizeY / 2);
				//
				Task.m_vDirection[i] = model.m_MTF_Direction[i]; // 0: Vertical, 1: Horizontal
				Task.m_vFrquency[i] = model.m_Line_Pulse;//0.225
				Task.m_vOffset[i] = 0.0;
				Task.m_vSFR[i] = 0.0;
				Task.m_vThreshold[i] = 0.0;
				Task.m_vROI[i].ptCenter.x = ptROICenter[i].x;
				Task.m_vROI[i].ptCenter.y = ptROICenter[i].y;

				if (model.m_MTF_Direction[i] == SFR_ROI_VERTICAL)//SFR_ROI_VERTICAL)
				{
					Task.m_vROI[i].nROIWidth = iSizeX;
					Task.m_vROI[i].nROIHeight = iSizeY;
				}
				else
				{
					Task.m_vROI[i].nROIWidth = iSizeY;
					Task.m_vROI[i].nROIHeight = iSizeX;
				}
				//Task.m_vROI[i].nROIWidth = model.m_MTF_Direction[i] == 1 ? iSizeX : iSizeY;
				//Task.m_vROI[i].nROIHeight = model.m_MTF_Direction[i] == 0 ? iSizeY : iSizeX;//SFR_ROI_HORIZONTAL ? iSizeY : iSizeX;
			}
			//
			if (MandoSfrSpec.INSP_SfrDeltaAlgorithmType == 0)
			{
				m_stSFRSpec.eSFRDeltaAlgorithmType = ESFRDelta_Diff;
			}
			else if (MandoSfrSpec.INSP_SfrDeltaAlgorithmType == 1)
			{
				m_stSFRSpec.eSFRDeltaAlgorithmType = ESFRDelta_Ratio;
			}
			//
			if (MandoSfrSpec.INSP_SfrAlgorithmType == 0)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_ISO12233;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 1)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_RHOMBUS;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 2)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_LGIT_ISO;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 3)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_VNE;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 4)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_Mobis;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 5)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_Tesla_Trinity;
			}
			//
			if (MandoSfrSpec.INSP_SfrAlgorithmMethod == 0)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmMethod = ESFRMethod_Freq2SFR;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmMethod == 1)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmMethod = ESFRMethod_SFR2Freq;
			}
			//
			if (MandoSfrSpec.INSP_SfrFrequencyUnit == 0)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_CyclePerPixel;			
			}
			else if (MandoSfrSpec.INSP_SfrFrequencyUnit == 1)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_LinePairPerMilliMeter;			
			}
			else if (MandoSfrSpec.INSP_SfrFrequencyUnit == 2)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_LineWidthPerPictureHeight;
			}
			m_stSFRSpec.tSFRConfig.nMaxROIWidth = iSizeX;
			m_stSFRSpec.tSFRConfig.nMaxROIHeight = iSizeY;
			m_stSFRSpec.tSFRConfig.dMaxEdgeAngle = MandoSfrSpec.INSP_SfrMaxEdgeAngle; //45.0;
			m_stSFRSpec.tSFRConfig.dPixelSize = model.m_dSize_CCD_Cell;// 4.2;
			//ESFRFreq_CyclePerPixel;			//�Ҽ���
			//ESFRFreq_LinePairPerMilliMeter;	//�����ڸ�
			m_stSFRSpec.dEdgeDir = Task.m_vDirection.data();
			m_stSFRSpec.dFrequency = Task.m_vFrquency.data();
			m_stSFRSpec.dSFR = Task.m_vSFR.data();
			m_stSFRSpec.dThreshold = Task.m_vThreshold.data();
			m_stSFRSpec.dGamma = MandoSfrSpec.INSP_SfrGamma;
			m_stSFRSpec.tROI.dOffset = Task.m_vOffset.data();
			m_stSFRSpec.tROI.eROIType = ROIType_POINT;
			m_stSFRSpec.tROI.pROIData = Task.m_vROI.data();
			m_stSFRSpec.tROI.ROICount = model.mGlobalSmallChartCount;// MTF_INSP_CNT;
			m_stSFRSpec.tDelataSpec = NULL;

        }// 1�� ����	



		bool bRet = true;
		bRet = m_pSFRProc->Inspect(MIU.m_pFrameRawBuffer, nWidth, nHeight, m_stSFRSpec,
			gMIUDevice.dTDATASPEC_n.eDataFormat, gMIUDevice.dTDATASPEC_n.eOutMode,
			gMIUDevice.dTDATASPEC_n.eSensorType,
			nBlackLevel, false, false, gMIUDevice.dTDATASPEC_n.eDemosaicMethod);
		

        int sfrMax = m_pSFRProc->GetMaxResolutionCount();
        float sfrValue = 0.0;
        for (i = 0; i < sfrMax; i++)
        {
            const TSFRROIResult* pROIResult = m_pSFRProc->GetSFRROIResult(i);
			sfrValue = pROIResult->dFinalResult[0];
            if (pROIResult)
            {
                if (!_finite(sfrValue) || sfrValue > 0.99 || sfrValue < 0.0)
                {
                    Task.SFR._64_fSfrN4[i] = 0.0;
                }
                else
                {
					Task.SFR._64_fSfrN4[i] = sfrValue;
                }
                //
				if (Task.SFR._64_fSfrN4[i] < 0.0)
				{
					Task.SFR._64_fSfrN4[i] = 0.0;
				}
//#ifdef SFR_COMP_MAX
				if (sysData.m_iSfrMaxValueInsp == 1)
				{
					if (fMax_SFR_N4[i] < Task.SFR._64_fSfrN4[i])
					{
						fMax_SFR_N4[i] = Task.SFR._64_fSfrN4[i];
					}
					else
					{
						fMax_SFR_N4[i] = fMax_SFR_N4[i];
					}

					strView.Format("SFR [%d]  =  %lf", i, fMax_SFR_N4[i]);
					putListLog(strView);

					Task.SFR.dMaxSfr_StepLog[i] = fMax_SFR_N4[i];
				}
				else
				{
					fMax_SFR_N4[i] += Task.SFR._64_fSfrN4[i];
				}
//#else
                //fMax_SFR_N4[i] += Task.SFR._64_fSfrN4[i]; 

//#endif
				//Task.SFR.dMaxSfr_StepLog[i] = fMax_SFR_N4[i];
            }
        }
    }

	// SFR����� ������� ���� �ִ�ġ�� ���� 
	// ����� SONY1
//#ifndef SFR_COMP_MAX
	
	if (Mode == SFR_STEP)
	{
		for (int i = 0; i < model.mGlobalSmallChartCount; i++)
		{
			//1,2�� Max �� ����
			if (Task.bFirstAA == true)
			{
				//2�� Max ��
				Task.SFR.dMaxSfr_Second_StepLog[i] = fMax_SFR_N4[i];
			}
			else
			{
				//1�� Max ��
				Task.SFR.dMaxSfr_Frist_StepLog[i] = fMax_SFR_N4[i];
			}
			_stprintf_s(szLog, SIZE_OF_1K, _T("SFR Max StepLog [%d]  =  %lf"), i, fMax_SFR_N4[i]);
			putListLog(szLog);
		}
	}
	
	for (int i = 0; i < model.mGlobalSmallChartCount; i++)
	{
		if (sysData.m_iSfrMaxValueInsp == 0)
		{
			fMax_SFR_N4[i] = fMax_SFR_N4[i] / iCnt_Focus;
		}
		
		Task.SFR.dMaxSfr_StepLog[i] = fMax_SFR_N4[i];
		_stprintf_s(szLog, SIZE_OF_1K, _T("SFR Avr/Max [%d]  =  %lf"), i, fMax_SFR_N4[i]);
		putListLog(szLog);
		
	}

	
//#endif

	//Plus �� 
	//[0]CENTER
	//[1]TOP , [2]BOTTOM , [3]LEFT , [5]RIGHT
	//
	//��������
	//[0]CENTER
	//[1]LT  , [2]RT	 , [3]BL   , [4]BR

	//model.sfrElem.m_SfrTestUse[index][i]
	//model.sfrElem.m_AATiltCalRoi[index][i]
	int nPartternRoiIndex = 0;
	int nSmallRoiIndex = 0;
	double mAvrSfrData = 0.0;
	double mAvrSfrPosX = 0.0;
	double mAvrSfrPosY = 0.0;

	double mTempPosX = 0.0;
	double mTempPosY = 0.0;
	int mIndex = 0;
	int mAvrIndex = 0;
	//new ���
	for (i = 0; i < 5; i++)	//AA ���� ����
	{
		nPartternRoiIndex = model.sfrElem.m_AATiltRoiIndex[i];
		mAvrSfrData = 0.0;
		mTempPosX = 0.0;
		mTempPosY = 0.0;
		mAvrIndex = 0;
		for (j = 0; j < 4; j++)		//���� ������ T,B,L,R ����
		{
			if (model.sfrElem.m_AATiltCalRoi[nPartternRoiIndex][j] == 1)	//aa��� �������� üũ��������
			{
				mIndex = Task.SFR.nTiltCalIndex[nSmallRoiIndex];
				mAvrSfrData += fMax_SFR_N4[mIndex];
				mTempPosX += (Task.SFR._64_Sfr_Rect[mIndex].left + Task.SFR._64_Sfr_Rect[mIndex].right) / 2.0;
				mTempPosY += (Task.SFR._64_Sfr_Rect[mIndex].top + Task.SFR._64_Sfr_Rect[mIndex].bottom) / 2.0;
				nSmallRoiIndex++;
				mAvrIndex++;
			}
		}
		mAvrSfrPosX = mTempPosX / mAvrIndex;
		mAvrSfrPosY = mTempPosY / mAvrIndex;
		Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][i] = mAvrSfrData / mAvrIndex; 
		Task.SFR.Sfr_pos[Task.m_iCnt_Step_AA_Total][i].x = mAvrSfrPosX;
		Task.SFR.Sfr_pos[Task.m_iCnt_Step_AA_Total][i].y = mAvrSfrPosY;
		/*if (Task.bSfrLogView)
		{
			_stprintf_s(szLog, SIZE_OF_1K, _T("[%d]Tilt Cal Average Sfr = %f[x:%.3lf/y:%.3lf]"), i, Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][i], Task.SFR.Sfr_pos[Task.m_iCnt_Step_AA_Total][i].x, Task.SFR.Sfr_pos[Task.m_iCnt_Step_AA_Total][i].y);
			putListLog(szLog);
		}*/
	}


#if (____AA_WAY == PCB_TILT_AA)
	for (int i=Motor_PCB_X ; i<= Motor_PCB_Xt; i++)
	{

		if (i == Motor_PCB_Xt || i == Motor_PCB_Yt)
		{
			Task.SFR.fMotorPos[Task.m_iCnt_Step_AA_Total][i] = (float)motor.GetCommandPos(i);
		}else
		{
			Task.SFR.fMotorPos[Task.m_iCnt_Step_AA_Total][i] = (float)motor.GetEncoderPos(i);
		}
	}
#elif (____AA_WAY == LENS_TILT_AA)
	for (int i=Motor_Lens_X ; i<= Motor_Lens_Z; i++)
	{
		if (i == Motor_Lens_Xt || i == Motor_Lens_Yt)
		{
			Task.SFR.fMotorPos[Task.m_iCnt_Step_AA_Total][i] = (float)motor.GetCommandPos(i);
		}else
		{
			Task.SFR.fMotorPos[Task.m_iCnt_Step_AA_Total][i] = (float)motor.GetEncoderPos(i);
		}
	}
#endif
	if (Mode == SFR_FINAL)
	{
		for (int i = 0; i < model.mGlobalSmallChartCount; i++)	//for (int i=0 ; i<MTF_INSP_CNT; i++)
		{
			Task.SFR._64_fSfrN4[i] = fMax_SFR_N4[i];
		}
	}
	if (Mode == SFR_MANUAL)
	{
		if (_calcImageAlignment())
		{
			_stprintf_s(szLog, SIZE_OF_1K, _T("	OC_X: %.3lf, OC_Y: %.3lf"), MandoInspLog.dOCResult[0], MandoInspLog.dOCResult[1]);
			putListLog(szLog);
			//
			_stprintf_s(szLog, SIZE_OF_1K, _T("	OC ���� = X: %.3lf, Y: %.3lf   Spec(%.03lf, %.03lf)"), Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y);
			putListLog(szLog);
			_stprintf_s(szLog, SIZE_OF_1K, _T("	Rotation = %.3lf"), Task.m_dShift_IMG_TH);
			putListLog(szLog);
		}
	}

	if(!saveSfrLog(Mode))
	{
		return -1;
	} 

	if (Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][0] > model.strInfo_AF1.m_fLimit_MTF || //0.25
		Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][1] > model.strInfo_AF1.m_fLimit_MTF || 
		Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][2] > model.strInfo_AF1.m_fLimit_MTF || 
		Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][3] > model.strInfo_AF1.m_fLimit_MTF || 
		Task.SFR.fSfrN4[Task.m_iCnt_Step_AA_Total][4] > model.strInfo_AF1.m_fLimit_MTF )	/* ���� MTF ���� ����Ʈ �̻��̸� fine pitch �̵� */
	{
		Task.m_bFlag_Fine_Move = true;
	}
	Task.m_iCnt_Step_AA_Total++;


	
	if ( Mode != SFR_FINAL )
	{
		autodispDlg->DrawGraph(2);		/* ���� ��ġ SFR ������ �׷��� �׸��� */
	}
	vision.drawOverlay(CCD);
	return 1;
}

int CAABonderDlg::_checkMaxSfrPos(int iMode) 
{
	double dMaxSFR[5] = {-1.0, -1.0, -1.0, -1.0, -1.0};

	int i_data_cnt = 0;
	int i_max_index = 0;

	double d_data_x[5];
	double d_data_y[5];

	double d_calc_A = 0.0;
	double d_calc_B = 0.0;
	double d_calc_C = 0.0;
	double sfrLimit = sysData.dMax_Sfr_Limit;
	if (sfrLimit < 0.01)
	{
		sfrLimit = 0.1;
	}
	if ( Task.m_iCnt_Step_AA < 4 ){
		return 0;
	}
	int i = 0;
	int iStartIndex = 0;

	if ( Task.bFirstAA ){
		iStartIndex = Task.m_iCnt_Second_AA_Start;
	}else if ( Task.m_bFlag_Decrease_SFR == true ){ 
		iStartIndex = 3;
	}

	for (i=0 ; i<5 ; i++)
	{
		for (int j=iStartIndex ; j<Task.m_iCnt_Step_AA_Total ; j++)
		{
			if ( Task.SFR.fSfrN4[j][i] > dMaxSFR[i] )
			{
				dMaxSFR[i] = Task.SFR.fSfrN4[j][i];
				Task.SFR.iMaxIndex[i] = j;
			}
		}

		if ( dMaxSFR[i] < sfrLimit){
			return -1;
		}

		if ( Task.SFR.iMaxIndex[i] > (Task.m_iCnt_Step_AA_Total-3) )
			return -1;


		i_max_index = Task.SFR.iMaxIndex[i];

		if(i_max_index==0)
		{
			i_data_cnt = 1;
			d_data_x[0] = Task.SFR.fMotorPos[0][TITLE_MOTOR_Z];
			d_data_y[0] = Task.SFR.fSfrN4[0][i];
		}
		else if(i_max_index==1)
		{
			if(Task.m_iCnt_Step_AA_Total<4)
			{
				return -1;
			}

			i_data_cnt = 4;
			d_data_x[0] = Task.SFR.fMotorPos[0][TITLE_MOTOR_Z];
			d_data_x[1] = Task.SFR.fMotorPos[1][TITLE_MOTOR_Z];
			d_data_x[2] = Task.SFR.fMotorPos[2][TITLE_MOTOR_Z];
			d_data_x[3] = Task.SFR.fMotorPos[3][TITLE_MOTOR_Z];

			d_data_y[0] = Task.SFR.fSfrN4[0][i];
			d_data_y[1] = Task.SFR.fSfrN4[1][i];
			d_data_y[2] = Task.SFR.fSfrN4[2][i];
			d_data_y[3] = Task.SFR.fSfrN4[3][i];
		}
		else
		{
			i_data_cnt = 3;
			d_data_x[0] = Task.SFR.fMotorPos[i_max_index-1][TITLE_MOTOR_Z];
			d_data_x[1] = Task.SFR.fMotorPos[i_max_index-0][TITLE_MOTOR_Z];
			d_data_x[2] = Task.SFR.fMotorPos[i_max_index+1][TITLE_MOTOR_Z];

			d_data_y[0] = Task.SFR.fSfrN4[i_max_index-1][i];
			d_data_y[1] = Task.SFR.fSfrN4[i_max_index-0][i];
			d_data_y[2] = Task.SFR.fSfrN4[i_max_index+1][i];
		}


		if(i_max_index==0)
		{
			Task.SFR.dMaxPos[i] = Task.SFR.fMotorPos[0][TITLE_MOTOR_Z];
		}
		else if(i_max_index==(Task.m_iCnt_Step_AA_Total-1) )
		{
			Task.SFR.dMaxPos[i] = Task.SFR.fMotorPos[Task.m_iCnt_Step_AA_Total-1][TITLE_MOTOR_Z];
		}
		else
		{
			if(!Alg._calcDoublePeakPos(i_data_cnt, d_data_x, d_data_y, d_calc_A, d_calc_B, d_calc_C))
			{
				putListLog("[Active Align] Peak ���� ��ġ ��� ����");
				Task.SFR.dMaxPos[i] = Task.SFR.fMotorPos[Task.SFR.iMaxIndex[i] ][TITLE_MOTOR_Z];
			}
			else
			{
				double d_max_Z_pos = -d_calc_B / (2*d_calc_A);

				if( (Task.SFR.fMotorPos[i_max_index+1][TITLE_MOTOR_Z] < d_max_Z_pos) && (d_max_Z_pos<Task.SFR.fMotorPos[i_max_index-1][TITLE_MOTOR_Z]) )
					Task.SFR.dMaxPos[i] = d_max_Z_pos;
				else
				{
					Task.SFR.dMaxPos[i] = Task.SFR.fMotorPos[i_max_index][TITLE_MOTOR_Z]; 
					//LogSave("ERROR ==========> ��Ŀ�� ���� �ּ� �ڽ¹� ��� ��� �̻�.");
				}
			}
		}
	}
	

	return 1;
}

//--Laser ���������� Tilting���� ��� - 
//-- ���� ����� ������ 4��и� �������� 2��и�->1��и�->4��и�->3��и����� ���
bool CAABonderDlg::_calcLaserTilt(CDPoint dMotorPos[4], double dLaser[4], double &TX, double &TY)
{
	CString strLog = "";
	double offsetTx = 1.0;
	double offsetTy = 1.0;
	double tempLaser = 0.0;
	double Length[4];
	// ======================= ���
	//���Ͱ� �Ÿ�(�غ�)
	Length[0] = abs(dMotorPos[0].y - dMotorPos[3].y);  // left  
	Length[1] = abs(dMotorPos[0].x - dMotorPos[1].x);  // top
	Length[2] = abs(dMotorPos[1].y - dMotorPos[2].y);  // Right 
	Length[3] = abs(dMotorPos[2].x - dMotorPos[3].x);  // bottom
	
	// ======================= X,Y ���( �߽� ������)
	double AvgTop, AvgBottom;
	double AvgLeft, AvgRight;
	AvgTop		 = (dLaser[0] + dLaser[1]) / 2;
	AvgBottom	 = (dLaser[2] + dLaser[3]) / 2;
	AvgLeft		 = (dLaser[0] + dLaser[3]) / 2;
	AvgRight	 = (dLaser[1] + dLaser[2]) / 2;

	double Height, Width;
	double radian, theta;

	//-------------------------------------------------------
	Width = Length[1];                   // top
	Height = AvgLeft - AvgRight;
	//Height = AvgRight - AvgLeft;   //+ - ��ȣ �ȸ����� ���� �ٲٱ�
	radian = atan(Height / Width);
	theta = radian * 180 / M_PI;
	//-------------------------------------------------------
	tempLaser = (-theta + Task.oldLaserTy);
	double perValue = 100 * (tempLaser / Task.oldLaserTy);
	if (fabs(perValue) > 100 || Task.oldLaserTy == 0)
	{
		offsetTy = 1.0;//����
	}
	else if (fabs(perValue) > 80)
	{
		offsetTy = 0.8;
	}
	else if (fabs(perValue) > 70)
	{
		offsetTy = 0.7;
	}
	else
	{
		offsetTy = 0.5;
	}
	offsetTy = 0.9;
	TY = -theta *offsetTy;//-theta *offsetTy;
	strLog.Format("[ty] %lf/ %lf/ %lf/ %lf/ %lf ", -theta, tempLaser,Task.oldLaserTy, offsetTy, perValue);
	putListLog(strLog);
	Task.oldLaserTy = TY;
	// ======================= ���� ���� ��� (TX)
	Width = Length[0];                   // left

    Height = AvgTop - AvgBottom;
	//Height = AvgBottom - AvgTop;		//+ - ��ȣ �ȸ����� ���� �ٲٱ�
	
	radian = atan(Height / Width);
	theta = radian * 180 / M_PI;
	//-------------------------------------------------------
	tempLaser = (-theta + Task.oldLaserTx);
	perValue = 100 * (tempLaser / Task.oldLaserTx);
	if (fabs(perValue) > 100 || Task.oldLaserTx == 0)
	{
		offsetTx = 1.0;//����
	}
	else if (fabs(perValue) > 80)
	{
		offsetTx = 0.8;
	}
	else if (fabs(perValue) > 70)
	{
		offsetTx = 0.7;
	}
	else
	{
		offsetTx = 0.5;
	}
	offsetTx = 0.9;
	TX = -theta *offsetTx;//-theta *offsetTx;
	strLog.Format("[tx] %lf/ %lf/ %lf/ %lf/ %lf ", -theta, tempLaser,Task.oldLaserTx, offsetTx, perValue);
	putListLog(strLog);
	Task.oldLaserTx = -theta;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////


double CAABonderDlg::_calcTiltX()
{
	double dTopZPos_A = 0.0;
	double dBottomZPos_A = 0.0;

	double dHeight_A = 0.0;
	double dTopPixelPos_A = 0.0;
	double dBottomPixelPos_A = 0.0;

	double dWidth_A = 0.0;

	double dTiltRad_A = 0.0;
	double dTiltDeg_A = 0.0;
	if (model.mGlobalChartType == 0)
	{
		//x����
		dTopZPos_A = (Task.SFR.dMaxPos[1] + Task.SFR.dMaxPos[2]) / 2;
		dBottomZPos_A = (Task.SFR.dMaxPos[3] + Task.SFR.dMaxPos[4]) / 2;

		dHeight_A = dBottomZPos_A - dTopZPos_A;
		dTopPixelPos_A = (Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[1]][1].y + Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[2]][2].y) / 2;
		dBottomPixelPos_A = (Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[3]][3].y + Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[4]][4].y) / 2;

		dWidth_A = (dBottomPixelPos_A - dTopPixelPos_A) * model.m_dSize_CCD_Cell / 1000;	// CCD Cell ���ش�

		dTiltRad_A = atan(dHeight_A / dWidth_A);
		dTiltDeg_A = dTiltRad_A * 180.0 / M_PI;
	}
	else
	{
		//+����
		//  + ����Ʈ�� ƿƮ�� ���
		dTopZPos_A = Task.SFR.dMaxPos[1];
		dBottomZPos_A = Task.SFR.dMaxPos[2];

		dHeight_A = dBottomZPos_A - dTopZPos_A;
		dTopPixelPos_A = Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[1]][1].y;
		dBottomPixelPos_A = Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[2]][2].y;

		dWidth_A = (dBottomPixelPos_A - dTopPixelPos_A) * model.m_dSize_CCD_Cell / 1000;	//* CCD Cell ���ش�

		dTiltRad_A = atan(dHeight_A / dWidth_A);
		dTiltDeg_A = dTiltRad_A * 180.0 / M_PI;
	}
	return dTiltDeg_A;
}


double CAABonderDlg::_calcTiltY()
{
	double dLeftZPos_A = 0.0;
	double dRightZPos_A = 0.0;

	double dHeight_A = 0.0;

	double dLeftPixelPos_A = 0.0;
	double dRightPixelPos_A = 0.0;

	double dWidth_A = 0.0;

	double dTiltRad_A = 0.0;
	double dTiltDeg_A = 0.0;
	if (model.mGlobalChartType == 0)
	{
		//x����
		dLeftZPos_A = (Task.SFR.dMaxPos[1] + Task.SFR.dMaxPos[3]) / 2;
		dRightZPos_A = (Task.SFR.dMaxPos[2] + Task.SFR.dMaxPos[4]) / 2;

		dHeight_A = dRightZPos_A - dLeftZPos_A;

		dLeftPixelPos_A = (Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[1]][1].x + Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[3]][3].x) / 2;
		dRightPixelPos_A = (Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[2]][2].x + Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[4]][4].x) / 2;

		dWidth_A = (dRightPixelPos_A - dLeftPixelPos_A) * model.m_dSize_CCD_Cell / 1000;		// CCD Cell ���ش�

		dTiltRad_A = atan(dHeight_A / dWidth_A);
		dTiltDeg_A = dTiltRad_A * 180.0 / M_PI;
	}
	else
	{
		dLeftZPos_A = Task.SFR.dMaxPos[3];
		dRightZPos_A = Task.SFR.dMaxPos[4];
		dHeight_A = dRightZPos_A - dLeftZPos_A;

		dLeftPixelPos_A = Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[3]][3].x;
		dRightPixelPos_A = Task.SFR.Sfr_pos[Task.SFR.iMaxIndex[4]][4].x;

		dWidth_A = (dRightPixelPos_A - dLeftPixelPos_A) * model.m_dSize_CCD_Cell / 1000;		//* CCD Cell ���ش�

		dTiltRad_A = atan(dHeight_A / dWidth_A);
		dTiltDeg_A = dTiltRad_A * 180.0 / M_PI;
	}

	return dTiltDeg_A;
}
int CAABonderDlg::_procTiltY()
{
	if (Task.SFR.dTilt_Y == 0.0f) {
		return 1;
	}
  	CString logStr="";

	//��갪�� ���� ���� ���� �̵�
	double dCurPos=0.0;
	double dMovePos = 0.0;
#if (____AA_WAY == PCB_TILT_AA)
	dMovePos = motor.GetCommandPos(TITLE_MOTOR_TY) + Task.SFR.dTilt_Y;
#elif (____AA_WAY == LENS_TILT_AA)
	dMovePos = motor.GetEncoderPos(TITLE_MOTOR_TY) + Task.SFR.dTilt_Y;
#endif
	logStr.Format("[procTiltY] TiltY : %.03lf", Task.SFR.dTilt_Y);
	putListLog(logStr);
	
	motor.MoveAxis(TITLE_MOTOR_TY, 1, dMovePos, sysData.fMotorSpeed[TITLE_MOTOR_TY], sysData.fMotorAccTime[TITLE_MOTOR_TY]);

	logStr.Format("[procTiltY] MovePos %.03lf", dMovePos);
	putListLog(logStr);

	double ep = myTimer(true);
	double ep2 = myTimer(true);
	Sleep(100);

	while (1)
	{
		ep2 = myTimer(true);
		if ( ep2-ep < 5000)
		{
#if (____AA_WAY == PCB_TILT_AA)
			//logStr.Format("[procTiltY] CurPos %.03lf", motor.GetCommandPos(TITLE_MOTOR_TY));
			//putListLog(logStr);
			if ( motor.IsStopAxis(TITLE_MOTOR_TY))
			{
				logStr.Format("[procTiltY] EpTime : %.01lf", myTimer(true)-ep);
				putListLog(logStr);
				dCurPos = motor.GetCommandPos(TITLE_MOTOR_TY);
#elif (____AA_WAY == LENS_TILT_AA)
			//logStr.Format("[procTiltY] CurPos %.03lf", motor.GetCommandPos(TITLE_MOTOR_TY));
			//putListLog(logStr);
			if ( motor.IsStopAxis(TITLE_MOTOR_TY))
			{
				logStr.Format("[procTiltY] EpTime : %.01lf", myTimer(true)-ep);
				putListLog(logStr);
				dCurPos = motor.GetCommandPos(TITLE_MOTOR_TY);
#endif	

				if (fabs(dCurPos-dMovePos)<=0.01)
				{
					break;
				}
				else
				{
					putListLog("Tilting PCB Yt Motor �̵� ������");
					errMsg2(Task.AutoFlag, sLangChange);
					return -1;
				}
			}
		}
		else
		{
			putListLog("Tilting PCB Yt Motor �̵� �ð� �ʰ�.");	
			return -1;
		}
	}
	logStr.Empty();
	return 1;
}


int	CAABonderDlg::_procTiltX()
{
  	CString logStr="";

	/* ��갪�� ���� ���� ���� �̵� */
	double dCurPos = 0.0;
	double dOrgPos = motor.GetCommandPos(TITLE_MOTOR_TX);
	double dMovePos = motor.GetCommandPos(TITLE_MOTOR_TX) + Task.SFR.dTilt_X;

	motor.MoveAxis(TITLE_MOTOR_TX, 1, dMovePos, sysData.fMotorSpeed[TITLE_MOTOR_TX], sysData.fMotorAccTime[TITLE_MOTOR_TX]);

	double ep = myTimer(true);
	Sleep(100);

	while (1)
	{

		if ( myTimer(true)-ep < 5000)
		{

			//logStr.Format("[procTiltX] CurPos %.03lf", motor.GetCommandPos(TITLE_MOTOR_TX));
			//putListLog(logStr);
			if ( motor.IsStopAxis(TITLE_MOTOR_TX) )
			{

				//logStr.Format("[procTiltX] EpTime : %.01lf", myTimer(true)-ep);
				//putListLog(logStr);

				dCurPos = motor.GetCommandPos(TITLE_MOTOR_TX);

				if (fabs(dCurPos-dMovePos)<=0.01)
				{
					break;
				}
				else
				{
					logStr.Format("Tilting Xt Motor �̵� ������  (Limit ���� Ȯ��).");
					putListLog(logStr);
					errMsg2(Task.AutoFlag, logStr);
					return -1;
				}
			}
		}
		else
		{
			putListLog("Tilting Xt Motor �̵� �ð� �ʰ�.");
			return -1;
		}
	}
	logStr.Empty();
	return 1;
}


int CAABonderDlg::_moveXYMotor()
{
	char logMsg[1000];
	double eTime;
	short axis[2];
	double pos[2];

	for (int i=Motor_Lens_X; i<=Motor_Lens_Y; i++)
	{
		if(!motor.m_bOrgFlag[i])
		{
			sLangChange.LoadStringA(IDS_STRING725);	//Lens �� X, Y ���Ͱ� ���� ���͸� ���� �ʾҽ��ϴ�.
			_stprintf_s(logMsg, _T("%s ") + sLangChange, MotorName[i]);
			errMsg2(Task.AutoFlag,logMsg);
			return false;
		}

		axis[i-Motor_Lens_X]		= i;

		if(i==Motor_Lens_X)					pos[i-Motor_Lens_X]	= motor.GetEncoderPos(Motor_Lens_X) - Task.dOpticalShiftX;
		else if(i==Motor_Lens_Y)			pos[i-Motor_Lens_X]	= motor.GetEncoderPos(Motor_Lens_Y) - Task.dOpticalShiftY;
	}


	int step=0;

#ifndef	ON_LINE_MOTOR
	step = 1000;
#endif

	double ep;

	while(1)
	{
		switch(step)
		{
		case 0:									// 3�� ���� ��� ��ġ�� �̵� ���..
			ep = myTimer(true);
			motor.goMotorPos(2, axis, pos);
			step = 100;
			break;

		case 100:								// �̵� �Ϸ� Ȯ��..
			if ( motor.IsStopAxis(Motor_Lens_X) && motor.GetInposition(Motor_Lens_X) )
			{
				step = 101;
			}
			break;

		case 101:
			if ( motor.IsStopAxis(Motor_Lens_Y) && motor.GetInposition(Motor_Lens_Y) )
			{
				step = 1000;
			}
			break;

		default:
			break;
		}

		if(step==1000)
			break;


		eTime = myTimer(true);
		if(eTime-ep>MOTOR_MOVE_TIME)
		{
			motor.StopAxis(Motor_Lens_X);
			motor.StopAxis(Motor_Lens_Y);

			if(step>100)
			{
				int motorNo = step - 100;
				sLangChange.LoadStringA(IDS_STRING723);	//Lens �� [%s ����] �̵� �ð� �ʰ�
				_stprintf_s(logMsg, sLangChange, MotorName[motorNo]);
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING724);	//Lens �� X, Y ���� ���� ���� �̵� ����.
				_stprintf_s(logMsg, sLangChange);
			}

			errMsg2(Task.AutoFlag, logMsg);

			return -1;
		}

		Sleep(20);
		checkMessage();
	}

	
	return 1;
}


bool CAABonderDlg::Bonding_Pos_Ckeck()
{
	if ((motor.IsStopAxis(Motor_Lens_X) && motor.GetInposition(Motor_Lens_X)) &&
		(motor.IsStopAxis(Motor_Lens_Y) && motor.GetInposition(Motor_Lens_Y)) &&
		(motor.IsStopAxis(Motor_Lens_Z) && motor.GetInposition(Motor_Lens_Z)) &&
		(motor.IsStopAxis(Motor_Lens_Xt) &&
		(motor.IsStopAxis(Motor_Lens_Yt)) && motor.GetInposition(Motor_Lens_Yt)) &&
			(motor.IsStopAxis(Motor_PCB_X) && motor.GetInposition(Motor_PCB_X)) &&
		(motor.IsStopAxis(Motor_PCB_Y) && motor.GetInposition(Motor_PCB_Y)) &&
		(motor.IsStopAxis(Motor_PCB_TH) && motor.GetInposition(Motor_PCB_TH)) &&
		(motor.IsStopAxis(Motor_PCB_Xt)) &&
		(motor.IsStopAxis(Motor_PCB_Yt)))
	{
		return true;
	}
	else
	{
		return false;
	}

}



void CAABonderDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CRect rcTemp;

	GetDlgItem(IDC_LABEL_TIME)->GetWindowRect(rcTemp);

	CDialogEx::OnLButtonDblClk(nFlags, point);
}



// MilProcImageChild[4] �̿� �˻� ����..
bool CAABonderDlg::_GetOpticAxis(int fi_scale, int fi_thVal, double &fo_dOffsetX, double &fo_dOffsetY)
{
	int pitch = MbufInquire(vision.MilProcImageChild[3], M_PITCH, NULL);
	int sizeX = MbufInquire(vision.MilProcImageChild[3], M_SIZE_X, NULL);
	int sizeY = MbufInquire(vision.MilProcImageChild[3], M_SIZE_Y, NULL);


	fo_dOffsetX = sizeX>>1;
	fo_dOffsetY = sizeY>>1;


	int* pHist_x;
	int* pHist_y;

	pHist_x = new int[sizeX];
	memset(pHist_x, 0 , sizeof(int)*sizeX);
	pHist_y = new int[sizeY];
	memset(pHist_y, 0 , sizeof(int)*sizeY);


	CRect  centRoi, upperRoi, inspRoi;

	centRoi.left	= (long)fo_dOffsetX - sizeX/20;
	centRoi.right	= (long)fo_dOffsetX + sizeX/20;
	centRoi.top		= (long)fo_dOffsetY - sizeY/20;
	centRoi.bottom	= (long)fo_dOffsetY + sizeY/20;

	upperRoi.left	= (long)fo_dOffsetX - sizeX/20;
	upperRoi.right	= (long)fo_dOffsetX + sizeX/20;
	upperRoi.top	= (long)0;
	upperRoi.bottom	= (long)sizeY/20;

	inspRoi.left	= (long)sizeX/20;
	inspRoi.right	= (long)sizeX - inspRoi.left;
	inspRoi.top		= (long)sizeY/20;
	inspRoi.bottom	= (long)sizeY - inspRoi.top;

	vision.boxlist[CCD].addList(centRoi, PS_SOLID, M_COLOR_GREEN);
	vision.boxlist[CCD].addList(upperRoi, PS_SOLID, M_COLOR_RED);
	vision.boxlist[CCD].addList(inspRoi, PS_SOLID, M_COLOR_BLUE);


	int centAvg, upperAvg, inspAvg;
	int x, y, pos, sum;
	

	int sx = centRoi.left;
	int ex = centRoi.right;
	int sy = centRoi.top;
	int ey = centRoi.bottom;

	sum = 0;
	for (y=sy; y<ey; y++)
	{
		pos = y * pitch + sx;

		for (x=sx; x<ex; x++)
		{
			sum += vision.MilImageBuffer[4][pos];
			pos++;
		}
	}
	centAvg = sum / ((ex-sx) * (ey-sy));


	sx = upperRoi.left;
	ex = upperRoi.right;
	sy = upperRoi.top;
	ey = upperRoi.bottom;

	sum = 0;
	for (y=sy; y<ey; y++)
	{
		pos = y * pitch + sx;

		for (x=sx; x<ex; x++)
		{
			sum += vision.MilImageBuffer[4][pos];
			pos++;
		}
	}
	upperAvg = sum / ((ex-sx) * (ey-sy));


	if(centAvg < 20/*60*/)	//KYH Mando Camera ���� ����
	{
		sLangChange.LoadStringA(IDS_STRING236);
		putListLog(sLangChange);
		return false;
	}

	if(centAvg < upperAvg)
	{
		sLangChange.LoadStringA(IDS_STRING237);
		putListLog(sLangChange);
		return false;
	}

	if(centAvg - upperAvg < 10/*20*/)	//KYH Mando Camera ���� ����
	{
		sLangChange.LoadStringA(IDS_STRING235);
		putListLog(sLangChange);
		return false;
	}


	Task.iOpticalThVal = inspAvg = (centAvg+upperAvg) / 2;


	sx = inspRoi.left;
	ex = inspRoi.right;
	sy = inspRoi.top;
	ey = inspRoi.bottom;

	int val;
	sum = 0;
	for (y=sy; y<ey; y++)
	{
		pos = y * pitch + sx;

		for (x=sx; x<ex; x++)
		{
			val = vision.MilImageBuffer[4][pos];

			if(val>=inspAvg)
			{
				pHist_x[x]++;
				pHist_y[y]++;
				sum++;
			}
			pos++;
		}
	}


	float fAddVal = 0;
	for (x=sx; x<ex; x++)
	{
		fAddVal += (float)(pHist_x[x] * x);
	}

	float f_Cent_X = fAddVal/sum;

	if( (f_Cent_X < pitch*0.3) || (f_Cent_X > pitch*0.7) )
	{
		sLangChange.LoadStringA(IDS_STRING238);
		putListLog(sLangChange);
		return false;

	}


	fAddVal = 0;
	for (y=sy; y<ey; y++)
	{
		fAddVal += (float)(pHist_y[y] * y);
	}

	float f_Cent_y = fAddVal/sum;

	if( (f_Cent_y < sizeY*0.3) || (f_Cent_y > sizeY*0.7) )
	{
		sLangChange.LoadStringA(IDS_STRING239);
		putListLog(sLangChange);
		return false;

	}

	fo_dOffsetX = f_Cent_X;
	fo_dOffsetY = f_Cent_y;

	delete [] pHist_x;
	pHist_x = NULL;

	delete [] pHist_y;
	pHist_y = NULL;

	return true;
}


void CAABonderDlg::Delete_Child_Dialog()
{
	//if ( m_pLEDDlg != NULL )
	//{
	//	m_pLEDDlg->DestroyWindow();
	//	delete m_pLEDDlg;
	//	m_pLEDDlg = NULL;
	//}

	/*if( m_pDisplacementDlg != NULL )
	{
		m_pDisplacementDlg->DestroyWindow();
		delete m_pDisplacementDlg;
		m_pDisplacementDlg = NULL;
	}*/
}

void CAABonderDlg::Make_Child_Dialog()
{
	Delete_Child_Dialog();

	BOOL bCreate = FALSE;

	/*m_pLEDDlg  = new CLightDlg;
	if ( m_pLEDDlg == NULL )
	{
		return ;
	}
	bCreate = m_pLEDDlg->Create(IDD_DIALOG_LIGHT);
	if ( bCreate == FALSE )
	{
		return;
	}
	m_pLEDDlg->ShowWindow(SW_HIDE);*/

	/*m_pDisplacementDlg = new CDisplacementDlg;
	if( m_pDisplacementDlg == NULL )	return;
	bCreate = m_pDisplacementDlg->Create(IDD_DIALOG_DISPLACEMENT);
	if ( bCreate == FALSE )
	{
		return ;
	}
	m_pDisplacementDlg->ShowWindow(SW_HIDE);*/
	
	return;
}



void CAABonderDlg::OnBnClickedButtonPause()
{
	if(Task.AutoFlag != 1)
	{
		sLangChange.LoadStringA(IDS_STRING1374);	//�ڵ��������� �ƴմϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return;
	}

	Dio.setAlarm(AUTO_STOP);


	//if(sysData.m_FreeRun == 1)
	//{
	//	Task.AutoFlag		= 0;			//	// 0:���� 1:�ڵ� 2: �Ͻ����� 20130405	
	//	Task.PausePCBStep	= 0;
	//}
	//else
	//{
		Task.AutoFlag		= 2;			//	// 0:���� 1:�ڵ� 2: �Ͻ����� 20130405	
		Task.PausePCBStep	= Task.PCBTask;
		//Task.PauseLensStep	= Task.LensTask;
	//}

	AutoRunView(Task.AutoFlag);
	Task.AutoReday = 0;

	bThreadTaskPcb = false;			//	 ���� �Ҷ� ��� 0:������ ���� ����
	bThreadTaskLens = false;

	CString sTemp;
	sLangChange.LoadStringA(IDS_STRING895);
	sTemp.Format(sLangChange, Task.AutoFlag, Task.PCBTask);
	putListLog(sTemp);

	sysData.m_FreeRun	= 0;
}


void CAABonderDlg::OnBnClickedButtonStop()
{
	Task.bMotorRunStop = false;		//pcb thread �ȿ� while���� ������������ �߰�, 

	Task.AutoFlag = 0;		//��ġ�̵� 250107

	bThreadEpoxyRun = false;

	motor.StopAxisAll();
	if (pThread_Epoxy != NULL)
	{
		::WaitForSingleObject(pThread_Epoxy->m_hThread, 100);
	}

	vision.clearOverlay(CAM1);
	vision.clearOverlay(CAM2);
	
	Dio.StartPBLampOn(false);
	Dio.setAlarm(AUTO_STOP);

	//Dio.PCBvaccumOn(VACCUM_OFF, false);

	
	AutoRunView(Task.AutoFlag);

	bThreadTaskPcb		= false;
	bThreadTaskLens		= false;
	Task.AutoReday		= 0;
	sysData.m_FreeRun	= 0;

	Task.PcbOnStage = 100;

	Task.m_iStatus_Unit_Epoxy = 0;
	m_btnStart.m_iStateBtn = 0;
	m_btnStart.Invalidate();

	//UVCommand.ReadyDevice();
	

	bThreadTaskPcbRun = false;
	bThreadTaskLensRun = false;

	vision.clearOverlay(CCD);
	
	if(!bThread_MIUCheckRun)
	{
		putListLog("CCD Close ����.");
		MIU.Close();
		putListLog("CCD Close �Ϸ�.");
	}
	DisableButton(Task.AutoFlag);
	g_bMovingflag = false;
}



void CAABonderDlg::OnBnClickedButtonAutorun()
{
	if(g_bMovingflag)
	{
		putListLog("	�����̵���!");
		return;
	}

	if(Task.AutoFlag == 1)
	{
		return;
	}
	if ( bThreadTaskReadyRun == true)
	{
		putListLog("	�����غ����Դϴ�!");
		return;
	}
	if(Task.AutoReday == 0 && Task.AutoFlag == 0)
	{
#ifndef	ON_LINE_MODE
		Task.AutoFlag = 1;
#endif
		sLangChange.LoadStringA(IDS_STRING1326);	//���� �غ� ���� �ʾҽ��ϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return;
	}


	for(int iCamNo=0; iCamNo<MARK_CNT; iCamNo++)
	{
		int iCh = 1;
		if(iCamNo == PCB_Chip_MARK)	iCh = 0;

		for (int iMarkNo=0; iMarkNo<2; iMarkNo++)
		{
			vision.geometricMarkPreProc(iCh, iCamNo, iMarkNo);
		}
	}

	if(Task.AutoFlag ==0 )
	{// 0:���� 1:�ڵ� 2: �Ͻ����� 20130405
		Task.LensTask	= 10000;		
		Task.PCBTask	= 10000;
	}
	else if(Task.AutoFlag ==2)
	{
		Task.LensTask	= Task.PauseLensStep;
		Task.PCBTask	= Task.PausePCBStep;
		Dio.StartPBLampOn(true);
	}

	Task.m_iStart_Step_PCB	= 10000;
	Task.m_iEnd_Step_PCB	= 170000;
	//
	Task.m_iStart_Step_LENS	= 10000;
	Task.m_iEnd_Step_LENS = 60000;

	#ifdef ON_LINE_VISION
		if(!m_bMiuRun && Task.AutoFlag)
		{
			
			if(!bThread_MIUCheckRun)
			{
				bThread_MIUCheckRun = true;
				MIUCheck_process();
				bThread_MIUCheckRun = false;
			}
			
		}
	#endif

	g_bMovingflag =true;
	//Dio.setAlarm(AUTO_RUN);
	Task.AutoFlag = 1;
	Task.m_iStatus_Unit_Epoxy = 0;
	AutoRunView(Task.AutoFlag);
	Task.PCBTaskTime = Task.LensTaskTime = myTimer(true);
	Task.AutoReday = 0;
	Dio.StartPBLampOn(true);
	g_AlarmFlag = true;

	Task.m_timeChecker.Init_Time();
	Task.m_timeChecker.Start_Time();

	pThread_TaskPcb = ::AfxBeginThread(Thread_TaskPcb, this);
	pThread_TaskLens = ::AfxBeginThread(Thread_TaskLens, this);
}


void CAABonderDlg::OnBnClickedButtonReady()		
{
	if(g_bMovingflag)
	{
		return;
	}

	if(Task.AutoFlag==1)
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_RED);
		return;
	}

	if(Task.AutoFlag==2)
	{
		sLangChange.LoadStringA(IDS_STRING1362);	//"�Ͻ� ���� �� �Դϴ�."
		delayMsg(sLangChange, 3000, M_COLOR_RED);
		return;
	}

	Dio.StartPBLampOn(false);

	if (Dio.DoorPBOnCheck(true, false))
	{
		Dio.DoorPBLampOn(true);
	}
	else
	{
		Dio.DoorPBLampOn(false);
	}
	if (sysData.m_iFront == 0)
	{
		if (!Dio.DoorLift(false, true))
		{

			sLangChange.LoadStringA(IDS_STRING1471);	//���� �غ� ����. Door Open ����..
			delayMsg(sLangChange, 50000, M_COLOR_RED);
			g_bMovingflag = false;
			return;
		}
	}

	//Dio.setAlarm(ALARM_OFF);

	Task.AutoFlag = 1;
	AutoRunView(Task.AutoFlag);

	for(int i=0; i<MAX_MTF_NO;i++)
	{
		Task.SFR.fSfrN4[Task.m_iDrawBarStep-1][i] = 0.0;
	}

	myTimer(false);
	Task.bManual_FindEpoxy = false;

	iLaser_Pos = 0;

	Task.ReadyTask	= 10000;
	Task.PCBTask	= 10000;


	Task.PausePCBStep	= 10000;

	pThread_TaskReady = ::AfxBeginThread(Thread_TaskReady, this);
}


void CAABonderDlg::DispCurModelName(CString sName)
{
	m_labelCurModelName.SetText(sName);
	m_labelCurModelName.Invalidate();
}

void CAABonderDlg::initInspResGrid()
{//���� ������ Camer �˻� ��� 
	dispInspResGrid();
}

void CAABonderDlg::dispInspResGrid()
{

}

void CAABonderDlg::InitGridCtrl_Result()
{
	CRect rect;
	CWnd *pWnd= (CWnd*)GetDlgItem(IDC_STATIC_RESULT_GRID); 
	ResultRow = 8;//�Ʒ�
	ResultCol = 2;//��
	int margin = 4;
	int gridHeight = 32;
	int gridWidth1 = 100;
	int gridWidth2 = 82;
	int totalWidth = gridWidth1+(gridWidth2*(ResultCol-1));
	//
	pWnd->GetWindowRect(rect);
	ScreenToClient(rect);

	rect.right = totalWidth +margin;
	rect.bottom = (gridHeight*ResultRow) +margin;
	pWnd->MoveWindow(rect.left, rect.top, rect.right, rect.bottom);//���̾�α��� ũ��� ��ġ�� ������ ���� �Լ�.

	GetDlgItem(IDC_STATIC_RESULT_GRID)->GetWindowRect(rect);
	ScreenToClient(rect);
	m_clGridResult.Create(rect, this, IDC_STATIC_RESULT_GRID, WS_TABSTOP | WS_VISIBLE);

	m_clGridResult.SetTextBkColor(GRID_COLOR_WHITE);
	m_clGridResult.SetFixedBkColor(GRID_COLOR_TITLE);
	m_clGridResult.SetFixedTextColor(GRID_COLOR_WHITE);
	m_clGridResult.SetReference_Setting();
	m_clGridResult.EnableSelection(FALSE);
	m_clGridResult.SetRowCount(ResultRow);
	m_clGridResult.SetColumnCount(ResultCol);
	m_clGridResult.SetFixedRowCount(1);
	m_clGridResult.SetFixedColumnCount(1);

	CString tmpStr="";

	m_clGridResult.SetItemText(0, 1, "SEC");
	m_clGridResult.SetItemText(1, 0, "���귮");
	m_clGridResult.SetItemText(2, 0, "NG ����");
	m_clGridResult.SetItemText(3, 0, "TOTAL T/T");
	m_clGridResult.SetItemText(4, 0, "Dispensing T/T");
	m_clGridResult.SetItemText(5, 0, "Laser ���� T/T");
	m_clGridResult.SetItemText(6, 0, "AA T/T");
	m_clGridResult.SetItemText(7, 0, "�˻� T/T");


	//
	int i=0, j=0;
	for (i = 0; i < ResultRow; i++)
	{
		m_clGridResult.SetRowHeight(i, gridHeight);
		
		for (j = 0; j < ResultCol; j++)
		{
			m_clGridResult.SetItemFormat(i, j, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			if (i == 0)
			{
				if (j == 0)
				{
					m_clGridResult.SetColumnWidth(j, gridWidth1);
				}else{
					m_clGridResult.SetColumnWidth(j, gridWidth2);
				}
			}			
		}
	}
	m_clGridResult.Invalidate();
	dispGrid();
}
void CAABonderDlg::ShowGridCtrl_Result()
{

}
void CAABonderDlg::OnDBClickedGridResult(NMHDR* pNMHDR, LRESULT* pResult)
{
	if(Task.AutoFlag == 1)
	{
		sLangChange.LoadStringA(IDS_STRING1463);
		delayMsg(sLangChange, 1000, M_COLOR_RED);
		return;
	}

	NM_GRIDVIEW* pNMGridView = (NM_GRIDVIEW*)pNMHDR;

	int iRow = pNMGridView->iRow;
	int iCol = pNMGridView->iColumn;

	if ( iRow <= 0 || iRow > 2 )	return;
	CString sTemp="";
	if ( iRow == 1 || iRow == 2 )
	{
		sTemp.Format("���귮�� �ʱ�ȭ �Ͻðڽ��ϱ�?");
		if ( askMsg(sTemp.GetBuffer()) == IDOK )
		{
	
			work.m_iCnt_Output = 0;
			work.m_iCnt_NG_Output = 0;
			// 20141103 LHC - ���귮 �ʱ�ȭ�� T/T �ʱ�ȭ
			Task.m_dTime_Total = 0;
			Task.m_dTime_Epoxy = 0;
			Task.m_dTime_LaserDpm = 0;
			Task.m_dTime_FineAA = 0;
			Task.m_dTime_TotalAA = 0;
		}
	}
	
	dispGrid();

}
void CAABonderDlg::initGrid()
{
	
}
bool CAABonderDlg::g_CalcImageAlign(bool bRotationUse)
{
    CString sLog = "";
    double dShiftX, dShiftY;
    double dRad, dAng;
    double dWidth, dHeight;
	double dCenterX = gMIUDevice.nWidth / 2;
	double dCenterY = gMIUDevice.nHeight / 2;	//(g_clModelData[nUnit].m_nHeight - 4) / 2;
    double m_Centerx = (Task.m_CirclePos_x[0] + Task.m_CirclePos_x[1] + Task.m_CirclePos_x[2] + Task.m_CirclePos_x[3]) / 4;
    double m_Centery = (Task.m_CirclePos_y[0] + Task.m_CirclePos_y[1] + Task.m_CirclePos_y[2] + Task.m_CirclePos_y[3]) / 4;

    model.m_oc_x = (gMIUDevice.nWidth / 2) - m_Centerx;		//g_CalcImageAlign
    model.m_oc_y = (gMIUDevice.nHeight / 2) - m_Centery;
	//
	//rotation
	double dSize_Cell = model.m_dSize_CCD_Cell;
	dWidth = (Task.m_CirclePos_x[3] - Task.m_CirclePos_x[2]) * (dSize_Cell / 1000);
	dHeight = (Task.m_CirclePos_y[3] - Task.m_CirclePos_y[2]) * (dSize_Cell / 1000);

	dRad = atan(dHeight / dWidth);
	dAng = dRad * 180.0f / M_PI;
	//
	if (bRotationUse == true)
	{
		Task.dLensRotation = dAng;
		sLog.Format("ROTATION:%.3lf", Task.dLensRotation);
		putListLog(sLog);


		MESCommunication.m_dMesOC[0] = model.m_oc_x + dCenterX;
		MESCommunication.m_dMesOC[1] = model.m_oc_y + dCenterY;
		MESCommunication.m_dMesDeltaOC[0] = model.m_oc_x;//delta �� ������
		MESCommunication.m_dMesDeltaOC[1] = model.m_oc_y;

		sLog.Format("[YUV] oc x: %.2lf , oc y:%.2lf", MESCommunication.m_dMesOC[0], MESCommunication.m_dMesOC[1]);
		putListLog(sLog);
	}
	
	

    sLog.Format("oc x: %.2lf , oc y:%.2lf", model.m_oc_x, model.m_oc_y);
    putListLog(sLog);


    return true;
}


void CAABonderDlg::dispGrid()
{
	return;
	CString sTemp="";

	if ( work.m_iCnt_Output < 0 )		sTemp = "-";
	else								sTemp.Format("%d", work.m_iCnt_Output);
	m_clGridResult.SetItemText(1, 1, sTemp);

	if ( work.m_iCnt_NG_Output < 0 )	sTemp = "-";
	else								sTemp.Format("%d", work.m_iCnt_NG_Output);
	m_clGridResult.SetItemText(2, 1, sTemp);

	if ( Task.m_dTime_Total < 0 )		sTemp = "-";
	else								sTemp.Format("%.1lf", Task.m_dTime_Total/1000);
	m_clGridResult.SetItemText(3, 1, sTemp);
	
	if ( Task.m_dTime_Epoxy < 0 )		sTemp = "-";
	else								sTemp.Format("%.1lf", Task.m_dTime_Epoxy/1000);
	m_clGridResult.SetItemText(4, 1, sTemp);

	if ( Task.m_dTime_LaserDpm < 0 )	sTemp = "-";
	else								sTemp.Format("%.1lf", Task.m_dTime_LaserDpm/1000);
	m_clGridResult.SetItemText(5, 1, sTemp);

	if (Task.m_dTime_FineAA < 0 )		sTemp = "-";
	else								sTemp.Format("%.1lf", Task.m_dTime_FineAA/1000);
	m_clGridResult.SetItemText(6, 1, sTemp);

	if ( Task.m_dTime_TotalAA < 0 )		sTemp = "-";
	else								sTemp.Format("%.1lf", Task.m_dTime_TotalAA/1000);
	m_clGridResult.SetItemText(7, 1, sTemp);
	m_clGridResult.Invalidate();
}

BEGIN_EVENTSINK_MAP(CAABonderDlg, CDialogEx)

END_EVENTSINK_MAP()




bool CAABonderDlg::_calcImageAlignment()
{
	double dSize_Cell = model.m_dSize_CCD_Cell;

	double dShiftX, dShiftY;
	double dRad, dAng;
	double dWidth, dHeight;
	double dCenterX = gMIUDevice.nWidth/2;
	double dCenterY = gMIUDevice.nHeight/2;
	CString sLog; 

	  
	dShiftX = ((Task.m_CircleP[0].x+Task.m_CircleP[1].x+Task.m_CircleP[2].x+Task.m_CircleP[3].x)/4.0) - dCenterX;
	dShiftY = (((Task.m_CircleP[0].y + Task.m_CircleP[1].y + Task.m_CircleP[2].y + Task.m_CircleP[3].y) / 4.0) - dCenterY);

    MandoInspLog.dOCResult[0] = dShiftX;
	MandoInspLog.dOCResult[1] = dShiftY;
	dShiftX *= dSize_Cell/1000.0f; 
	dShiftY *= dSize_Cell/1000.0f; 

	dWidth = (Task.m_CircleP[3].x - Task.m_CircleP[2].x) * (dSize_Cell / 1000);
	dHeight = (Task.m_CircleP[3].y - Task.m_CircleP[2].y) * (dSize_Cell / 1000);
	 
	dRad = atan(dHeight/dWidth);
	dAng = dRad * 180.0f / M_PI;


	if(fabs(dShiftX) > 2.0 || fabs(dShiftY) > 2.0 || fabs(dAng) > 10.0)//3.0)		// ���ǰ�...
	{

		if(!Task.bInsCenter)
		{
			errMsg2(Task.AutoFlag, "Image Center �̹��� ����� ������ ����Ʈ ����.");
			return false;
		}
		else
		{
			sLog.Format("Image Center ���� error");		//Image Center ���� error
			putListLog(sLog);
		}

	}
	 
	if(!Task.bInsCenter)
	{
		Task.m_dShift_IMG_X = dShiftX;
		Task.m_dShift_IMG_Y = dShiftY;
		Task.m_dShift_IMG_TH = dAng;
	}
	else
	{
		               
		int CX  = (Task.m_CircleP[0].x + Task.m_CircleP[1].x + Task.m_CircleP[2].x + Task.m_CircleP[3].x)/4;
		int CY  = (Task.m_CircleP[0].y + Task.m_CircleP[1].y + Task.m_CircleP[2].y + Task.m_CircleP[3].y)/4;
		dWidth  = (Task.m_CircleP[3].x - Task.m_CircleP[2].x);
		dHeight = (Task.m_CircleP[3].y - Task.m_CircleP[2].y);
		dRad = atan(dHeight/dWidth);
		dAng = dRad * 180.0 / M_PI;
  		sLog.Format("[�˻�] Image Center [X=%f, Y=%f, T=%f]", dShiftX, dShiftY, dAng);
		putListLog(sLog);
	}

	Task.m_dShift_IMG_X = dShiftX;
	Task.m_dShift_IMG_Y = dShiftY;
	sLog.Empty();
	
	return true;
}


bool CAABonderDlg::_MotorMove_IMG_Align()//���� sfr
{ 
	double ep = myTimer(true);
	int dicX = 1;
	int dicY = 1;


	if (model.Tilt_Diretion[0] < 0)
	{
		dicX = -1;
	}
	if (model.Tilt_Diretion[1] < 0)
	{
		dicY = -1;
	}

	if (sysData.m_iDicChange == 0)
	{
		motor.MoveAxis(TITLE_MOTOR_X, REL, Task.m_dShift_IMG_X*dicX, sysData.fMotorSpeed[TITLE_MOTOR_X], sysData.fMotorAccTime[TITLE_MOTOR_X]);
		motor.MoveAxis(TITLE_MOTOR_Y, REL, Task.m_dShift_IMG_Y*dicY, sysData.fMotorSpeed[TITLE_MOTOR_Y], sysData.fMotorAccTime[TITLE_MOTOR_Y]);
	}
	else
	{
		motor.MoveAxis(TITLE_MOTOR_X, REL, Task.m_dShift_IMG_Y*dicY, sysData.fMotorSpeed[TITLE_MOTOR_X], sysData.fMotorAccTime[TITLE_MOTOR_X]);
		motor.MoveAxis(TITLE_MOTOR_Y, REL, Task.m_dShift_IMG_X*dicX, sysData.fMotorSpeed[TITLE_MOTOR_Y], sysData.fMotorAccTime[TITLE_MOTOR_Y]);
	}
	
 
	
	Sleep(10);

	while (1)
	{
		if ( ( motor.IsStopAxis(TITLE_MOTOR_X) && motor.GetInposition(TITLE_MOTOR_X) ) && 
			 ( motor.IsStopAxis(TITLE_MOTOR_Y) && motor.GetInposition(TITLE_MOTOR_Y) ) )
		{
			break;
		}
		else if ( myTimer(true)-ep > 1000 )
		{
			sLangChange.LoadStringA(IDS_STRING1356);
			errMsg2(Task.AutoFlag, sLangChange);
			return false;
		}
	}

	return true;
}

bool CAABonderDlg::_MotorMove_IMG_AlignTheta()
{
	double ep = myTimer(true);
	int dicTH = 1;  

	if (model.Tilt_Diretion[4] < 0)
	{
		dicTH = -1;
	}


	motor.MoveAxis(Motor_PCB_TH, REL, Task.m_dShift_IMG_TH * dicTH, sysData.fMotorSpeed[Motor_PCB_TH], sysData.fMotorAccTime[Motor_PCB_TH]);//���� Ȯ���ؾߵ�
	Sleep(10);

	while (1)
	{
		if ( ( motor.IsStopAxis(Motor_PCB_TH)))// && motor.GetInposition(Motor_PCB_TH) ) )
		{
			break;
		}
		else if ( myTimer(true)-ep > 1000 )
		{
			sLangChange.LoadStringA(IDS_STRING1357);	//�̹��� ����� PCB Theta�� ���� �̵� �ð� �ʰ�.
			errMsg2(Task.AutoFlag, sLangChange);
			return false;
		}
	}

	return true;
}


void CAABonderDlg::OnBnClickedRadioAlign()
{
	ctrlSubDlg(MAIN_DLG);
	m_iCurCamNo =0;
	setCamDisplay(0, 1);
	changeMainBtnColor(MAIN_DLG);
}


void CAABonderDlg::OnBnClickedRadioCcd2()
{
	ctrlSubDlg(MAIN_DLG);
	setCamDisplay(3, 1);
	changeMainBtnColor(MAIN_DLG);
}


void CAABonderDlg::OnStnClickedLabelStatusUsbLive()
{
	if(Task.AutoFlag == 1)
	{
		sLangChange.LoadStringA(IDS_STRING1463);//"�ڵ� ���� �� ��� �Ұ�"
		delayMsg(sLangChange, 1000, M_COLOR_RED);
		return;
	}
	
	pThread_MIUCheck = ::AfxBeginThread(Thread_MIUCheck, this);
	return;
}


int CAABonderDlg::_checkDecreaseSFR()
{
	int j=0;
	int iIndex_Start=0;
	int iIndex_Check = model.m_iCnt_Check_SFR;//�Ķ����â ���� , ����3
	double sfrLimit = sysData.dMax_Sfr_Limit;
	if (sfrLimit < 0.01)
	{
		sfrLimit = 0.1;
	}
	if ( Task.m_bFlag_Decrease_SFR == true )
	{
		iIndex_Start = model.m_iCnt_Check_SFR;
		iIndex_Check = model.m_iCnt_Check_SFR * 2;
	}
	else{
		iIndex_Start = 0;
	}
	for (int i=iIndex_Start ; i<MAX_MTF_NO ; i++)
	{
//////////////////////////////////////////////////////////////////////////////////////////
			for (j=iIndex_Start ; j<iIndex_Check ; j++)
			{
				if (Task.SFR.fSfrN4[j][i] >= sfrLimit)//if ( Task.SFR.fSfrN4[j][i] >= 0.2 )
				{
					break;
				}
			}

			if (j == iIndex_Check){
				continue;
			}
			for (j=1 ; j<iIndex_Check ; j++)
			{
				if ( (Task.SFR.fSfrN4[j-1][i] - Task.SFR.fSfrN4[j][i]) < 0 )
					break;
			}

			if ( j == iIndex_Check ){
				return -1;
			}else{
				continue;
			}
	}

	return 1;
}


void CAABonderDlg::OnBnClickedCheckDist()
{
	m_bMeasureDist = !m_bMeasureDist;

	if ( m_bMeasureDist )
	{
		drawLine_MeasureDist(0);
	}
	else
	{
		vision.clearOverlay(m_iCurCamNo);
		vision.drawOverlay(m_iCurCamNo);
	}
}


void CAABonderDlg::drawLine_MeasureDist(int iMode)
{
	vision.clearOverlay(m_iCurCamNo);

	int iSx, iSy, iEx, iEy;
	int iSizeX, iSizeY;

	if ( m_iCurCamNo < 3 )
	{
		iSizeX = CAM_SIZE_X;
		iSizeY = CAM_SIZE_Y;
	}
	else
	{
		iSizeX = gMIUDevice.nWidth - 1;
		iSizeY = gMIUDevice.nHeight - 1;
	}

	iSx = 0;
	iSy = 0;
	iEx = iSizeX;
	iEy = iSizeY;

	if ( iMode == 0 )
	{
		m_iLine_Left	= (int)(iSizeX * 0.25 + 0.5);
		m_iLine_Top		= (int)(iSizeY * 0.25 + 0.5);
		m_iLine_Right	= (int)(iSizeX * 0.75 + 0.5);
		m_iLine_Bottom	= (int)(iSizeY * 0.75 + 0.5);
	}

	vision.linelist[m_iCurCamNo].addList(m_iLine_Left, iSy, m_iLine_Left, iEy, PS_SOLID, M_COLOR_RED);
	vision.linelist[m_iCurCamNo].addList(m_iLine_Right, iSy, m_iLine_Right, iEy, PS_SOLID, M_COLOR_RED);
	vision.linelist[m_iCurCamNo].addList(iSx, m_iLine_Top, iEx, m_iLine_Top, PS_SOLID, M_COLOR_MAGENTA);
	vision.linelist[m_iCurCamNo].addList(iSy, m_iLine_Bottom, iEx, m_iLine_Bottom, PS_SOLID, M_COLOR_MAGENTA);

	double dDistWidth, dDistHeight;
	int	iPos_Text;

	if ( m_iCurCamNo < 3 )
	{
		dDistWidth		= (m_iLine_Right - m_iLine_Left) * sysData.dCamResol[m_iCurCamNo].x;
		dDistHeight		= (m_iLine_Bottom - m_iLine_Top) * sysData.dCamResol[m_iCurCamNo].y;
		iPos_Text = 50;
	}
	else
	{
		dDistWidth		= (m_iLine_Right - m_iLine_Left) * model.m_dSize_CCD_Cell / 1000;
		dDistHeight		= (m_iLine_Bottom - m_iLine_Top) * model.m_dSize_CCD_Cell / 1000;
		iPos_Text = 150;
	}

	CString sTemp1, sTemp2;
	sTemp1.Format("Width  : %.3lf mm", dDistWidth);
	sTemp2.Format("Height : %.3lf mm", dDistHeight);

	vision.textlist[m_iCurCamNo].addList(150, 50, sTemp1, M_COLOR_RED, 16, 12, "Arial");
	vision.textlist[m_iCurCamNo].addList(150, 50+iPos_Text, sTemp2, M_COLOR_MAGENTA, 16, 12, "Arial");

	vision.drawOverlay(m_iCurCamNo);
}

int	CAABonderDlg::changeCursor_MeasureMode(CPoint point)
{
	int iRtn = -1;
	int iGap = 20;
	double dExpandFactorX;
	double dExpandFactorY;
	CPoint p;

	if (m_iCurCamNo<3)
	{
		dExpandFactorX = CAM_EXPAND_FACTOR_X;
		dExpandFactorY = CAM_EXPAND_FACTOR_Y;
		iGap = 20;
	}
	else
	{
		dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
		dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
		// 20140905 Overlay Box ��ġ �̵� �� Box ������ �󸶳� ������ �־�� ���콺 Ŀ�� ��� ���� �Ÿ� Ȯ�� ��..
		//iGap = 50;
		iGap = int(dExpandFactorX * 5);
	}
	point.x -= m_rectCamDispPos1.left;
	point.y -= m_rectCamDispPos1.top;

	p.x = (int)(point.x * dExpandFactorX + 0.5);
	p.y = (int)(point.y * dExpandFactorY + 0.5);

	if ( m_iLine_Left - iGap < p.x && m_iLine_Left + iGap > p.x )
	{
		::SetCursor(m_hCursor_Width);
		iRtn = 1;
	}
	else if ( m_iLine_Top - iGap < p.y && m_iLine_Top + iGap > p.y )
	{
		::SetCursor(m_hCursor_Height);
		iRtn = 2;
	}
	else if ( m_iLine_Right - iGap < p.x && m_iLine_Right + iGap > p.x )
	{
		::SetCursor(m_hCursor_Width);
		iRtn = 3;
	}
	else if ( m_iLine_Bottom - iGap < p.y && m_iLine_Bottom + iGap > p.y )
	{
		::SetCursor(m_hCursor_Height);
		iRtn = 4;
	}

	return iRtn;
}


void CAABonderDlg::DisableButton(bool AutorunFlag)
{
//	GetDlgItem(IDC_BUTTON_MAIN)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_MODEL)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_LENS)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_PCB)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_RESIN)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_CCD)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_MOTOR)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_IO)->EnableWindow(!AutorunFlag);
//	GetDlgItem(IDC_BUTTON_LIGHT)->EnableWindow(!AutorunFlag);
	GetDlgItem(IDC_BUTTON_EXIT)->EnableWindow(!AutorunFlag);

	GetDlgItem(IDC_BUTTON_ORIGIN)->EnableWindow(!AutorunFlag);
	GetDlgItem(IDC_BUTTON_READY)->EnableWindow(!AutorunFlag);
	GetDlgItem(IDC_BUTTON_AUTORUN)->EnableWindow(!AutorunFlag);
}



void CAABonderDlg::AutoRunView(int curstate)		// Display��
{// ����:0 �ڵ�����:1 �Ͻ�����:2 �����غ�:3 
	m_btnReady.m_iStateBtn = 0;
	m_btnAutorun.m_iStateBtn = 0;
	m_btnPause.m_iStateBtn = 0;
	m_btnStop.m_iStateBtn = 0;
	m_btnNgOut.m_iStateBtn = 0;

	if(curstate == 0)		
		m_btnStop.m_iStateBtn = 1;	
	else if(curstate == 1)		
		m_btnAutorun.m_iStateBtn = 1;	
	else if(curstate == 2)		
	{
		m_btnAutorun.m_iStateBtn = 1;	
		m_btnPause.m_iStateBtn = 1;
	}
	else if(curstate == 3)
		m_btnReady.m_iStateBtn = 1;	

	m_btnReady.Invalidate();
	m_btnAutorun.Invalidate();
	m_btnPause.Invalidate();
	m_btnStop.Invalidate();
	m_btnNgOut.Invalidate();
}

bool CAABonderDlg::func_MTF(BYTE* ChartRawImage, bool bAutoMode, int dindex)
{  
	vision.drawOverlay(CCD);
	char szTmp[256];
	double mtfOffset = 0.0;
	CString sTemp="";

	if(!bAutoMode)
	{
		sTemp.Format(" -------------MTF ���� �˻�");	
	}else
	{
		sTemp.Format(" -------------MTF �˻� [step:%d]", Task.PCBTask);	
	}
	putListLog(sTemp);

	if(sysData.m_iProductComp == 1)
	{
		putListLog(" --MTF UV AFTER INSP");
	}else
	{
		if(sysData.m_iMTFUvInsp==1)
		{
			putListLog(" --MTF UV BEFORE INSP");
		}else
		{
			putListLog(" --MTF UV AFTER INSP");
		}
	}

	//
	int iCnt_MtfFocus = 0;
	int i = 0;
	int j = 0;
	TCHAR szLog[SIZE_OF_1K];

	double fMax_MTF_N4[COMMON_MTF_INSP_CNT] = { 0.0, };
	bool bMtfMaxTest = true;
	if (model.strInfo_CCM.m_iCnt_Average_Fine_Focus < 1)
	{
		model.strInfo_CCM.m_iCnt_Average_Fine_Focus = 1;
	}
	if (model.strInfo_CCM.m_iCnt_Average_Fine_Focus > 1)
	{
		bMtfMaxTest = false;
	}

	for (i = 0; i<COMMON_MTF_INSP_CNT; i++)
	{
		fMax_MTF_N4[i] = 0.0;
	}
	//





	
	double min = 0.0;
	double max = 0.0;

	double mDif4F = 0.0;
	double mDif7F = 0.0;
	
	vision.clearOverlay(CCD);

	MandoInspLog.sBarcodeID = Task.ChipID;


	IplImage *cvImgMtf = cvCreateImage(cvSize(gMIUDevice.nWidth, gMIUDevice.nHeight), 8, 3); // bmp for display
	cvImgMtf->imageData = (char*)MIU.m_pFrameBMPBuffer;
	Jpg_ImageSave(cvImgMtf,MTF_JPG);


    CString sLog;
    bool bResult = true;
    double dSFR[COMMON_MTF_INSP_CNT];
     
    CPoint Center;
    int dic = 0;
    bool sfrRt = true;
    int nBlackLevel = 0;
    Task.getROI();				// ���� ��ũ ��ġ �ν�..

    vision.MilBufferUpdate();

    if (Task.getROI_SFR(MTF_INSP) == false)
    {
		cvReleaseImage(&cvImgMtf);
        return false;
    }

	g_CalcImageAlign(true);		//func_MTF
	
    Center.x = (Task.m_CircleP[0].x+Task.m_CircleP[1].x+Task.m_CircleP[2].x+Task.m_CircleP[3].x)/4;
    Center.y =(Task.m_CircleP[0].y+Task.m_CircleP[1].y+Task.m_CircleP[2].y+Task.m_CircleP[3].y)/4; 

    double m_Centerx = (Task.m_CirclePos_x[0]+Task.m_CirclePos_x[1]+Task.m_CirclePos_x[2]+Task.m_CirclePos_x[3])/4;
    double m_Centery =(Task.m_CirclePos_y[0]+Task.m_CirclePos_y[1]+Task.m_CirclePos_y[2]+Task.m_CirclePos_y[3])/4; 

    model.m_oc_x = m_Centerx - (gMIUDevice.nWidth / 2);
	model.m_oc_y = m_Centery - (gMIUDevice.nHeight / 2);

	model.m_LogOC_X = m_Centerx;
	model.m_LogOC_Y = m_Centery;

    model.m_LogOC_DelatX = model.m_oc_x;
    model.m_LogOC_DelatY = model.m_oc_y;

	double mTestLinePulse = model.m_Line_Pulse;

	sTemp.Format("LinePulse = [%.4lf]", mTestLinePulse);
	theApp.MainDlg->putListLog(sTemp);

    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    int nWidth = gMIUDevice.imageItp->width;
    int nHeight = gMIUDevice.imageItp->height;
    int iSizeX = model.m_iSize_ROI_X + 1;
    int iSizeY = model.m_iSize_ROI_Y + 1; 

    std::shared_ptr<CACMISResolutionSFR> m_pSFRProc = std::make_shared<CACMISResolutionSFR>();
    POINT ptROICenter[COMMON_MTF_INSP_CNT];
	TSFRSpec m_stSFRSpec;

	iCnt_MtfFocus = model.strInfo_CCM.m_iCnt_Average_Fine_Focus;

	for (j = 0; j < iCnt_MtfFocus; j++)
	{
		if (j == 0)
		{
			for (i = 0; i < model.mGlobalSmallChartCount; i++)//for (int i = 0; i < MTF_INSP_CNT; i++)
			{
				ptROICenter[i].x = Task.SFR._64_Sfr_Rect[i].left + (iSizeX / 2);
				ptROICenter[i].y = Task.SFR._64_Sfr_Rect[i].top + (iSizeY / 2);
				//
				Task.m_vDirection[i] = model.m_MTF_Direction[i]; // 0: Vertical, 1: Horizontal
				Task.m_vFrquency[i] = mTestLinePulse;//0.225

				Task.m_vOffset[i] = 0.0;
				Task.m_vSFR[i] = 0.0;

				Task.m_vThreshold[i] = 0.0;
				Task.m_vROI[i].ptCenter.x = ptROICenter[i].x;
				Task.m_vROI[i].ptCenter.y = ptROICenter[i].y;
				Task.m_vROI[i].nROIWidth = model.m_MTF_Direction[i] == 1 ? iSizeX : iSizeY;
				Task.m_vROI[i].nROIHeight = model.m_MTF_Direction[i] == 0 ? iSizeY : iSizeX;
			}
			//m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_ISO12233;	//ESFRAlgorithm_VNE
			//m_stSFRSpec.tSFRConfig.eAlgorithmMethod = ESFRMethod_Freq2SFR;
			//m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_CyclePerPixel;		//MTF
			//m_stSFRSpec.eSFRDeltaAlgorithmType = ESFRDelta_Diff;
			
			if (MandoSfrSpec.INSP_SfrDeltaAlgorithmType == 0)
			{
				m_stSFRSpec.eSFRDeltaAlgorithmType = ESFRDelta_Diff;
			}
			else if (MandoSfrSpec.INSP_SfrDeltaAlgorithmType == 1)
			{
				m_stSFRSpec.eSFRDeltaAlgorithmType = ESFRDelta_Ratio;
			}
			//
			if (MandoSfrSpec.INSP_SfrAlgorithmType == 0)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_ISO12233;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 1)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_RHOMBUS;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 2)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_LGIT_ISO;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 3)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_VNE;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 4)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_Mobis;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmType == 5)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmType = ESFRAlgorithm_Tesla_Trinity;
			}
			//
			if (MandoSfrSpec.INSP_SfrAlgorithmMethod == 0)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmMethod = ESFRMethod_Freq2SFR;
			}
			else if (MandoSfrSpec.INSP_SfrAlgorithmMethod == 1)
			{
				m_stSFRSpec.tSFRConfig.eAlgorithmMethod = ESFRMethod_SFR2Freq;
			}
			//
			if (MandoSfrSpec.INSP_SfrFrequencyUnit == 0)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_CyclePerPixel;
			}
			else if (MandoSfrSpec.INSP_SfrFrequencyUnit == 1)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_LinePairPerMilliMeter;
			}
			else if (MandoSfrSpec.INSP_SfrFrequencyUnit == 2)
			{
				m_stSFRSpec.tSFRConfig.eFrequencyUnit = ESFRFreq_LineWidthPerPictureHeight;
			}

			m_stSFRSpec.tSFRConfig.nMaxROIWidth = iSizeX;
			m_stSFRSpec.tSFRConfig.nMaxROIHeight = iSizeY;
			m_stSFRSpec.tSFRConfig.dMaxEdgeAngle = MandoSfrSpec.INSP_SfrMaxEdgeAngle;
			m_stSFRSpec.tSFRConfig.dPixelSize = model.m_dSize_CCD_Cell;// 4.2;


			//ESFRFreq_CyclePerPixel;//�Ҽ���
			//ESFRFreq_LinePairPerMilliMeter;	//�����ڸ�
			m_stSFRSpec.dEdgeDir = Task.m_vDirection.data();
			m_stSFRSpec.dFrequency = Task.m_vFrquency.data();
			m_stSFRSpec.dSFR = Task.m_vSFR.data();
			m_stSFRSpec.dThreshold = Task.m_vThreshold.data();
			m_stSFRSpec.dGamma = MandoSfrSpec.INSP_SfrGamma;
			m_stSFRSpec.tROI.dOffset = Task.m_vOffset.data();
			m_stSFRSpec.tROI.eROIType = ROIType_POINT;
			m_stSFRSpec.tROI.pROIData = Task.m_vROI.data();
			m_stSFRSpec.tROI.ROICount = model.mGlobalSmallChartCount;//MTF_INSP_CNT;
			m_stSFRSpec.tDelataSpec = NULL;
		}//1�� ���� �κ�

		if (LGIT_MODEL_INDEX == M2_FF_MODULE)
		{
			//�ݺ� ���� �Ҷ����� chart raw �̹��� ����� - �輼�� 241114
			if (sysData.m_iProductComp == 1)
			{
				MIU.func_Set_InspImageCopy(EOL_CHART, MIU.m_pFrameRawBuffer, j+1);	//================  ����ǰ ��� chart
			}
			else
			{
				if (Task.PCBTask == 65000 || Task.PCBTask == 60200)
				{
					MIU.func_Set_InspImageCopy(UV_BEFORE_CHART, MIU.m_pFrameRawBuffer, j + 1);			//========  AA UV BEFORE===
				}
				else
				{
					MIU.func_Set_InspImageCopy(UV_AFTER_CHART, MIU.m_pFrameRawBuffer, j + 1);		//========AA UV AFTER===
				}
			}
			
		}

		bool bRet = false;
		if (bMtfMaxTest)
		{
			bRet = m_pSFRProc->Inspect(ChartRawImage, nWidth, nHeight, m_stSFRSpec,
				gMIUDevice.dTDATASPEC_n.eDataFormat, gMIUDevice.dTDATASPEC_n.eOutMode,
				gMIUDevice.dTDATASPEC_n.eSensorType,
				nBlackLevel, false, false, gMIUDevice.dTDATASPEC_n.eDemosaicMethod);
		}
		else
		{
			bRet = m_pSFRProc->Inspect(MIU.m_pFrameRawBuffer, nWidth, nHeight, m_stSFRSpec,
				gMIUDevice.dTDATASPEC_n.eDataFormat, gMIUDevice.dTDATASPEC_n.eOutMode,
				gMIUDevice.dTDATASPEC_n.eSensorType,
				nBlackLevel, false, false, gMIUDevice.dTDATASPEC_n.eDemosaicMethod);
		}

		//MIU.m_pFrameRawBuffer

		int sfrMax = m_pSFRProc->GetMaxResolutionCount();
		float sfrValue = 0.0;

		double _offset = 0.0;
		for (i = 0; i < sfrMax; i++)
		{
			const TSFRROIResult* pROIResult = m_pSFRProc->GetSFRROIResult(i);
			sfrValue = pROIResult->dFinalResult[0];

			if (pROIResult)
			{
				dSFR[i] = sfrValue;

				model.m_LogSfr[i] = sfrValue;
				if (bMtfMaxTest)
				{
					if (Task.PCBTask == 65000 || Task.PCBTask == 60200)
					{
						MandoInspLog.dMTF_PreUV[i] = model.m_LogSfr[i];// dSFR[i];
					}
					else
					{
						MandoInspLog.dMTF_PostUV[i] = model.m_LogSfr[i]; //dSFR[i];

					}
					MESCommunication.m_dMesMTF[i] = model.m_LogSfr[i];
				}
				else
				{
					fMax_MTF_N4[i] += model.m_LogSfr[i];
				}
			}

		}
	}

	
	if (bMtfMaxTest == false)
	{
		for (i = 0; i < model.mGlobalSmallChartCount; i++)
		{
			fMax_MTF_N4[i] = fMax_MTF_N4[i] / iCnt_MtfFocus;

			model.m_LogSfr[i] = fMax_MTF_N4[i];

			if (Task.PCBTask == 65000 || Task.PCBTask == 60200)
			{
				MandoInspLog.dMTF_PreUV[i] = model.m_LogSfr[i];
			}
			else
			{
				MandoInspLog.dMTF_PostUV[i] = model.m_LogSfr[i];

			}
			MESCommunication.m_dMesMTF[i] = model.m_LogSfr[i];
		}

		

		
	}
	
	vision.drawOverlay(CCD);
	//CString MtfPos[9] = { "Center" , "04TL" , "04TR" , "04BL" , "04BR" , "08TL" , "08TR" , "08BL" , "08BR" };

	double dMTFTemp = 0;
	double dMTFSpecTemp = 0;
	
	double ocMinSpec = 0.0;
	double ocMaxSpec = 0.0;

	if(sysData.m_iProductComp == 1)
	{
		ocMinSpec = MandoSfrSpec.INSP_Procmode_OC_Min_Spec;
		ocMaxSpec = MandoSfrSpec.INSP_Procmode_OC_Max_Spec;
	}else
	{
		ocMinSpec = MandoSfrSpec.INSP_AAmode_OC_Min_Spec;
		ocMaxSpec = MandoSfrSpec.INSP_AAmode_OC_Max_Spec;
	}

	int sfrIndex = 0;
	
	if (Task.PCBTask == 65000)	// UV�� MTF�� ��� �Ǵܡڡڡڡڡڡڡڡڡڡ�
	{
		dMTFTemp = (MandoInspLog.dMTF_PreUV[0] + MandoInspLog.dMTF_PreUV[1] + MandoInspLog.dMTF_PreUV[2] + MandoInspLog.dMTF_PreUV[3]) / 4;
		double dCenter = (MandoInspLog.dMTF_PreUV[0] + MandoInspLog.dMTF_PreUV[1] + MandoInspLog.dMTF_PreUV[2] + MandoInspLog.dMTF_PreUV[3]) / 4;

		if (dindex == 1)
		{
			sfrIndex = 3;
		}
		//=====================================================================================================================================================================

		for (int i = 0; i<model.mGlobalSmallChartCount; i++)//for (int i = 0; i<MTF_INSP_CNT; i++)
		{
			dMTFSpecTemp = MandoSfrSpec.dAASFR_Spec[i][sfrIndex];
			if (MandoInspLog.dMTF_PreUV[i] < dMTFSpecTemp)
			{
				sTemp.Format("MTF UV BEFORE == [%d]   Spec NG: %lf (Spec:%.2f)", i, MandoInspLog.dMTF_PreUV[i], dMTFSpecTemp);
				//! Main Displayȭ�� Overlay NG List
				MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("MTF UV BEFORE == [%d]: %.3f (Spec:%.2f)", i, MandoInspLog.dMTF_PreUV[i], dMTFSpecTemp);
				MandoInspLog.iNGCnt++;
				bResult = false;
				theApp.MainDlg->putListLog(sTemp);
				MandoInspLog.sNGList += sTemp;
			}
			else
			{
				sTemp.Format("MTF UV BEFORE == [%d] %lf (Spec:%.2f)", i, MandoInspLog.dMTF_PreUV[i], dMTFSpecTemp);
				theApp.MainDlg->putListLog(sTemp);
			}
		}
		//
		if (model.m_oc_x >ocMaxSpec || model.m_oc_x < ocMinSpec)
		{
			bResult = false;
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("UVBefore_OC_X Spec:%.1f(INSP:%.2f~%.2f)", model.m_oc_x, ocMinSpec, ocMaxSpec);
			MandoInspLog.iNGCnt++;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format("[UVBefore_OC_X Spec %.1f]", model.m_oc_x);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_oc_y > ocMaxSpec || model.m_oc_y < ocMinSpec)
		{
			bResult = false;
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("UVBefore_OC_Y Spec:%.1f(INSP:%.2f~%.2f)", model.m_oc_y, ocMinSpec, ocMaxSpec);
			MandoInspLog.iNGCnt++;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format("[UVBefore_OC_Y Spec %.1f]", model.m_oc_y);
			MandoInspLog.sNGList += sTemp;
		}
		//==================================================================================================================================
	}
	else
	{ 	// UV�� MTF�� ��� �Ǵܡڡڡڡڡڡڡڡڡڡ�
		sfrIndex = 1;
		if(sysData.m_iProductComp == 1)
		{
			sfrIndex = 2;
		}
		if (dindex == 1)
		{
			sfrIndex = 3;
		}

		if (model.m_oc_x >ocMaxSpec || model.m_oc_x < ocMinSpec)
		{
			bResult = false;
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("UVAfter_OC_X Spec:%.1f(INSP:%.2f~%.2f)", model.m_oc_x, ocMinSpec, ocMaxSpec);
			MandoInspLog.iNGCnt++;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format("[UVAfter_OC_X Spec %.1f]", model.m_oc_x);
			MandoInspLog.sNGList += sTemp;
		}
		if (model.m_oc_y > ocMaxSpec || model.m_oc_y < ocMinSpec)
		{
			bResult = false;
			MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("UVAfter_OC_Y Spec:%.1f(INSP:%.2f~%.2f)", model.m_oc_y, ocMinSpec, ocMaxSpec);
			MandoInspLog.iNGCnt++;
			theApp.MainDlg->putListLog(sTemp);
			sTemp.Format("[UVAfter_OC_Y Spec %.1f]", model.m_oc_y);
			MandoInspLog.sNGList += sTemp;
		}
		//uv�� ����
		for (int i = 0; i<model.mGlobalSmallChartCount; i++)
		{
			dMTFSpecTemp = MandoSfrSpec.dAASFR_Spec[i][sfrIndex];
			if (MandoInspLog.dMTF_PostUV[i] < dMTFSpecTemp)
			{
				sTemp.Format("MTF UV AFTER [%d] Spec NG: %lf (Spec:%.2f)", i, MandoInspLog.dMTF_PostUV[i], dMTFSpecTemp);
				//! Main Displayȭ�� Overlay NG List
				MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("MTF UV AFTER [%d]: %.3f (Spec:%.2f)", i, MandoInspLog.dMTF_PostUV[i], dMTFSpecTemp);
				MandoInspLog.iNGCnt++;
				bResult = false;
				theApp.MainDlg->putListLog(sTemp);
				MandoInspLog.sNGList += sTemp;
			}
			else
			{
				sTemp.Format("MTF UV AFTER [%d] %lf (Spec:%.2f)", i, MandoInspLog.dMTF_PostUV[i], dMTFSpecTemp);
				theApp.MainDlg->putListLog(sTemp);
			}
		}

#if 0
		//SHM �� MTF Difference �˻� �߰� 231214
		int count1 = model.mCurModelName.Find(_T("SHM"));
		int count2 = model.mCurModelName.ReverseFind('SHM');
		if (count2 - count1 > 1)
		{
			double dEdgeAvr_4F[4];
			double dEdgeAvr_7F[4];
			dEdgeAvr_4F[0] = (MandoInspLog.dMTF_PostUV[4] + MandoInspLog.dMTF_PostUV[5]) / 2;
			dEdgeAvr_4F[1] = (MandoInspLog.dMTF_PostUV[6] + MandoInspLog.dMTF_PostUV[7]) / 2;
			dEdgeAvr_4F[2] = (MandoInspLog.dMTF_PostUV[8] + MandoInspLog.dMTF_PostUV[9]) / 2;
			dEdgeAvr_4F[3] = (MandoInspLog.dMTF_PostUV[10] + MandoInspLog.dMTF_PostUV[11]) / 2;
			//
			dEdgeAvr_7F[0] = (MandoInspLog.dMTF_PostUV[12] + MandoInspLog.dMTF_PostUV[13]) / 2;
			dEdgeAvr_7F[1] = (MandoInspLog.dMTF_PostUV[14] + MandoInspLog.dMTF_PostUV[15]) / 2;
			dEdgeAvr_7F[2] = (MandoInspLog.dMTF_PostUV[16] + MandoInspLog.dMTF_PostUV[17]) / 2;
			dEdgeAvr_7F[3] = (MandoInspLog.dMTF_PostUV[18] + MandoInspLog.dMTF_PostUV[19]) / 2;

			// �ִ� ã��
			//double maxValue = std::max({dEdgeAvr[0], dEdgeAvr[1], dEdgeAvr[2], dEdgeAvr[3]});

			// �ּڰ� ã��
			//double minValue = std::min({dEdgeAvr[0], dEdgeAvr[1], dEdgeAvr[2], dEdgeAvr[3]});

			

			min = *std::min_element(dEdgeAvr_4F, dEdgeAvr_4F + 4);
			max = *std::max_element(dEdgeAvr_4F, dEdgeAvr_4F + 4);
			MandoInspLog.dMtfDiff4F = max - min;

			min = *std::min_element(dEdgeAvr_7F, dEdgeAvr_7F + 4);
			max = *std::max_element(dEdgeAvr_7F, dEdgeAvr_7F + 4);
			MandoInspLog.dMtfDiff7F = max - min;
			
			if (MandoInspLog.dMtfDiff4F > MandoSfrSpec.INSP_Diff_Spec[0])
			{
				sTemp.Format("MTF Difference 4F Spec NG: %lf (Spec:%.2f)", MandoInspLog.dMtfDiff4F, MandoSfrSpec.INSP_Diff_Spec[0]);
				//! Main Displayȭ�� Overlay NG List
				MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("MTF Difference 4F : %.3f (Spec:%.2f)", MandoInspLog.dMtfDiff4F, MandoSfrSpec.INSP_Diff_Spec[0]);
				MandoInspLog.iNGCnt++;
				bResult = false;
				pFrame->putListLog(sTemp);
				MandoInspLog.sNGList += sTemp;
			}
			else
			{
				sTemp.Format("MTF Difference 4F : %lf(Spec:%.2f)", MandoInspLog.dMtfDiff4F, MandoSfrSpec.INSP_Diff_Spec[0]);
				pFrame->putListLog(sTemp);
			}
			if (MandoInspLog.dMtfDiff7F > MandoSfrSpec.INSP_Diff_Spec[1])
			{
				sTemp.Format("MTF Difference 7F Spec NG: %lf (Spec:%.2f)", MandoInspLog.dMtfDiff7F, MandoSfrSpec.INSP_Diff_Spec[1]);
				//! Main Displayȭ�� Overlay NG List
				MandoInspLog.sDispNG[MandoInspLog.iNGCnt].Format("MTF Difference 7F : %.3f (Spec:%.2f)", MandoInspLog.dMtfDiff7F, MandoSfrSpec.INSP_Diff_Spec[1]);
				MandoInspLog.iNGCnt++;
				bResult = false;
				pFrame->putListLog(sTemp);
				MandoInspLog.sNGList += sTemp;
				
			}
			else
			{
				sTemp.Format("MTF Difference 7F : %lf (Spec:%.2f)", MandoInspLog.dMtfDiff7F, MandoSfrSpec.INSP_Diff_Spec[1]);
				pFrame->putListLog(sTemp);
			}

			
		}
#endif

	}
	Task.m_bOkFlag=(bResult)?1:-1;  
	if(Task.m_bOkFlag==-1)
	{
		MandoInspLog.bInspRes = false;
		bResult=false;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
	cvReleaseImage(&cvImgMtf);
	vision.drawOverlay(CCD);

	Task.sSaveImageName="";

	g_SaveLGITLog(0, _T("Sfr"), m_pSFRProc->GetLogHeader(), m_pSFRProc->GetLogData());
	return bResult;

}


//20170214 �̹��˻� �����߰� lee
double CAABonderDlg::calcColorBalance(cv::Mat src, int nColorOrder)
{
	double ratioGG = 0.0;
	/*int nRoIsize = 100;
	int iRow=(src.rows/2)*2;
	int iCol=(src.cols/2)*2;

	cv::Mat ColorPlane0(iRow / 2, iCol / 2, CV_16UC1);
	cv::Mat ColorPlane1(iRow / 2, iCol / 2, CV_16UC1);
	cv::Mat ColorPlane2(iRow / 2, iCol / 2, CV_16UC1);
	cv::Mat ColorPlane3(iRow / 2, iCol / 2, CV_16UC1);

	for(int j = 0; j < iRow; j+=2)
	{
		for(int i = 0; i < iCol; i+=2)
		{
			ColorPlane0.at<ushort>(j/2, i/2) = src.at<ushort>(j+0, i+0);
			ColorPlane1.at<ushort>(j/2, i/2) = src.at<ushort>(j+0, i+1);
			ColorPlane2.at<ushort>(j/2, i/2) = src.at<ushort>(j+1, i+0);
			ColorPlane3.at<ushort>(j/2, i/2) = src.at<ushort>(j+1, i+1);
		}
	}
	double meanColorPlane0 = calcMeanRoI(ColorPlane0, nRoIsize);
	double meanColorPlane1 = calcMeanRoI(ColorPlane1, nRoIsize);
	double meanColorPlane2 = calcMeanRoI(ColorPlane2, nRoIsize);
	double meanColorPlane3 = calcMeanRoI(ColorPlane3, nRoIsize);
	
	double R, Gr, Gb, B;
	switch(nColorOrder)
	{
		case RGGB:
			R = meanColorPlane0;
			Gr = meanColorPlane1;
			Gb = meanColorPlane2;
			B = meanColorPlane3;
			break;
		case GRBG:
			Gr = meanColorPlane0;
			R = meanColorPlane1;
			B = meanColorPlane2;
			Gb = meanColorPlane3;
			break;
		case GBRG:
			Gb = meanColorPlane0;
			B = meanColorPlane1;
			R = meanColorPlane2;
			Gr = meanColorPlane3;
			break;
		case BGGR:
			B = meanColorPlane0;
			Gb = meanColorPlane1;
			Gr = meanColorPlane2;
			R = meanColorPlane3;
			break;

			default:
			break;
	}
	ratioGG = 100 * ( ( Gr - Gb ) / Gb );*/
	return ratioGG;
}

double CAABonderDlg::calcMeanRoI(cv::Mat src, int nRoIsize)
{
	cv::Mat RoI(src(cv::Rect(src.cols/2-nRoIsize/2, src.rows/2-nRoIsize/2, nRoIsize, nRoIsize)));
	cv::Scalar mean = cv::mean(RoI);
	return mean.val[0];
}


double CAABonderDlg::GetDistortion(LPBYTE Rgb,int m_Width, int m_Height,CPoint Center)
{
	COLORREF *m_pRgb = (COLORREF *)Rgb;

	int iMarkSize=100;

	CPoint A11,A12,B1,B2,A21,A22;

	int X=(m_Width/2)-336;
	int Y=(m_Height/2)-239;
	int CY=(m_Height/2)-181;


	A11=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x-X-iMarkSize	,Center.y-Y-iMarkSize	, Center.x-X+iMarkSize	, Center.y-Y+iMarkSize),false);
	A12=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x-X-iMarkSize	,Center.y+Y-iMarkSize	, Center.x-X+iMarkSize	, Center.y+Y+iMarkSize),false);

	B1=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x-iMarkSize		,Center.y-CY-iMarkSize	, Center.x+iMarkSize		, Center.y-CY+iMarkSize));
	B2=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x-iMarkSize		,Center.y+CY-iMarkSize	, Center.x+iMarkSize		, Center.y+CY+iMarkSize));

	A21=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x+X-iMarkSize	,Center.y-Y-iMarkSize	, Center.x+X+iMarkSize	, Center.y-Y+iMarkSize),false);
	A22=GetCirclePos((LPBYTE)m_pRgb,m_Width, m_Height,CRect(Center.x+X-iMarkSize	,Center.y+Y-iMarkSize	, Center.x+X+iMarkSize	, Center.y+Y+iMarkSize),false);

	if(A11.x== -1 || A11.y==-1 ||A12.x== -1 || A12.y==-1 ||B1.x== -1 || B1.y==-1 ||B2.x== -1 || B2.y==-1 ||A21.x== -1 || A21.y==-1 ||A22.x== -1 || A22.y==-1)
	{
		return -999;
	}

	Mark_Area(Center.x-X-iMarkSize	,Center.y-Y-iMarkSize	, Center.x-X+iMarkSize	, Center.y-Y+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);
	Mark_Area(Center.x-iMarkSize		,Center.y-CY-iMarkSize	, Center.x+iMarkSize		, Center.y-CY+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);
	Mark_Area(Center.x+X-iMarkSize	,Center.y-Y-iMarkSize	, Center.x+X+iMarkSize	, Center.y-Y+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);

	Mark_Area(Center.x-X-iMarkSize	,Center.y+Y-iMarkSize	, Center.x-X+iMarkSize	, Center.y+Y+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);
	Mark_Area(Center.x-iMarkSize		,Center.y+CY-iMarkSize	, Center.x+iMarkSize		, Center.y+CY+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);
	Mark_Area(Center.x+X-iMarkSize	,Center.y+Y-iMarkSize	, Center.x+X+iMarkSize	, Center.y+Y+iMarkSize, rgb(255,0,0), m_Width, m_Height, (LPBYTE)m_pRgb);

	double A1 = std::sqrt( std::pow((double)A11.x - (double)A12.x, 2) + std::pow((double)A11.y - (double)A12.y, 2));
	double A2 = std::sqrt( std::pow((double)A21.x - (double)A22.x, 2) + std::pow((double)A21.y - (double)A22.y, 2));
	double B = std::sqrt( std::pow((double)B1.x - (double)B2.x, 2) + std::pow((double)B1.y - (double)B2.y, 2));

	double A = (A1 + A2) / 2;
	double TVdistortion = 100 * ( A - B ) / B;


	return TVdistortion;
}

double CAABonderDlg::GetFoV(LPBYTE Rgb,int Width, int Height,CPoint Center)
{
	COLORREF *rgb_p = (COLORREF *)Rgb;

	const double cols = Width;
	const double rows = Height;
	const double pixelsize = 0.00155;;//0.0025;
	const double f = 5.018;;//25.21;
	
	CPoint cross1,cross2;
	CPoint cpCross1,cpCross2;

	int X=(Width/2)-800;
	int Y=200;
	int W=Y*2;
	
	cross1=GetCrossPos(Center.x-X-W/2,Center.y-W/2,W,(LPBYTE)rgb_p,Width, Height,Center);		
	cross2=GetCrossPos(Center.x+X-W/2,Center.y-W/2,W,(LPBYTE)rgb_p,Width, Height,Center);		

	cross1=GetCrossPos2(Center.x-X-W/2,Center.y-W/2,W,(LPBYTE)rgb_p,Width, Height,Center);		
	cross2=GetCrossPos2(Center.x+X-W/2,Center.y-W/2,W,(LPBYTE)rgb_p,Width, Height,Center);		

	if(cross1.x	== -1 || cross1.y==-1 || cross2.x==-1 || cross2.y== -1)
	{
		return -999;
	}

	//Mark_Area(Center.x-X-W/2,Center.y-Y, Center.x-X+W/2,Center.y+W/2, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);//����
	//Mark_Area(Center.x+X-W/2,Center.y-Y, Center.x+X+W/2,Center.y+W/2, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);//����

	double y1 = std::sqrt(std::pow(cross1.x - cols/2, 2) + std::pow(cross1.y - rows/2, 2));
	double y2 = std::sqrt(std::pow(cross2.x - cols/2, 2) + std::pow(cross2.y - rows/2, 2));

	double FoV = std::atan2(y1 * pixelsize, f) + std::atan2(y2 * pixelsize, f);


	//		Mark_Area(150,m_Height/2-200, 150+400, (m_Height/2-200)+400, RGB(0,0,255), Width, Height, (LPBYTE)m_pRgb);/���� �ּ�ó��

	//Mark_Area(cross1.x,cross1.y, cross1.x+10, cross1.y+10, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);//����
	//Mark_Area(cross2.x,cross2.y, cross2.x+10, cross2.y+10, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);//����
	Mark_Area(cross1.x,cross1.y, cross1.x+2, cross1.y+2, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);
	Mark_Area(cross2.x,cross2.y, cross2.x+2, cross2.y+2, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);
	return FoV;
}


CPoint CAABonderDlg::GetCrossPos(int x,int y,int size,LPBYTE Rgb,int Width, int Height,CPoint cpCenter)
{
	COLORREF *rgb_p = (COLORREF *)Rgb;
	long index;

	CPoint LT_RB[2];
	CPoint RT_LB[2];
	CPoint Center[4];

	bool bCheckFirstLT_RB=false;
	bool bCheckFirstRT_LB=false;

	int LineColor=90;
	COLORREF rgb;

	int iMax=0;
	int iMin=255;

	int iLx[4]={-1,-1,-1,-1};
	int iLy[4]={-1,-1,-1,-1};

	int MinXValue=0;
	int MinXSeq=0;
	int MinYValue=0;
	int MinYSeq=0;
	
	int r,g,b;

	for(LineColor=60;LineColor<220;LineColor+=10)
	{
		for(int i=0;i<size;i++)
		{
			// Left Top to Right Bottom
			index = (y+i) * Width + (x+i);
			rgb=*(rgb_p + index );
			r=GetRValue(rgb);	g=GetGValue(rgb);	b=GetBValue(rgb);
//			if(GetRValue(rgb) < LineColor && GetGValue(rgb) < LineColor && GetBValue(rgb) < LineColor)
			if(GetGValue(rgb) < LineColor)
			{
				if(!bCheckFirstLT_RB)
				{
					LT_RB[0]=CPoint(x+i,y+i);
					bCheckFirstLT_RB=true;
				}
				else LT_RB[1]=CPoint(x+i,y+i);
			}
			Mark_Area(x+i,y+i,x+i,y+i, RGB(0,255,0), Width, Height, (LPBYTE)rgb_p);		// �밢�� Ȯ��

			// Right Top to Left Bottom
			index = (y+i) * Width + (x+size-i);
			rgb=*(rgb_p + index );
			//			if(GetRValue(rgb) < LineColor && GetGValue(rgb) < LineColor && GetBValue(rgb) < LineColor)
			if(GetGValue(rgb) < LineColor)
			{
				if(!bCheckFirstRT_LB)
				{
					RT_LB[0]=CPoint(x+size-i,y+i);
					bCheckFirstRT_LB=true;
				}
				else RT_LB[1]=CPoint(x+size-i,y+i);
			}
			Mark_Area(x+size-i,y+i, x+size-i,y+i, RGB(0,0,255), Width, Height, (LPBYTE)rgb_p);		// �밢�� Ȯ��
		}

		iLx[0]=abs(RT_LB[0].x-LT_RB[0].x);
		iLx[1]=abs(RT_LB[0].x-LT_RB[1].x);
		iLx[2]=abs(RT_LB[1].x-LT_RB[0].x);
		iLx[3]=abs(RT_LB[1].x-LT_RB[1].x);

		iLy[0]=abs(RT_LB[0].y-LT_RB[0].y);
		iLy[1]=abs(RT_LB[0].y-LT_RB[1].y);
		iLy[2]=abs(RT_LB[1].y-LT_RB[0].y);
		iLy[3]=abs(RT_LB[1].y-LT_RB[1].y);


		Center[0].x=(RT_LB[0].x+LT_RB[0].x)/2;
		Center[1].x=(RT_LB[0].x+LT_RB[1].x)/2;
		Center[2].x=(RT_LB[1].x+LT_RB[0].x)/2;
		Center[3].x=(RT_LB[1].x+LT_RB[1].x)/2;

		Center[0].y=(RT_LB[0].y+LT_RB[0].y)/2;
		Center[1].y=(RT_LB[0].y+LT_RB[1].y)/2;
		Center[2].y=(RT_LB[1].y+LT_RB[0].y)/2;
		Center[3].y=(RT_LB[1].y+LT_RB[1].y)/2;

		MinXValue=iLx[0];
		MinXSeq=0;
		MinYValue=iLy[0];
		MinYSeq=0;

		for(int i=1;i<4;i++)
		{
			if(iLx[i] < MinXValue)
			{
				MinXValue=iLx[i];
				MinXSeq=i;
			}

			if(iLy[i] < MinYValue)
			{
				MinYValue=iLy[i];
				MinYSeq=i;
			}
		}
		if(abs(cpCenter.y - Center[MinYSeq].y) < 10)break;
	}

	return CPoint(Center[MinXSeq].x,Center[MinYSeq].y);
}
CPoint CAABonderDlg::GetCrossPos2(int x,int y,int size,LPBYTE Rgb,int Width, int Height,CPoint cpCenter)
{
	COLORREF *rgb_p = (COLORREF *)Rgb;
	long index;

	CPoint VH[2];
	CPoint VL[2];
	CPoint HL[2];
	CPoint HR[2];
	CPoint Center;

	bool bIsVH=false;
	bool bIsVL=false;
	bool bIsHL=false;
	bool bIsHR=false;

	int LineColor=160;
	COLORREF rgb;

	int iMax=0;
	int iMin=255;

	int iLx[4]={-1,-1,-1,-1};
	int iLy[4]={-1,-1,-1,-1};

	int MinXValue=0;
	int MinXSeq=0;
	int MinYValue=0;
	int MinYSeq=0;

	int r,g,b;
	int iX,iY;

	for(LineColor=60;LineColor<120;LineColor+=10)
	{
		for(int i=0;i<size;i++)
		{
			// VH
			iX=x+i;
			iY=y;
			iX=iX<Width?iX:Width-1;
			iY=iY<Height?iY:Height-1;
			index = (iY) * Width + (iX);
			rgb=rgb_p[index];
			if(GetGValue(rgb) < LineColor)
			{
				if(!bIsVH)
				{
					VH[0]=CPoint(iX,iY);
					bIsVH=true;
				}
				else VH[1]=CPoint(iX,iY);
			}
//			Mark_Area(iX,iY,iX,iY, rgb(0,100,100), Width, Height, (LPBYTE)rgb_p);		// ���μ� Ȯ��

			// VL
			iX=x+i;
			iY=y+size-1;
			iX=iX<Width?iX:Width-1;
			iY=iY<Height?iY:Height-1;
			index = (iY) * Width + (iX);
			rgb=rgb_p[index];
			g=GetGValue(rgb);
			if(GetGValue(rgb) < LineColor)
			{
				if(!bIsVL)
				{
					VL[0]=CPoint(iX,iY);
					bIsVL=true;
				}
				else VL[1]=CPoint(iX,iY);
			}
			//Mark_Area(iX,iY,iX,iY, rgb(255,0,0), Width, Height, (LPBYTE)rgb_p);		// ���μ� Ȯ��


			// HL
			iX=x;
			iY=y+i;
			iX=iX<Width?iX:Width-1;
			iY=iY<Height?iY:Height-1;
			index = (iY) * Width + (iX);
			rgb=rgb_p[index];
			if(GetGValue(rgb) < LineColor)
			{
				if(!bIsHL)
				{
					HL[0]=CPoint(iX,iY);
					bIsHL=true;
				}
				else HL[1]=CPoint(iX,iY);
			}
//			Mark_Area(iX,iY,iX,iY, rgb(0,100,100), Width, Height, (LPBYTE)rgb_p);		// ���μ� Ȯ��

			// HR
			iX=x+size-1;
			iY=y+i;
			iX=iX<Width?iX:Width-1;
			iY=iY<Height?iY:Height-1;
			index = (iY) * Width + (iX);
			rgb=rgb_p[index];
			if(GetGValue(rgb) < LineColor)
			{
				if(!bIsHR)
				{
					HR[0]=CPoint(iX,iY);
					bIsHR=true;
				}
				else HR[1]=CPoint(iX,iY);
			}
//			Mark_Area(iX,iY,iX,iY, rgb(0,100,100), Width, Height, (LPBYTE)rgb_p);		// ���μ� Ȯ��

		}

		Center=CPoint((VH[0].x+VH[1].x+VL[0].x+VL[1].x)/4,(HL[0].y+HL[1].y+HR[0].y+HR[1].y)/4);

		if(abs(cpCenter.y - Center.y) < 20)break;
	}

		Mark_Area(VH[0].x,VH[0].y,VH[1].x,VH[1].y, rgb(0,255,100), Width, Height, (LPBYTE)rgb_p);		// ã�� �� Ȯ��
		Mark_Area(VL[0].x,VL[0].y,VL[1].x,VL[1].y, rgb(0,255,100), Width, Height, (LPBYTE)rgb_p);		// ã�� �� Ȯ��
		Mark_Area(HL[0].x,HL[0].y,HL[1].x,HL[1].y, rgb(0,255,100), Width, Height, (LPBYTE)rgb_p);		// ã�� �� Ȯ��
		Mark_Area(HR[0].x,HR[0].y,HR[1].x,HR[1].y, rgb(0,255,100), Width, Height, (LPBYTE)rgb_p);		// ã�� �� Ȯ��

	return Center;
}
bool CAABonderDlg::findFiducialMark(LPBYTE Rgb, int sizeX, int sizeY, CPoint* cpFiducialPos)
{
	CString sLangChange;
	CString sLog;

	COLORREF *ucImage = (COLORREF *)Rgb;
//	long index;

	int maxSize = 400;

	int aiHistX[5000];
	int aiHistY[5000];

	CRect crFiducialRoi[4];
	crFiducialRoi[0]=CRect(1270, 280,1270+200, 280+200);
	crFiducialRoi[1]=CRect(2590, 280,2590+200, 280+200);
	crFiducialRoi[2]=CRect(1270,2580,1270+200,2580+200);
	crFiducialRoi[3]=CRect(2590,2580,2590+200,2580+200);

	int _CIRCLE_MARK_GAP=800;	// 800

	for (int i=0 ; i<4 ; i++)
	{
		if ( crFiducialRoi[i].left < 0 || crFiducialRoi[i].top < 0 || crFiducialRoi[i].right > sizeX || crFiducialRoi[i].bottom > sizeY )
		{
			sLog.Format("[��ũ #%d] ���� ��ũ �˻� ������ �߸��Ǿ����ϴ�.",i);
//			MSG_Display(sLog);
			return false;
		}

		int iSx, iSy, iEx, iEy;
		int x, y, iPos;
		int iMaxX = -1, iMinX = 9999;
		int iMaxY = -1, iMinY = 9999;
		int iMax_Left = -1 , iMax_Right = -1, iMax_Up = -1, iMax_Down = -1;

		int iMaxX2 = -1, iMinX2 = 9999;
		int iMaxY2 = -1, iMinY2 = 9999;
		int iMax_Left2 = -1 , iMax_Right2 = -1, iMax_Up2 = -1, iMax_Down2 = -1;

		int iSum;		
		int iGap;

		int iSum_Left, iSum_Right;
		int iSum_Top, iSum_Bottom;

		iSx = crFiducialRoi[i].left;
		iSy = crFiducialRoi[i].top;
		iEx = crFiducialRoi[i].right;
		iEy = crFiducialRoi[i].bottom;

		::memset(aiHistY, 0, sizeof(int)*(5000));
		::memset(aiHistX, 0, sizeof(int)*(5000));


		/* X���� ������׷� */
		for (x=iSx ; x<iEx ; x++)
		{
			iPos = iSy * sizeX + x;
			iSum = 0;

			for (y=iSy ; y<iEy ; y++)
			{
				//				iSum += (ucImage[iPos]*ucImage[iPos]);
				iSum += (ucImage[iPos]);
				iPos += sizeX;
			}

			aiHistX[x-iSx] = iSum;
		}

		/* X������� ���� ã�� */
		for (x=iSx+5 ; x<iEx-5 ; x++)
		{
			iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
			iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

			iGap = iSum_Left - iSum_Right;

			if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
			{
				iMax_Left = iGap;
				iMinX = x;
			}

			iGap = iSum_Right - iSum_Left;

			if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Right)
			{
				iMax_Right = iGap;
				iMaxX = x;
			}
		}


		iMax_Left = 0;
		iMinX2 = 0;
		int tmpSx = iMaxX-_CIRCLE_MARK_GAP;
		int tmpEx = iMinX - 30;
		if(tmpSx < (iSx+5))
			tmpSx = (iSx+5);
		if(tmpEx > (iEx-5))
			tmpEx = (iSx-5);

		for (x=tmpSx ; x<iMaxX; x++)
		{
			iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
			iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

			iGap = iSum_Left - iSum_Right;

			if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
			{
				iMax_Left = iGap;
				iMinX2 = x;
			}
		}


		iMax_Right = 0;
		iMaxX2 = 0;

		tmpSx = iMinX + 30;
		tmpEx = iMinX + _CIRCLE_MARK_GAP;
		if(tmpSx < (iSx+5))
			tmpSx = (iSx+5);
		if(tmpEx > (iEx-5))
			tmpEx = (iEx-5);


		for (x=tmpSx ; x<tmpEx; x++)
		{
			iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
			iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

			iGap = iSum_Right - iSum_Left;

			if (iGap>_CIRCLE_MARK_GAP && iGap > iMax_Right)
			{
				iMax_Right = iGap;
				iMaxX2 = x;
			}
		}


		if( iMaxX-iMinX>30 && iMaxX-iMinX<maxSize && iMinX>0)
		{
			iMinX = iMinX;
			iMaxX = iMaxX;
		}
		else if( iMaxX2-iMinX>30 && iMaxX2-iMinX<maxSize && iMinX>0 )
		{
			iMinX = iMinX;
			iMaxX = iMaxX2;
		}
		else if( iMaxX-iMinX2>30 && iMaxX-iMinX2<maxSize && iMinX2>0 )
		{
			iMinX = iMinX2;
			iMaxX = iMaxX;
		}

		/* Y ���� ������׷� */
		for (y=iSy ; y<iEy ; y++)
		{
			iPos = y * sizeX + iMinX;
			iSum = 0;

			for (x=iMinX ; x<iMaxX; x++)
			{
				iSum += ucImage[iPos++];
			}

			aiHistY[y-iSy] = iSum;
		}

		for (y=iSy+5 ; y<iEy-5 ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Top - iSum_Bottom;

			if ( iGap > iMax_Up )
			{
				iMax_Up = iGap;
				iMinY = y;
			}

			iGap = iSum_Bottom - iSum_Top;

			if ( iGap > iMax_Down )
			{
				iMax_Down = iGap;
				iMaxY = y;
			}
		}


		if ( iMinY >= iMaxY )
		{
			int iMinY2		= -9999;
			int iMaxY2		= -9999;
			int iMax_Up2	= 0;
			int iMax_Down2	= 0;

			int tmpSy = iMinY + 5;
			int tmpEy = iEy-5;

			for (y=tmpSy ; y<tmpEy ; y++)
			{
				iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
				iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

				iGap = iSum_Bottom - iSum_Top;

				if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
				{
					iMax_Down2 = iGap;
					iMaxY2 = y;
				}
			}


			tmpSy = iSy+5;
			tmpEy = iMaxY - 5;

			for (y=tmpSy ; y<tmpEy ; y++)
			{
				iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
				iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

				iGap = iSum_Top - iSum_Bottom;

				if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
				{
					iMax_Up2 = iGap;
					iMinY2 = y;
				}
			}

			if( (iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250)
			{
				iMinY = iMinY2;
			}
			else if( (iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250)
			{
				iMaxY = iMaxY2;
			}
		}
		else if( (iMaxY-iMinY) > _CIRCLE_MARK_GAP )
		{
			int iMaxY2		= -9999;
			int iMinY2		= -9999;
			int iMax_Up2	= 0;
			int iMax_Down2	= 0;

			int tmpSy = iMinY + 5;
			int tmpEy = iMaxY - 5;

			for (y=tmpSy ; y<tmpEy ; y++)
			{
				iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
				iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

				iGap = iSum_Top - iSum_Bottom;

				if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
				{
					iMax_Up2 = iGap;
					iMinY2 = y;
				}

				iGap = iSum_Bottom - iSum_Top;

				if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
				{
					iMax_Down2 = iGap;
					iMaxY2 = y;
				}
			}

			if( (iMinY2>0) && ((iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250) )
			{
				iMinY = iMinY2;
			}
			else if ( (iMaxY2>0) && ((iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250) )
			{
				iMaxY = iMaxY2;
			}
		}


		if(iMinX>iMaxX || (iMaxX-iMinX>maxSize) )
		{
			sLog.Format("[��ũ #%d] �¿� �ν� ��ġ�� ������ �Դϴ�.",i+1);
//			MSG_Display(sLog);
			return false;
		}


		if(iMinY>iMaxY || (iMaxY-iMinY>maxSize) )
		{
			sLog.Format("[��ũ #%d] �¿� �ν� ��ġ�� ������ �Դϴ�.",i+1);
//			MSG_Display(sLog);
			return false;
		}


		if ( iMaxX < 0 || iMaxY < 0 || iMaxX > sizeX || iMaxY > sizeY	||
			iMinX < 0 || iMinY < 0 || iMinX > sizeX || iMinY > sizeY )
		{
			sLog.Format("[��ũ #%d] Max, Min ��ġ�� ã�� ���߽��ϴ�.",i+1);
//			MSG_Display(sLog);
			return false;
		}
		Mark_Area(iMinX, iMinY, iMaxX, iMaxY, RGB(100,100,0), sizeX, sizeY, (LPBYTE)ucImage);

		cpFiducialPos[i].x = (iMaxX+iMinX)/2;
		cpFiducialPos[i].y = (iMaxY+iMinY)/2;
	}

	return true;
}


CPoint CAABonderDlg::GetCirclePos(LPBYTE Rgb, int sizeX, int sizeY, CRect crFiducialRoi)
{
	CString sLangChange;
	CString sLog;

	COLORREF *ucImage = (COLORREF *)Rgb;
//	long index;

	int maxSize = 400;

	int aiHistX[5000];
	int aiHistY[5000];

	crFiducialRoi.top=crFiducialRoi.top<0?0:crFiducialRoi.top;
	crFiducialRoi.left=crFiducialRoi.left<0?0:crFiducialRoi.left;

	int _CIRCLE_MARK_GAP=800;	// 800


	if ( crFiducialRoi.left < 0 || crFiducialRoi.top < 0 || crFiducialRoi.right > sizeX || crFiducialRoi.bottom > sizeY )
	{
		sLog.Format("���� ��ũ �˻� ������ �߸��Ǿ����ϴ�.");
//		MSG_Display(sLog);
		return false;
	}

	int iSx, iSy, iEx, iEy;
	int x, y, iPos;
	int iMaxX = -1, iMinX = 9999;
	int iMaxY = -1, iMinY = 9999;
	int iMax_Left = -1 , iMax_Right = -1, iMax_Up = -1, iMax_Down = -1;

	int iMaxX2 = -1, iMinX2 = 9999;
	int iMaxY2 = -1, iMinY2 = 9999;
	int iMax_Left2 = -1 , iMax_Right2 = -1, iMax_Up2 = -1, iMax_Down2 = -1;

	int iSum;		
	int iGap;

	int iSum_Left, iSum_Right;
	int iSum_Top, iSum_Bottom;

	iSx = crFiducialRoi.left;
	iSy = crFiducialRoi.top;
	iEx = crFiducialRoi.right;
	iEy = crFiducialRoi.bottom;

	::memset(aiHistY, 0, sizeof(int)*(5000));
	::memset(aiHistX, 0, sizeof(int)*(5000));


	/* X���� ������׷� */
	for (x=iSx ; x<iEx ; x++)
	{
		iPos = iSy * sizeX + x;
		iSum = 0;

		for (y=iSy ; y<iEy ; y++)
		{
			//				iSum += (ucImage[iPos]*ucImage[iPos]);
			iSum += (ucImage[iPos]);
			iPos += sizeX;
		}

		aiHistX[x-iSx] = iSum;
	}

	/* X������� ���� ã�� */
	for (x=iSx+5 ; x<iEx-5 ; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Left - iSum_Right;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
		{
			iMax_Left = iGap;
			iMinX = x;
		}

		iGap = iSum_Right - iSum_Left;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Right)
		{
			iMax_Right = iGap;
			iMaxX = x;
		}
	}


	iMax_Left = 0;
	iMinX2 = 0;
	int tmpSx = iMaxX-_CIRCLE_MARK_GAP;
	int tmpEx = iMinX - 30;
	if(tmpSx < (iSx+5))
		tmpSx = (iSx+5);
	if(tmpEx > (iEx-5))
		tmpEx = (iSx-5);

	for (x=tmpSx ; x<iMaxX; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Left - iSum_Right;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
		{
			iMax_Left = iGap;
			iMinX2 = x;
		}
	}


	iMax_Right = 0;
	iMaxX2 = 0;

	tmpSx = iMinX + 30;
	tmpEx = iMinX + _CIRCLE_MARK_GAP;
	if(tmpSx < (iSx+5))
		tmpSx = (iSx+5);
	if(tmpEx > (iEx-5))
		tmpEx = (iEx-5);


	for (x=tmpSx ; x<tmpEx; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Right - iSum_Left;

		if (iGap>_CIRCLE_MARK_GAP && iGap > iMax_Right)
		{
			iMax_Right = iGap;
			iMaxX2 = x;
		}
	}


	if( iMaxX-iMinX>30 && iMaxX-iMinX<maxSize && iMinX>0)
	{
		iMinX = iMinX;
		iMaxX = iMaxX;
	}
	else if( iMaxX2-iMinX>30 && iMaxX2-iMinX<maxSize && iMinX>0 )
	{
		iMinX = iMinX;
		iMaxX = iMaxX2;
	}
	else if( iMaxX-iMinX2>30 && iMaxX-iMinX2<maxSize && iMinX2>0 )
	{
		iMinX = iMinX2;
		iMaxX = iMaxX;
	}

	/* Y ���� ������׷� */
	for (y=iSy ; y<iEy ; y++)
	{
		iPos = y * sizeX + iMinX;
		iSum = 0;

		for (x=iMinX ; x<iMaxX; x++)
		{
			iSum += ucImage[iPos++];
		}

		aiHistY[y-iSy] = iSum;
	}

	for (y=iSy+5 ; y<iEy-5 ; y++)
	{
		iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
		iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

		iGap = iSum_Top - iSum_Bottom;

		if ( iGap > iMax_Up )
		{
			iMax_Up = iGap;
			iMinY = y;
		}

		iGap = iSum_Bottom - iSum_Top;

		if ( iGap > iMax_Down )
		{
			iMax_Down = iGap;
			iMaxY = y;
		}
	}


	if ( iMinY >= iMaxY )
	{
		int iMinY2		= -9999;
		int iMaxY2		= -9999;
		int iMax_Up2	= 0;
		int iMax_Down2	= 0;

		int tmpSy = iMinY + 5;
		int tmpEy = iEy-5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Bottom - iSum_Top;

			if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Down2 = iGap;
				iMaxY2 = y;
			}
		}


		tmpSy = iSy+5;
		tmpEy = iMaxY - 5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Top - iSum_Bottom;

			if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Up2 = iGap;
				iMinY2 = y;
			}
		}

		if( (iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250)
		{
			iMinY = iMinY2;
		}
		else if( (iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250)
		{
			iMaxY = iMaxY2;
		}
	}
	else if( (iMaxY-iMinY) > _CIRCLE_MARK_GAP )
	{
		int iMaxY2		= -9999;
		int iMinY2		= -9999;
		int iMax_Up2	= 0;
		int iMax_Down2	= 0;

		int tmpSy = iMinY + 5;
		int tmpEy = iMaxY - 5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Top - iSum_Bottom;

			if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Up2 = iGap;
				iMinY2 = y;
			}

			iGap = iSum_Bottom - iSum_Top;

			if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Down2 = iGap;
				iMaxY2 = y;
			}
		}

		if( (iMinY2>0) && ((iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250) )
		{
			iMinY = iMinY2;
		}
		else if ( (iMaxY2>0) && ((iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250) )
		{
			iMaxY = iMaxY2;
		}
	}

	Mark_Area(iMinX, iMinY, iMaxX, iMaxY, RGB(100,100,0), sizeX, sizeY, (LPBYTE)ucImage);

	return CPoint((iMaxX+iMinX)/2, (iMaxY+iMinY)/2);
}

CPoint CAABonderDlg::GetCirclePos(LPBYTE Rgb, int sizeX, int sizeY, CRect crFiducialRoi,bool bOver)
{
	CString sLangChange;
	CString sLog;

	COLORREF *ucImage = (COLORREF *)Rgb;
//	long index;

	int maxSize = 400;

	crFiducialRoi.top=crFiducialRoi.top<0?0:crFiducialRoi.top;
	crFiducialRoi.left=crFiducialRoi.left<0?0:crFiducialRoi.left;

	int aiHistX[5000];
	int aiHistY[5000];

	int _CIRCLE_MARK_GAP=800;	// 800


	if ( crFiducialRoi.left < 0 || crFiducialRoi.top < 0 || crFiducialRoi.right > sizeX || crFiducialRoi.bottom > sizeY )
	{
		sLog.Format("���� ��ũ �˻� ������ �߸��Ǿ����ϴ�.");
//		MSG_Display(sLog);
		return false;
	}

	int iSx, iSy, iEx, iEy;
	int x, y, iPos;
	int iMaxX = -1, iMinX = 9999;
	int iMaxY = -1, iMinY = 9999;
	int iMax_Left = -1 , iMax_Right = -1, iMax_Up = -1, iMax_Down = -1;

	int iMaxX2 = -1, iMinX2 = 9999;
	int iMaxY2 = -1, iMinY2 = 9999;
	int iMax_Left2 = -1 , iMax_Right2 = -1, iMax_Up2 = -1, iMax_Down2 = -1;

	int iSum;		
	int iGap;

	int iSum_Left, iSum_Right;
	int iSum_Top, iSum_Bottom;

	iSx = crFiducialRoi.left;
	iSy = crFiducialRoi.top;
	iEx = crFiducialRoi.right;
	iEy = crFiducialRoi.bottom;

	::memset(aiHistY, 0, sizeof(int)*(5000));
	::memset(aiHistX, 0, sizeof(int)*(5000));


	/* X���� ������׷� */
	for (x=iSx ; x<iEx ; x++)
	{
		iPos = iSy * sizeX + x;
		iSum = 0;

		for (y=iSy ; y<iEy ; y++)
		{
			//				iSum += (ucImage[iPos]*ucImage[iPos]);
			iSum += bOver?(ucImage[iPos]):(rgb(255,255,255)-ucImage[iPos]);
			iPos += sizeX;
		}

		aiHistX[x-iSx] = iSum;
	}

	/* X������� ���� ã�� */
	for (x=iSx+5 ; x<iEx-5 ; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Left - iSum_Right;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
		{
			iMax_Left = iGap;
			iMinX = x;
		}

		iGap = iSum_Right - iSum_Left;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Right)
		{
			iMax_Right = iGap;
			iMaxX = x;
		}
	}


	iMax_Left = 0;
	iMinX2 = 0;
	int tmpSx = iMaxX-_CIRCLE_MARK_GAP;
	int tmpEx = iMinX - 30;
	if(tmpSx < (iSx+5))
		tmpSx = (iSx+5);
	if(tmpEx > (iEx-5))
		tmpEx = (iSx-5);

	for (x=tmpSx ; x<iMaxX; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Left - iSum_Right;

		if (iGap>_CIRCLE_MARK_GAP &&  iGap > iMax_Left)
		{
			iMax_Left = iGap;
			iMinX2 = x;
		}
	}


	iMax_Right = 0;
	iMaxX2 = 0;

	tmpSx = iMinX + 30;
	tmpEx = iMinX + _CIRCLE_MARK_GAP;
	if(tmpSx < (iSx+5))
		tmpSx = (iSx+5);
	if(tmpEx > (iEx-5))
		tmpEx = (iEx-5);


	for (x=tmpSx ; x<tmpEx; x++)
	{
		iSum_Left  = aiHistX[x-iSx-4] + aiHistX[x-iSx-3] + aiHistX[x-iSx-2] + aiHistX[x-iSx-1];
		iSum_Right = aiHistX[x-iSx+4] + aiHistX[x-iSx+3] + aiHistX[x-iSx+2] + aiHistX[x-iSx+1];

		iGap = iSum_Right - iSum_Left;

		if (iGap>_CIRCLE_MARK_GAP && iGap > iMax_Right)
		{
			iMax_Right = iGap;
			iMaxX2 = x;
		}
	}


	if( iMaxX-iMinX>30 && iMaxX-iMinX<maxSize && iMinX>0)
	{
		iMinX = iMinX;
		iMaxX = iMaxX;
	}
	else if( iMaxX2-iMinX>30 && iMaxX2-iMinX<maxSize && iMinX>0 )
	{
		iMinX = iMinX;
		iMaxX = iMaxX2;
	}
	else if( iMaxX-iMinX2>30 && iMaxX-iMinX2<maxSize && iMinX2>0 )
	{
		iMinX = iMinX2;
		iMaxX = iMaxX;
	}

	/* Y ���� ������׷� */
	for (y=iSy ; y<iEy ; y++)
	{
		iPos = y * sizeX + iMinX;
		iSum = 0;

		for (x=iMinX ; x<iMaxX; x++)
		{
			iSum += bOver?(ucImage[iPos]):(rgb(255,255,255)-ucImage[iPos]);
			iPos++;
		}

		aiHistY[y-iSy] = iSum;
	}

	for (y=iSy+5 ; y<iEy-5 ; y++)
	{
		iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
		iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

		iGap = iSum_Top - iSum_Bottom;

		if ( iGap > iMax_Up )
		{
			iMax_Up = iGap;
			iMinY = y;
		}

		iGap = iSum_Bottom - iSum_Top;

		if ( iGap > iMax_Down )
		{
			iMax_Down = iGap;
			iMaxY = y;
		}
	}


	if ( iMinY >= iMaxY )
	{
		int iMinY2		= -9999;
		int iMaxY2		= -9999;
		int iMax_Up2	= 0;
		int iMax_Down2	= 0;

		int tmpSy = iMinY + 5;
		int tmpEy = iEy-5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Bottom - iSum_Top;

			if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Down2 = iGap;
				iMaxY2 = y;
			}
		}


		tmpSy = iSy+5;
		tmpEy = iMaxY - 5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Top - iSum_Bottom;

			if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Up2 = iGap;
				iMinY2 = y;
			}
		}

		if( (iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250)
		{
			iMinY = iMinY2;
		}
		else if( (iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250)
		{
			iMaxY = iMaxY2;
		}
	}
	else if( (iMaxY-iMinY) > _CIRCLE_MARK_GAP )
	{
		int iMaxY2		= -9999;
		int iMinY2		= -9999;
		int iMax_Up2	= 0;
		int iMax_Down2	= 0;

		int tmpSy = iMinY + 5;
		int tmpEy = iMaxY - 5;

		for (y=tmpSy ; y<tmpEy ; y++)
		{
			iSum_Top	= aiHistY[y-iSy-4] + aiHistY[y-iSy-3] + aiHistY[y-iSy-2] + aiHistY[y-iSy-1];
			iSum_Bottom = aiHistY[y-iSy+4] + aiHistY[y-iSy+3] + aiHistY[y-iSy+2] + aiHistY[y-iSy+1];

			iGap = iSum_Top - iSum_Bottom;

			if ( iGap > iMax_Up2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Up2 = iGap;
				iMinY2 = y;
			}

			iGap = iSum_Bottom - iSum_Top;

			if ( iGap > iMax_Down2 && (iGap>_CIRCLE_MARK_GAP) )
			{
				iMax_Down2 = iGap;
				iMaxY2 = y;
			}
		}

		if( (iMinY2>0) && ((iMaxY-iMinY2)>50 && (iMaxY-iMinY2)<250) )
		{
			iMinY = iMinY2;
		}
		else if ( (iMaxY2>0) && ((iMaxY2-iMinY)>50 && (iMaxY2-iMinY)<250) )
		{
			iMaxY = iMaxY2;
		}
	}

	Mark_Area(iMinX, iMinY, iMaxX, iMaxY, RGB(100,100,0), sizeX, sizeY, (LPBYTE)ucImage);

	return CPoint((iMaxX+iMinX)/2, (iMaxY+iMinY)/2);
}

void CAABonderDlg::Mark_Area(int x1, int y1, int x2, int y2, COLORREF color, int Width, int Height, LPBYTE Rgb)
{
	long index;
	COLORREF *rgb_p = (COLORREF *)Rgb;
	int thickness=5;
	for (int i = x1; i <= x2; i++)
	{
		for(int j=y1;j<y1+thickness;j++)
		{
			index = j * Width + i;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}

		for(int j=y2;j<y2+thickness;j++)
		{
			index = j * Width + i;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}
	}

	for (int i = y1; i <= y2; i++)
	{
		for(int j=x1;j<x1+thickness;j++)
		{
			index = Width * i + j;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}

		for(int j=x2;j<x2+thickness;j++)
		{
			index = Width * i + j;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}
	}
}

void CAABonderDlg::Mark_Cross(int x1, int y1, int x2, int y2, COLORREF color, int Width, int Height, LPBYTE Rgb)
{
	long index;
	COLORREF *rgb_p = (COLORREF *)Rgb;
	int thickness=5;

	for (int i = x1-(x2/2); i <= x1+(x2/2); i++)
	{
		for(int j=y1;j<y1+thickness;j++)
		{
			index = j * Width + i;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}
	}

	for (int i = y1-(y2/2); i <= y1+(y2/2); i++)
	{
		for(int j=x1;j<x1+thickness;j++)
		{
			index = Width * i + j;
			if (index >= 0 && index < Width * Height) *(rgb_p + index ) = color;
		}
	}
}


void CAABonderDlg::WriteRohm12Cbulk(int wRegAddr,unsigned char *wRegData,int len)
{
}


void CAABonderDlg::OnBnClickedButtonAlarm()
{
	ctrlSubDlg(ALARM_DLG);
	changeMainBtnColor(ALARM_DLG);
}
void CAABonderDlg::OnBnClickedButtonLight()
{
	//CLightDlg*			lightDlg = NULL;
	//if (m_bisLightBtn)	m_bisLightBtn = false;
	//else				m_bisLightBtn = true;

	//ctrlSubDlg(IDD_DIALOG_LIGHT);
	//changeMainBtnColor(IDD_DIALOG_LIGHT);

	//CLightDlg LightDlg;
	//LightDlg.DoModal();
	//LightDlg.ShowWindow(SW_SHOW);
	LightDlg.ShowWindow(SW_SHOW);
}

BOOL CAABonderDlg::DestroyWindow()
{
	g_ADOData.func_AA_DBDisConnect();

	return CDialogEx::DestroyWindow();
}



void CAABonderDlg::func_ChipID_Draw()
{
	CString strID="";
	strID.Format("%s", Task.ChipID);

	m_labelCCD_ID.SetText(Task.ChipID); 
	m_labelCCD_ID.Invalidate();
}

void CAABonderDlg::changeMainBtnColor(int dlg)
{
	if (dlg == m_oldDlg)
	{
		m_bMainBtn_Main.m_iStateBtn = 1;
	}
	else
	{
		m_bMainBtn_Main.m_iStateBtn =0;
		m_bMainBtn_Model.m_iStateBtn =0;
		m_bMainBtn_Align.m_iStateBtn =0;
		m_bMainBtn_CCD.m_iStateBtn =0;
		m_bMainBtn_Motor.m_iStateBtn =0;
		m_bMainBtn_IO.m_iStateBtn =0;
		m_bMainBtn_Light.m_iStateBtn =0;
		m_bMainBtn_Alarm.m_iStateBtn =0;
	}


	switch(dlg)
	{
	case MAIN_DLG:
		/*if (m_bMainBtn_Main.m_iStateBtn)
			m_bMainBtn_Main.m_iStateBtn = 0;
		else
			m_bMainBtn_Main.m_iStateBtn = 1;*/
		m_bMainBtn_Main.m_iStateBtn = 1;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;

		break;

	case MODEL_DLG:
		/*if (m_bMainBtn_Model.m_iStateBtn)
			m_bMainBtn_Model.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_Model.m_iStateBtn = 1;
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 1;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;

	case PCB_DLG:
	/*	if (m_bMainBtn_Align.m_iStateBtn)
			m_bMainBtn_Align.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_Align.m_iStateBtn = 1;
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 1;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;

	case CCD_DLG:
		/*if (m_bMainBtn_CCD.m_iStateBtn)
			m_bMainBtn_CCD.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_CCD.m_iStateBtn = 1;
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 1;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;

	case MOTOR_DLG2:
		/*if (m_bMainBtn_Motor.m_iStateBtn)
			m_bMainBtn_Motor.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_Motor.m_iStateBtn = 1;
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 1;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;

	case IO_DLG:
		/*if (m_bMainBtn_IO.m_iStateBtn)
			m_bMainBtn_IO.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_IO.m_iStateBtn = 1;
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 1;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;
	case IDD_DIALOG_LIGHT:
	/*	if (m_bMainBtn_Light.m_iStateBtn)
			m_bMainBtn_Light.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_Light.m_iStateBtn = 1;		
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 1;
		m_bMainBtn_Alarm.m_iStateBtn = 0;
		break;
	case ALARM_DLG:
		/*if (m_bMainBtn_Alarm.m_iStateBtn)
			m_bMainBtn_Alarm.m_iStateBtn = 0;
		else
		{
			m_bMainBtn_Alarm.m_iStateBtn = 1;		
			m_bMainBtn_Main.m_iStateBtn = 0;
		}*/
		m_bMainBtn_Main.m_iStateBtn = 0;
		m_bMainBtn_Model.m_iStateBtn = 0;
		m_bMainBtn_Align.m_iStateBtn = 0;
		m_bMainBtn_CCD.m_iStateBtn = 0;
		m_bMainBtn_Motor.m_iStateBtn = 0;
		m_bMainBtn_IO.m_iStateBtn = 0;
		m_bMainBtn_Light.m_iStateBtn = 0;
		m_bMainBtn_Alarm.m_iStateBtn = 1;
		break;

		
	}

	m_bMainBtn_Main.Invalidate();
	m_bMainBtn_Model.Invalidate();
	m_bMainBtn_Align.Invalidate();
	m_bMainBtn_CCD.Invalidate();
	m_bMainBtn_Motor.Invalidate();
	m_bMainBtn_IO.Invalidate();
	m_bMainBtn_Alarm.Invalidate();
	m_bMainBtn_Light.Invalidate();

	m_oldDlg = dlg;
}

void CAABonderDlg::OnStnClickedLabelStatusServo()
{
	Dio.setAlarm(ALARM_ON);
}

bool CAABonderDlg::MIUCheck_process()
{
	CString logStr=""; 

	/*if(gMIUDevice.CurrentState !=0) 
	{
		MIU.Stop();
	}*/

	if (MIU.m_pBoard->IsGrabStarted())		//if ( gMIUDevice.CurrentState >= 2 )
	{
		MIU.Stop();
		Sleep(300);
		MIU.Close();
		Sleep(500);
	}

	Sleep(100);
	double ep = myTimer(true);

	if(m_bMiuRun)
	{
		logStr.Format("[ CCD ] ���� ���Դϴ�.");
		putListLog(logStr);
		delayMsg(logStr.GetBuffer(100), 3000, M_COLOR_RED);
		m_bMiuRun = false;
		return false ;
	}
	Sleep(300);
	if(gMIUDevice.bMIUOpen==0)		//if(!MIU.m_pBoard->IsConnected()) 
	{
		if(!MIU.Open())
		{
			m_bMiuRun = false;
			theApp.MainDlg->putListLog("	MIU Open ����.");
			return false;
		}
	}
	Sleep(300);
	double time3 = myTimer(true);
	if(!MIU.Start())
	{ 
		m_bMiuRun = false;
		logStr.Format("MIU Start ����.");
		putListLog(logStr);
		LogSave(logStr);

		return false;
	}
	else
	{
		if (!bThreadCcmGrabRun)
		{
			CcmThreadStart();
		}
	}

	double time4 = myTimer(true);
	logStr.Format("MIU Start �Ϸ�.");//logStr.Format("Start �Ϸ�. [%.0f]", time4-time3);
	putListLog(logStr);

	gMIUDevice.CurrentState = 4;
	m_bMiuRun = true;
	return true;
}

int CAABonderDlg::NG_process(int iStep)
{
	CString sLog;
	int iRtnFunction = iStep;

	switch(iStep)
	{
	case  150000:

		if(Task.m_bNgUnload == true)
		{
			if(Task.m_bOkUnload == true)
				Task.m_bOkFlag = 1;
			else
				Task.m_bOkFlag = -1;

			iRtnFunction = 150100;

			m_btnNgOut.m_iStateBtn = 2;
			m_btnNgOut.Invalidate();
		}
		else
		{
			Task.m_bOkFlag = 0;
			m_btnNgOut.m_iStateBtn = 0;
			m_btnNgOut.Invalidate();

			iRtnFunction = -(Task.PausePCBStep);
			////Task.LensTask = -(Task.PauseLensStep);
		}
		break;

	case  150100:
		{
			double posLensX = fabs(motor.GetEncoderPos(Motor_Lens_X)-model.axis[Motor_Lens_X].pos[Bonding_Pos]);
			double posLensY = fabs(motor.GetEncoderPos(Motor_Lens_Y)-model.axis[Motor_Lens_Y].pos[Bonding_Pos]);
			double posLensZ = fabs(motor.GetCommandPos(Motor_Lens_Z)-model.axis[Motor_Lens_Z].pos[Bonding_Pos]);

			double posPcbX = fabs(motor.GetEncoderPos(Motor_PCB_X)-model.axis[Motor_PCB_X].pos[Bonding_Pos]);
			double posPcbY = fabs(motor.GetEncoderPos(Motor_PCB_Y)-model.axis[Motor_PCB_Y].pos[Bonding_Pos]);

			if ( (posLensX<3 && posLensY<3 && posLensZ<2 && posPcbX<3 && posPcbY<3)
				&& (Task.LensTask >= 30000) )
			{
				if(!/*gPassUV*/sysData.m_iUVPass == 1 && Task.m_bUvPassUnload == true)
				{// UV Shot -> Grip Open -> Z�� ���->Lens ��� ��ġ->PCB ����-> NG ��û Step���� �̵�
					iRtnFunction = 110000;
				}
				else
				{// Grip Open -> Z�� ���->Lens ��� ��ġ->PCB ����-> NG ��û Step���� �̵�
					iRtnFunction = 115000;
				}
			}
			else
			{
				Task.m_iRetry_Opt = 0;

				if(posLensX<3 && posLensY<3 && posLensZ<3)			// Z�� ���->Lens ��� ��ġ->PCB ����-> NG ��û Step���� �̵�
					iRtnFunction = 120000;
 				else
 					iRtnFunction = 121000;							// Z�� ��� Pass �ϰ� PCB ����-> NG ��û Step���� �̵�
			}
		}

		break;
	}

	return iRtnFunction;
}


void CAABonderDlg::OnBnClickedButtonNgOut()
{
	if(sysData.m_FreeRun == 1)
	{
		sLangChange.LoadStringA(IDS_STRING234);
		errMsg2(Task.AutoFlag, sLangChange);
		return;
	}


	if(Task.AutoFlag == 1)
	{
		sLangChange.LoadStringA(IDS_STRING364);
		putListLog(sLangChange);
		return;
	}


#ifdef	ON_LINE_MODE
	if(Task.AutoReday == 0 && Task.AutoFlag == 0)
	{
		sLangChange.LoadStringA(IDS_STRING1326);	//���� �غ� ���� �ʾҽ��ϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return;
	}




	if(Dio.PCBvaccumOnCheck(true, false) == false)
	{
		errMsg2(Task.AutoFlag, "PCB ���� ������ ���� �߽��ϴ�.");
		return;
	}
#endif

}


void CAABonderDlg::dispLotName()
{
	CString tmpStr;
	tmpStr.Format("%s", Task.LotNo);
	m_labelLotName.SetText(tmpStr);
}


void CAABonderDlg::OnBnClickedButtonPcbResult()
{
	if(Task.AutoFlag==1)
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_GREEN);
	}
	else if(Task.AutoFlag==0 || Task.AutoFlag==2)
	{
		if(Task.PcbOnStage==200)
		{
			Task.PcbOnStage = 1;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1299);	//��� �������� �Է� �ϼ���. [��ǰ Yes / �ҷ� No]
			if(askMsg(sLangChange))
				Task.m_bOkFlag = 1;
			else
				Task.m_bOkFlag = -1;

			Task.PcbOnStage = 200;
		}
	}
}

int CAABonderDlg::freeRun()
{
	
	return true;
}


void CAABonderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 999)
	{
		sLangChange.LoadStringA(IDS_STRING1404);
		if(Task.PcbOnStage == 200){
			GetDlgItem(IDC_BUTTON_PCB_RESULT)->SetWindowText(sLangChange);
			m_bPcbFinish.m_iStateBtn = 1;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1403);
			GetDlgItem(IDC_BUTTON_PCB_RESULT)->SetWindowText(sLangChange);
			m_bPcbFinish.m_iStateBtn = 0;	
		}
		m_bPcbFinish.Invalidate();

		if(Task.m_bOkDispense == 1){
			sLangChange.LoadStringA(IDS_STRING1200);
			GetDlgItem(IDC_BUTTON_DISPENSE_RESULT)->SetWindowText(sLangChange);
			m_bDispenseFinish.m_iStateBtn = 1;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1198);
			GetDlgItem(IDC_BUTTON_DISPENSE_RESULT)->SetWindowText(sLangChange);
			m_bDispenseFinish.m_iStateBtn = 0;
		}
		m_bDispenseFinish.Invalidate();

		if(Task.m_bOKLensPass == 1){
			sLangChange.LoadStringA(IDS_STRING717);
			GetDlgItem(IDC_BUTTON_LENS_PASS_RESULT)->SetWindowText(sLangChange);
			m_bLensPassFinish.m_iStateBtn = 1;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING716);
			GetDlgItem(IDC_BUTTON_LENS_PASS_RESULT)->SetWindowText(sLangChange);
			m_bLensPassFinish.m_iStateBtn = 0;
		}
		m_bLensPassFinish.Invalidate();

		
		if (sysData.m_iProductHolderBoning == 1) {
			m_bProHolderBondingCheck.m_iStateBtn = 1;
		}
		else
		{
			m_bProHolderBondingCheck.m_iStateBtn = 0;
		}
		m_bProHolderBondingCheck.Invalidate();



		if(sysData.m_iProductComp == 1){
			m_bProCompCheck.m_iStateBtn = 1;
		}
		else
		{
			m_bProCompCheck.m_iStateBtn = 0;
		}
		m_bProCompCheck.Invalidate();

		if(bThreadTaskReadyRun == true)
		{
			if(iReadyRunCnt%4<2)
				m_btnReady.m_iStateBtn = 0;
			else
				m_btnReady.m_iStateBtn = 1;

			m_btnReady.Invalidate();

			iReadyRunCnt++;
		}

		if(Task.AutoReday==1)
		{
			double pcbX = fabs(motor.GetEncoderPos(Motor_PCB_X) - model.axis[Motor_PCB_X].pos[Wait_Pos]);
			double pcbY = fabs(motor.GetEncoderPos(Motor_PCB_Y) - model.axis[Motor_PCB_Y].pos[Wait_Pos]);
			double LensX = fabs(motor.GetEncoderPos(Motor_Lens_X) - model.axis[Motor_Lens_X].pos[Wait_Pos]);
			double LensY = fabs(motor.GetEncoderPos(Motor_Lens_Y) - model.axis[Motor_Lens_Y].pos[Wait_Pos]);
			double LensZ = fabs(motor.GetCommandPos(Motor_Lens_Z) - model.axis[Motor_Lens_Z].pos[Wait_Pos]);

			if(pcbX>0.5 || pcbY>0.5 || LensX>0.5 || LensY>0.5 || LensZ>0.5)
			{
				Task.AutoReday = 0;

				m_btnReady.m_iStateBtn = 0;
				m_btnReady.Invalidate();
			}	
		}
		else if(alarmDlg != NULL)
		{
			if(g_AlarmFlag)				// �˶� ���� ��..
			{
				if (alarmDlg->IsWindowVisible())
				{
					if(alarmDlg->m_iAlarmKind==1)
						alarmDlg->m_btnAlarmMonitor.m_iStateBtn = 1;
					else
						alarmDlg->m_btnAlarmMonitor.m_iStateBtn = 0;

					m_bMainBtn_Alarm.m_iStateBtn = 1;
				}
				else
					m_bMainBtn_Alarm.m_iStateBtn = 0;

				g_AlarmCnt = 0;
			}
			else
			{
				if((g_AlarmCnt%4) < 2)
				{
					m_bMainBtn_Alarm.m_iStateBtn = 2;
					alarmDlg->m_btnAlarmMonitor.m_iStateBtn = 2;
				}
				else
				{
					if (alarmDlg->IsWindowVisible())
					{
						m_bMainBtn_Alarm.m_iStateBtn = 1;

						if(alarmDlg->m_iAlarmKind==1)
							alarmDlg->m_btnAlarmMonitor.m_iStateBtn = 1;
						else
							alarmDlg->m_btnAlarmMonitor.m_iStateBtn = 0;

						alarmDlg->m_btnAlarmMonitor.Invalidate();
					}
					else
						m_bMainBtn_Alarm.m_iStateBtn = 0;
				}

				g_AlarmCnt++;
			}

			alarmDlg->m_btnAlarmMonitor.Invalidate();
			m_bMainBtn_Alarm.Invalidate();

		}
	}

	if(nIDEvent == 9)
	{
		func_SocketControl_ConnectCheck();
	}

		motor.InDIO(0, curInDio[0]);
		motor.InDIO(2, curInDio[1]);

	CDialogEx::OnTimer(nIDEvent);
}


bool CAABonderDlg::MoveOffset_Prev_UV()
{
#if (____AA_WAY == PCB_TILT_AA)
	short axis[6] = {Motor_PCB_X, Motor_PCB_Y, Motor_PCB_Z, Motor_PCB_Xt, Motor_PCB_Yt, Motor_PCB_TH};
#elif (____AA_WAY == LENS_TILT_AA)
	short axis[6] = {Motor_Lens_X, Motor_Lens_Y, Motor_Lens_Z, Motor_Lens_Xt, Motor_Lens_Yt, Motor_PCB_TH};
#endif
	
	double dDes_Pos[6] = {0.0, };
	double offSetZ=0.0;

	//Task.SFR.dMaxPos[1~4]

	//for(int i = 1; i < 5; i++)
	//{
		//offSetZ += Task.SFR.dMaxPos[i];
	//}

	//offSetZ = Task.SFR.dMaxPos[0]-(offSetZ/4);
	/*Task.dUvOffset_Aver = offSetZ * model.UV_Weight*-1;  

	if(Task.dUvOffset_Aver < (sysData.m_dOffset_Prev_UV_Z/2))
	{
		Task.dUvOffset_Aver = sysData.m_dOffset_Prev_UV_Z/2;
	}
	
	if(Task.dUvOffset_Aver > fabs(sysData.m_dOffset_Prev_UV_Z/2))
	{
		Task.dUvOffset_Aver = fabs(sysData.m_dOffset_Prev_UV_Z/2);
	}*/

	//CString logStr;
	//logStr.Format("Uv Weight: %.3lf" , Task.dUvOffset_Aver);
	//putListLog(logStr);

	dDes_Pos[0] = motor.GetEncoderPos(axis[0]) + sysData.m_dOffset_Prev_UV_X;
	dDes_Pos[1] = motor.GetEncoderPos(axis[1]) + sysData.m_dOffset_Prev_UV_Y;
	//

	dDes_Pos[2] = motor.GetCommandPos(axis[2]) + sysData.m_dOffset_Prev_UV_Z;	// + Task.dUvOffset_Aver;

	
	//
	dDes_Pos[3] = motor.GetCommandPos(axis[3]) + sysData.m_dOffset_Prev_UV_Tx;
	dDes_Pos[4] = motor.GetCommandPos(axis[4]) + sysData.m_dOffset_Prev_UV_Ty;
	dDes_Pos[5] = motor.GetCommandPos(axis[5]) + sysData.m_dOffset_Prev_UV_Th;
	//
	motor.goMotorPos(6, axis, dDes_Pos, true);

	double iTime = myTimer(true);

	while (1)
	{
		if (myTimer(true) - iTime < 5000)//MOTOR_MOVE_TIME)
		{
			if (motor.IsStopAxis(axis[0]) == true	&&
				motor.IsStopAxis(axis[1]) == true	&&
				motor.IsStopAxis(axis[2]) == true	&&
				motor.IsStopAxis(axis[3]) == true	&&
				motor.IsStopAxis(axis[4]) == true	&&
				motor.IsStopAxis(axis[5]) == true	)
				break;
		}
		else
		{
			return false;
		}
	}

	return true;
}


void CAABonderDlg::OnClickedLabelTitle()
{
	if (Task.AutoFlag == 1) 
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_RED);
		return;
	}
	//theApp.MainDlg->SendPacketToBarcode(true);
	//Dio.DoorLift(false, false);
	//Dio.DoorPBLampOn(true);

	//UVCommand.Connect_Device(sysData.iCommPort[COMM_UV]);
	//UVCommand.UV_Shutter_PowerSet(95);//
	//vision.bmpImageSaveFn(0, 0);
	
#ifdef NORINDA_MODE
	CString str;
	g_SaveLGITLog(0, "RI", NULL, NULL);
	//theApp.MainDlg->SendPacketToBarcode(true);
	//theApp.MainDlg->_getMTF(SFR_FINAL);
	//sLangChange.LoadStringA(IDS_STRING1453);
	//LogSave(sLangChange);		//"AMP Alarm �߻�. MoveAxis ���� ����."
	//g_SaveLGITLog(0, "RI", NULL, NULL);
	//Jpg_ImageSave(NULL, MTF_JPG);
	//MIU.RawImageSave(str, NULL);
	/*MandoInspLog.func_LogSave_UVAfter(0);
	MandoInspLog.func_LogSave_UVBefore();	
	saveSfrLog(SFR_STEP);
	saveInspImage(LENS_IMAGE_SAVE, Task.m_iRetry_Opt);
	AlignResultSave(str);*/
	/*int kkk = 10;
	LightControl.ChartAllControl(true);
	byte value = static_cast<unsigned char>(kkk);*/

	_stprintf_s(Task.ChipID, sizeof(Task.ChipID), _T("EMPTY"));

	return;
	//CDPoint cpLeftPos;
	//CDPoint cpRighttPos;
	//bool bCircleFind = vision.FnShmEdgeFind(MIU.m_pFrameRawBuffer, false);
	//MandoInspLog.func_LogSave_Shm_vertex();
	//g_FinalInspLog();
	//AlignResultSave("1");
	//Ÿ��Ʋ

	//std::string testModel = "SHM";
	////const TCHAR* targetString = _T("SHM");
	//if (_tcscmp(model.mCurModelName, _T("SHM")) == 0)
	//const TCHAR* result = _tcsstr(model.mCurModelName, targetString);
	//int count1 = model.mCurModelName.Find(_T("SHM"));
	//int count2 = model.mCurModelName.ReverseFind('SHM');
	//int matchCount = std::strncmp(model.mCurModelName, _T("SHM"), model.mCurModelName.GetAllocLength());
	/*if (count2 - count1 > 2)
	{
		return;
	}
	else
	{
		return;
	}*/

#endif
	

	//Dio.setAlarm(ALARM_OFF);
}

void CAABonderDlg::DeleteOldData(int year, int month, int day)
{
	CString tmpStr;

	int deleteYear		= year;
	int deleteMonth		;
	int deleteDay		= day;

	if(sysData.m_Log_Retention_Period > 0)
	{
		deleteMonth		= month-sysData.m_Log_Retention_Period;
		while(deleteMonth < -12)
		{
			deleteMonth+=12;
			deleteYear--;
		}
	}
	else
	{
		deleteMonth		= month-1;
	}

	if(deleteMonth<1)
	{
		deleteMonth		= 12;
		deleteYear		= year-1;
	}

	int deleteYear2		= year;
	int deleteMonth2	= month-2;

	if(deleteMonth==0)
	{
		deleteMonth2	= 12;
		deleteYear2		= year-1;
	}
	else if(deleteMonth<0)
	{
		deleteMonth2	= 11;
		deleteYear2		= year-1;
	}


	CFileFind finder;
	BOOL IsFind;

	//tmpStr.Format("%s\\%04d%02d\\%02d", ALARM_DIR, deleteYear, deleteMonth, deleteDay);
	//IsFind = finder.FindFile(tmpStr);
	//if(IsFind)
	//{
	//	DeletePath(tmpStr);
	//	RemoveDirectory(tmpStr);
	//	return;
	//}
	
}

void CAABonderDlg::SetDigReference(int iMark)
{//Bright, Contrast ����

	long chRef;
	
	//if(iMark==0)			chRef = M_CH0_REF;
	//else if(iMark==1)		chRef = M_CH1_REF;
	if(iMark == PCB_Chip_MARK)				chRef = M_CH0_REF;	//CAM1
	else if(iMark == PCB_Holder_MARK)		chRef = M_CH0_REF;	//CAM1
	else if(iMark == LENS_Align_MARK)		chRef = M_CH1_REF;	//CAM2

#ifdef ON_LINE_VISION
	if(vision.MilDigitizer)
	{
		MdigReference(vision.MilDigitizer, M_BLACK_REF+chRef,0);//model.m_iBright[iMark]);
		MdigReference(vision.MilDigitizer, M_WHITE_REF+chRef, 0);//model.m_iContrast[iMark]);
	}
#endif
	//UpdateData(false);
}


void CAABonderDlg::InstantMarkDelete(int iMarkType)
{
	/*if((model.m_MarkSize[iMarkType][1].x == 0)|(model.m_MarkSize[iMarkType][1].y == 0))
		return;

	model.m_MarkCenter[iMarkType][1].x	=	0.0f;
	model.m_MarkCenter[iMarkType][1].y	=	0.0f;
	model.m_MarkSize[iMarkType][1].x	=	0;
	model.m_MarkSize[iMarkType][1].y	=	0;
	model.m_iLimitRate[iMarkType][1]	=	70;

	CString sDelFile;

#ifdef USE_GEOMETRIC
	MmodFree(vision.ModModel[iMarkType][1]);
	vision.ModModel[iMarkType][1] = M_NULL;
	
	if(iMarkType == PCB_Chip_MARK)			sDelFile.Format("%s\\Model\\%s\\LENS_Mark_%d.mod", DATA_DIR, model.mCurModelName, 1);
	else if(iMarkType == PCB_Holder_MARK)	sDelFile.Format("%s\\Model\\%s\\PCB_Mark_%d.mod", DATA_DIR, model.mCurModelName, 1);
	else									sDelFile.Format("%s\\Model\\%s\\LENS_Align_MARK_%d.mod", DATA_DIR, model.mCurModelName, 1);
#else
	MpatFree(vision.PatModel[iMarkType][m_iMarkNo]);
	vision.PatModel[iMarkType][m_iMarkNo] = M_NULL;

	if(iMarkType == PCB_Chip_MARK)			sDelFile.Format("%s\\Model\\%s\\LENS_Mark_%d.pat", DATA_DIR, model.mCurModelName, 1);
	else if(iMarkType == PCB_Holder_MARK)	sDelFile.Format("%s\\Model\\%s\\PCB_Mark_%d.pat", DATA_DIR, model.mCurModelName, 1);
	else									sDelFile.Format("%s\\Model\\%s\\LENS_Align_MARK_%d.pat", DATA_DIR, model.mCurModelName, 1);
#endif

	::DeleteFile(sDelFile);

	model.Save();*/
}

void CAABonderDlg::OnStnClickedLabelCcdRetry()
{
		CString logStr;
		sLangChange.LoadStringA(IDS_STRING1203);	//������ ��õ� Ƚ���� �ʱ�ȭ ��Ű�ڽ��ϱ�?
	if(askMsg(sLangChange))
	{
		work.m_iCoverUpDownCnt = 0;
	}

	logStr.Format("%d", work.m_iCoverUpDownCnt);
	m_labelCcdRetryCnt.SetText(logStr);
	m_labelCcdRetryCnt.Invalidate();
}


int CAABonderDlg::TiltAlignLimitCheck(double dOffsetX, double dOffsetY)										// [Insptype] 0:Lens PreAlign 1:PCB PreAlign
{								
													// Return	0:NG,  1:Retry,  2:OK
	if (Task.PCBTask == 27250  || Task.PCBTask == 13100)
	{
		if (fabs(dOffsetX)  > model.axis[Motor_PCB_Xt].m_dLimit_Err || fabs(dOffsetY)  > model.axis[Motor_PCB_Yt].m_dLimit_Err) {
			return 0;
		}
		if (fabs(dOffsetX)  > model.axis[Motor_PCB_Xt].m_dLimit_OK || fabs(dOffsetY)  > model.axis[Motor_PCB_Yt].m_dLimit_OK) {
			return 1;
		}
	}
	else
	{
		if (fabs(dOffsetX)  > model.axis[Motor_Lens_Xt].m_dLimit_Err || fabs(dOffsetY)  > model.axis[Motor_Lens_Yt].m_dLimit_Err) {
			return 0;
		}
		if (fabs(dOffsetX)  > model.axis[Motor_Lens_Xt].m_dLimit_OK || fabs(dOffsetY)  > model.axis[Motor_Lens_Yt].m_dLimit_OK) {
			return 1;
		}
	}
	
	return 2; 
}

void CAABonderDlg::OnBnClickedButtonDispenseResult()
{
	CString logStr="";
	if(Task.AutoFlag==1)
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_RED);
	}
	else if(Task.AutoFlag==0 || Task.AutoFlag==2)
	{	
		logStr.Format("���� �Ϸ� ���� �����ϼ���! \n�Ϸ� = ��(Y) / �̿Ϸ� = �ƴϿ�(N)");	//���� �Ϸ� ���θ� �Է� �ϼ���. \n[�Ϸ� ���� Yes / �̿Ϸ� ���� No]
		if(askMsg(logStr)){
			Task.m_bOkDispense = 1;
		}else{
			Task.m_bOkDispense = -1;
		}
	}
}

bool CAABonderDlg::func_MIU_ConnectLiveCheck()
{
	if( gMIUDevice.CurrentState != 4)
	{	
		sLangChange.LoadStringA(IDS_STRING1370);	//�ڵ� ���� �� CCD ������ ���� ȹ�� ����
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}
	return true;
}

bool CAABonderDlg::func_Check_LaserValueErr(double dVal[4])
{
	//20150617
	//for(int iNo = 1; iNo < 4; iNo++)
	//{
	//	if( dVal[0] != dVal[1])	 return true;	//�ϳ��� �ٸ��� OK
	//}

	//return false;
	return true;
}

void CAABonderDlg::showLanConnect()
{
}

bool CAABonderDlg::ConnectToServer()
{
	CString sLog;
#ifdef		ON_LINE_LAN
	if ( m_Socket != NULL || m_SocketMes != NULL)
	{
		if(m_Socket != NULL)
		{
			m_Socket.ShutDown();
			m_Socket.Close();
		}
		else
		{
			m_SocketMes.ShutDown();
			m_SocketMes.Close();
		}
	}

	if( !m_Socket.Create() || !m_SocketMes.Create())
	{
		sLangChange.LoadStringA(IDS_STRING1085);	//Socket ���� ����
		sLog.Format(sLangChange);
		putListLog(sLog);
		return false;
	}

	//SetTimer(10, 5000, NULL);

	CString sIPAdd;
	sIPAdd.Format("%s", SOCKET_MAIN);
	
	if( m_Socket.Connect(sIPAdd, 21000) == FALSE )
	{
		sLangChange.LoadStringA(IDS_STRING1276);	//������ ���� ����
		sLog.Format(sLangChange);
		putListLog(sLog);
		//KillTimer(10);
		return false;
	}
	//else
	//{
	//	KillTimer(10);
	//}
#endif

	return true;
}


bool CAABonderDlg::CheckMessage(CString sMsg)
{
	// 	int iIndex = atoi(sMsg.Right(1));
	// 	if ( iIndex != 1)		return false;
	int startIndex, endIndex;
	CString sTemp, logStr, sSendData ;//&%s$
	int stxIndex = sMsg.Find("&", 0);
	int etxIndex = sMsg.Find("%", 0);
	sTemp		= sMsg.Mid(stxIndex+1, etxIndex - stxIndex -1);	
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if(stxIndex<0 || etxIndex<0 || stxIndex>etxIndex)
	{
		sLangChange.LoadStringA(IDS_STRING1337);	//�̴��� Data ���� �̻�
		sTemp.Format(sLangChange + _T(" - %s\n"), sMsg);
		errMsg2(Task.AutoFlag, sTemp);
		return false;
	}

	startIndex	= sMsg.Find("$", 0);
	endIndex	= sMsg.Find("_", startIndex+1);
	sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
	
	//sMsg.Format("[���� -> Ŭ���̾�Ʈ] %s", sTemp);
	//putListLog(sMsg);

	if( lstrcmp(sTemp, "GETTILT") == 0 )
	{//GETTILT_���ڵ��Ī
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
		
		if( !g_ADOData.func_Get_RecordData(sTemp) )	//���ڵ� ������ DB �˻�
		{
			sSendData = ("NAK");
			SendMessageToServer(sSendData);
		}
		else
		{
			sSendData = ("ACK");
			SendMessageToServer(sSendData);
		}
	}
	else if( lstrcmp(sTemp, "GETDATA") == 0)
	{
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);

		if( g_ADOData.func_Get_RecordData(sTemp) == true )	//���ڵ� ������ DB �˻�
		{
			sSendData.Format("ALIGNX_%.04lf", g_ADOData.adoRegist.dOffset_Align[0]);
			SendMessageToServer(sSendData);
			Sleep(200);
			sSendData.Format("ALIGNY_%.04lf", g_ADOData.adoRegist.dOffset_Align[1]);
			SendMessageToServer(sSendData);
			Sleep(200);
			sSendData.Format("ALIGNT_%.04lf", g_ADOData.adoRegist.dOffset_Align[2]);
			SendMessageToServer(sSendData);
			Sleep(200);
			sSendData.Format("TILTX_%.04lf", g_ADOData.adoRegist.dOffset_TiltTx);
			SendMessageToServer(sSendData);
			Sleep(200);
			sSendData.Format("TILTY_%.04lf", g_ADOData.adoRegist.dOffset_TiltTy);
			SendMessageToServer(sSendData);
		}
		else
		{
			sSendData = ("NAK");
			SendMessageToServer(sSendData);
		}
	}
	else if( lstrcmp(sTemp, "NAK") == 0 )
	{
		sSendData = ("NAK");
		SendMessageToServer(sSendData);
	}
	return true;
}


bool CAABonderDlg::SendMessageToServer(CString sMsg)
{
	CString sLog;
#ifndef		ON_LINE_LAN
	return true;
#endif

	if(!m_bisConnect)
	{
		sLangChange.LoadStringA(IDS_STRING562);
		delayMsg(sLangChange, 3000, M_COLOR_RED);	//Ether-Net ���� ���� �����Դϴ�
		return false;
	}

	CString sHead;
	sHead.Format("&AA%d$", sysData.m_iUnitNo);
	CString sEnd = "%";
	CString sSendMsg = sHead + sMsg +sEnd;

	if(sMsg.GetLength()==0)
	{
		return false;
	}

	int iRtn;
	int retry_cnt = 5;

	m_csLock_Socket.Lock();
	sLangChange.LoadStringA(IDS_STRING212);
	sLog.Format(sLangChange, sMsg);
	putListLog(sLog);

	Sleep(10);
	for(int i=0; i<retry_cnt; i++)
	{
		iRtn = m_Socket.Send(sSendMsg, sSendMsg.GetLength() * 2);
		SocketDataSave(SEND_DATA, sMsg);

		if ( iRtn >= 0 )
			break;

		for(int j=0; j<100; j++)
		{
			Sleep(1);
			checkMessage();
		}
	}

	m_csLock_Socket.Unlock();

	if ( iRtn < 0 )
	{
		CString sLog;
		sLangChange.LoadStringA(IDS_STRING1275);	//������ �޽��� ���� ����.
		sLog.Format(sLangChange + _T(" [%s]"), sSendMsg);
		delayMsg(sLog.GetBuffer(99), 3000, M_COLOR_RED);
		return false;
	}

	return true;
}

void CAABonderDlg::func_SocketControl_ConnectCheck()
{
}


void CAABonderDlg::CreateServer()
{/* ���� ���� */
	CString sLog;

	if ( m_ListenSocket.Create(21000, SOCK_STREAM) == TRUE )
	{
		if ( m_ListenSocket.Listen() == TRUE )
		{
			sLangChange.LoadStringA(IDS_STRING1273);	//���� ���� �� ���� ��� ��...
			putListLog(sLangChange);
		}
		else
		{		
			sLangChange.LoadStringA(IDS_STRING1274);	//���� ���� ��� ����
			putListLog(sLangChange);
		}
	}
	else
	{
		sLangChange.LoadStringA(IDS_STRING1272);	//���� ���� ����
		putListLog(sLangChange);
	}
}

void CAABonderDlg::CloseServer()
{/* ���� �ݱ� */
	POSITION pos;
	pos = m_ListenSocket.m_ptrClientSocketList.GetHeadPosition();
	CClientSocket* pClient = NULL;

	while(pos != NULL)
	{
		pClient = (CClientSocket*)m_ListenSocket.m_ptrClientSocketList.GetNext(pos);

		if(pClient != NULL)
		{
			pClient->ShutDown();
			pClient->Close();

			delete pClient;
		}
	}

	m_ListenSocket.ShutDown();
	m_ListenSocket.Close();
}


void CAABonderDlg::CheckClientPosition(CSocket* pClient, CString sMsg)
{
	int iIndex = atoi(sMsg.Right(1)) - 1;

	if ( iIndex < 0 )
	{
		sLangChange.LoadStringA(IDS_STRING1379);		//�߸��� Ŭ���̾�Ʈ �����Դϴ�.\n\n(IP �ּҸ� Ȯ���� �ּ���)
		putListLog(sLangChange);
		return;
	}

	m_pos[iIndex] = m_ListenSocket.m_ptrClientSocketList.Find(pClient);

	CString sTemp;
	sTemp.Format("AA PC #%d Connection", iIndex+1);
	putListLog(sTemp);


}


bool CAABonderDlg::SendMessageToClient(int iCh, CString sMsg)
{
	//m_csSendMsg.Lock();

	CString sTemp = "&INSP$" + sMsg + "%";

	if(m_pos[iCh] == NULL) return false;

	if ( m_ListenSocket.SendData(m_pos[iCh], sTemp) == false )
	{
		sTempLang.LoadStringA(IDS_STRING169);
		sLangChange.Format(sTempLang, iCh+1, sTemp);
		sTemp.Format(sLangChange);
		putListLog(sTemp);
		//m_csSendMsg.Unlock();
		return false;
	}
	sLangChange.LoadStringA(IDS_STRING213);
	sTemp.Format(sLangChange, iCh+1, sMsg);
	putListLog(sTemp);

	//m_csSendMsg.Unlock();
	return true;
}

void CAABonderDlg::ServerCheckMessage(CSocket* pClient, CString sMsg)
{
	m_csRcvMsg.Lock();

	CString sTemp, sCompair;
	int iIndex = -1;
	for (int i=0 ; i<2 ; i++)
	{
		if ( m_pos[i] == m_ListenSocket.m_ptrClientSocketList.Find(pClient) )
		{
			iIndex = i;
			break;
		}
	}

	if(iIndex== -1)
	{
		sLangChange.LoadStringA(IDS_STRING1251);	//���� �޼��� �̻�.
		putListLog(sLangChange);
		m_csRcvMsg.Unlock();
		return;
	}

	int iCheckFirst = sMsg.Find('&', 0);
	int iCheckMiddle = sMsg.Find('$', 0);
	int iCheckLast = sMsg.Find('%', 0);

	if ( iCheckFirst < 0 || iCheckMiddle < 0 || iCheckLast < 0)
	{
		sLangChange.LoadStringA(IDS_STRING1251);	//���� �޼��� �̻�.
		putListLog(sLangChange);
		m_csRcvMsg.Unlock();
		return;
	}

	int startIndex, endIndex;
	CString logStr, sSendData ;//&%s$
	int stxIndex = sMsg.Find("&", 0);
	int etxIndex = sMsg.Find("%", 0);
	sTemp		= sMsg.Mid(stxIndex+1, etxIndex - stxIndex -1);	
	
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	if(stxIndex<0 || etxIndex<0 || stxIndex>etxIndex)
	{
		sLangChange.LoadStringA(IDS_STRING1337);	//�̴��� Data ���� �̻�
		sTemp.Format(sLangChange + _T(" - %s\n"), sMsg);
		errMsg2(Task.AutoFlag, sTemp);
		return;
	}

	startIndex	= sMsg.Find("$", 0);
	endIndex	= sMsg.Find("%", startIndex+1);
	sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
	
	startIndex	= sMsg.Find("$", 0);
	endIndex	= sMsg.Find("_", startIndex+1);
	sCompair	= sMsg.Mid(startIndex+1, endIndex - startIndex -1);

	if ( lstrcmp(sCompair, "TILTX") == 0 )
	{//TILTX_0.0245
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);

		Task.dTiltingManual[0] = atof(sTemp);
		Task.iRecvLenCnt[0] = 1;
	}
	else if( lstrcmp(sCompair, "TILTY") == 0 )
	{//TILTY_0.0245
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);

		Task.dTiltingManual[1] = atof(sTemp);
		Task.iRecvLenCnt[1] = 1;
	}
	else if( lstrcmp(sCompair, "ALIGNX") == 0 )
	{//������ X
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
		Task.dAlignManual[0] = atof(sTemp);
		Task.iRecvLenCnt[2] = 1;
	}
	else if( lstrcmp(sCompair, "ALIGNY") == 0 )
	{//������ Y
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
		Task.dAlignManual[1] = atof(sTemp);
		Task.iRecvLenCnt[3] = 1;
	}
	else if( lstrcmp(sCompair, "ALIGNT") == 0 )
	{//������ T
		startIndex	= sMsg.Find("_", 0);
		endIndex	= sMsg.Find("%", startIndex+1);
		sTemp		= sMsg.Mid(startIndex+1, endIndex - startIndex -1);
		Task.dAlignManual[2] = atof(sTemp);
		Task.iRecvLenCnt[4] = 1;
	}
	else if( lstrcmp(sTemp, "NAK") == 0 )
	{//!! ���� ���� ���
		Task.iRecvLenACK[iIndex] = 0;
	}
	else if( lstrcmp(sTemp, "ACK") == 0 )
	{//!! �� ���� ���� ���
		Task.iRecvLenACK[iIndex] = 1;
	}
	sLangChange.LoadStringA(IDS_STRING210);
	sMsg.Format(sLangChange, iIndex+1, sTemp);

	if (lstrcmp(m_sOldRcvMsg, sTemp) != 0)
	{
		putListLog(sMsg);
		m_sOldRcvMsg = sTemp;
	}

	m_csRcvMsg.Unlock();
}


void CAABonderDlg::func_Control_StateChange(int iCh)
{
	
}


void CAABonderDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	double dExpandFactorX, dExpandFactorY;

	if( ( lensDlg!=NULL && lensDlg->IsWindowVisible()) || 
		(lensEdgeDlg!=NULL && lensEdgeDlg->IsWindowVisible()) || 
		(pcbDlg!=NULL && pcbDlg->IsWindowVisible()) || 
//		(pcbInspDlg!=NULL && pcbInspDlg->IsWindowVisible()) || 
		(motorDlg!=NULL && motorDlg->m_bCalcResol) || 
		(motorDlg2!=NULL && motorDlg2->m_bCalcResol) || 
		(motorDlg3!=NULL && motorDlg3->m_bCalcResol)
		)
	{
		if (point.x>m_rectCamDispPos1.left &&
			point.x<m_rectCamDispPos1.right &&
			point.y>m_rectCamDispPos1.top &&
			point.y<m_rectCamDispPos1.bottom)
		{
			if (	lensDlg->IsWindowVisible())				m_iCurCamNo = CAM1; 
#if (____AA_WAY == PCB_TILT_AA)
			else if (lensEdgeDlg->IsWindowVisible())		m_iCurCamNo = CAM1;
#elif (____AA_WAY == LENS_TILT_AA)
			else if (lensEdgeDlg->IsWindowVisible())		m_iCurCamNo = CAM1;
#endif
			else if (pcbDlg->IsWindowVisible())				m_iCurCamNo = CAM1;
			else if ( motorDlg->IsWindowVisible())			m_iCurCamNo = motorDlg->m_iSelCam;
			else if ( motorDlg2->IsWindowVisible())			m_iCurCamNo = motorDlg2->m_iSelCam;
			else if ( motorDlg3->IsWindowVisible())			m_iCurCamNo = motorDlg3->m_iSelCam;			
			int iGap;
			if (m_iCurCamNo < 4)
			{
				dExpandFactorX = CAM_EXPAND_FACTOR_X;
				dExpandFactorY = CAM_EXPAND_FACTOR_Y;
				iGap = 20;
			}
			else
			{
				dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
				dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
				iGap = 200;
			}

			m_ClickP.x = point.x - m_rectCamDispPos1.left;
			m_ClickP.y = point.y - m_rectCamDispPos1.top;

			if ( m_bMeasureDist )
			{
				m_bDrawMeasureLine = true;
			}
			else
			{
				m_bDrawFlag = true;

				if (m_ClickP.x * dExpandFactorX>m_rBox.left-iGap	&&
					m_ClickP.y * dExpandFactorY>m_rBox.top-iGap		&&
					m_ClickP.x * dExpandFactorX<m_rBox.right+iGap	&&
					m_ClickP.y * dExpandFactorY<m_rBox.bottom+iGap)
				{
					m_bBoxMoveFlag = true;
				}

				m_iMoveType = checkMousePos(point, m_rBox);
			}
		}
	}
	//else if (ccdDlg->m_pDefectDlg!=NULL && ccdDlg->m_pDefectDlg->IsWindowVisible() )
	//{
	//	m_iCurCamNo = 3;

	//	if (point.x>m_rectCamDispPos1.left &&
	//		point.x<m_rectCamDispPos1.right &&
	//		point.y>m_rectCamDispPos1.top &&
	//		point.y<m_rectCamDispPos1.bottom)
	//	{
	//		m_bDrawFlag = true;
	//		
	//		m_ClickP.x = point.x - m_rectCamDispPos1.left;
	//		m_ClickP.y = point.y - m_rectCamDispPos1.top;

	//		if ( m_bBox_CCD_Zoom == true )
	//		{
	//			if ( m_bState_CCD_Zoom == false )
	//			{
	//				//! ���� ȭ���� ��ü ���� ������ ��
	//				//! Ȯ���� �簢 ���� ���� ����

	//				int iGap;

	//				dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CAM_SIZE_X;
	//				dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CAM_SIZE_Y;
	//				iGap = 200;

	//				if (m_ClickP.x * dExpandFactorX>m_rBox.left-iGap	&&
	//					m_ClickP.y * dExpandFactorY>m_rBox.top-iGap		&&
	//					m_ClickP.x * dExpandFactorX<m_rBox.right+iGap	&&
	//					m_ClickP.y * dExpandFactorY<m_rBox.bottom+iGap)
	//				{
	//					m_bBoxMoveFlag = true;							
	//				}

	//				m_bBox_Acting_CCD_Zoom = true;

	//				m_iMoveType = checkMousePos(point, m_rBox);
	//			}
	//			else
	//			{
	//				if ( m_bPan_CCD_Zoom == true )
	//				{
	//					//! Ȯ��� ���¿��� Mouse�� ȭ�� �̵� ����
	//					m_PanMoveP = m_ClickP;
	//					m_bActing_Pan_CCD_Zoom = true;
	//				}
	//			}							
	//		}
	//		else
	//		{
	//			//! Ŭ���� ������ ��Ⱚ ǥ��

	//			CPoint p, pShow;
	//			int pos, width;
	//			char szTmp[256];

	//			vision.clearOverlay(m_iCurCamNo);

	//			vision.MilBufferUpdate();

	//			width = MbufInquire(vision.MilGrabImageChild[4], M_PITCH, M_NULL);

	//			//! Ȯ��� ���¿����� Ŭ���� ������ ��Ȯ�ϰ� ���ڼ��� �׸���. 
	//			if ( m_bState_CCD_Zoom == true )
	//			{
	//				p.x = m_ViewPos.x + m_ClickP.x;
	//				p.y = m_ViewPos.y + m_ClickP.y;
	//				pShow.x = m_ClickP.x * gMIUDevice.nWidth / SMALL_CAM_SIZE_X;
	//				pShow.y = m_ClickP.y * gMIUDevice.nHeight / SMALL_CAM_SIZE_Y;
	//			}
	//			else
	//			{
	//				p.x = m_ClickP.x * gMIUDevice.nWidth / SMALL_CAM_SIZE_X;
	//				p.y = m_ClickP.y * gMIUDevice.nHeight / SMALL_CAM_SIZE_Y;
	//				pShow.x = p.x;
	//				pShow.y = p.y;
	//			}

	//			vision.crosslist[m_iCurCamNo].addList(pShow, 150, M_COLOR_RED);

	//			pos = p.y * width + p.x;

	//			sprintf_s(szTmp, "(%d, %d) ==> RGB %d, %d, %d", p.x, p.y, vision.Image[3][pos], vision.Image[4][pos], vision.Image[5][pos]);
	//			vision.textlist[m_iCurCamNo].addList(50, 680, szTmp, M_COLOR_RED, 17, 7, "Arial");
	//			vision.drawOverlay(m_iCurCamNo, true);
	//		}			
	//	} 

	//	CDialogEx::OnLButtonDown(nFlags, point);
	//	return;
	//}
	//else if (ccdDlg->m_pOSChkDlg!=NULL && ccdDlg->m_pOSChkDlg->IsWindowVisible() || (ccdDlg->m_pSFRDlg!=NULL && ccdDlg->m_pSFRDlg->IsWindowVisible() ) )
	else if ((ccdDlg->m_pSFRDlg!=NULL && ccdDlg->m_pSFRDlg->IsWindowVisible() ) )
	
	{
		m_iCurCamNo = 3;

		/*if (point.x>m_rectCamDispPos1.left &&
			point.x<m_rectCamDispPos1.right &&
			point.y>m_rectCamDispPos1.top &&
			point.y<m_rectCamDispPos1.bottom)*/
			if (point.x>m_rectCcdDispPos.left &&
				point.x<m_rectCcdDispPos.right &&
				point.y>m_rectCcdDispPos.top &&
				point.y<m_rectCcdDispPos.bottom)
		{
			int iGap;

			dExpandFactorX = (double)gMIUDevice.nWidth/SMALL_CCD_SIZE_X;
			dExpandFactorY = (double)gMIUDevice.nHeight/SMALL_CCD_SIZE_Y;
			iGap = 200;

			m_ClickP.x = point.x - m_rectCcdDispPos.left;// m_rectCamDispPos1.left;
			m_ClickP.y = point.y - m_rectCcdDispPos.top;// m_rectCamDispPos1.top;

            if (vision.m_FovSetMode == true)
            {
                ccdDlg->m_pSFRDlg->m_nSelectIndexFOV = ccdDlg->m_pSFRDlg->GetSelectedFOVNo(point);
                ccdDlg->m_pSFRDlg->drawRectFOV(ccdDlg->m_pSFRDlg->m_nSelectIndexFOV);//DrawRectFov(m_nSelectIndexCCD);
                return;
            }
            else if (vision.m_SnrSetMode == true)
            {
                ccdDlg->m_pSFRDlg->m_nSelectIndexSNR = ccdDlg->m_pSFRDlg->GetSelectedSNRNo(point);
                ccdDlg->m_pSFRDlg->drawRectSNR(ccdDlg->m_pSFRDlg->m_nSelectIndexSNR);//   drawRectSnr(m_nSelectIndexCCD);
                return;
            }

			m_iNo_SFR = ccdDlg->m_pSFRDlg->checkNoSFR(point);


			//if (m_iNo_SFR>=0 && m_iNo_SFR < LAST_MARK_CNT)
			if (m_iNo_SFR >= 0 && m_iNo_SFR < model.mGlobalChartCount)
			{
				m_bBoxMoveFlag_CCD = true;

				m_rBox.left		= ccdDlg->m_pSFRDlg->m_iOffsetX_SFR[m_iNo_SFR];
				m_rBox.top		= ccdDlg->m_pSFRDlg->m_iOffsetY_SFR[m_iNo_SFR];
				m_rBox.right	= m_rBox.left + ccdDlg->m_pSFRDlg->m_iSizeX_SFR[m_iNo_SFR];
				m_rBox.bottom	= m_rBox.top + ccdDlg->m_pSFRDlg->m_iSizeY_SFR[m_iNo_SFR];

				m_iMoveType = checkMousePos(point, m_rBox);
			}
			//else if (m_iNo_SFR>=LAST_MARK_CNT && m_iNo_SFR<LAST_MARK_CNT+4)
			else if (m_iNo_SFR >= model.mGlobalChartCount && m_iNo_SFR<model.mGlobalChartCount + 4)
			{
				m_bBoxMoveFlag_CCD = true;

				m_rBox	= ccdDlg->m_pSFRDlg->m_rcRoiBox[m_iNo_SFR- model.mGlobalChartCount];//LAST_MARK_CNT];

				m_iMoveType = checkMousePos(point, m_rBox);
			}
		}
	}


	if(m_iCurCamNo<0 || m_iCurCamNo>3)
	{
		putListLog("���콺 Ŭ�� - ī�޶� ��ȣ �̻�");
		return;
	}


	/*if (point.x>m_rectCamDispPos1.left &&
		point.x<m_rectCamDispPos1.right &&
		point.y>m_rectCamDispPos1.top &&
		point.y<m_rectCamDispPos1.bottom	&& !m_bMeasureDist && m_bBoxMoveFlag_CCD == false )
	{*/
	if (point.x>m_rectCcdDispPos.left &&
		point.x<m_rectCcdDispPos.right &&
		point.y>m_rectCcdDispPos.top &&
		point.y<m_rectCcdDispPos.bottom && !m_bMeasureDist && m_bBoxMoveFlag_CCD == false)
	{
		CPoint p;
		int pos = 0;
		int width = 0;
		int val = 0;
		char szTmp[256];

		vision.clearOverlay(m_iCurCamNo);

		if(m_iCurCamNo<3) 
		{
			p.x = (int)(m_ClickP.x * CAM_EXPAND_FACTOR_X + 0.5);
			p.y = (int)(m_ClickP.y * CAM_EXPAND_FACTOR_Y + 0.5);
			vision.crosslist[m_iCurCamNo].addList(p, 30, M_COLOR_RED);
			MbufCopy(vision.MilGrabImageChild[m_iCurCamNo], vision.MilProcImageChild[m_iCurCamNo]);
			width = MbufInquire(vision.MilProcImageChild[m_iCurCamNo], M_PITCH, M_NULL);
			pos = p.y * width + p.x;
			val = vision.MilImageBuffer[m_iCurCamNo][pos];

			sprintf_s(szTmp, "(%d, %d) %d", p.x, p.y, val);
			vision.textlist[m_iCurCamNo].addList(50, 700, szTmp, M_COLOR_RED, 17, 7, "Arial");

			if( Task.bManual_FindEpoxy == true)
			{
				Task.cpMEpoxyPos.x = p.x;
				Task.cpMEpoxyPos.y = p.y;
			}
		}
		else 
		{
			vision.MilBufferUpdate();

			width = MbufInquire(vision.MilGrabImageChild[4], M_PITCH, M_NULL);

			p.x = m_ClickP.x * gMIUDevice.nWidth / SMALL_CCD_SIZE_X;
			p.y = m_ClickP.y * gMIUDevice.nHeight / SMALL_CCD_SIZE_Y;

			vision.crosslist[m_iCurCamNo].addList(p, 150, M_COLOR_RED);
			 
			pos = p.y * width + p.x;

			sprintf_s(szTmp, "(%d, %d) ==> RGB %d, %d, %d", p.x, p.y, vision.MilImageBuffer[3][pos], vision.MilImageBuffer[4][pos], vision.MilImageBuffer[5][pos]);
			vision.textlist[m_iCurCamNo].addList(50, 680, szTmp, M_COLOR_RED, 17, 7, "Arial");
		}


        if (ccdDlg->m_pSFRDlg != NULL && ccdDlg->m_pSFRDlg->IsWindowVisible())
        {
            if (vision.m_FovSetMode == true)
            {
                ccdDlg->m_pSFRDlg->drawRectFOV(m_iNo_SFR);
            }
            else if (vision.m_FovSetMode == true)
            {
                ccdDlg->m_pSFRDlg->drawRectSNR(m_iNo_SFR);
            }
            else
            {
                ccdDlg->m_pSFRDlg->drawRectSFR(m_iNo_SFR);
            }
            
        }
		vision.drawOverlay(m_iCurCamNo, true);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CAABonderDlg::OnBnClickedButtonLensPassResult()
{
	CString logStr="";
	if(Task.AutoFlag==1)
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_RED);
	}
	else if(Task.AutoFlag==0 || Task.AutoFlag==2)
	{	
		logStr.Format("LENS �ѱ� �Ϸ� ���θ� �����ϼ���. ��ǰ ���� Ȯ�� �ϼ���..\n �ѱ� �Ϸ� ���� = Yes / �ѱ� �̿Ϸ� ���� = No");
		if( askMsg(logStr))
		{
			if(!Dio.LensMotorGripCheck(true, false) )//Lens ������ �ȵǾ��� ���  
			{
				sLangChange.Format(_T("Lens Grip �������°� �ƴմϴ�."));
				delayMsg(sLangChange.GetBuffer(99), 3000, M_COLOR_DARK_GREEN);
			}else
			{
				Task.m_bOKLensPass = 1;
			}
		}else
		{
			if(Dio.LensMotorGripCheck(true, false) )//Lens ������ �Ǿ��� ���  
			{
				sLangChange.Format(_T("Lens Grip �������°� �ƴմϴ�."));
				delayMsg(sLangChange.GetBuffer(99), 3000, M_COLOR_DARK_GREEN);
				
			}else
			{
				if(Dio.LensMotorGripCheck(false, false) )
				{
					Task.m_bOKLensPass = -1;
				}else
				{
					sLangChange.Format(_T("Lens Grip �������°� �ƴմϴ�."));
					delayMsg(sLangChange.GetBuffer(99), 3000, M_COLOR_DARK_GREEN);
				}
			}
			
		}
			
	}
}
// ���� �˻� 
bool CAABonderDlg::_EpoxyFinddispense(int cam)
{
	unsigned char *m_Imagebuf;
	m_Imagebuf = (unsigned char *)malloc(CAM_SIZE_X*CAM_SIZE_Y);

	Sleep(100);

	vision.m_csGrab.Lock();

	Sleep(100);

	MbufGet(vision.MilGrabImageChild[CAM1], m_Imagebuf);	// �̹��� ��ü �ȼ� ��Ⱚ ���� ���

	vision.m_csGrab.Unlock();
	
	bool bRtn = true;

	bool bRtn1 = true;
	bool bRtn2 = true;
	bool bRtn3 = true;
	bool bRtn4 = true;


	bRtn1 = _inspResignRect(false, CAM1, 0, 0, m_Imagebuf);		// left (����)
	bRtn2 = _inspResignRect(false, CAM1, 0, 1, m_Imagebuf);	// right (����)
	bRtn3 = _inspResignRect(false, CAM1, 1, 2, m_Imagebuf);	// top (����)
	bRtn4 = _inspResignRect(false, CAM1, 1, 3, m_Imagebuf);	// bottom (����)

	bRtn = (bRtn1 && bRtn2 && bRtn3 && bRtn4);


	free(m_Imagebuf);
	
	return bRtn;
}

bool CAABonderDlg::_inspResign(bool autoMode, int index, int dispMode)				// dispMode 0:ROI + ��� ���, 1:���� White ���� + Text ���, 2:���� White Edge ���
{
	int iCh = 0;
	if ( vision.getLiveMode() == 1 )
	{
		vision.getSnapImage(1);
	}


	if(Task.m_iStatus_Unit_Epoxy == 1)
	{
		saveInspImage(EPOXY_IMAGE_SAVE, index);
	}

	if(index<0 || index>>3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}

		
	Task.m_bResign_Result[index] = false;

	float innerOffsetX	= (float)((model.m_ResinDrawSize.x*0.5) - model.m_dResinInspOffset[0].x);
	float innerOffsetY	= (float)((model.m_ResinDrawSize.y*0.5) - model.m_dResinInspOffset[0].y);
	float resignCheckX	= (float)((model.m_ResinDrawSize.x*0.5) + model.m_dResinInspOffset[1].x);
	float resignCheckY	= (float)((model.m_ResinDrawSize.y*0.5) + model.m_dResinInspOffset[1].y);
	int i_limit_rate	= model.m_iResinInspLimit;

	int margine = 5;

	char	szDispData[256];
	CString sTemp;
	double ep1, ep2;

	ep1 = myTimer(true);


	vision.clearOverlay();

	int width = MbufInquire(vision.MilProcImage[0], M_PITCH, NULL);


	int x, y, pos, sum, minVal, maxVal, avgVal;
	int sx, sy, ex, ey;
	CRect inRect, outRect;

	double centX = Task.d_mark_pos_x[1][index];
	double centY = Task.d_mark_pos_y[1][index];

	inRect.left		= (int)(centX - innerOffsetX / sysData.dCamResol[iCh].x - 0.5);
	inRect.right	= (int)(centX + innerOffsetX / sysData.dCamResol[iCh].x + 0.5);
	inRect.top		= (int)(centY - innerOffsetY / sysData.dCamResol[iCh].y + 0.5);
	inRect.bottom	= (int)(centY + innerOffsetY / sysData.dCamResol[iCh].y - 0.5);

	outRect.left	= (int)(centX - resignCheckX / sysData.dCamResol[iCh].x - 0.5);
	outRect.right	= (int)(centX + resignCheckX / sysData.dCamResol[iCh].x + 0.5);
	outRect.top		= (int)(centY - resignCheckY / sysData.dCamResol[iCh].y + 0.5);
	outRect.bottom	= (int)(centY + resignCheckY / sysData.dCamResol[iCh].y - 0.5);


	vision.crosslist[iCh].addList((int)centX, (int)centY, 30, M_COLOR_GREEN);				// ���� �˻� ����..

	vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
	vision.boxlist[iCh].addList(outRect, PS_SOLID, M_COLOR_GREEN);


	int Hist[CAM_SIZE_X];


	int inspStartPosX[CAM_SIZE_X]	= {0, };
	int inspEndPosX[CAM_SIZE_X]		= {0, };
	int inspStartPosY[CAM_SIZE_Y]	= {0, };
	int inspEndPosY[CAM_SIZE_Y]		= {0, };

	int startPoint[CAM_SIZE_X]		= {0, };
	int endPoint[CAM_SIZE_X]		= {0, };


	int leftVal = 0;
	int leftPos = 0;
	int rightVal = 0;
	int rightPos = 0;
	int topVal = 0;
	int topPos = 0;
	int bottomVal = 0;
	int bottomPos = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// ���� ���� ���� �˻� Start..
	sx = outRect.left; 
	ex = inRect.left;
	sy = (int)(inRect.top + fabs((outRect.top - inRect.top)*0.5));
	ey = (int)(inRect.bottom - fabs((outRect.bottom - inRect.bottom)*0.5));


	if (sx<0)					sx = 0;
	if (ex>=CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey>=CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if((ex-sx)<10)
	{
		sTempLang.LoadStringA(IDS_STRING119);
		sLangChange.Format(sTempLang, sx, ex);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("Arial"));
		sLangChange.LoadStringA(IDS_STRING246);
		putListLog(sLangChange);
		return false;
	}

	if((ey-sy)<10)
	{
		sTempLang.LoadStringA(IDS_STRING120);
		sLangChange.Format(sTempLang, sy, ey);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("Arial"));
		sLangChange.LoadStringA(IDS_STRING247);
		putListLog(sLangChange);
		return false;
	}


	minVal = 255;
	maxVal = 0;
	avgVal = 0;

	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	for(x=sx; x<ex; x++)
	{
		sum = 0;
		pos = sy*width + x;

		for(y=sy; y<ey; y++)
		{
			sum += vision.MilImageBuffer[1][pos];
			pos += width;
		}
		avgVal += sum;
		Hist[x] = sum;

		if(minVal > sum / (ey - sy))
			minVal = sum / (ey - sy);
		if(maxVal < sum / (ey - sy))
			maxVal = sum / (ey - sy);
	}

	avgVal = avgVal / ((ex-sx)*(ey-sy));

	if(dispMode)
	{
		sTempLang.LoadStringA(IDS_STRING121);
		sLangChange.Format(sTempLang, minVal, maxVal, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 60, szDispData, M_COLOR_GREEN, 17, 8, _T("arialuni"));
	}


	if((maxVal-minVal)<70)
	{
		sTempLang.LoadStringA(IDS_STRING137);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING357);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ������ ��� ���̰� �ʹ� �۽��ϴ�.\n(Min %d, Max %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	if(avgVal>230)	//               ���� ������ ��� ���� ��
	{
		sTempLang.LoadStringA(IDS_STRING135);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING358);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ������ ������ �ʹ� ����ϴ�.\n(Avg %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	//if(maxVal<190)	  // ���� ���� �˻��ϴ°� �ƴϰ� ��ο� ���� �˻�
	if(minVal > 150)
	{
		sTempLang.LoadStringA(IDS_STRING136);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING356);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� Line�� �ʹ� ����ϴ�.\n(Min %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	


	int whiteVal	= (int)(minVal*1.3 + (avgVal * 0.3));//(maxVal*0.4 + avgVal*0.6);        // ���� �ִ��� ����
	int whiteVal2	= whiteVal * 3;							// ���� �� �������
	int whiteVal3	= whiteVal * 3;


	int val_start1, val_start2, val_end1, val_end2;
	int startPos, endPos, startVal, endVal;

	bool findFlag;


	sy = inRect.top - 5;
	ey = inRect.bottom + 5;

	sx += 5;
	ex -= 5;

	if(sx>(ex-3))
	{
		sLangChange.LoadStringA(IDS_STRING248);
		_stprintf_s(szDispData, sLangChange, sx-5, ex+5);
		putListLog(szDispData);

		sx = (sx+ex)/2 - 1;
		ex = sx + 3;
	}


	for(y=sy; y<ey; y++)
	{
		pos = y*width + sx;

		startVal	= -9999;
		endVal		= -9999;
		startPos	= 9999;
		endPos		= 0;
		findFlag	= false;

		val_start1	= vision.MilImageBuffer[iCh][pos-1] + vision.MilImageBuffer[iCh][pos-2] + vision.MilImageBuffer[iCh][pos-3] + vision.MilImageBuffer[iCh][pos-4] + vision.MilImageBuffer[iCh][pos-5];
		val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

		val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
		val_end2	= vision.MilImageBuffer[iCh][pos+1] + vision.MilImageBuffer[iCh][pos+2] + vision.MilImageBuffer[iCh][pos+3] + vision.MilImageBuffer[iCh][pos+4] + vision.MilImageBuffer[iCh][pos+5];

		for(x=sx; x<ex; x++)
		{
			if( (vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos-2]-margine)) && (vision.MilImageBuffer[1][pos-1]>=(vision.MilImageBuffer[iCh][pos-3]-margine)) && (vision.MilImageBuffer[iCh][pos-2]>=(vision.MilImageBuffer[iCh][pos-4]-margine)) &&
				vision.MilImageBuffer[iCh][pos-1]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end1-val_start1) > startVal)
				{
					startVal = val_end1-val_start1;
					startPos = x;
					inspStartPosX[x]++;

					startPoint[y] = x;
					
					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_MAGENTA);

					findFlag = true;
				}
			}

			if(	findFlag && 
				(	vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos+2]-margine)) && (vision.MilImageBuffer[iCh][pos+1]>=(vision.MilImageBuffer[iCh][pos+3]-margine)) && (vision.MilImageBuffer[iCh][pos+2]>=(vision.MilImageBuffer[iCh][pos+4]-margine)) &&
				vision.MilImageBuffer[iCh][pos+1]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end2-val_start2) > endVal)
				{
					endVal = val_end2-val_start2;
					endPos = x;
					if(dispMode==2)
					{
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_BLUE);
					}

					endPoint[y] = x;

					inspEndPosX[x]++;
				}
			}
			pos++;

			val_start1	= val_start1 + vision.MilImageBuffer[iCh][pos-1] - vision.MilImageBuffer[iCh][pos-6];
			val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

			val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
			val_end2	= val_end2 - vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+5];
		}
	}


	startVal	= -9999;
	endVal		= -9999;
	startPos	= 9999;
	endPos		= 0;

	for(x=sx; x<ex; x++)
	{
		if(inspStartPosX[x]>startVal)
		{
			startVal = inspStartPosX[x];
			startPos = x;
		}

		if(inspEndPosX[x] > endVal)
		{
			endVal = inspEndPosX[x];
			endPos = x;
		}
	}

	if(startVal>endVal)
	{
		endVal		= -9999;
		endPos		= 0;

		for(x=startPos; x<ex; x++)
		{
			if(inspEndPosX[x] > endVal)
			{
				endVal = inspEndPosX[x];
				endPos = x;
			}
		}
	}
	else
	{
		startVal	= -9999;
		startPos	= 9999;

		for(x=sx; x<endPos; x++)
		{
			if(inspStartPosX[x]>startVal)
			{
				startVal = inspStartPosX[x];
				startPos = x;
			}
		}
	}


	//	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	maxVal = 0;

	for (x=startPos; x<=endPos; x++)
	{
		if(Hist[x]>maxVal)
		{
			maxVal = Hist[x];
			leftPos = x;
		}
	}
	//	leftPos = (startPos + endPos + 1) / 2;
	if(leftPos == 0 || leftPos >= 270)
	{
		leftPos = 262;
	}

	leftVal = 0;
	for(y=sy; y<ey; y++)
	{
		pos = y*width + leftPos;

		startVal = vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+1];
		if(startVal <= whiteVal2)
		{
			leftVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos-2] + vision.MilImageBuffer[iCh][pos-3];
		if(startVal <= whiteVal2)
		{
			leftVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos+2] + vision.MilImageBuffer[iCh][pos+3];
		if(startVal <= whiteVal2)
		{
			leftVal++;
			continue;
		}
	}




	//! 2013. 06. 10. ���� �˻�.. Edge �ν� �� �̿��Ͽ� �˻� �ϵ��� Test..
// 	leftVal = 0;
// 	for(int y=sy; y<ey; y++)
// 	{
// 		if(startPoint[y]>sx && startPoint[y]<endPoint[y])
// 		{
// 			leftVal++;
// 		}
// 	}


	Task.m_line_cnt[0]	= leftVal;
	Task.m_line_pos[0]	= leftPos;
	Task.m_f_line_rate[0]= leftVal * 100.0f / (inRect.bottom - inRect.top - 20);

	if(Task.m_f_line_rate[0]>100)
		Task.m_f_line_rate[0] = 100;


	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	// ���� ���� ���� �˻� Start..

	sx = inRect.right;
	ex = outRect.right;
	sy = (int)(inRect.top + fabs((outRect.top - inRect.top)*0.5));
	ey = (int)(inRect.bottom - fabs((outRect.bottom - inRect.bottom)*0.5));

	if (sx<0)					sx = 0;
	if (ex>=CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey>=CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if((ex-sx)<10)
	{
		sTempLang.LoadStringA(IDS_STRING138);
		sLangChange.Format(sTempLang, sx, ex);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING1317);	//���� �˻� ���� ���� �� �̻�
		putListLog(sLangChange);
		return false;
	}

	if((ey-sy)<10)
	{
		sTempLang.LoadStringA(IDS_STRING139);
		sLangChange.Format(sTempLang, sy, ey);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sLangChange.LoadStringA(IDS_STRING1318);	//���� �˻� ���� ���� �� �̻�
		putListLog(sLangChange);
		return false;
	}


	minVal = 255;
	maxVal = 0;
	avgVal = 0;

	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	for(x=sx; x<ex; x++)
	{
		sum = 0;
		pos = sy*width + x;

		for(y=sy; y<ey; y++)
		{
			sum += vision.MilImageBuffer[iCh][pos];
			pos += width;
		}
		avgVal += sum;

		Hist[x] = sum;

		if(minVal > sum / (ey - sy))
			minVal = sum / (ey - sy);
		if(maxVal < sum / (ey - sy))
			maxVal = sum / (ey - sy);
	}

	avgVal = avgVal / ((ex-sx)*(ey-sy));


	if(dispMode)
	{
		sTempLang.LoadStringA(IDS_STRING140);
		sLangChange.Format(sTempLang, minVal, maxVal, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 100, szDispData, M_COLOR_GREEN, 17, 8, _T("arialuni"));
	}

	if((maxVal-minVal)<70)
	{
		sTempLang.LoadStringA(IDS_STRING143);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING354);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ������ ��� ���̰� �ʹ� �۽��ϴ�.\n(Min %d, Max %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);
		return false;
	}

	if(avgVal>230)	
	{
		sTempLang.LoadStringA(IDS_STRING141);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING355);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ������ �ʹ� ����ϴ�.\n(Avg %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);
		return false;
	}

	//if(maxVal<190)
	if(minVal > 150)
	{
		sTempLang.LoadStringA(IDS_STRING142);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING102);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� Line�� �ʹ� ����ϴ�.\n(Min %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}


	whiteVal	= (int)(minVal* 1.5 + avgVal*0.1);//(maxVal*0.4 + avgVal*0.6);
	whiteVal2	= whiteVal * 2;
	whiteVal3	= whiteVal * 3;


	sy = inRect.top - 5;
	ey = inRect.bottom + 5;

	sx += 5;
	ex -= 5;

	if(sx>(ex-3))
	{
		sLangChange.LoadStringA(IDS_STRING245);
		_stprintf_s(szDispData, sLangChange, sx-5, ex+5);
		putListLog(szDispData);

		sx = (sx+ex)/2 - 1;
		ex = sx + 3;
	}


	for(y=sy; y<ey; y++)
	{
		pos = y*width + sx;

		startVal	= -9999;
		endVal		= -9999;
		startPos	= 9999;
		endPos		= 0;
		findFlag	= false;

		val_start1	= vision.MilImageBuffer[iCh][pos-1] + vision.MilImageBuffer[iCh][pos-2] + vision.MilImageBuffer[iCh][pos-3] + vision.MilImageBuffer[iCh][pos-4] + vision.MilImageBuffer[iCh][pos-5];
		val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

		val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
		val_end2	= vision.MilImageBuffer[iCh][pos+1] + vision.MilImageBuffer[iCh][pos+2] + vision.MilImageBuffer[iCh][pos+3] + vision.MilImageBuffer[iCh][pos+4] + vision.MilImageBuffer[iCh][pos+5];

		for(x=sx; x<ex; x++)
		{
			if( (vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos-2]-margine)) && (vision.MilImageBuffer[iCh][pos-1]>=(vision.MilImageBuffer[iCh][pos-3]-margine)) && (vision.MilImageBuffer[iCh][pos-2]>=(vision.MilImageBuffer[iCh][pos-4]-margine)) &&
				vision.MilImageBuffer[iCh][pos-1]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end1-val_start1) > startVal)
				{
					startVal = val_end1-val_start1;
					startPos = x;
					inspStartPosX[x]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_MAGENTA);

					findFlag = true;
				}
			}

			if(	findFlag && 
				(	vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos+2]-margine)) && (vision.MilImageBuffer[iCh][pos+1]>=(vision.MilImageBuffer[iCh][pos+3]-margine)) && (vision.MilImageBuffer[iCh][pos+2]>=(vision.MilImageBuffer[iCh][pos+4]-margine)) &&
				vision.MilImageBuffer[iCh][pos+1]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end2-val_start2) > endVal)
				{
					endVal = val_end2-val_start2;
					endPos = x;
					inspEndPosX[x]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_BLUE);
				}
			}
			pos++;

			val_start1	= val_start1 + vision.MilImageBuffer[iCh][pos-1] - vision.MilImageBuffer[iCh][pos-6];
			val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

			val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
			val_end2	= val_end2 - vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+5];
		}
	}

	startVal	= -9999;
	endVal		= -9999;
	startPos	= 9999;
	endPos		= 0;

	for(x=sx; x<ex; x++)
	{
		if(inspStartPosX[x]>startVal)
		{
			startVal = inspStartPosX[x];
			startPos = x;
		}

		if(inspEndPosX[x] > endVal)
		{
			endVal = inspEndPosX[x];
			endPos = x;
		}
	}

	if(startVal>endVal)
	{
		endVal		= -9999;
		endPos		= 0;

		for(x=startPos; x<ex; x++)
		{
			if(inspEndPosX[x] > endVal)
			{
				endVal = inspEndPosX[x];
				endPos = x;
			}
		}
	}
	else
	{
		startVal	= -9999;
		startPos	= 9999;

		for(x=sx; x<endPos; x++)
		{
			if(inspStartPosX[x]>startVal)
			{
				startVal = inspStartPosX[x];
				startPos = x;
			}
		}
	}

	//	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	maxVal = 0;

	for (x=startPos; x<=endPos; x++)
	{
		if(Hist[x]>maxVal)
		{
			maxVal = Hist[x];
			rightPos = x;
		}
	}
	if(rightPos == 0 || rightPos < 760)
	{
		rightPos = 780;
	}
	//	rightPos = (startPos + endPos + 1) / 2;

	rightVal = 0;
	for(y=sy; y<ey; y++)
	{
		pos = y*width + rightPos;

		startVal = vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+1];
		if(startVal <= whiteVal2)
		{
			rightVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos-2] + vision.MilImageBuffer[iCh][pos-3];
		if(startVal <= whiteVal2)
		{
			rightVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos+2] + vision.MilImageBuffer[iCh][pos+3];
		if(startVal <= whiteVal2)
		{
			rightVal++;
			continue;
		}
	}

	Task.m_line_cnt[1]	= rightVal;
	Task.m_line_pos[1]	= rightPos;
	Task.m_f_line_rate[1]= rightVal * 100.0f / (inRect.bottom - inRect.top - 20);

	if(Task.m_f_line_rate[1]>100)
		Task.m_f_line_rate[1] = 100;




	double resinRectSizeX = (rightPos - leftPos) * sysData.dCamResol[iCh].x;

	if (model.m_ResinDrawSize.x*0.8>resinRectSizeX || resinRectSizeX>model.m_ResinDrawSize.x*1.2)
	{
		sTempLang.LoadStringA(IDS_STRING349);
		sLangChange.Format(sTempLang, resinRectSizeX, model.m_ResinDrawSize.x);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ���� �������Դϴ�.\n[���� �� %.01f mm, Spec %.01f mm

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);
	}



	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	// ��� ���� ���� �˻� Start..

	sx = (int)(inRect.left + fabs((outRect.left - inRect.left)*0.5));
	ex = (int)(inRect.right - fabs((outRect.right - inRect.right)*0.5));
	sy = outRect.top;
	ey = inRect.top;


	//	vision.boxlist[iCh].addList(sx, sy, ex, ey, PS_SOLID, M_COLOR_GREEN);

	if (sx<0)					sx = 0;
	if (ex>=CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey>=CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if((ex-sx)<10)
	{
		sTempLang.LoadStringA(IDS_STRING144);
		sLangChange.Format(sTempLang, sx, ex);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING242);
		putListLog(sLangChange);
		return false;
	}

	if((ey-sy)<10)
	{
		sTempLang.LoadStringA(IDS_STRING145);
		sLangChange.Format(sTempLang, sy, ey);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING243);
		putListLog(sLangChange);
		return false;
	}


	minVal = 255;
	maxVal = 0;
	avgVal = 0;

	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	for(y=sy; y<ey; y++)
	{
		sum = 0;
		pos = y*width + sx;

		for(x=sx; x<ex; x++)
		{
			sum += vision.MilImageBuffer[iCh][pos];
			pos++;
		}
		avgVal += sum;

		Hist[y] = sum;

		if(minVal > sum / (ex - sx))				minVal = sum / (ex - sx);
		if(maxVal < sum / (ex - sx))				maxVal = sum / (ex - sx);
	}

	avgVal = avgVal / ((ex-sx) * (ey-sy));


	if(dispMode)
	{
		sTempLang.LoadStringA(IDS_STRING122);
		sLangChange.Format(sTempLang, minVal, maxVal, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 140, szDispData, M_COLOR_GREEN, 17, 8, _T("arialuni"));
	}

	


	// ���� ������ ��ο� ������ ��� ���� 50 �̻��̰�, ��� ���� 200 ���Ϸ�..
	if( (maxVal-minVal)<70)
	{
		sTempLang.LoadStringA(IDS_STRING148);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING351);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n��� ���� ������ ��� ���̰� �ʹ� �۽��ϴ�.\n(Min %d, Max %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	if(avgVal>230 ) 	
	{
		sTempLang.LoadStringA(IDS_STRING146);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sTempLang.LoadStringA(IDS_STRING352);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n��� ���� ������ �ʹ� ����ϴ�.\n(Avg %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	//if(maxVal<190)	
	if(minVal > 150)
	{
		sTempLang.LoadStringA(IDS_STRING147);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING350);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n��� ���� Line�� �ʹ� ����ϴ�.\n(Min %d)
		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}


	whiteVal	= (int)(minVal*1.5 + avgVal*0.3);//(maxVal*0.9 + avgVal*0.1);
//	whiteVal	= (int)(maxVal*0.35 + avgVal*0.65);
	whiteVal2	= whiteVal * 3;
	whiteVal3	= whiteVal * 3;

	int width2	= 2*width;
	int width3	= 3*width;
	int width4	= 4*width;
	int width5	= 5*width;

	sx = inRect.left - 5;
	ex = inRect.right + 5;

	sy += 5;
	ey -= 5;

	if(sy>(ey-3))
	{
		sLangChange.LoadStringA(IDS_STRING244);
		_stprintf_s(szDispData, sLangChange, sy-5, ey+5);
		putListLog(szDispData);

		sy = (sy+ey)/2 - 1;
		ey = sy + 3;
	}


	for(x=sx; x<ex; x++)
	{
		pos = sy*width + x;

		startVal	= -9999;
		endVal		= -9999;
		startPos	= 9999;
		endPos		= 0;
		findFlag	= false;

		val_start1	= vision.MilImageBuffer[iCh][pos-width] + vision.MilImageBuffer[iCh][pos-width2] + vision.MilImageBuffer[iCh][pos-width3] + vision.MilImageBuffer[iCh][pos-width4] + vision.MilImageBuffer[iCh][pos-width5];
		val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+width];

		val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-width];
		val_end2	= vision.MilImageBuffer[iCh][pos+width] + vision.MilImageBuffer[iCh][pos+width2] + vision.MilImageBuffer[iCh][pos+width3] + vision.MilImageBuffer[iCh][pos+width4] + vision.MilImageBuffer[iCh][pos+width5];

		for(y=sy; y<ey; y++)
		{
			if( (vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos-width2]-margine)) && (vision.MilImageBuffer[iCh][pos-width]>=(vision.MilImageBuffer[iCh][pos-width3]-margine)) && (vision.MilImageBuffer[iCh][pos-width2]>=(vision.MilImageBuffer[iCh][pos-width4]-margine)) &&
				vision.MilImageBuffer[iCh][pos-width]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end1-val_start1) > startVal)
				{
					startVal = val_end1-val_start1;
					startPos = y;
					inspStartPosY[y]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_MAGENTA);

					findFlag = true;
				}
			}

			if(	findFlag && 
				(	vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos+width2]-margine)) && (vision.MilImageBuffer[iCh][pos+width]>=(vision.MilImageBuffer[iCh][pos+width3]-margine)) && (vision.MilImageBuffer[iCh][pos+width2]>=(vision.MilImageBuffer[iCh][pos+width4]-margine)) &&
				vision.MilImageBuffer[iCh][pos+width]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end2-val_start2) > endVal)
				{
					endVal = val_end2-val_start2;
					endPos = y;
					inspEndPosY[y]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_BLUE);
				}
			}
			pos += width;

			val_start1	= val_start1 + vision.MilImageBuffer[iCh][pos-1] - vision.MilImageBuffer[iCh][pos-6];
			val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

			val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
			val_end2	= val_end2 - vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+5];
		}
	}

	startVal	= -9999;
	endVal		= -9999;
	startPos	= 9999;
	endPos		= 0;

	for(y=sy; y<ey; y++)
	{
		if(inspStartPosY[y]>startVal)
		{
			startVal = inspStartPosY[y];
			startPos = y;
		}

		if(inspEndPosY[y] > endVal)
		{
			endVal = inspEndPosY[y];
			endPos = y;
		}
	}

	if(startVal>endVal)
	{
		endVal		= -9999;
		endPos		= 0;

		for(y=startPos; y<ey; y++)
		{
			if(inspEndPosY[y] > endVal)
			{
				endVal = inspEndPosY[y];
				endPos = y;
			}
		}
	}
	else
	{
		startVal	= -9999;
		startPos	= 9999;

		for(y=sy; y<endPos; y++)
		{
			if(inspStartPosY[y]>startVal)
			{
				startVal = inspStartPosY[y];
				startPos = y;
			}
		}
	}


	//	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	maxVal = 0;

	for (y=startPos; y<=endPos; y++)
	{
		if(Hist[y]>maxVal)
		{
			maxVal = Hist[y];
			topPos = y;
		}
	}
	if(topPos == 0)
	{
		topPos = 92;
	}
	//	topPos = (startPos + endPos + 1) / 2;

	topVal = 0;
	for(x=sx; x<ex; x++)
	{
		pos = topPos*width + x;

		startVal = vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+width];
		if(startVal <= whiteVal2)
		{
			topVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos-width2] + vision.MilImageBuffer[iCh][pos-width3];
		if(startVal <= whiteVal2)
		{
			topVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos+width2] + vision.MilImageBuffer[iCh][pos+width3];
		if(startVal <= whiteVal2)
		{
			topVal++;
			continue;
		}
		continue;
	}

	Task.m_line_cnt[2]	= topVal;
	Task.m_line_pos[2]	= topPos;
	Task.m_f_line_rate[2]= topVal * 100.0f / (inRect.right - inRect.left - 20);

	if(Task.m_f_line_rate[2]>100)
		Task.m_f_line_rate[2] = 100;


	//////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////
	// �Ϻ� ���� ���� �˻� Start..
	sx = (int)(inRect.left + fabs((outRect.left - inRect.left)*0.5));
	ex = (int)(inRect.right - fabs((outRect.right - inRect.right)*0.5));
	sy = inRect.bottom;
	ey = outRect.bottom;


	//	vision.boxlist[iCh].addList(sx, sy, ex, ey, PS_SOLID, M_COLOR_GREEN);

	if (sx<0)					sx = 0;
	if (ex>=CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey>=CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if((ex-sx)<10)
	{
		sTempLang.LoadStringA(IDS_STRING123);
		sLangChange.Format(sTempLang, sx, ex);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING249);
		putListLog(sLangChange);
		return false;
	}

	if((ey-sy)<10)
	{
		sTempLang.LoadStringA(IDS_STRING124);
		sLangChange.Format(sTempLang, sy, ey);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING250);
		putListLog(sLangChange);
		return false;
	}


	minVal = 255;
	maxVal = 0;
	avgVal = 0;

	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	for(y=sy; y<ey; y++)
	{
		sum = 0;
		pos = y*width + sx;

		for(x=sx; x<ex; x++)
		{
			sum += vision.MilImageBuffer[iCh][pos];
			pos++;
		}
		avgVal += sum;

		Hist[y] = sum;

		if(minVal > sum / (ex - sx))				minVal = sum / (ex - sx);
		if(maxVal < sum / (ex - sx))				maxVal = sum / (ex - sx);
	}

	avgVal = avgVal / ((ex-sx) * (ey-sy));

	if(dispMode)
	{
		sTempLang.LoadStringA(IDS_STRING125);
		sLangChange.Format(sTempLang, minVal, maxVal, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 180, szDispData, M_COLOR_GREEN, 17, 8, _T("arialuni"));
	}


	// ���� ������ ��ο� ������ ��� ���� 50 �̻��̰�, ��� ���� 200 ���Ϸ�..
	if( (maxVal-minVal)<70)
	{
		sTempLang.LoadStringA(IDS_STRING127);
		sLangChange.Format(sTempLang, minVal, maxVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		
		sTempLang.LoadStringA(IDS_STRING360);
		sLangChange.Format(sTempLang, minVal, maxVal);

		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n�Ϻ� ���� ������ ��� ���̰� �ʹ� �۽��ϴ�.\n(Min %d, Max %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	if( avgVal>230 ) 	
	{
		sTempLang.LoadStringA(IDS_STRING125);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING361);
		sLangChange.Format(sTempLang, avgVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n�Ϻ� ���� ������ �ʹ� ����ϴ�.\n(Avg %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}

	//if(maxVal<190)	
	if(minVal > 150)
	{
		sTempLang.LoadStringA(IDS_STRING126);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));

		sTempLang.LoadStringA(IDS_STRING359);
		sLangChange.Format(sTempLang, minVal);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n�Ϻ� ���� Line�� �ʹ� ����ϴ�.\n(Min %d)

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);

		return false;
	}



	whiteVal	= (int)(minVal*2 + avgVal*0.5);//(maxVal*0.9 + avgVal*0.1);
//	whiteVal	= (int)(maxVal*0.4 + avgVal*0.6);
	whiteVal2	= whiteVal * 2;
	whiteVal3	= whiteVal * 3;

	width2	= 2*width;
	width3	= 3*width;
	width4	= 4*width;
	width5	= 5*width;

	sx = inRect.left - 5;
	ex = inRect.right + 5;

	sy += 5;
	ey -= 5;

	if(sy>(ey-3))
	{
		sLangChange.LoadStringA(IDS_STRING251);
		_stprintf_s(szDispData, sLangChange, sy-5, ey+5);
		putListLog(szDispData);

		sy = (sy+ey)/2 - 1;
		ey = sy + 3;
	}


	for(x=sx; x<ex; x++)
	{
		pos = sy*width + x;

		startVal	= -9999;
		endVal		= -9999;
		startPos	= 9999;
		endPos		= 0;
		findFlag	= false;

		val_start1	= vision.MilImageBuffer[iCh][pos-width] + vision.MilImageBuffer[iCh][pos-width2] + vision.MilImageBuffer[iCh][pos-width3] + vision.MilImageBuffer[iCh][pos-width4] + vision.MilImageBuffer[iCh][pos-width5];
		val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+width];

		val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-width];
		val_end2	= vision.MilImageBuffer[iCh][pos+width] + vision.MilImageBuffer[iCh][pos+width2] + vision.MilImageBuffer[iCh][pos+width3] + vision.MilImageBuffer[iCh][pos+width4] + vision.MilImageBuffer[iCh][pos+width5];

		for(y=sy; y<ey; y++)
		{
			if(val_end1<300 && (vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos-width2]-margine)) && (vision.MilImageBuffer[iCh][pos-width]>=(vision.MilImageBuffer[iCh][pos-width3]-margine)) && (vision.MilImageBuffer[iCh][pos-width2]>=(vision.MilImageBuffer[iCh][pos-width4]-margine)) &&
				vision.MilImageBuffer[iCh][pos-width]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end1-val_start1) > startVal)
				{
					startVal = val_end1-val_start1;
					startPos = y;
					inspStartPosY[y]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_MAGENTA);

					findFlag = true;
				}
			}

			if(	findFlag && (val_start2<300) &&
				(	vision.MilImageBuffer[iCh][pos]>=(vision.MilImageBuffer[iCh][pos+width2]-margine)) && (vision.MilImageBuffer[iCh][pos+width]>=(vision.MilImageBuffer[iCh][pos+width3]-margine)) && (vision.MilImageBuffer[iCh][pos+width2]>=(vision.MilImageBuffer[iCh][pos+width4]-margine)) &&
				vision.MilImageBuffer[iCh][pos+width]>=whiteVal && vision.MilImageBuffer[iCh][pos]<=whiteVal)
			{
				if( (val_end2-val_start2) > endVal)
				{
					endVal = val_end2-val_start2;
					endPos = y;
					inspEndPosY[y]++;

					if(dispMode==2)
						vision.crosslist[iCh].addList(x, y, 1, M_COLOR_BLUE);
				}
			}
			pos += width;

			val_start1	= val_start1 + vision.MilImageBuffer[iCh][pos-1] - vision.MilImageBuffer[iCh][pos-6];
			val_end1	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos+1];

			val_start2	= vision.MilImageBuffer[iCh][pos+0] + vision.MilImageBuffer[iCh][pos-1];
			val_end2	= val_end2 - vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+5];
		}
	}
	//inspStartPosY[670] = 1;
	//inspEndPosY[670] = 1;

	startVal	= -9999;
	endVal		= -9999;
	startPos	= 9999;
	endPos		= 0;

	for(y=sy; y<ey; y++)
	{
		if(inspStartPosY[y]>startVal)
		{
			startVal = inspStartPosY[y];
			startPos = y;
		}

		if(inspEndPosY[y] > endVal)
		{
			endVal = inspEndPosY[y];
			endPos = y;
		}
	}

	if(startVal>endVal)
	{
		endVal		= -9999;
		endPos		= 0;

		for(y=startPos; y<ey; y++)
		{
			if(inspEndPosY[y] > endVal)
			{
				endVal = inspEndPosY[y];
				endPos = y;
			}
		}
	}
	else
	{
		startVal	= -9999;
		startPos	= 9999;

		for(y=sy; y<endPos; y++)
		{
			if(inspStartPosY[y]>startVal)
			{
				startVal = inspStartPosY[y];
				startPos = y;
			}
		}
	}


	//	memset(Hist, 0x00, sizeof(int)*CAM_SIZE_X);

	maxVal = 0;

	for (y=startPos; y<=endPos; y++)
	{
		if(Hist[y]>maxVal)
		{
			maxVal = Hist[y];
			bottomPos = y;
		}
	}

	if(bottomPos == 0 || bottomPos <= 640)
	{
		bottomPos = 670;
	}
	//	bottomPos = (startPos + endPos + 1) / 2;

	bottomVal = 0;
	for(x=sx; x<ex; x++)
	{
		pos = bottomPos*width + x;

		startVal = vision.MilImageBuffer[iCh][pos] + vision.MilImageBuffer[iCh][pos+width];
		if(startVal <= whiteVal2)
		{
			bottomVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos-width2] + vision.MilImageBuffer[iCh][pos-width3];
		if(startVal <= whiteVal2)
		{
			bottomVal++;
			continue;
		}

		startVal = vision.MilImageBuffer[iCh][pos+width2] + vision.MilImageBuffer[iCh][pos+width3];
		if(startVal <= whiteVal2)
		{
			bottomVal++;
			continue;
		}
	}

	Task.m_line_cnt[3]	= bottomVal;
	Task.m_line_pos[3]	= bottomPos;
	Task.m_f_line_rate[3]= bottomVal * 100.0f / (inRect.right - inRect.left - 20);

	if(Task.m_f_line_rate[3]>100)
		Task.m_f_line_rate[3] = 100;


	Task.m_bResign_Result[index] = true;


	double resinRectSizeY = (bottomPos - topPos) * sysData.dCamResol[iCh].y;

	if (model.m_ResinDrawSize.y*0.8>resinRectSizeY || resinRectSizeY>model.m_ResinDrawSize.y*1.2)
	{
		sTempLang.LoadStringA(IDS_STRING349);
		sLangChange.Format(sTempLang, resinRectSizeX, model.m_ResinDrawSize.x);
		_stprintf_s(szDispData, sLangChange);	//\n[���� �˻�]\n���� ���� ���� �������Դϴ�.\n[���� �� %.01f mm, Spec %.01f mm

		if(autoMode)
			errMsg2(false,szDispData);
		else
			putListLog(szDispData);
		//		return false;
	}



	unsigned long color;

	for(int i=0; i<4; i++)
	{
		if(Task.m_f_line_rate[i]>=i_limit_rate)
			color = M_COLOR_GREEN;
		else
		{
			Task.m_bResign_Result[index] = false;
			color = M_COLOR_RED;
		}

		if (i==0)
		{
			vision.linelist[iCh].addList(Task.m_line_pos[0]-2, Task.m_line_pos[2],	Task.m_line_pos[0]-2, Task.m_line_pos[3], PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0]-1, Task.m_line_pos[2],	Task.m_line_pos[0]-1, Task.m_line_pos[3], PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0]-0, Task.m_line_pos[2],	Task.m_line_pos[0]-0, Task.m_line_pos[3], PS_SOLID, color);
		}
		else if(i==1)
		{
			vision.linelist[iCh].addList(Task.m_line_pos[1]+2, Task.m_line_pos[2],	Task.m_line_pos[1]+2, Task.m_line_pos[3], PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[1]+1, Task.m_line_pos[2],	Task.m_line_pos[1]+1, Task.m_line_pos[3], PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[1]+0, Task.m_line_pos[2],	Task.m_line_pos[1]+0, Task.m_line_pos[3], PS_SOLID, color);
		}
		else if(i==2)
		{
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[2]-2, Task.m_line_pos[1], Task.m_line_pos[2]-2, PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[2]-1, Task.m_line_pos[1], Task.m_line_pos[2]-1, PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[2]-0, Task.m_line_pos[1], Task.m_line_pos[2]-0, PS_SOLID, color);
		}
		else if(i==3)
		{
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[3]+2, Task.m_line_pos[1], Task.m_line_pos[3]+2, PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[3]+1, Task.m_line_pos[1], Task.m_line_pos[3]+1, PS_SOLID, color);
			vision.linelist[iCh].addList(Task.m_line_pos[0],	Task.m_line_pos[3]+0, Task.m_line_pos[1], Task.m_line_pos[3]+0, PS_SOLID, color);
		}


		if(i==0)			sprintf_s(szDispData,  "[ L ] %.01f", Task.m_f_line_rate[i]);
		else if(i==1)		sprintf_s(szDispData,  "[ R ] %.01f", Task.m_f_line_rate[i]);
		else if(i==2)		sprintf_s(szDispData,  "[ T ] %.01f", Task.m_f_line_rate[i]);
		else if(i==3)		sprintf_s(szDispData,  "[ B ] %.01f", Task.m_f_line_rate[i]);

		vision.textlist[iCh].addList(CAM_SIZE_X-150, 650+i*25, szDispData, color, 15, 8, "Arial Black");
	}


	ep2 = myTimer(true);

	sTemp.Format("Insp Time %d msec", (int)((ep2 - ep1)));
	vision.textlist[iCh].addList(50, (CAM_SIZE_Y-60), sTemp, M_COLOR_RED, 24, 10, "Arial");


	vision.drawOverlay(CAM1);
	vision.drawOverlay(CAM2);

	if (!Task.m_bResign_Result[index])
	{
		return false;
	}

	return true; 
}
bool CAABonderDlg::_inspResignHole(bool autoMode, int index, int dispMode)
{
	int iCh = CAM1;
	if (vision.getLiveMode() == 1)
	{
		vision.getSnapImage(CAM1);
	}

	if (Task.m_iStatus_Unit_Epoxy == 1)
	{
		saveInspImage(EPOXY_IMAGE_SAVE, index);
	}

	if (index<0 || index >> 3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}

	Task.m_bResign_Result[index] = false;
	//====================================================�����˻��̹��� �߰�
	double dReduceFactorX = 0.0;
	double dReduceFactorY = 0.0;

	dReduceFactorX = (double)autodispDlg->m_iSizeX_Client / CAM_SIZE_X;
	dReduceFactorY = (double)autodispDlg->m_iSizeY_Client / CAM_SIZE_Y;
	//MimResize(vision.MilGrabImageChild[1], vision.MilDefectImage, dReduceFactorX, dReduceFactorY, M_DEFAULT);
	MimResize(vision.MilGrabImageChild[CAM1], vision.MilOptImage, dReduceFactorX, dReduceFactorY, M_DEFAULT);////���� �̹��� ȭ�鿡 ���÷��� 20180827_2
																											 //====================================================�����˻��̹��� �߰�
																											 //vision.drawOverlay(CCD);
																											 //end
	CPoint inRadius, outRadius;
	float inOffsetX = (float)((model.m_ResinDrawSize.x * 1) - model.m_dResinInspOffset[0].x);	//���� Offset
	float inOffsetY = (float)((model.m_ResinDrawSize.y * 1) - model.m_dResinInspOffset[0].y);
	float outOffsetX = (float)((model.m_ResinDrawSize.x * 1) + model.m_dResinInspOffset[1].x);	//�ٱ��� Offset
	float outOffsetY = (float)((model.m_ResinDrawSize.y * 1) + model.m_dResinInspOffset[1].y);
	inRadius.x = (int)inOffsetX / sysData.dCamResol[iCh].x;
	outRadius.x = (int)outOffsetX / sysData.dCamResol[iCh].x;
	int i_limit_rate = model.m_iResinInspLimit;

	int margine = 5;

	char	szDispData[256];
	CString sTemp = "";
	double ep1 = 0.0;

	ep1 = myTimer(true);

	vision.clearOverlay();

	int x = 0;
	int	y = 0;
	int	pos = 0;
	int	pos2 = 0;
	int	minVal = 0;
	int	maxVal = 0;
	int	avgVal = 0;

	int sx = 0;
	int	sy = 0;
	int	ex = 0;
	int	ey = 0;

	double centX = Task.d_mark_pos_x[PCB_Holder_MARK][0];
	double centY = Task.d_mark_pos_y[PCB_Holder_MARK][0];

	centX += model.dEpoxyOffset_X;
	centY += model.dEpoxyOffset_Y;

	CRect inRect, outRect;
	inRect.left = (int)(centX - inOffsetX / sysData.dCamResol[iCh].x);
	inRect.right = (int)(centX + inOffsetX / sysData.dCamResol[iCh].x);
	inRect.top = (int)(centY - inOffsetY / sysData.dCamResol[iCh].y);
	inRect.bottom = (int)(centY + inOffsetY / sysData.dCamResol[iCh].y);

	outRect.left = (int)(centX - outOffsetX / sysData.dCamResol[iCh].x);
	outRect.right = (int)(centX + outOffsetX / sysData.dCamResol[iCh].x);
	outRect.top = (int)(centY - outOffsetY / sysData.dCamResol[iCh].y);
	outRect.bottom = (int)(centY + outOffsetY / sysData.dCamResol[iCh].y);

	vision.crosslist[iCh].addList((int)centX, (int)centY, 30, M_COLOR_GREEN);	// ���� �˻� �߽� ��ġ..
	vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_CYAN);				// ���� �˻� ����..
	vision.boxlist[iCh].addList(outRect, PS_SOLID, M_COLOR_GREEN);


	int Hist[CAM_SIZE_X] = { 0, };

	int inspStartPosX[CAM_SIZE_X] = { 0, };
	int inspEndPosX[CAM_SIZE_X] = { 0, };
	int inspStartPosY[CAM_SIZE_Y] = { 0, };
	int inspEndPosY[CAM_SIZE_Y] = { 0, };

	int startPoint[CAM_SIZE_X] = { 0, };
	int endPoint[CAM_SIZE_X] = { 0, };

	int leftVal = 0;
	int leftPos = 0;
	int rightVal = 0;
	int rightPos = 0;
	int topVal = 0;
	int topPos = 0;
	int bottomVal = 0;
	int bottomPos = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// �� ���� �˻� Start..
	sx = (int)outRect.left;
	ex = (int)outRect.right;
	sy = (int)outRect.top;
	ey = (int)outRect.bottom;

	if (sx<0)					sx = 0;
	if (ex >= CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey >= CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if ((outOffsetX - inOffsetX) / sysData.dCamResol[iCh].x < 10)
	{
		sTemp.Format("[ ERROR ] Insp ROI size X Error.(%d~%d)", inOffsetX, outOffsetX);
		_stprintf_s(szDispData, sTemp);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		putListLog("[���� �˻�] �˻� ���� ���� �� �̻�");
		return false;
	}
	if ((outOffsetY - inOffsetY) / sysData.dCamResol[iCh].y < 10)
	{
		sTemp.Format("[ ERROR ] Insp ROI size Y Error.(%d~%d)", outOffsetY, inOffsetY);
		_stprintf_s(szDispData, sTemp);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		putListLog("[���� �˻�] �˻� ���� ���� �� �̻�");
		return false;
	}


	//!! Hole�� ������ �������� ��� 0x00���� Image ��ȯ
	double HoleW = 0, HoleH = 0;
	double Length = 0;
	unsigned char *m_Imagebuf;
	m_Imagebuf = (unsigned char *)malloc(CAM_SIZE_X*CAM_SIZE_Y);

	MbufGet(vision.MilGrabImageChild[iCh], m_Imagebuf);	// �̹��� ��ü �ȼ� ��Ⱚ ���� ���


														//!�ܺο��� �������� Buf�� ����.
	CPoint cpCutSize;
	cpCutSize.x = ex - sx;
	cpCutSize.y = ey - sy;

	unsigned char	*cutimgBuf;
	cutimgBuf = (unsigned char *)malloc(cpCutSize.x * cpCutSize.y);
	memset(cutimgBuf, 0, sizeof(cutimgBuf));




	Size size;
	size.width = cpCutSize.x;
	size.height = cpCutSize.y;

	Mat MdstImg = Mat(size, CV_8UC1);
	Mat MsrcImg = Mat(size, CV_8UC1);

	size.width = CAM_SIZE_X;
	size.height = CAM_SIZE_Y;
	Mat MbufImg = Mat(size, CV_8UC1);




	memcpy(MbufImg.data, m_Imagebuf, CAM_SIZE_X * CAM_SIZE_Y);

	//cvSetImageROI(bufImg, cvRect(sx, sy, cpCutSize.x, cpCutSize.y));

	MbufImg = MbufImg(cv::Rect(sx, sy, cpCutSize.x, cpCutSize.y));

	//cvCopy(bufImg, srcImg);
	MbufImg.copyTo(MsrcImg);
	imwrite("D:/EpoxyTest1.jpg", MsrcImg);
	//cvSaveImage("D:/1_cvPolarSrc.bmp", srcImg);	//===============

	size = MsrcImg.size();
	double M = size.width / log(size.width / 2.0);// srcImg->width / log(srcImg->width / 2.0);
												  //cvLogPolar(srcImg, dstImg, cvPoint2D32f(srcImg->width / 2, srcImg->height / 2), M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
												  //cv::logPolar
	cv::logPolar(MsrcImg, MdstImg, cv::Point2f(size.width / 2, size.height / 2), M, cv::INTER_LINEAR | cv::WARP_FILL_OUTLIERS);
	//cvSaveImage("D:/2_cvPolardst.bmp", dstImg);	//===============
	imwrite("D:/EpoxyTest2.jpg", MdstImg);



	int iEpoxySize = (outRect.right - inRect.right);// *0.3;
	unsigned char	*imgInsp;
	imgInsp = (unsigned char *)malloc(iEpoxySize * cpCutSize.y);
	sx = cpCutSize.x - iEpoxySize;

	//polarImg = cvCreateImage(cvSize(iEpoxySize, cpCutSize.y), 8, 1);
	size.width = iEpoxySize;
	size.height = cpCutSize.y;
	Mat MpolarImg = Mat(size, CV_8UC1);


	//cvSetImageROI(dstImg, cvRect(sx, 0, iEpoxySize, cpCutSize.y));
	MdstImg = MdstImg(cv::Rect(sx, 0, iEpoxySize, cpCutSize.y));
	//cvCopy(dstImg, polarImg);
	MdstImg.copyTo(MpolarImg);
	//cvSaveImage("D:/3_cvPolarInsp.bmp", dstImg);	//===============
	imwrite("D:/EpoxyTest3.jpg", MdstImg);


	memcpy(imgInsp, MpolarImg.data, iEpoxySize*cpCutSize.y);

	///---------------------------------------------
	// �˻�� unsigned char	*imgInsp��..
	CPoint inspSize;
	inspSize.x = iEpoxySize;
	inspSize.y = cpCutSize.y;

	minVal = 255;
	maxVal = 0;
	avgVal = 0;
	memset(Hist, 0x00, sizeof(int)*(CAM_SIZE_X));
	int sum = 0;

	int iSpecOverLine = 0;
	int iContinuityLine = 0;

	for (y = 0; y<inspSize.y; y++)
	{
		sum = 0;
		for (x = 0; x<inspSize.x; x++)
		{
			pos = y * inspSize.x + x;
			sum += imgInsp[pos];
		}

		avgVal += sum;
		Hist[x] = sum;

		if (sum / inspSize.x < 255.0 * (model.m_dResinInspHoleSpec / 100.0))
		{
			iContinuityLine++;
			if (iContinuityLine*sysData.dCamResol[iCh].x > model.m_dResinInspGapLength)
			{
				//! 5ȸ �������� ���� ���.
				sTemp.Format("[ Square ] Resign Line Spec Over  Error [14] -  y:%d", y);
				_stprintf_s(szDispData, sTemp);
				vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
				return false;
			}
		}
		else								iContinuityLine = 0;

		if (sum / inspSize.x < 255.0*(model.m_dResinInspHoleSpec / 100.0))
		{
			iSpecOverLine++;
		}
	}

#if 1

#else
	//! �ܺο������۸� cv�� ����.
	IplImage *bufImg, *dstImg, *srcImg, *polarImg;
	srcImg = cvCreateImage(cvSize(cpCutSize.x, cpCutSize.y), 8, 1);
	dstImg = cvCreateImage(cvSize(cpCutSize.x, cpCutSize.y), 8, 1);
	bufImg = cvCreateImage(cvSize(CAM_SIZE_X, CAM_SIZE_Y), 8, 1);

	memcpy(bufImg->imageData, m_Imagebuf, CAM_SIZE_X * CAM_SIZE_Y);

	cvSetImageROI(bufImg, cvRect(sx, sy, cpCutSize.x, cpCutSize.y));
	cvCopy(bufImg, srcImg);

	cvSaveImage("D:/1_cvPolarSrc.bmp", srcImg);	//===============

												//! cv Image Polar ó��
	double M = srcImg->width / log(srcImg->width / 2.0);
	cvLogPolar(srcImg, dstImg, cvPoint2D32f(srcImg->width / 2, srcImg->height / 2), M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvSaveImage("D:/2_cvPolardst.bmp", dstImg);	//===============

												//!�� ������ ���ۿ� �ٽ� ��� -> �� ũ���..
	int iEpoxySize = (outRect.right - inRect.right)*0.6;//outOffsetX -inOffsetX;//(inRect.left - outRect.left) * 1;
	unsigned char	*imgInsp;
	imgInsp = (unsigned char *)malloc(iEpoxySize * cpCutSize.y);
	sx = cpCutSize.x - iEpoxySize;//������ �� outRect.right -iEpoxySize;// 

	polarImg = cvCreateImage(cvSize(iEpoxySize, cpCutSize.y), 8, 1);

	size.width = iEpoxySize;
	size.height = cpCutSize.y;
	Mat MpolarImg = Mat(size, CV_8UC1);
	cvSetImageROI(dstImg, cvRect(sx, 0, iEpoxySize, cpCutSize.y));

	cvCopy(dstImg, polarImg);
	cvSaveImage("D:/3_cvPolarInsp.bmp", dstImg);	//===============

													//!���ۿ� �ٽ� ���
	memcpy(imgInsp, polarImg->imageData, iEpoxySize*cpCutSize.y);

	//! IplImage �޸� ����

	cvReleaseImage(&polarImg);
	cvReleaseImage(&dstImg);
	cvReleaseImage(&srcImg);
	cvResetImageROI(bufImg);
	cvReleaseImage(&bufImg);

	///---------------------------------------------
	// �˻�� unsigned char	*imgInsp��..
	CPoint inspSize;
	inspSize.x = iEpoxySize;
	inspSize.y = cpCutSize.y;

	minVal = 255;
	maxVal = 0;
	avgVal = 0;
	memset(Hist, 0x00, sizeof(int)*(CAM_SIZE_X));
	int sum = 0;


	int iSpecOverLine = 0;
	int iContinuityLine = 0;

	for (y = 0; y<inspSize.y; y++)
	{
		sum = 0;
		for (x = 0; x<inspSize.x; x++)
		{
			pos = y * inspSize.x + x;
			sum += imgInsp[pos];
		}

		avgVal += sum;
		Hist[x] = sum;

		if (sum / inspSize.x < 255.0 * (model.m_dResinInspHoleSpec / 100.0))
		{
			iContinuityLine++;
			if (iContinuityLine*sysData.dCamResol[iCh].x > model.m_dResinInspGapLength)
			{//! 5ȸ �������� ���� ���.
				sTempLang.LoadStringA(IDS_STRING134);
				sLangChange.Format(sTempLang, y);
				_stprintf_s(szDispData, sLangChange);
				vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
				return false;
			}
		}
		else								iContinuityLine = 0;

		if (sum / inspSize.x < 255.0*(model.m_dResinInspHoleSpec / 100.0))		iSpecOverLine++;
	}


	free(cutimgBuf);
	free(m_Imagebuf);
	free(imgInsp);


	sTemp.Empty();
#endif

	return true;
}


bool CAABonderDlg::_inspResignRect(bool autoMode, int index, int iDirection, int iRectCnt)
{//KKYH 20150622 �Ʒ� ���� �Լ� �߰�
	int iCh = 1;

	if( Task.m_iStatus_Unit_Epoxy == 1)
	{
		saveInspImage(EPOXY_IMAGE_SAVE, index);
	}

	if(index<0 || index>>3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}

	double centX = Task.d_mark_pos_x[PCB_Holder_MARK][0];
	double centY = Task.d_mark_pos_y[PCB_Holder_MARK][0];
	if(centX == 0 || centY ==0)
	{
		centX = CAM_SIZE_X/2;
		centY = CAM_SIZE_Y/2;
	}
	centX += model.dEpoxyOffset_X;
	centY += model.dEpoxyOffset_Y;
	int margine = 5;
//	char	szDispData[256];
	CString sTemp;
	double ep1;

	ep1 = myTimer(true);
	int x = 0;
	int	y = 0;
	int	pos = 0;
	int	pos2 = 0;
	int	minVal = 0;
	int	maxVal = 0;
	int	avgVal = 0;

	int iColorLevel=180;
	int iDispenseCnt=0;
	int iIsDispense;
	int iDispenseSpec=5;

	CPoint point;

	if(centX >= 0 || centY >= 0)
	{
		if(iDirection == 0)	// ���ι���
		{
			CRect inRect;
			inRect.left = (int)(centX + model.m_ResingRectStart[iRectCnt].x/sysData.dCamResol[iCh].x );
			inRect.top = (int)(centY + model.m_ResingRectStart[iRectCnt].y/sysData.dCamResol[iCh].y );
			inRect.right = (int)(centX + (model.m_ResingRectStart[iRectCnt].x+ model.m_ResingRectSize[iRectCnt].x)/sysData.dCamResol[iCh].x );
			inRect.bottom = (int)(centY + (model.m_ResingRectStart[iRectCnt].y+ model.m_ResingRectSize[iRectCnt].y)/sysData.dCamResol[iCh].y );


			//!! Image ��ȯ
			double HoleW=0, HoleH=0;
			double Length=0;
			unsigned char *m_Imagebuf;
			m_Imagebuf = (unsigned char *)malloc(CAM_SIZE_X*CAM_SIZE_Y);

			MbufGet(vision.MilGrabImageChild[iCh], m_Imagebuf);	// �̹��� ��ü �ȼ� ��Ⱚ ���� ���

			//! �˻� ������ �ȼ��� ��� �� ���ϱ�
			int iAllSumCnt = 0, iSumCnt = 0;				//! �˻� �������� ��Ⱚ ���� �ȼ� ��ü�� ī��Ʈ
			int sum = 0;									//! �ȼ� ��Ⱚ�� ��
			int whiteVal = 0;								//! ��� �ȼ� ��� �� ��ȯ

			for(y=inRect.top+1; y<inRect.bottom-1; y++)
			{
				iIsDispense=0;
				for(x=inRect.left+1; x<inRect.right-1; x++)
				{
					pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
					if(m_Imagebuf[pos]>iColorLevel)
					{
						iIsDispense=1;
						break;
					}					
					else
					{
						point.x=x;
						point.y=y;
						vision.pixellist[iCh].addList(point, M_COLOR_MAGENTA);
					}
				}
				if(iIsDispense == 0)
				{
					iDispenseCnt++;
					if(iDispenseCnt>=iDispenseSpec)break;
				}
				else iDispenseCnt=0;
			}
			free(m_Imagebuf);
			if(iDispenseCnt >= iDispenseSpec)//-model.m_iResinInspRectSpec)
			{
				vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
//				errMsg2(Task.AutoFlag, _T("���� �ҷ� �߰�"));
				return false;
			}
			else
			{
				vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_GREEN);				// ���� �˻� ����..
				return true;
			}
		}
		else	// ���ι���
		{  
			CRect inRect;
			inRect.left = (int)(centX + model.m_ResingRectStart[iRectCnt].x/sysData.dCamResol[iCh].x );
			inRect.top = (int)(centY + model.m_ResingRectStart[iRectCnt].y/sysData.dCamResol[iCh].y );
			inRect.right = (int)(centX + (model.m_ResingRectStart[iRectCnt].x+ model.m_ResingRectSize[iRectCnt].x)/sysData.dCamResol[iCh].x );
			inRect.bottom = (int)(centY + (model.m_ResingRectStart[iRectCnt].y+ model.m_ResingRectSize[iRectCnt].y)/sysData.dCamResol[iCh].y );

			//!! Image ��ȯ
			double HoleW=0, HoleH=0;
			double Length=0;
			unsigned char *m_Imagebuf;
			m_Imagebuf = (unsigned char *)malloc(CAM_SIZE_X*CAM_SIZE_Y);

			MbufGet(vision.MilGrabImageChild[iCh], m_Imagebuf);	// �̹��� ��ü �ȼ� ��Ⱚ ���� ���

			//! �˻� ������ �ȼ��� ��� �� ���ϱ�
			int iAllSumCnt = 0, iSumCnt = 0;				//! �˻� �������� ��Ⱚ ���� �ȼ� ��ü�� ī��Ʈ
			int sum = 0;									//! �ȼ� ��Ⱚ�� ��
			int whiteVal = 0;								//! ��� �ȼ� ��� �� ��ȯ

			for(x=inRect.left+1; x<inRect.right-1; x++)
			{
				iIsDispense=0;
				for(y=inRect.top+1; y<inRect.bottom-1; y++)
				{
					pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
					if(m_Imagebuf[pos]>iColorLevel)
					{
						iIsDispense=1;
						//break;
					}					
					else
					{
						point.x=x;
						point.y=y;
						vision.pixellist[iCh].addList(point, M_COLOR_MAGENTA);
					}					//! ��� �� ���ۿ� ����
				}
				if(iIsDispense == 0)
				{
					iDispenseCnt++;
					//if(iDispenseCnt>=iDispenseSpec)break;
				}
				else iDispenseCnt=0;
			}
			free(m_Imagebuf);
			if(iDispenseCnt >= iDispenseSpec)//-model.m_iResinInspRectSpec)
			{
				vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
//				errMsg2(Task.AutoFlag, _T("���� �ҷ� �߰�"));
				return false;
			}
			else
			{
				vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_GREEN);				// ���� �˻� ����..
				return true;
			}
		}



	}

	return true;
}


bool CAABonderDlg::_inspResignHole(bool autoMode, int index, int dispMode, int iCirCnt, unsigned char *m_Imagebuf)
{//KKYH 20150622 �Ʒ� ���� �Լ� ����
	int iCh = CAM2;

	if (index<0 || index >> 3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}

	Task.m_bResign_Result[index] = false;

	CPoint inRadius, outRadius;
	float inOffsetX = (float)((model.m_ResinDrawSize.x*0.5) - model.m_dResinInspOffset[0].x);	//���� Offset
	float inOffsetY = (float)((model.m_ResinDrawSize.y*0.5) - model.m_dResinInspOffset[0].y);
	float outOffsetX = (float)((model.m_ResinDrawSize.x*0.5) + model.m_dResinInspOffset[1].x);	//�ٱ��� Offset
	float outOffsetY = (float)((model.m_ResinDrawSize.y*0.5) + model.m_dResinInspOffset[1].y);
	inRadius.x = (int)inOffsetX / sysData.dCamResol[iCh].x;
	outRadius.x = (int)outOffsetX / sysData.dCamResol[iCh].x;
	int i_limit_rate = model.m_iResinInspLimit;

	int margine = 5;

	char	szDispData[256];
	CString sTemp;
	double ep1;

	ep1 = myTimer(true);

	//vision.clearOverlay();

	int x = 0;
	int	y = 0;
	int	pos = 0;
	int	pos2 = 0;
	int	minVal = 0;
	int	maxVal = 0;
	int	avgVal = 0;

	int sx = 0;
	int	sy = 0;
	int	ex = 0;
	int	ey = 0;

	double centX = 512 + model.m_CircleDrawCenter[iCirCnt].x;
	double centY = 384 + model.m_CircleDrawCenter[iCirCnt].y;
	//double centX = Task.d_mark_pos_x[PCB_Chip_MARK][0] + model.m_ResinDrawCenter[iCirCnt].x;
	//double centY = Task.d_mark_pos_y[PCB_Chip_MARK][0] + model.m_ResinDrawCenter[iCirCnt].y;
	//double dCenterX = centX + model.m_ResinDrawCenter[iCirCnt].x;
	//double dCenterY = centX + model.m_ResinDrawCenter[iCirCnt].y;
	CRect inRect, outRect;

	inRect.left = (int)(centX - inOffsetX / sysData.dCamResol[iCh].x - 0.5);
	inRect.right = (int)(centX + inOffsetX / sysData.dCamResol[iCh].x + 0.5);
	inRect.top = (int)(centY - inOffsetY / sysData.dCamResol[iCh].y + 0.5);
	inRect.bottom = (int)(centY + inOffsetY / sysData.dCamResol[iCh].y - 0.5);

	outRect.left = (int)(centX - outOffsetX / sysData.dCamResol[iCh].x - 0.5);
	outRect.right = (int)(centX + outOffsetX / sysData.dCamResol[iCh].x + 0.5);
	outRect.top = (int)(centY - outOffsetY / sysData.dCamResol[iCh].y + 0.5);
	outRect.bottom = (int)(centY + outOffsetY / sysData.dCamResol[iCh].y - 0.5);

	vision.crosslist[iCh].addList((int)centX, (int)centY, 30, M_COLOR_GREEN);	// ���� �˻� �߽� ��ġ..
	vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
	vision.boxlist[iCh].addList(outRect, PS_SOLID, M_COLOR_GREEN);


	int Hist[CAM_SIZE_X] = { 0, };

	int inspStartPosX[CAM_SIZE_X] = { 0, };
	int inspEndPosX[CAM_SIZE_X] = { 0, };
	int inspStartPosY[CAM_SIZE_Y] = { 0, };
	int inspEndPosY[CAM_SIZE_Y] = { 0, };

	int startPoint[CAM_SIZE_X] = { 0, };
	int endPoint[CAM_SIZE_X] = { 0, };

	int leftVal = 0;
	int leftPos = 0;
	int rightVal = 0;
	int rightPos = 0;
	int topVal = 0;
	int topPos = 0;
	int bottomVal = 0;
	int bottomPos = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// �� ���� �˻� Start..
	sx = (int)outRect.left;
	ex = (int)outRect.right;
	sy = (int)outRect.top;
	ey = (int)outRect.bottom;

	if (sx<0)					sx = 0;
	if (ex >= CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey >= CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if ((outOffsetX - inOffsetX) / sysData.dCamResol[iCh].x < 10)
	{
		sprintf_s(szDispData, "[ ERROR ] Insp ROI size X Error. (%f~%f)", inOffsetX, outOffsetX);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, "Arial");
		putListLog("[���� �˻�] �˻� ���� ���� �� �̻�");
		return false;
	}
	if ((outOffsetY - inOffsetY) / sysData.dCamResol[iCh].y < 10)
	{
		sprintf_s(szDispData, "[ ERROR ] Insp ROI size Y Error. (%f~%f)", outOffsetY, inOffsetY);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, "Arial");

		putListLog("[���� �˻�] �˻� ���� ���� �� �̻�");
		return false;
	}


	//!! Hole�� ������ �������� ��� 0x00���� Image ��ȯ
	double HoleW = 0, HoleH = 0;
	double Length = 0;

	//!�ܺο��� �������� Buf�� ����.
	CPoint cpCutSize;
	cpCutSize.x = ex - sx;
	cpCutSize.y = ey - sy;

	unsigned char	*cutimgBuf;
	cutimgBuf = (unsigned char *)malloc(cpCutSize.x * cpCutSize.y);
	memset(cutimgBuf, 0, sizeof(cutimgBuf));

	//! �ܺο������۸� cv�� ����.
	IplImage *bufImg, *dstImg, *srcImg, *polarImg;

	srcImg = cvCreateImage(cvSize(cpCutSize.x, cpCutSize.y), 8, 1);

	dstImg = cvCreateImage(cvSize(cpCutSize.x, cpCutSize.y), 8, 1);
	bufImg = cvCreateImage(cvSize(CAM_SIZE_X, CAM_SIZE_Y), 8, 1);
	memcpy(bufImg->imageData, m_Imagebuf, CAM_SIZE_X * CAM_SIZE_Y);

	cvSetImageROI(bufImg, cvRect(sx, sy, cpCutSize.x, cpCutSize.y));
	cvCopy(bufImg, srcImg);
	CString fileCirName;
	fileCirName.Format(_T("D:/1_cvPolarSrc_%d.bmp"), iCirCnt);
	cvSaveImage(fileCirName, srcImg);	//===============


										//! cv Image Polar ó��
	double M = srcImg->width / log(srcImg->width / 2.0);
	//cvLogPolar(srcImg, dstImg, cvPoint2D32f(srcImg->width/2, srcImg->height/2), M, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);
	cvLogPolar(srcImg, dstImg, cvPoint2D32f(srcImg->width / 2, srcImg->height / 2), M, CV_WARP_INVERSE_MAP + CV_WARP_FILL_OUTLIERS);
	fileCirName.Format(_T("D:/2_cvPolardst_%d.bmp"), iCirCnt);
	cvSaveImage(fileCirName, dstImg);	//===============

										//!�� ������ ���ۿ� �ٽ� ��� -> �� ũ���..
	int iEpoxySize = (inRect.left - outRect.left) * 0.32;//�� ũ�� ���� 0.32
	int iBuffSize = 0;
	unsigned char	*imgInsp;
	int iSizeY = 0;
	int my = cpCutSize.y / 4;
	if (iCirCnt < 1)
	{
		iSizeY = cpCutSize.y / 4; //iSizeY=(iCirCnt < 2)?(cpCutSize.y/4):((cpCutSize.y/4)-10);  ���� �ִ� if��
	}
	else
	{
		iSizeY = (cpCutSize.y / 4) - 10;
	}

	iBuffSize = iEpoxySize * iSizeY;
	imgInsp = (unsigned char *)malloc(iBuffSize);

	sx = cpCutSize.x - iEpoxySize;

	polarImg = cvCreateImage(cvSize(iEpoxySize, iSizeY), 8, 1);

	if (iCirCnt == 0) { cvSetImageROI(dstImg, cvRect(sx, 0 + 1, iEpoxySize, iSizeY)); }
	else if (iCirCnt == 1) { cvSetImageROI(dstImg, cvRect(sx, my + 8, iEpoxySize, iSizeY)); }
	else if (iCirCnt == 2) { cvSetImageROI(dstImg, cvRect(sx, my * 3 + 5, iEpoxySize, iSizeY)); }
	else if (iCirCnt == 3) { cvSetImageROI(dstImg, cvRect(sx, my * 2 + 10, iEpoxySize, iSizeY)); }

	cvCopy(dstImg, polarImg);
	fileCirName.Format(_T("D:/3_cvPolarInsp_%d.bmp"), iCirCnt);
	cvSaveImage(fileCirName, polarImg);

	//!���ۿ� �ٽ� ���
	memcpy(imgInsp, polarImg->imageData, iBuffSize);
	//! IplImage �޸� ����

	cvReleaseImage(&polarImg);
	cvReleaseImage(&dstImg);
	cvReleaseImage(&srcImg);

	cvResetImageROI(bufImg);
	cvReleaseImage(&bufImg);
	///-----------------------------------------------------------------------------------------------------------------------------
	CPoint inspSize;
	inspSize.x = iEpoxySize;
	inspSize.y = iSizeY;//cpCutSize.y;

	minVal = 255;
	maxVal = 0;
	avgVal = 0;
	memset(Hist, 0x00, sizeof(int)*(CAM_SIZE_X));

	int sum = 0;
	int iSpecOverLine = 0;
	int iContinuityLine = 0;
	int iCutErrCount = 0;
	double iCutLength = 0.0;
	for (y = 0; y<inspSize.y; y++)
	{
		sum = 0;
		for (x = 0; x<inspSize.x; x++)
		{
			pos = y * inspSize.x + x;
			sum += imgInsp[pos];
		}
		avgVal += sum;
		Hist[x] = sum;
		if (sum / inspSize.x < 255.0 * (model.m_dResinInspHoleSpec / 100.0))//���庸�� ������
		{
			iContinuityLine++;

			if (iContinuityLine*sysData.dCamResol[iCh].x > model.m_dResinInspGapLength)//! 5ȸ �������� ���� ���.
			{
				sTempLang.LoadStringA(IDS_STRING134);
				sLangChange.Format(sTempLang, y);
				_stprintf_s(szDispData, sLangChange);
				vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
				return false;
			}
		}
		iCutErrCount++;
		if (sum / inspSize.x > 255.0*(model.m_dResinInspHoleSpec / 100.0))		iSpecOverLine++;
	}
	//
	free(cutimgBuf);
	free(imgInsp);
	return true;
}

bool CAABonderDlg::_inspResignRect(bool autoMode, int index, int iDirection, int iRectCnt, unsigned char *m_Imagebuf)
{
	int iCh = index;
	TCHAR szLog[SIZE_OF_1K];
	CString sTemp;
	if( Task.m_iStatus_Unit_Epoxy == 1)
	{
		saveInspImage(EPOXY_IMAGE_SAVE, index);
	}

	if(index<0 || index>>3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}

	double centX = Task.d_mark_pos_x[PCB_Holder_MARK][0];	//PCB_Holder_MARK
	double centY = Task.d_mark_pos_y[PCB_Holder_MARK][0];
	if (centX < 1.0)
	{
		centX = CAM_SIZE_X / 2;
	}
	if (centY < 1.0)
	{
		centY = CAM_SIZE_Y / 2;
	}
	

	int x = 0;
	int	y = 0;
	int	pos = 0;
	int	pos2 = 0;

	
	int iContinuityLine = 0;
	double dBrightLimit = 0;
	bool pass = true;
	
	CRect inRect;

	int ndPosx = 0;
	int ndPosy = 0;
	TCHAR* pszCol[] = { _T("LEFT"), _T("RIGHT"), _T("TOP"), _T("BOTTOM") };
	
	

	//! �˻� ������ �ȼ��� ��� �� ���ϱ�
	int sum = 0;									//! �ȼ� ��Ⱚ�� ��

	dBrightLimit = 255.0 - (255.0 * (model.m_iResinInspRectSpec / 100.0));		//������ ������ �ݴ������
	CPoint point;

	ndPosx = (centX + (model.m_ResingRectStart[iRectCnt].x)) - (model.m_ResingRectSize[iRectCnt].x / 2);
	ndPosy = (centY + (model.m_ResingRectStart[iRectCnt].y)) - (model.m_ResingRectSize[iRectCnt].y / 2);

	inRect.left = (int)ndPosx;
	inRect.top = (int)ndPosy;
	inRect.right = (int)(inRect.left + model.m_ResingRectSize[iRectCnt].x);
	inRect.bottom = (int)(inRect.top + model.m_ResingRectSize[iRectCnt].y);



	if (iRectCnt == 0)
	{
		_stprintf_s(szLog, SIZE_OF_1K, _T("[EpoxyInsp] ��� ����:%.1lf (rgb)"), dBrightLimit);
		putListLog(szLog);
	}
	if(iDirection == 0)	// ���ι���
	{
#if 1
		for (y = inRect.top + 1; y < inRect.bottom - 1; y++)
		{
			sum = 0;
			for (x = inRect.left + 1; x < inRect.right - 1; x++)
			{
				pos = y * CAM_SIZE_X + x;
				sum += m_Imagebuf[pos];
			}
			if (sum / (inRect.right - inRect.left) > dBrightLimit)
			{
				iContinuityLine++;
				vision.linelist[iCh].addList(inRect.left, y, inRect.right , y, PS_SOLID, M_COLOR_BLUE);
			}
		}
		double dline = iContinuityLine*sysData.dCamResol[iCh].y;
		_stprintf_s(szLog, SIZE_OF_1K, _T("[EpoxyInsp] %s ������ ����: %0.5lf / %.3lf(mm)"), pszCol[iRectCnt], dline, model.m_dResinInspGapLength);
		putListLog(szLog);
		if (dline > model.m_dResinInspGapLength)
		{
			pass = false;
		}
#else
		for(y=inRect.top+1; y<inRect.bottom-1; y++)
		{
			iIsDispense=0;
			iDispenseCnt=0;
			for(x=inRect.left+1; x<inRect.right-1; x++)
			{
				pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
				if(m_Imagebuf[pos] > iColorLevel)
				{
					iDispenseCnt++;
				}					
				else
				{
					point.x=x;
					point.y=y;
					vision.pixellist[iCh].addList(point, M_COLOR_RED);
				}					//! ��� �� ���ۿ� ����
			}
			if(iDispenseCnt > iDispenseSpec)//���η� �̵��������� ����ۼ�Ʈ���� �� ��ĥ�� , ���� ���̶� ���� �ݴ�
			{
				pass = false;
					
			}

		}
#endif
	}
	else// ���ι���
	{
#if 1
		for (x = inRect.left + 1; x<inRect.right - 1; x++) 
		{ 
			sum = 0;
			for (y = inRect.top + 1; y<inRect.bottom - 1; y++)
			{
				pos = y * CAM_SIZE_X + x;
				sum += m_Imagebuf[pos];
			}
			if (sum / (inRect.bottom - inRect.top) > dBrightLimit)//255.0 * (model.m_iResinInspRectSpec / 100.0))
			{
				iContinuityLine++;
				vision.linelist[iCh].addList(x, inRect.top, x, inRect.bottom, PS_SOLID, M_COLOR_BLUE);
			}
		}
		double dline = iContinuityLine*sysData.dCamResol[iCh].x;
		_stprintf_s(szLog, SIZE_OF_1K, _T("[EpoxyInsp] %s ������ ����: %0.5lf / %.3lf(mm)"), pszCol[iRectCnt], dline, model.m_dResinInspGapLength);
		putListLog(szLog);
		if (dline > model.m_dResinInspGapLength)
		{
			pass = false;
		}
#else
		for(x=inRect.left+1; x<inRect.right-1; x++)
		{
			iIsDispense=0;
			iDispenseCnt=0;
			for(y=inRect.top+1; y<inRect.bottom-1; y++)
			{
				pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
				if(m_Imagebuf[pos]>iColorLevel)			
				{
					iDispenseCnt++;
				}					
				else
				{
					point.x=x;
					point.y=y;
					vision.pixellist[iCh].addList(point, M_COLOR_RED);
				}					//! ��� �� ���ۿ� ����
			}
			if(iDispenseCnt > iDispenseSpec)//���η� �̵��������� ����ۼ�Ʈ���� �� ��ĥ�� //, ���� ���̶� ���� �ݴ�
			{
				pass = false;
			}

		}
#endif
	}



	if(pass)
	{
		vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_GREEN);				// ���� �˻� ����..
		return true;
	}else
	{
		vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
		return false;
	}
	
	return true;
}

bool CAABonderDlg::_inspResignHole(bool autoMode, int index, int iRectCnt,unsigned char *m_Imagebuf)
{//KKYH 20150622 �Ʒ� ���� �Լ� ����
	int iCh = CAM2;

	if( Task.m_iStatus_Unit_Epoxy == 1)
	{
		saveInspImage(EPOXY_IMAGE_SAVE, index);
	}

	if(index<0 || index>3)
	{
		sLangChange.LoadStringA(IDS_STRING950);	//PCB Index ���� ������ �Դϴ�.
		errMsg2(Task.AutoFlag, sLangChange);
		return false;
	}


	Task.m_bResign_Result[index] = false;

	CPoint inRadius, outRadius;
	float inOffsetX		= (float)((model.m_ResinDrawSize.x*0.5) - model.m_dResinInspOffset[0].x);	//���� Offset
	float inOffsetY		= (float)((model.m_ResinDrawSize.y*0.5) - model.m_dResinInspOffset[0].y);
	float outOffsetX	= (float)((model.m_ResinDrawSize.x*0.5) + model.m_dResinInspOffset[1].x);	//�ٱ��� Offset
	float outOffsetY	= (float)((model.m_ResinDrawSize.y*0.5) + model.m_dResinInspOffset[1].y);
	inRadius.x	= (int)(inOffsetX/ sysData.dCamResol[iCh].x);
	outRadius.x	= (int)(outOffsetX/ sysData.dCamResol[iCh].x);		
	int i_limit_rate	= model.m_iResinInspLimit;

	int margine = 5;

	char	szDispData[256];
	CString sTemp;
	double ep1;

	ep1 = myTimer(true);

	int x = 0;
	int	y = 0;
	int	pos = 0;
	int	pos2 = 0;
	int	minVal = 0;
	int	maxVal = 0;
	int	avgVal = 0;

	int sx = 0;
	int	sy = 0;
	int	ex = 0;
	int	ey = 0;

	double centX = 512 + model.m_CircleDrawCenter[iRectCnt].x + Task.d_Align_offset_x[PCB_Holder_MARK];
	double centY = 384 + model.m_CircleDrawCenter[iRectCnt].y + Task.d_Align_offset_y[PCB_Holder_MARK];

	CRect inRect, outRect;
	inRect.left		= (int)(centX - inOffsetX / sysData.dCamResol[iCh].x - 0.5);
	inRect.right	= (int)(centX + inOffsetX / sysData.dCamResol[iCh].x + 0.5);
	inRect.top		= (int)(centY - inOffsetY / sysData.dCamResol[iCh].y + 0.5);
	inRect.bottom	= (int)(centY + inOffsetY / sysData.dCamResol[iCh].y - 0.5);

	outRect.left	= (int)(centX - outOffsetX / sysData.dCamResol[iCh].x - 0.5);
	outRect.right	= (int)(centX + outOffsetX / sysData.dCamResol[iCh].x + 0.5);
	outRect.top		= (int)(centY - outOffsetY / sysData.dCamResol[iCh].y + 0.5);
	outRect.bottom	= (int)(centY + outOffsetY / sysData.dCamResol[iCh].y - 0.5);

	vision.crosslist[iCh].addList((int)centX, (int)centY, 30, M_COLOR_GREEN);	// ���� �˻� �߽� ��ġ..
	vision.boxlist[iCh].addList(inRect, PS_SOLID, M_COLOR_RED);				// ���� �˻� ����..
	vision.boxlist[iCh].addList(outRect, PS_SOLID, M_COLOR_GREEN);

	int inspStartPosX[CAM_SIZE_X]	= {0, };
	int inspEndPosX[CAM_SIZE_X]		= {0, };
	int inspStartPosY[CAM_SIZE_Y]	= {0, };
	int inspEndPosY[CAM_SIZE_Y]		= {0, };

	int startPoint[CAM_SIZE_X]		= {0, };
	int endPoint[CAM_SIZE_X]		= {0, };

	int leftVal = 0;
	int leftPos = 0;
	int rightVal = 0;
	int rightPos = 0;
	int topVal = 0;
	int topPos = 0;
	int bottomVal = 0;
	int bottomPos = 0;

	//////////////////////////////////////////////////////////////////////////////////
	// �� ���� �˻� Start..
	sx = (int)outRect.left;
	ex = (int)outRect.right;
	sy = (int)outRect.top;
	ey = (int)outRect.bottom;

	if (sx<0)					sx = 0;
	if (ex>=CAM_SIZE_X)			ex = CAM_SIZE_X - 1;
	if (sy<0)					sy = 0;
	if (ey>=CAM_SIZE_Y)			ey = CAM_SIZE_Y - 1;

	if( (outOffsetX-inOffsetX) / sysData.dCamResol[iCh].x < 10)
	{
		sTempLang.LoadStringA(IDS_STRING132);
		sLangChange.Format(sTempLang, inOffsetX, outOffsetX);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING240);
		putListLog(sLangChange);
		return false;
	}
	if((outOffsetY-inOffsetY) / sysData.dCamResol[iCh].y < 10)
	{
		sTempLang.LoadStringA(IDS_STRING133);
		sLangChange.Format(sTempLang, outOffsetY, inOffsetY);
		_stprintf_s(szDispData, sLangChange);
		vision.textlist[iCh].addList(50, 690, szDispData, M_COLOR_RED, 17, 8, _T("arialuni"));
		sLangChange.LoadStringA(IDS_STRING241);
		putListLog(sLangChange);
		return false;
	}


	int iSpecOverLine = 0;
	int iCnt=0;

	{
		float sa=model.m_ResingStartAngle[iRectCnt];
		float ea=model.m_ResingFinishAngle[iRectCnt];
		float r=outRadius.x;
		float r2=inRadius.x;
		float sx,sy,ex,ey;
		float step=2*PI/(r*PI*1.5);


		CPoint point;

		if(ea<sa)
		{
			for(float theta=(sa/360)*(2*PI);theta<((360/360)*(2*PI));theta+=step)
			{
				sx=centX + r*sin(theta);
				sy=centY + r*cos(theta);

				ex=centX + r2*sin(theta);
				ey=centY + r2*cos(theta);
				if(!_inspLine(sx,sy,ex,ey,m_Imagebuf))
				{
					iSpecOverLine++;

					point.x=sx;
					point.y=sy;
					vision.pixellist[CAM2].addList(point, M_COLOR_RED);
				}

			}
			for(float theta=(0/360)*(2*PI);theta<((ea/360)*(2*PI));theta+=step)
			{
				sx=centX + r*sin(theta);
				sy=centY + r*cos(theta);

				ex=centX + r2*sin(theta);
				ey=centY + r2*cos(theta);
				if(!_inspLine(sx,sy,ex,ey,m_Imagebuf))
				{
					iSpecOverLine++;

					point.x=sx;
					point.y=sy;
					vision.pixellist[CAM2].addList(point, M_COLOR_RED);
				}
			}
		}
		else
		{
			for(float theta=(sa/360)*(2*PI);theta<((ea/360)*(2*PI));theta+=step)
			{
				sx=centX + r*sin(theta);
				sy=centY + r*cos(theta);

				ex=centX + r2*sin(theta);
				ey=centY + r2*cos(theta);

				if(!_inspLine(sx,sy,ex,ey,m_Imagebuf))
				{
					iSpecOverLine++;

					point.x=sx;
					point.y=sy;
					vision.pixellist[CAM2].addList(point, M_COLOR_RED);
				}
			}
		}
	}
	return iSpecOverLine<1;
}

bool CAABonderDlg::_inspLine(int sx, int sy, int ex, int ey, unsigned char *m_Imagebuf)
{
	int w=abs(ex-sx);
	int h=abs(ey-sy);

	int x,y;
	int dx=(ex>sx?1:-1);
	int dy=(ey>sy?1:-1);

	int iColorLevel=model.m_ResingInspLevel;
	int iDispenseCnt=0;
	int iIsDispense=0;
	int iDispenseSpec=10;
	int pos;
	CPoint point;

	point.x=sx;
	point.y=sy;
	vision.pixellist[CAM2].addList(point, M_COLOR_CYAN);

	point.x=ex;
	point.y=ey;
	vision.pixellist[CAM2].addList(point, M_COLOR_BLUE);
	 
	bool bSetPoint=true;
	
	if(w>=h)
	{
		for(int i=0;i<w;i++)
		{
			x=sx+i*dx;
			y=sy+i*((float)h/(float)w)*dy;

			pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
			if(m_Imagebuf[pos]>iColorLevel)
			{
				iDispenseCnt++;
			}
			else
			{
				point.x=x;
				point.y=y;
				if(!bSetPoint)
				{
					vision.pixellist[CAM2].addList(point, M_COLOR_MAGENTA);
					bSetPoint=true;
				}
				m_Imagebuf[pos]=0;
			}
			//			m_Imagebuf[pos]=0;
		}
	}
	else
	{
		for(int i=0;i<h;i++)
		{
			x=sx+i*((float)w/(float)h)*dx;
			y=sy+i*dy;
			pos = y* CAM_SIZE_X + x;				//! ��� ���� ���� �ȼ��� ��ġ ��
			if(m_Imagebuf[pos]>iColorLevel)
			{
				iDispenseCnt++;
			}
			else
			{
				point.x=x;
				point.y=y;
				if(!bSetPoint)
				{
					vision.pixellist[CAM2].addList(point, M_COLOR_MAGENTA);
					bSetPoint=true;
				}
				m_Imagebuf[pos]=0;
			}
			//			m_Imagebuf[pos]=0;
		}
	}

	return iDispenseCnt>=iDispenseSpec;
}


void CAABonderDlg::OnBnClickedButtonMinimize()
{
	SendMessage(WM_SYSCOMMAND,SC_MINIMIZE);
}

void CAABonderDlg::func_Set_SFR_N_Type()
{
		dSFR_N_4_PositionX = model.m_Line_Pulse;
		dSFR_N_8_PositionX = model.m_Line_Pulse;
}


void CAABonderDlg::func_Socket_Barcode()
{
	
	//! Data �ʱ�ȭ
	Task.iRecvLenACK[0] = Task.iRecvLenACK[1] = 0;
	Task.dTiltingManual[0] = Task.dTiltingManual[1] = 0.0;
	Task.dAlignManual[0] = Task.dAlignManual[0] = Task.dAlignManual[0] = 0.0;
	Task.iRecvLenCnt[0] = Task.iRecvLenCnt[1] = Task.iRecvLenCnt[2] = Task.iRecvLenCnt[3] = Task.iRecvLenCnt[4] = -1;

}

void CAABonderDlg::OnBnClickedButtonTimeCheck()
{		
	CString str, strTime;
	GetDlgItem(IDC_BUTTON_TIME_CHECK)->GetWindowText(str);

	sLangChange.LoadStringA(IDS_STRING538); //EPOXY TIME START
	if(str == sLangChange)
	{
		sLangChange.LoadStringA(IDS_STRING1269);	//������ �����Ͻðڽ��ϱ�?
		if(askMsg(sLangChange))
		{
			today = CTime::GetCurrentTime();

			work.m_Epoxy_Time_Flag = 1;
			work.m_Epoxy_Time_Check_Year = today.GetYear();
			work.m_Epoxy_Time_Check_Month = today.GetMonth();
			work.m_Epoxy_Time_Check_Day = today.GetDay();
			work.m_Epoxy_Time_Check_Hour = today.GetHour();
			work.m_Epoxy_Time_Check_Min = today.GetMinute();
			work.Save();

			sLangChange.LoadStringA(IDS_STRING1451);
			GetDlgItem(IDC_BUTTON_TIME_CHECK)->SetWindowText(sLangChange);
			m_EpoxyTimeCheck.m_iStateBtn = 1;
		}
	}
	else
	{
		sLangChange.LoadStringA(IDS_STRING1270);
		if(askMsg(sLangChange))	//"������ ���� �Ͻðڽ��ϱ�?"
		{
			work.m_Epoxy_Time_Flag = 0;

			work.m_Epoxy_Time_Check_Year = 0;
			work.m_Epoxy_Time_Check_Month = 0;
			work.m_Epoxy_Time_Check_Day = 0;
			work.m_Epoxy_Time_Check_Hour = 0;
			work.m_Epoxy_Time_Check_Min = 0;

			work.Save();
			sLangChange.LoadStringA(IDS_STRING538);
			GetDlgItem(IDC_BUTTON_TIME_CHECK)->SetWindowText(sLangChange);
			m_EpoxyTimeCheck.m_iStateBtn = 0;
		}
	}

	m_EpoxyTimeCheck.Invalidate();
}


bool CAABonderDlg::EpoxyTimeCheck(bool TimeFlag)
{
	// 20141119 LHC - �ڵ����� ���� ������ ��ü �ð� üũ�ϱ�
	CString str;
	int ResultTimeHour, ResultTimeMin;

	if(work.m_Epoxy_Time_Flag == 1)
	{
		CTime today2;
		CTime StartTime(work.m_Epoxy_Time_Check_Year, work.m_Epoxy_Time_Check_Month, work.m_Epoxy_Time_Check_Day, work.m_Epoxy_Time_Check_Hour, work.m_Epoxy_Time_Check_Min, 0);
		CTimeSpan ChkTime;
		today2 = CTime::GetCurrentTime();
		if(sysData.m_Epoxy_Change_Count < 60)
		{
			ResultTimeHour = 0;
			ResultTimeMin = sysData.m_Epoxy_Change_Count;
		}
		else
		{
			ResultTimeHour = sysData.m_Epoxy_Change_Count / 60;
			ResultTimeMin = sysData.m_Epoxy_Change_Count % 60;
		}
		
		today2 -= CTimeSpan(0, ResultTimeHour, ResultTimeMin,0);

		ChkTime = StartTime - today2;

		if(ChkTime <= 0)
		{
			Dio.setAlarm(ALARM_ON);
			sLangChange.LoadStringA(IDS_STRING540);	//Epoxy ��ü���� %d���� �������ϴ�\n ��ü �Ϸ� �� YES ��ư�� �����ּ���.\n [YES : ��ü �Ϸ� NO : ��ü �̿Ϸ�]
			str.Format(sLangChange,sysData.m_Epoxy_Change_Count);
		
			if(askMsg(str))
			{
				today = CTime::GetCurrentTime();
				work.m_Epoxy_Time_Flag = 1;
				work.m_Epoxy_Time_Check_Year = today.GetYear();
				work.m_Epoxy_Time_Check_Month = today.GetMonth();
				work.m_Epoxy_Time_Check_Day = today.GetDay();
				work.m_Epoxy_Time_Check_Hour = today.GetHour();
				work.m_Epoxy_Time_Check_Min = today.GetMinute();
				work.Save();
				Dio.setAlarm(ALARM_OFF);
			}
			else
			{			
				sLangChange.LoadStringA(IDS_STRING538);
				GetDlgItem(IDC_BUTTON_TIME_CHECK)->SetWindowText(sLangChange);
				work.m_Epoxy_Time_Flag = 0;
				work.m_Epoxy_Time_Check_Year = 0;
				work.m_Epoxy_Time_Check_Month = 0;
				work.m_Epoxy_Time_Check_Day = 0;
				work.m_Epoxy_Time_Check_Hour = 0;
				work.m_Epoxy_Time_Check_Min = 0;
				m_EpoxyTimeCheck.m_iStateBtn = 0;
				m_EpoxyTimeCheck.Invalidate();
				work.Save();
				Dio.setAlarm(ALARM_OFF);
				return false;
			}
		}
	}

	return true;
}


void CAABonderDlg::OnStnClickedLabelLotName()
{
	CKeyPadDlg keyDlg;
	CString strTemp;
	sLangChange.LoadStringA(IDS_STRING757);	//Lot���� �Է��ϼ���.
	keyDlg.m_strKeyPad = sLangChange;

	if (keyDlg.DoModal()==IDOK)
	{
		strTemp = keyDlg.GetstrKeypad();
		int strLength = strTemp.GetLength();
		if(strLength<1)
		{
			sLangChange.LoadStringA(IDS_STRING758);	//Lot���� �������Դϴ�.
			errMsg2(Task.AutoFlag, sLangChange);
			return;
		}
		sprintf_s(Task.LotNo, strTemp, sizeof(strTemp) );
		sprintf_s(Task.sNum, strTemp, sizeof(strTemp) );

		m_labelLotName.SetText(strTemp);
	}
	else
	{
		return;
	}
}

//141201 �ڼ��� [SocketCommunication] Added
void CAABonderDlg::OnBnClickedServerStart()
{
	//func_HidataSocket_ServerStart(8000);
}

void CAABonderDlg::OnBnClickedServerStop()
{
	//func_HidataSocket_ServerEnd();
}


void CAABonderDlg::OnBnClickedClientConnect()
{
	MESConnectToServer();
	/*
	CString sLog;
	if(m_SocketMes.Connect(sysData.m_Mes_Ip_Number, sysData.m_Mes_Port_Number) == FALSE)
	{
		sLangChange.LoadStringA(IDS_STRING771);	//"MES�� ���� ����"
		sLog.Format(sLangChange);
		putListLog(sLog);
	}
	else
	{
		sLangChange.LoadStringA(IDS_STRING770);	//"MES�� ���� ����"
		sLog.Format(sLangChange);
		putListLog(sLog);
	}*/
}


void CAABonderDlg::OnBnClickedClientDisconnect()
{
	//func_HidataSocket_ClientDisConnect();
}


void CAABonderDlg::OnBnClickedClientSend()
{
	//func_HiSocket_ClientSend("T*");
}

void CAABonderDlg::MESConnectToServer()
{
	return;
	CString sLog;
	if (m_SocketMes != NULL)
	{
		m_SocketMes.ShutDown();
		Sleep(1000);
		m_SocketMes.Close();
	}
	if(!m_SocketMes.Create())
	{
		sLangChange.LoadStringA(IDS_STRING1085);	//Socket ���� ����
		sLog.Format(sLangChange);
		putListLog(sLog);
		return;
	}
	if(sysData.m_iProductComp == 1)
	{
		if(m_SocketMes.Connect(sysData.m_Mes_Ip_Number, sysData.m_Mes_Comp_Port_Number) == FALSE)
		{
			sLog.Format("MES ���� ����. MES����,IP, PORT Ȯ�� �� �ּ���.");
			putListLog(sLog);
			m_labelMes.SetBkColor(M_COLOR_RED);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING770);	//"MES�� ���� ����"
			sLog.Format(sLangChange);
			putListLog(sLog);
			m_labelMes.SetBkColor(M_COLOR_GREEN);
		}
		m_labelMes.Invalidate();
	}
	else
	{
		if(m_SocketMes.Connect(sysData.m_Mes_Ip_Number, sysData.m_Mes_Port_Number) == FALSE)
		{
			//sLangChange.LoadStringA(IDS_STRING772);	//MES�� ���� ����. MES���� �� IP, PORT ��ȣ�� Ȯ�� �� �ּ���.
			sLog.Format("MES ���� ����. MES����,IP, PORT Ȯ�� �� �ּ���.");
			putListLog(sLog);
			m_labelMes.SetBkColor(M_COLOR_RED);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING770);	//"MES�� ���� ����"
			sLog.Format(sLangChange);
			putListLog(sLog);
			m_labelMes.SetBkColor(M_COLOR_GREEN);
			
		}
		m_labelMes.Invalidate();
	}

}

void CAABonderDlg::OnBnClickedButtonProcomp()
{
	if(Task.AutoFlag==1 )
	{
		sLangChange.LoadStringA(IDS_STRING1368);	//�ڵ� ���� �� �Դϴ�.
		delayMsg(sLangChange, 3000, M_COLOR_RED);
	}
	else if(Task.AutoFlag==0 || Task.AutoFlag==2)
	{	
		if(sysData.m_iProductComp == 0)
		{
			sLangChange.LoadStringA(IDS_STRING1308);	//����ǰ ���� ���� �Ͻðڽ��ϱ�?
			if(askMsg(sLangChange))
			{
				sysData.m_iProductComp = 1;
				sysData.Save();
			}
		}
		else if(sysData.m_iProductComp == 1)	
		{
			sLangChange.LoadStringA(IDS_STRING376);
			if(askMsg(sLangChange))
			{
				m_bProComp = false;
				sysData.m_iProductComp = 0;
				sysData.Save();
			}
		} 
	}
}

bool CAABonderDlg::func_Insp_CurrentMeasure(bool bLogDraw, bool bAutoMode)
{
	return false;
}


void CAABonderDlg::OnStnClickedLabelId()
{
	CKeyPadDlg keyDlg;
	CString strTemp;
	//strTemp.Format("ID�� �Է��ϼ���.");
	//strTemp.Empty;
	//keyDlg.m_strKeyPad.Format("%s", Task.ChipID);
	keyDlg.m_strKeyPad = strTemp;
	keyDlg.m_FirstFlag = FALSE;
	if (keyDlg.DoModal()==IDOK)
	{
		strTemp = keyDlg.GetstrKeypad();
		int strLength = strTemp.GetLength();
		if(strLength<1)
		{
			strTemp.Format("ID���� �������Դϴ�.");
			errMsg2(Task.AutoFlag, strTemp);
			return;
		}
		Task.m_bPBStart = 1;	//���ڵ� ���������� ���� ������.		 
		sprintf_s(Task.ChipID, strTemp, sizeof(strTemp) );
		sprintf_s(Task.sNum, strTemp, sizeof(strTemp) );

		 m_labelCCD_ID.SetText(strTemp);
	}
	else
	{
		return;
	}
}



void CAABonderDlg::OnBnClickedAutorunStart()
{
	Start_Btn_On = true;
	Sleep(1000);
	Start_Btn_On = false;
}


void CAABonderDlg::OnBnClickedDoorOpen()
{
	Dio.DoorLift(true, false);

	
}


void CAABonderDlg::OnBnClickedDoorClose()
{
	Dio.DoorLift(false, false);//
}


void CAABonderDlg::OnStnClickedLabelStatusMes()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString sMsg = "";

	barcode.func_Comm_Close();
	sMsg.Format("Barcode Close");
	Task.bConnectBarcode = false;
	theApp.MainDlg->putListLog(sMsg);
	Sleep(600);
	//
	MESConnectToServer();
}


void CAABonderDlg::OnStnClickedLabelModelname()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

//	CString sTemp;
//	CString str;
//	model.d_SIDE_MODEL_CH++;
//	if (model.d_SIDE_MODEL_CH > 1)
//	{
//		model.d_SIDE_MODEL_CH = 0;
//	}
//	if (model.d_SIDE_MODEL_CH == SIDE_FRONT)
//	{
//		sTemp.Format(" SIDE FRONT");
//	}
//	else
//	{
//		sTemp.Format("SIDE BACK");
//	}
//	DispCurModelName(sTemp);// model.name);
//
//	str.Format("SIDE MODEL CHANGE [%s]", sTemp);
//	theApp.MainDlg->putListLog(str);
//	model.Save();
//}
}


void CAABonderDlg::ChartRoiReset()
{
#ifdef ON_LINE_VISION
	ccdDlg->m_pSFRDlg->ComboxValueSet();
	vision.clearOverlay(CCD);
	ccdDlg->m_pSFRDlg->drawRectSFR();
	vision.drawOverlay(CCD);
#endif
}
void CAABonderDlg::OnBnClickedButtonSminiOqmode()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (Task.AutoFlag == 1)
	{
		delayMsg("�ڵ� ���� �� �Դϴ�.", 3000, M_COLOR_RED);
		return;
	}
	if (Task.AutoFlag == 2)
	{
		delayMsg("�Ͻ� ���� �� �Դϴ�.", 3000, M_COLOR_RED);
		return;
	}
	sysData.m_testRunMode = !sysData.m_testRunMode;

	if (sysData.m_testRunMode == true)
	{
		m_bSminiOQCheck.m_iStateBtn = 1;
	}
	else
	{
		m_bSminiOQCheck.m_iStateBtn = 0;
	}
	

	/*if (sysData.m_iSminiOQMOde == 0)
	{
		sysData.m_iSminiOQMOde = 1;
		m_bSminiOQCheck.m_iStateBtn = 1;
	}
	else
	{
		sysData.m_iSminiOQMOde = 0;
		m_bSminiOQCheck.m_iStateBtn = 0;
	}*/
	m_bSminiOQCheck.Invalidate();
}


void CAABonderDlg::OnBnClickedButtonSetDlg()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CInfoDlg infoDlg;
	infoDlg.DoModal();
}


void CAABonderDlg::OnBnClickedCheckLogView()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bLogView = !m_bLogView;

	if (m_bLogView)
	{
		//drawLine_MeasureDist(0);
		Task.bSfrLogView = true;
	}
	else
	{
		Task.bSfrLogView = false;
		//vision.clearOverlay(m_iCurCamNo);
		//vision.drawOverlay(m_iCurCamNo);
	}
}


void CAABonderDlg::OnBnClickedCheckBcrCount()
{
	// TODO: Add your control notification handler code here
	m_bBcrAutoCount = !m_bBcrAutoCount;

	if (m_bBcrAutoCount)
	{
		//drawLine_MeasureDist(0);
		Task.bBcrAutoCount = true;
	}
	else
	{
		Task.bBcrAutoCount = false;
		//vision.clearOverlay(m_iCurCamNo);
		//vision.drawOverlay(m_iCurCamNo);
	}
}


void CAABonderDlg::OnBnClickedButtonProcHolder()
{
	// TODO: Add your control notification handler code here
	CString logStr = "";

	if (Task.AutoFlag == 1)
	{
		delayMsg("�ڵ� ���� �� �Դϴ�.", 3000, M_COLOR_RED);
		return;
	}
	if (Task.AutoFlag == 2)
	{
		delayMsg("�Ͻ� ���� �� �Դϴ�.", 3000, M_COLOR_RED);
		return;
	}

	logStr.Format("HOLDER BONDING ���� �����Ͻðڽ��ϱ�?");
	if (askMsg(logStr)) 
	{
		sysData.m_iProductHolderBoning = 1;
	}
	else 
	{
		sysData.m_iProductHolderBoning = -1;
	}


}
