// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__306C1EA3_04CD_4EF1_85C4_B2CAE0E2BEE8__INCLUDED_)
#define AFX_STDAFX_H__306C1EA3_04CD_4EF1_85C4_B2CAE0E2BEE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "g_.h"

#include "xml/tinystr.h"
#include "xml/tinyxml.h"

#include <vector>
#include <utility>
using namespace std;

#include "../Share/DriverInterSt.h"
#include "../Share/DataDefHelp.h"
#include "../Share/MagCardCom.h"
#include "libCheckTrack.h"

#include "DataDef.h"

#define _STRINGIFY(X) #X
#define STRINGIFY(X) _STRINGIFY(X)
#define FL __FILE__" Line:"STRINGIFY(__LINE__)

void debug(CString s);
void debug(const char* s,...);


#define _LOG


//#define DLLMAG_DEBUG
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__306C1EA3_04CD_4EF1_85C4_B2CAE0E2BEE8__INCLUDED_)
