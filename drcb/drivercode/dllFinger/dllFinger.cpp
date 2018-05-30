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
* ��  �ܣ����豸��OpenDevice/CloseDevice��ɶԵ���
* ��  ����
* ����ֵ���ɹ�����0,�����0
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
	
	//�������ļ��ж�ȡ���������豸����
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
* ��  �ܣ��ر��豸
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI CloseDevice()
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
	pInst->CloseFp();
	IFingerCreator::ReleaseIntance();
	return 0;
}

/********************************************
* ��  �ܣ���λ
* ��  ����@rw - ��д���͡�1-����2-д
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI Reset(int rw)
{
	CDriverFinger *pInst = IFingerCreator::getIntance();
    int ret = pInst->ResetFp(rw);
	if(ret != SUCCESS)
	{
		pInst->CloseFp();
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //��λʧ��		
	}
	pInst->CloseFp();
	return 0;
}

/********************************************
    * ��  �ܣ������豸������ʱʱ��
    * ��  ����@timeout - �豸������ʱʱ�䡣��λΪ��
    * ����ֵ���ɹ�����0,�����0
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
* ��  �ܣ����豸��д����
* ��  ����
      @indata, ָ������
* ����ֵ���ɹ�����0,�����0
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
		return -1;  //�򿪴�����ʧ��
	}

	ret = pInst->WriteFp();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode((enRetCode)ret);
		return -2;           //д����ʧ�ܡ�
	}
	return 0;
}

/********************************************
* ��  �ܣ����豸�ж�����
* ��  ����
* ����ֵ���ɹ�����0,�����0
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
		return sub;  //�򿪴�����ʧ��
	}

    short optype ;   // 0 - �ɼ�ָ��ģ�� 1 - �ɼ�ָ������ 2 - ָ�Ʊȶ�
	if((*size) > 0)
		optype = atoi(indata[0]);
	else
		optype = GETFINGERDAT_OP;
	
	VPair outlist;
    
	if (optype == GETFINGERDAT_OP)
	{
		//��ݸũ������ʱֻ�иò�������
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
* ��  �ܣ���ȡ�豸����
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI GetDeviceType()
{
	return FINGERPRINT_DEV;
}

/********************************************
* ��  �ܣ���ȡ�豸��������
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("����ָ���Ƕ�̬��");
}

/********************************************
* ��  �ܣ�ȡ�õ�ǰ������ϸ����
* ��  ������
* ����ֵ�����ص�ǰ������ϸ�������޴��󷵻�NULL
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
		sprintf(errmess, "û���ҵ�ָ�������������ļ�!");
		break;
	case ERR_LOADCONF:
		sprintf(errmess, "����ָ���������ļ�ʧ��!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "��ָ���Ƕ˿�ʧ��!");
		break;
	case ERR_READTIMEOUT:
		sprintf(errmess, "��ȡ�豸���ݳ�ʱ!");
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
	case ERR_DEVRESPONSE:
		sprintf(errmess, "�����豸�д��󣬾���ԭ��:%s", pInst->GetStatusDesc());
		break;
	case ERR_WAITTIMEOUT:
		sprintf(errmess, "�ȴ�����ָָ�Ƴ�ʱ!");
		break;
	case ERR_DEVSENDDATA:
		sprintf(errmess, "ָ�����豸������������!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "��Ч��Ӳ��ָ��!");
		break;
	case ERR_FPCHECKFAIL:
		sprintf(errmess, "ǩ����Աָ����֤ʧ��!\r\n�������: %s\r\n������Ϣ:%s",pInst->GetFpErrCd(), pInst->GetFpErrMsg());
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "δ�ҵ�ָ��У��dll!");
		break;
	case ERR_NOFINDTHIRDCALL:
		sprintf(errmess, "δ�ҵ�������!");
		break;
	case ERR_READFAIL:
		sprintf(errmess, "ָ�Ʋ���ʧ��!\r\n�������: %s\r\n������Ϣ:%s",pInst->GetFpErrCd(), pInst->GetFpErrMsg());
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
