#pragma once

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



//20160517��ݸ��������ʵ�����֤��������

//������֤
typedef int (WINAPI *lpSD_ReadIDCard)(int, char *, int, int, const char *, PERSONINFO *);


#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);