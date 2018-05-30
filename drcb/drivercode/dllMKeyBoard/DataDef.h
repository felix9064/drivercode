#pragma once

#define CAMERELOG_DIR              "log"

#define KEYBORD_TYPE    0  //0--明文  1-加密型密码键盘
#define ENCYPTMODE	2
#define DEVICEINFO          "deviceinfo"
#define PORTTYPE            "porttype"
#define PORT                "port"
#define BAUDRATE            "baudrate"
#define PRITY               "prity"
#define BYTESIZE            "bytesize"
#define STOPBITS            "stopbits"
#define HANDSHAKE           "handshaking"
#define DEFAULTTIMEOUT      "deftimeout"

#define HASADAPTER          "hasadapter"
#define COM                 "COM"
#define USB                 "USB"
#define LPT                 "LPT"

#define CONTROL             "control"

#define GreenOn               "GreenOn"
#define GreenOff              "GreenOff"
#define ReadOn                 "ReadOn"

#define STEP                   "step"
#define NAME                   "name"
#define FLAG                   "flag"
#define DO                     "do"
#define DATA                   "data"
#define DELHEADNUM             "delheadnum"
#define DELENDNUM              "delendnum"
#define STARTCHAR              "startchar"
#define ENDCHAR                "endchar"
#define KEY_MAP_TABLE          "key_map_table"
#define KEYMAP                 "keymap"
#define DEV_DEAL               "dev_deal"
#define FIRST_INPUT            "first_input"
#define SECOND_INPUT           "second_input"
#define ENTER                  "enter"
#define CLEAR                  "clear"
#define WRITE                  "write"
#define READ                   "read"
#define RESET                  "reset"

#define ADAPTERINFO            "adapterinfo"
#define CALLDLL                "calldll"
#define THIRDDLL               "thirddll"

#define DEVOUTCHAR             "devoutchar"
#define MAP                    "map"

#define BUFSIZ_256             256

#define TIME_OUT 20
#define KBLOG_DIR              "log"
typedef struct _stKeyBdDoStep
{
    CString name;
	CString oper;
	short flag;
    CString writedata;
    CString startchar;
	CString endchar;
	CString enter;
	CString clear;
	short keymap;
	short reset;
}stKeyBdDoStep;

typedef vector<stKeyBdDoStep> VStepList;

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
	ERR_LENNOTENOUGH = -9,
	ERR_TWICEPASSWORD = -10,
	ERR_INVALIDORDER = -11,
	ERR_NOFINDCONF = -12,
	ERR_NOFINDMATCHDLL = -13,
	ERR_READPINFAIL = -14,
	ERR_WRITEMWKEY_FAIL = -15,
	ERR_RESET = -17,
	ERR_LOADDRV = -18,
	ERR_FINDINTERFACE=-19,
	ERR_INVOKED = -20,
	ERR_CANCEL = -21,
	ERR_ARGSERROR = -22
}enRetCode;



