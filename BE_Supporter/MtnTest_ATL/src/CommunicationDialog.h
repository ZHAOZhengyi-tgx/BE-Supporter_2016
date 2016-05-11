#if !defined(AFX_COMMUNICATIONDIALOG_H__C97F394F_4E72_4451_B542_297993FE2BB7__INCLUDED_)
#define AFX_COMMUNICATIONDIALOG_H__C97F394F_4E72_4451_B542_297993FE2BB7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CommunicationDialog.h : header file
//
#include "resource.h"
/////////////////////////////////////////////////////////////////////////////
// CCommunicationDialog dialog
class CCommunicationDialog : public CDialog
{
public:
	HANDLE Handle;
	CString m_ConnectionString;

protected:
	ACSC_PCI_SLOT PCICards[16];
	int ObtainedCards;

	// UI functions and variables
	void UI_EnableGroup_ACS_CommunicationDlg(BOOL bEnableFlag);
	void UpdateCopleyUI_byConnectFlag();
	void ResetCopleyDriverBaudRate9600();
	void RunScript_DriverRS232();
	void RunScript_ACS_PCI(); // 20121106

	void UpdateELMO_UI_byConnectFlag();
	void UpdateServoTronix_UI_byConnectFlag();
	void InitVarMachineType(int iFlagMachType);
	void FormatBufferAddSendAndReplyFromSerialPort();

	CWinThread* pDlgDriverCommunicationWinThread; // // 20121011

	LONG SerialPortReadReply(DWORD dwTimeout = INFINITE); // INFINITE: -1
// Construction
public:
	CCommunicationDialog( CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CCommunicationDialog)
	enum { IDD = IDD_COMMDIALOG };
	CComboBox	m_comboPCI;
	int		m_CommunicationType;
	int		m_IdxSerPortRate;
	int		m_IdxComboConnectionEthPort;
	CString	m_strAddressEthACS;
	int		m_PCICardNumber;
	//}}AFX_DATA

	void UI_InitSlider_Lighting124();
	void UpdateCommunicationDlgUI_byConnectFlag();
	void OutputCurrChannelLightingByCom(unsigned char ucLightCmdCh);

	void ReadByteFromEdit(int nResId, char *pcValue);
	void ReadDoubleFromEdit(int nResId, double *dValue);
	void ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue);

	void UpdateConnectButtonACS();
	void SetUserInterfaceLanguage();
	char cFlagDlgDriverCommunicationThreadRunning;  // 20121011
	void StartDriverCommunicationThread();
	UINT DlgDriverCommunicationThread();  // 20121011

	void DlgTimerEvent();
	int CCommunicationDialog::DlgGetFlagTimerRunning();
	virtual BOOL ShowWindow(int nCmdShow);
	void DlgUpdateDigitalInputStatusFilling(unsigned int uiStaticId, int iFlagLighting);
	void CCommunicationDialog::UpdateHallSensorUI();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommunicationDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCommunicationDialog)
	afx_msg void OnSerialRadioButton();
	afx_msg void OnEthernetRadioButton();
	afx_msg void OnPCIRadioButton();
	afx_msg void OnSimulRadioButton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void GetCommSettings(COMM_SETTINGS* CommSet);
	afx_msg void OnBnClickedButtunCommConnectSerport();
	afx_msg void OnEnKillfocusEditLightingValueCh();
	afx_msg void OnCbnSelchangeCombSelLightChannel();
	afx_msg void OnBnClickedCheckTestSerportComm();
	afx_msg void OnCbnSelchangeRatecombo();
	afx_msg void OnCbnSelchangePortcombo();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboSelSerportForApp();
	afx_msg void OnBnClickedButtonCommuConnectAcs();
	afx_msg void OnBnClickedButtonCommDlgStartEndGrabImage();
	afx_msg void OnBnClickedButtonCommDlgSaveImage();
	afx_msg void OnNMCustomdrawSliderLightingCh7Cored(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderLightingCh7CoBlue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderLightingCh7SiRed(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderLightingCh7SiBlue(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckCommunicationDlgConnectAcs();
	afx_msg void OnEnKillfocusEditCopleyIScale();
	afx_msg void OnEnKillfocusEditCopleyLimitIPeak();
	afx_msg void OnEnKillfocusEditCopleyLimitIRms();
	afx_msg void OnEnKillfocusEditCopleyLimitI2t();
	afx_msg void OnEnKillfocusEditCopleyGainCP();
	afx_msg void OnEnKillfocusEditCopleyGainCI();
	afx_msg void OnBnClickedButtonCopleySaveFlash();
	afx_msg void OnBnClickedButtonCoplyReadFlash();
	afx_msg void OnEnKillfocusEditElmoIScale();
	afx_msg void OnEnKillfocusEditElmoLimitIPeak();
	afx_msg void OnEnKillfocusEditElmoLimitIRms();
	afx_msg void OnEnKillfocusEditElmoCurrentGainP();
	afx_msg void OnEnKillfocusEditElmoCurrentGainI();
	afx_msg void OnEnKillfocusEditElmoLimitPeakTime();
	afx_msg void OnBnClickedButtonSerialPortSendCommDlg();
	afx_msg void OnEnKillfocusEditServotronixIScale();
	afx_msg void OnEnKillfocusEditServotronixDriverLimitCurrentPeak();
	afx_msg void OnEnKillfocusEditServotronixDriverLimitCurrentConti();
	afx_msg void OnEnKillfocusEditServotronixCurrentGainP();
	afx_msg void OnEnKillfocusEditServotronixCurrentGainI();
	afx_msg void OnEnKillfocusEditServotronixCurrentFf();
	afx_msg void OnEnKillfocusEditServotronixMotorLimitCurrentConti();
	afx_msg void OnEnKillfocusEditServotronixMotorLimitCurrentPeak();
	afx_msg void OnEnKillfocusEditServotronixCurrentKBcakEmf();
	afx_msg void OnBnClickedButtonServotronixEnableDisableDriver();
	afx_msg void OnCbnKillfocusCommuDlgSendCombo();
	afx_msg void OnBnClickedButtonCommudlgLoadScript();
	afx_msg void OnBnClickedButtonCommudlgRunScript();
//	afx_msg void OnBnClickedCheckCommunicationDlgSrvtronicsMfbdir();
	afx_msg void OnEnKillfocusEditCommunicationDlgSrvtronicsMphase();
	afx_msg void OnBnClickedCheckCommunicationDlgSrvtronicsPositionDir();
	afx_msg void OnEnKillfocusEditCommunicationDlgSrvtronicsMfbdir();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMUNICATIONDIALOG_H__C97F394F_4E72_4451_B542_297993FE2BB7__INCLUDED_)
