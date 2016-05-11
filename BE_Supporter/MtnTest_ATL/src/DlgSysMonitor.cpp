// DlgSysMonitor.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSysMonitor.h"

static int iEnableServoMonitor = 1;
static int iShowSysMonitor;

CDialog *pDlgParent;

// CDlgSysMonitor dialog
IMPLEMENT_DYNAMIC(CDlgSysMonitor, CDialog)

CDlgSysMonitor::CDlgSysMonitor(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSysMonitor::IDD, pParent)
{
	pDlgParent = (CDialog*)pParent;
}

CDlgSysMonitor::~CDlgSysMonitor()
{
}

void CDlgSysMonitor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSysMonitor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CDlgSysMonitor::OnBnClickedButtonMovingTestACS)
	ON_BN_CLICKED(IDC_CHECK_MONITOR_SERVO_CTRL, &CDlgSysMonitor::OnBnClickedCheckMonitorServoCtrl)
END_MESSAGE_MAP()

int CDlgSysMonitor::GetFlagShowSysMonitor()
{
	return iShowSysMonitor;
}

void CDlgSysMonitor::EnableFlagShowSysMonitor()
{
	iShowSysMonitor = 1;
//	InitUI();
}

#include "acs_buff_prog.h"   // 20120902
// CDlgSysMonitor message handlers
void CDlgSysMonitor::OnBnClickedButtonMovingTestACS()
{
	// Terminator all monitoring process
	// 1. ACS
	// 2. IO
	// 3. HostPC: Memory, ETH, LPT, SERPORT, PCI
	// 4. STEPPER
	// 5. DSP board
	// 6. 1394 Vision
//	pDlgParent->ShowWindow(1);

	iShowSysMonitor = 0;
	acs_disable_check_rms_flag();


	CDialog::OnCancel();
}

// IDC_CHECK_MONITOR_SERVO_CTRL
void CDlgSysMonitor::OnBnClickedCheckMonitorServoCtrl()
{
	iEnableServoMonitor = ((CButton*) GetDlgItem(IDC_CHECK_MONITOR_SERVO_CTRL))->GetCheck();
}

#include "MotAlgo_Dll.h"
#include "MtnWbDef.h"
// IDC_STATIC_SYS_MONITOR_ACS_AXIS_0
// IDC_STATIC_SYS_MONITOR_ACS_AXIS_1
// IDC_STATIC_SYS_MONITOR_ACS_AXIS_4
// IDC_STATIC_SYS_MONITOR_ACS_AXIS_5
void CDlgSysMonitor::InitUI()
{
	((CButton*) GetDlgItem(IDC_CHECK_MONITOR_SERVO_CTRL))->SetCheck(iEnableServoMonitor);
	int iMachType = get_sys_machine_type_flag();
	if(iMachType == WB_MACH_TYPE_HORI_LED 
		|| iMachType == BE_WB_HORI_20T_LED  // 20120826
		|| iMachType  == WB_STATION_XY_TOP
		|| iMachType  == BE_WB_ONE_TRACK_18V_LED)
	{
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_0)->SetWindowTextA(_T("Tbl.Y (ACS-0)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_1)->SetWindowTextA(_T("Tbl.X (ACS-1)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_4)->SetWindowTextA(_T("Bnd.Z (ACS-4)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_5)->SetWindowTextA(_T("BH-WC (ACS-5)"));
	}
	else  // if (get_sys_machine_type_flag() )  // default
	{
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_0)->SetWindowTextA(_T("Tbl.X (ACS-0)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_1)->SetWindowTextA(_T("Tbl.Y (ACS-1)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_4)->SetWindowTextA(_T("Bnd.Z (ACS-4)"));
		GetDlgItem(IDC_STATIC_SYS_MONITOR_ACS_AXIS_5)->SetWindowTextA(_T("BH-WC (ACS-5)"));
	}
	acs_enable_check_rms_flag(); // 20120902
// WB_MACH_TYPE_VLED_FORK
// WB_MACH_TYPE_ONE_TRACK_13V_LED )
}
// IDC_STATIC_MONITOR_RMS_TBL_X
// IDC_STATIC_MONITOR_RMS_TBL_Y
// IDC_STATIC_MONITOR_RMS_BND_Z
// IDC_STATIC_MONITOR_RMS_BND_WC
// IDC_STATIC_MONITOR_POSN_ERR_TBL_X
// IDC_STATIC_MONITOR_POSN_ERR_TBL_Y
// IDC_STATIC_MONITOR_POSN_ERR_BND_Z
// IDC_STATIC_MONITOR_POSN_ERR_BND_WC

double adServoCurrentRMS[4], adServoProtLmtRMS[4];
double adServoPositionErr[4], adServo_ProtLmtPosnErr[4];
#include "acs_buff_prog.h"

extern COMM_SETTINGS stServoControllerCommSet;
void CDlgSysMonitor::UpdateServoFeedback()
{
	CString cstrTemp;
	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		acs_read_current_rms_array_wb13v_cfg(adServoCurrentRMS);

		acs_read_current_rms_prot_lmt_array_wb13v_cfg(adServoProtLmtRMS);

		if(iEnableServoMonitor)
		{
			cstrTemp.Format("%4.1f <%4.0f", adServoCurrentRMS[0], adServoProtLmtRMS[0]);
			GetDlgItem(IDC_STATIC_MONITOR_RMS_TBL_X)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoCurrentRMS[1], adServoProtLmtRMS[1]);
			GetDlgItem(IDC_STATIC_MONITOR_RMS_TBL_Y)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoCurrentRMS[2], adServoProtLmtRMS[2]);
			GetDlgItem(IDC_STATIC_MONITOR_RMS_BND_Z)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoCurrentRMS[3], adServoProtLmtRMS[3]);
			GetDlgItem(IDC_STATIC_MONITOR_RMS_BND_WC)->SetWindowTextA(cstrTemp);
		}

		acs_read_current_posn_err_array_wb13v_cfg(adServoPositionErr);

		acs_read_current_pe_prot_lmt_array_wb13v_cfg(adServo_ProtLmtPosnErr);

		if(iEnableServoMonitor)
		{
			cstrTemp.Format("%4.1f <%4.0f", adServoPositionErr[0], adServo_ProtLmtPosnErr[0]);
			GetDlgItem(IDC_STATIC_MONITOR_POSN_ERR_TBL_X)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoPositionErr[1], adServo_ProtLmtPosnErr[1]);
			GetDlgItem(IDC_STATIC_MONITOR_POSN_ERR_TBL_Y)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoPositionErr[2], adServo_ProtLmtPosnErr[2]);
			GetDlgItem(IDC_STATIC_MONITOR_POSN_ERR_BND_Z)->SetWindowTextA(cstrTemp);
			cstrTemp.Format("%4.1f <%4.0f", adServoPositionErr[3], adServo_ProtLmtPosnErr[3]);
			GetDlgItem(IDC_STATIC_MONITOR_POSN_ERR_BND_WC)->SetWindowTextA(cstrTemp);
		}

double dLeftLimitPosn;
double dRightLimitPosn;
		if(iEnableServoMonitor)
		{
			// IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_X
			mtn_api_get_position_upp_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_X, &dRightLimitPosn);
			mtn_api_get_position_low_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_X, &dLeftLimitPosn);
			cstrTemp.Format("[%4.1E, %4.1E]", dLeftLimitPosn, dRightLimitPosn);
			GetDlgItem(IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_X)->SetWindowTextA(cstrTemp);
			// IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_Y
			mtn_api_get_position_upp_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_Y, &dRightLimitPosn);
			mtn_api_get_position_low_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_Y, &dLeftLimitPosn);
			cstrTemp.Format("[%4.1E, %4.1E]", dLeftLimitPosn, dRightLimitPosn);
			GetDlgItem(IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_Y)->SetWindowTextA(cstrTemp);

			// IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_A
			mtn_api_get_position_upp_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_A, &dRightLimitPosn);
			mtn_api_get_position_low_lmt(stServoControllerCommSet.Handle, ACSC_AXIS_A, &dLeftLimitPosn);
			cstrTemp.Format("[%4.1E, %4.1E]", dLeftLimitPosn, dRightLimitPosn);
			GetDlgItem(IDC_STATIC_MONITOR_SOFTWARE_LIMIT_ACS_A)->SetWindowTextA(cstrTemp);
		}
// double dPeakDrvCmd[]
		// IDC_STATIC_MONITOR_DRV_CMD_ACS_X
		// IDC_STATIC_MONITOR_DRV_CMD_ACS_Y
		// IDC_STATIC_MONITOR_DRV_CMD_ACS_A
	}

}