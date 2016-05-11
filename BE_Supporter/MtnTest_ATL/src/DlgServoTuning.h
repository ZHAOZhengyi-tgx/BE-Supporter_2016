#pragma once

#include "resource.h"
// CDlgServoTuning dialog

class CDlgServoTuning : public CDialog
{
	DECLARE_DYNAMIC(CDlgServoTuning)

public:
	CDlgServoTuning(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgServoTuning();

	void PauseTimer();
	void RestartTimer();

	void UI_ShowWindowPartsPositionForceCalibration(BOOL bEnableFlag);
	void UI_EnableWindowPartsPositionForceCalibration(BOOL bEnableFlag);
	void UI_ShowWindowPartsPositionForceCalibrationVarSP(BOOL bEnableFlag);
	void UI_HideWindowPartsAll();
	void UI_ShowWindowsCheckEncoder();
	void UI_ShowWindowsOpenLoopTest();
	void UI_ShowWindowWbAppTunePass();

	void ReadDoubleFromEdit(int nResId, double *dValue);

	void UI_Wb_UpdateParameterPositionForceCalibration();

	void OnTimerRoutine();
	void UpdateServoFbStatusInTimer();

// Dialog Data
	enum { IDD = IDD_DIALOG_SERVO_CONTROL_TUNING };
private:
	BOOL OnInitDialog();
	void InitTabCtrlServoSetup(CTabCtrl *pcTabCtrl, int iTabCtrlId,  char *strTabText);
	void EnableTabOpenLoopTest();
	void DisableTabOpenLoopTest();
	void InitAxisForWireBond();
	void InitRangeComboOpenLoopTest();
	void InitSliderRangeDriveOutput();
	void InitComboShowWbPerformOpt(int nTotalNumWire);
	void UpdateActualDriveOutBySlide();
	void UpdateServoCtrlUI_ACS(BOOL bEnableFlag);
	void CDlgServoTuning::InitComboShowWbPerformOpt_2();

	void SetUserInterfaceByTab(int iCurrTabId);
	void SetUserInterfaceCheckEncoder();
	void SetUserInterfaceTabPass();
	void UpdateCheckEncoderGuide();
	void ResetCheckEncIdxCounterReg();
	void SetUserInterfaceVelocityLoopCheck();
	void ReadIntegerFromEdit(int nResId, int *iValue);
	void StartServoTuneThread();

	void Update_UI_FlagInvertEncoderDrvOut_CtrlCard();

	char cFlagDlgServoTuningThreadRunning;
	CWinThread* pDlgServoTuningWinThread; // , THREAD_PRIORITY_TIME_CRITICAL);
	void SetCtrlTabLanguage();
	void SetAxisNameLanguage();
	void UpdateLabelMaxMinRange();
	void OnShowWindow(BOOL bShow, UINT nStatus);

	void CDlgServoTuning::UI_ShowPerformOptServoCtrlTuningWbApp();
	CString CDlgServoTuning::WB_TuneGetString_PerformOpt_All();


	int m_iLanguageOption;

	// Timer
	UINT_PTR  StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration);
	BOOL StopTimer();

public:
	UINT DlgServoTuningThread();
	void SetUserInterfaceLanguage(int iLanguageOption);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeServoSetupTuneTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawServoCtrlTuningSliderDriverOut(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboAxisServoCtrlTuningOpenLoop();
	afx_msg void OnCbnSelchangeComboOutRangeServoCtrlTuningOpenLoop();
	afx_msg void OnBnClickedButtonServoCtrlTuningOpenLoopEnable();
	afx_msg void OnTimer(UINT TimerVal);
	afx_msg void OnBnClickedButtonServoCtrlTuningZeroDriverOut();
	afx_msg void OnBnClickedButtonServoCtrlTuningZeroFeedbackPosn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonServoCtrlTuningGetEncIdxOnce();
	afx_msg void OnEnChangeEdit1stPosnServoCtrlVelLoop();
	afx_msg void OnEnChangeEdit2ndPosnServoCtrlVelLoop();
	afx_msg void OnBnClickedButtonRead1stPosnServoCtrlVelLoop();
	afx_msg void OnBnClickedButtonRead2ndPosnServoCtrlVelLoop();
	afx_msg void OnBnClickedButtonStartTestPosnServoCtrlVelLoop();
	afx_msg void OnEnChangeEditTotalNumLoopServoCtrlVelLoop();
	afx_msg void OnBnClickedCheckFlagPlotServoCtrlTuning();
	afx_msg void OnBnClickedButtonServoCtrlTuningGetServicePass();
	afx_msg void OnBnClickedButtonServoCtrlGroupTest();
	afx_msg void OnBnClickedCheckServoCtrlTuningEnaWaveform();
	afx_msg void OnBnClickedButtonServoCtrlTuningGetWbPerformIdx();
	afx_msg void OnBnClickedButtonServoCtrlTuningHomeCurrAxis();
	afx_msg void OnBnClickedButtonServoCtrlTuningHomeWbXyz();
	afx_msg void OnCbnSelchangeComboShowPerformOptServoCtrlTuningWbApp();
	afx_msg void OnBnClickedButtonGenAftReportServoTune();
	afx_msg void OnBnClickedCheckFlagInvertEncoder();
	afx_msg void OnBnClickedCheckFlagInvertDriverOut();
	afx_msg void OnBnClickedCheckServoAdjustPositionSpringCompensateEnable();
	afx_msg void OnEnKillfocusEditServoAdjustCalibrateUppPosn();
	afx_msg void OnEnKillfocusEditServoAdjustCalibrateLowPosn();
	afx_msg void OnBnClickedButtonServoAdjustStartCalibratePosn();
	afx_msg void OnBnClickedButtonServoAdjustVerifyPositionSpringGravity();
	afx_msg void OnBnClickedButtonServoAdjustVerifyPosnSpringGravity();
	afx_msg void OnCbnSelchangeComboServoCtrlTuningWbDebugLevel();
	afx_msg void OnEnKillfocusEditEncResServoCtrlTuning();
	afx_msg void OnBnClickedCheckSafetyForceFeedbackServoCtrlTuning();
};
