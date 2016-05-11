#pragma once


// MtnDialog_FbMonitor dialog
#include "resource.h"
#include "afxwin.h"
#include "MtnApi.h"

class MtnDialog_FbMonitor : public CDialog
{
	DECLARE_DYNAMIC(MtnDialog_FbMonitor)

public:
	MtnDialog_FbMonitor( CWnd* pParent = NULL);   // standard constructor
	virtual ~MtnDialog_FbMonitor();
	virtual void ShowWindow(int nCmdShow);

//	HANDLE Handle;				// communication handle
//	HANDLE m_hCommHandle;
	int m_iX_LeftBottom,    m_iY_LeftBottom,    m_iX_RightTop,  m_iY_RightTop;

// Dialog Data
	enum { IDD = IDD_FB_MONITOR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	int iMotorState;
	double dblFbPosition, dblFbVelocity, dblFbAcceleration, dblFbForce;
	int iFbIndex, iFbLimit;
	int iMonitorFlag;
	CString cstrMonitorFlag;

	BOOL m_bStart_ACS_TriggerEFO_CheckBy1739;
	BOOL m_fStopMonitorThread;
//	BOOL m_fConnect;
	CWinThread* m_pWinThread;

	// Thread Functions
	void ThreadUpdateFromEFO_BSD_By1739U(int iLanguageOption);
	void ThreadUpdateFrom_EFO_By_ACS_1739U();

//  UI functions
	void ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue);
	void UpdateIntToEdit(int nResId, int iValue);
	char  cFlagEnable_Group_Servo_ACS_ChannelFbMonitor;
	char  cFlagEnable_Group_ACS_OUT_MtnDialog_FbMonitor;

	int iDebug;
	CString cstrChannelNum;
	CString cstrFbPosition; 
	CString cstrFbVelocity; 
	CString cstrFbAcceleration; 
	CString cstrFbForce; 
	CString cstrFbIndex; 
	CString cstrFbLimit; 
	CString cstrMotionStatus; // IDC_FB_MOTOR_STATUS, iMotorState

	// 1739U
	LONG        lDrvHandleDigitalInput_IO1739U ;          // driver handle
	LONG        lDrvHandleDigitalOutput_IO1739U;          // driver handle
	LONG        lDrvHandleEFO_Monitor_1739U;          // driver handle

	DECLARE_MESSAGE_MAP()
	CComboBox combSelectChannel;
public:
	int iOut0[1];
	void ACS_TriggerEFO_ThenReset();
	void ACS_SwitchDualFAB();  // 20121018
	void CheckEfoFbBy1739U_WbAddr();
	void ACS_TriggerBSD_Relay();

	void SetUserInterfaceLanguage(int iLanguageOption);
	void UI_Enable_Group_1739burn_Efo207(BOOL bEnableFlag);
	void UI_Enable_Group_EFO207_Fb_CheckBy_1739U_Level(BOOL bEnableFlag);
	void UI_Enable_Group_ACS_OUT_MtnDialog_FbMonitor(BOOL bEnableFlag);
	void UI_Enable_Group_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag);
	void UI_Enable_Group_Setting_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag);
	void UI_Enable_Group_Normal_ACS_ServoHome_MtnDialog_FbMonitor(BOOL bEnableFlag);
	void UI_Enable_Group_Servo_ACS_FbMonitorDlg(BOOL bEnableFlag);
	void UI_InitWH_IOTextGuide(int iModuleId);
	void UI_UpdateTextGuideWH_IO(int iModuleId);
	void UI_UpdateTriggerEFO_RatioPeriod();
	void UI_OpenBoard1739CheckEFO_GoodNG();
	void InitUI_ComboChannelSel();
	void InitUI_ComboSelectUSG_Channel(); 

	CString GuideWH_IO_GetText_OneTrackVLED(int iModuleId);
	CString GuideWH_IO_GetText_VLED(int iModuleId);
	CString GuideWH_IO_GetText_HoriLed(int iModuleId);

	afx_msg void OnCbnSelchangeChannelSelCombo();
	afx_msg void OnEnChangeFbPositionText();

	// Thread related
public:
	UINT MonitorThread();
	void MonitorUpdateAcsFeedback();
	void MonitorUpdateDigitalIO1739Status();
	void MonitorUpdateDigitalInputStatic();
	void UpdateInputStaticFrom1739IO();
	// For 7248, 20120823
	void UpdateInputStaticFromAD7248IO();
	void DialogInitDigitalIO_7248();
	void MonitorUpdateDigitalIO7248CurrByte();
	void DigitalIO_ToggleOutputBit(unsigned short usMask);
	void DigitalIO7248_ToggleOutputBit(unsigned short usMask);

	void UpdateServoCtrlUI();
	// 1739U
	void DigitalIO1739_ToggleOutputBit(unsigned short usMask);
	void DigitalIO1739_TrigEFO_Byte();  // 20091121
	void DigitalIO1739_ResetTrigEFO_Byte();  // 20091121
	void DialogInitDigitalIO_1739U();
	void InitVarMachineType(int iFlagMachType);

protected:
	void MonitorUpdateDigitalInputStatusFilling(unsigned int uiStaticId, int iFlagLighting);
	void RunMonitorThread();
	void StopMonitorThread();
	void UpdateSearchHomeEdit();

	virtual BOOL OnInitDialog();
	virtual void OnClose();

protected:
//	CEdit cFbPositionEdit;
public:
	afx_msg void OnBnClickedEnableMotor();
	afx_msg void OnBnClickedFbMonitorStart();
	afx_msg void OnBnClickedCheckOut0();
	afx_msg void OnEnChangeEditSearchHomeMoveVel();
	afx_msg void OnEnChangeEditSearchHomeMoveAcc();
	afx_msg void OnEnChangeEditSearchHomeMoveDec();
	afx_msg void OnEnChangeEditSearchHomeMoveJerk();
	afx_msg void OnEnChangeEditSearchHomeDetectFreqFactor10khz();
	afx_msg void OnEnChangeEditSearchHomeDetectMaxDistProt();
	afx_msg void OnEnChangeEditSearchHomeMoveDistSearchLimit();
	afx_msg void OnEnChangeEditSearchHomeJogVelSearchLimit();
	afx_msg void OnEnChangeEditSearchHomePeThSearchLimit();
	afx_msg void OnEnChangeEditSearchHomeMoveDistBfSearchIndex1();
	afx_msg void OnEnChangeEditSearchHomeJogVelSearchIndex1();
	afx_msg void OnEnChangeEditSearchHomeMoveDistBfSearchIndex2();
	afx_msg void OnEnChangeEditSearchHomeJogVelSearchIndex2();
	afx_msg void OnBnClickedButtonAxisGoHome();
	afx_msg void OnBnClickedCheckDebugSearchIndex();
	afx_msg void OnEnChangeSearchHomePosnErrSettle();
	afx_msg void OnBnClickedButtonAxisGoLimit();
	afx_msg void OnBnClickedButtonAxisGoHomeFromLimit();
	afx_msg void OnBnClickedButtonAxisApiGoLimitAndHome();
	afx_msg void OnBnClickedButtonAxisApiGoLimit();
	afx_msg void OnBnClickedButtonAxisApiGoHomeFromLimit();
	afx_msg void OnBnClickedCheckSearchIndexVerifyByRepeating();
	afx_msg void OnBnClickedButtonBonderHomeAll();
	afx_msg void OnBnClickedCheckAcsOut1();
	afx_msg void OnBnClickedCheckAcsOut2();
	afx_msg void OnBnClickedCheckAcsOut3();
	afx_msg void OnCbnSelchangeComboDigitalIo1739List();
	afx_msg void OnCbnSelchangeComboDigitalIoSelectInputByte();
	afx_msg void OnCbnSelchangeComboDigitalIoSelectOutputByte();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit7();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit6();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit5();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit4();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit3();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit2();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit1();
	afx_msg void OnBnClickedButtonDigitalIoOutputBit0();
	afx_msg void OnCbnSelchangeComboDigitalOutput1739List();
	afx_msg void OnCbnSelchangeComboEfoMonitorUse1739uCard();
	afx_msg void OnBnClickedButtonFbMonitorEfoGoodngBy1739u();
	afx_msg void OnBnClickedCheckEfoMonitor1739uDebugFlag();
	afx_msg void OnBnClickedButtonFbMonitorHomeAllApi();
	afx_msg void OnCbnSelchangeComboEfoTrigUse1739uByte();
	afx_msg void OnBnClickedCheckFlagEnaAcsChMonitor();
	afx_msg void OnBnClickedCheckFlagEnaHomeAcsServoCh();
	afx_msg void OnBnClickedCheckEnaAcsIo();
	afx_msg void OnBnClickedCheckEnaFlagBurnEtorch1739();
	afx_msg void OnBnClickedCheckEnaEfo207FbCheckBy1739Level();
	afx_msg void OnCbnSelchangeComboDlgFbSelectWhMod();
	afx_msg void OnBnClickedCheckFbAcsOut8();
	afx_msg void OnBnClickedCheckFbAcsOut9();
	afx_msg void OnBnClickedCheckFbAcsOut10();
	afx_msg void OnBnClickedCheckFbAcsOut11();
	afx_msg void OnBnClickedCheckFbAcsOut4();
	afx_msg void OnBnClickedCheckFbAcsOut5();
	afx_msg void OnBnClickedCheckFbAcsOut6();
	afx_msg void OnBnClickedCheckFbAcsOut7();
	afx_msg void OnCbnSelchangeComboDlgFbSelectMachType();
	afx_msg void OnNMReleasedcaptureSliderFbDlgTrigEfoRatioPeriod(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnKillfocusEditFbMonitorEfoBsdBy1739uTotalCnt();
	afx_msg void OnCbnSelchangeComboSelectOutputUsgStation();
	afx_msg void OnBnClickedCheckFbMonitorDlgTestDualFab();
};
