// PuTIDAdapter.h : main header file for the PUTIDADAPTER DLL
//

#if !defined(AFX_PUTIDADAPTER_H__54BC07BD_6AC9_4110_A676_744976EAA881__INCLUDED_)
#define AFX_PUTIDADAPTER_H__54BC07BD_6AC9_4110_A676_744976EAA881__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPuTIDAdapterApp
// See PuTIDAdapter.cpp for the implementation of this class
//

class CPuTIDAdapterApp : public CWinApp
{
public:
	CPuTIDAdapterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPuTIDAdapterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPuTIDAdapterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUTIDADAPTER_H__54BC07BD_6AC9_4110_A676_744976EAA881__INCLUDED_)
