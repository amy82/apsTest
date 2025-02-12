
// DioControl.h : 


#pragma once

#include "stdafx.h"

//DIO INPUT

#define		DIO_IN_EMERGENCY1						0x00000001		//!  1) 
#define		DIO_IN_EMERGENCY2						0x00000002		//!  2) 
#define		DIO_IN_PCB_VACUUM						0x00000004		//!  3) 
#define		DIO_IN______TEMP4						0x00000008		//!  4) 
#define		DIO_IN______TEMP5						0x00000010		//!  5) 
#define		DIO_IN______TEMP6						0x00000020		//!  6) 
#define     DIO_IN_DOORSENSOR1						0x00000040		//!  7) 
#define     DIO_IN_DOORSENSOR2						0x00000080		//!  8) 
#define		DIO_IN_DOORSENSOR3						0x00000100		//!  9) 
#define		DIO_IN_DOORSENSOR4						0x00000200		//! 10) 
#define		DIO_IN_DOORSENSOR5						0x00000400		//! 11) 
#define		DIO_IN_DOORSENSOR6						0x00000800		//! 12)  
#define		DIO_IN_DOORSENSOR7						0x00001000		//! 13)
#define     DIO_IN______TEMP14						0x00002000		//! 14) 
#define     DIO_IN______TEMP15						0x00004000		//! 15) 
#define     DIO_IN______TEMP16						0x00008000		//! 16) 
//-----------------------------------------------------------------------------
#define		DIO_IN______TEMP17						0x00010000		//! 17) 
#define		DIO_IN______TEMP18						0x00020000		//! 18) 
#define     DIO_IN_UV_LAMP_ON						0x00040000		//! 19) 
#define     DIO_IN_UV_LAMP_ALARMUP					0x00080000		//! 20) 
#define     DIO_IN_UV_LAMP_ALARMDOWN				0x00100000		//! 21) 
#define		DIO_IN______TEMP22						0x00200000		//! 22) 
#define		DIO_IN______TEMP23						0x00400000		//! 23) 
#define		DIO_IN______TEMP24						0x00800000		//! 24) 
#define     DIO_IN_EPOXY_ON							0x01000000		//! 25) 
#define		DIO_IN______TEMP26						0x02000000		//! 26)
#define		DIO_IN______TEMP27						0x04000000		//! 27) 
#define		DIO_IN______TEMP28						0x08000000		//! 28) 
#define		DIO_IN______TEMP29						0x10000000		//! 29) 
#define		DIO_IN______TEMP30						0x20000000		//! 30) 
#define     DIO_IN_LIGHT_CURTAIN					0x40000000		//! 31) 
#define		DIO_IN_AA_STAGE1_INTERLOCK				0x80000000		//! 32) 


//1�� DIO
#define		DIO_IN_DIS_SENSOR						0x00000001		//!  1) LENS GRIP ����
#define		DIO_IN_PCBSTAGE_GRIP_BACK				0x00000002		//!  2) LENS GRIP ����
#define		DIO_IN_PCBSTAGE_GRIP_FOR				0x00000004		//!  3) Lens Stage Gripper ���� ����
#define		DIO_IN_DOOR_CLOSE						0x00000008		//!  4) DOOR Close
#define		DIO_IN_DOOR_OPEN						0x00000010		//!  5) DOOR Open
#define		DIO_IN______2TEMP6						0x00000020		//!  6) �������
#define		DIO_IN_START							0x00000040		//!  7) START PB
#define		DIO_IN_DOOR_PB							0x00000080		//!  8) DOOR PB
#define		DIO_IN______2TEMP9						0x00000100		//!  9) �������
#define		DIO_IN______2TEMP10						0x00000200		//! 10) �������
#define		DIO_IN______2TEMP11						0x00000400		//! 11) �������
#define		DIO_IN______2TEMP12						0x00000800		//! 12) �������
#define		DIO_IN______2TEMP13						0x00001000		//! 13) �������
#define		DIO_IN______2TEMP14						0x00002000		//! 14) �������
#define		DIO_IN_OC_FOR							0x00004000		//! 15) OC ���� ���� === �ӽ�
#define		DIO_IN_OC_BACK							0x00008000		//! 16) OC ���� ���� === �ӽ�
//------------------------------------------------------------------
#define		DIO_IN______2TEMP17						0x00010000		//! 17) Lens Grip ��������
#define		DIO_IN_LENS_GRIP_BACK					0x00020000		//! 18) Lens Grip ��������
#define     DIO_IN_LENS_GRIP_FOR					0x00040000		//! 19) LASER  ��¼��� DIO_IN_CHART_BACK
#define     DIO_IN_PCB_GRIP_BACK					0x00080000		//! 20) LASER  �ϰ����� DIO_IN_CHART_FOR
#define     DIO_IN_PCB_GRIP_FOR						0x00100000		//! 21) Dark �˻� ��¼���
#define     DIO_IN______2TEMP22						0x00200000		//! 22) Dark �˻� �ϰ�����
#define     DIO_IN______2TEMP23						0x00400000		//! 23) C/R ����
#define     DIO_IN______2TEMP24						0x00800000		//! 24) C/R ����
#define		DIO_IN_PCB_SOCKET_CLOSE					0x01000000		//! 25) PCB GRIP ����	-- ������	
#define     DIO_IN______2TEMP26						0x02000000		//! 26) PCB GRIP ����	-- ������
#define		DIO_IN______2TEMP27						0x04000000		//! 27) PCB Ŭ�� ���� ���� ����
#define		DIO_IN_OCLIGHT_FOR						0x08000000		//! 28) �˻� FEED ����
#define		DIO_IN_OCLIGHT_BACK						0x10000000		//! 29) �˻� FEED ����
#define		DIO_IN_LASER_CAM_DOWN					0x20000000		//! 30) DARK ����
#define		DIO_IN_LASER_CAM_FOR					0x40000000		//! 31) DARK ����
#define		DIO_IN______2TEMP32						0x80000000		//! 32) DARK CAP PUSHER ���



//�ܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡ�
//�ܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡܡ�
///////////////// DIO Output DEFINE /////////////////
//0�� ���

//!---------- Channel 0
	//0��
#define		DIO_OUT_PCB_VACUUM				0x01		//!  1) �������
#define		DIO_OUT_PCB_VACUUM_OFF			0x02		//!  2) �������
#define		DIO_OUT_SPARE3					0x04		//!  3) �������
#define		DIO_OUT_SPARE4					0x08		//!  4) �������
#define     DIO_OUT_SPARE5					0x10		//!  5) �������
#define     DIO_OUT_SPARE6					0x20		//!  6) �������
#define     DIO_OUT_SPARE7					0x40		//!  7) �������
#define     DIO_OUT_SPARE8					0x80		//!  8) �������
	//
#define     DIO_OUT_TOWER_LAMP_R			0x01		//!  9) LAMP R
#define     DIO_OUT_TOWER_LAMP_Y			0x02		//! 10) LAMP Y
#define		DIO_OUT_TOWER_LAMP_G			0x04		//! 11) LAMP G
#define		DIO_OUT_SPARE9					0x08		//! 12) �������
#define		DIO_OUT_BUZZER1					0x10		//! 13) BUZZER1
#define		DIO_OUT_BUZZER2					0x20		//! 14) BUZZER2
#define		DIO_OUT_BUZZER3					0x40		//! 15) BUZZER3
#define		DIO_OUT_BUZZER4					0x80		//! 16) BUZZER4
	//-----------------------------------------------------------------------------
#define		DIO_OUT_SPARE10					0x01		//! 17) �������
#define		DIO_OUT_SPARE11					0x02		//! 18) �������
#define		DIO_OUT_UV_ON					0x04		//! 19) UV ON
#define		DIO_OUT_UV_EMG					0x08		//! 20) UV EMG
#define		DIO_OUT_SPARE12					0x10		//! 21) �������
#define		DIO_OUT_SPARE13					0x20		//! 22) �������
#define		DIO_OUT_SPARE14					0x40		//! 23) �������
#define		DIO_OUT_SPARE15					0x80		//! 24) �������
	//
#define		DIO_OUT_EPOXY_ON				0x01		//! 25) ���� ON
#define		DIO_OUT_SPARE16					0x02		//! 26) �������
#define		DIO_OUT_SPARE17					0x04		//! 27) �������
#define		DIO_OUT_SPARE18					0x08		//! 28) �������
#define		DIO_OUT_SPARE19					0x10		//! 29) �������
#define		DIO_OUT_SPARE20					0x20		//! 30) �������
#define		DIO_OUT_SPARE21					0x40		//! 31) �������
#define		DIO_OUT_SPARE22					0x80		//! 32) �������



	//1��
#define		DIO_OUT_PCBSTAGE_GRIP_FOR		0x01		//!  1) HOLDER GRIP ����
#define		DIO_OUT_PCBSTAGE_GRIP_BACK		0x02		//!  2) HOLDER GRIP ����
#define		DIO_OUT_DOOR_CLOSE				0x04		//!  3) DOOR CLOSE
#define		DIO_OUT_DOOR_OPEN				0x08		//!  4) DOOR OPEN
#define		DIO_OUT_LENS_GRIP_BACK			0x10		//!  5) LENS GRIP ����
#define		DIO_OUT_LENS_GRIP_FOR			0x20		//!  6) LENS GRIP ����
#define		DIO_OUT_PCB_GRIP_BACK			0x40		//!  7) LENS ALIGN CHUCK ����
#define		DIO_OUT_PCB_GRIP_FOR			0x80		//!  8) LENS ALIGN CHUCK ����
	//------------------------------------------------------------------
#define		DIO_OUT_OCLIGHT_BACK			0x01		//!  9) �˻� FEED ����
#define		DIO_OUT_OCLIGHT_FOR				0x02		//! 10) �˻� FEED ����
#define		DIO_OUT_LASER_CAM_BACK			0x04		//! 11) DARK CAP PUSHER ���
#define		DIO_OUT_LASER_CAM_FOR			0x08		//! 12) DARK CAP PUSHER �ϰ�
#define		DIO_OUT_SPARE23					0x10		//! 13) �������
#define		DIO_OUT_SPARE24					0x20		//! 14) �������
#define		DIO_OUT_SPARE25					0x40		//! 15) �������
#define		DIO_OUT_SPARE26					0x80		//! 16) �������
	//------------------------------------------------------------------
#define		DIO_OUT_START_PB				0x01		//! 17) START PB
#define		DIO_OUT_DOOR_PB					0x02		//! 18) DOOR PB
#define		DIO_OUT_SPARE27					0x04		//! 19) �������
#define		DIO_OUT_SPARE28					0x08		//! 20) �������
#define		DIO_OUT_SPARE29					0x10		//! 21) �������
#define		DIO_OUT_SPARE30					0x20		//! 22) �������
#define		DIO_OUT_SPARE31					0x40		//! 23) �������
#define     DIO_OUT_SPARE32					0x80		//!	24) �������
	//
#define		DIO_OUT_SPARE33					0x01		//!	25) �������
#define		DIO_OUT_SPARE34					0x02		//!	26) �������
#define		DIO_OUT_SPARE35					0x04		//!	27) �������
#define		DIO_OUT_SPARE36					0x08		//!	28) �������
#define		DIO_OUT_SPARE37					0x10		//!	29) �������
#define		DIO_OUT_SPARE38					0x20		//!	30) �������
#define		DIO_OUT_OC_FOR					0x40		//!	31) OC ���� ���� === �ӽ�
#define		DIO_OUT_OC_BACK					0x80		//!	32) OC ���� ���� === �ӽ�


//�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�
//�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�
//�ڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡڡ�


class CDioControl
{
public:
	CDioControl();

	CCriticalSection	g_CriOutDio;
public:
	//241119
	//New
	bool OcSlinderMove(bool flag, bool waitFlag);
	bool OcSlinderCheck(bool flag, bool waitFlag);

	//Cam, Laser �Ǹ��� ��,����
	bool CamLaserSlinderMove(bool flag, bool waitFlag = true);
	//Cam, Laser �Ǹ��� ���� Ȯ��
	bool CamLaserSlinderCheck(bool flag, bool waitFlag = true);

	//Conti ���� ���� �׸�
	bool ContiTailGrip(bool flag, bool waitFlag = true);
	//Conti ���� ���� �׸� ���� Ȯ��
	bool ContiTailGripCheck(bool flag, bool waitFlag = true);

	//! UV�� ���� Ż��
	bool UVGasClean(bool flag);

	//! Lens Gripper ����/����
	bool HolderGrip(bool flag, bool waitFlag = true);
	//! Lens Gripper ����/���� ����
	bool HolderGripCheck(bool flag, bool waitFlag = true);

	//! Lens Gripper ����/����
	bool LensMotorGrip(bool flag, bool waitFlag = true);
	//! Lens Gripper ����/���� ����
	bool LensMotorGripCheck(bool flag, bool waitFlag = true);


	//! pcb Grip ����/����
	bool LensLift(bool flag, bool waitFlag = true);
	//! Lens Lift ���/�ϰ� ����
	bool LensLiftCheck(bool flag, bool waitFlag = true);

	//! PCB ���� ���� ����
	bool LensOnCheck(bool flag, bool waitFlag = true);

	//! PCB Cover ����/����
	bool PCBPush(bool flag, bool waitFlag = true);		//-- ������(���� ����)
														//! PCB Cover ����/���� ����
	bool PCBPushCheck(bool flag, bool waitFlag = true);	//-- ������(���� ����)

	bool PCBCoverCloseCheck(bool flag, bool waitFlag = true);	//PCB Ŀ�� Ŭ������ �������� T=����Ȯ��, F=����Ȯ��


	bool PCBDark(bool flag, bool waitFlag = true);					//! PCB Dark ���/�ϰ�
	bool PCBDarkCheck(bool flag, bool waitFlag = true);				//! PCB Dark ���/�ϰ� ����

																	//! PCB ����/Ż��
	bool PCBvaccumOn(int flag, bool waitFlag = true);				// 0:OFF, 1:����, 2:Ż��..
																	//! PCB ���� ����
	bool PCBvaccumOnCheck(bool flag, bool waitFlag = true);			// ������ Ȯ�� ����..


	bool BackLightOn(bool flag, bool waitFlag = true);				//! ��� ���� �ϰ�/���
	bool BackLightOnCheck(bool flag, bool waitFlag = true);			//! ��� ���� �ϰ�/��� ���� Ȯ��

																	//! Start P/B ����ġ ����
	bool StartPBOnCheck(bool flag, bool waitFlag = true);
	bool StartPBLampOn(bool flag);

	//! Door Open/Close
	bool DoorPBOnCheck(bool flag, bool waitFlag = true);
	bool DoorPBLampOn(bool flag);
	bool DoorLift(bool flag, bool waitflag = true);
	bool DoorLiftOnCheck(bool flag, bool waitflag = true);

	//! ���� ���� ����(Ŀư)
	bool LightCurtainOnCheck(bool flag, bool waitFlag = true);

	//! ����
	bool EpoxyDischargeOn(bool flag, bool waitFlag = true);			//Dispence ���� On/Off 
	bool EpoxyDischargeOnCheck(bool flag, bool waitFlag = true);	//Dispence ������ Check

																	//! 20150603 ���� I/O �߰�
	bool EpoxyDischargeOn2(bool flag, bool waitFlag = true);			//Dispence ���� On/Off 
	bool EpoxyDischargeOnCheck2(bool flag, bool waitFlag = true);	//Dispence ������ Check

	bool UVTrigger(bool flag, bool waitFlag = true);
	bool UVCheck(bool flag, bool waitFlag = true);

	int SensorContactCheck();

	void setAlarm(int AlarmNo);			//0: �˶� + Red Lamp On 1: Yellow Lamp 2: Green Lamp

	void SetLampOn(int iType);			//enum LAMP_NO { ALARM_ON, ALARM_OFF, AUTO_STOP, AUTO_RUN};
	void SetBuzzer(bool flag, int iType);

	bool PcbGrip(bool flag, bool waitFlag = true);			//PCBGRIP
	bool PcbGripCheck(bool flag, bool waitFlag = true);			//PCBUNGRIP

	bool CamLaserClinder(bool flag, bool waitFlag = true);					//! PCB Dark ���/�ϰ�
	bool CamLaserClinderCheck(bool flag, bool waitFlag = true);				//! PCB Dark ���/�ϰ� ����

	bool VacuumSol(bool flag, bool waitFlag = true); //! Vacuum �ı�

	bool Relay(bool flag, bool waitFlag = true); //! ������
	
public:

	bool LensVacuumOn(bool flag);

};
