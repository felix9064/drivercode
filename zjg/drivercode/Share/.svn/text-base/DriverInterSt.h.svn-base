#pragma once
#include <vector>
#include <utility>
using namespace std;

typedef pair<CString, CString> PAIR;
typedef vector< PAIR > VPair;
typedef vector< CString > VCStr;
typedef const char* TPCHR;
typedef char* PCHR;
typedef HMODULE HDLL;

typedef struct _stDriverInterface
{
    /********************************************
    * 功  能：打开设备，OpenDevice/CloseDevice需成对调用
    * 参  数：@modulepath - 动态库所在的目录路径
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *OpenDevice)(const char *modulepath);

	/********************************************
    * 功  能：设置设备操作超时时间
    * 参  数：@timeout - 设备操作超时时间。单位为秒
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *SetTimeOut)(int timeout);

    /********************************************
    * 功  能：关闭设备
    * 参  数：
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *CloseDevice)();

    /********************************************
    * 功  能：复位
    * 参  数：@rw - 读写类型。1-读，2-写
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *Reset)(int rw);

    /********************************************
    * 功  能：往设备中写数据
    * 参  数：@indata - 需要写入的数据二维指针
	          @size   - 写入数据的长度
			  @portno - 串并口号字符串指针
			  @conffile - 配置文件名称
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *Write)(char *indata[], int size, char *portno, char *conffile);

    /********************************************
    * 功  能：从设备中读数据
    * 参  数：@indata - 需要传入读参数二维指针
	          @size - 读取数据的长度
			  @portno - 串并口号字符串指针
			  @conffile - 配置文件名称
	          @ret  - 函数返回值，成功返回0，否则返回非0。
    * 返回值：成功返回数据串,否则返回NULL
	*********************************************/
	VPair & (WINAPI *Read)(char **indata, int *size, char *portno, char *conffile, int *ret);

    /********************************************
    * 功  能：获取设备类型
    * 参  数：
    * 返回值：成功返回0,否则非0
	*********************************************/
	int (WINAPI *GetDeviceType)();

    /********************************************
    * 功  能：获取设备驱动描述
    * 参  数：
    * 返回值：成功返回0,否则非0
	*********************************************/
	TCHAR * (WINAPI *GetDeviceDesc)();

	/********************************************
	* 功  能：取得当前错误详细描述
	* 参  数：无
	* 返回值：返回当前错误详细描述，无错误返回NULL
	*********************************************/
	PTCHAR (WINAPI *GetErrorDesc)();

	/********************************************
	* 功  能：版本兼容控制，返回插件接口版本号，判断本插件当前版本是否可用
	* 参  数：pFirst主版本号，pSecond次版本号，pFirst不相同则必不兼容，当pFirst相同，pSecond比容器大则不兼容，反之兼容
	* 返回值：成功返回0,否则非0
	*********************************************/
	long (WINAPI *GetPracticably)(long *pFirst,long *pSecond);

}DRIV_INTERFACE;

/********************************************
* 功  能：取得导出结构体指针
* 参  数：需要导出的结构体指针
* 返回值：成功返回TRUE,失败返回FALSE
*********************************************/
BOOL WINAPI GetDriverInterface(DRIV_INTERFACE *pDrivInterf);