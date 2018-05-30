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
	//加载磁条读写器配置文件
	BOOL LoadMagConfXml(CString &filepath);

	//获取操作端口的参数值
	int GetPortParam(int &port, int &BaudRate, char &Parity, 
		             char &ByteSize, char &StopBits, BOOL &isCOM, BOOL &hasAdap, short &TimeOut);
	//构造指令链表
	int InitContrOrder();
	int InitMapKeyList();
	//主要用于加密密码键盘，获取配置文件中的适配器和厂商dll
	int GetAdapterInfo(CString &adapter, CString &thirdll, BOOL &hasAdap);
	
	CString GetMapKey(char outchr);
	int ParaOrder(CString &old, char *pOutNew);
	void ParaStepNode(TiXmlElement *pElem);
    CString &VPairValue(const char *name, VPair &vlist);

	VStepList &GetStepListRef();

protected:
    TiXmlDocument *_pConfXmlDoc;

	VStepList _StepList;
	VPair _VMapKey;
	
}; 