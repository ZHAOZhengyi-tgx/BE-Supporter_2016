// ACSCTerm.h : main header file for the ACSCTERM application
//

#if !defined(AFX_ACSCTERM_H__2E5ED0D7_39CA_4880_A631_6F43D05292CC__INCLUDED_)
#define AFX_ACSCTERM_H__2E5ED0D7_39CA_4880_A631_6F43D05292CC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MtnApi.h"         // 

/////////////////////////////////////////////////////////////////////////////
// CACSCTermApp:
// See ACSCTerm.cpp for the implementation of this class
//

class CACSCTermApp : public CWinApp
{
public:
	CACSCTermApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACSCTermApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CACSCTermApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnBnClickedButton3();
//	afx_msg void OnBnClickedButtonParaDownload();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACSCTERM_H__2E5ED0D7_39CA_4880_A631_6F43D05292CC__INCLUDED_)
