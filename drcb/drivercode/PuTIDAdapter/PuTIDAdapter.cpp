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
* ��  �ܣ������ӿڡ�
* ��  ����@outlist - �������֤��������
* ��  ����@nPort - �˿ںš�
* ��  ����@isUSB - �Ƿ�ΪUSB�豸��
* ��  ����@errcd - �������
* ��  ����@errmsg - ������Ϣ��
* ����ֵ���ɹ�����0,�����0
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

	//ͼƬ���·������C:\\IDR\\picture\\num1.jpg,Ҫ��֤·�����ļ��д��ڣ����򴴽�ͼƬʧ��
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
		sprintf(errmsg, "�����豸������ʧ��!\r\n%s", path);
		return -10;
	}

	if (IdcrReadCardInfo == NULL)
	{
		FreeLibrary(hDll);
		//SetCurrentDirectory(buf);
		sprintf(errcd, "-9");
		sprintf(errmsg, "���س��������⺯��ʧ��!\r\n");
		return -9;
	}

	CString photo;
	photo.Format("%s\\front.bmp", tmpsvdir);
	logStr.Format("ReadIdentityCard--�����֤��Ϣ������ͷ��·��=%s", photo);
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

	// ����֤�����ͣ�1Ϊ���֤��2Ϊ��������þ���֤
	int iCardType = 1;
	
	PERSONINFOW person;
	FOREIGNERINFOW foreigner;

	int ret = IdcrReadCardInfo(nPort, dwBaud, 0, &iCardType, &person, &foreigner, szPhotoImg);

	if(ret != 0){
		FreeLibrary(hDll);
		//SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "��֤����Ϣʧ��");
		return ret;
	}

	logStr.Format("ReadIdentityCard--�����֤��Ϣ�ɹ�������ֵΪ%d��֤������%d", ret, iCardType);
	runLog(logStr, dir2);

	outlist.clear();
	CString val;
	CString tmp;
	// ���֤
	if(1 == iCardType)
	{
		//����
		char lpstr[255] = {0};
		WCharToMByte(person.name, lpstr);
		val.Format("IDTF_NAME=%s", lpstr);
		val.Remove(' ');
		PAIR name("name", val);
		outlist.push_back(name);

		//�Ա�
		WCharToMByte(person.sex, lpstr);
		val.Format("IDTF_SEX=%s", lpstr);
		val.Remove(' ');
		PAIR sex("sex", val);
		outlist.push_back(sex);

		//����
		WCharToMByte(person.ethnic, lpstr);
		val.Format("IDTF_NATION=%s", lpstr);
		val.Remove(' ');
		PAIR national("national", val);
		outlist.push_back(national);

		//��������
		WCharToMByte(person.birthDate, lpstr);
		val.Format("IDTF_BIRTHDAY=%s", lpstr);
		val.Remove(' ');
		PAIR born("born", val);
		outlist.push_back(born);

		//��ַ
		WCharToMByte(person.address, lpstr);
		val.Format("IDTF_ADDRESS=%s", lpstr);
		val.Remove(' ');
		PAIR address("address", val);
		outlist.push_back(address);

		//���֤��
		WCharToMByte(person.idNumber, lpstr);
		val.Format("IDTF_CARDID=%s", lpstr);
		val.Remove(' ');
		PAIR IDN("IDN", val);
		outlist.push_back(IDN);

		//�Ǽǻ���
		WCharToMByte(person.issuer, lpstr);
		val.Format("IDTF_POLICE=%s", lpstr);
		val.Remove(' ');
		PAIR reg("register", val);
		outlist.push_back(reg);

		//��ʼ����
		WCharToMByte(person.effectDate, lpstr);
		CString startDate = lpstr;
		val.Format("IDTF_VALIDSTART=%s", lpstr);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);

		//��ֹ����
		WCharToMByte(person.expiryDate, lpstr);
		CString endDate = lpstr;
		val.Format("IDTF_VALIDEND=%s", lpstr);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);

		//��Ч��
		startDate.Remove(' ');
		endDate.Remove(' ');
		CString validDate = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", validDate);
		val.Remove(' ');
		PAIR valdate("valdate", val);
		outlist.push_back(valdate);

		//��Ƭ·��
		val.Format("IDTF_PATH=%s", photo);
		val.Remove(' ');
		PAIR pic("picture", val);
		outlist.push_back(pic);
		logStr.Format("ReadIdentityCard--���֤ͷ���ļ�·����%s", val);
		runLog(logStr, dir2);
	}
	else if(2 == iCardType)
	{
		// ����˾���֤
		//Ӣ����
		char lpstr[255] = {0};
		WCharToMByte(foreigner.englishName, lpstr);
		val.Format("IDTF_ENAME=%s", lpstr);
		val.Remove(' ');
		PAIR name("name", val);
		outlist.push_back(name);

		//�Ա�
		WCharToMByte(foreigner.sex, lpstr);
		val.Format("IDTF_SEX=%s", lpstr);
		val.Remove(' ');
		PAIR sex("sex", val);
		outlist.push_back(sex);

		//����֤����
		WCharToMByte(foreigner.prNumber, lpstr);
		val.Format("IDTF_CARDID=%s", lpstr);
		val.Remove(' ');
		PAIR IDN("IDN", val);
		outlist.push_back(IDN);

		//����
		WCharToMByte(foreigner.country, lpstr);
		val.Format("IDTF_NATION=%s", lpstr);
		val.Remove(' ');
		PAIR national("national", val);
		outlist.push_back(national);

		//������
		WCharToMByte(foreigner.chineseName, lpstr);
		val.Format("IDTF_NAME=%s", lpstr);
		val.Remove(' ');
		PAIR cname("name", val);
		outlist.push_back(cname);

		//��ʼ����
		WCharToMByte(foreigner.issueDate, lpstr);
		CString startDate = lpstr;
		val.Format("IDTF_VALIDSTART=%s", lpstr);
		val.Remove(' ');
		PAIR startdate("startdate", val);
		outlist.push_back(startdate);

		//��ֹ����
		WCharToMByte(foreigner.expiryDate, lpstr);
		CString endDate = lpstr;
		val.Format("IDTF_VALIDEND=%s", lpstr);
		val.Remove(' ');
		PAIR enddate("enddate", val);
		outlist.push_back(enddate);

		//��Ч��
		startDate.Remove(' ');
		endDate.Remove(' ');
		CString validDate = startDate + endDate;
		val.Format("IDTF_VALIDATE=%s", validDate);
		val.Remove(' ');
		PAIR valdate("valdate", val);
		outlist.push_back(valdate);

		//��������
		WCharToMByte(foreigner.birthDate, lpstr);
		val.Format("IDTF_BIRTHDAY=%s", lpstr);
		val.Remove(' ');
		PAIR born("born", val);
		outlist.push_back(born);

		//�汾��
		WCharToMByte(foreigner.version, lpstr);
		val.Format("IDTF_CARDVER=%s", lpstr);
		val.Remove(' ');
		PAIR address("address", val);
		outlist.push_back(address);

		// ֤������
		if (2 == iCardType)
		{
			val.Format("IDTF_CARDTYPE=%s", "I");
			val.Remove(' ');
			PAIR pCardtype("Cardtype", val);
			outlist.push_back(pCardtype);
		}

		//ǩ������
		WCharToMByte(foreigner.issuer, lpstr);
		val.Format("IDTF_POLICE=%s", lpstr);
		val.Remove(' ');
		PAIR reg("register", val);
		outlist.push_back(reg);

		//����ͼƬ·��
		val.Format("IDTF_PATH=%s", photo);
		val.Remove(' ');
		PAIR pPhotoPath("picture", val);
		outlist.push_back(pPhotoPath);
		logStr.Format("ReadIdentityCard--����֤ͷ���ļ�·����%s", photo);
		runLog(logStr, dir2);
	}
	else
	{
		FreeLibrary(hDll);
		sprintf(errcd, "%d", -11);
		sprintf(errmsg, "��ȡ֤��֤������ʧ��");
		return -11;
	}
	FreeLibrary(hDll);
	return 0;
}

/********************************************
* ��  �ܣ������ַ���תΪ���ֽ��ַ���
* ��  ����
* ����ֵ���ɹ�����TRUE
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
* ��  �ܣ��豸���á�
* ��  �����ޡ�
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI ResetDev()
{
	return 0;
}


/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
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
* ��  �ܣ�ɾ��ָ��Ŀ¼�µ������ļ�
* ��  ������Ҫɾ���ļ���Ŀ¼
*********************************************/
void deleteAllFile(CString dirname)
{
	CFileFind tempFind;
	BOOL bFound; // �ж��Ƿ�ɹ��ҵ��ļ�
	bFound = tempFind.FindFile(dirname + "\\*.*"); // �Ƿ���ƥ����ļ�
	CString strTmp;
	while (bFound)
	{
		bFound = tempFind.FindNextFile(); // ��һ��ִ����ѡ�񵽵�һ���ļ����Ժ�ִ��Ϊѡ����һ���ļ�
		if (tempFind.IsDots())
			continue;  // ����ҵ����Ƿ����ϲ�Ŀ¼������������
		if (tempFind.IsDirectory())
			continue;  // ����ҵ�����һ��Ŀ¼������������
		strTmp = tempFind.GetFileName(); // �����ļ�����������׺��
		DeleteFile(dirname + "\\" +strTmp);
	}
	tempFind.Close();
	return;
}


void runLog(CString logStr, CString logdir)
{
#ifdef DEBUG_H_LOG
	//д��־���ڲ��
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