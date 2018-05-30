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
* ��  �ܣ����豸��OpenDevice/CloseDevice��ɶԵ���
* ��  ����
* ����ֵ���ɹ�����0,�����0
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
	
	//�������ļ��ж�ȡ���������豸����
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
* ��  �ܣ��ر��豸
* ��  ����
* ����ֵ���ɹ�����0,�����0
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
* ��  �ܣ���λ
* ��  ����@rw - ��д���͡�1-����2-д
* ����ֵ���ɹ�����0,�����0
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
* ��  �ܣ���ȡ�豸��������
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("������̨IC����̬��");
}

/********************************************
* ��  �ܣ���ȡ�豸����
* ��  ����
* ����ֵ���ɹ�����0,�����0
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
	CString &dir = pInst->GetModuleDir(); //������ʹdirָ��_moduledir��Ա����
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
		return sub;  //�򿪴�����ʧ��
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
		//������Ϣ
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
	    //pInst->RealseDriver();   //����F55��ʱҪ�õ�������Ϣʱ�����һЩ���ݣ���˴��������ͷš�
		
	}
    else if (2 == optype)
    {
		//����ȡ����Ϣ��ARQC
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
		//ȡ������ϸ��Ϣ
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
		//дARPC
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
		//������Ϣ
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
		//����ARQC
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
		//�µ�
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
* ��  �ܣ�ȡ�õ�ǰ������ϸ����
* ��  ������
* ����ֵ�����ص�ǰ������ϸ�������޴��󷵻�NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	//CDriverICCard *pInst = ICCardCreator::getIntance();
	static char errmess[BUFSIZ];
	CDriverICCard *pInst = CDriverICCard::GetInstance();
	if (!pInst)
	{
		sprintf(errmess, "��IC����������!");
		return errmess;
	}

	
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	
	switch(err)
	{
		case ERR_LOADCONF:
			sprintf(errmess, "����IC����д�������ļ�ʧ��!");
			break;
		case ERR_OPENPORT:
			sprintf(errmess, "��IC����д���豸�˿�ʧ��!");
			break;
		case ERR_CLOSEPORT:
			sprintf(errmess, "�رն˿�ʧ��!");
			break;
		case ERR_RELEASE:
			sprintf(errmess, "�ͷŶ�̬�⼰�رն˿�ʧ��!");
			break;
		case ERR_READCARDINFO:
			sprintf(errmess, "������Ϣʧ��!");
			break;
		case ERR_PREUPDATE:
			sprintf(errmess, "Ԥ����ʧ��!");
			break;
		case ERR_UPDATEICCARD:
			sprintf(errmess, "���¿���Ϣʧ��!");
			break;
		case ERR_ONLINEFAILED:
			sprintf(errmess, "����ʧ�ܴ���ʧ��!");
			break;
		case ERR_NOFINDCONF:
			sprintf(errmess, "û�ҵ������ļ�!");
			break;
		case ERR_LOADDRV:
			sprintf(errmess, "���ض�̬��ʧ��!");
			break;
		case ERR_NODRV:
			sprintf(errmess, "�޶�̬��!");
			break;
		case ERR_NOFINDMATCHDLL:
			sprintf(errmess, "û���ҵ�����������!");
			break;
		case ERR_ABCENTDATA:
			sprintf(errmess, "����ȱʧ!");
			break;
		case ERR_GENARQC:
			sprintf(errmess, "����ARQCʧ��!");
			break;
		case ERR_CTLSCRIPTDATA:
			sprintf(errmess, "дARPCʧ��!");
			break;
		case ERR_GETTRANDETAIL:
			sprintf(errmess, "��ȡ������ϸ��Ϣʧ��!");
			break;
		case ERR_GETICINFOANDGENARQC:
			sprintf(errmess, "����ȡ����Ϣ��ARQCʧ��!");
			break;
		case ERR_POWEROFF:
			sprintf(errmess, "�µ�ʧ��!");
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