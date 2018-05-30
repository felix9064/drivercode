#pragma once
/*
typedef int (CALLBACK *lpInitComm)( int);
typedef int (CALLBACK *lpAuthenticate)();
typedef int (CALLBACK *lpRead_Content)(int, char *);
typedef int (CALLBACK *lpRead_Content_Path)(char *, int);
typedef int (CALLBACK *lpCloseComm)();

#define SEXMAP      "sexmap"
#define NATIONMAP   "nationmap"
#define IDCODE      "IDCODE"
#define APPCODE     "appcode"
*/

// 20160321 新增南天BP8903-M身份证读卡器适配
//typedef int (__stdcall *lpIsDevConnect)(int, int, const char *);
typedef int (__stdcall *lpIDCard_GetInformation)(int, int, const char *, int, char *, const char *, const char *, char *, char *, char *);
// 20170717 新增外国人永久居留身份证读取适配器
typedef int (__stdcall *lpGetForeignInfo)(int, int, int, char *, char *, char *);


#define CAMERELOG_DIR    "log"

static void runLog(CString logStr, CString logdir = "C:\\");
static void deleteAllFile(CString dirname);