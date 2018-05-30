// Win32Port.h

#ifndef __SERIAL_H__
#define __SERIAL_H__

#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13
#define MAXBLOCK    512

#define _NO_DATA     -1
#define _ERR_RCVLEN  -10
#define _ERR_ENDCHAR -11
#define _ERR_READCOM -12
#define _ERR_READCOM2 -13
#define _ERR_WRITECOM -14
#define _ERR_TIMEOUT  -4
#define _ERR_RCVDATA  -3
#define _ERR_SETREAD  -5
#define _ERR_COMOPEN  -2
#define _OK_RCV  0

#define _NO_DEVICE  -20

class CWin32Port
{
public:
	int nPort;
    DCB dcb;
	
	void InitVar();
	short sReadByEndChar(unsigned char chEndChar,char TimeOut);
	short sReadByLen( short len, short TimeOut );
	short sReadChar(BYTE &ch, short Timeout);
	short sReadComNormal(char TimeOut);
	void AsynClearReadBuffer();
	short AsynReadByLen(short len);
	void SetupParam( int port, int BaudRate, char Parity, char ByteSize, char StopBits, BOOL isCom);

	char chRcvbuf[MAXBLOCK];
	char chSendbuf[MAXBLOCK];
	int nRcvlen;
	int nSendlen;
    char bReadOK;
	int nReadTotalNum;

	short AsynReadByEndChar(unsigned char chEndChar);
	CWin32Port();
	~CWin32Port();

	BOOL AsynOpen( );
	BOOL AsynClose( void );

	int AsynReadData( void *, int );
	int AsynSendData( const char *, int );
	int AsynGetReadLen( void );

	char AsynIsOpened( void ){ return( mbOpened ); }

	void SetExitReadVar(BOOL bVar){ m_bExitRead = bVar;}

protected:
	BOOL WriteCommByte( unsigned char );

	HANDLE m_hIDComDev;
	OVERLAPPED m_OverlappedRead, m_OverlappedWrite;
	char mbOpened;
	BOOL m_bExitRead;

private:
	char mStopBits;
	char mByteSize;
	char mParity;
	int mBaudRate;
	BOOL mIsCom;
};

#endif
