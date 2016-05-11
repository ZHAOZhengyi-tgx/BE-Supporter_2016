#pragma once

#include "resource.h"
// CDlgStepper_PCI8144 dialog

class CDlgStepper_PCI8144 : public CDialog
{
	DECLARE_DYNAMIC(CDlgStepper_PCI8144)

public:
	CDlgStepper_PCI8144(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgStepper_PCI8144();
	virtual BOOL OnInitDialog();
	UINT_PTR CDlgStepper_PCI8144::StartTimer(UINT TimerDuration);
	BOOL CDlgStepper_PCI8144::StopTimer();
	void CDlgStepper_PCI8144::UI_TimerEventUpdateMotionIO_Status();


// Dialog Data
	enum { IDD = IDD_DIALOG_STEPPER_8144 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void CDlgStepper_PCI8144::UI_InitCombo();
	void CDlgStepper_PCI8144::UI_UpdateAxisMovingCondition();
	void CDlgStepper_PCI8144::ReadDoubleFromEdit(int nResId, double *pdValue);
	void CDlgStepper_PCI8144::UI_UpdateAxisIdByCardId();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnCbnSelchangeComboBoardIdStp8144Dlg();
	afx_msg void OnCbnSelchangeComboAxisIdStp8144Dlg();
	afx_msg void OnCbnSelchangeComboProfileTypeTvSvStp8144Dlg();
	afx_msg void OnCbnSelchangeComboMotionModeStp8144Dlg();
	afx_msg void OnBnClickedEditStp8144DlgAxisMoveStart();
	afx_msg void OnEnKillfocusEditStp8144DlgAxisMoveDist();
	afx_msg void OnEnKillfocusEditStp8144DlgAxisMoveStartVel();
	afx_msg void OnEnKillfocusEditStp8144DlgAxisMoveMaxVel();
	afx_msg void OnEnKillfocusEditStp8144DlgAxisMoveTAcc();
};
