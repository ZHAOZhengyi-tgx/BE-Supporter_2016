#pragma once


// TimerDlg_TestUSG dialog
#include "resource.h"

class TimerDlg_TestUSG : public CDialog
{
	DECLARE_DYNAMIC(TimerDlg_TestUSG)

public:
	TimerDlg_TestUSG(HANDLE Handle, CWnd* pParent = NULL);   // standard constructor
	virtual ~TimerDlg_TestUSG();
	int DlgWidth, DlgHeight;
	char m_strText[256];
	int m_iCountTimer;
	UINT  StartTimer (UINT TimerDuration);
	BOOL StopTimer (UINT TimerVal);

	HANDLE m_hHandle;
	int m_iX_LeftBottom,    m_iY_LeftBottom,    m_iX_RightTop,  m_iY_RightTop;
	UINT_PTR m_iTimerVal;
	UINT     m_wTimerRes;

	void UpdateLPT_Data();
// Dialog Data
	enum { IDD = IDC_SYS_TIMER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnShowWindow(BOOL bShow, UINT nStatus);
//	virtual void OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT TimerVal);

	afx_msg void OnBnClickedButtonTimerStop();
	afx_msg void OnBnClickedButtonTimerStart();
	afx_msg void OnBnClickedTimerCheckDebugFile();
	afx_msg void OnBnClickedTimerVisibleCounterCheck();
	afx_msg void OnCbnSelchangeTimerUspBlkStartCombo();
	afx_msg void OnBnClickedButtonInitUsp();
	afx_msg void OnBnClickedButtonWarmReset();
	afx_msg void OnBnClickedUsgTimerWriteByteBtn();
	afx_msg void OnBnClickedButtonUsgByIntPci();
	afx_msg void OnBnClickedUsgTriggerAll();
	afx_msg void OnBnClickedButtonPauseUsg();
	afx_msg void OnBnClickedButtonResumeUsg();
	afx_msg void OnBnClickedUsgLptBit7();
	afx_msg void OnBnClickedUsgLptBit6();
	afx_msg void OnBnClickedUsgLptBit5();
	afx_msg void OnBnClickedUsgLptBit4();
	afx_msg void OnBnClickedUsgLptBit3();
	afx_msg void OnBnClickedUsgLptBit2();
	afx_msg void OnBnClickedUsgLptBit1();
	afx_msg void OnBnClickedUsgLptBit0();
	afx_msg void OnBnClickedUsgLptButtonSet();
	afx_msg void OnBnClickedTimerUsgCheckPathByLpt1();
	afx_msg void OnBnClickedButtonUsgDebugTimer();
};
