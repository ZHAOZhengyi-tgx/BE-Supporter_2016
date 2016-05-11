//////////////////////////////////////////////////////////////////////////////////////////////////
//  
// Efsika-Tech80 Ltd.
// Copyright ?2000. All Rights Reserved.
//
// FILE		:	 ACSCTerm.cpp
//
// OVERVIEW
// ========
//
// The sample of the communication terminal demonstrates the usage of the ACSC Library functions.  
//
// The sample shows how to open communication with the simulator or with the controller
// (via serial, ethernet or PCI bus) and how to send commands.
// 
// The sample opens the communication history buffer to show all sent to the 
// controller commands and all controller's reaponses.
//
// The communication history buffer is one of the feature of the ACSC Library.
// The library supports storing all messages sent to and received from the controller 
// in a memory buffer. The application can retrieve the full or partial contents of the buffer 
// and also can clear the history buffer.
// 
//////////////////////////////////////////////////////////////////////////////////////////////////
// Port from ACS CPP by VC60, to DotNet
// History
// YYYYMMDD Author  Notes
//                  stdafx.cpp, stdafx.h, targetver.h must be generated from Dotnet
// 20080820 Zhengyi For DotNet Compatibility
//                  
// 
#include "stdafx.h"
#include "ACSCTerm.h"
#include "ACSCTermDlg.h"
#include "MtnTesterEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CACSCTermApp

BEGIN_MESSAGE_MAP(CACSCTermApp, CWinApp)
	//{{AFX_MSG_MAP(CACSCTermApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
//	ON_BN_CLICKED(IDC_BUTTON3, &CACSCTermApp::OnBnClickedButton3)
// ON_BN_CLICKED(IDC_BUTTON_PARA_DOWNLOAD, &CACSCTermApp::OnBnClickedButtonParaDownload)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACSCTermApp construction

CACSCTermApp::CACSCTermApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CACSCTermApp object

CACSCTermApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CACSCTermApp initialization

#include "LimitSingleInstance.H"
CLimitSingleInstance gSingleInstanceObj(TEXT("Global\\{BE Supportor}"));

BOOL CACSCTermApp::InitInstance()
{
	// comment CPP by VC6, 20080820
//#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
//#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
//#endif
//
    if (gSingleInstanceObj.IsAnotherInstanceRunning())
    {
       return FALSE;
    }

    // 20080820
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
    // 20080820

	//CACSCTermDlg dlg;
	//m_pMainWnd = &dlg;
	//INT_PTR nResponse = dlg.DoModal();

MtnTesterEntry cMainDialog;
	m_pMainWnd = &cMainDialog;
	INT_PTR nResponse = cMainDialog.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else //if (nResponse == IDCANCEL) //20080820
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


