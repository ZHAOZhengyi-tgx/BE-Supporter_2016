#pragma once


// CMtnStepperDlg dialog
#include "resource.h"
#include "afxwin.h"
#include "mtnapi.h"


class CMtnStepperDlg : public CDialog
{
	DECLARE_DYNAMIC(CMtnStepperDlg)

public:
	CMtnStepperDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnStepperDlg();
	virtual BOOL ShowWindow(int nCmdShow);

	void MyShowErrorMsg(long ulErrCode);

	//UINT StartTimer(UINT TimerDuration);
	//BOOL StopTimer();

	// Thread
	BOOL m_fStopThreadDlgStepper1240;
	CWinThread* m_pWinThreadDlgStepper1240;
	void RunThreadDlgStepper1240();
	void StopThreadDlgStepper1240();
	void InitLocalVariable();   // 20110518
	void UpdateEditText();
	void DownloadWithErrorPrompt();
	UINT ThreadDlgStepper1240();
	void  DlgTimerEvent();
	int DlgGetFlagTimerRunning();
	void CMtnStepperDlg::OnChangeComboStepperBoardId_Ad1240();
	void CMtnStepperDlg::InitDetectStepperCtrlBord();

// Dialog Data
	enum { IDD = IDD_STEPPER_TEST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void UpdateButtonText();
	virtual BOOL OnInitDialog();
	void UploadSpeedProfileCurrBoard();
	void UpdateDigitalInput_StepperCtrl1240();
	void UpdateDigitalInput_StepperCtrlLeetro2812(); // 20120915
	void UpdateUI_FromDigitalInput();
	void UI_UpdateDigitalInputStatusFilling(unsigned int uiStaticId, unsigned short  usFlagLighting);
	unsigned int ReadUnsignedIntegerFromEdit(int nResId);
	void UpdateUnsignedIntToEdit(int nResId, unsigned int uiValue);

	void UpdateSpeedProfileText();
	void UpdatePositionRegistrationText();
	void UpdateWbAppName_CurrentStpCtrlBoard();
	void UI_SetAppNameForVerticalLED_Fork();
	void UI_SetAppNameNull();
	void UI_SetAppNameForHoriLED(unsigned int uiBoardId);
	void UI_SetAppNameForOneTrackVerLED(unsigned int uiBoardId);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckStepperSelAxisX();
	afx_msg void OnBnClickedCheckStepperSelAxisY();
	afx_msg void OnBnClickedCheckStepperSelAxisZ();
	afx_msg void OnBnClickedCheckStepperSelAxisU();
	afx_msg void OnCbnSelchangeComboStepperBoardId();
	CComboBox StepperBoardIdCombo;
	afx_msg void OnBnClickedButtonStepperOpenCloseBoard();
	afx_msg void OnBnClickedButtonStepperMoveDist();
	afx_msg void OnEnChangeEditStepperMoveDistX();
	afx_msg void OnEnChangeEditStepperMoveDistY();
	afx_msg void OnEnChangeEditStepperMoveDistZ();
	afx_msg void OnEnChangeEditStepperMoveDistU();
	afx_msg void OnEnChangeEditStepperTargetPosnX();
	afx_msg void OnEnChangeEditStepperTargetPosnY();
	afx_msg void OnEnChangeEditStepperTargetPosnZ();
	afx_msg void OnEnChangeEditStepperTargetPosnU();
//	afx_msg void OnTimer(UINT nIDEvent);

	afx_msg void OnBnClickedButtonStepperHomeAxis();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditStepperSvX();
	afx_msg void OnEnChangeEditStepperSvY();
	afx_msg void OnEnChangeEditStepperSvZ();
	afx_msg void OnEnChangeEditStepperSvU();
	afx_msg void OnEnChangeEditStepperDvX();
	afx_msg void OnEnChangeEditStepperDvY();
	afx_msg void OnEnChangeEditStepperDvZ();
	afx_msg void OnEnChangeEditStepperDvU();
	afx_msg void OnBnClickedButtonStepperSaveProfile();
	afx_msg void OnEnChangeEditStepperMdvX();
	afx_msg void OnEnChangeEditStepperMdvY();
	afx_msg void OnEnChangeEditStepperMdvZ();
	afx_msg void OnEnChangeEditStepperMdvU();
	afx_msg void OnEnChangeEditStepperAcX();
	afx_msg void OnEnChangeEditStepperAcY();
	afx_msg void OnEnChangeEditStepperAcZ();
	afx_msg void OnEnChangeEditStepperAcU();
	afx_msg void OnEnChangeEditStepperJerkX();
	afx_msg void OnEnChangeEditStepperJerkY();
	afx_msg void OnEnChangeEditStepperJerkZ();
	afx_msg void OnEnChangeEditStepperJerkU();
	afx_msg void OnEnChangeEditStepperPosLimitX();
	afx_msg void OnEnChangeEditStepperPosLimitY();
	afx_msg void OnEnChangeEditStepperPosLimitZ();
	afx_msg void OnEnChangeEditStepperPosLimitU();
	afx_msg void OnEnChangeEditStepperExtdriveMode();
	afx_msg void OnEnChangeEditStepperExtdriveNumPulse();
	afx_msg void OnBnClickedButtonStepperSetExtMode();
	afx_msg void OnEnChangeEditStepperHomeTypeX();
	afx_msg void OnEnChangeEditStepperHomeTypeY();
	afx_msg void OnEnChangeEditStepperHomeTypeZ();
	afx_msg void OnEnChangeEditStepperHomeTypeU();
	afx_msg void OnEnChangeEditStepperHomeP0VelX();
	afx_msg void OnEnChangeEditStepperHomeP0VelY();
	afx_msg void OnEnChangeEditStepperHomeP0VelZ();
	afx_msg void OnEnChangeEditStepperHomeP0VelU();
	afx_msg void OnEnChangeEditStepperHomeP0DirX();
	afx_msg void OnEnChangeEditStepperHomeP0DirY();
	afx_msg void OnEnChangeEditStepperHomeP0DirZ();
	afx_msg void OnEnChangeEditStepperHomeP0DirU();
	afx_msg void OnEnChangeEditStepperHomeP1VelX();
	afx_msg void OnEnChangeEditStepperHomeP1VelY();
	afx_msg void OnEnChangeEditStepperHomeP1VelZ();
	afx_msg void OnEnChangeEditStepperHomeP1VelU();
	afx_msg void OnEnChangeEditStepperHomeP1DirX();
	afx_msg void OnEnChangeEditStepperHomeP1DirY();
	afx_msg void OnEnChangeEditStepperHomeP1DirZ();
	afx_msg void OnEnChangeEditStepperHomeP1DirU();
	afx_msg void OnEnChangeEditStepperHomeOffsetX();
	afx_msg void OnEnChangeEditStepperHomeOffsetY();
	afx_msg void OnEnChangeEditStepperHomeOffsetZ();
	afx_msg void OnEnChangeEditStepperHomeOffsetU();
	afx_msg void OnEnChangeEditStepperHomeModeX();
	afx_msg void OnEnChangeEditStepperHomeModeY();
	afx_msg void OnEnChangeEditStepperHomeModeZ();
	afx_msg void OnEnChangeEditStepperHomeModeU();
	afx_msg void OnBnClickedCheckStepper1240FlagIterativeMove();
	afx_msg void OnEnKillfocusEditStepper1240MoveIteration();
	afx_msg void OnEnKillfocusEditStepper1240IterativeMoveDelay();
	afx_msg void OnBnClickedCheckPci1240Out4();
	afx_msg void OnBnClickedCheckPci1240Out5();
	afx_msg void OnBnClickedCheckPci1240Out6();
	afx_msg void OnBnClickedCheckPci1240Out7();
	afx_msg void OnBnClickedButtonStepperReverseMoveDist();
	afx_msg void OnBnClickedButtonStepperToTargetPosn();
	afx_msg void OnEnKillfocusEditStepperSvX();
	afx_msg void OnEnKillfocusEditStepperSvY();
	afx_msg void OnEnKillfocusEditStepperSvZ();
	afx_msg void OnEnKillfocusEditStepperSvU();
	afx_msg void OnEnKillfocusEditStepperDvX();
	afx_msg void OnEnKillfocusEditStepperDvY();
	afx_msg void OnEnKillfocusEditStepperDvZ();
	afx_msg void OnEnKillfocusEditStepperDvU();
	afx_msg void OnEnKillfocusEditStepperMdvX();
	afx_msg void OnEnKillfocusEditStepperMdvY();
	afx_msg void OnEnKillfocusEditStepperMdvZ();
	afx_msg void OnEnKillfocusEditStepperMdvU();
	afx_msg void OnEnKillfocusEditStepperAcX();
	afx_msg void OnEnKillfocusEditStepperAcY();
	afx_msg void OnEnKillfocusEditStepperAcZ();
	afx_msg void OnEnKillfocusEditStepperAcU();
	afx_msg void OnEnKillfocusEditStepperJerkX();
	afx_msg void OnEnKillfocusEditStepperJerkY();
	afx_msg void OnEnKillfocusEditStepperJerkZ();
	afx_msg void OnEnKillfocusEditStepperJerkU();
};
