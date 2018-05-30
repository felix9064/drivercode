// dllFinger.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllFinger.h"

#include "Driverfinger.h"
#include "fingercreator.h"

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
// CDllFingerApp

BEGIN_MESSAGE_MAP(CDllFingerApp, CWinApp)
	//{{AFX_MSG_MAP(CDllFingerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllFingerApp construction

CDllFingerApp::CDllFingerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllFingerApp object

CDllFingerApp theApp;

/********************************************
* 功  能：打开设备，OpenDevice/CloseDevice需成对调用
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
	CString tmpstr;
	if (NULL == pInst)
	{
		IFingerCreator::CoCreatIntance();
	    pInst = IFingerCreator::getIntance();
	}
	
	//从配置文件中读取驱动名和设备名称
	//CString devicena;
	//CString driverna;
	//TCHAR temp[255];
	//memset(temp, 0, 255);

	CString csAppPath ;
	csAppPath.Format("%s\\fingerdll\\", modulepath);
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
	CDriverFinger *pInst = IFingerCreator::getIntance();
	pInst->CloseFp();
	IFingerCreator::ReleaseIntance();
	return 0;
}

/********************************************
* 功  能：复位
* 参  数：@rw - 读写类型。1-读，2-写
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI Reset(int rw)
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
    int ret = pInst->ResetFp(rw);
	if(ret != SUCCESS)
	{
		pInst->CloseFp();
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //复位失败		
	}
	pInst->CloseFp();
	return 0;
}

/********************************************
    * 功  能：设置设备操作超时时间
    * 参  数：@timeout - 设备操作超时时间。单位为秒
    * 返回值：成功返回0,否则非0
*********************************************/
int WINAPI SetTimeOut(int timeout)
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
	if (NULL == pInst)
	{
		IFingerCreator::CoCreatIntance();
		pInst = IFingerCreator::getIntance();
	}

    pInst->SetTimeOut(timeout);
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
	CDriverFinger *pInst = IFingerCreator::getIntance();
	if(conffile == 0)
		return ERR_NOFINDCONF;
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    int ret = pInst->OpenFp();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //打开串并口失败
	}

	ret = pInst->WriteFp();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode((enRetCode)ret);
		return -2;           //写磁条失败。
	}
	return 0;
}

/********************************************
* 功  能：从设备中读数据
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString strRetCode;
	CDriverFinger *pInst = IFingerCreator::getIntance();
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

    (*ret) = pInst->OpenFp();
	if((*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //打开串并口失败
	}

    short optype ;   // 0 - 采集指纹模板 1 - 采集指纹特征 2 - 指纹比对
	if((*size) > 0)
		optype = atoi(indata[0]);
	else
		optype = GETFINGERDAT_OP;
	
	VPair outlist;
    
	if (optype == GETFINGERDAT_OP)
	{
		//东莞农商行暂时只有该操作类型
		(*ret) = pInst->ReadFp(outlist, optype);
	} 
	else
	{
		//(*ret) = pInst->ReadFp(indata[0], indata[1], fpdata, optype, indata[3]);
	}
	
	if( (*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret));
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
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
	return FINGERPRINT_DEV;
}

/********************************************
* 功  能：获取设备驱动描述
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("操作指纹仪动态库");
}

/********************************************
* 功  能：取得当前错误详细描述
* 参  数：无
* 返回值：返回当前错误详细描述，无错误返回NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
	static char errmess[BUFSIZ];
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	switch(err)
	{
	case ERR_NOFINDCONF:
		sprintf(errmess, "没有找到指纹仪驱动配置文件!");
		break;
	case ERR_LOADCONF:
		sprintf(errmess, "加载指纹仪配置文件失败!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "打开指纹仪端口失败!");
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
		sprintf(errmess, "指纹仪设备发送数据有误!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "无效的硬件指令!");
		break;
	case ERR_FPCHECKFAIL:
		sprintf(errmess, "签到柜员指纹验证失败!\r\n错误代码: %s\r\n错误信息:%s",pInst->GetFpErrCd(), pInst->GetFpErrMsg());
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "未找到指纹校验dll!");
		break;
	case ERR_NOFINDTHIRDCALL:
		sprintf(errmess, "未找到驱动库!");
		break;
	case ERR_READFAIL:
		sprintf(errmess, "指纹操作失败!\r\n错误代码: %s\r\n错误信息:%s",pInst->GetFpErrCd(), pInst->GetFpErrMsg());
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
