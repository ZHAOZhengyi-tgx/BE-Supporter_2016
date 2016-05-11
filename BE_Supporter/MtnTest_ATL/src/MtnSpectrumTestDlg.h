#pragma once


// CMtnSpectrumTestDlg dialog
#include "resource.h"

class CMtnSpectrumTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CMtnSpectrumTestDlg)

public:
	CMtnSpectrumTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnSpectrumTestDlg();
	virtual BOOL OnInitDialog();
	virtual void ShowWindow(int nCmdShow);

	void InitPrbsGroupTestConfigUI();
	void InitPrbsGroupTestConfigCombo_OneAxis(int nResId, int iAxis);
	void InitPrbsGroupTestConfigVar();
	void InitPrbsGroupTestConfigEditOneAxis(int nResId, int iValue);
	void ReadIntegerFromEdit(int nResId, int *iValue);
	void ReadShortFromEdit(int nResId, short *psValue);
	void CMtnSpectrumTestDlg::ReadDoubleFromEdit(int nResId, double *pdValue);
	void CMtnSpectrumTestDlg::ReadUnsigndShortFromEdit(int nResId, unsigned short *pusValue);

	void UpdateShortToEdit(int nResId, short sValue);
	void UpdateDoubleToEdit(int nResId, double dValue);

	void UI_EnableEditDebug(int iEnableFlag);
	void UI_EnableEditTuningMaxParameter(BOOL bEnableFlag = FALSE);  // 20121018

	unsigned int spectrum_dlg_calculate_total_num_cases();
	virtual void EndDialog(int nResult);
	virtual void OnOK();
	virtual void OnCancel();

	int UI_GetShowWindowFlag();
	void CMtnSpectrumTestDlg::Update_DlgUI();

	void SetUserInterfaceLanguage(int iLanguageOption);

	UINT_PTR  StartTimer (UINT nIDT_TimerResMacro, UINT TimerDuration);
	BOOL StopTimer(UINT_PTR uiPtrTimer);

	// Thread related
	UINT SpectrumTestThread();
	UINT DoGroupSpectrumTest();  // 20121009
	UINT SpectrumTuningThread(); // 20121017

	void RunSpectrumTestThread();
	void StopSpectrumTestThread();

// Dialog Data
	enum { IDD = IDD_MTN_SPECTRUM_TEST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Thread related
	CWinThread* m_pWinThreadSpecTest;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT TimerVal);
	afx_msg void OnEnChangeEditSpectrumPrbsLen();
	afx_msg void OnEnChangeEditSpectrumPrbsAmplitude();
	afx_msg void OnEnChangeEditSpectrumPrbsFreqFactor();
	afx_msg void OnBnClickedButtonSpectrumTest();
	afx_msg void OnCbnSelchangeComboSpectrumTestAxis();
	afx_msg void OnBnClickedCheckSpectrumTestDebug();
	afx_msg void OnCbnSelchangeComboAxis1GroupPrbs();
	afx_msg void OnCbnSelchangeComboAxis2GroupPrbs();
	afx_msg void OnCbnSelchangeComboAxis3GroupPrbs();
	afx_msg void OnCbnSelchangeComboAxis4GroupPrbs();
	afx_msg void OnBnClickedCheckExciteFlagAxis1();
	afx_msg void OnBnClickedCheckExciteFlagAxis2();
	afx_msg void OnBnClickedCheckExciteFlagAxis3();
	afx_msg void OnBnClickedCheckExciteFlagAxis4();
	afx_msg void OnEnChangeEditNumPointsGroupPrbsAxis1();
	afx_msg void OnEnChangeEditNumPointsGroupPrbsAxis2();
	afx_msg void OnEnChangeEditNumPointsGroupPrbsAxis3();
	afx_msg void OnEnChangeEditNumPointsGroupPrbsAxis4();
	afx_msg void OnEnChangeEditLowLimitGroupPrbsAxis1();
	afx_msg void OnEnChangeEditLowLimitGroupPrbsAxis2();
	afx_msg void OnEnChangeEditLowLimitGroupPrbsAxis3();
	afx_msg void OnEnChangeEditLowLimitGroupPrbsAxis4();
	afx_msg void OnEnChangeEditUppLimitGroupPrbsAxis1();
	afx_msg void OnEnChangeEditUppLimitGroupPrbsAxis2();
	afx_msg void OnEnChangeEditUppLimitGroupPrbsAxis3();
	afx_msg void OnEnChangeEditUppLimitGroupPrbsAxis4();
	afx_msg void OnEnChangeEditPrbsAmpGroupPrbsAxis1();
	afx_msg void OnEnChangeEditPrbsAmpGroupPrbsAxis2();
	afx_msg void OnEnChangeEditPrbsAmpGroupPrbsAxis3();
	afx_msg void OnEnChangeEditPrbsAmpGroupPrbsAxis4();
	afx_msg void OnEnChangeEditPrbsLenGroupPrbsAxis1();
	afx_msg void OnEnChangeEditPrbsLenGroupPrbsAxis2();
	afx_msg void OnEnChangeEditPrbsLenGroupPrbsAxis3();
	afx_msg void OnEnChangeEditPrbsLenGroupPrbsAxis4();
	afx_msg void OnEnChangeEditPrbsFreqFactorGroupPrbsAxis1();
	afx_msg void OnEnChangeEditPrbsFreqFactorGroupPrbsAxis2();
	afx_msg void OnEnChangeEditPrbsFreqFactorGroupPrbsAxis3();
	afx_msg void OnEnChangeEditPrbsFreqFactorGroupPrbsAxis4();
	afx_msg void OnBnClickedButtonStartGroupPrbs();
	afx_msg void OnBnClickedButtonSaveGroupPrbsConfig();
	afx_msg void OnBnClickedButtonLoadGroupPrbsConfig();
	afx_msg void OnBnClickedButtonGroupPrbsEstimateTime();
	afx_msg void OnBnClickedButtonStopSpectrumTest();
	afx_msg void OnBnClickedButtonPlotSpectrumTest();
	afx_msg void OnBnClickedCheckSpectrumFlagVelLoop();
	afx_msg void OnEnChangeEditPrbsVelAmpGroupPrbsAxis1();
	afx_msg void OnEnChangeEditPrbsVelAmpGroupPrbsAxis2();
	afx_msg void OnEnChangeEditPrbsVelAmpGroupPrbsAxis3();
	afx_msg void OnEnChangeEditPrbsVelAmpGroupPrbsAxis4();
	afx_msg void OnBnClickedCheckSpectrumGroupTestDebug();
	afx_msg void OnBnClickedCheckSpectrumFlagDspData();
	afx_msg void OnEnKillfocusEditSineSweepLowFreqHz();
	afx_msg void OnEnKillfocusEditSineSweepUppFreqHz();
	afx_msg void OnEnKillfocusEditSineSweepNumCasesPerDec();
	afx_msg void OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis1();
	afx_msg void OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis2();
	afx_msg void OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis3();
	afx_msg void OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis4();
	afx_msg void OnBnClickedButtonStartGroupSineSweep();
	afx_msg void OnBnClickedCheckSpectrumDlgGotoVelStepAfter();
	afx_msg void OnBnClickedCheckSpectrumDlgTuneVelGain();
	afx_msg void OnBnClickedCheckSpectrumDlgTuneDrv();
	afx_msg void OnEnKillfocusEditMaxVkp1SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxVkp2SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxVkp3SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxVkp4SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxSof1SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxSof2SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxSof3SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxSof4SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxCkp1SpectrumDlg2();
	//afx_msg void OnEnKillfocusEditMaxCkp2SpectrumDlg();
	//afx_msg void OnEnKillfocusEditMaxCkp3SpectrumDlg();
	//afx_msg void OnEnKillfocusEditMaxCkp4SpectrumDlg();
	afx_msg void OnEnKillfocusEditMaxCki1SpectrumDlg();
	//afx_msg void OnEnKillfocusEditMaxCki2SpectrumDlg();
	//afx_msg void OnEnKillfocusEditMaxCki3SpectrumDlg();
	//afx_msg void OnEnKillfocusEditMaxCki4SpectrumDlg();
	afx_msg void OnCbnSelchangeComboPathScanningVelLoopSpectrumDlg();
	afx_msg void OnCbnSelchangeComboSpectrumDlgTuneDriverAxis();
	afx_msg void OnCbnSelchangeComboSpectrumDlgTuneDriverComport();
	afx_msg void OnCbnSelchangeComboSpectrumDlgTuneDriverType();
};
