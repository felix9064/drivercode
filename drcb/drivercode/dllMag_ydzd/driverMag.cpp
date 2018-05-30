#include "StdAfx.h"
#include "Win32Port.h"
#include "paraxml.h"
#include "driverMag.h"
#pragma comment(lib,"carddeslib.lib")

CDriverMag::CDriverMag()
{
	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();
	_ndeftimeout = 15;
	_ntimeout = 0;
	m_baudrate = 115200;
}

CDriverMag::~CDriverMag()
{
	if(_pWinPort)
	{
		delete _pWinPort;
		_pWinPort = NULL;
	}

	if(_pParaXml)
	{
		delete _pParaXml;
		_pParaXml = NULL;
	}
}

void CDriverMag::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

CString & CDriverMag::GetModuleDir()
{
	return _moduledir;
}

void CDriverMag::SetPortNo(CString port)
{
	_portno = port;
}

CString & CDriverMag::GetPortNo()
{
	return _portno;
}

void CDriverMag::SetTimeOut(short timeout)
{
	_ntimeout = timeout;
}

void CDriverMag::SetTimeOutZero()
{
	_ntimeout = 0;
}

void CDriverMag::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CDriverMag::SetFormatXmlPath(CString &path)
{
	_fortxmlpath = path;
}

void CDriverMag::SetErrCode(enRetCode errcode)
{
	_errcode = errcode;
}

enRetCode CDriverMag::GetErrCode()
{
	return _errcode;
}
/*
int CDriverMag::OpenMag()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
	{
		return ERR_LOADCONF;                               //加载文件失败
	}
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	BOOL isCom;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, isCom, _ndeftimeout);
	if(!_portno.IsEmpty())
	{
		if(_portno.GetLength() <= 3)
			return ERR_OPENPORT;
		CString porttmp = _portno.Mid(3,_portno.GetLength() - 3);
		CString comlpttmp = _portno.Mid(0, 3);
		if(comlpttmp == LPT)
			isCom = FALSE;
		else
			isCom = TRUE;
		port = atoi(porttmp);
	}
	_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, isCom);
	if(!_pWinPort->AsynOpen())
		return ERR_OPENPORT;                              //端口打开失败
	return SUCCESS;
}
*/

int CDriverMag::OpenICd()
{
    if(!_pParaXml->LoadMagConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //加载文件失败
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, _isCom, _CallDll, _ThirdDll, _bThirdCall, _ndeftimeout);
	if(!_portno.IsEmpty())
	{
		if(_portno.GetLength() <= 3)
			return ERR_OPENPORT;
		CString porttmp = _portno.Mid(3,_portno.GetLength() - 3);
		CString comlpttmp = _portno.Mid(0, 3);
		if(comlpttmp == LPT)
			_isCom = 0;
		else if(comlpttmp == USB)
			_isCom = 2;
		else
			_isCom = 1;
		port = atoi(porttmp);
	}
	
	_nPort = port;
	if (baudrate > 0)
	{
		m_baudrate = baudrate;
	}

	if(_isCom != 2 && _bThirdCall == FALSE)
	{
		_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, _isCom?TRUE:FALSE);
		if(!_pWinPort->AsynOpen())
			return ERR_OPENPORT;                              //端口打开失败
	}
	
	return SUCCESS;
}

void CDriverMag::CloseMag()
{
    _pWinPort->AsynClose();
}

int CDriverMag::ResetMag(int rw)
{
	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);
	int i;
	BOOL bOperStatus = FALSE;
	//读操作
	if(rw == 1)
	{
		VRWList &readlist = _pParaXml->GetRListRef();
		for(i = 0; i < readlist.size(); i++)
		{
			stRWNode &rwnode = readlist[i];
			if(rwnode.trackno != 23)
				continue;
			
			VStepList *pStepList = rwnode.pList;
			if(pStepList == 0)
			{
				_pParaXml->FreeRWList();
				_pParaXml->FreeMagFormatList();
				return ERR_OCREXCEPTION;
			}
			
			for(int j = 0; j < pStepList->size(); j++)
			{
				stOperStep &step = (*pStepList)[j];
				if(step.opername == WRITE && step.reset == 1)
				{
					if(step.wrtype == WTYPE_SETDATAFROMCONF)
					{
						memset(neworder, 0, bufsize);
						if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //将十六进制字符串转化为十进制。
						{
							_pParaXml->FreeRWList();
							_pParaXml->FreeMagFormatList();
							return ERR_INVALIDORDER;    //转换指令错误
						}
						
						if(!_pWinPort->SendData(neworder, strlen(neworder)))
						{
							_pParaXml->FreeRWList();
							_pParaXml->FreeMagFormatList();
							return ERR_SENDDATA;
						}

						bOperStatus = TRUE;
						_pWinPort->SetExitReadVar(FALSE);   //使其能够退出读串口循环
						break;
					}
				}
			}

			if(bOperStatus)
				break;
		}
	}
	else
	{
		bOperStatus = FALSE;
		VRWList &writelist = _pParaXml->GetWListRef();
		for(i = 0; i < writelist.size(); i++)
		{
			stRWNode &rwnode = writelist[i];
			if(rwnode.trackno != 23)
				continue;
			
			VStepList *pStepList = rwnode.pList;
			if(pStepList == 0)
			{
				_pParaXml->FreeRWList();
				_pParaXml->FreeMagFormatList();
				return ERR_OCREXCEPTION;
			}
			
			for(int j = 0; j < pStepList->size(); j++)
			{
				stOperStep &step = (*pStepList)[j];
				if(step.opername == WRITE && step.reset == 1)
				{
					if(step.wrtype == WTYPE_SETDATAFROMCONF)
					{
						memset(neworder, 0, bufsize);
						if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //将十六进制字符串转化为十进制。
						{
							_pParaXml->FreeRWList();
							_pParaXml->FreeMagFormatList();
							return ERR_INVALIDORDER;    //转换指令错误
						}
						
						if(!_pWinPort->SendData(neworder, strlen(neworder)))
						{
							_pParaXml->FreeRWList();
							_pParaXml->FreeMagFormatList();
							return ERR_SENDDATA;
						}
						
						bOperStatus = TRUE;
						_pWinPort->SetExitReadVar(FALSE);   //使其能够退出读串口循环
						break;
					}
				}
			}
			
			if(bOperStatus)
				break;
		}		
	}

	_pParaXml->FreeRWList();
	_pParaXml->FreeMagFormatList();
	return 0;
}

int CDriverMag::ReadICd(const char *magformat, const char *trackno, VPair &vlist)
{
	//调用厂商驱动
	if(_bThirdCall)
	{
		int magtype = atoi(trackno);

		typedef pair<CString, CString> PAIR;

		CString trackNo;
		trackNo.Format("%s", trackno);
		PAIR track("trackno", trackNo);
		vlist.push_back(track);
		
		PAIR dllnamepr("thirdll", _moduledir + _ThirdDll);
		vlist.insert(vlist.begin(), dllnamepr);
        if (0 == _ntimeout)
        {
			_ntimeout = _ndeftimeout;
        }
		CString tmptimeout;
		tmptimeout.Format("%d", _ntimeout);
		PAIR timeout("timeout", tmptimeout);
		vlist.push_back(timeout);

		CString tmpbaudrate;
		tmpbaudrate.Format("%d", m_baudrate);
		PAIR baudrate("baudrate", tmpbaudrate);
		vlist.push_back(baudrate);

		typedef BOOL (CALLBACK * LPGETMAGCOM)(MAG_INTERFACE *);
		
		HMODULE hDll = LoadLibrary(_moduledir + _CallDll);
		if(!hDll)
		{
			return ERR_NOFINDMATCHDLL;
		}
        LPGETMAGCOM lpMagCom = (LPGETMAGCOM)GetProcAddress(hDll, "GetMagCOM");
		if(!lpMagCom)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		MAG_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(MAG_INTERFACE));
		BOOL bGetCOM = lpMagCom(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

	
		char errcd[20];
		memset(errcd, 0, 20);
		char errmsg[256];
	    memset(errmsg, 0, 256);
		short isUSB = (_isCom != 2 ? 0 : 1);
		int ret = stCOM.ReadCard(_nPort, isUSB, vlist, errcd, errmsg);
		if(ret != 0)
		{
			_errcd = errcd;
			_errmsg = errmsg;
			FreeLibrary(hDll);
			return ERR_READFAIL;
		}

		PAIR track2data = vlist[0];
		CString track2 = track2data.second;
		
		PAIR track3data = vlist[1];
		CString track3 = track3data.second;
		
		vlist.clear();

		int track2len = track2.GetLength();
		int track3len = track3.GetLength();

		if(track2len == 0 && track3len == 0)
		{
			return ERR_CARD_ABSCENT_DATA;
		}

		CString strformat ;
		if(magformat == NULL || magformat == "")
		{
			//2012-08-21更改磁条格式判断规则
			//GetMagFormatByTrackLen(track2, track2len, track3, track3len, strformat);
			GetMagFormatByTrackData(track2, track3, strformat);
		}
		else
		{
			strformat = magformat;
		}

		//磁道信息为空
		if(strformat == EMPTY_MAG)
		{
			return ERR_CARD_ABSCENT_DATA;
		}

		_cardtype = strformat;
  		CString temp;
		CString itemval;

		//以下为2012-08-21更改磁条格式判断规则,取各种凭证的账号域
		if (_cardtype == GYCARD)
		{
			temp = track2.Mid(0,6);
			if (temp.GetLength() != 6)
			{
				return ERR_READTRACK;
			}
			itemval.Format("USERID=%s", temp);
			pair< CString, CString> pairUSERID("USERID", itemval);
			vlist.push_back(pairUSERID);
		}
		if (_cardtype == CARD)
		{
			int pos = track2.Find('=');
			if (-1 == pos)
			{
				pos = track2.Find('\'');
			}
			if (-1 == pos)
			{
				pos =  track2.Find('>');
			}
			if (pos > 0)
			{
				temp = track2.Mid(0,pos);
				itemval.Format("ACCTNO=%s", temp);
				pair< CString, CString> pairACCTNO("ACCTNO", itemval);
				vlist.push_back(pairACCTNO);
			}
			else
			{
				return ERR_CARD_ABSCENT_DATA;
			}
		}
		if (_cardtype == PASSBK_COMPANY)
		{
			//凭证号
			temp = track3.Mid(1,13);
			itemval.Format("PNGZHM=%s", temp);
			pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
			vlist.push_back(pairPNGZHM);

			//对公账户
			temp = track3.Mid(22,18);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);

			//支付条件
			temp = track3.Mid(41,3);
			itemval.Format("ZHIFTJ=%s", temp);
			pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
			vlist.push_back(pairZHIFTJ);
		}
		if (_cardtype == PASSBK_PERSON)
		{
			//凭证代号
			temp = track3.Mid(40,13);
			itemval.Format("PNGZHM=%s", temp);
			pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
			vlist.push_back(pairPNGZHM);
			
			//一本通账号
			temp = track3.Mid(21,15);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
			
			//支付条件
			temp = track3.Mid(36,3);
			itemval.Format("ZHIFTJ=%s", temp);
			pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
			vlist.push_back(pairZHIFTJ);
		}
		if (_cardtype == PASSBK_NEW)
		{
			int pos = track2.Find('=');
			if (-1 == pos)
			{
				pos = track2.Find('\'');
			}
			if (-1 == pos)
			{
				pos =  track2.Find('>');
			}
			if (pos > 0)
			{
				//temp.Format("%d", pos);
				//AfxMessageBox(temp);
				//账号
				temp = track2.Mid(2,pos-2);
				//AfxMessageBox(temp);
				//判断2、3磁道账号是否一致
				if (track3.GetLength() < (13 + temp.GetLength()))
				{
					return ERR_PASSBK_NEW_ABSCENT_DATA;
				}
				if (temp != track3.Mid(13, temp.GetLength()))
				{
					return ERR_PASSBK_NEW_ABSCENT_DATA;
				}
				itemval.Format("ACCTNO=%s", temp);
				pair< CString, CString> pairACCTNO("ACCTNO", itemval);
				vlist.push_back(pairACCTNO);
				//支付条件
				if (track2.GetLength() > pos+12)
				{
					temp = track2.Mid(pos+12,1);
					itemval.Format("ZHIFTJ=%s", temp);
					pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
					vlist.push_back(pairZHIFTJ);
				}

				////取凭证种类
				if (track3.GetLength() > pos+14)
				{
					temp = track3.Mid(pos+12,3);
					itemval.Format("PNGZZL=%s", temp);
					pair< CString, CString> pairPNGZZL("PNGZZL", itemval);
					vlist.push_back(pairPNGZZL);
				}

				////分行代码
				if (track3.GetLength() > pos+16)
				{
					temp = track3.Mid(pos+15,2);
					itemval.Format("FENHDM=%s", temp);
					pair< CString, CString> pairFENHDM("FENHDM", itemval);
					vlist.push_back(pairFENHDM);
				}

				////凭证批号
				if (track3.GetLength() > pos+20)
				{
					temp = track3.Mid(pos+17,4);
					itemval.Format("PNGZPH=%s", temp);
					pair< CString, CString> pairPNGZPH("PNGZPH", itemval);
					vlist.push_back(pairPNGZPH);
				}

				////凭证序号
				if (track3.GetLength() > pos+28)
				{
					temp = track3.Mid(pos+21,8);
					itemval.Format("PNGZXH=%s", temp);
					pair< CString, CString> pairPNGZXH("PNGZXH", itemval);
					vlist.push_back(pairPNGZXH);
				}
				
			}
			else
			{
				return ERR_PASSBK_NEW_ABSCENT_DATA;
			}
		}
		if (_cardtype == PASSBK_OLD1)
		{
			if (track2.GetLength() < 21)
			{
				return ERR_PASSBK_OLD1_ABSCENT_DATA;
			}
			
			temp = track2.Mid(3,18);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
			
		}
		if (_cardtype == PASSBK_OLD2)
		{
			if (track2.GetLength() < 20)
			{
				return ERR_PASSBK_OLD2_ABSCENT_DATA;
			}
			
			temp = track2.Mid(2,18);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
		}
		if (_cardtype == OTHER_BANK_CARD)
		{

			int pos = track2.Find('=');
			if (-1 == pos)
			{
				pos = track2.Find('\'');
			}
			if (-1 == pos)
			{
				pos =  track2.Find('>');
			}
			if (pos > 0)
			{
				temp = track2.Mid(0,pos);
				itemval.Format("ACCTNO=%s", temp);
				pair< CString, CString> pairACCTNO("ACCTNO", itemval);
				vlist.push_back(pairACCTNO);
			}
			else
			{
				//return ERR_OTHER_BANK_CARD_ABSCENT_DATA;
			}
		}
		if (_cardtype == STOCK_CARD)
		{
			//股金证号
			temp = track3.Mid(11,13);
			itemval.Format("PNGZHM=%s", temp);
			pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
			vlist.push_back(pairPNGZHM);
			
			//股东号
			temp = track3.Mid(1,9);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			
			vlist.push_back(pairACCTNO);
		}
		if (_cardtype == HOUSE_FUND)
		{
			//房维凭证号
			temp = track3.Mid(17,13);
			itemval.Format("PNGZHM=%s", temp);
			pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
			vlist.push_back(pairPNGZHM);
			
			//房维账号
			temp = track3.Mid(1,15);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			
			vlist.push_back(pairACCTNO);
		}

		if (_cardtype == VIP_CARD)
		{
			temp = "59"+track2;
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
		}
		if (_cardtype == IDENTITY_CARD)
		{
			temp = track2.Mid(0,13);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);

			temp = track2.Mid(0,13);
			itemval.Format("PNGZHM=%s", temp);
			pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
			vlist.push_back(pairPNGZHM);

			temp = track2.Mid(13,6);
			itemval.Format("CHCKNO=%s", temp);
			pair< CString, CString> pairCHCKNO("CHCKNO", itemval);
			vlist.push_back(pairCHCKNO);
		}

		//磁道2，3数据也返回
		itemval.Format("TRACK2=%s", track2);
		pair< CString, CString> nsubTrack2("TRACK2", itemval);
		vlist.push_back(nsubTrack2);
		
		itemval.Format("TRACK3=%s", track3);
		pair< CString, CString> nsubTrack3("TRACK3", itemval);
		vlist.push_back(nsubTrack3);

		//返回磁条格式
		itemval.Format("MAGTYPE=%s", _cardtype);
		pair< CString, CString> subMAGTYPE("MAGTYPE", itemval);
		vlist.push_back(subMAGTYPE);
		
		return SUCCESS;
		//以上为2012-08-21更改磁条格式判断规则,取各种凭证的账号域
	}
	return SUCCESS;
}


int CDriverMag::WriteICd(const char *magformat, const char *trackno, int isReWrite, VPair &vlist)
{
	//调用厂商驱动
	if(_bThirdCall)
	{
		CString trackNo;
		trackNo.Format("%s", trackno);
		PAIR track("trackno", trackNo);
		vlist.insert(vlist.begin(), track);

		typedef pair<CString, CString> PAIR;
		PAIR dllnamepr("thirdll", _moduledir + _ThirdDll);
		vlist.insert(vlist.begin(), dllnamepr);
        if (0 == _ntimeout)
        {
			_ntimeout = _ndeftimeout;
        }

		CString tmptimeout;
		tmptimeout.Format("%d", _ntimeout);
		PAIR timeout("timeout", tmptimeout);
		vlist.push_back(timeout);

		CString tmpbaudrate;
		tmpbaudrate.Format("%d", m_baudrate);
		PAIR baudrate("baudrate", tmpbaudrate);
		vlist.push_back(baudrate);
		
		TCHAR buf[255];
		memset(buf, 0, 255);
	
		typedef BOOL (CALLBACK * LPGETMAGCOM)(MAG_INTERFACE *);

		HMODULE hDll = LoadLibrary(_moduledir + _CallDll);

		if(!hDll)
		{
			return ERR_NOFINDMATCHDLL;
		}
        LPGETMAGCOM lpMagCom = (LPGETMAGCOM)GetProcAddress(hDll, "GetMagCOM");
		if(!lpMagCom)
		{
			FreeLibrary(hDll);
			return ERR_NOFINDMATCHDLL;
		}

		MAG_INTERFACE stCOM;
		memset(&stCOM, 0, sizeof(MAG_INTERFACE));
		BOOL bGetCOM = lpMagCom(&stCOM);
		if(!bGetCOM)
		{
			FreeLibrary(hDll);
			return ERR_WRITETRACK;
		}

	
		char errcd[20];
		memset(errcd, 0, 20);
		char errmsg[256];
	    memset(errmsg, 0, 256);
		short isUSB = (_isCom != 2 ? 0 : 1);
		int ret = stCOM.WriteCard(_nPort, isUSB, vlist, errcd, errmsg);
		if(ret != 0)
		{
			_errcd = errcd;
			_errmsg = errmsg;
			FreeLibrary(hDll);
			return ERR_READFAIL;
		}

		_pParaXml->FreeMagFormatList();
		FreeLibrary(hDll);
		return SUCCESS;
	}

	return SUCCESS;
}


int CDriverMag::InitlizeConfigParam()
{
	if(!_pParaXml->LoadMagFormatXml(_fortxmlpath))
		return ERR_LOADFMT;                                  //加载磁条格式xml文件失败
	if(_pParaXml->InitContrOrder() != 0)
		return ERR_LOADCONFIGS;
	if(_pParaXml->ConstructMagTypeList() != 0)
		return ERR_LOADCONFIGS;
	if(_pParaXml->ConstructRegList() != 0)
		return ERR_LOADCONFIGS;
	return SUCCESS;
}
CString CDriverMag::GetReadOperRspEndData(int trackno)
{
	CString endData;
	char endstr[20] = {0};
	VRWList &readlist = _pParaXml->GetRListRef();
	for(int i = 0; i < readlist.size(); i++)
	{
		stRWNode &rwnode = readlist[i];
		if(rwnode.trackno != trackno)
			continue;
		
		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			return endData;
		}
		
		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == READ && step.wrtype == RTYPE_GETOPERSTATUS)
			{
				if(_pParaXml->ParaOrder(step.endstr, endstr) == 0)
				{
					endData = endstr;
				}
				return endData;
			}
		}
	}
	return endData;
}
CString CDriverMag::GetOperRspEndData(int trackno)
{
	CString endData;
	char endstr[20] = {0};
	VRWList &writelist = _pParaXml->GetWListRef();
	for(int i = 0; i < writelist.size(); i++)
	{
		stRWNode &rwnode = writelist[i];
		if(rwnode.trackno != trackno)
			continue;
		
		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			return endData;
		}
		
		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == READ && step.wrtype == RTYPE_GETOPERSTATUS)
			{
				if(_pParaXml->ParaOrder(step.endstr, endstr) == 0)
				{
					endData = endstr;
				}
				return endData;
			}
		}
	}
	return endData;
}
int CDriverMag::GetReadOperRspDataLen(int trackno)
{
	int len = 0;
	VRWList &readlist = _pParaXml->GetRListRef();
	for(int i = 0; i < readlist.size(); i++)
	{
		stRWNode &rwnode = readlist[i];
		if(rwnode.trackno != trackno)
			continue;
		
		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			return 0;
		}
		
		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == READ && step.wrtype == RTYPE_GETOPERSTATUS)
			{
				return step.len;
			}
		}
	}
	return len;
}
int CDriverMag::GetOperRspDataLen(int trackno)
{
	int len = 0;
	VRWList &writelist = _pParaXml->GetWListRef();
	for(int i = 0; i < writelist.size(); i++)
	{
		stRWNode &rwnode = writelist[i];
		if(rwnode.trackno != trackno)
			continue;
		
		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			return 0;
		}
		
		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == READ && step.wrtype == RTYPE_GETOPERSTATUS)
			{
				return step.len;
			}
		}
	}
	return len;
}

/******************************************************************************************
int CDriverMag::WriteMag()
Author          : chenzheng  Date Of Creation: 2011-05-14
Founction       : CDriverMag::WriteMag
Return          : 0:写磁操作成功
                  其他:写磁操作失败
Parameter List  : 
Description     : 写磁
Calls           :
Called By       :
Other           : dllMag.dll
Modify Record   :
      1. author : chenzheng
          date  : 2011-05-14
          desc  : 创建
	  2. author : chenzheng
          date  : 20112-12-11
          desc  : 修改写2、3磁控制逻辑，允许写2、3磁时，2、3磁道数据有一个为空的情况
******************************************************************************************/
int CDriverMag::WriteMag(const char *magformat, const char *trackno, int isReWrite, VPair &vlist)
{
	
	if (0 == _ntimeout)
	{
		_ntimeout = _ndeftimeout;
	}

	if(!_pParaXml->LoadMagFormatXml(_fortxmlpath))
		return ERR_LOADFMT;                                  //加载磁条格式xml文件失败

	if(_pParaXml->ConstructMagTypeList() != 0)
		return ERR_GETFMTLIST; 

	if(_pParaXml->ConstructRegList() != 0)
		return ERR_CONSTRUCTREGLIST; 

	if(_pParaXml->InitContrOrder() != 0)
	{
		_pParaXml->FreeMagFormatList();
		return ERR_LOADCONFIGS;
	}
	//debug("初始化配置完成");
	CString outTrack2;
	CString outTrack3;
	int trkno = atoi(trackno);
	for(int idx = 0; idx < vlist.size(); idx ++)
	{
		pair<CString, CString> &itempair = vlist[idx];
		if(itempair.first == "TRACK2")    //如果不是完整的数据而只有部分数据，就不会有"TRACK2"字段。写磁道时要完整的数据，故下面要组装出完整的数据。
		{
			outTrack2 = itempair.second;
		}
		else if(itempair.first == "TRACK3")
		{
			outTrack3 = itempair.second;
		}
	}

    CString track2;
	CString track3;

	CString track23;
	CString senddata;
	int hasoutvalue = 0;

	track2 = outTrack2;
	track3 = outTrack3;
	debug("trkno:%d,track2:%s,track3:%s",trkno,track2,track3);

	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);

	const int strsize = 20;
	char startstr[20];
	char endstr[20];
	char compartstr[20];
	CString readdata;
	CString readdatitem;
	int orderstrlen = 0;

	_pWinPort->SetExitReadVar(TRUE);
	int ret;
	BOOL bOperStatus = FALSE;
	VRWList &writelist = _pParaXml->GetWListRef();
	for(int i = 0; i < writelist.size(); i++)
	{
		stRWNode &rwnode = writelist[i];
		if(rwnode.trackno != atoi(trackno))
			continue;

		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			_pParaXml->FreeRWList();
		    _pParaXml->FreeMagFormatList();
			return ERR_OCREXCEPTION;
		}

		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == WRITE)
			{
				if(step.wrtype == WTYPE_SETDATAFROMCONF)
				{
					memset(neworder, 0, bufsize);
					if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //将十六进制字符串转化为十进制。
					{
                         _pParaXml->FreeRWList();
		                 _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;    //转换指令错误
					}

					debug("step.writedata:%s",step.writedata);
					if(!_pWinPort->SendData(neworder, strlen(neworder)))
					{
						_pParaXml->FreeRWList();
	                     _pParaXml->FreeMagFormatList();
				         return ERR_SENDDATA;
					}
				}
				else
				{
					if(step.startstr.IsEmpty())
					{
						_pParaXml->FreeRWList();
				    _pParaXml->FreeMagFormatList();
						return ERR_MISSORDER; //丢失指令
					}
					if(step.endstr.IsEmpty())
					{
						_pParaXml->FreeRWList();
						_pParaXml->FreeMagFormatList();
						return ERR_MISSORDER;
					}

					memset(startstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.startstr, startstr) != 0)
					{
						 _pParaXml->FreeRWList();
					     _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;
					}

					memset(endstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.endstr, endstr) != 0)
					{
						 _pParaXml->FreeRWList();
						 _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;
					}

					senddata.Empty();
					if(trkno == 2)
					{
						senddata = startstr;
						senddata += track2;
						senddata += endstr;
					}
					else if(trkno == 3)
					{
						senddata = startstr;
						senddata += track3;
						senddata += endstr;
					}
					else
					{	
						if(step.compartstr.IsEmpty())
						{
							_pParaXml->FreeRWList();
							_pParaXml->FreeMagFormatList();
							return ERR_MISSORDER;
						}
						memset(compartstr, 0, strsize);
						if(_pParaXml->ParaOrder(step.compartstr, compartstr) != 0)
						{
						   _pParaXml->FreeRWList();
						   _pParaXml->FreeMagFormatList();
				           return ERR_INVALIDORDER;
						}

						senddata = startstr;
						/*
						if (track2.IsEmpty())
						{
							char tmpChar = 0x7F;
							senddata += tmpChar;
							
						}
						else
						{
							senddata += track2;
						}
						*/
						senddata += track2;
						
						//这样做的目的：当到调用写23磁道指令时，可以成功写入一个磁道数据
						//if(track2.IsEmpty() == FALSE && track3.IsEmpty() == FALSE)
						//{
							//senddata += compartstr;
						//}
						
						if (track3.IsEmpty())
						{
							//char tmpChar = 0x7F;
							//senddata += tmpChar;
						} 
						else
						{
							senddata += compartstr;
							//senddata += track3;
						}
						
						senddata += track3;
						senddata += endstr;
					}
	
					debug("write data[AsynSendData]:%s",senddata);
					if(!_pWinPort->AsynSendData(senddata.GetBuffer(senddata.GetLength()), senddata.GetLength()))
					{
						 _pParaXml->FreeRWList();
						 _pParaXml->FreeMagFormatList();
				         return ERR_SENDDATA;
					}

				}
			}
			else
			{
				if(step.wrtype == RTYPE_GETTRACKDATA)
				{ }
				else
				{
					memset(startstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.startstr, startstr) != 0)
					{
						_pParaXml->FreeRWList();
						_pParaXml->FreeMagFormatList();
						return ERR_INVALIDORDER;
					}
					
					memset(endstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.endstr, endstr) != 0)
					{
						_pParaXml->FreeRWList();
						 _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;
					}
					
					readdata.Empty();
					_pWinPort->InitVar();
					char ch;

					//例如：我使用写23磁道指令只写2磁道数据，返回的操作响应数据长度是写2磁道指令配置流程中的长度，
					//所以新增了GetOperRspDataLen这个函数获取操作响应长度。
					int rsplen = 0;
					if(track2.IsEmpty() == FALSE && track3.IsEmpty() == FALSE)
					{
						rsplen = step.len;
					}
					else
					{
						if(!track2.IsEmpty())
							rsplen = GetOperRspDataLen(2);
						else
							rsplen = GetOperRspDataLen(3);
					}
					if(rsplen == 0)
					{
						_pParaXml->FreeMagFormatList();
						_pParaXml->FreeRWList();
						return ERR_OCREXCEPTION;
					}

					while((ret = _pWinPort->sReadChar(ch, _ntimeout)) == 1)  //超时时间为20S
					{
						if(ch == 0)
							continue;
						debug("ch:%x",ch);
						readdata += ch;
						if(readdata.GetLength() == rsplen)
						{
							break;
						}
					}
					if(ret <= 0)
					{
						_pParaXml->FreeRWList();
						_pParaXml->FreeMagFormatList();
						if(ret == ERR_RESET)
						{
							return SUCCESS_RESET;
						}
						return ERR_WRITETIMEOUT;
					}

//					BOOL bHeadTrue = FALSE;
					BOOL bEndTrue = FALSE;
// 					orderstrlen = strlen(startstr);
// 					if(orderstrlen && step.len > orderstrlen)
// 					{
// 						readdatitem = readdata.Mid(0, orderstrlen);
// 						if(readdatitem == startstr)
// 							bHeadTrue = TRUE;
// 					}
					debug("read data:%s",readdata);
					orderstrlen = strlen(endstr);
					debug("orderstrlen:%d,endstr:%s",orderstrlen,endstr);
					if(orderstrlen && step.len > orderstrlen) 
					{
						readdatitem = readdata.Right(orderstrlen);

						
						//虽然是写2、3磁的指令，但实际只写2磁或3磁时，实际返回的写磁状态数据是只写2磁或3磁的状态码，所以此处要根据情况从配置文件中取写2磁或3磁状态码，然后和串口实际返回状态数据比较，以确定写磁是否成功。
						CString tmpEndString;
						tmpEndString = endstr;
						if(track2.IsEmpty())
						{
							tmpEndString = GetOperRspEndData(3);
						}
						if(track3.IsEmpty())
						{
							tmpEndString = GetOperRspEndData(2);
						}
						
						if(readdatitem == tmpEndString)
							bEndTrue = TRUE;
					}

					if(bEndTrue /*&& bHeadTrue*/)
						bOperStatus = TRUE;
				}
			}
		}
	}
	
	int retcode ;
	if(bOperStatus)
		retcode = SUCCESS;
	else
		retcode = ERR_WRITETRACK;
	_pParaXml->FreeMagFormatList();
	_pParaXml->FreeRWList();
	return retcode;
}

VPair &CDriverMag::GetMagInfoListRef()
{
	return _itemlist;
}

int CDriverMag::ReadMag(const char *magformat, const char *trackno, VPair &vlist)
{
	if (0 == _ntimeout)
	{
		_ntimeout = _ndeftimeout;
	}
    //清空读缓冲区
	_pWinPort->AsynClearReadBuffer();

	if(InitlizeConfigParam() != 0)
	{
		_pParaXml->FreeRWList();
		_pParaXml->FreeMagFormatList();
		return ERR_LOADCONFIGS;
	}
	int magtype = atoi(trackno);
    CString track2;
	CString track3;

	CString track23;
	CString senddata;

	char neworder[255];
	const int bufsize = 255;
	memset(neworder, 0, bufsize);

	const int strsize = 20;
	char startstr[20];
	char endstr[20];
	char compartstr[20];
	CString readdata;
	CString readdatitem;
	int startstrlen = 0;
	int endstrlen = 0;
	int compstrlen = 0;
	bool isNoTrack3 = false;
	bool isNoTrack2 = false;


    _pWinPort->SetExitReadVar(TRUE);
	int ret;
	int i;
	char ch;
	BOOL bOperStatus = FALSE;
	VRWList &readlist = _pParaXml->GetRListRef();
	for(i = 0; i < readlist.size(); i++)
	{
		stRWNode &rwnode = readlist[i];
		if(rwnode.trackno != atoi(trackno))
			continue;

		VStepList *pStepList = rwnode.pList;
		if(pStepList == 0)
		{
			_pParaXml->FreeRWList();
		    _pParaXml->FreeMagFormatList();
			return ERR_OCREXCEPTION;
		}

		for(int j = 0; j < pStepList->size(); j++)
		{
			stOperStep &step = (*pStepList)[j];
			if(step.opername == WRITE)
			{
				if(step.wrtype == WTYPE_SETDATAFROMCONF)
				{
					memset(neworder, 0, bufsize);
					if(_pParaXml->ParaOrder(step.writedata, neworder) != 0)  //将十六进制字符串转化为十进制。
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;    //转换指令错误
					}

					if(!_pWinPort->SendData(neworder, strlen(neworder)))
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
				         return ERR_SENDDATA;
					}
				}
			}
			else
			{
				if(step.wrtype == RTYPE_GETTRACKDATA)
				{
					if(step.startstr.IsEmpty())
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_MISSORDER; //丢失指令
					}
					if(step.endstr.IsEmpty())
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_MISSORDER;
					}
					/*if(step.compartstr.IsEmpty())   //读2或3磁道时，此指令可能没有
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_MISSORDER;
					}*/
					
					memset(startstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.startstr, startstr) != 0)
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_INVALIDORDER;
					}
					startstrlen = strlen(startstr);
					
					memset(endstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.endstr, endstr) != 0)
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
				         return ERR_INVALIDORDER;
					}
					endstrlen = strlen(endstr);

					memset(compartstr, 0, strsize);
					if(_pParaXml->ParaOrder(step.compartstr, compartstr) != 0)
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
				        return ERR_INVALIDORDER;
					}

					readdata.Empty();
					_pWinPort->InitVar();

					while((ret = _pWinPort->sReadChar(ch, _ntimeout)) == 1)  //超时时间为20S
					{
						//肖健 2014-05-29
						//某些设备读出的第一个字符为0x00,导致readdata的第一个字符就为终止符，读不到数据
						if(ch==0)
							continue;
						readdata += ch;
						if(readdata.GetLength() > endstrlen)
						{
							if(readdata.Right(endstrlen) == endstr)
								break;
						}
					}

					if(ret == ERR_RESET)
					{
						_pParaXml->FreeRWList();
						_pParaXml->FreeMagFormatList();
				        return SUCCESS_RESET;
					}

					if(ret < 0)
					{
						_pParaXml->FreeRWList();
						_pParaXml->FreeMagFormatList();
						if(ret == _ERR_TIMEOUT)
						{
							return ERR_READTIMEOUT;
						}
						return ERR_READDATA;
					}

					BOOL bHeadTrue = FALSE;
					BOOL bEndTrue = FALSE;					
					if(startstrlen && readdata.GetLength() > startstrlen)
					{
						readdatitem = readdata.Mid(0, startstrlen);
						if(readdatitem == startstr)
							bHeadTrue = TRUE;
					}
					else
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_READTRACK;
					}
					
					if(endstrlen && readdata.GetLength() > endstrlen)
					{
						readdatitem = readdata.Right(endstrlen);
						if(readdatitem == endstr)
							bEndTrue = TRUE;
					}
					else
					{
						_pParaXml->FreeRWList();
		                _pParaXml->FreeMagFormatList();
						return ERR_READTRACK;
					}
					
					if(bEndTrue && bHeadTrue)
					{
						int pos = readdata.Find(compartstr);
						if(atoi(trackno) == 23)
						{
							if(pos == -1)
							{
								_pParaXml->FreeRWList();
								_pParaXml->FreeMagFormatList();
								return ERR_READTRACK;
							}
						}

						if(atoi(trackno) == 2)
							track2 = readdata.Mid(startstrlen, readdata.GetLength() - endstrlen - startstrlen);
						else if(atoi(trackno) == 3)
							track3 = readdata.Mid(startstrlen, readdata.GetLength() - endstrlen - startstrlen );
						else if(atoi(trackno) == 23)
						{
							track2 = readdata.Mid(startstrlen, pos - startstrlen);
							int track2len = track2.GetLength();
							if ((track2len == 0) || (1 == track2len && 0x7F == track2[0]))
							{
								isNoTrack2 = true;
								track2.Empty();
							}
							track3 = readdata.Mid(pos + 1, readdata.GetLength() - endstrlen - pos - 1 );
							int track3len = track3.GetLength();
							if ((track3len == 0) || (1 == track3len && 0x7F == track3[0]))
							{
								isNoTrack3 = true;
								track3.Empty();
							}
							
						}

					}
				}
				else
				{
				}
			}
		}
	}
	
	vlist.clear();
	

	int track2len = track2.GetLength();
	int track3len = track3.GetLength();

	if(track2len == 0 && track3len == 0)
	{
		return ERR_CARD_ABSCENT_DATA;
	}
	CString strformat ;
	if(magformat == NULL || magformat == "")
	{
		//2012-08-21更改磁条格式判断规则
		//GetMagFormatByTrackLen(track2, track2len, track3, track3len, strformat);
		GetMagFormatByTrackData(track2, track3, strformat);
	}
	else
	{
		strformat = magformat;
	}

	//磁道信息为空
	if(strformat == EMPTY_MAG)
	{
		return ERR_CARD_ABSCENT_DATA;
	}

	_cardtype = strformat;
  	CString temp;
	CString itemval;
// 	temp.Format("track2len:%d\r\ntrack3len: %d\r\nformatname:%s", track2len, track3len, strformat);
 	//AfxMessageBox(_cardtype);
	//以下为2012-08-21更改磁条格式判断规则,取各种凭证的账号域
	if (_cardtype == GYCARD)
	{
		temp = track2.Mid(0,6);
		if (temp.GetLength() != 6)
		{
			return ERR_READTRACK;
		}
		itemval.Format("USERID=%s", temp);
		pair< CString, CString> pairUSERID("USERID", itemval);
		vlist.push_back(pairUSERID);
	}
	if (_cardtype == CARD)
	{
		int pos = track2.Find('=');
		if (-1 == pos)
		{
			pos = track2.Find('\'');
		}
		if (-1 == pos)
		{
			pos =  track2.Find('>');
		}
		if (pos > 0)
		{
			temp = track2.Mid(0,pos);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
		}
		else
		{
			return ERR_CARD_ABSCENT_DATA;
		}
	}
	if (_cardtype == PASSBK_COMPANY)
	{
		//凭证号
		temp = track3.Mid(1,13);
		itemval.Format("PNGZHM=%s", temp);
		pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
		vlist.push_back(pairPNGZHM);

		//对公账户
		temp = track3.Mid(22,18);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);

		//支付条件
		temp = track3.Mid(41,3);
		itemval.Format("ZHIFTJ=%s", temp);
		pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
		vlist.push_back(pairZHIFTJ);
	}
	if (_cardtype == PASSBK_PERSON)
	{
		//凭证代号
		temp = track3.Mid(40,13);
		itemval.Format("PNGZHM=%s", temp);
		pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
		vlist.push_back(pairPNGZHM);
		
		//一本通账号
		temp = track3.Mid(21,15);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);
		
		//支付条件
		temp = track3.Mid(36,3);
		itemval.Format("ZHIFTJ=%s", temp);
		pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
		vlist.push_back(pairZHIFTJ);
	}
	if (_cardtype == PASSBK_NEW)
	{
		int pos = track2.Find('=');
		if (-1 == pos)
		{
			pos = track2.Find('\'');
		}
		if (-1 == pos)
		{
			pos =  track2.Find('>');
		}
		if (pos > 0)
		{
			//temp.Format("%d", pos);
			//AfxMessageBox(temp);
			//账号
			temp = track2.Mid(2,pos-2);
			//AfxMessageBox(temp);
			//判断2、3磁道账号是否一致
			if (track3.GetLength() < (13 + temp.GetLength()))
			{
				return ERR_PASSBK_NEW_ABSCENT_DATA;
			}
			if (temp != track3.Mid(13, temp.GetLength()))
			{
				return ERR_PASSBK_NEW_ABSCENT_DATA;
			}
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
			//支付条件
			if (track2.GetLength() > pos+12)
			{
				temp = track2.Mid(pos+12,1);
				itemval.Format("ZHIFTJ=%s", temp);
				pair< CString, CString> pairZHIFTJ("ZHIFTJ", itemval);
				vlist.push_back(pairZHIFTJ);
			}

			////取凭证种类
		    if (track3.GetLength() > pos+14)
			{
				temp = track3.Mid(pos+12,3);
				itemval.Format("PNGZZL=%s", temp);
				pair< CString, CString> pairPNGZZL("PNGZZL", itemval);
				vlist.push_back(pairPNGZZL);
			}

			////分行代码
			if (track3.GetLength() > pos+16)
			{
				temp = track3.Mid(pos+15,2);
				itemval.Format("FENHDM=%s", temp);
				pair< CString, CString> pairFENHDM("FENHDM", itemval);
				vlist.push_back(pairFENHDM);
			}

			////凭证批号
			if (track3.GetLength() > pos+20)
			{
				temp = track3.Mid(pos+17,4);
				itemval.Format("PNGZPH=%s", temp);
				pair< CString, CString> pairPNGZPH("PNGZPH", itemval);
				vlist.push_back(pairPNGZPH);
			}

			////凭证序号
			if (track3.GetLength() > pos+28)
			{
				temp = track3.Mid(pos+21,8);
				itemval.Format("PNGZXH=%s", temp);
				pair< CString, CString> pairPNGZXH("PNGZXH", itemval);
				vlist.push_back(pairPNGZXH);
			}
			
		}
		else
		{
			return ERR_PASSBK_NEW_ABSCENT_DATA;
		}
	}
	if (_cardtype == PASSBK_OLD1)
	{
		if (track2.GetLength() < 21)
		{
			return ERR_PASSBK_OLD1_ABSCENT_DATA;
		}
		
		temp = track2.Mid(3,18);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);
		
	}
	if (_cardtype == PASSBK_OLD2)
	{
		if (track2.GetLength() < 20)
		{
			return ERR_PASSBK_OLD2_ABSCENT_DATA;
		}
		
		temp = track2.Mid(2,18);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);
	}
	if (_cardtype == OTHER_BANK_CARD)
	{

		int pos = track2.Find('=');
		if (-1 == pos)
		{
			pos = track2.Find('\'');
		}
		if (-1 == pos)
		{
			pos =  track2.Find('>');
		}
		if (pos > 0)
		{
			temp = track2.Mid(0,pos);
			itemval.Format("ACCTNO=%s", temp);
			pair< CString, CString> pairACCTNO("ACCTNO", itemval);
			vlist.push_back(pairACCTNO);
		}
		else
		{
			//return ERR_OTHER_BANK_CARD_ABSCENT_DATA;
		}
		
		

	}
	if (_cardtype == STOCK_CARD)
	{
		//股金证号
		temp = track3.Mid(11,13);
		itemval.Format("PNGZHM=%s", temp);
		pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
		vlist.push_back(pairPNGZHM);
		
		//股东号
		temp = track3.Mid(1,9);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		
		vlist.push_back(pairACCTNO);
	}
	if (_cardtype == HOUSE_FUND)
	{
		//房维凭证号
		temp = track3.Mid(17,13);
		itemval.Format("PNGZHM=%s", temp);
		pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
		vlist.push_back(pairPNGZHM);
		
		//房维账号
		temp = track3.Mid(1,15);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		
		vlist.push_back(pairACCTNO);
	}

	if (_cardtype == VIP_CARD)
	{
		temp = "59"+track2;
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);
	}
	if (_cardtype == IDENTITY_CARD)
	{
		temp = track2.Mid(0,13);
		itemval.Format("ACCTNO=%s", temp);
		pair< CString, CString> pairACCTNO("ACCTNO", itemval);
		vlist.push_back(pairACCTNO);

		temp = track2.Mid(0,13);
		itemval.Format("PNGZHM=%s", temp);
		pair< CString, CString> pairPNGZHM("PNGZHM", itemval);
		vlist.push_back(pairPNGZHM);

		temp = track2.Mid(13,6);
		itemval.Format("CHCKNO=%s", temp);
		pair< CString, CString> pairCHCKNO("CHCKNO", itemval);
		vlist.push_back(pairCHCKNO);
	}

	//磁道2，3数据也返回
	itemval.Format("TRACK2=%s", track2);
	pair< CString, CString> nsubTrack2("TRACK2", itemval);
	vlist.push_back(nsubTrack2);
	
	itemval.Format("TRACK3=%s", track3);
	pair< CString, CString> nsubTrack3("TRACK3", itemval);
	vlist.push_back(nsubTrack3);

	//返回磁条格式
	itemval.Format("MAGTYPE=%s", _cardtype);
	pair< CString, CString> subMAGTYPE("MAGTYPE", itemval);
	vlist.push_back(subMAGTYPE);
	
	_pParaXml->FreeRWList();
    _pParaXml->FreeMagFormatList();
    return SUCCESS;
	//以上为2012-08-21更改磁条格式判断规则,取各种凭证的账号域

    CString ReadBuf = _pWinPort->chRcvbuf;
	VMagTp &tplist = _pParaXml->GetMagTpVList();
    
	

	
    for (i = 0; i < tplist.size(); i ++)
    {
		stMagFormatTp &tp = tplist[i];
        if(tp.magname == strformat)
		{
			if(tp.pList == 0)
			{
				_pParaXml->FreeRWList();
		        _pParaXml->FreeMagFormatList();
				return ERR_GETFMTLIST;
			}

			for(int j = 0; j < tp.pList->size(); j ++)
			{
				stMagFormat &format = (*tp.pList)[j];
				if(format.track == magtype && format.track == 2)
				{
					if(format.type == SEPARVAL)
						continue;
                    temp = track2.Mid(format.startps, format.len);
				}
				else if(format.track == magtype && format.track == 3)
				{
					if(format.type == SEPARVAL)
						continue;
                    temp = track3.Mid(format.startps, format.len);                   
				}
				else
				{
					if(format.type == SEPARVAL)
						continue;
					if(format.track == 2)
                        temp = track2.Mid(format.startps, format.len);
					else
						temp = track3.Mid(format.startps, format.len);
				}

				temp.Remove('=');
				temp.Remove('\'');
				temp.Remove('>');
				if(format.type == OUTOFVAL)
				{
					itemval.Format("%s=%s", format.outofval, temp);
					pair< CString, CString> sub(format.name, itemval);
				    vlist.push_back(sub);
				}
				itemval.Format("%s=%s", format.name, temp);
				pair< CString, CString> sub(format.name, itemval);
				vlist.push_back(sub);
			}
			break;
		}
    }

	//磁道2，3数据也返回
	itemval.Format("TRACK2=%s", track2);
	pair< CString, CString> subTrack2("TRACK2", itemval);
	vlist.push_back(subTrack2);

	itemval.Format("TRACK3=%s", track3);
	pair< CString, CString> subTrack3("TRACK3", itemval);
	vlist.push_back(subTrack3);

	_pParaXml->FreeRWList();
    _pParaXml->FreeMagFormatList();
    return SUCCESS;
}

void CDriverMag::GetMagFormatByTrackLen(CString &track2dat, int track2len, CString &track3dat, int track3len, CString &magname)
{
	VRegList &reglist = _pParaXml->GetRegListRef();
	int times = 0;
	int i = 0;
	for(i = 0; i < reglist.size(); i ++)
	{
		stMagFormatReg &regnode = reglist[i];
		if(track2len == regnode.track2len && track3len == regnode.track3len)
		{
			magname = regnode.name;
			times ++;
		}
	}
	
	if(times > 1)
	{
		CString strflg;
		for(i = 0; i < reglist.size(); i ++)
		{
			stMagFormatReg &regnode = reglist[i];
			
			if(track2len == regnode.track2len && track3len == regnode.track3len)
			{			
				if(regnode.flagtk == 2)
				{
					strflg = track2dat.Mid(regnode.flagpos, regnode.flagval.GetLength());
					if(strflg == regnode.flagval)
					{
						magname = regnode.name;
						break;
					}
				}
				else if(regnode.flagtk == 3)
				{
					strflg = track3dat.Mid(regnode.flagpos, regnode.flagval.GetLength());
					if(strflg == regnode.flagval)
					{
						magname = regnode.name;
						break;
					}
				}
				else
				{
					continue;
				}
			}
		}		
	}
}

void CDriverMag::GetMagFormatByTrackData(CString track2dat, CString track3dat, CString &magname)
{
	
	if (!track2dat.IsEmpty() && track2dat != "0")
	{
		if (track2dat.GetLength() == 11 && (track2dat.Mid(0,4) == "1688" || track2dat.Mid(0,4) == "2688"))
		{
			magname = VIP_CARD;//对公贵宾卡
			return;
		}
		else if (track2dat.GetLength() == 19 && track2dat.Mid(0,2) == "45")
		{
			magname = IDENTITY_CARD;//身份识别卡
			return;
		}
		else
		{
			magname = CARD;//本行卡
			return;
		}
	}
	else
	{
		if (!track3dat.IsEmpty() && track3dat != "0")
		{
			if(track3dat.GetLength() == 25)
			{
				magname = STOCK_CARD;//股金证
				return;
			}
			else if (track3dat.GetLength() == 31)
			{
				magname = HOUSE_FUND;//房维专用折
				return;
			}
			else if (track3dat.Mid(0,1) == "0")
			{
				magname = PASSBK_COMPANY;//对公存折
				return;
			}
			else if(track3dat.Mid(0,6) == "940062")
			{
				magname = PASSBK_PERSON;//对私存折
				return;
			}
			else
			{
				magname = OTHER_BANK_BOOK;//他行折
				return;
			}
			
		}
		else
		{
			magname = EMPTY_MAG;//空磁道信息
			return;
		}
	}
}