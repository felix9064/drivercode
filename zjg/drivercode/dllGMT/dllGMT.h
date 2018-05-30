// dllGMT.h : main header file for the DLLGMT DLL
//

#if !defined(AFX_DLLGMT_H__E815C4A8_6B71_4E4A_8227_0BA35B073ECE__INCLUDED_)
#define AFX_DLLGMT_H__E815C4A8_6B71_4E4A_8227_0BA35B073ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllGMTApp
// See dllGMT.cpp for the implementation of this class
//

class CDllGMTApp : public CWinApp
{
public:
	CDllGMTApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllGMTApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllGMTApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLGMT_H__E815C4A8_6B71_4E4A_8227_0BA35B073ECE__INCLUDED_)
