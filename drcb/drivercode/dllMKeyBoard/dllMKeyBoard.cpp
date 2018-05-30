// dllMKeyBoard.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllMKeyBoard.h"

#include "DriverMKeyBoard.h"
#include "keyboardcreator.h"

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
// CDllMKeyBoardApp

BEGIN_MESSAGE_MAP(CDllMKeyBoardApp, CWinApp)
	//{{AFX_MSG_MAP(CDllMKeyBoardApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllMKeyBoardApp construction

CDllMKeyBoardApp::CDllMKeyBoardApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllMKeyBoardApp object

CDllMKeyBoardApp theApp;

/********************************************
* ��  �ܣ����豸��OpenDevice/CloseDevice��ɶԵ���
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if (NULL == pInst)
	{
		IMKeyBdCreator::CoCreatIntance();
		pInst = IMKeyBdCreator::getIntance();
	}
	//IMKeyBdCreator::CoCreatIntance();
	//CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	

	CString csAppPath ;
	csAppPath.Format("%s\\keyboarddll\\", modulepath);
	pInst->SetModuleDir(csAppPath);

	return 0;
}


int WINAPI SetTimeOut(int timeout)
{
    CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if (NULL == pInst)
	{
		IMKeyBdCreator::CoCreatIntance();
		pInst = IMKeyBdCreator::getIntance();
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
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	pInst->CloseMKeyBd();
	IMKeyBdCreator::ReleaseIntance();
	return 0;
}

/********************************************
* ��  �ܣ���λ
* ��  ����@rw - ��д���͡�1-����2-д
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI Reset(int rw)
{
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
    int ret = pInst->ResetMKeyBd(rw);
	if(ret != SUCCESS)
	{
		pInst->CloseMKeyBd();
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //��λʧ��		
	}
	pInst->CloseMKeyBd();
	return 0;
}

/********************************************
* ��  �ܣ����豸��д����
* ��  ����
      @indata, ָ������
* ����ֵ���ɹ�����0,�����0
-----------------------
д������Կ�������:
0:����Կ�������� ��1 - ���Ĺ�עTMK; 2 - ���Ĺ�עTMK; 3 - ���Ĺ�עTPK.���Ϸ�ʹ���ֹ���TMK,�������Ϸ������3 ��Ԫ�ز���ʡ
1:����Կ������ //�趨Ϊ0����Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
2:������Կ������ //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
3������Կ���� //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
4��������Կ���� //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
5����ԿУ�鴮��//���û��У�鴮����Ԫ��Ϊ��0���ַ�����
*********************************************/
int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	debug("write");
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	if(conffile == 0)
		return ERR_NOFINDCONF;
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    int ret = pInst->OpenMKeyBd();
	if(ret != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)ret );
		return -1;  //�򿪴�����ʧ��
	}

	int OPType = atoi(indata[0]);  //��ע��Կ�������͡�1 - ���Ĺ�ע����Կ; 2 - ���Ĺ�ע����Կ; 3 - ���Ĺ�ע������Կ.
	//TPCHR lpOldKey = NULL;
	//TPCHR lpKeyCK = NULL;
	//if(strcmp(indata[3], "0") != 0)
		//lpOldKey = indata[3];
	//if(strcmp(indata[5], "0") != 0)
		//lpKeyCK = indata[5];
	
	debug("%d;%s;%s;%s;%s;%s",OPType, indata[1], indata[2], indata[3], indata[4], indata[5]);
//	debug("write");
	 ret = pInst->WriteMKeyBd(OPType, indata[1], indata[2], indata[3], indata[4], indata[5]);
	if(ret != SUCCESS)
	{
		pInst->SetTimeOutZero();
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
-----------------------------
��Pin���Ĵ����������:
0:�������� 0��������,1:���������кţ�2����ȡ����ֵ��3����ȡ�豸�ţ�4��д����������Կ��5��д����������Կ��6��д�빤����Կ
1:����ģʽ(1-����������,2 - ��ʾ��������ʾ��)
2:���볤��
3:����Կ������ //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
4:������Կ������ //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
5:�ʺ� //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��
6:ҵ������ //��Ԫ�ز���ʡ��û���ⲿ�����Ϊ��0��

*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString tmpLogStr;
	CString strRetCode;
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
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
	CString &dir = pInst->GetModuleDir(); //������ʹdirָ��_moduledir��Ա����
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);
	
    (*ret) = pInst->OpenMKeyBd();
	debug("OpenMKeyBd");
	if((*ret) != SUCCESS)
	{
		pInst->SetErrCode( (enRetCode)(*ret) );
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //�򿪴�����ʧ��
	}

	int readType = atoi(indata[0]);
	debug("readType:%d",readType);
	if(readType == 0)
	{
		CString pwd;
		CString TPKCK;
		int minLen = atoi(indata[2]);
		int maxLen = atoi(indata[3]);
		//int BKType = atoi(indata[0]); //
		
		
		(*ret) = pInst->ReadPin(atoi(indata[1]),&minLen ,&maxLen, indata[4], "", indata[5], pwd, TPKCK, indata[6]);
		
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("PASS_WORD=%s",pwd);
			pair< CString, CString > passwd("password", msg);

			sub.push_back(retcode);
			sub.push_back(passwd);
		}
	}
	else if(readType == 1)
	{
		char result[21] = {0};
		(*ret) = pInst->GetPinPadID(result);
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("PinPadID=%s",result);
			pair< CString, CString > passwd("PinPadID", msg);
			sub.push_back(retcode);
			sub.push_back(passwd);
			
		}
	}
	else if (readType == 2)
	{
		int evaluateVal = 0;
		*ret = pInst->GetCustomEvaluate(&evaluateVal);
		if( (*ret) != SUCCESS)
		{
			pInst->SetErrCode( (enRetCode)(*ret));
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
		}
		else
		{
			CString msg;
			msg.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", msg);
			
			msg.Format("EVALUATE_VALUE=%d",evaluateVal);
			pair< CString, CString > passwd("EVALUATE_VALUE", msg);
			sub.push_back(retcode);
			sub.push_back(passwd);
			
		}
	}
	else 
	{
		VPair outlist;
		if (readType == 3)
		{
			// ��ȡ������̵����к�
		}
		else if (readType == 4)
		{
			// д�빫Կ
			if ((*size) < 3)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// �㷨��ʶ
			PAIR ranKeyLen("ranKeyLen", indata[1]);
			outlist.push_back(ranKeyLen);
			
			// ��Կ
			PAIR PK("PK", indata[2]);
			outlist.push_back(PK);
		}
		else if (readType == 5)
		{
			// д������Կ
			if ((*size) < 4)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// ����Կ����
			PAIR keyIndex("keyIndex", indata[1]);
			outlist.push_back(keyIndex);

			// �㷨��ʶ
			PAIR newKeyLen("newKeyLen", indata[2]);
			outlist.push_back(newKeyLen);

			// ����Կ
			PAIR newKeyBySM4Key("newKeyBySM4Key", indata[3]);
			outlist.push_back(newKeyBySM4Key);

		}
		else if (readType == 6)
		{
			// д�빤����Կ
			if ((*size) < 5)
			{
				(*ret) = ERR_ARGSERROR;
				strRetCode.Format("%d", (*ret));
				pair< CString, CString > retcode("returncode", strRetCode);
				sub.push_back(retcode);
				return sub;
			}

			// ����Կ����
			PAIR mainkeyIndex("mainkeyIndex", indata[1]);
			outlist.push_back(mainkeyIndex);

			// ������Կ����
			PAIR workkeyIndex("workkeyIndex", indata[2]);
			outlist.push_back(workkeyIndex);

			// �㷨��ʶ
			PAIR newKeyLen("newKeyLen", indata[3]);
			outlist.push_back(newKeyLen);

			// ������Կ
			PAIR newKeyBySM4Key("newKeyBySM4Key", indata[4]);
			outlist.push_back(newKeyBySM4Key);
		}
		(*ret) = pInst->driveKB(readType, outlist);
		sub.clear();
		sub = outlist;
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
	return PASSWORDKEY_DEV;
}

/********************************************
* ��  �ܣ���ȡ�豸��������
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
TCHAR * WINAPI GetDeviceDesc()
{
	return _T("������̨������̶�̬��");
}

/********************************************
* ��  �ܣ�ȡ�õ�ǰ������ϸ����
* ��  ������
* ����ֵ�����ص�ǰ������ϸ�������޴��󷵻�NULL
*********************************************/
PTCHAR WINAPI GetErrorDesc()
{
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	static char errmess[BUFSIZ];
	memset(errmess, 0, sizeof(errmess));
	enRetCode err = pInst->GetErrCode();
	switch(err)
	{
	case ERR_LOADCONF:
		sprintf(errmess, "��������С���������ļ�ʧ��!");
		break;
	case ERR_OPENPORT:
		sprintf(errmess, "������С�����豸�˿�ʧ��!");
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
	case ERR_LENNOTENOUGH:
		sprintf(errmess, "���볤�Ȳ���ȷ!");
		break;
	case ERR_TWICEPASSWORD:
		sprintf(errmess, "������������벻���!");
		break;
	case ERR_INVALIDORDER:
		sprintf(errmess, "��Ч��Ӳ��ָ��!");
		break;
	case ERR_NOFINDMATCHDLL:
		sprintf(errmess, "û���ҵ�����������!");
		break;
	case ERR_LOADDRV:
		sprintf(errmess, "����������ʧ��!");
		break;
	case ERR_READPINFAIL:
		sprintf(errmess, "��ȡPIN����ʧ��!");
		break;
	case ERR_WRITEMWKEY_FAIL:
		sprintf(errmess, "��ע��Կʧ��!");
		break;	
	default:
		sprintf(errmess,pInst->GetErrMsg());
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
	if (pDrivInterf == NULL)
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
#ifdef _DEBUG
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);

	 FILE* m_stream = fopen("drv.log", "a+");
	if (m_stream == NULL)
	{
		ASSERT(1==0);
	}

	time_t t;
	time(&t);
	tm *ptm = localtime(&t);
	fprintf(m_stream,"[%02d:%02d:%02d][%s]%s\n",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,FL,s);
	fclose(m_stream);
	
	SetCurrentDirectory(tmpdir);
#endif
}

void debug(const char* s,...)
{
#ifdef _DEBUG
	CDriverMKeyBd *pInst = IMKeyBdCreator::getIntance();
	CString dir = pInst->GetModuleDir();
	char tmpdir[1024] = {0};
	GetCurrentDirectory(1024,tmpdir);
	SetCurrentDirectory(dir);

	FILE* m_stream = fopen("drv.log", "a+");
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
	fprintf(m_stream,"[%02d:%02d:%02d][%s]%s\n",ptm->tm_hour,ptm->tm_min,ptm->tm_sec,FL,msg);
	fclose(m_stream);

	SetCurrentDirectory(tmpdir);
#endif
}
