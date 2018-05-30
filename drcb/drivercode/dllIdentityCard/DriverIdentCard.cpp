#include "StdAfx.h"
#include "Win32Port.h"
#include "paraxml.h"
#include "DriverIdentCard.h"

CDriverIdentCard::CDriverIdentCard()
{
	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();
	_ndeftimeout = 15;
	_ntimeout = 0;
	m_baudrate = 115200;
}

CDriverIdentCard::~CDriverIdentCard()
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

VPair &CDriverIdentCard::GetRetListRef()
{
	return _Sub;
}

int CDriverIdentCard::OpenICd()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //加载文件失败
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, _isCom, _CallDll, _ThirdDll, _bThirdCall, _ndeftimeout);
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

int CDriverIdentCard::ResetICd(int rw)
{
	if(_bThirdCall)
	{
		typedef BOOL (CALLBACK * LPGETFPCOM)(IDENT_CARD_INTERFACE *);
		HMODULE hDll = LoadLibrary(_moduledir + _CallDll);
		if(!hDll)
		{
			return ERR_NOFINDMATCHDLL;
		}
        LPGETFPCOM lpGetIdentCardCom = (LPGETFPCOM)GetProcAddress(hDll, "GetIdentityCardCOM");
		if(!lpGetIdentCardCom)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		
		IDENT_CARD_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(IDENT_CARD_INTERFACE));
		BOOL bGetCOM = lpGetIdentCardCom(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		int ret = stCOM.ResetDev();
		if(ret != 0)
		{
			FreeLibrary(hDll);
			return ERR_RESET;
		}

		FreeLibrary(hDll);
		return SUCCESS;
	}

	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);

	int len;
	VStepList &steplist = _pParaXml->GetStepListRef();
	for(int i = 0; i < steplist.size(); i++)
	{
		stFpDoStep &step = steplist[i];
		
		if(step.oper == WRITE && step.reset == 1)
		{
			memset(neworder, 0, bufsize);
			if(_pParaXml->ParaOrder(step.writedata, neworder, len) != 0)  //将十六进制字符串转化为十进制。
				return ERR_INVALIDORDER;    //转换指令错误
			
			if(!_pWinPort->AsynSendData(neworder, len))
				return ERR_SENDDATA;

			_pWinPort->SetExitReadVar(FALSE);
			break;
		}
	}
	return SUCCESS;
}

void CDriverIdentCard::SetErrCode(enRetCode code)
{
	_RetCode = code;
}

enRetCode CDriverIdentCard::GetErrCode()
{
	return _RetCode;
}

void CDriverIdentCard::CloseICd()
{
	if(!_bThirdCall)
        _pWinPort->AsynClose();
}

void CDriverIdentCard::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CDriverIdentCard::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

CString & CDriverIdentCard::GetModuleDir()
{
	return _moduledir;
}

void CDriverIdentCard::SetPortNo(CString port)
{
	_portno = port;
}

void CDriverIdentCard::SetTimeOut(short timeout)
{
	_ntimeout = timeout;
}

void CDriverIdentCard::SetTimeOutZero()
{
	_ntimeout = 0;
}

CString & CDriverIdentCard::GetPortNo()
{
	return _portno;
}

int CDriverIdentCard::WriteICd()
{
	return SUCCESS;
}

int CDriverIdentCard::ReadICd(HWND hWnd, VPair &readlist)
{
	//调用厂商驱动
	if(_bThirdCall)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);

		typedef BOOL (CALLBACK * LPGETFPCOM)(IDENT_CARD_INTERFACE *);

		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + _CallDll);
		SetCurrentDirectory(buf);
		if(!hDll)
		{
			return ERR_NOFINDMATCHDLL;
		}
        LPGETFPCOM lpGetIdentCardCom = (LPGETFPCOM)GetProcAddress(hDll, "GetIdentityCardCOM");
		if(!lpGetIdentCardCom)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		IDENT_CARD_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(IDENT_CARD_INTERFACE));
		BOOL bGetCOM = lpGetIdentCardCom(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("thirdll", _moduledir + _ThirdDll);
		readlist.insert(readlist.begin(), dllnamepr);
        if (0 == _ntimeout)
        {
			_ntimeout = _ndeftimeout;
        }
		CString tmptimeout;
		tmptimeout.Format("%d", _ntimeout);
		PAIR timeout("timeout", tmptimeout);
		readlist.push_back(timeout);

		CString tmpbaudrate;
		tmpbaudrate.Format("%d", m_baudrate);
		PAIR paudrate("baudrate", tmpbaudrate);
		readlist.push_back(paudrate);

		char errcd[20];
		memset(errcd, 0, 20);
		char errmsg[256];
	    memset(errmsg, 0, 256);
		short isUSB = (_isCom != 2 ? 0 : 1);
		int ret = stCOM.ReadIdentityCard(_nPort, isUSB, readlist, errcd, errmsg);
		if(ret != 0)
		{
			_errcd = errcd;
			_errmsg = errmsg;
			FreeLibrary(hDll);
			return ERR_READFAIL;
		}

		FreeLibrary(hDll);
		return SUCCESS;
	}

	return SUCCESS;
}

int CDriverIdentCard::InitlizeConfigParam()
{
	return _pParaXml->InitContrOrder();
}