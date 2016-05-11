#pragma once


// CDlgSysMonitor dialog
#include "resource.h"

class CDlgSysMonitor : public CDialog
{
	DECLARE_DYNAMIC(CDlgSysMonitor)

public:
	CDlgSysMonitor(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSysMonitor();

// Dialog Data
	enum { IDD = IDD_FORM_VIEW_SYS_IO };

	//
	void CDlgSysMonitor::InitUI();
	void CDlgSysMonitor::UpdateServoFeedback();
	int CDlgSysMonitor::GetFlagShowSysMonitor();
	void CDlgSysMonitor::EnableFlagShowSysMonitor();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMovingTestACS();
	afx_msg void OnBnClickedCheckMonitorServoCtrl();
};
