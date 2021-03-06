#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
  指纹验证标准接口

  指纹验印在柜面中的应用，多数牵涉到其他软件公司的指纹验印系统。
  柜面将采集的的指纹数据，发往指纹验印系统进行验证。为了实现配置化，
  验印功能只需要实现_stFpMatch中的接口和配置好设备文件。如此就无需修改驱动。
*/

typedef vector< pair<CString, CString> > VPair;

typedef struct _stFpMatch
{
    /********************************************
    * 功  能：采集指纹模板接口。此接口提供给驱动调用。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 适配器库名称 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@pFpDat  - 指纹数据
	* 参  数：@pFpLen  - 指纹数据长度
	* 参  数：@errcd  - 错误代码
	* 参  数：@errmsg - 错误信息。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *GetFpTemplate)(VPair &inList, BYTE *pFpDat, DWORD *pFpLen, char *errcd, char *errmsg);

    /********************************************
    * 功  能：采集指纹特征接口。此接口提供给驱动调用。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 适配器库名称 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@pFpDat  - 指纹数据
	* 参  数：@pFpLen  - 指纹数据长度
	* 参  数：@errcd  - 错误代码
	* 参  数：@errmsg - 错误信息。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *GetFpData)(VPair &inList, BYTE *pFpDat, DWORD *pFpLen, char *errcd, char *errmsg);

    /********************************************
    * 功  能：验印接口。此接口提供给驱动调用。
    * 参  数：@inList - 验印所需数据。
	* 参  数：@errcd  - 错误代码
	* 参  数：@errmsg - 错误信息。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *Match)(VPair &inList, char *errcd, char *errmsg);

}FP_MATCH_INTERFACE;

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetFpCOM(FP_MATCH_INTERFACE *pFpInterf);