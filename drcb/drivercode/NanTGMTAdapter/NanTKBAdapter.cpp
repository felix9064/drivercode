// NanTKBAdapter.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "NanTKBAdapter.h"
#include "variabledef.h"

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
// CNanTKBAdapterApp

BEGIN_MESSAGE_MAP(CNanTKBAdapterApp, CWinApp)
	//{{AFX_MSG_MAP(CNanTKBAdapterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNanTKBAdapterApp construction

CNanTKBAdapterApp::CNanTKBAdapterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNanTKBAdapterApp object

CNanTKBAdapterApp theApp;



/********************************************
* ��  �ܣ�����������
* ��  ����@path - ������·��
* ����ֵ���ɹ����ط�0,����0
*********************************************/
HDLL WINAPI SFKBLoadDrv(TPCHR path, PCHR ErrDesc)
{
	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	
	CString tmp = path;
	CString dir = tmp.Mid(0, tmp.ReverseFind('\\') + 1);

	SetCurrentDirectory(dir);
	HDLL hDrv = LoadLibrary(path);
	SetCurrentDirectory(buf);
    if(!hDrv)
	{
		FILE* file = fopen("dll.log","a+");
		fprintf(file,"����ʧ��\n");
		fclose(file);
		sprintf(ErrDesc, "%s������δ�ҵ�", path);
		return 0;
	}

	return hDrv;
}

/********************************************
* ��  �ܣ��ͷ�������
* ��  ����@hDrv - ��������
* ����ֵ����
*********************************************/
void WINAPI SFKBFreeDrv(HDLL hDrv)
{
	FreeLibrary(hDrv);
}


/********************************************
* ��  �ܣ�����Pin
* ��  ����@inList - �ɼ��������ݡ�
	                 �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں� 5 - ���볤��
* ��  ����@MKeyIdx - ����Կ������
* ��  ����@WKeyIdx - ������Կ������
* ��  ����@ACCTNO  - �ͻ��ʺ�
* ��  ����@mode    - ����ģʽ 1��һ���������롣2��������������
* ��  ����@PIN     - ����������Pin����
* ��  ����@TPKCheck - ��������������PIN��TPK��ԿУ��ֵ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI SFKBReadPin(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR ACCTNO, int mode, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR pwdlen = inList[5];


    if(!hDrv)
	{
		sprintf(ErrDesc, "%s������δ�ҵ�", thirddll.second);
	//	AfxMessageBox("������δ�ҵ�");
		return -1;
	}
	/*
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	if(!OpenComm)
	{
		sprintf(ErrDesc, "%s��������OpenCommδ�ҵ�", thirddll.second);
		return -6;
	}
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	if(!CloseComm)
	{
		sprintf(ErrDesc, "%s��������CloseCommδ�ҵ�", thirddll.second);
		return -7;
	}
	lpCMCC_SetPinlength CMCC_SetPinlength = (lpCMCC_SetPinlength)GetProcAddress(hDrv, "CMCC_SetPinlength");
	if (!CMCC_SetPinlength)
	{
        sprintf(ErrDesc, "%s��������CMCC_SetPinlengthδ�ҵ�", thirddll.second);
		return -9;
	}
	lpCMCC_ReadPinANSI98 CMCC_ReadPinANSI98 = (lpCMCC_ReadPinANSI98)GetProcAddress(hDrv, "CMCC_ReadPinANSI98");
	if(!CMCC_ReadPinANSI98)
	{
		sprintf(ErrDesc, "%s��������CMCC_ReadPinANSI98δ�ҵ�", thirddll.second);
		return -8;
	}	
	*/

	lpReadPin ReadPin = (lpReadPin)GetProcAddress(hDrv,"readPin");
	if(!ReadPin)
	{
		sprintf(ErrDesc, "%s��������lpReadPinδ�ҵ�", thirddll.second);
		//AfxMessageBox("getprocaddress-readPin");
		return -8;
	}
	/*
	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
*/
	try
	{
		CString port;
		port.Format("COM%s",portno.second);
	//	AfxMessageBox("the port");
	//	PIN = ReadPin(0,mode,atoi(pwdlen.second),port.GetBuffer(port.GetLength()),9600,timeout);
		//AfxMessageBox("after read");
	/*
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "�򿪴���ʧ��!");
		    return -3;
		}

		ret = CMCC_SetPinlength(hComm, 6, 6, 0, timeout);
		if (ret != 0)
		{
            sprintf(ErrDesc, "�������볤��ʧ��!");
			CloseComm(hComm);
			return -11;
		}

		int pinlen = 0;
		if(mode == 1)
		{
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 0, &pinlen, PIN, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "��ȡPinBlockʧ��!");
				CloseComm(hComm);
				return -4;
			}

		}
		else
		{
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 0, &pinlen, PIN, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "��ȡPinBlockʧ��!");
				CloseComm(hComm);
				return -4;
			}

			char PIN2[BUFSIZ_256];
			memset(PIN2, 0, BUFSIZ_256);
			ret = CMCC_ReadPinANSI98(hComm, MKeyIdx, WKeyIdx, ACCTNO, 1, &pinlen, PIN2, timeout);
			if(ret != 0)
			{
				sprintf(ErrDesc, "�ڶ��ζ�ȡPinBlockʧ��!");
				CloseComm(hComm);
				return -4;
			}

			if(strcmp(PIN, PIN2) != 0)
			{
				sprintf(ErrDesc, "������������벻һ��!");
				CloseComm(hComm);
				return -5;
			}
		}

		CloseComm(hComm);
		*/
	}
	catch (...)
	{
		sprintf(ErrDesc, "��ȡPinBlockģ�鷢���쳣!");
		return -10;
	}


	return 0;
}
int WINAPI SFKBReadPin2(HDLL hDrv, VPair &inList, TPCHR MKeyIdx, TPCHR WKeyIdx, TPCHR ACCTNO, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	PAIR minLen = inList[5];
	PAIR maxLen = inList[6];
	PAIR prBaudrate = inList[7];
	PAIR prTwice = inList[8];
	PAIR srvType = inList[9];
	CString srvtype = srvType.second;
	
	PinPadGuomiPinNew ReadPinNew = (PinPadGuomiPinNew)GetProcAddress(hDrv,"PinPadGuomiPinNew");
	PinPadGuomiPin ReadPin = (PinPadGuomiPin)GetProcAddress(hDrv,"PinPadGuomiPin");
	if(ReadPinNew == NULL || ReadPin == NULL)
	{
		sprintf(ErrDesc, "%s��������δ�ҵ�PinPadGuomiPin��PinPadGuomiPinNew", thirddll.second);
		return -19;
	}
	//debug("PinPadGuomiPin success!");

	CString csport = (porttype.second=="1"?"COM":"USB") + portno.second;
	
	debug("���������%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",csport,"1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,srvType.second);

	int ret;
	if ("0" == srvtype)
	{
		ret = ReadPin(csport,"","1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,PIN);
	}
	else
	{
		ret = ReadPinNew(csport,"",srvtype.GetBuffer(srvtype.GetLength()),"1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second,PIN);
	}

	//debug("����ֵ��%d,���룺%s",ret,PIN);
	if (ret<0)
	{
		debug("����ʧ��,����ֵ%d,����Ϊ:%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ret,csport,"","1",MKeyIdx,"X",ACCTNO,"0",tmptimeout,minLen.second,maxLen.second);
		return -14;//������ʧ��
	}
	if (prTwice.second == "2")
	{
		char PIN2[32]={0};
		if ("0" == srvtype)
		{
			ret = ReadPin(csport,"","1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second,PIN2);
		}
		else
		{
			ret = ReadPinNew(csport,"",srvtype.GetBuffer(srvtype.GetLength()),"1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second,PIN2);
		}
		
		if(ret<0)
		{
			debug("����ʧ��,����ֵ%d,����Ϊ:%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",ret,csport,"","1",MKeyIdx,"X",ACCTNO,"1",tmptimeout,minLen.second,maxLen.second);
			return -14;//������ʧ��
		}
		if(strcmp(PIN,PIN2)!=0)
		{
			return -10;//�������벻һ��
		}
	}
	
	
	return ret;

}
/********************************************
* ��  �ܣ�����TMK��������̡�
* ��  ����@inList - �ɼ��������ݡ�
	                    �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
* ��  ����@MKeyIdx - ����Կ������
* ��  ����@lpOldMKey  - ����Կ��
* ��  ����@mode    - ����Կ��״̬��0 - ����״̬ 1 - ����״̬
* ��  ����@lpKeyCK - ��ԿУ��ֵ�����봫��������
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI SFKBLoadMasterKey(HDLL hDrv, VPair &inList, int MKeyIdx, TPCHR lpOldMKey, TPCHR lpMKey, int mode, PCHR lpKeyCK, PCH ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
    if(!hDrv)
	{
		sprintf(ErrDesc, "%s������δ�ҵ�", thirddll.second);
		return -1;
	}
	
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	lpCMCC_LoadMasterKey CMCC_LoadMasterKey = (lpCMCC_LoadMasterKey)GetProcAddress(hDrv, "CMCC_LoadMasterKey");
	

	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
	
	try
	{		
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "�򿪴���ʧ��!");
			return -3;
		}

		ret = CMCC_LoadMasterKey(hComm, MKeyIdx, lpOldMKey, lpKeyCK, timeout);
		if(ret != 0)
		{
			sprintf(ErrDesc, "��עTMKʧ��!");
			CloseComm(hComm);
			return -4;
		}
		
		CloseComm(hComm);
	}
	catch (...)
	{
		sprintf(ErrDesc, "��ȡPinBlockģ�鷢���쳣!");
		return -10;
	}

	return 0;
}

/********************************************
* ��  �ܣ�����TPK/TAK��������̡�
* ��  ����@inList - �ɼ��������ݡ�
	         �����±���ͣ�0 - ģ������Ŀ¼ 1 - ������������ 2 - ������������ 3 - �˿����� 4 - �˿ں�
* ��  ����@MKeyIdx - ����Կ������
* ��  ����@WKeyIdx - ������Կ������
* ��  ����@lpMKey  - ������Կ��
* ��  ����@lpKeyCK - ��ԿУ��ֵ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI SFKBLoadWorkKey(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpWKey, TPCHR lpKeyCK, PCHR ErrDesc)
{	
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
    if(!hDrv)
	{
		sprintf(ErrDesc, "%s������δ�ҵ�", thirddll.second);
		return -1;
	}
	
	lpOpenComm OpenComm = (lpOpenComm)GetProcAddress(hDrv, "OpenComm");
	lpCloseComm CloseComm = (lpCloseComm)GetProcAddress(hDrv, "CloseComm");
	lpCMCC_LoadKey CMCC_LoadKey = (lpCMCC_LoadKey)GetProcAddress(hDrv, "CMCC_LoadKey");
	
	int ret;
	CHAR serialno[BUFSIZ_256];
	memset(serialno, 0, BUFSIZ_256);
	HANDLE hComm;
	
	try
	{
		
		sprintf(serialno, "COM%s", portno.second);
		hComm = OpenComm(serialno, 9600, 0);
		if(!hComm)
		{
			sprintf(ErrDesc, "�򿪴���ʧ��!");
			return -3;
		}
		
		ret = CMCC_LoadKey(hComm, MKeyIdx, WKeyIdx, lpWKey, lpKeyCK, timeout);
		if(ret != 0)
		{
			if(ret == -6)
				sprintf(ErrDesc, "У��ֵ��һ��!��Ҫ��ע��ȷ������Կ��");
			else
				sprintf(ErrDesc, "��עTPK/TAKʧ��!");
			CloseComm(hComm);
			return -4;
		}
		
		CloseComm(hComm);
	}
	catch (...)
	{
		sprintf(ErrDesc, "��ȡPinBlockģ�鷢���쳣!");
		return -10;
	}
	
	return 0;
}

int SFKBLoadWorkKey2(HDLL hDrv, VPair &inList, const char *MKeyIdx, const char *WKeyIdx, const char * KeyLen, const char * keyByTMK, const char * KeyValue, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];
	
	//debug("incoming KPInputKEY!");
	PinPadWriteWorkKeyWithMainkey LoadWorkKey = (PinPadWriteWorkKeyWithMainkey)GetProcAddress(hDrv,"PinPadWriteWorkKeyWithMainkey");
	if (!LoadWorkKey)
	{
		sprintf(ErrDesc, "PinPadWriteWorkKeyWithMainkeyδ�ҵ�");
		return -19;
	}
	//debug("end of PinPadWriteWorkKeyWithMainkey!");
	//const char* port = "COM" + portno.second;
	CString port;
	port.Format("COM%s",portno.second);
	//debug("PinPadWriteWorkKeyWithMainkey���:%s,%s,%s,%s,%s,%s",port,MKeyIdx,WKeyIdx,KeyLen,keyByTMK,KeyValue);
	char retKeyValue[16] = {0};
	int retCode = LoadWorkKey(port,"",MKeyIdx,WKeyIdx,const_cast<char*>(KeyLen),const_cast<char*>(keyByTMK),retKeyValue);
	//debug("retKeyValue:%s,retCode:%d",retKeyValue,retCode);
	if (retCode > 0)
	{
		char *temp = new char[retCode+1];
		strncpy(temp,KeyValue,retCode);
		if (CString(*temp) != CString(*retKeyValue))
		{
			sprintf(ErrDesc, "У�鴮��ƥ��");
			debug("����У�鴮:%s,������̷���:%s",KeyValue,retKeyValue);
			delete [] temp;
			return -15;//����Կʧ��
		}
		delete [] temp;
	}else{
		sprintf(ErrDesc, "��У�鴮����");
		debug("����[PinPadWriteWorkKeyWithMainkey]ʧ��,����ֵ%d,����Ϊ:%s,%s,%s,%s,%s,%s,%s",retCode,port,"",MKeyIdx,WKeyIdx,KeyLen,keyByTMK,KeyValue);
		return -15;//����Կʧ��
	}

	return 0;
}

/********************************************
* ��  �ܣ�����MAC��
* ��  ����@inList - �ɼ��������ݡ�
	                �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
* ��  ����@MKeyIdx - ����Կ������
* ��  ����@WKeyIdx - ������Կ������
* ��  ����@lpData  - mac����
* ��  ����@lpMAC  - ����������MACֵ��
* ��  ����@lpTAKKCK - ����������TAKУ��ֵ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI SFKBGenerateMAC(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, PCHR lpMAC, PCHR lpTAKKCK, PCHR ErrDesc)
{
	return 0;
}

/********************************************
* ��  �ܣ���֤MAC��
* ��  ����@inList - �ɼ��������ݡ�
	                �����±���ͣ�0 - ģ������Ŀ¼ 1 - ��ʱʱ�� 2 - ������������ 3 - �˿����� 4 - �˿ں�
* ��  ����@MKeyIdx - ����Կ������
* ��  ����@WKeyIdx - ������Կ������
* ��  ����@lpData  - mac����
* ��  ����@lpMAC  - MACֵ��
* ����ֵ���ɹ�����0,�����0
*********************************************/
int WINAPI SFKBVerifyMAC(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, TPCHR lpMAC, PCHR ErrDesc)
{
	return 0;
}

int KPGetPinPadID(HDLL hDrv, VPair &inList,PCHR result, PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];

	_KPGetPinPadID GetPinPadID = (_KPGetPinPadID)GetProcAddress(hDrv,"PinPadGetID");
	if (!GetPinPadID)
	{
		sprintf(ErrDesc, "KPGetPinPadIDδ�ҵ�");
		return -19;
	}
	CString port;
	port.Format("COM%s",portno.second);
	int retCode = GetPinPadID(port,"",result);

	if(retCode < 0)
	{
		debug("����[PinPadGetID]ʧ��,����ֵ%d,����Ϊ:%s,%s",retCode,port,"");
		return -7;
	}
		
	return retCode;

}

int PinPadReadEvaluate(HDLL hDrv, VPair &inList,PCHR ErrDesc)
{
	PAIR module = inList[0];
	PAIR ptimeout = inList[1];
	CString tmptimeout = ptimeout.second;
	int timeout = atoi(tmptimeout.GetBuffer(tmptimeout.GetLength()));
	PAIR thirddll = inList[2];
	PAIR porttype = inList[3];
	PAIR portno = inList[4];

	_PinPadReadEvaluate Evaluate = (_PinPadReadEvaluate)GetProcAddress(hDrv,"PinPadReadEvaluate");
	if (!Evaluate)
	{
		sprintf(ErrDesc, "PinPadReadEvaluateδ�ҵ�");
		return -19;
	}
	CString port;
	port.Format("COM%s",portno.second);

	return Evaluate(port,"","1",tmptimeout);

}

int driveKeyBd(HDLL hDrv, VPair &inList, PCHR ErrDesc)
{
	debug("��ʼ����driveKeyBd");
	
	FUNC_PinPadGetCompanyID PinPadGetCompanyID = (FUNC_PinPadGetCompanyID)GetProcAddress(hDrv, "PinPadGetCompanyID");
	FUNC_PinPadEncBySm2_PK PinPadEncBySm2_PK = (FUNC_PinPadEncBySm2_PK)GetProcAddress(hDrv, "PinPadEncBySm2_PK");
	FUNC_PinPadWriteKeyWithSM4Ciper PinPadWriteKeyWithSM4Ciper = (FUNC_PinPadWriteKeyWithSM4Ciper)GetProcAddress(hDrv, "PinPadWriteKeyWithSM4Ciper");
	FUNC_PinPadWriteWorkKeyWithMainkey PinPadWriteWorkKeyWithMainkey = (FUNC_PinPadWriteWorkKeyWithMainkey)GetProcAddress(hDrv, "PinPadWriteWorkKeyWithMainkey");

	if (NULL == PinPadGetCompanyID || NULL == PinPadEncBySm2_PK || NULL == PinPadWriteKeyWithSM4Ciper || NULL == PinPadWriteWorkKeyWithMainkey)
	{
		sprintf(ErrDesc, "���̺���δ�ҵ�");
		return -19;
	}

	CString type = inList[0].second;
	PAIR porttype = inList[1];
	PAIR portno = inList[2];
	CString csport = (porttype.second=="1"?"COM":"USB") + portno.second;

	int optype = atoi(type.GetBuffer(type.GetLength()));
	int ret;

	CString tmpStr;

	if (3 == optype)
	{
		char SeqNo[21] = {0};
		memset(SeqNo, 0, sizeof(SeqNo));
		char factoryID[21] = {0};
		memset(factoryID, 0, sizeof(factoryID));

		ret = PinPadGetCompanyID(csport, "", SeqNo, factoryID);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "���̺���PinPadGetCompanyID���ر���");
			debug("���̺���PinPadGetCompanyID���ر���");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", SeqNo);
		PAIR seqNo("SeqNo", tmpStr);
		inList.push_back(seqNo);

		tmpStr.Format("%s", factoryID);
		PAIR factId("factoryID", tmpStr);
		inList.push_back(factId);

	}
	else if (4 == optype)
	{
		char ranKeyLen[4+1] = {0};
		char PK[256+1] = {0};
		strcpy(ranKeyLen, inList[3].second);
		strcpy(PK, inList[4].second);

		char ciper[256+1] = {0};
		memset(ciper, 0, sizeof(ciper));
		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadEncBySm2_PK(csport, "", ranKeyLen, PK, ciper, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "���̺���PinPadEncBySm2_PK���ر���");
			debug("���̺���PinPadEncBySm2_PK���ر���");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", ciper);
		PAIR Ciper("ciper", tmpStr);
		inList.push_back(Ciper);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
		
	}
	else if (5 == optype)
	{
		char keyIndex[4+1] = {0};
		char newKeyLen[4+1] = {0};
		char newKeyBySM4Key[256+1] = {0};
		strcpy(keyIndex, inList[3].second);
		strcpy(newKeyLen, inList[4].second);
		strcpy(newKeyBySM4Key, inList[5].second);

		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadWriteKeyWithSM4Ciper(csport, "", keyIndex, newKeyLen, newKeyBySM4Key, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "���̺���PinPadWriteKeyWithSM4Ciper���ر���");
			debug("���̺���PinPadWriteKeyWithSM4Ciper���ر���");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
	}
	else if (6 == optype)
	{
		char mainkeyIndex[4+1] = {0};
		char workkeyIndex[4+1] = {0};
		char newKeyLen[4+1] = {0};
		char newKeyBySM4Key[256+1] = {0};
		strcpy(mainkeyIndex, inList[3].second);
		strcpy(workkeyIndex, inList[4].second);
		strcpy(newKeyLen, inList[5].second);
		strcpy(newKeyBySM4Key, inList[6].second);

		char keyChk[32+1] = {0};
		memset(keyChk, 0, sizeof(keyChk));

		ret = PinPadWriteWorkKeyWithMainkey(csport, "", mainkeyIndex, workkeyIndex, newKeyLen, newKeyBySM4Key, keyChk);

		inList.clear();

		if (ret < 0)
		{
			sprintf(ErrDesc, "���̺���PinPadWriteWorkKeyWithMainkey���ر���");
			debug("���̺���PinPadWriteWorkKeyWithMainkey���ر���");
			return ret;
		}

		tmpStr.Format("%d", ret);
		PAIR returncode("returncode", tmpStr);
		inList.push_back(returncode);

		tmpStr.Format("%s", keyChk);
		PAIR Check("keyChk", tmpStr);
		inList.push_back(Check);
	}
	debug("����driveKeyBd����");
	return ret;
}


/********************************************
* ��  �ܣ�ȡ�õ����ṹ��ָ��
* ��  ������Ҫ�����Ľṹ��ָ��
* ����ֵ���ɹ�����TRUE,ʧ�ܷ���FALSE
*********************************************/
BOOL WINAPI GetSFKBCOM(KB_ADAPTER_INTERFACE *pInterf)
{
	if(pInterf)
	{
		pInterf->SFKBLoadDrv = SFKBLoadDrv;
		pInterf->SFKBFreeDrv = SFKBFreeDrv;
		pInterf->SFKBReadPin = SFKBReadPin;
		pInterf->SFKBReadPin2 = SFKBReadPin2;
		pInterf->SFKBLoadMasterKey = SFKBLoadMasterKey;
		pInterf->SFKBLoadWorkKey = SFKBLoadWorkKey;
		pInterf->SFKBLoadWorkKey2 = SFKBLoadWorkKey2;
		pInterf->SFKBGenerateMAC = SFKBGenerateMAC;
		pInterf->SFKBVerifyMAC = SFKBVerifyMAC;
		pInterf->KPGetPinPadID = KPGetPinPadID;
		pInterf->PinPadReadEvaluate = PinPadReadEvaluate;
		pInterf->driveKeyBd = driveKeyBd;
		return TRUE;
	}
    return FALSE;
}


void debug(CString s)
{
#ifdef _INFO
	
	
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
	

#endif
}

void debug(const char* s,...)
{
#ifdef _INFO

	
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
	

#endif
}