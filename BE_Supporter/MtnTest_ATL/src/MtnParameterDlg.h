#pragma once


// MtnParameterDlg dialog

class MtnParameterDlg : public CDialog
{
	DECLARE_DYNAMIC(MtnParameterDlg)

public:
	MtnParameterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MtnParameterDlg();

// language functions
	void SetUserInterfaceLanguage(int iLanguageOption);
	void UpdateUI_ByTimer();
	int UI_GetShowWindowFlag();
	virtual void MtnParameterDlg::ShowWindow(int nCmdShow);

// Dialog Data
	enum { IDD = IDD_PARAMETER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void SetComboAxisNameLanguage(int m_iLanguageOption, int iComboResId, int iDefComboOption);
	void UpdateDoubleToEdit(int nResId, double dValue, char *strFormat);
	void ReadDoubleFromEdit(int nResId, double *pdValue);
	void UpdateTuningParameterCurrentAxis();
	void UpdateTuningParameterCurrSectorTableX();
	void UpdateTuningParameterCurrSectorTableY();
	void UpdateTuningParameterBndHeadZ();

	void UpdateParameterTuneOutputCurrentAxis();
	void UpdateServoParameterDuringTune();
	void InitComboUI();
	void UpdateTuningBlkCurrAxis();
	void UpdateUI_ComboByVar();
	void UI_Show_TuneB1W(int iFlagEnable);
	void UI_UpdateCheckStatusB1W();
	void UpdateB1W_PerformanceButton();
	void PrepareStartThreadB1W();

	void UpdateB1W_ParaBndZ_TextEditBox();
	void EnableUI_B1W_EditOnly_ParaBndZ(int iEnaFlag);

public:
	void StartServoTuneThread();
	UINT DlgServoParaTuningThread();
	CWinThread* pDlgServoParaTuningWinThread; // 
	void StopServoTuneThread();
	UINT StartTimer(UINT TimerDuration);
	BOOL StopTimer();
	void EnableTuningUI_Combo_Button(bool bEnableFlag);
	void EnableTuningUI_Edit_Button_ByPass(bool bEnableFlag);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonParameterInitFromFile();
	afx_msg void OnBnClickedButtonParameterUploadFromCtrlBoard();
	afx_msg void OnBnClickedButtonParaDownload();
	afx_msg void OnBnClickedButtonParameterSaveFile();
	afx_msg void OnBnClickedButtonParameterDatabaseHomeAllAxis();
	afx_msg void OnBnClickedButtonParaInitDownload();
//	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButtonParameterSaveXls();
	afx_msg void OnCbnSelchangeComboParameterTuningAxis();
	afx_msg void OnBnClickedButtonParameterDlgStartTune();
	afx_msg void OnBnClickedButtonParameterDlgStopTune();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundVkp();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundVki();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundPkp();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundSof();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundVli();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundVli();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundSof();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundPkp();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundVki();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundVkp();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundAffc();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundAffc();
	afx_msg void OnEnKillfocusEditParameterDlgLowBoundTunePosn();
	afx_msg void OnEnKillfocusEditParameterDlgUppBoundTunePosn();
	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnCbnSelchangeComboParameterAxisTuningTheme();
	afx_msg void OnCbnSelchangeComboParameterAxisTuningAlgo();
	afx_msg void OnCbnSelchangeComboParameterAxisTuningPosnBlk();
	afx_msg void OnBnClickedCheckParameterDlgTuneDebugFlag();
	afx_msg void OnCbnSelchangeComboParameterAxisTuningTablePosnBlk();
	afx_msg void OnBnClickedButtonParameterDlgTuneAllStart();
	afx_msg void OnBnClickedCheckParameterDlgFlagTuneTblX();
	afx_msg void OnBnClickedCheckParameterDlgFlagTuneTblY();
	afx_msg void OnBnClickedCheckParameterDlgFlagTuneBondheadZ();
	afx_msg void OnCbnSelchangeComboParameterAxisTuningTablePosnInRange();
	afx_msg void OnBnClickedCheckTuneFlagTableSector();
	afx_msg void OnBnClickedButtonParameterDlgRunLastB1w();
	afx_msg void OnBnClickedCheckTuneFlagB1wActualContact();
	afx_msg void OnBnClickedCheckParaTuneDlgEnableB1w();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonParameterDlgLoadTuneCfg();
	afx_msg void OnBnClickedButtonParameterDlgRunSysB1w();
	afx_msg void OnBnClickedCheckTuneFlagB1wMotion();
	afx_msg void OnBnClickedButtonParameterDlgTuneIdle();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wSrch2b();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wReset();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wTail();
	afx_msg void OnBnClickedButtonParameterDlgTuneIdleB1wTraj2b();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wLooptop();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wLooping();
	afx_msg void OnBnClickedButtonParameterDlgTuneIdleB1wMove1b();
	afx_msg void OnBnClickedButtonParameterDlgTuneB1wSrch1b();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wSrch1b();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wIdle();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wMove1b();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wLooping();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wLooptop();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wTraj2b();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wTail();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wReset();
	afx_msg void OnBnClickedCheckParameterDlgTuneB1wSrch2b();
	afx_msg void OnBnClickedFlagCheckTuningByDll();
	afx_msg void OnEnKillfocusEditParameterDlgB1wVkp();
	afx_msg void OnEnKillfocusEditParameterDlgB1wAccffc();
	afx_msg void OnEnKillfocusEditParameterDlgB1wVki();
	afx_msg void OnEnKillfocusEditParameterDlgB1wPkp();
};
