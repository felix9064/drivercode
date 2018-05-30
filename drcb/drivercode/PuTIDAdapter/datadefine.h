#pragma once

#define TYPE_IDCARD 1//证件类型：身份证
#define TYPE_PRCARD 2//证件类型：外国人永久居留证

//身份证文字信息
typedef struct _personinfow
{
	wchar_t name[16];
	wchar_t sexCode[2];
	wchar_t sex[2];
	wchar_t ethnicCode[4];
	wchar_t ethnic[10];
	wchar_t birthDate[10];
	wchar_t address[36];
	wchar_t idNumber[20];
	wchar_t issuer[16];
	wchar_t effectDate[10];
	wchar_t expiryDate[10];
	wchar_t appendInfo[36];
}PERSONINFOW, *PPERSONINFOW;

//外国人永久居留证文字信息
typedef struct _foreignerinfow
{
	wchar_t englishName[62];
	wchar_t sexCode[2];
	wchar_t sex[2];
	wchar_t prNumber[16];
	wchar_t countryCode[4];
	wchar_t country[16];
	wchar_t chineseName[16];
	wchar_t issueDate[10];
	wchar_t expiryDate[10];
	wchar_t birthDate[10];
	wchar_t version[4];
	wchar_t issuerCode[6];
	wchar_t issuer[16];
}FOREIGNERINFOW, *PFOREIGNERINFOW;

/*
typedef struct PERSONINFO
{
	char name[80];           //姓名
	char EngName[80];          // 英文姓名
	char version[10];           //证件版本号
	char govCode[10];           //单次申请受理机关代码
	char cardType[10];          //证件类型标志
	char otherData[10];         //预留项s
	char sex[10];           //性别
	char nation[50];        //民族
	char birthday[30];       //出生日期（如20041101）
	char address[180];      //地址
	char cardId[50];        //身份证号或者永久居留证号码
	char police[80];        //签发机关
	char validStart[30];     //有效期起始(如20041101)
	char validEnd[30];      //有效期终止(如20141101)
	char sexCode[10];      //性别代码
	char nationCode[10];    //民族代码或者国籍、地区代码
	char appendMsg[180];  //新住址
	int  iFlag;            //标志位，0—普通居民二代证，1—外国人永久居住证
}PERSONINFO;
*/

typedef int (__stdcall *lpIdcrReadCardInfo)(int iPort, DWORD dwBaudRate, int iBpPort, int *piCardType, PPERSONINFOW pPerson, PFOREIGNERINFOW pForeigner, LPCWSTR pszPhotoPath);


#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);
static BOOL WCharToMByte(LPCWSTR lpcwszStr, char *lpszStr);