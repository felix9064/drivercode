// dllFinger.h : main header file for the DLLFINGER DLL
//

#if !defined(AFX_DLLFINGER_H__88FF8263_ED6A_466C_B5B2_5EB11115B2AA__INCLUDED_)
#define AFX_DLLFINGER_H__88FF8263_ED6A_466C_B5B2_5EB11115B2AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllFingerApp
// See dllFinger.cpp for the implementation of this class
//

class CDllFingerApp : public CWinApp
{
public:
	CDllFingerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllFingerApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllFingerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLFINGER_H__88FF8263_ED6A_466C_B5B2_5EB11115B2AA__INCLUDED_)
