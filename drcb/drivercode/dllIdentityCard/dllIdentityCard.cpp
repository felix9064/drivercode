// dllIdentityCard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllIdentityCard.h"

#include "identcardcreator.h"
#include "DriverIdentCard.h"
#include <direct.h>

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
// CDllIdentityCardApp

BEGIN_MESSAGE_MAP(CDllIdentityCardApp, CWinApp)
	//{{AFX_MSG_MAP(CDllIdentityCardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllIdentityCardApp construction

CDllIdentityCardApp::CDllIdentityCardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllIdentityCardApp object

CDllIdentityCardApp theApp;

/********************************************
* 功  能：打开设备，OpenDevice/CloseDevice需成对调用
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
	if (NULL == pInst)
	{
		IIdentCardCreator::CoCreatIntance();
		pInst = IIdentCardCreator::getIntance();
	}
	
	//从配置文件中读取驱动名和设备名称
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);

	CString csAppPath ;
	csAppPath.Format("%s\\identitycarddll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	return 0;
}

int WINAPI SetTimeOut(int timeout)
{
    CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
	if (NULL == pInst)
	{
		IIdentCardCreator::CoCreatIntance();
		pInst = IIdentCardCreator::getIntance();
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
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
	pInst->CloseICd();
	IIdentCardCreator::ReleaseIntance();
	return 0;
}

/********************************************
* 功  能：复位
* 参  数：@rw - 读写类型。1-读，2-写
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Reset(int rw)
{
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
    int ret = pInst->ResetICd(rw);
	if(ret != SUCCESS)
	{
		pInst->CloseICd();
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //复位失败		
	}
	pInst->CloseICd();
	return 0;
}

/********************************************
* 功  能：往设备中写数据
* 参  数：
      @indata, 指针数组
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
	if(conffile == 0)
		return ERR_NOFINDCONF;
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    int ret = pInst->OpenICd();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //打开串并口失败
	}

	ret = pInst->WriteICd();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode((enRetCode)ret);
		return -2;           //写磁条失败。
	}
	return 0;
}

/********************************************
* 功  能：从设备中读数据
* 参  数：indata[0] 身份证信息存入路径
* 返回值：成功返回0,否则非0
*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString strRetCode;
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
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
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    (*ret) = pInst->OpenICd();
	if((*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //打开串并口失败
	}

    CString fpdata;
	VPair outlist;

	char buf[255] = {0};
	GetCurrentDirectory(255,buf);
	SetCurrentDirectory(pInst->GetModuleDir());
	if(!_mkdir("tmp"))
	{
		pInst->SetErrCode( ERR_NEWDIR );
		strRetCode.Format("%d", (ERR_NEWDIR));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub; 
	}
	SetCurrentDirectory(buf);

	CString svdir;
	svdir.Format("%stmp",pInst->GetModuleDir());
	PAIR svimgpr("savedir", svdir);
	outlist.push_back(svimgpr);

	if ( (*size) == 0 )
	{
		PAIR optype("optype", "0");
		outlist.push_back(optype);
	}
	else if ( (*size) > 0 )
	{
		PAIR optype("optype", indata[0]);
		outlist.push_back(optype);
	}
	
	(*ret) = pInst->ReadICd(0, outlist);

	if( (*ret) != SUCCESS)
	{
		sub = outlist;
		pInst->SetErrCode( (enRetCode)(*ret));
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.insert(sub.begin(), retcode);
	}
	else
	{
		sub = outlist;
		CString strRetCode;
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.insert(sub.begin(), retcode);
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
	return IDENTCARD_DEV;
}

/********************************************
* 功  能：获取设备驱动描述
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("操作身份证核查设备动态库");
}

/********************************************
* 功  能：取得当前错误详细描述
* 参  数：无
* 返回值：返回当前错误详细描述，无错误返回NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	CDriverIdentCard *pInst = IIdentCardCreator::getIntance();
	static char errmess[BUFSIZ];
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	switch(err)
	{
	case ERR_NOFINDCONF:
		sprintf(errmess, "没有找到身份证核查仪驱动配置文件!");
		break;
	case ERR_LOADCONF:
		sprintf(errmess, "加载身份证核查仪配置文件失败!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "打开身份证核查仪端口失败!");
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
	case ERR_DEVRESPONSE:
		sprintf(errmess, "操作设备有错误，具体原因:%s", pInst->GetStatusDesc());
		break;
	case ERR_WAITTIMEOUT:
		sprintf(errmess, "等待按手指指纹超时!");
		break;
	case ERR_DEVSENDDATA:
		sprintf(errmess, "设备发送数据有误!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "无效的硬件指令!");
		break;
	case ERR_READFAIL:
		sprintf(errmess, "获取身份信息失败!\r\n错误代码: %s\r\n错误信息:%s",pInst->GetErrCd(), pInst->GetErrMsg());
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "未找到适配器dll!");
		break;
	case ERR_NEWDIR:
		sprintf(errmess, "创建照片文件夹失败");
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
	pDrivInterf->SetTimeOut = SetTimeOut;

	return TRUE;
}