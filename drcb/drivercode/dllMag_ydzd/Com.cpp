// Com.cpp: implementation of the CCom class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Com.h"
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCom::CCom()
{
	hCommDev = INVALID_HANDLE_VALUE;
	return ;
}

//L: �� overlaped �ṹ

CCom::CCom(
	BYTE Port,					/* 1-4 = COM1, COM2, COM3, COM4 */
	DWORD BaudRate,				/* actual baud rate value       */
    	BYTE Parity,			/* 0-4 = no,odd,even,mark,space */
        BYTE ByteSize,			/* number of bits/byte, 4-8     */
        BYTE StopBits,			/* 0,1,2 = 1, 1.5, 2            */
		BYTE HandShaking,		/* I added these code */
		BOOL bComm
        )
{
	hCommDev = INVALID_HANDLE_VALUE;
	if( Port == (BYTE)-1) return;

	//�˴��Ѿ����ô�COM PORT�Ĳ���
	if( CCom::OpenSerialPort(
		Port,					/* 1-4 = COM1, COM2, COM3, COM4 */
		BaudRate,				/* actual baud rate value       */
		Parity,					/* 0-4 = no,odd,even,mark,space */
		ByteSize,				/* number of bits/byte, 4-8     */
		StopBits,				/* 0,1,2 = 1, 1.5, 2            */
		HandShaking,			/* HandShaking */
		bComm
		) != 0)
	{
		hCommDev = INVALID_HANDLE_VALUE;
	}
}

CCom::~CCom()
{
	//if (iSendAsc) {Sleep(100);iSendAsc = 0;}
	if ( hCommDev != INVALID_HANDLE_VALUE )
	{
		//KeyboardControl(1);	//���⴦��
		if( CloseHandle(hCommDev))
			hCommDev = INVALID_HANDLE_VALUE;
	}
}


//�򿪴��ڲ���
long CCom::OpenSerialPort(
						  BYTE Port,			/* 1-4 = COM1, COM2, COM3, COM4 */
						  DWORD BaudRate,		/* actual baud rate value       */
						  BYTE Parity,			/* 0-4 = no,odd,even,mark,space */
						  BYTE ByteSize,		/* number of bits/byte, 4-8     */
						  BYTE StopBits,		/* 0,1,2 = 1, 1.5, 2            */
						  BYTE HandShaking,		/* HandShaking 0-comNone,1-comXOnXOff,2-comRTS,3-comRTSXOnXOff */
						  BOOL bComm
						  )
{
	DCB		dcb;
	DWORD	dwError;
	BOOL	fSucess;
	COMMTIMEOUTS    CommTimeOuts;
	
	// �ر��Ѿ��򿪵��豸
	if( Port == -1) return COMERROR;
	if ( hCommDev != INVALID_HANDLE_VALUE )
	{
		DWORD errors; 
		COMSTAT stat; 
		ClearCommError( hCommDev, &errors, &stat);
		if( CloseSerialPort() != 0)
			return COMERROR;
		else hCommDev = INVALID_HANDLE_VALUE;
	}
	
	// �򿪴���
	char Dev[16] = {0};
	if(bComm)
	{
		if (Port > 9) sprintf(Dev,"\\\\.\\COM%d",Port);
	    else sprintf(Dev,"COM%d",Port);
	}
    else
	{
		sprintf(Dev, "LPT%d", Port);
	}
	hCommDev = CreateFile((LPCTSTR)Dev,
		GENERIC_READ | GENERIC_WRITE,
		0,							/* comm devices must be opend with exclusive acess */
		NULL,						/* no security attrs */
		OPEN_EXISTING,		        /* comm devices must use OPEN_EXISTING */
		0,							/* not overlapped I/O */
		NULL						/* hTemplate must be NULL for comm devices */
		);
	
	if ( hCommDev == INVALID_HANDLE_VALUE )
	{
		dwError = GetLastError();
		switch(dwError)
		{
		case 2://�豸û�б���ʼ��
			return COMERROR;
		case 5://�˿ڱ������豸��
			return COMERROR;
		}
	}
	fSucess = GetCommState( hCommDev, &dcb);
	if ( fSucess == FALSE )
	{
		dwError = GetLastError();
		return COMERROR;
	}
	//DCB��ı�����������ݴ����ʡ���żУ�鷽���Լ�����λ��ֹͣλ
	//���SetCommState�������õ�DCB�ṹ�е�XonChar��Ա����XoffChar��Ա�����������ʧ��
	dcb.fBinary = true;
	dcb.BaudRate = BaudRate;
	dcb.ByteSize = ByteSize;
	dcb.Parity = Parity;
	dcb.StopBits = StopBits;
	
	if(dcb.Parity==NOPARITY)
	{
		dcb.fParity=false;
	}
	else
	{
		dcb.fParity=true;
	}
	
	// �ر����е�������
	// >>
	// �ر��������
	dcb.fOutX=FALSE;
	dcb.fInX=FALSE;
	
	// �ر�Ӳ������	***
	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
	dcb.fOutxCtsFlow = FALSE;
	
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fOutxDsrFlow = FALSE;

	//dcb.fDtrControl = DTR_CONTROL_ENABLE;
	//dcb.fOutxDsrFlow = TRUE;

	// <<
	
	// ѡ����Ӧ��������
	// >>
	switch(HandShaking)
	{
	case comNone:
		break;
	case comXOnXOff:
		dcb.fOutX=TRUE;
		dcb.fInX=TRUE;
		break;
	case comRTS:
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fOutxCtsFlow = TRUE;
		break;
	case comRTSXOnXOff:
		dcb.fOutX=TRUE;
		dcb.fInX=TRUE;
		dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
		dcb.fOutxCtsFlow = TRUE;
		break;
	default:
		break;
	}
	//  <<
	fSucess = SetCommState(hCommDev, &dcb);
	
	if ( fSucess == FALSE )
	{
		dwError = GetLastError();
		return COMERROR;
	}
	
	PurgeComm( hCommDev, PURGE_TXCLEAR|PURGE_RXCLEAR);

	//��������
	//SetCommMask(hCommDev, EV_RXCHAR|EV_TXEMPTY );
	SetCommMask( hCommDev,EV_RXCHAR );
	//����buffer  ��������BOOL SetupComm(HANDLE hFile,DWORD dwInQueue,DWORD dwOutQueue); ������պͷ��ͻ������Ĵ�С
	SetupComm( hCommDev,4096,4096 );
	//���buffer���κ���Ϣ
	PurgeComm( hCommDev,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	//���ó�ʱ����
	DWORD dwTemp = 1000 / (BaudRate / 8);
	CommTimeOuts.ReadIntervalTimeout = 100;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier=2*	CBR_9600/BaudRate;
	CommTimeOuts.WriteTotalTimeoutConstant =0;
	SetCommTimeouts( hCommDev ,&CommTimeOuts);

	return COMOK;
}


//�رմ���
long CCom::CloseSerialPort()
{
	if( hCommDev != INVALID_HANDLE_VALUE )
	{
		DWORD errors; 
		COMSTAT stat; 
		ClearCommError( hCommDev, &errors, &stat);

		if( CloseHandle(hCommDev))
		{
			hCommDev = INVALID_HANDLE_VALUE;
			return COMOK;
		}
		else return COMERROR;
	}
	else return COMOK;
}

//L: ������ջ�����
void CCom::PurgeRecvBuf()
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return;
	
	DWORD errors; 
	COMSTAT stat; 
	ClearCommError( hCommDev, &errors, &stat);
	//������ջ�����
	PurgeComm( hCommDev, PURGE_RXCLEAR);
}

//L: ������ͻ�����
void CCom::PurgeSndBuf()
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return;

	DWORD errors; 
	COMSTAT stat; 
	ClearCommError( hCommDev, &errors, &stat);
	//X������ͻ�����
	/************************************************************************X
	PurgeComm( hCommDev, PURGE_TXCLEAR);
	PurgeComm ���������ڶ�д������ͬʱ����ջ���������Ӧ�ó����ڶ�д����ʱ����PurgeComm���������ܱ�֤�������е������ַ��������͡�
	���Ҫ��֤�������������ַ��������ͣ�Ӧ�õ���FlushFileBuffers�������ú���ֻ���������Ƶ�֧�䣬���ܳ�ʱ���Ƶ�֧�䣬�������е�д��
	������ɺ�ŷ��ء�FlushFileBuffers �������£�BOOL FlushFileBuffers(HANDLE hFile); ����hFileָ��CreateFile�����򿪵ľ����
	************************************************************************/
	FlushFileBuffers(hCommDev);
}

//L: д����
long CCom::WriteSerialPort(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite)
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return COMERROR;

	BOOL	fSucess = FALSE;
	DWORD	tmpNumberOfBytesWrite;

	fSucess = WriteFile(hCommDev, lpBuffer, nNumberOfBytesToWrite,
	&tmpNumberOfBytesWrite, NULL);
	if ( fSucess == FALSE )
	{
		DWORD errors; 
		COMSTAT stat; 
		ClearCommError( hCommDev, &errors, &stat);
		return COMERROR;
	}
	else return COMOK;
}

//��COM��
long CCom::ReadSerialPort(LPVOID lpBuffer, DWORD nNumberOfBytesToRead, long * lpNumberOfBytesRead)
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return COMERROR;
	DWORD   tempReadLen;
	BOOL iret = ReadFile(hCommDev, lpBuffer, nNumberOfBytesToRead, &tempReadLen, NULL);
	*lpNumberOfBytesRead = (long)tempReadLen;
	if (!iret)
	{
		DWORD errors;
		COMSTAT stat;
		ClearCommError( hCommDev, &errors, &stat);
		return COMERROR;
	}
	else return COMOK;
}
 
BOOL CCom::PortOpen()
{
	if (hCommDev!= INVALID_HANDLE_VALUE)
		return TRUE;
	else
		return FALSE;
}
