//定义全局变量
/*extern int iSendAsc;
extern DWORD dwEvtMask;
extern LPOVERLAPPED os;
extern int iret;
extern long nret;
extern unsigned char ReceBuffer[];
extern unsigned char SendBuffer[];
extern long SBPointer;
extern long RBPointer;
extern int FirstOpenPort;
extern int iOpenListen;
extern MSG Message;*/

#define	ESC	0x1B

// 定义接收和发送缓冲区的大小
//#define MAXRB	10240
//#define MAXSB	100

//L: 定义串口握手方式
#define comNone	0		//L: 无握手
#define comXOnXOff 1	//L: XOnXOff 软件握手
#define comRTS 2		//L: 硬件握手
#define comRTSXOnXOff 3	//L: 硬件、XOnXOff 软件握手

//定义发送接收包中的包头，包尾
#define STX 0x02		//发送开始字符
#define ETX 0x03		//发送结束字符
#define DLE 0x10		//数据链接字符
#define NAK 0x15		//错误反馈字符

//REM  0: 无握手  2:硬件握手协议
#define HANDSHAKING 0
