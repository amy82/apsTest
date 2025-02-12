#pragma once
#ifndef __GLOBAL_DEFINE
#define __GLOBAL_DEFINE


//#define		NORINDA_MODE

//=======================================================================
#define	PCB_TILT_AA				0
#define LENS_TILT_AA			1

#define ____AA_WAY					LENS_TILT_AA	//PCB_TILT_AA, LENS_TILT_AA




#define	AA_RUN_MODE				2101
#define HOLDER_RUN_MODE			3102

#define HEAD_MODEL

#define	__RUN_MODE			AA_RUN_MODE//AA_RUN_MODE

const int M2_FF_MODULE = 200200;		//240726 �ڵ���, �輼�� ����, �簢���� ���� ��ǰ ����� ���� ��Ʈ 9�� LGIT_GRABBER_20240715_01_v2.2.3  epoxy���� = 0.035

static int LGIT_MODEL_INDEX = M2_FF_MODULE;
//----------------------------------------------------------------------------------------------------------------------------------- 
#define					VER_STR		"250207_2"			//����ǰ ������ ��
//#define					VER_STR		"250207_1"			//�輼�� ���� ��û
//#define					VER_STR		"250206_1"			//bcr Ȯ��, ��ư ���� 
//#define					VER_STR		"250124_1"			//�ڵ� ����
//#define					VER_STR		"250123_3"			// �輼�� ���� ��û OC �̹� ���� �˻� txty �������� -> ƼĪ pos �� �˻�, bcr pos = DarkPos
//#define					VER_STR		"250123_2"			// SR-X100 ����, ���� Ȯ��
//#define					VER_STR		"250122_2"			//Sleep(300);, LENS_DEAD_END_MARK_2 ���� �� BCR IP UI ����
//#define					VER_STR		"250122_1"			//LensHoleFind �ڵ����� Process �߰� LensAlign �� 
//#define					VER_STR		"250121_4"			// ccd cell size ���̺꿡 �߰� ���� �ϵ����� 3.0 -> 0.0 �ʱ�ȭ�� �ٲ�
//#define					VER_STR		"250121_3"			//
//#define					VER_STR		"250121_2"			// Lens Hole MarkFind = procCamComAlign, Dispense_Pos2 = LensHoleFind, Dark_Pos = BCRread
//#define					VER_STR		"250121_1"			// LogData, Image, ACMIS, Excel ��� �� -> ��, OC Spec UI ���� ����
//#define					VER_STR		"250120_2"			// SubCam ȭ�� �ٽ� ����, ������ ���� ��÷ �˰��� Ÿ�� UI ����
//#define					VER_STR		"250120_1"			// �簢���� //�߰� ���� = UI �� DisLength 5 ������, SubDlg ��ư, SubCam ȭ�� ���� 
//#define					VER_STR		"250117_1"			//DLL ����, �̹������� ���ϰ�� Month -> Day 
//#define					VER_STR		"250116_2"			//���� ��ȣ �ݴ�
//#define					VER_STR		"250116_1"			//MandoInspLog.bInspRes = false; otp Write, Verify ��ǰ ��� ���� ����
//#define					VER_STR		"250115_2"			//
//#define					VER_STR		"250115_1"			//
//#define					VER_STR		"250114_5"			//
//#define					VER_STR		"250114_4"			//���� Lee
//#define					VER_STR		"250114_3"			//
//#define					VER_STR		"250114_2"			//
//#define					VER_STR		"250114_1"			//
//#define					VER_STR		"250113_1"			//doorlift, bcr func_Barcode_Read ���⸦ ��Ž
//#define					VER_STR		"250110_6"			//BCR
//#define					VER_STR		"250110_5"			//����ǰ ALIGN ��ȣ �ݴ�
//#define					VER_STR		"250110_4"			//Sfr ��հ� 1,2�� �и� Lee
//#define					VER_STR		"250110_3"			//Sfr ��հ� 1,2�� �и�
//#define					VER_STR		"250110_2"			//AA�ݺ��� ����
//#define					VER_STR		"250110_1"			//AA�ݺ���
//#define					VER_STR		"250109_3"			//SfrMax��, �ݺ��� UI �� ����
//#define					VER_STR		"250109_2"			//����� ������ �����Ʈ
//#define					VER_STR		"250109_1"			//LENS ALIGN �� XY �����ġ �Ȱ��� �ٷ� ���� ���� 
//#define					VER_STR		"250108_3"			//LENS ALIGN �� XY �����ġ �Ȱ��� �ٷ� ���� ���� 
//#define					VER_STR		"250108_2"			//������Ʈ�ѷ� Ŭ���� 1���� 
//#define					VER_STR		"250108_1"			//������ ������ UI Z �ุ Dispense_Pos1 ��  
//#define					VER_STR		"250107_1"			//�����˻� ���� ,  otp���� writeData[5] = 0x11;	////0x0F;
//bThreadTaskPcbRun = false;	������ �߰�
//bThreadTaskLensRun = false;
//Task.bMotorRunStop ���� �߰� (�����ص� ���� �̵� while ������ �Ⱥ��������� �� ���� �߰�

//#define					VER_STR		"250103_4"				//LENS ALIGN
//#define					VER_STR		"250103_3"				//
//#define					VER_STR		"250103_2"				//holder bonding mode
//#define					VER_STR		"250103_1"				//�ڵ��� SensorAlign ����� ��ȣ ����, UV After�� OC �� MandoInspLog. �̰� �ٿ��־ 
//#define					VER_STR		"250102_2"				//
//#define					VER_STR		"250102_1"				//
//#define					VER_STR		"241231_2"				//�ڵ� AA 
//#define					VER_STR		"241231_1"				//HOLDER_RUN exe 
//#define					VER_STR		"241230_1"				//LGIT_GRABBER_20241230_01_v2.2.5_TEMP ������ å�� DLL����
//#define					VER_STR		"241227_3"			//OTP, F/W, Sensor ID AA, EOL ������ MIU.Close ���� 
//#define					VER_STR		"241227_2"			//���� �ε�, AA Pos �̵� ���� ���� 39000 <-> 40000 
//#define					VER_STR		"241227_1"			//Laser ���� ��ư LT �� ����
//#define					VER_STR		"241226_2"			//mark mil10 ����
//#define					VER_STR		"241226_1"				//OTP �˻� ������ư
//#define					VER_STR		"241224_1"				//DRY_RUN, TRI UV ����
//#define					VER_STR		"241223_1"				//���� �� ����ƼĪ
//#define					VER_STR		"241220_1"				//�� ����� OTP �Լ� �߰� + �������� ������ �߰� + ���̳� �α� �Լ��� SensorID, OTP, FW ���� �߰�
//#define					VER_STR		"241219_1"				//���� ��Ʈ�ѷ� 1000OP ����  BOARD_TYPE_LAON1000OP �˻� ����̹� ��ġ �� ����
//#define					VER_STR		"241211_1"				//PCBCoverCloseCheck  pcb motor move �Լ�, pcb motor Y move �Լ� �� �ּ� ó�� �Ǿ�����, Vaccum On ������ AA Loading ó���� �߰�
// #define					VER_STR		"241127_1"				//HOLDER_RUN_MODE ��� Ȯ�� �Ϸ�
//#define					VER_STR		"241121_1"				//�����ڵ� ��,
//#define					VER_STR		"241120_1"				//Uv Altis �߰�
//#define					VER_STR		"241119_3"				//io���� �Ϸ�
//#define					VER_STR		"241119_2"				//[20241029]Distribute_V2.8.0.1_VS2015 dll����
//#define					VER_STR		"241119_1"				//���� ���� ����� ���� ���� , mil10���� ��ü �Ϸ�


//
//#define					VER_STR		"241114_1"
//��Ʈ ��� green 150���� ���Ⱚ���� ���� , �������� = 0.035 ~ 0.039����
//0x300B 0x05   //; exposure H
//0x300C 0x78   //; exposure L
//uv�� , uv�� , ����ǰ mtf������ ������� �̹����� ������ ����
//#define					VER_STR		"241112_1"			//M2_FF_MODULE �� �輼�� ���� sfr , ������ ���� ��ճ� ������ aa ���� ��û, ������

//opencv dll �߰�
//PATH=$(ProjectDir)\Library\lib64;C:\Opencv346\install\x64\vc14\bin;%PATH%

//io , ����,������ ����

//#define	MACHINE_1ST				110
//#define MACHINE______2ST		304
////
//#define	__MACHINE_MODEL			MACHINE______2ST



//=======================================================================
//=======================================================================
#define		EVMS
#define		ON_LINE_MODE
#ifdef NORINDA_MODE

//#define		ON_LINE_VISION
//#define		ON_LINE_MOTOR
//#define		ON_LINE_DIO
//#define		ON_LINE_MOTOR

#else

#ifdef		ON_LINE_MODE
#define		ON_LINE_MOTOR
////#define		ON_LINE_LAN
#define		ON_LINE_DIO
#define		ON_LINE_VISION
#define		ON_LINE_KEYENCE
#define		ON_LINE_MONITOR
#endif
#endif


//=======================================================================
//=======================================================================
#define  S_ONSEMI					2100		//���� �¼��� ����
#define  S_OMNIVISION				2200		//�ȴϺ��� ����


#define ____SENSOR_TYPE					999999999		//S_OMNIVISION


//=================================================================================================================================================================


static CString sTempLang;
static CString sLangChange;

#define SWAP(a,b)		{int t; t=a; a=b; b=t;}		/* int�� ���� */
#define SWAP_STR(a,b)	{CString t; t=a; a=b; b=t;}	/* CString�� ���� */
#define rgb(r,g,b) (r*256*256+g*256+b)


#define PDC_LAVENDER			RGB(230,230,250)
#define PDC_GREEN					RGB(0,128,0)
#define PDC_LIGHTGREEN			RGB(144,238,144)
#define PDC_YELLOW				RGB(255,255,0)
#define PDC_BLACK					RGB(0,0,0)
#define PDC_RED						RGB(255,0,0)
#define PDC_BLUE					RGB(0,0,255)

#define CV_LAVENDER				CV_RGB(230,230,250)
#define CV_GREEN					CV_RGB(0,128,0)
#define CV_LIGHTGREEN			CV_RGB(144,238,144)
#define CV_YELLOW					CV_RGB(255,255,0)
#define CV_BLACK					CV_RGB(0,0,0)
#define CV_RED						CV_RGB(255,0,0)
#define CV_BLUE						CV_RGB(0,0,255)


enum MOTOR_SPEED { SPEED_AUTO_H = 0, SPEED_AUTO_L, SPEED_MANUAL };				//!! ���� ���� �ӵ�
enum MODE_TYPE { MODE_STOP = 0, MODE_READY, MODE_AUTO, MODE_PAUSE, MODE_ERR };	//!! ��� ���� ���� 0=��������, 1=�����غ����, 2=�ڵ���������, 3=�Ͻ���������
enum HOME_READY_TYPE { TYPE_HOME = 0, TYPE_READY, TYPE_HOME_READY };					//!! ���� ���� ���� 

#define		MAX_CAM_NO		4
#define		MAX_MARK		3
enum CAMERA { CAM1 = 0, CAM2, CAM_NULL, Cam_Cnt };

enum MARK { PCB_Chip_MARK = 0, PCB_Holder_MARK, LENS_Align_MARK, MARK_CNT };

enum CHART_FIELD { CHART_CENTER = 0, CHART_TOP, CHART_BOTTOM, CHART_LEFT, CHART_RIGHT, CHART_FIELD_CNT };
enum SFR_ROI { CT = 0, CB, CL, CR, TL, TB, BR, BT, LB, LR, RT, RL, SFR_ROI_CNT };

#define SOCKET_MAIN			"192.168.3.3"
#define SOCKET_AA1			"192.168.3.1"
#define SOCKET_AA2			"192.168.3.2"

#define SFR_COMP_MAX		// SFR��� �� ��հ�, �ִ�ġ �� ����.  ���ǽ� �ִ�ġ 

#define J_RCCC	0
#define J_CRCC	1
#define J_CCRC	2
#define J_CCCR	3

#define R_RESULT_PASS					0	//��ǰ 
#define R_RESULT_FAIL					1	//�ҷ�
#define R_FAIL_NOISE					2
#define R_FAIL_BLACK_SPOT				3
#define R_FAIL_STAIN					4

#define IMG_PROC_TYPE	7


#define		USE_GEOMETRIC
#define     USE_MIL_BAYER_CONVERSION

#define _DEF_MAX_BLOBS		200  

#define		CHART_CENTER_MARK	1000



enum LED_LIGHT_CONTROL {
	LIGHT_OC, LIGHT_PCB,
	LIGHT_LEFT_CHART, LIGHT_RIGHT_CHART,
	LIGHT_TOP_CHART1, LIGHT_TOP_CHART2, LIGHT_TOP_CHART3, LIGHT_TOP_CHART4, LIGHT_TOP_CHART5, LIGHT_TOP_CHART6,
	LIGHT_LENS,
	MAX_LIGHT_CONTROL
};


enum LED_LIGHT_DATA {
	LEDDATA_SENSOR, LEDDATA_HOLDER, LEDDATA_CHART_L, LEDDATA_CHART_R, 
	LEDDATA_TOP1_CHART, LEDDATA_TOP2_CHART, LEDDATA_TOP3_CHART, LEDDATA_TOP4_CHART, LEDDATA_TOP5_CHART, LEDDATA_TOP6_CHART,
	LEDDATA_OC, LEDDATA_EPOXY_INSP, LEDDATA_RESIN_CALC, LEDDATA_LENS, LEDDATA_STAIN, LEDDATA_DEFECT, MAX_LIGHT_DATA
};
//LEDDATA_6500K, LEDDATA_2800K
enum	RS_232_COMM { COMM_LASER, COMM_BARCODE, COMM_UV, COMM_LIGHT1, COMM_LIGHT2, COMM_LIGHT3, COMM_UV2, COMM_LIGHT4, COMM_LIGHT5, COMM_LIGHTOC_1ST, COMM_X100_BCR, MAX_RS232_NO };


#define		SIZE_OF_100BYTE					100
#define		SIZE_OF_1K						1024

#define		MAIN_DLG_SIZE_X		1920//1280						// Main Dialog..
#define		MAIN_DLG_SIZE_Y		1080
#define		SUB_DLG_OFFSET_Y	10

#define		MARK_SIZE_X			183							// ���� ��ũ Display ũ��..
#define		MARK_SIZE_Y			167

#define		MARKSET_SIZE_X		377							// ���� ��ũ Display ũ��..
#define		MARKSET_SIZE_Y		333

#define		CAM_SIZE_X			1280						// CIS ī�޶� ���� ũ��..
#define		CAM_SIZE_Y			1024



#define		SMALL_CAM_SIZE_X	(CAM_SIZE_X/3+100)			// ���� ȭ�� Display..
#define		SMALL_CAM_SIZE_Y	(CAM_SIZE_Y/3+100)

#define		SMALL_CCD_SIZE_X	(CAM_SIZE_X/3+600)			//386
#define		SMALL_CCD_SIZE_Y	(CAM_SIZE_Y/3+240)			//100





#define		CAM_REDUCE_FACTOR_X		(double)SMALL_CAM_SIZE_X/CAM_SIZE_X
#define		CAM_REDUCE_FACTOR_Y		(double)SMALL_CAM_SIZE_Y/CAM_SIZE_Y
#define		CAM_EXPAND_FACTOR_X		(double)CAM_SIZE_X/SMALL_CAM_SIZE_X
#define		CAM_EXPAND_FACTOR_Y		(double)CAM_SIZE_Y/SMALL_CAM_SIZE_Y


#define		IO_DELAY_TIME			20000

#define		IN_POSITION_VAL			0.2

#define		MAX_DEVICE				2


#define		CCD				Cam_Cnt

#define		OS_PIN_COUNT		55

#define		MAX_ALARM_PAGE_CNT			20			// �˶� �����丮 â�� �ִ� �˶� ������ ��..
#define		MAX_ALARM_LIST_IN_PAGE		24			// �˶� ������ �� ��µǴ� �˶� ��..

#define		MAX_NG_LIST					20			// �ҷ� ���� �� NG �׸� ��� ��..



enum OUTPUT_MODE { ASCII_MODE = 0, ASCII_MODE_2, FLOAT_MODE, DEC_MODE, HEX_MODE, FuSeID_MODE};
enum E_GRAB_METHOD { LAON_CVT_COLOR, MIL_BAYER_CVT_COLOR };
enum IMAGE_SAVE_ID { LENS_IMAGE_SAVE, PCB_IMAGE_SAVE, CHIP_IMAGE_SAVE, AA_FINAL_IMAGE_SAVE, AA_NG_IMAGE_SAVE, AA_CIRCLE_NG_IMAGE_SAVE, OPTIC_IMAGE_SAVE, DEFECT_NG_IMAGE_SAVE, EPOXY_IMAGE_SAVE };
enum AA_MODE { MODE_MARKCENTER = 0, MODE_DISTORTIONCENTER, MODE_OPTICAL };
enum MTF_INSP_MODE { MTF_17P = 0, MTF_24P };

#define		COMMON_LAST_MARK_CNT			20		//���� ���� , ���� �����θ� ���
#define		COMMON_MTF_INSP_CNT				50		//���� ���� , ���� �����θ� ���


#define MAX_FOV_COUNT				9       //����

#define MAX_SNR_COUNT				3       //��,��,�˼���


#define		SFR_ROI_VERTICAL			0//1
#define		SFR_ROI_HORIZONTAL			1//2

// Cneter SFR �˻� Box ��ȣ..
#define		SFR_CENTER_UP					0	
#define		SFR_CENTER_DOWN					1	
#define		SFR_CENTER_LEFT					2
#define		SFR_CENTER_RIGHT				3
#define		SFR_CENTER_END					3

#define		SFR_05_LEFT_UP_H				4	
#define		SFR_05_LEFT_UP_V				5	

#define		SFR_05_RIGHT_UP_H				6
#define		SFR_05_RIGHT_UP_V				7

#define		SFR_05_LEFT_DOWN_H				8	
#define		SFR_05_LEFT_DOWN_V				9	

#define		SFR_05_RIGHT_DOWN_H				10
#define		SFR_05_RIGHT_DOWN_V				11
//--------------------------------------------------------else
#define		SFR_07_LEFT_UP_H				12	
#define		SFR_07_LEFT_UP_V				13	

#define		SFR_07_RIGHT_UP_H				14
#define		SFR_07_RIGHT_UP_V				15

#define		SFR_07_LEFT_DOWN_H				16	
#define		SFR_07_LEFT_DOWN_V				17	

#define		SFR_07_RIGHT_DOWN_H				18
#define		SFR_07_RIGHT_DOWN_V				19
//--------------------------------------------------------visteon






// 0.5 �ʵ� �ڳʺ� SFR �˻� Box ��ȣ..
#define		SFR_50_05						4
#define		SFR_50_06						5
#define		SFR_50_07						6
#define		SFR_50_08						7

// 0.7 �ʵ� �ڳʺ� SFR �˻� Box ��ȣ..		
#define		SFR_70_09						8
#define		SFR_70_10						9
#define		SFR_70_11						10
#define		SFR_70_12						11

// 0.7 �ʵ� �ڳʺ� SFR �˻� Box ��ȣ..						
#define		SFR_LEFT_UP_INDEX				16		// Ȳ����  ���� �˻���õ� ���� ����Ǿ� ����
#define		SFR_LEFT_UP_INDEX_2				17

#define		SFR_RIGHT_UP_INDEX				18		// Ȳ����  ���� �˻���õ� ���� ����Ǿ� ����
#define		SFR_RIGHT_UP_INDEX_2			19

#define		SFR_RIGHT_DOWN_INDEX			20
#define		SFR_RIGHT_DOWN_INDEX_2			21

#define		SFR_LEFT_DOWN_INDEX				22
#define		SFR_LEFT_DOWN_INDEX_2			23



#define		LED_CHART			1
#define		LED_OC				2
#define		LED_LENS			3
#define		LED_PCB				4


#define		SFR_FIRST							0
#define		SFR_SECOND							1
#define		SFR_FINAL							2
#define		OC_INSP								3
#define		SFR_BEFORE_UV						4
#define		SFR_AFTER_UV						5
#define		SFR_OFFSET_DATA						6
#define		SFR_AFTER_OFFSET_MOVE				7
#define		SFR_AFTER_TILT						8
#define		SFR_MANUAL							9
#define		MTF_INSP							10
#define		SFR_STEP							11



#define		UNLOADING_INDEX		0
#define		AA1_INDEX			1
#define		AA2_INDEX			2
#define		OC_INDEX			3
#define		UV_INDEX			4


#define		RECEIVE_DATA		FALSE
#define		SEND_DATA			TRUE

enum SIDE_MODEL { SIDE_FRONT = 0, SIDE_BACK };

enum DIALOG { MAIN_DLG, MODEL_DLG, LENS_DLG, LENS_EDGE_DLG, PCB_DLG, RESIN_DLG, CCD_DLG, MOTOR_DLG, MOTOR_DLG2, MOTOR_DLG3, IO_DLG, ALARM_DLG, MOTOR_INSPDLG, PCB_INSPDLG , MOTOR_DLG4};

enum RAW_CAPTURE_TYPE { MID_6500K_RAW = 0, LOW_LEVEL_RAW, MID_2800K_RAW, UV_BEFORE_CHART, UV_AFTER_CHART, EOL_CHART, CHART_RAW, CHART_SECOND_RAW };
enum JPG_CAPTURE_TYPE { BLACKSPOT_JPG = 0, LCBJPG, YMEAN_JPG, DEFECT_MID_JPG, DEFECT_LOW_JPG, MTF_JPG };
enum FIDUCIAL_MARK_TYPE { DOT_TYPE = 0, GRID_TYPE = 2 };

enum BOX_MOVE_TYPE { MOVE_POS = 0, MOVE_SIZE };
enum SFR_MOVE_TYPE { MOVE_UP = 0, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };

/* ���ϸ�Ī ��� */
enum PAT_RESULT { FIND_OK, FIND_ERR, NOMARK, RATE_ERR, ROTATE_ERR };
enum MOUSE_CURSOR { MOVE_ALL, MOVE_WIDTH_LEFT, MOVE_WIDTH_RIGHT, MOVE_HEIGHT_TOP, MOVE_HEIGHT_BOTTOM, MOVE_NW, MOVE_NE, MOVE_SW, MOVE_SE };
enum LAMP_NO { ALARM_ON, ALARM_OFF, AUTO_STOP, AUTO_RUN };

enum STEP_ID { STEP_LENS_SUPPLY, STEP_PCB_SUPPLY, STEP_OS_CHECK, STEP_RESIN_SUPPLY, STEP_CCD_ALIGN, STEP_CCD_INSP, STEP_UVLAMP, STEP_EMISSION };
enum STEP_RESULT { STEP_NORMAL, STEP_OK, STEP_NG };


enum TIMER_NUM { MAIN_TIMER = 1, MOTOR_TIMER, JOG_TIMER, IO_TIMER, LENS_TIMER, PCB_TIMER, RESIN_TIMER, MSG_TIMER, HOME_TIMER, CCD_DEFECT_TIMER, KILL_TIMER };


enum EPOXY_MODEL { CRICLE_EPOXY, RECT_EPOXY, POLYGON_EPOXY, POINT_EPOXY};

#define	MAX_MODEL_CNT			1000


enum	MOTOR_POS 
{
	Wait_Pos, Loading_Pos, Holder_Pos, Dispense_Pos, Lens_Pos,
	Laser_Pcb_Pos, Laser_Lens_Pos, Align_Pos, Bonding_Pos, Dark_Pos, Defect_Pos, OC_6500K_Pos, OC_2800K_Pos,
	Pcb_Loading_Pos, Cover_Loading_Pos, Cover_UnLoading_Pos, PDispense_Pos,
	Contact_Pos, Calc_Pos, Clean_Pos, Insp_Pos, Holder_Uv_Pos, ContiTailDelay_Pos,
	Com_Laser_Pos, Calc2_Pos, CompleteAlign_Pos,
	Dispense_Pos1, Dispense_Pos2, Dispense_Pos3,
	MAX_POS_NO

};

static char MotorPosName[MAX_POS_NO][50] = { "Wait Pos",	"Loading Pos",	"Holder Pos",	"Dispense Pos", "Lens Align",
"pcb Laser",		"Lens Laser",	"Sensor Align", "Bonding Pos", "Barcode Pos", "Defect Pos", "OC 6500K Pos" , "OC 2800K Pos",
"Pcb Load Pos" , "Cover Load Pos",	"Cover UnLoad Pos",	"PDispense Pos",
"Contact Pos", "Calc Pos", "Ready Pos", "�����˻� Pos", "OQLaser Pos", "TailDelay_Pos",
"����ǰLaserPos", "����Calc2 ��ġ", "ComAlign Pos","������������Pos", "Dispense Pos2", "Dispense Pos3" };


enum	MTF_POS { MTF_Center, MTF_left_upper, MTF_right_upper, MTF_left_bottom, MTF_right_bottom, MAX_MTF_NO };




static TCHAR* DEFECT_SPEC_NAME[26] =//[19] =
{ _T("AnBlockSizeHot"),	 _T("AnBlockSizeDark"),			_T("AnBlockSizeBright"),
_T("AnThresholdTypeHot"),		_T("AnThresholdTypeDark"),		_T("AnThresholdTypeBright"),
_T("AdDefectThresholdHot"),	_T("AdDefectThresholdDark"),	_T("AdDefectThresholdBright"),
_T("AnMaxHotDefectNum"),		_T("AnMaxDarkDefectNum"),		_T("AnMaxBrightDefectNum"),
_T("AnMaxHotClusterNum"),		_T("AnMaxDarkClusterNum"),		_T("AnMaxBrightClusterNum"),
_T("AnEnableChannel"),
_T("AnDefectInCluster"),
_T("ADefectInClusterChannel"),	//_T("AspecColumnFPN"),
_T("AEnableCircle"),
_T("APosOffsetX"),		_T("APosOffsetY"),
_T("ARadiusRatioX"),	_T("ARadiusRatioY"),
_T("AThresholdRatio"),	_T("AROIRange"),		_T("AUsediFixedCircle")
};

static TCHAR* pszXtype[5] = { _T("CENTER"), _T("TL"), _T("TR"), _T("BL"), _T("BR") };
static TCHAR* pszPlustype[5] = { _T("CENTER"), _T("T"), _T("B"), _T("L"), _T("R") };
//
static TCHAR* OC_SPEC_NAME[2] =
{
	_T("AnFiducialMarkNum"), _T("AnFiducialMarkType")
};
//
static TCHAR* COLOR_SENS_SPEC_NAME[8] =
{ _T("AdSpecDevMaxRG"),_T("AdSpecDevMinRG"),_T("AdSpecDevMaxBG"),
_T("AdSpecDevMinBG"),_T("AdSpecDevMaxGrGb"),_T("AdSpecDevMinGrGb")
,_T("AnAdjustType"),_T("AnTypicalValueType")
};
//
static TCHAR* SNR_SPEC_NAME[3] =
{ _T("AdSNRThreshold"), _T("AdSNRMIN"), _T("AdSNRMAX")
};
//


static TCHAR* UNIFORMITY_SPEC_NAME[7] =
{ _T("AnGridSizeX"),		_T("AnGridSizeY"),
_T("AnColorSpaceType"),		_T("AnMaxDiffType"),
_T("AspecMaxDiffRG"),		_T("AspecMaxDiffBG"),
_T("AspecMaxDiffRB")
};
//

static TCHAR* R_ILLUMINATION_SPEC_NAME[5] =
{ _T("AdSpecCenterIntensity1"),		_T("AdSpecCenterIntensity2"),		_T("AdSpecCenterIntensity3"),
_T("AdSpecCenterIntensity4"),		_T("AnSpecPixelCntInBlock")
};

static TCHAR* STAIN_BLEMISH_SPEC_NAME[20] =
{
	_T("AnBlockWidth"),			_T("AnBlockHeight"),		_T("AnClusterSize"),
	_T("AnDefectInCluster"),	_T("AdDefectRatio"),
	_T("AbEnableCircle"),		_T("AnPosOffsetX"),			_T("AnPosOffsetY"),
	_T("AdRadiusRatioX"),		_T("AdRadiusRatioY"),		_T("AdThresholdRatio"),
	_T("AdROIRange"),			_T("AnUsedFixedCircle"),	_T("AbEnableMultiCircle"),
	_T("AdZoneSizeRatio1"),		_T("AdZoneSizeRatio2"),		_T("AdZoneSizeRatio3"),
	_T("AdThreshold1"),			_T("AdThreshold2"),			_T("AdThreshold3")
};
const int LcbMaxCount = 35;


static TCHAR* STAIN_LCB_SPEC_NAME[LcbMaxCount] =
{
	_T("AdCenterThreshold"),		_T("AdEdgeThreshold"),		_T("AdCornerThreshold"),
	_T("AnMaxSingleDefectNum"),
	_T("AnMinDefectWidthHeigh"),
	_T("AnCenterMaxR"),				_T("AnCenterMaxGR"),			_T("AnCenterMaxGB"),	_T("AnCenterMaxB"),
	_T("AnEdgeMaxR"),				_T("AnEdgeMaxGR"),				_T("AnEdgeMaxGB"),			_T("AnEdgeMaxB"),
	_T("AnCornerMaxR"),			_T("AnCornerMaxGR"),			_T("AnCornerMaxGB"),		_T("AnCornerMaxB"),
	_T("AbEnableCircle"),		_T("AnPosOffsetX"),				_T("AnPosOffsetY"),
	_T("AdRadiusRatioX"),			_T("AdRadiusRatioY"),		_T("AdThresholdRatio"),			_T("AdROIRange"),
	_T("AnUsedFixedCircle"),		_T("AbEnableMultiCircle"),
	_T("AdZoneSizeRatio1"),		_T("AdZoneSizeRatio2"),		_T("AdZoneSizeRatio3"),
	_T("AdThreshold1"),				_T("AdThreshold2"),			_T("AdThreshold3"),
	_T("AnBlobSize1"),				_T("AnBlobSize2"),			_T("AnBlobSize3")
};
const int YmeanMaxCount = 28;
static TCHAR* STAIN_YMEAN_SPEC_NAME[YmeanMaxCount] =
{
	_T("AdEdgeSize"),				_T("AdCenterThreshold"),		_T("AdEdgeThreshold"),
	_T("AdCornerThreshold"),		_T("AnDefecBlockSize"),			_T("AnLscBlockSize"),
	_T("AnCalcType"),				_T("AnnMaxRecursiveCount"),     _T("AdMaxDefectSize"),
	_T("AdPixelSize"),
	_T("AbEnableCircle"),
	_T("AnPosOffsetX"),				_T("AnPosOffsetY"),
	_T("AdRadiusRatioX"),			_T("AdRadiusRatioY"),
	_T("AdThresholdRatio"),			_T("AdROIRange"),
	_T("AnUsedFixedCircle"),		_T("AbEnableMultiCircle"),
	_T("AdZoneSizeRatio1"),			_T("AdZoneSizeRatio2"),			_T("AdZoneSizeRatio3"),
	_T("AdThreshold1"),				_T("AdThreshold2"),				_T("AdThreshold3"),
	_T("AnBlobSize1"),				_T("AnBlobSize2"),			_T("AnBlobSize3")
};

const int FdfMaxCount = 30;
static TCHAR* STAIN_FDF_SPEC_NAME[FdfMaxCount] =		//30��
{
	_T("AdCenterThreshold"),		_T("AdEdgeThreshold"),		_T("AdCornerThreshold"),
	_T("AnMedianFilterSize"),		_T("AnLscBlockSize"),
	_T("AnWidthScaleRatio"),		_T("AnHeightScaleRatio"),
	_T("AnMinDefectWidthHeight"),	_T("AnWindowSize"),
	_T("AnEdgeSize"),				_T("AbEnableChannel"),
	_T("AnMaxSingleDefectNum"),
	_T("AbEnableCircle"),
	_T("AnPosOffsetX"),				_T("AnPosOffsetY"),
	_T("AdRadiusRatioX"),			_T("AdRadiusRatioY"),
	_T("AdThresholdRatio"),			_T("AdROIRange"),
	_T("AnUsedFixedCircle"),
	_T("AbEnableMultiCircle"),
	_T("AdZoneSizeRatio1"),			_T("AdZoneSizeRatio2"),			_T("AdZoneSizeRatio3"),
	_T("AdThreshold1"),				_T("AdThreshold2"),				_T("AdThreshold3"),
	_T("AnBlobSize1"),				_T("AnBlobSize2"),			_T("AnBlobSize3")
};

static TCHAR* CHART_SPEC_NAME[5] =
{
	_T("AdRealGapX"), _T("AdRealGapY"),
	_T("AnFiducialMarkType"),		_T("AdModuleChartDistance"),
	_T("AnDistortionAlrotithmType")
};
static TCHAR* RI_SPEC_NAME[8] =
{
	_T("AnSpecPixelCntInBlock"),    _T("AnEnableChannel"),
	_T("AdSpecCenterIntensity1"),_T("AdSpecCenterIntensity2"),_T("AdSpecCenterIntensity3"),_T("AdSpecCenterIntensity4")
	,_T("AnCornerVariationMin"),_T("AnCornerVariationMax")
};

static TCHAR* SATURATION_SPEC_NAME[2] =
{
	_T("AnSpecSaturationMin"),    _T("AnSpecSaturationMax")
};



enum	MARK_DIRECTION { LEFT_BLACK = 0, RIGHT_BLACK };			// ����ȣ 20140905 �߰�.. íƮ���� SFR �˻� ������ BOX�� �¿� �� ������� ���� ������?
enum	DIRECTION_NO { LEFT_DIR = 0, RIGHT_DIR, TOP_DIR, BOTTOM_DIR };


#define		MOTOR_Lens_Z_OFFSET			1		// 1mm  ���� ��ġ �̵��� 1mm ���� �� ��� ���� �̵� �Ϸ��� ���� ��ġ�� �̵�.

#define		RGB_DLG_BG						RGB(40, 39, 35)
#define		RGB_CTRL_BG						RGB(174,54,27)//RGB(55, 73, 93)


#define		GRID_COLOR_TITLE			RGB(0,155,155)
//
#define		GRID_COLOR_WHITE            RGB(255,255,255)
#define		GRID_COLOR_GREEN			RGB(128, 255, 128)
#define		GRID_COLOR_RED				RGB(255, 94, 164)

#define		GRID_COLOR_SPARE_MOTOR		RGB(128, 128, 128)
#define		GRID_COLOR_SELECT			RGB(244, 216, 234)

#define		GRID_COLOR_DIO_OUT			RGB(128, 255, 128)
#define		GRID_COLOR_DIO_IN			RGB(255, 94, 164)

#define		RGB_COLOR_WHITE					RGB(255, 255, 255)
#define		RGB_COLOR_BLACK					RGB(0, 0, 0)
#define		RGB_COLOR_RED					RGB(255, 0, 0)
#define		RGB_COLOR_GREEN					RGB(0, 255, 0)
#define		RGB_COLOR_BLUE					RGB(0, 0, 255)
#define		RGB_COLOR_GRAY					RGB(100, 100, 100)

#define		A_COLOR_BLACK                                 M_RGB888(0,0,0)
#define		A_COLOR_RED                                   M_RGB888(255,0,0)
#define		A_COLOR_GREEN                                 M_RGB888(0,255,0)
#define		A_COLOR_BLUE                                  M_RGB888(0,0,255)
#define		A_COLOR_YELLOW                                M_RGB888(255,255,0)
#define		A_COLOR_MAGENTA                               M_RGB888(255,0,255)
#define		A_COLOR_CYAN                                  M_RGB888(0,255,255)
#define		A_COLOR_WHITE                                 M_RGB888(255,255,255)
#define		A_COLOR_GRAY                                  M_RGB888(128,128,128)
#define		A_COLOR_BRIGHT_GRAY                           M_RGB888(192,192,192)
#define		A_COLOR_LIGHT_GRAY                            M_RGB888(160,160,164)
#define		A_COLOR_LIGHT_GREEN                           M_RGB888(192,220,192)
#define		A_COLOR_LIGHT_BLUE                            M_RGB888(166,202,240)
#define		A_COLOR_LIGHT_WHITE                           M_RGB888(255,251,240)
#define		A_COLOR_DARK_RED                              M_RGB888(128,0,0)
#define		A_COLOR_DARK_GREEN                            M_RGB888(0,128,0)
#define		A_COLOR_DARK_BLUE                             M_RGB888(0,0,128)
#define		A_COLOR_DARK_YELLOW                           M_RGB888(128,128,0)
#define		A_COLOR_DARK_MAGENTA                          M_RGB888(128,0,128)
#define		A_COLOR_DARK_CYAN                             M_RGB888(0,128,128)

#define CV_LAVENDER					CV_RGB(230,230,250)
#define CV_GREEN					CV_RGB(0,128,0)
#define CV_LIGHTGREEN				CV_RGB(144,238,144)
#define CV_YELLOW					CV_RGB(255,255,0)
#define CV_BLACK					CV_RGB(0,0,0)
#define CV_RED						CV_RGB(255,0,0)
#define CV_BLUE						CV_RGB(0,0,255)

#define		MOTOR_GAP				0.1				// Encoder  ���� ��� ����

#define		MOTOR_ORIGIN_TIME	    320000			// msec..// test��
#define		MOTOR_MOVE_TIME			50000			// msec..//test��
#define		MOTOR_STOP_TIME			1000			// msec..
#define		IO_CHECK_TIME			3000			// msec..

#define		MOTOR_JOG_LOW			0.1				//JOG_SPEED_LOW
#define		MOTOR_JOG_MID			0.5				//JOG_SPEED_MID
#define		MOTOR_JOG_HIGH			1.0				//JOG_SPEED_HIGH

#ifndef M_PI
#define M_PI       	3.14159265358979323846f
#endif

#define M_PI_2     	1.57079632679489661923f
#define M_PI_4     	0.785398163397448309616f
#define M2_PI      	(M_PI+M_PI)
//! <--------------------------------------------------------------------------
//! ���������� Motion Controller ���� ��ü
#include "AxlMotion/AXTDefine.h"
#include "AxlMotion/AxlMotorSet.h"
#include "AxlMotion/FlowProcess.h"



class mCDPoint
{
public:
	double x, y;
	mCDPoint()
	{
		x = y = 0.0;
	}
};



#endif