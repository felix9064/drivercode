#pragma once
class CWin32Port;
class CParaXml;

class CDriverMKeyBd
{
public:
	CDriverMKeyBd();
	~CDriverMKeyBd();
public:
	int OpenMKeyBd();
	void CloseMKeyBd();
	int ResetMKeyBd(int rw);
	int InitlizeConfigParam();
    int WriteMKeyBd(int optype, const char* mkeyidx,const char* wkeyidx, TPCHR lpOldMWKey, TPCHR lpMWKey, TPCHR lpKeyCK);
	int ReadMKeyBd(int twice, int len, CString &pwd);
	int ReadPin(int twice, int* minLen,int *maxLen, const char* mkeyidx, const char* wkeyidx,const char* random,CString &pwd, CString &TPKCK, const char* svrType);
	/*读取应用编号:GetPinPadID
		输入参数：	
		CommPort	字符串，使用串口的端口号，必须为4个字符且前三个字符必为COM，最后一位取值为1~8。例："COM1"。
		ExtPort字符串，BP盒扩展口选择指令串。为空时表示不使用扩展盒。例如：选择扩展口K指令串为:"\033%K"。
		输出参数：
			   idnum： ASCII码编码，长度1-20位；
		返回值：
		0     成功
		<0    失败
	*/
	int  GetPinPadID(char *idnum);
	int GetCustomEvaluate(int *value);
	void SetConfXmlPath(CString &path);	
	void ConvertPasswordStr(CString &passwd);
	void SetErrCode(enRetCode code);
	enRetCode GetErrCode();

	int driveKB(int optype, VPair &readlist);

	VPair& GetRetListRef();

	void SetModuleDir(CString dir);
	CString &GetModuleDir();
	
	void SetPortNo(CString port);
	void SetTimeOut(short TimeOut);
	void SetTimeOutZero();
	CString & GetPortNo();

	int GetErrCd() { return _errcd; }
	CString &GetErrMsg() { return _ErrDesc; }

	static void runLog(CString logStr, CString logdir = "C:\\");

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
	int _iBaudrate;

	int _errcd;
	CString _ErrDesc;
	short _ntimeout;
	short _ndeftimeout;
	

	HANDLE m_hObject;

public:
	static FILE * m_stream;
};