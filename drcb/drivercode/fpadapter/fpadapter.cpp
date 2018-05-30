// fpadapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "fpadapter.h"
#include "DIRECT.H"

#include "fpcomtypedef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CFpadapterApp

BEGIN_MESSAGE_MAP(CFpadapterApp, CWinApp)
	//{{AFX_MSG_MAP(CFpadapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFpadapterApp construction

CFpadapterApp::CFpadapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFpadapterApp object

CFpadapterApp theApp;

#define CAMERELOG_DIR              "log"
static FILE *m_stream = NULL;
void runLog(CString logStr, CString logdir = "C:\\");

/********************************************
* 功  能：采集指纹模板接口。此接口提供给驱动调用。
* 参  数：@inList - 采集所需数据。
                    数组下标解释：0 - 模块所在目录 1 - 适配器库名称 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@pFpDat  - 指纹数据
* 参  数：@pFpLen  - 指纹数据长度
* 参  数：@errcd  - 错误代码
* 参  数：@errmsg - 错误信息。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI GetFpTemplate(VPair &inList, BYTE *pFpDat, DWORD *pFpLen, char *errcd, char *errmsg)
{
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);

	if(inList.size() != 6)
	{
		errcd = "-1";
		sprintf(errmsg, "缺少传入接口的参数!");
		return -1;
	}
	
	PAIR module = inList[0];
	PAIR apdater = inList[1];
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
    PAIR timeout = inList[5];

	int ret ;
	try
	{
		SetCurrentDirectory(module.second);
		HINSTANCE hDll = LoadLibrary(module.second + thirddll.second);
		SetCurrentDirectory(buf);
		if(!hDll)
		{
			sprintf(errcd, "-2");
			sprintf(errmsg, "%s驱动库未找到", thirddll.second);
			return -2;
		}
		LPGETFPTEMPLATE lpGetFpTemplate = (LPGETFPTEMPLATE)GetProcAddress(hDll, "SFPGetTemplate");
		if(!lpGetFpTemplate)
		{
			FreeLibrary(hDll);
			sprintf(errcd, "-3");
			sprintf(errmsg, "获取%s驱动库SFPGetTemplate接口失败!", thirddll.second);
			return -3;
		}

		LPGETERRMESS lpGetErrMess = (LPGETERRMESS)GetProcAddress(hDll, "SFPGetErrMess");
		if(!lpGetErrMess)
		{
			FreeLibrary(hDll);
			sprintf(errcd, "-4");
			sprintf(errmsg, "获取%s驱动库SFPGetErrMess接口失败!", thirddll.second);
			return -4;
		}

		if(atoi(porttype.second) == 2)
            ret = lpGetFpTemplate(0, atoi(timeout.second), pFpDat, pFpLen);
        else
			ret = lpGetFpTemplate(atoi(portno.second), atoi(timeout.second), pFpDat, pFpLen);
		Sleep(200);

		if(ret != 0)
		{
			sprintf(errcd, "-5");
			sprintf(errmsg, "%s", lpGetErrMess());
			FreeLibrary(hDll);
			return -5;
		}

		FreeLibrary(hDll);
	}
	catch (...)
	{
		sprintf(errcd, "-10");
		sprintf(errmsg, "获取指纹模板模块发生异常");
		return -10;
	}

	return 0;
}

/********************************************
* 功  能：采集指纹特征接口。此接口提供给驱动调用。
* 参  数：@inList - 采集所需数据。
                    数组下标解释：0 - 模块所在目录 1 - 适配器库名称 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@pFpDat  - 指纹数据
* 参  数：@pFpLen  - 指纹数据长度
* 参  数：@errcd  - 错误代码
* 参  数：@errmsg - 错误信息。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI GetFpData(VPair &inList, BYTE *pFpDat, DWORD *pFpLen, char *errcd, char *errmsg)
{
	CString logStr;
	if(inList.size() < 6)
	{
		sprintf(errcd, "-1");
		sprintf(errmsg, "缺少传入接口的参数!");
		return -1;
	}
	
	PAIR module = inList[0];
	PAIR apdater = inList[1];
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR timeout = inList[5];

	CString path = module.second + thirddll.second;
	int ps = path.ReverseFind('\\');
	CString dir2 = path.Mid(0, ps + 1);
	
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(dir2);
	HINSTANCE hDll = LoadLibrary(path);
	SetCurrentDirectory(buf);
	if(!hDll)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-10");
		sprintf(errmsg, "加载设备驱动库失败!\r\n%s", path);
		return -10;
	}

	int ret;
	/*
	char strSerId[256+1] = "";
	char szErrMsg[512+1] = "";
	char szVerBuf[1536] = "";
	int iVerLen = 0;
	
	FPIGetFeature lpFPIGetFeature = (FPIGetFeature)GetProcAddress(hDll, "FPIGetFeature");
	if(!lpFPIGetFeature)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-3");
		sprintf(errmsg, "获取%s驱动库FPIGetFeature接口失败!", thirddll.second);
		return -3;
	}
	//2为usb
	if(atoi(porttype.second) == 2)
        ret = lpFPIGetFeature(0, (char*)pFpDat, (int*)pFpLen, strSerId, szErrMsg);
		//ret = lpFPIGetFeature(0, szVerBuf, &iVerLen, strSerId, szErrMsg);
    else
		ret = lpFPIGetFeature(atoi(portno.second), (char*)pFpDat, (int*)pFpLen, strSerId, szErrMsg);
	Sleep(200);
	*/

	lpSCCBA_StartFinger SCCBA_StartFinger = (lpSCCBA_StartFinger)GetProcAddress(hDll, "SCCBA_StartFinger");
	if(SCCBA_StartFinger == NULL)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-9");
		sprintf(errmsg, "获取%s驱动库SCCBA_StartFinger函数失败!", thirddll.second);
		return -9;
	}

	// 端口
	CString portStr = portno.second;
	int iPortNo = atoi(portStr.GetBuffer(portStr.GetLength()));

	// 超时时间
	CString timeoutStr = timeout.second;
	int iTimeOut = atoi(timeoutStr.GetBuffer(timeoutStr.GetLength()));

	char picdata[81920+1] = {0};
	int picdataLen = 81920;

	char feature[81920+1] = {0};
	int featureLen = 81920;

	char fpTypeId[1024+1] = {0};

	logStr.Format("ReadFP--读指纹传入参数：port=%d，timeout=%d", iPortNo, iTimeOut);
	runLog(logStr, dir2);

	ret = SCCBA_StartFinger(iPortNo, "", iTimeOut, picdata, &picdataLen, feature, &featureLen, fpTypeId);

	if(ret != 0)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "读取指纹特征数据失败");
		return -5;
	}

	logStr.Format("ReadFP--读指纹厂商返回数据：picdata=%s, picdataLen=%d\r\nfeature=%s, featureLen=%d, fpTypeId=%s", picdata, picdataLen, feature, featureLen, fpTypeId);
	runLog(logStr, dir2);

	inList.clear();
	CString val;

	val.Format("FP_DEVICEID=%s", fpTypeId);
	val.Remove(' ');
	PAIR typeId("fpTypeId", val);
	inList.push_back(typeId);
	logStr.Format("ReadFP--读指纹返回设备编号：%s", val);
	runLog(logStr, dir2);

	val.Format("FP_FEATURE=%s", feature);
	val.Remove(' ');
	PAIR fpfeature("feature", val);
	inList.push_back(fpfeature);
	logStr.Format("ReadFP--读指纹返回特征数据：%s", val);
	runLog(logStr, dir2);

	val.Format("FP_PICDATA=%s", picdata);
	val.Remove(' ');
	PAIR fppicdata("picdata", val);
	inList.push_back(fppicdata);
	logStr.Format("ReadFP--读指纹返回图片数据：%s", val);
	runLog(logStr, dir2);

	FreeLibrary(hDll);
	return 0;
}

/********************************************
* 功  能：验印接口。此接口提供给驱动调用。
* 参  数：@inList - 验印所需数据。
* 参  数：@errcd  - 错误代码
* 参  数：@errmsg - 错误信息。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Match(VPair &inList, char *errcd, char *errmsg)
{
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	
	if(inList.size() < 13)
	{
		sprintf(errcd, "-1");
		sprintf(errmsg, "缺少传入接口的参数!");
		return -1;
	}
	
	PAIR module = inList[0];
	PAIR apdater = inList[1];
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR timeout = inList[5];
	PAIR fingerData = inList[6];
	PAIR pairUserid = inList[7];
	PAIR pairDepartNo = inList[8];
	PAIR pairSerIP = inList[9];
	PAIR pairSerPort = inList[10];
	PAIR pairSysno = inList[11];
	PAIR pairVendor = inList[12];
	
	int ret ;
	try
	{
		SetCurrentDirectory(module.second);
		HINSTANCE hDll = LoadLibrary(module.second + thirddll.second);
		SetCurrentDirectory(buf);
		if(!hDll)
		{
			sprintf(errcd, "-2");
			sprintf(errmsg, "%s驱动库未找到", thirddll.second);
			return -2;
		}
	
		FPIVerifyComm lpFPIVerifyComm = (FPIVerifyComm)GetProcAddress(hDll, "FPIVerifyComm");
		if(!lpFPIVerifyComm)
		{
			FreeLibrary(hDll);
			sprintf(errcd, "-3");
			sprintf(errmsg, "获取%s驱动库FPIVerifyComm接口失败!", thirddll.second);
			return -3;
		}
		//if(atoi(porttype.second) == 2)
            //ret = lpFPIGetFeature(0, (char*)pFpDat, (int*)pFpLen, strSerId, szErrMsg);
			//ret = lpFPIGetFeature(0, szVerBuf, &iVerLen, strSerId, szErrMsg);
        //else
			//ret = lpFPIGetFeature(atoi(portno.second), (char*)pFpDat, (int*)pFpLen, strSerId, szErrMsg);
		//Sleep(200);

		CString strSerIP = pairSerIP.second;
		int iSerport = atoi(pairSerPort.second);
		CString strSysNo = pairSysno.second;
		CString strDeptNo = pairDepartNo.second;
		CString strTlrNo = pairUserid.second;
		CString strVendor = pairVendor.second;
		CString strFingerData = fingerData.second;
		CString strSerID;
		char strSerId[256+1] = "";
		char szErrMsg[512+1] = "";

		CString strLog;
		strLog.Format("strSerIP:%s\niSerport:%d\nstrSysNo:%s\nstrDeptNo:%s\nstrTlrNo:%s\nstrVendor:%s\nstrFingerData:%s\nstrSerID:%s\n",
			strSerIP, iSerport, strSysNo, strDeptNo, strTlrNo, strVendor, strFingerData, strSerID);
		runLog(strLog, module.second);
		ret = lpFPIVerifyComm(strSerIP.GetBuffer(strSerIP.GetLength()), iSerport, strSysNo.GetBuffer(strSysNo.GetLength()),
			strDeptNo.GetBuffer(strDeptNo.GetLength()), strTlrNo.GetBuffer(strTlrNo.GetLength()), strVendor.GetBuffer(strVendor.GetLength()),
			strFingerData.GetBuffer(strFingerData.GetLength()), strSerID.GetBuffer(strSerID.GetLength()), szErrMsg);
		if(ret != 0)
		{
			sprintf(errcd, "-5");
			sprintf(errmsg, "%s", szErrMsg);
			FreeLibrary(hDll);
			return -5;
		}
		
		FreeLibrary(hDll);
	}
	catch (...)
	{
		sprintf(errcd, "-10");
		sprintf(errmsg, "获取指纹特征模块发生异常");
		return -10;
	}
	
	return 0;
	
}

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetFpCOM(FP_MATCH_INTERFACE *pFpInterf)
{
	if(pFpInterf)
	{
		pFpInterf->GetFpTemplate = GetFpTemplate;
		pFpInterf->GetFpData = GetFpData;
		pFpInterf->Match = Match;
		return TRUE;
	}

	return FALSE;
}


void runLog(CString logStr, CString logdir)
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
	logFileName.Format("%s\\%s%04d%02d%02d.txt", CAMERELOG_DIR, "FP", tim->tm_year+1900, tim->tm_mon+1, tim->tm_mday);
	m_stream = fopen(logFileName, "a+");
	if (m_stream != NULL)
	{
		fprintf(m_stream, "%02d:%02d:%02d----%s\n", tim->tm_hour, tim->tm_min, tim->tm_sec, logStr);
		fflush(m_stream);
	}
	fclose(m_stream);
	
	SetCurrentDirectory(buf);
#endif
	
	return;
}