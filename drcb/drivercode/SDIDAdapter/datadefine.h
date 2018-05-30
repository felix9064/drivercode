#pragma once

typedef struct tagPERSONINFO
{
	char name[32];       //姓名
	char sex[4];         //性别
	char nation[20];     //民族
	char birthday[12];   //出生日期（如20141101）
	char address[72];    //地址
	char cardId[20];     //身份证号
	char police[32];     //签发机关
	char validStart[12]; //有效期起始(如20141101)
	char validEnd[12];   //有效期终止(如20141101)
	char sexCode[4];     //性别代码
	char nationCode[4];  //民族代码
	char appendMsg[72];  //新住址
}PERSONINFO;



//20160517东莞柜面新增实达二代证读卡适配

//读二代证
typedef int (WINAPI *lpSD_ReadIDCard)(int, char *, int, int, const char *, PERSONINFO *);


#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);