#pragma once
#include <vector>
#include <utility>
using namespace std;
/*
  加密键盘适配标准接口
*/

typedef vector< pair<CString, CString> > VPair;
typedef const char* TPCHR;
typedef char*       PCHR;
typedef HINSTANCE HDLL;

typedef struct _stKBAdapter
{
    /********************************************
    * 功  能：加载驱动库
    * 参  数：@path - 库所在路径
    * 返回值：成功返回非0,否则0
	*********************************************/
	HDLL (WINAPI *SFKBLoadDrv)(TPCHR path, PCHR ErrDesc);

    /********************************************
    * 功  能：释放驱动库
    * 参  数：@hDrv - 驱动库句柄
    * 返回值：无
	*********************************************/
	void (WINAPI *SFKBFreeDrv)(HDLL hDrv);

    /********************************************
    * 功  能：加密Pin
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号 5 - 密码长度
    * 参  数：@MKeyIdx - 主密钥索引号
    * 参  数：@WKeyIdx - 工作密钥索引号
	* 参  数：@ACCTNO  - 客户帐号
	* 参  数：@mode    - 语音模式 1：请输入密码。2：请再次输入密码
	* 参  数：@PIN     - 传出参数，Pin密文
	* 参  数：@TPKCheck - 传出参数，加密PIN的TPK密钥校验值。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBReadPin)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR ACCTNO, int mode, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc);

    /********************************************
    * 功  能：加载TMK到密码键盘。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@MKeyIdx - 主密钥索引号
	* 参  数：@lpOleMKey  - 主密钥串
	* 参  数：@lpMKey  - （无用参数）
	* 参  数：@mode    - 主密钥串状态。0 - 明文状态 1 - 密文状态
	* 参  数：@lpKeyCK - 密钥校验值，传入传出参数。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBLoadMasterKey)(HDLL hDrv, VPair &inList, int MKeyIdx, TPCHR lpOldMKey, TPCHR lpMKey, int mode, PCHR lpKeyCK, PCHR ErrDesc);

    /********************************************
    * 功  能：加载TPK/TAK到密码键盘。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@MKeyIdx - 主密钥索引号
	* 参  数：@WKeyIdx - 工作密钥索引号
	* 参  数：@lpMKey  - 工作密钥串
	* 参  数：@lpKeyCK - 密钥校验值。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBLoadWorkKey)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpWKey, TPCHR lpKeyCK, PCHR ErrDesc);

    /********************************************
    * 功  能：生成MAC。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@MKeyIdx - 主密钥索引号
	* 参  数：@WKeyIdx - 工作密钥索引号
	* 参  数：@lpData  - mac数据
	* 参  数：@lpMAC  - 传出参数，MAC值。
	* 参  数：@lpTAKKCK - 传出参数，TAK校验值。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBGenerateMAC)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, PCHR lpMAC, PCHR lpTAKKCK, PCHR ErrDesc);

    /********************************************
    * 功  能：验证MAC。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
	* 参  数：@MKeyIdx - 主密钥索引号
	* 参  数：@WKeyIdx - 工作密钥索引号
	* 参  数：@lpData  - mac数据
	* 参  数：@lpMAC  - MAC值。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBVerifyMAC)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR lpData, TPCHR lpMAC, PCHR ErrDesc);

}KB_ADAPTER_INTERFACE;

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetSFKBCOM(KB_ADAPTER_INTERFACE *pInterf);

typedef BOOL (CALLBACK * LPGetSFKBCOM)(KB_ADAPTER_INTERFACE *);