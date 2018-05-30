// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BA56E6B6_508F_4961_89AE_9584DAC73E96__INCLUDED_)
#define AFX_STDAFX_H__BA56E6B6_508F_4961_89AE_9584DAC73E96__INCLUDED_

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


#include "xml/tinystr.h"
#include "xml/tinyxml.h"

#include "../Share/DriverInterSt.h"
#include "../Share/DataDefHelp.h"
#include "../Share/EncryptKBCom.h"

#include "DataDef.h"

#ifndef DEBUG_H_LOG
#define DEBUG_H_LOG
#endif

#define _STRINGIFY(X) #X
#define STRINGIFY(X) _STRINGIFY(X)
#define FL __FILE__" Line:"STRINGIFY(__LINE__)

void debug(CString s);
void debug(const char* s,...);
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BA56E6B6_508F_4961_89AE_9584DAC73E96__INCLUDED_)
