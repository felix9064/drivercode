#include <vector>
using namespace std;
//加入WINAPI会发生异常,谨记:以后开发时先不要加调用约定,如果有问题再加.
typedef int ( *lpCMCC_ReadPinANSI98)(HANDLE hCommDev,const int MasterKeyIndex,
										   const int WorkKeyIndex,const char * accNo,
										   const int modex,int * pinLength,char * pinCryptogram,const int timeout);

typedef int ( *lpCMCC_LoadMasterKey)(HANDLE hCommDev,const int MasterKeyIndex,
										 const char * NewMasterKeyByOldMasterKey,const char *KeyValue,
										 const long time_out);

//typedef int ( *lpCMCC_SearchPinpad)(int startComport,int Baud,int Verify,char *SerialNo);
typedef HANDLE ( *lpOpenComm)(const char *PortName, int nBaudRate,int Verify);
typedef void ( *lpCloseComm)(HANDLE hCommDev);

typedef int ( *lpCMCC_SetPinlength)(HANDLE hCommDev,const int MaxLength,
										  const int MinLength,const int IsAutoReturn,const int time_out);

typedef int ( *lpCMCC_PinInit)(HANDLE hCommDev,const int time_out);

//typedef int ( *lpCMCC_InputMasterKey)(HANDLE hCommDev,const int MasterKeyIndex,
											//const char *OldMasterKey,const char * NewMasterKey,
											//char *KeyValue,const long time_out);

typedef int ( *lpCMCC_LoadKey)(HANDLE hCommDev,const int MasterKeyIndex,
									 const int WorkKeyIndex,const char * keyByMasterKey,
									 const char * KeyValue,const long time_out);

typedef int ( *lpCMCC_GenerateMAC)(HANDLE hCommDev, const int MasterKeyIndex, const int WorkKeyIndex,
										 const int  lenOfMacData, const char * macData, char * mac,const long time_out);
//东莞农商行
typedef int (WINAPI *lpReadPin)(int mode,int iTimeout,char* pPinData);
//信息交互接口
typedef int (WINAPI *lpViewMessage)(const int ViewType,const char* MsgRow1,const char* MsgRow2,const int timeout);
typedef int (WINAPI *lpOpenCommDG)(int icomPort);
typedef void (WINAPI *lpCloseCommDG)();
typedef int (WINAPI *lpSelectExport)(char Port);
typedef void (WINAPI *lpAbort)();
//东莞农商行加密键盘
typedef int  (*_KPGetPinPadID)(const char * CommPort, const char *ExtPort,char *idnum);
typedef int (*PinPadInputKEY)(const char * CommPort, const char *ExtPort,const char * TMKIndex, const char * KEYIndex, const char * KeyLen, const char * keyByTMK, const char * KeyValue);
typedef int (*PinPadGuomiPin)(const char * CommPort,const char *ExtPort,const char * flag,const char *KeyIndex,const char * keytype,const char *accNo, const char *modex, const char *timeout,const char * MinPinlen,const char *MaxPinlen,char *pinCrypt);
typedef int (*PinPadGuomiPinNew)(const char * CommPort,const char *ExtPort, const char *srvType, const char * flag,const char *KeyIndex,const char * keytype,const char *accNo, const char *modex, const char *timeout,const char * MinPinlen,const char *MaxPinlen,char *pinCrypt);
typedef int (*PinPadWriteWorkKeyWithMainkey)(const char *CommPort,const char *ExtPort,const char * mainkeyIndex, const char *workkeyIndex, char *newKeyLen, char *newKeyByOldKey,char *keyChk);
typedef int (*_PinPadReadEvaluate)(const char * CommPort,const char * ExtPort,const char * VocFlag,const char * timeout);

//20180517东莞农商行密码键盘在线灌注密钥改造
typedef int (*FUNC_PinPadGetCompanyID)(const char *CommPort, const char *ExtPort, char *SeqNo, char *factoryID);
typedef int (*FUNC_PinPadEncBySm2_PK)(const char *CommPort, const char *ExtPort, char *ranKeyLen, char *pk, char *ciper, char *keyChk);
typedef int (*FUNC_PinPadWriteKeyWithSM4Ciper)(const char *CommPort, const char *ExtPort, const char *keyIndex, char *newKeyLen, char *newKeyByOldKey, char *keyChk);
typedef int (*FUNC_PinPadWriteWorkKeyWithMainkey)(const char *CommPort, const char *ExtPort, const char *mainkeyIndex, const char *workkeyIndex, char *newKeyLen, char *newKeyByOldKey, char *keyChk);

typedef pair<CString, CString> PAIR;
#define  BUFSIZ_256  256

CString genDisplayInfo(VPair &info);
int count(char* src,const char* delimiter);
void split(vector<CString> &ret ,char* src,const char* delimiter);




