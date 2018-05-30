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

int CParaXml::GetPortParam(int &port, int &BaudRate, char &Parity,
						   char &ByteSize, char &StopBits, short &isCOM,
						   CString &calldll, CString &thirddll, BOOL &bThirdcall, short &deftimeout)
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
							else if (strcmp(pElem2->Value(), DEFAULT_TIMEOUT) == 0)
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

void CParaXml::ParaStepNode(TiXmlElement *pElem, VFpCpy *pStepList)
{
	stFpDoStep step;
    memset(&step, 0, sizeof(stFpDoStep));
	//获取每个subitem节点里的属性值
	TiXmlAttribute *pAttr = NULL;
	for(pAttr = pElem->FirstAttribute(); pAttr; pAttr = pAttr->Next())
	{
		if(strcmp(pAttr->Name(), NAME) == 0)
		{
			strcpy(step.name, pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), DO) == 0)
		{
			strcpy(step.oper, pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), TIMES) == 0)
		{
			step.times = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), WRITETYPE) == 0)
		{
			step.rwtype = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), READTYPE) == 0)
		{
			step.rwtype = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), DATA) == 0)
		{
			strcpy(step.writedata, pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), STARTSTRING) == 0)
		{
			strcpy(step.startstr, pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), FPLENPOS) == 0)
		{
			step.fplenpos = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), FPLENBYTENUM) == 0)
		{
			step.fplenbytenum = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), FPDATAPOS) == 0)
		{
			step.fpdatapos = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), HEADERLEN) == 0)
		{
			step.headerlen = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), OKSTATUS) == 0)
		{
			strcpy(step.okstatus, pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), STATUSPOS) == 0)
		{
			step.statuspos = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), STATUSBYTENUM) == 0)
		{
			step.statusbytenum = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), CHECKBYTENUM) == 0)
		{
			step.checkbytenum = atoi(pAttr->Value());
		}
		else if(strcmp(pAttr->Name(), RESET) == 0)
		{
			step.reset = atoi(pAttr->Value());
		}
	}

    if(!pStepList)
		_StepList.push_back(step);
	else
	{
		stFpDoStepCpy stepcpy;
		memset(&stepcpy, 0, sizeof(stFpDoStepCpy));
		memcpy(&stepcpy, &step, sizeof(stFpDoStepCpy));
		pStepList->push_back(stepcpy);
	}
}

int CParaXml::InitContrOrder()
{
	if(_pConfXmlDoc == 0)
		return ERR_LOADCONF;           //文件没有加载

	FreeStepList();
	
	//CString oper;
    VStepList::iterator it;
	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		if ( elem )
		{
			while ( elem )
			{
				if(strcmp(elem->Value(), FP_READ) == 0 )
				{
                    TiXmlElement *pElem2 = elem->FirstChildElement();
					if(pElem2)
					{
						while (pElem2)
						{
							if(strcmp(pElem2->Value(), STEP) == 0)
							{
                                ParaStepNode(pElem2, NULL);
								if(_StepList.size() == 0)
								{
									pElem2 = pElem2->NextSiblingElement();
									continue;
								}
								//stFpDoStep& item = _StepList.back();
								it = _StepList.end();
								it --;
								if(strcmp(it->oper, LOOP) == 0)
								{
									VFpCpy *pList = new VFpCpy;
									pList->clear();
									TiXmlElement *pElem3 = pElem2->FirstChildElement();
									while(pElem3)
									{
										if(strcmp(pElem3->Value(), STEP) == 0)
										{
											ParaStepNode(pElem3, pList);
										}
										pElem3 = pElem3->NextSiblingElement();
									}

									if(_StepList.size())
									{
										it = _StepList.end();
								        it --;
										if( it->pList )
										{
											delete it->pList;
											it->pList = NULL;
										}
										it->pList = pList;
									}
									else
									{
										delete pList;
										pList = NULL;
									}

								}
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

int CParaXml::InitStatusTab()
{
	_VStatusTab.clear();
	TiXmlElement *pRoot = _pConfXmlDoc->RootElement();
	if ( pRoot )
	{
		TiXmlElement *elem = pRoot->FirstChildElement();
		while ( elem )
		{
			if(strcmp(elem->Value(), STATUSTAB) == 0 )
			{
				TiXmlElement *elem1 = elem->FirstChildElement();
				while ( elem1 )
				{
					CString code;
					CString desc;
					TiXmlAttribute *pAttr = NULL;
					for(pAttr = elem1->FirstAttribute(); pAttr; pAttr = pAttr->Next())
					{
						if(strcmp(pAttr->Name(), CODE) == 0)
						{
							code = pAttr->Value();
						}
						else if(strcmp(pAttr->Name(), DESC) == 0)
						{
							desc = pAttr->Value();
						}
					}
					pair<CString, CString> key(code, desc);
					_VStatusTab.push_back(key);

					elem1 = elem1->NextSiblingElement();
				}
				break;
			}

			elem = elem->NextSiblingElement();
		}
	}
	return SUCCESS;
}

void CParaXml::FreeStepList()
{
	for(int i = 0; i < _StepList.size(); i ++)
	{
		_stFpDoStep &ITEM = _StepList[i];
		if(ITEM.pList)
		{
			delete ITEM.pList;
			_StepList[i].pList = NULL;
		}
	}

	_StepList.clear();
}

CString CParaXml::VPairValue(const char *name, VPair &vlist)
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

CString CParaXml::GetStatusDesc(char statuscode)
{
	CString order = "";
	char byt[2];
	int len;
	for(int i = 0; i < _VStatusTab.size(); i ++)
	{
		pair<CString, CString> &item = _VStatusTab[i];
		ParaOrder(item.first, byt, len);
		if(byt[0] == statuscode)
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

int CParaXml::ParaOrder(CString old, char *pOutNew, int &len)
{
	if(old.GetLength() % 2 != 0)
		return -1;

	int idx = 0;
    for(int i = 0; i < old.GetLength(); i += 2)
	{
	    CString temp;		
		temp = old.Mid(i, 2);

		//十六进制字符串转换成十进制整形数
		//char *p = temp.GetBuffer(2);
		int n=0;
		int stridx = 0;
		while( stridx < temp.GetLength() )
		{
			if( temp[stridx] <= '9' && temp[stridx] >= '0' )
				n=n*16+ temp[stridx] - '0';
			else if( temp[stridx] <= 'f' && temp[stridx] >= 'a' )
				n = n * 16 + temp[stridx] - 'a' + 10;
			else if( temp[stridx] <= 'F' && temp[stridx] >= 'A' )
			    n = n * 16 + temp[stridx] - 'A' + 10;

			stridx ++;
		}

		//temp.ReleaseBuffer();

		pOutNew[idx ++] = n;
	}

	len = idx;
	return SUCCESS;
}
