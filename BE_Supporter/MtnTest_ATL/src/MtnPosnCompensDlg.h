#pragma once

#include "resource.h"

// CMtnPosnCompensDlg dialog

class CMtnPosnCompensDlg : public CDialog
{
	DECLARE_DYNAMIC(CMtnPosnCompensDlg)

public:
	CMtnPosnCompensDlg(HANDLE hInHandle, CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnPosnCompensDlg();
	virtual void ShowWindow(int nCmdShow);

	void UpdateDataFromVarToUI();
	void UpdateDataFromUIToVar();
	void UpdateEditCurrSegment(unsigned int uiCurrSeg);

	void UI_UpdateEditByDetectionFlag(char cFlagDetectionSwitch);

	void UI_InitPositionForceCalibrationDlg(); // 20121129
	HANDLE m_hHandle;
	void ThreadDoSearchContactWithForceControl();
	void ThreadDoForceVerification(); // 20130103
	void ThreadDoForceCalibration();

	void ThreadDoForceVerificationByGramJig(); // 20130107

	void Thread_ForceLogVerification();  // 20130103

	CWinThread* m_pWinThreadPosnForceCali;

	void RunPosnForceCaliThread();
	void StopPosnForceCaliThread();
//	void DoThreadForceVerificationLog();

	BOOL mFlagStopPosnForceCaliThread;
	void UI_SetEnableButton(BOOL bFlagEnable);
	void ReadDoubleFromEdit(int nResId, double *pdValue);

	int UI_GetShowWindowFlag();
	void DlgTimerEvent();
	void DlgEventUpdateForceFbAdc();
	void InitForceFbAdc();
// Dialog Data
	enum { IDD = IDC_POSNCOMPN_OFFSET_LABEL};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPosnCompensateStartTune();
	afx_msg void OnBnClickedPosncompenDebugFlag();
	afx_msg void OnBnClickedPosnCompensateStartVerify();
	afx_msg void OnBnClickedButtonTeachContact();
	afx_msg void OnEnChangeEditForceControlMaxSegment();
	afx_msg void OnCbnSelchangeComboForceControlEditSegment();
	afx_msg void OnEnChangeEditForceControlRampCountCurrSeg();
	afx_msg void OnEnChangeEditForceControlLevelCntCurrSeg();
	afx_msg void OnEnChangeEditForceControlLevelAmpCurrSeg();
	afx_msg void OnBnClickedButtonSearchContactWithForceControl();
	afx_msg void OnEnChangeForceControlHoldCountInitForce();
	afx_msg void OnBnClickedButtonForceCalibration();
	afx_msg void OnBnClickedButtonForceVerification();
	afx_msg void OnEnChangeEditForceCaliTotalSeg();
	afx_msg void OnEnChangeEditForceCaliGramFirstSeg();
	afx_msg void OnEnChangeEditForceCaliGramLastSeg();
	afx_msg void OnEnChangeEditForceVerifyDesiredGram();
	afx_msg void OnEnChangePosncompnUpperLimit();
	afx_msg void OnEnChangePosncompnTotalPoint();
	afx_msg void OnEnChangePosncompnLowerLimit();
	afx_msg void OnBnClickedButtonVerificationLog();
	afx_msg void OnBnClickedOkDialogPositionCompensation();
	afx_msg void OnBnClickedCheckContactTuneFlagB1w();
	afx_msg void OnCbnSelchangeComboPosnForceCompensationDlgForceControlDetectionFlag();
	afx_msg void OnEnKillfocusPosncompnUpperLimit();
	afx_msg void OnEnKillfocusPosncompnLowerLimit();
	afx_msg void OnEnKillfocusEditTeachContactStartDist();
	afx_msg void OnEnKillfocusPosncompnTotalPoint();
	afx_msg void OnEnKillfocusEditTeachContactStartVel();
	afx_msg void OnEnKillfocusEditTeachContactSearchVel();
	afx_msg void OnEnKillfocusEditTeachContactEndPosn();
	afx_msg void OnEnKillfocusEditTeachContactMaxDist();
	afx_msg void OnEnKillfocusEditTeachContactAntiBounce();
	afx_msg void OnEnKillfocusEditTeachContactTimeOut();
	afx_msg void OnEnKillfocusEditForceControlMaxSegment();
	afx_msg void OnEnKillfocusEditContactForceLoopTest();
	afx_msg void OnEnKillfocusEditForceControlRampCountCurrSeg();
	afx_msg void OnEnKillfocusEditForceControlLevelCntCurrSeg();
	afx_msg void OnEnKillfocusEditForceControlLevelAmpCurrSeg();
	afx_msg void OnEnKillfocusEditForceCaliTotalSeg();
	afx_msg void OnEnKillfocusEditForceCaliGramFirstSeg();
	afx_msg void OnEnKillfocusEditForceCaliGramLastSeg();
	afx_msg void OnEnKillfocusEditForceVerifyDesiredGram();
	afx_msg void OnCbnSelchangeComboSearchVelPosnForceCalibrationDlg();
	afx_msg void OnCbnSelchangeComboSearchTolPePosnForceCalibrationDlg();
	afx_msg void OnCbnKillfocusComboDampPosnForceCalibrationDlg();
	afx_msg void OnCbnSelchangeComboPreImpForcePositionForceCaliDlg();
	afx_msg void OnEnKillfocusEditMaxJerkMoveSrchHtPosnForceCaliDlg();
	afx_msg void OnCbnSelchangeComboLoopCountCheckForcePositionCalibrationDlg();
	afx_msg void OnBnClickedButtonCalibrateForceRatioPosnForceCalibrateDlg();
	afx_msg void OnEnSetfocusEditForceReadByGramGaugePosnForceDlg();
	afx_msg void OnEnKillfocusForceVerifyTrialsPerLoop();
	afx_msg void OnBnClickedCheckForceVerifyByGramJig();
	afx_msg void OnEnKillfocusEditForceCaliDeltaDcom();
};
