#include "StdAfx.h"
#include "PcbProcess1.h"

CWinThread* pThread_TaskCCD1 = NULL;
bool bThreadCCDRun1 = false;
typedef struct THREADV
{
	int t_Unit;
}_threadV;
//
UINT Thread_TaskCCD1(LPVOID parm)
{
	CAABonderDlg* pFrame = (CAABonderDlg*)AfxGetApp()->m_pMainWnd;
	CString logStr = "";


	bThreadCCDRun1 = true;
	if (!pFrame->MIUCheck_process())// || gMIUDevice.CurrentState != 4)
	{
		logStr.Format("CCD ��� ���� �ʱ�ȭ ����.\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.");
		errMsg2(Task.AutoFlag, logStr);
		bThreadCCDRun1 = false;
		return false;
	}
	Sleep(300);
	bThreadCCDRun1 = false;


	return true;
}

void CPcbProcess1::putListLog(CString strLog)
{
	theApp.MainDlg->putListLog(strLog);
}


CPcbProcess1::CPcbProcess1(void)
{


}


CPcbProcess1::~CPcbProcess1(void)
{
}

int CPcbProcess1::Ready_process(int iStep)
{
	CString sLog = "";
	int iRtnFunction = iStep;

	switch (iStep)
	{
	case 10000:
		Task.PCBTaskTime = myTimer(true);
		vision.clearOverlay(CCD);

		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);



		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			Dio.LensTailGrip(false, false);
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

		//Laser �Ǹ��� �ʱ�ȭ
	case 10160:
		if (Dio.LaserCylinderUPDOWN(true, false))
		{
			sLog.Format("LASER �Ǹ��� ��� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 10170;
		}
		else
		{
			sLog.Format("LASER �Ǹ��� ��� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -10160;
		}
		break;

	case 10170:
		if (Dio.LaserCylinderCheck(true, false))
		{
			sLog.Format("LASER �Ǹ��� ��� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(sLog);
			iRtnFunction = 10200;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			sLog.Format("LASER �Ǹ��� ��� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -10170;
		}
		break;
		//Laser �Ǹ��� �ʱ�ȭ END

	case 10200:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))		//PCB Z�� ��� ��ġ �̵�
		{
			iRtnFunction = 10300;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING305);
			sLog.Format(sLangChange, iStep);	//"[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10200;
		}

		break;

	case 10300:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))		//Lens Z�� ��� ��ġ
		{
			iRtnFunction = 10400;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING303);
			sLog.Format(sLangChange, iStep);	//"[���� �غ�] Lens Z�� �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10300;
		}
		break;
	case 10400:

		iRtnFunction = 10600;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;

	case 10600:
		if (motor.Pcb_Motor_Move(Wait_Pos))			//PCB�� ���� �����ġ �̵�
		{
			iRtnFunction = 10700;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING306);
			sLog.Format(sLangChange, iStep);	//"[���� �غ�] PCB �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10600;
		}
		break;
	case 10700:
		if (motor.Lens_Motor_MoveXY(0, Wait_Pos))			//Lens�� ���� ��� ��ġ �̵�(Lens X/Y/Z��)
		{
			iRtnFunction = 10750;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING304);
			sLog.Format(sLangChange, iStep);	//"[���� �غ�] Lens �����ġ �̵� ����[%d]"
			putListLog(sLog);
			iRtnFunction = -10700;
		}
		break;
	case 10750:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10800;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING1017);	//PCB�� Tx, Ty�� ��� ��ġ �̵� ����
			sLog.Format(sLangChange);
			putListLog(sLog);
			iRtnFunction = -10750;
		}
		break;
	case 10800:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10900;
			sLog.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(sLog);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENS�� Tx, Ty�� ��� ��ġ �̵� ����
			sLog.Format(sLangChange);
			putListLog(sLog);
			iRtnFunction = -10800;
		}
		break;

	case  10900:
		///////////////////////
		//if( !Dio.LensGrip(true, true) )
		//{		//-- Lens ���� ���¿��� Grip ���� �ȵ� ��� NG
		//	sLangChange.LoadStringA(IDS_STRING300);
		//	sLog.Format(sLangChange, iStep);	//"[���� �غ�] Lens Grip ���� ���� [%d]"
		//	putListLog(sLog);
		//	iRtnFunction = -10900;
		//	break;
		//}			
		//else if( Dio.LensGripCheck(false, false) )			//Lens ������ �ȵǾ��� ���  
		//{//-- Lens Grip ���� ����
		//if(! Dio.HolderGrip(false,false) )    //-- Lens ���� �ȵǰ�/ Grip ���� �ȵ� ��� NG
		//{  
		//	sLangChange.LoadStringA(IDS_STRING301);
		//	sLog.Format(sLangChange, iStep);	//"[���� �غ�] Lens Grip ���� ���� [%d]"
		//	putListLog(sLog);
		//	iRtnFunction = -10900;
		//	break;
		//}
		iRtnFunction = 10950;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	case 10950:
		iRtnFunction = 10970;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	case 10970:
		if (!MIU.Stop())					// 95 ~ 100 msec
		{
			sLog.Format(_T("CCD CLOSE FAIL [%d]"), iStep);
			putListLog(sLog);
			iRtnFunction = -10970;
			break;
		}
		iRtnFunction = 15000;	//While�� ����
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	case 15000:
		UVCommand.UV_Shutter_PowerSet(model.UV_Power);// 95);//
		sLog.Format("	#1 UV POWER SET: %d", model.UV_Power);
		putListLog(sLog);
		Sleep(500);
		UVCommand2.UV_Shutter_PowerSet(model.UV_Power);// 95);//
		sLog.Format("	#2 UV POWER SET: %d", model.UV_Power);
		putListLog(sLog);
		iRtnFunction = 19000;
		break;
	case 19000:
		Task.AutoReday = 1;
		//		AutoRunView(3);
		iRtnFunction = 19900;
		sLog.Format("		Ready Step [%d]", iRtnFunction);
		putListLog(sLog);
		break;
	}
	return iRtnFunction;
}
int CPcbProcess1::RunProc_ProductLoading(int iUseStep)
{//! �۾��� ��ǰ ���� �ڵ� Step		(10000 ~ 10999)
	int iRtnFunction = iUseStep;
	CString logStr = "", sBarCode = "";
	bool bChk = true;

	switch (iUseStep)
	{
	case 10000://! DoorĿư Lift Open.
		Task.interlockLens  = 0;
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
		Dio.LaserCylinderUPDOWN(true, false);

		logStr.Format("LASER �Ǹ��� ��� �Ϸ�[%d]", iRtnFunction);
		theApp.MainDlg->putListLog(logStr);
		Task.PCBTaskTime = myTimer(true);

		iRtnFunction = 10060;
		break;

	case 10060:
		if (Dio.LaserCylinderCheck(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 10100;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� ��� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10060;
		}
		break;

	case 10100://! Chip ���� Draw
		theApp.MainDlg->func_ChipID_Draw();

		iRtnFunction = 10110;

		logStr.Format("		Start ��ư�� �����ּ���[%d]", iRtnFunction);
		putListLog(logStr);
		theApp.MainDlg->m_btnStart.m_iStateBtn = 4;
		theApp.MainDlg->m_btnStart.Invalidate();
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
					logStr.Format("		loading Step [%d]", iRtnFunction);
					putListLog(logStr);
					break;
				}
			}
			logStr.Format("		loading Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		break;
	case 10165://! Start P/B Button Push ���. 
			   //Dio.StartPBLampOn(false);
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
		putListLog("	");
		putListLog(logStr);
		putListLog("	");
		iRtnFunction = 10170;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;
	case 10170://! Start P/B Button Push ���.
	{
		bool bflag = true;

		Task.d_Align_offset_x[LENS_Align_MARK] = 0;
		Task.d_Align_offset_y[LENS_Align_MARK] = 0;
		Task.d_Align_offset_th[LENS_Align_MARK] = 0;

		/*bflag = EpoxyTimeCheck(bEpoxyTimeChk);
		if(!bflag)
		{
		iRtnFunction = -10170;
		break;
		}*/
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
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
	}
	break;
	case 10180:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 10200;
			logStr.Format("		loading Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -10180;
		}
		break;
	case 10200:
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 10202;
			logStr.Format("		Ready Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENS�� Tx, Ty�� ��� ��ġ �̵� ����
			logStr.Format(sLangChange);
			putListLog(logStr);
			iRtnFunction = -10200;
		}
		break;

	case 10202:
		if (Dio.LaserCylinderCheck(true, false) == false)
		{
			logStr.Format("LASER �Ǹ��� ��� Ȯ�� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = -10202;
			break;
		}
		iRtnFunction = 10205;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;

	case 10205:
		if (sysData.m_iProductComp == 1)
		{
			iRtnFunction = 10250;
			logStr.Format("		loading Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			iRtnFunction = 10210;
			logStr.Format("		loading Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		break;

	case 10210:
		if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
		{
			iRtnFunction = 10250;
			break;
		}

		iRtnFunction = 10250;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;

	case 10250:
		iRtnFunction = 10400;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;
	case 10400:	//! ���ڵ� ���� Check

				////////////////////////////////
		MandoInspLog.func_InitData();	//-- Log �ʱ�ȭ
		sBarCode.Format("%s", Task.ChipID);

		if (sBarCode == "EMPTY")//���ڵ������� ���� ��� Ȯ�� Message
		{
			logStr.Format("���ڵ� ������ �����ϴ�. \n [ %s ]������ ���� �Ͻðڽ��ϱ�?", Task.ChipID);
			if (askMsg(logStr) == IDOK)
			{
				sBarCode.Format("%s", Task.ChipID);
			}
			else
			{
				iRtnFunction = 10100;
				logStr.Format("		loading Step [%d]", iRtnFunction);
				putListLog(logStr);
				break;
			}
		}
		else
		{
			if (!g_ADOData.func_AATaskToRecordID(modelList.curModelName, Task.ChipID))
			{
				logStr.Format("		loading Step [%d]", iRtnFunction);
				putListLog(logStr);
#if (____AA_WAY == PCB_TILT_AA)
				/*if(sysData.m_iProductComp==1)
				{
				logStr.Format("��ġ�ϴ� ���̵� �����ϴ�.[%d]", iUseStep);
				}else
				{
				logStr.Format("DataBase ���� ����.[%d]\n MS Office�� �ݾ��ּ���.", iUseStep);
				}
				errMsg2(Task.AutoFlag,logStr);
				iRtnFunction = -10400;
				break;*/
#endif		

			}
		}


		iRtnFunction = 10440;

		break;
	case 10440:
		if (sysData.m_FreeRun == 0 && sysData.m_iProductComp != 1)
		{
			pThread_TaskCCD1 = ::AfxBeginThread(Thread_TaskCCD1, this);

		}
		iRtnFunction = 10500;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;
	case 10500:
		iRtnFunction = 10900;
		break;

		break;
	//case 10550:
	//	//�׸�Ȯ��
	//	if (Dio.PcbGripCheck(true, false) == true)
	//	{
	//		logStr.Format(_T("PCB GRIP Ȯ�� �Ϸ� [%d]"), iRtnFunction);
	//		iRtnFunction = 10600;
	//	}
	//	else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
	//	{
	//		logStr.Format(_T("PCB GRIP ���� [%d]"), iRtnFunction);
	//		iRtnFunction = -10550;
	//		errMsg2(Task.AutoFlag, logStr);
	//	}
	//	break;
	//case 10600:
	//	//��
	//	Sleep(1000);
	//	if (Dio.PcbTurn(false, false) == true)
	//	{
	//		logStr.Format(_T("PCB RETURN �Ϸ� [%d]"), iRtnFunction);
	//		iRtnFunction = 10650;
	//	}
	//	else
	//	{
	//		logStr.Format(_T("PCB RETURN ���� [%d]"), iRtnFunction);
	//		iRtnFunction = -10600;
	//		errMsg2(Task.AutoFlag, logStr);
	//	}
	//	putListLog(logStr);
	//	Task.PCBTaskTime = myTimer(true);

	//	break;
	//case 10650:
	//	//��Ȯ��
	//	if (Dio.PcbTurnCheck(false, false) == true)
	//	{
	//		logStr.Format(_T("PCB RETURN Ȯ�� �Ϸ� [%d]"), iRtnFunction);
	//		iRtnFunction = 10700;
	//	}
	//	else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
	//	{
	//		logStr.Format(_T("PCB RETURN Ȯ�� ���� [%d]"), iRtnFunction);
	//		iRtnFunction = -10650;
	//		errMsg2(Task.AutoFlag, logStr);
	//	}

	//	break;
	//case 10700:
	//	//PCB �ε� �� Ȯ�� �����ּ���
	//	if (askMsg("PCB �ε� �� Ȯ�� �����ּ���") == IDOK)
	//	{
	//		iRtnFunction = 10900;
	//	}

	//	break;
	case 10900:
		Task.PcbOnStage = 100;
		Task.m_b_AA_Retry_Flag = false;

		logStr.Format("%d", Task.m_iPcbPickupNo);
		theApp.MainDlg->m_labelPickupNoPcb.SetText(logStr);
		theApp.MainDlg->m_labelPickupNoPcb.Invalidate();

		MandoInspLog.sBarcodeID.Format("%s", Task.ChipID);	//���� ������Camera �˻� Log ����
		iLaser_Pos = 0;


		if (sysData.m_iIrChartUse == 1)
		{
			IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_TOP1_CHART], IR_CHART);
		}
		else
		{
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART2, model.m_iLedValue[LEDDATA_TOP2_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART3, model.m_iLedValue[LEDDATA_TOP3_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART4, model.m_iLedValue[LEDDATA_TOP4_CHART]);
			LightControl.ctrlLedVolume(LIGHT_LEFT_CHART, model.m_iLedValue[LEDDATA_CHART_L]);
			LightControl.ctrlLedVolume(LIGHT_RIGHT_CHART, model.m_iLedValue[LEDDATA_CHART_R]);
		}




#if (____AA_WAY == PCB_TILT_AA)
		if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
		{
			logStr.Format("		Lens �׸� ���� [%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 15500;// 10250;
			break;
		}
#endif
		iRtnFunction = 11000;
		logStr.Format("		loading Step [%d]", iRtnFunction);
		putListLog(logStr);
		break;

	default:
		sLangChange.LoadStringA(IDS_STRING1007);	//PCB ��ǰ ���� Thread Step ��ȣ ������ . 
		logStr.Format("PCB ��ǰ ���� Thread Step ��ȣ ������ [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess1::RunProc_PCBOutsideAlign(int iUseStep)
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

	switch (iUseStep)
	{
	case 11000:
		/////////////////////////
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 11500;
		break;

	case 11500:
		iRtnFunction = 12000;
		break;
	case 12000://! ���ڵ� DB Data ����
			   /*if( !g_ADOData.func_AATaskToRecordID(modelList.curModelName, Task.ChipID) )
			   {
			   logStr.Format("DataBase ���� ����.[%d]\n MS Office�� �ݾ��ּ���.", iUseStep);
			   errMsg2(Task.AutoFlag,logStr);
			   iRtnFunction = -12000;
			   break;
			   }*/
		iRtnFunction = 13000;
		break;

	case 13000:
		if (Task.m_bOKLensPass == 1)   //Lens �ѱ� �Ϸ����
		{
			iRtnFunction = 14100;
			break;
		}

		Task.m_iRetry_Opt = 0;
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align�� ���� ON
		Task.d_Align_offset_x[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_y[PCB_Holder_MARK] = 0;
		Task.d_Align_offset_th[PCB_Holder_MARK] = 0;
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
			Sleep(1000);
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
				//logStr.Format("PCB Holder ��˻� %d ȸ ����[%d]", Task.m_iRetry_Opt, iUseStep);
				//errMsg2(Task.AutoFlag,logStr);
			}

			break;
		}

		offsetX = offsetY = offsetTh = 0.0;
		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Holder_MARK, false, offsetX, offsetY, offsetTh);
		offsetTh = 0.0;
		saveInspImage(PCB_IMAGE_SAVE, Task.m_iRetry_Opt);

		Task.m_iRetry_Opt++;
		offsetTh = 0;//holder x,y�ุ
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Holder_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Holder_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Holder_MARK] += offsetTh;

			logStr.Format("Pcb Holder : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);//0.000
																																													 //			Sleep(500);
																																													 //pcbDlg->m_labelAlignResult.SetText(logStr);
																																													 //pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);
			if (iRtnVal == 1)
			{
				iRtnFunction = 14000;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				sLangChange.LoadStringA(IDS_STRING654);	//Lens Align [%d] �Ϸ� [%d]
				logStr.Format("		Pcb Holder [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);

				theApp.MainDlg->InstantMarkDelete(PCB_Holder_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("		Pcb Holder Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]);//0.000);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 14100;				// �Ϸ�
			}
			else
			{
				logStr.Format("Pcb Holder [%d] ���� ���� �ʰ�[%d]", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(PCB_Holder_MARK);
				iRtnFunction = -13700;				// NG
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

int	CPcbProcess1::RunProc_SensorAlign(int iUseStep)
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
		offsetX = offsetY = offsetTh = 0.0;
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("		PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26000;
			break;
		}
		iRtnFunction = 26050;
		break;
	case 26050:
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 26600;
		break;

	case 26600:
		//Task.LensTaskTime = myTimer(true);
		iRtnFunction = 26610;
		break;

	case 26610:
		//ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(PCB_Chip_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align�� ���� ON

		if (motor.Pcb_Motor_Move(Align_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Align_Pos))
			{
				logStr.Format("PCB Sensor Align ��ġ �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);
				Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] = 0;
				iRtnFunction = 26615;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -26610;
		}
		break;
	case 26615:
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
			}
			else
			{
				iRtnFunction = 26620;
				logStr.Format("PCB Align ��˻� ����[%d]", iUseStep);
				putListLog(logStr);
			}
			break;

			break;
		}

		offsetX = offsetY = offsetTh = 0.0;

		iRtn = theApp.MainDlg->procCamAlign(CAM2 - 1, PCB_Chip_MARK, false, offsetX, offsetY, offsetTh);
		saveInspImage(CHIP_IMAGE_SAVE, Task.m_iRetry_Opt);
		Task.m_iRetry_Opt++;

		//offsetTh=0;
		if (iRtn == 0)
		{
			Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] -= offsetTh;


			Task.dPcbAlignTh = Task.d_Align_offset_th[PCB_Chip_MARK];

			logStr.Format("PCB Sensor Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM2 - 1, offsetX, offsetY, offsetTh);

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

				Task.m_timeChecker.Measure_Time(4);	//Laser ���� ���� ���� �ð�

				iRtnFunction = 26629;		// �Ϸ�//iRtnFunction = 16630;				// �Ϸ�
				LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] ���� ���� �ʰ�", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -26620;				// NG
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
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -26629;
			break;
		}
		iRtnFunction = 26990;
		break;
	case 26990://! Lens-Z�� ��� ��ġ �̵�
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

int	CPcbProcess1::RunProc_LaserMeasure(int iUseStep)
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
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("[���� �غ�] PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -27000;
			break;
		}

		//if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		//{
		//	iRtnFunction = 30000;	//-- ���� Pass Mode �̰ų�, ���� �Ϸ� �� ���
		//	break;
		//}

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
		if (iLaser_Pos == 0)
		{
			if (!motor.PCB_Z_Motor_Move(Laser_Pcb_Pos))
			{
				logStr.Format("PCB Laser_Pcb_Pos Z �̵� ����[%d]", iUseStep);
				putListLog(logStr);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -27150;
				break;
			}
			else
			{
				logStr.Format("PCB Laser_Pcb_Pos Z �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
			}
		}

		if (iLaser_Pos == 0) { iRtnFunction = 27160; }//ó������ �Ǹ��� �ϰ�
		else { iRtnFunction = 27200; }//ó���ƴϸ� �ٷ� �˻��.
		Task.PCBTaskTime = myTimer(true);
		break;
		//laser �Ǹ��� �ϰ�
	case 27160:
		if (Dio.LaserCylinderUPDOWN(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 27170;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� �ϰ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27160;
		}
		break;

	case 27170:
		if (Dio.LaserCylinderCheck(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 27200;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27170;
		}
		break;
		//laser�Ǹ��� �ϰ� END
	case 27200:
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			Sleep(200);
			iRtnFunction = 27250;
		}
		break;
	case 27250:

		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Sleep(100);
		logStr.Format("�� ������ %lf - %d ��ġ[%d]", Task.m_Laser_Point[iLaser_Pos], iLaser_Pos + 1, iUseStep);
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
						if (Task.m_dataOffset_x[0] == 0 || Task.m_dataOffset_y[0] == 0)
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
						iRtnFunction = 27310;//->Laser�Ǹ������, 27350;->PCB WaitPos
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

		//laser �Ǹ��� ���
	case 27310:
		if (Dio.LaserCylinderUPDOWN(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 27320;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� ��� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27310;
		}
		break;
	case 27320:
		if (Dio.LaserCylinderCheck(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 27350;
			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� ��� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27320;
		}
		break;
		//laser�Ǹ��� ��� END

	case 27350:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 27600;
		}
		else
		{
			logStr.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -27350;
		}

		break;
	case 27600:

		iRtnFunction = 27650;
		break;
	case 27650:
		//Sensor Align �߰�
		//
		offsetX = offsetY = offsetTh = 0.0;
		iRtnFunction = 27750;
		break;
	case 27750:
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 27800;
		break;
	case 27800:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);		// Align�� ���� ON

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
			Task.d_Align_offset_x[PCB_Chip_MARK] -= offsetX;
			Task.d_Align_offset_y[PCB_Chip_MARK] -= offsetY;
			Task.d_Align_offset_th[PCB_Chip_MARK] -= offsetTh;

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
				LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON 
				LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, 0);		// Align�� ���� ON
			}
			else
			{
				logStr.Format("PCB Sensor Align [%d] ���� ���� �ʰ�", Task.m_iRetry_Opt, iUseStep);
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

int CPcbProcess1::RunProc_EpoxyNewResing(int iUseStep)
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
			sLangChange.LoadStringA(IDS_STRING1012);	//PCB_Z�� �����ġ �̵� ����.
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = -30000;
			break;
		}

		if ((sysData.m_iDispensePass == 1) || (Task.m_bOkDispense == 1))
		{
			iRtnFunction = 34000;// 31000;	//-- ���� Pass Mode �̰ų�, ���� �Ϸ� �� ���
		}
		else
		{
			iRtnFunction = 30020;
		}
		break;
	case 30020: //Dispenser -Z�� �ϰ�
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 30030;
		}
		else
		{
			logStr.Format("PCB Z Wait ��ġ �̵� �Ϸ� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30020;
		}
		break;
	case 30030:  //���� ��ġ �̵�
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align ���� OFF
		if (motor.Pcb_Motor_Move(Dispense_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING935);	//PCB Dispense ���� ��ġ �̵� �Ϸ�-������ X:%.03lf,Y:%.03lf,T:%.03lf
			logStr.Format(sLangChange, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
			putListLog(logStr);
			iRtnFunction = 30040;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING938);	//PCB Dispense ��ġ �̵� ����[%
			logStr.Format(sLangChange + _T("d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30030;
		}
		break;
	case 30040: //Dispenser-z �� ���
		if (motor.PCB_Z_Motor_Move(Dispense_Pos))
		{
			iRtnFunction = 30050;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING977);	//PCB Z Dispense ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30040;
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
			LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_SENSOR]);//���� �˻��� ����θ�ũ�� ����Ʈ
			iRtnFunction = 30100;
		}
		break;
	case 30100:
		iRtnFunction = 31000;
		break;
	case 31000:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -31000;
			break;
		}
		else
		{
			logStr.Format("PCB Z ��� ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
		}
		iRtnFunction = 34000;
		/*if( sysData.m_iDispenseInspPass == 0 )
		{
		iRtnFunction = 32000;

		}else
		{
		iRtnFunction = 34000;
		}*/
		break;

	case 32000: // �����˻� �̵�
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align�� ���� ON
		if (motor.Pcb_Motor_Move(Holder_Pos))
		{
			iRtnFunction = 32500;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING989);	//PCB �����˻� ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32000;
		}
		break;

	case 32500:// �̵���, ���� �ð� ���� �� ��ũ Ȯ��
		if (motor.PCB_Z_Motor_Move(Holder_Pos))

		{
			Task.PCBTaskTime = myTimer(true);
			sLangChange.LoadStringA(IDS_STRING990);	//PCB �����˻� ��ġ �̵� �Ϸ�[%d]
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 33000;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING989);	//PCB �����˻� ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -32500;
		}
		break;
	case 33000: //�����˻� z�� �̵�
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
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
		}
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_EPOXY_INSP]);
		break;
	case 33500: //�����˻�                                �߰��ؾ���
		Sleep(100);
		vision.clearOverlay(CAM1);
		bChk = theApp.MainDlg->_EpoxyFinddispense(CAM1);
		vision.drawOverlay(CAM1);
		if (!bChk)
		{
			putListLog("Epoxy ���� �˻� ����");
			//logStr.Format("Epoxy Table ��ǥ (X:%.03f, Y:%.03f, ��:%.03f)", motor.GetEncoderPos(Motor_PCB_X), motor.GetEncoderPos(Motor_PCB_Y), motor.GetEncoderPos(Motor_PCB_TH));
			//putListLog(logStr);
			logStr.Format("Epoxy ���� �˻� ����. \n�ڵ� ���� ���� �Ͻðڽ��ϱ�?");
			//_stprintf_s(motor.m_szLogMsg, "Epoxy ���� �˻� ����. \n�ڵ� ���� ���� �Ͻðڽ��ϱ�?");

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
				iRtnFunction = -33500;
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
			sLangChange.LoadStringA(IDS_STRING1012);	//PCB_Z�� �����ġ �̵� ����.
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = -34000;
			break;
		}
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 39000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 35000;
#endif
		break;
	default:
		sLangChange.LoadStringA(IDS_STRING939);	//PCB Dispenser Thread Step ��ȣ ������ .
		logStr.Format(sLangChange + _T(" [%d]"), iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess1::RunProc_InspAAPos(int iUseStep)
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
	case 39000:
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 41000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 40000;
#endif		
		break;
		vision.clearOverlay(CAM1);
		vision.drawOverlay(CAM1);
		//if (sysData.m_iDefectInspPass == 1)	// DEFECT �˻� PASS
		//{
		//    iRtnFunction = 39999;
		//}
		//else
		//{
		//    iRtnFunction = 39100;
		//}
		//if(model.m_IsUseBrightDefectTest == 1)	// ���� Defect Picxel �˻� ����

		iRtnFunction = 39700;//aa�� defect Pass
		break;
	case 39100:
		LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, model.m_iLedValue[LEDDATA_6500K]);
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCB�� ���� 0C 6500K �˻� ��ġ �̵� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -39100;
		}
		else
		{
			iRtnFunction = 39300;
		}
		break;
	case 39300:
		if (!motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			logStr.Format(_T("PCB Z axis 0C 6500K Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39300;
			break;
		}
		else
		{
			logStr.Format(_T("PCB Z axis 0C 6500K Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39350;
			break;
		}


	case 39350: //UV�� OC Defect �˻� 
		Sleep(100);
		MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);
		logStr.Format("MID_6500K_RAW SAVE OK [%d]", iUseStep);
		putListLog(logStr);
		Sleep(500);

		LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, 0);
		iRtnFunction = 39400;
		break;

	case 39400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Defect Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39400;
			break;
		}
		else
		{
			logStr.Format(_T("PCB Z axis Defect Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39450;
			break;
		}

	case 39450:	//Dark �˻� ��ġ �̵�
		iRtnFunction = 39500;
		//if(model.m_IsUseDarkDefectTest == 1)	// �ϸ� Defect Picxel �˻� ����
		//{
		//	iRtnFunction = 18102;				//�ϸ� �˻� ��ġ �̵�
		//}
		//else
		//{
		//	iRtnFunction = 18115;				
		//}
		break;

	case 39500:	//Dark �˻� ��ġ �̵�
		if (!motor.Pcb_Motor_Move(Dark_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING1022);	//PCB�� ���� Dark �˻� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = -39500;
		}
		else
		{
			LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, 0);
			LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);
			iRtnFunction = 39600;
		}
		break;

	case 39600:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			logStr.Format(_T("PCB Z axis Dark Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39600;
		}
		else
		{
			logStr.Format(_T("PCB Z axis Dark Pos Move Complete [%d]"), iUseStep);
			//errMsg2(Task.AutoFlag,logStr);
			iRtnFunction = 39620;
		}
		break;

	case 39620:
		Sleep(500);
		MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);
		logStr.Format("LOW_LEVEL_RAW SAVE OK [%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 39650;
		break;
	case 39650:
		if (g_clApsInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, false) == false)
		{
			logStr.Format("[AUTO] Defect �˻� ���� [%d]", iUseStep);
			MandoInspLog.bInspRes = false;
		}

		logStr.Format("[AUTO] Defect �˻� �Ϸ� [%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 39700;
		break;

	case 39700:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format(_T("PCB Z axis Wait Pos Move Failed [%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -39700;
		}
		else
		{
			iRtnFunction = 39850;
		}
		break;
	case 39850:
		iRtnFunction = 39900;
		Task.PCBTaskTime = myTimer(true);
		break;
	case 39900: // ���� ��ġ �̵�
		iRtnFunction = 39999;
		break;
	case 39999:			//��������� 180706_1
						//if( func_MIU_ConnectLiveCheck() == false)		//MIU ���� Check
						//{
						//	iRtnFunction = -39999;
						//	break;
						//}
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 41000;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 40000;
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

int CPcbProcess1::RunProc_Bonding_Pos_GO(int iUseStep)
{
	int iRtnFunction = iUseStep;
	int iRtn = 0;
	CString logStr = "";
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	bool bChk = true;
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;

	switch (iUseStep)
	{
	case 41000:
		if (sysData.m_iIrChartUse == 1)
		{
			IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_TOP1_CHART], IR_CHART);
		}
		else
		{

		}
		

		iRtnFunction = 41050;
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
		
		iRtnFunction = 41150;
		break;
	case 41150: // ���� ��ġ �̵�
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 41200;
		break;
	case 41200:
		if (motor.Lens_Motor_MoveXY(0, Bonding_Pos))
		{
#if (____AA_WAY == LENS_TILT_AA)
			if (!motor.Lens_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41200;
				break;
			}

#endif
			logStr.Format("Lens Bonding ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			Task.interlockPcb = 1;
			iRtnFunction = 41250;
		}
		else
		{
			logStr.Format("Lens Bonding ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
			break;
		}
		break;
	case 41250:
#if (____AA_WAY == PCB_TILT_AA)
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING913);	//"PCB Bonding ��ġ �̵� �Ϸ�[%d]"
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 41300;
			if (!motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("PCB tx,ty Bonding_Pos �̵� ����");
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41250;
				break;
			}
			break;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41250;
			break;
		}
#elif	(____AA_WAY == LENS_TILT_AA)

		
		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			iRtnFunction = 41300;
			break;
		}
		else
		{
			if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
			{
				sLangChange.LoadStringA(IDS_STRING913);	//"PCB Bonding ��ġ �̵� �Ϸ�[%d]"
				logStr.Format(sLangChange, iUseStep);
				putListLog(logStr);
				iRtnFunction = 41300;	//jump
				break;
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
				logStr.Format(sLangChange + _T("[%d]"), iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -41250;
				break;
			}
		}
		
#endif
		break;
	case 41300:
		if (sysData.m_iEpoxyLaserInspPass == 1)
		{
			//Laser ���� Pass
			logStr.Format("Laser ���� pass[%d]", iRtnFunction);
			putListLog(logStr);
			if (!motor.Pcb_Tilt_Motor_Move(Bonding_Pos))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Bonding_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -41300;
				break;
			}
			iRtnFunction = 41310;
			break;
		}
		else
		{
			if (motor.Pcb_Tilt_Trinity_Move(Task.m_PcbLaserAfterTx, Task.m_PcbLaserAfterTy))	//Laser ������ ��
			{
				iRtnFunction = 41310;
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
				logStr.Format(sLangChange + _T("[%d]"), iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -41250;
			}
		}
		


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
		theApp.MainDlg->setCamDisplay(3, 1);
		iRtnFunction = 41400;

		break;
	case 41400:
		if (LGIT_MODEL_INDEX != M1_HEATING_GEN2)
		{
			iRtnFunction = 48000;
			break;
		}
		if (motor.Pcb_Motor_Move(Dark_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			logStr.Format("PCB Tail X,Y ��ġ �̵� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 41500;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41400;
		}

		break;
	case 41500:
		if (!askMsg("���� #1?"))
		{
			errMsg2(Task.AutoFlag, "�Ͻ�����");
			iRtnFunction = -41500;
			break;
		}
		iRtnFunction = 41600;
		break;
	case 41600:
		if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			logStr.Format("PCB Z�� Tail ��ġ �̵� ����[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41600;
			break;
		}
		else
		{
			logStr.Format("PCB Z�� Tail ��ġ �̵� �Ϸ�[%d]", iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 41700;
		}

		break;
	case 41700:
		Dio.LensTailGrip(false, false);

		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 41800;
		break;
	case 41800:
		if (Dio.LensTailGripCheck(false, false) == true)
		{
			logStr.Format("Lens Tail UnGrip Ȯ�� �Ϸ�");
			putListLog(logStr);
			iRtnFunction = 41900;
			break;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Tail UnGrip Ȯ�� �ð� �ʰ�");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41700;
			break;
		}

		break;
	case 41900:
		if (!askMsg("���� #2?"))
		{
			errMsg2(Task.AutoFlag, "�Ͻ�����");
			iRtnFunction = -41500;
			break;
		}
		iRtnFunction = 42000;
		break;
	case 42000:
		if (motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING913);	//"PCB Bonding ��ġ �̵� �Ϸ�[%d]"
			logStr.Format(sLangChange, iUseStep);
			putListLog(logStr);
			iRtnFunction = 42100;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING912);	//PCB Bonding ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -42000;
		}

		break;
	case 42100:
		if (!askMsg("���� #3?"))
		{
			errMsg2(Task.AutoFlag, "�Ͻ�����");
			iRtnFunction = -41500;
			break;
		}
		iRtnFunction = 48000;
		break;
	case 48000:
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		// Align�� ���� ON

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

int	CPcbProcess1::procAutoFocus(int iStep)
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
		if (gMIUDevice.bMIUOpen == 0 || gMIUDevice.CurrentState != 4)
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
	
		MbufClear(vision.MilOptImage, 0x00);
		MbufClear(vision.MilDefectImage, 0x00);

		Task.initSFRElem();

		Task.PCBTaskTime = myTimer(true);

		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, "-");

		theApp.MainDlg->autodispDlg->DrawGraph(0);
		theApp.MainDlg->autodispDlg->DrawGraph(1);
		MandoInspLog.bInspRes = true;	//60000 �ʱ�ȭ
		Task.m_timeChecker.Measure_Time(6);	//AA ���� �ð�
		iRtnFunction = 60010;
		break;
	case  60010:
		if (bThreadCCDRun1 == false)		//	CCD ���� ������ ���߸� ����... step = 12000 ����
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
					if (motor.IsStopAxis(Motor_Lens_Z))// && motor.GetInposition(Motor_Lens_Z) )
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
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else {
			iRtnFunction = -60100;
		}
		Task.PCBTaskTime = myTimer(true);
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

					theApp.MainDlg->autodispDlg->DrawBarGraph();
					//MIU.func_Set_InspImageCopy(CHART_RAW);			//1�� AA�� 
					Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);		//1�� AA�� ����
					if (Task.sfrResult)
					{
						logStr.Format("1�� AA ��� Spec OK.");
						putListLog(logStr);
						iRtnFunction = 69999;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
						////int boxIndex[5] = {SFR_CENTER_UP, MTF_INSP_CNT-8, MTF_INSP_CNT-6, MTF_INSP_CNT-4, MTF_INSP_CNT-2};

						//double maxCornor = Task.SFR._64_fSfrN4[boxIndex[1]];//SFR_LEFT_UP_INDEX
						//double minCornor = Task.SFR._64_fSfrN4[boxIndex[1]];//SFR_LEFT_UP_INDEX

						//for (int i=2; i<5; i++)
						//{
						//	if (maxCornor < Task.SFR._64_fSfrN4[boxIndex[i]])
						//		maxCornor = Task.SFR._64_fSfrN4[boxIndex[i]];
						//	if (minCornor > Task.SFR._64_fSfrN4[boxIndex[i]])
						//		minCornor = Task.SFR._64_fSfrN4[boxIndex[i]];
						//}

						//if( maxCornor-minCornor < sysData.dLimit_SFR_Balance)
						//{
						//	//saveSfrSumLog();//���� Data log ����
						//	logStr.Format("1�� AA ��� Spec OK.");
						//	putListLog(logStr);
						//	iRtnFunction = 69999;
						//	logStr.Format("	AA Step [%d]" , iRtnFunction);
						//	putListLog(logStr);
						//	break;
						//}
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
			Task.PCBTaskTime = myTimer(true);
		}
		else
		{
			iRtnFunction = -60200;
		}
	}

	break;

	case 60900:			/* Z�� ���� ��ġ ���� �� */
	{
		if ((myTimer(true) - Task.PCBTaskTime) > model.strInfo_Cam[CCD].m_iDelayTime)
		{
			iRtnFunction = 60910;
		}
		else
		{
			iRtnFunction = 60900;
		}
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
				if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x &&fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y)
				{
					logStr.Format("		 ���� Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf",
						sysData.m_dOcSpec.x, sysData.m_dOcSpec.y,
						Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);
					iRtnFunction = 62000;
					//Task.ocCheckIng = false; 
					break;
				}
				else
				{
					logStr.Format("		 ���� Shift.  X %.3lf, Y %.3lf ,Spec(%.03lf, %.03lf)", Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y);
					putListLog(logStr);
					iRtnFunction = 60920;
					//Task.ocCheckIng = true;
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
	{
		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 62000;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
	}

	break;

	case 62000:
	{
		iRtn = theApp.MainDlg->_getMTF((Task.bFirstAA == true) ? SFR_FINAL : SFR_FIRST);
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
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
				}
				else if (Task.m_bFlag_Decrease_SFR == true && Task.m_iCnt_Step_AA == model.m_iCnt_Check_SFR * 2)
				{
					iRtnFunction = 63000;
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
				}
				else {
					iRtnFunction = 64000;
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
				}
			}
			else {
				iRtnFunction = 64000;
				logStr.Format("	AA Step [%d]", iRtnFunction);
				putListLog(logStr);
			}
			//////////////////////////////////////////////////////////////////////////
		}
		else {
			iRtnFunction = -63000;
		}
	}

	break;

	case 63000:		/* �ʱ� 3�� ������ Ȯ�� �� ���� ���̸� Z�� ���� �̵� */
	{
		iRtn = theApp.MainDlg->_checkDecreaseSFR();
		if (iRtn > 0) {
			iRtnFunction = 64000;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			Task.m_bFlag_Decrease_SFR = true;
			theApp.MainDlg->autodispDlg->m_iCur_Index_Through = Task.m_iCnt_Step_AA_Total;

			//autodispDlg->DrawGraph(0);			//------>20180607_1
			//autodispDlg->DrawGraph(1);
			iRtnFunction = 60070;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
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
			//if (!Task.bFirstAA)
			//{  
			//	Task.SFR.dTilt_Y = _calcTiltX();//*model.Tilt_Weight_Pre;//*modelSign;//modelWeight_Pre;
			//	Task.SFR.dTilt_X = _calcTiltY();//*model.Tilt_Weight_Pre;//*modelSign;//modelWeight_Pre;
			//}else
			//{
			//	Task.SFR.dTilt_Y = _calcTiltX();//*model.Tilt_Weight_Post;//*modelSign;//modelWeight_Post;
			//	Task.SFR.dTilt_X = _calcTiltY();//*model.Tilt_Weight_Post;//*modelSign;//modelWeight_Post;
			//} 
			//
#if 0
			if (Task.bFirstAA == false)
			{
				double dCurPos = motor.GetCommandPos(TITLE_MOTOR_Z);
				double setZ = sysData.m_dOffset_Conti_Down_Z;
				if (true)//setZ < 0)
				{
					double dMovePos = setZ + dCurPos;
					logStr.Format("Z CurPos : (%.3f) , Z down : (%.3f)", dMovePos, setZ);
					putListLog(logStr);
					////
					if (fabs(setZ) > 0.5)
					{
						logStr.Format("Z Down Limit :%.3f [0.5]", fabs(setZ));
						putListLog(logStr);
						errMsg2(Task.AutoFlag, logStr);
						iRtnFunction = -64000;
						break;
					}
					motor.MoveAxis(TITLE_MOTOR_Z, ABS, dMovePos, sysData.fMotorSpeed[TITLE_MOTOR_Z], sysData.fMotorAccTime[TITLE_MOTOR_Z]);
					double ep = myTimer(true);
					while (1)
					{
						if (myTimer(true) - ep < 5000)
						{
							if (motor.IsStopAxis(TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
							{
								dCurPos = motor.GetEncoderPos(TITLE_MOTOR_Z);
								if (fabs(dCurPos - dMovePos) <= 0.003)
								{
									Task.PCBTaskTime = myTimer(true);
									logStr.Format("Z Down ��ġ �̵� �Ϸ�");
									putListLog(logStr);
									break;
								}
							}
						}
						else
						{
							logStr.Format("Z Down ��ġ �̵� �ð� �ʰ�");
							putListLog(logStr);
							iRtnFunction = -64000;
							break;
						}
					}
					Sleep(600);
					//------------------------------------------------------------------------
					dCurPos = motor.GetCommandPos(TITLE_MOTOR_Z);
					setZ = sysData.m_dOffset_Conti_Down_Z * -1;
					dMovePos = setZ + dCurPos;
					logStr.Format("Z CurPos 2: (%.3f) , Z down : (%.3f)", dMovePos, setZ);
					putListLog(logStr);
					//end 1
					motor.MoveAxis(TITLE_MOTOR_Z, ABS, dMovePos, sysData.fMotorSpeed[TITLE_MOTOR_Z], sysData.fMotorAccTime[TITLE_MOTOR_Z]);
					ep = myTimer(true);
					while (1)
					{
						if (myTimer(true) - ep < 5000)
						{
							if (motor.IsStopAxis(TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
							{
								dCurPos = motor.GetEncoderPos(TITLE_MOTOR_Z);
								if (fabs(dCurPos - dMovePos) <= 0.003)
								{
									Task.PCBTaskTime = myTimer(true);
									logStr.Format("Z Down ����ġ �̵� �Ϸ�");
									putListLog(logStr);
									break;
								}
							}
						}
						else
						{
							logStr.Format("Z Down ����ġ �̵� �ð� �ʰ�");
							putListLog(logStr);
							iRtnFunction = -64000;
							break;
						}
					}
					//end 2
				}
			}
#endif

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

			if (sysData.m_iDicChange == 0)
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltX() *  mDixTx;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltY() *  mDixTy;
			}
			else
			{
				Task.SFR.dTilt_X = theApp.MainDlg->_calcTiltY() *  mDixTy;
				Task.SFR.dTilt_Y = theApp.MainDlg->_calcTiltX() *  mDixTx;
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
				logStr.Format("	AA Step [%d]", iRtnFunction);
				putListLog(logStr);
			}
		}
		else {
			iRtnFunction = 60100;		/* �ٽ� ���� */
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
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
		z_move_dist = fabs(model.axis[theApp.MainDlg->TITLE_MOTOR_Z].pos[Bonding_Pos] - dMovePos) * -1;	//1�� ->2 ���̵�
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
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
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
	{
		iRtn = theApp.MainDlg->_procTiltX();

		if (iRtn>0) {
			iRtnFunction = 64600;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else {
			iRtnFunction = -64400;
		}
	}

	break;

	case 64600:
	{
		iRtn = theApp.MainDlg->_procTiltY();

		if (iRtn>0) {
			iRtnFunction = 64700;
		}
		else {
			iRtnFunction = -64600;
		}
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
			theApp.MainDlg->autodispDlg->DrawBarGraph();

			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())
				{//-- Align ���� Limit(������ �� �����...1.0)
					sLangChange.LoadStringA(IDS_STRING605);	//ImageCenter NG.  X : %.3lf Y : %.3lf
					logStr.Format(sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
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
							logStr.Format("	AA Step [%d]", iRtnFunction);
							putListLog(logStr);
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
				vision.MilBufferUpdate();
				Sleep(500);
				MIU.func_Set_InspImageCopy(UV_BEFORE_CHART, MIU.m_pFrameRawBuffer);			//AA UV BEFORE
				Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);				// [AA UV BEFORE] #1 MIU.m_pFrameRawBuffer);// 

				MandoInspLog.func_LogSave_UVBefore();	// UV�� Log Data ����

				theApp.MainDlg->autodispDlg->DrawBarGraph();
				if (!Task.sfrResult)// && bMtfCheck2)
				{
					saveInspImage(AA_NG_IMAGE_SAVE, Task.m_iCnt_Step_AA_Total);
					logStr.Format("UV�� SFR �˻� NG.  \nAA RETRY �Ͻðڽ��ϱ�?");//logStr.Format("SFR ���� �˻� NG.  \nAA RETRY �Ͻðڽ��ϱ�?");
					if (askMsg(logStr))
					{
						iRtnFunction = 60000;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
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
				logStr.Format("	AA Step [%d]", iRtnFunction);
				putListLog(logStr);
			}
			else
			{
				logStr.Format("SFR ���� �˻� ����.");
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -65000;
			}
		}
		else
		{//! Active Alignment 1��° ����
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
			if (iRtn > 0)
			{
				if (!theApp.MainDlg->_calcImageAlignment())	//����� //����AA
				{//-- Align ���� Limit(������ �� �����...1.0)
					sLangChange.LoadStringA(IDS_STRING848);	//NG.  X : %.3lf Y : %.3lf
					logStr.Format(sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
					putListLog(logStr);

					iRtnFunction = -65000;

					break;
				}
				else
				{
					if (fabs(Task.m_dShift_IMG_X) <= sysData.m_dOcSpec.x &&
						fabs(Task.m_dShift_IMG_Y) <= sysData.m_dOcSpec.y)
					{//X/Y �������� Spec In�� ��� ���� Step
						sLangChange.LoadStringA(IDS_STRING1302);	//���� Shift. Spec(%.03lf, %.03lf) In : X %.3lf, Y %.3lf
						logStr.Format(_T("		 ") + sLangChange, sysData.m_dOcSpec.x, sysData.m_dOcSpec.y, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 69999;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
						break;
					}
					else
					{
						sLangChange.LoadStringA(IDS_STRING1301);	//���� Shift.  X %.3lf, Y %.3lf
						logStr.Format(_T("		 ") + sLangChange, Task.m_dShift_IMG_X, Task.m_dShift_IMG_Y);
						putListLog(logStr);
						iRtnFunction = 65500;
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
					}
				}
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING1074);	//SFR ���� ȹ�� ����.
				errMsg2(Task.AutoFlag, sLangChange);
				iRtnFunction = -65000;
			}
		}
	}

	//iRtnFunction = -65000;
	break;

	case 65500:
		if (!theApp.MainDlg->_MotorMove_IMG_Align()) {
			iRtnFunction = -65500;
		}
		else
		{
			if (Task.iMTFCnt >= 5)
			{
				if (Task.bFirstAA == true) {
					iRtnFunction = 65520;// 65000;
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
					Task.PCBTaskTime = myTimer(true);
				}
				else
				{
					iRtnFunction = 69999;
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
				}
			}
			else
			{
				Task.iMTFCnt++;
				iRtnFunction = 65520;// 65000;
				logStr.Format("	AA Step [%d]", iRtnFunction);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);
			}
		}
		break;

	case 65520:
		if ((myTimer(true) - Task.PCBTaskTime) > iDelayTime)
		{
			iRtnFunction = 65000;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}

		break;

	case 69000:								// ���� SFR �˻� ��� ����..
	{									// SFR �˻� NG �߻� �� 1ȸ ��˻� �� ��� ����.
		if (myTimer(true) - Task.PCBTaskTime > model.strInfo_Cam[CCD].m_iDelayTime * 2 || (myTimer(true)<Task.PCBTaskTime))
		{
			iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);		/* MTF�� ȹ�� */

			Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
			theApp.MainDlg->autodispDlg->DrawBarGraph();

			if (iRtn>0)
			{
				if (Task.m_b_AA_Retry_Flag == false)
				{
					Task.m_b_AA_Retry_Flag = true;		// TRUE�� ��� ���������� �˻� ��� NG �� �˶� �߻�..
					iRtnFunction = 60000;				// 60000���� 1, 2�� AA �˻� ��� �ʱ�ȭ..
					logStr.Format("	AA Step [%d]", iRtnFunction);
					putListLog(logStr);
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
						logStr.Format("	AA Step [%d]", iRtnFunction);
						putListLog(logStr);
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
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
		else
		{
			//procOptical Aling����...
			//Task.m_timeChecker.Measure_Time(4);
			//Task.m_dTime_ThroughAA = Task.m_timeChecker.m_adTime[4] - Task.m_timeChecker.m_adTime[3];

			iRtnFunction = 80000;
			logStr.Format("	AA Step [%d]", iRtnFunction);
			putListLog(logStr);
		}
	}

	break;
	}
	logStr.Empty();
	return iRtnFunction;
}

int CPcbProcess1::UV_process(int iStep)
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

		iRtnFunction = 110500;
		break;

	case 110500:
		if (theApp.MainDlg->Bonding_Pos_Ckeck())
		{
			sLog.Format("AA after PCB Z : %f , set Offset Z : %f", motor.GetCommandPos(theApp.MainDlg->TITLE_MOTOR_Z), sysData.m_dOffset_Prev_UV_Z);	//AA�� Lens Z : %f , ���� Offset Z : %f
			putListLog(sLog);

			theApp.MainDlg->MoveOffset_Prev_UV();	/* PCB X, Y, Z, Tx, Ty, PCB Th UV �� offset �̵� */

			Sleep(200);
			if (motor.IsStopAxis(theApp.MainDlg->TITLE_MOTOR_Z))// && motor.GetInposition(TITLE_MOTOR_Z))
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

	case 110600:									// 20140612 ���� �߰�.. UV ��ȭ �� SFR ����.
	{

		//	sLangChange.LoadStringA(IDS_STRING864);	//Offset Z ���� �� Lens Z : %f
		//sLog.Format(sLangChange, motor.GetEncoderPos(Motor_Lens_Z));
		//putListLog(sLog);
		Sleep(200);
		saveSfrLog(SFR_OFFSET_DATA);
		theApp.MainDlg->_getMTF(SFR_AFTER_OFFSET_MOVE);
		//saveSfrLog(SFR_OFFSET_DATA);
		iRtnFunction = 110650;
		Task.PCBTaskTime = myTimer(true);
	}
	break;

	case 110650:					//UV�� MTF �˻� ����
	{
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();
		Task.bInsCenter = false;

		iRtnFunction = 110700;

	}
	break;


	case 110700:	// Lens Z  UV �� offset �̵� 
					// 20150730 HJH Offset �ߺ� �������� ����
					//		MoveOffset_Z();	/* Lens Z  UV �� offset �̵� */

		iRtnFunction = 111000;
		Task.PCBTaskTime = myTimer(true);
		break;

	case 111000:	// UV ��ȭ
	{
		sLangChange.LoadStringA(IDS_STRING1122);
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, sLangChange);

		Task.PCBTaskTime = myTimer(true);
		//Task.m_timeChecker.Measure_Time(9);

		if (sysData.m_iUVPass == 1)
		{
			sLog.Format("====UV Pass [%d]====", iStep);
			putListLog(sLog);
			iRtnFunction = 113500;
			break;
		}
		UVCommand.UV_Shutter_Open();
		if (sysData.nUvIndex == 1)
		{
			Sleep(100);
			UVCommand2.UV_Shutter_Open();
		}
		Sleep(10);

		sLog.Format("===> UV ON");
		putListLog(sLog);
		Sleep(model.UV_Time);

		UVCommand.UV_Shutter_Close();
		if (sysData.nUvIndex == 1)
		{
			Sleep(100);
			UVCommand2.UV_Shutter_Close();
		}
		sLog.Format("===> UV OFF [%d]", iStep);

		putListLog(sLog);
		iRtnFunction = 113500;
	}
	break;

	case 113500:
		sLangChange.LoadStringA(IDS_STRING1124);
		theApp.MainDlg->autodispDlg->dispCurTaskOnLabel(1, sLangChange);//"UV ��ȭ �Ϸ�"

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
		curMotorDist[2] = fabs(motor.GetCommandPos(Motor_Lens_Z) - model.axis[Motor_Lens_Z].pos[Bonding_Pos]);

		if ((curMotorDist[0]<model.axis[Motor_Lens_X].m_dLimit_Err && curMotorDist[1]<model.axis[Motor_Lens_Y].m_dLimit_Err && curMotorDist[2]<model.axis[Motor_Lens_Z].m_dLimit_Err))
		{
			Task.PcbOnStage = 200;			// �˻� �Ϸ�.

			iRtnFunction = 115300;

			//if(!Dio.HolderGrip(false, true))				//Lens Gripper open
			//{
			//	sLog.Format("Lens Grip Open ���� [%d]", iStep);
			//	errMsg2(Task.AutoFlag, sLog);
			//	iRtnFunction = -115000;
			//}
			//else
			//{
			//	Task.m_bOKLensPass = -1;//���� �ѱ� �Ϸ� Lamp ���
			//	Task.PCBTaskTime = myTimer(true);
			//	iRtnFunction = 115300;
			//}

		}

		break;

	case 115300:
		if (!Dio.LensMotorGrip(false, true))
		{

			sLog.Format("Lens Unit Gripper ���� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -115300;
			break;
		}
		Task.m_bOKLensPass = -1;//���� �ѱ� �Ϸ� Lamp ���
		Task.m_iFineThCnt = 0;
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 115400;
#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 115500;
#endif
		break;
	case 115400:
		iRtnFunction = 115450;
		/*if(Dio.CamLaserSlinderMove(false, true))
		{
		iRtnFunction = 115450;
		}
		else
		{
		sLog.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iStep);
		errMsg2(Task.AutoFlag,sLog);
		iRtnFunction = -115400;
		}*/
		break;
	case 115450:
		iRtnFunction = 115500;
		//if(Dio.CamLaserSlinderCheck(false, false))
		//{
		//	sLog.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iStep);
		//	putListLog(sLog);
		//	iRtnFunction = 115500;
		//}
		//else if((myTimer(true) - Task.PCBTaskTime) > 10000)//IO_DELAY_TIME)
		//{
		//	sLog.Format("Laser/Cam �Ǹ��� ���� �ð� �ʰ�[%d]", iStep);
		//	errMsg2(Task.AutoFlag,sLog);
		//	iRtnFunction = -115450;
		//}
		break;
	case 115500: //UV�Ϸ��� �����˻�
		Task.m_bOkFlag = 0;
		work.m_iCnt_Output++;
		work.Save();
		iRtnFunction = 120000;
		theApp.MainDlg->_getMTF(SFR_AFTER_UV);
		break;


	default:
		sLangChange.LoadStringA(IDS_STRING1120);	//UV Process Step ��ȣ�� �������Դϴ�.
		sLog.Format(sLangChange + _T(" [%d]"), iStep);
		errMsg2(Task.AutoFlag, sLog);
		break;
	}
	sLog.Empty();
	sImgPath.Empty();
	return iRtnFunction;
}

int	CPcbProcess1::func_MandoFinalSFR(int iStep)
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

		iRtnFunction = 121000;
		break;

	case 121000:

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
		MIU.func_Set_InspImageCopy(UV_AFTER_CHART, MIU.m_pFrameRawBuffer);		//AA UV AFTER
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
	case 122550:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
			break;
		}

		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING747);	//Lens�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122550;
		}



		iRtnFunction = 122700;
		break;
	case 122700:
		Task.LensTaskTime = myTimer(true);
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 122800;
		break;
	case 122800:
		if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
			iRtnFunction = 123000;
		}
		else
		{
			//iRtnFunction = 122850;
		}
		break;
	case 122850:
		/*if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122850;
		}
		else
		{
			sLog.Format("PCB�� Dark ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 122900;
		}*/

		break;
	case 122900:
		/*if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark Z ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -122900;
			break;
		}*/
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
		Sleep(100);
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);
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
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122970;
			break;
		}
		iRtnFunction = 123000;
		break;
	case 123000:
		if (sysData.m_iDefectInspPass == 1 && sysData.m_iStaintInspPass == 1)
		{
			iRtnFunction = 124500;
			break;
		}
		if (sysData.m_iIrChartUse == 1)
		{
			IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_6500K], IR_OC);
		}
		else
		{
#ifdef FURONTEER_OC
			OCControl.SendLightLxStep(model.m_iLedValue[LEDDATA_6500K]);
#else
			LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, model.m_iLedValue[LEDDATA_6500K]);
#endif

		}
		iRtnFunction = 123050;

		break;
	case 123050:
		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING1023);	//PCB�� ���� Defect �˻� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123050;
		}
		else
		{
			iRtnFunction = 123100;
		}
		break;
	case 123100:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123200;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
		}
		break;
	case 123200:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
		}
		break;
	case 123400:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 124000;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z 6500K ����Ȯ�� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123100;
		}
		break;
	case 124000:
		Sleep(dFinalDelay);
		if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
		}
		else
		{
			if (g_clApsInsp.func_Insp_Defect(MIU.vDefectMidBuffer_6500K, MIU.vDefectLowBuffer, true) == true)	//AA
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
			sLog.Format(_T("[PASS] Stain Insp"), iStep);
			putListLog(sLog);

		}
		else
		{
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

		if (MIU.Close())//if(MIU.Stop())
		{
			iRtnFunction = 125000;
		}
		else
		{
			iRtnFunction = 124500;

			if (Task.m_iRetry_Opt>3)
			{
				sLog.Format("CCD ���� Close ���� [%d]", iStep);	//CCD ���� Close ����[%d]
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -124500;
			}
			else {
				Task.m_iRetry_Opt++;
			}
		}
		break;
	case 125000:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127000;
		}
		else
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -125000;
		}
		break;
	case 127000:
		//MIU.m_acsInspImage.Unlock();
		Task.m_iRetry_Opt = 0;
		MandoInspLog.func_LogSave_UVAfter();	//���� ������Camera �˻� Log ����(�˻�/������ ���� -> ����Log)
												/*inspMES();
												MESCommunication.Messave();*/
												//! �˻� Count ó��
		work.iInspCnt_Output++;
		if (MandoInspLog.bInspRes == false)	work.iInspCnt_NG_Output++;
		theApp.MainDlg->dispInspResGrid();
		work.Save();
		work.Load();

		Task.m_timeChecker.Measure_Time(13);	//Final �˻� �Ϸ� �ð�
		Task.m_dTime_TotalAA = Task.m_timeChecker.m_adTime[13] - Task.m_timeChecker.m_adTime[12];
		theApp.MainDlg->dispGrid();

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
		iRtnFunction = 127500;
		break;
	case 127500:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING987);	//PCB ��� ��ġ �̵� �Ϸ� [%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = 127900;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING986);	//PCB ��� ��ġ �̵� ���� [%d]
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127500;
		}
		break;
	case 127900:

		//////Dio.PcbGrip(false, false);   //�������?

		iRtnFunction = 128000;
		break;
	case 128000:
		if (!motor.Lens_Motor_MoveXY(0, Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING748);	//Lens�� ���� ��� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
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
		if (motor.Lens_Tilt_Motor_Move(Wait_Pos))
		{
			iRtnFunction = 128500;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING745);	//LENS�� Tx, Ty�� ��� ��ġ �̵� ����
			sLog.Format(sLangChange);
			putListLog(sLog);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128200;
		}
		break;
	case 128500:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129000;
		}
		else
		{
			sLog.Format("PCB TX,TY �����ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -128500;
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
		if (true)
		{
			memset(Task.ChipID, 0x00, 256);
			sprintf_s(Task.ChipID, "EMPTY");
			theApp.MainDlg->func_ChipID_Draw();
		}

		Task.m_timeChecker.Measure_Time(14);	//����ǰ ���� �ð�
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);

		//Dio.PCBvaccumOn(VACCUM_OFF, false);


		//////////////////////////////////////////////////////////////////////////////////////////////////////
		Task.PCBTask = 10000;
		Task.LensTask = 10000;
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

int CPcbProcess1::RunProc_LensNewPassPickup(int iUseStep)
{//! Lens Pickup�� ��ǰ �ѱ� Step	(15000 ~ 16600)
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
		if (!motor.Lens_Motor_MoveXY(0, Loading_Pos))
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
		iRtnFunction = 15450;
		//Sleep(400);	//�Ǹ��� ������� PCB Stage UnGrip�� ���� ���� ��, Delay
		/*if(Dio.HolderGrip(false,false))
		{
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 15450;
		}
		else
		{
		logStr.Format("Lens Gripper ���� ����..[%d]", iUseStep);
		errMsg2(Task.AutoFlag,logStr);

		iRtnFunction = -15400;
		}*/
		break;

	case 15450:
		iRtnFunction = 15452;
		/*if(Dio.HolderGripCheck(false, false) )
		{
		iRtnFunction = 15452;
		}
		else if((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
		logStr.Format("PCB Stage Lens Gripper ���� ����..[%d]", iUseStep);
		errMsg2(Task.AutoFlag,logStr);

		iRtnFunction = -15450;
		}*/
		//Sleep(600);
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
		}
		//
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -15500;
		}
		iRtnFunction = 15550;
		break;
	case 15550:
		if (motor.Pcb_Motor_Move(Wait_Pos))			//PCB�� ���� �����ġ �̵�
		{
			iRtnFunction = 15800;
			logStr.Format("Pcb x,y ��� ��ġ �̵� �Ϸ� [%d]", iUseStep);
			putListLog(logStr);
		}
		else
		{
			logStr.Format("Pcb x,y ��� ��ġ �̵� ���� [%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -15550;
		}
		break;
	case 15800:

		Task.LensTask = 30000;	//pcb AA�϶� lens �� Align �̵��� laser
		iRtnFunction = 15900;
		logStr.Format("PCB Epoxy Waiting[%d]", iUseStep);
		putListLog(logStr);
		break;
	case 15900:
		//pcb �� 
		//lens�� Align , laser��ġ�� 
		//������ġ�� ������ ������ ��� ��ġ�ΰ��� ����ؾߵȴ�.

		if (Task.interlockLens == 1)
		{
			Task.interlockLens = 0;
			iRtnFunction = 26000;
		}
		break;
	default:
		logStr.Format("Lens ���� ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int	CPcbProcess1::RunProc_LensAlign(int iUseStep)			//(35000 ~ 39000)
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
		iRtnFunction = 35050;
		break;
	case 35050:
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
		}
		else
		{
			logStr.Format("Lens �ѱ� �Ϸ�� Lens Align Pass![%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 35400;
		}
		break;

	case 35150:
		theApp.MainDlg->ctrlSubDlg(MAIN_DLG);
		theApp.MainDlg->m_iCurCamNo = 0;
		//SetDigReference(LENS_Align_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);		// Align�� ���� ON
		if (motor.Pcb_Motor_Move(Lens_Pos))
		{
			if (motor.PCB_Z_Motor_Move(Lens_Pos))
			{
				logStr.Format("PCB Lens_Pos Align ��ġ �̵� �Ϸ�[%d]", iUseStep);
				putListLog(logStr);
				Task.PCBTaskTime = myTimer(true);

				iRtnFunction = 35200;
			}
		}
		else
		{
			logStr.Format("PCB Sensor Align ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35150;
		}
		break;
	case 35200:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 35250;
		}
		break;

	case 35250:

		if (Task.m_iRetry_Opt>iAlignRetry)
		{
			logStr.Format("PCB Align ��˻� %d ȸ ����[%d]\n ���� ���������� �����Ͻðڽ��ϱ�?", Task.m_iRetry_Opt, iUseStep);
			if (askMsg(logStr) == IDOK)
			{
				Task.d_Align_offset_x[LENS_Align_MARK] = 0;
				Task.d_Align_offset_y[LENS_Align_MARK] = 0;
				Task.d_Align_offset_th[LENS_Align_MARK] = 0;
				iRtnFunction = 35400;
			}
			else
			{
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 35250;
				logStr.Format("PCB Lens Align ��˻� ����[%d]", iUseStep);
				putListLog(logStr);

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
			Task.d_Align_offset_th[LENS_Align_MARK] = 0.0;//offsetTh;

			Task.dPcbAlignTh = Task.d_Align_offset_th[LENS_Align_MARK];

			logStr.Format("PCB Lens Align : %.3lf %.3lf %.3lf", Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
			//pcbDlg->m_labelAlignResult.SetText(logStr);
			//pcbDlg->m_labelAlignResult.Invalidate();

			putListLog(logStr);

			int iRtnVal = theApp.MainDlg->AlignLimitCheck(CAM1, offsetX, offsetY, offsetTh);

			if (iRtnVal == 1)
			{
				iRtnFunction = 35300;				// ���� �̵�
			}
			else if (iRtnVal == 2)
			{
				logStr.Format("		PCB Lens Align [%d] �Ϸ� [%d]", Task.m_iRetry_Opt, iUseStep);
				putListLog(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);			//�ӽ� ��� ��ũ ����

				logStr.Format("		PCB Lens Align Complete : %d, %.03f, %.03f, %.03f", Task.m_iPcbPickupNo, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;

				Task.m_timeChecker.Measure_Time(4);	//Laser ���� ���� ���� �ð�

				iRtnFunction = 35400;		// �Ϸ�
			}
			else
			{
				logStr.Format("PCB Lens Align [%d] ���� ���� �ʰ�", Task.m_iRetry_Opt, iUseStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				//InstantMarkDelete(PCB_Chip_MARK);
				iRtnFunction = -35250;				// NG
			}
		}
		else
		{
			logStr.Format("PCB Lens Align [%d] ��ũ �ν� ����[%d]", Task.m_iRetry_Opt, iUseStep);
			putListLog(logStr);

			iRtnFunction = 35200;				//  ��˻� 
			Sleep(300);
		}
		break;

	case 35300://- Sensor Align Retry
		if (motor.Pcb_Holder_Align_Move(Lens_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 35200;
		}
		else
		{
			logStr.Format("PCB Sensor Align ���� ��ġ �̵� ����[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -35300;
		}
		break;
	case 35400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -35400;
			break;
		}
		iRtnFunction = 35500;
		break;
	case 35500://! Lens-Z�� ��� ��ġ �̵�
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

int CPcbProcess1::RunProc_LensLoading(int iUseStep)
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
	case 40000:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens_Z�� ����ġ �̵� ����");
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40000;
			break;
		}

		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB Z�� �����ġ �̵� ����[%d]", iUseStep);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40000;
			break;
		}

		if (Task.m_bOKLensPass != 1)//! Lens �ѱ� �̿Ϸ� �� ���	 
		{

			Task.d_Align_offset_x[LENS_Align_MARK] = 0.0;
			Task.d_Align_offset_y[LENS_Align_MARK] = 0.0;
			if (!motor.Pcb_Motor_Move(Loading_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], 0))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -40000;
				break;
			}
			//M_TRINITY ���� �������� ƿƮ ���ؼ� ������ġ���� �ٽ� ������ ƿƮ�� tx,ty ������ �̵� 
			if (!motor.Pcb_Tilt_Motor_Move(Loading_Pos))
			{
				logStr.Format("PCB %s �̵� ����", MotorPosName[Loading_Pos]);
				delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
				iRtnFunction = -40000;
				break;
			}
			iRtnFunction = 40100;
		}
		else
		{
			iRtnFunction = 40500;
		}
		break;
	case 40100:
		if (!Dio.LensMotorGrip(false, false))
		{
			logStr.Format("Lens Unit Gripper ���� ����[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = -40100;
			break;
		}
		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			Dio.LensTailGrip(false, false);
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 40150;
		break;

	case 40150:
		if (Dio.LensMotorGripCheck(false, false)) 
		{
			iRtnFunction = 40170;
			Task.PCBTaskTime = myTimer(true);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens UnGrip Ȯ�� �ð� �ʰ�");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40150;
			break;
		}
		break;
	case 40170:
		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			if (Dio.LensTailGripCheck(false, false))
			{
				iRtnFunction = 40200;
				Task.PCBTaskTime = myTimer(true);
			}
			else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
			{
				logStr.Format("Lens Tail UnGrip Ȯ�� �ð� �ʰ�");
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -40170;
				break;
			}
		}

		else
		{
			iRtnFunction = 40200;
		}
		
		break;

	case 40200: // ���� 
		if (!motor.Lens_Tilt_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens %s Tx,Ty�̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40200;
			break;
		}
		if (!motor.Lens_Motor_MoveXY(0, Loading_Pos))
		{
			logStr.Format("Lens %s X,Y �̵� ����", MotorPosName[Loading_Pos]);
			delayMsg(logStr.GetBuffer(99), 3000, M_COLOR_RED);
			iRtnFunction = -40200;
			break;
		}

		logStr.Format("Lens ���� ��ġ X,Y �̵� �Ϸ�[%d]", iUseStep);
		putListLog(logStr);
		iRtnFunction = 40250;
		break;

	case 40250:
		if (!motor.PCB_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("PCB loading Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40250;
			break;
		}
		if (!motor.LENS_Z_Motor_Move(Loading_Pos))
		{
			logStr.Format("Lens Z ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40250;
			break;
		}
		iRtnFunction = 40252;
		break;
	case 40252:
		Dio.LensMotorGrip(true, false);
		Sleep(500);
		iRtnFunction = 40255;
		break;

	case 40255:
		Dio.LensMotorGrip(false, false);
		Sleep(300);
		iRtnFunction = 40300;
		break;
	case 40300: // ���� �׸�
		if (Dio.LensMotorGrip(true, false))
		{
			Task.m_bOKLensPass = 1;
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 40350;
		}
		else
		{
			Task.m_bOKLensPass = -1;
			logStr.Format("Lens �׸� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40300;
		}
		break;

	case 40350://���� �׸� Ȯ��
		if (Dio.LensMotorGripCheck(true, false))
		{
			logStr.Format("Lens �׸� �Ϸ�[%d]", iUseStep);
			putListLog(logStr);
			iRtnFunction = 40400;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("Lens Gripper ���� ���� �ð� �ʰ�..[%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40350;
		}
		break;

	case 40400:
		Sleep(500);	//�Ǹ��� ������� PCB Stage UnGrip�� ���� ���� ��, Delay 0.2s
		iRtnFunction = 40455;
		break;

	case 40455:
		iRtnFunction = 40500;
		break;
	case 40500: // ��� ��ġ �̵�
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("Lens Z ��� ��ġ �̵� ���� [%d]", iUseStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40500;
			break;
		}
		//
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			logStr.Format("PCB waiting Z �̵� ����");
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40500;
			break;
		}
		iRtnFunction = 40550;
		break;
	case 40550:

		iRtnFunction = 40600;
		break;
	case 40600:
		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			Dio.LensTailGrip(true, false);
		}
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 40650;
		break;
	case 40650:
		if (LGIT_MODEL_INDEX == M1_HEATING_GEN2)
		{
			if (Dio.LensTailGripCheck(true, false))
			{
				iRtnFunction = 40700;

			}
			else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
			{
				logStr.Format("Lens Tail Grip Ȯ�� �ð� �ʰ�");
				errMsg2(Task.AutoFlag, logStr);
				iRtnFunction = -40650;
				break;
			}

		}
		else
		{
			iRtnFunction = 40700;
		}
		break;
	case 40700:

		iRtnFunction = 40750;
		break;
	case 40750:

		iRtnFunction = 40900;
		break;
	case 40900:
		iRtnFunction = 41000;
		break;
	default:
		logStr.Format("Lens ���� ���� Thread Step ��ȣ ������ . [%d]", iUseStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}

int CPcbProcess1::procProductComplete(int iStep)
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
		Sleep(1000);
		Task.oldLaserTx = 0.1;
		Task.oldLaserTy = 0.1;
		Task.PCBTaskTime = myTimer(true);
		
		bThreadCCDRun1 = true;
		if (!theApp.MainDlg->MIUCheck_process() || gMIUDevice.CurrentState != 4)
		{
			sLangChange.LoadStringA(IDS_STRING452);	//CCD ��� ���� �ʱ�ȭ ����.[%d]\n ��ǰ ���� ���� �� ��ǰ �ҷ� Ȯ�� �ϼ���.
			logStr.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -11000;
			break;
		}
		bThreadCCDRun1 = false;

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
		Task.d_Align_offset_x[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_y[PCB_Chip_MARK] = 0;
		Task.d_Align_offset_th[PCB_Chip_MARK] = 0;

		if (sysData.m_iAlignCamInspPass == 1)		//procProductComplete
		{
			logStr.Format("����ǰ Align Pass[%d]", iStep);
			putListLog(logStr);
			iRtnFunction = 12800;
			break;
		}
		iRtnFunction = 12100;
		break;

	case 12100:

		Task.m_iRetry_Opt = 0;
		//SetDigReference(PCB_Holder_MARK);
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_HOLDER]);		// Align�� ���� ON
		
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
				Task.d_Align_offset_x[PCB_Chip_MARK] -= 0;
				Task.d_Align_offset_y[PCB_Chip_MARK] -= 0;
				Task.d_Align_offset_th[PCB_Chip_MARK] += 0;
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
		//LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);		//������ �˻��� off
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iStep);
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
#if (____AA_WAY == LENS_TILT_AA) 
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
																		  //
			logStr.Format("PCB ȭ��˻� Chart Pos X(%.3lf), Y(%.3lf)", Task.dAAPcbMotor[0], Task.dAAPcbMotor[1]);
			putListLog(logStr);
			//
			logStr.Format("PCB ȭ��˻� TX(%.3lf), TY(%.3lf)", Task.dAAPcbMotor[3], Task.dAAPcbMotor[4]);
			putListLog(logStr);

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


#else
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
			iRtnFunction = 13300;		//����ǰ�� ������ �н�
			break;
		}


		if (motor.Pcb_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[PCB_Holder_MARK], Task.d_Align_offset_y[PCB_Holder_MARK], Task.d_Align_offset_th[PCB_Holder_MARK]))
		{
			Task.PCBTaskTime = myTimer(true);
			if (motor.PCB_Z_Motor_Move(Com_Laser_Pos))//Laser_Lens_Pos))
			{
				Task.PCBTaskTime = myTimer(true);
				logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iStep);
				putListLog(logStr);
				iRtnFunction = 12910;// 12950;
			}
			else {
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
		if (iLaser_Pos > 0)
		{
			iRtnFunction = 12950;
			break;
		}
		if (Dio.LaserCylinderUPDOWN(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 12920;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� �ϰ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12910;
		}
		break;

	case 12920:
		if (Dio.LaserCylinderCheck(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 12950;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -12920;
		}
		break;
		//laser�Ǹ��� �ϰ� END
	case 12950:
		checkMessage();
		if ((myTimer(true) - Task.PCBTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_PCB_X) && motor.IsStopAxis(Motor_PCB_Y)))
		{
			iRtnFunction = 13100;
		}
		break;
	case 13100:
		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
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
					int Rnd = theApp.MainDlg->TiltAlignLimitCheck(Task.d_Align_offset_xt[0], Task.d_Align_offset_yt[0]);
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
		if (Dio.LaserCylinderUPDOWN(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 13310;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� ��� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13300;
		}
		break;
	case 13310:
		if (Dio.LaserCylinderCheck(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 13350;
			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� ��� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -13310;
		}
		break;
		//laser�Ǹ��� ��� END
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
		iRtnFunction = 19600;
		break;

	case 19600: // ���� ��ġ �̵� 
				//if(motor.Pcb_Motor_Move(Bonding_Pos, Task.d_Align_offset_x[PCB_Holder_MARK] + Task.dAlignManual[0], Task.d_Align_offset_y[PCB_Holder_MARK] + Task.dAlignManual[1], Task.d_Align_offset_th[PCB_Holder_MARK] + Task.dAlignManual[2]) )
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
#if (____AA_WAY == LENS_TILT_AA)
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
#else
		iRtnFunction = 19800;
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
			if (sysData.m_iIrChartUse == 1)
			{
				IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_TOP1_CHART], IR_CHART);
			}
			else
			{
				LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
				LightControl.ctrlLedVolume(LIGHT_TOP_CHART2, model.m_iLedValue[LEDDATA_TOP2_CHART]);
				LightControl.ctrlLedVolume(LIGHT_TOP_CHART3, model.m_iLedValue[LEDDATA_TOP3_CHART]);
				LightControl.ctrlLedVolume(LIGHT_TOP_CHART4, model.m_iLedValue[LEDDATA_TOP4_CHART]);
			}


			LightControl.ctrlLedVolume(LIGHT_TOP_CHART1, model.m_iLedValue[LEDDATA_TOP1_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART2, model.m_iLedValue[LEDDATA_TOP2_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART3, model.m_iLedValue[LEDDATA_TOP3_CHART]);
			LightControl.ctrlLedVolume(LIGHT_TOP_CHART4, model.m_iLedValue[LEDDATA_TOP4_CHART]);
			LightControl.ctrlLedVolume(LIGHT_LEFT_CHART, model.m_iLedValue[LEDDATA_CHART_L]);
			LightControl.ctrlLedVolume(LIGHT_RIGHT_CHART, model.m_iLedValue[LEDDATA_CHART_R]);



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
		sLangChange.LoadStringA(IDS_STRING1004);	//PCB ����ǰ �˻� Thread Step ��ȣ ������ . [%d]
		logStr.Format(sLangChange, iStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -20000;
		break;
	}

	logStr.Empty();
	sSocketMsg.Empty();
	sBarCode.Empty();
	return iRtnFunction;
}

int	CPcbProcess1::Complete_FinalInsp(int iStep)
{
	CString sLog = _T("");
	//CString sTemp = _T("");
	//CString sNgMsg = _T("");
	//CString sImgPath = _T("");
	//CString logStr = _T("");
	double	dFinalDelay = (double)(model.strInfo_Cam[3].m_iDelayTime);
	double offsetX = 0.0, offsetY = 0.0, offsetTh = 0.0;
	int iRtn = 0;
	int iRtnFunction = iStep;
	bool bMtfCheck4 = true;
	switch (iStep)
	{
	case 120000:
		theApp.MainDlg->setCamDisplay(3, 1);
		Sleep(dFinalDelay);

		/*sLog.Format("Gen2 Current:%.03lf", model.dGen2Current);
		putListLog(sLog);
		g_clApsInsp.func_insp_CDP800_Current();*/

		iRtnFunction = 121000;
		break;
	case 121000:

		iRtnFunction = 122000;
		break;

	case 122000:	//����ǰ �˻縸 �ش� (Final �˻��� X/Y/T ����)
		Sleep(dFinalDelay);
		iRtn = theApp.MainDlg->_getMTF(SFR_FINAL);
		Sleep(500);
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
		MIU.func_Set_InspImageCopy(EOL_CHART, MIU.m_pFrameRawBuffer);	//����ǰ ���
		Sleep(300);
		iRtnFunction = 122520;
		break;
	case 122520:
		Task.bInsCenter = true;

		theApp.MainDlg->_calcImageAlignment();

		Task.bInsCenter = false;

		Task.sfrResult = theApp.MainDlg->func_MTF(MIU.vChartBuffet);//EOL ȭ�� #1

		if (Task.sfrResult == true)
		{
			sLangChange.LoadStringA(IDS_STRING229);
			putListLog("[�˻�] MTF �˻� ����.");// Optic_Axis,
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING228);
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
		}
		Task.m_iDrawBarStep = Task.m_iCnt_Step_AA_Total;
		theApp.MainDlg->autodispDlg->DrawBarGraph();
		iRtnFunction = 122550;
		break;
	case 122550:
		vision.clearOverlay(CCD);
		vision.drawOverlay(CCD, true);

		iRtnFunction = 122600;
		break;
	case 122600:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -122600;
			break;
		}
		iRtnFunction = 122700;
		break;
	case 122700:
		if (!motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING747);	//Lens�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
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
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
			putListLog(sLog);
			iRtnFunction = 123400;
		}
		else
		{
			//iRtnFunction = 123100;
		}
		break;
	case 123100:
		/*if (!motor.Pcb_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123100;
		}
		else
		{
			sLog.Format("PCB�� Dark ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 123200;
		}*/
		break;
	case 123200:
		/*if (!motor.PCB_Z_Motor_Move(Dark_Pos))
		{
			sLog.Format("PCB�� Dark Z ��ġ �̵� ����[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = -123200;
			break;
		}*/
		Task.PCBTaskTime = myTimer(true);
		iRtnFunction = 123300;
		break;
	case 123300:
		Sleep(100);
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Sleep(100);
			MIU.func_Set_InspImageCopy(LOW_LEVEL_RAW, MIU.m_pFrameRawBuffer);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123400;
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > 10000)
		{
			sLog.Format("PCB Z DAR ����Ȯ�� �ð� �ʰ�[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123200;
		}
		break;
	case 123400:
		if (!motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING305);	//PCB�� z�� ���� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
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
		if (sysData.m_iIrChartUse == 1)
		{
			IrvAlignLed.Irv_Light_SetValue(model.m_iLedValue[LEDDATA_6500K], IR_OC);
		}
		else
		{
#ifdef FURONTEER_OC
			OCControl.SendLightLxStep(model.m_iLedValue[LEDDATA_6500K]);
#else
			LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, model.m_iLedValue[LEDDATA_6500K]);
#endif
			//LightControlthird.ctrlLedVolume(LIGHT_OC_6500K, model.m_iLedValue[LEDDATA_6500K]);
		}

		iRtnFunction = 123600;
		break;
	case 123600:

		if (!motor.Pcb_Motor_Move(OC_6500K_Pos, Task.d_Align_offset_x[PCB_Chip_MARK], Task.d_Align_offset_y[PCB_Chip_MARK], Task.d_Align_offset_th[PCB_Chip_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING1023);	//PCB�� ���� Defect �˻� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -123600;
		}
		else
		{
			iRtnFunction = 123700;
		}
		break;
	case 123700:
		if (motor.PCB_Z_Motor_Move(OC_6500K_Pos))
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Complete[%d]", iStep);
			putListLog(sLog);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 123800;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
		}
		break;
	case 123800:
		if ((myTimer(true) - Task.PCBTaskTime) > 500)
		{
			iRtnFunction = 123850;
		}
		break;
	case 123850:
		if (motor.IsStopAxis(Motor_PCB_Z))
		{
			Task.PCBTaskTime = myTimer(true);

			//6500k �̹��� ����
			Sleep(300);
			MIU.func_Set_InspImageCopy(MID_6500K_RAW, MIU.m_pFrameRawBuffer);
			//ccdDlg->m_pSFRDlg->Raw_ImageSave(MIU.m_pFrameRawBuffer, MID_6500K_RAW);
			//MID_6500K_RAW �̹��� ����

			iRtnFunction = 123900;
		}
		else
		{
			sLog.Format("PCB Z Axis OC 6500K Pos Move Check Fail[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -123700;
		}
		break;
	case 123900:
		Sleep(dFinalDelay);
		if (sysData.m_iDefectInspPass == 1)	// �̹��˻� ����
		{
			sLog.Format(_T("[PASS] Defect Insp"), iStep);
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
			sLog.Format(_T("[PASS] Stain Insp"), iStep);
			putListLog(sLog);

		}
		else
		{
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
		iRtnFunction = 127000;
		break;
	case 127000:
		if (motor.PCB_Z_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� �Ϸ�[%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 127600;
		}
		else
		{
			sLog.Format("PCB Z�� ��� ��ġ �̵� ����[%d]", iStep);
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -127000;
		}
		break;

	case 127600:
		Task.m_iRetry_Opt = 0;
		iRtnFunction = 127700;
		break;
	case 127700:
		//
		if (MIU.Close())
		{
			iRtnFunction = 127800;
		}
		else
		{
			iRtnFunction = 127700;

			if (Task.m_iRetry_Opt>3)
			{
				sLog.Format("CCD ���� Close ���� [%d]", iStep);	//CCD ���� Close ����[%d]
				errMsg2(Task.AutoFlag, sLog);
				iRtnFunction = -127700;
			}
			else {
				Task.m_iRetry_Opt++;
			}
		}
		break;
	case 127800:

		iRtnFunction = 127900;
		break;
	case 127900:
		//MIU.m_acsInspImage.Unlock();
		Task.m_iRetry_Opt = 0;
		MandoInspLog.func_LogSave_UVAfter();	//���� ������Camera �˻� Log ����(�˻�/������ ���� -> ����Log)
												//! �˻� Count ó��
		g_FinalInspLog();
		//inspMES();
		//MESCommunication.Messave();
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
		if (!motor.Lens_Motor_MoveXY(0, Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING748);	//Lens�� ���� ��� ��ġ �̵� ����[%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = -129200;
		}
		else
		{
			iRtnFunction = 129220;
		}
		break;

	case 129220:

		iRtnFunction = 129300;
		break;
	case 129300:
		if (motor.Pcb_Motor_Move(Wait_Pos))
		{
			sLangChange.LoadStringA(IDS_STRING987);	//PCB ��� ��ġ �̵� �Ϸ� [%d]
			sLog.Format(sLangChange, iStep);
			putListLog(sLog);
			iRtnFunction = 129350;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING986);	//PCB ��� ��ġ �̵� ���� [%d]
			sLog.Format(sLangChange, iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129300;
		}

		break;
	case 129350:
		iRtnFunction = 129400;
		break;
	case 129400:
		if (motor.Pcb_Tilt_Motor_Move(Wait_Pos))
		{
			sLog.Format("PCB TX,TY ��� ��ġ �̵� �Ϸ� [%d]", iStep);
			putListLog(sLog);
			iRtnFunction = 129500;
		}
		else
		{
			sLog.Format("PCB TX,TY �����ġ �̵� ����[%d]", iStep);
			errMsg2(Task.AutoFlag, sLog);
			iRtnFunction = -129400;
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
		if (true)
		{
			memset(Task.ChipID, 0x00, 256);
			sprintf_s(Task.ChipID, "EMPTY");
			theApp.MainDlg->func_ChipID_Draw();
		}

		Task.m_timeChecker.Measure_Time(14);	//����ǰ ���� �ð�
		Task.m_dTime_Total = Task.m_timeChecker.m_adTime[14] - Task.m_timeChecker.m_adTime[1];

		theApp.MainDlg->dispGrid();
		theApp.MainDlg->EpoxyTimeCheck(theApp.MainDlg->bEpoxyTimeChk);
		//////////////////////////////////////////////////////////////////////////////////////////////////////
		Task.PCBTask = 10000;
		Task.LensTask = 10000;
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


int CPcbProcess1::RunProc_LENS_AlignLaserMeasure(int iLensStep)
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
			iRtnFunction = 30100;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -30000;
		}

		break;
	case 30100:
#if (____AA_WAY == PCB_TILT_AA)
		iRtnFunction = 30500;
#elif (____AA_WAY == LENS_TILT_AA)
		if (motor.Lens_Motor_MoveXY(0, Wait_Pos))
		{
			iRtnFunction = 43000;//41900;		//������ ī�޶� ����
		}
		else
		{
			logStr.Format("Lens �����ġ �̵� ����[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = -30050;
		}
		break;

#endif

		break;
	case 30500:
		iRtnFunction = 30600;

		break;
	case 30600:
		iRtnFunction = 30900;

		break;
	case 30900:
#if (____AA_WAY == PCB_TILT_AA)
		if (sysData.m_iLaserInspPass == 1)	//����
		{
			iRtnFunction = 41100;			//Laser ���� Pass
		}
		else
		{
			iRtnFunction = 31000;		// 31300;//����� �ӽ� �н�
		}

#elif (____AA_WAY == LENS_TILT_AA)
		iRtnFunction = 43000;//41900;		//������ ī�޶� ����
#endif

		break;
	case 31000:
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 31050;
		}
		else
		{
			logStr.Format("Lens_Z�� �����ġ �̵� ����.[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31000;
		}



		break;

	case 31050:
		LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);
		if (motor.Lens_Motor_MoveXY(0, Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Lens Align ��ġ �̵� �Ϸ�[%d]", iLensStep);
			putListLog(logStr);
			iRtnFunction = 31060;
		}
		else
		{
			logStr.Format("Lens Align ��ġ �̵� ����[%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31050;
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
		checkMessage();
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
	case 31250:
		LightControlthird.ctrlLedVolume(LIGHT_PCB, 0);
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
		if (motor.Lens_Motor_Move_Laser(iLaser_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]))
		{
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iLensStep);	//Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]
			putListLog(logStr);
			iRtnFunction = 40000;
		}
		else
		{
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -31300;
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
			Task.LensTaskTime = myTimer(true);
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]", iLensStep);	//Laser ���� ���� �ܺ� ��ġ �̵� �Ϸ�[%d]
			putListLog(logStr);
			if (iLaser_Pos == 0) { iRtnFunction = 40210; }//ó���� �ϰ����.
			else { iRtnFunction = 40300; }
		}
		else
		{
			logStr.Format("Laser ���� ���� �ܺ� ��ġ �̵� ���� [%d]", iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40200;
		}
		break;

		//Laser �Ǹ��� �ϰ�
	case 40210:
		if (Dio.LaserCylinderUPDOWN(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 40220;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� �ϰ� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40210;
		}
		break;

	case 40220:
		if (Dio.LaserCylinderCheck(false, false))
		{
			logStr.Format("LASER �Ǹ��� �ϰ� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 40300;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� �ϰ� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -40220;
		}
		break;
	case 40300://! ���� �� Delay
		checkMessage();
		if ((myTimer(true) - Task.LensTaskTime) > iLaserDelay && (motor.IsStopAxis(Motor_Lens_X) && motor.IsStopAxis(Motor_Lens_Y)))	//�����Ӵ�, Delay Ȯ��
		{
			iRtnFunction = 41000;
		}
		break;
		//Laser �Ǹ��� �ϰ� END

	case 41000: //�ܺ� ���� ���� (���� ���� �ϰ� Align �̵�)
				//Keyence.func_LT9030_Scan(Task.m_Laser_Point[iLaser_Pos]);
		Keyence.func_CL3000_Scan(Task.m_Laser_Point[iLaser_Pos]);
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
					sLangChange.LoadStringA(IDS_STRING639);	//Laser ������ �̻�..�ܺ� ���� �������� �̻�..[%d]\n ������:%.04lf, %.04lf, %.04lf, %.04lf
					logStr.Format(sLangChange, iLensStep, LaserValue[0], LaserValue[1], LaserValue[2], LaserValue[3]);
					errMsg2(Task.AutoFlag, logStr);
					iRtnFunction = -41000;
					break;
				}

				if (theApp.MainDlg->_calcLaserTilt(LaserPos, LaserValue, Task.d_Align_offset_xt[LENS_Align_MARK], Task.d_Align_offset_yt[LENS_Align_MARK]))
				{
					Task.m_dataOffset_x[LENS_Align_MARK] = Task.d_Align_offset_xt[LENS_Align_MARK];
					Task.m_dataOffset_y[LENS_Align_MARK] = Task.d_Align_offset_yt[LENS_Align_MARK];


					sLangChange.LoadStringA(IDS_STRING1264);		//������ Tx: %.04lf, Ty: %.04lf
					logStr.Format(_T("		") + sLangChange, Task.d_Align_offset_xt[LENS_Align_MARK], Task.d_Align_offset_yt[LENS_Align_MARK]);
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
							sLangChange.LoadStringA(IDS_STRING627);	//Laser ���� �� �� �̻� �߻�..
							logStr.Format(sLangChange + _T("Xt : %lf, Yt : %lf"), Task.m_dataOffset_x[LENS_Align_MARK], Task.m_dataOffset_y[LENS_Align_MARK]);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -41000;
							break;
						}
						if (!g_ADOData.func_AATaskToRecordLaser(Task.ChipID, Task.m_dataOffset_x[LENS_Align_MARK], Task.m_dataOffset_y[LENS_Align_MARK], Task.m_Laser_Point))
						{
							sLangChange.LoadStringA(IDS_STRING489);	//DataBase Laser ���� ���� Data ��� ����.[%d]\n MS Office�� �ݾ��ּ���.
							logStr.Format(sLangChange, iLensStep);
							errMsg2(Task.AutoFlag, logStr);
							iRtnFunction = -41000;
							break;
						}

						LightControlthird.ctrlLedVolume(LIGHT_PCB, model.m_iLedValue[LEDDATA_LENS]);		// Align�� ���� ON
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
		if (Dio.LaserCylinderUPDOWN(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			Task.PCBTaskTime = myTimer(true);
			iRtnFunction = 41120;
		}
		else
		{
			logStr.Format("LASER �Ǹ��� ��� ���� [%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41110;
		}
		break;

	case 41120:
		if (Dio.LaserCylinderCheck(true, false))
		{
			logStr.Format("LASER �Ǹ��� ��� Ȯ�� �Ϸ�[%d]", iRtnFunction);
			theApp.MainDlg->putListLog(logStr);
			iRtnFunction = 41200;

			Sleep(300);
		}
		else if ((myTimer(true) - Task.PCBTaskTime) > IO_DELAY_TIME)
		{
			logStr.Format("LASER �Ǹ��� ��� �ð� �ʰ�..[%d]", iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41120;
		}
		break;
		//laser�Ǹ��� ���END

	case 41200://! Lens-Z�� ��� ��ġ �̵�
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 41850;;//// 41300; //41850;//�ӽ� ���� align pass
								  //iRtnFunction = 41300;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING739);	//Lens_Z�� �����ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41200;
		}
		break;

	case 41300:
		if (motor.Lens_Motor_MoveXY(0, Lens_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			sLangChange.LoadStringA(IDS_STRING659);	//Lens Align �̵� [%d]
			logStr.Format(sLangChange, iLensStep);
			putListLog(logStr);
			iRtnFunction = 41400;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING660);	//Lens Align �̵� ����[%d]
			logStr.Format(sLangChange, iLensStep);
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
			sLangChange.LoadStringA(IDS_STRING661);	//Lens Align ��ġ �̵� �Ϸ�[%d]
			logStr.Format(sLangChange, iLensStep);
			putListLog(logStr);
			iRtnFunction = 41500;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING660);	//Lens Align ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iLensStep);
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
			for (int i = 0; i<100; i++)
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

			sLangChange.LoadStringA(IDS_STRING651);	//Lens Align : %.3lf %.3lf %.3lf
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
				sLangChange.LoadStringA(IDS_STRING654);	//Lens Align [%d] �Ϸ� [%d]
				logStr.Format(_T("		") + sLangChange, Task.m_iRetry_Opt, iLensStep);
				putListLog(logStr);

				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);			//�ӽ� ��� ��ũ ����

				sLangChange.LoadStringA(IDS_STRING655);	//Lens Align Complete : %d, %.03f, %.03f, %.03f
				logStr.Format(_T("		") + sLangChange, Task.m_iLensPickupNoCentering, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK], Task.d_Align_offset_th[LENS_Align_MARK]);
				AlignResultSave(logStr);
				Task.m_iRetry_Opt = 0;
				iRtnFunction = 41800;				// �Ϸ�
			}
			else
			{
				sLangChange.LoadStringA(IDS_STRING653);	//Lens Align [%d] ���� ���� �ʰ�
				logStr.Format(sLangChange + _T("[%d]"), Task.m_iRetry_Opt, iLensStep);
				errMsg2(Task.AutoFlag, logStr);
				AlignLogSave(logStr);
				theApp.MainDlg->InstantMarkDelete(LENS_Align_MARK);
				iRtnFunction = -41600;				// NG
			}
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING652);	//Lens Align [%d] ��ũ �ν� ����[%d]
			logStr.Format(sLangChange, Task.m_iRetry_Opt, iLensStep);
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
			sLangChange.LoadStringA(IDS_STRING657);	//Lens Align ���� ��ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iLensStep);
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


		iRtnFunction = 41850;
		//if (Dio.CamLaserSlinderMove(false, true))
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iLensStep);
		//	//putListLog(logStr);
		//	Task.LensTaskTime = myTimer(true);
		//	iRtnFunction = 41850;
		//}
		//else
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iLensStep);
		//	errMsg2(Task.AutoFlag, logStr);
		//	iRtnFunction = -41800;
		//}

		// 20141217 LHC - sensor_holder�� align ���� ���� 0�� ���� �����ϰԲ�
		//�迵ȣ 20150602 FraneeGrabber  ���� �ӽ� Pass
		//if(Task.m_dataOffset_x[1] == 0 || Task.m_dataOffset_y[1] == 0 || Task.m_dataOffset_th[1] == 0)
		//{
		//	sLangChange.LoadStringA(IDS_STRING963);	//PCB Sensor_Holder�� Align ������ �̻� �߻� : X:%.03f, Y:%.03f, ��:%.04f [%d]
		//	logStr.Format(_T("		") + sLangChange, Task.m_dataOffset_x[1], Task.m_dataOffset_y[1],Task.m_dataOffset_th[1], iLensStep);
		//	putListLog(logStr);
		//	errMsg2(Task.AutoFlag,logStr);
		//	iRtnFunction = -41800;
		//	break;
		//}

		//-- ADO DB ����
		//g_ADOData.func_AATaskToRecordAlign(Task.ChipID, MandoInspLog.dLensOffset[0], MandoInspLog.dLensOffset[1], MandoInspLog.dLensOffset[2]);

		break;
	case 41850:
		iRtnFunction = 41870;
		//if (Dio.CamLaserSlinderMove(false, true))
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� �Ϸ�[%d]", iLensStep);
		//	//putListLog(logStr);
		//	Task.LensTaskTime = myTimer(true);
		//	iRtnFunction = 41870;
		//}
		//else
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� ����[%d]", iLensStep);
		//	errMsg2(Task.AutoFlag, logStr);
		//	iRtnFunction = -41850;
		//}
		break;
	case 41870:
		iRtnFunction = 41900;
		//if (Dio.CamLaserSlinderCheck(false, false))
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� Ȯ�� �Ϸ�[%d]", iLensStep);
		//	putListLog(logStr);
		//	iRtnFunction = 41900;
		//}
		//else if ((myTimer(true) - Task.LensTaskTime) > 10000)//IO_DELAY_TIME)
		//{
		//	logStr.Format("Laser/Cam �Ǹ��� ���� �ð� �ʰ�[%d]", iLensStep);
		//	errMsg2(Task.AutoFlag, logStr);
		//	iRtnFunction = -41870;
		//}
		break;
	case 41900://! Lens-Z�� ��� ��ġ �̵�
		if (motor.LENS_Z_Motor_Move(Wait_Pos))
		{
			Task.LensTaskTime = myTimer(true);
			iRtnFunction = 41950;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING739);	//Lens_Z�� �����ġ �̵� ����
			logStr.Format(sLangChange + _T("[%d]"), iLensStep);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -41900;
		}
		break;
	case 41950:
		iRtnFunction = 42000;


		break;
	case 42000: // ���� ��ġ �̵�
		if (motor.Lens_Motor_MoveXY(0, Bonding_Pos, Task.d_Align_offset_x[LENS_Align_MARK], Task.d_Align_offset_y[LENS_Align_MARK]))
		{
			sLangChange.LoadStringA(IDS_STRING659);	//Lens Align �̵� [%d]
			logStr.Format(sLangChange, iRtnFunction);
			putListLog(logStr);
			iRtnFunction = 42200;
		}
		else
		{
			sLangChange.LoadStringA(IDS_STRING664);	//Lens Bonding ��ġ �̵� ����[%d]
			logStr.Format(sLangChange, iRtnFunction);
			errMsg2(Task.AutoFlag, logStr);
			iRtnFunction = -42000;
		}
		break;
	case 42200:
		//�׽� ��  , lens ������ġ���� z�� �����ġ���� pcb ������ġ�� �����ͼ� z�� �� ���÷����ߵ�
		iRtnFunction = 43000;
		break;
	case 42400:
		if ((myTimer(true) - Task.LensTaskTime) > iCamDelay && (motor.IsStopAxis(Motor_Lens_Z)))
		{
			iRtnFunction = 43000;
		}
		break;
	case 43000:
		Task.interlockPcb = 1;
		Task.interlockLens = 1;
		iRtnFunction = 50000;
		break;
	default:
		sLangChange.LoadStringA(IDS_STRING628);	//Laser ���� ���� Thread Step ��ȣ ������ .
		logStr.Format(sLangChange + _T("[%d]"), iLensStep);
		errMsg2(Task.AutoFlag, logStr);
		iRtnFunction = -1;
		break;
	}

	return iRtnFunction;
}




