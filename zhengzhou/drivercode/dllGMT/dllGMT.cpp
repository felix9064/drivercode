// dllGMT.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "dllGMT.h"
#include "GmtDriver.h"

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


int MyCallback(char* message)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	
	CString &dir = pInst->GetModuleDir(); //������ʹdirָ��_moduledir��Ա����
	VPair &sub = pInst->GetRetListRef();
	CString logStr;
	logStr.Format("�ص��������ÿ�ʼ��sub.size()=%d, ����message=%s", sub.size(), message);
	CGmtDriver::runLog(logStr, dir);

	// ���ڴ˴���message��utf-8���룬����Ҫת�����Σ���һ��ת��ʱ�Ĳ���������CP_UTF8
	// ��ȡת��Ϊ���ֽں���Ҫ�Ļ�������С���������ֽڻ�����
	UINT nLen = MultiByteToWideChar(CP_UTF8, NULL, message, -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen+1];
	nLen = MultiByteToWideChar(CP_UTF8, NULL, message, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;

	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen+1];
	nLen = WideCharToMultiByte(936, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;
	
	logStr.Format("�����Ĳ���message=%s", szBuffer);
	CGmtDriver::runLog(logStr, dir);

	pInst->readJson(szBuffer, sub);
	logStr.Format("�ص��������ý�����sub.size()=%d��result=%s", sub.size(), sub[sub.size()-1].second);
	CGmtDriver::runLog(logStr, dir);
	
	delete []wszBuffer;
	delete []szBuffer;

	return 0;
}

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
	CGmtDriver *pInst = CGmtDriver::GetInstance();
	if (!pInst)
	{
		return -1;
	}
	
	CString devicena;
	CString driverna;
	TCHAR temp[255];
	memset(temp, 0, 255);
	
	CString csAppPath ;
	csAppPath.Format("%s\\gmtdll\\", modulepath);
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

	CGmtDriver *pInst = CGmtDriver::GetInstance();
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
	CGmtDriver *pInst = CGmtDriver::GetInstance();
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

VPair &WINAPI Read(char **indata, int *size, char *portno, char *conffile, int *ret)
{
	CString logstr;//��־��ʽ�ַ���
	
	int type = atoi(indata[0]);
	
	int port = atoi(portno);
	CString _partno;
	if (port == 0)
	{
		_partno.Format("USB0");
	}
	else
	{
		_partno.Format("COM%d", port);
	}

	CString strRetCode;
	CGmtDriver *pInst = CGmtDriver::GetInstance();

	CString &dir = pInst->GetModuleDir(); //������ʹdirָ��_moduledir��Ա����
	VPair &sub = pInst->GetRetListRef();
    sub.clear();

	// ���û�д������ļ�������Ĭ�ϵ���������
	CString confpath;
	if (conffile == 0)
	{
		confpath.Format("%sNanTianGmt.xml", dir);
	}
	else
	{
		confpath.Format("%s%s", dir, conffile);
	}
	
	logstr.Format("��ʼ���ã������ļ���%s", confpath);
	CGmtDriver::runLog(logstr, dir);

	pInst->SetConfXmlPath(confpath);
	pInst->SetPortNo(_partno);
    
	(*ret) = pInst->OpenICd();
	if((*ret) != SUCCESS)
	{
		CGmtDriver::runLog("��ȡxml�����ļ�����ʧ��!", dir);
		strRetCode.Format("%d", (*ret));
		pair< CString, CString > retcode("returncode", strRetCode);
		sub.push_back(retcode);
		return sub;  //�򿪴�����ʧ��
	}

	int result = pInst->LoadDriver(type);
	if(result != SUCCESS)
	{
		CGmtDriver::runLog("��������LoadDriverʧ��! \r\n",dir);
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	if (!pInst->IsDriverLoaded())
	{	
		CGmtDriver::runLog("��������IsDriverLoadedʧ��! \r\n",dir);
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	char *args = (char *)malloc(1024 + 1);
	char *resmsg = (char *)malloc(4*1024*1024 + 1);
	char *errmsg = (char *)malloc(256 + 1);
	memset(resmsg, 0x00, sizeof(resmsg));
	memset(errmsg, 0x00, sizeof(errmsg));
	memset(args, 0x00, sizeof(args));

	int timeout = pInst->GetTimeOut();
	
	logstr.Format("�������portno=%s, type=%d, _partno=%s", portno, type, _partno);
	CGmtDriver::runLog(logstr, dir);

	// ��ʼ��
	result = pInst->initDriver(&MyCallback);
	if (result != SUCCESS)
	{	
		CGmtDriver::runLog("���ó���������ʼ������initDriverʧ��! \r\n", dir);
		pInst->RealseDriver();
		pair< CString, CString > retcode("returncode", "0");
		sub.push_back(retcode);
		return sub;
	}

	// ����ͨѶ����
	CString tmp("USB0");
	if (_partno.Compare(tmp) == 0)
	{
		_partno.Format("USB");
	}
	pInst->setPortAttr(_partno.GetBuffer(_partno.GetLength()));
	
	VPair outlist;
	outlist.clear();
	CString str;

	if (1 == type)
	{
		logstr.Format("��������Կ:call ResetZmk()...");
		CGmtDriver::runLog(logstr, dir);
		result = pInst->ResetZmk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (8 == type)
	{
		logstr.Format("��ȡ�豸���к�:call ReadDeviceInfo()...");
		CGmtDriver::runLog(logstr, dir);
		result = pInst->ReadDeviceInfo(args, resmsg, errmsg, timeout, timeout);
	}
	else if (11 == type)
	{
		logstr.Format("��������Կ:call LoadZmk()...");
		CGmtDriver::runLog(logstr, dir);
		
		logstr.Format("���:mkNew=%s", indata[5]);
		CGmtDriver::runLog(logstr, dir);

		// ����Կ
		PAIR zmk("Zmk", indata[5]);
		outlist.push_back(zmk);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("���ɵ�json���ݣ�%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->LoadZmk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (12 == type)
	{
		logstr.Format("���빤����Կ:call LoadZpk()...");
		CGmtDriver::runLog(logstr, dir);

		logstr.Format("���:wk=%s", indata[4]);
		CGmtDriver::runLog(logstr, dir);

		// ������Կ
		PAIR zpk("Zpk", indata[4]);
		outlist.push_back(zpk);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("���ɵ�json���ݣ�%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->LoadZpk(args, resmsg, errmsg, timeout, timeout);
	}
	else if (17 == type)
	{
		logstr.Format("��������:call ReadCipher()...");
		CGmtDriver::runLog(logstr, dir);

		// ����λ��
		PAIR PinLen("PinLen", indata[2]);
		outlist.push_back(PinLen);

		// ��ʱʱ��
		timeout = atoi(indata[3]);

		// ���ŵ�����
		PAIR Speech("Speech", indata[4]);
		outlist.push_back(Speech);

		// �˺�
		PAIR AccNo("AccNo", indata[5]);
		outlist.push_back(AccNo);

		// Html
		PAIR Html("Html", "");
		outlist.push_back(Html);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("���ɵ�json���ݣ�%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);
		result = pInst->ReadCipher(args, resmsg, errmsg, timeout, timeout);

		// �ɹ�ʱ��������ĳ���ƴ���������ĵ�ǰ�棬���ȱ���ռ2λ
		if (0 == result)
		{
			int pwdlen = atoi(indata[2]);
			CString lenStr;
			if (pwdlen < 10)
			{
				lenStr.Format("0%d", pwdlen);
			}
			else
			{
				lenStr.Format("%d", pwdlen);
			}

			CString passwd;
			passwd.Format("%s%s", lenStr, resmsg);

			logstr.Format("���ϳ��ȱ�ʶ������룺%s", passwd);
			CGmtDriver::runLog(logstr, dir);

			memset(resmsg, 0x00, sizeof(resmsg) + 4);
			strcpy(resmsg, passwd.GetBuffer(passwd.GetLength()));
		}

	}
	else if (26 == type)
	{
		logstr.Format("��ʾ��ȷ����Ϣ:call DisplayHtml()...");
		CGmtDriver::runLog(logstr, dir);

		VPair btnlist;
		btnlist.clear();

		// ��ť����
		PAIR Num("Num", "2");
		btnlist.push_back(Num);

		// ��ť1
		PAIR Button1("Button1", "ȷ��");
		btnlist.push_back(Button1);

		// ��ť2
		PAIR Button2("Button2", "ȡ��");
		btnlist.push_back(Button2);

		pInst->writeJson(btnlist);
		CString btnjson = btnlist[0].second;
		btnjson.Replace(" ", "");
		btnjson.Replace("\n", "");

		// Html
		PAIR Html("Html", indata[1]);
		outlist.push_back(Html);

		// ��ʾ����
		PAIR Speech("Speech", indata[2]);
		outlist.push_back(Speech);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		// ��ťjson
		PAIR Button("Button", btnjson);
		outlist.push_back(Button);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("���ɵ�json���ݣ�%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);

		// ��ʱʱ��
		timeout = atoi(indata[3]);
		result = pInst->DisplayHtml(args, resmsg, errmsg, timeout, timeout);
	}
	else if (27 == type)
	{
		logstr.Format("��ʾ��Ϣ:call DisplayHtml()...");
		CGmtDriver::runLog(logstr, dir);

		VPair btnlist;
		btnlist.clear();

		// ��ť����
		PAIR Num("Num", "0");
		btnlist.push_back(Num);

		pInst->writeJson(btnlist);
		CString btnjson = btnlist[0].second;
		btnjson.Replace(" ", "");
		btnjson.Replace("\n", "");

		// Html
		PAIR Html("Html", indata[1]);
		outlist.push_back(Html);

		// ��ʾ����
		PAIR Speech("Speech", indata[2]);
		outlist.push_back(Speech);

		// Audio
		PAIR Audio("Audio", "");
		outlist.push_back(Audio);

		// ��ťjson
		PAIR Button("Button", btnjson);
		outlist.push_back(Button);

		pInst->writeJson(outlist);
		CString json = outlist[0].second;

		logstr.Format("���ɵ�json���ݣ�%s", json);
		CGmtDriver::runLog(logstr, dir);

		strcpy(args, json);

		// ��ʱʱ��
		timeout = atoi(indata[3]);
		result = pInst->DisplayHtml(args, resmsg, errmsg, timeout, timeout);
	}
	else
	{
		strcpy(resmsg, "�ɹ�");
		result = 0;
	}

	logstr.Format("���ó��̺���������Ϣ:iret=%d, resmsg=%s, errmsg=%s", result, resmsg, errmsg);
	CGmtDriver::runLog(logstr, dir);

	CGmtDriver::runLog("���ó��������ͷ���Դ����deinit()...", dir);
	pInst->deinit();

	sub = pInst->GetRetListRef();
	// �����̷�����תΪ�����Ӧ�ķ�����
	if (result == 0)
	{
		str.Format("%d", 1);
	}
	else if (result == 1)
	{
		str.Format("%d", 0);
	}
	else
	{
		str.Format("%d", result);
	}
	PAIR returncode("returncode", str);
	if (sub.size() == 0)
	{
		sub.push_back(returncode);
	}
	else
	{
		sub.insert(sub.begin(), returncode);
	}
	
	if(26 != type)
	{
		str.Format("%s", resmsg);
		PAIR res_msg("res_msg", str);
		sub.push_back(res_msg);
		
		if (result != 0)
		{
			str.Format("%s", errmsg);
			PAIR err_msg("err_msg", str);
			sub.push_back(err_msg);
		}
	}

	free(args);
	free(resmsg);
	free(errmsg);

	(*size) = sub.size();

	pInst->RealseDriver();
	CGmtDriver::runLog("���ý���... \r\n", dir);
	return sub;

	//�ָ���������Կ
	/*
	if(1==type) 
	{
		runLog("�ָ���������Կ:call SMInitMainKey()... \r\n",dir);
		
		char bpPort={0};

		
		logstr.Format("���:nPort=%d\r\n",port);
		runLog(logstr,dir);
		
		int iRet=pInst->SMInitMainKey(port,bpPort);

		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			sub.push_back(returnmsg);
			
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;
		
	}
	//������������Կ
	if(2==type) 
	{
		logstr.Format("������������Կ:call SMSetMainKeyClear()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		char mk1[32]={0};
		char mk2[32]={0};
		char bpPort={0};
		
		strcpy(mk1,indata[2]);
		strcpy(mk2,indata[3]);
		logstr.Format("���:nPort=%d,mkNo=%d,mk1=%s,mk2=%s\r\n",nPort,mkNo,mk1,mk2);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetMainKeyClear(nPort,bpPort,mkNo,mk1,mk2);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//������������Կ
	if(3==type) 
	{
	
		logstr.Format("������������Կ:call SMSetMainKeyEncrypt()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		char mkEncrypt[32]={0};
		char bpPort={0};
	
		strcpy(mkEncrypt,indata[2]);
		
		logstr.Format("���:nPort=%d,mkNo=%d,mkEncrypt=%s\r\n",nPort,mkNo,mkEncrypt);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetMainKeyEncrypt(nPort,bpPort,mkNo,mkEncrypt);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//���ù�����Կ
	if(4==type) 
	{
	
		logstr.Format("���ù�����Կ:call SMSetWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char wk[32]={0};
		char bpPort={0};
		
		strcpy(wk,indata[3]);
		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d,wk=%s\r\n",nPort,mkNo,wkNo,wk);
		runLog(logstr,dir);
		
		int iRet=pInst->SMSetWorkKey(nPort,bpPort,mkNo,wkNo,wk);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);
		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//�������Կ
	if(5==type) 
	{
	
		logstr.Format("�������Կ:call SMActivateWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char bpPort={0};

		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d\r\n",nPort,mkNo,wkNo);
		runLog(logstr,dir);
		
		int iRet=pInst->SMActivateWorkKey(nPort,bpPort,mkNo,wkNo);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//������Կ�����ַ���
	if(6==type) 
	{
	
		logstr.Format("������Կ�����ַ���:call SMEncryptString()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char data[32]={0};
		char result[160]={0};
		char bpPort={0};
		strcpy(data,indata[3]);

		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d,data=%s\r\n",nPort,mkNo,wkNo,data);
		runLog(logstr,dir);
		
		int iRet=pInst->SMEncryptString(nPort,bpPort,mkNo,wkNo,data,result);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��������
	if(7==type) 
	{
	
		logstr.Format("��������:call SMInputPin()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char inputCount[30]={0};
		int pinLen=atoi(indata[2]);
		int timeout=atoi(indata[3]);

		strcpy(inputCount,indata[1]);

		char pwd[33]={0};
		char bpPort={0};

		logstr.Format("���:inputCount=%s,pinLen=%d,timeout=%d,nPort=%d,\r\n",inputCount,pinLen,timeout,nPort);
		runLog(logstr,dir);
		
		int iRet=pInst->SMInputPin(nPort,bpPort,inputCount,pinLen,timeout,pwd);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("pwd",pwd);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:pwd=%s\r\n",pwd);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ�豸���к�	
	if(8==type) 
	{
	
		logstr.Format("��ȡ�豸���к�:call GetDevID()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int len=atoi(indata[1]);
		char devID[100];
		char bpPort={0};

		logstr.Format("���:nPort=%d,len=%d\r\n",nPort,len);
		runLog(logstr,dir);
		
		int iRet=pInst->GetDevID(nPort,bpPort,devID,len);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("devID",devID);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:devID=%s\r\n",devID);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//�����豸���к�	
	if(9==type) 
	{
	
		logstr.Format("�����豸���к�:call SetDevID()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char devID[100];
		char bpPort={0};

		strcpy(devID,indata[1]);

		logstr.Format("���:devID=%s,nPort=%d\r\n",devID,nPort);
		runLog(logstr,dir);
		
		int iRet=pInst->SetDevID(nPort,bpPort,devID);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��ʼ������Կ	
	if(10==type) 
	{
	
		logstr.Format("��ʼ������Կ:call InitFactoryKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		logstr.Format("���:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->InitFactoryKey(nPort,bpPort);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��������Կ	
	if(11==type) 
	{
	
		logstr.Format("��������Կ:call SetMainKey()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int mkLenOld=atoi(indata[2]);
		char mkOld[48]={0};
		int mkLenNew=atoi(indata[4]);
		char mkNew[48]={0};

		strcpy(mkOld,indata[3]);
		strcpy(mkNew,indata[5]);

		logstr.Format("���:nPort=%d,mkNo=%d,mkLenOld=%d,mkOld=%s,mkLenNew=%d,mkNew=%s\r\n",nPort,mkNo,mkLenOld,mkOld,mkLenNew,mkNew);
		runLog(logstr,dir);

		int iRet=pInst->SetMainKey(nPort,bpPort,mkNo,mkLenOld,mkOld,mkLenNew,mkNew);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//���ù�����Կ	
	if(12==type) 
	{
	
		logstr.Format("���ù�����Կ:call SetWorkKey()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		int wkLen=atoi(indata[3]);
		char wk[48]={0};

		strcpy(wk,indata[4]);

		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d,wkLen=%d,wk=%s\r\n",nPort,mkNo,wkNo,wkLen,wk);
		runLog(logstr,dir);

		int iRet=pInst->SetWorkKey(nPort,bpPort,mkNo,wkNo,wkLen,wk);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//ѡ������Կ	
	if(13==type) 
	{
	
		logstr.Format("ѡ������Կ:call ActivateWorkKey()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);

		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d\r\n",nPort,mkNo,wkNo);
		runLog(logstr,dir);

		int iRet=pInst->ActivateWorkKey(nPort,bpPort,mkNo,wkNo);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//�����ַ���	
	if(14==type) 
	{
	
		logstr.Format("�����ַ���:call EncryptString()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int mkNo=atoi(indata[1]);
		int wkNo=atoi(indata[2]);
		char dataIn[100]={0};
		char dataOut[100]={0};

		strcpy(dataIn,indata[3]);

		logstr.Format("���:nPort=%d,mkNo=%d,wkNo=%d,dataIn=%s\r\n",nPort,mkNo,wkNo,dataIn);
		runLog(logstr,dir);

		int iRet=pInst->EncryptString(nPort,bpPort,mkNo,wkNo,dataIn,dataOut);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("dataOut",dataOut);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:dataOut=%s\r\n",dataOut);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//������������	
	if(15==type) 
	{
	
		logstr.Format("������������:call GetClearTextPin()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);

		logstr.Format("���:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice);
		runLog(logstr,dir);

		int iRet=pInst->GetClearTextPin(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,result);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//3DES��������	
	if(16==type) 
	{
	
		logstr.Format("3DES��������:call GetCipherTextPin()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);

		logstr.Format("���:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice);
		runLog(logstr,dir);

		int iRet=pInst->GetCipherTextPin(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,result);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//PINBLOCK��������	
	if(17==type) 
	{
	
		logstr.Format("PINBLOCK��������:call GetCipherTextPin98()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		int inputTime=atoi(indata[1]);
		int pwLen=atoi(indata[2]);
		int Timeout=atoi(indata[3]);
		char pVoice[100]={0};
		char accNo[20]={0};
		char result[100]={0};

		strcpy(pVoice,indata[4]);
		strcpy(accNo,indata[5]);

		logstr.Format("���:nPort=%d,inputTime=%d,pwLen=%d,Timeout=%d,pVoice=%s,accNo=%s\r\n",nPort,inputTime,pwLen,Timeout,pVoice,accNo);
		runLog(logstr,dir);

		int iRet=pInst->GetCipherTextPin98(nPort,bpPort,inputTime,pwLen,Timeout,pVoice,accNo,result);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR returnmsg("result",result);
			sub.push_back(returnmsg);
			logstr.Format("������Ϣ:result=%s\r\n",result);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//���ù�Ա��Ϣ	
	if(18==type) 
	{
	
		logstr.Format("���ù�Ա��Ϣ:call SetTellerInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		char name[30]={0};
		char workId[30]={0};
		int grade=atoi(indata[3]);
		char other[30]={0};


		strcpy(name,indata[1]);
		strcpy(workId,indata[2]);
		strcpy(other,indata[4]);

		logstr.Format("���:name=%s,workId=%s,grade=%d,other=%s,nPort=%d\r\n",name,workId,grade,other,nPort);
		runLog(logstr,dir);

		int iRet=pInst->SetTellerInfo(nPort,bpPort,name,workId,grade,other);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ��Ա��Ϣ	
	if(19==type) 
	{
	
		logstr.Format("��ȡ��Ա��Ϣ:call GetTellerInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort={0};
		
		char name[30]={0};
		char workId[30]={0};
		int grade=0;
		char other[30]={0};

		int iRet=pInst->GetTellerInfo(nPort,bpPort,name,workId,&grade,other);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR pname("name",name);
			sub.push_back(pname);
			PAIR pworkId("workId",workId);
			sub.push_back(pworkId);
			str.Format("%d",grade);
			PAIR pgrade("grade",str);
			sub.push_back(pgrade);
			PAIR pother("other",other);
			sub.push_back(pother);
			logstr.Format("������Ϣ:name=%s,workId=%s,grade=%d,other=%s\r\n",name,workId,grade,other);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//��ʼ��ʱ	
	if(20==type) 
	{
	
		logstr.Format("��ʼ��ʱ:call BeginWorkTick()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		char bpPort={0};

		int iRet=pInst->BeginWorkTick(nPort,bpPort);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//������ʱ	
	if(21==type) 
	{
	
		logstr.Format("������ʱ:call EndWorkTick()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		DWORD tick=atol(indata[1]);
		char bpPort={0};

		int iRet=pInst->EndWorkTick(nPort,bpPort,&tick);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//���ط��ٱ�֤��
	if(22==type) 
	{
	
		logstr.Format("���ط��ٱ�֤��:call DownloadLicense()... \r\n");
		runLog(logstr,dir);
		
		char path[100]={0};

		strcpy(path,indata[1]);
		logstr.Format("���:path=%s",path);
		runLog(logstr,dir);

		int iRet=pInst->DownloadLicense(path);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ʾ��Ա��Ϣ
	if(23==type) 
	{
	
		logstr.Format("��ʾ��Ա��Ϣ:call ShowTellerInfo()... \r\n");
		runLog(logstr,dir);
		
		int nPort=port;
		int timeout=atoi(indata[1]);
		char bpPort=0;

		logstr.Format("���:nPort=%d,timeout=%d\r\n",nPort,timeout);
		runLog(logstr,dir);

		int iRet=pInst->ShowTellerInfo(nPort,bpPort,timeout);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}

	//��������
	if(24==type) 
	{
	
		logstr.Format("��������:call InputEvaluation()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		int timeout=atoi(indata[1]);
		char seviceType[100]={0};
		strcpy(seviceType,indata[2]);


		logstr.Format("���:seviceType=%s,timeout=%d,nPort=%d\r\n",seviceType,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->InputEvaluation(nPort,bpPort,seviceType,timeout);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//�����ļ�
	if(25==type) 
	{
	
		logstr.Format("�����ļ�:call SaveFile()... \r\n");
		runLog(logstr,dir);
		
		int type=atoi(indata[1]);
		char path[100]={0};
		strcpy(path,indata[2]);

		logstr.Format("���:path=%s,type=%d\r\n",path,type);
		runLog(logstr,dir);

		int iRet=pInst->SaveFile(type,path);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//����Ϣ��ʾ����Ļ���ÿͻ�ȷ����Ϣ
	if(26==type) 
	{
	
		logstr.Format("����Ϣ��ʾ����Ļ���ÿͻ�ȷ����Ϣ:call ConfirmInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		char pHtml[2048]={0};
		char pVoice[100]={0};
		char pResult[20]={0};

		strcpy(pHtml,indata[1]);
		strcpy(pVoice,indata[2]);
		int timeout=atoi(indata[3]);
		

		logstr.Format("���:pHtml=%s,pVoice=%s,timeout=%d,nPort=%d\r\n",pHtml,pVoice,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->ConfirmInfo(nPort,bpPort,pHtml,pVoice,timeout,sizeof(pResult),pResult);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			PAIR pResult("pResult",pResult);
			sub.push_back(pResult);
			logstr.Format("������Ϣ:pResult=%s\r\n",pResult);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//����Ϣ��ʾ����Ļ���ÿͻ��鿴��Ϣ
	if(27==type) 
	{
	
		logstr.Format("����Ϣ��ʾ����Ļ���ÿͻ��鿴��Ϣ:call ShowInfo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		
		char pHtml[2048]={0};
		char pVoice[100]={0};

		strcpy(pHtml,indata[1]);
		strcpy(pVoice,indata[2]);
		int timeout=atoi(indata[3]);
		

		logstr.Format("���:pHtml=%s,pVoice=%s,timeout=%d,nPort=%d\r\n",pHtml,pVoice,timeout,nPort);
		runLog(logstr,dir);

		int iRet=pInst->ShowInfo(nPort,bpPort,pHtml,pVoice,timeout);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//������Ա������ʹ�豸���Ź�棬�ɳ�����Ϣ�������������룬���۵�
	if(28==type) 
	{
	
		logstr.Format("����:call CancelCustomProcess()... \r\n");
		runLog(logstr,dir);
		

		int nPort=port;
		char bpPort=0;

		logstr.Format("���:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->CancelCustomProcess(nPort,bpPort);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//3DES������������Կ
	if(29==type) 
	{
	
		logstr.Format("3DES������������Կ:call SetMainKeyClear()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;

		int mkNo=atoi(indata[1]);
		int mkLen1=atoi(indata[2]);
		char mk1[64]={0};
		int mkLen2=atoi(indata[4]);
		char mk2[64]={0};

		strcpy(mk1,indata[3]);
		strcpy(mk2,indata[5]);

		logstr.Format("���:nPort=%d,mkNo=%d,mkLen1=%d,mk1=%s,mkLen2=%d,mk2=%s \r\n",nPort,mkNo,mkLen1,mk1,mkLen2,mk2);
		runLog(logstr,dir);

		boolean iRet=pInst->SetMainKeyClear(nPort,bpPort,mkNo,mkLen1,mk1,mkLen2,mk2);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//���ò��Ź������
	if(30==type) 
	{
	
		logstr.Format("���ò��Ź������:call SetPosterType()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		int type=atoi(indata[1]);

		logstr.Format("���:nPort=%d,type=%d \r\n",nPort,type);
		runLog(logstr,dir);

		int iRet=pInst->SetPosterType(type);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//ɾ�����ͼƬ
	if(31==type) 
	{
	
		logstr.Format("ɾ�����ͼƬ:call DeletePic()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char picName[100]={0};

		strcpy(picName,indata[1]);

		logstr.Format("���:nPort=%d,picName=%s \r\n",nPort,picName);
		runLog(logstr,dir);

		int iRet=pInst->DeletePic(picName);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//ɾ�����й��ͼƬ
	if(32==type) 
	{
	
		logstr.Format("ɾ�����й��ͼƬ:call DeleteAllPic()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		logstr.Format("���:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->DeleteAllPic();
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ�����Ƶ�б�
	if(33==type) 
	{
	
		logstr.Format("��ȡ�����Ƶ�б�:call GetVideoList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		char pVideoList[200]={0};

		logstr.Format("���:nPort=%d\r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetVideoList(sizeof(pVideoList),pVideoList);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString vlist(pVideoList);
			vlist.Replace("&","#");
			PAIR pVideo_List("pVideoList",vlist);
			sub.push_back(pVideo_List);
			logstr.Format("��ȡ�����Ƶ�б�:%s\r\n",pVideoList);
			runLog(logstr,dir);
		}
		(*size) = sub.size();
		return sub;	
	}
	//ɾ�������Ƶ
	if(34==type) 
	{
	
		logstr.Format("ɾ�������Ƶ:call DeleteVideo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char pVideoName[100]={0};

		strcpy(pVideoName,indata[1]);

		logstr.Format("���:nPort=%d,pVideoName=%s \r\n",nPort,pVideoName);
		runLog(logstr,dir);

		int iRet=pInst->DeleteVideo(pVideoName);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//ɾ�����й����Ƶ
	if(35==type) 
	{
	
		logstr.Format("ɾ�����й����Ƶ:call DeleteAllVideo()... \r\n");
		runLog(logstr,dir);

		int nPort=port;

		logstr.Format("���:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->DeleteAllVideo();
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;
		char pError[200]={0};
		
		if(iRet==0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			pInst->GetErrorMsg(sizeof(pError),pError);
			str.Format("%s",pError);
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ���ͼƬ�б�
	if(36==type) 
	{
	
		logstr.Format("��ȡ���ͼƬ�б�:call GetPicList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char picList[200]={0};

		logstr.Format("���:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetPicList(nPort,bpPort,sizeof(picList),picList);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString plist(picList);
			plist.Replace("&","#");
			PAIR pic_list("picList",plist);
			sub.push_back(pic_list);

			logstr.Format("��ȡ���ͼƬ�б�:%s\r\n",picList);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ�����б�
	if(37==type) 
	{
	
		logstr.Format("��ȡ�����б�:call GetPlayList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char playList[200]={0};

		logstr.Format("���:nPort=%d \r\n",nPort);
		runLog(logstr,dir);

		int iRet=pInst->GetPlayList(nPort,bpPort,playList,sizeof(playList));
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

			CString plist(playList);
			plist.Replace("&","#");
			PAIR play_List("playList",plist);
			sub.push_back(play_List);
			logstr.Format("��ȡ�����б�:%s\r\n",playList);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//���ò����б�
	if(38==type) 
	{
	
		logstr.Format("���ò����б�:call SetPlayList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		char playList[200]={0};
		if(indata[1]!=NULL){
			strcpy(playList,indata[1]);
		}

		logstr.Format("���:nPort=%d,playList=%s \r\n",nPort,playList);
		runLog(logstr,dir);

		int iRet=pInst->SetPlayList(nPort,bpPort,playList);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ȡ�ļ��б�
	if(39==type) 
	{
	
		logstr.Format("��ȡ�ļ��б�:call GetFileList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char buf[200]={0};

		type=atoi(indata[1]);

		logstr.Format("���:nPort=%d,type=%d \r\n",nPort,type);
		runLog(logstr,dir);

		int iRet=pInst->GetFileList(nPort,bpPort,type,sizeof(buf),buf);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			CString replacestr(buf);
			replacestr.Replace("&","#");
			PAIR p_buf("filelist",replacestr);
			sub.push_back(p_buf);
			logstr.Format("��ȡ�ļ��б�:%s\r\n",buf);
			runLog(logstr,dir);

		}
		(*size) = sub.size();
		return sub;	
	}
	//ɾ���ļ�
	if(40==type) 
	{
	
		logstr.Format("ɾ���ļ�:call DeleteFileList()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char fileName[100]={0};

		type=atoi(indata[1]);
		strcpy(fileName,indata[2]);

		logstr.Format("���:nPort=%d,fileName=%s \r\n",nPort,fileName);
		runLog(logstr,dir);

		int iRet=pInst->DeleteFileList(nPort,bpPort,type,fileName);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	//��ͣ����
	if(41==type) 
	{
	
		logstr.Format("��ͣ����:call ShowService()... \r\n");
		runLog(logstr,dir);

		int nPort=port;
		char bpPort=0;
		int type=1;
		char info[100]={0};
		bool show=1;

		strcpy(info,indata[1]);
		if(!strcmp("0",indata[2]))
		{
			show=0;
		}
		else
		{
			show=1;
		}

		logstr.Format("���:nPort=%d,info=%s,show=%d \r\n",nPort,info,show);
		runLog(logstr,dir);

		int iRet=pInst->ShowService(nPort,bpPort,info,show);
		logstr.Format("������:iRet=%d\r\n",iRet);
		runLog(logstr,dir);

		CString str;		
		if(iRet<=0)
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);
			
			str.Format("%s",GetGmtErrmsg(iRet));
			PAIR returnmsg("returnmsg",str);
			logstr.Format("���ش�����Ϣ:%s\r\n",str);
			runLog(logstr,dir);
			
			sub.push_back(returnmsg);
		}
		else
		{
			str.Format("%d",iRet);
			PAIR returncode("returncode",str);
			sub.push_back(returncode);

		}
		(*size) = sub.size();
		return sub;	
	}
	
	pInst->RealseDriver();
	return sub;
	*/
}

int WINAPI SetTimeOut(int timeout)
{
   
	return 0;
}

int WINAPI Write(char **indata, int size, char *portno, char *conffile)
{
	CGmtDriver *pInst = CGmtDriver::GetInstance();
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


