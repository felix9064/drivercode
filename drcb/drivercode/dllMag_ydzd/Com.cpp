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

//L: 用 overlaped 结构

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

	//此处已经调用打开COM PORT的操作
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
		//KeyboardControl(1);	//特殊处理
		if( CloseHandle(hCommDev))
			hCommDev = INVALID_HANDLE_VALUE;
	}
}


//打开串口操作
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
	
	// 关闭已经打开的设备
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
	
	// 打开串口
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
		case 2://设备没有被初始化
			return COMERROR;
		case 5://端口被其它设备打开
			return COMERROR;
		}
	}
	fSucess = GetCommState( hCommDev, &dcb);
	if ( fSucess == FALSE )
	{
		dwError = GetLastError();
		return COMERROR;
	}
	//DCB最常改变的设置是数据传输率、奇偶校验方法以及数据位和停止位
	//如果SetCommState函数调用的DCB结构中的XonChar成员等于XoffChar成员，则函数会调用失败
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
	
	// 关闭所有的流控制
	// >>
	// 关闭软件流控
	dcb.fOutX=FALSE;
	dcb.fInX=FALSE;
	
	// 关闭硬件流控	***
	dcb.fRtsControl = RTS_CONTROL_TOGGLE;
	dcb.fOutxCtsFlow = FALSE;
	
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fOutxDsrFlow = FALSE;

	//dcb.fDtrControl = DTR_CONTROL_ENABLE;
	//dcb.fOutxDsrFlow = TRUE;

	// <<
	
	// 选择相应的流控制
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

	//设置掩码
	//SetCommMask(hCommDev, EV_RXCHAR|EV_TXEMPTY );
	SetCommMask( hCommDev,EV_RXCHAR );
	//设置buffer  其声明：BOOL SetupComm(HANDLE hFile,DWORD dwInQueue,DWORD dwOutQueue); 定义接收和发送缓冲区的大小
	SetupComm( hCommDev,4096,4096 );
	//清除buffer中任何信息
	PurgeComm( hCommDev,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR );
	//设置超时参数
	DWORD dwTemp = 1000 / (BaudRate / 8);
	CommTimeOuts.ReadIntervalTimeout = 100;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0; 
	CommTimeOuts.ReadTotalTimeoutConstant = 1000;
	CommTimeOuts.WriteTotalTimeoutMultiplier=2*	CBR_9600/BaudRate;
	CommTimeOuts.WriteTotalTimeoutConstant =0;
	SetCommTimeouts( hCommDev ,&CommTimeOuts);

	return COMOK;
}


//关闭串口
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

//L: 清除接收缓冲区
void CCom::PurgeRecvBuf()
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return;
	
	DWORD errors; 
	COMSTAT stat; 
	ClearCommError( hCommDev, &errors, &stat);
	//清除接收缓冲区
	PurgeComm( hCommDev, PURGE_RXCLEAR);
}

//L: 清除发送缓冲区
void CCom::PurgeSndBuf()
{
	if( hCommDev == INVALID_HANDLE_VALUE ) return;

	DWORD errors; 
	COMSTAT stat; 
	ClearCommError( hCommDev, &errors, &stat);
	//X清除发送缓冲区
	/************************************************************************X
	PurgeComm( hCommDev, PURGE_TXCLEAR);
	PurgeComm 函数可以在读写操作的同时，清空缓冲区。当应用程序在读写操作时调用PurgeComm函数，不能保证缓冲区中的所有字符都被发送。
	如果要保证缓冲区的所有字符都被发送，应该调用FlushFileBuffers函数。该函数只受流量控制的支配，不受超时控制的支配，它在所有的写操
	作都完成后才返回。FlushFileBuffers 声明如下：BOOL FlushFileBuffers(HANDLE hFile); 参数hFile指向CreateFile函数打开的句柄。
	************************************************************************/
	FlushFileBuffers(hCommDev);
}

//L: 写串口
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

//读COM口
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
