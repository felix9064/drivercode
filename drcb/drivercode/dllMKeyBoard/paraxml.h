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
	//���ش�����д�������ļ�
	BOOL LoadMagConfXml(CString &filepath);

	//��ȡ�����˿ڵĲ���ֵ
	int GetPortParam(int &port, int &BaudRate, char &Parity, 
		             char &ByteSize, char &StopBits, BOOL &isCOM, BOOL &hasAdap, short &TimeOut);
	//����ָ������
	int InitContrOrder();
	int InitMapKeyList();
	//��Ҫ���ڼ���������̣���ȡ�����ļ��е��������ͳ���dll
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