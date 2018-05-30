#pragma once
class CWin32Port;
class CParaXml;

class CDriverIdentCard
{
public:
	CDriverIdentCard();
	~CDriverIdentCard();
public:
	int OpenICd();
	void CloseICd();
	int ResetICd(int rw);
	int InitlizeConfigParam();
    int WriteICd();
	int ReadICd(HWND hWnd, VPair &readlist);
	void SetConfXmlPath(CString &path);	
	void SetErrCode(enRetCode code);
	enRetCode GetErrCode();

	VPair& GetRetListRef();

	void SetModuleDir(CString dir);
	CString &GetModuleDir();
	
	void SetPortNo(CString port);
	void SetTimeOut(short timeout);
	void SetTimeOutZero();
	CString & GetPortNo();

	CString &GetStatusDesc() { return _statusdesc; }
	CString &GetErrCd() { return _errcd; }
	CString &GetErrMsg() { return _errmsg; }
protected:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _moduledir;
	CString _portno;
	enRetCode _RetCode;
	VPair _Sub;
	CString _statusdesc;

	CString _errcd;
	CString _errmsg;

	CString _CallDll;
	CString _ThirdDll;
	BOOL _bThirdCall;
	short _isCom;
	short _nPort;
	short _ndeftimeout;
	short _ntimeout;
	int m_baudrate;
};