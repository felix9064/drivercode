#pragma once

class CWin32Port;
class CParaXml;

class CDriverMag
{
public:
	CDriverMag();
	~CDriverMag();
public:
	int OpenICd();
	int OpenMag();
	int ResetMag(int rw);
	void CloseMag();
	int InitlizeConfigParam();
    int WriteMag(const char *magformat, const char *trackno, int isReWrite, VPair &vlist);
	int ReadMag(const char *magformat, const char *trackno, VPair &vlist);

	int WriteICd(const char *magformat, const char *trackno, int isReWrite, VPair &readlist);
	int ReadICd(const char *magformat, const char *trackno, VPair &readlist);
	
	void SetConfXmlPath(CString &path);
	void SetFormatXmlPath(CString &path);

	void GetMagFormatByTrackLen(CString &track2dat, int track2len, CString &track3dat, int track3len, CString &magname);
	void GetMagFormatByTrackData(CString track2dat, CString track3dat, CString &magname);

	VPair &GetMagInfoListRef();
	void SetErrCode(enRetCode errcode);
	enRetCode GetErrCode();

	void SetModuleDir(CString dir);
	CString &GetModuleDir();

	void SetPortNo(CString port);
	CString & GetPortNo();
	void SetTimeOut(short timeout);
	void SetTimeOutZero();

	inline CString& GetCardType(){ return _cardtype; }

	int GetOperRspDataLen(int trackno);
	CString GetOperRspEndData(int trackno);

	int GetReadOperRspDataLen(int trackno);
	CString GetReadOperRspEndData(int trackno);

protected:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _fortxmlpath;
	CString _moduledir;
	CString _portno;
	CString _cardtype;
	VPair _itemlist;
	enRetCode _errcode;
	
	CString _errcd;
	CString _errmsg;

	CString _CallDll;
	CString _ThirdDll;
	CString _IDPhotoName;
	BOOL _bThirdCall;

	short _ndeftimeout;
	short _ntimeout;
	short _isCom;
	short _nPort;
	int m_baudrate;
};