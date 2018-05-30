#pragma once
typedef int (CALLBACK *FUNC_InitDev)(int nComId,int nBaude, int nDevType);
typedef int (CALLBACK *FUNC_CloseDev)();
typedef int (CALLBACK *FUNC_ReleaseDev)();
typedef int (CALLBACK *FUNC_ReadCardInfo)(char *pAIdLst,int nAIdLstLen, 
										  char *pTag_5A,
										  char *pTag_5F34,
										  char *pTag_9F77,
										  char *pTag_9F78,
										  char *pTag_9F79,
										  char *pTag_9F6D);

typedef int (CALLBACK *FUNC_FastReadCardNo)(char *pAIdLst,int nAIdLstLen, 
										  char *pTag_5A,
										  char *pTag_5F34,
										  char *pTag_9F77,
										  char *pTag_9F78,
										  char *pTag_9F79,
										  char *pTag_9F6D,
										  char *pTag_57);

typedef int (CALLBACK *FUNC_PreUpdate)(char *pAIdLst,int nAIdLstLen,
									   char *pTag_9F02,				//授权金额 6字节
									   char *pTag_9F03,				//其他金额 6字节
									   char *pTag_9F1A,				//终端国家代码 2字节
									   char *pTag_95,				//终端验证结果TVR 5字节
									   char *pTag_5F2A,				//交易货币代码2字节
									   char *pTag_9A,				//交易日期 3字节
									   char *pTag_9C,				//交易类型 1字节
									   char *pTag_9F37,				//不可预知数 4字节
									   char *pTag_9F21,				//交易时间 3字节
									   char *pTag_9F4E,				//商户名称 20字节
									   char *pTag_82,				//应用交互特征(AIP),2字节
									   char *pTag_9F36,				//ATC 2字节
									   char *pTag_9F26,				//ARQC 8字节
									   char *pTag_9F10,				//发卡行应用数据,最大32字节
									   int *p9F10Len					//发卡行应用数据长度
									   );
typedef int (CALLBACK *FUNC_UpDateIccInfo)(char *pArpc,char *pArc,char *pScript,int nScriptLen,
										   char *pResult,int *pResultLen);
typedef int (CALLBACK *FUNC_OnLineFailed)();

typedef int (CALLBACK *FUNC_getIcInfo)(char *TermType,char *PortType,
							   char *PortNO,char*ICFlag,
							   char *TagList,char *AIDList, 
							   char *UserInfo,
							   char *ICType, char*lpicappdata,char*ErrMsg);

typedef int (CALLBACK *FUNC_CT_getIcInfo)( char PortType, char PortNO, char BpNo, char ICFlag, char *Taglist, int nWaitTime,
										  char *UserInfo, char * ErrMsg );

typedef int  (CALLBACK *FUNC_genARQC)(char *TermType,char *PortType,
							 char *PortNO,char*ICFlag,char *TxData,char*AIDList,
							 char*lpicappdata,char*ARQCLen,char*ARQC,char*Field60,
							 char*ICType,char*ErrMsg);

typedef int (CALLBACK *FUNC_CT_genARQC)( char PortType, char PortNO, char BpNo, char ICFlag, 
										char *TxData, int *pnARQCLen, char* ARQC, char * ErrMsg );

typedef int  (CALLBACK *FUNC_GetTranDetail)(char *TermType,char *PortType,
								   char *PortNO,char*ICFlag,char *AIDList,char*TxDetail,
								   char*ICType,char*ErrMsg);

typedef int (CALLBACK *FUNC_CT_GetTranDetail)( char PortType, char PortNO, char BpNo, char ICFlag, 
											  char *TxDetail, char * ErrMsg );

typedef int  (CALLBACK *FUNC_CtlScriptData)(char *TermType,char *PortType,
								   char *PortNO,char*ICFlag,char *TxData,char*lpicappdata,
								   char *ARPCLen,char*ARPC,char *Status,char *TC,
								   char *ScriptResult,char *ErrMsg);


typedef int  (CALLBACK *FUNC_getCardNo)(char *TermType,char *PortType,char *PortNO,char *AIDList,char *ReadBinCmd,
										char * UserInfo,char *ICType,char*ErrMsg);

typedef int (CALLBACK *FUNC_CT_CtlScriptData)( char PortType, char PortNO, char BpNo, char ICFlag,char* TxData,  
											  int ARPCLen, char * ARPC , char *TC, char * ScriptResult, char * ErrMsg );


typedef int  (CALLBACK *FUNC_CT_PowerOff)(char PortType, char PortNO, char BpNo, char ICFlag, char * ErrMsg);

typedef int  (CALLBACK *FUNC_CT_getIcInfoAndGenARQC)(char PortType, char PortNO, char BpNo, char ICFlag,
													 char *Taglist, int nWaitTime, char *TxData, char *UserInfo, 
													 int *pnARQCLen, char* ARQC, char * ErrMsg );

//typedef int (CALLBACK *FUNC_split_taglist)( char *sSrc ,int *iCount);


// 20170921 增加苏南ETC读写卡驱动
typedef int (CALLBACK *FUNC_drive)(int term_type,
								   char *tty_name,
								   int port_id,
								   int time_out,
								   int func_id,
								   int func_argc,
								   char *func_argv[],
								   char *result_buf,
								   int buf_size);

bool Hex2Raw(BYTE *pData, const char *szMsg, int nMsgLen);
void Raw2HexStr(char *pStr, const BYTE *pRaw, int nRawLen);
int ParaOrder(CString &old, char *pOutNew);
int ConvertHexStr2Deci(char *pStrHex);

/*
获得tag 的值
tagname:要找的TAG
taglen: 要找的TAG的长度
indata: 查找TAG的数据源
inlen:  数据源的长度
tagvalue: 返回TAG的值
tagvaluelen:返回TAG值的长度
*/
int gwiGettagvalue(unsigned char *tagname,int taglen,unsigned char * indata,int inlen,unsigned char * tagvalue,int * tagvaluelen);


/*
功能：查找指定标签的数据(FCP模板62开头，用于文件控制参数,FMD模板64开头，用于文件管理参数)
参数：tagname,标签名
taglen, 标签长度
indata, 提供查找的源数据
inlen,  源数据的长度
        0为不限制长度，遇到标签为0或FF退出
*/
unsigned char * gwiFindtag(unsigned  char *tagname, int taglen, unsigned char *indata,int inlen);

int Get_NoTagARQC( char * oldARQC, char * newARQC, char *tag9F10, char *tag95);

int  getAsc(unsigned char *instr,unsigned char * outstr,int ilen);

int  getHex(int mode,unsigned char *instr,unsigned char *outstr,int ilen);


class CWin32Port;
class CParaXml;

class CDriverICCard
{
public:
    CDriverICCard();
	~CDriverICCard();

	static CDriverICCard *GetInstance(){return &m_DriverICCard;}
	int OpenIC();
    void SetModuleDir(CString dir);
	CString &GetModuleDir();
	void SetErrCode(enRetCode code);
	enRetCode GetErrCode();
	VPair& GetRetListRef();
	void SetConfXmlPath(CString &path);
	void SetPortNo(CString port);
    int ReadICAcount(VPair &OutList);
	int GetThirdCalled();
	int LoadDriver();
	int LoadDriver2();
	void RealseDriver();
	bool IsDriverLoaded(){return m_bDriverLoaded;}
	bool IsDriverLoaded2(){return m_bDriverLoaded2;}
	void SetTimeOut(short timeout);
	void ConvertTagValue(char Tag, CString &TagValue);

	int getIcInfo(VPair &OutList);
	int getIcNo(VPair &OutList);
	int genARQC(char *pTxData, VPair &OutList);
	int GetTranDetail(VPair &OutList);
	int CtlScriptData(char *pARPC, VPair &OutList);
	int getCardNo(VPair &OutList);
	int getIcInfoAndGenARQC(char *pTxData, VPair &OutList);
	int PowerOff(VPair &OutList);
	int invokeDrive(int optype, char *func_argv, VPair &OutList);

private:
	void SetPortParam(int baudrate, short parity, short bytesize, short stopbits, BOOL isCom);

protected:

	FUNC_getIcInfo m_pgetIcInfo;
	FUNC_genARQC m_pgenARQC;
	FUNC_GetTranDetail m_pGetTranDetail;
	FUNC_CtlScriptData m_pCtlScriptData;
	FUNC_getCardNo m_pgetCardNo;

	FUNC_CT_getIcInfo m_CT_pgetIcInfo;
	FUNC_CT_genARQC m_CT_pgenARQC;
	FUNC_CT_GetTranDetail m_CT_pGetTranDetail;
	FUNC_CT_CtlScriptData m_CT_pCtlScriptData;
	FUNC_CT_PowerOff m_CT_pPowerOff;
	FUNC_CT_getIcInfoAndGenARQC m_CT_pgetIcInfoAndGenARQC;

	FUNC_drive m_drive;

protected:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _moduledir;
	CString _portno;
	enRetCode _RetCode;
	VPair _Sub;
	
	short _porttype;
	short _nPort;
	int _baudrate; //波特率
	short _parity;   //奇偶校验
	short _bytesize; //字节所占bit位数
	short _stopbits; //停止位
	BOOL _isCom;
	
	int _errcd;
	CString _DefAID;
	CString _ErrDesc;
	BOOL _hasAdap;
	CString _adapter, _thirdll, _etcdll;
	short _timeOut;
	int _dllType;
	short _ntimeout;
	char _ICFlag;
	char _BpNO;
	char m_TxData[1024];
	char m_icappdata[8192];
	CString m_ICKXULH;
	FILE * m_stream;

	static HINSTANCE m_hLibModule;
	static HINSTANCE m_hLibModule2;
	static bool m_bDriverLoaded;
	static bool m_bDriverLoaded2;
	static CDriverICCard m_DriverICCard;
};


inline char Hex2Char(BYTE b)
{
	if (b<10) return b+'0';
	if (b<16) return b-10+'a';
	return '0';
}

inline int Char2Hex(char c)
{
	if ('0'<=c && c<='9') return c-'0';
	if ('a'<=c && c<='f') return c-'a'+10;
	if ('A'<=c && c<='F') return c-'A'+10;
	return -1;
}
