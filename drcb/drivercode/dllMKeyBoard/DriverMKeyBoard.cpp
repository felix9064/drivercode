#include "StdAfx.h"
#include "Win32Port.h"
#include "paraxml.h"
#include "DriverMKeyBoard.h"
#include <direct.h>

FILE * CDriverMKeyBd::m_stream = NULL;

CDriverMKeyBd::CDriverMKeyBd()
{
	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();
	_ErrDesc = "";
	_errcd = 0;
	_ndeftimeout = 15; //Ĭ�ϳ�ʱʱ��
	_ntimeout = 0;
	m_hObject = INVALID_HANDLE_VALUE;
}

CDriverMKeyBd::~CDriverMKeyBd()
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
	m_hObject = INVALID_HANDLE_VALUE;
}

VPair &CDriverMKeyBd::GetRetListRef()
{
	return _Sub;
}

int CDriverMKeyBd::OpenMKeyBd()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //�����ļ�ʧ��
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	BOOL isCom;
	BOOL hasadap = FALSE;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, isCom, hasadap, _ndeftimeout);
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
	_iBaudrate = baudrate;
	if(_porttype != 2 && hasadap == FALSE)
	{
		_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, isCom);
		if(!_pWinPort->AsynOpen())
			return ERR_OPENPORT;                              //�˿ڴ�ʧ��
	}
	
	return SUCCESS;
}

int CDriverMKeyBd::ResetMKeyBd(int rw)
{
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;

    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	if(hasAdap)
	{/*
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + adapter);
		SetCurrentDirectory(buf);
		if(!hDll)
		{
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		
		KB_ADAPTER_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		VPair inlist;
		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);

		CString tmpstr;
		PAIR thirddrvname("drvname", thirdll);
		inlist.push_back(thirddrvname);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);
		
		tmpStr.Format("%d", len);
		PAIR PWDLen("len", tmpStr);
		inlist.push_back(PWDLen);
		
		char pin[256];
		memset(pin, 0, 256);
		char tpkck[256];
		memset(tpkck, 0, 256);
		HDLL hTDll = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
		if(!hTDll)
		{
			FreeLibrary(hDll);
			_errcd = -1;
			_ErrDesc = errmsg;
			return ERR_LOADDRV;
		}
		
		int ret = stCOM.SFKBReadPin(hTDll, inlist, mkeyidx, wkeyidx, acctno, twice, pin, tpkck, errmsg);
		if(ret != 0)
		{
			stCOM.SFKBFreeDrv(hTDll);
			FreeLibrary(hDll);
			_ErrDesc = errmsg;
			_errcd = ret;
			return ERR_READPINFAIL;
		}
		
		stCOM.SFKBFreeDrv(hTDll);
		FreeLibrary(hDll);
        return SUCCESS;
		*/
		return SUCCESS;
	}

	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);

	VStepList &steplist = _pParaXml->GetStepListRef();
	for(int i = 0; i < steplist.size(); i++)
	{
		stKeyBdDoStep &step = steplist[i];
		
		if(step.oper == WRITE && step.reset == 1)
		{
			memset(neworder, 0, bufsize);
			if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //��ʮ�������ַ���ת��Ϊʮ���ơ�
				return ERR_INVALIDORDER;    //ת��ָ�����
			
			if(!_pWinPort->AsynSendData(neworder, strlen(neworder)))
				return ERR_SENDDATA;

			_pWinPort->SetExitReadVar(FALSE);
			break;
		}
	}
	return SUCCESS;
}

void CDriverMKeyBd::SetErrCode(enRetCode code)
{
	_RetCode = code;
}

enRetCode CDriverMKeyBd::GetErrCode()
{
	return _RetCode;
}

void CDriverMKeyBd::CloseMKeyBd()
{	
	if (m_hObject != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hObject);
		m_hObject = INVALID_HANDLE_VALUE;
	}
    _pWinPort->AsynClose();
}

void CDriverMKeyBd::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CDriverMKeyBd::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

CString & CDriverMKeyBd::GetModuleDir()
{
	return _moduledir;
}

void CDriverMKeyBd::SetPortNo(CString port)
{
	_portno = port;
}

void CDriverMKeyBd::SetTimeOut(short TimeOut)
{
	_ntimeout = TimeOut;
}

void CDriverMKeyBd::SetTimeOutZero()
{
	_ntimeout = 0;
}

CString & CDriverMKeyBd::GetPortNo()
{
	return _portno;
}

/*
optype--��������  1--���Ĺ�����Կ��2--���Ĺ�����Կ��3--�๤����Կ
mkeyidx--����Կ������
wkeyidx--������Կ������  (��������Կʱ��ֵ������)
lpOldMWKey--Ҫ�����������Կ��  (��������Կʱ�贫���ֵ)
lpMWKey--Ҫ������Ĺ�����Կ�� (�๤������Կʱ�贫���ֵ)
lpKeyCK--У�鴮(����Կ��������̻᷵�ؼ��鴮��Ҫ�ʹ˴����бȶԣ���һ�������Կ�ɹ�)(��ũ��δ��У�鴮)
*/
int CDriverMKeyBd::WriteMKeyBd( int optype, const char* mkeyidx, const char* wkeyidx, TPCHR lpOldMWKey, TPCHR lpMWKey, TPCHR lpKeyCK )
{
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	TCHAR buf[255];
	memset(buf, 0, 255);
	
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	
	CString tmpStr;
	CHAR errmsg[BUFSIZ_256];
	memset(errmsg, 0, BUFSIZ_256);
	HMODULE hDll = 0;
	int ret = -1;
	KB_ADAPTER_INTERFACE stCOM;
	memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
	VPair inlist;
	typedef pair<CString, CString> PAIR;
	debug("hasAdap:%d",hasAdap);
	if(hasAdap)
	{
		
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		hDll = LoadLibrary(_moduledir + adapter);
		
		if(!hDll)
		{
			SetCurrentDirectory(buf);
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		debug("GetSFKBCOM success!");
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		

		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);
		
		if (0 == _ntimeout)
		{
			_ntimeout = _ndeftimeout;
		}
		CString tmpstr;
		tmpstr.Format("%d",_ntimeout);
		PAIR timeout("timeout", tmpstr);
		inlist.push_back(timeout);
		
		PAIR thirddrvname("drvname", thirdll);
		inlist.push_back(thirddrvname);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);
	}
	else
	{
		SetCurrentDirectory(buf);
		FreeLibrary(hDll);
		return ERR_NOFINDMATCHDLL;
	}

	HDLL hDrv = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
	if(!hDrv)
	{
		_errcd = -1;
		_ErrDesc = errmsg;
		SetCurrentDirectory(buf);
		FreeLibrary(hDll);
		return ERR_LOADDRV;
	}
	debug("SFKBLoadDrv success!");
	if(optype == 1)
	{//���ļ���TMK
		char lpOutKeyCK[BUFSIZ_256];
		memset(lpOutKeyCK, 0, BUFSIZ_256);
		//ret = stCOM.SFKBLoadMasterKey(hDrv, inlist, mkeyidx, lpOldMWKey, lpMWKey, 0, lpOutKeyCK, errmsg);
		if(ret != 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hDrv);
			FreeLibrary(hDll);
			_errcd = ret;
			_ErrDesc = errmsg;
			return ERR_WRITEMWKEY_FAIL;
		}
	}
	else if(optype == 2)
	{//���ļ���TMK
		//ret = stCOM.SFKBLoadMasterKey(hDrv, inlist, mkeyidx, lpOldMWKey, lpMWKey, 1, (PCHR)lpKeyCK, errmsg);
		if(ret != 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hDrv);
			FreeLibrary(hDll);
			_errcd = ret;
			_ErrDesc = errmsg;
			return ERR_WRITEMWKEY_FAIL;
		}
	}
	else
	{//���ļ���TPK
		debug("incoming SFKBLoadWorkKey2!");
		debug("���:%s,%s,%s",mkeyidx,lpMWKey,lpKeyCK);
		ret = stCOM.SFKBLoadWorkKey2(hDrv, inlist, mkeyidx, wkeyidx, "X", lpMWKey,lpKeyCK, errmsg);
		debug("SFKBLoadWorkKey2 return:%d",ret);
		if(ret != 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hDrv);
			FreeLibrary(hDll);
			_errcd = ret;
			_ErrDesc = errmsg;
			return ERR_WRITEMWKEY_FAIL;
		}

	}

	SetCurrentDirectory(buf);
	stCOM.SFKBFreeDrv(hDrv);
	FreeLibrary(hDll);
	return SUCCESS;
}

void CDriverMKeyBd::ConvertPasswordStr(CString &passwd)
{
	CString tmpLogStr;
	tmpLogStr.Format("ConvertPasswordStr start...");
	CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

	CString mapkey;
	char *pStr = passwd.GetBuffer(passwd.GetLength());
	for(int i = 0; i < passwd.GetLength(); i ++)
	{
		mapkey = _pParaXml->GetMapKey(passwd[i]);
        pStr[i] = mapkey[0];
	}

	tmpLogStr.Format("ConvertPasswordStr end!passwd is %s",passwd);
	CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());
}

int CDriverMKeyBd::ReadPin( int twice, int* minLen,int *maxLen, const char* mkeyidx, const char* wkeyidx,const char* random,CString &pwd, CString &TPKCK, const char* srvType)
{
	//debug("ReadPin��Σ�%d,%d,%s,%s,%s",twice,*len,mkeyidx,wkeyidx,random);
	if(!twice)
		twice = 2;
	
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	

	CString tmpStr;
	char errmsg[256];
	memset(errmsg, 0, 256);
	debug("hasAdap:%d",hasAdap);
	if(hasAdap)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + adapter);
		//SetCurrentDirectory(buf);
		if(!hDll)
		{
			SetCurrentDirectory(buf);
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		debug("GetSFKBCOM success!");
		KB_ADAPTER_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		VPair inlist;
		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);
		
		if (0 == _ntimeout)
		{
			_ntimeout = _ndeftimeout;
		}
		CString tmpstr;
		tmpstr.Format("%d",_ntimeout);
		PAIR timeout("timeout", tmpstr);
		inlist.push_back(timeout);
		
		PAIR thirddrvname("drvname", thirdll);
		inlist.push_back(thirddrvname);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);

		tmpStr.Format("%d", *minLen);
		PAIR PWDMinLen("minLen", tmpStr);
		inlist.push_back(PWDMinLen);

		tmpStr.Format("%d", *maxLen);
		PAIR PWDMaxLen("maxLen", tmpStr);
		inlist.push_back(PWDMaxLen);

		tmpStr.Format("%d", _iBaudrate);
		PAIR BaudRate("baudrate", tmpStr);
		inlist.push_back(BaudRate);

		tmpStr.Format("%d", twice);
		PAIR Twice("twice", tmpStr);
		inlist.push_back(Twice);

		tmpStr.Format("%s", srvType);
		PAIR srvtype("srvType", tmpStr);
		inlist.push_back(srvtype);
		
		char pin[256];
		memset(pin, 0, 256);
		char tpkck[256];
		memset(tpkck, 0, 256);

		HDLL hTDll = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
		
// 		FILE* file = fopen("dll.log","a+");
// 		fprintf(file,"��������Ϊ%s\n",_moduledir + thirdll);
// 		fclose(file);

		if(!hTDll)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			_errcd = -1;
			_ErrDesc = errmsg;
			return ERR_LOADDRV;
		}
		debug("SFKBLoadDrv success!");
		
		int ret = stCOM.SFKBReadPin2(hTDll,inlist,mkeyidx, wkeyidx, random, pin, tpkck, errmsg);

		if(ret < 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hTDll);
			FreeLibrary(hDll);
			if (ret == ERR_CANCEL)
			{
				return SUCCESS;
			}
			_ErrDesc = errmsg;
			_errcd = ret;
			return ret;
		}
		
		stCOM.SFKBFreeDrv(hTDll);
		FreeLibrary(hDll);
		pwd = pin;
		TPKCK = tpkck;
		
		SetCurrentDirectory(buf);
	}else{
	//	return ReadMKeyBd(twice, *len, pwd);
	}

	return SUCCESS;
}

int  CDriverMKeyBd::GetPinPadID(char *idnum)
{
	debug("incoming GetPinPadID");
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	

	CString tmpStr;
	char errmsg[256];
	memset(errmsg, 0, 256);
	if(hasAdap)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + adapter);
		//SetCurrentDirectory(buf);
		if(!hDll)
		{
			SetCurrentDirectory(buf);
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		
		KB_ADAPTER_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		VPair inlist;
		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);
		
		if (0 == _ntimeout)
		{
			_ntimeout = _ndeftimeout;
		}
		CString tmpstr;
		tmpstr.Format("%d",_ntimeout);
		PAIR timeout("timeout", tmpstr);
		inlist.push_back(timeout);
		
		PAIR thirddrvname("drvname", thirdll);
		inlist.push_back(thirddrvname);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);


		HDLL hTDll = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
		
// 		FILE* file = fopen("dll.log","a+");
// 		fprintf(file,"��������Ϊ%s\n",_moduledir + thirdll);
// 		fclose(file);

		if(!hTDll)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			_errcd = -1;
			_ErrDesc = errmsg;
			return ERR_LOADDRV;
		}
		
		
		int ret = stCOM.KPGetPinPadID(hTDll,inlist,idnum, errmsg);

		if(ret < 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hTDll);
			FreeLibrary(hDll);
			if (ret == ERR_CANCEL)
			{
				return SUCCESS;
			}
			_ErrDesc = errmsg;
			_errcd = ret;
			return ret;
		}
		
		stCOM.SFKBFreeDrv(hTDll);
		FreeLibrary(hDll);
		
		
		SetCurrentDirectory(buf);
		return SUCCESS;	
	}
	return 0;
}

int CDriverMKeyBd::GetCustomEvaluate(int *value)
{
	debug("incoming GetCustomEvaluate");
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	

	CString tmpStr;
	char errmsg[256];
	memset(errmsg, 0, 256);
	if(hasAdap)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + adapter);
		//SetCurrentDirectory(buf);
		if(!hDll)
		{
			SetCurrentDirectory(buf);
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		
		KB_ADAPTER_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		VPair inlist;
		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("moduledir", _moduledir);
		inlist.push_back(dllnamepr);
		
		if (0 == _ntimeout)
		{
			_ntimeout = _ndeftimeout;
		}
		CString tmpstr;
		tmpstr.Format("%d",_ntimeout);
		PAIR timeout("timeout", tmpstr);
		inlist.push_back(timeout);
		
		PAIR thirddrvname("drvname", thirdll);
		inlist.push_back(thirddrvname);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);


		HDLL hTDll = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
		
// 		FILE* file = fopen("dll.log","a+");
// 		fprintf(file,"��������Ϊ%s\n",_moduledir + thirdll);
// 		fclose(file);

		if(!hTDll)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			_errcd = -1;
			_ErrDesc = errmsg;
			return ERR_LOADDRV;
		}
		

		*value = stCOM.PinPadReadEvaluate(hTDll,inlist,errmsg);

			
		stCOM.SFKBFreeDrv(hTDll);
		FreeLibrary(hDll);
		
		
		SetCurrentDirectory(buf);
		return SUCCESS;	
	}
	return 0;
}

int CDriverMKeyBd::driveKB(int optype, VPair &readlist)
{
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	
	CString tmpStr;
	char errmsg[256];
	memset(errmsg, 0, 256);
	debug("hasAdap:%d", hasAdap);
	if(hasAdap)
	{
		TCHAR buf[255];
		memset(buf, 0, 255);
		GetCurrentDirectory(255, buf);
		
		SetCurrentDirectory(_moduledir);
		HMODULE hDll = LoadLibrary(_moduledir + adapter);
		//SetCurrentDirectory(buf);
		if(!hDll)
		{
			SetCurrentDirectory(buf);
			return ERR_NOFINDMATCHDLL;
		}
        LPGetSFKBCOM GetSFKBCOM = (LPGetSFKBCOM)GetProcAddress(hDll, "GetSFKBCOM");
		if(!GetSFKBCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}
		debug("GetSFKBCOM success!");
		KB_ADAPTER_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(KB_ADAPTER_INTERFACE));
		BOOL bGetCOM = GetSFKBCOM(&stCOM);
		if(!bGetCOM)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		HDLL hTDll = stCOM.SFKBLoadDrv(_moduledir + thirdll, errmsg);
		if(!hTDll)
		{
			SetCurrentDirectory(buf);
			FreeLibrary(hDll);
			_errcd = -1;
			_ErrDesc = errmsg;
			return ERR_LOADDRV;
		}
		debug("SFKBLoadDrv success!");

		VPair inlist;
		typedef pair<CString, CString> PAIR;
		
		tmpStr.Format("%d", optype);
		PAIR type("optype", tmpStr);
		inlist.push_back(type);
		
		tmpStr.Format("%d", _porttype);
		PAIR PortTp("porttype", tmpStr);
		inlist.push_back(PortTp);
		
		tmpStr.Format("%d", _nPort);
		PAIR PortNo("portno", tmpStr);
		inlist.push_back(PortNo);
		
		if (4 == optype)
		{
			inlist.push_back(readlist[0]);
			inlist.push_back(readlist[1]);
		}
		else if (5 == optype)
		{
			inlist.push_back(readlist[0]);
			inlist.push_back(readlist[1]);
			inlist.push_back(readlist[2]);
		}
		else if (6 == optype)
		{
			inlist.push_back(readlist[0]);
			inlist.push_back(readlist[1]);
			inlist.push_back(readlist[2]);
			inlist.push_back(readlist[3]);
		}

		int ret = stCOM.driveKeyBd(hTDll, inlist, errmsg);

		readlist.clear();
		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if(ret < 0)
		{
			SetCurrentDirectory(buf);
			stCOM.SFKBFreeDrv(hTDll);
			FreeLibrary(hDll);
			_ErrDesc = errmsg;
			_errcd = ret;
			return ret;
		}

		if (3 == optype || 4 == optype)
		{
			readlist.push_back(inlist[0]);
			readlist.push_back(inlist[1]);
		}
		else if (5 == optype || 6 == optype)
		{
			readlist.push_back(inlist[0]);
		}
		
		stCOM.SFKBFreeDrv(hTDll);
		FreeLibrary(hDll);
		SetCurrentDirectory(buf);
	}
	else
	{
	}

	return SUCCESS;
}

int CDriverMKeyBd::ReadMKeyBd(int twice, int len, CString &pwd)
{
	CString tmpLogStr;
	tmpLogStr.Format("twice:%d,len:%d",twice,len);
	CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

	if(!twice)
		twice = 2;
	if(!len)
		len = 6;
	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);
	
	CString adapter, thirdll;
	BOOL hasAdap = FALSE;
	
	InitlizeConfigParam();
	_pParaXml->InitMapKeyList();
    _pParaXml->GetAdapterInfo(adapter, thirdll, hasAdap);
	
	CString tmpStr;
	CString pwd1;
	CString pwd2;
	char chStart[2];
	char chEnd[2];
	char chEnter[2] = "";
	char chClear[2];
	int ret ;
	char ch;
	if(!hasAdap)
	{
		tmpLogStr.Format("hasAdap:false!");
		CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

		_pWinPort->SetExitReadVar(TRUE);
		VStepList &steplist = _pParaXml->GetStepListRef();
		for(int i = 0; i < steplist.size(); i++)
		{
			stKeyBdDoStep &step = steplist[i];
			if(twice == 1 && step.flag == 2)
				continue;

			tmpLogStr.Format("step.oper:%s",step.oper);
			CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());
			
			if(step.oper == WRITE)
			{
				memset(neworder, 0, bufsize);

				tmpLogStr.Format("step.writedata:%s",step.writedata);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //��ʮ�������ַ���ת��Ϊʮ���ơ�
					return ERR_INVALIDORDER;    //ת��ָ�����

				tmpLogStr.Format("ת����ָ�neworder:%s",neworder);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				if(!_pWinPort->AsynSendData(neworder, strlen(neworder)))
					return ERR_SENDDATA;

				tmpLogStr.Format("AsynSendData finished");
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

			}
			else
			{
				memset(chStart, 0, 2);

				tmpLogStr.Format("step.startchar:%s",step.startchar);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				if(_pParaXml->ParaOrder(step.startchar, chStart) != 0)
					return ERR_INVALIDORDER;

				tmpLogStr.Format("chStart:%s\nstep.endchar:%s",chStart,step.endchar);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				memset(chEnd, 0, 2);
				if(_pParaXml->ParaOrder(step.endchar, chEnd) != 0)
					return ERR_INVALIDORDER;

				tmpLogStr.Format("chEnd:%s",step.endchar);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				memset(chClear, 0, 2);
				if(_pParaXml->ParaOrder(step.clear, chClear) != 0)
					return ERR_INVALIDORDER;
				
				tmpLogStr.Format("chClear:%s",step.clear);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				tmpStr.Empty();
				
				_pWinPort->InitVar();
				while((ret = _pWinPort->sReadChar(ch, TIME_OUT)) == 1)
				{
					tmpLogStr.Format("ch:%s",ch);
					CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

					if(chStart[0] != 0x00 && ch == chStart[0])	//������ʼ��־�ַ�����ʾ��ʾ����һ����ʼ����
						continue;
					else if(chEnd[0] != 0x00 && ch == chEnd[0]) //����������־�ַ����������룬���ַ����������뵱��
						break;
					else if(chClear[0] != 0x00 && ch == chClear[0])  //��������������ԭ��������룬������char
					{
						tmpStr.Empty();
						continue;
					}
					
					tmpStr += ch;
				}

				tmpLogStr.Format("tmpStr:%s;ret is:%d",tmpStr,ret);
				CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());

				if(ret == ERR_RESET)
					return SUCCESS_RESET; //��λ���ء�
				if(ret == _ERR_TIMEOUT)
					return ERR_READTIMEOUT;
				
				if(step.flag == 1)
				{
					pwd1 = tmpStr;
					//������Ҫmap��Key������ת������
					if(step.keymap)
					{
						ConvertPasswordStr(pwd1);
					}
				}
				else if(step.flag == 2)
				{
					pwd2 = tmpStr;
					if(step.keymap)
						ConvertPasswordStr(pwd2);
				}
			}
		}
		
		if(pwd2.GetLength() != len && pwd1.GetLength() != len)
			return ERR_LENNOTENOUGH;
		
		if(twice == 2)
		{
			if(pwd2 != pwd1)
				return ERR_TWICEPASSWORD;    //������������벻��;
		}
		
		pwd = pwd1;
		tmpLogStr.Format("the pwd is :%s",pwd);
		CDriverMKeyBd::runLog(tmpLogStr, this->GetModuleDir());
	}
	
	return SUCCESS;
}

int CDriverMKeyBd::InitlizeConfigParam()
{
	return _pParaXml->InitContrOrder();
}


void CDriverMKeyBd::runLog(CString logStr, CString logdir)
{
	
#ifdef DEBUG_H_LOG
	//д��־���ڲ��
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(logdir);
	long ntime ;
	time(&ntime);
	struct tm *tim = localtime(&ntime);
	CString logFileName;
	_mkdir(CAMERELOG_DIR);
	logFileName.Format("%s\\%04d%02d%02d.txt", CAMERELOG_DIR,tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
	m_stream = fopen(logFileName, "a+");
	if (m_stream != NULL)
	{
		
		fprintf(m_stream, "%02d:%02d:%02d----%s\n", tim->tm_hour,tim->tm_min,tim->tm_sec,logStr);
		fflush(m_stream);
	}
	fclose(m_stream);
	
	SetCurrentDirectory(buf);
#endif
	
	return;
}