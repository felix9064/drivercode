// GmtDriver.h: interface for the CGmtDriver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_)
#define AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 1.ȡ������״̬
typedef int (__stdcall *FUNC_GWQ_ClosePinPad)(int iPortNo, char extendPort, int iBaudRate);
// 2.�������볤��
typedef int (__stdcall *FUNC_GWQ_SetCodeLen)(int iPortNo, char extendPort, int iBaudRate, int iPinLen);
// 3.��ȡ�Ǽ�������
typedef int (__stdcall *FUNC_GWQ_NonEpt_ReadPin)(int iPortNo, char extendPort, int iBaudRate, int iVoiceType, int iTimeOut, char *Pin, int iPinSize);
// 4.��ʼ���������
typedef int (__stdcall *FUNC_GWQ_InitPinPad)(int iPortNo, char extendPort, int iBaudRate);
// 5.��������Կ
typedef int (__stdcall *FUNC_GWQ_UpdateMKey)(int iPortNo, char extendPort, int iBaudRate, int ZmkIndex, int ZmkLength, char *Zmk, char *CheckValue);
// 6.���ع�����Կ
typedef int (__stdcall *FUNC_GWQ_DownLoadWKey)(int iPortNo, char extendPort, int iBaudRate, int MKeyIndex, int WKeyIndex, int WKeyLength, char *Key, char *CheckValue);
// 7.�������Կ
typedef int (__stdcall *FUNC_GWQ_ActiveWKey)(int iPortNo, char extendPort, int iBaudRate, int MKeyIndex, int WKeyIndex);
// 8.��ȡ�ͻ�����(DES)
typedef int (__stdcall *FUNC_GWQ_GetDES)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, int iVoiceType, int EndType, char *PinCrypt);
// 9.��ȡ�ͻ�����(ANSI98)
typedef int (__stdcall *FUNC_GWQ_GetPin)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, char *AccNo, int iVoiceType, int EndType, char *PinCrypt);
// 10.������Ϣ����
typedef int (__stdcall *FUNC_GWQ_StartInfoHtml)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, int modex, char *Info, int *iDisplayResult);
// 11.д���豸��
typedef int (__stdcall *FUNC_GWQ_WriteDeviceId)(int iPortNo, char extendPort, int iBaudRate, char *pDeviceId);
// 12.��ȡ�豸��
typedef int (__stdcall *FUNC_GWQ_ReadDeviceId)(int iPortNo, char extendPort, int iBaudRate, char *pDeviceId, int iSize);
// 13.���ּ���
typedef int (__stdcall *FUNC_GWQ_StartKeyboard)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, int numType, char *iDisplayResult);
// 14.�������ӹ���
typedef int (__stdcall *FUNC_GWQ_StartElectronicCard)(int iPortNo, char extendPort, int iBaudRate, char *tellerName, char *tellerNo, int nStarLevel, char *headFile, int iTimeOut);
// 15.����Ա��ͷ��
typedef int (__stdcall *FUNC_GWQ_DownHeadFile)(int iPortNo, char extendPort, int iBaudRate, char *pFilePath);
// 16.��ѯԱ��ͷ��
typedef int (__stdcall *FUNC_GWQ_FindHeadPhoto)(int iPortNo, char extendPort, int iBaudRate, char *pHeadPhoto);
// 17.ɾ��Ա��ͷ��
typedef int (__stdcall *FUNC_GWQ_DelHeadFile)(int iPortNo, char extendPort, int iBaudRate, char *pFilename);
// 18.���Ż�ӭ����
typedef int (__stdcall *FUNC_GWQ_Welcome)(int iPortNo, char extendPort, int iBaudRate);
// 19.���ù�Ա��Ϣ
typedef int (__stdcall *FUNC_GWQ_SetEvaluateInfo)(int iPortNo, char extendPort, int iBaudRate, char *tellerName, char *tellerNo, int nstarLevel, char *headFile);
// 20.��������
typedef int (__stdcall *FUNC_GWQ_StartEvaluate)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, int *evalValue);
// 21.��������
typedef int (__stdcall *FUNC_GWQ_CancleEvaluate)(int iPortNo, char extendPort, int iBaudRate);
// 22.��ȡ��ý���б�
typedef int (__stdcall *FUNC_GWQ_GetVideoList)(int iPortNo, char extendPort, int iBaudRate, int format, char *pFilelist, int nSize);
// 23.���ض�ý���ļ�
typedef int (__stdcall *FUNC_GWQ_DownVideoFile)(int iPortNo, char extendPort, int iBaudRate, int format, char *pFilename);
// 24.ɾ����ý���ļ�
typedef int (__stdcall *FUNC_GWQ_DelVideoFile)(int iPortNo, char extendPort, int iBaudRate, int format, char *pFilename);
// 25.ɾ��ȫ����ý���ļ�
typedef int (__stdcall *FUNC_GWQ_DelAllVideoFile)(int iPortNo, char extendPort, int iBaudRate, int format);
// 26.�汾����
typedef int (__stdcall *FUNC_GWQ_VsUpgrade)(int iPortNo, char extendPort, int iBaudRate, char *pFilename);
// 27.�汾��Ϣ��ѯ
typedef int (__stdcall *FUNC_GWQ_GetVer)(int iPortNo, char extendPort, int iBaudRate, char *VerInfo);
// 28.��ѯ�ļ���Ϣ(�ļ���С��MD5)
typedef int (__stdcall *FUNC_GWQ_QueryFileInfo)(int iPortNo, char extendPort, int iBaudRate, int format, char *FileName, char *FileInfo);
// 29.��沥��ʱ������
typedef int (__stdcall *FUNC_GWQ_SetTimeValue)(int iPortNo, char extendPort, int iBaudRate, int timeValue);
// 30.չʾPDF�ļ�
typedef int (__stdcall *FUNC_GWQ_CallShowPDF)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, int signMode, char *pdfPath, char *location, char *imgPath, char *textPath);
// 31.���ü���ģʽ
typedef int (__stdcall *FUNC_GWQ_SetMD)(int iPortNo, char extendPort, int iBaudRate, int mformat);
// 32.�����������Կ����
typedef int (__stdcall *FUNC_GWQ_GetRandomZMK)(int iPortNo, char extendPort, int iBaudRate, int iTimeOut, char *PubKey, int SuanfaType, char *CryptZMK);

class CWin32Port;
class CParaXml;


class CGmtDriver  
{
public:
	CGmtDriver();
	virtual ~CGmtDriver();
public:
	static HINSTANCE m_hLibModule;
	static bool m_bDriverLoaded;
	static FILE * m_stream;
	static void runLog(CString logStr, CString logdir = "C:\\");
public:
	int LoadDriver();
	void RealseDriver();
	bool IsDriverLoaded(){return m_bDriverLoaded;}
	void SetModuleDir(CString dir);
	CString& GetModuleDir();
	VPair& GetRetListRef();
public:
	FUNC_GWQ_ClosePinPad p_GWQ_ClosePinPad;
	FUNC_GWQ_SetCodeLen p_GWQ_SetCodeLen;
	FUNC_GWQ_NonEpt_ReadPin p_GWQ_NonEpt_ReadPin;
	FUNC_GWQ_InitPinPad p_GWQ_InitPinPad;
	FUNC_GWQ_UpdateMKey p_GWQ_UpdateMKey;
	FUNC_GWQ_DownLoadWKey p_GWQ_DownLoadWKey;
	FUNC_GWQ_ActiveWKey p_GWQ_ActiveWKey;
	FUNC_GWQ_GetDES p_GWQ_GetDES;
	FUNC_GWQ_GetPin p_GWQ_GetPin;
	FUNC_GWQ_StartInfoHtml p_GWQ_StartInfoHtml;
	FUNC_GWQ_WriteDeviceId p_GWQ_WriteDeviceId;
	FUNC_GWQ_ReadDeviceId p_GWQ_ReadDeviceId;
	FUNC_GWQ_StartKeyboard p_GWQ_StartKeyboard;
	FUNC_GWQ_StartElectronicCard p_GWQ_StartElectronicCard;
	FUNC_GWQ_DownHeadFile p_GWQ_DownHeadFile;
	FUNC_GWQ_FindHeadPhoto p_GWQ_FindHeadPhoto;
	FUNC_GWQ_DelHeadFile p_GWQ_DelHeadFile;
	FUNC_GWQ_Welcome p_GWQ_Welcome;
	FUNC_GWQ_SetEvaluateInfo p_GWQ_SetEvaluateInfo;
	FUNC_GWQ_StartEvaluate p_GWQ_StartEvaluate;
	FUNC_GWQ_CancleEvaluate p_GWQ_CancleEvaluate;
	FUNC_GWQ_GetVideoList p_GWQ_GetVideoList;
	FUNC_GWQ_DownVideoFile p_GWQ_DownVideoFile;
	FUNC_GWQ_DelVideoFile p_GWQ_DelVideoFile;
	FUNC_GWQ_DelAllVideoFile p_GWQ_DelAllVideoFile;
	FUNC_GWQ_VsUpgrade p_GWQ_VsUpgrade;
	FUNC_GWQ_GetVer p_GWQ_GetVer;
	FUNC_GWQ_QueryFileInfo p_GWQ_QueryFileInfo;
	FUNC_GWQ_SetTimeValue p_GWQ_SetTimeValue;
	FUNC_GWQ_CallShowPDF p_GWQ_CallShowPDF;
	FUNC_GWQ_SetMD p_GWQ_SetMD;
	FUNC_GWQ_GetRandomZMK p_GWQ_GetRandomZMK;

public:
	
	int InitlizeConfigParam();
	void SetConfXmlPath(CString &path);	

	int OpenGmtDriver();

	
	void SetPortNo(CString port);
	void SetTimeOut(short timeout);
	void SetTimeOutZero();
	CString & GetPortNo();

	int getNport();

	CString &GetStatusDesc() { return _statusdesc; }
	CString &GetErrCd() { return _errcd; }
	CString &GetErrMsg() { return _errmsg; }
public:
	int driveGmt(int optype, VPair &readlist);
public:
	CWin32Port *_pWinPort;
    CParaXml  *_pParaXml;
	CString _confxmlpath;
	CString _portno;
	enRetCode _RetCode;
	VPair _Sub;
	CString _statusdesc;
	CString _moduledir;
	CString _errcd;
	CString _errmsg;

	CString _CallDll;
	CString _ThirdDll;
	CString _IDPhotoName;
	BOOL _bThirdCall;
	short _isCom;
	short _nPort;
	short _ndeftimeout;
	short _ntimeout;
	int m_baudrate;

};

#endif // !defined(AFX_GMTDRIVER_H__18F9BA1B_D4CB_4A78_9C2A_496D069D2D19__INCLUDED_)
