// NanTKBAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NanTKBAdapter.h"
#include "variabledef.h"

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
// CNanTKBAdapterApp

BEGIN_MESSAGE_MAP(CNanTKBAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CNanTKBAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNanTKBAdapterApp construction

CNanTKBAdapterApp::CNanTKBAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNanTKBAdapterApp object

CNanTKBAdapterApp theApp;



/********************************************
* 功  能：加载驱动库
* 参  数：@path - 库所在路径
* 返回值：成功返回非0,否则0
*********************************************/
HDLL WINAPI SFKBLoadDrv(TPCHR path, PCHR ErrDesc)
{
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	
	CString tmp = path;
	CString dir = tmp.Mid(0, tmp.ReverseFind('\\') + 1);

	SetCurrentDirectory(dir);
	HDLL hDrv = LoadLibrary(path);
	SetCurrentDirectory(buf);
    if(!hDrv)
	{
		FILE* file = fopen("dll.log","a+");
		fprintf(file,"加载失败\n");
		fclose(file);
		sprintf(ErrDesc, "%s驱动库未找到", path);
		return 0;
	}

	return hDrv;
}

/********************************************
* 功  能：释放驱动库
* 参  数：@hDrv - 驱动库句柄
* 返回值：无
*********************************************/
void WINAPI SFKBFreeDrv(HDLL hDrv)
{
	FreeLibrary(hDrv);
}


/********************************************
* 功  能：加密Pin
* 参  数：@inList - 采集所需数据。
	                 数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号 5 - 密码长度
* 参  数：@MKeyIdx - 主密钥索引号
* 参  数：@WKeyIdx - 工作密钥索引号
* 参  数：@ACCTNO  - 客户帐号
* 参  数：@mode    - 语音模式 1：一次输入密码。2：两次输入密码
* 参  数：@PIN     - 传出参数，Pin密文
* 参  数：@TPKCheck - 传出参数，加密PIN的TPK密钥校验值。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SFKBReadPin(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR ACCTNO, int mode, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR pwdlen = inList[5];


    if(!hDrv)
	{
		sprintf(ErrDesc, "%s驱动库未找到", thirddll.second);
	//	AfxMessageBox("驱动库未找到");
		return -1;
	}
	/*
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	if(!OpenComm)
	{
		sprintf(ErrDesc, "%s驱动库中OpenComm未找到", thirddll.second);
		return -6;
	}
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	if(!CloseComm)
	{
		sprintf(ErrDesc, "%s驱动库中CloseComm未找到", thirddll.second);
		return -7;
	}
	lpCMCC_SetPinlength CMCC_SetPinlength = (lpCMCC_SetPinlength)GetProcAddress(hDrv, "CMCC_SetPinlength");
	if (!CMCC_SetPinlength)
	{
        sprintf(ErrDesc, "%s驱动库中CMCC_SetPinlength未找到", thirddll.second);
		return -9;
	}
	lpCMCC_ReadPinANSI98 CMCC_ReadPinANSI98 = (lpCMCC_ReadPinANSI98)GetProcAddress(hDrv, "CMCC_ReadPinANSI98");
	if(!CMCC_ReadPinANSI98)
	{
		sprintf(ErrDesc, "%s驱动库中CMCC_ReadPinANSI98未找到", thirddll.second);
		return -8;
	}	
	*/

	lpReadPin ReadPin = (lpReadPin)GetProcAddress(hDrv,"readPin");
	if(!ReadPin)
	{
		sprintf(ErrDesc, "%s驱动库中lpReadPin未找到", thirddll.second);
		//AfxMessageBox("getprocaddress-readPin");
		return -8;
	}
	/*
	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
*/
	try
	{
		CString port;
		port.Format("COM%s",portno.second);
	//	AfxMessageBox("the port");
	//	PIN = ReadPin(0,mode,atoi(pwdlen.second),port.GetBuffer(port.GetLength()),9600,timeout);
		//AfxMessageBox("after read");
	/*
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "打开串口失败!");
		    return -3;
		}

		ret = CMCC_SetPinlength(hComm, 6, 6, 0, timeout);
		if (ret != 0)
		{
            sprintf(ErrDesc, "设置密码长度失败!");
			CloseComm(hComm);
			return -11;
		}

		int pinlen = 0;
		if(mode == 1)
		{
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 0, &pinlen, PIN, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "读取PinBlock失败!");
				CloseComm(hComm);
				return -4;
			}

		}
		else
		{
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 0, &pinlen, PIN, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "读取PinBlock失败!");
				CloseComm(hComm);
				return -4;
			}

			char PIN2[BUFSIZ_256];
			memset(PIN2, 0, BUFSIZ_256);
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 1, &pinlen, PIN2, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "第二次读取PinBlock失败!");
				CloseComm(hComm);
				return -4;
			}

			if(strcmp(PIN, PIN2) != 0)
			{
				sprintf(ErrDesc, "两次输入的密码不一致!");
				CloseComm(hComm);
				return -5;
			}
		}

		CloseComm(hComm);
		*/
	}
	catch (...)
	{
		sprintf(ErrDesc, "获取PinBlock模块发生异常!");
		return -10;
	}


	return 0;
}
int WINAPI SFKBReadPin2(HDLL hDrv, VPair &inList, TPCHR MKeyIdx, TPCHR WKeyIdx, TPCHR ACCTNO, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR minLen = inList[5];
	PAIR maxLen = inList[6];
	PAIR prBaudrate = inList[7];
	PAIR prTwice = inList[8];
	PAIR srvType = inList[9];
	CString srvtype = srvType.second;
	
	PinPadGuomiPinNew ReadPinNew = (PinPadGuomiPinNew)GetProcAddress(hDrv,"PinPadGuomiPinNew");
	PinPadGuomiPin ReadPin = (PinPadGuomiPin)GetProcAddress(hDrv,"PinPadGuomiPin");
	if(ReadPinNew == NULL || ReadPin == NULL)
	{
		sprintf(ErrDesc, "%s驱动库中未找到PinPadGuomiPin或PinPadGuomiPinNew", thirddll.second);
		return -19;
	}
	//debug("PinPadGuomiPin success!");

	CString csport = (porttype.second=="1"?"COM":"USB") + portno.second;
	
	debug("输入参数：%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",csport,"1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,srvType.second);

	int ret;
	if ("0" == srvtype)
	{
		ret = ReadPin(csport,"","1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,PIN);
	}
	else
	{
		ret = ReadPinNew(csport,"",srvtype.GetBuffer(srvtype.GetLength()),"1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,PIN);
	}

	//debug("返回值：%d,密码：%s",ret,PIN);
	if (ret<0)
	{
		debug("调用失败,返回值%d,参数为:%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ret,csport,"","1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second);
		return -14;//读密码失败
	}
	if (prTwice.second == "2")
	{
		char PIN2[32]={0};
		if ("0" == srvtype)
		{
			ret = ReadPin(csport,"","1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second,PIN2);
		}
		else
		{
			ret = ReadPinNew(csport,"",srvtype.GetBuffer(srvtype.GetLength()),"1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second,PIN2);
		}
		
		if(ret<0)
		{
			debug("调用失败,返回值%d,参数为:%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ret,csport,"","1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second);
			return -14;//读密码失败
		}
		if(strcmp(PIN,PIN2)!=0)
		{
			return -10;//两次密码不一致
		}
	}
	
	
	return ret;

}
/********************************************
* 功  能：加载TMK到密码键盘。
* 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@MKeyIdx - 主密钥索引号
* 参  数：@lpOldMKey  - 主密钥串
* 参  数：@mode    - 主密钥串状态。0 - 明文状态 1 - 密文状态
* 参  数：@lpKeyCK - 密钥校验值，传入传出参数。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SFKBLoadMasterKey(HDLL hDrv, VPair &inList, int MKeyIdx, TPCHR lpOldMKey, TPCHR lpMKey, int mode, PCHR lpKeyCK, PCH ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
    if(!hDrv)
	{
		sprintf(ErrDesc, "%s驱动库未找到", thirddll.second);
		return -1;
	}
	
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	lpCMCC_LoadMasterKey CMCC_LoadMasterKey = (lpCMCC_LoadMasterKey)GetProcAddress(hDrv, "CMCC_LoadMasterKey");
	

	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
	
	try
	{		
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "打开串口失败!");
			return -3;
		}

		ret = CMCC_LoadMasterKey(hComm, MKeyIdx, lpOldMKey, lpKeyCK, timeout);
		if(ret != 0)
		{
			sprintf(ErrDesc, "灌注TMK失败!");
			CloseComm(hComm);
			return -4;
		}
		
		CloseComm(hComm);
	}
	catch (...)
	{
		sprintf(ErrDesc, "获取PinBlock模块发生异常!");
		return -10;
	}

	return 0;
}

/********************************************
* 功  能：加载TPK/TAK到密码键盘。
* 参  数：@inList - 采集所需数据。
	         数组下标解释：0 - 模块所在目录 1 - 适配器库名称 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@MKeyIdx - 主密钥索引号
* 参  数：@WKeyIdx - 工作密钥索引号
* 参  数：@lpMKey  - 工作密钥串
* 参  数：@lpKeyCK - 密钥校验值。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SFKBLoadWorkKey(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpWKey, TPCHR lpKeyCK, PCHR ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
    if(!hDrv)
	{
		sprintf(ErrDesc, "%s驱动库未找到", thirddll.second);
		return -1;
	}
	
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	lpCMCC_LoadKey CMCC_LoadKey = (lpCMCC_LoadKey)GetProcAddress(hDrv, "CMCC_LoadKey");
	
	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
	
	try
	{
		
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "打开串口失败!");
			return -3;
		}
		
		ret = CMCC_LoadKey(hComm, MKeyIdx, WKeyIdx, lpWKey, lpKeyCK, timeout);
		if(ret != 0)
		{
			if(ret == -6)
				sprintf(ErrDesc, "校验值不一致!需要灌注正确的主密钥。");
			else
				sprintf(ErrDesc, "灌注TPK/TAK失败!");
			CloseComm(hComm);
			return -4;
		}
		
		CloseComm(hComm);
	}
	catch (...)
	{
		sprintf(ErrDesc, "获取PinBlock模块发生异常!");
		return -10;
	}
	
	return 0;
}

int SFKBLoadWorkKey2(HDLL hDrv, VPair &inList, const char *MKeyIdx, const char *WKeyIdx, const char * KeyLen, const char * keyByTMK, const char * KeyValue, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
	//debug("incoming KPInputKEY!");
	PinPadWriteWorkKeyWithMainkey LoadWorkKey = (PinPadWriteWorkKeyWithMainkey)GetProcAddress(hDrv,"PinPadWriteWorkKeyWithMainkey");
	if (!LoadWorkKey)
	{
		sprintf(ErrDesc, "PinPadWriteWorkKeyWithMainkey未找到");
		return -19;
	}
	//debug("end of PinPadWriteWorkKeyWithMainkey!");
	//const char* port = "COM" + portno.second;
	CString port;
	port.Format("COM%s",portno.second);
	//debug("PinPadWriteWorkKeyWithMainkey入参:%s,%s,%s,%s,%s,%s",port,MKeyIdx,WKeyIdx,KeyLen,keyByTMK,KeyValue);
	char retKeyValue[16] = {0};
	int retCode = LoadWorkKey(port,"",MKeyIdx,WKeyIdx,const_cast<char*>(KeyLen),const_cast<char*>(keyByTMK),retKeyValue);
	//debug("retKeyValue:%s,retCode:%d",retKeyValue,retCode);
	if (retCode > 0)
	{
		char *temp = new char[retCode+1];
		strncpy(temp,KeyValue,retCode);
		if (CString(*temp) != CString(*retKeyValue))
		{
			sprintf(ErrDesc, "校验串不匹配");
			debug("传入校验串:%s,密码键盘返回:%s",KeyValue,retKeyValue);
			delete [] temp;
			return -15;//灌密钥失败
		}
		delete [] temp;
	}else{
		sprintf(ErrDesc, "无校验串返回");
		debug("调用[PinPadWriteWorkKeyWithMainkey]失败,返回值%d,参数为:%s,%s,%s,%s,%s,%s,%s",retCode,port,"",MKeyIdx,WKeyIdx,KeyLen,keyByTMK,KeyValue);
		return -15;//灌密钥失败
	}

	return 0;
}

/********************************************
* 功  能：生成MAC。
* 参  数：@inList - 采集所需数据。
	                数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@MKeyIdx - 主密钥索引号
* 参  数：@WKeyIdx - 工作密钥索引号
* 参  数：@lpData  - mac数据
* 参  数：@lpMAC  - 传出参数，MAC值。
* 参  数：@lpTAKKCK - 传出参数，TAK校验值。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SFKBGenerateMAC(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, PCHR lpMAC, PCHR lpTAKKCK, PCHR ErrDesc)
{
	return 0;
}

/********************************************
* 功  能：验证MAC。
* 参  数：@inList - 采集所需数据。
	                数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
* 参  数：@MKeyIdx - 主密钥索引号
* 参  数：@WKeyIdx - 工作密钥索引号
* 参  数：@lpData  - mac数据
* 参  数：@lpMAC  - MAC值。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SFKBVerifyMAC(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, TPCHR lpMAC, PCHR ErrDesc)
{
	return 0;
}

int KPGetPinPadID(HDLL hDrv, VPair &inList,PCHR result, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];

	_KPGetPinPadID GetPinPadID = (_KPGetPinPadID)GetProcAddress(hDrv,"PinPadGetID");
	if (!GetPinPadID)
	{
		sprintf(ErrDesc, "KPGetPinPadID未找到");
		return -19;
	}
	CString port;
	port.Format("COM%s",portno.second);
	int retCode = GetPinPadID(port,"",result);

	if(retCode < 0)
	{
		debug("调用[PinPadGetID]失败,返回值%d,参数为:%s,%s",retCode,port,"");
		return -7;
	}
		
	return retCode;

}

int PinPadReadEvaluate(HDLL hDrv, VPair &inList,PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];

	_PinPadReadEvaluate Evaluate = (_PinPadReadEvaluate)GetProcAddress(hDrv,"PinPadReadEvaluate");
	if (!Evaluate)
	{
		sprintf(ErrDesc, "PinPadReadEvaluate未找到");
		return -19;
	}
	CString port;
	port.Format("COM%s",portno.second);

	return Evaluate(port,"","1",tmptimeout);

}

int driveKeyBd(HDLL hDrv, VPair &inList, PCHR ErrDesc)
{
	debug("开始调用driveKeyBd");
	
	FUNC_PinPadGetCompanyID PinPadGetCompanyID = (FUNC_PinPadGetCompanyID)GetProcAddress(hDrv, "PinPadGetCompanyID");
	FUNC_PinPadEncBySm2_PK PinPadEncBySm2_PK = (FUNC_PinPadEncBySm2_PK)GetProcAddress(hDrv, "PinPadEncBySm2_PK");
	FUNC_PinPadWriteKeyWithSM4Ciper PinPadWriteKeyWithSM4Ciper = (FUNC_PinPadWriteKeyWithSM4Ciper)GetProcAddress(hDrv, "PinPadWriteKeyWithSM4Ciper");
	FUNC_PinPadWriteWorkKeyWithMainkey PinPadWriteWorkKeyWithMainkey = (FUNC_PinPadWriteWorkKeyWithMainkey)GetProcAddress(hDrv, "PinPadWriteWorkKeyWithMainkey");

	if (NULL == PinPadGetCompanyID || NULL == PinPadEncBySm2_PK || NULL == PinPadWriteKeyWithSM4Ciper || NULL == PinPadWriteWorkKeyWithMainkey)
	{
		sprintf(ErrDesc, "厂商函数未找到");
		return -19;
	}

	CString type = inList[0].second;
	PAIR porttype = inList[1];
	PAIR portno = inList[2];
	CString csport = (porttype.second=="1"?"COM":"USB") + portno.second;

	int optype = atoi(type.GetBuffer(type.GetLength()));
	int ret;

	CString tmpStr;

	if (3 == optype)
	{
		char SeqNo[21] = {0};
		memset(SeqNo, 0, sizeof(SeqNo));
		char factoryID[21] = {0};
		memset(factoryID, 0, sizeof(factoryID));

		ret = PinPadGetCompanyID(csport, "", SeqNo, factoryID);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "厂商函数PinPadGetCompanyID返回报错");
			debug("厂商函数PinPadGetCompanyID返回报错");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", SeqNo);
		PAIR seqNo("SeqNo", tmpStr);
		inList.push_back(seqNo);

		tmpStr.Format("%s", factoryID);
		PAIR factId("factoryID", tmpStr);
		inList.push_back(factId);

	}
	else if (4 == optype)
	{
		char ranKeyLen[4+1] = {0};
		char PK[256+1] = {0};
		strcpy(ranKeyLen, inList[3].second);
		strcpy(PK, inList[4].second);

		char ciper[256+1] = {0};
		memset(ciper, 0, sizeof(ciper));
		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadEncBySm2_PK(csport, "", ranKeyLen, PK, ciper, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "厂商函数PinPadEncBySm2_PK返回报错");
			debug("厂商函数PinPadEncBySm2_PK返回报错");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", ciper);
		PAIR Ciper("ciper", tmpStr);
		inList.push_back(Ciper);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
		
	}
	else if (5 == optype)
	{
		char keyIndex[4+1] = {0};
		char newKeyLen[4+1] = {0};
		char newKeyBySM4Key[256+1] = {0};
		strcpy(keyIndex, inList[3].second);
		strcpy(newKeyLen, inList[4].second);
		strcpy(newKeyBySM4Key, inList[5].second);

		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadWriteKeyWithSM4Ciper(csport, "", keyIndex, newKeyLen, newKeyBySM4Key, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "厂商函数PinPadWriteKeyWithSM4Ciper返回报错");
			debug("厂商函数PinPadWriteKeyWithSM4Ciper返回报错");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
	}
	else if (6 == optype)
	{
		char mainkeyIndex[4+1] = {0};
		char workkeyIndex[4+1] = {0};
		char newKeyLen[4+1] = {0};
		char newKeyBySM4Key[256+1] = {0};
		strcpy(mainkeyIndex, inList[3].second);
		strcpy(workkeyIndex, inList[4].second);
		strcpy(newKeyLen, inList[5].second);
		strcpy(newKeyBySM4Key, inList[6].second);

		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadWriteWorkKeyWithMainkey(csport, "", mainkeyIndex, workkeyIndex, newKeyLen, newKeyBySM4Key, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "厂商函数PinPadWriteWorkKeyWithMainkey返回报错");
			debug("厂商函数PinPadWriteWorkKeyWithMainkey返回报错");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
	}
	debug("调用driveKeyBd结束");
	return ret;
}


/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetSFKBCOM(KB_ADAPTER_INTERFACE *pInterf)
{
	if(pInterf)
	{
		pInterf->SFKBLoadDrv = SFKBLoadDrv;
		pInterf->SFKBFreeDrv = SFKBFreeDrv;
		pInterf->SFKBReadPin = SFKBReadPin;
		pInterf->SFKBReadPin2 = SFKBReadPin2;
		pInterf->SFKBLoadMasterKey = SFKBLoadMasterKey;
		pInterf->SFKBLoadWorkKey = SFKBLoadWorkKey;
		pInterf->SFKBLoadWorkKey2 = SFKBLoadWorkKey2;
		pInterf->SFKBGenerateMAC = SFKBGenerateMAC;
		pInterf->SFKBVerifyMAC = SFKBVerifyMAC;
		pInterf->KPGetPinPadID = KPGetPinPadID;
		pInterf->PinPadReadEvaluate = PinPadReadEvaluate;
		pInterf->driveKeyBd = driveKeyBd;
		return TRUE;
	}
    return FALSE;
}


void debug(CString s)
{
#ifdef _INFO
	
	
	FILE* m_stream = fopen("err.log", "a+");
	if (m_stream == NULL)
	{
		ASSERT(1==0);
	}
	
	time_t t;
	time(&t);
	tm *ptm = localtime(&t);
	fprintf(m_stream,"[%04d-%02d-%02d %02d:%02d:%02d]%s\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,s);
	
	fclose(m_stream);
	

#endif
}

void debug(const char* s,...)
{
#ifdef _INFO

	
	FILE* m_stream = fopen("err.log", "a+");
	if (m_stream == NULL)
	{
		ASSERT(1==0);
	}
	char msg[1024];
	va_list ap;
	va_start(ap,s);
	_vsnprintf(msg,1024,s,ap);
	va_end(ap);
	
	time_t t;
	time(&t);
	tm *ptm = localtime(&t);
	fprintf(m_stream,"[%04d-%02d-%02d %02d:%02d:%02d]%s\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,msg);

	fclose(m_stream);
	

#endif
}