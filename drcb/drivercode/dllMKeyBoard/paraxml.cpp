#include "StdAfx.h"
#include "paraxml.h"

CParaXml::CParaXml()
{
	_pConfXmlDoc = new TiXmlDocument();
	_StepList.clear();
}

CParaXml::~CParaXml()
{
	if(_pConfXmlDoc)
		delete _pConfXmlDoc;
}

BOOL CParaXml::LoadMagConfXml(CString &filepath)
{
	return _pConfXmlDoc->LoadFile(filepath);
}

int CParaXml::GetPortParam(int &port, int &BaudRate, char &Parity, char &ByteSize, char &StopBits, BOOL &isCOM, BOOL &hasAdap, short &TimeOut)
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
									isCOM = TRUE;
								else
									isCOM = FALSE;
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
							else if(strcmp(pElem2->Value(), HASADAPTER) == 0)
							{
								if(strcmp(pElem2->GetText(), "true") == 0)
									hasAdap = TRUE;
								else
									hasAdap = FALSE;
							}
							else if(strcmp(pElem2->Value(), DEFAULTTIMEOUT) == 0)
							{
								TimeOut = atoi(pElem2->GetText());
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

void CParaXml::ParaStepNode(TiXmlElement *pElem)
{
	stKeyBdDoStep step;
	step.flag = 0;
	step.keymap = 0;
	step.reset = 0;
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
			step.oper = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), FLAG) == 0)
		{
			step.flag = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), DATA) == 0)
		{
			step.writedata = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), STARTCHAR) == 0)
		{
			step.startchar = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), ENDCHAR) == 0)
		{
			step.endchar = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), ENTER) == 0)
		{
			step.enter = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), CLEAR) == 0)
		{
			step.clear = pAttr->Value();
		}
		else if(strcmp(pAttr->Name(), KEYMAP) == 0)
		{
			step.keymap = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), RESET) == 0)
		{
			step.reset = atoi(pAttr->Value());
		}
	}

    if(!step.name.IsEmpty())
		_StepList.push_back(step);
}

int CParaXml::GetAdapterInfo(CString &adapter, CString &thirdll, BOOL &hasAdap)
{
	if(_pConfXmlDoc == 0)
		return ERR_LOADCONF;           //文件没有加载
	
	hasAdap = FALSE;

	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), ADAPTERINFO) == 0 )
				{
					hasAdap = TRUE;

                    TiXmlElement *pElem2 = elem->FirstChildElement();
					if(pElem2)
					{
						while (pElem2)
						{
							if(strcmp(pElem2->Value(), CALLDLL) == 0)
							{
                                adapter = pElem2->GetText();
							}
							else if(strcmp(pElem2->Value(), THIRDDLL) == 0)
							{
								thirdll = pElem2->GetText();
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
	
	return SUCCESS;
}

int CParaXml::InitContrOrder()
{
	if(_pConfXmlDoc == 0)
		return ERR_LOADCONF;           //文件没有加载

	_StepList.clear();
	
	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), DEV_DEAL) == 0 )
				{
                    TiXmlElement *pElem2 = elem->FirstChildElement();
					if(pElem2)
					{
						while (pElem2)
						{
							if(strcmp(pElem2->Value(), STEP) == 0)
							{
                                ParaStepNode(pElem2);
							}
							else
							{
                                TiXmlElement *pElem3 = pElem2->FirstChildElement();
								while(pElem3)
								{
									if(strcmp(pElem3->Value(), STEP) == 0)
									{
										ParaStepNode(pElem3);
									}
									pElem3 = pElem3->NextSiblingElement();
								}
							}

// 							CString text;
// 							CString value;
// 							text = pElem2->Value();
// 							value = pElem2->GetText();
// 							pair<CString, CString> order(text, value);
// 							_ControlList.push_back(order);
							
							pElem2 = pElem2->NextSiblingElement();
						}
					}
					
					break;
				}
				elem = elem->NextSiblingElement();
			}
		}	
	}
	
	return SUCCESS;
}

int CParaXml::InitMapKeyList()
{
	_VMapKey.clear();
	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		while ( elem )
		{
			if(strcmp(elem->Value(), KEY_MAP_TABLE) == 0 )
			{
				TiXmlElement *elem1 = elem->FirstChildElement();
				while ( elem1 )
				{
					CString outchar;
					CString mapkey;
					TiXmlAttribute *pAttr = NULL;
					for(pAttr = elem1->FirstAttribute(); pAttr; pAttr = pAttr->Next())
					{
						if(strcmp(pAttr->Name(), DEVOUTCHAR) == 0)
						{
							outchar = pAttr->Value();
						}
						else if(strcmp(pAttr->Name(), MAP) == 0)
						{
							mapkey = pAttr->Value();
						}
					}
					pair<CString, CString> key(outchar, mapkey);
					_VMapKey.push_back(key);

					elem1 = elem1->NextSiblingElement();
				}
				break;
			}

			elem = elem->NextSiblingElement();
		}
	}
	return SUCCESS;
}

CString &CParaXml::VPairValue(const char *name, VPair &vlist)
{
	CString value = "";
	for(int i = 0; i < vlist.size(); i ++)
	{
		pair<CString, CString> item = vlist[i];
		if(item.first == name)
		{
			return item.second;
		}
	}
	
    return value;
}

CString CParaXml::GetMapKey(char outchr)
{
	CString order = "";
	for(int i = 0; i < _VMapKey.size(); i ++)
	{
		pair<CString, CString> item = _VMapKey[i];
		CString &temp = item.first;
		if(temp[0] == outchr)
		{
			return item.second;
		}
	}

    return order;
}

VStepList &CParaXml::GetStepListRef()
{
	return _StepList;
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
