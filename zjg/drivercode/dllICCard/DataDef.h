#pragma once

#define DEVICEINFO          "deviceinfo"
#define PORTTYPE            "porttype"
#define PORT                "port"
#define BAUDRATE            "baudrate"
#define PRITY               "prity"
#define BYTESIZE            "bytesize"
#define STOPBITS            "stopbits"
#define HANDSHAKE           "handshaking"
#define HASADAPTER          "hasadapter"
#define COM                 "COM"
#define USB                 "USB"
#define LPT                 "LPT"
#define DEFAID              "defaid"
#define DLLTYPE             "dlltype"
#define ICFLAG              "icflag"
#define BPNO                "bpno"
#define DEFAULTTIMEOUT      "deftimeout"

#define ADAPTERINFO         "adapterinfo"
#define CALLDLL             "calldll"
#define THIRDDLL            "thirddll"
#define ETCDLL				"etcdll"

typedef enum _enRetCode
{
    SUCCESS = 0,
	SUCCESS_RESET = 1,
	ERR_LOADCONF = -1,
	ERR_OPENPORT = -3,
	ERR_CLOSEPORT = -4,
	ERR_RELEASE = -5, //释放厂家动态库，关闭端口失败
	ERR_READCARDINFO = -6,
	ERR_PREUPDATE = -7,
	ERR_UPDATEICCARD = -8,
	ERR_ONLINEFAILED = -9,
	ERR_NOFINDCONF = -12,
	ERR_NOFINDMATCHDLL = -13,
	ERR_TIMEOUT = -14,
	ERR_LOADDRV = -15,  //加载动态库失败
	ERR_NODRV = -16,    //无第三方动态库
	ERR_RESET = -17,
	ERR_GETTHIRDDLL = -18,
	ERR_NOTHIRDCALL = -25,
	ERR_READICAOUNT = -26,
	ERR_NOICCARD = -27,
	ERR_POWERON = -28,
	ERR_ABCENTDATA = -29,
	ERR_GENARQC = -30,
	ERR_CTLSCRIPTDATA = -31,
	ERR_GETTRANDETAIL = -32,
	ERR_GETICINFOANDGENARQC = -33,
	ERR_POWEROFF = -34,
	ERR_NOSUCHMETHOD = -35,
	ERR_OTHERSERROR = -99
}enRetCode;

