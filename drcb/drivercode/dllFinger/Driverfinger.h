#pragma once
class CWin32Port;
class CParaXml;

class CDriverFinger
{
public:
	CDriverFinger();
	~CDriverFinger();
public:
	int OpenFp();
	void CloseFp();
	int ResetFp(int rw);
	int InitlizeConfigParam();
    int WriteFp();
	int ReadFp(VPair &outlist, short optype);
	void SetConfXmlPath(CString &path);	
	void SetErrCode(enRetCode code);
	enRetCode GetErrCode();

	VPair& GetRetListRef();

	void SetModuleDir(CString dir);
	void SetTimeOut( short timeout);
	void SetTimeOutZero();
	CString &GetModuleDir();
	
	void SetPortNo(CString port);
	CString & GetPortNo();

	CString &GetStatusDesc() { return _statusdesc; }
	CString &GetFpErrCd() { return _fperrcd; }
	CString &GetFpErrMsg() { return _fperrmsg; }
protected:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _moduledir;
	CString _portno;
	enRetCode _RetCode;
	VPair _Sub;
	CString _statusdesc;

	CString _fperrcd;
	CString _fperrmsg;

	short _porttype;
	CString _MatchDll;
	CString _ThirdDll;
	BOOL _bThirdCall;
	CString _CallDll;
	CString _ThirdDrvDll;
	BOOL _bThirdMatch;
	short _nPort;
	short _ntimeout;
    short _ndeftimeout;

	CString _serIP;
	CString _serPort;
	CString _lpSysno;
	CString _lpVendor;
	CString _lpSerId;


};