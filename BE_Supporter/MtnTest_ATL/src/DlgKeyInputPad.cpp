// DlgKeyInputPad.cpp : implementation file
//

#include "stdafx.h"
#include "DlgKeyInputPad.h"


static CString czInputString;

// CDlgKeyInputPad dialog

IMPLEMENT_DYNAMIC(CDlgKeyInputPad, CDialog)

CDlgKeyInputPad::CDlgKeyInputPad(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgKeyInputPad::IDD, pParent)
{

}

CDlgKeyInputPad::~CDlgKeyInputPad()
{
}

void CDlgKeyInputPad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgKeyInputPad, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_1, &CDlgKeyInputPad::OnBnClickedButtonDlgKey1)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_2, &CDlgKeyInputPad::OnBnClickedButtonDlgKey2)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_3, &CDlgKeyInputPad::OnBnClickedButtonDlgKey3)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_4, &CDlgKeyInputPad::OnBnClickedButtonDlgKey4)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_5, &CDlgKeyInputPad::OnBnClickedButtonDlgKey5)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_6, &CDlgKeyInputPad::OnBnClickedButtonDlgKey6)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_7, &CDlgKeyInputPad::OnBnClickedButtonDlgKey7)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_8, &CDlgKeyInputPad::OnBnClickedButtonDlgKey8)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_9, &CDlgKeyInputPad::OnBnClickedButtonDlgKey9)
	ON_BN_CLICKED(IDC_BUTTON_DLG_KEY_0, &CDlgKeyInputPad::OnBnClickedButtonDlgKey0)
	ON_BN_CLICKED(IDC_CHECK_DLG_SHOW_NUMBER, &CDlgKeyInputPad::OnBnClickedCheckDlgShowNumber)
	ON_BN_CLICKED(IDOK, &CDlgKeyInputPad::OnBnClickedOk)
//	ON_BN_CLICKED(IDCANCEL, &CDlgKeyInputPad::OnBnClickedCancel)
ON_BN_CLICKED(ID_BUTTON_DLG_KEY_PAD_CLEAR, &CDlgKeyInputPad::OnBnClickedButtonDlgKeyPadClear)
ON_EN_CHANGE(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT, &CDlgKeyInputPad::OnEnChangeEditDlgKeyInputShowText)
END_MESSAGE_MAP()

// CDlgKeyInputPad message handlers
BOOL CDlgKeyInputPad::OnInitDialog()
{
	((CButton *)GetDlgItem(IDC_CHECK_DLG_SHOW_NUMBER))->SetCheck(bFlagShowNumberKeyPad);
CString cstrTemp;
	cstrTemp.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(_T(cstrTemp));
	return CDialog::OnInitDialog();
}

void CDlgKeyInputPad::SetInputNumber(int iNumber)
{
	iInputNumber = iNumber;
}

int CDlgKeyInputPad::GetReturnNumber()
{
	return iReturnNumber;
}

void CDlgKeyInputPad::SetFlagShowNumberOnKeyPad(BOOL bShow)
{
	bFlagShowNumberKeyPad = bShow;
}

// IDC_BUTTON_DLG_KEY_1
void CDlgKeyInputPad::OnBnClickedButtonDlgKey1()
{
	iInputNumber = iInputNumber * 10 + 1;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
//	if(bFlagShowNumberKeyPad)
//	{
//		(CEdit*)GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->shw
//	}
//	else
//	{
//	}
}
// IDC_BUTTON_DLG_KEY_2
void CDlgKeyInputPad::OnBnClickedButtonDlgKey2()
{
	iInputNumber = iInputNumber * 10 + 2;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_3
void CDlgKeyInputPad::OnBnClickedButtonDlgKey3()
{
	iInputNumber = iInputNumber * 10 + 3;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}

// IDC_BUTTON_DLG_KEY_4
void CDlgKeyInputPad::OnBnClickedButtonDlgKey4()
{
	iInputNumber = iInputNumber * 10 + 4;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_5
void CDlgKeyInputPad::OnBnClickedButtonDlgKey5()
{
	iInputNumber = iInputNumber * 10 + 5;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_6
void CDlgKeyInputPad::OnBnClickedButtonDlgKey6()
{
	iInputNumber = iInputNumber * 10 + 6;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_7
void CDlgKeyInputPad::OnBnClickedButtonDlgKey7()
{
	iInputNumber = iInputNumber * 10 + 7;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_8
void CDlgKeyInputPad::OnBnClickedButtonDlgKey8()
{
	iInputNumber = iInputNumber * 10 + 8;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_9
void CDlgKeyInputPad::OnBnClickedButtonDlgKey9()
{
	iInputNumber = iInputNumber * 10 + 9;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_BUTTON_DLG_KEY_0
void CDlgKeyInputPad::OnBnClickedButtonDlgKey0()
{;
	iInputNumber = iInputNumber * 10 + 0;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}
// IDC_CHECK_DLG_SHOW_NUMBER
void CDlgKeyInputPad::OnBnClickedCheckDlgShowNumber()
{
	// TODO: Add your control notification handler code here
	bFlagShowNumberKeyPad = ((CButton*)GetDlgItem(IDC_CHECK_DLG_SHOW_NUMBER))->GetCheck();
}

void CDlgKeyInputPad::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	iReturnNumber = iInputNumber;
	OnOK();
}

// ID_BUTTON_DLG_KEY_PAD_CLEAR
void CDlgKeyInputPad::OnBnClickedButtonDlgKeyPadClear()
{
	iInputNumber = 0;
	czInputString.Format("%d", iInputNumber);
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->SetWindowTextA(czInputString);
}

// IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT
void CDlgKeyInputPad::OnEnChangeEditDlgKeyInputShowText()
{
	static char tempChar[32];
	GetDlgItem(IDC_EDIT_DLG_KEY_INPUT_SHOW_TEXT)->GetWindowTextA(tempChar, 32);

	if(tempChar[0] <= '9' &&  tempChar[0] >= '0')
	{
		czInputString.Format("%s", tempChar);
		sscanf_s(tempChar, "%d", &iInputNumber);
	}
	else
	{
		AfxMessageBox("Pass must be positive integer", 0);
		OnBnClickedButtonDlgKeyPadClear();
	}
}

void CDlgKeyInputPad::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}
