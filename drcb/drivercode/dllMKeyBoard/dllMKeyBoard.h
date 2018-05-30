// dllMKeyBoard.h : main header file for the DLLMKEYBOARD DLL
//

#if !defined(AFX_DLLMKEYBOARD_H__4A8F0500_9772_466E_81E7_AD0481AE25BA__INCLUDED_)
#define AFX_DLLMKEYBOARD_H__4A8F0500_9772_466E_81E7_AD0481AE25BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllMKeyBoardApp
// See dllMKeyBoard.cpp for the implementation of this class
//

class CDllMKeyBoardApp : public CWinApp
{
public:
	CDllMKeyBoardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllMKeyBoardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllMKeyBoardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLMKEYBOARD_H__4A8F0500_9772_466E_81E7_AD0481AE25BA__INCLUDED_)
