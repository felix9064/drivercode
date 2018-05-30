#pragma once

#define CAMERELOG_DIR              "log"



#define DEVICEINFO          "deviceinfo"
#define PORTTYPE            "porttype"
#define PORT                "port"
#define BAUDRATE            "baudrate"
#define PRITY               "prity"
#define BYTESIZE            "bytesize"
#define STOPBITS            "stopbits"
#define HANDSHAKE           "handshaking"
#define COM                 "COM"
#define LPT                 "LPT"
#define USB                 "USB"
#define THIRDCALL          "thirdcall"
#define CALLDLL            "calldll"
#define THIRDDLL            "thirddll"
#define DEFAULT_TIMEOUT     "default_timeout"
#define IDPHOTONAME         "idphotoname"

#define CONTROL             "control"

#define GreenOn               "GreenOn"
#define GreenOff              "GreenOff"
#define ReadOn                 "ReadOn"

#define STEP                   "step"
#define NAME                   "name"
#define WRITETYPE              "writetype"
#define READTYPE               "readtype"
#define DO                     "do"
#define DATA                   "data"
#define STARTSTRING            "startstring"
#define FPLENPOS               "fplenpos"
#define FPLENBYTENUM           "fplenbytenum"
#define FPDATAPOS              "fpdatapos"
#define HEADERLEN              "headerlen"
#define OKSTATUS               "okstatus"
#define STATUSPOS              "statuspos"
#define STATUSBYTENUM          "statusbytenum"
#define CHECKBYTENUM           "checkbytenum"
#define WRITE                  "write"
#define READ                   "read"
#define RESET                  "reset"

#define LOOP                   "loop"
#define TIMES                  "times"

#define FP_READ                 "fp_read"

#define STATUSTAB              "statustab"
#define STATUS                 "status"
#define CODE                   "code"
#define DESC                   "desc"

#define DEVOUTCHAR             "devoutchar"
#define MAP                    "map"

#define TIME_OUT 20

#define NAME_LEN 100
#define OPER_LEN 10
#define OKSTATUS_LEN 10
#define WRITEDATA_LEN 100
#define STARTSTR_LEN 50
/*
typedef struct _stFpDoStep
{
    CString name;
	CString oper;
	short times;
	short rwtype;
	short fplenpos;
	short fplenbytenum;
	short fpdatapos;
    short headerlen;
	CString okstatus;
	short statuspos;
	short statusbytenum;
	short checkbytenum;
    CString writedata;
    CString startstr;
	short reset;
	VOID *pList;
}stFpDoStep;
*/
typedef struct _stFpDoStepCpy
{
    char name[NAME_LEN];
	char oper[OPER_LEN];
	short times;
	short rwtype;
	short fplenpos;
	short fplenbytenum;
	short fpdatapos;
    short headerlen;
	char okstatus[OKSTATUS_LEN];
	short statuspos;
	short statusbytenum;
	short checkbytenum;
    char writedata[WRITEDATA_LEN];
    char startstr[STARTSTR_LEN];
	short reset;
	VOID *pList;
}stFpDoStepCpy;
typedef vector<stFpDoStepCpy> VFpCpy;

typedef struct _stFpDoStep
{
    char name[NAME_LEN];
	char oper[OPER_LEN];
	short times;
	short rwtype;
	short fplenpos;
	short fplenbytenum;
	short fpdatapos;
    short headerlen;
	char okstatus[OKSTATUS_LEN];
	short statuspos;
	short statusbytenum;
	short checkbytenum;
    char writedata[WRITEDATA_LEN];
    char startstr[STARTSTR_LEN];
	short reset;
	VFpCpy *pList;
}stFpDoStep;

typedef vector<stFpDoStep> VStepList;

typedef enum _enReadType
{
	READ_DATA = 1,
	READ_FP_DATA = 2    //读取指纹数据
}enReadType;

typedef enum _enWriteType
{
	WRITE_DATA = 1,
		WRITE_FP_DATA = 2    //注册指纹数据
}enWriteType;

typedef enum _enRetCode
{
    SUCCESS = 0,
	SUCCESS_RESET = 1,
    ERR_LOADCONF = -1,
	ERR_LOADFMT = -2,
	ERR_OPENPORT = -3,
	ERR_GETFMTLIST = -4,
	ERR_READTIMEOUT = -5,
	ERR_IOPEANDING = -6,
	ERR_READDATA = -7,
	ERR_SENDDATA = -8,
	ERR_DEVRESPONSE = -9,     //设备返回失败状态报错。
	ERR_WAITTIMEOUT = -10,
	ERR_INVALIDORDER = -11,
	ERR_NOFINDCONF = -12,
	ERR_DEVSENDDATA = -13,
	ERR_RESET = -17,
	ERR_NOFINDMATCHDLL = -18,
	ERR_READFAIL = -19,
	ERR_ABCENTPAMS = -20,
	ERR_ARGSERROR = -21,
	ERR_NOSUCHMETHOD = -35
}enRetCode;



