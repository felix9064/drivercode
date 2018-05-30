#include "StdAfx.h"
#include "Win32Port.h"
#include "paraxml.h"
#include "Driverfinger.h"

CDriverFinger::CDriverFinger()
{
	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();

	_bThirdMatch = FALSE;
	_bThirdCall = FALSE;
	_ntimeout = 0;
	_ndeftimeout = 15;
}

CDriverFinger::~CDriverFinger()
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

VPair &CDriverFinger::GetRetListRef()
{
	return _Sub;
}

int CDriverFinger::OpenFp()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //加载文件失败
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, _porttype, _MatchDll, _ThirdDll, 
		                    _bThirdMatch, _bThirdCall, _CallDll, _ThirdDrvDll, _ndeftimeout, _serIP, _serPort, _lpSysno, _lpVendor);
	if(!_portno.IsEmpty())
	{
		if(_portno.GetLength() <= 3)
			return ERR_OPENPORT;
		CString porttmp = _portno.Mid(3,_portno.GetLength() - 3);
		CString comlpttmp = _portno.Mid(0, 3);
		if(comlpttmp == LPT)
			_porttype = 0;
		else if(comlpttmp == USB)
			_porttype = 2;
		else
			_porttype = 1;
		port = atoi(porttmp);
	}

	_nPort = port;
	if(_porttype != 2 && _bThirdCall == FALSE)
	{
		_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, _porttype?TRUE:FALSE);
		if(!_pWinPort->AsynOpen())
			return ERR_OPENPORT;                              //端口打开失败
	}
	return SUCCESS;
}

int CDriverFinger::ResetFp(int rw)
{
	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);

	if(_bThirdCall)
		return SUCCESS;
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

void CDriverFinger::SetErrCode(enRetCode code)
{
	_RetCode = code;
}

enRetCode CDriverFinger::GetErrCode()
{
	return _RetCode;
}

void CDriverFinger::CloseFp()
{
	if(!_bThirdCall)
	{
		_pWinPort->AsynClose();
	}
}

void CDriverFinger::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CDriverFinger::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

void CDriverFinger::SetTimeOut( short timeout)
{
    _ntimeout = timeout;
}

void CDriverFinger::SetTimeOutZero()
{
	_ntimeout = 0;
}

CString & CDriverFinger::GetModuleDir()
{
	return _moduledir;
}

void CDriverFinger::SetPortNo(CString port)
{
	_portno = port;
}

CString & CDriverFinger::GetPortNo()
{
	return _portno;
}

int CDriverFinger::WriteFp()
{
	return SUCCESS;
}

int CDriverFinger::ReadFp(VPair &inlist, short optype)
{
	CString tmpStr;
	char errcd[20];
	char errmsg[256];
	int ret ;
	BYTE fpbuf[FPDATA_LEN];

	//调用指纹适配器
	if(_bThirdCall)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		typedef BOOL (CALLBACK * LPGETFPCOM)(FP_MATCH_INTERFACE *);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + _CallDll);
		SetCurrentDirectory(buf);
		if(!hDll)
		{
			return ERR_NOFINDTHIRDCALL;
		}
        LPGETFPCOM lpGetCom = (LPGETFPCOM)GetProcAddress(hDll, "GetFpCOM");
		if(!lpGetCom)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDTHIRDCALL;
		}
		
		FP_MATCH_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(FP_MATCH_INTERFACE));
		BOOL bGetCOM = lpGetCom(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDTHIRDCALL;
		}
		
		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);

		PAIR apdatename("apdate", _CallDll);
		inlist.push_back(apdatename);

		PAIR thirddrvname("drvname", _ThirdDrvDll);
		inlist.push_back(thirddrvname);

		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);

		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);

		if (0 == _ntimeout)
		{
			_ntimeout = _ndeftimeout;
		}
		tmpStr.Format("%d", _ntimeout);
		PAIR TimeOut("timeout", tmpStr);
		inlist.push_back(TimeOut);

		memset(errcd, 0, 20);
		memset(errmsg, 0, 256);
		memset(fpbuf, 0, FPDATA_LEN);
		DWORD fpdrvLen = 0;

		if(optype == GETFINGERDAT_OP)
			ret = stCOM.GetFpData(inlist, fpbuf, &fpdrvLen, errcd, errmsg);
		else if(optype == GETTEMPLATE_OP)
			ret = stCOM.GetFpTemplate(inlist, fpbuf, &fpdrvLen, errcd, errmsg);
		else if(optype == VERIFYCOMM)
			ret = stCOM.Match(inlist, errcd, errmsg);
		else
			ret = stCOM.GetFpData(inlist, fpbuf, &fpdrvLen, errcd, errmsg);
		if(ret != 0)
		{
			_fperrcd = errcd;
			_fperrmsg = errmsg;
			FreeLibrary(hDll);
			return ERR_READFAIL;
		}
		
		FreeLibrary(hDll);
		return SUCCESS;
	}
	return SUCCESS;
}

int CDriverFinger::InitlizeConfigParam()
{
	return _pParaXml->InitContrOrder();
}