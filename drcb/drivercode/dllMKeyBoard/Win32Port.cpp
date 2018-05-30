// Serial.cpp
#include "StdAfx.h"
#include <windows.h>

#include "Win32Port.h"
#include "DriverMKeyBoard.h"
#include "keyboardcreator.h"
//#include "zttools.h"

CWin32Port::CWin32Port()
{

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );
	m_hIDComDev = NULL;
	mbOpened = 0;
    m_bExitRead = TRUE;
}

CWin32Port::~CWin32Port()
{
	//AsynClose();
}

void CWin32Port::InitVar()
{
	nRcvlen = 0;
	nSendlen = 0;
	nReadTotalNum = -1;
	memset( chRcvbuf,'\0', MAXBLOCK);
	memset( chSendbuf,'\0', MAXBLOCK);
}

void CWin32Port::SetupParam(int port, int BaudRate, char Parity, char ByteSize, char StopBits, BOOL isCom)
{
	nPort = port;
	mBaudRate=BaudRate;
	mParity=Parity;
	mStopBits = StopBits;
	mByteSize = ByteSize;
	mIsCom = isCom;
}

BOOL CWin32Port::AsynOpen( )
{

	if( mbOpened ) return( TRUE );

	char szPort[15];
	DCB dcb;
	if(mIsCom)
	{
		if(nPort>9)
			wsprintf( szPort, "\\\\.\\COM%d", nPort );
		else
			wsprintf( szPort, "COM%d", nPort );
	}
	else
	{
		wsprintf( szPort, "LPT%d", nPort );
	}

	m_hIDComDev = CreateFile( szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL );
	if( m_hIDComDev == NULL ) return( FALSE );

	memset( &m_OverlappedRead, 0, sizeof( OVERLAPPED ) );
 	memset( &m_OverlappedWrite, 0, sizeof( OVERLAPPED ) );

	dcb.DCBlength = sizeof( DCB );
	GetCommState( m_hIDComDev, &dcb );

    if (mBaudRate == 300)
        dcb.BaudRate = CBR_300 ;
    else if ( mBaudRate == 600 )
        dcb.BaudRate = CBR_600;
    else if ( mBaudRate == 1200 )
        dcb.BaudRate = CBR_1200;
    else if ( mBaudRate == 2400 )
        dcb.BaudRate = CBR_2400;
    else if ( mBaudRate == 4800 )
        dcb.BaudRate = CBR_4800;
    else if ( mBaudRate == 19200 )
        dcb.BaudRate = CBR_19200;
    else if ( mBaudRate == 38400 )
        dcb.BaudRate = CBR_38400;
    else if ( mBaudRate == 56000 )
        dcb.BaudRate = CBR_56000;
    else if ( mBaudRate == 57600 )
        dcb.BaudRate = CBR_57600;
    else if ( mBaudRate == 115200 )
        dcb.BaudRate = CBR_115200;
    else
        dcb.BaudRate = CBR_9600;
    
    //奇偶校验设置
    if ( mParity == 'O' )//奇校验
        dcb.Parity = ODDPARITY;
    else if ( mParity == 'E' )//偶校验
        dcb.Parity = EVENPARITY;
    else//无校验
        dcb.Parity = NOPARITY;

    //数据位设置
    if ( mByteSize == 7 )
        dcb.ByteSize = 7 ;
    else
        dcb.ByteSize = 8 ;

    //停止位设置
    if ( mStopBits == 2 )
        dcb.StopBits = TWOSTOPBITS;
    else
        dcb.StopBits = ONESTOPBIT ;

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = MAXDWORD;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 0;
	CommTimeOuts.ReadTotalTimeoutConstant = 0;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 0;
	CommTimeOuts.WriteTotalTimeoutConstant = 5000;
	SetCommTimeouts( m_hIDComDev, &CommTimeOuts );


	m_OverlappedRead.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	m_OverlappedWrite.hEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	unsigned char ucSet;
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_DTRDSR ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_RTSCTS ) != 0 );
	ucSet = (unsigned char) ( ( FC_RTSCTS & FC_XONXOFF ) != 0 );
	if( !SetCommState( m_hIDComDev, &dcb ) ||
		!SetupComm( m_hIDComDev, 10000, 10000 ) ||
		m_OverlappedRead.hEvent == NULL ||
		m_OverlappedWrite.hEvent == NULL ){
		DWORD dwError = GetLastError();
		if( m_OverlappedRead.hEvent != NULL ) CloseHandle( m_OverlappedRead.hEvent );
		if( m_OverlappedWrite.hEvent != NULL ) CloseHandle( m_OverlappedWrite.hEvent );
		CloseHandle( m_hIDComDev );
		return( FALSE );
		}

	mbOpened = 1;

	return( TRUE );

}

BOOL CWin32Port::AsynClose( void )
{

	if( !mbOpened || m_hIDComDev == NULL ) return( TRUE );

	if( m_OverlappedRead.hEvent != NULL )
	{
		CloseHandle( m_OverlappedRead.hEvent );
	}

	if( m_OverlappedWrite.hEvent != NULL )
	{
		CloseHandle( m_OverlappedWrite.hEvent );
	}

	CloseHandle( m_hIDComDev );
	mbOpened = 0;
	m_hIDComDev = NULL;

	return( TRUE );

}

BOOL CWin32Port::WriteCommByte( unsigned char ucByte )
{
	BOOL bWriteStat;
	DWORD dwBytesWritten;

	bWriteStat = WriteFile( m_hIDComDev, (LPSTR) &ucByte, 1, &dwBytesWritten, &m_OverlappedWrite );
	if( !bWriteStat && ( GetLastError() == ERROR_IO_PENDING ) )
	{
		if( WaitForSingleObject( m_OverlappedWrite.hEvent, 1000 ) ) 
		{
			dwBytesWritten = 0;
		}
		else
		{
			GetOverlappedResult( m_hIDComDev, &m_OverlappedWrite, &dwBytesWritten, FALSE );
			m_OverlappedWrite.Offset += dwBytesWritten;
		}
	}

	return( TRUE );

}

//正确：返回》0的个数
int CWin32Port::AsynSendData( const char *buffer, int size )
{

	if( !mbOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwBytesWritten = 0;
	int i;
	for( i=0; i<size; i++ ){
		WriteCommByte( buffer[i] );
		dwBytesWritten++;
		}

	return( (int) dwBytesWritten );

}

int CWin32Port::AsynGetReadLen( void )
{

	if( !mbOpened || m_hIDComDev == NULL ) return( 0 );

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );

	return( (int) ComStat.cbInQue );

}

int CWin32Port::AsynReadData( void *buffer, int limit )
{

	if( !mbOpened || m_hIDComDev == NULL ) return( 0 );

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	if( !ComStat.cbInQue ) return( 0 );

	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( limit < (int) dwBytesRead ) dwBytesRead = (DWORD) limit;

	bReadStatus = ReadFile( m_hIDComDev, buffer, dwBytesRead, &dwBytesRead, &m_OverlappedRead );
	if( !bReadStatus ){
		if( GetLastError() == ERROR_IO_PENDING ){
			WaitForSingleObject( m_OverlappedRead.hEvent, 2000 );
			return( (int) dwBytesRead );
			}
		return( 0 );
		}

	return( (int) dwBytesRead );

}

short CWin32Port::AsynReadByLen(short len)
{

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	short i, RcvCurrent=0, RcvBefore=0;


	
	memset( chRcvbuf,'\0', MAXBLOCK);
	nRcvlen = 0;

    ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( dwBytesRead<=0 ) 
		return( _NO_DATA );

    unsigned long TimeCurr, TimeDelay;
	unsigned char ch[2];
	char *chInBuf ;

    TimeCurr = GetTickCount();
    TimeDelay = 300000;

	chInBuf = chRcvbuf;
	RcvBefore = (short) dwBytesRead;

	bReadOK = 0;
	RcvBefore = 0;
	nRcvlen = 0;
	while(1)
	{
		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			AsynClearReadBuffer();
//			WriteLogInt("AsynReadByLen _ERR_RCVLEN TimeDelay",TimeDelay);
//			WriteLogInt("AsynReadByLen _ERR_RCVLEN TimeCurr",TimeCurr);
			return _ERR_RCVLEN;
		}
		SleepEx(10,TRUE);

        ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	    RcvCurrent = (short) ComStat.cbInQue;
		if( RcvCurrent == 0 ) continue;

		for(i=0;i<RcvCurrent;i++ ) 
		{
			bReadStatus = ReadFile( m_hIDComDev, ch,1, 
						  &dwBytesRead,&m_OverlappedRead  ) ;
			if( !bReadStatus )
			{
				if( GetLastError() == ERROR_IO_PENDING )
				{
//					WriteLogInt("AsynReadByLen _ERR_READCOM ",_ERR_READCOM);
					return _ERR_READCOM;
				}
//				WriteLogInt("AsynReadByLen _ERR_READCOM2 ",_ERR_READCOM2);
				return _ERR_READCOM2 ;
			}

			*chInBuf = ch[0];
			nRcvlen++;
			chInBuf++;
		}   // end for
		if( nRcvlen >= len )
		{
			 bReadOK = 1;
			 break;
		}
	}
    *chInBuf = '\0';

    AsynClearReadBuffer();
    return nRcvlen ;


}


void CWin32Port::AsynClearReadBuffer()
{
	PurgeComm(m_hIDComDev, PURGE_TXCLEAR);//清除输入缓冲区
}

short CWin32Port::AsynReadByEndChar(unsigned char chEndChar)
{

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	short i, RcvCurrent=0, RcvBefore=0;

	memset( chRcvbuf,'\0', MAXBLOCK);
	nRcvlen = 0;

    ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	dwBytesRead = (DWORD) ComStat.cbInQue;
	if( !dwBytesRead ) return( _NO_DATA );

    unsigned long TimeCurr, TimeDelay;
	unsigned char ch[2];
	char *chInBuf ;

    TimeCurr = GetTickCount();
    TimeDelay = 1000;

	chInBuf = chRcvbuf;
	RcvBefore = (short) dwBytesRead;

	bReadOK = 0;
	RcvBefore = 0;
	while(1)
	{
		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			AsynClearReadBuffer();
			return _ERR_ENDCHAR;
		}
		SleepEx(10,TRUE);
        ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	    RcvCurrent = (short) ComStat.cbInQue;
		if( RcvCurrent == 0 ) continue;

		for(i=0;i<RcvCurrent;i++ ) 
		{
			if( GetTickCount()-TimeCurr > TimeDelay )
			{
				AsynClearReadBuffer();
				return _ERR_ENDCHAR;
			}
			bReadStatus = ReadFile( m_hIDComDev, ch,1, 
						  &dwBytesRead,&m_OverlappedRead  ) ;
			if( !bReadStatus )
			{
				if( GetLastError() == ERROR_IO_PENDING )
				{
					SleepEx(10,TRUE);
					continue;
				}
			}

			if ( ch[0]==chEndChar )
			{
				 bReadOK = 1;
			}

			*chInBuf = ch[0];
			nRcvlen++;
			chInBuf++;
		}
		if( bReadOK ) break;
	}
    *chInBuf = '\0';

    AsynClearReadBuffer();
    return nRcvlen ;

}

short CWin32Port::sReadByEndChar(unsigned char chEndChar, char TimeOut)
{

    unsigned long TimeCurr, TimeDelay;
	short ret ;
	MSG Message;

	
    TimeCurr = GetTickCount();
    TimeDelay = TimeOut * 1000;

	while(1)
	{
		if( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) {
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}

		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			AsynClearReadBuffer();
			return _ERR_ENDCHAR;
		}
		ret = AsynReadByEndChar(chEndChar);
		if( ret == _NO_DATA ) 
		{
			SleepEx(10,TRUE);
			continue;
		}
		return ret;
	}
	return ret;

}

short CWin32Port::sReadChar(char &ch, short TimeOut)
{

    unsigned long TimeCurr, TimeDelay;
	MSG Message;
    TimeCurr = GetTickCount();
    TimeDelay = TimeOut * 1000;

	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;
	
	short RcvCurrent=0;
	unsigned char chr[2];

	while(m_bExitRead)
	{

		if( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) {
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}
		
		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			AsynClearReadBuffer();
			
			return _ERR_TIMEOUT;
		}

// 		if(nRcvlen == nReadTotalNum) //此处决定正常退出循环，不然只有等到超时才能退出
// 				return _OK_RCV;
        ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
		RcvCurrent = (short) ComStat.cbInQue;



		if( RcvCurrent == 0 ) continue;

// 		if(nRcvlen == 0 && RcvCurrent)
// 			nReadTotalNum = RcvCurrent;   //此处很关键

		//硬件不出异常的话，始终返回TRUE


		bReadStatus = ReadFile( m_hIDComDev, chr,1, 
			&dwBytesRead,&m_OverlappedRead  ) ;

		

		if( !bReadStatus )
		{

			if( GetLastError() == ERROR_IO_PENDING )
			{
				return _ERR_READCOM;
			}

			return _ERR_READCOM2 ;
		}


		if(dwBytesRead == 1)
		{
			ch = chr[0];
			nRcvlen++;
			break;
		}
	}

	if(!m_bExitRead)
		return ERR_RESET;
	return dwBytesRead;
}

short CWin32Port::sReadByLen(short len,short TimeOut)
{
    unsigned long TimeCurr, TimeDelay;
	short ret ;
	MSG Message;
    TimeCurr = GetTickCount();
    TimeDelay = TimeOut * 1000;

	while(1)
	{
		if( ::PeekMessage(&Message,NULL,0,0,PM_REMOVE) ) {
			::TranslateMessage(&Message);
			::DispatchMessage(&Message);
		}

		if( GetTickCount()-TimeCurr > TimeDelay )
		{
			AsynClearReadBuffer();

//WriteLogInt("sReadByLen _ERR_RCVLEN TimeDelay",TimeDelay);
//WriteLogInt("sReadByLen _ERR_RCVLEN TimeCurr",TimeCurr);

			return _ERR_RCVLEN;
		}
		ret = AsynReadByLen(len);
		if( ret == _NO_DATA ) 
		{
			SleepEx(10,TRUE);
			continue;
		}
		return ret;
	}
	return ret;

}

short CWin32Port::sReadComNormal(char TimeOut)
{
	BOOL bReadStatus;
	DWORD dwBytesRead, dwErrorFlags;
	COMSTAT ComStat;

	memset( chRcvbuf,'\0', MAXBLOCK);
	nRcvlen = 0;

    unsigned long  TimeBegin,TimeEnd;
	unsigned char ch[2];
	char *chInBuf ;

    TimeBegin = GetTickCount();
    TimeEnd = 1000 * TimeOut;

	chInBuf = chRcvbuf;

	bReadOK = 0;
	nRcvlen = 0;

	while(1)
	{
		if( GetTickCount()-TimeBegin > TimeEnd )
		{
			AsynClearReadBuffer();
			return _ERR_RCVDATA;
		}

		ClearCommError( m_hIDComDev, &dwErrorFlags, &ComStat );
	    dwBytesRead = (DWORD) ComStat.cbInQue;
		if( dwBytesRead<=0 ){
			SleepEx(10,TRUE);
			continue;
		}
		break;
	}

    TimeBegin = GetTickCount();
    TimeEnd = 200;

	char bNoDataRcvStatus = 0;

	SleepEx(400,TRUE);

	while(1)
	{

		bReadStatus = ReadFile( m_hIDComDev, ch,1, &dwBytesRead,&m_OverlappedRead  ) ;
		if( !bReadStatus )
		{
			if( GetLastError() == ERROR_IO_PENDING )
			{
				return _ERR_READCOM;
			}
			return _ERR_READCOM2 ;
		}

		if( dwBytesRead < 1 ){
            if( GetTickCount()-TimeBegin > TimeEnd )
			    break;
		}

		if( dwBytesRead < 1 && bNoDataRcvStatus == 0) {
			bNoDataRcvStatus  = 1;
		    TimeBegin = GetTickCount();
			continue;
		}

		bNoDataRcvStatus = 0;

		*chInBuf = ch[0];
		nRcvlen++;
		chInBuf++;
	}
    *chInBuf = '\0';
	 bReadOK = 1;

    AsynClearReadBuffer();
    return nRcvlen ;
}