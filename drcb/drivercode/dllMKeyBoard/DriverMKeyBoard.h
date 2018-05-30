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
	/*��ȡӦ�ñ��:GetPinPadID
		���������	
		CommPort	�ַ�����ʹ�ô��ڵĶ˿ںţ�����Ϊ4���ַ���ǰ�����ַ���ΪCOM�����һλȡֵΪ1~8������"COM1"��
		ExtPort�ַ�����BP����չ��ѡ��ָ���Ϊ��ʱ��ʾ��ʹ����չ�С����磺ѡ����չ��Kָ�Ϊ:"\033%K"��
		���������
			   idnum�� ASCII����룬����1-20λ��
		����ֵ��
		0     �ɹ�
		<0    ʧ��
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