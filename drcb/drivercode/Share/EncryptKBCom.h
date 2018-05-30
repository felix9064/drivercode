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
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号 5-密码长度 6-波特率 7-密码输入次数
    * 参  数：@MKeyIdx - 主密钥索引号
    * 参  数：@WKeyIdx - 工作密钥索引号
	* 参  数：@ACCTNO  - 客户帐号
	* 参  数：@mode    - 加密方式 0 - 默认值des加密 1 - 不加密 2-des加密 3-3des加密
	* 参  数：@PIN     - 传出参数，Pin密文
	* 参  数：@TPKCheck - 传出参数，加密PIN的TPK密钥校验值。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SFKBReadPin)(HDLL hDrv, VPair &inList, int MKeyIdx, int WKeyIdx, TPCHR ACCTNO, int mode, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc);
	/*
	增加屏幕确认信息
	*/
	int (WINAPI *SFKBReadPin2)(HDLL hDrv, VPair &inList, TPCHR MKeyIdx, TPCHR WKeyIdx, TPCHR ACCTNO, PCHR PIN, PCHR TPKCheck, PCHR ErrDesc);

    /********************************************
    * 功  能：加载TMK到密码键盘。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号 5-密码长度 
	* 参  数：@MKeyIdx - 主密钥索引号
	* 参  数：@lpOleMKey  - 主密钥串
	* 参  数：@lpMKey  - （无用参数）
	* 参  数：@mode    - 加密方式。
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
    * 功  能：加载TPK/TAK到密码键盘。
    * 参  数：@inList - 采集所需数据。
	                    数组下标解释：0 - 模块所在目录 1 - 超时时间 2 - 第三方驱动库 3 - 端口类型 4 - 端口号
		TMKIndex	字符串，使用TMK的索引号，必须为3长度的字符串。取值范围为"001"-"007"。合法字符为0~9。例："001"。 
		KEYIndex	字符串，使用TPK/TAK的索引号，必须为2长度的字符串。取值范围为"01"-"02"，合法字符为0~2。"01"表示TPK，"02"表示TAK。
		KeyLen		字符串，密钥长度标志，只能取1/2/3，长度1字符。例"1"单倍长密钥，"2"双倍长密钥，"3"三倍长密钥。
		keyByTMK	字符串，用终端主密钥加密的密钥，长度为keyLen决定。长度可以取16/32/48。
		KeyValue	字符串，长度8字符，密钥明文加密64bit0取结果前部分。
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (*SFKBLoadWorkKey2)(HDLL hDrv, VPair &inList, const char *MKeyIdx, const char *WKeyIdx, const char * KeyLen, const char * keyByTMK, const char * KeyValue, PCHR ErrDesc);

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

	/********************************************
    * 功  能：复位
    * 参  数：@hDrv - 句柄
    * 返回值：无
	*********************************************/
	void (WINAPI *SFKBResetDev)(HDLL hDrv, VPair &inList,PCHR ErrDesc);

	/********************************************
    * 功  能：读取应用编号
    * 参  数：@hDrv - 句柄
    * 返回值：无
	*********************************************/
	int (*KPGetPinPadID)(HDLL hDrv, VPair &inList, PCHR result, PCHR ErrDesc);

	/********************************************
	功能：评价输入
    参数：
		CommPort，字符串，以空字符结束，使用串口的端口号。例："COM1"。
		ExtPort, 字符串，以空字符结束，使用串口扩展器时用。不使用串口扩展器时为NULL。
		VocFlag, 字符串，“1”:语音提示：请您对本次服务进行评价 
						“0”:蜂鸣器提示一声。 
		timeout		字符串，输入的等待时间，单位为秒。密码键盘需要有设置超时功能。
		返回：
		0		客户按下满意键
		1		客户按下一般键
		2		客户按下不满意键		
	*********************************************/
	int (*PinPadReadEvaluate)(HDLL hDrv, VPair &inList,PCHR ErrDesc);

	int (*driveKeyBd)(HDLL hDrv, VPair &inList, PCHR ErrDesc);
}KB_ADAPTER_INTERFACE;

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetSFKBCOM(KB_ADAPTER_INTERFACE *pInterf);

typedef BOOL (CALLBACK * LPGetSFKBCOM)(KB_ADAPTER_INTERFACE *);