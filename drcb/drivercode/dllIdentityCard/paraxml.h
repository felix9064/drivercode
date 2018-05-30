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
		             char &ByteSize, char &StopBits, short &isCOM,
					 CString &calldll, CString &thirddll, BOOL &bThirdcall, short &deftimeout);
	//����ָ������
	int InitContrOrder();
	int InitStatusTab();
	
	CString GetStatusDesc(char statuscode);
	int ParaOrder(CString old, char *pOutNew, int &len);
	void ParaStepNode(TiXmlElement *pElem, VFpCpy *pStepList);
    CString VPairValue(const char *name, VPair &vlist);

	VStepList &GetStepListRef();

	void FreeStepList();

protected:
    TiXmlDocument *_pConfXmlDoc;

	VStepList _StepList;
	VPair _VStatusTab;
	
}; 