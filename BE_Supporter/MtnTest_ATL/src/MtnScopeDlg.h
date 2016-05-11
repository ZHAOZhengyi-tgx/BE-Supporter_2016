#pragma once


#include "MtnApi.h"
#include "afxwin.h"
#include "resource.h"

// CMtnScope dialog

class CMtnScope : public CDialog
{
	DECLARE_DYNAMIC(CMtnScope)

public:
	CMtnScope(MTN_SCOPE *pstSystemScope, HANDLE hInHandle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnScope();
	HANDLE Handle;				// communication handle

// Dialog Data
	enum { IDD = IDD_SCOPE_SETUP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	char cTempEdit[32];
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSamplePeriod_ms;
	CString m_strScopeDataLen;
	CString m_strScopeNumVar;
	afx_msg void OnEnChangeScopeSamplePeriodEdit();
	afx_msg void OnEnChangeScopeDataLengthEdit();
	afx_msg void OnEnChangeScopeNumDataEdit();
	afx_msg void OnLbnSelchangeVariableNameListScope();
	CListBox m_cListBoxVarName;
	CString m_strListVarName;
	afx_msg void OnBnClickedOk();
};
