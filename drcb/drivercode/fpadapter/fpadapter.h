// fpadapter.h : main header file for the FPADAPTER DLL
//

#if !defined(AFX_FPADAPTER_H__AB65D6FD_0170_477A_B011_5937DDBB8FA7__INCLUDED_)
#define AFX_FPADAPTER_H__AB65D6FD_0170_477A_B011_5937DDBB8FA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFpadapterApp
// See fpadapter.cpp for the implementation of this class
//

class CFpadapterApp : public CWinApp
{
public:
	CFpadapterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFpadapterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFpadapterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FPADAPTER_H__AB65D6FD_0170_477A_B011_5937DDBB8FA7__INCLUDED_)
