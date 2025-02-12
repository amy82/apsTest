#include "StdAfx.h"
#include "UVControl.h"
CUVControl::CUVControl(void)
{
	m_iRecvSize = 0;
	memset(m_acRecvStorage, 0, sizeof(m_acRecvStorage));
		
	m_iNo_Machine = -1;
}


CUVControl::~CUVControl(void)
{
	Close_Device();
}

//! RS-232C ����� ����� �Ŀ� �� ó�� 1�� �� �����ؾ� �� ����� �����ϴ�. 
void CUVControl::ReadyDevice()
{
}

//! ���� ó�� ��ü �Լ�
void CUVControl::ProcessReceiveData(void *pProcPtr, BYTE *pbyBuff, int iSize, bool bFlag_Active, int iCnt_Recv)
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
void CUVControl::OutputRecvData(void *pProcPtr, TCHAR *pcBuff, int iDataSize, int iCnt_Recv)
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
void CUVControl::PostProcRecvData()
{
}

//! ���� ��Ʈ�ѷ��� ����
//! [���� ����] ���� �����Ͱ� ���� ��쿡��, �ݵ��, SetReceiveProcPtr �Լ��� ���� ó���� �� Dialog�� GUI Ŭ������ �����ϰ�, �� �Լ��� ȣ���Ѵ�. 
bool CUVControl::Connect_Device(CString sPort, int iNoMachine)
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
	strSerial.dwBaudRate   = CBR_9600;
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
	/*if (myNum == 0)
	{
		DPS_Light_OnOffLevel(0, 1, 1);
	}*/
	
	return true;
}

//! ���� ����
void CUVControl::Close_Device()
{
	ClosePort();

	m_iNo_Machine = -1;
}

//! ���ڿ� ���� ����
int CUVControl::SendData_Light_Controller(BYTE *pbySend, int iSendSize)
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
int CUVControl::SendData_Light_Controller(CString sSend)
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
int CUVControl::SetChannel_OnOff(int iNoChannel, bool bSwitch_On)
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
int CUVControl::SetChannel_Value(int iNoChannel, int iValue)
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

int CUVControl::DPS_SetChannel_Value(int iNoChannel, int iValue)
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

bool CUVControl::DPS_Light_OnOffLevel(int channel, int onoff, int data)
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

int CUVControl::DPS_SetChannel_Value2(int iNoChannel, int iValue)
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

bool CUVControl::DPS_Light_OnOffLevel2(int channel, int onoff, int data)
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

void CUVControl::ctrlLedVolume(int iChannel, int iValue)
{
	int chNo = 0;
	Sleep(10);
	chNo = iChannel + 1;
	
	if (myNum == 0)
	{
		DPS_SetChannel_Value(chNo, iValue);
	}

	chNo = iChannel + 1;
	SetChannel_OnOff(chNo, true);
	Sleep(5);
	SetChannel_Value(chNo, iValue);

}

void CUVControl::SideChartctrlLedVolume(int iChannel, int iValue)
{
	int chNo = 0;
	Sleep(10);
	chNo = iChannel + 1;
	
	if (myNum == 0)
	{
		DPS_SetChannel_Value(chNo, iValue);
	}
	

	chNo = iChannel + 1;
	SetChannel_OnOff(chNo, true);
	Sleep(5);
	SetChannel_Value(chNo, iValue);
}

bool CUVControl::allControl(int iChannel , bool onOff)
{
	

	return true;
}

bool CUVControl::Altis_UV_Connect(int data)
{
	unsigned char sSendBuff[100];
	int nSendSize = 0;
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	CString sSend = _T("");



	if (data == 0)
	{
		//FF 03 00 00 00 05 90 17  
		sSendBuff[0] = 0xFF;								//Header
		sSendBuff[1] = 0x03;								//Length
		sSendBuff[2] = 0x00;
		sSendBuff[3] = 0x00;
		sSendBuff[4] = 0x00;
		sSendBuff[5] = 0x05;
		sSendBuff[6] = 0x90;
		sSendBuff[7] = 0x17;
		//
		//

		//sSend = _T("FF03000000059017");

	}
	else if (data == 1)
	{
		//FF 03 01 00 00 2B 11 F7   
		sSendBuff[0] = 0xFF;								//Header
		sSendBuff[1] = 0x03;								//Length
		sSendBuff[2] = 0x01;
		sSendBuff[3] = 0x00;
		sSendBuff[4] = 0x00;
		sSendBuff[5] = 0x2B;
		sSendBuff[6] = 0x11;
		sSendBuff[7] = 0xF7;
		//sSend = _T("FF030100002B11F7");

	}
	else if (data == 2)
	{
		//FF 03 02 00 00 2B 11 B3  
		sSendBuff[0] = 0xFF;								//Header
		sSendBuff[1] = 0x03;								//Length
		sSendBuff[2] = 0x02;
		sSendBuff[3] = 0x00;
		sSendBuff[4] = 0x00;
		sSendBuff[5] = 0x2B;
		sSendBuff[6] = 0x11;
		sSendBuff[7] = 0xB3;
		//sSend = _T("FF030200002B11B3");

	}
	else if (data == 3)
	{
		//FF 03 03 00 00 2B 10 4F   
		sSendBuff[0] = 0xFF;								//Header
		sSendBuff[1] = 0x03;								//Length
		sSendBuff[2] = 0x03;
		sSendBuff[3] = 0x00;
		sSendBuff[4] = 0x00;
		sSendBuff[5] = 0x2B;
		sSendBuff[6] = 0x10;
		sSendBuff[7] = 0x4F;
		//sSend = _T("FF030300002B104F");

	}
	else if (data == 4)
	{
		//FF 03 04 00 00 2B 11 3B
		sSendBuff[0] = 0xFF;								//Header
		sSendBuff[1] = 0x03;								//Length
		sSendBuff[2] = 0x04;
		sSendBuff[3] = 0x00;
		sSendBuff[4] = 0x00;
		sSendBuff[5] = 0x2B;
		sSendBuff[6] = 0x11;
		sSendBuff[7] = 0x3B;
		//sSend = _T("FF030400002B113B");
	}

	//int iRet = SendData_Light_Controller(sSend);
	nSendSize = 8;
	int iRet = SendData_Light_Controller(sSendBuff , nSendSize);
	if (iRet < 0)
	{
		return false;
	}

	return true;
	/*nSendSize = 8;

	Sleep(100);
	int nRetVal = SendData_UV_Controller(sSendBuff, nSendSize);
	if (nRetVal < 0)
	{
	return false;
	}

	return true;*/
}


bool CUVControl::Altis_UC_Controller_OnOff(int channel, int onoff)
{
	unsigned char sSendBuff[100];
	int nSendSize = 0;
	char mch = 0x00 + channel;
	memset(sSendBuff, 0x00, sizeof(sSendBuff));
	if (channel == 1)
	{
		if (onoff == 1)
		{
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x01;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x01;
			sSendBuff[9] = 0x70;
			sSendBuff[10] = 0x9A;
		}
		else
		{
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x01;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x00;
			sSendBuff[9] = 0xB1;
			sSendBuff[10] = 0x5A;
		}
	}
	if (channel == 2)
	{
		if (onoff == 1)					//01 10 02 2a 00 01 02 00 01 43 9a 2ä��
		{
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x02;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x01;
			sSendBuff[9] = 0x43;
			sSendBuff[10] = 0x9A;
		}
		else
		{								//	01 10 02 2A 00 01 02 00 00 82 5A
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x02;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x00;
			sSendBuff[9] = 0x82;
			sSendBuff[10] = 0x5A;
		}
	}
	if (channel == 3)
	{
		if (onoff == 1)					//01 10 03 2a 00 01 02 00 01 53 5a 3ä��
		{
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x03;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x01;
			sSendBuff[9] = 0x53;
			sSendBuff[10] = 0x5A;
		}
		else
		{									//01 10 03 2A 00 01 02 00 00 92 9A 
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x03;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x00;
			sSendBuff[9] = 0x92;
			sSendBuff[10] = 0x9A;
		}
	}
	if (channel == 4)
	{
		if (onoff == 1)					//01 10 04 2a 00 01 02 00 01 25 9a 4ä��
		{
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x04;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x01;
			sSendBuff[9] = 0x25;
			sSendBuff[10] = 0x9A;
		}
		else
		{								//	01 10 04 2A 00 01 02 00 00 E4 5A
			sSendBuff[0] = 0x01;
			sSendBuff[1] = 0x10;
			sSendBuff[2] = 0x04;
			sSendBuff[3] = 0x2A;
			sSendBuff[4] = 0x00;
			sSendBuff[5] = 0x01;
			sSendBuff[6] = 0x02;
			sSendBuff[7] = 0x00;
			sSendBuff[8] = 0x00;
			sSendBuff[9] = 0xE4;
			sSendBuff[10] = 0x5A;
		}
	}
	nSendSize = 11;

	///01 10 01 2A 00 01 02 00 00 B1 5A
	//
	//sSendBuff[11] = 0x0A;
	//sSendBuff[12] = 0x0D;
	//01 10 01 2A 00 01 02 00 01 70 9A 

	Sleep(100);
	int nRetVal = SendData_Light_Controller(sSendBuff, nSendSize);
	if (nRetVal < 0)
	{
	return false;
	}

	return true;
}