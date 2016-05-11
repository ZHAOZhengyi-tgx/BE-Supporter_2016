// MtnScope.cpp : implementation file
//

#include "stdafx.h"
#include "MtnScopeDlg.h"


IMPLEMENT_DYNAMIC(CMtnScope, CDialog)

CMtnScope::CMtnScope(MTN_SCOPE *pstSystemScope, HANDLE hInHandle, CWnd* pParent /*=NULL*/)
	: CDialog(CMtnScope::IDD, pParent)
	, m_strSamplePeriod_ms(_T(""))
	, m_strScopeDataLen(_T(""))
	, m_strScopeNumVar(_T(""))
	, m_strListVarName(_T(""))
{
	m_strSamplePeriod_ms.Format("%8.2f", pstSystemScope->dSamplePeriod_ms);
	m_strScopeDataLen.Format("%d", pstSystemScope->uiDataLen);
	m_strScopeNumVar.Format("%d", pstSystemScope->uiNumData);

	Handle = hInHandle;

}

CMtnScope::~CMtnScope()
{
}

void CMtnScope::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_SCOPE_SAMPLE_PERIOD_EDIT, m_strSamplePeriod_ms);
	DDX_Text(pDX, IDC_SCOPE_DATA_LENGTH_EDIT, m_strScopeDataLen);
	DDX_Text(pDX, IDC_SCOPE_NUM_DATA_EDIT, m_strScopeNumVar);
	DDX_Control(pDX, IDC_VARIABLE_NAME_LIST_SCOPE, m_cListBoxVarName);
	DDX_LBString(pDX, IDC_VARIABLE_NAME_LIST_SCOPE, m_strListVarName);
}


BEGIN_MESSAGE_MAP(CMtnScope, CDialog)
	ON_EN_CHANGE(IDC_SCOPE_SAMPLE_PERIOD_EDIT, &CMtnScope::OnEnChangeScopeSamplePeriodEdit)
	ON_EN_CHANGE(IDC_SCOPE_DATA_LENGTH_EDIT, &CMtnScope::OnEnChangeScopeDataLengthEdit)
	ON_EN_CHANGE(IDC_SCOPE_NUM_DATA_EDIT, &CMtnScope::OnEnChangeScopeNumDataEdit)
	ON_LBN_SELCHANGE(IDC_VARIABLE_NAME_LIST_SCOPE, &CMtnScope::OnLbnSelchangeVariableNameListScope)
	ON_BN_CLICKED(IDOK, &CMtnScope::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CMtnScope::OnInitDialog()
{
//	CString strTemp;

	m_strListVarName.Format("FPOS(0)");
	m_cListBoxVarName.AddString(m_strListVarName);
	m_strListVarName.Format("FVEL(0)");
	m_cListBoxVarName.AddString(m_strListVarName);
	m_strListVarName.Format("RPOS(0)");
	m_cListBoxVarName.AddString(m_strListVarName);
	m_strListVarName.Format("RVEL(0)");
	m_cListBoxVarName.AddString(m_strListVarName);
	m_strListVarName.Format("FACC(0)");
	m_cListBoxVarName.AddString(m_strListVarName);

	m_cListBoxVarName.SetDlgItemTextA(0, m_strListVarName);
	m_cListBoxVarName.SetDlgItemTextA(1, m_strListVarName);
	m_cListBoxVarName.SetDlgItemTextA(2, m_strListVarName);

	//m_cListBoxVarName.InsertString(0, "FPOS(0)");
	//m_cListBoxVarName.InsertString(1, "FVEL(0)");
	//m_cListBoxVarName.InsertString(2, "RPOS(0)");
	//m_cListBoxVarName.InsertString(3, "RVEL(0)");
	//m_cListBoxVarName.InsertString(4, "FACC(0)");

//	m_cListBoxVarName.
	UpdateData(FALSE);
//	GetDlgItem(IDC_VARIABLE_NAME_LIST_SCOPE)

	return CDialog::OnInitDialog();
//	return TRUE;
}

// CMtnScope message handlers

// IDC_SCOPE_SAMPLE_PERIOD_EDIT
void CMtnScope::OnEnChangeScopeSamplePeriodEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_SCOPE_SAMPLE_PERIOD_EDIT)->GetWindowTextA( &cTempEdit[0], 32);
	sscanf_s(cTempEdit, "%f", &gstSystemScope.dSamplePeriod_ms);

}
// IDC_SCOPE_DATA_LENGTH_EDIT
void CMtnScope::OnEnChangeScopeDataLengthEdit()
{
	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_SCOPE_DATA_LENGTH_EDIT)->GetWindowTextA( &cTempEdit[0], 32);
	sscanf_s(cTempEdit, "%d", &gstSystemScope.uiDataLen);
}
// IDC_SCOPE_NUM_DATA_EDIT
void CMtnScope::OnEnChangeScopeNumDataEdit()
{
	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_SCOPE_NUM_DATA_EDIT)->GetWindowTextA( &cTempEdit[0], 32);
	sscanf_s(cTempEdit, "%d", &gstSystemScope.uiNumData);
}

// IDC_VARIABLE_NAME_LIST_SCOPE
void CMtnScope::OnLbnSelchangeVariableNameListScope()
{
	// TODO: Add your control notification handler code here

}

void CMtnScope::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
//	unsigned int ii;
	acsc_ClearVariables(Handle, NULL);
	mtnscope_declare_var_on_controller(Handle);

// 	OnOK();
}
