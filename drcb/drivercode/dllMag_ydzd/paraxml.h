#pragma once
using std::vector;
using std::pair;

//��������װ�ŵ�����ʱ�����ⲿ��������ֵ�����д��ʱ���Լ���װ�����ݣ��������ϲ���װ�õ�
#define HASOUTVALUE 1

class TiXmlDocument;

class CParaXml
{
public:
	CParaXml();
	~CParaXml();

public:
	//���ش�����д�������ļ�
	BOOL LoadMagConfXml(CString &filepath);
	//���ش�����д������xml�ļ�
	BOOL LoadMagFormatXml(CString &filepath);

	//��ȡ�����˿ڵĲ���ֵ
	int GetPortParam(int &port, int &BaudRate, char &Parity, 
		             char &ByteSize, char &StopBits, short &isCOM,
					 CString &calldll, CString &thirddll, BOOL &bThirdcall, short &deftimeout);
	//����ָ����������
	int InitContrOrder();
	//���������д����������
	int ConstructMagTypeList();
	//����������ʽ�Ǽ�����
	int ConstructRegList();

	int GetLenByTrack(int trackno, const char *magformat);

	VMagTp &GetMagTpVList();

	CString ComboxTrack(const char *formatname, int trackno, VPair &sub, int &hasOutvalue);
    CString VPairValue(CString &attrival, areaList *pList, VPair &vlist);

	CString GetCheckTrackData(char *pData, int type);
	int GetCheckNum(char * string);
	char * GetCardCheckNum(char * string);
	int GetBookAcctCheckNum(char *acctno, char *OutVal); //checkfun.cpp
	void FreeRWList();
	void FreeMagFormatList();
	VRWList &GetRListRef();
	VRWList &GetWListRef();
	VRegList &GetRegListRef();
	void ParaStepNode(TiXmlElement *pElem, VStepList *pList);
	int ParaOrder(CString &old, char *pOutNew);         //ʮ�������ַ���ת����ʮ����char
protected:
    TiXmlDocument *_pConfXmlDoc;
	TiXmlDocument *_pFormatXmlDoc;

	VRWList _RList;          //��������������
	VRWList _WList;          //д������������
	VMagTp _MagTypeList;     //������ʽ����
	VRegList _RegList;       //������ʽ�Ǽ�����
	
}; 