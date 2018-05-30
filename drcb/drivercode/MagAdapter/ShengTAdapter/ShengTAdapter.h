// ShengTAdapter.h : main header file for the SHENGTADAPTER DLL
//

#if !defined(AFX_SHENGTADAPTER_H__3AFA4857_32E4_47FF_8CDA_7351AB331EBF__INCLUDED_)
#define AFX_SHENGTADAPTER_H__3AFA4857_32E4_47FF_8CDA_7351AB331EBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CShengTAdapterApp
// See ShengTAdapter.cpp for the implementation of this class
//

class CShengTAdapterApp : public CWinApp
{
public:
	CShengTAdapterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShengTAdapterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CShengTAdapterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHENGTADAPTER_H__3AFA4857_32E4_47FF_8CDA_7351AB331EBF__INCLUDED_)
