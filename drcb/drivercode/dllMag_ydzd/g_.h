//����ȫ�ֱ���
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

// ������պͷ��ͻ������Ĵ�С
//#define MAXRB	10240
//#define MAXSB	100

//L: ���崮�����ַ�ʽ
#define comNone	0		//L: ������
#define comXOnXOff 1	//L: XOnXOff �������
#define comRTS 2		//L: Ӳ������
#define comRTSXOnXOff 3	//L: Ӳ����XOnXOff �������

//���巢�ͽ��հ��еİ�ͷ����β
#define STX 0x02		//���Ϳ�ʼ�ַ�
#define ETX 0x03		//���ͽ����ַ�
#define DLE 0x10		//���������ַ�
#define NAK 0x15		//�������ַ�

//REM  0: ������  2:Ӳ������Э��
#define HANDSHAKING 0
