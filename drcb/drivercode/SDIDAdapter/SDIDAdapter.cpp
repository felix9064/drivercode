// SDIDAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SDIDAdapter.h"

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
// CSDIDAdapterApp

BEGIN_MESSAGE_MAP(CSDIDAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CSDIDAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDIDAdapterApp construction

CSDIDAdapterApp::CSDIDAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSDIDAdapterApp object

CSDIDAdapterApp theApp;


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
	lpSD_ReadIDCard SD_ReadIDCard = 0;

	PERSONINFO m_PERSONINFO;
	memset(&m_PERSONINFO, 0, sizeof(tagPERSONINFO));
	
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
	
	int timeout = 15;
	if(outlist.size() > 2)
	{
		PAIR timeoutpar = outlist[2];
		timeout = atoi(timeoutpar.second);
	}
	int nTimeout = timeout;

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(dir2);
	HINSTANCE hDll = LoadLibrary(path);
	SetCurrentDirectory(buf);

	if (hDll != NULL)
	{
		SD_ReadIDCard = (lpSD_ReadIDCard)GetProcAddress(hDll, "SD_ReadIDCard");

		if (SD_ReadIDCard == NULL)
		{
			//SetCurrentDirectory(buf);
			sprintf(errcd, "-9");
			sprintf(errmsg, "���س��������⺯��ʧ��!\r\n");
			return -9;
		}
	}
	else
	{
		//SetCurrentDirectory(buf);
		sprintf(errcd, "-10");
		sprintf(errmsg, "�����豸������ʧ��!\r\n%s", path);
		return -10;
	}


	int dwBaud = 9600;
	CString bpNo = "";
	const char * szPhotoImg = tmpsvdir.GetBuffer(tmpsvdir.GetLength());

	int ret;

	ret = SD_ReadIDCard(nPort, bpNo.GetBuffer(bpNo.GetLength()), dwBaud, nTimeout, szPhotoImg, &m_PERSONINFO);
	if(ret != 0){
		//SetCurrentDirectory(buf);
		sprintf(errcd, "%d", ret);
		sprintf(errmsg, "�����֤��Ϣʧ��");
		return ret;
	}

	logStr.Format("ReadIdentityCard--�����֤��Ϣ�ɹ���%s", szPhotoImg);
	runLog(logStr, dir2);

	outlist.clear();
	CString val;
	
	//����
	val.Format("IDTF_NAME=%s", m_PERSONINFO.name);
	val.Remove(' ');
	PAIR name("name", val);
	outlist.push_back(name);

	//�Ա�
	val.Format("IDTF_SEX=%s", m_PERSONINFO.sex);
	val.Remove(' ');
	PAIR sex("sex", val);
	outlist.push_back(sex);

	//����
	val.Format("IDTF_NATION=%s", m_PERSONINFO.nation);
	val.Remove(' ');
	PAIR national("national", val);
	outlist.push_back(national);

	//��������
	val.Format("IDTF_BIRTHDAY=%s", m_PERSONINFO.birthday);
	val.Remove(' ');
	PAIR born("born", val);
	outlist.push_back(born);

	//��ַ
	val.Format("IDTF_ADDRESS=%s", m_PERSONINFO.address);
	val.Remove(' ');
	PAIR address("address", val);
	outlist.push_back(address);

	//���֤��
	val.Format("IDTF_CARDID=%s", m_PERSONINFO.cardId);
	val.Remove(' ');
	PAIR IDN("IDN", val);
	outlist.push_back(IDN);

	//�Ǽǻ���
	val.Format("IDTF_POLICE=%s", m_PERSONINFO.police);
	val.Remove(' ');
	PAIR reg("register", val);
	outlist.push_back(reg);

	//��ʼ����
	val.Format("IDTF_VALIDSTART=%s", m_PERSONINFO.validStart);
	val.Remove(' ');
	PAIR startdate("startdate", val);
	outlist.push_back(startdate);

	//��ֹ����
	val.Format("IDTF_VALIDEND=%s", m_PERSONINFO.validEnd);
	val.Remove(' ');
	PAIR enddate("enddate", val);
	outlist.push_back(enddate);

	//��Ч��
	CString startDate = m_PERSONINFO.validStart;
	startDate.Remove(' ');
	CString endDate = m_PERSONINFO.validEnd;
	endDate.Remove(' ');
	
	CString validDate = startDate + endDate;
	val.Format("IDTF_VALIDATE=%s", validDate);
	val.Remove(' ');
	PAIR valdate("valdate", val);
	outlist.push_back(valdate);

	//��Ƭ·��
	CString idtfpath;
	idtfpath.Format("%s\\%s.bmp", tmpsvdir, m_PERSONINFO.cardId);
	val.Format("IDTF_PATH=%s", idtfpath);
	val.Remove(' ');

	logStr.Format("ReadIdentityCard--���֤ͷ���ļ�·����%s", val);
	runLog(logStr, dir2);

	PAIR pic("picture", val);
	outlist.push_back(pic);

	//��סַ
	val.Format("IDTF_NEWADDRESS=%s", m_PERSONINFO.appendMsg);
	val.Remove(' ');
	PAIR newaddress("newaddress", val);
	outlist.push_back(newaddress);

	return 0;
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
	logFileName.Format("%s\\%s%04d%02d%02d.log", CAMERELOG_DIR, "SD",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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