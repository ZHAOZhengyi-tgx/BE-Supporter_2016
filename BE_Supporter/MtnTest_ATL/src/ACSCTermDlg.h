// ACSCTermDlg.h : header file
//

#if !defined(AFX_ACSCTERMDLG_H__5DB2106E_E6AC_433D_B36E_E2E1CA1E51A1__INCLUDED_)
#define AFX_ACSCTERMDLG_H__5DB2106E_E6AC_433D_B36E_E2E1CA1E51A1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ACSCTerm.h"

/////////////////////////////////////////////////////////////////////////////
// CACSCTermDlg dialog

class CACSCTermDlg : public CDialog
{
// Construction
public:
	CACSCTermDlg(COMM_SETTINGS CommSet, CWnd* pParent = NULL);	// standard constructor

	virtual void ShowWindow(int nCmdShow);
// UI with BuffProg
	void InitUI_ForBuffProg();
	void UI_Enable_Group_ACS_BuffProg(BOOL bEnableFlag);
// Dialog Data
	//{{AFX_DATA(CACSCTermDlg)
	enum { IDD = IDD_ACSCTERM_DIALOG };
	CEdit m_editTerminal;
	CComboBox	m_comboSend;
//	CString	m_ConnectionString;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CACSCTermDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	COMM_SETTINGS CommSet;
	HICON m_hIcon;
	BOOL m_fStopTermThread;
//	BOOL m_fConnect;
	CWinThread* m_pWinThread;
	char m_HistBuf[HISTBUF_SIZE];
	char m_EditBuf[EDITBUF_SIZE];
	int m_nEditPos;

	// Generated message map functions
	//{{AFX_MSG(CACSCTermDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();
//	afx_msg void OnConnect();
	virtual void OnCancel();
	afx_msg void OnSend();
	afx_msg void OnClose();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	UINT TerminalThread();

protected:
	void RunTermThread();
	void StopTermThread();
	void FormatBuffer();
public:
	afx_msg void OnBnClickedCheckAcsProgBuffId0();
	afx_msg void OnBnClickedCheckAcsProgBuffId1();
	afx_msg void OnBnClickedCheckAcsProgBuffId2();
	afx_msg void OnBnClickedCheckAcsProgBuffId3();
	afx_msg void OnBnClickedCheckAcsProgBuffId4();
	afx_msg void OnBnClickedCheckAcsProgBuffId5();
	afx_msg void OnBnClickedCheckAcsProgBuffId6();
	afx_msg void OnBnClickedCheckAcsProgBuffId7();
	afx_msg void OnBnClickedCheckAcsProgBuffId8();
	afx_msg void OnBnClickedCheckAcsProgBuffId9();
	afx_msg void OnBnClickedCheckAcsProgBuffId10();
	afx_msg void OnBnClickedButtonSaveAcsBufferProgTermDlg();
	afx_msg void OnBnClickedCheckFlagViewAcsBufferProgTermDlg();
	afx_msg void OnBnClickedCheckFlagDebugAcsBufferProgTermDlg();
	afx_msg void OnBnClickedButtonAcsTerminalSaveParameter();
	afx_msg void OnBnClickedButtonAcsTerminalSaveProgram();
	afx_msg void OnBnClickedButtonAcsTerminalDlgEnablePsLostCheck();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACSCTERMDLG_H__5DB2106E_E6AC_433D_B36E_E2E1CA1E51A1__INCLUDED_)
