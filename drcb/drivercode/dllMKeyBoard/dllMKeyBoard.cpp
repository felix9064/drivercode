// dllMKeyBoard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllMKeyBoard.h"

#include "DriverMKeyBoard.h"
#include "keyboardcreator.h"

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
// CDllMKeyBoardApp

BEGIN_MESSAGE_MAP(CDllMKeyBoardApp, CWinApp)
	//{{AFX_MSG_MAP(CDllMKeyBoardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllMKeyBoardApp construction

CDllMKeyBoardApp::CDllMKeyBoardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllMKeyBoardApp object

CDllMKeyBoardApp theApp;

/********************************************
* 功  能：打开设备，OpenDevice/CloseDevice需成对调用
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if (NULL == pInst)
	{
		IMKeyBdCreator::CoCreatIntance();
		pInst = IMKeyBdCreator::getIntance();
	}
	//IMKeyBdCreator::CoCreatIntance();
	//CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	

	CString csAppPath ;
	csAppPath.Format("%s\\keyboarddll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	return 0;
}


int WINAPI SetTimeOut(int timeout)
{
    CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if (NULL == pInst)
	{
		IMKeyBdCreator::CoCreatIntance();
		pInst = IMKeyBdCreator::getIntance();
	}
	pInst->SetTimeOut(timeout);
	return 0;
}


/********************************************
* 功  能：关闭设备
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI CloseDevice()
{
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	pInst->CloseMKeyBd();
	IMKeyBdCreator::ReleaseIntance();
	return 0;
}

/********************************************
* 功  能：复位
* 参  数：@rw - 读写类型。1-读，2-写
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Reset(int rw)
{
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
    int ret = pInst->ResetMKeyBd(rw);
	if(ret != SUCCESS)
	{
		pInst->CloseMKeyBd();
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //复位失败		
	}
	pInst->CloseMKeyBd();
	return 0;
}

/********************************************
* 功  能：往设备中写数据
* 参  数：
      @indata, 指针数组
* 返回值：成功返回0,否则非0
-----------------------
写工作密钥数组解释:
0:灌密钥操作类型 （1 - 明文灌注TMK; 2 - 密文灌注TMK; 3 - 密文灌注TPK.）南方使用手工灌TMK,所以在南方恒等于3 此元素不能省
1:主密钥索引号 //设定为0，此元素不能省，没有外部输入就为“0”
2:工作密钥索引号 //此元素不能省，没有外部输入就为“0”
3：主密钥密文 //此元素不能省，没有外部输入就为“0”
4：工作密钥密文 //此元素不能省，没有外部输入就为“0”
5：密钥校验串。//如果没有校验串，此元素为“0”字符串。
*********************************************/
int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	debug("write");
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if(conffile == 0)
		return ERR_NOFINDCONF;
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    int ret = pInst->OpenMKeyBd();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //打开串并口失败
	}

	int OPType = atoi(indata[0]);  //灌注密钥操作类型。1 - 明文灌注主密钥; 2 - 密文灌注主密钥; 3 - 密文灌注工作密钥.
	//TPCHR lpOldKey = NULL;
	//TPCHR lpKeyCK = NULL;
	//if(strcmp(indata[3], "0") != 0)
		//lpOldKey = indata[3];
	//if(strcmp(indata[5], "0") != 0)
		//lpKeyCK = indata[5];
	
	debug("%d;%s;%s;%s;%s;%s",OPType, indata[1], indata[2], indata[3], indata[4], indata[5]);
//	debug("write");
	 ret = pInst->WriteMKeyBd(OPType, indata[1], indata[2], indata[3], indata[4], indata[5]);
	if(ret != SUCCESS)
	{
		pInst->SetTimeOutZero();
		pInst->SetErrCode((enRetCode)ret);
		return ret;           //写磁条失败。
	}

	pInst->SetTimeOutZero();
	return SUCCESS;
}

/********************************************
* 功  能：从设备中读数据
* 参  数：
* 返回值：成功返回0,否则非0
-----------------------------
读Pin密文传入数组解释:
0:操作类型 0：读密码,1:读键盘序列号，2：获取评价值，3：读取设备号，4：写入明文主密钥，5：写入密文主密钥，6：写入工作密钥
1:输入模式(1-请输入密码,2 - 表示有两次提示音)
2:密码长度
3:主密钥索引号 //此元素不能省，没有外部输入就为“0”
4:工作密钥索引号 //此元素不能省，没有外部输入就为“0”
5:帐号 //此元素不能省，没有外部输入就为“0”
6:业务类型 //此元素不能省，没有外部输入就为“0”

*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString tmpLogStr;
	CString strRetCode;
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	VPair &sub = pInst->GetRetListRef();
	sub.clear();
	if(conffile == 0)
	{
		(*ret) = ERR_NOFINDCONF;
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;
	}
	CString &dir = pInst->GetModuleDir(); //用引用使dir指向_moduledir成员变量
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);
	
    (*ret) = pInst->OpenMKeyBd();
	debug("OpenMKeyBd");
	if((*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //打开串并口失败
	}

	int readType = atoi(indata[0]);
	debug("readType:%d",readType);
	if(readType == 0)
	{
		CString pwd;
		CString TPKCK;
		int minLen = atoi(indata[2]);
		int maxLen = atoi(indata[3]);
		//int BKType = atoi(indata[0]); //
		
		
		(*ret) = pInst->ReadPin(atoi(indata[1]),&minLen ,&maxLen, indata[4], "", indata[5], pwd, TPKCK, indata[6]);
		
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("PASS_WORD=%s",pwd);
			pair< CString, CString > passwd("password", msg);

			sub.push_back(retcode);
			sub.push_back(passwd);
		}
	}
	else if(readType == 1)
	{
		char result[21] = {0};
		(*ret) = pInst->GetPinPadID(result);
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("PinPadID=%s",result);
			pair< CString, CString > passwd("PinPadID", msg);
			sub.push_back(retcode);
			sub.push_back(passwd);
			
		}
	}
	else if (readType == 2)
	{
		int evaluateVal = 0;
		*ret = pInst->GetCustomEvaluate(&evaluateVal);
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("EVALUATE_VALUE=%d",evaluateVal);
			pair< CString, CString > passwd("EVALUATE_VALUE", msg);
			sub.push_back(retcode);
			sub.push_back(passwd);
			
		}
	}
	else 
	{
		VPair outlist;
		if (readType == 3)
		{
			// 读取密码键盘的序列号
		}
		else if (readType == 4)
		{
			// 写入公钥
			if ((*size) < 3)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// 算法标识
			PAIR ranKeyLen("ranKeyLen", indata[1]);
			outlist.push_back(ranKeyLen);
			
			// 公钥
			PAIR PK("PK", indata[2]);
			outlist.push_back(PK);
		}
		else if (readType == 5)
		{
			// 写入主密钥
			if ((*size) < 4)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// 主密钥索引
			PAIR keyIndex("keyIndex", indata[1]);
			outlist.push_back(keyIndex);

			// 算法标识
			PAIR newKeyLen("newKeyLen", indata[2]);
			outlist.push_back(newKeyLen);

			// 主密钥
			PAIR newKeyBySM4Key("newKeyBySM4Key", indata[3]);
			outlist.push_back(newKeyBySM4Key);

		}
		else if (readType == 6)
		{
			// 写入工作密钥
			if ((*size) < 5)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// 主密钥索引
			PAIR mainkeyIndex("mainkeyIndex", indata[1]);
			outlist.push_back(mainkeyIndex);

			// 工作密钥索引
			PAIR workkeyIndex("workkeyIndex", indata[2]);
			outlist.push_back(workkeyIndex);

			// 算法标识
			PAIR newKeyLen("newKeyLen", indata[3]);
			outlist.push_back(newKeyLen);

			// 工作密钥
			PAIR newKeyBySM4Key("newKeyBySM4Key", indata[4]);
			outlist.push_back(newKeyBySM4Key);
		}
		(*ret) = pInst->driveKB(readType, outlist);
		sub.clear();
		sub = outlist;
	}

	pInst->SetTimeOutZero();
    (*size) = sub.size();
	return sub;
}

/********************************************
* 功  能：获取设备类型
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI GetDeviceType()
{
	return PASSWORDKEY_DEV;
}

/********************************************
* 功  能：获取设备驱动描述
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("操作柜台密码键盘动态库");
}

/********************************************
* 功  能：取得当前错误详细描述
* 参  数：无
* 返回值：返回当前错误详细描述，无错误返回NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	static char errmess[BUFSIZ];
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	switch(err)
	{
	case ERR_LOADCONF:
		sprintf(errmess, "加载密码小键盘配置文件失败!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "打开密码小键盘设备端口失败!");
		break;
	case ERR_READTIMEOUT:
		sprintf(errmess, "读取设备数据超时!");
		break;
	case ERR_IOPEANDING:
		sprintf(errmess, "设备正忙!");
		break;
	case ERR_READDATA:
		sprintf(errmess, "读取数据失败!");
		break;
	case ERR_SENDDATA:
		sprintf(errmess, "发送数据失败!");
		break;
	case ERR_LENNOTENOUGH:
		sprintf(errmess, "密码长度不正确!");
		break;
	case ERR_TWICEPASSWORD:
		sprintf(errmess, "两次输入的密码不相符!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "无效的硬件指令!");
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "没有找到厂家适配器!");
		break;
	case ERR_LOADDRV:
		sprintf(errmess, "加载驱动库失败!");
		break;
	case ERR_READPINFAIL:
		sprintf(errmess, "读取PIN密码失败!");
		break;
	case ERR_WRITEMWKEY_FAIL:
		sprintf(errmess, "灌注密钥失败!");
		break;	
	default:
		sprintf(errmess,pInst->GetErrMsg());
		break;
	}
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
	if (pDrivInterf == NULL)
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
	pDrivInterf->SetTimeOut = SetTimeOut;

	return TRUE;
}

void debug(CString s)
{
#ifdef _DEBUG
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);

	 FILE* m_stream = fopen("drv.log", "a+");
	if (m_stream == NULL)
	{
		ASSERT(1==0);
	}

	time_t t;
	time(&t);
	tm *ptm = localtime(&t);
	fprintf(m_stream,"[%02d:%02d:%02d][%s]%s\n",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,FL,s);
	fclose(m_stream);
	
	SetCurrentDirectory(tmpdir);
#endif
}

void debug(const char* s,...)
{
#ifdef _DEBUG
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);

	FILE* m_stream = fopen("drv.log", "a+");
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
	fprintf(m_stream,"[%02d:%02d:%02d][%s]%s\n",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,FL,msg);
	fclose(m_stream);

	SetCurrentDirectory(tmpdir);
#endif
}
