// dllMag.h : main header file for the DLLMAG DLL
//

#if !defined(AFX_DLLMAG_H__279A516E_E218_4E1F_8863_17819B35C2B7__INCLUDED_)
#define AFX_DLLMAG_H__279A516E_E218_4E1F_8863_17819B35C2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllMagApp
// See dllMag.cpp for the implementation of this class
//

class CDllMagApp : public CWinApp
{
public:
	CDllMagApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllMagApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllMagApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLMAG_H__279A516E_E218_4E1F_8863_17819B35C2B7__INCLUDED_)
