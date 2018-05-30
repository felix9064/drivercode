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
	//����IC��д�������ļ�
	BOOL LoadICConfXml(CString &filepath);

	//��ȡ�����˿ڵĲ���ֵ
	//int GetPortParam(int &port, BOOL &isCOM, BOOL &hasAdap, int &AID);
	int GetPortParam(int &port, int &BaudRate, char &Parity, char &ByteSize, char &StopBits, BOOL &isCOM,
		             BOOL &hasAdap, CString DefAID, int &DllType, char &ICFlag, char &BpNO, short &defaultTimeOut);

	//��Ҫ����IC����д������ȡ�����ļ��е��������ͳ���dll
	int GetAdapterInfo(CString &adapter, CString &thirdll, CString &etcdll);
	
	

protected:
    TiXmlDocument *_pConfXmlDoc;
	
}; 