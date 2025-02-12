#include "StdAfx.h"
#include "LightControl.h"

#include <cstddef>  // std::byte
CLightControl::CLightControl(void)
{
	m_iRecvSize = 0;
	memset(m_acRecvStorage, 0, sizeof(m_acRecvStorage));
		
	m_iNo_Machine = -1;
}


CLightControl::~CLightControl(void)
{
	Close_Device();
}

//! RS-232C ����� ����� �Ŀ� �� ó�� 1�� �� �����ؾ� �� ����� �����ϴ�. 
void CLightControl::ReadyDevice()
{
}

//! ���� ó�� ��ü �Լ�
void CLightControl::ProcessReceiveData(void *pProcPtr, BYTE *pbyBuff, int iSize, bool bFlag_Active, int iCnt_Recv)
{
	if ( m_iNo_Machine < 0 )
	{
		return;
	}
	if ( pProcPtr == NULL || pbyBuff == NULL || iSize <= 0 )
	{
		return;
	}
	
	TCHAR acRecvTmp[8192];
	memset(acRecvTmp, 0, sizeof(acRecvTmp));
	_tcscpy_s(acRecvTmp, m_acRecvStorage);	//! ���� �Լ� ȣ�� ���� ���� �����͸� �����´�. 

	int iRecvCnt = m_iRecvSize;
	if ( iRecvCnt < 0 )
	{
		iRecvCnt = 0;
	}

	int i = 0;
	for ( i = 0; i < iSize; i++ )
	{
		if ( (pbyBuff[i] == ']')  && (iRecvCnt == 0) )		//! ����
		{
			memset(acRecvTmp, 0, sizeof(acRecvTmp));
			iRecvCnt = 0;			
		}
		else if ( (pbyBuff[i] == ']') && (iRecvCnt > 0) )	//! ��ħ
		{
			acRecvTmp[iRecvCnt] = 0x00;
			iRecvCnt++;

			if ( bFlag_Active == true )
			{
				//! �غ�� ���� ���ڿ��� ������ ����Ѵ�. 
				OutputRecvData(pProcPtr, acRecvTmp, iRecvCnt, iCnt_Recv);
			}

			memset(acRecvTmp, 0, sizeof(acRecvTmp));
			iRecvCnt = 0;			
		}
		else
		{
			acRecvTmp[iRecvCnt] = (TCHAR)(pbyBuff[i]);	//! pbyBuff -> acRecvTmp ���۷� �����͸� �ű��. 
			iRecvCnt++;
		}
	}

	acRecvTmp[iRecvCnt] = 0x00;
	m_iRecvSize = iRecvCnt;

	memset(m_acRecvStorage, 0, sizeof(m_acRecvStorage));
	_tcscpy_s(m_acRecvStorage, acRecvTmp);	//! ó���ϰ� ���� �����͸� ���� �Լ� ȣ�⶧�� ���� ������ �д�. 
}

//! [���] ���� ���� ���ڿ��� Parsing�ϰų�, �ٸ� ���� �����Ѵ�.  
void CLightControl::OutputRecvData(void *pProcPtr, TCHAR *pcBuff, int iDataSize, int iCnt_Recv)
{
	//! TCHAR* -> CString 
	CString sRecv = _T("");
	sRecv.Format(_T("%s"), pcBuff);

	int iStringLength = (int)(sRecv.GetLength());
	
	if ( pProcPtr != NULL )
	{
	}
}

//! ���� ó�� �� ��ó�� �Լ�
void CLightControl::PostProcRecvData()
{
}

//! ���� ��Ʈ�ѷ��� ����
//! [���� ����] ���� �����Ͱ� ���� ��쿡��, �ݵ��, SetReceiveProcPtr �Լ��� ���� ó���� �� Dialog�� GUI Ŭ������ �����ϰ�, �� �Լ��� ȣ���Ѵ�. 
bool CLightControl::Connect_Device(CString sPort, int iNoMachine)
{
	if ( iNoMachine < 0 )
	{
		return false;
	}
	/*if ( sPort.GetLength() < 8 )
	{
		return false;
	}*/
	if ( sPort.Find(_T("COM"), 0) < 0 )
	{
		return false;
	}

	strSetupInfo_RS232C strSerial;
	strSerial.InitInfo();
	strSerial.dwBaudRate   = CBR_19200;
	strSerial.byDataBit    = 8;
	strSerial.byStopBit = ONESTOPBIT;
	strSerial.byParityBit  = NOPARITY;
	strSerial.sPortName    = sPort;
	strSerial.eFlowControl = EHandshakeOff;
	strSerial.bFlag_Close_COM_Port   = true;
	strSerial.bFlag_Use_CPU_Affinity = false;
	strSerial.bFlag_Recv_ThreadStart = false;	//! ���� ������ ó���� Thread�� �������� ����
	strSerial.iTimeOut_Close_Thread  = 3000;
	int iReturn = OpenPort(&strSerial);

	if ( iReturn < 0 )
	{
		return false;
	}

	m_iNo_Machine = iNoMachine;

	return true;
}

//! ���� ����
void CLightControl::Close_Device()
{
	ClosePort();

	m_iNo_Machine = -1;
}

//! ���ڿ� ���� ����
int CLightControl::SendData_Light_Controller(BYTE *pbySend, int iSendSize)
{
	if ( pbySend == NULL || iSendSize <= 0 )
	{
		return -1;
	}
	if ( m_iNo_Machine < 0 )
	{
		return -2;
	}
	
	BYTE bySendBuf[256];
	memset(bySendBuf, 0x00, sizeof(bySendBuf));

	int i = 0;
	DWORD dwSendCnt = 0;
	
	//bySendBuf[dwSendCnt++] = 0x0D;
	for ( i = 0; i < iSendSize; i++ )
	{
		bySendBuf[dwSendCnt++] = pbySend[i];
	}
	//bySendBuf[dwSendCnt++] = 0x0D;

	DWORD dwRet = WriteComm(bySendBuf, dwSendCnt);
	if ( dwRet < dwSendCnt )
	{
		return -3;
	}

	return 1;
}

//! ���ڿ� ���� ����
int CLightControl::SendData_Light_Controller(CString sSend)
{
	int iStringSize = sSend.GetLength();
	if ( iStringSize <= 0 )
	{
		return -1;
	}

	BYTE bySendBuffer[256];
	memset(bySendBuffer, 0x00, sizeof(bySendBuffer));

	int i = 0;
	for ( i = 0; i < iStringSize; i++ )
	{
		bySendBuffer[i] = (BYTE)(sSend.GetAt(i));
	}

	int iRet = SendData_Light_Controller(bySendBuffer, iStringSize);
	if ( iRet < 0 )
	{
		return (-10 + iRet);
	}

	return 1;
}

//! Channel On/Off
//! [���� ����] 'iNoChannel' ���� ���� ��Ʈ�ѷ��� ���� ä�� �Է°��̴�. 
int CLightControl::SetChannel_OnOff(int iNoChannel, bool bSwitch_On)
{
	CString sSend = _T("");
	sSend.Format(_T("]%02d"), iNoChannel);

	if ( bSwitch_On == true )
	{
		sSend += _T("1");
	}
	else
	{
		sSend += _T("0");
	}

	int iRet = SendData_Light_Controller(sSend);
	if ( iRet < 0 )
	{
		return (-10 + iRet);
	}

	return 1;
}

//! �� Channel�� ��Ⱚ(PWM��) ����
//! [���� ����] 'iNoChannel' ���� ���� ��Ʈ�ѷ��� ���� ä�� �Է°��̴�. 
int CLightControl::SetChannel_Value(int iNoChannel, int iValue)
{
	if ( iValue < 0 )
	{
		iValue = 0;
	}
	if ( iValue > 255 )
	{
		iValue = 255;
	}

	CString sSend = _T("");
	sSend.Empty();
	sSend.Format(_T("[%02d%03d"), iNoChannel, iValue);//sSend.Format(_T("[%02d%03d"), iNoChannel, iValue);

	int iRet = SendData_Light_Controller(sSend);
	if ( iRet < 0 )
	{ 
		return (-10 + iRet);
	}

	return 1;
}
#if 0
int CLightControl::DPS_SetChannel_Value(int iNoChannel, int iValue)
{
	if (iValue < 0)
	{
		iValue = 0;
	}
	if (iValue > 999)	//255)
	{
		iValue = 999;// 255;
	}
	int nSendSize;
	int nRetVal;
	int nIndex;
#if 1
	BYTE sSendBuff[100];
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	sSendBuff[0] = 0x59;		//ASCII 'Y'  Hex Code 0x59 �� ����
	sSendBuff[1] = 0x07;		//Header ~ Checksum������ byte�� , 7�� ����
	sSendBuff[2] = iNoChannel;			//ä�� 0x01 ~ 0x08 , 1ä�θ��� ��� 0x01�� ����
	sSendBuff[3] = 0x31;			//On = 0x31 , Off = 0x30 , Ready Check = 0x3F
	sSendBuff[4] = (char)((iValue >> 8) & 0x00FF);		//Value-0 , MSB, OFF�ΰ�� ���ǹ̾���, 1024 Level�� ��� 0��0000 ~ 0x03ff���� ��밡��
	sSendBuff[5] = (char)(iValue & 0x00FF);;		//Value-1 , LSB, OFF�ΰ�� ���ǹ̾���
	sSendBuff[6] = 0x00;
	for (int i = 0; i < 6; i++)
	{
		sSendBuff[6] += sSendBuff[i];		//Checksum = Header ~ Value-1������ ��
	}

	nSendSize = 7;



#else
	CString sSend = _T("");
	sSend.Empty();
	sSend.Format(_T("[%02d%03d"), iNoChannel, iValue);
#endif
	int iRet = SendData_Light_Controller(sSendBuff, nSendSize);
	if (iRet < 0)
	{
		return (-10 + iRet);
	}

	return 1;
}

bool CLightControl::DPS_Light_OnOffLevel(int channel, int onoff, int data)
{
	//Level�� 0 ~ 1023
	unsigned char sSendBuff[100];
	int nSendSize = 0;
	char mch = 0x1 + channel;
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	sSendBuff[0] = 0x59;								//Header
	sSendBuff[1] = 0x07;								//Length
	sSendBuff[2] = (char)(mch & 0x00FF);	//Channel (0x1 ~ 0x4)
	sSendBuff[3] = (onoff == 0) ? 0x30 : 0x31;			//Command
	sSendBuff[4] = (char)((data >> 8) & 0x00FF);	//Data-0 [MSB]
	sSendBuff[5] = (char)(data & 0x00FF);		//Data-1 [LSB]
	sSendBuff[6] = 0x00;								//BCC

	for (int i = 0; i < 6; i++)
	{
		sSendBuff[6] += sSendBuff[i];
	}
	nSendSize = 7;

	Sleep(100);
	int nRetVal = SendData_Light_Controller(sSendBuff, nSendSize);
	if (nRetVal != nSendSize)
	{
		return false;
	}
	return true;
}

int CLightControl::DPS_SetChannel_Value2(int iNoChannel, int iValue)
{
	if (iValue < 0)
	{
		iValue = 0;
	}
	if (iValue > 999)	//255)
	{
		iValue = 999;// 255;
	}
	int nSendSize;
	int nRetVal;
	int nIndex;
#if 1
	BYTE sSendBuff[100];
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	sSendBuff[0] = 0x59;		//ASCII 'Y'  Hex Code 0x59 �� ����
	sSendBuff[1] = 0x07;		//Header ~ Checksum������ byte�� , 7�� ����
	sSendBuff[2] = iNoChannel;			//ä�� 0x01 ~ 0x08 , 1ä�θ��� ��� 0x01�� ����
	sSendBuff[3] = 0x31;			//On = 0x31 , Off = 0x30 , Ready Check = 0x3F
	sSendBuff[4] = (char)((iValue >> 8) & 0x00FF);		//Value-0 , MSB, OFF�ΰ�� ���ǹ̾���, 1024 Level�� ��� 0��0000 ~ 0x03ff���� ��밡��
	sSendBuff[5] = (char)(iValue & 0x00FF);;		//Value-1 , LSB, OFF�ΰ�� ���ǹ̾���
	sSendBuff[6] = 0x00;
	for (int i = 0; i < 6; i++)
	{
		sSendBuff[6] += sSendBuff[i];		//Checksum = Header ~ Value-1������ ��
	}

	nSendSize = 7;



#else
	CString sSend = _T("");
	sSend.Empty();
	sSend.Format(_T("[%02d%03d"), iNoChannel, iValue);
#endif
	int iRet = SendData_Light_Controller(sSendBuff, nSendSize);
	if (iRet < 0)
	{
		return (-10 + iRet);
	}

	return 1;
}

bool CLightControl::DPS_Light_OnOffLevel2(int channel, int onoff, int data)
{
	//Level�� 0 ~ 1023
	unsigned char sSendBuff[100];
	int nSendSize = 0;
	char mch = 0x1 + channel;
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	sSendBuff[0] = 0x59;								//Header
	sSendBuff[1] = 0x07;								//Length
	sSendBuff[2] = (char)(mch & 0x00FF);	//Channel (0x1 ~ 0x4)
	sSendBuff[3] = (onoff == 0) ? 0x30 : 0x31;			//Command
	sSendBuff[4] = (char)((data >> 8) & 0x00FF);	//Data-0 [MSB]
	sSendBuff[5] = (char)(data & 0x00FF);		//Data-1 [LSB]
	sSendBuff[6] = 0x00;								//BCC

	for (int i = 0; i < 6; i++)
	{
		sSendBuff[6] += sSendBuff[i];
	}
	nSendSize = 7;

	Sleep(100);
	int nRetVal = SendData_Light_Controller(sSendBuff, nSendSize);
	if (nRetVal != nSendSize)
	{
		return false;
	}
	return true;
}
# endif

void CLightControl::ctrlLedVolume(int iChannel, int iValue)
{
	int chNo = 0;
	Sleep(10);
	chNo = iChannel + 1;


	chNo = iChannel + 1;
	SetChannel_OnOff(chNo, true);
	Sleep(5);
	SetChannel_Value(chNo, iValue);

	/*if (myNum == 0)
	{
		DPS_SetChannel_Value(chNo, iValue);
	}*/


}

void CLightControl::SideChartctrlLedVolume(int iChannel, int iValue)
{
	int chNo = 0;
	Sleep(10);
	chNo = iChannel + 1;
	
	

	chNo = iChannel + 1;
	SetChannel_OnOff(chNo, true);
	Sleep(5);
	SetChannel_Value(chNo, iValue);
}

bool CLightControl::allControl(int iChannel , bool onOff)
{
	byte sendData[100];
	memset(sendData, 0x00, sizeof(sendData));
	CString temp = _T("");
	temp.Format("%X", 255);
	sendData[0] = 0x02;
	sendData[1] = 0x50;
	//
	if (onOff == true)
	{
		sendData[2] = model.m_iLedValue[LEDDATA_TOP1_CHART];
		sendData[3] = model.m_iLedValue[LEDDATA_TOP1_CHART];
		sendData[4] = model.m_iLedValue[LEDDATA_TOP1_CHART];
		sendData[5] = model.m_iLedValue[LEDDATA_TOP1_CHART];
	}

	//
	sendData[6] = 0x03;
	int iRet = SendData_Light_Controller(sendData , 7);
	if (iRet < 0)
	{
		return false;
	}

	return true;
}

bool CLightControl::ChartAllControl(bool onOff)
{
	byte sendData[100];
	memset(sendData, 0x00, sizeof(sendData));
	int i = 0;
	CString temp = _T("");
	temp.Format("%X", 255);
	sendData[0] = 0x02;	//START
	sendData[1] = 0x50;	//������
	//
	if (onOff == true)
	{
		sendData[2] = 0;	//ch1 oc
		sendData[3] = 0;	//ch2 pcb align
		if (onOff == true)
		{
			//sendData[4] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_CHART_L]);	//ch3 Left Chart
			//sendData[5] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_CHART_R]);	//ch4 Right Chart
			//sendData[6] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP1_CHART]);	//ch5 Top1
			//sendData[7] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP2_CHART]);	//ch6 Top2
			//sendData[8] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP3_CHART]);	//ch7 Top3
			//sendData[9] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP4_CHART]);	//ch8 Top4
			//sendData[10] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP5_CHART]);	//ch9 Top5
			//sendData[11] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_TOP6_CHART]);	//ch10 Top6
			for (i = 0; i < 8; i++)
			{
				sendData[4 + i] = static_cast<unsigned char>(model.m_iLedValue[LEDDATA_CHART_L + i]);	//ch3 Left Chart
			}
		}
		else
		{
			for (i = 0; i < 8; i++)
			{
				sendData[4 + i] = 0x00;
			}
		}
		sendData[12] = 0;	//ch11 Lens Align
	}

	//
	sendData[13] = 0x03;		//ETX
	int iRet = SendData_Light_Controller(sendData, 14);
	if (iRet < 0)
	{
		return false;
	}

	return true;
}


