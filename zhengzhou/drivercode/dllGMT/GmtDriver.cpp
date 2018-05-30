// GmtDriver.cpp: implementation of the CGmtDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"

#include "Win32Port.h"
#include "paraxml.h"
#include "DIRECT.H"

#include "json/json.h"
#include "json/value.h"
#include "json/reader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HMODULE CGmtDriver::m_hLibModule=NULL;
bool CGmtDriver::m_bDriverLoaded=false;
CGmtDriver CGmtDriver::m_GmtDriver;


FILE * CGmtDriver::m_stream = NULL;
void CGmtDriver::runLog(CString logStr, CString logdir)
{
	
#ifdef DEBUG_H_LOG
	//写日志便于查错
	TCHAR buf[255];
	memset(buf, 0, 255);
	
	long ntime ;
	time(&ntime);
	struct tm *tim = localtime(&ntime);
	CString logFileName;
	_mkdir(logdir+CAMERELOG_DIR);
	logFileName.Format("%s\\gmt%04d%02d%02d.log", CAMERELOG_DIR,tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
	m_stream = fopen(logdir+logFileName, "a+");
	if (m_stream != NULL)
	{
		fprintf(m_stream, "%02d:%02d:%02d----%s\n", tim->tm_hour,tim->tm_min,tim->tm_sec,logStr);
		fflush(m_stream);
	}
	fclose(m_stream);
#endif
	
	return;
}

int CGmtDriver::writeJson(VPair &readlist)
{
	Json::Value root;
	int size = readlist.size();

	for (int i = 0; i < size; i++)
	{
		CString first = readlist[i].first;
		CString second = readlist[i].second;
		
		std::string key = (LPCSTR)first;
		std::string value = (LPCSTR)second;

		root[key] = value;
	}

	CString jsonStr(root.toStyledString().data());
	readlist.clear();
	PAIR json("json", jsonStr);
	readlist.push_back(json);
	return 0;
}

int CGmtDriver::readJson(LPCSTR message, VPair &readlist)
{
	Json::Reader reader;
    Json::Value value;

	if(reader.parse(message, value))
	{
		CString result = value["Result"].asString().c_str();
		CString msg = value["Msg"].asString().c_str();
		CString str = "";
		if(0 == result.Compare("Timeout"))
		{
			str = "Timeout2";
		}
		else if (0 == result.Compare("Success"))
		{
			if(0 == msg.Compare("确认"))
			{
				str = "Success1";
			}
			else if (0 == msg.Compare("取消"))
			{
				str = "Success0";
			}
		}
		else if (0 == result.Compare("Failure"))
		{
			str = "Failure0";
		}
		PAIR Result("Result", str);
		readlist.push_back(Result);
	}
	else
	{
		return 1;
	}
	return 0;
}

CGmtDriver::CGmtDriver()
{
	p_ReadDeviceInfo = NULL;
	p_LoadZmk = NULL;
	p_LoadZpk = NULL;
	p_ResetZmk = NULL;
	p_ReadCipher = NULL;
	p_DisplayHtml = NULL;
	p_initDriver = NULL;
	p_setPortAttr = NULL;
	p_deinit = NULL;
	p_Test = NULL;

	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();
	_ndeftimeout = 15;
	m_baudrate = 9600;
}

CGmtDriver::~CGmtDriver()
{
	if(_pWinPort)
	{
		delete _pWinPort;
		_pWinPort = NULL;
	}
	
	if(_pParaXml)
	{
		delete _pParaXml;
		_pParaXml = NULL;
	}
}

void CGmtDriver::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

CString& CGmtDriver::GetModuleDir()
{
    return _moduledir;
}

int CGmtDriver::GetTimeOut()
{
	return _ndeftimeout;
}

VPair& CGmtDriver::GetRetListRef()
{
	return _Sub;
}

int CGmtDriver::OpenICd()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //加载文件失败
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, _isCom, _CallDll, _ThirdDll, _bThirdCall, _ndeftimeout, _IDPhotoName);
	if(!_portno.IsEmpty())
	{
		if(_portno.GetLength() <= 3)
			return ERR_OPENPORT;
		CString porttmp = _portno.Mid(3,_portno.GetLength() - 3);
		CString comlpttmp = _portno.Mid(0, 3);
		if(comlpttmp == LPT)
			_isCom = 0;
		else if(comlpttmp == USB)
			_isCom = 2;
		else
			_isCom = 1;
		port = atoi(porttmp);
	}
	
	_nPort = port;
	if (baudrate > 0)
	{
		m_baudrate = baudrate;
	}

	if(_isCom != 2 && _bThirdCall == FALSE)
	{
		_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, _isCom?TRUE:FALSE);
		if(!_pWinPort->AsynOpen())
			return ERR_OPENPORT;                              //端口打开失败
	}
	return SUCCESS;
}

void CGmtDriver::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CGmtDriver::SetPortNo(CString port)
{
	_portno = port;
}

int CGmtDriver::LoadDriver(int optype)
{
	runLog("开始调用LoadDriver", _moduledir);
	if (m_hLibModule)
	{
		return SUCCESS;
	}
	CString third_dll;
	if (26 == optype || 27 == optype)
	{
		third_dll.Format("%sGwq\\%s", _moduledir, _CallDll);
	}
	else
	{
		third_dll.Format("%sPin\\%s", _moduledir, _CallDll);
	}
	runLog(third_dll, _moduledir);

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(_moduledir);
    m_hLibModule = LoadLibrary(third_dll);
	SetCurrentDirectory(buf);
	if( NULL == m_hLibModule)
	{
		return ERR_LOADDRV;
	}

	p_initDriver = (FUNC_initDriver)GetProcAddress(m_hLibModule, "initDriver");
	p_setPortAttr = (FUNC_setPortAttr)GetProcAddress(m_hLibModule, "setPortAttr");
	p_deinit = (FUNC_deinit)GetProcAddress(m_hLibModule, "deinit");
	p_Test = (FUNC_Test)GetProcAddress(m_hLibModule, "Test");
	
	if (26 == optype || 27 == optype)
	{
		p_DisplayHtml = (FUNC_DisplayHtml)GetProcAddress(m_hLibModule, "DisplayHtml");
		if (NULL == p_DisplayHtml || NULL == p_initDriver
		|| NULL == p_setPortAttr || NULL == p_deinit || NULL == p_Test)
		{
			runLog("获取厂商柜外清导出函数存在失败", _moduledir);
			FreeLibrary(m_hLibModule);
			return -1;
		}
	}
	else
	{
		p_ReadDeviceInfo = (FUNC_ReadDeviceInfo)GetProcAddress(m_hLibModule, "ReadDeviceInfo");
		p_LoadZmk = (FUNC_LoadZmk)GetProcAddress(m_hLibModule, "LoadZmk");
		p_LoadZpk = (FUNC_LoadZpk)GetProcAddress(m_hLibModule, "LoadZpk");
		p_ResetZmk = (FUNC_ResetZmk)GetProcAddress(m_hLibModule, "ResetZmk");
		p_ReadCipher = (FUNC_ReadCipher)GetProcAddress(m_hLibModule, "ReadCipher");

		if (NULL == p_ReadDeviceInfo || NULL == p_LoadZmk || NULL == p_LoadZpk || NULL == p_ResetZmk
		|| NULL == p_ReadCipher || NULL == p_initDriver
		|| NULL == p_setPortAttr || NULL == p_deinit || NULL == p_Test)
		{
			runLog("获取厂商密码键盘导出函数存在失败", _moduledir);
			FreeLibrary(m_hLibModule);
			return -1;
		}
	}
	runLog("获取厂商导出函数完毕", _moduledir);
	m_bDriverLoaded = true;
	
	return SUCCESS;
}

void CGmtDriver::RealseDriver()
{
	if(m_hLibModule)
	{
		::FreeLibrary(m_hLibModule);
	}
	m_hLibModule = NULL;
	m_bDriverLoaded = false;
}

int CGmtDriver::initDriver(Callback func)
{
	runLog("开始调用厂商初始化函数initDriver", _moduledir);
	return p_initDriver(func);
}

int CGmtDriver::deinit()
{
	return p_deinit();
}

void CGmtDriver::setPortAttr(char* port)
{
	int len = strlen(port);
	CString logStr;
	logStr.Format("开始调用设置通讯参数函数setPortAttr，port=%s, baud=%d, len=%d", port, m_baudrate, len);
	runLog(logStr, _moduledir);
	p_setPortAttr(port, len, m_baudrate, 0);
}

int CGmtDriver::ReadDeviceInfo(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_ReadDeviceInfo(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}

int CGmtDriver::ResetZmk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_ResetZmk(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}

int CGmtDriver::LoadZmk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_LoadZmk(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}

int CGmtDriver::LoadZpk(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_LoadZpk(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}

int CGmtDriver::ReadCipher(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_ReadCipher(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}

int CGmtDriver::DisplayHtml(char *args, char *resmsg, char *errmsg, int readTimeOut, int writeTimeOut)
{
	return p_DisplayHtml(args, resmsg, errmsg, readTimeOut, writeTimeOut);
}
