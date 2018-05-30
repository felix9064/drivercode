#pragma once

typedef int ( * LPGETFPTEMPLATE)(int, int, BYTE *, DWORD *);
typedef int ( * LPGETFPDATA)(int , int , BYTE *, DWORD *);
typedef int ( * LPFPMATCH)(BYTE *, int ,BYTE *, int);
typedef char * ( * LPGETERRMESS)();


//20130902
typedef int (CALLBACK *FPIGetFeature)(int , char*, int*, char*, char*);
typedef int (CALLBACK *FPIVerifyComm)(char*, int, char*, char*, char*, char*, char*, char*, char*);

//20170807
typedef int (__stdcall *lpSCCBA_StartFinger)(int, char *, int, char *, int *, char *, int *, char *);


typedef pair<CString, CString> PAIR;