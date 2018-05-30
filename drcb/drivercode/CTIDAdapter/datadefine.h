#pragma once

/**
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
*/

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



//20160408东莞柜面新增升腾二代证读卡适配

//读二代证
typedef long (WINAPI *lpCT_ReadIDCard)(const char *, const char *, const long, const long, char *, PERSONINFO *);

//20170717东莞农商行新增外国人永久居留身份证
typedef long (WINAPI *lpCT_ReadIDCardForeigner)(const char *, const char *, const long, const long, char *, PERSONINFO *);

//保存图片
typedef long (WINAPI *lpCT_SaveImg)(int, const PERSONINFO *, char *);
//保存头像图片
typedef long (WINAPI *lpCT_SaveHeadImg)(const PERSONINFO *, char *);
//保存正面图片
typedef long (WINAPI *lpCT_SaveFrontImg)(const PERSONINFO *, char *);
//保存正面图片
typedef long (WINAPI *lpCT_SaveBackImg)(const PERSONINFO *, char *);
//保存整张二代证图片
typedef long (CALLBACK *lpCT_SaveCardImg)(const PERSONINFO *, char *);

#define CAMERELOG_DIR              "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);