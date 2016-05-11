// MtnTesterEntry.cpp : implementation file
//

#include "stdafx.h"
#include "MtnTesterEntry.h"
//#include "MtnDialog_FbMonitor.h"
#include "CommunicationDialog.h"
#include "MtnTest_Move.h"
#include "TimerDlg_TestUSG.h"
#include "AcsServo.h" //"ServoController.h"
#include "MtnInitAcs.h"
#include "MtnTesterResDef.h"
#include "DigitalIO.h"
#include "WinTiming.h"
#include "DlgServoTuning.h"
#include "DlgKeyInputPad.h"

// #define __ROYA_TEST_STATION__
static char cSystemDebugLevel = __SYSTEM_PROTECTING_OPERATOR__;


static int iFlagCleanEnvironmentOnExit ;
void sys_set_flag_clean_env_on_exit(int iFlagClean)
{
	iFlagCleanEnvironmentOnExit = iFlagClean;
}


static int iFlagSysDebugMode;
void sys_set_flag_debug_mode(int iFlagMode)
{
	iFlagSysDebugMode = iFlagMode;
//	iFlagEnableEditEncResolution = iFlagMode;
}
int get_sys_flag_debug_mode()
{
	return iFlagSysDebugMode;
}


// 20110013
extern char *astrMachineTypeNameLabel_en[];
extern char *astrMachineTypeNameLabel_cn[];

//double dSampleTime_ms[] = {1.0};
CString cstrVersion;
unsigned int Ver;

IMPLEMENT_DYNAMIC(MtnTesterEntry, CDialog)

extern COMM_SETTINGS stServoControllerCommSet;

MtnTesterEntry::MtnTesterEntry(CWnd* pParent /*=NULL*/)
	: CDialog(MtnTesterEntry::IDD, pParent)
{
	sys_set_comm_default();
	// When starting the TerminalButton is disabled by default
//	GetDlgItem(IDC_SYS_TERMINAL_ACS)->EnableWindow(FALSE);
}

MtnTesterEntry::~MtnTesterEntry()
{

}

void MtnTesterEntry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(MtnTesterEntry, CDialog)
	ON_BN_CLICKED(IDC_SYS_COMM_SET, &MtnTesterEntry::OnBnClickedSysCommSet)
	ON_BN_CLICKED(IDC_SYS_TERMINAL_ACS, &MtnTesterEntry::OnBnClickedSysTerminal)
	ON_BN_CLICKED(IDCLOSE, &MtnTesterEntry::OnBnClickedClose)
	ON_BN_CLICKED(IDC_SYS_FB_MONITOR, &MtnTesterEntry::OnBnClickedSysFbMonitor)
	ON_BN_CLICKED(IDC_SYS_MOVING_TEST, &MtnTesterEntry::OnBnClickedButtonMovingTestACS)
	ON_BN_CLICKED(IDC_SYS_SCOPE_SETUP, &MtnTesterEntry::OnBnClickedSysScopeSetup)
	ON_BN_CLICKED(IDC_SYS_POSN_TUNE1, &MtnTesterEntry::OnBnClickedSysPosnTune1)
	ON_BN_CLICKED(IDC_TUNE_PARAMETER_INIT_SERVO, &MtnTesterEntry::OnBnClickedTuningServoPara)
	ON_BN_CLICKED(IDC_SYS_TIMER, &MtnTesterEntry::OnBnClickedSysTimer)
	ON_BN_CLICKED(IDC_SYS_STEPPER, &MtnTesterEntry::OnBnClickedSysStepper)
	ON_BN_CLICKED(IDC_SYS_PROFILE_CALC, &MtnTesterEntry::OnBnClickedSysProfileCalc)
	ON_BN_CLICKED(IDC_SYS_SPECTRUM_TEST, &MtnTesterEntry::OnBnClickedSysSpectrumTest)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUT_CHANGE_LANGUAGE, &MtnTesterEntry::OnBnClicked_Toggle_ChangeLanguage)
	ON_BN_CLICKED(IDC_SYS_ROYA_DSP_TEST, &MtnTesterEntry::OnBnClickedSysRoyaDspTest)
	ON_BN_CLICKED(IDC_SYS_MONITOR, &MtnTesterEntry::OnBnClickedSysMonitor)
	ON_BN_CLICKED(IDC_SERVO_ADJUSTMENT, &MtnTesterEntry::OnBnClickedServoAdjustment)
	ON_BN_CLICKED(IDC_CHECK_SYS_ENTRY_OFF_SERVO, &MtnTesterEntry::OnBnClickedCheckSysEntryOffServo)
	ON_BN_CLICKED(IDC_CHECK_SYS_CLEAN_ON_EXIT, &MtnTesterEntry::OnBnClickedCheckSysCleanOnExit)
	ON_CBN_SELCHANGE(IDC_COMBO_SYS_MACHINE_TYPE, &MtnTesterEntry::OnCbnSelchangeComboSysMachineType)
//	ON_BN_CLICKED(IDC_SYS_STEPPER_CTRL_PCI_8144, &MtnTesterEntry::OnBnClickedSysStepperCtrlPci8144)
ON_BN_CLICKED(IDC_CHECK_ENABLE_EDITING_MACH, &MtnTesterEntry::OnBnClickedCheckEnableEditingMach)
ON_EN_KILLFOCUS(IDC_EDIT_SYS_MACH_SN, &MtnTesterEntry::OnEnKillfocusEditSysMachSn)
END_MESSAGE_MAP()

// MtnTesterEntry message handlers
char strSystemPath[128];
char strFileName[128];
extern void InitWireBondServoAxisName(); // 

void mtest_fb_init_search_limit_home_parameter_vled_bonder();
void mtest_fb_init_search_limit_home_parameter_hori_bonder();
void mtest_fb_init_search_limit_home_parameter_13v_bonder();

#include "MotAlgo_DLL.h"

static int iFlagEnableEditMachineSn;
void MtnTesterEntry::InitButtonUI()
{
		// 
//	char tempSystemPath[256];

	CString cstrTemp;
	cstrTemp.Format("%d", get_sys_machine_serial_num());
	GetDlgItem(IDC_EDIT_SYS_MACH_SN)->SetWindowTextA(cstrTemp);
	GetDlgItem(IDC_EDIT_SYS_MACH_SN)->EnableWindow(iFlagEnableEditMachineSn);
	// IDC_CHECK_ENABLE_EDITING_MACH
	((CButton *)GetDlgItem(IDC_CHECK_ENABLE_EDITING_MACH))->SetCheck(iFlagEnableEditMachineSn);

	Ver = acsc_GetLibraryVersion();

	VERSION_INFO stMotAlgoVersion;
	mtnapi_dll_init_get_version(&stMotAlgoVersion);
	cstrVersion.Format("MkDate-%s, Time-%s; ACS: %d.%d.%d.%d, Ts:%3.1f ms, MtnAlgo:%d.%d,%d-%d-%d", 
//		strFileName,
		__DATE__,__TIME__,
		HIBYTE(HIWORD(Ver)), LOBYTE(HIWORD(Ver)), HIBYTE(LOWORD(Ver)), LOBYTE(LOWORD(Ver)), sys_get_controller_ts(),  // 20121203
		stMotAlgoVersion.usVerMajor, stMotAlgoVersion.usVerMinor, 
		stMotAlgoVersion.usVerYear, stMotAlgoVersion.usVerMonth, stMotAlgoVersion.usVerDate);
	GetDlgItem(IDC_VERSION_LABEL)->SetWindowText(cstrVersion);

//	CMenu cmenuMain;
//	cmenuMain.LoadMenuA(IDR_SYS_TUNING_MENU);
//	cmenuMain.CreateMenu();
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_BUT_CHANGE_LANGUAGE)->SetWindowTextA(_T("中"));
	}
	else
	{
		GetDlgItem(IDC_BUT_CHANGE_LANGUAGE)->SetWindowTextA(_T("En"));
	}
	((CButton *)GetDlgItem(IDC_CHECK_SYS_CLEAN_ON_EXIT))->SetCheck(iFlagCleanEnvironmentOnExit);

#ifdef __ROYA_TEST_STATION__
	GetDlgItem(IDC_SYS_SPECTRUM_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TERMINAL_ACS_ACS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_FB_MONITOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_MOVING_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_SCOPE_SETUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_POSN_TUNE1)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TIMER_DLG)->EnableWindow(FALSE);
	GetDlgItem(IDC_TUNE_PARAMETER_INIT_SERVO)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_COMM_SET)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_STEPPER)->EnableWindow(FALSE);
#endif

//	GetDlgItem(IDC_SYS_PROFILE_CALC)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_SCOPE_SETUP)->EnableWindow(FALSE);
//	GetDlgItem(IDC_SYS_MONITOR)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_SCOPE_SETUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TERMINAL_ACS)->EnableWindow(FALSE);
}

// Initialization
#include "MtnTune.h"
#include "acs_buff_prog.h"
#include "MtnAft.h"

extern void mtn_api_prompt_acs_error_code(HANDLE stCommHandleAcs);  // 20110803
extern void move_test_random_burn_in_variables();
#include "_Digital_IO_Ad7248.h"  // 20120823

BOOL MtnTesterEntry::OnInitDialog()
{

HINSTANCE gLiDLL = NULL;
    gLiDLL=LoadLibrary("ServoTuningWB.dll");//加载DLL

	// Initialize WinTiming Module
	init_win_timing();

    mtnapi_dll_init_all();       // Variable and memory in dll
	mtnapi_static_lib_init_all(); // Some variable in static lib

//  be done after initialize Enc resolution  20111010
	aft_init_spec();

	mtn_tune_init_position_compensation_condition();

	// Machine config has been set in mtnapi_dll_init_all
	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_MACH_TYPE_VLED_FORK)  // iFlagSysMachineType // machine type dependency Item-5
	{
		set_sys_language_option(LANGUAGE_UI_CN);
		mtest_fb_init_search_limit_home_parameter_vled_bonder();
	}
	else if(iFlagMachType == WB_MACH_TYPE_HORI_LED || iFlagMachType == WB_STATION_XY_TOP
		|| iFlagMachType == BE_WB_ONE_TRACK_18V_LED || iFlagMachType == BE_WB_HORI_20T_LED)  // 20120826
	{
		set_sys_language_option(LANGUAGE_UI_CN);
		mtest_fb_init_search_limit_home_parameter_hori_bonder();
	}
	else if(iFlagMachType == WB_STATION_EFO_BSD 
		|| 	iFlagMachType == WB_STATION_XY_VERTICAL 
		|| 	iFlagMachType == WB_STATION_XY_TOP
		|| 	iFlagMachType == WB_STATION_BH
		|| 	iFlagMachType == WB_STATION_WIRE_CLAMP		)
	{
		set_sys_language_option(LANGUAGE_UI_EN);
	}
	else  // Default safest 1 Cup
	{
		set_sys_language_option(LANGUAGE_UI_CN);
		mtest_fb_init_search_limit_home_parameter_13v_bonder();
	}
	OnBnClicked_Toggle_ChangeLanguage();


	// 2. Machine Configuration
	stServoControllerCommSet.Handle = ACSC_INVALID;  // 20120318
	if(iFlagMachType != WB_STATION_EFO_BSD && iFlagMachType != WB_STATION_USG_LIGHTING_PR)
	{
		if(sys_init_acs_communication() == MTN_API_OK_ZERO)
		{
			if(theAcsServo.GetServoOperationMode() != OFFLINE_MODE)
			{
				mtn_api_prompt_acs_error_code(stServoControllerCommSet.Handle);  // 20110803
			}
			mtn_api_clear_acs_buffer_prof();  // Each time stop and clean the buffer program, before downloading
			Sleep(200);
			OnCommunicationConnected();
			// Buffer Program
			acs_buff_prog_init_comm(stServoControllerCommSet.Handle);
			mtn_api_init_acs_buffer_prog();

		}
		else
		{
			CommunicationFailure();
		}
	}
	else
	{
		CommunicationFailure();
	}

	InitButtonUI();
	UpdateComboBoxMachineType();
    // Random Burn-in variables
	move_test_random_burn_in_variables();

	// Digital IO		// Init PCI Cards for Digital IO
	theDigitalIO.InitializeIoCard();
	DigitalIO_7248_Registration(); // 20120823

	StartTimer(500); // 500 ms timer

	// Automation
	if(iFlagMachType == WB_STATION_EFO_BSD)
	{
		OnBnClickedSysFbMonitor();
		Sleep(1000);
		OnBnClickedSysRoyaDspTest();  // 20110822
		Sleep(1000);
	}
	else if(iFlagMachType == WB_STATION_USG_LIGHTING_PR)
	{
		OnBnClickedSysFbMonitor();
		Sleep(1000);
		OnBnClickedSysCommSet();
	}
	else if(iFlagMachType == WB_STATION_XY_VERTICAL ||
		iFlagMachType == WB_STATION_XY_TOP ||
		iFlagMachType == WB_STATION_BH)
	{
		OnBnClickedServoAdjustment();  // 20110822
	}

	UI_InitSubDialogs();

	return CDialog::OnInitDialog(); 

}

#include "DlgSysMonitor.h"
CDlgSysMonitor *pDlgSysMonitor;
// IDD_FORM_VIEW_SYS_IO
void MtnTesterEntry::OnBnClickedSysMonitor()
{
	pDlgSysMonitor->InitUI();
	pDlgSysMonitor->ShowWindow(1);
	pDlgSysMonitor->EnableFlagShowSysMonitor();
}

#include "DlgServoTuning.h"
extern CDlgServoTuning *pDlgServoCtrlTuning;
// IDC_SERVO_ADJUSTMENT
void MtnTesterEntry::OnBnClickedServoAdjustment()
{

	if(pDlgServoCtrlTuning == NULL)
	{
		pDlgServoCtrlTuning = new  CDlgServoTuning();
		pDlgServoCtrlTuning->Create(IDD_DIALOG_SERVO_CONTROL_TUNING);
	}

    pDlgServoCtrlTuning->ShowWindow(1);
	pDlgServoCtrlTuning->RestartTimer();
}

#include "MtnStepperDlg.h"
CMtnStepperDlg *cpTestStepperDlg;

#include "MtnVelStepTestDlg.h"
CMtnVelStepTestDlg *cpVelStepGroupTest;

#include "MtnSpectrumTestDlg.h"
CMtnSpectrumTestDlg *cpTestSpectrum;

#include "MtnTest_Move.h"
CMtnTest_Move *cpMovingTest;

#include "DlgTestRoyaBoard.h"
CDlgTestRoyaBoard * cpTestRoyaBd;

#include "MtnParameterDlg.h"
MtnParameterDlg *cpParameterDlg;

#include "MtnPosnCompensDlg.h"
CMtnPosnCompensDlg *cpMtnTune1;

void MtnTesterEntry::UI_InitSubDialogs()
{
	pDlgSysMonitor = new CDlgSysMonitor(this);
	pDlgSysMonitor->Create(IDD_FORM_VIEW_SYS_IO);

//	cpTestStepperDlg = new CMtnStepperDlg;
//	cpTestStepperDlg->Create(IDD_STEPPER_TEST_DLG);

	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType != WB_STATION_EFO_BSD)  // machine type dependency Item-6
	{
		if(cpVelStepGroupTest == NULL)
		{
			cpVelStepGroupTest = new CMtnVelStepTestDlg();
			cpVelStepGroupTest->Create(IDD_DLG_GROUP_VEL_STEP_TEST);
		}

		if(cpTestSpectrum == NULL)
		{
			cpTestSpectrum = new CMtnSpectrumTestDlg;
			cpTestSpectrum->Create(IDD_MTN_SPECTRUM_TEST_DLG);
		}
	}
}

static UINT_PTR iTimerIdTestEntry;

#include <time.h>
struct tm stTime;
struct tm *stpTimeGlobalVar = &stTime;
static __time64_t stLongTime;
static int iFlagTriggerAcsCommuConnectedOnce = 1;
static int iFlagTriggerAcsCommuFailureOnce = 1;
void mtntestdlg_entry_reset_flag_trigger_acs_comm()
{
	iFlagTriggerAcsCommuConnectedOnce = 1;
	iFlagTriggerAcsCommuFailureOnce = 1;
}

#include "ACSCTerm.h"
CCommunicationDialog *pDlgCommunicationSetup;

void MtnTesterEntry::OnTimer(UINT nIDEvent)
{
	// IDC_LOCAL_TIME_LABEL
	_time64(&stLongTime);
//	stpTimeGlobalVar = _localtime64( &stLongTime);
	_localtime64_s(stpTimeGlobalVar, &stLongTime);

static	char strTimeDate[128];
	CString cstrTemp;
	asctime_s(strTimeDate, 128, stpTimeGlobalVar);
	cstrTemp = _T(strTimeDate);
	GetDlgItem(IDC_LOCAL_TIME_LABEL)->SetWindowTextA(cstrTemp);

	if (stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		if(iFlagTriggerAcsCommuConnectedOnce == 1)
		{
			OnCommunicationConnected();
			iFlagTriggerAcsCommuConnectedOnce = 0;
		}
	}
	else
	{
		if(iFlagTriggerAcsCommuFailureOnce == 1)
		{
			iFlagTriggerAcsCommuFailureOnce = 0;
		}
		CommunicationFailure();
	}
	if(pDlgSysMonitor->GetFlagShowSysMonitor() == TRUE)
	{
		pDlgSysMonitor->UpdateServoFeedback();
	}
	if(cpTestSpectrum != NULL)
	{
		if(cpTestSpectrum->UI_GetShowWindowFlag() == 1 )
		{
			cpTestSpectrum->Update_DlgUI();
		}
	}
	if(cpMovingTest != NULL)
	{
		if(cpMovingTest->UI_GetShowWindowFlag() == TRUE)
		{
			cpMovingTest->DlgTimerEvent();
		}
	}
	if(cpTestRoyaBd != NULL)
	{
		if(cpTestRoyaBd->UI_GetShowWindowFlag() == TRUE)
		{
			cpTestRoyaBd->DlgTimerEvent();
		}
	}
	if(cpParameterDlg != NULL)
	{
		if(cpParameterDlg->UI_GetShowWindowFlag() == TRUE)
		{
			cpParameterDlg->UpdateUI_ByTimer();
		}
	}
	if(cpVelStepGroupTest != NULL)
	{
		if(cpVelStepGroupTest->DlgGetFlagTimerRunning() == TRUE)
		{
			cpVelStepGroupTest->DlgTimerEvent();
		}
	}
	if(cpTestStepperDlg != NULL)
	{
		if(cpTestStepperDlg->DlgGetFlagTimerRunning() == TRUE)
		{
			cpTestStepperDlg->DlgTimerEvent();
		}
	}
	if(pDlgCommunicationSetup != NULL)
	{
		if(pDlgCommunicationSetup->DlgGetFlagTimerRunning() == TRUE)
		{
			pDlgCommunicationSetup->DlgTimerEvent();
		}
	}
	if(cpMtnTune1 != NULL)
	{
		if(cpMtnTune1->UI_GetShowWindowFlag() == TRUE)
		{
			cpMtnTune1->DlgTimerEvent();
		}
	}
}

UINT MtnTesterEntry::StartTimer(UINT TimerDuration)
{
	iTimerIdTestEntry = SetTimer(IDT_TIMER_SYS_ENTRY, TimerDuration, 0);
	
	if (iTimerIdTestEntry == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}

    return (UINT_PTR)iTimerIdTestEntry;
}// end StartTimer

BOOL MtnTesterEntry::StopTimer()
{
	if (!KillTimer (iTimerIdTestEntry))
	{
		return FALSE;
	}
	return TRUE;
}

void MtnTesterEntry::OnClose()
{
}
void MtnTesterEntry::OnCancel()
{
//	OnClose();
	//acsc_CloseHistoryBuffer(stServoControllerCommSet.Handle);
	if (stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		mtn_api_clear_acs_buffer_prof();
		acsc_CloseComm(stServoControllerCommSet.Handle);
	}
	StopTimer();
	exit_win_timing();
	if(iFlagCleanEnvironmentOnExit)
	{
		static char strSystemCommand[128];
		sprintf_s(strSystemCommand, 128, "del D:\\MT\\*.txt");
		system(strSystemCommand);
		Sleep(500);
		sprintf_s(strSystemCommand, 128, "del D:\\MT\\*.m");
		system(strSystemCommand);
		Sleep(500);
	}
	DeleteAllDialogResource();
	CDialog::OnCancel();
}
void MtnTesterEntry::EnableProtectButtons()
{
	GetDlgItem(IDC_SYS_PROFILE_CALC)->EnableWindow(TRUE);
}
void MtnTesterEntry::OnCommunicationConnected()
{
	CString strCommStatus("OK");
	GetDlgItem(IDC_COMM_STATUS)->SetWindowText(strCommStatus);
	GetDlgItem(IDC_SYS_SPECTRUM_TEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYS_FB_MONITOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYS_MOVING_TEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYS_TIMER_DLG)->EnableWindow(TRUE);
	GetDlgItem(IDC_TUNE_PARAMETER_INIT_SERVO)->EnableWindow(TRUE);
	GetDlgItem(IDC_SERVO_ADJUSTMENT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_SYS_ENTRY_OFF_SERVO)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYS_POSN_TUNE1)->EnableWindow(TRUE);
//	GetDlgItem(IDC_SYS_SCOPE_SETUP)->EnableWindow(TRUE);
	GetDlgItem(IDC_SYS_MONITOR)->EnableWindow(TRUE);

	GetDlgItem(IDC_SYS_TERMINAL_ACS)->EnableWindow(TRUE);

	mtnscope_declare_var_on_controller(stServoControllerCommSet.Handle);
//	acsc_ReadReal(stServoControllerCommSet.Handle, ACSC_NONE, "CTIME", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, dSampleTime_ms, NULL);
	cstrVersion.Format("%s: BuildDate-%s, Time-%s; SPii+C: %d.%d.%d.%d, Ts:%5.2f ms", 
		strFileName,
		__DATE__,__TIME__,
		HIBYTE(HIWORD(Ver)), LOBYTE(HIWORD(Ver)), HIBYTE(LOWORD(Ver)), LOBYTE(LOWORD(Ver)), sys_get_controller_ts() ); // 20121203
//	cstrVersion.Format("Version: SPii+C: %d.%d.%d.%d, Ts:%5.2f ms", HIBYTE(HIWORD(Ver)), LOBYTE(HIWORD(Ver)), HIBYTE(LOWORD(Ver)), LOBYTE(LOWORD(Ver)), *dSampleTime_ms);
	GetDlgItem(IDC_VERSION_LABEL)->SetWindowText(cstrVersion);

//	ReleaseDlgPointer_ACS();
}

void MtnTesterEntry::CommunicationFailure()
{
	GetDlgItem(IDC_SYS_SPECTRUM_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TERMINAL_ACS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_MOVING_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_SCOPE_SETUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_POSN_TUNE1)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TIMER_DLG)->EnableWindow(FALSE);
//	GetDlgItem(IDC_TUNE_PARAMETER_INIT_SERVO)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_SYS_ENTRY_OFF_SERVO)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_TERMINAL_ACS)->EnableWindow(FALSE);
	GetDlgItem(IDC_SYS_MONITOR)->EnableWindow(FALSE); // 20120116
//	GetDlgItem(IDC_SYS_PROFILE_CALC)->EnableWindow(FALSE);

//	GetDlgItem(IDC_SYS_FB_MONITOR)->EnableWindow(FALSE);
//	GetDlgItem(IDC_SERVO_ADJUSTMENT)->EnableWindow(FALSE);

}

void MtnTesterEntry::OnBnClickedClose()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

#include "MtnDialog_FbMonitor.h"
static MtnDialog_FbMonitor *cpDlgMonitorFeedback;
extern  MtnDialog_FbMonitor *cpRefFromBsdDlgMonitorFeedback;

void MtnTesterEntry::OnBnClickedSysFbMonitor()
{
	if(cpDlgMonitorFeedback == NULL)
	{
	    cpDlgMonitorFeedback = new  MtnDialog_FbMonitor;
		cpDlgMonitorFeedback->Create(IDD_FB_MONITOR_DIALOG);
		cpRefFromBsdDlgMonitorFeedback = cpDlgMonitorFeedback;
	}    
    cpDlgMonitorFeedback->ShowWindow(1);
}

// IDC_TUNE_PARAMETER_INIT_SERVO
void MtnTesterEntry::OnBnClickedTuningServoPara()
{
	if(cpParameterDlg == NULL)
	{
		cpParameterDlg = new MtnParameterDlg();
		cpParameterDlg->Create(IDD_PARAMETER_DLG);
	}
	cpParameterDlg->ShowWindow(1);
}
// 
void MtnTesterEntry::OnBnClickedButtonMovingTestACS()
{
	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
	if(cDlgInputPad.DoModal() == IDOK) //  20120906
	{
		if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_sg_8d_zzy())
		{
			if(cpMovingTest == NULL)
			{
				cpMovingTest = new CMtnTest_Move;
				cpMovingTest->Create(IDD_MOTION_TEST_DIALOG);
			}
			cpMovingTest->ShowWindow(1);
		}
	}
}

void MtnTesterEntry::OnBnClickedSysCommSet()
{
	if(pDlgCommunicationSetup == NULL)
	{
		pDlgCommunicationSetup = new CCommunicationDialog(this);
		pDlgCommunicationSetup->Create(IDD_COMMDIALOG);
	}
    pDlgCommunicationSetup->ShowWindow(1);

}

#include "ACSCTermDlg.h"
static CACSCTermDlg *stpDlgTerminal;
void MtnTesterEntry::OnBnClickedSysTerminal()
{
	//if(stpDlgTerminal != NULL)
	//{
	//	delete stpDlgTerminal;
	//}
	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
	if(cDlgInputPad.DoModal() == IDOK)
	{
		if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_sg_8d_zzy())
		{
			if(stpDlgTerminal == NULL)
			{
				stpDlgTerminal = new CACSCTermDlg(stServoControllerCommSet);
				stpDlgTerminal->Create(IDD_ACSCTERM_DIALOG);
			}
			stpDlgTerminal->ShowWindow(1);
		}
	}
}


#include "MtnScopeDlg.h"
CMtnScope *cpMtnScope;
// IDD_SCOPE_SETUP_DIALOG
void MtnTesterEntry::OnBnClickedSysScopeSetup()
{
	// TODO: Add your control notification handler code here
	if(cpMtnScope != NULL)
	{
		delete cpMtnScope;
	}
	cpMtnScope = new CMtnScope(&gstSystemScope, stServoControllerCommSet.Handle);
    cpMtnScope->Create(IDD_SCOPE_SETUP_DIALOG);
    cpMtnScope->ShowWindow(1);
}

extern COMM_SETTINGS stServoControllerCommSet;

void MtnTesterEntry::OnBnClickedSysPosnTune1()
{

	if(cpMtnTune1 == NULL)
	{
		cpMtnTune1 = new CMtnPosnCompensDlg(stServoControllerCommSet.Handle);
		cpMtnTune1->Create(IDD_POSN_COMPENSATION_DIALOG);
	}

	if(theAcsServo.GetServoOperationMode() != OFFLINE_MODE
		&& cSystemDebugLevel != __SYSTEM_PROTECTING_ENGINEER__)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_sg_8d_zzy())
			{
				set_sys_pass_protect_level(__SYSTEM_PROTECTING_ENGINEER__);
				cpMtnTune1->ShowWindow(1);
			}
		}
	}
	else
	{
		cpMtnTune1->ShowWindow(1);
	}
}

void MtnTesterEntry::OnBnClickedSysTimer()
{
	// IDC_SYS_TIMER_DLG
	TimerDlg_TestUSG *cpTimerDialog = new TimerDlg_TestUSG(stServoControllerCommSet.Handle);
	cpTimerDialog->Create(IDC_SYS_TIMER_DLG);
	cpTimerDialog->ShowWindow(1);
	
}

// IDC_SYS_STEPPER
void MtnTesterEntry::OnBnClickedSysStepper()
{
	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			// wb_mtn_tester_get_password_sg_8d_zzy()
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_gxc())
			{
#ifndef NO_IO_PCI_CARD
				if(cpTestStepperDlg == NULL)
				{
					cpTestStepperDlg = new CMtnStepperDlg;
					cpTestStepperDlg->Create(IDD_STEPPER_TEST_DLG);
				}
				cpTestStepperDlg->ShowWindow(1);
#endif // NO_IO_PCI_CARD
			}
		}
}

#include "ProfCalcDlg.h"
static CProfCalcDlg *cpTestProfCalculator;
void MtnTesterEntry::OnBnClickedSysProfileCalc()
{
	if(cpTestProfCalculator == NULL)
	{
		cpTestProfCalculator = new CProfCalcDlg;
		cpTestProfCalculator->Create(IDD_PROFILE_CALCULATOR_DLG);
	}

	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
	if(stServoControllerCommSet.Handle == ACSC_INVALID)
	{
		cpTestProfCalculator->ShowWindow(1);
	}
	else
	{
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_sg_8d_zzy())
			{
				cpTestProfCalculator->ShowWindow(1);
			}
		}
	}
}

void MtnTesterEntry::OnBnClickedSysRoyaDspTest()
{
#ifdef  __PROT_PASSWORD__
	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
	if(cDlgInputPad.DoModal() == IDOK)
	{
		if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_gxc())
		{
#endif  // __PROT_PASSWORD__
			// IDC_SYS_ROYA_DSP_TEST
			if(cpTestRoyaBd == NULL)
			{
				cpTestRoyaBd = new CDlgTestRoyaBoard;
				cpTestRoyaBd->Create(IDD_DLG_TEST_ROYA_BOARD);
			}
			cpTestRoyaBd->ShowWindow(1);
#ifdef  __PROT_PASSWORD__
		}
	}
#endif  // __PROT_PASSWORD__

}

void MtnTesterEntry::OnBnClickedSysSpectrumTest()
{
	// IDC_SYS_SPECTRUM_TEST
	if(cpTestSpectrum == NULL)
	{
		cpTestSpectrum = new CMtnSpectrumTestDlg;
		cpTestSpectrum->Create(IDD_MTN_SPECTRUM_TEST_DLG);
	}

	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);

	if(cSystemDebugLevel != __SYSTEM_PROTECTING_ENGINEER__)
	{
		if(cDlgInputPad.DoModal() == IDOK) //  20120906
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_sg_8d_zzy())
			{
				set_sys_pass_protect_level(__SYSTEM_PROTECTING_ENGINEER__);
				cpTestSpectrum->ShowWindow(1);

			}
		}
	}
	else
	{
		cpTestSpectrum->ShowWindow(1);
	}
}


void MtnTesterEntry::OnBnClicked_Toggle_ChangeLanguage()
{
	// Toggle
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		set_sys_language_option(LANGUAGE_UI_CN);
		GetDlgItem(IDC_BUT_CHANGE_LANGUAGE)->SetWindowTextA(_T("En"));
		GetDlgItem(IDC_SYS_MOVING_TEST)->SetWindowTextA(_T("伺服测试"));
		GetDlgItem(IDC_SYS_STEPPER)->SetWindowTextA(_T("步进马达测试"));
		GetDlgItem(IDC_SYS_COMM_SET)->SetWindowTextA(_T("通讯设置"));
		GetDlgItem(IDC_TUNE_PARAMETER_INIT_SERVO)->SetWindowTextA(_T("参数调节"));
		GetDlgItem(IDC_SYS_FB_MONITOR)->SetWindowTextA(_T("反馈观测"));
		GetDlgItem(IDC_SYS_SPECTRUM_TEST)->SetWindowTextA(_T("频谱测试"));
		GetDlgItem(IDC_SYS_TERMINAL_ACS)->SetWindowTextA(_T("终端程式"));
		GetDlgItem(IDC_SYS_ROYA_DSP_TEST)->SetWindowTextA(_T("DSP箱"));
		GetDlgItem(IDC_SYS_POSN_TUNE1)->SetWindowTextA(_T("位置压力校准"));
		GetDlgItem(IDC_SYS_TIMER)->SetWindowTextA(_T("定时器超声"));
		GetDlgItem(IDC_SERVO_ADJUSTMENT)->SetWindowTextA(_T("伺服调节"));
		GetDlgItem(IDC_CHECK_SYS_ENTRY_OFF_SERVO)->SetWindowTextA(_T("伺服断电"));
		GetDlgItem(IDCLOSE)->SetWindowTextA(_T("退出系统"));		
		GetDlgItem(IDC_STATIC_SYS_LABEL_MACHINE_TYPE)->SetWindowTextA(_T("机器类型"));
		GetDlgItem(IDC_SYS_MONITOR)->SetWindowTextA(_T("监测"));
	}
	else
	{
		set_sys_language_option(LANGUAGE_UI_EN); // iFlagLanguage = 
		GetDlgItem(IDC_BUT_CHANGE_LANGUAGE)->SetWindowTextA(_T("中"));
		GetDlgItem(IDC_SYS_MOVING_TEST)->SetWindowTextA(_T("Moving Test"));
		GetDlgItem(IDC_SYS_STEPPER)->SetWindowTextA(_T("StepperMotor"));
		GetDlgItem(IDC_SYS_COMM_SET)->SetWindowTextA(_T("Communication"));
		GetDlgItem(IDC_TUNE_PARAMETER_INIT_SERVO)->SetWindowTextA(_T("ParaTune"));
		GetDlgItem(IDC_SYS_FB_MONITOR)->SetWindowTextA(_T("Fb Monitor"));
		GetDlgItem(IDC_SYS_SPECTRUM_TEST)->SetWindowTextA(_T("Spectrum Test"));
		GetDlgItem(IDC_SYS_TERMINAL_ACS)->SetWindowTextA(_T("Term.Buff."));
		GetDlgItem(IDC_SYS_ROYA_DSP_TEST)->SetWindowTextA(_T("BoxDSP"));
		GetDlgItem(IDC_SYS_POSN_TUNE1)->SetWindowTextA(_T("PosnForceCali"));
		GetDlgItem(IDC_SYS_TIMER)->SetWindowTextA(_T("TimerUSG"));
		GetDlgItem(IDC_SERVO_ADJUSTMENT)->SetWindowTextA(_T("ServoAdjust"));
		GetDlgItem(IDC_CHECK_SYS_ENTRY_OFF_SERVO)->SetWindowTextA(_T("ServOff"));
		GetDlgItem(IDCLOSE)->SetWindowTextA(_T("Exit"));
		GetDlgItem(IDC_STATIC_SYS_LABEL_MACHINE_TYPE)->SetWindowTextA(_T("MachineType"));
		GetDlgItem(IDC_SYS_MONITOR)->SetWindowTextA(_T("SysMonitor"));
	}
	UpdateComboBoxMachineType();
	// Set the already started dialogs
	if(cpTestRoyaBd != NULL)
	{
		cpTestRoyaBd->SetUserInterfaceLanguage(get_sys_language_option());
	}
	if(cpTestSpectrum != NULL)
	{
		cpTestSpectrum->SetUserInterfaceLanguage(get_sys_language_option());
	}

	if(cpDlgMonitorFeedback != NULL)
	{
		cpDlgMonitorFeedback->SetUserInterfaceLanguage(get_sys_language_option());
	}

	if(cpMovingTest != NULL)
	{
		cpMovingTest->SetUserInterfaceLanguage(get_sys_language_option());
	}

	if(cpParameterDlg != NULL)
	{
		cpParameterDlg->SetUserInterfaceLanguage(get_sys_language_option());
	}
	if(pDlgServoCtrlTuning !=NULL)
	{
		pDlgServoCtrlTuning->SetUserInterfaceLanguage(get_sys_language_option());
	}
	if(cpVelStepGroupTest != NULL)
	{
		cpVelStepGroupTest->SetUserInterfaceLanguage(get_sys_language_option());
	}
	if(pDlgCommunicationSetup != NULL)
	{
		pDlgCommunicationSetup->SetUserInterfaceLanguage();
	}
}

void MtnTesterEntry::UpdateComboBoxMachineType()
{
	CComboBox *pSysMachineTypeCombo =((CComboBox *) GetDlgItem(IDC_COMBO_SYS_MACHINE_TYPE));
	CString cstrTemp;

	//pWbAppPerformCheckCombo->Clear(); clear current item
	pSysMachineTypeCombo->ResetContent();

	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_VLED_MAGAZINE, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_VLED_MAGAZINE]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_VLED_FORK, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_VLED_FORK]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_HORI_LED, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_HORI_LED]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_ONE_TRACK_13V_LED, _T(astrMachineTypeNameLabel_en[WB_MACH_TYPE_ONE_TRACK_13V_LED]));
		pSysMachineTypeCombo->InsertString(WB_STATION_XY_VERTICAL, _T(astrMachineTypeNameLabel_en[WB_STATION_XY_VERTICAL]));
		pSysMachineTypeCombo->InsertString(WB_STATION_XY_TOP, _T(astrMachineTypeNameLabel_en[WB_STATION_XY_TOP]));
		pSysMachineTypeCombo->InsertString(WB_STATION_BH, _T(astrMachineTypeNameLabel_en[WB_STATION_BH]));
		pSysMachineTypeCombo->InsertString(WB_STATION_WIRE_CLAMP, _T(astrMachineTypeNameLabel_en[WB_STATION_WIRE_CLAMP]));
		pSysMachineTypeCombo->InsertString(WB_STATION_EFO_BSD, _T(astrMachineTypeNameLabel_en[WB_STATION_EFO_BSD]));
		pSysMachineTypeCombo->InsertString(WB_STATION_USG_LIGHTING_PR, _T(astrMachineTypeNameLabel_en[WB_STATION_USG_LIGHTING_PR]));
		pSysMachineTypeCombo->InsertString(BE_WB_ONE_TRACK_18V_LED, _T(astrMachineTypeNameLabel_en[BE_WB_ONE_TRACK_18V_LED]));  // 20120704
		pSysMachineTypeCombo->InsertString(BE_WB_HORI_20T_LED, _T(astrMachineTypeNameLabel_en[BE_WB_HORI_20T_LED]));  // 20120827
	}
	else
	{
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_VLED_MAGAZINE, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_VLED_MAGAZINE]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_VLED_FORK, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_VLED_FORK]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_HORI_LED, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_HORI_LED]));
		pSysMachineTypeCombo->InsertString(WB_MACH_TYPE_ONE_TRACK_13V_LED, _T(astrMachineTypeNameLabel_cn[WB_MACH_TYPE_ONE_TRACK_13V_LED]));
		pSysMachineTypeCombo->InsertString(WB_STATION_XY_VERTICAL, _T(astrMachineTypeNameLabel_cn[WB_STATION_XY_VERTICAL]));
		pSysMachineTypeCombo->InsertString(WB_STATION_XY_TOP, _T(astrMachineTypeNameLabel_cn[WB_STATION_XY_TOP]));
		pSysMachineTypeCombo->InsertString(WB_STATION_BH, _T(astrMachineTypeNameLabel_cn[WB_STATION_BH]));
		pSysMachineTypeCombo->InsertString(WB_STATION_WIRE_CLAMP, _T(astrMachineTypeNameLabel_cn[WB_STATION_WIRE_CLAMP]));
		pSysMachineTypeCombo->InsertString(WB_STATION_EFO_BSD, _T(astrMachineTypeNameLabel_cn[WB_STATION_EFO_BSD]));
		pSysMachineTypeCombo->InsertString(WB_STATION_USG_LIGHTING_PR, _T(astrMachineTypeNameLabel_cn[WB_STATION_USG_LIGHTING_PR]));
		pSysMachineTypeCombo->InsertString(BE_WB_ONE_TRACK_18V_LED, _T(astrMachineTypeNameLabel_cn[BE_WB_ONE_TRACK_18V_LED]));  // 20120704
		pSysMachineTypeCombo->InsertString(BE_WB_HORI_20T_LED, _T(astrMachineTypeNameLabel_cn[BE_WB_HORI_20T_LED]));  // 20120704
	}
	pSysMachineTypeCombo->SetCurSel(get_sys_machine_type_flag());  // iFlagSysMachineType, 
}

void MtnTesterEntry::DeleteAllDialogResource()
{
	if(cpMtnTune1 != NULL)
	{
		cpMtnTune1 = NULL; // delete cpMtnTune1;
	}
	if(cpTestRoyaBd != NULL)
	{
		cpTestRoyaBd = NULL; // delete cpTestRoyaBd;
	}
	if(cpTestSpectrum != NULL)
	{
		cpTestSpectrum = NULL; // delete cpTestSpectrum;
	}
	if(cpDlgMonitorFeedback != NULL)
	{
		cpDlgMonitorFeedback = NULL; // delete cpDlgMonitorFeedback;
	}
	if(cpMovingTest != NULL)
	{
		cpMovingTest = NULL; // delete cpMovingTest;
	}
	if(cpParameterDlg != NULL)
	{
		cpParameterDlg = NULL; // delete cpParameterDlg;
	}
	if(pDlgServoCtrlTuning !=NULL)
	{
		pDlgServoCtrlTuning = NULL; // delete pDlgServoCtrlTuning;
	}
	if(cpVelStepGroupTest != NULL)
	{
		cpVelStepGroupTest = NULL; // delete cpVelStepGroupTest;
	}
#ifdef __TEST_STEPPER_CTRL_8144__
	if(cpStepperCtrl_PCI8144 != NULL)
	{
		cpStepperCtrl_PCI8144 = NULL;
	}
#endif  // __TEST_STEPPER_CTRL_8144__
}

// IDC_CHECK_SYS_ENTRY_OFF_SERVO
void MtnTesterEntry::OnBnClickedCheckSysEntryOffServo()
{
	int iFlagOffServo = ((CButton*)GetDlgItem(IDC_CHECK_SYS_ENTRY_OFF_SERVO))->GetCheck();
	if(stServoControllerCommSet.Handle != ACSC_INVALID && iFlagOffServo == TRUE)
	{
		mtn_wb_table_servo_go_to_safety_position(stServoControllerCommSet.Handle, APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID);
		mtn_api_servo_off_all(stServoControllerCommSet.Handle);
	}
}

// IDC_CHECK_SYS_CLEAN_ON_EXIT
void MtnTesterEntry::OnBnClickedCheckSysCleanOnExit()
{
	iFlagCleanEnvironmentOnExit = ((CButton *)GetDlgItem(IDC_CHECK_SYS_CLEAN_ON_EXIT))->GetCheck();
}

void aft_spectrum_set_test_condition_dual_track_20_vled();
void aft_spectrum_set_test_condition_hori_led();
void aft_spectrum_set_test_condition_1_cup_vled();
void aft_spectrum_set_test_condition_station_hori_xy();
void aft_spectrum_set_test_condition_18v_led();

//extern char sys_acs_communication_get_flag_sc_udi();
#include "aftprbs.h"

void sys_config_machine_type(int iTempMachCfg)
{
		mtn_wb_dll_set_sys_machine_type(iTempMachCfg);  // iFlagSysMachineType = ;
		if(WB_MACH_TYPE_VLED_FORK == iTempMachCfg)   // machine type dependency Item-6
		{
			mtest_fb_init_search_limit_home_parameter_vled_bonder();
			aft_rand_burn_in_init_def_var();
			mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
			aft_spectrum_set_test_condition_dual_track_20_vled();

		}
		else if(WB_MACH_TYPE_HORI_LED == iTempMachCfg || iTempMachCfg == WB_STATION_XY_TOP || iTempMachCfg == BE_WB_HORI_20T_LED)
		{
			mtest_fb_init_search_limit_home_parameter_hori_bonder();
			aft_rand_burn_in_init_def_var_hori_led_bonder();
			mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
			aft_spectrum_set_test_condition_hori_led();
		}
		else if(iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)  // 20120801
		{
			mtest_fb_init_search_limit_home_parameter_hori_bonder();
			mtn_dll_init_def_para_search_index_18v_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
			aft_rand_burn_in_init_def_var_hori_led_bonder();
			aft_spectrum_set_test_condition_hori_led();
		}
		else  // Default 1-cup VLED, safest
		{
			mtest_fb_init_search_limit_home_parameter_13v_bonder();
			aft_rand_burn_in_init_def_var_1cup_v_led_bonder();
			mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
			aft_spectrum_set_test_condition_1_cup_vled();
		}
		// From DLL
		// 5. Tuning Related
		if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK || iTempMachCfg == WB_STATION_XY_VERTICAL
			|| iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)  // machine type dependency Item-7
		{
			mtn_tune_init_wb_table_y_tuning_vled_position_set();
		}
		else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP || iTempMachCfg == BE_WB_HORI_20T_LED)  // 20120826
		{
			mtn_tune_init_wb_table_y_tuning_hori_led_position_set();
		}
		else // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
		{
			mtn_tune_init_wb_table_y_tuning_vled_position_set();
		}

		// AFT related 20110318
		aft_init_spec();
		// Application name axis mapping
		//if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK)
		//{
		//	InitWireBondServoAxisName_VerLED_ACS();
		//}
		//else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == WB_STATION_XY_TOP)
		//{
		//	InitWireBondServoAxisName_HoriLED_ACS();
		//}
		//else // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
		//{
		//	InitWireBondServoAxisName_VerLED_ACS();
		//}
		if(sys_acs_communication_get_flag_sc_udi() == 0)
		{
			InitWireBondServoAxisName();
		}
		else
		{
			InitWireBondNameServo_ACS_SC_UDI_Axis();
		}

		mtn_move_test_config_burn_in_condition();
		if(aft_spectrum_initialize() != MTN_API_OK_ZERO)
		{
			AfxMessageBox(_T("Error Initialize Environment Variables"));
		}
		aft_spectrum_init_sine_sweep_cfg();

		if(cpTestSpectrum != NULL)  		// 20110719
		{
			cpTestSpectrum->InitPrbsGroupTestConfigVar();
			cpTestSpectrum->InitPrbsGroupTestConfigUI();
		}
		if(cpVelStepGroupTest != NULL)
		{
			cpVelStepGroupTest->InitVelStepGroupTestCfg();
		}
}
// IDC_COMBO_SYS_MACHINE_TYPE
void MtnTesterEntry::OnCbnSelchangeComboSysMachineType()
{
	int iPrevSel = get_sys_machine_type_flag();  //iFlagSysMachineType;
	int iTempMachCfg =  ((CComboBox*)GetDlgItem(IDC_COMBO_SYS_MACHINE_TYPE))->GetCurSel();
	if(iTempMachCfg != iPrevSel)  //iFlagSysMachineType) get_sys_machine_type_flag()
	{
		sys_config_machine_type(iTempMachCfg);
	}
}

#ifdef __TEST_STEPPER_CTRL_8144__
#include "DlgStepper_PCI8144.h"
static CDlgStepper_PCI8144 *cpStepperCtrl_PCI8144;

// IDC_SYS_STEPPER_CTRL_PCI_8144
void MtnTesterEntry::OnBnClickedSysStepperCtrlPci8144()
{
	cpStepperCtrl_PCI8144 = new CDlgStepper_PCI8144(this);
	cpStepperCtrl_PCI8144->Create(IDD_DIALOG_STEPPER_8144);
	cpStepperCtrl_PCI8144->ShowWindow(1);
}
#endif // __TEST_STEPPER_CTRL_8144__
// IDC_CHECK_ENABLE_EDITING_MACH
// iFlagEnableEditMachineSn
void MtnTesterEntry::OnBnClickedCheckEnableEditingMach()
{
	CDlgKeyInputPad cDlgInputPad;

	int iTemp = ((CButton *)GetDlgItem(IDC_CHECK_ENABLE_EDITING_MACH))->GetCheck();
	if(iTemp == TRUE)
	{
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		//			cDlgInputPad.SetWindowTextA(_T("Input Password"));
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber() == wb_mtn_tester_get_password_brightlux_6d_zzy() )
			{
				iFlagEnableEditMachineSn = iTemp;
			}
			else
			{
				iFlagEnableEditMachineSn = FALSE;// DisableTabOpenLoopTest();
			}
		}
	}
	else
	{
		if(iFlagEnableEditMachineSn == TRUE)
		{
			iFlagEnableEditMachineSn = FALSE;
			// Save File once
		}
	}

	((CButton *)GetDlgItem(IDC_CHECK_ENABLE_EDITING_MACH))->SetCheck(iFlagEnableEditMachineSn);
	GetDlgItem(IDC_EDIT_SYS_MACH_SN)->EnableWindow(iFlagEnableEditMachineSn);
}

// IDC_EDIT_SYS_MACH_SN
void MtnTesterEntry::OnEnKillfocusEditSysMachSn()
{
	unsigned int uiTempMachSn;
	ReadUnsignedIntegerFromEdit(IDC_EDIT_SYS_MACH_SN, &uiTempMachSn);

	set_sys_machine_serial_num(uiTempMachSn);
}

void MtnTesterEntry::ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

void MtnTesterEntry::ReleaseDlgPointer_ACS()
{
//	cpDlgMonitorFeedback = NULL;
//	cpMovingTest = NULL;
//	stpDlgTerminal = NULL;
//	cpMtnTune1 = NULL;
//	cpParameterDlg = NULL;
//	cpTestProfCalculator = NULL;
//	cpTestSpectrum = NULL;
//	pDlgServoCtrlTuning = NULL;
//	cpVelStepGroupTest = NULL;
}