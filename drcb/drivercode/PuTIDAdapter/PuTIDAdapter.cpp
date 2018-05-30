// PuTIDAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PuTIDAdapter.h"

#include "../Share/IdentCardCom.h"
#include "datadefine.h"
#include "DIRECT.H"

#ifdef DEBUG_H_LOG
static FILE * m_stream;
#endif

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
// CPuTIDAdapterApp

BEGIN_MESSAGE_MAP(CPuTIDAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CPuTIDAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPuTIDAdapterApp construction

CPuTIDAdapterApp::CPuTIDAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPuTIDAdapterApp object

CPuTIDAdapterApp theApp;

/********************************************
* 功  能：读卡接口。
* 参  数：@outlist - 传出身份证数据链表。
* 参  数：@nPort - 端口号。
* 参  数：@isUSB - 是否为USB设备。
* 参  数：@errcd - 错误代码
* 参  数：@errmsg - 错误信息。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI ReadIdentityCard(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg)
{
	CString logStr;

	lpIdcrReadCardInfo IdcrReadCardInfo = NULL;
	
	typedef pair<CString, CString> PAIR;
	PAIR dllpath = outlist[0];
	CString path = dllpath.second;
	int ps = path.ReverseFind('\\');
	CString dir2 = path.Mid(0, ps + 1);

	//图片存放路径，如C:\\IDR\\picture\\num1.jpg,要保证路径下文件夹存在，否则创建图片失败
	CString tmpsvdir;
	if(outlist.size() > 1)
	{
		PAIR svdir = outlist[1];
		tmpsvdir = svdir.second;
	}

	if(!tmpsvdir.IsEmpty())
	{
		deleteAllFile(tmpsvdir);
	}
	
	int optype = 0;
	if(outlist.size() > 2)
	{
		PAIR opertype = outlist[2];
		CString type = opertype.second;
		optype = atoi(type.GetBuffer(type.GetLength()));
	}
	
	int timeout = 15;
	if(outlist.size() > 3)
	{
		PAIR readtimeout = outlist[3];
		CString tmpvartime = readtimeout.second;
		timeout = atoi(tmpvartime.GetBuffer(tmpvartime.GetLength()));
	}

	DWORD dwBaud = 9600;
	if (outlist.size() > 4)
	{
		PAIR baud = outlist[4];
		CString tmpbaud = baud.second;
		dwBaud = atoi(tmpbaud.GetBuffer(tmpbaud.GetLength()));
	}

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(dir2);
	HINSTANCE hDll = LoadLibrary(path);
	SetCurrentDirectory(buf);

	if (hDll != NULL)
	{
		IdcrReadCardInfo = (lpIdcrReadCardInfo)GetProcAddress(hDll, "IdcrReadCardInfo");
	}
	else
	{
		FreeLibrary(hDll);
		//SetCurrentDirectory(buf);
		sprintf(errcd, "-10");
		sprintf(errmsg, "加载设备驱动库失败!\r\n%s", path);
		return -10;
	}

	if (IdcrReadCardInfo == NULL)
	{
		FreeLibrary(hDll);
		//SetCurrentDirectory(buf);
		sprintf(errcd, "-9");
		sprintf(errmsg, "加载厂商驱动库函数失败!\r\n");
		return -9;
	}

	CString photo;
	photo.Format("%s\\front.bmp", tmpsvdir);
	logStr.Format("ReadIdentityCard--读身份证信息参数，头像路径=%s", photo);
	runLog(logStr, dir2);

	LPCWSTR szPhotoImg = photo.AllocSysString();
	//memcpy(szPhotoImg, tmpsvdir.GetBuffer(tmpsvdir.GetLength()), tmpsvdir.GetLength());
	
	int port = 0;
	if(isUSB)
	{
		port = 1001;
	}
	else
	{
		port = nPort;
	}

	// 传出证件类型，1为身份证，2为外国人永久居留证
	int iCardType = 1;
	
	PERSONINFOW person;
	FOREIGNERINFOW foreigner;

	int ret = IdcrReadCardInfo(nPort, dwBaud, 0, &iCardType, &person, &foreigner, szPhotoImg);

	if(ret != 0){
		FreeLibrary(hDll);
		//SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "读证件信息失败");
		return ret;
	}

	logStr.Format("ReadIdentityCard--读身份证信息成功，返回值为%d，证件类型%d", ret, iCardType);
	runLog(logStr, dir2);

	outlist.clear();
	CString val;
	CString tmp;
	// 身份证
	if(1 == iCardType)
	{
		//姓名
		char lpstr[255] = {0};
		WCharToMByte(person.name, lpstr);
		val.Format("IDTF_NAME=%s", lpstr);
		val.Remove(' ');
		PAIR name("name", val);
		outlist.push_back(name);

		//性别
		WCharToMByte(person.sex, lpstr);
		val.Format("IDTF_SEX=%s", lpstr);
		val.Remove(' ');
		PAIR sex("sex", val);
		outlist.push_back(sex);

		//民族
		WCharToMByte(person.ethnic, lpstr);
		val.Format("IDTF_NATION=%s", lpstr);
		val.Remove(' ');
		PAIR national("national", val);
		outlist.push_back(national);

		//出生日期
		WCharToMByte(person.birthDate, lpstr);
		val.Format("IDTF_BIRTHDAY=%s", lpstr);
		val.Remove(' ');
		PAIR born("born", val);
		outlist.push_back(born);

		//地址
		WCharToMByte(person.address, lpstr);
		val.Format("IDTF_ADDRESS=%s", lpstr);
		val.Remove(' ');
		PAIR address("address", val);
		outlist.push_back(address);

		//身份证号
		WCharToMByte(person.idNumber, lpstr);
		val.Format("IDTF_CARDID=%s", lpstr);
		val.Remove(' ');
		PAIR IDN("IDN", val);
		outlist.push_back(IDN);

		//登记机关
		WCharToMByte(person.issuer, lpstr);
		val.Format("IDTF_POLICE=%s", lpstr);
		val.Remove(' ');
		PAIR reg("register", val);
		outlist.push_back(reg);

		//起始日期
		WCharToMByte(person.effectDate, lpstr);
		CString startDate = lpstr;
		val.Format("IDTF_VALIDSTART=%s", lpstr);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);

		//终止日期
		WCharToMByte(person.expiryDate, lpstr);
		CString endDate = lpstr;
		val.Format("IDTF_VALIDEND=%s", lpstr);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);

		//有效期
		startDate.Remove(' ');
		endDate.Remove(' ');
		CString validDate = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", validDate);
		val.Remove(' ');
		PAIR valdate("valdate", val);
		outlist.push_back(valdate);

		//照片路径
		val.Format("IDTF_PATH=%s", photo);
		val.Remove(' ');
		PAIR pic("picture", val);
		outlist.push_back(pic);
		logStr.Format("ReadIdentityCard--身份证头像文件路径，%s", val);
		runLog(logStr, dir2);
	}
	else if(2 == iCardType)
	{
		// 外国人居留证
		//英文名
		char lpstr[255] = {0};
		WCharToMByte(foreigner.englishName, lpstr);
		val.Format("IDTF_ENAME=%s", lpstr);
		val.Remove(' ');
		PAIR name("name", val);
		outlist.push_back(name);

		//性别
		WCharToMByte(foreigner.sex, lpstr);
		val.Format("IDTF_SEX=%s", lpstr);
		val.Remove(' ');
		PAIR sex("sex", val);
		outlist.push_back(sex);

		//居留证号码
		WCharToMByte(foreigner.prNumber, lpstr);
		val.Format("IDTF_CARDID=%s", lpstr);
		val.Remove(' ');
		PAIR IDN("IDN", val);
		outlist.push_back(IDN);

		//国籍
		WCharToMByte(foreigner.country, lpstr);
		val.Format("IDTF_NATION=%s", lpstr);
		val.Remove(' ');
		PAIR national("national", val);
		outlist.push_back(national);

		//中文名
		WCharToMByte(foreigner.chineseName, lpstr);
		val.Format("IDTF_NAME=%s", lpstr);
		val.Remove(' ');
		PAIR cname("name", val);
		outlist.push_back(cname);

		//起始日期
		WCharToMByte(foreigner.issueDate, lpstr);
		CString startDate = lpstr;
		val.Format("IDTF_VALIDSTART=%s", lpstr);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);

		//终止日期
		WCharToMByte(foreigner.expiryDate, lpstr);
		CString endDate = lpstr;
		val.Format("IDTF_VALIDEND=%s", lpstr);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);

		//有效期
		startDate.Remove(' ');
		endDate.Remove(' ');
		CString validDate = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", validDate);
		val.Remove(' ');
		PAIR valdate("valdate", val);
		outlist.push_back(valdate);

		//出生日期
		WCharToMByte(foreigner.birthDate, lpstr);
		val.Format("IDTF_BIRTHDAY=%s", lpstr);
		val.Remove(' ');
		PAIR born("born", val);
		outlist.push_back(born);

		//版本号
		WCharToMByte(foreigner.version, lpstr);
		val.Format("IDTF_CARDVER=%s", lpstr);
		val.Remove(' ');
		PAIR address("address", val);
		outlist.push_back(address);

		// 证件类型
		if (2 == iCardType)
		{
			val.Format("IDTF_CARDTYPE=%s", "I");
			val.Remove(' ');
			PAIR pCardtype("Cardtype", val);
			outlist.push_back(pCardtype);
		}

		//签发机关
		WCharToMByte(foreigner.issuer, lpstr);
		val.Format("IDTF_POLICE=%s", lpstr);
		val.Remove(' ');
		PAIR reg("register", val);
		outlist.push_back(reg);

		//生成图片路径
		val.Format("IDTF_PATH=%s", photo);
		val.Remove(' ');
		PAIR pPhotoPath("picture", val);
		outlist.push_back(pPhotoPath);
		logStr.Format("ReadIdentityCard--居留证头像文件路径，%s", photo);
		runLog(logStr, dir2);
	}
	else
	{
		FreeLibrary(hDll);
		sprintf(errcd, "%d", -11);
		sprintf(errmsg, "获取证件证件类型失败");
		return -11;
	}
	FreeLibrary(hDll);
	return 0;
}

/********************************************
* 功  能：将宽字符串转为多字节字符串
* 参  数：
* 返回值：成功返回TRUE
*********************************************/
BOOL WCharToMByte(LPCWSTR lpcwszStr, char *lpszStr)
{
	memset(lpszStr, 0, 255);
	int dwMinSize;
	dwMinSize = WideCharToMultiByte(CP_ACP, 0, lpcwszStr, -1, NULL, 0, NULL, FALSE);

	WideCharToMultiByte(CP_ACP, 0, lpcwszStr, -1, lpszStr, dwMinSize, NULL, FALSE);
	return TRUE;
}


/********************************************
* 功  能：设备重置。
* 参  数：无。
* 返回值：成功返回0,否则非0
*********************************************/
int WINAPI ResetDev()
{
	return 0;
}


/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetIdentityCardCOM(IDENT_CARD_INTERFACE *pIdentCardInterf)
{
	if(pIdentCardInterf)
	{
		pIdentCardInterf->ReadIdentityCard = ReadIdentityCard;
		pIdentCardInterf->ResetDev = ResetDev;
		return TRUE;
	}
	return FALSE;
}


/********************************************
* 功  能：删除指定目录下的所有文件
* 参  数：需要删除文件的目录
*********************************************/
void deleteAllFile(CString dirname)
{
	CFileFind tempFind;
	BOOL bFound; // 判断是否成功找到文件
	bFound = tempFind.FindFile(dirname + "\\*.*"); // 是否有匹配的文件
	CString strTmp;
	while (bFound)
	{
		bFound = tempFind.FindNextFile(); // 第一次执行是选择到第一个文件，以后执行为选择到下一个文件
		if (tempFind.IsDots())
			continue;  // 如果找到的是返回上层目录，则跳过继续
		if (tempFind.IsDirectory())
			continue;  // 如果找到的是一个目录，则跳过继续
		strTmp = tempFind.GetFileName(); // 保存文件名，包括后缀名
		DeleteFile(dirname + "\\" +strTmp);
	}
	tempFind.Close();
	return;
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
	logFileName.Format("%s\\%s%04d%02d%02d.log", CAMERELOG_DIR, "PT", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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