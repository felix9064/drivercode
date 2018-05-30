// dllIdentityCard.h : main header file for the DLLIDENTITYCARD DLL
//

#if !defined(AFX_DLLIDENTITYCARD_H__71AD5775_6BC9_4776_B6A4_72D0EEECE68D__INCLUDED_)
#define AFX_DLLIDENTITYCARD_H__71AD5775_6BC9_4776_B6A4_72D0EEECE68D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDllIdentityCardApp
// See dllIdentityCard.cpp for the implementation of this class
//

class CDllIdentityCardApp : public CWinApp
{
public:
	CDllIdentityCardApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllIdentityCardApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDllIdentityCardApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLIDENTITYCARD_H__71AD5775_6BC9_4776_B6A4_72D0EEECE68D__INCLUDED_)
