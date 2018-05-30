// dllGMT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"
#include "gmtcreator.h"

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
// CDllGMTApp

BEGIN_MESSAGE_MAP(CDllGMTApp, CWinApp)
	//{{AFX_MSG_MAP(CDllGMTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllGMTApp construction

CDllGMTApp::CDllGMTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDllGMTApp object

CDllGMTApp theApp;


PTCHAR WINAPI GetGmtErrmsg(int iRet)
{
	static char errmsg[100]={0};
	switch(iRet)
	{
	case 0:
		sprintf(errmsg,"�豸��Ӧ��");
		break;
	case -2:
		sprintf(errmsg,"�豸δ����");
		break;
	case -3:
		sprintf(errmsg,"��������");
		break;
	}

	return errmsg;
}


/********************************************
* ��  �ܣ����豸��OpenDevice/CloseDevice��ɶԵ���
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI OpenDevice(const char *modulepath)
{
	CGmtDriver *pInst = IGMTCreator::getIntance();
	CString tmpstr;
	if (NULL == pInst)
	{
		IGMTCreator::CoCreatIntance();
	    pInst = IGMTCreator::getIntance();
	}
	
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);
	
	CString csAppPath ;
	csAppPath.Format("%s\\keyboarddll\\", modulepath);
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
	CGmtDriver *pInst = IGMTCreator::getIntance();
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
	CGmtDriver *pInst = IGMTCreator::getIntance();
	if (!pInst)
	{
		pInst->RealseDriver();
		IGMTCreator::ReleaseIntance();
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
	return _T("����������");
}

/********************************************
* ��  �ܣ���ȡ�豸����
* ��  ����
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI GetDeviceType()
{
	return GMT_DEV;
}

/********************************************
* ��  �ܣ�����������
* ��  ����
* ����ֵ���ɹ�����0�������0
*********************************************/
VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString logstr;//��־��ʽ�ַ���
	CString strRetCode;
	
	CGmtDriver *pInst = IGMTCreator::getIntance();
	VPair &sub = pInst->GetRetListRef();
	sub.clear();
	VPair outlist;

	if(conffile == 0)
	{
		(*ret) = ERR_NOFINDCONF;
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;
	}
	CString &dir = pInst->GetModuleDir();
	CString confpath = dir + conffile;
	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(portno);

    (*ret) = pInst->OpenGmtDriver();
	if((*ret) != SUCCESS)
	{
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //�򿪴�����ʧ��
	}
	int result;
	result = pInst->LoadDriver();

	if(result != SUCCESS)
	{
		logstr.Format("��������ʧ��!ret=%d", result);
		CGmtDriver::runLog(logstr, dir);
		pair< CString, CString > retcode("returncode", "1");
		sub.push_back(retcode);
		return sub;
	}
	if (!pInst->IsDriverLoaded())
	{	
		CGmtDriver::runLog("����δ����!", dir);
		pair< CString, CString > retcode("returncode", "1");
		sub.push_back(retcode);
		return sub;
	}

	int type = atoi(indata[0]);
	if (1 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_ClosePinPad--ȡ������״̬(�ر��������)", dir);
		outlist.clear();
	}
	else if (2 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_SetCodeLen--�������볤��", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iPinLen=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);
		// ���볤��
		PAIR iPinLen("iPinLen", indata[1]);
		outlist.push_back(iPinLen);
	}
	else if (3 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_NonEpt_ReadPin--��ȡ�Ǽ�������", dir);
		outlist.clear();
		
		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iVoiceType=%s��iTimeOut=%s��iPinSize=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);
		
		// ������ʾ����
		PAIR iVoiceType("iVoiceType", indata[1]);
		outlist.push_back(iVoiceType);
		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[2]);
		outlist.push_back(timeout);
		//��������С
		PAIR iPinSize("iPinSize", indata[3]);
		outlist.push_back(iPinSize);
	}
	else if (4 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_InitPinPad--��ʼ���������", dir);
		outlist.clear();
	}
	else if (5 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_UpdateMKey--��������Կ", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--ZmkIndex=%s, ZmkLength=%s, Zmk=%s, CheckValue=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// ��Կ������
		PAIR ZmkIndex("ZmkIndex", indata[1]);
		outlist.push_back(ZmkIndex);
		// ��Կ����
		PAIR ZmkLength("ZmkLength", indata[2]);
		outlist.push_back(ZmkLength);
		// ��Կ����
		PAIR Zmk("Zmk", indata[3]);
		outlist.push_back(Zmk);
		// ��Կ����ֵ
		PAIR CheckValue("CheckValue", indata[4]);
		outlist.push_back(CheckValue);
	}
	else if (6 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_DownLoadWKey--���ع�����Կ", dir);
		outlist.clear();

		if ((*size) < 6)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--MKeyIndex=%s, WKeyIndex=%s, WKeyLength=%s, Key=%s, CheckValue=%s", indata[1], indata[2], indata[3], indata[4], indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// ����Կ������
		PAIR MKeyIndex("MKeyIndex", indata[1]);
		outlist.push_back(MKeyIndex);
		// ������Կ������
		PAIR WKeyIndex("WKeyIndex", indata[2]);
		outlist.push_back(WKeyIndex);
		// ������Կ����
		PAIR WKeyLength("WKeyLength", indata[3]);
		outlist.push_back(WKeyLength);
		// ��Կ����
		PAIR Key("Key", indata[4]);
		outlist.push_back(Key);
		// ��Կ����ֵ
		PAIR CheckValue("CheckValue", indata[5]);
		outlist.push_back(CheckValue);
	}
	else if (7 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_ActiveWKey--�������Կ", dir);
		outlist.clear();

		if ((*size) < 3)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--MKeyIndex=%s, WKeyIndex=%s", indata[1], indata[2]);
		CGmtDriver::runLog(logstr, dir);

		// ����Կ������
		PAIR MKeyIndex("MKeyIndex", indata[1]);
		outlist.push_back(MKeyIndex);
		// ������Կ������
		PAIR WKeyIndex("WKeyIndex", indata[2]);
		outlist.push_back(WKeyIndex);
	}
	else if (8 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_GetDES--��ȡ�ͻ�����(DES)", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iTimeOut=%s, VoiceType=%s, EndType=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// ������ʾ����
		PAIR VoiceType("VoiceType", indata[2]);
		outlist.push_back(VoiceType);
		// ��������Ľ�����ʽ
		PAIR EndType("EndType", indata[3]);
		outlist.push_back(EndType);
	}
	else if (9 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_GetPin--��ȡ�ͻ�����(ANSI98)", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iTimeOut=%s, AccNo=%s, VoiceType=%s, EndType=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// �˺�
		PAIR AccNo("AccNo", indata[2]);
		outlist.push_back(AccNo);
		// ������ʾ����
		PAIR VoiceType("VoiceType", indata[3]);
		outlist.push_back(VoiceType);
		// ��������Ľ�����ʽ
		PAIR EndType("EndType", indata[4]);
		outlist.push_back(EndType);
	}
	else if (10 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_StartInfoHtml--������Ϣ����(html)", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--timeout=%s, modex=%s, info=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);
		
		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// ��������
		PAIR modex("modex", indata[2]);
		outlist.push_back(modex);
		// ��ʾ������Ϣ
		PAIR info("info", indata[3]);
		outlist.push_back(info);
	}
	else if (11 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_WriteDeviceId--д���豸��", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--pDeviceId=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);
		
		// �豸��
		PAIR deviceId("deviceId", indata[1]);
		outlist.push_back(deviceId);
	}
	else if (12 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_ReadDeviceId--��ȡ�豸��", dir);
		outlist.clear();
	}
	else if (13 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_StartKeyboard--�������ּ���", dir);
		outlist.clear();

		if ((*size) < 3)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iTimeOut=%s, numType=%s", indata[1], indata[2]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// ���ּ�������
		PAIR numType("numType", indata[2]);
		outlist.push_back(numType);
	}
	else if (14 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_StartElectronicCard--�������ӹ���", dir);
		outlist.clear();

		if ((*size) < 6)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--tellerName=%s, tellerNo=%s, nStarLevel=%s, headFile=%s, iTimeout=%s", indata[1], indata[2], indata[3], indata[4], indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// ��Ա����
		PAIR tellerName("tellerName", indata[1]);
		outlist.push_back(tellerName);
		// ��Ա��
		PAIR tellerNo("tellerNo", indata[2]);
		outlist.push_back(tellerNo);
		// �Ǽ�
		PAIR nStarLevel("nStarLevel", indata[3]);
		outlist.push_back(nStarLevel);
		// ͼƬ����
		PAIR headFile("headFile", indata[4]);
		outlist.push_back(headFile);
		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[5]);
		outlist.push_back(timeout);
	}
	else if (15 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_DownHeadFile--���ع�Աͷ��", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--pFilePath=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// ��Աͷ������·��
		PAIR pFilePath("pFilePath", indata[1]);
		outlist.push_back(pFilePath);
	}
	else if (16 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_FindHeadPhoto--��ѯ��Աͷ��", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--pHeadPhoto=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// ��Աͷ������
		PAIR pHeadPhoto("pHeadPhoto", indata[1]);
		outlist.push_back(pHeadPhoto);
	}
	else if (17 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_DelHeadFile--ɾ����Աͷ��", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--pFilename=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// ��Աͷ���ļ�����
		PAIR pFilename("pFilename", indata[1]);
		outlist.push_back(pFilename);
	}
	else if (18 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_Welcome--���Ż�ӭ����", dir);
		outlist.clear();
	}
	else if (19 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_SetEvaluateInfo--���ù�Ա��Ϣ", dir);
		outlist.clear();

		if ((*size) < 5)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--tellerName=%s, tellerNo=%s, nStarLevel=%s, headFile=%s", indata[1], indata[2], indata[3], indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// ��Ա����
		PAIR tellerName("tellerName", indata[1]);
		outlist.push_back(tellerName);
		// ��Ա��
		PAIR tellerNo("tellerNo", indata[2]);
		outlist.push_back(tellerNo);
		// �Ǽ�
		PAIR nStarLevel("nStarLevel", indata[3]);
		outlist.push_back(nStarLevel);
		// ͼƬ����
		PAIR headFile("headFile", indata[4]);
		outlist.push_back(headFile);
	}
	else if (20 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_StartEvaluate--��������", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iTimeOut=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
	}
	else if (21 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_CancleEvaluate--��������", dir);
		outlist.clear();
	}
	else if (30 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_CallShowPDF--չʾPDF�ļ��ӿ�", dir);
		outlist.clear();

		if ((*size) < 7)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--timeout=%s, signMode=%s, pdfPath=%s, location=%s, imgPath=%s, textPath=%s", indata[1], indata[2], indata[3], indata[4], indata[5], indata[6]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);
		// ǩ��ģʽ
		PAIR signMode("signMode", indata[2]);
		outlist.push_back(signMode);
		// pdf����ƾ֤����·��
		PAIR pdfPath("pdfPath", indata[3]);
		outlist.push_back(pdfPath);
		// ǩ��ͼƬҪ��ʾ������λ��ǩ��������
		PAIR location("location", indata[4]);
		outlist.push_back(location);
		// ǩ��ͼƬ����·��
		PAIR imgPath("imgPath", indata[5]);
		outlist.push_back(imgPath);
		// ǩ������txt�ļ�·��
		PAIR textPath("textPath", indata[6]);
		outlist.push_back(textPath);
	}
	else if (31 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_SetMD--���ü���ģʽ", dir);
		outlist.clear();

		if ((*size) < 2)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--mformat=%s", indata[1]);
		CGmtDriver::runLog(logstr, dir);

		// ��������
		PAIR mformat("mformat", indata[1]);
		outlist.push_back(mformat);
	}
	else if (32 == type)
	{
		CGmtDriver::runLog("��ʼ����--GWQ_GetRandomZMK--�����������Կ����", dir);
		outlist.clear();

		if ((*size) < 4)
		{
			(*ret) = ERR_ARGSERROR;
			strRetCode.Format("%d", (*ret));
			pair< CString, CString > retcode("returncode", strRetCode);
			sub.push_back(retcode);
			return sub;
		}

		logstr.Format("�������--iTimeOut=%s, PubKey=%s, SuanfaType=%s", indata[1], indata[2], indata[3]);
		CGmtDriver::runLog(logstr, dir);

		// ��ʱʱ��
		PAIR timeout("iTimeOut", indata[1]);
		outlist.push_back(timeout);

		// ����ƽ̨�ṩ�Ĺ�Կ
		PAIR PubKey("PubKey", indata[2]);
		outlist.push_back(PubKey);

		// �㷨����
		PAIR SuanfaType("SuanfaType", indata[3]);
		outlist.push_back(SuanfaType);
	}

	int iRet = pInst->driveGmt(type, outlist);

	CGmtDriver::runLog("��������\n", dir);

	sub = outlist;
	(*size) = sub.size();
	//pInst->RealseDriver();

	return sub;
}

int WINAPI SetTimeOut(int timeout)
{
	return 0;
}

int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CGmtDriver *pInst = IGMTCreator::getIntance();
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
	static char errmess[BUFSIZ];	
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
	//pDrivInterf->SetTimeOut = SetTimeOut;
	
	return TRUE;
}


