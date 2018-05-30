// dllICCard.h : main header file for the DLLICCARD DLL
//

#if !defined(AFX_DLLICCARD_H__27ED3473_1796_4C25_B923_B2007E600353__INCLUDED_)
#define AFX_DLLICCARD_H__27ED3473_1796_4C25_B923_B2007E600353__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllICCardApp
// See dllICCard.cpp for the implementation of this class
//

class CDllICCardApp : public CWinApp
{
public:
	CDllICCardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllICCardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllICCardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLICCARD_H__27ED3473_1796_4C25_B923_B2007E600353__INCLUDED_)
