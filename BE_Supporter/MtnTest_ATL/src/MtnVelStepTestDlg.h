#pragma once


// CMtnVelStepTestDlg dialog
#include "resource.h"
#include "MtnApi.h"
#include "AftVelLoopTest.h"

class CMtnVelStepTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CMtnVelStepTestDlg)

public:
	CMtnVelStepTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnVelStepTestDlg();

// Dialog Data
	enum { IDD = IDD_DLG_GROUP_VEL_STEP_TEST };

	void InitVelStepGroupTestCfg();
	void InitVelStepGroupTestConfigUI();
	void InitVelStepGroupTestConfigEditOneAxis(int nResId, int iValue);
	void InitVelStepGroupTestConfigCombo_OneAxis(int nResId, int iAxisCtrlCard);
	void ReadIntegerFromEdit(int nResId, int *iValue);
	void ReadUnsignIntegerFromEdit(int nResId, unsigned int *uiValue);
	void VelStepGroupTestUpdateDoubleToEdit(int nResId, double dValue, const char * strFormat);
	void ReadDoubleFromEdit(int nResId, double *dValue);
	void SetUserInterfaceLanguage(int iLanguageOption);
	void UI_EnableEditTestCondition(int bEnableFlag);
	void PauseTimer();
	void RestartTimer();
	void DlgTimerEvent();
	int DlgGetFlagTimerRunning();
	virtual BOOL ShowWindow(int nCmdShow);

	virtual BOOL OnInitDialog();
	UINT_PTR  StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration);

	char cFlagDlgVelStepGroputTestThreadRunning;
	CWinThread* pDlgVelStepGroupTestWinThread; // , THREAD_PRIORITY_TIME_CRITICAL);
	void StartVelStepGroupTestThread();
	UINT VelStepGroupTestThread();
	UINT VelStepTestTuningThread();  // 20120801
	void DlgVelStepTuningPerAxis(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTuningInput);  // 20120802

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	int m_iLanguageOption;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditNumPointsGroupVelStepAxis1();
	afx_msg void OnEnChangeEditNumPointsGroupVelStepAxis2();
	afx_msg void OnEnChangeEditNumPointsGroupVelStepAxis3();
	afx_msg void OnEnChangeEditNumPointsGroupVelStepAxis4();
	afx_msg void OnEnChangeEditLowLimitGroupVelStepAxis1();
	afx_msg void OnEnChangeEditLowLimitGroupVelStepAxis2();
	afx_msg void OnEnChangeEditLowLimitGroupVelStepAxis3();
	afx_msg void OnEnChangeEditLowLimitGroupVelStepAxis4();
	afx_msg void OnEnChangeEditUppLimitGroupVelStepAxis1();
	afx_msg void OnEnChangeEditUppLimitGroupVelStepAxis2();
	afx_msg void OnEnChangeEditUppLimitGroupVelStepAxis3();
	afx_msg void OnEnChangeEditUppLimitGroupVelStepAxis4();
	afx_msg void OnEnChangeEditVelStepDistGroupVelStepAxis1();
	afx_msg void OnEnChangeEditVelStepDistGroupVelStepAxis2();
	afx_msg void OnEnChangeEditVelStepDistGroupVelStepAxis3();
	afx_msg void OnEnChangeEditVelStepDistGroupVelStepAxis4();
	afx_msg void OnEnChangeEditVelStepMaxVelGroupAxis1();
	afx_msg void OnEnChangeEditVelStepMaxVelGroupAxis2();
	afx_msg void OnEnChangeEditVelStepMaxVelGroupAxis3();
	afx_msg void OnEnChangeEditVelStepMaxVelGroupAxis4();
	afx_msg void OnCbnSelchangeComboAxis1GroupVelStep();
	afx_msg void OnCbnSelchangeComboAxis2GroupVelStep();
	afx_msg void OnCbnSelchangeComboAxis3GroupVelStep();
	afx_msg void OnCbnSelchangeComboAxis4GroupVelStep();
	afx_msg void OnBnClickedCheckExciteFlagVelStepAxis1();
	afx_msg void OnBnClickedCheckExciteFlagVelStepAxis2();
	afx_msg void OnBnClickedCheckExciteFlagVelStepAxis3();
	afx_msg void OnBnClickedCheckExciteFlagVelStepAxis4();
	afx_msg void OnBnClickedButtonStartGroupVelStep();
	afx_msg void OnEnChangeEditVelStepGroupLoopNumAxis1();
	afx_msg void OnEnChangeEditVelStepGroupLoopNumAxis2();
	afx_msg void OnEnChangeEditVelStepGroupLoopNumAxis3();
	afx_msg void OnEnChangeEditVelStepGroupLoopNumAxis4();
	afx_msg void OnEnChangeEditVelStepGroupTolPcSettleTimeAxis4();
	afx_msg void OnEnChangeEditVelStepGroupTolPcSettleTimeAxis3();
	afx_msg void OnEnChangeEditVelStepGroupTolPcSettleTimeAxis2();
	afx_msg void OnEnChangeEditVelStepGroupTolPcSettleTimeAxis1();
	afx_msg void OnEnChangeEditVelStepGroupEncPerMmAxis1();
	afx_msg void OnEnChangeEditVelStepGroupEncPerMmAxis2();
	afx_msg void OnEnChangeEditVelStepGroupEncPerMmAxis3();
	afx_msg void OnEnChangeEditVelStepGroupEncPerMmAxis4();
	afx_msg void OnBnClickedButtonStopVelStepTest();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT nTimerVal);

	afx_msg void OnBnClickedCheckFlagSaveChartVelStepGroupTest();
	afx_msg void OnBnClickedCheckFlagPlotChartVelStepGroupTest();
	afx_msg void OnBnClickedCheckVelStepDlgEnableEdit();
	afx_msg void OnBnClickedCheckTuningVelLoopByStepResponse();
	afx_msg void OnEnKillfocusEditVelStepMaxVelGroupAxis3();
	afx_msg void OnEnKillfocusEditVelStepMaxVelGroupAxis4();
	afx_msg void OnEnKillfocusEditVelStepMaxVelGroupAxis1();
	afx_msg void OnEnKillfocusEditVelStepMaxVelGroupAxis2();
	afx_msg void OnEnKillfocusEditVelStepDistGroupVelStepAxis1();
	afx_msg void OnEnKillfocusEditVelStepDistGroupVelStepAxis2();
	afx_msg void OnEnKillfocusEditVelStepDistGroupVelStepAxis3();
	afx_msg void OnEnKillfocusEditVelStepDistGroupVelStepAxis4();
	afx_msg void OnEnKillfocusEditUppLimitGroupVelStepAxis1();
	afx_msg void OnEnKillfocusEditUppLimitGroupVelStepAxis2();
	afx_msg void OnEnKillfocusEditUppLimitGroupVelStepAxis3();
	afx_msg void OnEnKillfocusEditUppLimitGroupVelStepAxis4();
	afx_msg void OnEnKillfocusEditLowLimitGroupVelStepAxis1();
	afx_msg void OnEnKillfocusEditLowLimitGroupVelStepAxis2();
	afx_msg void OnEnKillfocusEditLowLimitGroupVelStepAxis3();
	afx_msg void OnEnKillfocusEditLowLimitGroupVelStepAxis4();
	afx_msg void OnEnKillfocusEditNumPointsGroupVelStepAxis1();
	afx_msg void OnEnKillfocusEditNumPointsGroupVelStepAxis2();
	afx_msg void OnEnKillfocusEditNumPointsGroupVelStepAxis3();
	afx_msg void OnEnKillfocusEditNumPointsGroupVelStepAxis4();
	afx_msg void OnBnClickedCheckVelStepDlgTeachLimit();
};

extern CMtnVelStepTestDlg *cpVelStepGroupTest;
