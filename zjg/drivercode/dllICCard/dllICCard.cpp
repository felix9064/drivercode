// dllICCard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllICCard.h"

#include "DriverICCard.h"
//#include "iccardcreator.h"

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
// CDllICCardApp

BEGIN_MESSAGE_MAP(CDllICCardApp, CWinApp)
	//{{AFX_MSG_MAP(CDllICCardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllICCardApp construction

CDllICCardApp::CDllICCardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllICCardApp object

CDllICCardApp theApp;

/********************************************
* 功  能：打开设备，OpenDevice/CloseDevice需成对调用
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	//if (pInst == NULL)
	//{
		//ICCardCreator::CoCreatIntance();
		//pInst = ICCardCreator::getIntance();
	//}
	CDriverICCard *pInst = CDriverICCard::GetInstance();
	if (!pInst)
	{
		return -1;
	}
	
	//从配置文件中读取驱动名和设备名称
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);
	
	CString csAppPath ;
	csAppPath.Format("%s\\iccarddll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	return 0;
}

int WINAPI SetTimeOut(int timeout)
{
    CDriverICCard *pInst = CDriverICCard::GetInstance();
	if (!pInst)
	{
		return -1;
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
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	//pInst->CloseMKeyBd();
	//ICCardCreator::ReleaseIntance();

	CDriverICCard *pInst = CDriverICCard::GetInstance();
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
	//CDriverICCard *pInst = ICCardCreator::getIntance();

	CDriverICCard *pInst = CDriverICCard::GetInstance();
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
	return _T("操作柜台IC卡动态库");
}

/********************************************
* 功  能：获取设备类型
* 参  数：
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI GetDeviceType()
{
	return ICCARD_DEV;
}


VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString strRetCode;
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	CDriverICCard *pInst = CDriverICCard::GetInstance();
	
	TCHAR current_buf[255];
	memset(current_buf, 0, 255);
	GetCurrentDirectory(255, current_buf);
	CString &dir = pInst->GetModuleDir(); //用引用使dir指向_moduledir成员变量
	SetCurrentDirectory(dir);
	VPair &sub = pInst->GetRetListRef();
    sub.clear();
	if(conffile == 0)
	{
		(*ret) = ERR_NOFINDCONF;
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		SetCurrentDirectory(current_buf);
		return sub;
	}
	
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(CString(portno));
	(*ret) = pInst->OpenIC();
    if((*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		SetCurrentDirectory(current_buf);
		return sub;  //打开串并口失败
	}
	(*ret) = pInst->GetThirdCalled();

	if ((*size) < 1)
	{
		(*ret) = ERR_ABCENTDATA;
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		SetCurrentDirectory(current_buf);
		return sub;
	}
	int optype = atoi(indata[0]);
	if (1 == optype)
	{
		//读卡信息
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		
		(*ret) = pInst->getIcInfo(sub);
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
	        pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}
		else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
	    //pInst->RealseDriver();   //生成F55域时要用到读卡信息时保存的一些数据，因此次数不能释放。
		
	}
    else if (2 == optype)
    {
		//卡获取卡信息与ARQC
		if ((*size) < 2)
		{
			(*ret) = ERR_ABCENTDATA;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		
		//AfxMessageBox(indata[1]);
		(*ret) = pInst->getIcInfoAndGenARQC(indata[1], sub);
        if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
	        pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
		
    }
	else if (3 == optype)
	{
		//取交易详细信息
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		(*ret) = pInst->GetTranDetail(sub);
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
	        pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
		pInst->RealseDriver();
	}
	
	else if (4 == optype)
	{
		//写ARPC
		if ((*size) < 2)
		{
			(*ret) = ERR_ABCENTDATA;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		(*ret) = pInst->CtlScriptData(indata[1], sub);
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
	        //pInst->RealseDriver();
			return sub;
		}else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
		
	    //pInst->RealseDriver();
	}
	else if (5 == optype)
	{
		//读卡信息
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		
		(*ret) = pInst->getIcInfo(sub);
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}
		else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
	    pInst->RealseDriver();

	}
	else if (6 == optype)
	{
		//生成ARQC
		if ((*size) < 2)
		{
			(*ret) = ERR_ABCENTDATA;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		
		//AfxMessageBox(indata[1]);
		(*ret) = pInst->genARQC(indata[1], sub);
        if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
	}
	else if (7 == optype)
	{
		//下电
		(*ret) = pInst->LoadDriver();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		if (!pInst->IsDriverLoaded())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}
		
		(*ret) = pInst->PowerOff(sub);
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}
		else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(),retcode);
		}
	    pInst->RealseDriver();
	}
	else
	{
		if ((*size) < 2)
		{
			(*ret) = ERR_ABCENTDATA;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}

		(*ret) = pInst->LoadDriver2();
		if((*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}

		if (!pInst->IsDriverLoaded2())
		{
			(*ret) = ERR_LOADDRV;
			pInst->SetErrCode( (enRetCode)(*ret) );
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			SetCurrentDirectory(current_buf);
			return sub;
		}

		(*ret) = pInst->invokeDrive(optype, indata[1], sub);
		if((*ret) != SUCCESS)
		{
			//pInst->SetErrCode( (enRetCode)(*ret) );
			
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(), retcode);
			pInst->RealseDriver();
			SetCurrentDirectory(current_buf);
			return sub;
		}
		else
		{
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.insert(sub.begin(), retcode);
		}
	    pInst->RealseDriver();
	}

	(*size) = sub.size();
	SetCurrentDirectory(current_buf);
	return sub;

}

int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	CDriverICCard *pInst = CDriverICCard::GetInstance();
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
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	static char errmess[BUFSIZ];
	CDriverICCard *pInst = CDriverICCard::GetInstance();
	if (!pInst)
	{
		sprintf(errmess, "无IC卡操作对象!");
		return errmess;
	}

	
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	
	switch(err)
	{
		case ERR_LOADCONF:
			sprintf(errmess, "加载IC卡读写器配置文件失败!");
			break;
		case ERR_OPENPORT:
			sprintf(errmess, "打开IC卡读写器设备端口失败!");
			break;
		case ERR_CLOSEPORT:
			sprintf(errmess, "关闭端口失败!");
			break;
		case ERR_RELEASE:
			sprintf(errmess, "释放动态库及关闭端口失败!");
			break;
		case ERR_READCARDINFO:
			sprintf(errmess, "读卡信息失败!");
			break;
		case ERR_PREUPDATE:
			sprintf(errmess, "预更新失败!");
			break;
		case ERR_UPDATEICCARD:
			sprintf(errmess, "更新卡信息失败!");
			break;
		case ERR_ONLINEFAILED:
			sprintf(errmess, "联网失败处理失败!");
			break;
		case ERR_NOFINDCONF:
			sprintf(errmess, "没找到配置文件!");
			break;
		case ERR_LOADDRV:
			sprintf(errmess, "加载动态库失败!");
			break;
		case ERR_NODRV:
			sprintf(errmess, "无动态库!");
			break;
		case ERR_NOFINDMATCHDLL:
			sprintf(errmess, "没有找到厂家适配器!");
			break;
		case ERR_ABCENTDATA:
			sprintf(errmess, "参数缺失!");
			break;
		case ERR_GENARQC:
			sprintf(errmess, "生成ARQC失败!");
			break;
		case ERR_CTLSCRIPTDATA:
			sprintf(errmess, "写ARPC失败!");
			break;
		case ERR_GETTRANDETAIL:
			sprintf(errmess, "读取交易详细信息失败!");
			break;
		case ERR_GETICINFOANDGENARQC:
			sprintf(errmess, "卡获取卡信息与ARQC失败!");
			break;
		case ERR_POWEROFF:
			sprintf(errmess, "下电失败!");
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