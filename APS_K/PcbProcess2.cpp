#include "StdAfx.h"
#include "PcbProcess2.h"

CWinThread* pThread_TaskCCD = NULL;
bool bThreadCCDRun = false;
typedef struct THREADV
{
	int t_Unit;
}_threadV;
//
UINT Thread_TaskCCD(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
	CString logStr = "";


	bThreadCCDRun = true;			//DRY_RUN
	if (!pFrame->MIUCheck_process())		// || gMIUDevice.CurrentState != 4)
	{
		logStr.Format("CCD ��� ���� �ʱ�ȭ ����.\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.");
		errMsg2(Task.AutoFlag, logStr);
		bThreadCCDRun = false;
		return false;
	}
	Sleep(300);
	bThreadCCDRun = false;


	return true;
}
CPcbProcess2::CPcbProcess2(void)
{
	dwTickStartRun = false;

}


CPcbProcess2::~CPcbProcess2(void)
{
}

void CPcbProcess2::putListLog(CString strLog)
{
	theApp.MainDlg->putListLog(strLog);
}

int CPcbProcess2::Ready_process(int iStep)
{
	CString sLog = "";
	int iRtnFunction = iStep;

	switch (iStep)
	{
	case 10000:
		Task.PCBTaskTime = myTimer(true);
		vision.clearOverlay(CCD);

		if (sysData.m_iIrChartUse == 1)
		{
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP2_CHART]);
		}
		else
		{
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, 0);
			Sleep(100);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
		}
		iRtnFunction = 10150;
		break;


	case  10150:
		if (Dio.LensMotorGripCheck(true, false))
		{
			if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
			{
				iRtnFunction = 10160;
				sLog.Format("		Ready Step [%d]", iRtnFunction);
				putListLog(sLog);
			}
			else
			{
				sLog.Format("[���� �غ�] Lens Grip ���� ���� �Դϴ�[%d]\n����ѱ���� Ȯ�ιٶ��ϴ�", iStep);
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -10150;
				break;
			}
		}
		else
		{
			if (!Dio.LensMotorGripCheck(false, false))
			{
				sLog.Format("[���� �غ�]Lens Grip ���� Ȯ�� ���� [%d]", iStep);
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -10150;
			}
			else
			{
				if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
				{
					sLog.Format("[���� �غ�]Lens Grip ���� ���� [%d]]\n����ѱ���� Ȯ�ιٶ��ϴ�", iStep);
					errMsg2(Task.AutoFlag, sLog);
					iRtnFunction = -10150;
				}
				else
				{
					iRtnFunction = 10160;
					sLog.Format("		Ready Step [%d]", iRtnFunction);
					putListLog(sLog);
				}

			}
		}
		break;

	case 10160:

		iRtnFunction = 10170;
		break;

	case 10170:
		iRtnFunction = 10200;
		break;

	case 10200:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))		//PCB Z�� ��� ��ġ �̵�
		{
			iRtnFunction = 10300;
		}
		else
		{
			sLog.Format("[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]", iStep);	//"[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10200;
		}

		break;

	case 10300:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))		//Lens Z�� ��� ��ġ
		{
			iRtnFunction = 10400;
		}
		else
		{
			sLog.Format("Lens Z�� �����ġ �̵� ����[%d]", iStep);	//"[���� �غ�] Lens Z�� �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10300;
		}
		break;
	case 10400:
		iRtnFunction = 10600;
		break;
	case 10600:
		if (motor.Lens_Motor_MoveX(Wait_Pos))		//Lens X �� ��� ��ġ
		{
			iRtnFunction = 10601;
		}
		else
		{
			sLog.Format("LensLaserAlign Z�� �����ġ �̵� ����[%d]", iStep);	//"[���� �غ�] Lens Z�� �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10600;
		}
		break;
	case 10601:
		iRtnFunction = 10602;
		break;
	case 10602:
		if (motor.Pcb_Motor_Move(Wait_Pos))			//PCB�� ���� �����ġ �̵�
		{
			iRtnFunction = 10700;
		}
		else
		{
			sLog.Format("PCB �����ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -10602;
		}
		break;
	case 10700:
		iRtnFunction = 10701;
		break;
	case 10701:
		if (motor.Lens_Motor_MoveXY(Wait_Pos))			//Lens�� ���� ��� ��ġ �̵�(Lens X/Y/Z��)
		{
			iRtnFunction = 10750;
		}
		else
		{
			sLog.Format(" Lens �����ġ �̵� ����[%d]", iStep);	//"[���� �غ�] Lens �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10701;
		}
		break;
	case 10750:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10800;
		}
		else
		{
			sLog.Format("PCB�� Tx, Ty�� ��� ��ġ �̵� ����");
			putListLog(sLog);
			iRtnFunction = -10750;
		}
		break;
	case 10800:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10900;
		}
		else
		{
			sLog.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� ����");
			putListLog(sLog);
			iRtnFunction = -10800;
		}
		break;
	case  10900:
		//Dio.LensVacuumOn(false);
		iRtnFunction = 10901;
		break;
	case  10901:
		iRtnFunction = 10950;
		break;
	case 10950:
		Dio.HolderGrip(false, false);
		sLog.Format("Holder UnGrip ���� [%d]", iRtnFunction);
		theApp.MainDlg->putListLog(sLog);

		iRtnFunction = 10970;
		break;
	case 10970:
		MIU.Close();
		iRtnFunction = 15000;	//While�� ����
		break;
	case 15000:
		//UVCommand.UV_Shutter_PowerSet(model.UV_Power);

		//sLog.Format("	#1 UV POWER SET: %d", model.UV_Power);
		//putListLog(sLog);
		Sleep(500);
		//UVCommand2.UV_Shutter_PowerSet(model.UV_Power);
		UVCommand.UV_Shutter_PowerSet(model.UV_Power);
		Dio.DoorLift(true, false);
		//UVCommand.UV_Shutter_Open();  //TRI UV
		//sLog.Format("	#2 UV POWER SET: %d", model.UV_Power);
		//putListLog(sLog);
		iRtnFunction = 19000;
		break;

	case 19000:
		Task.AutoReday = 1;
		iRtnFunction = 19900;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	}
	return iRtnFunction;
}

int CPcbProcess2::RunProc_ProductLoading(int iUseStep)
{//! �۾��� ��ǰ ���� �ڵ� Step		(10000 ~ 10999)
	int iRtnFunction = iUseStep;
	CString logStr = "", sBarCode = "";
	bool bChk = true;
	bool bRetVal = false;
	switch (iUseStep)
	{
	case 10000:
		Task.interlockLens = 0;
		Task.interlockPcb = 0;
		iRtnFunction = 10010;
		break;
	case 10010:
		iRtnFunction = 10030;
		break;
	case 10030:
		iRtnFunction = 10050;
		break;
	case 10050:
		//LightControl.ctrlLedVolume(LIGHT_OC_6500K, 0);		// Align�� ���� ON
		Task.PCBTaskTime = myTimer(true);

		iRtnFunction = 10060;
		break;

	case 10060:
		if (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y) && motor.checkLensMotorPos(Wait_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 10070;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			logStr.Format(_T("Lens x,y axis ���� Ȯ�� ����[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10060;
			break;
		}
		break;
	case 10070:
		iRtnFunction = 10100;
		break;
	case 10100://! Chip ���� Draw
		//theApp.MainDlg->func_ChipID_Draw();
		iRtnFunction = 10101;

		logStr.Format("		Start ��ư�� �����ּ���[%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		theApp.MainDlg->m_btnStart.m_iStateBtn = 4;
		theApp.MainDlg->m_btnStart.Invalidate();
		break;
	case 10101://! Start P/B Button Push ���. 
		if (Dio.StartPBOnCheck(true, false))//
		{
			Dio.PCBvaccumOn(VACCUM_ON, false);      //����
			iRtnFunction = 10102;
			break;
		}
		break;
	case 10102:
		if (Dio.PCBvaccumOnCheck(true, false) == false)		//DRY_RUN
		{
			logStr.Format("PCB ���� ���� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10102;
			break;
		}
		logStr.Format("PCB ���� ���� Ȯ�� �Ϸ� [%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		iRtnFunction = 10110;
		break;
	case 10110://! Start P/B Button Push ���. 
		if (Dio.StartPBOnCheck(true, false))// || Start_Btn_On)	//���α׷��� start ��ư �߰�
		{

			iRtnFunction = 10165;
			MbufClear(vision.MilOptImage, 0);

			theApp.MainDlg->m_btnStart.m_iStateBtn = 0;
			theApp.MainDlg->m_btnStart.Invalidate();
			theApp.MainDlg->Start_Btn_On = false;

			if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
			{
				if (!askMsg("���� �ѱ�����Դϴ�. \n��� �����Ͻðڽ��ϱ�?") == IDOK)
				{
					iRtnFunction = 10000;
					break;
				}
			}
			logStr.Format("START BUTTON PRESS [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
		}
		break;
	case 10165://! Start P/B Button Push ���. 
		Dio.StartPBLampOn(false);
		Dio.DoorLift(false, false);
		Dio.PCBvaccumOn(VACCUM_ON, false);      //����
		sBarCode.Format("%s", Task.ChipID);
		if (sysData.m_iProductComp == 1)
		{
			logStr.Format("====%s ����ǰ �˻� start====", Task.ChipID);
		}
		else
		{
			logStr.Format("====%s AA start====", Task.ChipID);
		}
		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD);
		theApp.MainDlg->putListLog(logStr);
		iRtnFunction = 10170;
		break;
	case 10170:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 10171;
			break;
		}
		break;
	case 10171:
		iRtnFunction = 10178;
		break;
	case 10178:
	{
		bool bflag = true;

		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;

		MandoInspLog.func_InitData();

		Task.func_TactTimeInit();


		theApp.MainDlg->dispGrid();

		Task.m_timeChecker.Measure_Time(1);	//Start ��ư Ŭ�� �ð�
		theApp.MainDlg->func_ChipID_Draw();
		MandoInspLog.bInspRes = true;
		vision.clearOverlay(CCD);
		int iSocketCount = 0;

		//logStr.Format("%d", sysData.m_Socket_Count);
		iSocketCount = atoi(logStr);
		iSocketCount++;
		sysData.m_Socket_Count = iSocketCount;
		sysData.Save();

		iRtnFunction = 10180;
	}
	break;
	case 10180:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10200;
			break;
		}
		else
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
			break;
		}
		break;
	case 10200:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10202;
			break;
		}
		else
		{
			logStr.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� ����");
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = -10200;
			break;
		}
		break;

	case 10202:

		iRtnFunction = 10205;
		break;

	case 10205:
		if (sysData.m_iProductComp == 1)
		{
			iRtnFunction = 10250;
			break;
		}
		else
		{
			iRtnFunction = 10210;
			break;
		}
		break;

	case 10210:
		if (Dio.HolderGrip(true, false))
		{
			logStr.Format("Holder Grip ���� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 10250;
			break;
		}
		else
		{
			logStr.Format("Holder Grip ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10210;
			break;
		}
		
		break;

	case 10250:
		if (!Dio.HolderGripCheck(true, false))
		{
			logStr.Format("Holder Grip Ȯ�� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10400;
			break;
			
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 3000)
		{
			if (askMsg("Holder Grip Fail \n��� �����Ͻðڽ��ϱ�?") == IDOK)
			{
				iRtnFunction = 10400;
				break;
			}
			logStr.Format("HoldertLens Grip Fail[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10210;
			break;
			
		}
		break;
	case 10400:	
		//! X Y ���ڵ� POS �̵�
		if (motor.Pcb_Motor_Move(Dark_Pos))	//BarcodePos
		{
			logStr.Format("PCB XY BarcodePos ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10401;
			break;
		}
		else
		{
			logStr.Format("PCB XY BarcodePos ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
			break;
		}
		break;
	case 10401:
		// Z ���ڵ� POS �̵�
		if (motor.PCB_Z_Motor_Move(Dark_Pos))  //BarcodePos
		{
			logStr.Format("PCB Z BarcodePos ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10402;
		}
		else
		{
			logStr.Format("PCB Z BarcodePos ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
		}
		break;
		iRtnFunction = 10402;
		break;
	case 10402:
		//���ڵ� ����
		bRetVal = theApp.MainDlg->SendPacketToBarcode(true);
		sBarCode.Format("%s", Task.ChipID);
		//g_ADOData.func_AATaskToRecordID(modelList.curModelName, Task.ChipID);
		if (bRetVal == false)
		{
			logStr.Format("[LAN]Barcode BCR Read ���� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10402;
			break;
		}
		logStr.Format("[LAN]Barcode BCR Read ���� ���� [%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		iRtnFunction = 10440;
		break;

	case 10440:
		if (sysData.m_testRunMode == false)			//DRY_RUN
		{
			pThread_TaskCCD = ::AfxBeginThread(Thread_TaskCCD, this);

		}
		iRtnFunction = 10500;
		break;
	case 10500:
		if (sysData.m_iFront == 1)
		{
			iRtnFunction = 10900;
			logStr.Format("Door Open Mode [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			iRtnFunction = 10550;
		}
		break;

	case 10550:	//! Area ���� ���� Check 10450
		if (Dio.LightCurtainOnCheck(false, false))
		{
			if (Dio.DoorLift(false, false))
			{
				Sleep(100);
				iRtnFunction = 10600;
			}
			else
			{
				logStr.Format("Door Close ���� [%d]", iUseStep);	//Door Close ����
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -10550;
			}
		}
		else
		{
			logStr.Format(("LightCurtain ����[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10550;
		}
		break;

	case 10600:	//Light Ŀư ���� ���� ���� Ȯ�� 10460
		if (Dio.DoorLiftOnCheck(false, false))
		{
			iRtnFunction = 10900;
			break;
		}

		if (Dio.LightCurtainOnCheck(true, false))//Lift Close���� ���� ���� ���� �� ��� Open
		{
			if (Dio.DoorLift(true, false))
			{
				iRtnFunction = 10550;
				putListLog(logStr);			
			}
			else
			{
				logStr.Format("Door Close ���� [%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -10600;
			}
		}
		break;
	case 10900:
		Task.PcbOnStage = 100;
		Task.m_b_AA_Retry_Flag = false;

		logStr.Format("%d", Task.m_iPcbPickupNo);
		theApp.MainDlg->m_labelPickupNoPcb.SetText(logStr);
		theApp.MainDlg->m_labelPickupNoPcb.Invalidate();

		MandoInspLog.sBarcodeID.Format("%s", Task.ChipID);	//���� ������Camera �˻� Log ����
		iLaser_Pos = 0;


		LightControl.ChartAllControl(true);		//chart Led All On


#if (____AA_WAY == PCB_TILT_AA)
		if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
		{
			logStr.Format("		Lens �׸� ���� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 15500;// 10250;
			break;
		}
#else
		if (sysData.m_iProductComp == 0)		//lens Thread ���� �κ�
		{
			if (Task.m_bOKLensPass != 1)
			{
				Task.LensTask = 30000;	//Lens AA,, ���� �ε� ���� ����, ���� Ȯ���� ����Ͻÿ� 240115
			}
			else
			{
				Task.interlockLens = 1;		//Lens AA ���
				logStr.Format("Lens �ѱ� �Ϸ�  [%d]", iRtnFunction);
				theApp.MainDlg->putListLog(logStr);
			}
		}
#endif
		iRtnFunction = 11000;
		break;

	default:
		logStr.Format("PCB ��ǰ ���� Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::RunProc_PCBOutsideAlign(int iUseStep)
{
	//! PCB �ܺ� Align Step(11000 ~ 14999)					//!! MIU_Initialize -> PCB Holder Align ��ġ
	int iRtnFunction = iUseStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	//ctrlSubDlg(MAIN_DLG);		//20161212  ccd start ����
	theApp.MainDlg->m_iCurCamNo = 0;
	theApp.MainDlg->setCamDisplay(1, 0);
	theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
	//	Dio.PCBvaccumOn(VACCUM_OFF, false); 

	switch (iUseStep)
	{
		//����
		//���� Ȯ��
		//
	case 11000:
		/////////////////////////
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 11500;
		break;

	case 11500:

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(" PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -11500;
			break;

		}

		Task.d_Align_offset_x[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_y[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_th[PCB_Holder_MARK] = 0;
		iRtnFunction = 12000;
		break;
	case 12000:

		if (Task.m_bOKLensPass == 1 || sysData.m_iAlignCamInspPass == 1)   //Lens �ѱ� �Ϸ���� RunProc_PCBOutsideAlign
		{
			iRtnFunction = 14100;
			break;
		}

		iRtnFunction = 13000;
		break;
	case 13000:
		Task.m_iRetry_Opt = 0;
		LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align�� ���� ON
		
		if (motor.Pcb_Motor_Move(Holder_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb Holder ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 13300;
		}
		else
		{
			logStr.Format("Pcb Holder ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13000;
		}
		break;

	case 13300:
		if (motor.PCB_Z_Motor_Move(Holder_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb Holder Z��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			//151109 ���� �ӽ� ������
			Sleep(300);
			iRtnFunction = 13500;
		}
		else
		{
			logStr.Format("Pcb Holder Z��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13300;
		}
		break;
	case 13500:
		iRtnFunction = 13600;
		break;
	case 13600:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 13700;
		}
		break;
	case 13700: // lens Align

		if (Task.m_iRetry_Opt > iAlignRetry)
		{
			logStr.Format("PCB Holder ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iUseStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[PCB_Holder_MARK] = 0;
				Task.d_Align_offset_y[PCB_Holder_MARK] = 0;
				Task.d_Align_offset_th[PCB_Holder_MARK] = 0;
				iRtnFunction = 14100;
			}
			else
			{
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 13700;
				logStr.Format("PCB Holder ��˻� ����[%d]", iUseStep);
				putListLog(logStr);
			}

			break;
		}

		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamAlign(CAM1, PCB_Holder_MARK, false, offsetX, offsetY, offsetTh);
		offsetTh = 0.0;
		saveInspImage(PCB_IMAGE_SAVE, Task.m_iRetry_Opt);

		Task.m_iRetry_Opt++;
		offsetTh = 0;//holder x,y�ุ
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Holder_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Holder_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Holder_MARK] += offsetTh;

			logStr.Format("Pcb Holder : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh);
			if (iRtnVal == 1)
			{
				iRtnFunction = 14000;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		Pcb Holder [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);

				logStr.Format("		Pcb Holder Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);//0.000);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 14100;				// �Ϸ�
				break;
			}
			else
			{
				logStr.Format("Pcb Holder [%d] ���� ���� �ʰ�[%d]", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(PCB_Holder_MARK);
				iRtnFunction = -13700;				// NG
				break;
			}
		}
		else
		{
			logStr.Format("Pcb Holder [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(300);
			iRtnFunction = 13700;					//  ��˻� 
		}
		break;

	case 14000://!! ������ �̵�.- Retry
		if (motor.Pcb_Holder_Align_Move(Holder_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 13500;
		}
		else
		{
			logStr.Format("Pcb Holder ���� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -14000;
		}
		break;
	case 14100:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(" PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -14100;
			break;

		}
		iRtnFunction = 14200;
		break;
	case 14200:

#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 15000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 26000;

#endif
		break;
	default:
		logStr.Format("PCB ��ǰ ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	return iRtnFunction;
}

int	CPcbProcess2::RunProc_SensorAlign(int iUseStep)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = _T("");
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay
	switch (iUseStep)
	{
	case 26000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("		PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26000;
			break;
		}

		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;

		iRtnFunction = 26050;
		break;
	case 26050:
		if (sysData.m_iAlignCamInspPass == 1)	//RunProc_SensorAlign
		{

			logStr.Format("PCB Sensor Align Pass[%d]", iUseStep);
			putListLog(logStr);


			Task.m_iRetry_Opt = 0;
			iRtnFunction = 26629;
			break;
		}
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 26600;
		break;

	case 26600:
		iRtnFunction = 26610;
		break;

	case 26610:
		//ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(PCB_Chip_MARK);

		LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align�� ���� ON

		if (motor.Pcb_Motor_Move(Align_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Align_Pos))
			{
				logStr.Format("PCB Sensor Align ��ġ �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
				
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 26615;
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align Z ��ġ �̵� ���� [%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -26610;
				break;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align X,Y��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26610;
		}
		break;
	case 26615:	
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 26616;
		break;
	case 26616:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 26620;
		}
		break;
	case 26620:
		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Align(*SensorAlign*) ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iUseStep);
			Task.d_Align_offset_x[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_y[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_th[PCB_Chip_MARK] = 0.0;
			Task.m_iRetry_Opt = 0;


			if (askMsg(logStr) == IDOK)
			{
				iRtnFunction = 26629;
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align ���� [%d]", iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -26620;
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(CHIP_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;

		//offsetTh=0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX; 
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] += offsetTh;

			Task.dPcbAlignTh = Task.d_Align_offset_th[PCB_Chip_MARK];

			logStr.Format("PCB Sensor Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 26625;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Sensor Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("		PCB Sensor Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);

				iRtnFunction = 26629;		// �Ϸ�
				LightControl.ctrlLedVolume(LIGHT_PCB, 0);
				break;
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] ���� ���� �ʰ�", Task.m_iRetry_Opt);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -26620;				// NG
				break;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(300);
			iRtnFunction = 26620;				//  ��˻� 
		}
		break;
	case 26625://- Sensor Align Retry
		if (motor.Pcb_Holder_Align_Move(Align_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 26615;
		}
		else
		{
			logStr.Format("PCB Sensor Align ���� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26625;
		}
		break;
	case 26629:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26629;
			break;
		}
		iRtnFunction = 26990;
		break;
	case 26990:				

		iRtnFunction = 26991;
		break;
	case 26991://! Lens-Z�� ��� ��ġ �̵�
#if (____AA_WAY == PCB_TILT_AA)
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			iRtnFunction = 30000;	//�����̵�
		}
		else
		{
			iRtnFunction = 27000;	//Laser���� ->Align -> ����
		}
#elif (____AA_WAY == LENS_TILT_AA)
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			iRtnFunction = 30000;//Laser ���� Pass
		}
		else
		{
			iRtnFunction = 27000;
		}
#endif
		break;
	default:
		logStr.Format("Lens �ܺ� Align Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::RunProc_LaserMeasure(int iUseStep)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay
	switch (iUseStep)
	{
	case 27000:
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		iLaser_Pos = 0;// 4;//4�����Ͱ� laser out point


		LightControl.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON


		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -27000;
			break;
		}
		LightControl.ctrlLedVolume(LIGHT_OC, 0);



		MandoInspLog.dLaserTestTilt[0] = 0.0;
		MandoInspLog.dLaserTestTilt[1] = 0.0;
		iRtnFunction = 27100;// 27350;//<-�ӽ� �н�   Original-> 27100; 
		break;

	case 27100:
		if (motor.Pcb_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27150;
		}
		else
		{
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27100;
		}
		break;
	case 27150:
		iRtnFunction = 27151;
		break;
	case 27151:
		if (iLaser_Pos == 0)
		{
			if (!motor.PCB_Z_Motor_Move(Laser_Pcb_Pos))
			{
				logStr.Format("PCB Laser_Pcb_Pos Z �̵� ����[%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27151;
				break;
			}
			else
			{
				logStr.Format("PCB Laser_Pcb_Pos Z �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
			}
		}
		iRtnFunction = 27200;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27200:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Sleep(200);
			iRtnFunction = 27250;
		}
		break;
	case 27250:
		//Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Keyence.func_LT9030_Scan(Task.m_Laser_Point[iLaser_Pos]);  //func_LT9030_Scan

		Sleep(100);

		logStr.Format("�������� %lf - %d ��ġ[%d]", Task.m_Laser_Point[iLaser_Pos], iLaser_Pos + 1, iUseStep);
		putListLog(logStr);

		LaserPos[iLaser_Pos].x = motor.GetEncoderPos(Motor_PCB_X);
		LaserPos[iLaser_Pos].y = motor.GetEncoderPos(Motor_PCB_Y);
		LaserValue[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];

		MandoInspLog.dLaserTestPoint[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];
		
		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				if (theApp.MainDlg->func_Check_LaserValueErr(LaserValue) == false)
				{
					logStr.Format("Laser �������̻�: %.04lf, %.04lf, %.04lf, %.04lf [%d]", LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3], iUseStep);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -27250;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]))
				{
					Task.m_dataOffset_x[0] = Task.d_Align_offset_xt[PCB_Chip_MARK];
					Task.m_dataOffset_y[0] = Task.d_Align_offset_yt[PCB_Chip_MARK];
					logStr.Format("	������ Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]);
					putListLog(logStr);

					int Rnd = theApp.MainDlg->TiltAlignLimitCheck(Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]);
					if (Rnd == 2)
					{
						if (Task.m_dataOffset_x[0] == 0.0 || Task.m_dataOffset_y[0] == 0.0)
						{
							logStr.Format("Laser ���� �� �� �̻� �߻�..Xt : %lf, Yt : %lf", Task.m_dataOffset_x[0], Task.m_dataOffset_y[0]);
							//errMsg2(Task.AutoFlag, logStr);
							//iRtnFunction = -27250;
							//break;
						}
						if (!g_ADOData.func_AATaskToRecordLaser(Task.ChipID, Task.m_dataOffset_x[0], Task.m_dataOffset_y[0], Task.m_Laser_Point))
						{
							logStr.Format("DataBase Laser ���� ���� Data ��� ����.[%d]\n MS Office�� �ݾ��ּ���.", iUseStep);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -27250;
							break;
						}
						Task.m_PcbLaserAfterTx = motor.GetCommandPos(Motor_PCB_Xt);
						Task.m_PcbLaserAfterTy = motor.GetCommandPos(Motor_PCB_Yt);

						logStr.Format("	PCB Laser tx:%lf , ty:%lf [%d]", model.axis[Motor_PCB_Xt].pos[Laser_Pcb_Pos], model.axis[Motor_PCB_Yt].pos[Laser_Pcb_Pos], iUseStep);
						putListLog(logStr);
						logStr.Format("	PCB Laser After tx:%lf , ty:%lf [%d]", Task.m_PcbLaserAfterTx, Task.m_PcbLaserAfterTy, iUseStep);
						putListLog(logStr);

						iRtnFunction = 27310;	//->Laser�Ǹ������, 27350;->PCB WaitPos
						iLaser_Pos++;
					}
					else if (Rnd == 1)
					{
						iRtnFunction = 27300;
					}
					else
					{
						logStr.Format("Laser Tilt  ������ Limit�� �ʰ� �Ͽ����ϴ�.[%d]", iUseStep);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -27250;
					}
				}
			}
			else
			{
				iRtnFunction = 27100;
				iLaser_Pos++;
			}
		}
		else
		{
			iRtnFunction = 27250;
		}
		break;
	case 27300:
		MandoInspLog.dLaserTestTilt[0] += Task.d_Align_offset_xt[PCB_Chip_MARK];
		MandoInspLog.dLaserTestTilt[1] += Task.d_Align_offset_yt[PCB_Chip_MARK];

		if (motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK]))
		{
			iRtnFunction = 27100;
			logStr.Format("PCB Xt : %lf, Yt : %lf [%d]", Task.d_Align_offset_xt[PCB_Chip_MARK], Task.d_Align_offset_yt[PCB_Chip_MARK], iUseStep);
			putListLog(logStr);
			iLaser_Pos = 0;
		}
		else
		{
			logStr.Format("PCB tx,ty��  ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27300;
		}
		break;

	case 27310:
		logStr.Format("PCB Laser End Xt : %lf, Yt : %lf [%d]", MandoInspLog.dLaserTestTilt[0], MandoInspLog.dLaserTestTilt[1], iUseStep);
		putListLog(logStr); 

		iLaser_Pos = 0;
		if (LGIT_MODEL_INDEX == M2_FF_MODULE)	//�ܺ� ������ ����Ʈ ������
		{
			iRtnFunction = 27315;
		}
		else
		{
			iRtnFunction = 27350;		//jump step
		}
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27315:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -27315;
			break;
		}
		if (LGIT_MODEL_INDEX == M2_FF_MODULE)
		{
			iRtnFunction = 27320;		//�ѹ��� �ٸ� ��ġ ������ ����Ʈ ������
		}
		else
		{
			iRtnFunction = 27340;
		}
		break;
	case 27320:
		if (motor.Pcb_Motor_Move_ComLaser(iLaser_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("Com Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27325;

			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			logStr.Format("Com Laser ���� ���� �ܺ� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27320;
		}
		
		
		break;
	case 27325:
		if (iLaser_Pos == 0)
		{
			if (!motor.PCB_Z_Motor_Move(Com_Laser_Pos))
			{
				logStr.Format("PCB Com Laser_Pcb_Pos Z �̵� ����[%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27325;
				break;
			}
			else
			{
				logStr.Format("PCB Laser_Pcb_Pos Z �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
			}
		}
		iRtnFunction = 27330;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 27330:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Sleep(200);
			iRtnFunction = 27332;
		}
		
		break;
	case 27332:
		Keyence.func_LT9030_Scan(Task.m_Laser_Point[iLaser_Pos]);  //func_LT9030_Scan
		//Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("Side �������� %lf - %d ��ġ[%d]", Task.m_Laser_Point[iLaser_Pos], iLaser_Pos + 1, iUseStep);
		putListLog(logStr);

		MandoInspLog.dLaserTestComPoint[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];
		
		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				iRtnFunction = 27340;
				break;
			}
			else
			{
				iRtnFunction = 27320;
				iLaser_Pos++;
				break;
			}
		}
		else
		{
			iRtnFunction = 27332;
		}
		break;
	case 27340:
		iRtnFunction = 27350;
		break;
	case 27350:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27351;
		}
		else
		{
			logStr.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27350;
		}

		break;
	case 27351:
		//if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		//{
		//	iRtnFunction = 31000;	//-- ���� Pass Mode �̰ų�, ���� �Ϸ� �� ���
		//}
		//else
		//{
		//	iRtnFunction = 30020;
		//}
		iRtnFunction = 27600;
		break;
	case 27600:

		iRtnFunction = 27650;
		break;
	case 27650:
		offsetX = offsetY = offsetTh = 0.0;
		iRtnFunction = 27750;
		break;
	case 27750:
		iRtnFunction = 27960;		// �Ϸ� �ӽ�
									//
									//
									//
		break;


#if 0


		Task.m_iRetry_Opt = 0;
		iRtnFunction = 27800;
		break;
	case 27800:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align�� ���� ON

		if (motor.Pcb_Motor_Move(Align_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Align_Pos))
			{
				logStr.Format("PCB Sensor Align ��ġ �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 27850;
				Task.PCBTaskTime = myTimer(true);
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27960;
		}
		break;
	case 27850:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 27900;
		}
		break;
	case 27900:
		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Align(*Laser*) ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iUseStep);
			Task.m_iRetry_Opt = 0;
			Task.d_Align_offset_x[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_y[PCB_Chip_MARK] = 0.0;
			Task.d_Align_offset_th[PCB_Chip_MARK] = 0.0;
			if (askMsg(logStr) == IDOK)
			{
				iRtnFunction = 27960;
				logStr.Format("PCB Align �̺��� ����[%d]", iUseStep);
				putListLog(logStr);
			}
			else
			{
				iRtnFunction = 27900;
				logStr.Format("PCB Align ��˻� ����[%d]", iUseStep);
				putListLog(logStr);
				Sleep(500);
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);

		saveInspImage(CHIP_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		//offsetTh=0;
		if (iRtn == 0)
		{
			/*Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] -= offsetTh;*/

			Task.d_Align_offset_x[PCB_Holder_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Holder_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Holder_MARK] += offsetTh;			//lim

			Task.dPcbAlignTh = Task.d_Align_offset_th[PCB_Chip_MARK];
			logStr.Format("PCB Sensor Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 27950;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Sensor Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				logStr.Format("		PCB Sensor Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);	//Laser ���� ���� ���� �ð�


				iRtnFunction = 27960;		// �Ϸ�
				LightControl.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON 
				LightControl.ctrlLedVolume(LIGHT_OC, 0);		// Align�� ���� ON
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] ���� ���� �ʰ� [%d]", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				iRtnFunction = -27900;				// NG
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align(*Laser*) [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);
			Sleep(500);
			iRtnFunction = 27900;				//  ��˻� 
		}
		break;
	case 27950:
		if (motor.Pcb_Holder_Align_Move(Align_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 27850;
		}
		else
		{
			logStr.Format("PCB Sensor Align ���� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27950;
		}
		break;

#endif

	case 27960:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB�� Z�� ��� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27960;
			break;
		}
		else
		{
			logStr.Format("PCB�� Z�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27970;
		}

		break;
	case 27970:
		iRtnFunction = 27990;
		break;
	case 27990:
		iRtnFunction = 30000;
		break;
	default:
		logStr.Format("Pcb Thread Step ��ȣ ������ .[%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	return iRtnFunction;
}

int CPcbProcess2::RunProc_EpoxyNewResing(int iUseStep)
{//! Epoxy ���� �� ���� �˻� Step(17000 ~ 17999)
	int iRtnFunction = iUseStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ���� ȹ�� Delay
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;

	switch (iUseStep)
	{
	case 30000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB_Z�� �����ġ �̵� ����[%d].", iUseStep);
			putListLog(logStr);
			iRtnFunction = -30000;
			break;
		}
		logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� [%d]", iUseStep);
		putListLog(logStr);

		if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		{
			iRtnFunction = 34100;// 31000;	//-- ���� Pass Mode �̰ų�, ���� �Ϸ� �� ���
			break;
		}
		else
		{
			iRtnFunction = 30020;
			break;
		}
		break;
	case 30020:
		iRtnFunction = 30030;
		break;
	case 30030:  //���� ��ġ �̵�
		LightControl.ctrlLedVolume(LIGHT_PCB, 0);		// Align ���� OFF
		if (motor.Pcb_Motor_Move(Dispense_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCB Dispense ���� ��ġ �̵� �Ϸ�-������ X:%.03lf,Y:%.03lf,T:%.03lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			putListLog(logStr);
			iRtnFunction = 30040;
		}
		else
		{
			logStr.Format("PCB Dispense ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30030;
		}
		break;
	case 30040:

		iRtnFunction = 30045;
		break;

	case 30045: //Dispenser-z �� ���
		if (motor.PCB_Z_Motor_Move(Dispense_Pos))
		{
			iRtnFunction = 30050;
		}
		else
		{
			logStr.Format("PCB Z Dispense ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30045;
		}
		break;
	case 30050: // ���潺 ����

		if (sysData.nEpoxyIndex == CRICLE_EPOXY)
		{
			bChk = motor.func_Epoxy_CircleDraw();
		}
		else if (sysData.nEpoxyIndex == RECT_EPOXY)
		{
			bChk = motor.func_Epoxy_Draw();
		}
		else if (sysData.nEpoxyIndex == POLYGON_EPOXY)
		{
			motor.func_Epoxy_Rect_Circle_Draw();
		}
		else if (sysData.nEpoxyIndex == POINT_EPOXY)
		{
			//point ��
			logStr.Format("PCB ���� ������ ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30050;
			break;
		}

		if (bChk)
		{
			Task.m_bOkDispense = 1;

			logStr.Format("PCB Dispense �Ϸ�[%d]", iUseStep);
			putListLog(logStr);

			iRtnFunction = 30070;
		}
		else
		{
			logStr.Format("PCB Dispense ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30050;
		}
		break;
	case 30070:
		if (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y))
		{
			iRtnFunction = 30100;
		}
		break;
	case 30100:
		iRtnFunction = 31000;
		break;
	case 31000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(" PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -31000;
			break;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 32000;
			break;
		}
		break;
	case 32000:
		iRtnFunction = 32100;
		break;

	case 32100: // �����˻� �̵�

		if (motor.Pcb_Motor_Move(Holder_Pos))
		{
			LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		//Ȧ�� ���� ����
			iRtnFunction = 32500;
		}
		else
		{
			logStr.Format("PCB �����˻� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32100;
		}
		break;

	case 32500:// �̵���, ���� �ð� ���� �� ��ũ Ȯ��
		if (motor.PCB_Z_Motor_Move(Holder_Pos))

		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("PCB �����˻� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 33000;
		}
		else
		{
			logStr.Format("PCB �����˻� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32500;
		}
		break;
	case 33000: //�����˻� z�� �̵�

		if ((myTimer(true) - Task.PCBTaskTime) > 1000 && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
#if 1
			vision.bmpImageSaveFn(CAM1, 0);
			iRtnFunction = 33500;

			Task.PCBTaskTime = myTimer(true);
			break;
#else
			Sleep(300);
			int iMarkNo = 0;
			CString sTemp = "";

			vision.clearOverlay(CAM1);

			if (vision.getLiveMode())
			{
				vision.getSnapImage(CAM1);
				vision.setLiveMode(true);
			}

			double ep1 = 0.0;
			MappTimer(M_TIMER_RESET, &ep1);
			iMarkNo = vision.findMark(CAM1, PCB_Holder_MARK);

			double ep2 = 0.0;
			MappTimer(M_TIMER_READ, &ep2);
			sTemp.Format(" %.3lf sec", ep2 - ep1);
			vision.textlist[CAM1].addList((CAM_SIZE_X - 140), (CAM_SIZE_Y - 60), sTemp, M_COLOR_GREEN, 24, 10, "Arial");

			vision.drawOverlay(CAM1);

			if (iMarkNo != -1)
			{
#ifdef USE_GEOMETRIC
				MgraColor(M_DEFAULT, M_COLOR_GREEN);
				MmodDraw(M_DEFAULT, vision.ModResult[PCB_Holder_MARK][iMarkNo], vision.MilOverlayImage[CAM1], M_DRAW_BOX + M_DRAW_POSITION + M_DRAW_EDGES + M_DRAW_AXIS, M_DEFAULT, M_DEFAULT);

#endif
			}
			CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
			pFrame->m_rBox.left = 0;
			pFrame->m_rBox.top = 0;
			pFrame->m_rBox.right = 0;
			pFrame->m_rBox.bottom = 0;
			iRtnFunction = 33500;
#endif
			
		}
		break;
	case 33500: // 
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 33600;
		}
		break;

	case 33600:
		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamAlign(CAM1, PCB_Holder_MARK, false, offsetX, offsetY, offsetTh);
		if (iRtn == 0)
		{
			LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_EPOXY_INSP]);		//���� �˻� ���� ����
			iRtnFunction = 33620;
			Task.PCBTaskTime = myTimer(true);
			break;
		}
		else
		{
			logStr.Format("PCB Holder �˻� ����\n ���� ���������� �����Ͻðڽ��ϱ�?");
			putListLog(logStr);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_mark_pos_x[PCB_Holder_MARK][0] = 0.0;
				Task.d_mark_pos_y[PCB_Holder_MARK][0] = 0.0;
				iRtnFunction = 33620;
				break;
			}

			logStr.Format("Pcb Holder ��ũ �ν� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -33600;				// NG
			break;
		}
		break;
	case 33620:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			iRtnFunction = 33700;
		}
		
		break;

	case 33700:
		iRtnFunction = 33800;
		break;
	case 33800:
		iRtnFunction = 33900;
		break;

	case 33900: //�����˻� 
		

		vision.clearOverlay(CAM1);

		bChk = theApp.MainDlg->_EpoxyFinddispense(CAM1);		//auto

		vision.drawOverlay(CAM1);
		if (!bChk)
		{
			putListLog("Epoxy ���� �˻� ����");

			logStr.Format("Epoxy ���� �˻� ����. \n�ڵ� ���� ���� �Ͻðڽ��ϱ�?");

			Dio.setAlarm(ALARM_ON);
			if (askMsg(logStr))
			{
				Task.PCBTaskTime = myTimer(true);
				Dio.setAlarm(ALARM_OFF);
				Dio.setAlarm(AUTO_RUN);
			}
			else
			{
				Dio.setAlarm(AUTO_STOP);
				logStr.Format("Epoxy ���� �˻� ����[%d]", iUseStep);
				iRtnFunction = -33900;
				break;
			}
		}
		Task.m_timeChecker.Measure_Time(3);	//���� �Ϸ� �ð�
		Task.m_dTime_Epoxy = Task.m_timeChecker.m_adTime[3] - Task.m_timeChecker.m_adTime[2];
		theApp.MainDlg->dispGrid();

		logStr.Format("PCB �����˻� �Ϸ�[%d]", iUseStep);
		putListLog(logStr);

		iRtnFunction = 34000;
		break;
	case 34000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB_Z�� �����ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -34000;
			break;
		}

		iRtnFunction = 34100;
		break;
	case 34100:

#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 39000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 35000;
#endif
		break;
	default:
		logStr.Format("PCB Dispenser Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::RunProc_InspAAPos(int iUseStep)
{
	//! Lens Pickup�� ��ǰ �ѱ� Step	(18000 ~ 19999)

	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 40000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40000;
			break;
		}

		iRtnFunction = 40050;
		break;
	case 40050:
		//240108 ���� �̵� pcb ������ġ
		logStr.Format("PCB %s �̵� ���[%d] ", MotorPosName[Bonding_Pos], iUseStep);
		putListLog(logStr);

		if (!motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK], false))	//if (!motor.Pcb_Motor_MoveY(Bonding_Pos))
		{
			logStr.Format("PCB %s �̵� ��� ���� ", MotorPosName[Bonding_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40050;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 40060;

	case 40060:


#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 41000;

#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 41000;


		//if (Task.interlockLens == 1 && Task.m_bOKLensPass == 1)
		//{
		//	iRtnFunction = 40000;/// 41000;		//2ȣ�� ����� Lens Thread���� �ε�
		//	break;
		//}
		//if ((myTimer(true) - Task.PCBTaskTime) > 10000)	//10�� ���
		//{
		//	logStr.Format(_T("Lens Load Ȯ�� ����[%d]"), iUseStep);
		//	errMsg2(Task.AutoFlag, logStr);
		//	iRtnFunction = -39999;
		//	break;
		//}

#endif		
		break;
	default:
		logStr.Format("Pcb Align ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::RunProc_Bonding_Pos_GO(int iUseStep)
{//! Lens Pickup�� ��ǰ �ѱ� Step	(18000 ~ 19999)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 41000:
		/*if (sysData.m_iIrChartUse == 1)
		{
			LightControl.ctrlLedVolume(0,0);
			Sleep(100);
			LightControl.ctrlLedVolume(0, model.m_iLedValue[LEDDATA_TOP2_CHART]);
		}*/

		LightControl.ChartAllControl(true);

		iRtnFunction = 41020;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 41020:
		iRtnFunction = 41030;
		break;
	case 41030:
		if ((motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y) && motor.IsStopAxis(Motor_PCB_TH)))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 41050;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 3000)
		{
			logStr.Format(_T("PCB x,y,th axis ���� Ȯ�� ����[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41030;
		}
		break;
	case 41050:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41050;
		}
		else
		{
			iRtnFunction = 41100;
		}
		break;
	case 41100:
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			//LASER PASS
			if (motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("PCB TX,TY Bonding_Pos �̵� �Ϸ� [%d]", iUseStep);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);
				iRtnFunction = 41150;
			}
			else
			{
				logStr.Format(_T("PCB Tilt Bonding Pos Move Fail[%d]"), iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -41100;
				break;
			}
		}
		else
		{
			//������ ������ �Ѱ�� ������ PCB TX,TY ������ �ٽ� �̵�
			if (motor.Pcb_Tilt_Trinity_Move(Task.m_PcbLaserAfterTx, Task.m_PcbLaserAfterTy))
			{
				Task.PCBTaskTime = myTimer(true);
				iRtnFunction = 41150;
			}
			else
			{
				logStr.Format(_T("PCB Tilt Bonding Pos Move Fail[%d]"), iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -41100;
				break;
			}
		}
		
		break;
	case 41150:
		iRtnFunction = 41200;

		break;
	case 41200:
		iRtnFunction = 41220;

		break;
	case 41220:
#if (____AA_WAY == PCB_TILT_AA)
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING913);	//"PCB Bonding ��ġ �̵� �Ϸ�[%d]"
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 41250;
			if (!motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("PCB tx,ty Bonding_Pos �̵� ����");
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41200;
				break;
			}
			break;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
			break;
		}
#elif	(____AA_WAY == LENS_TILT_AA)
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCB Bonding ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 41250;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			logStr.Format("PCB Bonding ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
#endif
		break;
	case 41250:
		iRtnFunction = 41270;
		break;
	case 41270:

		if (motor.Lens_Motor_MoveXY(Bonding_Pos))
		{
#if (____AA_WAY == LENS_TILT_AA)
			if (!motor.Lens_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41200;
				break;
			}
			else
			{
				Task.interlockPcb = 1;

				logStr.Format("Lens TX,TY Bonding_Pos �̵� �Ϸ� [%d]", iUseStep);
				putListLog(logStr);
			}

#endif
			logStr.Format("Lens X,Y Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			
			iRtnFunction = 41300;
			break;
		}
		else
		{
			logStr.Format("Lens Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41250;
			break;
		}
		break;
	case 41300:
		iRtnFunction = 41310;

		break;
	case 41310:
		if (Task.interlockPcb == 1)		//pcbƿƮ�� ���  ���� ������,����� �۾� ������ ������ġ �ö����� ��ٷ��ߵż�
		{
			iRtnFunction = 41350;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 50000)
		{
			logStr.Format("Lens Bonding ��ġ �̵� �ð� �ʰ�[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41310;
		}
		break;
	case 41350:
		iRtnFunction = 41400;

		break;
	case 41400:
		//if( func_MIU_ConnectLiveCheck() == false)		//MIU ���� Check				
		//{
		//	iRtnFunction = -41400;
		//	break;
		//}

		theApp.MainDlg->setCamDisplay(3, 1);
		iRtnFunction = 48000;

		break;
	case 48000:

		logStr.Format("Loading End[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 49000;
		break;
		//
	default:
		logStr.Format("Pcb Align ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::procAutoFocus(int iStep)
{
	int iRtn = 0;
	int iRtnFunction = 0;
	CString logStr = _T("");
	int iDelayTime = 0;

	bool limitCheckFlag = false;

	double dMovePos = 0.0;

	double dAutoZ = 0.0;

	if (iStep >= 60300 && iStep != 69999)
	{
		if (gMIUDevice.bMIUOpen == 0 || gMIUDevice.CurrentState != 4)				//DRY_RUN
		{
			logStr.Format("CCD ������ ���� �����Դϴ�.");
			errMsg2(Task.AutoFlag, logStr);
			return iStep;
		}
	}

	if (Task.m_bFlag_Fine_Move == false)
	{
		iDelayTime = model.strInfo_AF1.m_iDelayTime;
	}
	else
	{
		iDelayTime = model.strInfo_AF2.m_iDelayTime;
	}

	iRtnFunction = iStep;

	switch (iStep)
	{
	case 60000:
	{
		MbufClear(vision.MilOptImage, 0x00);
		MbufClear(vision.MilDefectImage, 0x00);

		Task.initSFRElem();
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "-");
		theApp.MainDlg->autodispDlg->DrawGraph(0);
		theApp.MainDlg->autodispDlg->DrawGraph(1);


		MandoInspLog.bInspRes = true;	//60000 �ʱ�ȭ
		Task.m_timeChecker.Measure_Time(6);	//AA ���� �ð�
		iRtnFunction = 60010;
		Task.PCBTaskTime = myTimer(true);
	}
	break;
	case  60010:
		if (bThreadCCDRun == false)		//	CCD ���� ������ ���߸� ����... step = 12000 ����
		{
			iRtnFunction = 60015;
		}
		else
		{
			logStr.Format("CCD ���� Open �������Դϴ�[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60010;
		}
		break;
	case  60015:
		if (motor.LENS_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("Lens Z Bonding ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 60020;

		}
		else
		{
			logStr.Format("Lens Z Bonding ��ġ �̵� ���� [%d]", iStep);
			putListLog(logStr);
			iRtnFunction = -60015;
		}
		break;
	case 60020:
		if (!motor.PCB_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("PCB Z�� Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60020;
			break;
		}
		else
		{
			iRtnFunction = 60030;
			logStr.Format("PCB Z�� Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
		}

		break;
	case 60030:
	{
		double MotorPos[2];

		MotorPos[0] = fabs(motor.GetEncoderPos(theApp.MainDlg->TITLE_MOTOR_X) - model.axis[theApp.MainDlg->TITLE_MOTOR_X].pos[Bonding_Pos]);
		MotorPos[1] = fabs(motor.GetEncoderPos(theApp.MainDlg->TITLE_MOTOR_Y) - model.axis[theApp.MainDlg->TITLE_MOTOR_Y].pos[Bonding_Pos]);

		if ((MotorPos[0] < model.axis[theApp.MainDlg->TITLE_MOTOR_X].m_dLimit_Err && MotorPos[1] < model.axis[theApp.MainDlg->TITLE_MOTOR_Y].m_dLimit_Err))
		{
			logStr.Format("PCB�� x,y ���� ��ġ�� ���� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 60040;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
			break;
		}
		else
		{
			logStr.Format("Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60030;
			break;
		}
	}
	break;

	case 60040:		/* PCB Z���� ������ġ �̵� */
	{
		iRtn = theApp.MainDlg->_checkPcbMotor();
		if (iRtn > 0)
		{
			logStr.Format("Z ���� ���� ��ġ �̵�");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);

			theApp.MainDlg->setCamDisplay(3, 1);
			double dCurPos = 0.0;
			double dMovePos = model.axis[Motor_Lens_Z].pos[Bonding_Pos];
			motor.MoveAxis(Motor_Lens_Z, ABS, dMovePos, sysData.fMotorSpeed[Motor_Lens_Z], sysData.fMotorAccTime[Motor_Lens_Z]);
			double ep = myTimer(true);

			while (1)
			{
				if (myTimer(true) - ep < 5000)
				{
					if (motor.IsStopAxis(Motor_Lens_Z))	// && motor.GetInposition(Motor_Lens_Z) )
					{
						dCurPos = motor.GetCommandPos(Motor_Lens_Z);

						if (fabs(dCurPos - dMovePos) <= 0.003)
						{
							Task.PCBTaskTime = myTimer(true);

							logStr.Format("Auto Z Pass");
							putListLog(logStr);
							iRtnFunction = 60900;
							logStr.Format("	AA Step [%d]", iRtnFunction);
							putListLog(logStr);
							break;
						}
					}
				}
				else
				{
					putListLog("Auto Focusing �� LENS Z Motor �̵� �ð� �ʰ�.");
					iRtnFunction = -60040;
					break;
				}
			}
		}
		else
		{
			putListLog("PCB �� ������ġ�� �������� ���߽��ϴ�.");
			iRtnFunction = -60040;
			break;
		}
	}
	break;
	case 60070:		/* SFR ���� �������� �϶��� ��, Z���� �̵� */
	{
		double dCurPos = 0.0;
		double dMovePos = model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] + model.m_dMove_Offset_Z;
		//�������� �϶��� model.m_dMove_Offset_Z��ŭ z���� �����δ� 
		if (fabs(model.m_dMove_Offset_Z) > 0.2)
		{
			logStr.Format("z�� �̵�ġ Limit �ʰ� : Z : %.3lf", dMovePos);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60070;
			break;
		}
		motor.MoveAxis(theApp.MainDlg->TITLE_MOTOR_Z, ABS, dMovePos, sysData.fMotorSpeed[theApp.MainDlg->TITLE_MOTOR_Z], sysData.fMotorAccTime[theApp.MainDlg->TITLE_MOTOR_Z]);
		double ep = myTimer(true);
		Task.m_bFlag_Fine_Move = false;
		while (1)
		{
			if (myTimer(true) - ep< 5000)
			{
				if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
				{
					dCurPos = motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z);
					if (fabs(dCurPos - dMovePos) <= 0.003)
					{
						//240117 �ʱ�ȭ �ڵ� �߰�
						Task.initSFRElem();
						theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "-");
						theApp.MainDlg->autodispDlg->DrawGraph(0);
						theApp.MainDlg->autodispDlg->DrawGraph(1);


						Task.PCBTaskTime = myTimer(true);
						iRtnFunction = 60900;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
					}
				}
			}
			else
			{
				putListLog("Auto Focusing �� Z Motor �̵� �ð� �ʰ�.");
				iRtnFunction = -60070;
				break;
			}
		}
	}
	break;

	case 60100:
	{
		double dMoveDist = 0.0;
		if (!Task.m_bFlag_Fine_Move)
		{
			dMoveDist = model.strInfo_AF1.m_dMoveDistance;
			logStr.Format("Through Focusing");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);
		}
		else
		{
			dMoveDist = model.strInfo_AF2.m_dMoveDistance;
			logStr.Format("Fine Focusing");
			theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, logStr);
		}

		iRtn = theApp.MainDlg->_moveZMotor(dMoveDist, model.strInfo_AF1.m_dMoveVel);		/* Z���� Rough �̵� */
		if (iRtn > 0)
		{
			iRtnFunction = 61000;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			iRtnFunction = -60100;
			logStr.Format("	Z Motor �̵� �ð� �ʰ� [%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			break;
		}

	}
	break;
	case 60200:
	{
		if (Task.bFirstAA)
		{
			if (sysData.m_i1stAaEbable)	//1�� AA�� �����
			{
				iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);

				if (iRtn > 0)
				{
					Task.m_iCnt_Second_AA_Start = Task.m_iCnt_Step_AA_Total;
					Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
					Task.m_iCnt_Step_AA = 0;

					theApp.MainDlg->autodispDlg->DrawBarGraph();		// 60200  procAutoFocus
					MIU.func_Set_InspImageCopy(CHART_RAW, MIU.m_pFrameRawBuffer);			//1�� AA�� 
					Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);		//1�� AA�� ����
					if (Task.sfrResult)
					{
						logStr.Format("1�� AA ��� Spec OK.");
						putListLog(logStr);
						iRtnFunction = 69999;
						break;
					}
				}
			}
			else
			{
				Task.m_iCnt_Second_AA_Start = Task.m_iCnt_Step_AA_Total;
				Task.m_iCnt_Step_AA = 0;
			}
		}
		double dMovePos = 0.0;
#if (____AA_WAY == PCB_TILT_AA)
		dMovePos = 1 * sysData.m_dMove_MaxPos_Fine;
#elif (____AA_WAY == LENS_TILT_AA)
		dMovePos = -1 * sysData.m_dMove_MaxPos_Fine;
#endif	


		iRtn = theApp.MainDlg->_moveZMotor(dMovePos, model.strInfo_AF1.m_dMoveVel);		/* Z���� Rough �̵� */

		if (iRtn > 0)
		{
			iRtnFunction = 60900;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
		}
		else
			iRtnFunction = -60200;
	}

	break;

	case 60900:			/* Z�� ���� ��ġ ���� �� */

		if ((myTimer(true) - Task.PCBTaskTime) > model.strInfo_Cam[CCD].m_iDelayTime)
		{
			iRtnFunction = 60910;
		}
		else
		{
			iRtnFunction = 60900;
		}

		break;

	case 60910:	// AA�� Lens X/Y, PCB Theta ����

		iRtn = theApp.MainDlg->_getMTF(SFR_FIRST/*SFR_FINAL*/);
		if (iRtn > 0)
		{
			if (!theApp.MainDlg->_calcImageAlignment())
			{
				logStr.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(logStr);
				iRtnFunction = -60910;
			}
			else
			{
				if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x * 2 && fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y * 2 &&fabs(Task.m_dShift_IMG_TH) < 0.03)
				{
					logStr.Format("		 ���� Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf",
						sysData.m_dOcSpec.x, sysData.m_dOcSpec.y,
						Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);
					iRtnFunction = 62000;
					break;
				}
				else
				{
					logStr.Format("		 ���� Shift.  X %.3lf, Y %.3lf ,Spec(%.03lf, %.03lf)", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y);
					putListLog(logStr);
					iRtnFunction = 60920;
				}
			}
		}
		else
		{
			logStr.Format("SFR ���� ȹ�� ����.");	//SFR ���� ȹ�� ����.
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -60910;
		}
		break;

	case 60920:
		if (!theApp.MainDlg->_MotorMove_IMG_Align())
		{
			iRtnFunction = -60920;
		}
		else
		{
			iRtnFunction = 60930;
		}
		break;

	case 60930:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -60930;
		}
		else
		{
			Sleep(200);
			iRtnFunction = 60900;
		}
		break;

	case 61000:

		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 62000;
		}

		break;

	case 62000:
	{
		iRtn = theApp.MainDlg->_getMTF((Task.bFirstAA == true) ? SFR_FINAL : SFR_FIRST);		//�ӵ������� sfr �α� �����
		logStr.Format("Step :[%d] SFR [%.03f] [Z %.03f]", Task.m_iCnt_Step_AA, Task.SFR.fSfrN4[Task.m_iCnt_Step_AA][0], motor.GetEncoderPos((theApp.MainDlg->TITLE_MOTOR_Z)));
		putListLog(logStr);
		if (iRtn>0)
		{
			Task.m_iCnt_Step_AA++;		/* Auto Focus rough ���� Ƚ�� ���� */

			if (Task.bFirstAA)
			{
				theApp.MainDlg->autodispDlg->DrawGraph(1);		/* Z ���� ��ġ�� SFR ������ �׷��� �׸��� */
			}
			else
			{
				theApp.MainDlg->autodispDlg->DrawGraph(0);		/* Z ���� ��ġ�� SFR ������ �׷��� �׸��� */
			}

			if (!Task.bFirstAA == true)
			{
				if (Task.m_iCnt_Step_AA == model.m_iCnt_Check_SFR)
				{
					iRtnFunction = 63000;
				}
				else if (Task.m_bFlag_Decrease_SFR == true && Task.m_iCnt_Step_AA == model.m_iCnt_Check_SFR * 2)
				{
					iRtnFunction = 63000;
				}
				else
				{
					iRtnFunction = 64000;
				}
			}
			else
			{
				iRtnFunction = 64000;
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else
		{
			iRtnFunction = -63000;
		}
	}

	break;

	case 63000:		/* �ʱ� 3�� ������ Ȯ�� �� ���� ���̸� Z�� ���� �̵� */
	{
		iRtn = theApp.MainDlg->_checkDecreaseSFR();
		if (iRtn > 0)
		{
			iRtnFunction = 64000;
		}
		else
		{
			Task.m_bFlag_Decrease_SFR = true;
			theApp.MainDlg->autodispDlg->m_iCur_Index_Through = Task.m_iCnt_Step_AA_Total;

			//autodispDlg->DrawGraph(0);			//------>20180607_1
			//autodispDlg->DrawGraph(1);
			iRtnFunction = 60070;
		}
	}
	break;

	case 64000:
	{
		iRtn = 1;
		int iRtn2 = 1;

		iRtn = theApp.MainDlg->_checkMaxSfrPos(0);		/* MAX �� ã�� */

		theApp.MainDlg->ccdDlg->SetAlignData_Tilt(Task.SFR.dTilt_X, Task.SFR.dTilt_Y);

		if (iRtn>0)			// && iRtn2>0)
		{

			int mDixTx = 1;
			int mDixTy = 1;
			if (model.Tilt_Diretion[2] < 0)
			{
				mDixTx = -1;
			}
			if (model.Tilt_Diretion[3] < 0)
			{
				mDixTy = -1;
			}

			if (sysData.m_iDicChange == 0)										//IM
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltX() *  mDixTx;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltY() *  mDixTy;
			}
			else
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltY() *  mDixTx; ;// mDixTy;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltX() *  mDixTy; ;// mDixTx;
			}

			logStr.Format("TiltX : %.4f TiltY : %.4f", Task.SFR.dTilt_X, Task.SFR.dTilt_Y);
			putListLog(logStr);

			if (saveSfrLog(SFR_SECOND) == false)
			{
				iRtnFunction = -64000;
			}
			else if (fabs(Task.SFR.dTilt_X) > model.axis[theApp.MainDlg->TITLE_MOTOR_TX].m_dLimit_Err ||
				fabs(Task.SFR.dTilt_Y) > model.axis[theApp.MainDlg->TITLE_MOTOR_TY].m_dLimit_Err)
			{
				logStr.Format("Tilt ������ Limit �ʰ� : Tx : %.4f Ty : %.4f", Task.SFR.dTilt_X, Task.SFR.dTilt_Y);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -64000;
			}
			else
			{
				if (Task.bFirstAA == true)
				{
					//2��
					logStr.Format("	AA Step 2��[%d]", iRtnFunction);
					putListLog(logStr);
				}
				else
				{
					//1��
					logStr.Format("	AA Step 1��[%d]", iRtnFunction);
					putListLog(logStr);
				}
				iRtnFunction = 64200;		/* Focusing �Ϸ� */
			}
		}
		else
		{
			iRtnFunction = 60100;		/* �ٽ� ���� */
		}

		if (Task.m_iCnt_Step_AA>model.strInfo_AF1.m_iStepCnt + 5)
		{
			logStr.Format("SFR ���� �ִ� Ƚ�� �ʰ�.");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -64000;	/* ī��Ʈ �ʰ��� ���� */
		}

		double z_move_dist = fabs(model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] - motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z));

		if (z_move_dist > 1.0)//0.5
		{
			logStr.Format("[SFR ����] Z ���� �ϰ� �Ÿ� �ʰ� (1.0mm �̻� ���� �� �����ϴ�.)");
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -64000;
		}
	}
	break;

	case 64200:		/* Trough Focus �� Center SFR �� MAX ��ġ�� Z���� �̵� */
	{
		//theApp.MainDlg->_getMTF(SFR_STEP);			//250116 IM
		//saveSfrLog(SFR_STEP);
		double dCurPos = 0.0;
		double dMovePos = Task.SFR.dMaxPos[0];//-3.970

											  //////////////////////////////////////////////////////////////////////////////////////////
											  //////////////////////////////////////////////////////////////////////////////////////////

		double z_move_dist = 0.0;
		//PCB Z�s�� ���̹Ƿ� Tilt������ PCB-Z�s�� �ϱ� ������.
		//
		//2�� �Ѿ�鼭 Z�� ���� �κ�
		//Lens�� Z�� ���
		//Pcb�� Z�� �ϰ�
#if (____AA_WAY == PCB_TILT_AA)
		z_move_dist = fabs(model.axis[TITLE_MOTOR_Z].pos[Bonding_Pos] - dMovePos) * -1;	//1�� ->2 ���̵�
#elif (____AA_WAY == LENS_TILT_AA)
		z_move_dist = fabs(model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] - dMovePos) * 1;	//1�� ->2 ���̵�
#endif
		if (z_move_dist > 1.0)
		{
			logStr.Format("[SFR ����] Z ���� �ϰ� �Ÿ� �ʰ� (1.0mm �̻� ���� �� �����ϴ�.)");
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -64200;
			break;
		}

		motor.MoveAxis(theApp.MainDlg->TITLE_MOTOR_Z, ABS, dMovePos, /*model.strInfo_AF1.m_dMoveVel*/10.000, sysData.fMotorAccTime[theApp.MainDlg->TITLE_MOTOR_Z]);

		double ep = myTimer(true);

		while (1)
		{
			if (myTimer(true) - ep < 5000)
			{
				if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z) )
				{
					dCurPos = motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z);

					if (fabs(dCurPos - dMovePos) <= 0.003)
					{
						iRtnFunction = 64400;
						Task.PCBTaskTime = myTimer(true);

						break;
					}
				}
			}
			else
			{
				putListLog("Auto Focusing �� LENS Z Motor �̵� �ð� �ʰ�.");
				errMsg2(Task.AutoFlag, sLangChange);
				iRtnFunction = -64200;
				break;
			}
		}
	}

	break;


	case 64400:

		iRtn = theApp.MainDlg->_procTiltX();

		if (iRtn>0)
		{
			iRtnFunction = 64600;
		}
		else {
			iRtnFunction = -64400;
		}


		break;

	case 64600:

		iRtn = theApp.MainDlg->_procTiltY();

		if (iRtn>0)
		{
			iRtnFunction = 64700;
		}
		else {
			iRtnFunction = -64600;
		}


		break;

	case 64700:			/* Tilt ���� �� Delay */
		if (Task.bFirstAA == true)
		{
			if ((myTimer(true) - Task.PCBTaskTime) > model.strInfo_Cam[CCD].m_iDelayTime)
			{
				iRtnFunction = 65000;
			}
			else
			{
				iRtnFunction = 64700;
			}
		}
		else
		{
			iRtnFunction = 65000;
		}
		break;

	case 65000:			/* Tilt �� MTF ���� */
	{
		//Task.bFirstAA=true;
		if (Task.bFirstAA == true)//! Active Alignment 2��° ���� 
		{
			Sleep(200);
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);

			Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
			theApp.MainDlg->autodispDlg->DrawBarGraph();	//65000

			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())
				{//-- Align ���� Limit(������ �� �����...1.0)
					logStr.Format("ImageCenter NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);
					iRtnFunction = -65000;
					break;
				}
				else
				{
					if (Task.iMTFCnt < 6)
					{
						if (fabs(Task.m_dShift_IMG_X) > sysData.m_dOcSpec.x &&
							fabs(Task.m_dShift_IMG_Y) > sysData.m_dOcSpec.y)
						{
							logStr.Format("ImageCenter ���� Shift.  X %.3lf, Y %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
							putListLog(logStr);
							Sleep(200);
							iRtnFunction = 65500;
							break;
						}
						else
						{
							logStr.Format("ImageCenter ���� Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf", sysData.m_dOcSpec.x, sysData.m_dOcSpec.y, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
							putListLog(logStr);
						}
					}
				}

				bool bMtfCheck2 = true;
				MIU.func_Set_InspImageCopy(UV_BEFORE_CHART, MIU.m_pFrameRawBuffer);			//========  AA UV BEFORE===
				Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);				// [AA UV BEFORE] #1
				MandoInspLog.func_LogSave_UVBefore();	// UV�� Log Data ����

				theApp.MainDlg->autodispDlg->DrawBarGraph();	//65000
				if (!Task.sfrResult && bMtfCheck2)
				{ 
					saveInspImage(AA_NG_IMAGE_SAVE, Task.m_iCnt_Step_AA_Total);
					logStr.Format("UV�� SFR �˻� NG.  \nAA RETRY �Ͻðڽ��ϱ�?");//logStr.Format("SFR ���� �˻� NG.  \nAA RETRY �Ͻðڽ��ϱ�?");
					if (askMsg(logStr))
					{
						iRtnFunction = 60000;
						break;
					}
				}

				//saveSfrSumLog();//���� Data log ����

				double dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ;
				dCurPosX = motor.GetEncoderPos(Motor_PCB_X);
				dCurPosY = motor.GetEncoderPos(Motor_PCB_Y);
				dCurPosTH = motor.GetCommandPos(Motor_PCB_TH);
				dCurPosTX = motor.GetCommandPos(Motor_PCB_Xt);
				dCurPosTY = motor.GetCommandPos(Motor_PCB_Yt);
				dCurPosZ = motor.GetCommandPos(Motor_PCB_Z);
				if (!g_ADOData.func_AATaskToAAPcbMoter(Task.ChipID, dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ))
				{
					logStr.Format("DataBase ���� ����.[%d]\n MS Office�� �ݾ��ּ���.", iStep);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -69999;
					break;
				}
				iRtnFunction = 69999;
			}
			else
			{
				logStr.Format("SFR ���� �˻� ����.");
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -65000;
			}
			break;
		}
		else
		{//! Active Alignment 1��° ����
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())	//����� //����AA
				{
					//-- Align ���� Limit(������ �� �����...1.0)
					logStr.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);

					iRtnFunction = -65000;

					break;
				}
				else
				{
					if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x &&
						fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y)
					{//X/Y �������� Spec In�� ��� ���� Step
						logStr.Format("���� Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf", sysData.m_dOcSpec.x, sysData.m_dOcSpec.y, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 69999;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
					}
					else
					{
						logStr.Format("���� Shift.  X %.3lf, Y %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 65500;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
					}
				}
			}
			else
			{
				errMsg2(Task.AutoFlag, "SFR ���� ȹ�� ����.");
				iRtnFunction = -65000;
			}
		}
	}

	break;

	case 65500:
		if (!theApp.MainDlg->_MotorMove_IMG_Align())
		{
			iRtnFunction = -65500;
		}
		else
		{
			if (Task.iMTFCnt >= 5)
			{
				if (Task.bFirstAA == true) {
					iRtnFunction = 65520;// 65000;
					Task.PCBTaskTime = myTimer(true);
				}
				else
				{
					iRtnFunction = 69999;
				}
			}
			else
			{
				Task.iMTFCnt++;
				iRtnFunction = 65520;// 65000;
				Task.PCBTaskTime = myTimer(true);
			}
		}
		break;

	case 65520:
		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 65000;
		}

		break;

	case 69000:								// ���� SFR �˻� ��� ����..
	// SFR �˻� NG �߻� �� 1ȸ ��˻� �� ��� ����.
	if (myTimer(true) - Task.PCBTaskTime > 600)	//(model.strInfo_Cam[CCD].m_iDelayTime  *  2) || (myTimer(true)<Task.PCBTaskTime))
	{
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);		/* MTF�� ȹ�� */

		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();	//69000

		if (iRtn>0)
		{
			if (Task.m_b_AA_Retry_Flag == false)
			{
				Task.m_b_AA_Retry_Flag = true;		// TRUE�� ��� ���������� �˻� ��� NG �� �˶� �߻�..
				iRtnFunction = 60000;				// 60000���� 1, 2�� AA �˻� ��� �ʱ�ȭ..
			}
			else
			{
				saveInspImage(AA_NG_IMAGE_SAVE, Task.m_iCnt_Step_AA_Total);
				Dio.setAlarm(ALARM_ON);

				logStr.Format("SFR ���� �˻� NG. �ڵ� ���� �Ͻðڽ��ϱ�?");
				if (askMsg(sLangChange))
				{
					Dio.setAlarm(ALARM_OFF);
					logStr.Format("NG ���� �Ͻðڽ��ϱ�. (��:NG, �ƴϿ�:OK)");
					if (askMsg(logStr)) {
						Task.m_bOkFlag = 0;
					}
					else {
						Task.m_bOkFlag = 1;
					}
					iRtnFunction = 69999;
				}
				else
				{
					Dio.setAlarm(ALARM_OFF);

					//Task.PauseLensStep = abs(Task.LensTask);
					Task.PausePCBStep = abs(Task.PCBTask);
					Task.AutoFlag = 2;		// �Ͻ�����
					theApp.MainDlg->AutoRunView(Task.AutoFlag);
					iRtnFunction = -65000;
				}
			}
		}
		else
		{
			logStr.Format("SFR ���� �˻� (2��) ����.");
			errMsg2(Task.AutoFlag, sLangChange);
			iRtnFunction = -65000;
		}
	}
	break;
	case 69999:
	{
		double dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ;
		dCurPosX = motor.GetEncoderPos(Motor_PCB_X);
		dCurPosY = motor.GetEncoderPos(Motor_PCB_Y);
		dCurPosTH = motor.GetCommandPos(Motor_PCB_TH);
		dCurPosTX = motor.GetCommandPos(Motor_PCB_Xt);
		dCurPosTY = motor.GetCommandPos(Motor_PCB_Yt);
		dCurPosZ = motor.GetCommandPos(Motor_PCB_Z);
		//
		theApp.MainDlg->putListLog("	");
		logStr.Format("database teaching data save [%s]", Task.ChipID);
		theApp.MainDlg->putListLog("dbData/teachingData");
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA x: %lf/%lf", dCurPosX, model.axis[Motor_PCB_X].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA y: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Y].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA th: %lf/%lf", dCurPosX, model.axis[Motor_PCB_TH].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA tx: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Xt].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA ty: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Yt].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		logStr.Format("AA z: %lf/%lf", dCurPosX, model.axis[Motor_PCB_Z].pos[Bonding_Pos]);
		theApp.MainDlg->putListLog(logStr);
		//
		if (!g_ADOData.func_AATaskToAAPcbMoter(Task.ChipID, dCurPosX, dCurPosY, dCurPosTH, dCurPosTX, dCurPosTY, dCurPosZ))
		{
			logStr.Format("DataBase ���� ����.[%d]\n MS Office�� �ݾ��ּ���.", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -69999;
			break;
		}
		if (Task.bFirstAA == true)
		{
			Task.m_timeChecker.Measure_Time(7);	//AA �Ϸ� �ð�
			Task.m_dTime_FineAA = Task.m_timeChecker.m_adTime[5] - Task.m_timeChecker.m_adTime[4];
			theApp.MainDlg->dispGrid();

			iRtnFunction = 110000;
		}
		else
		{
			iRtnFunction = 80000;
		}
	}

	break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::UV_process(int iStep)
{
	CString sLog = _T("");
	CString sImgPath = _T("");
	//	int iRtn;
	int reCnt = 0;
	int iRtnFunction = iStep;
	double curMotorDist[3];

	switch (iStep)
	{
	case 110000:
		//if (sysData.m_testRunMode == true)
		//{
		//	iRtnFunction = 120000;

		//	break;
		//}
		iRtnFunction = 110500;
		break;

	case 110500:
		if (theApp.MainDlg->Bonding_Pos_Ckeck())
		{
			sLog.Format("AA after PCB Z : %f , set Offset Z : %f", motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z), sysData.m_dOffset_Prev_UV_Z);	//AA�� Lens Z : %f , ���� Offset Z : %f
			putListLog(sLog);

			theApp.MainDlg->MoveOffset_Prev_UV();	/* PCB X, Y, Z, Tx, Ty, PCB Th UV �� offset �̵� */

			Sleep(200);
			//if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
			if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z) && motor.GetInposition(theApp.MainDlg->TITLE_MOTOR_Z))
			{
				iRtnFunction = 110600;
				Task.PCBTaskTime = myTimer(true);
			}
			else
			{
				iRtnFunction = -110500;
			}
		}
		break;

	case 110600:
		Sleep(200);
		saveSfrLog(SFR_OFFSET_DATA);
		theApp.MainDlg->_getMTF(SFR_AFTER_OFFSET_MOVE);

		iRtnFunction = 110650;
		Task.PCBTaskTime = myTimer(true);
		break;

	case 110650:					//UV�� MTF �˻� ����

		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();
		Task.bInsCenter = false;

		iRtnFunction = 110700;


		break;


	case 110700:
		iRtnFunction = 111000;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 111000:	// UV ��ȭ
	{
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "UV ��ȭ");

		Task.PCBTaskTime = myTimer(true);

		if (sysData.m_iUVPass == 1)
		{
			sLog.Format("====UV Pass [%d]====", iStep);
			putListLog(sLog);
			iRtnFunction = 113500;
			break;
		}
#if 1
		sLog.Format("===> UV ON");
		putListLog(sLog);

		//Dio.UVTrigger(true, false);
		UVCommand.UV_Shutter_Open();
		Sleep(model.UV_Time);

		//Dio.UVTrigger(false, false);
		UVCommand.UV_Shutter_Close();
		sLog.Format("===> UV OFF [%d]", iStep);
		putListLog(sLog);
#else
		UVControl.Altis_UC_Controller_OnOff(1, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(2, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(3, 1);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(4, 1);
		Sleep(10);

		sLog.Format("===> UV ON");
		putListLog(sLog);
		Sleep(model.UV_Time);

		UVControl.Altis_UC_Controller_OnOff(1, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(2, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(3, 0);
		Sleep(100);
		UVControl.Altis_UC_Controller_OnOff(4, 0);
		Sleep(10);

		sLog.Format("===> UV OFF [%d]", iStep);

		putListLog(sLog);

#endif
		iRtnFunction = 113500;
	}
	break;

	case 113500:
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "UV ��ȭ �Ϸ�");//"UV ��ȭ �Ϸ�"

		Task.m_timeChecker.Measure_Time(8);	//UV �Ϸ� �ð�
		Task.m_dTime_UV = Task.m_timeChecker.m_adTime[8] - Task.m_timeChecker.m_adTime[7];


		if (myTimer(true) - Task.PCBTaskTime> 100)  // Delay üũ
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 115000;
		}
		break;

	case 115000:
		curMotorDist[0] = fabs(motor.GetEncoderPos(Motor_Lens_X) - model.axis[Motor_Lens_X].pos[Bonding_Pos]);
		curMotorDist[1] = fabs(motor.GetEncoderPos(Motor_Lens_Y) - model.axis[Motor_Lens_Y].pos[Bonding_Pos]);
		curMotorDist[2] = fabs(motor.GetEncoderPos(Motor_Lens_Z) - model.axis[Motor_Lens_Z].pos[Bonding_Pos]);

		if ((curMotorDist[0] < model.axis[Motor_Lens_X].m_dLimit_Err && curMotorDist[1] < model.axis[Motor_Lens_Y].m_dLimit_Err && curMotorDist[2] < model.axis[Motor_Lens_Z].m_dLimit_Err))
		{
			Task.PcbOnStage = 200;			// �˻� �Ϸ�.
			
			if (!Dio.HolderGrip(false, true))				//Lens Gripper open
			{
				sLog.Format("Lens Grip Open ���� [%d]", iStep);
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -115000;
				break;
			}
			else
			{
				Task.m_bOKLensPass = -1;//���� �ѱ� �Ϸ� Lamp ���
				Task.PCBTaskTime = myTimer(true);
				iRtnFunction = 115300;
				break;
			}
			

		}

		break;
	case 115300:
		iRtnFunction = 115310;
		break;
	case 115310:	//jump Step	
		if (!Dio.LensMotorGrip(false, true))
		{

			sLog.Format("Lens Unit Gripper ���� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -115310;
			break;
		}
		Task.m_bOKLensPass = -1;//���� �ѱ� �Ϸ� Lamp ���


		Task.m_iFineThCnt = 0;
		iRtnFunction = 115400;
		break;
	case 115400:
		if (Dio.CamLaserSlinderMove(false, true))
		{
			iRtnFunction = 115450;
			break;
		}
		else
		{
			sLog.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -115400;
			break;
		}
		break;
	case 115450:
		if (Dio.CamLaserSlinderCheck(false, false))
		{
			sLog.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 115500;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)//IO_DELAY_TIME)
		{
			sLog.Format("Laser/Cam �Ǹ��� ���� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -115450;
			break;
		}
		break;
	case 115500: //UV�Ϸ��� �����˻�
		Task.m_bOkFlag = 0;
		work.m_iCnt_Output++;
		work.Save();

		iRtnFunction = 120000;
		theApp.MainDlg->_getMTF(SFR_AFTER_UV);
		break;


	default:
		sLog.Format("UV Process Step ��ȣ�� �������Դϴ�. [%d]", iStep);
		errMsg2(Task.AutoFlag, sLog);
		break;
	}
	sLog.Empty();
	sImgPath.Empty();
	return iRtnFunction;
}

int	CPcbProcess2::Complete_FinalInsp(int iStep)
{
	CString sLog = _T("");
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iRtn = 0;
	int iRtnFunction = iStep;
	bool bMtfCheck4 = true;
	bool beolRtn = false;
	switch (iStep)
	{
	case 120000:	//����ǰ
		Task.m_iFineThCnt = 0;
		theApp.MainDlg->setCamDisplay(3, 1);
		Sleep(dFinalDelay);

		//g_clApsInsp.func_insp_Voltage();

		iRtnFunction = 121000;
		break;
	case 121000:
		//g_clApsInsp.func_Insp_CurrentMeasure();
		iRtnFunction = 122000;
		break;

	case 122000:	//����ǰ �˻縸 �ش� (Final �˻��� X/Y/T ����)
		Sleep(dFinalDelay);
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
		Sleep(300);
		if (iRtn > 0)
		{
			//if (Task.m_iFineThCnt == 0)
			//{
			//	theApp.MainDlg->g_CalcImageAlign(true);	//����ǰ rotation �˻� 240131
			//}


			if (!theApp.MainDlg->_calcImageAlignment())	//-- Align ���� Limit(������ �� �����...1.0)
			{
				sLog.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(sLog);
				iRtnFunction = -122000;
				break;
			}
			else
			{

				//ƿ�ð� �ȸ��� ��� ���� �����ϰ� �ݺ���.. 3ȸ ������ �ϰ� �ȸ����� Spec IN ��Ű��.., 
				if (Task.m_iFineThCnt > 3)	Task.m_dShift_IMG_TH = 0.0;
				if (fabs(Task.m_dShift_IMG_TH) <= sysData.dSpec_OC_Theta)//X/Y �������� Spec In�� ��� ���� Step
				{
					sLog.Format("	���� Shift. Spec In(%.03lf) In : T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					iRtnFunction = 122500;
				}
				else
				{
					sLog.Format("	���� Shift(Spec : %.03lf).  T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					Task.m_iFineThCnt++;
					iRtnFunction = 122100;
				}
			}
		}
		else
		{
			errMsg2(Task.AutoFlag, "MTF ���� �� SFR ���� ȹ�� ����..");
			iRtnFunction = -122000;
		}
		break;

	case 122100:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -122100;
		}
		else
		{
			Sleep(100);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122200;	//Retry
		}
		break;
	case 122200:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 122000;
			sLog.Format("	AA Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		break;
	case 122500:	//Final �� �˻� - Ȯ�ο�
		theApp.MainDlg->setCamDisplay(3, 1);
		Sleep(dFinalDelay);

		MIU.func_Set_InspImageCopy(EOL_CHART, MIU.m_pFrameRawBuffer);	//================  ����ǰ ��� chart
		Sleep(100);
		iRtnFunction = 122520;
		break;
	case 122520:
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);//EOL ȭ�� #1

		if (Task.sfrResult == true)
		{
			putListLog("[�˻�] MTF �˻� ����.");// Optic_Axis,
		}
		else
		{
			sLog.Format("[�˻�] MTF �˻� Fail.[%d]", iStep);
			putListLog(sLog);
		}
		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();	//122520	Complete_FinalInsp
		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD, true);

		iRtnFunction = 122550;   //122600; <- �̹��� save ���常 �Ҷ�
		break;

	case 122550:
		
		iRtnFunction = 122560;
		break;
	case 122560:
		iRtnFunction = 122565;
		break;
	case 122565:
		//Sleep(500);
		iRtnFunction = 122570;
		break;
	case 122570:
		iRtnFunction = 122575;
		break;
	case 122575:
		iRtnFunction = 122580;
		break;
	case 122580:
		//mtf ����
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		//Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);//EOL ȭ�� #1


		//Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		//theApp.MainDlg->autodispDlg->DrawBarGraph();		//122580   Complete_FinalInsp
		//vision.FnShmEdgeFind(MIU.m_pFrameRawBuffer);
		/////MandoInspLog.func_LogSave_UVAfter(1);	//eol _RAW.csv  �̹��˻��� ���� ����

		//MandoInspLog.func_LogSave_Shm_vertex();


		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD, true);
		iRtnFunction = 122590;  // 122590; Chart YUV - RAW 6500 RAW			IMHS	JUMP
		break;
	case 122590:
		iRtnFunction = 122600;
		break;
	case 122600:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� �����ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122600;
			break;
		}
		iRtnFunction = 122650;
		break;
	case 122650:
		iRtnFunction = 122700;
		break;
	case 122700:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("Lens�� z�� ���� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -122700;
			break;
		}

		Task.LensTaskTime = myTimer(true);
		//Task.m_iRetry_Opt = 0;
		iRtnFunction = 123000;
		break;
	case 123000:
		if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Defect Insp[%d]"), iStep);
			putListLog(sLog);
			iRtnFunction = 123400;
			break;
		}
		else
		{
			//iRtnFunction = 123100;
			break;
		}
		break;
	case 123100:
		if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123100;
			break;
		}
		else
		{
			sLog.Format("PCB�� Dark ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 123200;
			break;
		}
		break;
	case 123200:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark Z ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123200;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 123300;
		break;
	case 123300:
		Sleep(100);
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);		//����ǰ dark
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z DAR ����Ȯ�� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123200;
			break;
		}
		break;
	case 123400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� �����ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123400;
			break;
		}
		iRtnFunction = 123500;
		break;
	case 123500:
		if (sysData.m_iStaintInspPass == 1 && sysData.m_iDefectInspPass == 1)
		{
			iRtnFunction = 127000;
			break;
		}
		LightControl.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_STAIN]);

		iRtnFunction = 123600;
		break;
	case 123600:
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLog.Format("PCB�� ���� Defect �˻� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123600;
			break;
		}
		else
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123650;
			break;
		}
		break;

	case 123650:
		//�˻� �� �����ġ Ȯ��
		iRtnFunction = 123670;
		break;
	case 123670:
		if (!motor.Pcb_Tilt_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Tx Ty �� �̹��˻� ��ġ �̵� ����[%d]", iStep);	//�輼�� ���� ��û 
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123670;
			break;
		}
		else
		{
			sLog.Format("PCB Tx Ty �� �̹��˻� ��ġ �̵� ����[%d]", iStep);	//�輼�� ���� ��û 
			putListLog(sLog);
			iRtnFunction = 123700;
			break;
		}
		break;
	case 123700:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123800;
			break;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
			break;
		}
		break;
	case 123800:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			iRtnFunction = 123850;
			break;
		}
		break;
	case 123850:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Task.PCBTaskTime = myTimer(true);

			//6500k �̹��� ����
			Sleep(300);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);			//oc
			//ccdDlg->m_pSFRDlg->Raw_ImageSave(MIU.m_pFrameRawBuffer, MID_6500K_RAW);
			//MID_6500K_RAW �̹��� ����

			iRtnFunction = 123900;
			break;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Check Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
			break;
		}
		break;
	case 123900:
		Sleep(dFinalDelay);
		if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Defect Insp[%d]"), iStep);
			putListLog(sLog);
		}
		else
		{
			if (g_clApsInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, true) == true)	//EOL
			{
				putListLog("[�˻�] Defect �˻� ����.");
			}
			else
			{
				sLog.Format("[�˻�] Defect �˻�  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}
		}
		if (sysData.m_iStaintInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Stain Insp[%d]"), iStep);
			putListLog(sLog);

		}
		else
		{
			if (LGIT_MODEL_INDEX == M2_FF_MODULE)		//g_GetIllumination ����ǰ
			{
				//�̹��������� oc ���� ��û240725
				g_clApsInsp.g_GetIllumination(MIU.vDefectMidBuffer_6500K);
				Sleep(100);
			}


			if (g_clApsInsp.func_Insp_Stain(MIU.vDefectMidBuffer_6500K) == true)
			{
				putListLog("[�˻�] Stain �˻� ����.");
			}
			else
			{
				sLog.Format("[�˻�] Stain �˻�  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}

		}
		

		iRtnFunction = 127000;	/// 127150;// 127000;			IMHS	JUMP
		break;
	case 127000:
		iRtnFunction = 127500;
		break;
	case 127500:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127600;
		}
		else
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127500;
		}
		break;

	case 127600:
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 127700;
		break;
	case 127700:
		//Head Model
		MIU.OtpRead_Head_Fn();
		MIU.partNumberVerifyFn();
		MIU.FwVersionRead_Head_Fn();
		MIU.SensorIdRead_Head_Fn();
		iRtnFunction = 127710;
		break;
	case 127710:
		MandoInspLog.func_LogSave_UVAfter(0);	//eol _RAW.csv
		MIU.Close();
		iRtnFunction = 127800;
		break;
	case 127800:
		MESCommunication.OTPVerifyFail.Empty();
		MESCommunication.sensorId.Empty();
		MESCommunication.partNumber.Empty();
		MESCommunication.fwVersion.Empty();
		iRtnFunction = 127900;
		break;
	case 127900:
		Task.m_iRetry_Opt = 0;

		//g_FinalInspLog();

		work.iInspCnt_Output++;
		if (MandoInspLog.bInspRes == false)	work.iInspCnt_NG_Output++;
		theApp.MainDlg->dispInspResGrid();
		work.Save();
		work.Load();

		Task.m_timeChecker.Measure_Time(13);	//Final �˻� �Ϸ� �ð�
		Task.m_dTime_TotalAA = Task.m_timeChecker.m_adTime[13] - Task.m_timeChecker.m_adTime[12];
		theApp.MainDlg->dispGrid();

		iRtnFunction = 128000;
		break;
	case 128000:
		Sleep(100);
		vision.clearOverlay(CCD);
		if (MandoInspLog.bInspRes == true)
		{
			vision.textlist[CCD].addList(50, 100, "PASS", M_COLOR_GREEN, 65, 85, "Arial");
		}
		else
		{
			vision.textlist[CCD].addList(50, 100, "FAIL", M_COLOR_RED, 80, 100, "Arial");
			MandoInspLog.func_DrawNG_Overlay(CCD);	//ȭ�� Overlay�� NG List Draw
			work.m_iCnt_NG_Output++;
		}
		vision.drawOverlay(CCD, true);

		//ccd���� ��ȯ

		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->setCamDisplay(3, 1);
		theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
		iRtnFunction = 128100;
		break;

	case 128100:
		iRtnFunction = 129200;
		break;
	case 129200:
		if (!motor.Lens_Motor_MoveXY(Wait_Pos))
		{
			sLog.Format("Lens�� ���� ��� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129200;
			break;
		}
		else
		{
			iRtnFunction = 129220;
		}
		break;

	case 129220:
		iRtnFunction = 129300;
		break;

		break;
	case 129300:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129350;
		}
		else
		{
			sLog.Format("PCB ��� ��ġ �̵� ���� [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129300;
		}

		break;
	case 129350:
		//Dio.PcbGrip(false, false);
		iRtnFunction = 129400;
		break;
	case 129400:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129410;
		}
		else
		{
			sLog.Format("PCB TX,TY �����ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129400;
		}
		break;
	case 129410:
		if (Dio.DoorLift(true, false))
		{
			sLog.Format("Door Open [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129500;
			break;
		}
		else
		{
			sLog.Format("Door Close ���� [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129410;
			break;
		}
		break;
	case 129500:
		Task.m_bOkDispense = -1;	//���� ���� �Ϸ� Lamp ���
		putListLog("		");
		putListLog("		");
		sLog.Format("			END =======%s", Task.ChipID);
		putListLog(sLog);
		putListLog("		");
		putListLog("		");
		Task.m_bPBStart = 0;	//���ڵ� ���� �ʱ�ȭ


		memset(Task.ChipID, 0x00, 256);
		//sprintf_s(Task.ChipID, _T("EMPTY"));

		_stprintf_s(Task.ChipID, sizeof(Task.ChipID), _T("EMPTY"));
		theApp.MainDlg->func_ChipID_Draw();

		Task.m_timeChecker.Measure_Time(14);	//����ǰ ���� �ð�
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		Task.PCBTask = 10000;
		iRtnFunction = 10000;
		break;
	default:
		sLog.Format("Mando Test Process Step Number Error. [%d]", iStep);
		errMsg2(Task.AutoFlag, sLog);
		iRtnFunction = -iStep;
		break;
	}
	return iRtnFunction;
}

int	CPcbProcess2::func_MandoFinalSFR(int iStep)
{
	CString sLog = _T("");
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	double offsetX = 0.0;
	double offsetY = 0.0;
	double offsetTh = 0.0;
	int iRtn = 0;
	int iRtnFunction = iStep;
	bool bUseDispense = true;	//���� Pass ����

	if ((sysData.m_iDispensePass == 1))
	{
		bUseDispense = false;//���� Pass Mode �̰ų�, ���� �Ϸ� �� ���
	}
	else
	{
		bUseDispense = true;
	}

	switch (iStep)
	{
	case 120000:
		//if (sysData.m_testRunMode == true)
		//{
		//	iRtnFunction = 122550;///// 125000;

		//	break;
		//}
		//g_clApsInsp.func_insp_Voltage();
		iRtnFunction = 121000;
		break;

	case 121000:
		//g_clApsInsp.func_Insp_CurrentMeasure();
		iRtnFunction = 122000;
		break;
	case 122000:	//����ǰ �˻縸 �ش� (Final �˻��� X/Y/T ����)
		Sleep(dFinalDelay);
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
		if (iRtn > 0)
		{
			if (!theApp.MainDlg->_calcImageAlignment())	//-- Align ���� Limit(������ �� �����...1.0)
			{
				sLog.Format("NG.  X : %.3lf Y : %.3lf", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
				putListLog(sLog);
				iRtnFunction = -122000;
				break;
			}
			else
			{
				//ƿ�ð� �ȸ��� ��� ���� �����ϰ� �ݺ���.. 3ȸ ������ �ϰ� �ȸ����� Spec IN ��Ű��..,
				if (Task.m_iFineThCnt > 5)	Task.m_dShift_IMG_TH = 0.0;
				if (fabs(Task.m_dShift_IMG_TH) <= sysData.dSpec_OC_Theta)//X/Y �������� Spec In�� ��� ���� Step
				{
					sLog.Format("	���� Shift. Spec In(%.03lf) In : T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					iRtnFunction = 122500;
				}
				else
				{
					sLog.Format("	���� Shift(Spec : %.03lf).  T: %.3lf", sysData.dSpec_OC_Theta, Task.m_dShift_IMG_TH);
					putListLog(sLog);
					if (bUseDispense == false) //AAB, ���� �̻���� ���->���� Shift���ϰ� ����
					{
						iRtnFunction = 122500;
					}
					else//���� Shift ����
					{
						Task.m_iFineThCnt++;
						iRtnFunction = 122100;
					}
				}
			}
		}
		else
		{
			if (askMsg("����?") == IDOK)
			{
				iRtnFunction = 122500;
				break;
			}
			else
			{
				errMsg2(Task.AutoFlag, "MTF ���� �� SFR ���� ȹ�� ����..");
				iRtnFunction = -122000;
			}
		}
		break;

	case 122100:
		if (!theApp.MainDlg->_MotorMove_IMG_AlignTheta())
		{
			iRtnFunction = -122100;
		}
		else
		{
			Sleep(100);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122200;	//Retry
		}
		break;
	case 122200:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 122000;
			sLog.Format("	AA Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		break;
	case 122500:	//Final �� �˻� - Ȯ�ο�
		theApp.MainDlg->setCamDisplay(3, 1);

		Sleep(dFinalDelay);
		MIU.func_Set_InspImageCopy(UV_AFTER_CHART, MIU.m_pFrameRawBuffer);		//========AA UV AFTER===
		Sleep(300);
		iRtnFunction = 122520;
		break;
	case 122520:

		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);			// [AA UV AFTER] #1

		if (Task.sfrResult == true)
		{
			putListLog("[�˻�] MTF �˻� ����.");// Optic_Axis,
		}
		else
		{
			putListLog("[�˻�] MTF �˻� ����.");// Optic_Axis,
		}
		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();
		iRtnFunction = 122550;

		break;
	case 122550:			//jump step ���� nono
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB�� z�� ���� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
			break;
		}

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("Lens�� z�� ���� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
			break;
		}



		iRtnFunction = 122700;
		break;
	case 122700:

		sLog.Format("Lens Y ��� ��ġ �̵� ���[%d]", iStep);
		putListLog(sLog);

		motor.Lens_Motor_MoveY(Wait_Pos, 0.0, false);		//Lens x ���� �̵� 240108

		Task.m_iRetry_Opt = 0;
		iRtnFunction = 122800;
		break;
	case 122800:
		iRtnFunction = 123000;

		//if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		//{
		//	sLog.Format(_T("[PASS] Defect Insp[%d]"), iStep);
		//	putListLog(sLog);
		//	iRtnFunction = 123000;
		//}
		//else
		//{
		//	iRtnFunction = 122850;
		//}
		break;
#if 0
	case 122850:

		if (motor.Pcb_Motor_MoveY(Wait_Pos))
		{
			iRtnFunction = 122860;
			sLog.Format("Pcb y ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
		}
		else
		{
			sLog.Format("Pcb y ��� ��ġ �̵� ���� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122850;
		}
		break;
	case 122860:
		if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122860;
		}
		else
		{
			sLog.Format("PCB�� Dark ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 122900;
		}

		break;
	case 122900:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark Z ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122900;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 122950;
		break;
	case 122950:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 122960;
		}
		break;
	case 122960:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);			//aa Dark
			iRtnFunction = 122970;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z DAR ����Ȯ�� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -122900;
		}

		break;

	case 122970:

		iRtnFunction = 123000;
		break;
#endif
	case 123000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� �����ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123000;
			break;
		}
		sLog.Format("Pcb Z�� �����ġ �̵� �Ϸ� [%d]", iStep);
		putListLog(sLog);
		iRtnFunction = 123050;
		break;
	case 123050:
		/*Sleep(5000);
		if (motor.Pcb_Motor_MoveY(OC_6500K_Pos))
		{
			sLog.Format("Pcb Y �̹��˻� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 123051;
			break;
		}
		else
		{
			sLog.Format("Pcb Y �̹��˻� ��ġ �̵� ���� [%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123050;
			break;
		}
		break;*/
		iRtnFunction = 123051;
		break;
	case 123051:
		if (sysData.m_iStaintInspPass == 1)
		{
			sLog.Format("Pcb Y �̹��˻� PASS [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 124500;
			break;
		}
		LightControl.ctrlLedVolume(LIGHT_OC, model.m_iLedValue[LEDDATA_STAIN]);

		iRtnFunction = 123055;

		break;
	case 123055:
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB�� �̹��˻� ��ġ �̵� ����[%d]", iStep);	//PCB�� ���� Defect �˻� ��ġ �̵� ����[%d]
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123055;
			break;
		}
		else
		{
			iRtnFunction = 123056;
			break;
		}
		break;
	case 123056:
		if (!motor.Pcb_Tilt_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Tx Ty �� �̹��˻� ��ġ �̵� ����[%d]", iStep);	//�輼�� ���� ��û 
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123056;
			break;
		}
		else
		{
			sLog.Format("PCB Tx Ty �� �̹��˻� ��ġ �̵� ����[%d]", iStep);	//�輼�� ���� ��û 
			putListLog(sLog);
			iRtnFunction = 123058;
			break;
		}
		break;
	case 123058:
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 123100;
		break;
	case 123100:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
			break;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
			break;
		}
		break;
	case 123400:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);		//aa oc
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 124000;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z 6500K ����Ȯ�� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
			break;
		}
		break;
	case 124000:
		//if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		//{
		//	sLog.Format(_T("[PASS] Defect Insp [%d]"), iStep);
		//	putListLog(sLog);
		//}
		//else
		//{
		//	if (g_clApsInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, true) == true)	//AA
		//	{
		//		putListLog("[�˻�] Defect �˻� ����.");
		//	}
		//	else
		//	{
		//		sLog.Format("[�˻�] Defect �˻�  Error[%d]", iStep);
		//		putListLog(sLog);
		//		MandoInspLog.bInspRes = false;
		//	}
		//}
		if (sysData.m_iStaintInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Stain Insp [%d]"), iStep);
			putListLog(sLog);

		}
		else
		{

			if (LGIT_MODEL_INDEX == M2_FF_MODULE)		//g_GetIllumination AA
			{
				//�̹��������� oc ���� ��û240725
				g_clApsInsp.g_GetIllumination(MIU.vDefectMidBuffer_6500K);
				Sleep(100);
			}


			if (g_clApsInsp.func_Insp_Stain(MIU.vDefectMidBuffer_6500K) == true)
			{
				putListLog("[�˻�] Stain �˻� ����.");
			}
			else
			{
				sLog.Format("[�˻�] Stain �˻�  Error[%d]", iStep);
				putListLog(sLog);
				MandoInspLog.bInspRes = false;
			}
		}

		iRtnFunction = 124500;
		break;
	case 124500:
		iRtnFunction = 124501;
		break;
	case 124501:
		iRtnFunction = 124502;
		break;
	case 124502:
		//Head Model
		//otp Write
		if (!MIU.OtpWrite_Head_Fn())
		{
			sLog.Format("OTP Write Fail [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			putListLog(sLog);
			MESCommunication.OTPWriteFail.Format(_T("NG"));
			MandoInspLog.bInspRes = false;
			iRtnFunction = -124502;
			break;
		}
		sLog.Format("OTP Write...Sucsess [%d]", iStep);
		MESCommunication.OTPWriteFail.Format(_T("PASS"));
		putListLog(sLog);
		iRtnFunction = 124503;
		break;
	case 124503:
		//OTP
		//ccd close  MIU.Close

		if (!MIU.Close())
		{
			sLog.Format("MIU Close ���� [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -124503;
			break;
		}
		sLog.Format("MIU Close... [%d]", iStep);
		putListLog(sLog);
		iRtnFunction = 124504;
		break;
	case 124504:
		//ccd open  MIU.Open

		if (!theApp.MainDlg->MIUCheck_process())
		{
			sLog.Format("CCD ��� ���� �ʱ�ȭ ����.\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.");
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -124504;
			break;
		}
		sLog.Format("CCD Start ����[%d]", iStep);
		putListLog(sLog);
		iRtnFunction = 124505;
		break;
	case 124505:
		// otp Fw ver
		MIU.FwVersionRead_Head_Fn();
		iRtnFunction = 124506;
		break;
	case 124506:
		//Otp OtpRead_Head_Fn

		MIU.OtpRead_Head_Fn();
		sLog.Format("OTP Read [%d]", iStep);
		putListLog(sLog);
		iRtnFunction = 124507;
		break;
	case 124507:
		//Sensor ID Read
		MIU.SensorIdRead_Head_Fn();
		sLog.Format("Sensor ID Read [%d]", iStep);
		putListLog(sLog);
		iRtnFunction = 124508;
		break;
	case 124508:
		//Otp Verify
		if (!MIU.partNumberVerifyFn())
		{
			sLog.Format("OTP Verify Fail [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			MandoInspLog.bInspRes = false;
			putListLog(sLog);
			iRtnFunction = -124508;
			break;
		}
		sLog.Format("OTP Verify...Sucsess [%d]", iStep);
		iRtnFunction = 124510;
		break;
	case 124510:
		MIU.Close();
		iRtnFunction = 125000;
		break;
	case 125000: 	//jump step ���� nono
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 125500;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -125000;
		}
		break;
	case 125500:
		iRtnFunction = 126000;
		break;
	case 126000:
		iRtnFunction = 127000;
		break;
	case 127000:
		iRtnFunction = 127100;
		break;
	case 127100:
		Task.m_iRetry_Opt = 0;
		MandoInspLog.func_LogSave_UVAfter(0);	//���� ������Camera �˻� Log ����(�˻�/������ ���� -> ����Log)


		/*inspMES();
		MESCommunication.Messave();*/
		//! �˻� Count ó��
		/*work.iInspCnt_Output++;
		if(MandoInspLog.bInspRes == false)	work.iInspCnt_NG_Output++;
		dispInspResGrid();
		work.Save();
		work.Load();*/

		Task.m_timeChecker.Measure_Time(13);	//Final �˻� �Ϸ� �ð�
		Task.m_dTime_TotalAA = Task.m_timeChecker.m_adTime[13] - Task.m_timeChecker.m_adTime[12];
		///dispGrid();

		iRtnFunction = 127300;
		break;

	case 127300:
		Sleep(100);
		vision.clearOverlay(CCD);
		if (MandoInspLog.bInspRes == true)
		{
			vision.textlist[CCD].addList(50, 100, "PASS", M_COLOR_GREEN, 65, 85, "Arial");
		}
		else
		{
			vision.textlist[CCD].addList(50, 100, "FAIL", M_COLOR_RED, 80, 100, "Arial");
			MandoInspLog.func_DrawNG_Overlay(CCD);	//ȭ�� Overlay�� NG List Draw
			work.m_iCnt_NG_Output++;
		}
		vision.drawOverlay(CCD, true);

		//ccd���� ��ȯ

		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->setCamDisplay(3, 1);
		theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
		iRtnFunction = 127400;
		break;

	case 127400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127400;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 127500;
		break;
	case 127500:
		//OTP
		MESCommunication.OTPWriteFail.Empty();      
		MESCommunication.OTPVerifyFail.Empty();
		MESCommunication.sensorId.Empty();
		MESCommunication.partNumber.Empty();
		iRtnFunction = 127510;
		break;
	case 127510:
		iRtnFunction = 127520;
		break;
	case 127520:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127900;

		}
		else
		{
			sLog.Format("PCB ��� ��ġ �̵� ���� [%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127520;
		}
		break;
	case 127900:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127950;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			sLog.Format("PCB TX,TY �����ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127900;
		}
		break;
	case 127950:

		if (motor.IsStopAxis(Motor_Lens_Y) && motor.checkLensMotorPos(Wait_Pos, 0))
		{
			iRtnFunction = 128000;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format(_T("Lens y axis ��� ��ġ Ȯ�� ����[%d]"), iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127900;
		}

		break;
	case 128000:
		if (!motor.Lens_Motor_MoveXY(Wait_Pos))
		{
			sLog.Format("Lens�� ���� ��� ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128000;
		}
		else
		{
			iRtnFunction = 128200;
		}
		break;
	case 128200:
		if (Dio.DoorLift(true, false))
		{
			sLog.Format("Door Open [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 128500;
			break;
		}
		else
		{
			sLog.Format("Door Close ���� [%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128200;
			break;
		}
		break;
	case 128500:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 129000;
			break;
		}
		else
		{
			sLog.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� ����");
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128500;
			break;
		}
		break;

	case 129000:
		iRtnFunction = 129500;
		break;
	case 129500:
		Task.m_bOkDispense = -1;	//���� ���� �Ϸ� Lamp ���
		putListLog("		");
		putListLog("		");
		sLog.Format("			END =======%s", Task.ChipID);
		putListLog(sLog);
		putListLog("		");
		putListLog("		");
		putListLog("		");
		putListLog("		");
		Task.m_bPBStart = 0;	//���ڵ� ���� �ʱ�ȭ
	
		memset(Task.ChipID, 0x00, 256);
		_stprintf_s(Task.ChipID, sizeof(Task.ChipID), _T("EMPTY"));
		theApp.MainDlg->func_ChipID_Draw();

		Task.m_timeChecker.Measure_Time(14);	//����ǰ ���� �ð�
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);

		Dio.PCBvaccumOn(VACCUM_OFF, false);

		//////////////////////////////////////////////////////////////////////////////////////////////////////
		
		Task.PCBTask = 10000;
		iRtnFunction = 10000;
		break;

	default:
		sLog.Format("Mando Test Process Step Number Error. [%d]", iStep);
		errMsg2(Task.AutoFlag, sLog);
		iRtnFunction = -iStep;
		break;
	}
	sLog.Empty();
	return iRtnFunction;
}


int CPcbProcess2::RunProc_LensNewPassPickup(int iUseStep)
{
	//! Lens Pickup�� ��ǰ �ѱ� Step	(15000 ~ 16600)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 15000:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens_Z�� ����ġ �̵� ����");
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15000;
			break;
		}

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15000;
			break;
		}

		if (Task.m_bOKLensPass != 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{
			if (!motor.Pcb_Motor_Move(Loading_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], 0))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -15000;
				break;
			}
			iRtnFunction = 15100;
		}
		else
		{
			iRtnFunction = 15500;
		}
		break;
	case 15100:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -15100;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 15150;
		break;

	case 15150:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 15200;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -15150;
		}
		break;

	case 15200: // ����
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(Loading_Pos))
		{
			logStr.Format("Lens %s X,Y �̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -15200;
			break;
		}
		logStr.Format("Lens ���� ��ġ �̵� �Ϸ�[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 15250;
		break;

	case 15250:
		if (!motor.PCB_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB loading Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15250;
		}
		else
		{
			iRtnFunction = 15252;
		}
		break;
	case 15252:
		if (motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			iRtnFunction = 15255;
		}
		else
		{
			logStr.Format("Lens Z ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15252;
		}
		break;

	case 15255:
		iRtnFunction = 15300;
		break;


	case 15300: // ���� �׸�
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 15350;
			Sleep(500);
			Dio.LensMotorGrip(false, false);
			Sleep(500);
			Dio.LensMotorGrip(true, false);
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens �׸� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15300;
		}
		break;

	case 15350://���� �׸� Ȯ��
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens �׸� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 15400;

			Sleep(800);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ���� ���� �ð� �ʰ�..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15350;
		}
		break;
	case 15400:
		Sleep(400);	//�Ǹ��� ������� PCB Stage UnGrip�� ���� ���� ��, Delay
		if (Dio.HolderGrip(false, false))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 15450;
		}
		else
		{
			logStr.Format("Lens Gripper ���� ����..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -15400;
		}
		break;

	case 15450:
		if (Dio.HolderGripCheck(false, false))
		{
			iRtnFunction = 15452;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			if (askMsg("Holder UnGrip Fail \n��� �����Ͻðڽ��ϱ�?") == IDOK)
			{
				iRtnFunction = 15452;
				break;
			}
			logStr.Format("PCB Stage Lens Gripper ���� ����..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -15450;
		}
		Sleep(600);
		break;
	case 15452:

		iRtnFunction = 15455;
		break;

	case 15455:

		iRtnFunction = 15500;
		break;
	case 15500: // ��� ��ġ �̵�		//���� �׸����¿��� �����ϸ� �̸� �´� ����������

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15500;
			break;
		}
		//
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15500;
			break;
		}
		iRtnFunction = 15550;
		break;
	case 15550:
		//if (motor.Pcb_Motor_Move(Wait_Pos))			//PCB�� ���� �����ġ �̵�


		if (motor.Pcb_Motor_MoveY(Wait_Pos))
		{
			iRtnFunction = 15800;
			logStr.Format("Pcb y ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
		}
		else
		{
			logStr.Format("Pcb y ��� ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -15550;
		}
		break;
	case 15800:

		Task.LensTask = 30000;	//pcb AA�϶� lens �� Align �̵��� laser
		iRtnFunction = 15900;

		break;
	case 15900:

		iRtnFunction = 26000;

		//pcb �� 
		//lens�� Align , laser��ġ�� 
		//������ġ�� ������ ������ ��� ��ġ�ΰ��� ����ؾߵȴ�.

		//if (Task.interlockLens == 1)	//PCB AA ���
		//{
		//	Task.interlockLens = 0;	//PCB AA ���
		//	iRtnFunction = 26000;
		//}
		break;
	default:
		logStr.Format("Lens ���� ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int	CPcbProcess2::RunProc_LensAlign(int iUseStep)			//(35000 ~ 39000)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay
	switch (iUseStep)
	{
	case 35000:
		checkMessage();
		offsetX = offsetY = offsetTh = 0.0;
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("		PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35000;
			break;
		}
		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;

		iRtnFunction = 35050;	//2ȣ�� lens align pass 240723
		break;

#if 1
	case 35050:
		if (sysData.m_iAlignCamInspPass == 1)		//xxxxx
		{
			Task.m_iRetry_Opt = 0;
			iRtnFunction = 35400;
			break;
		}
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 35100;
		break;

	case 35100:
		Task.m_bOKLensPass = -1;//PCB��ε� �������� �ѱ���·� ���������Ƿ� �ʱ�ȭ.
		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;


		if (Task.m_bOKLensPass != 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{
			iRtnFunction = 35150;
			break;
		}
		else
		{
			logStr.Format("Lens �ѱ� �Ϸ�� Lens Align Pass![%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 35400;
			break;
		}
		break;
	case 35150:					//IM
		if (motor.Pcb_Motor_Move(Lens_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1498);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Lens_Pos], iUseStep);
			iRtnFunction = 35151;
			break;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Lens_Pos], iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35150;
			break;
		}
		break;
	case 35151:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(LENS_Align_MARK);
		LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);		// Align�� ���� ON
		if (motor.Pcb_Motor_Move(Lens_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Lens_Pos))
			{
				logStr.Format("PCB Lens_Pos Align ��ġ �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);
				iRtnFunction = 35152;
				break;
			}
		}
		else
		{
			logStr.Format("PCB Lens Align ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35150;
			break;
		}
		break;

	case 35152:
		//LensHoleFind
		iRtn = theApp.MainDlg->procCamComAlign(CAM1, LENS_Align_MARK, false, offsetX, offsetY, offsetTh);
		if (iRtn == 1)
		{
			logStr.Format("[�ڵ� Lens Hole Find] ��ũ �ν� ����");
			putListLog(logStr);
			iRtnFunction = 35200;
			break;
		}
		else
		{
			logStr.Format("[�ڵ� Lens Hole Find] ��ũ �ν� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35152;
			break;
		}
		break;
	case 35200:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 35250;
			break;
		}
		break;

	case 35250:

		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Lens Align ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iUseStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[LENS_Align_MARK] = 0;
				Task.d_Align_offset_y[LENS_Align_MARK] = 0;
				Task.d_Align_offset_th[LENS_Align_MARK] = 0;
				iRtnFunction = 35400;
				break;
			}
			else
			{
				Task.m_iRetry_Opt = 0;
				logStr.Format("PCB Lens Align ��˻� ����[%d]", iUseStep);
				putListLog(logStr);
				iRtnFunction = 35250;
				break;

				//iRtnFunction = -35250;
				//logStr.Format("PCB Align ��˻� %d ȸ ����[%d]", Task.m_iRetry_Opt, iUseStep);
				//errMsg2(Task.AutoFlag,logStr);
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM1, LENS_Align_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(LENS_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		offsetTh = 0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[LENS_Align_MARK] -= offsetX;
			Task.d_Align_offset_y[LENS_Align_MARK] -= offsetY;
			Task.d_Align_offset_th[LENS_Align_MARK] += 0.0;//offsetTh;

			Task.dPcbAlignTh = Task.d_Align_offset_th[LENS_Align_MARK];

			logStr.Format("PCB Lens Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 35300;				// ���� �̵�
				break;
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Lens Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("		PCB Lens Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);	//Laser ���� ���� ���� �ð�

				iRtnFunction = 35450;  //35400;		// �Ϸ�
				break;
			}
			else
			{
				logStr.Format("PCB Lens Align [%d] ���� ���� �ʰ�", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -35250;				// NG
				break;
			}
		}
		else
		{
			logStr.Format("PCB Lens Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);

			iRtnFunction = 35200;				//  ��˻� 
			break;
			Sleep(300);
		}
		break;

	case 35300://- Sensor Align Retry
		if (motor.Pcb_Holder_Align_Move(Lens_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 35450;
			break;
		}
		else
		{
			logStr.Format("PCB Lens Align ���� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35300;
			break;
		}
		break;
	case 35400:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING1498);
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Wait_Pos], iUseStep);
			iRtnFunction = 35450;
			break;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1499);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange + _T("[%d]"), MotorPosName[Wait_Pos], iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35400;
			break;
		}
		break;
#endif
	case 35450:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35450;
			break;
		}
		iRtnFunction = 35500;
		break;
	case 35500:
		//LensHoleFind XY �� �̵�
		iRtnFunction = 35510;
		break;
	case 35510:
		//LensHoleFind Z �� �̵�
		iRtnFunction = 35520;
		break;
	case 35520:
		
	case 35530:
		//LensHoleFind
		iRtnFunction = 39000;
		break;
	default:
		logStr.Format("Lens �ܺ� Align Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess2::RunProc_LensLoading(int iUseStep)
{//! Lens Pickup�� ��ǰ �ѱ� Step	(15000 ~ 16600)
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	//Task.m_bOKLensPass = -1;//PCB��ε� �������� �ѱ���·� ���������Ƿ� �ʱ�ȭ.

	switch (iUseStep)
	{
	case 39000:

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens_Z�� ����ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -39000;
			break;
		}
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			delayMsg(logStr, 3000, M_COLOR_RED);
			iRtnFunction = -39000;
			break;
		}
		if (Task.m_bOKLensPass != 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{
			//�������� ƿƮ ���ؼ� ������ġ���� �ٽ� ������ ƿƮ�� tx,ty ������ �̵� 

			if (!motor.Pcb_Tilt_Motor_Move(Loading_Pos))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				putListLog(logStr);
				delayMsg(logStr, 3000, M_COLOR_RED);
				iRtnFunction = -39000;
				break;
			}
			if (!motor.Pcb_Motor_Move(Loading_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				putListLog(logStr);
				delayMsg(logStr, 3000, M_COLOR_RED);
				iRtnFunction = -39000;
				break;
			}
			iRtnFunction = 39100;
			break;
		}
		else
		{
			iRtnFunction = 39500;	//���� �ε� �Ϸ� Jump
		}
		break;

	case 39100:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -39100;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 39150;
		break;

	case 39150:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 39170;
			Task.PCBTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -39150;
		}
		break;
	case 39170:
		iRtnFunction = 39200;
		break;

	case 39200: // ���� 
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -39200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(Loading_Pos))
		{
			logStr.Format("Lens %s X,Y �̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -39200;
			break;
		}

		logStr.Format("Lens ���� ��ġ �̵� �Ϸ�[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 39210;
		break;
	case 39210:
		iRtnFunction = 39250;
		break;
	case 39250:
		if (!motor.PCB_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB loading Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39250;
			break;
		}
		iRtnFunction = 39251;
		break;
	case 39251:
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39251;
			break;
		}
		iRtnFunction = 39252;
		break;
	case 39252:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 39255;
		break;

	case 39255:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 39300;
		break;
	case 39300: // ���� �׸�
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 39350;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens �׸� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39300;
		}
		break;

	case 39350://���� �׸� Ȯ��
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens �׸� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 39400;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ���� ���� �ð� �ʰ�..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39350;
		}
		break;

	case 39400:
		Sleep(500);
		if (Dio.HolderGrip(false, false))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 39455;
		}
		else
		{
			logStr.Format("Lens Gripper ���� ����..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -39400;
		}
		
		break;

	case 39455:
		if (Dio.HolderGripCheck(false, false))
		{
			iRtnFunction = 39500;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			if (askMsg("Holder UnGrip Fail \n��� �����Ͻðڽ��ϱ�?") == IDOK)
			{
				iRtnFunction = 39500;
				break;
			}
			logStr.Format("PCB Stage Lens Gripper ���� ����..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);

			iRtnFunction = -39400;
		}
		Sleep(300);
		
		break;
	case 39500: // ��� ��ġ �̵�
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39500;
			break;
		}
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39500;
			break;
		}
		iRtnFunction = 39550;
		break;
	case 39550:
		iRtnFunction = 39900;
		break;
	case 39900:
		iRtnFunction = 39920;
		break;

	case 39920:	//jump step
		iRtnFunction = 40000;
		break;
	default:
		logStr.Format("Lens ���� ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess2::procProductComplete(int iStep)
{
	CString logStr = "";
	CString sSocketMsg = "";
	CString sBarCode = "";
	//
	int iRtnFunction = iStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;	//PCB ���� ȹ�� Delay
	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	double offsetX = 0.0;
	double offsetY = 0.0;
	double offsetTh = 0.0;
	int iTestPatCnt = 0;
	int iRtn = 0;
	//
	vision.clearOverlay(CCD);
	vision.drawOverlay(CCD);
	theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
	theApp.MainDlg->m_iCurCamNo = 0;
	theApp.MainDlg->changeMainBtnColor(MAIN_DLG);
	//
	switch (iStep)
	{
	case 11000:	// PCB MIU ����	
		iLaser_Pos = 0;
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
		Task.PCBTaskTime = myTimer(true);
		//if (sysData.m_FreeRun == 0)		
		//{
		//	bThread_MIUCheckRun = true;
		//	if (!MIUCheck_process() || gMIUDevice.CurrentState != 4)
		//	{
		//		sLangChange.LoadStringA(IDS_STRING452);	//CCD ��� ���� �ʱ�ȭ ����.[%d]\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.
		//		logStr.Format(sLangChange, iStep);
		//		errMsg2(Task.AutoFlag, logStr);
		//		iRtnFunction = -11000;
		//		break;
		//	}
		//	bThread_MIUCheckRun = false;
		//}
		iRtnFunction = 12000;
		break;
	case 12000:
		//Ȯ���ʿ� norinda
		Task.m_bPBStart = 1;
		//if( Task.m_bPBStart ==  1 )
		//{
		//	func_Socket_Barcode();
		//}
		//else
		//{
		//	//! ���ڵ尡 ���� ������ ��� ��� ����.
		//	Task.iRecvLenACK[0] = Task.iRecvLenACK[1] = 0;
		//	Task.dTiltingManual[0] = Task.dTiltingManual[1] = 0.0;
		//	Task.dAlignManual[0] = Task.dAlignManual[0] = Task.dAlignManual[0] = 0.0;
		//}
		//iRtnFunction = 12900;			//laser ������ ����
		//break;
		
		
		if (sysData.m_iAlignCamInspPass == 1)		//procProductComplete
		{
			logStr.Format("����ǰ Align Pass[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12800;
			break;
		}

		iRtnFunction = 12100;			//laser ������ ����
		break;
		
		
		break;
	case 12100:
		
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 12110;
		break;
	case 12110:

		Task.m_iRetry_Opt = 0;
		//SetDigReference(PCB_Holder_MARK);
		LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align�� ���� ON

		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;

		if (motor.Pcb_Motor_Move(CompleteAlign_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb ����ǰ Align ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12200;
		}
		else
		{
			logStr.Format("Pcb ����ǰ Align ��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12100;
		}
		break;
	case 12200:
		if (motor.PCB_Z_Motor_Move(CompleteAlign_Pos))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("Pcb ����ǰ Align Z��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12400;
		}
		else
		{
			logStr.Format("Pcb ����ǰ Align Z��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12200;
		}
		break;

	case 12400:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 12500;
		}
		break;
	case 12500:

		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB ����ǰ Align ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 12800;
			}
			else
			{
				iRtnFunction = -12500;
				logStr.Format("PCB Align ��˻� %d ȸ ����[%d]", Task.m_iRetry_Opt, iStep);
				errMsg2(Task.AutoFlag, logStr);
			}
			break;

		}
		//-----------------------------------
		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamComAlign(CAM1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);

		offsetTh = 0.0;
		saveInspImage(PCB_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;
		if (iRtn == 1)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] += offsetTh;

			logStr.Format("PCB ����ǰ Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();
			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh); // PCB����� �����̴� �κ��̶� Limit Check �Ҷ� CAM1��

			if (iRtnVal == 1)
			{
				iRtnFunction = 12700;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				iLaser_Pos = 0;
				logStr.Format("		PCB ����ǰ Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				logStr.Format("		PCB ����ǰ Align Complete :  %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				Task.dSocketRecvTime = myTimer(true);
				iRtnFunction = 12800;

			}
			else
			{
				logStr.Format("PCB ����ǰ Align [%d] ���� ���� �ʰ�[%d]", Task.m_iRetry_Opt, iStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -12500;
			}
		}
		else
		{
			logStr.Format("PCB ����ǰ Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iStep);
			putListLog(logStr);
			iRtnFunction = 12500;				//  ��˻� 
		}
		break;

	case 12700://!! ������ �̵�.- Retry
		if (motor.Pcb_Holder_Align_Move(CompleteAlign_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 12200;
		}
		else
		{
			logStr.Format("Pcb ����ǰ Align ���� ��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12700;
		}
		break;
	case 12800:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -12800;
			break;
		}

		iRtnFunction = 12850;


		break;
	case 12850:
		iLaser_Pos = 0;
		vision.clearOverlay(CAM1);
		//vision.drawOverlay(CAM1);
		//�ܺ� ���������� Read
#if (____AA_WAY == LENS_TILT_AA)//PCB_TILT_AA) 
		if (g_ADOData.func_Get_RecordData(Task.ChipID))//DB�� Data�� ���� ���
		{

			Task.dAAPcbMotor[0] = g_ADOData.adoRegist.dPCBMotor[0];   //PCB X
			Task.dAAPcbMotor[1] = g_ADOData.adoRegist.dPCBMotor[1];   //PCB Y
			Task.dAAPcbMotor[2] = g_ADOData.adoRegist.dPCBMotor[2];   //PCB TH
																	  //TX[3] , TY[4]
			Task.dAAPcbMotor[3] = g_ADOData.adoRegist.dPCBMotor[3];   //PCB TX 
			Task.dAAPcbMotor[4] = g_ADOData.adoRegist.dPCBMotor[4];   //PCB TY 
																	  //
			Task.dAAPcbMotor[5] = g_ADOData.adoRegist.dPCBMotor[5];   //PCB Z
			Task.dAlignManual[0] = g_ADOData.adoRegist.dOffset_Align[0];  //AlignOffsetX
			Task.dAlignManual[1] = g_ADOData.adoRegist.dOffset_Align[1];  //AlignOffsetY
			Task.dAlignManual[2] = g_ADOData.adoRegist.dOffset_Align[2];  //AlignOffsetT
											
			logStr.Format("PCB ȭ��˻� Chart Pos X(%.3lf), Y(%.3lf)", Task.dAAPcbMotor[0], Task.dAAPcbMotor[1]);
			putListLog(logStr);
			//
			logStr.Format("PCB ȭ��˻� TX(%.3lf), TY(%.3lf)", Task.dAAPcbMotor[3], Task.dAAPcbMotor[4]);
			putListLog(logStr);

			if (fabs(Task.dAAPcbMotor[0] - model.axis[Motor_PCB_X].pos[Bonding_Pos]) > 5.0)
			{
				logStr.Format("Pcb ȭ��˻� X spec Over[%d]", iStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12850;
				break;
			}
			if (fabs(Task.dAAPcbMotor[1] - model.axis[Motor_PCB_Y].pos[Bonding_Pos]) > 5.0)
			{
				logStr.Format("Pcb ȭ��˻� Y spec Over[%d]", iStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12850;
				break;
			}
			iRtnFunction = 12900;
			break;
		}
		else
		{
			Task.dAAPcbMotor[0] = model.axis[Motor_PCB_X].pos[Bonding_Pos];   //PCB X
			Task.dAAPcbMotor[1] = model.axis[Motor_PCB_Y].pos[Bonding_Pos];   //PCB Y
			Task.dAAPcbMotor[2] = model.axis[Motor_PCB_TH].pos[Bonding_Pos];   //PCB TH
																			   //TX[3] , TY[4]
			Task.dAAPcbMotor[3] = model.axis[Motor_PCB_Xt].pos[Bonding_Pos];   //PCB TX 	model.m_InspPcbTilt[0];//
			Task.dAAPcbMotor[4] = model.axis[Motor_PCB_Yt].pos[Bonding_Pos];   //PCB TY 	model.m_InspPcbTilt[1];//

																			   //
			Task.dAAPcbMotor[5] = model.axis[Motor_PCB_Z].pos[Bonding_Pos];   //PCB Z

			iRtnFunction = 12900;

			//iRtnFunction = 12900;
			//logStr.Format("��ġ �� �����ϴ�.\ bonding ��ġ�� ���� �Ͻðڽ��ϱ�?");	//��ü ���� ���͸� ���� �Ͻðڽ��ϱ�?
			//if (askMsg(logStr))
			//{

			//	
			//	break;
			//}
			/*logStr.Format("��ġ �� �����ϴ�.[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12850;*/
			//��ġ �� ���� ��� ���� ƼĪ�� â����.
			/*Task.m_iRtnTiltingDlg = 2;
			TiltingManualdlg->ShowWindow(SW_SHOW);
			iRtnFunction = 12870;*/
		}


#else //#elif (____AA_WAY == LENS_TILT_AA)
		Task.dTiltingManual[0] = 0.0;
		Task.dTiltingManual[1] = 0.0;
		iRtnFunction = 12900;
		/*
		Task.dTiltingManual[0] = g_ADOData.adoRegist.dOffset_TiltTx;
		Task.dTiltingManual[1] = g_ADOData.adoRegist.dOffset_TiltTy;

		Task.dAlignManual[0] = g_ADOData.adoRegist.dOffset_Align[0];
		Task.dAlignManual[1] = g_ADOData.adoRegist.dOffset_Align[1];
		Task.dAlignManual[2] = g_ADOData.adoRegist.dOffset_Align[2];
		*/

		break;
#endif
		break;
	case 12870:
		if (Task.m_iRtnTiltingDlg == 0)
		{
			logStr.Format("�������� Data �Է� �� �ڵ���� ���� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12870;
			break;
		}
		else if (Task.m_iRtnTiltingDlg == 1)
		{
			logStr.Format("�ܺ� ���� Tilting �Ϸ� ��ġ : Tx=%.03lf, Ty=%.03lf", Task.dTiltingManual[0], Task.dTiltingManual[1]);
			putListLog(logStr);

			Task.m_timeChecker.Measure_Time(4);	//Laser ���� ���� ���� �ð�
												//iRtnFunction = 12900;//pcb aa�� ������ �ϵ��� 200708

			iRtnFunction = 12900;

		}
		break;
	case 12900:
		if (sysData.m_iLaserInspPass == 1)		//����ǰ
		{
			logStr.Format("����ǰ Laser pass");
			putListLog(logStr);
			iRtnFunction = 13300;		//����ǰ�� ������ �н�
			break;
		}

		if (!motor.Pcb_Tilt_Motor_Move(Com_Laser_Pos))
		{
			logStr.Format("PCB tx,ty Com_Laser_Pos �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -12900;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 12901;
		break;

	case 12901:
			////if(motor.Pcb_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]))

		if (motor.Pcb_Motor_Move_ComLaser(iLaser_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			if (motor.PCB_Z_Motor_Move(Com_Laser_Pos))
			{
				Task.PCBTaskTime = myTimer(true);
				logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iStep);
				putListLog(logStr);
				iRtnFunction = 12910;// 12950;
			}
			else
			{
				logStr.Format("PCB Com_Laser_Pos Z �̵� ����[%d]", iStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -12900;
				break;
			}
		}
		else
		{
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12900;
		}

		break;
	case 12910:

		iRtnFunction = 12920;
		break;

	case 12920:
		iRtnFunction = 12950;
		break;
		//laser�Ǹ��� �ϰ� END
	case 12950:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 13100;
		}
		break;
	case 13100:
		Keyence.func_LT9030_Scan(Task.m_Laser_Point[iLaser_Pos]);  //func_LT9030_Scan

		MandoInspLog.dLaserTestComPoint[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];

		//Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("���� ���� %lf[%d] - �ܺ� - %d ��ġ", Task.m_Laser_Point[iLaser_Pos], iStep, iLaser_Pos + 1);
		putListLog(logStr);
		LaserPos[iLaser_Pos].x = motor.GetEncoderPos(Motor_PCB_X);
		LaserPos[iLaser_Pos].y = motor.GetEncoderPos(Motor_PCB_Y);
		LaserValue[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];

		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				if (theApp.MainDlg->func_Check_LaserValueErr(LaserValue) == false)
				{
					logStr.Format("Laser ������ �̻�..�ܺ� ���� �������� �̻�..[%d]\n ������:%.04lf, %.04lf, %.04lf, %.04lf ", iStep, LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3]);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -13100;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]))
				{
					/*	logStr.Format("		������ Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0] );
					putListLog(logStr);
					logStr.Format("		���� ������ ���� Tx: %.04lf, Ty: %.04lf", Task.dTiltingManual[0], Task.dTiltingManual[1] );
					putListLog(logStr);
					int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0], Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] );
					*/


					//Task.m_dataOffset_x[0] =  Task.d_Align_offset_xt[PCB_Holder_MARK];
					//Task.m_dataOffset_y[0] =  Task.d_Align_offset_yt[PCB_Holder_MARK];
					logStr.Format("		������ Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]);
					putListLog(logStr);
					logStr.Format("		�ܺ� ������ ���� Tx: %.04lf, Ty: %.04lf", Task.dTiltingManual[0], Task.dTiltingManual[1]);
					putListLog(logStr);


					//int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[PCB_Holder_MARK],Task.d_Align_offset_yt[PCB_Holder_MARK]);
					//int Rnd = TiltAlignLimitCheck(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0], Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] );


					int Rnd = 2; //theApp.MainDlg->TiltAlignLimitCheck(Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]); //250207LIM
					if (Rnd == 2)
					{
						Task.m_timeChecker.Measure_Time(5);	//Laser ���� ���� �Ϸ� �ð�
						Task.m_dTime_LaserDpm = Task.m_timeChecker.m_adTime[5] - Task.m_timeChecker.m_adTime[4];
						//dispGrid();
						iRtnFunction = 13300;

					}
					else if (Rnd == 1)
					{
						iRtnFunction = 13200;	//������ �̵�		
					}
					else
					{
						logStr.Format("Laser Tilt  ������ Limit�� �ʰ� �Ͽ����ϴ�.[%d]", iStep);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -13100;
					}
				}
			}
			else
			{
				iRtnFunction = 12900;
				iLaser_Pos++;
			}
		}
		else
		{
			iRtnFunction = 13100;
		}
		break;
	case 13200:
		//if(motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[0] + Task.dTiltingManual[0],Task.d_Align_offset_yt[0] + Task.dTiltingManual[1] ))


		if (motor.Pcb_Move_Tilt(Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]))
		{
			iLaser_Pos = 0;
			iRtnFunction = 12900;
		}
		break;

	case 13300:
		iRtnFunction = 13310;
		break;
	case 13310:
		iRtnFunction = 13350;
		break;
	case 13350:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 14000;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� ����[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = -13350;
		}
		break;
	case 14000:

		iRtnFunction = 14100;
		break;
	case 14100:
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 14200;
		break;

	case 14200:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 14300;
			Task.m_iRetry_Opt = 0;
		}
		break;
	case 14300:

		iRtnFunction = 14400;				// �Ϸ�
		break;
	case 14400:
		iRtnFunction = 14500;
		break;
	case 14500:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 15200;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� ����[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = -14500;
		}
		break;
	case 15200:
		LightControl.ChartAllControl(true);
		iRtnFunction = 19600;
		break;

	case 19600: // ���� ��ġ �̵� 
		

		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			logStr.Format("PCB Bonding ���� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 19700;
		}
		else
		{
			logStr.Format("PCB Bonding ��ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19600;
		}
		break;
	case 19700:
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 19800;
#elif (____AA_WAY == LENS_TILT_AA)
		if (motor.Pcb_Move_Tilt(Task.dAAPcbMotor[3], Task.dAAPcbMotor[4]))
		{
			
			iRtnFunction = 19800;
		}
		else
		{
			logStr.Format(_T("PCB Tilt Bonding Pos Move Fail[%d]"), iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19700;
		}
		
#endif
		break;
	case 19800: //z�� ���� ��ġ �̵�
		if (!motor.PCB_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format(_T("PCB Z Bonding Pos Move Fail[%d]"), iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -19800;
			break;
		}
		Task.PCBTaskTime = myTimer(true);

		iRtnFunction = 19900;
		break;
	case 19900:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			

			//if( func_MIU_ConnectLiveCheck() == false)		//MIU ���� Check
			//{
			//	iRtnFunction = -19900;
			//	break;
			//}
			theApp.MainDlg->setCamDisplay(3, 1);

			iRtnFunction = 120000;//����ǰ �˻�
		}
		break;

	default:
		logStr.Format("PCB ����ǰ �˻� Step �̻� [%d]", iStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -20000;
		break;
	}
	return iRtnFunction;
}
int CPcbProcess2::RunProc_LENS_AlignLaserMeasure(int iLensStep)
{
	int iRtnFunction = iLensStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay

	switch (iLensStep)
	{
	case 30000:
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;
		//
		Task.m_iRetry_Opt = 0;

		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 30050;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30000;
		}

		break;

	case 30050:
		if (motor.Lens_Motor_MoveY(Wait_Pos))
		{
			logStr.Format("Lens Y �����ġ �̵� ���[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 30100;		//������ ī�޶� ����
		}
		else
		{
			logStr.Format("Lens Y �����ġ �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -30050;
		}
		break;
	case 30100:
		if (motor.Lens_Motor_MoveXY(Wait_Pos))
		{
#if (____AA_WAY == PCB_TILT_AA)
			//iRtnFunction = 30500;
			iRtnFunction = 43000;
			LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);

#elif (____AA_WAY == LENS_TILT_AA)

			iRtnFunction = 43000;		//������ ī�޶� ����
			break;

#endif
		}
		else
		{
			logStr.Format("Lens �����ġ �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -30050;
		}
		break;

	case 30500:
		if (Dio.CamLaserSlinderMove(true, true))
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			Sleep(500);
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 30600;

		}
		else
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30500;
		}

		break;
	case 30600:
		if (Dio.CamLaserSlinderCheck(true, false))
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� Ȯ�� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 30900;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 13000)
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� �ð� �ʰ�[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30600;
		}
		break;
	case 30900:
		if (sysData.m_iLaserInspPass == 1)
		{
			iRtnFunction = 41100;//Laser ���� Pass
		}
		else
		{
			iRtnFunction = 31250;/////// 31000;
		}

		break;
#if 0 //ī�޶� ����
	case 31000:
		if (motor.Lens_Motor_MoveXY(Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Lens Align ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 31050;
		}
		else
		{
			logStr.Format("Lens Align ��ġ �̵� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31000;
		}
		break;

	case 31050:
		//tx,ty�� �̵��ǰ� ���� 221006
		if (!motor.Lens_Tilt_Motor_Move(Lens_Pos))
		{
			logStr.Format("LensAlign_Pos Tx,Ty�̵� ����");
			putListLog(logStr);
			delayMsg(logStr, 3000, M_COLOR_RED);
			iRtnFunction = -31050;
			break;
		}
		else
		{
			logStr.Format("LensAlign_Pos Tx,Ty ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 31060;
		}
		
		break;
	case 31060:
		if (motor.LENS_Z_Motor_Move(Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Lens Align Z ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 31080;
		}
		else
		{
			logStr.Format("Lens Align Z ��ġ �̵� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31060;
		}
		break;

	case 31080:
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y)))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 31100;
		}
		break;

	case 31100: // lens Align
		if (Task.m_iRetry_Opt > iAlignRetry)
		{
			logStr.Format("Lens Align ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iLensStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[LENS_Align_MARK] = 0;
				Task.d_Align_offset_y[LENS_Align_MARK] = 0;
				Task.d_Align_offset_th[LENS_Align_MARK] = 0;
				iRtnFunction = 31300;
			}
			else
			{
				iRtnFunction = -31100;
				logStr.Format("Lens Align ��˻� %d ȸ ����[%d]", Task.m_iRetry_Opt, iLensStep);
				errMsg2(Task.AutoFlag, logStr);
			}
			break;
		}

		offsetX = offsetY = offsetTh = 0.0;
		Sleep(200);
		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, LENS_Align_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(LENS_IMAGE_SAVE, Task.m_iRetry_Opt);

		Task.m_iRetry_Opt++;
		offsetY = 0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[LENS_Align_MARK] -= offsetX;
			Task.d_Align_offset_y[LENS_Align_MARK] += offsetY;
			Task.d_Align_offset_th[LENS_Align_MARK] = 0;//+= offsetTh;	//Lens�� ������ Theta�� ����

			logStr.Format("Lens Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2, offsetX, offsetY, 0);
			if (iRtnVal == 1)
			{
				iRtnFunction = 31200;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		Lens Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iLensStep);
				putListLog(logStr);
				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("		Lens Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iLensPickupNoCentering, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 31250;// 31300;				// �Ϸ�
			}
			else
			{
				logStr.Format("Lens Align [%d] ���� ���� �ʰ�[%d]", Task.m_iRetry_Opt, iLensStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);
				iRtnFunction = -31100;				// NG
			}
		}
		else
		{
			logStr.Format("Lens Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iLensStep);
			putListLog(logStr);
			iRtnFunction = 31100;					//  ��˻� 
		}
		break;

	case 31200://!! ������ �̵�.- Retry
		if (motor.Lens_Motor_Align_Move(Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 31080;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING657);	//Lens Align ���� ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31200;
		}
		break;

#endif
	case 31250:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 31300;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31250;
		}
		break;
	case 31300://! Lens-Z�� ��� ��ġ �̵�
		iLaser_Pos = 0;
		//iRtnFunction = 39000; //���� ī�޶� ������
		iRtnFunction = 43000; //���� ī�޶� ������
		break;
	case 39000:
		if (!motor.Lens_Tilt_Motor_Move(Laser_Lens_Pos))
		{
			logStr.Format("Laser_Lens_Pos Tx,Ty�̵� ����");
			putListLog(logStr);
			delayMsg(logStr, 3000, M_COLOR_RED);
			iRtnFunction = -39000;
			break;
		}
		else
		{
			logStr.Format("Laser_Lens_Pos Tx,Ty ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 40000;
		}
		break;
	case 40000://! Laser-Z�� �ܺ� ������ġ �̵�
		if (motor.LENS_Z_Motor_Move(Laser_Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 40100;
		}
		else
		{
			logStr.Format("Laser-Z�� ���� ���� �ܺ� ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40000;
		}
		break;

	case 40100:
		iRtnFunction = 40200;
		break;

	case 40200://! Lens Stage �ܺ� Laser ������ġ �̵� 
		if (motor.Lens_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iLensStep);	//Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]
			putListLog(logStr);
			iRtnFunction = 40210;
		}
		else
		{
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40200;
		}
		break;

	case 40210:
		iRtnFunction = 40220;
		break;

	case 40220:
		Task.LensTaskTime = myTimer(true);
		iRtnFunction = 40300;
		break;

	case 40300://! ���� �� Delay
		if ((myTimer(true) - Task.LensTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y)))	//�����Ӵ�, Delay Ȯ��
		{
			iRtnFunction = 41000;
		}
		break;
		//Laser �Ǹ��� �ϰ� END

	case 41000: //�ܺ� ���� ���� (���� ���� �ϰ� Align �̵�)
		Keyence.func_LT9030_Scan(Task.m_Laser_Point[iLaser_Pos]);
		//Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		logStr.Format("���� ���� %lf[%d] - �ܺ� - %d ��ġ", Task.m_Laser_Point[iLaser_Pos], iLensStep, iLaser_Pos + 1);
		putListLog(logStr);
		LaserPos[iLaser_Pos].x = motor.GetEncoderPos(Motor_Lens_X) - sysData.dDataset[Motor_Lens_X];
		LaserPos[iLaser_Pos].y = motor.GetEncoderPos(Motor_Lens_Y) - sysData.dDataset[Motor_Lens_Y];
		LaserValue[iLaser_Pos] = Task.m_Laser_Point[iLaser_Pos];
		if (Task.m_Laser_Point[iLaser_Pos] != 0)
		{
			if (iLaser_Pos == 3)
			{
				if (theApp.MainDlg->func_Check_LaserValueErr(LaserValue) == false)
				{
					logStr.Format("Laser ������ �̻�..�ܺ� ���� �������� �̻�..[%d]\n ������:%.04lf, %.04lf, %.04lf, %.04lf", iLensStep, LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3]);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -41000;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[LENS_Align_MARK], Task.d_Align_offset_yt[LENS_Align_MARK]))
				{
					Task.m_dataOffset_x[LENS_Align_MARK] = Task.d_Align_offset_xt[LENS_Align_MARK];
					Task.m_dataOffset_y[LENS_Align_MARK] = Task.d_Align_offset_yt[LENS_Align_MARK];


					logStr.Format("������ Tx: %.04lf, Ty: %.04lf", Task.d_Align_offset_xt[LENS_Align_MARK], Task.d_Align_offset_yt[LENS_Align_MARK]);
					putListLog(logStr);

					int Rnd = theApp.MainDlg->TiltAlignLimitCheck(Task.m_dataOffset_x[LENS_Align_MARK], Task.m_dataOffset_y[LENS_Align_MARK]);
					//20160117 ������ ����

					if (Rnd == 2)
					{

						Task.m_timeChecker.Measure_Time(5);	//Laser ���� ���� �Ϸ� �ð�
						Task.m_dTime_LaserDpm = Task.m_timeChecker.m_adTime[5] - Task.m_timeChecker.m_adTime[4];
						//dispGrid();
						MandoInspLog.dTilteOffset[0] = Task.m_dataOffset_x[LENS_Align_MARK];	//���� ������Camera �˻� Log ����
						MandoInspLog.dTilteOffset[1] = Task.m_dataOffset_y[LENS_Align_MARK];


						//20141217 LHC - DataBase�� 0���� ����Ǵ� ��찡 ���� 0���� �� ��� ���������ʰ� �˶�ġ����.
						//�迵ȣ 20150602 FraneeGrabber  ���� �ӽ� Pass
						if (Task.m_dataOffset_x[LENS_Align_MARK] == 0 || Task.m_dataOffset_y[LENS_Align_MARK] == 0)
						{
							logStr.Format("Laser ���� �� �� �̻� �߻�..Xt : %lf, Yt : %lf", Task.m_dataOffset_x[LENS_Align_MARK], Task.m_dataOffset_y[LENS_Align_MARK]);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -41000;
							break;
						}
						if (!g_ADOData.func_AATaskToRecordLaser(Task.ChipID, Task.m_dataOffset_x[LENS_Align_MARK], Task.m_dataOffset_y[LENS_Align_MARK], Task.m_Laser_Point))
						{
							logStr.Format("DataBase Laser ���� ���� Data ��� ����.[%d]\n MS Office�� �ݾ��ּ���.", iLensStep);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -41000;
							break;
						}

						LightControl.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);		// Align�� ���� ON
						Task.LensTaskTime = myTimer(true);
						iRtnFunction = 41100;
						iLaser_Pos++;

					}
					else if (Rnd == 1)
					{
						iRtnFunction = 41050;
					}
					else
					{
						logStr.Format("Laser Tilt  ������ Limit�� �ʰ� �Ͽ����ϴ�.[%d]", iLensStep);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -41000;
					}
				}
			}
			else
			{
				iRtnFunction = 40200;
				iLaser_Pos++;
			}
		}
		else
		{
			iRtnFunction = 41000;
		}
		break;

	case 41050: // �̵�  �������� 0���� ����� ���� �̵�
		if (motor.Lens_Move_Tilt(Task.d_Align_offset_xt[LENS_Align_MARK], Task.d_Align_offset_yt[LENS_Align_MARK]))
		{
			iRtnFunction = 40200;
			iLaser_Pos = 0;//4;

		}
		break;

	case 41100:
		iRtnFunction = 41110;
		break;

		//laser�Ǹ��� ���
	case 41110:
		iRtnFunction = 41120;
		break;

	case 41120:
		iRtnFunction = 41200;
		break;

	case 41200://! Lens-Z�� ��� ��ġ �̵�
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 41850;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
		break;

	case 41300:
		if (motor.Lens_Motor_MoveXY(Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Lens Align �̵� [%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 41400;
		}
		else
		{
			logStr.Format("Lens Align �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41300;
		}

		break;
	case 41400:

		if (motor.LENS_Z_Motor_Move(Lens_Pos))
		{
			Task.d_Align_offset_x[LENS_Align_MARK] = 0;
			Task.d_Align_offset_y[LENS_Align_MARK] = 0;
			Task.d_Align_offset_th[LENS_Align_MARK] = 0;
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Lens Align ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 41500;
		}
		else
		{
			logStr.Format("Lens Align ��ġ �̵� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41400;
		}
		break;

	case 41500:
		checkMessage();
		if ((myTimer(true) - Task.LensTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y)))
		{
			iRtnFunction = 41600;
			Task.m_iRetry_Opt = 0;
		}
		break;

	case 41600://! LENS Align(�������� ������, Shift ��� �뵵)
		if (sysData.m_FreeRun == 1)
		{
			for (int i = 0; i < 100; i++)
			{
				Sleep(10);
				checkMessage();
			}

			iRtnFunction = 41700;
			Task.d_Align_offset_x[LENS_Align_MARK] = 0;
			Task.d_Align_offset_y[LENS_Align_MARK] = 0;
			Task.d_Align_offset_th[LENS_Align_MARK] = 0;
			break;
		}
		if (Task.m_iRetry_Opt > iAlignRetry)
		{
			logStr.Format("Lens Align ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iLensStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[LENS_Align_MARK] = 0;
				Task.d_Align_offset_y[LENS_Align_MARK] = 0;
				Task.d_Align_offset_th[LENS_Align_MARK] = 0;
				iRtnFunction = 41800;
			}
			else
			{
				iRtnFunction = -41600;
				logStr.Format("Lens Align ��˻� %d ȸ ����[%d]", Task.m_iRetry_Opt, iLensStep);
				errMsg2(Task.AutoFlag, logStr);
			}

			break;
		}
		offsetX = offsetY = offsetTh = 0.0;
		Sleep(200);
		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, LENS_Align_MARK, false, offsetX, offsetY, offsetTh);

		saveInspImage(LENS_IMAGE_SAVE, Task.m_iRetry_Opt);

		Task.m_iRetry_Opt++;

		if (iRtn == 0)
		{
			Task.d_Align_offset_x[LENS_Align_MARK] -= offsetX;
			Task.d_Align_offset_y[LENS_Align_MARK] += offsetY;
			Task.d_Align_offset_th[LENS_Align_MARK] = 0;//+= offsetTh;	//Lens�� ������ Theta�� ����

			logStr.Format("Lens Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
			//			Sleep(500);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2, offsetX, offsetY, 0);
			if (iRtnVal == 1)
			{
				iRtnFunction = 41700;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("Lens Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iLensStep);
				putListLog(logStr);

				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("Lens Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iLensPickupNoCentering, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 41800;				// �Ϸ�
			}
			else
			{
				logStr.Format("Lens Align [%d] ���� ���� �ʰ� [%d]", Task.m_iRetry_Opt, iLensStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);
				iRtnFunction = -41600;				// NG
			}
		}
		else
		{
			logStr.Format("Lens Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iLensStep);
			putListLog(logStr);

			iRtnFunction = 41600;					//  ��˻� 
		}
		break;

	case 41700://!! ������ �̵�.- Retry
		if (motor.Lens_Motor_Align_Move(Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 41500;
		}
		else
		{
			logStr.Format("Lens Align ���� ��ġ �̵� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41700;
		}
	case 41800: // �� ����2

				//!!!!! 1�� Align ������ ���� ���� ����(PCB<-> Lens Sensor��)
		Task.m_dataOffset_x[1] = Task.d_Align_offset_x[LENS_Align_MARK];
		Task.m_dataOffset_y[1] = Task.d_Align_offset_y[LENS_Align_MARK];
		//		Task.m_dataOffset_th[1] = Task.d_Align_offset_th[LENS_Align_MARK] - Task.d_Align_offset_th[PCB_Chip_MARK];

		//sLangChange.LoadStringA(IDS_STRING962);	//PCB Sensor_Holder�� Align ������ : X:%.03f, Y:%.03f, ��:%.04f [%d]
		//logStr.Format(_T("		") + sLangChange, Task.m_dataOffset_x[1], Task.m_dataOffset_y[1],Task.m_dataOffset_th[1], iLensStep);
		//putListLog(logStr);

		//���� ������Camera �˻� Log ����
		MandoInspLog.dLensOffset[0] = Task.m_dataOffset_x[1];
		MandoInspLog.dLensOffset[1] = Task.m_dataOffset_y[1];
		MandoInspLog.dLensOffset[2] = Task.m_dataOffset_th[1];


		if (Dio.CamLaserSlinderMove(false, true))
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iLensStep);
			//putListLog(logStr);
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 41850;
		}
		else
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41800;
		}

		break;
	case 41850:
		if (Dio.CamLaserSlinderCheck(false, false))
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� Ȯ�� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 41900;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)//IO_DELAY_TIME)
		{
			logStr.Format("Laser/Cam �Ǹ��� ���� �ð� �ʰ�[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41850;
		}
		break;
	case 41900://! Lens-Z�� ��� ��ġ �̵�
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			
			iRtnFunction = 41950;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41900;
		}
		break;
	case 41950:
		iRtnFunction = 42000;


		break;
	case 42000: // ���� ��ġ �̵�
		if (motor.Lens_Motor_MoveXY(Bonding_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK]))
		{
			logStr.Format("Lens Bonding_Pos �̵� [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 42200;

			Task.LensTaskTime = myTimer(true);
		}
		else
		{
			logStr.Format("Lens Bonding_Pos �̵� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -42000;
		}
		break;
	case 42200:
		iRtnFunction = 43000;
		break;

	case 43000:
		Task.interlockPcb = 1;			//end RunProc_LENS_AlignLaserMeasure
		Task.interlockLens = 1;		//end RunProc_LENS_AlignLaserMeasure
		iRtnFunction = 50000;
		break;
	default:
		logStr.Format("Laser ���� ���� Thread Step ��ȣ ������ .[%d]", iLensStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
}

	return iRtnFunction;
}
int CPcbProcess2::RunProc_LENS_LensLoad(int iLensStep)
{
	int iRtnFunction = iLensStep;
	int iCamDelay = model.strInfo_Cam[1].m_iDelayTime;		//PCB ���� ȹ�� Delay
	int iAlignRetry = model.strInfo_Cam[1].m_iRetryCnt;
	int iRtn = 0;
	CString logStr = "";
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	int iLaserDelay = model.strInfo_Cam[0].m_iDelayTime;	//Laser ������ Delay

	switch (iLensStep)
	{
	case 30000:
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 50000;
		break;
#endif
		Task.interlockLens = 0;			//Lens AA ���
		if (Task.m_bOKLensPass == 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{
			logStr.Format("Lens �ѱ� �Ϸ� ���� [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 39500;
			break;
		}
		iRtnFunction = 30500;
		break;
	case 30500:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 30700;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30500;
		}

		break;
	case 30700:

		iRtnFunction = 30800;
		break;
	case 30800:

		iRtnFunction = 31000;
		break;
	case 31000:
		if (motor.Lens_Motor_MoveXY(Wait_Pos))
		{
			iRtnFunction = 31500;		//������ ī�޶� ����
		}
		else
		{
			logStr.Format("Lens �����ġ �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -31000;
		}
		break;
	case 31500:
		if (motor.Lens_Motor_MoveY(Loading_Pos))
		{
			iRtnFunction = 32000;		//������ ī�޶� ����
		}
		else
		{
			logStr.Format("Lens LensLoad �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -31500;
		}
		break;
	case 32000:
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -32000;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(Loading_Pos))
		{
			logStr.Format("Lens %s X,Y �̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -32000;
			break;
		}

		logStr.Format("Lens ���� ��ġ �̵� �Ϸ�[%d]", iLensStep);
		putListLog(logStr);
		iRtnFunction = 32500;
		break;
	case 32500:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -32500;
			break;
		}
		Task.LensTaskTime = myTimer(true);
		iRtnFunction = 33000;
		break;
	case 33000:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 33500;
			Task.LensTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.LensTaskTime) > IO_DELAY_TIME)
		{
			iRtnFunction = -32500;
		}
		break;
	case 33500:
		iRtnFunction = 34000;
		break;
	case 34000:
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -34000;
			break;
		}
		Sleep(300);
		iRtnFunction = 34500;
		break;
	case 34500:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 35000;
		break;
	case 35000:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 35500;
		break;
	case 35500:
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 36000;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens �׸� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35500;
		}
		break;
	case 36000:
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens �׸� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 36200;
		}
		else if ((myTimer(true) - Task.LensTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ���� ���� �ð� �ʰ�..[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35500;
		}
		break;
	case 36200:

		iRtnFunction = 36300;
		break;
	case 36300:
		
		iRtnFunction = 36400;
		break;
	case 36400:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -36400;
			break;
		}
		iRtnFunction = 36600;
		break;
	case 36600:
		iRtnFunction = 36800;
		break;
	case 36800:
		if (!motor.Lens_Motor_MoveX(Wait_Pos))
		{
			logStr.Format("Lens X �����ġ �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -36800;
			break;
		}
		iRtnFunction = 37000;
		break;
	case 37000:
		iRtnFunction = 37200;
		break;
	case 37200:
		iRtnFunction = 37400;
		break;
	case 37400:
		iRtnFunction = 39500;
		break;
	case 39500:
		Task.interlockLens = 1;		//Lens AA ��� 2ȣ��, Lens Thread , ���� �ε� �Ϸ� �κ�
		iRtnFunction = 50000;
		break;
	default:
		logStr.Format("Lens Load Thread Step ��ȣ ������ [%d]", iLensStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}


int	CPcbProcess2::HolderRun_Loading(int iUseStep)			//��ǰ ����		10000 ~ 19000
{
	int iRtnFunction = iUseStep;
	CString logStr = "";

	switch (iUseStep)
	{
	case 10000:
		iRtnFunction = 10100;
		break;

	case 10100:
		logStr.Format("		Start ��ư�� �����ּ���[%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		theApp.MainDlg->m_btnStart.m_iStateBtn = 4;
		theApp.MainDlg->m_btnStart.Invalidate();

		iRtnFunction = 10110;
		break;
	case 10110://! Start P/B Button Push ���. 
		if (Dio.StartPBOnCheck(true, false))// || Start_Btn_On)	//���α׷��� start ��ư �߰�
		{

			
			MbufClear(vision.MilOptImage, 0);

			theApp.MainDlg->m_btnStart.m_iStateBtn = 0;
			theApp.MainDlg->m_btnStart.Invalidate();
			theApp.MainDlg->Start_Btn_On = false;

			if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
			{
				if (!askMsg("���� �ѱ�����Դϴ�. \n��� �����Ͻðڽ��ϱ�?") == IDOK)
				{
					iRtnFunction = 10000;
					break;
				}
			}
			logStr.Format("START BUTTON PRESS [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);

			iRtnFunction = 10165;
			break;
		}
		break;
	case 10165://! Start P/B Button Push ���. 
		Dio.StartPBLampOn(false);
		Dio.PCBvaccumOn(VACCUM_ON, false);      //����

		logStr.Format("==== Holder Run start====");
		theApp.MainDlg->putListLog(logStr);


		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD);
		iRtnFunction = 10170;
		break;


	case 10170:
		if ((myTimer(true) - Task.PCBTaskTime) > 300)
		{
			iRtnFunction = 10171;
			break;
		}
		break;
	case 10171:
		if (Dio.PCBvaccumOnCheck(true, false) == false)		//DRY_RUN
		{
			logStr.Format("PCB ���� ���� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10171;
			break;
		}
		logStr.Format("PCB ���� ���� Ȯ�� �Ϸ� [%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		iRtnFunction = 10172;
		break;
	case 10172://! Start P/B Button Push ���.

		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;

		MandoInspLog.func_InitData();

		Task.func_TactTimeInit();


		theApp.MainDlg->dispGrid();

		Task.m_timeChecker.Measure_Time(1);	//Start ��ư Ŭ�� �ð�
		MandoInspLog.bInspRes = true;

		vision.clearOverlay(CCD);

		iRtnFunction = 10180;
	
		break;
	case 10180:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10200;
			break;
		}
		else
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
			break;
		}
		break;
	case 10200:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z Wait ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 10202;
			break;
		}
		else
		{
			logStr.Format("Lens Z Wait ��ġ �̵� ���� [%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10200;
			break;
		}
		break;

	case 10202:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			logStr.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� �Ϸ�");
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10205;
			break;
		}
		else
		{
			logStr.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� ����");
			theApp.MainDlg->putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10200;
			break;
		}
		break;
	case 10205:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB TX,TY Wait_Pos �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 10500;
			break;
		}
		else
		{
			logStr.Format(_T("PCB TX,TY Wait_Pos Move Fail[%d]"), iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10205;
			break;
		}
		break;
	case 10500:
		if (sysData.m_iFront == 1)
		{
			logStr.Format("Door Open Mode [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 10900;
			break;
		}
		else
		{
			iRtnFunction = 10550;
			break;
		}
		break;

	case 10550:	//! Area ���� ���� Check 10450
		if (Dio.LightCurtainOnCheck(false, false))
		{
			if (Dio.DoorLift(false, false))
			{
				Sleep(100);
				iRtnFunction = 10600;
				break;
			}
			else
			{
				logStr.Format("Door Close ���� [%d]", iUseStep);	//Door Close ����
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -10550;
				break;
			}
		}
		else
		{
			logStr.Format(("LightCurtain ����[%d]"), iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10550;
			break;
		}
		break;

	case 10600:	//Light Ŀư ���� ���� ���� Ȯ�� 10460
		if (Dio.DoorLiftOnCheck(false, false))
		{
			iRtnFunction = 10900;
			break;
		}

		if (Dio.LightCurtainOnCheck(true, false))//Lift Close���� ���� ���� ���� �� ��� Open
		{
			if (Dio.DoorLift(true, false))
			{
				iRtnFunction = 10550;
				putListLog(logStr);
				break;
			}
			else
			{
				logStr.Format("Door Close ���� [%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -10600;
				break;
			}
		}
		break;
	case 10900:
		iRtnFunction = 19000;
		break;
	case 19000:
		iRtnFunction = 20000;
		break;
	default:
		logStr.Format("HolderRun_Loading Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int	CPcbProcess2::HolderRun_Dispensing(int iUseStep)		//����			20000 ~ 29000
{
	int iRtnFunction = iUseStep;
	CString logStr = "";
	bool bChk = false;
	switch (iUseStep)
	{
	case 20000:

		iRtnFunction = 20200;
		break;
	case 20200:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB_Z�� �����ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -20200;
			break;
		}

		if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		{
			iRtnFunction = 29000;
			break;
		}
		else
		{
			iRtnFunction = 20400;
			break;
		}
		break;
	case 20400:
		if (motor.Pcb_Motor_Move(Dispense_Pos))
		{
			logStr.Format("PCB Dispense ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 20600;
		}
		else
		{
			logStr.Format("PCB Dispense ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -20400;
		}
		
		break;
	case 20600:
		if (motor.PCB_Z_Motor_Move(Dispense_Pos))
		{
			iRtnFunction = 20800;
		}
		else
		{
			logStr.Format("PCB Z Dispense ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -20600;
		}
		
		break;
	case 20800:
		if (sysData.nEpoxyIndex == CRICLE_EPOXY)
		{
			bChk = motor.func_Epoxy_CircleDraw();
		}
		else if (sysData.nEpoxyIndex == RECT_EPOXY)
		{
			bChk = motor.func_Epoxy_Draw();
		}
		else if (sysData.nEpoxyIndex == POLYGON_EPOXY)
		{
			motor.func_Epoxy_Rect_Circle_Draw();
		}
		else if (sysData.nEpoxyIndex == POINT_EPOXY)
		{
			//point ��
			logStr.Format("PCB ���� ������ ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -20800;
			break;
		}

		if (bChk)
		{
			Task.m_bOkDispense = 1;

			logStr.Format("PCB Dispense �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 21000;
		}
		else
		{
			logStr.Format("PCB Dispense ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -20800;
		}
		
		break;
	case 21000:
		if (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y))
		{
			iRtnFunction = 29000;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 60000)
		{
			logStr.Format(_T("Dispense Fail[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -21000;
		}
		break;

	case 29000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(" PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -29000;
			break;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 30000;
			break;
		}
		break;
	default:
		logStr.Format("HolderRun_Dispensing Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}
int	CPcbProcess2::HolderRun_LensLoading(int iUseStep)		//���� �ε�		30000 ~ 39000
{
	int iRtnFunction = iUseStep;
	CString logStr = "";

	switch (iUseStep)
	{
	case 30000:
		iRtnFunction = 30200;
		break;

	case 30200:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 30400;
		}
		else
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30200;
		}
		break;
	case 30400:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z Wait ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 30600;
		}
		else
		{
			logStr.Format("Lens Z Wait ��ġ �̵� ���� [%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30400;
		}
		break;
	case 30600:

		if (Task.m_bOKLensPass != 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{
			//�������� ƿƮ ���ؼ� ������ġ���� �ٽ� ������ ƿƮ�� tx,ty ������ �̵� 

			if (!motor.Pcb_Tilt_Motor_Move(Loading_Pos))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				putListLog(logStr);
				delayMsg(logStr, 3000, M_COLOR_RED);
				iRtnFunction = -30600;
				break;
			}

			if (!motor.Pcb_Motor_Move(Loading_Pos))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				putListLog(logStr);
				delayMsg(logStr, 3000, M_COLOR_RED);
				iRtnFunction = -30600;
				break;
			}
			iRtnFunction = 30800;
			break;
		}
		else
		{
			iRtnFunction = 39000;	//���� �ε� �Ϸ� Jump
		}
		break;
	case 30800:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -30800;
			break;
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 31000;
		break;

	case 31000:
		if (Dio.LensMotorGripCheck(false, false))
		{
			iRtnFunction = 31200;
			Task.PCBTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Unit Gripper ���� Ȯ�� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31000;
		}
		break;

	case 31200:
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -31200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(Loading_Pos))
		{
			logStr.Format("Lens %s X,Y �̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -31200;
			break;
		}

		logStr.Format("Lens ���� ��ġ �̵� �Ϸ�[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 31400;
		break;

	case 31400:
		if (!motor.PCB_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB loading Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31400;
			break;
		}
		iRtnFunction = 31600;
		break;

	case 31600:
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31600;
			break;
		}
		iRtnFunction = 31800;
		break;
	case 31800:
		iRtnFunction = 32000;
		break;
	case 32000:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 32200;
		break;
	case 32200:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 32400;
		break;

	case 32400:
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 32600;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens �׸� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32400;
		}
		break;
	case 32600:
		if (Dio.LensMotorGripCheck(true, false))
		{
			Sleep(500);
			logStr.Format("Lens �׸� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);

			iRtnFunction = 32800;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ���� ���� �ð� �ʰ�..[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32400;
		}
		break;
	case 32800:
		iRtnFunction = 39000;
		break;

	case 39000:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39000;
			break;
		}

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39000;
			break;
		}

		iRtnFunction = 40000;		//next ���� + UV
		break;
	default:
		logStr.Format("HolderRun_LensLoading Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}
int	CPcbProcess2::HolderRun_BondingUv(int iUseStep)			//���� Uv		40000 ~ 49000
{
	int iRtnFunction = iUseStep;
	CString logStr = "";

	switch (iUseStep)
	{
	case 40000:

		iRtnFunction = 40200;
		break;
	case 40200:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40200;
			break;
		}

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40200;
			break;
		}
		iRtnFunction = 40400;
		break;
	case 40400:
		if (motor.Pcb_Motor_Move(Bonding_Pos))
		{
			logStr.Format("PCB Bonding ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 40600;
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			logStr.Format("PCB Bonding ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40400;
		}
		break;
	case 40600:
		if (motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
		{
			logStr.Format("PCB TX,TY Bonding_Pos �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 40800;
		}
		else
		{
			logStr.Format(_T("PCB Tilt Bonding Pos Move Fail[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40600;
			break;
		}
		
		break;
	case 40800:
		if (motor.Lens_Motor_MoveXY(Bonding_Pos))
		{
			logStr.Format("Lens X,Y Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);


			if (!motor.Lens_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -40800;
				break;
			}
			else
			{
				logStr.Format("Lens TX,TY Bonding_Pos �̵� �Ϸ� [%d]", iUseStep);
				putListLog(logStr);
				iRtnFunction = 41000;
				break;
			}
		}
		else
		{
			logStr.Format("Lens Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40800;
			break;
		}
		
		break;
	case 41000:

		iRtnFunction = 41200;
		break;
	case 41200:
		if (motor.LENS_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("Lens Z Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 41400;

		}
		else
		{
			logStr.Format("Lens Z Bonding ��ġ �̵� ���� [%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
		
		break;
	case 41400:
		if (!motor.PCB_Z_Motor_Move(Bonding_Pos))
		{
			logStr.Format("PCB Z�� Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41400;
			break;
		}
		else
		{
			logStr.Format("PCB Z�� Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 41600;
		}

		break;
	case 41600:
		//Lens �� Pcb Z�� Offset ��� �� ���� ���� ��
		iRtnFunction = 41800;
		break;
	case 41800:
		iRtnFunction = 42000;
		break;
	case 42000:
		iRtnFunction = 42200;
		break;
	case 42200:
		logStr.Format("===> Holder UV ON");
		putListLog(logStr);

		//Dio.UVTrigger(true, false);
		UVCommand.UV_Shutter_Open();
		iRtnFunction = 42400;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 42400:

		if (myTimer(true) - Task.PCBTaskTime > model.UV_Time)  // Delay üũ
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 42600;
		}

		break;
	case 42600:
		logStr.Format("===> Holder UV OFF [%d]", iRtnFunction);
		putListLog(logStr);

		//Dio.UVTrigger(false, false);
		UVCommand.UV_Shutter_Close();
		iRtnFunction = 42800;
		break;

	case 42800:
		Sleep(600);
		iRtnFunction = 43200;
		break;

	case 43200:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -43200;
			break;
		}

		Task.PCBTaskTime = myTimer(true);
		Task.m_bOKLensPass = -1;//���� �ѱ� �Ϸ� Lamp ���


		iRtnFunction = 43400;
		break;

	case 43400:
		if (Dio.LensMotorGripCheck(false, false))
		{
			Sleep(300);
			logStr.Format("Lens UnGrip Ȯ�� �Ϸ� [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 43600;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens UnGrip Ȯ�� �Ϸ� �ð� �ʰ�[%d]", iRtnFunction);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -43200;
		}
		break;


	case 43600:
		iRtnFunction = 43800;
		break;
	case 43800:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -43800;
			break;
		}
		else
		{
			logStr.Format("Lens Z ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 44000;
		}
		break;
	case 44000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z ��� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -44000;
			break;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 44200;
		}
		
		break;
	case 44200:
		iRtnFunction = 44400;
		break;
	case 44400:
		iRtnFunction = 49000;
		break;

	case 49000:
		iRtnFunction = 50000;
		break;
	default:
		logStr.Format("HolderRun_BondingUv Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}
int	CPcbProcess2::HolderRun_FinalOut(int iUseStep)			//����			50000 ~ 59000
{
	int iRtnFunction = iUseStep;
	CString logStr = "";

	switch (iUseStep)
	{
	case 50000:

		iRtnFunction = 50200;
		break;
	case 50200:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens�� z�� ��� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -50200;
			break;
		}
		else
		{
			logStr.Format("Lens�� z�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 50400;
		}
		break;
	case 50400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Pcb�� z�� ��� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -50400;
			break;
		}
		else
		{
			logStr.Format("Pcb�� z�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 50600;
		}
		break;
	case 50600:
		if (motor.Pcb_Motor_MoveY(Wait_Pos))
		{
			logStr.Format("Pcb y ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 50800;
		}
		else
		{
			logStr.Format("Pcb y ��� ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -50600;
			break;
		}
		
		break;
	case 50800:
		if (motor.Lens_Motor_MoveY(Wait_Pos))
		{
			logStr.Format("Lens Y �����ġ �̵� ���[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 51000;
		}
		else
		{
			logStr.Format("Lens Y �����ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -50800;
			break;
		}
		
		break;
	case 51000:

		iRtnFunction = 51200;
		break;
	case 51200:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 51400;

		}
		else
		{
			logStr.Format("PCB ��� ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -51200;
		}
		break;

	case 51400:
		if (!motor.Lens_Motor_MoveXY(Wait_Pos))
		{
			logStr.Format("Lens ���� ��� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -51400;
			break;
		}
		else
		{
			logStr.Format("Lens ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 51600;
		}
		break;
	case 51600:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB TX,TY ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 51800;
		}
		else
		{
			logStr.Format("PCB TX,TY �����ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -51600;
		}
		break;

	case 51800:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			logStr.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 52000;
		}
		else
		{
			logStr.Format("LENS�� Tx, Ty�� ��� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -51800;
		}
		break;
	case 52000:
		iRtnFunction = 59000;
		break;
	case 59000:
		iRtnFunction = 10000;
		break;
	default:
		logStr.Format("HolderRun_FinalOut Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}