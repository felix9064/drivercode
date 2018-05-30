// NanTIDAdapter8903M.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NanTIDAdapter8903M.h"

#include "../Share/IdentCardCom.h"
#include "datdefine.h"
#include <direct.h>

/*
#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

#include <string>
using namespace std;
*/

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
// CNanTIDAdapter8903MApp

BEGIN_MESSAGE_MAP(CNanTIDAdapter8903MApp, CWinApp)
	//{{AFX_MSG_MAP(CNanTIDAdapter8903MApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNanTIDAdapter8903MApp construction

CNanTIDAdapter8903MApp::CNanTIDAdapter8903MApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNanTIDAdapter8903MApp object

CNanTIDAdapter8903MApp theApp;


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

	//lpIsDevConnect IsDevConnect = 0;
	lpIDCard_GetInformation IDCard_GetInformation = 0;
	lpGetForeignInfo GetForeignInfo = 0;

	typedef pair<CString, CString> PAIR;
	PAIR dllpath = outlist[0];
	CString path = dllpath.second;
	int ps = path.ReverseFind('\\');
	CString dir = path.Mid(0, ps + 1);
	CString dir2 = dir; //至此dir2是以\结尾的字符串，其意义为厂家驱动的所在路径

	CString tmpsvdir;
	short readtime = 15;
	int dwBaud = 9600;
	
	if(outlist.size() > 1)
	{
		PAIR svdir = outlist[1];
		tmpsvdir = svdir.second;
	}

	int optype = 0;
	if (outlist.size() > 2)
	{
		PAIR opertype = outlist[2];
		CString type = opertype.second;
		optype = atoi(type.GetBuffer(type.GetLength()));
	}

	if (outlist.size() > 3)
	{
		PAIR readtimeout = outlist[3];
		CString tmpvartime = readtimeout.second;
		readtime = atoi(tmpvartime.GetBuffer(tmpvartime.GetLength()));
	}

	if (outlist.size() > 4)
	{
		PAIR baud = outlist[4];
		CString tmpbaud = baud.second;
		dwBaud = atoi(tmpbaud.GetBuffer(tmpbaud.GetLength()));
	}

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);

	if(!tmpsvdir.IsEmpty())
	{
		deleteAllFile(tmpsvdir);
	}

	SetCurrentDirectory(dir2);
	HINSTANCE hDll = LoadLibrary(path);
	if (hDll != NULL)
	{
		//IsDevConnect = (lpIsDevConnect)GetProcAddress(hDll,"IsDevConnect");
		IDCard_GetInformation = (lpIDCard_GetInformation)GetProcAddress(hDll,"IDCard_GetInformation");
		GetForeignInfo = (lpGetForeignInfo)GetProcAddress(hDll,"GetForeignInfo");
	}
	else
	{
		SetCurrentDirectory(buf);
		sprintf(errcd, "-10");
		sprintf(errmsg, "加载设备驱动库失败!\r\n%s", path);
		return -10;
	}

	if(IDCard_GetInformation == NULL || GetForeignInfo == NULL)
	{
		FreeLibrary(hDll);
		SetCurrentDirectory(buf);
		sprintf(errcd, "-9");
		sprintf(errmsg, "加载厂商驱动库函数失败!\r\n");
		return -9;
	}

	int _port;
	if(isUSB){
		_port = 100;
	}else{
		_port = nPort;
	}
	
	/*
	ret = IsDevConnect(_port, dwBaud, bpNo.GetBuffer(bpNo.GetLength()));
	if(ret != 1)
	{
		SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "设备未连接");
		return -1;
	}
	*/

	char idInfo[1024];
	int ret;
	if (0 == optype)
	{
		ret = IDCard_GetInformation(_port, dwBaud, "", readtime, idInfo, ".", ".\\tmp", NULL, NULL, NULL);
	}
	else
	{
		ret = GetForeignInfo(_port, dwBaud, readtime, ".\\tmp", idInfo, NULL);
	}

	
	if(ret != 0)
	{
		FreeLibrary(hDll);
		SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "读身份证信息失败");
		return -1;
	}

	logStr.Format("读身份证信息成功：%s", idInfo);
	runLog(logStr, dir2);

	outlist.clear();

	if(0 == optype)
	{
		CString tmpStr = idInfo;
		int tmpPos = 0;
		CString tmp;
		CString val;
		CString idNumber; // 缓存身份证号码，为后面保存照片路径

		// 姓名
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_NAME=%s", tmp);
		val.Remove(' ');
		PAIR name("name", val);
		outlist.push_back(name);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符

		// 身份证号码
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		idNumber = tmp;
		val.Format("IDTF_CARDID=%s", tmp);
		val.Remove(' ');
		PAIR IDN("IDN", val);
		outlist.push_back(IDN);
		tmpStr.Delete(0, tmpPos+1);

		// 性别
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_SEX=%s", tmp);
		val.Remove(' ');
		PAIR sex("sex", val);
		outlist.push_back(sex);
		tmpStr.Delete(0, tmpPos+1);

		// 名族
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_NATION=%s", tmp);
		val.Remove(' ');
		PAIR national("national", val);
		outlist.push_back(national);
		tmpStr.Delete(0, tmpPos+1);

		// 出生日期
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_BIRTHDAY=%s", tmp);
		val.Remove(' ');
		PAIR born("born", val);
		outlist.push_back(born);
		tmpStr.Delete(0, tmpPos+1);

		// 地址
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_ADDRESS=%s", tmp);
		val.Remove(' ');
		PAIR address("address", val);
		outlist.push_back(address);
		tmpStr.Delete(0, tmpPos+1);
		
		// 登记机关
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_POLICE=%s", tmp);
		val.Remove(' ');
		PAIR reg("register", val);
		outlist.push_back(reg);
		tmpStr.Delete(0, tmpPos+1);

		//起始日期
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		CString startDate = tmp;
		val.Format("IDTF_VALIDSTART=%s", tmp);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);
		tmpStr.Delete(0, tmpPos+1);

		//终止日期
		//生效的只有终止日期，且可能不包含分隔符
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		else
		{
			tmp = tmpStr;
		}
		CString endDate = tmp;
		val.Format("IDTF_VALIDEND=%s", tmp);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);

		// 有效日期
		tmp = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", tmp);
		val.Remove(' ');
		PAIR valdate("valdate", val);
		outlist.push_back(valdate);

		// 照片
		CString dirpath;
		dirpath.Format("%s\\%s.bmp", tmpsvdir, idNumber);
		val.Format("IDTF_PATH=%s", dirpath);
		val.Remove(' ');

		logStr.Format("ReadIdentityCard--身份证头像文件路径，%s", val);
		runLog(logStr, dir2);

		PAIR pic("picture", val);
		outlist.push_back(pic);
	}
	else
	{
		// 英文姓名|性别代码|永久居留证号码|国籍或所在地区代码|中文姓名|证件签发日期|证件终止日期|出生日期|证件版本号|当次申请受理机关代码|证件类型标识|预留项
		CString tmpStr = idInfo;
		int tmpPos = 0;
		CString tmp;
		CString val;
		CString idNumber; // 缓存居留证号码，为后面保存照片路径

		// 英文名
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_ENAME=%s", tmp);
		val.Remove(' ');
		PAIR pEnglishName("EnglishName", val);
		outlist.push_back(pEnglishName);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("EnglishName: %s\n", tmp);
		runLog(logStr, dir2);

		// 性别
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_SEX=%s", tmp);
		val.Remove(' ');
		PAIR pSex("Sex", val);
		outlist.push_back(pSex);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("Sex: %s\n", tmp);
		runLog(logStr, dir2);

		// 居留证号码
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		idNumber = tmp;
		val.Format("IDTF_CARDID=%s", tmp);
		val.Remove(' ');
		PAIR pID("ID", val);
		outlist.push_back(pID);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("ID: %s\n", tmp);
		runLog(logStr, dir2);

		// 国籍或所在地区代码
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_NATION=%s", tmp);
		val.Remove(' ');
		PAIR pNation("Nation", val);
		outlist.push_back(pNation);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("Nation: %s\n", tmp);
		runLog(logStr, dir2);

		// 中文名
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_NAME=%s", tmp);
		val.Remove(' ');
		PAIR pChineseName("ChineseName", val);
		outlist.push_back(pChineseName);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("ChineseName: %s\n", tmp);
		runLog(logStr, dir2);

		// 有效期
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		CString startDate = tmp; // 起始有效期
		val.Format("IDTF_VALIDSTART=%s", tmp);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符

		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		CString endDate = tmp; // 终止有效期
		val.Format("IDTF_VALIDEND=%s", tmp);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		
		CString valdate = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", valdate);
		val.Remove(' ');
		PAIR pValDate("valdate", val);
		outlist.push_back(pValDate);
		logStr.Format("ValDate: %s\n", valdate);
		runLog(logStr, dir2);

		// 出生日期
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_BIRTHDAY=%s", tmp);
		val.Remove(' ');
		PAIR pBirthday("Birthday", val);
		outlist.push_back(pBirthday);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("Birthday: %s\n", tmp);
		runLog(logStr, dir2);

		// 证件版本号
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_CARDVER=%s", tmp);
		val.Remove(' ');
		PAIR pCardver("Cardver", val);
		outlist.push_back(pCardver);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("Cardver: %s\n", tmp);
		runLog(logStr, dir2);

		// 签发机关代码
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		val.Format("IDTF_POLICE=%s", tmp);
		val.Remove(' ');
		PAIR pPolicecode("Policecode", val);
		outlist.push_back(pPolicecode);
		tmpStr.Delete(0, tmpPos+1); //删除该字段包括分隔符
		logStr.Format("Policecode: %s\n", tmp);
		runLog(logStr, dir2);

		// 证件类型
		tmpPos = tmpStr.Find('|', 0);
		if (tmpPos != -1)
		{
			tmp = tmpStr.Mid(0, tmpPos);
		}
		else
		{
			tmp = tmpStr;
		}
		val.Format("IDTF_CARDTYPE=%s", tmp);
		val.Remove(' ');
		PAIR pCardtype("Cardtype", val);
		outlist.push_back(pCardtype);
		logStr.Format("Cardtype: %s\n", tmp);
		runLog(logStr, dir2);

		// 照片路径
		CString dirpath;
		dirpath.Format("%s\\%s.bmp", tmpsvdir, idNumber);
		val.Format("IDTF_PATH=%s", dirpath);
		val.Remove(' ');
		PAIR pPhotoPath("picture", val);
		outlist.push_back(pPhotoPath);
		logStr.Format("PhotoPath: %s\n", dirpath);
		runLog(logStr, dir2);
	}
	FreeLibrary(hDll);
	return 0;
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
	logFileName.Format("%s\\%s%04d%02d%02d.log", CAMERELOG_DIR, "NT", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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