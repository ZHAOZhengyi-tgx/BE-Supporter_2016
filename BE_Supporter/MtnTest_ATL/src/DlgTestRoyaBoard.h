#pragma once


// CDlgTestRoyaBoard dialog
#include "resource.h"

class CDlgTestRoyaBoard : public CDialog
{
	DECLARE_DYNAMIC(CDlgTestRoyaBoard)

public:
	CDlgTestRoyaBoard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTestRoyaBoard();
	virtual BOOL OnInitDialog();
	virtual void ShowWindow(int nCmdShow);
	void UpdateLCD_BtnText();
	virtual UINT StartTimer(UINT TimerDuration);
	BOOL StopTimer ();

	void DlgTimerEvent();
	int UI_GetShowWindowFlag();

	void UI_InitSignalGenerator();
	void CDlgTestRoyaBoard::UI_UpdateSignalGen_SineOutCfg();
	void CDlgTestRoyaBoard::UI_ShowWindow_SignalGen_RoyaDlg(int iFlag);
/// BQM
	void CDlgTestRoyaBoard::UI_UpdateBQM_RoyaDlg();
	void CDlgTestRoyaBoard::UI_ShowWindow_QBM_RoyaDlg(int iFlag);


	void UpdateRoyaBoardUsgStartButton();
	void InitDlgBSD();
	void UpdateParameterDlgBSD();
	void UpdateStatusDlgBSD();
	void SetUserInterfaceLanguage(int iLanguageOption);

	void UpdateUShortToEdit(int nResId, unsigned short usValue);
	void ReadUShortFromEdit(int nResId, unsigned short *pusValue);
	void ReadFloatFromEdit(int nResId, float *pfValue);
	void InitParameterBSD();
	void UpdateFloatToEdit(int nResId, float fValue);

	void UI_EnableGroupUSG_Items(int iEnableFlag);

	void BSDAutoCheckCommuError();
	void UpdateRoyaBoardKeyInterruptCounter();
	void UpdateSticknessStatusUI();

// Dialog Data
	enum { IDD = IDD_DLG_TEST_ROYA_BOARD };

private:
	void RoyaDlgCheckAndShowDebugMessage(short sRetCode);
	void UpdateRoyaDlgRT_Clock();
	void UpdateUsgSeg();
	void init_roya_board_usg_blk();
	void InitDacSliderRangePosn();
	void UpdateDacVoltageValueBySlider();

	void CDlgTestRoyaBoard::UpdateSignalGenSineOutampVoltValueBySlider();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	void ShowWindow(int nCmdShow);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRoyaTestButtonSendCommtest();
	afx_msg void OnBnClickedButtonRoyaGetVersion();
	afx_msg void OnBnClickedRoyaLcdBit7();
	afx_msg void OnBnClickedRoyaLcdBit6();
	afx_msg void OnBnClickedRoyaLcdBit5();
	afx_msg void OnBnClickedRoyaLcdBit4();
	afx_msg void OnBnClickedRoyaLcdBit3();
	afx_msg void OnBnClickedRoyaLcdBit2();
	afx_msg void OnBnClickedRoyaLcdBit1();
	afx_msg void OnBnClickedRoyaLcdBit0();
	afx_msg void OnBnClickedBtnRoyaTestLcd();
	afx_msg void OnCbnSelchangeRoyaUsgComboBlkNo();
	afx_msg void OnEnChangeRoyaUsgEditSegNo();

	afx_msg void OnBnClickedRoyaButtonSetUsgSeg();
	afx_msg void OnEnChangeRoyaUsgEditNextBlkIdx();
	afx_msg void OnEnChangeRoyaUsgEditTrigAddr();
	afx_msg void OnEnChangeRoyaUsgEditMaxCountTrigProt();
	afx_msg void OnBnClickedRoyaBoardSetTimerPeriod();
	afx_msg void OnBnClickedStartRoyaUsgProfFsm();
	afx_msg void OnTimer(UINT TimerVal);
	afx_msg void OnBnClickedButtonWarmResetRoyaUsg();
	afx_msg void OnEnChangeEditTrigPatternRoyaUsgByAcs();
	afx_msg void OnBnClickedButtonTrigRoyaUsgViaAcs();
	afx_msg void OnCbnSelchangeComboRoyaSelectDacIdx();
	afx_msg void OnEnChangeEditRoyaDacValue();
	afx_msg void OnBnClickedButtonRoyaSetDac();
	afx_msg void OnCbnSelchangeComboRoyaAdcIndex();
	afx_msg void OnCbnSelchangeRoyaNsdBlkIdx();
	afx_msg void OnEnChangeRoyaNsdEditFactorIsr();
	afx_msg void OnEnChangeRoyaNsdEditSampleLen();
	afx_msg void OnEnChangeRoyaNsdEditAdcIdx();
	afx_msg void OnEnChangeRoyaNsdEditDacIdx();
	afx_msg void OnEnChangeRoyaNsdEditDacValue();
	afx_msg void OnBnClickedRoyaNsdButtonSetBlkPara();
	afx_msg void OnBnClickedRoyaNsdButtonStartWBlk();
	afx_msg void OnBnClickedRoyaNsdButtonWarmReset();
	afx_msg void OnBnClickedRoyaNsdButtonGetData();
	afx_msg void OnBnClickedRoyaUsgCheckAuto();
	afx_msg void OnBnClickedRoyaAddaCheckAuto();
	afx_msg void OnBnClickedRoyaNsdCheckAuto();
	afx_msg void OnBnClickedRoyaNsdButtonApiSetBlkPara();
	afx_msg void OnBnClickedRoyaNsdButtonApiGetBlkPara();
	afx_msg void OnEnChangeEditRoyaRtcYear();
	afx_msg void OnEnChangeEditRoyaRtcMonth();
	afx_msg void OnEnChangeEditRoyaRtcMdate();
	afx_msg void OnEnChangeEditRoyaRtcWday();
	afx_msg void OnEnChangeEditRoyaRtcHour();
	afx_msg void OnEnChangeEditRoyaRtcMin();
	afx_msg void OnEnChangeEditRoyaRtcSecond();
	afx_msg void OnBnClickedButtonSetRoyaRtClock();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeRoyaComboTimerId();
	afx_msg void OnEnChangeEditRoyaBsdDebounce1stBond();
	afx_msg void OnEnChangeEditRoyaBsdDebounce2ndBond();
	afx_msg void OnEnChangeEditRoyaBsdThreHoldV1stBond();
	afx_msg void OnEnChangeEditRoyaBsdThreHoldV2ndBond();
	afx_msg void OnEnChangeEditRoyaBsdDebounce1stBondConseLow();
	afx_msg void OnEnChangeEditRoyaBsdDebounce2ndBondConseLow();
	afx_msg void OnEnChangeEditRoyaBsdSkipSample1stBond();
	afx_msg void OnEnChangeEditRoyaBsdSkipSample2ndBond();
	afx_msg void OnEnChangeEditRoyaBsdThreHoldVLowerLead2ndBond();
	afx_msg void OnBnClickedRoyaBsdButtonApiSetAnalyzeSticknessPara();
	afx_msg void OnBnClickedRoyaBsdButtonApiGetAnalyzeSticknessPara();
	afx_msg void OnBnClickedRoyaBsdButtonGetFirmwareAnalyze();
	afx_msg void OnBnClickedRoyaBsdButtonFirmwareAnalyzeStickness();
	afx_msg void OnBnClickedCheckCheckRoyaCommunicationByBsd();
	afx_msg void OnBnClickedRoyaTestCheckEnableTimerAll();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedRoyaButtonDownloadFirmware();
	afx_msg void OnBnClickedRoyaButtonLoadFirmwareBin();
	afx_msg void OnBnClickedRoyaCheckDownloadVerify();
	afx_msg void OnBnClickedRoyaBsdFlagCheckWaveform();
//	afx_msg void OnNMCustomdrawSliderSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnNMThemeChangedSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnTRBNThumbPosChangingSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRoyaBsdFlagCheckDryRun();
	afx_msg void OnBnClickedRoyaBsdButtonETestWiring();
	afx_msg void OnBnClickedCheckRoyaBsdDebug();
	afx_msg void OnBnClickedButtonRoyaBsdDlgTestPingStartDead();
	afx_msg void OnStnClickedStaticRoyaUsgProfDuration();
	afx_msg void OnNMReleasedcaptureSliderRoyaSigGenSineOutAmpDac(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderRoyaSetDacVolt(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditRoyaSigSineOutGenCycle();
	afx_msg void OnEnKillfocusEditRoyaSigSineOutGenFracCycle();
	afx_msg void OnEnKillfocusEditRoyaBqmPreCounterTh1();
	afx_msg void OnEnKillfocusEditRoyaBqmPreCounterTh2();
	afx_msg void OnEnKillfocusEditRoyaBqmMaxCounterAdc1();
	afx_msg void OnEnKillfocusEditRoyaBqmMaxCounterAdc2();
	afx_msg void OnCbnSelchangeRoyaComboDacMaxPercent();
	afx_msg void OnEnKillfocusEditRoyaStepCmdDac2V1();
	afx_msg void OnEnKillfocusEditRoyaStepCmdDac2V2();
	afx_msg void OnEnKillfocusEditRoyaStepCmdDac2ProtV();
	afx_msg void OnCbnSelchangeComboRoyaStepCmdDac2T();
	afx_msg void OnBnClickedCheckRoyaStepCmdDac2Run();
};
