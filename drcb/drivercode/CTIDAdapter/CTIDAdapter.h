// CTIDAdapter.h : main header file for the CTIDADAPTER DLL
//

#if !defined(AFX_CTIDADAPTER_H__BAFA37B0_6C89_45FF_BC16_B6FDAFCE093D__INCLUDED_)
#define AFX_CTIDADAPTER_H__BAFA37B0_6C89_45FF_BC16_B6FDAFCE093D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCTIDAdapterApp
// See CTIDAdapter.cpp for the implementation of this class
//

class CCTIDAdapterApp : public CWinApp
{
public:
	CCTIDAdapterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCTIDAdapterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCTIDAdapterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CTIDADAPTER_H__BAFA37B0_6C89_45FF_BC16_B6FDAFCE093D__INCLUDED_)
