// dllMag.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllMag.h"
#include "magcreator.h"
#include "driverMag.h"

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
// CDllMagApp

BEGIN_MESSAGE_MAP(CDllMagApp, CWinApp)
	//{{AFX_MSG_MAP(CDllMagApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllMagApp construction

CDllMagApp::CDllMagApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllMagApp object

CDllMagApp theApp;

/********************************************
* ��  �ܣ����豸��OpenDevice/CloseDevice��ɶԵ���
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	//IMagCreator::CoCreatIntance();
	//CDriverMag *pInst = IMagCreator::getIntance();
	
	CDriverMag *pInst = IMagCreator::getIntance();
	if (NULL == pInst)
	{
		IMagCreator::CoCreatIntance();
	    pInst = IMagCreator::getIntance();
	}
		
	//�������ļ��ж�ȡ���������豸����
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);

	CString csAppPath ;
	csAppPath.Format("%s\\magdll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	CString formatPath = csAppPath + "magformat.ptt";
	pInst->SetFormatXmlPath(formatPath);

	return 0;
}


int WINAPI SetTimeOut(int timeout)
{
    CDriverMag *pInst = IMagCreator::getIntance();
	if (NULL == pInst)
	{
		IMagCreator::CoCreatIntance();
		pInst = IMagCreator::getIntance();
	}
	pInst->SetTimeOut(timeout);
	return 0;
}


/********************************************
* ��  �ܣ��ر��豸
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI CloseDevice()
{
	CDriverMag *pInst = IMagCreator::getIntance();
	pInst->CloseMag();
	IMagCreator::ReleaseIntance();
	return 0;
}

int WINAPI Reset(int rw)
{
	CDriverMag *pInst = IMagCreator::getIntance();
	int ret = pInst->ResetMag(rw);
	if(ret != 0)
	{
		pInst->SetErrCode((enRetCode)ret);
		pInst->CloseMag();
		return -1;
	}
	pInst->CloseMag();
	return 0;
}

/********************************************
* ��  �ܣ����豸��д����
* ��  ����
      @indata, ָ�����飬��һ��Ԫ���Ǵ�����ʽ���ƣ��ڶ���Ԫ�شŵ��ţ�����Ԫ����д��������Ҫ�����ݡ�
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CDriverMag *pInst = IMagCreator::getIntance();
	if(conffile == 0)
	{
		pInst->SetErrCode(ERR_NOFINDCONF);
		return ERR_NOFINDCONF;
	}
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    int ret = pInst->OpenICd();
	if(ret != SUCCESS)
	{
        pInst->SetErrCode((enRetCode)ret);
		return ret;
	}

	VPair sub;
	int i;
	CString substr;
	CString itemname;
	CString itemval;
	//debug("magformat:%s,trackno:%s",indata[0],indata[1]);
    for (i = 2; i < size; i ++)
    {
		//debug("��...������:%s",indata[i]);
		substr = indata[i];
        if(substr.IsEmpty())
			continue;
		int ps = substr.Find('=', 0);
        if(ps == -1)
		{
            pInst->SetErrCode(ERR_NOFINDDATA);
			return ERR_NOFINDDATA;
		}
		itemname = substr.Mid(0, ps);
		itemval = substr.Mid(ps + 1, substr.GetLength() - ps - 1);
		pair< CString , CString> item(itemname, itemval);
		sub.push_back(item);
    }

	if(size > 0)
	{
		if(size > 1)
			ret = pInst->WriteICd(indata[0], indata[1], 0, sub);
		else
			ret = pInst->WriteICd(indata[0], "23", 0, sub);
	}
	else
		ret = ERR_OCREXCEPTION;	
	//debug("WriteMag����ֵ:%d",ret);
	if(ret != SUCCESS)
	{
		if(ret == 1)
			return ret;
		pInst->SetErrCode((enRetCode)ret);
		return ret;           //д����ʧ�ܡ�
	}
	pInst->SetTimeOutZero();
	return SUCCESS;
}

/********************************************
* ��  �ܣ����豸�ж�����
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString strRetCode;
	CDriverMag *pInst = IMagCreator::getIntance();
	VPair &sub = pInst->GetMagInfoListRef();
	sub.clear();
	if(conffile == 0)
	{
		(*ret) = ERR_NOFINDCONF;
		pInst->SetErrCode(ERR_NOFINDCONF);
		strRetCode.Format("%d", (*ret));
	    pair< CString, CString> retpair("returncode", strRetCode);
		sub.push_back(retpair);
		(*size) = sub.size();
		return sub;
	}
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    (*ret) = pInst->OpenICd();
	if((*ret) != SUCCESS)
	{
		pInst->SetErrCode(enRetCode(*ret));
		strRetCode.Format("%d", (*ret));
		pair< CString, CString> retpair("returncode", strRetCode);
		sub.push_back(retpair);
		(*size) = sub.size();
		return sub;
	}

	if((*size) > 0)
	{
		if((*size) > 1)
			(*ret) = pInst->ReadICd(indata[0], indata[1], sub);
		else
			(*ret) = pInst->ReadICd(indata[0], "23", sub);
	}
	else
        (*ret) = pInst->ReadICd(NULL, "23", sub);

	if((*ret) != SUCCESS && (*ret) != SUCCESS_RESET)
	{
		pInst->SetErrCode((enRetCode)(*ret));
	}

	strRetCode.Format("%d", (*ret));
	pair< CString, CString> retpair("returncode", strRetCode);
    sub.insert(sub.begin(), retpair);

	pInst->SetTimeOutZero();
	(*size) = sub.size();

	return sub;
}

/********************************************
* ��  �ܣ���ȡ�豸����
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI GetDeviceType()
{
	return YDZDMAG_DEV;
}

/********************************************
* ��  �ܣ���ȡ�豸��������
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("����������д����̬��");
}

/********************************************
* ��  �ܣ�ȡ�õ�ǰ������ϸ����
* ��  ������
* ����ֵ�����ص�ǰ������ϸ�������޴��󷵻�NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	static char errmess[255];
	memset(errmess, 0, 255);
	CDriverMag *pInst = IMagCreator::getIntance();
	if(!pInst)
	{
		sprintf(errmess, "��������������������!");
		return errmess;
	}
	enRetCode err = pInst->GetErrCode();
	switch(err)
	{
	case ERR_LOADCONF:
		sprintf(errmess, "���ش�����д�������ļ�ʧ��!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "�򿪴�����д���豸�˿�ʧ��!");
		break;
	case ERR_READTIMEOUT:
		sprintf(errmess, "��ȡ�豸���ݳ�ʱ!");
		break;
	case ERR_WRITETIMEOUT:
		sprintf(errmess, "д�ų�ʱ!");
		break;
	case ERR_IOPEANDING:
		sprintf(errmess, "�豸��æ!");
		break;
	case ERR_READDATA:
		sprintf(errmess, "��ȡ����ʧ��!");
		break;
	case ERR_SENDDATA:
		sprintf(errmess, "��������ʧ��!");
		break;
	case ERR_WRITETRACK:
		sprintf(errmess, "д�ŵ�����ʧ��!");
		break;
	case ERR_MISSORDER:
		sprintf(errmess, "��ʧָ��!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "��Ч��Ӳ��ָ��!");
		break;
	case ERR_READTRACK:
		sprintf(errmess, "���ŵ�����ʧ��!");
		break;
	case ERR_LOADCONFIGS:
		sprintf(errmess, "������������ļ�ʧ��!");
		break;
	case ERR_OCREXCEPTION:
		sprintf(errmess, "�������쳣�����ò���ȷ���ڴ��ַ!");
		break;
	case ERR_NOFINDDATA:
		sprintf(errmess, "ȱʧ���д������������!");
		break;
	case ERR_NOFINDCONF:
		sprintf(errmess, "δ���ҵ����Ӳ���豸����������ļ�!");
		break;
	case ERR_NOWRITEDATA:
		sprintf(errmess, "ȱ�ٶ����ŵ�����!");
		break;
	case ERR_OTHER_BANK_CARD_ABSCENT_DATA:
		sprintf(errmess, "��ˢ���п��ŵ�����ȱʧ!");
		break;
	case ERR_PASSBK_OLD2_ABSCENT_DATA:
		sprintf(errmess, "��ˢ����PASSBK_OLD2�ŵ�����ȱʧ!");
		break;
	case ERR_PASSBK_OLD1_ABSCENT_DATA:
		sprintf(errmess, "��ˢ����PASSBK_OLD1�ŵ�����ȱʧ!");
		break;
	case ERR_PASSBK_NEW_ABSCENT_DATA:
		sprintf(errmess, "��ˢ����PASSBK_NEW�ŵ�����ȱʧ!");
		break;
	case ERR_CARD_ABSCENT_DATA:
		sprintf(errmess, "��ˢ�����ŵ�����ȱʧ!");
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "û���ҵ�������dll!");
		break;
	case ERR_READFAIL:
		sprintf(errmess, "��ȡ�ŵ���Ϣʧ��!");
		break;
	}
	return errmess;
}

/********************************************
* ��  �ܣ��汾���ݿ��ƣ����ز���ӿڰ汾�ţ��жϱ������ǰ�汾�Ƿ����
* ��  ����pFirst���汾�ţ�pSecond�ΰ汾�ţ�pFirst����ͬ��ز����ݣ���pFirst��ͬ��pSecond���������򲻼��ݣ���֮����
* ����ֵ���ɹ�����0,�����0
*********************************************/
long WINAPI GetPracticably(long *pFirst,long *pSecond)
{
	return 0;
}

/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
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



void debug(CString s)
{
//#ifdef _DEBUG
#ifdef _LOG
	CDriverMag *pInst = IMagCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);
	
	FILE* m_stream = fopen("err.log", "a+");
	if (m_stream == NULL)
	{
		ASSERT(1==0);
	}
	
	time_t t;
	time(&t);
	tm *ptm = localtime(&t);
	fprintf(m_stream,"[%04d-%02d-%02d %02d:%02d:%02d]%s\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,s);
	fclose(m_stream);
	
	SetCurrentDirectory(tmpdir);
#endif
}

void debug(const char* s,...)
{
//#ifdef _DEBUG
#ifdef _LOG
	CDriverMag *pInst = IMagCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);
	
	FILE* m_stream = fopen("err.log", "a+");
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
	fprintf(m_stream,"[%04d-%02d-%02d %02d:%02d:%02d]%s\n",ptm->tm_year+1900,ptm->tm_mon+1,ptm->tm_mday,ptm->tm_hour,ptm->tm_min,ptm->tm_sec,msg);
	fclose(m_stream);
	
	SetCurrentDirectory(tmpdir);
#endif
}