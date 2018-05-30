#pragma once

#define TYPE_IDCARD 1//֤�����ͣ����֤
#define TYPE_PRCARD 2//֤�����ͣ���������þ���֤

//���֤������Ϣ
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

//��������þ���֤������Ϣ
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
	char name[80];           //����
	char EngName[80];          // Ӣ������
	char version[10];           //֤���汾��
	char govCode[10];           //��������������ش���
	char cardType[10];          //֤�����ͱ�־
	char otherData[10];         //Ԥ����s
	char sex[10];           //�Ա�
	char nation[50];        //����
	char birthday[30];       //�������ڣ���20041101��
	char address[180];      //��ַ
	char cardId[50];        //���֤�Ż������þ���֤����
	char police[80];        //ǩ������
	char validStart[30];     //��Ч����ʼ(��20041101)
	char validEnd[30];      //��Ч����ֹ(��20141101)
	char sexCode[10];      //�Ա����
	char nationCode[10];    //���������߹�������������
	char appendMsg[180];  //��סַ
	int  iFlag;            //��־λ��0����ͨ�������֤��1����������þ�ס֤
}PERSONINFO;
*/

typedef int (__stdcall *lpIdcrReadCardInfo)(int iPort, DWORD dwBaudRate, int iBpPort, int *piCardType, PPERSONINFOW pPerson, PFOREIGNERINFOW pForeigner, LPCWSTR pszPhotoPath);


#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);
static BOOL WCharToMByte(LPCWSTR lpcwszStr, char *lpszStr);