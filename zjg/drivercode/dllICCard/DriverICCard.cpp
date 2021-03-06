#include "StdAfx.h"
#include "Win32Port.h"
#include "paraxml.h"
#include "DriverICCard.h"





bool Hex2Raw(BYTE *pData, const char *szMsg, int nMsgLen)
{
	int hi, lo;
	for (int i=0;i<nMsgLen/2;i++)
	{
		hi=Char2Hex(szMsg[i*2]);
		lo=Char2Hex(szMsg[i*2+1]);
		if (hi<0 || lo<0) return false;
		pData[i]=(hi<<4)+lo;
	}
	return true;
}

void Raw2HexStr(char *pStr, const BYTE *pRaw, int nRawLen)
{
	BYTE b;
	for (int i=0; i<nRawLen; i++)
	{
		b=pRaw[i];
		pStr[2*i]=Hex2Char(b>>4);
		pStr[2*i+1]=Hex2Char(b & 0x0f);
	}
	pStr[2*nRawLen]=0;
}


int ParaOrder(CString &old, char *pOutNew)
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

int ConvertHexStr2Deci(char *pStrHex)
{
	int sRault = 0;
	int StrHexLen = strlen(pStrHex);
	//char tmpBuff[40] = {0};
	char *ptr = pStrHex+StrHexLen-1;
	for(int i = 0; i < StrHexLen; i++)
	{
		char ch = *ptr;
		if (ch >= '0' && ch <= '9')
		{
			if (i == 0)
			{
				sRault += atoi(&ch);
			}
			else
			{
				int iTmp = atoi(&ch)*16;
				for(int j = 1; j < i; j++)
				{
					iTmp = iTmp*16;
				}
				sRault += iTmp;
			}
		} 
		else if(ch >= 'A' && ch <= 'F')
		{
			if (i == 0)
			{
				sRault += ch - 'A' + 10;
			}
			else
			{
				int iTmp = (ch - 'A' + 10)*16;
				for(int j = 1; j < i; j++)
				{
					iTmp = iTmp*16;
				}
				sRault += iTmp;
			}
		}
		else if(ch >= 'a' && ch <= 'f')
		{
			if (i == 0)
			{
				sRault += ch - 'a' + 10;
			}
			else
			{
				int iTmp = (ch - 'a' + 10)*16;
				for(int j = 1; j < i; j++)
				{
					iTmp = iTmp*16;
				}
				sRault += iTmp;
			}
		}
		ptr--;
		
	}
	return sRault;
}
/*
获得tag 的值
tagname:要找的TAG
taglen: 要找的TAG的长度
indata: 查找TAG的数据源
inlen:  数据源的长度
tagvalue: 返回TAG的值
tagvaluelen:返回TAG值的长度
*/
int gwiGettagvalue(unsigned char *tagname,int taglen,unsigned char * indata,int inlen,unsigned char * tagvalue,int * tagvaluelen)
{
	unsigned char * p;
	int ilen=0;
	
	p=gwiFindtag(tagname,taglen,indata,inlen);
	if(p==NULL)
		return -1;
	p+=taglen;
	ilen=0;
	if(p[0]==0x82)
	{
		ilen+=(p[1]*256+p[2])&0x0FFFF;
		p+=3;
	}
	else
		if(p[0]==0x81)
		{
			ilen=indata[1];
			p+=2;
		}
		else
		{
			ilen=p[0];
			p+=1;
		}
		
		memcpy(tagvalue,p,ilen);
		
		(*tagvaluelen)=ilen;
		return 0;  
}


/*
功能：查找指定标签的数据(FCP模板62开头，用于文件控制参数,FMD模板64开头，用于文件管理参数)
参数：tagname,标签名
	  taglen, 标签长度
	  indata, 提供查找的源数据
      inlen,  源数据的长度
              0为不限制长度，遇到标签为0或FF退出
*/
unsigned char * gwiFindtag(unsigned  char *tagname, int taglen, unsigned char *indata,int inlen)
{
	unsigned char * p;
	
	unsigned char *pp;
	unsigned char tmptag[256],tmptaglen=0;
        int iloop=0;
	p=NULL;
	iloop=0;
	if(inlen==0)
	{
		while(indata[iloop]!=0x00)
		{
		    if(indata[iloop]==0x0FF)
			  return p;
		    tmptag[0]=indata[iloop];
		    tmptaglen=1;
		    if((indata[iloop]&0x1F)==0x1F)     //低五位为全1，表示多字节标签，否则为单字节标签
		    {    
			 iloop++;                     //多字节标签
		         while((indata[iloop]&0x80)&&(iloop<inlen))    //b8=1,后面还有一字节，b8=0，最后一个字节
			 {
				 if(indata[iloop]==0x0FF)
		                      return p;
				 tmptag[tmptaglen]=indata[iloop];
				 tmptaglen++;
				 iloop++;
			 }
			 tmptag[tmptaglen]=indata[iloop];
			 tmptaglen++;
			
		   }
		   iloop++;
		   if((memcmp(tagname,tmptag,tmptaglen)==0)&&(taglen==tmptaglen))//找到标签
	  	   {
			
			p=&indata[iloop-tmptaglen];
			return p;
		   }
		//未找到标签
		   if(indata[iloop-tmptaglen]&0x20)         //b6=0,基本数据，b6=1,结构数据
		   {
			if(indata[iloop]==0x82)              //82 三字节长度。Len 范围为0-65535,后面跟两字节长度
				iloop+=3;
			else
				if(indata[iloop]==0x81)          //81 两字节长度  Len 范围为0-255,后面跟一字节长度
                                    iloop+=2;
				else
					iloop++;                    //0-127,一字节长度  TLV 长度后面是标签
		    }
	  	    else
		    {                             //基本数据，加上整个数据对象的长度
                        if(indata[iloop]==0x82)
			{
				iloop+=(indata[iloop+1]*256+indata[iloop+2])&0x0FFFF;
				iloop+=3;
			}
			else
				if(indata[iloop]==0x81)
				{
					iloop+=indata[iloop+1];
					iloop+=2;
				}
				else
					iloop+=indata[iloop]+1;
		   }                        //找下一个标签
	   }
	   return p;
       }

	while(iloop<inlen)
	{
		if(indata[iloop]==0x0FF)
			return p;
		tmptag[0]=indata[iloop];
		tmptaglen=1;
		if((indata[iloop]&0x1F)==0x1F)     //低五位为全1，表示多字节标签，否则为单字节标签
		{    
			 iloop++;                     //多字节标签
		     while((indata[iloop]&0x80)&&(iloop<inlen))    //b8=1,后面还有一字节，b8=0，最后一个字节
			 {
				 if(indata[iloop]==0x0FF)
		                      return p;
				 tmptag[tmptaglen]=indata[iloop];
				 tmptaglen++;
				 iloop++;
			 }
			 if(iloop>=inlen)
				 return p;
			 tmptag[tmptaglen]=indata[iloop];
			 tmptaglen++;
			
		}
		iloop++;
		
			pp=&indata[iloop-tmptaglen];

		if((memcmp(tagname,tmptag,tmptaglen)==0)&&(taglen==tmptaglen))//找到标签
		{
			
			p=&indata[iloop-tmptaglen];
			return p;
		}
		//未找到标签
		if(indata[iloop-tmptaglen]&0x20)         //b6=0,基本数据，b6=1,结构数据
		{
			if(indata[iloop]==0x82)              //82 三字节长度。Len 范围为0-65535,后面跟两字节长度
				iloop+=3;
			else
				if(indata[iloop]==0x81)          //81 两字节长度  Len 范围为0-255,后面跟一字节长度
                    iloop+=2;
				else
					iloop++;                    //0-127,一字节长度  TLV 长度后面是标签
		}
		else
		{                             //基本数据，加上整个数据对象的长度
            if(indata[iloop]==0x82)
			{
				iloop+=(indata[iloop+1]*256+indata[iloop+2])&0x0FFFF;
				iloop+=3;
			}
			else
				if(indata[iloop]==0x81)
				{
					iloop+=indata[iloop+1];
					iloop+=2;
				}
				else
					iloop+=indata[iloop]+1;
		}                        //找下一个标签
	}
	return p;
}

int Get_NoTagARQC( char * oldARQC, char * newARQC, char *tag9F10, char *tag95)
{
     char strasc[1024];
     int i;
     unsigned char sd[255];
     unsigned char strhex[255];
	 
     int ilen;
     unsigned char sTmp[255];
     unsigned char sARQCmw[255];
	
     int ioldARQCLen = 0;
     char s9F10tmp[100];

     //write_log_ic(__FILE__,__LINE__,0,"Get_NoTagARQC=[%d]\n",strlen(oldARQC));
     memset(newARQC, 0 , sizeof(newARQC));
     memset(strasc, 0 ,sizeof(strasc));

     if(strlen(oldARQC) == 0)
         return -1;

     ioldARQCLen = strlen(oldARQC) / 2;

     getAsc((unsigned char*)oldARQC,(unsigned char*)strasc,strlen(oldARQC));

     //取 ATC 
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9F\x36",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9f36=[%s][%d]\n",strhex,ilen);
         memset((char*)sTmp, 0 ,sizeof(sTmp));
         sprintf((char*)sTmp, "J%03d%s",strlen((char*)strhex),strhex);
         strcat((char*)newARQC, (char*)sTmp);
     }
     else
         return -1;

     //取 ARQC 密文
     memset(sd,0,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9F\x26",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9f26=[%s][%d]\n",strhex,ilen);
         memset(sTmp, 0 ,sizeof(sTmp));
         sprintf((char*)sTmp, "Q%03d%s",strlen((char*)strhex),strhex);
         strcat(newARQC, (char*)sTmp);
     }
     else
         return -1 ;

     memset(sTmp, 0 ,sizeof(sTmp));
     memset(strhex, 0 ,sizeof(strhex));
     //取  明文的授权金额
     memset(sd, 0 ,sizeof(sd));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9f\x02",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9f02=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的其他金额
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9f\x03",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9f03=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的终端国家代码
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9f\x1A",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9f1A=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的终端验证结果
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x95",1,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"95=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
		 strncpy(tag95, (char*)strhex, 255);
     }
     else
         return -1;
     //取  明文的交易货币代码
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x5F\x2A",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"5f2A=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的交易日期
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9A",1,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9A=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的交易类型
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9C",1,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9C=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的不可预知数
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9F\x37",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9F37=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的应用交互特征
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x82",1,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"82=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的应用交易计数器
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9F\x36",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9F36=[%s][%d]\n",strhex,ilen);
         strcat((char*)sTmp,(char*)strhex);
     }
     else
         return -1;
     //取  明文的卡验证结果
     memset(sd, 0 ,sizeof(sd));
     memset(strhex, 0 ,sizeof(strhex));
     ilen=0;
     i = gwiGettagvalue((unsigned char*)"\x9F\x10",2,(unsigned char*)strasc,ioldARQCLen,sd,&ilen);
     if (i==0)
     {   
         getHex(1,(unsigned char*)sd,(unsigned char*)strhex,ilen);
         //write_log_ic(__FILE__,__LINE__,0,"9F10=[%s][%d]\n",strhex,ilen);
         memset(s9F10tmp, 0 ,sizeof(s9F10tmp));
         strncpy(s9F10tmp, (char*)strhex+6,8);
         //write_log_ic(__FILE__,__LINE__,0,"9F10=[%s][%d][%s]\n",strhex,ilen,s9F10tmp);
         strcat((char*)sTmp,s9F10tmp);
		 strncpy(tag9F10, s9F10tmp, 8);
     }
     else
         return -1;

    sprintf((char*)sARQCmw,"R%03d%s",strlen((char*)sTmp),sTmp);
    strcat((char*)newARQC, (char*)sARQCmw);

    //write_log_ic(__FILE__,__LINE__,0,"newARQC=[%s]\n",newARQC);
    return 0;
}

int  getAsc(unsigned char *instr,unsigned char * outstr,int ilen)
{
	int i,j=0,tmp1=0,tmp2=0;
	for(i=0;i<ilen;i=i+2)
	{
label1:	if(instr[i]==0x20)
		{
			i=i+1;
			if (i==ilen)
				break;
			goto label1;
		}
		
		tmp1=instr[i];
		if(tmp1>='a')
			tmp1=tmp1-32;
		if(tmp1>='A')
			tmp1=tmp1-55;
		else
			tmp1=tmp1-48;
		tmp2=instr[i+1];
		if(tmp2==0x20)
			return -1;
		if(tmp2>='a')
			tmp2=tmp2-32;
		if(tmp2>='A')
			tmp2=tmp2-55;
		else
			tmp2=tmp2-48;
		outstr[j]=tmp1*16+tmp2;
		outstr[j+1]='\0';
		j++;
	}
	return j;
}


int  getHex(int mode,unsigned char *instr,unsigned char *outstr,int ilen)
{
	int i,j,tmp;
	tmp=0;
	for(i=0;i<ilen;i++)
	{
		if(mode==0)
			j=i*3;
		else
			j=i*2;
		
		tmp=(instr[i]>>4)&0x0f;
		if((mode==0)||(mode==1))
		{
			if(tmp>9)
				tmp=tmp+55;
			else
				tmp=tmp+48;
		}
		else
			tmp=tmp+48;
		outstr[j]=tmp;
		tmp=instr[i]&0x0f;
		if((mode==0)||(mode==1))
		{
			if(tmp>9)
				tmp=tmp+55;
			else
				tmp=tmp+48;
		}
		else
			tmp=tmp+48;
		outstr[j+1]=tmp;
		if(mode==0)
		{
			outstr[j+2]=32;
			outstr[j+3]='\0';
		}
		else
			outstr[j+2]='\0';
	}
	return 1;
}

HINSTANCE CDriverICCard::m_hLibModule = NULL;
HINSTANCE CDriverICCard::m_hLibModule2 = NULL;
bool CDriverICCard::m_bDriverLoaded = false;
bool CDriverICCard::m_bDriverLoaded2 = false;
CDriverICCard CDriverICCard::m_DriverICCard;

CDriverICCard::CDriverICCard()
{
	_pWinPort = new CWin32Port();
	_pParaXml = new CParaXml();
	_ErrDesc = "";
	_errcd = 0;
	_DefAID = "";
	_hasAdap = FALSE;
	_adapter = "";
	_thirdll = "";
	_timeOut = 15;
	_dllType = 1;
	_ntimeout = 0;
	_BpNO = 0;


	m_pgetIcInfo = NULL;
	m_pgetCardNo = NULL;
	m_pGetTranDetail = NULL;
	m_pgenARQC = NULL;
	m_pCtlScriptData = NULL;
	m_stream = NULL;

	m_CT_pgetIcInfo = NULL;
	m_CT_pGetTranDetail = NULL;
	m_CT_pgenARQC = NULL;
	m_CT_pCtlScriptData = NULL;
	m_CT_pPowerOff = NULL;
	m_CT_pgetIcInfoAndGenARQC = NULL;

	m_drive = NULL;

	memset(m_TxData, 0, sizeof m_TxData);
	memset(m_icappdata, 0, sizeof m_icappdata);
}

CDriverICCard::~CDriverICCard()
{
	if(_pParaXml)
	{
		delete _pParaXml;
		_pParaXml = NULL;
	}
	if (_pWinPort)
	{
		delete _pWinPort;
		_pWinPort = NULL;
	}
}

int CDriverICCard::OpenIC()
{
	if(!_pParaXml->LoadICConfXml(_confxmlpath))
		return ERR_LOADCONF;                               //加载文件失败
	int port = 0;
	int baudrate = 0;
	char parity;
	char bytesize ;
	char stopbits ;
	BOOL isCom;
	_pParaXml->GetPortParam(port, baudrate, parity, bytesize, stopbits, isCom, _hasAdap,_DefAID, _dllType, _ICFlag, _BpNO, _timeOut);
	if(!_portno.IsEmpty())
	{
		if(_portno.GetLength() <= 3)
			return ERR_OPENPORT;
		CString porttmp = _portno.Mid(3,_portno.GetLength() - 3);
		CString comlpttmp = _portno.Mid(0, 3);
		if(comlpttmp == LPT)
			_porttype = 0;
		else if(comlpttmp == USB)
			_porttype = 2;
		else
			_porttype = 1;
		port = atoi(porttmp.GetBuffer(porttmp.GetLength()));
	}
	_nPort = port;
	SetPortParam(baudrate,atoi(&parity), atoi(&bytesize), atoi(&stopbits), isCom);
	if(_porttype != 2 && _hasAdap == FALSE)
	{
		_pWinPort->SetupParam(port, baudrate, parity, bytesize, stopbits, isCom);
		if(!_pWinPort->AsynOpen())
			return ERR_OPENPORT;                              //端口打开失败
	}
    return SUCCESS;
}

void CDriverICCard::SetPortParam(int baudrate, short parity, short bytesize, short stopbits, BOOL isCom)
{
    _baudrate = baudrate;
	_parity = parity;
	_bytesize = bytesize;
	_stopbits = stopbits;
    _isCom = isCom;
}

void CDriverICCard::SetModuleDir(CString dir)
{
	_moduledir = dir;
}

CString & CDriverICCard::GetModuleDir()
{
    return _moduledir;
}

enRetCode CDriverICCard::GetErrCode()
{
	return _RetCode;
}

void CDriverICCard::SetErrCode(enRetCode code)
{
	_RetCode = code;
}

VPair &CDriverICCard::GetRetListRef()
{
	return _Sub;
}

void CDriverICCard::SetConfXmlPath(CString &path)
{
	_confxmlpath = path;
}

void CDriverICCard::SetPortNo(CString port)
{
	_portno = port;
}

int CDriverICCard::GetThirdCalled()
{
	int ret;
	if (!_pParaXml)
	{
		return ERR_GETTHIRDDLL;
	}
    ret = _pParaXml->GetAdapterInfo(_adapter, _thirdll, _etcdll);
    if (SUCCESS != ret)
    {
		return ERR_GETTHIRDDLL;
    }
    return SUCCESS;	
}

int CDriverICCard::LoadDriver()
{
	if (m_hLibModule)
	{
		return SUCCESS;
	}
	if(_thirdll.IsEmpty())
	{
	    return ERR_NODRV;
	}
	CString ThirDllPath = _moduledir + _thirdll;

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(_moduledir);
    m_hLibModule = LoadLibrary(ThirDllPath);
	SetCurrentDirectory(buf);
	if( NULL ==m_hLibModule)
	{
		return ERR_LOADDRV;
	}
	
	/*
	m_pgetIcInfo = (FUNC_getIcInfo)GetProcAddress(m_hLibModule,"gzrcb_getIcInfo");
	m_pgetCardNo = (FUNC_getCardNo)GetProcAddress(m_hLibModule,"gzrcb_getCardNo");
	m_pgenARQC = (FUNC_genARQC)GetProcAddress(m_hLibModule,"gzrcb_genARQC");
	m_pGetTranDetail = (FUNC_GetTranDetail)GetProcAddress(m_hLibModule,"gzrcb_GetTranDetail");
	m_pCtlScriptData = (FUNC_CtlScriptData)GetProcAddress(m_hLibModule,"gzrcb_CtlScriptData");
	if( m_pgetIcInfo == NULL ||
		m_pgetCardNo == NULL ||
		m_pgenARQC == NULL ||
		m_pGetTranDetail == NULL ||
		m_pCtlScriptData == NULL)
	{
		return ERR_LOADDRV;
	}
	*/

	m_CT_pgetIcInfo = (FUNC_CT_getIcInfo)GetProcAddress(m_hLibModule,"_CT_getIcInfo@32");
	m_CT_pgenARQC = (FUNC_CT_genARQC)GetProcAddress(m_hLibModule,"_CT_genARQC@32");
	m_CT_pGetTranDetail = (FUNC_CT_GetTranDetail)GetProcAddress(m_hLibModule,"_CT_GetTranDetail@24");
	m_CT_pCtlScriptData = (FUNC_CT_CtlScriptData)GetProcAddress(m_hLibModule,"_CT_CtlScriptData@40");
	m_CT_pPowerOff = (FUNC_CT_PowerOff)GetProcAddress(m_hLibModule,"_CT_PowerOff@20");
	m_CT_pgetIcInfoAndGenARQC = (FUNC_CT_getIcInfoAndGenARQC)GetProcAddress(m_hLibModule,"_CT_getIcInfoAndGenARQC@44");
	if( m_CT_pgetIcInfo == NULL ||
		m_CT_pgenARQC == NULL ||
		m_CT_pGetTranDetail == NULL ||
		m_CT_pCtlScriptData == NULL ||
		m_CT_pPowerOff == NULL ||
		m_CT_pgetIcInfoAndGenARQC == NULL)
	{
		return ERR_LOADDRV;
	}
	m_bDriverLoaded = true;
	
	return SUCCESS;
}

int CDriverICCard::LoadDriver2()
{
	if (m_hLibModule2)
	{
		return SUCCESS;
	}
	if(_etcdll.IsEmpty())
	{
	    return ERR_NODRV;
	}
	CString ThirDllPath = _moduledir + _etcdll;

	TCHAR buf[255];
	memset(buf, 0, 255);
	GetCurrentDirectory(255, buf);
	SetCurrentDirectory(_moduledir);
    m_hLibModule2 = LoadLibrary(ThirDllPath);
	SetCurrentDirectory(buf);
	if( NULL == m_hLibModule2)
	{
		return ERR_LOADDRV;
	}
	
	m_drive = (FUNC_drive)GetProcAddress(m_hLibModule2, "drive");

	if( m_drive == NULL)
	{
		return ERR_LOADDRV;
	}
	m_bDriverLoaded2 = true;
	
	return SUCCESS;
}

void CDriverICCard::RealseDriver()
{
	if(m_hLibModule)
		FreeLibrary(m_hLibModule);

	if(m_hLibModule2)
		FreeLibrary(m_hLibModule2);

	m_hLibModule = NULL;
	m_bDriverLoaded = false;
	m_hLibModule2 = NULL;
	m_bDriverLoaded2 = false;

	m_pgetIcInfo = NULL;
	m_pgetCardNo = NULL;
	m_pGetTranDetail = NULL;
	m_pgenARQC = NULL;
	m_pCtlScriptData = NULL;

	m_CT_pgetIcInfo = NULL;
	m_CT_pGetTranDetail = NULL;
	m_CT_pgenARQC = NULL;
	m_CT_pCtlScriptData = NULL;
	m_CT_pPowerOff = NULL;
	m_CT_pgetIcInfoAndGenARQC = NULL;

	m_drive = NULL;
}

void CDriverICCard::SetTimeOut(short timeout)
{
	_ntimeout = timeout;
}


int CDriverICCard::ReadICAcount(VPair &OutList)
{
    return SUCCESS;
}

//为了提高读卡速度，加一个简化版的读卡信息
int CDriverICCard::getIcNo(VPair &OutList)
{
	if(m_pgetIcInfo == NULL)
	{
		return ERR_READCARDINFO;
	}
	
	CString tmpnode;
	CString SubTmpNode;
	char Tag;
	CString OutTag;
	int TagLen = 0;
	CString SubNode;
	PAIR PairNode;
	char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	char PortNO[8] = {0};  //端口号
	char Taglist[256] = "ABI";
	char AIDList[256] = {0};
	char UserInfo[1024] = {0};
	char ICType;
	char icappdata[8192] = {0};
	char ErrMsg[200] = {0};

	if(_porttype == 2)
	{
		PortType = 'U';
		PortNO[0] = '1';  //USB口时端口号为100
		PortNO[1] = '0';
		PortNO[2] = '0';
	}
	else
	{
		PortType = 'C';
		PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	
	if(!m_pgetIcInfo(TermType, &PortType, PortNO, &_ICFlag, Taglist, AIDList, UserInfo, &ICType, icappdata, ErrMsg))
	{
		
		memcpy( m_icappdata, icappdata, sizeof icappdata);
		tmpnode = UserInfo;
		for (; !tmpnode.IsEmpty();)
		{
			if (tmpnode.GetLength() < 4)
			{
				return ERR_READCARDINFO;
			}
			Tag = tmpnode.GetAt(0);
			if (Tag == 'A')
			{
				OutTag = "ACCTNO";
			} 
			else if(Tag == 'B')
			{
				OutTag = "TRACK2";
			}
			else if(Tag == 'I')
			{
				OutTag = "ICKXULH";
			}
			else
			{
				OutTag = "OTHERS";
			}
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_READCARDINFO;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			//保存卡序列号,到生成ARQC时再传出
			if('I' == Tag)
			{
				m_ICKXULH = SubNode;	
			}
			SubTmpNode.Format("%s=%s", OutTag, SubNode);
			PairNode.first = OutTag;
			PairNode.second = SubTmpNode;
			OutList.push_back(PairNode);
			tmpnode.Delete(0, 1+3+TagLen);
		}
		return SUCCESS;
	}
	return ERR_READCARDINFO;
}
int CDriverICCard::getIcInfo(VPair &OutList)
{
	/*
	if(m_pgetIcInfo == NULL)
	{
		return ERR_READCARDINFO;
	}
	*/

	if(m_CT_pgetIcInfo == NULL)
	{
		return ERR_READCARDINFO;
	}
	
	CString ICPersonData = "9F61009F62005F20009F0B00";
	CString tmpnode;
	CString SubTmpNode;
	char Tag;
	CString OutTag;
	int TagLen = 0;
	int TagLen2 = 0;
	CString SubNode;
	PAIR PairNode;
	//char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};  //扩展口
	char Taglist[256] = "ABCDEFGHIJKLMN";
	char AIDList[256] = {0};
	char UserInfo[1024] = {0};
	//char ICType;
	char icappdata[8192] = {0};
	char ErrMsg[256] = {0};

	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	int nWaitTime = 0;
	if (_ntimeout != 0)
	{
		nWaitTime = _ntimeout;
	} 
	else
	{
		nWaitTime = _timeOut;
	}
	int ret = m_CT_pgetIcInfo(PortType, PortNO, _BpNO, _ICFlag, Taglist, nWaitTime, UserInfo, ErrMsg);
	if(0 == ret)
	{
		tmpnode = UserInfo;
		
		for (; !tmpnode.IsEmpty();)
		{
			if (tmpnode.GetLength() < 4)
			{
				return ERR_READCARDINFO;
			}
			Tag = tmpnode.GetAt(0);
			if (Tag == 'A')
			{
				OutTag = "ACCTNO";
			} 
			else if(Tag == 'B')
			{
				OutTag = "TRACK2";
			}
			else if(Tag == 'C')
			{
				OutTag = "TRACK1";
			}
			else if(Tag == 'D')
			{
				OutTag = "ICYHBDH";
			}
			else if(Tag == 'E')
			{
				OutTag = "ICXJYUE";
			}
			else if(Tag == 'F')
			{
				OutTag = "ICYUESX";
			}
			else if(Tag == 'G')
			{
				OutTag = "ICYSXRQ";
			}
			else if(Tag == 'H')
			{
				OutTag = "ICDJESX";
			}
			else if(Tag == 'I')
			{
				OutTag = "ICKXULH";
			}
			else if(Tag == 'J')
			{
				OutTag = "ICATC";
			}
			else if(Tag == 'K')
			{
				OutTag = "IdNo";
			}
			else if(Tag == 'L')
			{
				OutTag = "IdType";
			}
			else if(Tag == 'M')
			{
				OutTag = "CustomName";
			}
			else if(Tag == 'N')
			{
				OutTag = "CustomNameEx";
			}
			else
			{
				OutTag = "OTHERS";
			}
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			TagLen2 = TagLen/2;
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_READCARDINFO;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			//金额域要将单位由分转换为元
			if('E' == Tag || 'F' == Tag || 'H' == Tag)
			{
				int tmpTagLen = SubNode.GetLength();
				if (tmpTagLen > 2)
				{
					SubNode.Insert(tmpTagLen-2, '.');
				}
				else if (tmpTagLen == 2)
				{
					SubNode.Insert(0, "0.");
				}
				else if (tmpTagLen == 1)
				{
					SubNode.Insert(0, "0.0");
				} 
				
			}
			//日期域要将6位转换为8位
			if('G' == Tag)
			{
				
				SubNode.Insert(0, "20");
			}
			//保存卡序列号,到生成ARQC时再传出
			if('I' == Tag)
			{
				m_ICKXULH = SubNode;	
			}
			//组装个人信息数据
			if('K' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F61", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('L' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F62", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('M' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("5F20", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('N' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F0B", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}

			//个人信息部分不需要单独输出各个字段的解析结果
			if ('K' == Tag || 'L' == Tag || 'M' == Tag || 'N' == Tag)
			{
				//tmpnode.Delete(0, 1+3+TagLen);
				//continue;
				char pBuff[256] = {0};
				Hex2Raw((BYTE *)pBuff, SubNode.GetBuffer(SubNode.GetLength()), SubNode.GetLength());
				SubNode = pBuff;
			}
			SubTmpNode.Format("%s=%s", OutTag, SubNode);
			PairNode.first = OutTag;
			PairNode.second = SubTmpNode;
			OutList.push_back(PairNode);
			tmpnode.Delete(0, 1+3+TagLen);
		}
		OutTag = "ICPersonData";
		SubNode = ICPersonData;
		SubTmpNode.Format("%s=%s", OutTag, SubNode);
		PairNode.first = OutTag;
		PairNode.second = SubTmpNode;
		OutList.push_back(PairNode);
		return SUCCESS;
	}
#ifdef TIME_DUG
	tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  error code of getIcInfo is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, tmpnode);
		fflush(m_stream);
	}
#endif
	return ERR_READCARDINFO;
}

int CDriverICCard::getIcInfoAndGenARQC(char *pTxData, VPair &OutList)
{
	if(m_CT_pgetIcInfoAndGenARQC == NULL)
	{
		return ERR_GETICINFOANDGENARQC;
	}
	
	CString ICPersonData = "9F61009F62005F20009F0B00";
	CString tmpnode;
	CString SubTmpNode;
	char Tag;
	CString OutTag;
	int TagLen = 0;
	int TagLen2 = 0;
	CString SubNode;
	PAIR PairNode;
	//char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};  //扩展口
	char Taglist[256] = "ABCDEFGHIJKLMN";
	char AIDList[256] = {0};
	char UserInfo[1024] = {0};
	//char ICType;
	char icappdata[8192] = {0};
	char ErrMsg[256] = {0};

	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	int nWaitTime = 0;
	if (_ntimeout != 0)
	{
		nWaitTime = _ntimeout;
	} 
	else
	{
		nWaitTime = _timeOut;
	}

	char TxData[256] = {0};
	//char AIDList[256] = {0};
	//char icappdata[8192] = {0};
	//char ARQCLen[3] = {0};
	int ARQCLen = 0;
	char ARQC[512] = {0};
	char Field60[256] = {0};
	//char ICType;
	char tag95[260] = {0};
	char tag9F10[100] = {0};
	
	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	
	CString CTxData = pTxData;
	//CTxData += "Q0040000";
	CTxData += "R003156";
	CTxData += "V003CNT";
	
	if (CTxData.GetLength() >= 256)
	{
		return ERR_GETICINFOANDGENARQC;
	}
	memcpy(TxData, CTxData.GetBuffer(CTxData.GetLength()), CTxData.GetLength());
	memset(m_TxData,0, sizeof(m_TxData));
	memcpy(m_TxData, TxData, strlen(TxData));
	
	int ret = m_CT_pgetIcInfoAndGenARQC(PortType, PortNO, _BpNO, _ICFlag, Taglist, nWaitTime, TxData, UserInfo, &ARQCLen, ARQC, ErrMsg);
	if(0 == ret)
	{
		tmpnode = UserInfo;
		
		for (; !tmpnode.IsEmpty();)
		{
			if (tmpnode.GetLength() < 4)
			{
				return ERR_GETICINFOANDGENARQC;
			}
			Tag = tmpnode.GetAt(0);
			if (Tag == 'A')
			{
				OutTag = "ACCTNO";
			} 
			else if(Tag == 'B')
			{
				OutTag = "TRACK2";
			}
			else if(Tag == 'C')
			{
				OutTag = "TRACK1";
			}
			else if(Tag == 'D')
			{
				OutTag = "ICYHBDH";
			}
			else if(Tag == 'E')
			{
				OutTag = "ICXJYUE";
			}
			else if(Tag == 'F')
			{
				OutTag = "ICYUESX";
			}
			else if(Tag == 'G')
			{
				OutTag = "ICYSXRQ";
			}
			else if(Tag == 'H')
			{
				OutTag = "ICDJESX";
			}
			else if(Tag == 'I')
			{
				OutTag = "ICKXULH";
			}
			else if(Tag == 'J')
			{
				OutTag = "ICATC";
			}
			else if(Tag == 'K')
			{
				OutTag = "IdNo";
			}
			else if(Tag == 'L')
			{
				OutTag = "IdType";
			}
			else if(Tag == 'M')
			{
				OutTag = "CustomName";
			}
			else if(Tag == 'N')
			{
				OutTag = "CustomNameEx";
			}
			else
			{
				OutTag = "OTHERS";
			}
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			TagLen2 = TagLen/2;
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETICINFOANDGENARQC;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			//金额域要将单位由分转换为元
			if('E' == Tag || 'F' == Tag || 'H' == Tag)
			{
				int tmpTagLen = SubNode.GetLength();
				if (tmpTagLen > 2)
				{
					SubNode.Insert(tmpTagLen-2, '.');
				}
				else if (tmpTagLen == 2)
				{
					SubNode.Insert(0, "0.");
				}
				else if (tmpTagLen == 1)
				{
					SubNode.Insert(0, "0.0");
				} 
				
			}
			//日期域要将6位转换为8位
			if('G' == Tag)
			{
				
				SubNode.Insert(0, "20");
			}
			//保存卡序列号,到生成ARQC时再传出
			if('I' == Tag)
			{
				m_ICKXULH = SubNode;	
			}
			//组装个人信息数据
			if('K' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F61", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('L' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F62", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('M' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("5F20", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}
			if('N' == Tag)
			{
				int tmpTagPos = ICPersonData.Find("9F0B", 0);
				if (tmpTagPos != -1)
				{
					if (TagLen2 < 16)
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+5, 1);
						ICPersonData.Insert(tmpTagPos+5, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					} 
					else
					{
						CString tmpStrTagLen;
						//tmpStrTagLen.Format("%d", TagLen);
						tmpStrTagLen.Format("%X", TagLen2);
						ICPersonData.Delete(tmpTagPos+4, 2);
						ICPersonData.Insert(tmpTagPos+4, tmpStrTagLen.GetBuffer(tmpStrTagLen.GetLength()));
						ICPersonData.Insert(tmpTagPos+6,SubNode.GetBuffer(SubNode.GetLength()));
					}
					
				}
			}

			//个人信息部分不需要单独输出各个字段的解析结果
			if ('K' == Tag || 'L' == Tag || 'M' == Tag || 'N' == Tag)
			{
				//tmpnode.Delete(0, 1+3+TagLen);
				//continue;
				char pBuff[256] = {0};
				Hex2Raw((BYTE *)pBuff, SubNode.GetBuffer(SubNode.GetLength()), SubNode.GetLength());
				SubNode = pBuff;
			}
			SubTmpNode.Format("%s=%s", OutTag, SubNode);
			PairNode.first = OutTag;
			PairNode.second = SubTmpNode;
			OutList.push_back(PairNode);
			tmpnode.Delete(0, 1+3+TagLen);
		}
		OutTag = "ICPersonData";
		SubNode = ICPersonData;
		SubTmpNode.Format("%s=%s", OutTag, SubNode);
		PairNode.first = OutTag;
		PairNode.second = SubTmpNode;
		OutList.push_back(PairNode);
		//return SUCCESS;

		//以下整合生成F55域部分
		char newARQC[512] = {0};
		if(Get_NoTagARQC(ARQC, newARQC, tag9F10, tag95))
		{
			return ERR_GETICINFOANDGENARQC;
		}
		
		
		tmpnode.Empty();
		tmpnode = newARQC;
		char Tag;
		CString OutTag;
		int TagLen;
		CString SubNode;
		CString SubTmpNode;
		PAIR PairNode;
		for (; !tmpnode.IsEmpty();)
		{
			if (tmpnode.GetLength() < 4)
			{
				return ERR_GETICINFOANDGENARQC;
			}
			Tag = tmpnode.GetAt(0);
			if (Tag == 'J')
			{
				OutTag = "ICATC";
			} 
			else if(Tag == 'Q')
			{
				OutTag = "ICARQCMI";
			}
			else if(Tag == 'R')
			{
				OutTag = "ICARQC";
			}
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETICINFOANDGENARQC;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			SubTmpNode.Format("%s=%s", OutTag, SubNode);
			PairNode.first = OutTag;
			PairNode.second = SubTmpNode;
			OutList.push_back(PairNode);
			tmpnode.Delete(0, 1+3+TagLen);
		}
		
		//输出卡序列号,此值来自读卡
		//tmpnode.Format("ICKXULH=%s",m_ICKXULH);
		//PAIR PairICKXULH("ICKXULH",tmpnode);
		//OutList.push_back(PairICKXULH);
		
		//发卡行应用数据,ARQC源数据中只取了9F10标签的第4到7个字节的内容
		CString tmpARQC;
		tmpARQC = ARQC;
		CString tmpStr9F10;
		int tmp9F10POS = tmpARQC.Find("9F10", 0);
		if (tmp9F10POS != -1)
		{
			if (tmpARQC.GetLength() > tmp9F10POS+5)
			{
				int Tag9F10Len = 2*(ConvertHexStr2Deci(tmpARQC.Mid(tmp9F10POS+4, 2).GetBuffer(2)));
				if (tmpARQC.GetLength() > tmp9F10POS+5+Tag9F10Len)
				{
					tmpStr9F10 = tmpARQC.Mid(tmp9F10POS+6, Tag9F10Len);
				}
			}
			
		}
		tmpnode.Format("ICCardAppData=%s",tmpStr9F10);
		PAIR Pairtag9F10("ICCardAppData",tmpnode);
		OutList.push_back(Pairtag9F10);
		
		//终端验证结果
		tmpnode.Format("TtypRs=%s",tag95);
		PAIR Pairtag95("TtypRs",tmpnode);
		OutList.push_back(Pairtag95);
		
		return SUCCESS;
	}
#ifdef TIME_DUG
	tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  error code of getIcInfoAndGenARQC is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, tmpnode);
		fflush(m_stream);
	}
#endif
	return ERR_GETICINFOANDGENARQC;
}
int CDriverICCard::PowerOff(VPair &OutList)
{
	if (m_CT_pPowerOff == NULL)
	{
		return ERR_POWEROFF;
	}
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};  //扩展口
	
	char ErrMsg[256] = {0};
	
	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	int ret = m_CT_pPowerOff(PortType, PortNO, _BpNO, _ICFlag, ErrMsg);
	if (0 == ret)
	{
		return SUCCESS;
	}
	return ERR_POWEROFF;

}
int CDriverICCard::genARQC(char *pTxData, VPair &OutList)
{
	/*
	if (m_pgenARQC == NULL)
	{
		return ERR_GENARQC;
	}
	*/
	if (m_CT_pgenARQC == NULL)
	{
		return ERR_GENARQC;
	}

	CString tmpnode;
	//char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};
	char TxData[256] = {0};
	char AIDList[256] = {0};
	//char icappdata[8192] = {0};
	//char ARQCLen[3] = {0};
	int ARQCLen = 0;
	char ARQC[512] = {0};
	char Field60[256] = {0};
	//char ICType;
	char ErrMsg[200] = {0};
	char tag95[260] = {0};
	char tag9F10[100] = {0};
	
	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	
	CString CTxData = pTxData;
	//CTxData += "Q0040000";
	CTxData += "R003156";
	CTxData += "V003CNT";

	if (CTxData.GetLength() >= 256)
	{
		return ERR_GENARQC;
	}
	memcpy(TxData, CTxData.GetBuffer(CTxData.GetLength()), CTxData.GetLength());
	memcpy(m_TxData, TxData, strlen(TxData));
	
#ifdef TIME_DUG
	//tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  TxData of genARQC is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, TxData);
		fflush(m_stream);
	}
#endif
	//int ret = m_pgenARQC(TermType, &PortType, PortNO, &_ICFlag, TxData, AIDList, m_icappdata, ARQCLen, ARQC, Field60, &ICType, ErrMsg);
	int ret = m_CT_pgenARQC(PortType, PortNO, _BpNO, _ICFlag, TxData, &ARQCLen, ARQC, ErrMsg);
	if (0 == ret)
	{

		char newARQC[512] = {0};
		if(Get_NoTagARQC(ARQC, newARQC, tag9F10, tag95))
		{
			return ERR_GENARQC;
		}
		

		tmpnode.Empty();
		tmpnode = newARQC;
		char Tag;
		CString OutTag;
		int TagLen;
		CString SubNode;
		CString SubTmpNode;
		PAIR PairNode;
		for (; !tmpnode.IsEmpty();)
		{
			if (tmpnode.GetLength() < 4)
			{
				return ERR_GENARQC;
			}
			Tag = tmpnode.GetAt(0);
			if (Tag == 'J')
			{
				OutTag = "ICATC";
			} 
			else if(Tag == 'Q')
			{
				OutTag = "ICARQCMI";
			}
			else if(Tag == 'R')
			{
				OutTag = "ICARQC";
			}
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GENARQC;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			SubTmpNode.Format("%s=%s", OutTag, SubNode);
			PairNode.first = OutTag;
			PairNode.second = SubTmpNode;
			OutList.push_back(PairNode);
			tmpnode.Delete(0, 1+3+TagLen);
		}
		
		//输出卡序列号,此值来自读卡
		tmpnode.Format("ICKXULH=%s",m_ICKXULH);
		PAIR PairICKXULH("ICKXULH",tmpnode);
		OutList.push_back(PairICKXULH);

		//发卡行应用数据,ARQC源数据中只取了9F10标签的第4到7个字节的内容
		CString tmpARQC;
		tmpARQC = ARQC;
		CString tmpStr9F10;
		int tmp9F10POS = tmpARQC.Find("9F10", 0);
		if (tmp9F10POS != -1)
		{
			if (tmpARQC.GetLength() > tmp9F10POS+5)
			{
				int Tag9F10Len = 2*(ConvertHexStr2Deci(tmpARQC.Mid(tmp9F10POS+4, 2).GetBuffer(2)));
				if (tmpARQC.GetLength() > tmp9F10POS+5+Tag9F10Len)
				{
					tmpStr9F10 = tmpARQC.Mid(tmp9F10POS+6, Tag9F10Len);
				}
			}
			
		}
		tmpnode.Format("ICCardAppData=%s",tmpStr9F10);
		PAIR Pairtag9F10("ICCardAppData",tmpnode);
		OutList.push_back(Pairtag9F10);

		//终端验证结果
		tmpnode.Format("TtypRs=%s",tag95);
		PAIR Pairtag95("TtypRs",tmpnode);
		OutList.push_back(Pairtag95);

		return SUCCESS;
	}
#ifdef TIME_DUG
	tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  error code of genARQC is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, tmpnode);
		fflush(m_stream);
	}
#endif
	return ERR_GENARQC;
}
int CDriverICCard::GetTranDetail(VPair &OutList)
{
	/*
	if(m_pGetTranDetail == NULL)
	{
		return ERR_GETTRANDETAIL;
	}
	*/

	if(m_CT_pGetTranDetail == NULL)
	{
		return ERR_GETTRANDETAIL;
	}
	
	CString tmpnode;
	CString SubTmpNode;
	char Tag;
	int TagLen = 0;
	int pos = 0;
	CString OutTag;
	CString SubNode;
	CString Node;
	PAIR PairNode;
	//char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};
	char AIDList[256] = {0};
	char TxDetail[4098] = {0};
	//char ICType;
	char ErrMsg[200] = {0};
	
	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}
	
	//int ret = m_pGetTranDetail(PortType, PortNO, BpNo, _ICFlag, TxDetail, ErrMsg);
	int ret = m_CT_pGetTranDetail(PortType, PortNO, _BpNO, _ICFlag, TxDetail, ErrMsg);
	if(0 == ret)
	{
		//AfxMessageBox(TxDetail);
		tmpnode = TxDetail;
		//获取交易笔数
		if (tmpnode.GetLength() < 2)
		{
			return ERR_GETTRANDETAIL;
		}
		SubNode = tmpnode.Mid(0, 2);
		OutTag = "TranSum";
		SubTmpNode.Format("%s=%s", OutTag, SubNode);
		PairNode.first = OutTag;
		PairNode.second = SubTmpNode;
		//OutList.push_back(PairNode);
		tmpnode.Delete(0, 2);

		for (; !tmpnode.IsEmpty();)
		{
			pos = tmpnode.Find('P');
			if (-1 == pos)
			{
				break;
			}
			tmpnode.Delete(0, pos);
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//其他金额
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//交易货币代码
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//交易日期
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//交易类型
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//交易时间
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//终端国家代码
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//商户名称
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			char cReMove = 0x20;
			SubNode.Remove(cReMove); //去除空格
			Node += SubNode;
			Node += '|';   //添加分隔符
			tmpnode.Delete(0, 1+3+TagLen);

			//应用交易计数器(ATC)
			Tag = tmpnode.GetAt(0);
			TagLen = atoi(tmpnode.Mid(1, 3).GetBuffer(3));
			if (tmpnode.GetLength() < 4+TagLen)
			{
				return ERR_GETTRANDETAIL;
			}
			SubNode = tmpnode.Mid(4, TagLen);
			ConvertTagValue(Tag, SubNode);
			Node += SubNode;
			
			//Node += '\n'; 
			//Node += '|'; 
			Node += "\r\n";
			tmpnode.Delete(0, 1+3+TagLen);

			//SubTmpNode.Format("%s=%s", "Trance", Node);
			

		}
		
		
		PairNode.first = Tag;
		PairNode.second = Node;
		OutList.push_back(PairNode);

		return SUCCESS;
	}
#ifdef TIME_DUG
	tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  error code of GetTranDetail is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, tmpnode);
		fflush(m_stream);
	}
#endif	
	return ERR_GETTRANDETAIL;

}
void CDriverICCard::ConvertTagValue(char Tag, CString &TagValue)
{
	//金额域要将单位由分转换为元
	if('P' == Tag || 'Q' == Tag)
	{
		int tmpMoney = atoi(TagValue);
		TagValue.Format("%d", tmpMoney);
		int tmpTagLen = TagValue.GetLength();
		if (tmpTagLen > 2)
		{
			TagValue.Insert(tmpTagLen-2, '.');
		}
		else if (tmpTagLen == 2)
		{
			TagValue.Insert(0, "0.");
		}
		else if (tmpTagLen == 1)
		{
			TagValue.Insert(0, "0.0");
		} 
		
	}
	//日期域要将6位转换为8位
	if('S' == Tag)
	{	
		TagValue.Insert(0, "20");
	}
	//币种域固定输出"人民币"
	if('R' == Tag)
	{	
		TagValue = "人民币";
	}
	//时间域
	if('U' == Tag)
	{	
		TagValue.Insert(2, ':');
		TagValue.Insert(5, ':');
	}
	//计数器改为十进制显示
	if('X' == Tag)
	{	
		int tmpi = 0;
		int *pStr = &tmpi;
		unsigned char tmpc[4] = {0};
		Hex2Raw(tmpc, TagValue, 4);
		*(pStr) = tmpc[1];
		*(pStr+1) = tmpc[0];
		TagValue.Format("%d", tmpi);
	}
}
int CDriverICCard::CtlScriptData(char *pARPC, VPair &OutList)
{
	/*
	if (m_pCtlScriptData == NULL)
	{
		return ERR_CTLSCRIPTDATA;
	}
	*/

	if (m_CT_pCtlScriptData == NULL)
	{
		return ERR_CTLSCRIPTDATA;
	}
	
	CString tmpnode;
	//char TermType[2];
	char PortType;  //端口类型U--USB、C--COM
	//char PortNO[8] = {0};  //端口号
	char PortNO = {0};  //端口号
	char BpNo = {0};
	//char TxData[1024] = {0};
	//char icappdata[8192] = {0};
	char ARPCLen[3] = {0};
	char ARPC[512] = {0};
	char Status[8] = {0};
	char TC[512] = {0};
	char ScriptResult[32] = {0};
	char ErrMsg[256] = {0};
	
	if(_porttype == 2)
	{
		PortType = 'U';
		//PortNO[0] = '1';  //USB口时端口号为100
		//PortNO[1] = '0';
		//PortNO[2] = '0';
		PortNO = 0x30;
	}
	else
	{
		PortType = 'C';
		//PortNO[0] = 0x30 + _nPort;  //将short类型1映射成字符'1'
		PortNO = 0x30 + _nPort;  //将short类型1映射成字符'1'
	}

	int iARPClen = strlen(pARPC);
	CString tmpLen;
	tmpLen.Format("%d",iARPClen);

	memcpy(ARPC, pARPC, iARPClen);
	
	//int ret = m_pCtlScriptData(TermType, &PortType, PortNO, &_ICFlag, m_TxData, m_icappdata, tmpLen.GetBuffer(tmpLen.GetLength()), ARPC, Status, TC, ScriptResult, ErrMsg);
	int ret = m_CT_pCtlScriptData(PortType, PortNO, _BpNO, _ICFlag, m_TxData, iARPClen, ARPC, TC, ScriptResult, ErrMsg);
	if (0 == ret)
	{

		//返回密文数据TC
		tmpnode.Format("ICCardMWSJ=%s",TC);
		PAIR PairTC("ICCardMWSJ",tmpnode);
		OutList.push_back(PairTC);

		//脚本执行结果
		tmpnode.Format("ICCardScriptRs=%s",ScriptResult);
		PAIR PairScriptRs("ICCardScriptRs",tmpnode);
		OutList.push_back(PairScriptRs);
		return SUCCESS;
	}
#ifdef TIME_DUG	
	tmpnode.Format("%d", ret);
	m_stream = fopen("icpams.txt", "a+");
	if (m_stream != NULL)
	{
		long ntime ;
		time(&ntime);
		struct tm *tim = localtime(&ntime);
		fprintf(m_stream, "%04d%02d%02d--%02d:%02d:%02d  error code of CtlScriptData is [%s]\n", tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday, tim->tm_hour,tim->tm_min,tim->tm_sec, tmpnode);
		fflush(m_stream);
	}
#endif
	return ERR_CTLSCRIPTDATA;
}

int CDriverICCard::invokeDrive(int optype, char *func_argv, VPair &OutList)
{
	if(m_drive == NULL)
	{
		return ERR_NOSUCHMETHOD;
	}

	CString tmpnode;

	// 终端类型
	int term_type = 0;
	//终端名称
	char tty_name[20] = {0};

	// 函数ID
	// 1-读ETC卡基本信息，2-ETC卡充值初始化，3-ETC卡充值，4-ETC持卡客户信息写入第一步
	// 5-ETC持卡客户信息写入第二步，6-ETC卡车辆信息写入第一步，7-ETC卡车辆信息写入第一步
	int func_id = 0;
	func_id = optype % 10;
	if(func_id == 0)
	{
		return ERR_OTHERSERROR;
	}

	// 函数参数个数，函数ID为1时，参数个数为0，其他情况默认为1
	int func_argc = 1;
	if(func_id == 1)
	{
		func_argc = 0;
	}

	int nWaitTime = 0;
	if (_ntimeout != 0)
	{
		nWaitTime = _ntimeout;
	} 
	else
	{
		nWaitTime = _timeOut;
	}

	char *result_arr[1];
	if(func_argc == 1)
	{
		result_arr[0] = func_argv;
	}


	char result_buf[1024+1] = {0};
	int buf_size = 1024;
	int ret = m_drive(term_type, tty_name, _nPort, nWaitTime, func_id, func_argc, result_arr, result_buf, buf_size);

	if(0 == ret)
	{
		//成功返回对应函数的数据
		tmpnode.Format("RESULT_BUF=%s", result_buf);
		PAIR Result("result", tmpnode);
		OutList.push_back(Result);
		return SUCCESS;
	}
	else
	{
		tmpnode.Format("RESULT_BUF=%s", result_buf);
		PAIR Result("result", tmpnode);
		OutList.push_back(Result);
	}

	return ret;
}