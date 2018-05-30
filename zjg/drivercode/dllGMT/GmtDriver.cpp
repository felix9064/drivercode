// GmtDriver.cpp: implementation of the CGmtDriver class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"

#include "Win32Port.h"
#include "paraxml.h"
#include "DIRECT.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
HMODULE CGmtDriver::m_hLibModule = NULL;
bool CGmtDriver::m_bDriverLoaded = false;
FILE * CGmtDriver::m_stream = NULL;

CGmtDriver::CGmtDriver()
{
	p_GWQ_ClosePinPad = NULL;
	p_GWQ_SetCodeLen = NULL;
	p_GWQ_NonEpt_ReadPin = NULL;
	p_GWQ_InitPinPad = NULL;
	p_GWQ_UpdateMKey = NULL;
	p_GWQ_DownLoadWKey = NULL;
	p_GWQ_ActiveWKey = NULL;
	p_GWQ_GetDES = NULL;
	p_GWQ_GetPin = NULL;
	p_GWQ_StartInfoHtml = NULL;
	p_GWQ_WriteDeviceId = NULL;
	p_GWQ_ReadDeviceId = NULL;
	p_GWQ_StartKeyboard = NULL;
	p_GWQ_StartElectronicCard = NULL;
	p_GWQ_DownHeadFile = NULL;
	p_GWQ_FindHeadPhoto = NULL;
	p_GWQ_DelHeadFile = NULL;
	p_GWQ_Welcome = NULL;
	p_GWQ_SetEvaluateInfo = NULL;
	p_GWQ_StartEvaluate = NULL;
	p_GWQ_CancleEvaluate = NULL;
	p_GWQ_GetVideoList = NULL;
	p_GWQ_DownVideoFile = NULL;
	p_GWQ_DelVideoFile = NULL;
	p_GWQ_DelAllVideoFile = NULL;
	p_GWQ_VsUpgrade = NULL;
	p_GWQ_GetVer = NULL;
	p_GWQ_QueryFileInfo = NULL;
	p_GWQ_SetTimeValue = NULL;
	p_GWQ_CallShowPDF = NULL;
	p_GWQ_SetMD = NULL;
	p_GWQ_GetRandomZMK = NULL;

	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();

	_ndeftimeout = 15;
	_ntimeout = 0;
	m_baudrate = 115200;
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

VPair& CGmtDriver::GetRetListRef()
{
	return _Sub;
}

int CGmtDriver::OpenGmtDriver()
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

int CGmtDriver::LoadDriver()
{
	
	if (m_hLibModule)
	{
		m_bDriverLoaded = true;
		return SUCCESS;
	}
	
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(_moduledir);

    m_hLibModule = LoadLibrary(_moduledir + _CallDll);

	SetCurrentDirectory(buf);
	if( NULL == m_hLibModule)
	{
		return ERR_NOFINDMATCHDLL;
	}
	
	p_GWQ_ClosePinPad = (FUNC_GWQ_ClosePinPad)GetProcAddress(m_hLibModule, "GWQ_ClosePinPad");
	p_GWQ_SetCodeLen = (FUNC_GWQ_SetCodeLen)GetProcAddress(m_hLibModule, "GWQ_SetCodeLen");
	p_GWQ_NonEpt_ReadPin = (FUNC_GWQ_NonEpt_ReadPin)GetProcAddress(m_hLibModule, "GWQ_NonEpt_ReadPin");
	p_GWQ_InitPinPad = (FUNC_GWQ_InitPinPad)GetProcAddress(m_hLibModule, "GWQ_InitPinPad");
	p_GWQ_UpdateMKey = (FUNC_GWQ_UpdateMKey)GetProcAddress(m_hLibModule, "GWQ_UpdateMKey");
	p_GWQ_DownLoadWKey = (FUNC_GWQ_DownLoadWKey)GetProcAddress(m_hLibModule, "GWQ_DownLoadWKey");
	p_GWQ_ActiveWKey = (FUNC_GWQ_ActiveWKey)GetProcAddress(m_hLibModule, "GWQ_ActiveWKey");
	p_GWQ_GetDES = (FUNC_GWQ_GetDES)GetProcAddress(m_hLibModule, "GWQ_GetDES");
	p_GWQ_GetPin = (FUNC_GWQ_GetPin)GetProcAddress(m_hLibModule, "GWQ_GetPin");
	p_GWQ_StartInfoHtml = (FUNC_GWQ_StartInfoHtml)GetProcAddress(m_hLibModule, "GWQ_StartInfoHtml");
	p_GWQ_WriteDeviceId = (FUNC_GWQ_WriteDeviceId)GetProcAddress(m_hLibModule, "GWQ_WriteDeviceId");
	p_GWQ_ReadDeviceId = (FUNC_GWQ_ReadDeviceId)GetProcAddress(m_hLibModule, "GWQ_ReadDeviceId");
	p_GWQ_StartKeyboard = (FUNC_GWQ_StartKeyboard)GetProcAddress(m_hLibModule, "GWQ_StartKeyboard");
	p_GWQ_StartElectronicCard = (FUNC_GWQ_StartElectronicCard)GetProcAddress(m_hLibModule, "GWQ_StartElectronicCard");
	p_GWQ_DownHeadFile = (FUNC_GWQ_DownHeadFile)GetProcAddress(m_hLibModule, "GWQ_DownHeadFile");
	p_GWQ_FindHeadPhoto = (FUNC_GWQ_FindHeadPhoto)GetProcAddress(m_hLibModule, "GWQ_FindHeadPhoto");
	p_GWQ_DelHeadFile = (FUNC_GWQ_DelHeadFile)GetProcAddress(m_hLibModule, "GWQ_DelHeadFile");
	p_GWQ_Welcome = (FUNC_GWQ_Welcome)GetProcAddress(m_hLibModule, "GWQ_Welcome");
	p_GWQ_SetEvaluateInfo = (FUNC_GWQ_SetEvaluateInfo)GetProcAddress(m_hLibModule, "GWQ_SetEvaluateInfo");
	p_GWQ_StartEvaluate = (FUNC_GWQ_StartEvaluate)GetProcAddress(m_hLibModule, "GWQ_StartEvaluate");
	p_GWQ_CancleEvaluate = (FUNC_GWQ_CancleEvaluate)GetProcAddress(m_hLibModule, "GWQ_CancleEvaluate");
	p_GWQ_GetVideoList = (FUNC_GWQ_GetVideoList)GetProcAddress(m_hLibModule, "GWQ_GetVideoList");
	p_GWQ_DownVideoFile = (FUNC_GWQ_DownVideoFile)GetProcAddress(m_hLibModule, "GWQ_DownVideoFile");
	p_GWQ_DelVideoFile = (FUNC_GWQ_DelVideoFile)GetProcAddress(m_hLibModule, "GWQ_DelVideoFile");
	p_GWQ_DelAllVideoFile = (FUNC_GWQ_DelAllVideoFile)GetProcAddress(m_hLibModule, "GWQ_DelAllVideoFile");
	p_GWQ_VsUpgrade = (FUNC_GWQ_VsUpgrade)GetProcAddress(m_hLibModule, "GWQ_VsUpgrade");
	p_GWQ_GetVer = (FUNC_GWQ_GetVer)GetProcAddress(m_hLibModule, "GWQ_GetVer");
	p_GWQ_QueryFileInfo = (FUNC_GWQ_QueryFileInfo)GetProcAddress(m_hLibModule, "GWQ_QueryFileInfo");
	p_GWQ_SetTimeValue = (FUNC_GWQ_SetTimeValue)GetProcAddress(m_hLibModule, "GWQ_SetTimeValue");
	p_GWQ_CallShowPDF = (FUNC_GWQ_CallShowPDF)GetProcAddress(m_hLibModule, "GWQ_CallShowPDF");
	p_GWQ_SetMD = (FUNC_GWQ_SetMD)GetProcAddress(m_hLibModule, "GWQ_SetMD");
	p_GWQ_GetRandomZMK = (FUNC_GWQ_GetRandomZMK)GetProcAddress(m_hLibModule, "GWQ_GetRandomZMK");

	if(NULL == p_GWQ_ClosePinPad || NULL == p_GWQ_SetCodeLen || NULL == p_GWQ_NonEpt_ReadPin || NULL == p_GWQ_InitPinPad
		|| NULL == p_GWQ_UpdateMKey || NULL == p_GWQ_DownLoadWKey || NULL == p_GWQ_ActiveWKey || NULL == p_GWQ_GetDES
		|| NULL == p_GWQ_GetPin || NULL == p_GWQ_StartInfoHtml || NULL == p_GWQ_WriteDeviceId || NULL == p_GWQ_ReadDeviceId
		|| NULL == p_GWQ_StartKeyboard || NULL == p_GWQ_StartElectronicCard || NULL == p_GWQ_DownHeadFile || NULL == p_GWQ_FindHeadPhoto
		|| NULL == p_GWQ_DelHeadFile || NULL == p_GWQ_Welcome || NULL == p_GWQ_SetEvaluateInfo || NULL == p_GWQ_StartEvaluate
		|| NULL == p_GWQ_CancleEvaluate || NULL == p_GWQ_GetVideoList || NULL == p_GWQ_DownVideoFile || NULL == p_GWQ_DelVideoFile
		|| NULL == p_GWQ_DelAllVideoFile || NULL == p_GWQ_VsUpgrade || NULL == p_GWQ_GetVer || NULL == p_GWQ_QueryFileInfo
		|| NULL == p_GWQ_SetTimeValue || NULL == p_GWQ_CallShowPDF || NULL == p_GWQ_SetMD || NULL == p_GWQ_GetRandomZMK)
    {
        FreeLibrary(m_hLibModule);
        return -1;
    }

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

void CGmtDriver::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CGmtDriver::SetPortNo(CString port)
{
	_portno = port;
}

void CGmtDriver::SetTimeOut(short timeout)
{
	_ntimeout = timeout;
}

void CGmtDriver::SetTimeOutZero()
{
	_ntimeout = 0;
}

CString & CGmtDriver::GetPortNo()
{
	return _portno;
}

int CGmtDriver::getNport()
{
	return _nPort;
}

int CGmtDriver::driveGmt(int optype, VPair &readlist)
{
	CString logStr;
	CString tmpStr;
	char extendPort = '9';
	int ret = 0;

	if (1 == optype)
	{
		if (NULL == p_GWQ_ClosePinPad)
		{
			return ERR_NOSUCHMETHOD;
		}

		CGmtDriver::runLog("调用函数--GWQ_ClosePinPad--开始", _moduledir);

		ret = p_GWQ_ClosePinPad(_nPort, extendPort, m_baudrate);
		
		logStr.Format("调用函数--GWQ_ClosePinPad--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (2 == optype)
	{
		if (NULL == p_GWQ_SetCodeLen)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR PinLen = readlist[0];
		CString pinLenStr = PinLen.second;
		int iPinLen = atoi(pinLenStr.GetBuffer(pinLenStr.GetLength()));

		CGmtDriver::runLog("调用函数--GWQ_SetCodeLen--开始", _moduledir);

		ret = p_GWQ_SetCodeLen(_nPort, extendPort, m_baudrate, iPinLen);

		logStr.Format("调用函数--GWQ_SetCodeLen--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (3 == optype)
	{
		if (NULL == p_GWQ_NonEpt_ReadPin)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR VoiceType = readlist[0];
		CString voiceStr = VoiceType.second;
		int iVoiceType = atoi(voiceStr.GetBuffer(voiceStr.GetLength()));

		PAIR timeout = readlist[1];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR PinSize = readlist[2];
		CString pinSizeStr = PinSize.second;
		int iPinSize = atoi(pinSizeStr.GetBuffer(pinSizeStr.GetLength()));

		char Pin[64] = {0};
		memset(Pin, 0x00, sizeof(Pin));

		CGmtDriver::runLog("调用函数--GWQ_NonEpt_ReadPin--开始", _moduledir);

		ret = p_GWQ_NonEpt_ReadPin(_nPort, extendPort, m_baudrate, iVoiceType, iTimeOut, Pin, iPinSize);

		logStr.Format("调用函数--GWQ_NonEpt_ReadPin--返回值ret=%d, Pin=%s", ret, Pin);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", Pin);
			PAIR Password("Pin", tmpStr);
			readlist.push_back(Password);
		}

	}
	else if (4 == optype)
	{
		if (NULL == p_GWQ_InitPinPad)
		{
			return ERR_NOSUCHMETHOD;
		}

		CGmtDriver::runLog("调用函数--GWQ_InitPinPad--开始", _moduledir);

		ret = p_GWQ_InitPinPad(_nPort, extendPort, m_baudrate);

		logStr.Format("调用函数--GWQ_InitPinPad--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (5 == optype)
	{
		if (NULL == p_GWQ_UpdateMKey)
		{
			return ERR_NOSUCHMETHOD;
		}

		char Zmk[64] = {0};
		char CheckValue[16] = {0};

		PAIR index = readlist[0];
		CString indexStr = index.second;
		int ZmkIndex = atoi(indexStr.GetBuffer(indexStr.GetLength()));

		PAIR length = readlist[1];
		CString lenStr = length.second;
		int ZmkLength = atoi(lenStr.GetBuffer(lenStr.GetLength()));

		PAIR zmk = readlist[2];
		CString zmkStr = zmk.second;
		strcpy(Zmk, zmkStr);

		PAIR check = readlist[3];
		CString checkStr = check.second;
		strcpy(CheckValue, checkStr);

		CGmtDriver::runLog("调用函数--GWQ_UpdateMKey--开始", _moduledir);

		ret = p_GWQ_UpdateMKey(_nPort, extendPort, m_baudrate, ZmkIndex, ZmkLength, Zmk, CheckValue);

		logStr.Format("调用函数--GWQ_UpdateMKey--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		CGmtDriver::runLog("已清空readlist", _moduledir);

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		CGmtDriver::runLog("已将返回码ret放入readlist", _moduledir);
	}
	else if (6 == optype)
	{
		if (NULL == p_GWQ_DownLoadWKey)
		{
			return ERR_NOSUCHMETHOD;
		}

		char Key[64] = {0};
		char CheckValue[16] = {0};

		PAIR mKeyIndex = readlist[0];
		CString mIndexStr = mKeyIndex.second;
		int MKeyIndex = atoi(mIndexStr.GetBuffer(mIndexStr.GetLength()));

		PAIR wKeyIndex = readlist[1];
		CString wIndexStr = wKeyIndex.second;
		int WKeyIndex = atoi(wIndexStr.GetBuffer(wIndexStr.GetLength()));

		PAIR length = readlist[2];
		CString lenStr = length.second;
		int WKeyLength = atoi(lenStr.GetBuffer(lenStr.GetLength()));

		PAIR key = readlist[3];
		CString keyStr = key.second;
		strcpy(Key, keyStr);

		PAIR check = readlist[4];
		CString checkStr = check.second;
		strcpy(CheckValue, checkStr);

		CGmtDriver::runLog("调用函数--GWQ_DownLoadWKey--开始", _moduledir);

		ret = p_GWQ_DownLoadWKey(_nPort, extendPort, m_baudrate, MKeyIndex, WKeyIndex, WKeyLength, Key, CheckValue);

		logStr.Format("调用函数--GWQ_DownLoadWKey--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (7 == optype)
	{
		if (NULL == p_GWQ_ActiveWKey)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR mKeyIndex = readlist[0];
		CString mIndexStr = mKeyIndex.second;
		int MKeyIndex = atoi(mIndexStr.GetBuffer(mIndexStr.GetLength()));

		PAIR wKeyIndex = readlist[1];
		CString wIndexStr = wKeyIndex.second;
		int WKeyIndex = atoi(wIndexStr.GetBuffer(wIndexStr.GetLength()));

		CGmtDriver::runLog("调用函数--GWQ_ActiveWKey--开始", _moduledir);

		ret = p_GWQ_ActiveWKey(_nPort, extendPort, m_baudrate, MKeyIndex, WKeyIndex);

		logStr.Format("调用函数--GWQ_ActiveWKey--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (8 == optype)
	{
		if (NULL == p_GWQ_GetDES)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR voiceType = readlist[1];
		CString vTypeStr = voiceType.second;
		int VoiceType = atoi(vTypeStr.GetBuffer(vTypeStr.GetLength()));

		PAIR endType = readlist[2];
		CString eTypeStr = endType.second;
		int EndType = atoi(eTypeStr.GetBuffer(eTypeStr.GetLength()));

		char PinCrypt[48] = {0};
		memset(PinCrypt, 0x00, sizeof(PinCrypt));

		CGmtDriver::runLog("调用函数--GWQ_GetDES--开始", _moduledir);

		ret = p_GWQ_GetDES(_nPort, extendPort, m_baudrate, iTimeOut, VoiceType, EndType, PinCrypt);

		logStr.Format("调用函数--GWQ_GetDES--返回值ret=%d, PinCrypt=%s", ret, PinCrypt);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", PinCrypt);
			PAIR pinCrypt("PinCrypt", tmpStr);
			readlist.push_back(pinCrypt);
		}
	}
	else if (9 == optype)
	{
		if (NULL == p_GWQ_GetPin)
		{
			return ERR_NOSUCHMETHOD;
		}

		char AccNo[16] = {0};

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR accNo = readlist[1];
		CString accNoStr = accNo.second;
		strcpy(AccNo, accNoStr);

		PAIR voiceType = readlist[2];
		CString vTypeStr = voiceType.second;
		int VoiceType = atoi(vTypeStr.GetBuffer(vTypeStr.GetLength()));

		PAIR endType = readlist[3];
		CString eTypeStr = endType.second;
		int EndType = atoi(eTypeStr.GetBuffer(eTypeStr.GetLength()));

		char PinCrypt[48] = {0};
		memset(PinCrypt, 0x00, sizeof(PinCrypt));

		CGmtDriver::runLog("调用函数--GWQ_GetPin--开始", _moduledir);

		ret = p_GWQ_GetPin(_nPort, extendPort, m_baudrate, iTimeOut, AccNo, VoiceType, EndType, PinCrypt);

		logStr.Format("调用函数--GWQ_GetPin--返回值ret=%d, PinCrypt=%s", ret, PinCrypt);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", PinCrypt);
			PAIR pinCrypt("PinCrypt", tmpStr);
			readlist.push_back(pinCrypt);
		}
	}
	else if (10 == optype)
	{
		if (NULL == p_GWQ_StartInfoHtml)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR pModex = readlist[1];
		CString modexStr = pModex.second;
		int modex = atoi(modexStr.GetBuffer(modexStr.GetLength()));

		char info[2048] = {0};
		PAIR Info = readlist[2];
		CString infoStr = Info.second;
		strcpy(info, infoStr);

		int iDisplayResult = 0;

		CGmtDriver::runLog("调用函数--GWQ_StartInfoHtml--开始", _moduledir);
		
		ret = p_GWQ_StartInfoHtml(_nPort, extendPort, m_baudrate, iTimeOut, modex, info, &iDisplayResult);

		logStr.Format("调用函数--GWQ_StartInfoHtml--返回值ret=%d, iDisplayResult=%d", ret, iDisplayResult);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%d", iDisplayResult);
			PAIR DisplayResult("DisplayResult", tmpStr);
			readlist.push_back(DisplayResult);
		}
	}
	else if (11 == optype)
	{
		if (NULL == p_GWQ_WriteDeviceId)
		{
			return ERR_NOSUCHMETHOD;
		}

		char pDeviceId[20] = {0};
		PAIR DeviceId = readlist[0];
		CString deviceIdStr = DeviceId.second;
		strcpy(pDeviceId, deviceIdStr);

		CGmtDriver::runLog("调用函数--GWQ_WriteDeviceId--开始", _moduledir);

		ret = p_GWQ_WriteDeviceId(_nPort, extendPort, m_baudrate, pDeviceId);

		logStr.Format("调用函数--GWQ_WriteDeviceId--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (12 == optype)
	{
		if (NULL == p_GWQ_ReadDeviceId)
		{
			return ERR_NOSUCHMETHOD;
		}

		int iSize = 20;
		char pDeviceId[20] = {0};
		memset(pDeviceId, 0x00, sizeof(pDeviceId));

		CGmtDriver::runLog("调用函数--GWQ_ReadDeviceId--开始", _moduledir);
		
		ret = p_GWQ_ReadDeviceId(_nPort, extendPort, m_baudrate, pDeviceId, iSize);

		logStr.Format("调用函数--GWQ_ReadDeviceId--返回值ret=%d, pDeviceId=%s", ret, pDeviceId);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", pDeviceId);
			PAIR DeviceId("pDeviceId", tmpStr);
			readlist.push_back(DeviceId);
		}
	}
	else if (13 == optype)
	{
		if (NULL == p_GWQ_StartKeyboard)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR NumType = readlist[1];
		CString numStr = NumType.second;
		int numType = atoi(numStr.GetBuffer(numStr.GetLength()));

		char iDisplayResult[32] = {0};
		memset(iDisplayResult, 0x00, sizeof(iDisplayResult));

		CGmtDriver::runLog("调用函数--GWQ_StartKeyboard--开始", _moduledir);

		ret = p_GWQ_StartKeyboard(_nPort, extendPort, m_baudrate, iTimeOut, numType, iDisplayResult);

		logStr.Format("调用函数--GWQ_StartKeyboard--返回值ret=%d, iDisplayResult=%s", ret, iDisplayResult);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", iDisplayResult);
			PAIR DisplayResult("iDisplayResult", tmpStr);
			readlist.push_back(DisplayResult);
		}
	}
	else if (14 == optype)
	{
		if (NULL == p_GWQ_StartElectronicCard)
		{
			return ERR_NOSUCHMETHOD;
		}

		char tellerName[32] = {0};
		char tellerNo[8] = {0};
		char headFile[128] = {0};

		PAIR TellerName = readlist[0];
		CString tNameStr = TellerName.second;
		strcpy(tellerName, tNameStr);
		
		PAIR TellerNo = readlist[1];
		CString tNoStr = TellerNo.second;
		strcpy(tellerNo, tNoStr);

		PAIR StarLevel = readlist[2];
		CString sLevelStr = StarLevel.second;
		int nStarLevel = atoi(sLevelStr.GetBuffer(sLevelStr.GetLength()));

		PAIR HeadFile = readlist[3];
		CString hFileStr = HeadFile.second;
		strcpy(headFile, hFileStr);

		PAIR timeout = readlist[4];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		CGmtDriver::runLog("调用函数--GWQ_StartElectronicCard--开始", _moduledir);

		ret = p_GWQ_StartElectronicCard(_nPort, extendPort, m_baudrate, tellerName, tellerNo, nStarLevel, headFile, iTimeOut);

		logStr.Format("调用函数--GWQ_StartElectronicCard--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (15 == optype)
	{
		if (NULL == p_GWQ_DownHeadFile)
		{
			return ERR_NOSUCHMETHOD;
		}
		
		char pFilePath[1024] = {0};

		PAIR FilePath = readlist[0];
		CString fPathStr = FilePath.second;
		strcpy(pFilePath, fPathStr);

		CGmtDriver::runLog("调用函数--GWQ_DownHeadFile--开始", _moduledir);

		ret = p_GWQ_DownHeadFile(_nPort, extendPort, m_baudrate, pFilePath);

		logStr.Format("调用函数--GWQ_DownHeadFile--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (16 == optype)
	{
		if (NULL == p_GWQ_FindHeadPhoto)
		{
			return ERR_NOSUCHMETHOD;
		}
		
		char pHeadPhoto[128] = {0};

		PAIR HeadPhoto = readlist[0];
		CString hPhotoStr = HeadPhoto.second;
		strcpy(pHeadPhoto, hPhotoStr);

		CGmtDriver::runLog("调用函数--GWQ_FindHeadPhoto--开始", _moduledir);

		ret = p_GWQ_FindHeadPhoto(_nPort, extendPort, m_baudrate, pHeadPhoto);

		logStr.Format("调用函数--GWQ_FindHeadPhoto--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (17 == optype)
	{
		if (NULL == p_GWQ_DelHeadFile)
		{
			return ERR_NOSUCHMETHOD;
		}
		
		char pFileName[128] = {0};

		PAIR FileName = readlist[0];
		CString fNameStr = FileName.second;
		strcpy(pFileName, fNameStr);

		CGmtDriver::runLog("调用函数--GWQ_DelHeadFile--开始", _moduledir);

		ret = p_GWQ_DelHeadFile(_nPort, extendPort, m_baudrate, pFileName);

		logStr.Format("调用函数--GWQ_DelHeadFile--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (18 == optype)
	{
		if (NULL == p_GWQ_Welcome)
		{
			return ERR_NOSUCHMETHOD;
		}

		CGmtDriver::runLog("调用函数--GWQ_Welcome--开始", _moduledir);

		ret = p_GWQ_Welcome(_nPort, extendPort, m_baudrate);

		logStr.Format("调用函数--GWQ_Welcome--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (19 == optype)
	{
		if (NULL == p_GWQ_SetEvaluateInfo)
		{
			return ERR_NOSUCHMETHOD;
		}

		char tellerName[32] = {0};
		char tellerNo[8] = {0};
		char headFile[128] = {0};

		PAIR TellerName = readlist[0];
		CString tNameStr = TellerName.second;
		strcpy(tellerName, tNameStr);
		
		PAIR TellerNo = readlist[1];
		CString tNoStr = TellerNo.second;
		strcpy(tellerNo, tNoStr);

		PAIR StarLevel = readlist[2];
		CString sLevelStr = StarLevel.second;
		int nStarLevel = atoi(sLevelStr.GetBuffer(sLevelStr.GetLength()));

		PAIR HeadFile = readlist[3];
		CString hFileStr = HeadFile.second;
		strcpy(headFile, hFileStr);

		CGmtDriver::runLog("调用函数--GWQ_SetEvaluateInfo--开始", _moduledir);

		ret = p_GWQ_SetEvaluateInfo(_nPort, extendPort, m_baudrate, tellerName, tellerNo, nStarLevel, headFile);

		logStr.Format("调用函数--GWQ_SetEvaluateInfo--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (20 == optype)
	{
		if (NULL == p_GWQ_StartEvaluate)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		int evalValue = 0;

		CGmtDriver::runLog("调用函数--GWQ_StartEvaluate--开始", _moduledir);

		ret = p_GWQ_StartEvaluate(_nPort, extendPort, m_baudrate, iTimeOut, &evalValue);

		logStr.Format("调用函数--GWQ_StartEvaluate--返回值ret=%d, evalValue=%d", ret, evalValue);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%d", evalValue);
			PAIR EvalValue("evalValue", tmpStr);
			readlist.push_back(EvalValue);
		}
	}
	else if (21 == optype)
	{
		if (NULL == p_GWQ_CancleEvaluate)
		{
			return ERR_NOSUCHMETHOD;
		}

		CGmtDriver::runLog("调用函数--GWQ_CancleEvaluate--开始", _moduledir);

		ret = p_GWQ_CancleEvaluate(_nPort, extendPort, m_baudrate);

		logStr.Format("调用函数--GWQ_CancleEvaluate--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);

		readlist.clear();

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (30 == optype)
	{
		if (NULL == p_GWQ_CallShowPDF)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR mode = readlist[1];
		CString smode = mode.second;
		int signMode = atoi(smode.GetBuffer(smode.GetLength()));

		char pdfPath[4096] = {0};
		char location[128] = {0};
		char imgPath[1024] = {0};
		char textPath[1024] = {0};

		strcpy(pdfPath, readlist[2].second);
		strcpy(location, readlist[3].second);
		strcpy(imgPath, readlist[4].second);
		strcpy(textPath, readlist[5].second);

		CGmtDriver::runLog("调用函数--GWQ_CallShowPDF--开始", _moduledir);

		ret = p_GWQ_CallShowPDF(_nPort, extendPort, m_baudrate, iTimeOut, signMode, pdfPath, location, imgPath, textPath);
		
		logStr.Format("调用函数--GWQ_CallShowPDF--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);
		
		readlist.clear();
		
		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if(31 == optype)
	{
		if (NULL == p_GWQ_SetMD)
		{
			return ERR_NOSUCHMETHOD;
		}

		PAIR mformat = readlist[0];
		CString mformatStr = mformat.second;
		int formatModel = atoi(mformatStr.GetBuffer(mformatStr.GetLength()));

		CGmtDriver::runLog("调用函数--GWQ_SetMD--开始", _moduledir);

		ret = p_GWQ_SetMD(_nPort, extendPort, m_baudrate, formatModel);
		
		logStr.Format("调用函数--GWQ_SetMD--返回值ret=%d", ret);
		CGmtDriver::runLog(logStr, _moduledir);
		
		readlist.clear();
		
		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);
	}
	else if (32 == optype)
	{
		if (NULL == p_GWQ_GetRandomZMK)
		{
			return ERR_NOSUCHMETHOD;
		}

		char pubkey[256] = {0};
		char CryptZMK[256] = {0};
		memset(CryptZMK, 0x00, sizeof(CryptZMK));

		PAIR timeout = readlist[0];
		CString time = timeout.second;
		int iTimeOut = atoi(time.GetBuffer(time.GetLength()));

		PAIR PubKey = readlist[1];
		CString pubKeyStr = PubKey.second;
		strcpy(pubkey, pubKeyStr);

		PAIR SuanfaType = readlist[2];
		CString suanfaStr = SuanfaType.second;
		int suanfa = atoi(suanfaStr.GetBuffer(suanfaStr.GetLength()));

		CGmtDriver::runLog("调用函数--GWQ_GetRandomZMK--开始", _moduledir);

		ret = p_GWQ_GetRandomZMK(_nPort, extendPort, m_baudrate, iTimeOut, pubkey, suanfa, CryptZMK);
		
		logStr.Format("调用函数--GWQ_GetRandomZMK--返回值ret=%d, CryptZMK=%s", ret, CryptZMK);
		CGmtDriver::runLog(logStr, _moduledir);
		
		readlist.clear();
		
		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		readlist.push_back(returncode);

		if (0 == ret)
		{
			tmpStr.Format("%s", CryptZMK);
			PAIR cryptZMK("CryptZMK", tmpStr);
			readlist.push_back(cryptZMK);
		}
	}
	return ret;
}

void CGmtDriver::runLog(CString logStr, CString logdir)
{
	
#ifdef DEBUG_H_LOG
	//写日志便于查错
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(logdir);
	long ntime ;
	time(&ntime);
	struct tm *tim = localtime(&ntime);
	CString logFileName;
	_mkdir(CAMERELOG_DIR);
	logFileName.Format("%s\\gmt%04d%02d%02d.txt", CAMERELOG_DIR,tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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