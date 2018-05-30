// ShengTAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ShengTAdapter.h"
#include "datadef.h"
#include "DIRECT.H"

#ifdef DEBUG_H_LOG
static FILE * m_stream;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void runLog(CString logStr, CString logdir = "C:\\");
void MyDeleteFile(CString dirPath);

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
// CShengTAdapterApp

BEGIN_MESSAGE_MAP(CShengTAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CShengTAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShengTAdapterApp construction

CShengTAdapterApp::CShengTAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShengTAdapterApp object

CShengTAdapterApp theApp;


/********************************************
* ��  �ܣ���������
* ��  ����@outlist - ��������������
* ��  ����@nPort - �˿ںš�
* ��  ����@isUSB - �Ƿ�ΪUSB�豸��
* ��  ����@errcd - �������
* ��  ����@errmsg - ������Ϣ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI ReadCard(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg)
{

	typedef pair<CString, CString> PAIR;
	int size = outlist.size();
	CString logStr;
	lpCT_ReadMsgCard CT_ReadMsgCard = NULL;
	
	CString dir, dir2;
	//��������̬��
	CString path;
	if(size > 0)
	{
		PAIR tmp = outlist[0];
		path = tmp.second;
		int ps = path.ReverseFind('\\');
		dir2 = path.Mid(0, ps + 1); //����dir2����\��β���ַ�����������Ϊ��������������·��
		dir = dir2;
	}

	//�ŵ���Ϣ 2:��ȡ���ŵ���Ϣ 3:��ȡ���ŵ���Ϣ
	char track[10] = {0};
	if(size > 1)
	{
		PAIR tmp = outlist[1];
		strcpy(track , tmp.second);
	}

	//��ʱʱ��
	int nTimeout = 15;
	if(size > 2)
	{
		PAIR tmp = outlist[2];
		nTimeout = atoi(tmp.second);
	}

	//������
	int iBaudrate = 115200;
	if(size > 3)
	{
		PAIR tmp = outlist[3];
		iBaudrate = atoi(tmp.second);
	}

	//���ں�
	char ComPort[10] = {0};
	if(isUSB)
	{
		CString usb = "USB0";
		memcpy(ComPort, usb.GetBuffer(usb.GetLength()), usb.GetLength());	
	}
	else
	{
		CString _port;
		_port.Format("COM%d", nPort);
		strcpy(ComPort, _port);	
	}

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(dir2);
	HMODULE hDll = LoadLibrary(path);
	SetCurrentDirectory(buf);

	if (hDll != NULL)
	{
		CT_ReadMsgCard = (lpCT_ReadMsgCard)GetProcAddress(hDll, "ReadMsgCard");
	}
	else
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-9");
		sprintf(errmsg, "�����豸������ʧ��!\r\n%s", path);
		logStr.Format("�����豸������ʧ�ܣ�%d\r\n%s", -9, path);
		runLog(logStr, dir2);
		return -9;
	}

	if(CT_ReadMsgCard == NULL)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-10");
		sprintf(errmsg, "��ȡ���������ӿں���ʧ��!\r\n%s", path);
		logStr.Format("��ȡ���������ӿں���ʧ�ܣ�%d\r\n%s", -10, path);
		runLog(logStr, dir2);
		return -10;
	}

	char szTrack2data[512], szTrack3data[512];

	memset(szTrack2data, 0, sizeof(szTrack2data));
	memset(szTrack3data, 0, sizeof(szTrack3data));

	logStr.Format("���ź������������\n�˿�=%s;\n������=%d;\n���ŵ�����=%s;\n���ŵ�����=%d;\n���ŵ�����=%s;\n���ŵ�����=%d;", ComPort, iBaudrate, szTrack2data, sizeof(szTrack2data), szTrack3data, sizeof(szTrack3data));
	runLog(logStr, dir2);

	int iret = CT_ReadMsgCard(ComPort, iBaudrate, "", 1, nTimeout, NULL, 0, szTrack2data, sizeof(szTrack2data), szTrack3data, sizeof(szTrack3data));
	
	logStr.Format("���ó��̶��ź�������ֵ��%d", iret);
	runLog(logStr, dir2);
	
	if(iret != 0)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "%d", iret);
		sprintf(errmsg, "��ȡ�ŵ���Ϣʧ��!\r\n%s", path);
		return -20;
	}

	logStr.Format("���ŷ��ض��ŵ����ݣ�%s;\n���ŵ����ݣ�%s", szTrack2data, szTrack3data);
	runLog(logStr, dir2);

	outlist.clear();

	CString strTrack2;
	strTrack2.Format("%s", szTrack2data);
	PAIR Track2("t2", strTrack2);
	outlist.push_back(Track2);

	CString strTrack3;
	strTrack3.Format("%s", szTrack3data);
	PAIR Track3("t3", strTrack3);
	outlist.push_back(Track3);

	FreeLibrary(hDll);
	return 0;
}

/********************************************
* ��  �ܣ�д������
* ��  ����@outlist - ���뿨��������
* ��  ����@nPort - �˿ںš�
* ��  ����@isUSB - �Ƿ�ΪUSB�豸��
* ��  ����@errcd - �������
* ��  ����@errmsg - ������Ϣ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI WriteCard(short nPort, short isUSB, VPair &inlist, char *errcd, char *errmsg)
{
	typedef pair<CString, CString> PAIR;
	int size = inlist.size();
	CString dir, dir2;
	CString logStr;
	lpCT_WriteMsgCard CT_WriteMsgCard = NULL;
	
	CString path;
	if(size > 0)
	{
		PAIR tmp = inlist[0];
		path = tmp.second;
		int ps = path.ReverseFind('\\');
		dir2 = path.Mid(0, ps + 1); //����dir2����\��β���ַ�����������Ϊ��������������·��
		dir = dir2;
	}
	//�ŵ�
	int track = 1;
	if(size > 1)
	{
		PAIR tmp = inlist[1];
		CString _track = tmp.second;
		if("23" == _track)
		{
			track = 1;
		}
		else if("2" == _track)
		{
			track = 2;
		}
		else if("3" == _track)
			track = 3;
	}

	char track2[512] = {0};
	if(size > 2)
	{
		PAIR tmp = inlist[2];
		CString _track = tmp.second;
		memcpy(track2, _track.GetBuffer(_track.GetLength()), _track.GetLength());
	}

	char track3[512] = {0};
	if(size > 3)
	{
		PAIR tmp = inlist[3];
		CString _track = tmp.second;
		memcpy(track3, _track.GetBuffer(_track.GetLength()), _track.GetLength());
	}

	//��ʱʱ��
	int nTimeout = 15;
	if(size > 4)
	{
		PAIR tmp = inlist[4];
		nTimeout = atoi(tmp.second);
	}

	//������
	int iBaudrate = 9600;
	if(size > 5)
	{
		PAIR tmp = inlist[5];
		iBaudrate = atoi(tmp.second);
	}

	//���ں�
	char ComPort[10] = {0};
	if(isUSB)
	{
		CString usb = "USB0";
		memcpy(ComPort, usb.GetBuffer(usb.GetLength()), usb.GetLength());	
	}
	else
	{
		CString _port;
		_port.Format("COM%d", nPort);
		strcpy(ComPort, _port);	
	}

	HMODULE hDll = LoadLibrary(path);
	if (hDll != NULL)
	{
		CT_WriteMsgCard = (lpCT_WriteMsgCard)GetProcAddress(hDll, "WriteMsgCard");
	}
	else
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-10");
		sprintf(errmsg, "�����豸������ʧ��!\r\n%s", path);
		return -10;
	}
	
	if(CT_WriteMsgCard == NULL)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "-10");
		sprintf(errmsg, "��ȡ�ӿ�ʧ��!\r\n%s", path);
		return -10;
	}

	logStr.Format("���ź�������������˿�=%s;\n������=%d;\n���ŵ�����=%s;\n���ŵ�����=%s;", ComPort, iBaudrate, track2, track3);
	runLog(logStr, dir2);

	int iret;
	if(1 == track)
	{
		iret = CT_WriteMsgCard(ComPort, iBaudrate, "", 0, nTimeout, "", track2, track3);
	}
	else if (2 == track)
	{
		iret = CT_WriteMsgCard(ComPort, iBaudrate, "", 0, nTimeout, "", track2, "");
	}
	else if (3 == track)
	{
		iret = CT_WriteMsgCard(ComPort, iBaudrate, "", 0, nTimeout, "", "", track3);
	}

	logStr.Format("���ó��̶��ź�������ֵ��%d", iret);
	runLog(logStr, dir2);

	if(iret != 0)
	{
		FreeLibrary(hDll);
		sprintf(errcd, "%d", iret);
		sprintf(errmsg, "д��ʧ��!\r\n%s", path);
		return -20;
	}

	FreeLibrary(hDll);
	return 0;
}

/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
*********************************************/
BOOL WINAPI GetMagCOM(MAG_INTERFACE *pInterf)
{
	if(pInterf)
	{

		pInterf->WriteCard = WriteCard;
		pInterf->ReadCard = ReadCard;
		return TRUE;
	}
	return FALSE;
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
	logFileName.Format("%s\\%04d%02d%02d.txt", CAMERELOG_DIR,tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday);
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



void MyDeleteFile(CString dirPath)
{
	BOOL bFind = FALSE;
	CFileFind ff;
	CString tmpDirPath;
	if (dirPath.GetAt(dirPath.GetLength()-1) == '\\')
	{
		tmpDirPath = dirPath.Mid(dirPath.GetLength()-1);
	} 
	else
	{
		tmpDirPath = dirPath;
	}
	
	tmpDirPath += "\\*.*";
	bFind = ff.FindFile(tmpDirPath);
	while (bFind)
	{
		bFind = ff.FindNextFile();
		if (!ff.IsDots()&&!ff.IsDirectory())//�ļ�
		{
			CString strFilePath = ff.GetFilePath();
			CString strFileName = ff.GetFileName();
			CString strExtName;
			
			
			if(strFileName.Left(4) == ".bmp")
			{
				DeleteFile(strFileName);
			}
			
			
		}
		
		if(!ff.IsDots() && ff.IsDirectory()) //Ŀ¼
		{
			CString strFilePath = ff.GetFilePath();
			//�ݹ�
			MyDeleteFile(strFilePath);
		}
	}
	
	ff.Close();
}