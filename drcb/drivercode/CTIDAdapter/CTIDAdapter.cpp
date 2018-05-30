// CTIDAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "CTIDAdapter.h"

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
// CCTIDAdapterApp

BEGIN_MESSAGE_MAP(CCTIDAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CCTIDAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCTIDAdapterApp construction

CCTIDAdapterApp::CCTIDAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCTIDAdapterApp object

CCTIDAdapterApp theApp;


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

	lpCT_ReadIDCard CT_ReadIDCard = NULL;
	lpCT_ReadIDCardForeigner CT_ReadIDCardForeigner = NULL;

	PERSONINFO m_PERSONINFO;
	
	typedef pair<CString, CString> PAIR;
	PAIR dllpath = outlist[0];
	CString path = dllpath.second;
	int ps = path.ReverseFind('\\');
	CString dir2 = path.Mid(0, ps + 1);

	int optype = 0;
	if(outlist.size() > 1)
	{
		PAIR opertype = outlist[1];
		CString type = opertype.second;
		optype = atoi(type.GetBuffer(type.GetLength()));
	}

	//图片存放路径，如C:\\IDR\\picture\\num1.jpg,要保证路径下文件夹存在，否则创建图片失败
	CString tmpsvdir;
	if(outlist.size() > 2)
	{
		PAIR svdir = outlist[2];
		tmpsvdir = svdir.second;
	}
	
	if(!tmpsvdir.IsEmpty())
	{
		deleteAllFile(tmpsvdir);
	}
	
	int timeout = 15;
	if(outlist.size() > 3)
	{
		PAIR readtimeout = outlist[3];
		CString tmpvartime = readtimeout.second;
		timeout = atoi(tmpvartime.GetBuffer(tmpvartime.GetLength()));
	}

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(dir2);
	HINSTANCE hDll = LoadLibrary(path);
	SetCurrentDirectory(buf);

	if (hDll != NULL)
	{
		CT_ReadIDCard = (lpCT_ReadIDCard)GetProcAddress(hDll, "CT_ReadIDCard");
		CT_ReadIDCardForeigner = (lpCT_ReadIDCardForeigner)GetProcAddress(hDll, "CT_ReadIDCardForeigner");

		if (CT_ReadIDCard == NULL || CT_ReadIDCardForeigner == NULL)
		{
			//SetCurrentDirectory(buf);
			sprintf(errcd, "-9");
			sprintf(errmsg, "加载厂商驱动库函数失败!\r\n");
			return -9;
		}
	}
	else
	{
		//SetCurrentDirectory(buf);
		sprintf(errcd, "-10");
		sprintf(errmsg, "加载设备驱动库失败!\r\n%s", path);
		return -10;
	}


	int dwBaud = 9600;
	char bpNo[1] = {""};
	char szPhotoImg[128] = {0};
	memcpy(szPhotoImg, tmpsvdir.GetBuffer(tmpsvdir.GetLength()), tmpsvdir.GetLength());
	
	char strProt[10] = {0};
	if(isUSB){
		strcpy(strProt, "HID");
	}else{
		sprintf(strProt, "COM%d", nPort);
	}

	int ret;
	if(0 == optype)
	{
		ret = CT_ReadIDCard(strProt, bpNo, dwBaud, timeout, szPhotoImg, &m_PERSONINFO);
	}
	else 
	{
		ret = CT_ReadIDCardForeigner(strProt, bpNo, dwBaud, timeout, szPhotoImg, &m_PERSONINFO);
	}
	
	if(ret != 0){
		//SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "读身份证信息失败");
		return ret;
	}

	outlist.clear();
	CString val;
	
	//姓名
	val.Format("IDTF_NAME=%s", m_PERSONINFO.name);
	PAIR name("name", val);
	outlist.push_back(name);

	//性别
	val.Format("IDTF_SEX=%s", m_PERSONINFO.sex);
	PAIR sex("sex", val);
	outlist.push_back(sex);

	//民族
	val.Format("IDTF_NATION=%s", m_PERSONINFO.nation);
	PAIR national("national", val);
	outlist.push_back(national);

	//出生日期
	val.Format("IDTF_BIRTHDAY=%s", m_PERSONINFO.birthday);
	PAIR born("born", val);
	outlist.push_back(born);

	//地址
	val.Format("IDTF_ADDRESS=%s", m_PERSONINFO.address);
	PAIR address("address", val);
	outlist.push_back(address);

	//身份证号
	val.Format("IDTF_CARDID=%s", m_PERSONINFO.cardId);
	PAIR IDN("IDN", val);
	outlist.push_back(IDN);

	//登记机关
	val.Format("IDTF_POLICE=%s", m_PERSONINFO.police);
	PAIR reg("register", val);
	outlist.push_back(reg);

	//起始日期
	val.Format("IDTF_VALIDSTART=%s", m_PERSONINFO.validStart);
	PAIR startdate("startdate", val);
	outlist.push_back(startdate);

	//终止日期
	val.Format("IDTF_VALIDEND=%s", m_PERSONINFO.validEnd);
	PAIR enddate("enddate", val);
	outlist.push_back(enddate);

	//有效期
	CString validDate;
	validDate.Format("%s%s", m_PERSONINFO.validStart, m_PERSONINFO.validEnd);
	val.Format("IDTF_VALIDATE=%s", validDate);
	PAIR valdate("valdate", val);
	outlist.push_back(valdate);

	//照片路径
	CString idtfpath;
	idtfpath.Format("%s\\%s-head.bmp", szPhotoImg, m_PERSONINFO.cardId);
	val.Format("IDTF_PATH=%s", idtfpath);
	PAIR pic("picture", val);
	outlist.push_back(pic);

	//新住址
/*	val.Format("IDTF_NEWADDRESS=%s", m_PERSONINFO.appendMsg);
	PAIR newaddress("newaddress", val);
	outlist.push_back(newaddress);
*/
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
	logFileName.Format("%s\\%s%04d%02d%02d.log", CAMERELOG_DIR, "CT", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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