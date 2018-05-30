#pragma once

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
#define DEFTIMEOUT          "deftimeout"

#define CONTROL             "control"
#define RESET               "reset"
#define RDH                 "rdh"
#define SEPARATOR23         "cm23"
#define RDT                 "rdt"
#define WBUFFST             "wrh"
#define WBUFFED             "wrt"
#define RD2                 "rd2"
#define RD3                 "rd3"
#define RD23                "rd23"
#define GETSTATUS           "getstatus"
#define OK2                 "ok2"
#define OK3                 "ok3"
#define OK23                "ok23"
#define ERRSYMBOL           "errsymbol"
#define RGETSTATUS           "rgetstatus"
#define ROK2                 "rok2"
#define ROK3                 "rok3"
#define ROK23                "rok23"
#define RERRSYMBOL           "rerrsymbol"
#define WGETSTATUS           "wgetstatus"
#define WOK2                 "wok2"
#define WOK3                 "wok3"
#define WOK23                "wok23"
#define WERRSYMBOL           "werrsymbol"


#define MAG_READ             "mag_read"
#define MAG_WRITE            "mag_write"
#define TRACKREAD            "trackread"
#define TRACKWRITE           "trackwrite"
#define WRITE                "write"
#define READ                 "read"
#define TRACKNO              "trackno"
#define STEP                 "step"
#define NAME                 "name"
#define DO                   "do"
#define WRITEDATA            "data"
#define WRITETYPE            "writetype"
#define READTYPE             "readtype"
#define COMPARTSTR           "compartstr"
#define STARTSTRING          "startstring"
#define ENDSTRING            "endstring"
#define RESET                "reset"
#define LEN                  "len"

#define MAG_FORMAT_REG       "mag_format_register"
#define SECOND_TRACK_LEN     "secondtracklen"
#define THIRD_TRACK_LEN      "thirdtracklen"
#define FLAGTK               "flagtk"
#define FLAGPOS              "flagpos"
#define FLAGVAL              "flagval"
#define FORMAT               "format"

#define ITEM                 "item"
#define SUBITEM              "subitem"
#define NAME                 "name"
#define TRACK                "track"
#define STARTPOS             "startpos"
#define TYPE                 "type"
#define LEN                  "len"
#define VALUE                "value"
#define OUTOFVAULUE          "outofvalue"
#define FROM                 "from"

#define READLIST 1
#define WRITELIST 2

#define TIME_OUT  15
typedef struct _stOperStep
{
	CString name;
	CString opername;
	CString writedata;
	short wrtype;
	short reset;
    CString compartstr;
	CString startstr;
	CString endstr;
	short len;
}stOperStep;
typedef vector<stOperStep> VStepList;

typedef struct _stRWNode
{
    short trackno;
    VStepList *pList;
}stRWNode;
typedef vector<stRWNode> VRWList;

typedef struct _stMagFormat
{
	CString name;          //�ֶ�����
	short track;           //�ŵ���
	short startps;         //��ʼλ��
	short len;             //����
    short type;            //����
    CString value;         //�̶�ֵ
    CString outofval;      //�ⲿֵ
    short from;            //���ⲿֵʲôλ��ȡ
}stMagFormat;

typedef vector<stMagFormat> areaList;

typedef struct _stMagFormatTp
{
	CString magname;
    areaList *pList;
}stMagFormatTp;

typedef vector<stMagFormatTp> VMagTp;

typedef struct _stMagFormatReg
{
	CString name;
	short track2len;
	short track3len;
	short flagtk;
	short flagpos;
	CString flagval;
}stMagFormatReg;
typedef vector<stMagFormatReg> VRegList;

typedef enum _enWType
{
	WTYPE_SETDATAFROMCONF = 1,
		WTYPE_SETDATAFROMOUT = 2
}WTYPE;

typedef enum _enRType
{
	RTYPE_GETTRACKDATA = 1,
		RTYPE_GETOPERSTATUS = 2
}RTYPE;

typedef enum _enAreaType
{
    FIXVAL = 1,
    OUTOFVAL = 2,
    SUBVAL = 3,
    VERIFYVAL = 4,
    TWOTRACKVAL = 5,
    THREETRACKVAL = 6,
    SEPARVAL = 7,
	ONLYGET = 8
}enAreaType;

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
	ERR_INVALIDORDER = -9,
	ERR_MISSORDER = -10,
	ERR_WRITETRACK = -11,
	ERR_READTRACK = -12,
	ERR_LOADCONFIGS = -13,
	ERR_OCREXCEPTION = -14,
	ERR_NOFINDDATA = -15,
	ERR_NOFINDCONF = -16,
	ERR_RESET = -17,
	ERR_WRITETIMEOUT = -18,
	ERR_NOWRITEDATA = -19,
	ERR_OTHER_BANK_CARD_ABSCENT_DATA = -20,
	ERR_PASSBK_OLD2_ABSCENT_DATA = -21,
	ERR_PASSBK_OLD1_ABSCENT_DATA = -22,
	ERR_PASSBK_NEW_ABSCENT_DATA = -23,
	ERR_CARD_ABSCENT_DATA = -24,
	ERR_CONSTRUCTREGLIST = -25,
	ERR_NOFINDMATCHDLL = -26,
	ERR_READFAIL = -27
	//ERR_NOTRACK3 = 2
}enRetCode;

//2012-08-21���Ĵ�����ʽ�жϹ���,�Դ�����ʽ�ĺ궨��
#define OTHER_BANK_CARD  "otherbankcard"       //���п�
#define OTHER_BANK_BOOK  "otherbankbook"       //������
#define EMPTY_MAG  "emptyMag"       //�մŵ�
#define PASSBK_NEW       "newdeposit"          //��ϵͳ����
#define PASSBK_OLD1      "olddeposit10"        //��ϵͳ����1-2λ10
#define PASSBK_OLD2      "olddeposit2"        //��ϵͳ����1-2λ2'
#define CARD             "card"                //һ��ͨ
#define GYCARD           "guiyuanCard"        //��Ա��
#define STOCK_CARD		 "stockCard"			//�ɽ�֤
#define VIP_CARD		 "vipCard"				//��λ�����
#define IDENTITY_CARD	 "identityCard"			//���ʶ��
#define HOUSE_FUND		 "housefundbook"	//��άר����
#define PASSBK_COMPANY   "companypassbook"  //�Թ�����
#define PASSBK_PERSON    "personpassbook"   //��˽����
#define VERIFY_FAIL_BOOK    "verifyFailBook"   //У��ʧ�ܵĴ���


