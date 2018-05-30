// SDIDAdapter.h : main header file for the SDIDADAPTER DLL
//

#if !defined(AFX_SDIDADAPTER_H__EE9B69FD_3D7B_4409_B701_7EF712477676__INCLUDED_)
#define AFX_SDIDADAPTER_H__EE9B69FD_3D7B_4409_B701_7EF712477676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSDIDAdapterApp
// See SDIDAdapter.cpp for the implementation of this class
//

class CSDIDAdapterApp : public CWinApp
{
public:
	CSDIDAdapterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDIDAdapterApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CSDIDAdapterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDIDADAPTER_H__EE9B69FD_3D7B_4409_B701_7EF712477676__INCLUDED_)
