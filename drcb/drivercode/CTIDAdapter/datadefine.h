#pragma once

/**
typedef struct tagPERSONINFO
{
	char name[32];       //����
	char sex[4];         //�Ա�
	char nation[20];     //����
	char birthday[12];   //�������ڣ���20141101��
	char address[72];    //��ַ
	char cardId[20];     //���֤��
	char police[32];     //ǩ������
	char validStart[12]; //��Ч����ʼ(��20141101)
	char validEnd[12];   //��Ч����ֹ(��20141101)
	char sexCode[4];     //�Ա����
	char nationCode[4];  //�������
	char appendMsg[72];  //��סַ
}PERSONINFO;
*/

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



//20160408��ݸ�����������ڶ���֤��������

//������֤
typedef long (WINAPI *lpCT_ReadIDCard)(const char *, const char *, const long, const long, char *, PERSONINFO *);

//20170717��ݸũ����������������þ������֤
typedef long (WINAPI *lpCT_ReadIDCardForeigner)(const char *, const char *, const long, const long, char *, PERSONINFO *);

//����ͼƬ
typedef long (WINAPI *lpCT_SaveImg)(int, const PERSONINFO *, char *);
//����ͷ��ͼƬ
typedef long (WINAPI *lpCT_SaveHeadImg)(const PERSONINFO *, char *);
//��������ͼƬ
typedef long (WINAPI *lpCT_SaveFrontImg)(const PERSONINFO *, char *);
//��������ͼƬ
typedef long (WINAPI *lpCT_SaveBackImg)(const PERSONINFO *, char *);
//�������Ŷ���֤ͼƬ
typedef long (CALLBACK *lpCT_SaveCardImg)(const PERSONINFO *, char *);

#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);