#include "StdAfx.h"
#include "paraxml.h"

CParaXml::CParaXml()
{
	_pConfXmlDoc = new TiXmlDocument();
	_pFormatXmlDoc = new TiXmlDocument();
	_RegList.clear();
	_MagTypeList.clear();
	_RList.clear();
	_WList.clear();
}

CParaXml::~CParaXml()
{
	if(_pFormatXmlDoc)
		delete _pFormatXmlDoc;

	if(_pConfXmlDoc)
		delete _pConfXmlDoc;
}

BOOL CParaXml::LoadMagConfXml(CString &filepath)
{
	return _pConfXmlDoc->LoadFile(filepath);
}

BOOL CParaXml::LoadMagFormatXml(CString &filepath)
{
	return _pFormatXmlDoc->LoadFile(filepath);
}


void CParaXml::FreeRWList()
{
	int i;
//	if(listtype == 1)
	{//释放读指令链表
		for(i = 0; i < _RList.size(); i++)
		{
			if(_RList[i].pList)
				delete _RList[i].pList;
		}

		_RList.clear();
	}
//	else
	{//释放写指令链表
		for(i = 0; i < _WList.size(); i++)
		{
			if(_WList[i].pList)
				delete _WList[i].pList;
		}
		_WList.clear();
	}
}

VRWList & CParaXml::GetRListRef()
{
	return _RList;
}

VRWList & CParaXml::GetWListRef()
{
	return _WList;
}

VRegList & CParaXml::GetRegListRef()
{
	return _RegList;
}

int CParaXml::GetPortParam(int &port, int &BaudRate, char &Parity, char &ByteSize, char &StopBits, short &isCOM, CString &calldll, CString &thirddll, BOOL &bThirdcall, short &deftimeout)
{
	if(_pConfXmlDoc == 0)
		return -1;           //文件没有加载

	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), DEVICEINFO) == 0 )
				{
                    TiXmlElement *pElem2 = elem->FirstChildElement();
					if(pElem2)
					{
						while (pElem2)
						{
							if(strcmp(pElem2->Value(), PORTTYPE) == 0)
							{
								if(strcmp(pElem2->GetText(), COM) == 0)
									isCOM = 1;
								if(strcmp(pElem2->GetText(), USB) == 0)
									isCOM = 2;
								else
									isCOM = 0;
							}
							else if(strcmp(pElem2->Value(), PORT) == 0)
							{
								port = atoi(pElem2->GetText());
							}
							else if(strcmp(pElem2->Value(), BAUDRATE) == 0)
							{
								BaudRate = atoi(pElem2->GetText());
							}
							else if(strcmp(pElem2->Value(), PRITY) == 0)
							{
								Parity = pElem2->GetText()[0];
							}
							else if(strcmp(pElem2->Value(), BYTESIZE) == 0)
							{
								ByteSize = pElem2->GetText()[0];
							}
							else if(strcmp(pElem2->Value(), STOPBITS) == 0)
							{
								StopBits = pElem2->GetText()[0];
							}
							else if(strcmp(pElem2->Value(), THIRDCALL) == 0)
							{
								if(strcmp(pElem2->GetText(),"false") == 0)
									bThirdcall = FALSE;
								else
									bThirdcall = TRUE;
							}
							else if(strcmp(pElem2->Value(), CALLDLL) == 0)
							{
								calldll = pElem2->GetText();
							}
							else if(strcmp(pElem2->Value(), THIRDDLL) == 0)
							{
								thirddll = pElem2->GetText();
							}
							else if (strcmp(pElem2->Value(), DEFTIMEOUT) == 0)
							{
								deftimeout = atoi(pElem2->GetText());
							}

							pElem2 = pElem2->NextSiblingElement();
						}
					}

					break;
				}
				elem = elem->NextSiblingElement();
			}
		}	
	}

	return 0;
}

void CParaXml::ParaStepNode(TiXmlElement *pElem, VStepList *pList)
{
	stOperStep step;
	step.wrtype = 0;
	step.reset = 0;
	step.len = 0;
	//获取每个subitem节点里的属性值
	TiXmlAttribute *pAttr = NULL;
	for(pAttr = pElem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
	{
		if(strcmp(pAttr->Name(), NAME) == 0)
		{
			step.name = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), DO) == 0)
		{
			step.opername = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), READTYPE) == 0)
		{
			step.wrtype = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), WRITETYPE) == 0)
		{
			step.wrtype = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), WRITEDATA) == 0)
		{
			step.writedata = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), STARTSTRING) == 0)
		{
			step.startstr = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), ENDSTRING) == 0)
		{
			step.endstr = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), COMPARTSTR) == 0)
		{
			step.compartstr = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), RESET) == 0)
		{
			step.reset = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), LEN) == 0)
		{
			step.len = atoi(pAttr->Value());
		}
	}

	if(pList)
		pList->push_back(step);
}

int CParaXml::ConstructRegList()
{
	_RegList.clear();

	TiXmlElement *pRoot = _pFormatXmlDoc->RootElement();
	TiXmlElement *elem = 0;
	TiXmlElement *pElem2 = 0;
	TiXmlElement *pElem3 = 0;
	TiXmlAttribute *pAttr = 0;

	BOOL bHasFormatNode = FALSE;
	if ( pRoot )
	{
		elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), MAG_FORMAT_REG) == 0 )
				{
					bHasFormatNode = TRUE;
                    pElem2 = elem->FirstChildElement();
					while (pElem2)
					{
						if(strcmp(pElem2->Value(), FORMAT) == 0)
						{
							stMagFormatReg regnode;
							regnode.track2len = 0;
							regnode.track3len = 0;
							regnode.flagpos = 0;
							regnode.flagtk = 0;
							
							//取出FORMAT属性
							pAttr = NULL;
							for(pAttr = pElem2->FirstAttribute(); pAttr; pAttr = pAttr->Next())
							{
								if(strcmp(pAttr->Name(), NAME) == 0)
									regnode.name = pAttr->Value();
								else if(strcmp(pAttr->Name(), SECOND_TRACK_LEN) == 0)
									regnode.track2len = atoi(pAttr->Value());
								else if(strcmp(pAttr->Name(), THIRD_TRACK_LEN) == 0)
									regnode.track3len = atoi(pAttr->Value());
								else if(strcmp(pAttr->Name(), FLAGTK) == 0)
									regnode.flagtk = atoi(pAttr->Value());
								else if(strcmp(pAttr->Name(), FLAGPOS) == 0)
									regnode.flagpos = atoi(pAttr->Value());
								else if(strcmp(pAttr->Name(), FLAGVAL) == 0)
									regnode.flagval = pAttr->Value();
							}

							_RegList.push_back(regnode);
						}
						
						pElem2 = pElem2->NextSiblingElement();
					}

					break;
				}
				
				elem = elem->NextSiblingElement();
			}
		}	
	}

	if(!bHasFormatNode)
	{
		return ERR_LOADCONF;
	}
	return SUCCESS;
}

int CParaXml::InitContrOrder()
{
	if(_pConfXmlDoc == 0)
		return -1;           //文件没有加载
	
	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	TiXmlElement *elem = 0;
	TiXmlElement *pElem2 = 0;
	TiXmlElement *pElem3 = 0;
	TiXmlAttribute *pAttr = 0;
	BOOL isHasStep = FALSE;
	BOOL bHasRWNode = FALSE;

	FreeRWList();
	if ( pRoot )
	{
		elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), MAG_READ) == 0 )
				{
					bHasRWNode = TRUE;
                    pElem2 = elem->FirstChildElement();
					while (pElem2)
					{
						if(strcmp(pElem2->Value(), TRACKREAD) == 0)
						{
							stRWNode rnode;
							memset(&rnode, 0, sizeof(stRWNode));
							rnode.pList = new VStepList;
							isHasStep = FALSE;

							//取出trackno属性
							pAttr = NULL;
							for(pAttr = pElem2->FirstAttribute(); pAttr; pAttr = pAttr->Next())
							{
								if(strcmp(pAttr->Name(), TRACKNO) == 0)
									rnode.trackno = atoi(pAttr->Value());
							}
							//遍历每个step节点
							pElem3 = pElem2->FirstChildElement();
							while(pElem3)
							{
								if(strcmp(pElem3->Value(), STEP) == 0)
								{
									isHasStep = TRUE;
                                    ParaStepNode(pElem3, rnode.pList);
								}
								pElem3 = pElem3->NextSiblingElement();
							}

							if(isHasStep)
								_RList.push_back(rnode);
							else
								delete rnode.pList;
						}
							
						pElem2 = pElem2->NextSiblingElement();
					}
				}
				else if(strcmp(elem->Value(), MAG_WRITE) == 0)
				{
					bHasRWNode = TRUE;
                    pElem2 = elem->FirstChildElement();
					while (pElem2)
					{
						if(strcmp(pElem2->Value(), TRACKWRITE) == 0)
						{
							stRWNode wnode;
							memset(&wnode, 0, sizeof(stRWNode));
							wnode.pList = new VStepList;
							isHasStep = FALSE;
							
							//取出trackno属性
							pAttr = NULL;
							for(pAttr = pElem2->FirstAttribute(); pAttr; pAttr = pAttr->Next())
							{
								if(strcmp(pAttr->Name(), TRACKNO) == 0)
									wnode.trackno = atoi(pAttr->Value());
							}
							//遍历每个step节点
							pElem3 = pElem2->FirstChildElement();
							while(pElem3)
							{
								if(strcmp(pElem3->Value(), STEP) == 0)
								{
									isHasStep = TRUE;
                                    ParaStepNode(pElem3, wnode.pList);
								}
								pElem3 = pElem3->NextSiblingElement();
							}
							
							if(isHasStep)
								_WList.push_back(wnode);
							else
								delete wnode.pList;
						}
						
						pElem2 = pElem2->NextSiblingElement();
					}
				}
				
				elem = elem->NextSiblingElement();
			}
		}	
	}
	
	if(!bHasRWNode)
	{
		return ERR_LOADCONF;
	}
	return 0;
}

CString CParaXml::VPairValue(CString &attrival, areaList *pList, VPair &vlist)
{
	CString value = "";
	int addchrps = 0;
	int chrpreps = 0;
	CString item;
	BOOL bBreak = TRUE;
	BOOL bFindList = FALSE;
	int i;
	CString str;
	addchrps = attrival.Find("+", addchrps);
	if(addchrps != -1)
	{
		addchrps = 0;
		while (bBreak)
		{
			if(addchrps >= attrival.GetLength())
				break;
			addchrps = attrival.Find("+", addchrps);
			if(addchrps == -1)
			{
				if(chrpreps < attrival.GetLength())
				{
					bBreak = FALSE;
				}
			}

			if(bBreak)
			    item = attrival.Mid(chrpreps, addchrps - chrpreps);
			else
				item = attrival.Mid(chrpreps, attrival.GetLength() - chrpreps);

			bFindList = FALSE;
            if(pList)
			{
				for(i = 0; i < pList->size(); i ++)
				{
					stMagFormat &format = (*pList)[i];
					if(format.name = item)
					{
						value += format.value;
						bFindList = TRUE;
						break;
					}
				}
			}

			if(!bFindList)
			{
				for(i = 0; i < vlist.size(); i ++)
				{
					pair<CString, CString> itempair = vlist[i];
					if(itempair.first == item)
					{
						value += itempair.second;
						break;
					}
				}
			}

			addchrps ++;
			chrpreps = addchrps;
		}
	}
	else
	{
		for(i = 0; i < vlist.size(); i ++)
		{
			pair<CString, CString> itempair = vlist[i];
			if(itempair.first == attrival)
			{
				return itempair.second;
			}
		}
	}

    return value;
}

void CParaXml::FreeMagFormatList()
{
    for(int i = 0; i < _MagTypeList.size(); i++)
	{
		stMagFormatTp &tpnode = _MagTypeList[i];
		if(tpnode.pList)
		{
			delete tpnode.pList;
			tpnode.pList = 0;
		}
	}

	_MagTypeList.clear();
}

int CParaXml::ConstructMagTypeList()
{
	if(_pFormatXmlDoc == 0)
		return -1;           //文件没有加载
	debug("构造磁条类型链表");
	BOOL bHasItem = FALSE;
	TiXmlElement *pRoot = _pFormatXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), MAG_FORMAT_REG) == 0)
				{
					elem = elem->NextSiblingElement();
					continue;
				}

				bHasItem = TRUE;
				stMagFormatTp tp;
				tp.pList = new areaList;

                //磁条类型
				TiXmlAttribute *ParentAttr = elem->FirstAttribute();
				tp.magname = ParentAttr->Value();
				
				TiXmlElement *pElem2 = elem->FirstChildElement();
				if(pElem2)
				{
					while (pElem2)
					{
						stMagFormat info;

						//获取每个subitem节点里的属性值
                        TiXmlAttribute *pAttr = NULL;
						for(pAttr = pElem2->FirstAttribute(); pAttr; pAttr = pAttr->Next())
						{
							if(strcmp(pAttr->Name(), NAME) == 0)
							{
								info.name = pAttr->Value();
							}
							else if(strcmp(pAttr->Name(), TRACK) == 0)
							{
                                info.track = atoi(pAttr->Value());
							}
							else if(strcmp(pAttr->Name(), STARTPOS) == 0)
							{
								info.startps = atoi(pAttr->Value());
							}
							else if(strcmp(pAttr->Name(), LEN) == 0)
							{
								info.len = atoi(pAttr->Value());
							}
							else if(strcmp(pAttr->Name(), TYPE) == 0)
							{
								info.type = atoi(pAttr->Value());
							}
							else if(strcmp(pAttr->Name(), VALUE) == 0)
							{
								info.value = pAttr->Value();
							}
							else if(strcmp(pAttr->Name(), OUTOFVAULUE) == 0)
							{
								info.outofval = pAttr->Value();
							}
							else if(strcmp(pAttr->Name(), FROM) == 0)
							{
								info.from = atoi(pAttr->Value());
							}
						}

						if(tp.pList)
							tp.pList->push_back(info);

						pElem2 = pElem2->NextSiblingElement();
					}
				}
				else
				{
					delete tp.pList;
					tp.pList = NULL;
				}
				
				if(tp.pList)
					_MagTypeList.push_back(tp);

				elem = elem->NextSiblingElement();
			}
		}	
	}

	if(!bHasItem)
		return ERR_LOADFMT;
	return 0;
}

int CParaXml::GetLenByTrack(int trackno, const char *magformat)
{
	int len;
	for(int i = 0; i < _MagTypeList.size(); i ++)
	{
		if(_MagTypeList[i].magname == magformat)
		{
			stMagFormatTp &tp = _MagTypeList[i];

			if(!tp.pList)
				return 0;

			for(int j = 0; j < tp.pList->size(); j ++)
			{
				stMagFormat &format = (*tp.pList)[j];
                if(format.track == trackno && trackno == 2)
				{
					len += format.len;
				}
				else if(format.track == trackno && trackno == 3)
				{
					len += format.len;
				}
				else
				{
					len += format.len;
				}
			}
		}
	}
	return len;
}

VMagTp &CParaXml::GetMagTpVList()
{
	return _MagTypeList;
}

CString CParaXml::ComboxTrack(const char *formatname, int trackno, VPair &sub, int &hasOutvalue)
{
	CString data;
	CString temp;
	int valen;
	int i;
	int j;
	int k;
	for(i = 0; i < _MagTypeList.size(); i ++)
	{
		if(_MagTypeList[i].magname == formatname)
		{
			stMagFormatTp &tp = _MagTypeList[i];
			
			if(!tp.pList)
				return data;
			
			for(j = 0; j < tp.pList->size(); j ++)
			{
				stMagFormat &format = (*tp.pList)[j];
                if(format.track == trackno)
				{
					switch (format.type)
					{
					case FIXVAL:
						{
							data += format.value;
							break;
						}
					case OUTOFVAL:
						{
							temp = VPairValue(format.outofval, tp.pList, sub);
							valen = temp.GetLength();
							if(temp.IsEmpty())
							{
								for (k = 0; k < format.len; k ++)
								{
									temp += "0";
								}
							}
							else
							{
								hasOutvalue = HASOUTVALUE;
							}

							if(valen < format.len)
							{
								for (k = 0; k < format.len - valen; k ++)
								{
									temp += "=";
								}
								data += temp.Mid(0, format.len);
							}
							else
							    data += temp.Mid(0, format.len);
							break;
						}
					case SUBVAL:
						{
							temp = VPairValue(format.outofval, tp.pList, sub);
							valen = temp.GetLength();
							if(temp.IsEmpty())
							{
								for (k = 0; k < format.len; k ++)
								{
									temp += "0";
								}
							}
							if(valen < format.len)
							{
								for (k = 0; k < format.len - valen; k ++)
								{
									temp += "=";
								}
							}
							data += temp.Mid(format.from, format.len);
							break;
						}
					case VERIFYVAL:
						{
							temp = VPairValue(format.outofval, tp.pList, sub);
							if(temp.IsEmpty())
							{
								for (k = 0; k < format.len; k ++)
								{
									temp += "0";
								}
							}
							temp = GetCheckTrackData(temp.GetBuffer(temp.GetLength()), 0);
							data += temp;
							break;
						}
					case TWOTRACKVAL:
						{
							break;
						}
					case THREETRACKVAL:
						{
							break;
						}
					case ONLYGET:
						{
							break;
						}
					case SEPARVAL:
						{
							data += "=";
							break;
						}
					}
				}
			}

			break;
		}
	}
	return data;
}

int CParaXml::ParaOrder(CString &old, char *pOutNew)
{
	if(old.GetLength() % 2 != 0)
		return -1;
	CString temp;
	int idx = 0;
    for(int i = 0; i < old.GetLength(); i += 2)
	{
		
		temp = old.Mid(i, 2);
		
		//十六进制字符串转换成十进制整形数
		char *p = temp.GetBuffer(temp.GetLength());
		int n=0;
		while( (*p) != '\0' )
		{
			if( (*p) <= '9' && (*p) >= '0' )
				n=n*16+ (*p) - '0';
			else if( (*p) <= 'f' && (*p) >= 'a' )
				n = n * 16 + (*p) - 'a' + 10;
			else if( (*p) <= 'F' && (*p) >= 'A' )
				n = n * 16 + (*p) - 'A' + 10;
			p++;
		}
		
		pOutNew[idx ++] = n;
	}
	
	return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// PAN 的校验位算法是采用计算模 10 "隔位倍加" 校验数的 Luhn 公式:
// 1. 从右边第 1 数字开始 (低序) 每隔 1 位乘以 2;
// 2. 把步骤 1 中获得的乘积的各位数字与原号码中未乘 2 的各位数字相加;
// 3. 求这个总和的个位数字的 "10的补数", 这个补数就是校验数字.
//
// 例如: 无校验数字的帐号  499273 9871
//   步骤  4 9 9 2 7 3 9 8 7 1
//   1      *2  *2  *2  *2  *2
//        --------------------
//          18   4   6  16   2
//	 2	4 + 1 + 8 + 9 + 4 + 7 + 6 + 9 + 1 + 6 + 7 + 2 = 64
//	 3	4 的补数 = 6
//   结果: 带有校验数字的帐号为
//         499273 98716
// getCheckNum(char *)
int CParaXml::GetCheckNum(char * string)
{
	if( string[0] == 0) return -1;
	else
	{
		for( int i = 0; i < strlen( string); i++)
		{
			if( string[i] > 0X39 || string[i] < 0x30) return -1;
		}
		int value = 0;
		int tempValue = 0;
		int len = strlen( string);
		if( len % 2 != 0)
		{
			for( i = 0; i < len; i++)
			{
				tempValue = (string[i] - 0x30) * ((i + 1) % 2 + 1);
				value += tempValue / 10 + tempValue % 10;
			}
		}
		else
		{
			for( i = 0; i < strlen( string); i++)
			{
				tempValue = (string[i] - 0x30) * ( i % 2 + 1);
				value += tempValue / 10 + tempValue % 10;
			}
		}
		value = 10 - value % 10;
		if( value >= 10) value = 0;
		return value;
	}
	return 0;
}

char * CParaXml::GetCardCheckNum(char * string)
{
	return 0;
}

int CParaXml::GetBookAcctCheckNum(char *acctno, char *OutVal)
{
	return 0;
}

CString CParaXml::GetCheckTrackData(char *pData, int type)
{
	CString checkdata = "";
    if(type == 0)//缺省类型
	{
		checkdata.Format("%d", GetCheckNum(pData));
	}
	else if(type == 1)
	{

	}
	else if(type == 2)
	{

	}

	return checkdata;
}


