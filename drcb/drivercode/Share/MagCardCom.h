#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
IC卡设备适配接口

*/

typedef vector< pair<CString, CString> > VPair;

typedef struct _stMagCard
{
	/********************************************
	* 功  能：读磁条。
	* 参  数：@outlist - 传出卡数据链表。
	* 参  数：@nPort - 端口号。
    * 参  数：@isUSB - 是否为USB设备。
	* 参  数：@errcd - 错误代码
	* 参  数：@errmsg - 错误信息。
	* 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *ReadCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);




	/********************************************
	* 功  能：写磁条。
	* 参  数：@outlist - 传入卡数据链表。
	* 参  数：@nPort - 端口号。
    * 参  数：@isUSB - 是否为USB设备。
	* 参  数：@errcd - 错误代码
	* 参  数：@errmsg - 错误信息。
	* 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *WriteCard)(short nPort, short isUSB, VPair &outlist, char *errcd, char *errmsg);
	/********************************************
	* 功  能：设备重置。
	* 参  数：无。
	* 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *ResetDev)();

}MAG_INTERFACE;

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetCardCOM(MAG_INTERFACE *pMagInterf);