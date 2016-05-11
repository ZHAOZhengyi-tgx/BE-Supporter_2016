#pragma once


// CProfCalcDlg dialog
#include "resource.h"

class CProfCalcDlg : public CDialog
{
	DECLARE_DYNAMIC(CProfCalcDlg)

public:
	CProfCalcDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProfCalcDlg();
	virtual void ShowWindow(int nCmdShow);

	void ProfCalcDlg_UpdateDesiredUIFromLocalVar();
	void ProfCalcDlg_UpdateActualUIFromLocalVar();
	void ProfCalcDlg_SaveProfileToFile();

	void InitProfCalcDlgUI();

	// Write UI
	void UpdateUShortToEdit(int nResId, unsigned short usValue);
	void UpdateIntToEdit(int nResId, int iValue);

	// Read from UI
	void ReadIntegerFromEdit(int nResId, int *iValue);
	void ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue);
	void ReadDoubleFromEdit(int nResId, double *pdValue);

	void EnableDlgUserInput(BOOL bFlagEnable);

	// Thread
	UINT TestMoveThread();
	BOOL m_fStopTestMoveThread;
	CWinThread* m_pWinThread;
	void RunTestMoveThread();
	void StopTestMoveThread();

	// Timer
	UINT_PTR  StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration);
	BOOL  StopTimer(UINT_PTR nTimerVal);
// Dialog Data
	enum { IDD = IDD_PROFILE_CALCULATOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT TimerVal);
	afx_msg void OnEnChangeProfCalcDesDist();
	afx_msg void OnBnClickedUturnHalfTwinSineBtn();
	afx_msg void OnEnChangeProfCalcDesMaxVel();
	afx_msg void OnEnChangeProfCalcDesMaxAcc();
	afx_msg void OnEnChangeProfCalcDesMaxJerk();
	afx_msg void OnEnChangeProfCalcDesInitVel();
	afx_msg void OnEnChangeProfCalcDesEndVel();
	afx_msg void OnEnChangeProfCalcDesInitAcc();
	afx_msg void OnEnChangeProfCalcDesEndAcc();
	afx_msg void OnEnChangeProfCalcDesInitJerk();
	afx_msg void OnEnChangeProfCalcDesEndJerk();
	afx_msg void OnBnClickedProfCalcSaveProfileBtn();
	afx_msg void OnBnClickedProfCalcDlgSaveProfFileCheck();
	afx_msg void OnBnClickedOfstSineBtn();
	afx_msg void OnCbnSelchangeProfCalcTesterComboCtrlAxis();
	afx_msg void OnEnChangeEditProfCalcTestMoveIter();
	afx_msg void OnBnClickedButtonProfCalcTestMoveStart();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedProfCalcTestSaveScopeDataCheck();
	afx_msg void OnBnClickedPolySam11234Btn();
};
