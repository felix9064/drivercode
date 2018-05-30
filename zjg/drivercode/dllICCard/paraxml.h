#pragma once
using std::vector;
using std::pair;

class TiXmlDocument;

class CParaXml
{
public:
	CParaXml();
	~CParaXml();

public:
	//加载IC卡写器配置文件
	BOOL LoadICConfXml(CString &filepath);

	//获取操作端口的参数值
	//int GetPortParam(int &port, BOOL &isCOM, BOOL &hasAdap, int &AID);
	int GetPortParam(int &port, int &BaudRate, char &Parity, char &ByteSize, char &StopBits, BOOL &isCOM,
		             BOOL &hasAdap, CString DefAID, int &DllType, char &ICFlag, char &BpNO, short &defaultTimeOut);

	//主要用于IC卡读写器，获取配置文件中的适配器和厂商dll
	int GetAdapterInfo(CString &adapter, CString &thirdll, CString &etcdll);
	
	

protected:
    TiXmlDocument *_pConfXmlDoc;
	
}; 