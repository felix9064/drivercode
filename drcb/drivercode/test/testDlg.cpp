// testDlg.cpp : implementation file
//

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestDlg::OnOK() 
{
	// TODO: Add extra validation here
	String temp="MIIEKjCCA8+gAwIBAgIUMQAApSpSo2ozowFRGVKHAXMAEAEwCgYIKoEcz1UBg3UwFzEVMBMGA1UECgwM5rWL6K+V57O757ufMB4XDTE3MTEyOTIyNTA0OVoXDTIyMDEyOTIyNTA0OVowLDEqMCgGA1UECgwh5pm65oWn6YeR6ZKl5YyZ56eR5oqA5pyJ6ZmQ5YWs5Y+4MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAEwmdCLyVWluqPu6prcZFs4qa4JAiz2onM7nKwl2nVWmJQdZQ9ADiTACdHh7Dfz1OzK5t0ycj+hjucD6R07rVTBqOCAuIwggLeMBsGBQYDiBANBBJaWkpHRDE1MTE5NTI4NzAxNzMwJAYFBgOIEA4EG1paSkdETTMxMDAwMDAwMTUxMTk1Mjg3MDE3MzASBgUGA4gQDwQJNTYwMDAwOTgxMCIGBQYDiBAQBBlaVFNGMzEwMDAwMDAxNTExOTUyODcwMTczMBgGBQYDiBARBA80NDE1MTE5NTI4NzAxNzMwKgYFBgOIEBIEIeaZuuaFp+mHkemSpeWMmeenkeaKgOaciemZkOWFrOWPuDAVBgUGA4gQEwQM6JCl5Lia5omn54WnMA8GBQYDiBAUBAblvKDmn5AwEgYFBgOIEBkECei0sOWNg+S4hzAeBgUGA4gQHQQV5ZOI5bCU5ruo5biC6Iqx5Zut6LevMBkGBQYDiBAhBBAyMDEx5bm0OeaciDE35pelMBkGBQYDiBAjBBAyMDEx5bm0OeaciDE35pelMBkGBQYDiBAmBBAyMDE45bm0OeaciDE35pelMFEGBQYDiBAnBEjnlLXohJHorr7lpIfjgIHoh6rliqjljJborr7lpIfjgIHov5vlj6Plip7lhazoh6rliqjljJborr7lpIfjgIHmmbrog73mnLowHgYFBgOIECoEFem7kem+meaxn+ecgeW3peWVhuWxgDAZBgUGA4gQKwQQMjAxMeW5tDnmnIgxN+aXpTAzBgUGA4gQLAQqaHR0cDovL2d1b2ppYWdvbmdzL3B1YmxpYy9nc2dzL3NlcnZpY2UuZ292MCcGBQYDiBAtBB7msZ/oi4/nnIHlt6XllYbooYzmlL/nrqHnkIblsYAwDQYFBgOIEC4EBFYxLjAwJwYFBgOIEC8EHjMxMDAwMFpaSkdEMTUxMTk1Mjg3MDE3MzAwMTAwMTAKBgUGA4gQWgQBQTAKBgUGA4gQWwQBMTAPBgUGA4gQXQQGMzEwMDAwMCEGBQYDiBA0BBhBenFKaFlGL1RlL25QVGFQVlRCVlJBPT0wCgYIKoEcz1UBg3UDSQAwRgIhAJhS7cT0CP+qjurOzlLqytqMZHpnEwrF4/0sl/rzRuOtAiEAxz1YPVzXSYdfxNnCKeM8B7VAhL6Kwa9ooa/+GKQKW2Q=";
	CDialog::OnOK();
}