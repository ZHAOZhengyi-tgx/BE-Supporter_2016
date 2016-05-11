#pragma once


// MtnTesterEntry dialog

#include "mtnapi.h"
extern COMM_SETTINGS stServoControllerCommSet;
void sys_config_machine_type(int iTempMachCfg);

#include "resource.h"
class MtnTesterEntry : public CDialog
{
	DECLARE_DYNAMIC(MtnTesterEntry)

public:
	MtnTesterEntry(CWnd* pParent = NULL);   // standard constructor
	virtual ~MtnTesterEntry();

	void EnableProtectButtons();  // 20110518
	void OnCommunicationConnected();
	void CommunicationFailure();

	void DeleteAllDialogResource();
	void ReleaseDlgPointer_ACS();

	void InitButtonUI();
	void ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue);
	void UI_InitSubDialogs();

// Dialog Data
	enum { IDD = IDC_SYS_COMM_SET };
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); // 
	virtual void OnClose();
	virtual void OnCancel();

	void UpdateComboBoxMachineType();

	BOOL StopTimer();
	UINT StartTimer(UINT TimerDuration);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSysCommSet();
	afx_msg void OnBnClickedSysTerminal();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSysFbMonitor();
	afx_msg void OnBnClickedButtonMovingTestACS();
	afx_msg void OnBnClickedSysScopeSetup();
	afx_msg void OnBnClickedSysPosnTune1();
	afx_msg void OnBnClickedTuningServoPara();
	afx_msg void OnBnClickedSysTimer();
	afx_msg void OnBnClickedSysStepper();
	afx_msg void OnBnClickedSysProfileCalc();
	afx_msg void OnBnClickedSysSpectrumTest();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClicked_Toggle_ChangeLanguage();
	afx_msg void OnBnClickedSysRoyaDspTest();
	afx_msg void OnBnClickedSysMonitor();
	afx_msg void OnBnClickedServoAdjustment();
	afx_msg void OnBnClickedCheckSysEntryOffServo();
	afx_msg void OnBnClickedCheckSysCleanOnExit();
	afx_msg void OnCbnSelchangeComboSysMachineType();
//	afx_msg void OnBnClickedSysStepperCtrlPci8144();
	afx_msg void OnBnClickedCheckEnableEditingMach();
	afx_msg void OnEnKillfocusEditSysMachSn();
};
