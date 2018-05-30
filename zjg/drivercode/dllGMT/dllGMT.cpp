// dllGMT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"
#include "gmtcreator.h"

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
	CGmtDriver *pInst = IGMTCreator::getIntance();
	CString tmpstr;
	if (NULL == pInst)
	{
		IGMTCreator::CoCreatIntance();
	    pInst = IGMTCreator::getIntance();
	}
	
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);
	
	CString csAppPath ;
	csAppPath.Format("%s\\keyboarddll\\", modulepath);
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
	CGmtDriver *pInst = IGMTCreator::getIntance();
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
	CGmtDriver *pInst = IGMTCreator::getIntance();
	if (!pInst)
	{
		pInst->RealseDriver();
		IGMTCreator::ReleaseIntance();
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

/********************************************
* 功  能：操作柜外清
* 参  数：
* 返回值：成功返回0，否则非0
*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString logstr;//日志格式字符串
	CString strRetCode;
	
	CGmtDriver *pInst = IGMTCreator::getIntance();
	VPair &sub = pInst->GetRetListRef();
	sub.clear();
	VPair outlist;

	if(conffile == 0)
	{
		(*ret) = ERR_NOFINDCONF;
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;
	}
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    (*ret) = pInst->OpenGmtDriver();
	if((*ret) != SUCCESS)
	{
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //打开串并口失败
	}
	int result;
	result = pInst->LoadDriver();

	if(result != SUCCESS)
	{
		logstr.Format("加载驱动失败!ret=%d", result);
		CGmtDriver::runLog(logstr, dir);
		pair< CString, CString > retcode("returncode", "1");
		sub.push_back(retcode);
		return sub;
	}
	if (!pInst->IsDriverLoaded())
	{	
		CGmtDriver::runLog("驱动未加载!", dir);
		pair< CString, CString > retcode("returncode", "1");
		sub.push_back(retcode);
		return sub;
	}

	int type = atoi(indata[0]);
	if (1 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_ClosePinPad--取消输入状态(关闭密码键盘)", dir);
		outlist.clear();
	}
	else if (2 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_SetCodeLen--设置密码长度", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iPinLen=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);
		// 密码长度
		PAIR iPinLen("iPinLen", indata[1]);
		outlist.push_back(iPinLen);
	}
	else if (3 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_NonEpt_ReadPin--读取非加密密码", dir);
		outlist.clear();
		
		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iVoiceType=%s，iTimeOut=%s，iPinSize=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);
		
		// 声音提示类型
		PAIR iVoiceType("iVoiceType", indata[1]);
		outlist.push_back(iVoiceType);
		// 超时时间
		PAIR timeout("iTimeOut", indata[2]);
		outlist.push_back(timeout);
		//缓冲区大小
		PAIR iPinSize("iPinSize", indata[3]);
		outlist.push_back(iPinSize);
	}
	else if (4 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_InitPinPad--初始化密码键盘", dir);
		outlist.clear();
	}
	else if (5 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_UpdateMKey--更新主密钥", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--ZmkIndex=%s, ZmkLength=%s, Zmk=%s, CheckValue=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// 密钥索引号
		PAIR ZmkIndex("ZmkIndex", indata[1]);
		outlist.push_back(ZmkIndex);
		// 密钥长度
		PAIR ZmkLength("ZmkLength", indata[2]);
		outlist.push_back(ZmkLength);
		// 密钥密文
		PAIR Zmk("Zmk", indata[3]);
		outlist.push_back(Zmk);
		// 密钥检验值
		PAIR CheckValue("CheckValue", indata[4]);
		outlist.push_back(CheckValue);
	}
	else if (6 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_DownLoadWKey--下载工作密钥", dir);
		outlist.clear();

		if ((*size) < 6)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--MKeyIndex=%s, WKeyIndex=%s, WKeyLength=%s, Key=%s, CheckValue=%s", indata[1], indata[2], indata[3], indata[4], indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// 主密钥索引号
		PAIR MKeyIndex("MKeyIndex", indata[1]);
		outlist.push_back(MKeyIndex);
		// 工作密钥索引号
		PAIR WKeyIndex("WKeyIndex", indata[2]);
		outlist.push_back(WKeyIndex);
		// 工作密钥长度
		PAIR WKeyLength("WKeyLength", indata[3]);
		outlist.push_back(WKeyLength);
		// 密钥密文
		PAIR Key("Key", indata[4]);
		outlist.push_back(Key);
		// 密钥检验值
		PAIR CheckValue("CheckValue", indata[5]);
		outlist.push_back(CheckValue);
	}
	else if (7 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_ActiveWKey--激活工作密钥", dir);
		outlist.clear();

		if ((*size) < 3)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--MKeyIndex=%s, WKeyIndex=%s", indata[1], indata[2]);
		CGmtDriver::runLog(logstr, dir);

		// 主密钥索引号
		PAIR MKeyIndex("MKeyIndex", indata[1]);
		outlist.push_back(MKeyIndex);
		// 工作密钥索引号
		PAIR WKeyIndex("WKeyIndex", indata[2]);
		outlist.push_back(WKeyIndex);
	}
	else if (8 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_GetDES--读取客户密码(DES)", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iTimeOut=%s, VoiceType=%s, EndType=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// 声音提示类型
		PAIR VoiceType("VoiceType", indata[2]);
		outlist.push_back(VoiceType);
		// 密码输入的结束方式
		PAIR EndType("EndType", indata[3]);
		outlist.push_back(EndType);
	}
	else if (9 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_GetPin--读取客户密码(ANSI98)", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iTimeOut=%s, AccNo=%s, VoiceType=%s, EndType=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// 账号
		PAIR AccNo("AccNo", indata[2]);
		outlist.push_back(AccNo);
		// 声音提示类型
		PAIR VoiceType("VoiceType", indata[3]);
		outlist.push_back(VoiceType);
		// 密码输入的结束方式
		PAIR EndType("EndType", indata[4]);
		outlist.push_back(EndType);
	}
	else if (10 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_StartInfoHtml--启动信息交互(html)", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--timeout=%s, modex=%s, info=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);
		
		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// 交互类型
		PAIR modex("modex", indata[2]);
		outlist.push_back(modex);
		// 显示文字信息
		PAIR info("info", indata[3]);
		outlist.push_back(info);
	}
	else if (11 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_WriteDeviceId--写入设备号", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--pDeviceId=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);
		
		// 设备号
		PAIR deviceId("deviceId", indata[1]);
		outlist.push_back(deviceId);
	}
	else if (12 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_ReadDeviceId--读取设备号", dir);
		outlist.clear();
	}
	else if (13 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_StartKeyboard--启动数字键盘", dir);
		outlist.clear();

		if ((*size) < 3)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iTimeOut=%s, numType=%s", indata[1], indata[2]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// 数字键盘类型
		PAIR numType("numType", indata[2]);
		outlist.push_back(numType);
	}
	else if (14 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_StartElectronicCard--启动电子工牌", dir);
		outlist.clear();

		if ((*size) < 6)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--tellerName=%s, tellerNo=%s, nStarLevel=%s, headFile=%s, iTimeout=%s", indata[1], indata[2], indata[3], indata[4], indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// 柜员名称
		PAIR tellerName("tellerName", indata[1]);
		outlist.push_back(tellerName);
		// 柜员号
		PAIR tellerNo("tellerNo", indata[2]);
		outlist.push_back(tellerNo);
		// 星级
		PAIR nStarLevel("nStarLevel", indata[3]);
		outlist.push_back(nStarLevel);
		// 图片名称
		PAIR headFile("headFile", indata[4]);
		outlist.push_back(headFile);
		// 超时时间
		PAIR timeout("iTimeOut", indata[5]);
		outlist.push_back(timeout);
	}
	else if (15 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_DownHeadFile--下载柜员头像", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--pFilePath=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// 柜员头像完整路径
		PAIR pFilePath("pFilePath", indata[1]);
		outlist.push_back(pFilePath);
	}
	else if (16 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_FindHeadPhoto--查询柜员头像", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--pHeadPhoto=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// 柜员头像名称
		PAIR pHeadPhoto("pHeadPhoto", indata[1]);
		outlist.push_back(pHeadPhoto);
	}
	else if (17 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_DelHeadFile--删除柜员头像", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--pFilename=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// 柜员头像文件名称
		PAIR pFilename("pFilename", indata[1]);
		outlist.push_back(pFilename);
	}
	else if (18 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_Welcome--播放欢迎光临", dir);
		outlist.clear();
	}
	else if (19 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_SetEvaluateInfo--设置柜员信息", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--tellerName=%s, tellerNo=%s, nStarLevel=%s, headFile=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// 柜员名称
		PAIR tellerName("tellerName", indata[1]);
		outlist.push_back(tellerName);
		// 柜员号
		PAIR tellerNo("tellerNo", indata[2]);
		outlist.push_back(tellerNo);
		// 星级
		PAIR nStarLevel("nStarLevel", indata[3]);
		outlist.push_back(nStarLevel);
		// 图片名称
		PAIR headFile("headFile", indata[4]);
		outlist.push_back(headFile);
	}
	else if (20 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_StartEvaluate--启动评价", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iTimeOut=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
	}
	else if (21 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_CancleEvaluate--撤销评价", dir);
		outlist.clear();
	}
	else if (30 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_CallShowPDF--展示PDF文件接口", dir);
		outlist.clear();

		if ((*size) < 7)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--timeout=%s, signMode=%s, pdfPath=%s, location=%s, imgPath=%s, textPath=%s", indata[1], indata[2], indata[3], indata[4], indata[5], indata[6]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// 签名模式
		PAIR signMode("signMode", indata[2]);
		outlist.push_back(signMode);
		// pdf电子凭证本地路径
		PAIR pdfPath("pdfPath", indata[3]);
		outlist.push_back(pdfPath);
		// 签名图片要显示的坐标位和签名区域宽高
		PAIR location("location", indata[4]);
		outlist.push_back(location);
		// 签名图片本地路径
		PAIR imgPath("imgPath", indata[5]);
		outlist.push_back(imgPath);
		// 签名数据txt文件路径
		PAIR textPath("textPath", indata[6]);
		outlist.push_back(textPath);
	}
	else if (31 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_SetMD--设置加密模式", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--mformat=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// 加密类型
		PAIR mformat("mformat", indata[1]);
		outlist.push_back(mformat);
	}
	else if (32 == type)
	{
		CGmtDriver::runLog("开始调用--GWQ_GetRandomZMK--产生随机主密钥密文", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("传入参数--iTimeOut=%s, PubKey=%s, SuanfaType=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);

		// 超时时间
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);

		// 加密平台提供的公钥
		PAIR PubKey("PubKey", indata[2]);
		outlist.push_back(PubKey);

		// 算法类型
		PAIR SuanfaType("SuanfaType", indata[3]);
		outlist.push_back(SuanfaType);
	}

	int iRet = pInst->driveGmt(type, outlist);

	CGmtDriver::runLog("结束调用\n", dir);

	sub = outlist;
	(*size) = sub.size();
	//pInst->RealseDriver();

	return sub;
}

int WINAPI SetTimeOut(int timeout)
{
	return 0;
}

int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CGmtDriver *pInst = IGMTCreator::getIntance();
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


