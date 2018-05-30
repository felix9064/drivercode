// dllGMT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"

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
// CDllGMTApp

BEGIN_MESSAGE_MAP(CDllGMTApp, CWinApp)
	//{{AFX_MSG_MAP(CDllGMTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllGMTApp construction

CDllGMTApp::CDllGMTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllGMTApp object

CDllGMTApp theApp;


int MyCallback(char* message)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	
	CString &dir = pInst->GetModuleDir(); //用引用使dir指向_moduledir成员变量
	VPair &sub = pInst->GetRetListRef();
	CString logStr;
	logStr.Format("回调函数调用开始，sub.size()=%d, 参数message=%s", sub.size(), message);
	CGmtDriver::runLog(logStr, dir);

	// 由于此处的message是utf-8编码，故需要转换两次，第一次转换时的参数必须是CP_UTF8
	// 获取转换为宽字节后需要的缓冲区大小，创建宽字节缓冲区
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, message, -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen+1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, message, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen+1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	
	logStr.Format("处理后的参数message=%s", szBuffer);
	CGmtDriver::runLog(logStr, dir);

	pInst->readJson(szBuffer, sub);
	logStr.Format("回调函数调用结束，sub.size()=%d，result=%s", sub.size(), sub[sub.size()-1].second);
	CGmtDriver::runLog(logStr, dir);
	
	delete []wszBuffer;
	delete []szBuffer;

	return 0;
}

PTCHAR WINAPI GetGmtErrmsg(int iRet)
{
	static char errmsg[100]={0};
	switch(iRet)
	{
	case 0:
		sprintf(errmsg,"设备无应答");
		break;
	case -2:
		sprintf(errmsg,"设备未连接");
		break;
	case -3:
		sprintf(errmsg,"参数错误");
		break;
	}

	return errmsg;
}


/********************************************
* 功  能：打开设备，OpenDevice/CloseDevice需成对调用
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	if (!pInst)
	{
		return -1;
	}
	
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);
	
	CString csAppPath ;
	csAppPath.Format("%s\\gmtdll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	return 0;
}

/********************************************
* 功  能：关闭设备
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI CloseDevice()
{

	CGmtDriver *pInst = CGmtDriver::GetInstance();
	if (!pInst)
	{
		return -1;
	}

	return 0;
}

/********************************************
* 功  能：复位
* 参  数：@rw - 读写类型。1-读，2-写
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Reset(int rw)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	if (!pInst)
	{
		return -1;
	}
    
	return 0;
}


/********************************************
* 功  能：获取设备驱动描述
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("操作柜外清");
}

/********************************************
* 功  能：获取设备类型
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI GetDeviceType()
{
	return GMT_DEV;
}

VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString logstr;//日志格式字符串
	
	int type = atoi(indata[0]);
	
	int port = atoi(portno);
	CString _partno;
	if (port == 0)
	{
		_partno.Format("USB0");
	}
	else
	{
		_partno.Format("COM%d", port);
	}

	CString strRetCode;
	CGmtDriver *pInst = CGmtDriver::GetInstance();

	CString &dir = pInst->GetModuleDir(); //用引用使dir指向_moduledir成员变量
	VPair &sub = pInst->GetRetListRef();
    sub.clear();

	// 如果没有传配置文件，则用默认的南天配置
	CString confpath;
	if (conffile == 0)
	{
		confpath.Format("%sNanTianGmt.xml", dir);
	}
	else
	{
		confpath.Format("%s%s", dir, conffile);
	}
	
	logstr.Format("开始调用，配置文件：%s", confpath);
	CGmtDriver::runLog(logstr, dir);

	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(_partno);
    
	(*ret) = pInst->OpenICd();
	if((*ret) != SUCCESS)
	{
		CGmtDriver::runLog("获取xml配置文件参数失败!", dir);
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //打开串并口失败
	}

	int result = pInst->LoadDriver(type);
	if(result != SUCCESS)
	{
		CGmtDriver::runLog("载入驱动LoadDriver失败! \r\n",dir);
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	if (!pInst->IsDriverLoaded())
	{	
		CGmtDriver::runLog("载入驱动IsDriverLoaded失败! \r\n",dir);
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	char *args = (char *)malloc(1024 + 1);
	char *resmsg = (char *)malloc(4*1024*1024 + 1);
	char *errmsg = (char *)malloc(256 + 1);
	memset(resmsg, 0x00, sizeof(resmsg));
	memset(errmsg, 0x00, sizeof(errmsg));
	memset(args, 0x00, sizeof(args));

	int timeout = pInst->GetTimeOut();
	
	logstr.Format("传入参数portno=%s, type=%d, _partno=%s", portno, type, _partno);
	CGmtDriver::runLog(logstr, dir);

	// 初始化
	result = pInst->initDriver(&MyCallback);
	if (result != SUCCESS)
	{	
		CGmtDriver::runLog("调用厂商驱动初始化函数initDriver失败! \r\n", dir);
		pInst->RealseDriver();
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	// 设置通讯参数
	CString tmp("USB0");
	if (_partno.Compare(tmp) == 0)
	{
		_partno.Format("USB");
	}
	pInst->setPortAttr(_partno.GetBuffer(_partno.GetLength()));
	
	VPair outlist;
	outlist.clear();
	CString str;

	if (1 == type)
	{
		logstr.Format("重置主密钥:call ResetZmk()...");
		CGmtDriver::runLog(logstr, dir);
		result = pInst->ResetZmk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (8 == type)
	{
		logstr.Format("获取设备序列号:call ReadDeviceInfo()...");
		CGmtDriver::runLog(logstr, dir);
		result = pInst->ReadDeviceInfo(args, resmsg, errmsg, timeout, timeout);
	}
	else if (11 == type)
	{
		logstr.Format("载入主密钥:call LoadZmk()...");
		CGmtDriver::runLog(logstr, dir);
		
		logstr.Format("入参:mkNew=%s", indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// 主密钥
		PAIR zmk("Zmk", indata[5]);
		outlist.push_back(zmk);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("生成的json数据：%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->LoadZmk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (12 == type)
	{
		logstr.Format("载入工作密钥:call LoadZpk()...");
		CGmtDriver::runLog(logstr, dir);

		logstr.Format("入参:wk=%s", indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// 工作密钥
		PAIR zpk("Zpk", indata[4]);
		outlist.push_back(zpk);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("生成的json数据：%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->LoadZpk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (17 == type)
	{
		logstr.Format("输入密码:call ReadCipher()...");
		CGmtDriver::runLog(logstr, dir);

		// 密码位数
		PAIR PinLen("PinLen", indata[2]);
		outlist.push_back(PinLen);

		// 超时时间
		timeout = atoi(indata[3]);

		// 播放的语音
		PAIR Speech("Speech", indata[4]);
		outlist.push_back(Speech);

		// 账号
		PAIR AccNo("AccNo", indata[5]);
		outlist.push_back(AccNo);

		// Html
		PAIR Html("Html", "");
		outlist.push_back(Html);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("生成的json数据：%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->ReadCipher(args, resmsg, errmsg, timeout, timeout);

		// 成功时，将密码的长度拼在密码密文的前面，长度本身占2位
		if (0 == result)
		{
			int pwdlen = atoi(indata[2]);
			CString lenStr;
			if (pwdlen < 10)
			{
				lenStr.Format("0%d", pwdlen);
			}
			else
			{
				lenStr.Format("%d", pwdlen);
			}

			CString passwd;
			passwd.Format("%s%s", lenStr, resmsg);

			logstr.Format("加上长度标识后的密码：%s", passwd);
			CGmtDriver::runLog(logstr, dir);

			memset(resmsg, 0x00, sizeof(resmsg) + 4);
			strcpy(resmsg, passwd.GetBuffer(passwd.GetLength()));
		}

	}
	else if (26 == type)
	{
		logstr.Format("显示并确认信息:call DisplayHtml()...");
		CGmtDriver::runLog(logstr, dir);

		VPair btnlist;
		btnlist.clear();

		// 按钮个数
		PAIR Num("Num", "2");
		btnlist.push_back(Num);

		// 按钮1
		PAIR Button1("Button1", "确认");
		btnlist.push_back(Button1);

		// 按钮2
		PAIR Button2("Button2", "取消");
		btnlist.push_back(Button2);

		pInst->writeJson(btnlist);
		CString btnjson = btnlist[0].second;
		btnjson.Replace(" ", "");
		btnjson.Replace("\n", "");

		// Html
		PAIR Html("Html", indata[1]);
		outlist.push_back(Html);

		// 提示语音
		PAIR Speech("Speech", indata[2]);
		outlist.push_back(Speech);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		// 按钮json
		PAIR Button("Button", btnjson);
		outlist.push_back(Button);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("生成的json数据：%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);

		// 超时时间
		timeout = atoi(indata[3]);
		result = pInst->DisplayHtml(args, resmsg, errmsg, timeout, timeout);
	}
	else if (27 == type)
	{
		logstr.Format("显示信息:call DisplayHtml()...");
		CGmtDriver::runLog(logstr, dir);

		VPair btnlist;
		btnlist.clear();

		// 按钮个数
		PAIR Num("Num", "0");
		btnlist.push_back(Num);

		pInst->writeJson(btnlist);
		CString btnjson = btnlist[0].second;
		btnjson.Replace(" ", "");
		btnjson.Replace("\n", "");

		// Html
		PAIR Html("Html", indata[1]);
		outlist.push_back(Html);

		// 提示语音
		PAIR Speech("Speech", indata[2]);
		outlist.push_back(Speech);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		// 按钮json
		PAIR Button("Button", btnjson);
		outlist.push_back(Button);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("生成的json数据：%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);

		// 超时时间
		timeout = atoi(indata[3]);
		result = pInst->DisplayHtml(args, resmsg, errmsg, timeout, timeout);
	}
	else
	{
		strcpy(resmsg, "成功");
		result = 0;
	}

	logstr.Format("调用厂商函数返回信息:iret=%d, resmsg=%s, errmsg=%s", result, resmsg, errmsg);
	CGmtDriver::runLog(logstr, dir);

	CGmtDriver::runLog("调用厂商驱动释放资源函数deinit()...", dir);
	pInst->deinit();

	sub = pInst->GetRetListRef();
	// 将厂商返回码转为柜面对应的返回码
	if (result == 0)
	{
		str.Format("%d", 1);
	}
	else if (result == 1)
	{
		str.Format("%d", 0);
	}
	else
	{
		str.Format("%d", result);
	}
	PAIR returncode("returncode", str);
	if (sub.size() == 0)
	{
		sub.push_back(returncode);
	}
	else
	{
		sub.insert(sub.begin(), returncode);
	}
	
	if(26 != type)
	{
		str.Format("%s", resmsg);
		PAIR res_msg("res_msg", str);
		sub.push_back(res_msg);
		
		if (result != 0)
		{
			str.Format("%s", errmsg);
			PAIR err_msg("err_msg", str);
			sub.push_back(err_msg);
		}
	}

	free(args);
	free(resmsg);
	free(errmsg);

	(*size) = sub.size();

	pInst->RealseDriver();
	CGmtDriver::runLog("调用结束... \r\n", dir);
	return sub;

	//恢复出厂主密钥
	/*
	if(1==type) 
	{
		runLog("恢复出厂主密钥:call SMInitMainKey()... \r\n",dir);
		
		char bpPort={0};

		
		logstr.Format("入参:nPort=%d\r\n",port);
		runLog(logstr,dir);
		
		int iRet=pInst->SMInitMainKey(port,bpPort);

		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			sub.push_back(returnmsg);
			
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;
		
	}
	//明文设置主密钥
	if(2==type) 
	{
		logstr.Format("明文设置主密钥:call SMSetMainKeyClear()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		char mk1[32]={0};
		char mk2[32]={0};
		char bpPort={0};
		
		strcpy(mk1,indata[2]);
		strcpy(mk2,indata[3]);
		logstr.Format("入参:nPort=%d,mkNo=%d,mk1=%s,mk2=%s\r\n",nPort,mkNo,mk1,mk2);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetMainKeyClear(nPort,bpPort,mkNo,mk1,mk2);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//密文设置主密钥
	if(3==type) 
	{
	
		logstr.Format("密文设置主密钥:call SMSetMainKeyEncrypt()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		char mkEncrypt[32]={0};
		char bpPort={0};
	
		strcpy(mkEncrypt,indata[2]);
		
		logstr.Format("入参:nPort=%d,mkNo=%d,mkEncrypt=%s\r\n",nPort,mkNo,mkEncrypt);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetMainKeyEncrypt(nPort,bpPort,mkNo,mkEncrypt);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//设置工作密钥
	if(4==type) 
	{
	
		logstr.Format("设置工作密钥:call SMSetWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char wk[32]={0};
		char bpPort={0};
		
		strcpy(wk,indata[3]);
		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d,wk=%s\r\n",nPort,mkNo,wkNo,wk);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetWorkKey(nPort,bpPort,mkNo,wkNo,wk);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);
		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//激活工作密钥
	if(5==type) 
	{
	
		logstr.Format("激活工作密钥:call SMActivateWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char bpPort={0};

		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d\r\n",nPort,mkNo,wkNo);
		runLog(logstr,dir);
		
		int iRet=pInst->SMActivateWorkKey(nPort,bpPort,mkNo,wkNo);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//工作密钥加密字符串
	if(6==type) 
	{
	
		logstr.Format("工作密钥加密字符串:call SMEncryptString()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char data[32]={0};
		char result[160]={0};
		char bpPort={0};
		strcpy(data,indata[3]);

		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d,data=%s\r\n",nPort,mkNo,wkNo,data);
		runLog(logstr,dir);
		
		int iRet=pInst->SMEncryptString(nPort,bpPort,mkNo,wkNo,data,result);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//输入密码
	if(7==type) 
	{
	
		logstr.Format("输入密码:call SMInputPin()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char inputCount[30]={0};
		int pinLen=atoi(indata[2]);
		int timeout=atoi(indata[3]);

		strcpy(inputCount,indata[1]);

		char pwd[33]={0};
		char bpPort={0};

		logstr.Format("入参:inputCount=%s,pinLen=%d,timeout=%d,nPort=%d,\r\n",inputCount,pinLen,timeout,nPort);
		runLog(logstr,dir);
		
		int iRet=pInst->SMInputPin(nPort,bpPort,inputCount,pinLen,timeout,pwd);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("pwd",pwd);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:pwd=%s\r\n",pwd);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//获取设备序列号	
	if(8==type) 
	{
	
		logstr.Format("获取设备序列号:call GetDevID()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int len=atoi(indata[1]);
		char devID[100];
		char bpPort={0};

		logstr.Format("入参:nPort=%d,len=%d\r\n",nPort,len);
		runLog(logstr,dir);
		
		int iRet=pInst->GetDevID(nPort,bpPort,devID,len);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("devID",devID);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:devID=%s\r\n",devID);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//设置设备序列号	
	if(9==type) 
	{
	
		logstr.Format("设置设备序列号:call SetDevID()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char devID[100];
		char bpPort={0};

		strcpy(devID,indata[1]);

		logstr.Format("入参:devID=%s,nPort=%d\r\n",devID,nPort);
		runLog(logstr,dir);
		
		int iRet=pInst->SetDevID(nPort,bpPort,devID);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//初始化主密钥	
	if(10==type) 
	{
	
		logstr.Format("初始化主密钥:call InitFactoryKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		logstr.Format("入参:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->InitFactoryKey(nPort,bpPort);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//设置主密钥	
	if(11==type) 
	{
	
		logstr.Format("设置主密钥:call SetMainKey()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int mkLenOld=atoi(indata[2]);
		char mkOld[48]={0};
		int mkLenNew=atoi(indata[4]);
		char mkNew[48]={0};

		strcpy(mkOld,indata[3]);
		strcpy(mkNew,indata[5]);

		logstr.Format("入参:nPort=%d,mkNo=%d,mkLenOld=%d,mkOld=%s,mkLenNew=%d,mkNew=%s\r\n",nPort,mkNo,mkLenOld,mkOld,mkLenNew,mkNew);
		runLog(logstr,dir);

		int iRet=pInst->SetMainKey(nPort,bpPort,mkNo,mkLenOld,mkOld,mkLenNew,mkNew);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//设置工作密钥	
	if(12==type) 
	{
	
		logstr.Format("设置工作密钥:call SetWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		int wkLen=atoi(indata[3]);
		char wk[48]={0};

		strcpy(wk,indata[4]);

		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d,wkLen=%d,wk=%s\r\n",nPort,mkNo,wkNo,wkLen,wk);
		runLog(logstr,dir);

		int iRet=pInst->SetWorkKey(nPort,bpPort,mkNo,wkNo,wkLen,wk);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//选择工作密钥	
	if(13==type) 
	{
	
		logstr.Format("选择工作密钥:call ActivateWorkKey()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);

		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d\r\n",nPort,mkNo,wkNo);
		runLog(logstr,dir);

		int iRet=pInst->ActivateWorkKey(nPort,bpPort,mkNo,wkNo);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//加密字符串	
	if(14==type) 
	{
	
		logstr.Format("加密字符串:call EncryptString()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char dataIn[100]={0};
		char dataOut[100]={0};

		strcpy(dataIn,indata[3]);

		logstr.Format("入参:nPort=%d,mkNo=%d,wkNo=%d,dataIn=%s\r\n",nPort,mkNo,wkNo,dataIn);
		runLog(logstr,dir);

		int iRet=pInst->EncryptString(nPort,bpPort,mkNo,wkNo,dataIn,dataOut);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("dataOut",dataOut);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:dataOut=%s\r\n",dataOut);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//明文输入密码	
	if(15==type) 
	{
	
		logstr.Format("明文输入密码:call GetClearTextPin()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);

		logstr.Format("入参:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice);
		runLog(logstr,dir);

		int iRet=pInst->GetClearTextPin(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,result);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//3DES密码输入	
	if(16==type) 
	{
	
		logstr.Format("3DES密码输入:call GetCipherTextPin()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);

		logstr.Format("入参:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice);
		runLog(logstr,dir);

		int iRet=pInst->GetCipherTextPin(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,result);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//PINBLOCK密码输入	
	if(17==type) 
	{
	
		logstr.Format("PINBLOCK密码输入:call GetCipherTextPin98()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char accNo[20]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);
		strcpy(accNo,indata[5]);

		logstr.Format("入参:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s,accNo=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice,accNo);
		runLog(logstr,dir);

		int iRet=pInst->GetCipherTextPin98(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,accNo,result);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("返回信息:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//设置柜员信息	
	if(18==type) 
	{
	
		logstr.Format("设置柜员信息:call SetTellerInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		char name[30]={0};
		char workId[30]={0};
		int grade=atoi(indata[3]);
		char other[30]={0};


		strcpy(name,indata[1]);
		strcpy(workId,indata[2]);
		strcpy(other,indata[4]);

		logstr.Format("入参:name=%s,workId=%s,grade=%d,other=%s,nPort=%d\r\n",name,workId,grade,other,nPort);
		runLog(logstr,dir);

		int iRet=pInst->SetTellerInfo(nPort,bpPort,name,workId,grade,other);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//获取柜员信息	
	if(19==type) 
	{
	
		logstr.Format("获取柜员信息:call GetTellerInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		char name[30]={0};
		char workId[30]={0};
		int grade=0;
		char other[30]={0};

		int iRet=pInst->GetTellerInfo(nPort,bpPort,name,workId,&grade,other);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR pname("name",name);
			sub.push_back(pname);
			PAIR pworkId("workId",workId);
			sub.push_back(pworkId);
			str.Format("%d",grade);
			PAIR pgrade("grade",str);
			sub.push_back(pgrade);
			PAIR pother("other",other);
			sub.push_back(pother);
			logstr.Format("返回信息:name=%s,workId=%s,grade=%d,other=%s\r\n",name,workId,grade,other);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//开始计时	
	if(20==type) 
	{
	
		logstr.Format("开始计时:call BeginWorkTick()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		int iRet=pInst->BeginWorkTick(nPort,bpPort);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//结束计时	
	if(21==type) 
	{
	
		logstr.Format("结束计时:call EndWorkTick()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		DWORD tick=atol(indata[1]);
		char bpPort={0};

		int iRet=pInst->EndWorkTick(nPort,bpPort,&tick);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//下载反假币证书
	if(22==type) 
	{
	
		logstr.Format("下载反假币证书:call DownloadLicense()... \r\n");
		runLog(logstr,dir);
		
		char path[100]={0};

		strcpy(path,indata[1]);
		logstr.Format("入参:path=%s",path);
		runLog(logstr,dir);

		int iRet=pInst->DownloadLicense(path);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//显示柜员信息
	if(23==type) 
	{
	
		logstr.Format("显示柜员信息:call ShowTellerInfo()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int timeout=atoi(indata[1]);
		char bpPort=0;

		logstr.Format("入参:nPort=%d,timeout=%d\r\n",nPort,timeout);
		runLog(logstr,dir);

		int iRet=pInst->ShowTellerInfo(nPort,bpPort,timeout);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}

	//启动评价
	if(24==type) 
	{
	
		logstr.Format("启动评价:call InputEvaluation()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		int timeout=atoi(indata[1]);
		char seviceType[100]={0};
		strcpy(seviceType,indata[2]);


		logstr.Format("入参:seviceType=%s,timeout=%d,nPort=%d\r\n",seviceType,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->InputEvaluation(nPort,bpPort,seviceType,timeout);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//下载文件
	if(25==type) 
	{
	
		logstr.Format("下载文件:call SaveFile()... \r\n");
		runLog(logstr,dir);
		
		int type=atoi(indata[1]);
		char path[100]={0};
		strcpy(path,indata[2]);

		logstr.Format("入参:path=%s,type=%d\r\n",path,type);
		runLog(logstr,dir);

		int iRet=pInst->SaveFile(type,path);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//把信息显示在屏幕上让客户确认信息
	if(26==type) 
	{
	
		logstr.Format("把信息显示在屏幕上让客户确认信息:call ConfirmInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		char pHtml[2048]={0};
		char pVoice[100]={0};
		char pResult[20]={0};

		strcpy(pHtml,indata[1]);
		strcpy(pVoice,indata[2]);
		int timeout=atoi(indata[3]);
		

		logstr.Format("入参:pHtml=%s,pVoice=%s,timeout=%d,nPort=%d\r\n",pHtml,pVoice,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->ConfirmInfo(nPort,bpPort,pHtml,pVoice,timeout,sizeof(pResult),pResult);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR pResult("pResult",pResult);
			sub.push_back(pResult);
			logstr.Format("返回信息:pResult=%s\r\n",pResult);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//把信息显示在屏幕上让客户查看信息
	if(27==type) 
	{
	
		logstr.Format("把信息显示在屏幕上让客户查看信息:call ShowInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		char pHtml[2048]={0};
		char pVoice[100]={0};

		strcpy(pHtml,indata[1]);
		strcpy(pVoice,indata[2]);
		int timeout=atoi(indata[3]);
		

		logstr.Format("入参:pHtml=%s,pVoice=%s,timeout=%d,nPort=%d\r\n",pHtml,pVoice,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->ShowInfo(nPort,bpPort,pHtml,pVoice,timeout);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//撤销柜员操作，使设备播放广告，可撤销信息交互，密码输入，评价等
	if(28==type) 
	{
	
		logstr.Format("撤销:call CancelCustomProcess()... \r\n");
		runLog(logstr,dir);
		

		int nPort=port;
		char bpPort=0;

		logstr.Format("入参:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->CancelCustomProcess(nPort,bpPort);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//3DES设置明文主密钥
	if(29==type) 
	{
	
		logstr.Format("3DES设置明文主密钥:call SetMainKeyClear()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;

		int mkNo=atoi(indata[1]);
		int mkLen1=atoi(indata[2]);
		char mk1[64]={0};
		int mkLen2=atoi(indata[4]);
		char mk2[64]={0};

		strcpy(mk1,indata[3]);
		strcpy(mk2,indata[5]);

		logstr.Format("入参:nPort=%d,mkNo=%d,mkLen1=%d,mk1=%s,mkLen2=%d,mk2=%s \r\n",nPort,mkNo,mkLen1,mk1,mkLen2,mk2);
		runLog(logstr,dir);

		boolean iRet=pInst->SetMainKeyClear(nPort,bpPort,mkNo,mkLen1,mk1,mkLen2,mk2);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//设置播放广告类型
	if(30==type) 
	{
	
		logstr.Format("设置播放广告类型:call SetPosterType()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		int type=atoi(indata[1]);

		logstr.Format("入参:nPort=%d,type=%d \r\n",nPort,type);
		runLog(logstr,dir);

		int iRet=pInst->SetPosterType(type);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//删除广告图片
	if(31==type) 
	{
	
		logstr.Format("删除广告图片:call DeletePic()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char picName[100]={0};

		strcpy(picName,indata[1]);

		logstr.Format("入参:nPort=%d,picName=%s \r\n",nPort,picName);
		runLog(logstr,dir);

		int iRet=pInst->DeletePic(picName);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//删除所有广告图片
	if(32==type) 
	{
	
		logstr.Format("删除所有广告图片:call DeleteAllPic()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		logstr.Format("入参:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->DeleteAllPic();
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//获取广告视频列表
	if(33==type) 
	{
	
		logstr.Format("获取广告视频列表:call GetVideoList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		char pVideoList[200]={0};

		logstr.Format("入参:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetVideoList(sizeof(pVideoList),pVideoList);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString vlist(pVideoList);
			vlist.Replace("&","#");
			PAIR pVideo_List("pVideoList",vlist);
			sub.push_back(pVideo_List);
			logstr.Format("获取广告视频列表:%s\r\n",pVideoList);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//删除广告视频
	if(34==type) 
	{
	
		logstr.Format("删除广告视频:call DeleteVideo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char pVideoName[100]={0};

		strcpy(pVideoName,indata[1]);

		logstr.Format("入参:nPort=%d,pVideoName=%s \r\n",nPort,pVideoName);
		runLog(logstr,dir);

		int iRet=pInst->DeleteVideo(pVideoName);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//删除所有广告视频
	if(35==type) 
	{
	
		logstr.Format("删除所有广告视频:call DeleteAllVideo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		logstr.Format("入参:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->DeleteAllVideo();
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//获取广告图片列表
	if(36==type) 
	{
	
		logstr.Format("获取广告图片列表:call GetPicList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char picList[200]={0};

		logstr.Format("入参:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetPicList(nPort,bpPort,sizeof(picList),picList);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString plist(picList);
			plist.Replace("&","#");
			PAIR pic_list("picList",plist);
			sub.push_back(pic_list);

			logstr.Format("获取广告图片列表:%s\r\n",picList);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//获取播放列表
	if(37==type) 
	{
	
		logstr.Format("获取播放列表:call GetPlayList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char playList[200]={0};

		logstr.Format("入参:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetPlayList(nPort,bpPort,playList,sizeof(playList));
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString plist(playList);
			plist.Replace("&","#");
			PAIR play_List("playList",plist);
			sub.push_back(play_List);
			logstr.Format("获取播放列表:%s\r\n",playList);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//设置播放列表
	if(38==type) 
	{
	
		logstr.Format("设置播放列表:call SetPlayList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char playList[200]={0};
		if(indata[1]!=NULL){
			strcpy(playList,indata[1]);
		}

		logstr.Format("入参:nPort=%d,playList=%s \r\n",nPort,playList);
		runLog(logstr,dir);

		int iRet=pInst->SetPlayList(nPort,bpPort,playList);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//获取文件列表
	if(39==type) 
	{
	
		logstr.Format("获取文件列表:call GetFileList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char buf[200]={0};

		type=atoi(indata[1]);

		logstr.Format("入参:nPort=%d,type=%d \r\n",nPort,type);
		runLog(logstr,dir);

		int iRet=pInst->GetFileList(nPort,bpPort,type,sizeof(buf),buf);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			CString replacestr(buf);
			replacestr.Replace("&","#");
			PAIR p_buf("filelist",replacestr);
			sub.push_back(p_buf);
			logstr.Format("获取文件列表:%s\r\n",buf);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//删除文件
	if(40==type) 
	{
	
		logstr.Format("删除文件:call DeleteFileList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char fileName[100]={0};

		type=atoi(indata[1]);
		strcpy(fileName,indata[2]);

		logstr.Format("入参:nPort=%d,fileName=%s \r\n",nPort,fileName);
		runLog(logstr,dir);

		int iRet=pInst->DeleteFileList(nPort,bpPort,type,fileName);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//暂停服务
	if(41==type) 
	{
	
		logstr.Format("暂停服务:call ShowService()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char info[100]={0};
		bool show=1;

		strcpy(info,indata[1]);
		if(!strcmp("0",indata[2]))
		{
			show=0;
		}
		else
		{
			show=1;
		}

		logstr.Format("入参:nPort=%d,info=%s,show=%d \r\n",nPort,info,show);
		runLog(logstr,dir);

		int iRet=pInst->ShowService(nPort,bpPort,info,show);
		logstr.Format("返回码:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("返回错误信息:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	
	pInst->RealseDriver();
	return sub;
	*/
}

int WINAPI SetTimeOut(int timeout)
{
   
	return 0;
}

int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	if (!pInst)
	{
		return -1;
	}

	return SUCCESS;
}


/********************************************
* 功  能：取得当前错误详细描述
* 参  数：无
* 返回值：返回当前错误详细描述，无错误返回NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{

	static char errmess[BUFSIZ];	
	return errmess;
}

/********************************************
* 功  能：版本兼容控制，返回插件接口版本号，判断本插件当前版本是否可用
* 参  数：pFirst主版本号，pSecond次版本号，pFirst不相同则必不兼容，当pFirst相同，pSecond比容器大则不兼容，反之兼容
* 返回值：成功返回0,否则非0
*********************************************/
long WINAPI GetPracticably(long *pFirst,long *pSecond)
{
	return 0;
}


/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetDriverInterface(DRIV_INTERFACE *pDrivInterf)
{
	if(!pDrivInterf)
	{
		return FALSE;
	}
	pDrivInterf->OpenDevice = OpenDevice;
	pDrivInterf->CloseDevice = CloseDevice;
	pDrivInterf->GetDeviceDesc = GetDeviceDesc;
	pDrivInterf->GetDeviceType = GetDeviceType;
	pDrivInterf->GetErrorDesc = GetErrorDesc;
	pDrivInterf->GetPracticably = GetPracticably;
	pDrivInterf->Read = Read;
	pDrivInterf->Write = Write;
	pDrivInterf->Reset = Reset;
	//pDrivInterf->SetTimeOut = SetTimeOut;
	
	return TRUE;
}


