#pragma once


// CDlgKeyInputPad dialog
#include "resource.h"

extern int wb_mtn_tester_get_password_sg_8d_zzy();
extern int wb_mtn_tester_get_password_brightlux_6d_gxc();
extern int wb_mtn_tester_get_password_brightlux_6d_zzy();

class CDlgKeyInputPad : public CDialog
{
	DECLARE_DYNAMIC(CDlgKeyInputPad)

public:
	CDlgKeyInputPad(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgKeyInputPad();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG_KEY_INPUT };

//
	BOOL bFlagShowNumberKeyPad;
	int iInputNumber;
	int iReturnNumber;

public:
	void SetInputNumber(int iNumber);
	int GetReturnNumber();
	void SetFlagShowNumberOnKeyPad(BOOL bShow);
	void ReadIntegerFromEdit(int nResId, int *iValue);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDlgKey1();
	afx_msg void OnBnClickedButtonDlgKey2();
	afx_msg void OnBnClickedButtonDlgKey3();
	afx_msg void OnBnClickedButtonDlgKey4();
	afx_msg void OnBnClickedButtonDlgKey5();
	afx_msg void OnBnClickedButtonDlgKey6();
	afx_msg void OnBnClickedButtonDlgKey7();
	afx_msg void OnBnClickedButtonDlgKey8();
	afx_msg void OnBnClickedButtonDlgKey9();
	afx_msg void OnBnClickedButtonDlgKey0();
	afx_msg void OnBnClickedCheckDlgShowNumber();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonDlgKeyPadClear();
	afx_msg void OnEnChangeEditDlgKeyInputShowText();
};
