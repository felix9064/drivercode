#include "StdAfx.h"
#include "paraxml.h"

CParaXml::CParaXml()
{
	_pConfXmlDoc = new TiXmlDocument();
}

CParaXml::~CParaXml()
{
	if(_pConfXmlDoc)
		delete _pConfXmlDoc;
}

BOOL CParaXml::LoadICConfXml(CString &filepath)
{
	return _pConfXmlDoc->LoadFile(filepath);
}

int CParaXml::GetPortParam(int &port, int &BaudRate, char &Parity, char &ByteSize, char &StopBits, BOOL &isCOM, 
						   BOOL &hasAdap, CString DefAID, int &DllType, char &ICFlag, char &BpNO, short &defaultTimeOut)
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
							else if(strcmp(pElem2->Value(), DEFAID) == 0)
							{
								DefAID = pElem2->GetText();
							}
							else if(strcmp(pElem2->Value(), DLLTYPE) == 0)
							{
								DllType = atoi(pElem2->GetText());
							}
							else if(strcmp(pElem2->Value(), ICFLAG) == 0)
							{
								ICFlag = pElem2->GetText()[0];
							}
							else if(strcmp(pElem2->Value(), BPNO) == 0)
							{
								BpNO = pElem2->GetText()[0];
							}
							else if (strcmp(pElem2->Value(), DEFAULTTIMEOUT) == 0)
							{
								defaultTimeOut = atoi(pElem2->GetText());
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

int CParaXml::GetAdapterInfo(CString &adapter, CString &thirdll, CString &etcdll)
{
	if(_pConfXmlDoc == 0)
		return ERR_LOADCONF;           //文件没有加载
	
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
							else if(strcmp(pElem2->Value(), ETCDLL) == 0)
							{
								etcdll = pElem2->GetText();
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
	if(thirdll.IsEmpty())
	{
	    return ERR_NOTHIRDCALL;
	}
	return SUCCESS;
}