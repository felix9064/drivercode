// Com.h: interface for the CCom class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COM_H__46CFBEE4_DC2C_11D6_B775_0050BA8231EA__INCLUDED_)
#define AFX_COM_H__46CFBEE4_DC2C_11D6_B775_0050BA8231EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#define DEBUG
#define COMERROR	-1
#define COMOK		0

class CCom  
{
public:
	BOOL PortOpen();
	CCom();
	CCom(
			BYTE Port,	     /* 1-4 = COM1, COM2, COM3, COM4 */
			DWORD BaudRate,      /* actual baud rate value       */
			BYTE Parity,         /* 0-4 = no,odd,even,mark,space */
			BYTE ByteSize,       /* number of bits/byte, 4-8     */
			BYTE StopBits,        /* 0,1,2 = 1, 1.5, 2           */
			BYTE HandShaking,	/* I added these code */
			BOOL bComm
		);
	virtual ~CCom();
	
	long OpenSerialPort(
			BYTE Port,			/* 1-4 = COM1, COM2, COM3, COM4 */
			DWORD BaudRate,		/* actual baud rate value       */
			BYTE Parity,		/* 0-4 = no,odd,even,mark,space */
			BYTE ByteSize,		/* number of bits/byte, 4-8     */
			BYTE StopBits,		/* 0,1,2 = 1, 1.5, 2            */
			BYTE HandShaking,   /* S: I added these code */
			BOOL bComm
		);
    
	void PurgeRecvBuf();
    void PurgeSndBuf();	
	
	long CloseSerialPort();
	long WriteSerialPort(
			LPCVOID lpBuffer,	/* address of data to write to file */
			DWORD nNumberOfBytesToWrite			/* number of bytes to write */
		);
	long PreReadSerialPort();
	long ReadSerialPort(
			LPVOID lpBuffer,	/* address of buffer that receives data */
			DWORD nNumberOfBytesToRead,			/* number of bytes to read  */
			long *  lpNumberOfBytesRead
		);
	HANDLE	hCommDev;
};

#endif // !defined(AFX_COM_H__46CFBEE4_DC2C_11D6_B775_0050BA8231EA__INCLUDED_)
