// GmtDriver.h: interface for the CGmtDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_)
#define AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//回调指针
typedef int (*Callback)(char* message);


//############国密################
//读设备编号
typedef int (__cdecl *FUNC_ReadDeviceInfo)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//载入主密钥
typedef int (__cdecl *FUNC_LoadZmk)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//载入工作密钥
typedef int (__cdecl *FUNC_LoadZpk)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//重置主密钥
typedef int (__cdecl *FUNC_ResetZmk)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//输入密码
typedef int (__cdecl *FUNC_ReadCipher)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//显示信息（阅读同步，确认异步）
typedef int (__cdecl *FUNC_DisplayHtml)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);

//初始化
typedef int (__cdecl *FUNC_initDriver)(Callback cfun);

//释放资源
typedef int (__cdecl *FUNC_deinit)();

//设置通信参数
typedef void (__cdecl *FUNC_setPortAttr)(char* port, int portLen, int baud, int extPort);

//通用测试
typedef int (__cdecl *FUNC_Test)(char* args, char* resmsg, char* errmsg, int readTimeout, int writeTimeout);


class CWin32Port;
class CParaXml;

class CGmtDriver  
{
public:
	CGmtDriver();
	virtual ~CGmtDriver();
public:
	static HINSTANCE m_hLibModule;
	static CGmtDriver m_GmtDriver;
	static CGmtDriver *GetInstance(){return &m_GmtDriver;}
	static bool m_bDriverLoaded;
	static FILE * m_stream;
	static void runLog(CString logStr, CString logdir = "C:\\");
public:
	int LoadDriver(int optype);
	void RealseDriver();
	bool IsDriverLoaded(){return m_bDriverLoaded;}
	void SetModuleDir(CString dir);
	CString& GetModuleDir();
	VPair& GetRetListRef();
	
	int InitlizeConfigParam();
	void SetConfXmlPath(CString &path);	
	int OpenICd();
	void SetPortNo(CString port);
	int GetTimeOut();
public:
	FUNC_ReadDeviceInfo p_ReadDeviceInfo;
	FUNC_LoadZmk p_LoadZmk;
	FUNC_LoadZpk p_LoadZpk;
	FUNC_ResetZmk p_ResetZmk;
	FUNC_ReadCipher p_ReadCipher;
	FUNC_DisplayHtml p_DisplayHtml;
	
	FUNC_initDriver p_initDriver;
	FUNC_setPortAttr p_setPortAttr;
	FUNC_deinit p_deinit;
	FUNC_Test p_Test;
public:
	int initDriver(Callback func);
	void setPortAttr(char* partno);
	int deinit();
	int GetErrorMsg(int iLen, char* pError);
	int ReadDeviceInfo(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int ResetZmk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int LoadZmk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int LoadZpk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int ReadCipher(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int DisplayHtml(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut);
	int writeJson(VPair &readlist);
	int readJson(LPCSTR message, VPair &readlist);
public:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _portno;
	enRetCode _RetCode;
	CString _moduledir;
	VPair _Sub;
	CString _statusdesc;

	CString _errcd;
	CString _errmsg;

	CString _CallDll;
	CString _ThirdDll;
	CString _IDPhotoName;
	BOOL _bThirdCall;
	short _isCom;
	short _nPort;
	short _ndeftimeout;
	int m_baudrate;
};

int MyCallback(char* message);

#endif // !defined(AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_)
