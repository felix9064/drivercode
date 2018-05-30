#ifndef DATADEF_IDSHENGT_20120905_H
#define DATADEF_IDSHENGT_20120905_H

#pragma once


//错误号含义
#define SUCESS 0          //成功
#define ERR_OPENPORT -1    //端口未打开
#define ERR_TRANSPORT -2   //dataBuf长度不够
#define ERR_ERRORDATA -3    //超时



#define CAMERELOG_DIR              "log"


/*
函数说明：读磁卡
参数：
	nPort	输入参数，设备连接端口。串口取值COM1，COM2，…… 当串口取值为：COM10时为蓝牙通信
									其他为SDT, HID , PCSC
	dwBaud	输入参数，串口波特率，当为串口时有效。
	chBpNo	输入参数，BP口，取值为"A"、 " B"'、 "C"、 "K"分别为A、B、C、K口, 取值为NULL或""表示不连接BP口。
	nCharset	输入参数，磁道格式：取值为0,1,2,3对应表示为0-ISO格式，1-IBM格式, 2-ISO格式(德卡)，3-IBM格式(德卡)
	nTimeout	输入参数，表示寻卡超时时间，单位为秒。
	szTrack1	输出参数，当szTrack1取值为NULL时，则不读1磁道，非NULL时则保存1磁道数据。
	nTrack1Len	输入参数，szTrack1的长度。
	szTrack2	输出参数，当szTrack2取值为NULL时，则不读2磁道，非NULL时则保存2磁道数据。
	nTrack2Len	输入参数，szTrack2的长度。
	szTrack3	输出参数，当szTrack3取值为NULL时，则不读3磁道，非NULL时则保存3磁道数据。
	nTrack3Len	输入参数，szTrack3的长度。
返回参数：
	0	 成功
	-1	 打开设备失败
	-2	 发送指令失败----与设备通讯失败
	-3	 接收数据失败----与设备通讯失败
	-4   接收到的报文格式错误
	-5	 读卡操作失败
	-6	 写卡操作失败
	-7	 用户取消操作
	-8	 超时
	-100 其他错误
*/
typedef long (CALLBACK *lpCT_ReadMsgCard)(const char* nPort, const int dwBaud, const char* chBpNo, const int nCharset, const int nTimeout,
										 char* szTrack1, const int nTrack1Len, char* szTrack2, const int nTrack2Len, char* szTrack3, const int nTrack3Len);


/*
函数说明：写磁卡
	nPort	输入参数，设备连接端口。串口取值COM1，COM2，…… 当串口取值为：COM10时为蓝牙通信
									其他为SDT, HID , PCSC
	dwBaud	输入参数，串口波特率，当为串口时有效。
	chBpNo	输入参数，BP口，取值为"A"、 " B"'、 "C"、 "K" 分别为A、B、C、K口，取值为NULL或""表示不连接BP口。
	nCharset	输入参数，磁道格式：取值为0,1,2,3对应表示为0-ISO格式，1-IBM格式, 2-ISO格式(德卡)，3-IBM格式(德卡)
	nTimeout	输入参数，表示寻卡超时时间，单位为秒。
	szTrack1	输入参数，当szTrack1取值为NULL时，则不写1磁道，非NULL时则将数据写入1磁道。
	szTrack2	输入参数，当szTrack2取值为NULL时，则不写2磁道，非NULL时则将数据写入2磁道。
	szTrack3	输入参数，当szTrack3取值为NULL时，则不写3磁道，非NULL时则将数据写入3磁道。

 返回参数：
	0	 成功
	-1	 打开设备失败
	-2	 发送指令失败----与设备通讯失败
	-3	 接收数据失败----与设备通讯失败
	-4   接收到的报文格式错误
	-5	 读卡操作失败
	-6	 写卡操作失败
	-7	 用户取消操作
	-8	 超时
	-100 其他错误
*/
typedef long (CALLBACK *lpCT_WriteMsgCard)(const char* nPort, const int dwBaud, const char* chBpNo, const int nCharset,
										  const int nTimeout, const char* szTrack1, const char* szTrack2, const char* szTrack3);


typedef int (CALLBACK *lpCT_CancelReadMsgCard)();



#endif