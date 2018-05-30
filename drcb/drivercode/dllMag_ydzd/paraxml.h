#pragma once
using std::vector;
using std::pair;

//表明在组装磁道数据时有由外部传过来的值，因此写磁时用自己组装的数据，而不用上层组装好的
#define HASOUTVALUE 1

class TiXmlDocument;

class CParaXml
{
public:
	CParaXml();
	~CParaXml();

public:
	//加载磁条读写器配置文件
	BOOL LoadMagConfXml(CString &filepath);
	//加载磁条读写器类型xml文件
	BOOL LoadMagFormatXml(CString &filepath);

	//获取操作端口的参数值
	int GetPortParam(int &port, int &BaudRate, char &Parity, 
		             char &ByteSize, char &StopBits, short &isCOM,
					 CString &calldll, CString &thirddll, BOOL &bThirdcall, short &deftimeout);
	//构造指令流程链表
	int InitContrOrder();
	//构造磁条读写器类型链表
	int ConstructMagTypeList();
	//创建磁条格式登记链表
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
	int ParaOrder(CString &old, char *pOutNew);         //十六进制字符串转换成十进制char
protected:
    TiXmlDocument *_pConfXmlDoc;
	TiXmlDocument *_pFormatXmlDoc;

	VRWList _RList;          //读磁条流程链表
	VRWList _WList;          //写磁条流程链表
	VMagTp _MagTypeList;     //磁条格式链表
	VRegList _RegList;       //磁条格式登记链表
	
}; 