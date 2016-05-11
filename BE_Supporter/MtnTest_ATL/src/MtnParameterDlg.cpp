// MtnParameterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MtnParameterDlg.h"
#include "mtnapi.h"
#include "MtnInitAcs.h"
#include "MtnTesterResDef.h" // 
#include "MtnTune.h"

extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableX_DRA;
extern MTN_TUNE_GENETIC_INPUT stMtnTuneInputTableY_DRA;
extern MTN_TUNE_GENETIC_INPUT astMtnTuneInput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER]; 
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneOutput[MAX_SERVO_AXIS_WIREBOND][MAX_BLK_PARAMETER];

extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableX[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_INPUT astMtnTuneSectorInputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];
extern MTN_TUNE_GENETIC_OUTPUT astMtnTuneSectorOutputTableY[MAX_NUM_SECTOR_POSN_RANGE_TBL][MAX_BLK_PARAMETER];

//extern char mtn_wb_tune_thread_get_flag_running();
//extern void mtn_wb_tune_thread_set_flag_running(char cFlag);

extern CMtnTune mMotionTuning;

extern COMM_SETTINGS stServoControllerCommSet;
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];
//extern double afEncoderResolution_cnt_p_mm[MAX_SERVO_AXIS_WIREBOND];
extern int idxTuningBlockBondHead;
extern int idxTuningBlockTable;
extern int iFlagDebug_SaveFile_UIProtPass;
extern int iFlagTableSectorTuning;
extern unsigned int idxCurrSectorTable;
//extern int iFlagIsActualContact;
extern char cTuneBlocks_FlagTableX, cTuneBlocks_FlagTableY, cTuneBlocks_FlagBondHeadZ;
extern char cFlagTuningRoutine;

//static int iCurrSelAxisServoParaTuning = WB_AXIS_TABLE_X;  // X
//static unsigned int uiCurrWireNo=0, uiBlkMovePosnSet=0;
extern unsigned int uiCurrWireNo;

static double dTunePosnLowerBound, dTunePosnUpperBound;
static int iServoParaTuningAlgoOption = SERVO_PARA_TUNE_ALGO_1_DIM_AFT_1; 


//void mtn_wb_tune_init_def_servo_para_tune_input(MTN_TUNE_GENETIC_INPUT *stpMtnServoParaTuneInput, int iWbAxisOpt, unsigned int uiBlkMovePosnSet);
static int iFlagServoTuningByDLL = 0;
static int iFlagDoPosnCompBefB1W_ParaTuneDlg = 1;
HWND GetConsoleHwnd(void)
{
       #define MY_BUFSIZE 1024 // Buffer size for console window titles.
       HWND hwndFound;         // This is what is returned to the caller.
       char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
                                           // WindowTitle.
       char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
                                           // WindowTitle.

       // Fetch current window title.
       GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);

       // Format a "unique" NewWindowTitle.
       wsprintf(pszNewWindowTitle,"%d/%d",
                   GetTickCount(),
                   GetCurrentProcessId());

       // Change current window title.
       SetConsoleTitle(pszNewWindowTitle);

       // Ensure window title has been updated.
       Sleep(40);

       // Look for NewWindowTitle.
       hwndFound=FindWindow(NULL, pszNewWindowTitle);

       // Restore original window title.
       SetConsoleTitle(pszOldWindowTitle);

       return(hwndFound);
}

// MtnParameterDlg dialog
//char strServoSpeedFilenameInitParameterACS[] = "F:\\Zhengyi\\Mywork\\MtnTest_ATL\\Release\\ParaBase\\ServoMaster.ini";
char strServoSpeedFilenameInitParameterACS[] = "C:\\WbData\\ParaBase\\ServoMaster.ini";

static BOOL bAcsParameterIsInitialized = FALSE;

BOOL para_get_flag_initialization()
{
	return bAcsParameterIsInitialized;
}

IMPLEMENT_DYNAMIC(MtnParameterDlg, CDialog)

#undef MOTALGO_DLL_EXPORTS
#include "MotAlgo_DLL.h"

MtnParameterDlg::MtnParameterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MtnParameterDlg::IDD, pParent)
{
  mtn_wb_tune_thread_set_flag_running(FALSE);
}

MtnParameterDlg::~MtnParameterDlg()
{
}

void MtnParameterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
 
// IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_X
void MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneTblX()
{
	cTuneBlocks_FlagTableX = ((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_X))->GetCheck();
}
// IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_Y
void MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneTblY()
{
	cTuneBlocks_FlagTableY = ((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_Y))->GetCheck();
}
// IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_BONDHEAD_Z
void MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneBondheadZ()
{
	cTuneBlocks_FlagBondHeadZ = ((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_BONDHEAD_Z))->GetCheck();
}

#include "AcsServo.h" // 20120522
#include "acs_buff_prog.h"

BOOL MtnParameterDlg::OnInitDialog()
{
	if(mtnapi_init_master_config_acs(strServoSpeedFilenameInitParameterACS, stServoControllerCommSet.Handle) == MTN_API_OK_ZERO)
	{
		GetDlgItem(IDC_BUTTON_PARAMETER_INIT_FROM_FILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_LOAD_TUNE_CFG)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_PARAMETER_INIT_FROM_FILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_LOAD_TUNE_CFG)->EnableWindow(FALSE);
		AfxMessageBox("失败参数文件路径");
	}

	//show the master filename
	GetDlgItem(IDC_STATIC_PARAMETER_DATABASE_MASTER_FILE_PATH_NAME)->SetWindowTextA(_T(strServoSpeedFilenameInitParameterACS));

	// Language
	SetUserInterfaceLanguage(get_sys_language_option());

	// Disable some buttons, TBA
//	GetDlgItem(IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PARA_INIT_DOWNLOAD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PARAMETER_SAVE_XLS)->EnableWindow(FALSE);
//	GetDlgItem(IDC_BUTTON_PARAMETER_SAVE_FILE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PARA_DOWNLOAD)->EnableWindow(FALSE);

	// Init Parts for Tuning
	SetComboAxisNameLanguage(get_sys_language_option(), IDC_COMBO_PARAMETER_TUNING_AXIS, mtn_wb_tune_get_curr_wb_axis_servo_para_tuning());
	mtnapi_upload_wb_servo_speed_parameter_acs(stServoControllerCommSet.Handle);

	// Init variables in static/global memory
	((CButton*) GetDlgItem(IDC_FLAG_CHECK_TUNING_BY_DLL))->SetCheck(iFlagServoTuningByDLL);

	// Initialization for motion servo tuning
	mtn_dll_wb_tune_initialization();

	int iFlagMachineType = get_sys_machine_type_flag();
	// initialization for searching home
	if(iFlagMachineType == WB_MACH_TYPE_VLED_FORK)   // machine type dependency Item-12
	{
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iFlagMachineType == WB_MACH_TYPE_HORI_LED || iFlagMachineType == BE_WB_HORI_20T_LED ) // 20120826  // 20120731
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iFlagMachineType == BE_WB_ONE_TRACK_18V_LED)
	{
		mtn_dll_init_def_para_search_index_18v_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else  // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
	{
		mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}

	// 
	UpdateTuningBlkCurrAxis();
	UpdateParameterTuneOutputCurrentAxis();
	InitComboUI();

//	((CButton *)GetDlgItem(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR))->ShowWindow(FALSE);  // TBA

	int iFlagDownloadB1W_ParaToACS = 1;  // 20120522
	if(theAcsServo.GetServoOperationMode()  == OFFLINE_MODE)  
	{
		iFlagDownloadB1W_ParaToACS = 1;
	}
	else if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		acs_buffprog_upload_prog_get_line(0);
		if(acs_buffprog_get_num_lines_at_buff(0) <= 20)
		{
			iFlagDownloadB1W_ParaToACS = 1;
		}

	}
//	if(iFlagDownloadB1W_ParaToACS == 1)
//	{
		if(mtnapi_init_servo_control_para_acs(strServoSpeedFilenameInitParameterACS, stServoControllerCommSet.Handle) != MTN_API_OK_ZERO)
		{
			if(get_sys_language_option() == LANGUAGE_UI_EN)
			{
				AfxMessageBox("Error: updated from the file");
			}
			else
			{
				AfxMessageBox("失败参数从文件更新");
			}
		}
		mtn_wb_dll_download_acs_servo_speed_parameter_acs(stServoControllerCommSet.Handle);
//	}  // 20120522

	AfxMessageBox("In WireBonder, Reload Servo Parameter. \n\r Enter and exit Servo System Configure");
	UpdateB1W_ParaBndZ_TextEditBox();
	StartTimer(100);
//  TBA
//	GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT)->EnableWindow(FALSE);
	return CDialog::OnInitDialog(); // TRUE; 
}


BEGIN_MESSAGE_MAP(MtnParameterDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_INIT_FROM_FILE, &MtnParameterDlg::OnBnClickedButtonParameterInitFromFile)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD, &MtnParameterDlg::OnBnClickedButtonParameterUploadFromCtrlBoard)
	ON_BN_CLICKED(IDC_BUTTON_PARA_DOWNLOAD, &MtnParameterDlg::OnBnClickedButtonParaDownload)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_SAVE_FILE, &MtnParameterDlg::OnBnClickedButtonParameterSaveFile)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS, &MtnParameterDlg::OnBnClickedButtonParameterDatabaseHomeAllAxis)
	ON_BN_CLICKED(IDC_BUTTON_PARA_INIT_DOWNLOAD, &MtnParameterDlg::OnBnClickedButtonParaInitDownload)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_SAVE_XLS, &MtnParameterDlg::OnBnClickedButtonParameterSaveXls)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_TUNING_AXIS, &MtnParameterDlg::OnCbnSelchangeComboParameterTuningAxis)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_START_TUNE, &MtnParameterDlg::OnBnClickedButtonParameterDlgStartTune)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_STOP_TUNE, &MtnParameterDlg::OnBnClickedButtonParameterDlgStopTune)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVkp)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVki)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundPkp)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundSof)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVli)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVli)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundSof)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundPkp)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVki)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVkp)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundAffc)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundAffc)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN, &MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundTunePosn)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN, &MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundTunePosn)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_B1W_CTRL_BLK, &MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTheme)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_AXIS_TUNING_ALGO, &MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningAlgo)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK, &MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningPosnBlk)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_DEBUG_FLAG, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneDebugFlag)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK, &MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTablePosnBlk)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_ALL_START, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneAllStart)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_X, &MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneTblX)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_Y, &MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneTblY)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_BONDHEAD_Z, &MtnParameterDlg::OnBnClickedCheckParameterDlgFlagTuneBondheadZ)
	ON_CBN_SELCHANGE(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE, &MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTablePosnInRange)
	ON_BN_CLICKED(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR, &MtnParameterDlg::OnBnClickedCheckTuneFlagTableSector)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_RUN_LAST_B1W, &MtnParameterDlg::OnBnClickedButtonParameterDlgRunLastB1w)
	ON_BN_CLICKED(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT, &MtnParameterDlg::OnBnClickedCheckTuneFlagB1wActualContact)
	ON_BN_CLICKED(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W, &MtnParameterDlg::OnBnClickedCheckParaTuneDlgEnableB1w)
	ON_BN_CLICKED(IDCANCEL, &MtnParameterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_LOAD_TUNE_CFG, &MtnParameterDlg::OnBnClickedButtonParameterDlgLoadTuneCfg)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W, &MtnParameterDlg::OnBnClickedButtonParameterDlgRunSysB1w)
	ON_BN_CLICKED(IDC_CHECK_TUNE_FLAG_B1W_MOTION, &MtnParameterDlg::OnBnClickedCheckTuneFlagB1wMotion)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdle)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wSrch2b)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wReset)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wTail)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdleB1wTraj2b)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wLooptop)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wLooping)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdleB1wMove1b)
	ON_BN_CLICKED(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B, &MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wSrch1b)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wSrch1b)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wIdle)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wMove1b)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wLooping)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wLooptop)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wTraj2b)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wTail)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wReset)
	ON_BN_CLICKED(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B, &MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wSrch2b)
	ON_BN_CLICKED(IDC_FLAG_CHECK_TUNING_BY_DLL, &MtnParameterDlg::OnBnClickedFlagCheckTuningByDll)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_B1W_VKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wVkp)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_B1W_ACCFFC, &MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wAccffc)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_B1W_VKI, &MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wVki)
	ON_EN_KILLFOCUS(IDC_EDIT_PARAMETER_DLG_B1W_PKP, &MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wPkp)
END_MESSAGE_MAP()

static UINT_PTR iTimerIdParameterDlg;
//extern BOND_HEAD_PERFORMANCE    stBondHeadPerformanceBestB1W_;   // 20110523
extern WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE]; // 20110529
extern char *astrTextContactOrNot[];

void MtnParameterDlg::UpdateB1W_PerformanceButton()
{
	CString cstrTemp;
	cstrTemp.Format("Idle:%3.1f, %s", astWbOneWirePerformance[0].stBondHeadPerformance.dIdleRMS_5ms, astrTextContactOrNot[astWbOneWirePerformance[0].iFlagIsDryRun]);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE)->SetWindowTextA(cstrTemp);               // 20110521
	cstrTemp.Format("Srch1B: %3.0f, %d", 
		astWbOneWirePerformance[0].stBondHeadPerformance.f1stBondSearchFeedAccRMS, astWbOneWirePerformance[0].stBondHeadPerformance.iFlagIsFalseContact1stB);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("MvTo1B:%3.0f,%3.0f", 
		astWbOneWirePerformance[0].stBondHeadPerformance.f1stBondMoveToSearchHeightOverUnderShoot, astWbOneWirePerformance[0].stBondHeadPerformance.f1stBondVelErrAtMoveSrchHeight); // 20110524
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B)->SetWindowTextA(cstrTemp);   

	cstrTemp.Format("Lping: %3.0f", astWbOneWirePerformance[0].stBondHeadPerformance.fReverseHeightOverUnderShoot);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("LpTop: %3.0f", astWbOneWirePerformance[0].stBondHeadPerformance.fLoopTopOverUnderShoot);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("Traj2B:%3.0f,%3.0f", 
		astWbOneWirePerformance[0].stBondHeadPerformance.f2ndBondMoveToSearchHeightOverUnderShoot, astWbOneWirePerformance[0].stBondHeadPerformance.f2ndBondVelErrAtTrajEnd); // 20110524
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("Tail: %3.0f", astWbOneWirePerformance[0].stBondHeadPerformance.fTailOverUnderShoot);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL)->SetWindowTextA(cstrTemp); 

	cstrTemp.Format("Reset: %3.0f", astWbOneWirePerformance[0].stBondHeadPerformance.fEndFireLevelOverUnderShoot);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("Srch2B: %3.0f, %d", 
		astWbOneWirePerformance[0].stBondHeadPerformance.f2ndBondSearchFeedAccRMS, astWbOneWirePerformance[0].stBondHeadPerformance.iFlagIsFalseContact2ndB);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B)->SetWindowTextA(cstrTemp);        

}

static int iDlgSrvoParameterTuneShowFlag = 0;
int MtnParameterDlg::UI_GetShowWindowFlag()
{
	return iDlgSrvoParameterTuneShowFlag;
}
void MtnParameterDlg::ShowWindow(int nCmdShow)
{
	iDlgSrvoParameterTuneShowFlag = 1;
	UpdateB1W_ParaBndZ_TextEditBox(); // 20120522
	CDialog::ShowWindow(nCmdShow);
}
extern int nFeasibleParameter;
extern MTN_TUNE_CASE astMotionTuneHistory[];

void MtnParameterDlg::UpdateUI_ByTimer()
{
	CString cstrTemp;
	static unsigned int idxCurrTune;
	static unsigned int nTotal;
	static double dCurrBest;

	int iPosnTune1, iPosnTune2;
	int iAxisTuningACS = mMotionTuning.mtn_tune_get_tune_axis();
	unsigned int uiCurrTuningSector, nTotalSector;
	int idxBestParaSet;

	switch(cFlagTuningRoutine)
	{
	case SERVO_PARA_TUNING_ROUTINE_ONE_CASE:
	//if(cFlagTuningRoutine == )
	//{
		if(iServoParaTuningAlgoOption == SERVO_PARA_TUNE_ALGO_COMPLETE_SEARCH)
		{
			mMotionTuning.mtn_tune_get_status_axis_complete(&idxCurrTune, &nTotal, &dCurrBest);
		}
		else // SERVO_PARA_TUNE_ALGO_1_DIM_AFT_1
		{
			mMotionTuning.mtn_tune_get_status_axis_1by1(&idxCurrTune, &nTotal, &dCurrBest);
		}
		break;
	//}
	//else if (cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL)
	//	cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W
	//{
	case SERVO_PARA_TUNING_ROUTINE_AUTO_FULL:
	case SERVO_PARA_TUNING_ROUTINE_AUTO_B1W:
		mMotionTuning.mtn_tune_get_status_axis_1by1(&idxCurrTune, &nTotal, &dCurrBest);
		iPosnTune1 = mMotionTuning.mtn_tune_get_move_point_1();
		iPosnTune2 = mMotionTuning.mtn_tune_get_move_point_2();
		if(iAxisTuningACS == astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS ||
			iAxisTuningACS == astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS)
		{
			uiCurrTuningSector = mtn_wb_tune_get_curr_tune_sector();
			if( iAxisTuningACS == astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS )
			{
				nTotalSector = mtn_wb_tune_get_max_sector_table_x();
			}
			else
			{
				nTotalSector = mtn_wb_tune_get_max_sector_table_y();
			}
		}
		else if( iAxisTuningACS == astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS )
		{
			nTotalSector = WB_BH_2ND_CONTACT+1; //WB_BH_RESET;
			uiCurrTuningSector = mtn_wb_tune_get_blk_move_posn_set();
		}

		//
		if(iFlagDebug_SaveFile_UIProtPass)
		{
			if(iFlagTableSectorTuning == FALSE)
			{
				UpdateTuningParameterCurrentAxis();
			}
			else
			{
				if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X)
				{
					UpdateTuningParameterCurrSectorTableX();
				}
				else if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_Y)
				{
					UpdateTuningParameterCurrSectorTableY();
				}
				else if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_BOND_Z)
				{
					UpdateTuningParameterBndHeadZ();
				}
			}
		}

		UpdateUI_ComboByVar();
		break;
	}
	switch(cFlagTuningRoutine)
	{
	case SERVO_PARA_TUNING_ROUTINE_ONE_CASE:
		cstrTemp.Format("Curr: %d/%d, BestObj: %6.2f", idxCurrTune, nTotal, dCurrBest);
		break;
	case SERVO_PARA_TUNING_ROUTINE_AUTO_FULL:
		cstrTemp.Format("Axis: %d, [%d, %d], Curr: %d/%d, Sector: %d/%d, BestObj: %6.1f, B1W:)-%d", 
			iAxisTuningACS, iPosnTune1, iPosnTune2, idxCurrTune, nTotal, uiCurrTuningSector, nTotalSector, dCurrBest,
			nFeasibleParameter);
		break;
	case SERVO_PARA_TUNING_ROUTINE_AUTO_B1W:
		idxBestParaSet = mMotionTuning.mtn_tune_get_best_idx_tune_1by1();
		WB_TUNE_B1W_BH_OBJ *stpTuneB1wObjValues;
		stpTuneB1wObjValues = &(astMotionTuneHistory[idxBestParaSet].stTuneB1wObjValues);

		cstrTemp.Format("B1W, Curr: %d/%d, Sector: %d/%d, BestObj: %6.1f, :)-%d, [%5.0f, %5.0f, %5.0f, %5.0f, %5.0f, %5.0f, %5.0f, %5.0f, %5.0f]", 
			idxCurrTune, nTotal, uiCurrTuningSector, nTotalSector, dCurrBest,
			nFeasibleParameter, 
			stpTuneB1wObjValues->dObj[0], stpTuneB1wObjValues->dObj[1], stpTuneB1wObjValues->dObj[2], stpTuneB1wObjValues->dObj[3],
			stpTuneB1wObjValues->dObj[4], stpTuneB1wObjValues->dObj[5], stpTuneB1wObjValues->dObj[6], stpTuneB1wObjValues->dObj[7],
			stpTuneB1wObjValues->dObj[8]);
		break;
	}

	if(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W)
	{
		UpdateB1W_PerformanceButton();
		UpdateTuningParameterCurrentAxis();
	}

	if(mtn_wb_tune_thread_get_flag_running() == FALSE)
	{
		if(stServoControllerCommSet.Handle == ACSC_INVALID)
		{
			EnableTuningUI_Combo_Button(FALSE);
			GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W)->EnableWindow(TRUE);
		}
		else
		{
			EnableTuningUI_Combo_Button(TRUE);
			GetDlgItem(IDC_BUTTON_PARA_DOWNLOAD)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);  // 20120515
			GetDlgItem(IDC_BUTTON_PARAMETER_SAVE_FILE)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);  // 20120515
			GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);  // 20120515
			GetDlgItem(IDC_BUTTON_PARAMETER_INIT_FROM_FILE)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);  // 20120515
			GetDlgItem(IDC_BUTTON_PARAMETER_DLG_START_TUNE)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);  // 20120515
//			GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);
//			GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W)->EnableWindow(iFlagDebug_SaveFile_UIProtPass);
		}		
	}
	else
	{   // During Tuning Thread
		EnableTuningUI_Combo_Button(FALSE);
		EnableUI_B1W_EditOnly_ParaBndZ(FALSE); // 20120522
		UpdateServoParameterDuringTune(); //
		GetDlgItem(IDC_STATIC_SERVO_PARA_TUNE_STATUS)->SetWindowTextA(cstrTemp);
	}
}
void MtnParameterDlg::EnableTuningUI_Combo_Button(bool bEnableFlag)
{
	GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK)->EnableWindow(bEnableFlag); // Bond Head Position Block
	GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_ALGO)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_PARAMETER_B1W_CTRL_BLK)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_PARAMETER_TUNING_AXIS)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE)->EnableWindow(bEnableFlag);

	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN)->EnableWindow(bEnableFlag);

	// DoNot Update during tuning
	EnableTuningUI_Edit_Button_ByPass(bEnableFlag && iFlagDebug_SaveFile_UIProtPass);

//	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_START_TUNE)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_ALL_START)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_LAST_B1W)->EnableWindow(bEnableFlag);  // 20120515
//	

	GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT)->EnableWindow(bEnableFlag);
//	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION)->EnableWindow(bEnableFlag); // 20110521

	//B1W, Checks and Buttons, 20110523
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE)->EnableWindow(bEnableFlag);               // 20110521
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B)->EnableWindow(bEnableFlag);   
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL)->EnableWindow(bEnableFlag); 
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B)->EnableWindow(bEnableFlag);        
     /// Checks
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET)->EnableWindow(bEnableFlag);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B)->EnableWindow(bEnableFlag);        /// 20110523

	GetDlgItem(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR)->EnableWindow(bEnableFlag);  // 20120717
}

void MtnParameterDlg::EnableTuningUI_Edit_Button_ByPass(bool bEnableFlag)
{
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC)->EnableWindow(bEnableFlag);

//	((CButton *)GetDlgItem(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR))->ShowWindow(bEnableFlag);
}

void MtnParameterDlg::UpdateTuningBlkCurrAxis()
{
	if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X || mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_Y)
	{
		mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockTable);
		mtn_wb_tune_set_curr_sector_table(idxCurrSectorTable);

		if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X)  // 20120717
		{
			((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE))->SetCurSel(mtn_wb_tune_get_max_sector_table_x());
		}
		else
		{
			((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE))->SetCurSel(mtn_wb_tune_get_max_sector_table_y());
		}

	}
	else if (mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_BOND_Z)
	{
		mtn_wb_tune_set_blk_move_posn_set(idxTuningBlockBondHead);
	}
//	mtn_wb_tune_update_tuning_condition(&astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningEncTimeConfig, 
//		mtn_wb_tune_get_curr_wb_axis_servo_para_tuning(), mtn_wb_tune_get_curr_wire_number(), mtn_wb_tune_get_blk_move_posn_set());

	if(iFlagTableSectorTuning == FALSE)
	{
		UpdateTuningParameterCurrentAxis();
	}
	else
	{
		if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X)
		{
			UpdateTuningParameterCurrSectorTableX();
		}
		else if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_Y)
		{
			UpdateTuningParameterCurrSectorTableY();
		}
	}
	UpdateParameterTuneOutputCurrentAxis();
}

void MtnParameterDlg::UpdateDoubleToEdit(int nResId, double dValue, char *strFormat)
{
	CString cstrTemp;
	cstrTemp.Format(strFormat, dValue); // "%5.1f"
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}

static char *strStopString;
void MtnParameterDlg::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*pdValue = strtod(tempChar, &strStopString);
//	sscanf_s(tempChar, "%f", pdValue);
}

void MtnParameterDlg::UpdateTuningParameterCurrentAxis()
{
	int idxCurrTuningAxis = mtn_wb_tune_get_curr_wb_axis_servo_para_tuning();

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopLimitI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopLimitI, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dAccFFC, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dAccFFC, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePosition_1, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePointion_2, "%5.1f");

	double dEncUnit_m = (double)(astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningEncTimeConfig.dEncCntUnit_um/1000000);
 
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_VEL, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxVelocity * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_ACC, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxAcceleration * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_JERK, astMtnTuneInput[idxCurrTuningAxis][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxJerk * dEncUnit_m, "%5.1f");
}

void MtnParameterDlg::UpdateTuningParameterCurrSectorTableX()
{
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopLimitI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopLimitI, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dAccFFC, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dAccFFC, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePosition_1, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePointion_2, "%5.1f");

	double dEncUnit_m = astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningEncTimeConfig.dEncCntUnit_um/1000000;
 
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_VEL, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxVelocity * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_ACC, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxAcceleration * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_JERK, astMtnTuneSectorInputTableX[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxJerk * dEncUnit_m, "%5.1f");
}

void MtnParameterDlg::UpdateTuningParameterCurrSectorTableY()
{
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopLimitI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopLimitI, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dAccFFC, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dAccFFC, "%5.1f");

	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePosition_1, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePointion_2, "%5.1f");

	double dEncUnit_m = astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningEncTimeConfig.dEncCntUnit_um/1000000;
 
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_VEL, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxVelocity * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_ACC, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxAcceleration * dEncUnit_m, "%5.2f");
	UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_SPEED_MAX_JERK, astMtnTuneSectorInputTableY[idxCurrSectorTable][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.stMtnSpeedProfile.dMaxJerk * dEncUnit_m, "%5.1f");
}

void MtnParameterDlg::UpdateTuningParameterBndHeadZ()
{
}

// MtnParameterDlg message handlers
void MtnParameterDlg::SetComboAxisNameLanguage(int m_iLanguageOption, int iComboResId, int iDefComboOption)
{
	CComboBox *pAxisComboAxisWB = (CComboBox*)GetDlgItem(iComboResId);
	
	if(m_iLanguageOption == LANGUAGE_UI_EN)
	{
		for(int ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
		{
			pAxisComboAxisWB->InsertString(ii, _T(astAxisInfoWireBond[ii].strAxisNameEn));
		}
	}
	else if(m_iLanguageOption == LANGUAGE_UI_CN)
	{
		for(int ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
		{
			pAxisComboAxisWB->InsertString(ii, _T(astAxisInfoWireBond[ii].strAxisNameCn));
		}
	}
	else
	{
	}
	pAxisComboAxisWB->SetCurSel(iDefComboOption);
}
extern char *astrTablePosnRange[];
extern unsigned int nNumSectorPosnRangeTableX;
extern unsigned int nNumSectorPosnRangeTableY;

void MtnParameterDlg::InitComboUI()
{
	CComboBox *pAxisComboTuneTheme = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_B1W_CTRL_BLK);
	pAxisComboTuneTheme->InsertString(WB_BH_IDLE, _T("IDLE"));
	pAxisComboTuneTheme->InsertString(WB_BH_1ST_CONTACT, _T("1Srch"));
	pAxisComboTuneTheme->InsertString(WB_BH_SRCH_HT, _T("Mv1sB"));
	pAxisComboTuneTheme->InsertString(WB_BH_LOOPING, _T("RH1"));
	pAxisComboTuneTheme->InsertString(WB_BH_LOOP_TOP, _T("LpTop"));
	pAxisComboTuneTheme->InsertString(WB_BH_TRAJECTORY, _T("Traj"));
	pAxisComboTuneTheme->InsertString(WB_BH_TAIL, _T("Tail"));
	pAxisComboTuneTheme->InsertString(WB_BH_RESET, _T("Reset"));
	pAxisComboTuneTheme->InsertString(WB_BH_2ND_CONTACT, _T("2Srch"));
	pAxisComboTuneTheme->InsertString(WB_BH_RD, _T("RD"));
	pAxisComboTuneTheme->InsertString(WB_BH_RH2, _T("RH2"));
	pAxisComboTuneTheme->InsertString(WB_BH_RD2, _T("RD2"));
	pAxisComboTuneTheme->InsertString(WB_BH_RH3, _T("RH3"));
	pAxisComboTuneTheme->InsertString(WB_BH_RD3, _T("RD3"));
	pAxisComboTuneTheme->InsertString(WB_BH_RH4, _T("RH4"));
	pAxisComboTuneTheme->InsertString(WB_BH_RTWS, _T("RTWS"));
	pAxisComboTuneTheme->InsertString(WB_BH_FTWS, _T("FTWS"));
	pAxisComboTuneTheme->SetCurSel(idxTuningBlockBondHead); // 20120522

	CComboBox *pAxisComboTuneAlgo = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_ALGO);
	pAxisComboTuneAlgo->InsertString(SERVO_PARA_TUNE_ALGO_COMPLETE_SEARCH, _T("FULL SRCH"));
	pAxisComboTuneAlgo->InsertString(SERVO_PARA_TUNE_ALGO_1_DIM_AFT_1, _T("1BY1 SRCH"));
	pAxisComboTuneAlgo->SetCurSel(iServoParaTuningAlgoOption);

	CComboBox *pBondHeadComboTuneBlock = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK);
	pBondHeadComboTuneBlock->InsertString(WB_BH_IDLE, _T("Idle")); // = 0,
	pBondHeadComboTuneBlock->InsertString(WB_BH_1ST_CONTACT, _T("1st_Contact")); // = 1,
	pBondHeadComboTuneBlock->InsertString(WB_BH_SRCH_HT, _T("Srch_Ht")); // = 2,
	pBondHeadComboTuneBlock->InsertString(WB_BH_LOOPING, _T("RH1")); // = 3,
	pBondHeadComboTuneBlock->InsertString(WB_BH_LOOP_TOP, _T("Loop_Top")); // = 4,
	pBondHeadComboTuneBlock->InsertString(WB_BH_TRAJECTORY, _T("Trajectory")); // = 5,
	pBondHeadComboTuneBlock->InsertString(WB_BH_TAIL, _T("Tail")); // = 6,
	pBondHeadComboTuneBlock->InsertString(WB_BH_RESET, _T("Reset")); // = 7,
	pBondHeadComboTuneBlock->InsertString(WB_BH_2ND_CONTACT, _T("2nd_Contact")); // = 8,
	pBondHeadComboTuneBlock->InsertString(WB_BH_RD, _T("RD"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RH2, _T("RH2"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RD2, _T("RD2"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RH3, _T("RH3"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RD3, _T("RD3"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RH4, _T("RH4"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_RTWS, _T("RTWS"));
	pBondHeadComboTuneBlock->InsertString(WB_BH_FTWS, _T("FTWS"));

	CComboBox *pTableComboTuneBlock = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK);
	pTableComboTuneBlock->InsertString(WB_TBL_MOTION_IDX_MOVE_BTO, _T("BTO"));
	pTableComboTuneBlock->InsertString(WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST, _T("R_DIST"));
	pTableComboTuneBlock->InsertString(WB_TBL_MOTION_IDX_MOVE_TRAJ, _T("TRAJ"));
	pTableComboTuneBlock->InsertString(WB_TBL_MOTION_IDX_MOVE_NEXT_PR, _T("NEXT_PR"));

	((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_X))->SetCheck(cTuneBlocks_FlagTableX);
	((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_TBL_Y))->SetCheck(cTuneBlocks_FlagTableY);
	((CButton*)GetDlgItem(IDC_CHECK_PARAMETER_DLG_FLAG_TUNE_BONDHEAD_Z))->SetCheck(cTuneBlocks_FlagBondHeadZ);
	((CButton *)GetDlgItem(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR))->SetCheck(iFlagTableSectorTuning);


	CString cstrTemp; cstrTemp.Format("TblSector:1U=%dmm",UNIT_DIST_MM_CLASSIFY_TABLE_SERVO); 
	GetDlgItem(IDC_STATIC_SECTOR_POSN_TABLE)->SetWindowTextA(cstrTemp);
	CComboBox *pTableComboTunePosnRangeSector = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE);
	unsigned int ii, nMaxNumSectorTable;
	if(nNumSectorPosnRangeTableX > nNumSectorPosnRangeTableY)
	{
		nMaxNumSectorTable = nNumSectorPosnRangeTableX;
	}
	else
	{
		nMaxNumSectorTable = nNumSectorPosnRangeTableY;
	}
	for(ii=0; ii<nMaxNumSectorTable; ii++)
	{
		pTableComboTunePosnRangeSector->InsertString(ii, _T(astrTablePosnRange[ii]));
	}
	UpdateUI_ComboByVar();

	((CButton *) GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W))->SetCheck(iFlagDoPosnCompBefB1W_ParaTuneDlg);
	UI_Show_TuneB1W(iFlagDoPosnCompBefB1W_ParaTuneDlg);
}

void MtnParameterDlg::UpdateUI_ComboByVar()
{
	CComboBox *pAxisComboTuneTheme = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_B1W_CTRL_BLK);
	pAxisComboTuneTheme->SetCurSel(astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningTheme.iThemeType);
	CComboBox *pBondHeadComboTuneBlock = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK);
	pBondHeadComboTuneBlock->SetCurSel(idxTuningBlockBondHead);
	CComboBox *pTableComboTuneBlock = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK);
	pTableComboTuneBlock->SetCurSel(idxTuningBlockTable);

	((CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_TUNING_AXIS))->SetCurSel(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning());
	((CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE))->SetCurSel(idxCurrSectorTable);
	
}

#include "DlgKeyInputPad.h"
// IDC_BUTTON_PARAMETER_INIT_FROM_FILE
void MtnParameterDlg::OnBnClickedButtonParameterInitFromFile()
{
	// TODO: Add your control notification handler code here
	//mtnapi_init_servo_parameter_from_file();
	//mtnapi_init_speed_parameter_from_file();
	//mtnapi_init_position_reg_from_file(); // 20090129
	static CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(123456);
	cDlgInputPad.SetInputNumber(0);
	if(cDlgInputPad.DoModal() == IDOK)
	{
//		if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_zzy())
//		{
//			if(mtnapi_init_servo_control_para_acs(strServoSpeedFilenameInitParameterACS, stServoControllerCommSet.Handle) == MTN_API_OK_ZERO)
			_mtnapi_dll_init_wb_servo_parameter_acs(stServoControllerCommSet.Handle);

			if(_mtnapi_dll_init_servo_speed_para_acs(stServoControllerCommSet.Handle) != MTN_API_OK_ZERO) 
			{
				if(get_sys_language_option() == LANGUAGE_UI_EN)
				{
					AfxMessageBox("PC RAM is NOT updated from the file");
				}
				else
				{
					AfxMessageBox("文件更新错误: Sorry: PC File NOT loaded"); // 随机存储(断电则遗失)从
				}
			}
			else
			{  // 20120518
				mtn_dll_wb_tune_initialization();

			}
//		}
	}
}

extern void mtnapi_get_debug_message(char strDebugMessageInitPara[512]);
static char strDebugMessage[512];
// IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD
void MtnParameterDlg::OnBnClickedButtonParameterUploadFromCtrlBoard()
{
	char strErrMsg[1024];
	int iRet = 0;
	iRet = mtnapi_upload_servo_parameter_acs(stServoControllerCommSet.Handle);
	if(iRet != MTN_API_OK_ZERO)
	{
		mtnapi_get_debug_message(strDebugMessage);
		sprintf_s(strErrMsg, 128, "Error upload: %d -- %s", iRet, strDebugMessage);
		AfxMessageBox((LPCSTR)strErrMsg);
	}
	else
	{
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			AfxMessageBox("PC RAM is updated from the Control Card");
		}
		else
		{
			AfxMessageBox("PC 随机存储(断电则遗失)从控制卡更新");
		}
	}
}
// IDC_BUTTON_PARA_DOWNLOAD
void MtnParameterDlg::OnBnClickedButtonParaDownload()
{
	if(mtnapi_download_servo_all_axis_acs() == MTN_API_OK_ZERO)
	{
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			AfxMessageBox("ACS-Controller RAM is updated from the PC");
		}
		else
		{
			AfxMessageBox("控制卡随机存储(断电则遗失)从PC更新");
		}
	}
}
// IDC_BUTTON_PARAMETER_SAVE_FILE
void MtnParameterDlg::OnBnClickedButtonParameterSaveFile()
{
	if(mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle) != MTN_API_OK_ZERO)
	{
		if(get_sys_language_option() == LANGUAGE_UI_EN)
		{
			AfxMessageBox("Error!!! Parameter in PC RAM NOT saved ");
		}
		else
		{
			AfxMessageBox("Error!!! PC 随机存储写入下面文件(断电保存)");
		}
	}
}
extern void mtnapi_get_debug_message_home_acs(char strDebugMessageHomeAcs[512]);

// IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS
void MtnParameterDlg::OnBnClickedButtonParameterDatabaseHomeAllAxis()
{
static char strDebugText[768];
	// For Home and safety position
	//	Initialize parameter just before using
	if(get_sys_machine_type_flag() == WB_MACH_TYPE_VLED_FORK)   // machine type dependency Item-11
	{
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	if(mtn_dll_bonder_xyz_start_search_limit_go_home(stServoControllerCommSet.Handle, APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z()) == MTN_API_ERROR)
	{
		mtnapi_get_debug_message_home_acs(strDebugMessage);
		sprintf_s(strDebugText, 768, "Error: %s", strDebugMessage);
		AfxMessageBox(strDebugText);
	}

}
// IDC_BUTTON_PARA_INIT_DOWNLOAD
void MtnParameterDlg::OnBnClickedButtonParaInitDownload()
{
	// TODO: Add your control notification handler code here
}

void MtnParameterDlg::SetUserInterfaceLanguage(int iLanguageOption)
{
	// IDC_BUTTON_PARAMETER_INIT_FROM_FILE
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_BUTTON_PARAMETER_INIT_FROM_FILE)->SetWindowTextA(_T("Init from file"));
		GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->SetWindowTextA(_T("Upload from bd"));
		GetDlgItem(IDC_BUTTON_PARAMETER_SAVE_FILE)->SetWindowTextA(_T("Save to file"));
		GetDlgItem(IDC_BUTTON_PARA_DOWNLOAD)->SetWindowTextA(_T("Download to bd"));

		//
		GetDlgItem(IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS)->SetWindowTextA(_T("HomeAllAxis"));
		GetDlgItem(IDC_BUTTON_PARA_INIT_DOWNLOAD)->SetWindowTextA(_T("InitDownload"));
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_ALL_START)->SetWindowTextA(_T("TuneAll"));               // TuneAll
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_START_TUNE)->SetWindowTextA(_T("TuneOnce"));               // TuneOnce
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_STOP_TUNE)->SetWindowTextA(_T("Stop"));               // 
		GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W)->SetWindowTextA(_T("Show"));               // 
		GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT)->SetWindowTextA(_T("ActualContact"));               // 
		GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION)->SetWindowTextA(_T("B1W Motion"));               // 20110521
	}
	else
	{
		GetDlgItem(IDC_BUTTON_PARAMETER_INIT_FROM_FILE)->SetWindowTextA(_T("读下文件到内存"));         // Init from file
		GetDlgItem(IDC_BUTTON_PARAMETER_UPLOAD_FROM_CTRL_BOARD)->SetWindowTextA(_T("从控制卡内存上传参数")); // Upload from bd
		GetDlgItem(IDC_BUTTON_PARAMETER_SAVE_FILE)->SetWindowTextA(_T("参数存下文件")); // Save to file
		GetDlgItem(IDC_BUTTON_PARA_DOWNLOAD)->SetWindowTextA(_T("下载参数控制卡内存")); // Download to bd

		//
		GetDlgItem(IDC_BUTTON_PARAMETER_DATABASE_HOME_ALL_AXIS)->SetWindowTextA(_T("ＸＹＺ归零")); // Init_Download
		GetDlgItem(IDC_BUTTON_PARA_INIT_DOWNLOAD)->SetWindowTextA(_T("初始下传"));               // HomeAll
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_ALL_START)->SetWindowTextA(_T("调节全部"));               // TuneAll
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_START_TUNE)->SetWindowTextA(_T("调节单次"));               // TuneOnce
		GetDlgItem(IDC_BUTTON_PARAMETER_DLG_STOP_TUNE)->SetWindowTextA(_T("停止调节"));               // 
		GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W)->SetWindowTextA(_T("显示"));               // 
		GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT)->SetWindowTextA(_T("有压力控制"));               // 
		GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION)->SetWindowTextA(_T("运动轨迹"));               // 20110521
	}
}


// XLS class
#ifdef __XLS_BY_OLE__
#include "CWorkbook.h"
static CWorkbook *pcXls_ACS_Para_OLE;

#else
//#include "XLEzAutomation.h"
///class CXLEzAutomation;
//static CXLEzAutomation *pcXLS_ACS_Parameter;

#endif

extern SERVO_ACS stServoACS;

#define COL_BASIC_PARA_ACS_AXIS_TABLE_X	 4
#define COL_BASIC_PARA_ACS_AXIS_TABLE_Y	 5
#define COL_BASIC_PARA_ACS_AXIS_BONDER_Z	 6
#define COL_BASIC_PARA_ACS_AXIS_WIRE_CLAMP	 7

#define ROW_BASIC_PARA_ACS_MOTOR_FLAG    8

int mtnapi_write_acs_para_into_xls_file_ptr()
{
	int iRet = MTN_API_OK_ZERO;

	return iRet;
}

#define DEFAULT_ACS_CONFIG_FILE_PATH	"C:\\WbData\\ACS_Parameter.xls"
int mtnapi_save_acs_para_into_def_xls_file()
{
	int iRet = MTN_API_OK_ZERO;
	CString cstrXlsFilepath;
	cstrXlsFilepath = _T(DEFAULT_ACS_CONFIG_FILE_PATH);


//RETURN_SAVE_ACS_PARA_INTO_DEF_XLS_FILE:
	return iRet;
}

// IDC_BUTTON_PARAMETER_SAVE_XLS
void MtnParameterDlg::OnBnClickedButtonParameterSaveXls()
{
	mtnapi_save_acs_para_into_def_xls_file();
}


#define __LEN_FILENAME__    128
static char strTuningOutputParaFilename[__LEN_FILENAME__];

// IDC_COMBO_PARAMETER_TUNING_AXIS
void MtnParameterDlg::OnCbnSelchangeComboParameterTuningAxis()
{
	CComboBox *pAxisComboAxisWB = (CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_TUNING_AXIS);
	int iCurrAxisServoParaTuning = pAxisComboAxisWB->GetCurSel(); mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(iCurrAxisServoParaTuning);
	((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_DEBUG_FLAG))->SetCheck(astMtnTuneInput[iCurrAxisServoParaTuning][mtn_wb_tune_get_blk_move_posn_set()].iDebugFlag);
	((CComboBox*)GetDlgItem(IDC_COMBO_PARAMETER_B1W_CTRL_BLK))->SetCurSel(astMtnTuneInput[iCurrAxisServoParaTuning][mtn_wb_tune_get_blk_move_posn_set()].stTuningTheme.iThemeType);
	UpdateTuningBlkCurrAxis();	//UpdateTuningParameterCurrentAxis();
	UpdateParameterTuneOutputCurrentAxis();

}
// IDC_BUTTON_PARAMETER_DLG_START_TUNE
void MtnParameterDlg::OnBnClickedButtonParameterDlgStartTune()
{
	cFlagTuningRoutine = SERVO_PARA_TUNING_ROUTINE_ONE_CASE;
	mtn_tune_make_output_folder(0); // 20120522, WB_AXIS_TABLE_X, WB_AXIS_TABLE_Y
	StartServoTuneThread();
	EnableTuningUI_Combo_Button(FALSE);
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_ALL_START --- Tune All
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneAllStart()
{
	cFlagTuningRoutine = SERVO_PARA_TUNING_ROUTINE_AUTO_FULL;
	mtn_tune_make_output_folder(0); // 20120522, WB_AXIS_TABLE_X, WB_AXIS_TABLE_Y
	StartServoTuneThread();
	EnableTuningUI_Combo_Button(FALSE);
}

// IDC_BUTTON_PARAMETER_DLG_STOP_TUNE
void MtnParameterDlg::OnBnClickedButtonParameterDlgStopTune()
{
	StopServoTuneThread();
} 	

UINT ServoParaTuningThreadInDialog( LPVOID pParam )
{
    MtnParameterDlg* pObject = (MtnParameterDlg *)pParam;
	return pObject->DlgServoParaTuningThread(); 	
}

void MtnParameterDlg::StartServoTuneThread()
{
	mtn_wb_tune_thread_set_flag_running(TRUE);
	pDlgServoParaTuningWinThread = AfxBeginThread(ServoParaTuningThreadInDialog, this); // , THREAD_PRIORITY_TIME_CRITICAL);
	pDlgServoParaTuningWinThread->m_bAutoDelete = FALSE;
}

void MtnParameterDlg::StopServoTuneThread()
{
	mtn_dll_wb_tune_servo_stop_thread();
	Sleep(500);
	mMotionTuning.StopTuneCompleteMesh();
	mtn_wb_tune_thread_set_flag_running(FALSE);  // 20120117
	mtn_wb_tune_b1w_stop();
	Sleep(500);
	//mtn_dll_wb_servo_tune_set_stop_flag(TRUE); // 
	UpdateParameterTuneOutputCurrentAxis();
}

extern void mtn_wb_tune_save_group_para_bef_tuning();
extern void mtn_wb_tune_save_group_tuning_output(char *strFilename, int iLenChar);
extern int mtn_wb_tune_xyz_multi_cases(HANDLE stCommHandle);
extern int mtn_wb_bh_verify_consolidate_save_para_tune_out();
#include "acs_buff_prog.h"

#include "ForceCali.h"
#define FILE_NAME_SEARCH_CONTACT_INI_SRCH  "B1W_SrchContact.m"

// extern int iFlagStopThread_ServoTuning;
UINT MtnParameterDlg::DlgServoParaTuningThread()
{
CString cstrTemp;
int iRet = MTN_API_OK_ZERO;
double dBondHeadSafeUpperPosition; 
	dBondHeadSafeUpperPosition = mtn_wb_init_bh_relax_position_from_sp(stServoControllerCommSet.Handle);  // mtn_wb_get_bh_upper_limit_position();  // 20110711
	mtn_wb_tune_save_group_para_bef_tuning();

	if(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_ONE_CASE)
	{
		mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle); // SaveCurrent Parameters to "C:\\WbData\\ParaBase\\" , 20110425
		if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X|| mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_Y)
		{
			acsc_ToPoint(stServoControllerCommSet.Handle, 0, // start up the motion immediately
				sys_get_acs_axis_id_bnd_z(), dBondHeadSafeUpperPosition, NULL);  // astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET].stTuningCondition.dMovePointion_2 // 20110711
			while(mtn_qc_is_axis_still_moving(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z()) == TRUE){		Sleep(1);}
		}
		mMotionTuning.mtn_tune_set_tune_axis(astAxisInfoWireBond[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()].iAxisInCtrlCardACS);
		if(astAxisInfoWireBond[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()].iAxisInCtrlCardACS == sys_get_acs_axis_id_bnd_z())
		{		// MOVE XY-Table to SafePosition, if tuing Axis-BondHead
			mtn_wb_table_servo_go_to_safety_position(stServoControllerCommSet.Handle, APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID);
		}
		if(iServoParaTuningAlgoOption == SERVO_PARA_TUNE_ALGO_COMPLETE_SEARCH)
		{
			mMotionTuning.mtn_tune_axis_complete_mesh(&astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()],
				&astMtnTuneOutput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()]);
		}
		else // SERVO_PARA_TUNE_ALGO_1_DIM_AFT_1
		{
			mMotionTuning.mtn_tune_axis_one_by_one_para(&astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()],
				&astMtnTuneOutput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()]);
		}
		if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_BOND_Z)
		{
			acsc_ToPoint(stServoControllerCommSet.Handle, 0, // start up the motion immediately
				sys_get_acs_axis_id_bnd_z(), dBondHeadSafeUpperPosition, NULL);  // astMtnTuneInput[WB_AXIS_BOND_Z][WB_BH_RESET].stTuningCondition.dMovePointion_2, 20110711
			while(mtn_qc_is_axis_still_moving(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z()) == TRUE){		Sleep(1);}
		}
	}
	else if(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W)
	{
		mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle); // SaveCurrent Parameters to "C:\\WbData\\ParaBase\\" , 20110425
		mtn_wb_tune_b1w_stop_srch_init_cfg(stServoControllerCommSet.Handle);
		mtn_wb_tune_b1w_stop_srch_download_cfg();
		mtn_wb_dll_download_acs_servo_speed_parameter_acs(stServoControllerCommSet.Handle);

		//// Search Upper Limit and Home, // 20120802, AutoTuning B1W
		mtn_dll_axis_search_limit_go_home(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z());
		int iPosnUpperLimitBH = (int)mtn_wb_get_bh_upper_limit_position();
		double dLowLimitPosn;

		acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();
		Sleep(10);

		//// Teach Contact
#ifdef __TEACH_CONTACT_BY_ACS_BUFF_PROG__
		TEACH_CONTACT_INPUT stTeachContactParameter;
		TEACH_CONTACT_OUTPUT stTeachContactResult;
		acsc_upload_search_contact_parameter(stServoControllerCommSet.Handle, &stTeachContactParameter);
		mtnapi_get_speed_profile(stServoControllerCommSet.Handle, stTeachContactParameter.iAxis, &(stTeachContactParameter.stSpeedProfileTeachContact), 0);
		stTeachContactParameter.iFlagSwitchToForceControl = 0;
		stTeachContactParameter.iResetPosition = (int)mtn_wb_init_bh_relax_position_from_sp(stServoControllerCommSet.Handle);
		stTeachContactParameter.iSearchHeightPosition = stTeachContactParameter.iResetPosition - 2000;
		stTeachContactParameter.iSearchTolPE = 30;
		stTeachContactParameter.iSearchVel = -15000;
		stTeachContactParameter.iMaxDist = 15000;   
		stTeachContactParameter.iStartVel = -200000;
		stTeachContactParameter.iAntiBounce = 12;
		stTeachContactParameter.iTimeOut= 20000;
		stTeachContactParameter.iFlagDebug= 0;
		stTeachContactParameter.iMaxJerkMoveSrchHt = 10; // 100km/s^3
		stTeachContactParameter.iMaxAccMoveSrchHt = 600;   // 600m/s^2

		mtn_teach_contact_acs(stServoControllerCommSet.Handle, &stTeachContactParameter, &stTeachContactResult);

		gstSystemScope.uiNumData = 7;
		gstSystemScope.uiDataLen = 3000;
		gstSystemScope.dSamplePeriod_ms = 1;
		mtn_tune_contact_save_data(stServoControllerCommSet.Handle, &gstSystemScope, stTeachContactParameter.iAxis, 
				FILE_NAME_SEARCH_CONTACT_INI_SRCH, "%% ACSC Controller, Axis- %d: RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n");

		dLowLimitPosn = stTeachContactResult.iContactPosnReg;
#else // 

		
		if(aft_teach_lower_limit(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z(), 
			0, &dLowLimitPosn) != MTN_API_OK_ZERO)
		{
			return 0;
		}

#endif  // __TEACH_CONTACT_BY_ACS_BUFF_PROG__
		
		cstrTemp.Format("Contact Position: %6.0f; UpperLimit: %d; Attention to protect capillary, OK to proceed;", 
			dLowLimitPosn, iPosnUpperLimitBH);
		AfxMessageBox(cstrTemp);


		MTUNE_OUT_POSN_COMPENSATION stPosnCompensationTuneOutputB1W; // 20120802, AutoTuning B1W
		MTN_TUNE_POSN_COMPENSATION stPosnCompensationTuneB1W;

		stPosnCompensationTuneB1W.dLowerPosnLimit = dLowLimitPosn + 1000.0;  // stTeachContactResult.iContactPosnReg
		stPosnCompensationTuneB1W.dUpperPosnLimit = iPosnUpperLimitBH - 1000.0;
		stPosnCompensationTuneB1W.uiTotalPoints = 8;
		stPosnCompensationTuneB1W.dPositionFactor = 0;
		stPosnCompensationTuneB1W.dCtrlOutOffset = 0;
		stPosnCompensationTuneB1W.uiGetStartingDateLen = 200;
		stPosnCompensationTuneB1W.uiSleepInterAction_ms = 250;
		mtune_position_compensation(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z(), &stPosnCompensationTuneB1W, &stPosnCompensationTuneOutputB1W);
		// 20120802, AutoTuning B1W

		mMotionTuning.mtn_tune_set_tune_axis(sys_get_acs_axis_id_bnd_z()); mtn_wb_tune_set_curr_wb_axis_servo_para_tuning(WB_AXIS_BOND_Z);

		cstrTemp = AutoTuneBondHeadB1W_StopSrch(); // AutoTuneBondHeadB1W();

		iRet = mtn_wb_bh_verify_consolidate_save_para_tune_out();

		mtn_tune_save_servo_parameter_b1w_folder(stServoControllerCommSet.Handle); // 20120728

		if(mtn_qc_is_axis_locked_safe(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z()) == TRUE)
		{
			iRet = MTN_API_OK_ZERO;
		}
		else
		{
			iRet = MTN_API_ERROR_TUNING_NOT_ALL_PASS;
			goto label_return_DlgServoParaTuningThread;
		}

		GetDlgItem(IDC_STATIC_SERVO_PARA_TUNE_STATUS)->SetWindowTextA(cstrTemp);

		Sleep(500);
	}
	else if(cFlagTuningRoutine != 0)
	{
		// 20110920
		if(iFlagServoTuningByDLL == 0)
		{
			mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle); // SaveCurrent Parameters to "C:\\WbData\\ParaBase\\" , 20110425
			iRet = mtn_wb_tune_xyz_multi_cases(stServoControllerCommSet.Handle);
		}
		else // 20120117
		{

			UINT uiBitFlagComboTuningThread = 0;

			if(cTuneBlocks_FlagTableX)
			{
				uiBitFlagComboTuningThread = uiBitFlagComboTuningThread | WB_SERVO_TUNING_BIT_TUNE_2POINTS_X;
			}
			if(cTuneBlocks_FlagTableY)
			{
				uiBitFlagComboTuningThread = uiBitFlagComboTuningThread | WB_SERVO_TUNING_BIT_TUNE_2POINTS_Y;
			}
			if(cTuneBlocks_FlagBondHeadZ)
			{
				uiBitFlagComboTuningThread = uiBitFlagComboTuningThread | WB_SERVO_TUNING_BIT_TUNE_2POINTS_Z;
			}
			if(iFlagTableSectorTuning)
			{
				uiBitFlagComboTuningThread = uiBitFlagComboTuningThread | WB_SERVO_TUNING_BIT_TUNE_SECTOR_XY;
			}
			mtn_dll_wb_tune_servo_trigger_move_2points_thread(uiBitFlagComboTuningThread);
			Sleep(1000);
			int iFlagStopThread_ServoTuningTemp;
			iFlagStopThread_ServoTuningTemp = mtn_dll_wb_servo_tune_get_stop_flag();
			while(iFlagStopThread_ServoTuningTemp == FALSE)
			{
				Sleep(1000);
				iFlagStopThread_ServoTuningTemp = mtn_dll_wb_servo_tune_get_stop_flag();
			}
		}
//		mtn_tune_save_servo_parameter_xy_folder(stServoControllerCommSet.Handle); // 20120728, stCommHandle
	}

	mtn_wb_tune_save_group_tuning_output(strTuningOutputParaFilename, __LEN_FILENAME__);

label_return_DlgServoParaTuningThread:
	mtn_wb_tune_thread_set_flag_running(FALSE);
	UpdateParameterTuneOutputCurrentAxis();
	GetDlgItem(IDC_STATIC_PARAMETER_DATABASE_MASTER_FILE_PATH_NAME)->SetWindowTextA(_T(strTuningOutputParaFilename));
	acs_bufprog_0_stop_();

	
	if(iRet != MTN_API_OK_ZERO)
	{
		if(mtn_api_prompt_acs_error_message(stServoControllerCommSet.Handle))
		{
			cstrTemp = (_T(mtn_api_get_acs_error_string()));
			AfxMessageBox(cstrTemp);
		}
		Beep(523, 2000); Beep(587, 2000); Beep(659, 2000); 
	}
	else
	{
		mtn_dll_music_logo_normal_stop(); // 20120217  // Logo Normal Exit 3-2-1
		int iFlagUpdateFile;

		if(get_sys_language_option() == LANGUAGE_UI_CN)
		{
			iFlagUpdateFile = AfxMessageBox(_T("调节完毕，请根据结果更新伺服参数。"), MB_YESNO);
		}
		else
		{
			iFlagUpdateFile = AfxMessageBox(_T("Tuning OK. Please update parameters accordingly."), MB_YESNO);
		}

		// cTuneBlocks_FlagTableX == TRUE &&
		if(iFlagUpdateFile == IDYES && 
			(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL ||   // cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL ||
			 cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W )
		   )
		{
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_x.ini", "C:\\WbData\\DefParaBase\\ctrl_acsc_x.ini", 0);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_y.ini", "C:\\WbData\\DefParaBase\\ctrl_acsc_y.ini", 0);
			CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini", "C:\\WbData\\DefParaBase\\ctrl_acsc_z.ini", 0);
		}
		else if(iFlagUpdateFile == IDYES && cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W && iRet == MTN_API_ERROR_TUNING_NOT_ALL_PASS)
		{
			AfxMessageBox(_T("请手动更新参数, Update parameters manually \n\r D:\\MT\\PutPassParametersTo_C_WbData_DefParaBase_ctrl_acsc_z.ini to C:\\WbData\\DefParaBase\\ctrl_acsc_z.ini"));
		}

		// cTuneBlocks_FlagTableX == TRUE &&
		if(iFlagUpdateFile == IDYES && 
			(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL ||  // cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_FULL ||
			 cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W )
		   )
		{
			if(cFlagTuningRoutine == SERVO_PARA_TUNING_ROUTINE_AUTO_B1W)
			{
				// Update ACS-Buffer
				acs_clear_buffer_7_from_ram_para_();

				int iMechCfg = get_sys_machine_type_flag();		// 20111214
				acs_update_buffer_7_from_ram_para_(iMechCfg);	// 20111214
				acs_compile_start_buffer_7();  // 20110603

				// Save JerkFF to ACS-Flash				// 20110603
				char strSaveBuffer7[16] = "#SAVEPROG 7\r";
				for(int ii=0; ii<3; ii++)
				{
					if(acsc_Send(stServoControllerCommSet.Handle, strSaveBuffer7, strlen(strSaveBuffer7), NULL))
					{
						Sleep(500);
					}
				}
			}
			AfxMessageBox(_T("WireBonder.exe -> 调试 -> Diagnostic Tools诊断工具 -> Load (Servo) Control Parameter -> Execute"));
		}
	}

	UpdateB1W_ParaBndZ_TextEditBox(); // 20120522
	EnableUI_B1W_EditOnly_ParaBndZ(TRUE); // 20120522

	cFlagTuningRoutine = FALSE;

	return 0;
}

// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVkp()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKP, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKP);
}
// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVki()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VKI, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopKI);
}
// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundPkp()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_PKP, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dPosnLoopKP);
}
// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundSof()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_SOF, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dSecOrdFilterFreq_Hz);
}
// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundVli()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_VLI, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dVelLoopLimitI);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVli()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VLI, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopLimitI);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundSof()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_SOF, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dSecOrdFilterFreq_Hz);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundPkp()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_PKP, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dPosnLoopKP);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVki()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKI, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKI);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundVkp()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_VKP, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dVelLoopKP);
}
// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundAffc()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_AFFC, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterLowBound.stMtnPara.dAccFFC);
}
// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundAffc()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_AFFC, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningParameterUppBound.stMtnPara.dAccFFC);
}

// 20120522
// IDC_STATIC_PARAMETER_DLG_OUTPUT_VKP
// IDC_STATIC_PARAMETER_DLG_OUTPUT_VKI
// IDC_STATIC_PARAMETER_DLG_OUTPUT_PKP
// IDC_STATIC_PARAMETER_DLG_OUTPUT_ACCFFC
// IDC_STATIC_PARAMETER_DLG_OUTPUT_SOF_FREQ
// IDC_STATIC_PARAMETER_DLG_OUTPUT_VLI
static	CTRL_PARA_ACS stServoLoopAcsCtrlParaDlg; // 20090831
void MtnParameterDlg::UpdateServoParameterDuringTune()
{
	int idxCurrTuningWbAxis = mtn_wb_tune_get_curr_wb_axis_servo_para_tuning();
	if(iFlagDebug_SaveFile_UIProtPass == TRUE)
	{
		if(idxCurrTuningWbAxis ==  WB_AXIS_TABLE_X
			|| idxCurrTuningWbAxis == WB_AXIS_TABLE_Y)
		{
			mtnapi_upload_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, 
				astMtnTuneInput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].iAxisTuning, &stServoLoopAcsCtrlParaDlg); // 20090930
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKP, stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_PKP, stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_ACCFFC, stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_SOF_FREQ, stServoLoopAcsCtrlParaDlg.dSecondOrderLowPassFilterBandwidth, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VLI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorLimit, "%5.1f");
		}
		else if(idxCurrTuningWbAxis == WB_AXIS_BOND_Z)
		{
			MTN_TUNE_PARAMETER_SET stTuneParaSet;
			int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
			mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
			stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain = stTuneParaSet.dVelLoopKP;
			stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain = stTuneParaSet.dVelLoopKI;
			stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain = stTuneParaSet.dPosnLoopKP;
			stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward = stTuneParaSet.dAccFFC;

			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKP, stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_PKP, stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_ACCFFC, stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_SOF_FREQ, stServoLoopAcsCtrlParaDlg.dSecondOrderLowPassFilterBandwidth, "%5.1f");
			UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VLI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorLimit, "%5.1f");
		}
	}
}

void MtnParameterDlg::UpdateParameterTuneOutputCurrentAxis()
{
	int idxCurrTuningWbAxis = mtn_wb_tune_get_curr_wb_axis_servo_para_tuning();

	if(idxCurrTuningWbAxis ==  WB_AXIS_TABLE_X
		|| idxCurrTuningWbAxis == WB_AXIS_TABLE_Y)
	{
		stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dVelLoopKP;
		stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dVelLoopKI;
		stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dPosnLoopKP;
		stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dAccFFC;
		stServoLoopAcsCtrlParaDlg.dSecondOrderLowPassFilterBandwidth = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dSecOrdFilterFreq_Hz;
		stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorLimit = astMtnTuneOutput[idxCurrTuningWbAxis][mtn_wb_tune_get_blk_move_posn_set()].stBestParameterSet.dVelLoopLimitI;
	}
	else if(idxCurrTuningWbAxis == WB_AXIS_BOND_Z)
	{
		MTN_TUNE_PARAMETER_SET stTuneParaSet;
		int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
		mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
		stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain = stTuneParaSet.dVelLoopKP;
		stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain = stTuneParaSet.dVelLoopKI;
		stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain = stTuneParaSet.dPosnLoopKP;
		stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward = stTuneParaSet.dAccFFC;
	}

	if(idxCurrTuningWbAxis == WB_AXIS_TABLE_X
		|| idxCurrTuningWbAxis == WB_AXIS_TABLE_Y
		|| idxCurrTuningWbAxis == WB_AXIS_BOND_Z
		)
	{
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKP, stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain, "%5.1f");
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VKI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain, "%5.1f");
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_PKP, stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain, "%5.1f");
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_ACCFFC, stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward, "%5.1f");
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_SOF_FREQ, stServoLoopAcsCtrlParaDlg.dSecondOrderLowPassFilterBandwidth, "%5.1f");
		UpdateDoubleToEdit(IDC_STATIC_PARAMETER_DLG_OUTPUT_VLI, stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorLimit, "%5.1f");
	}
}

// IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN
void MtnParameterDlg::OnEnKillfocusEditParameterDlgLowBoundTunePosn()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_LOW_BOUND_TUNE_POSN, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePosition_1); 
}

// IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN
void MtnParameterDlg::OnEnKillfocusEditParameterDlgUppBoundTunePosn()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_UPP_BOUND_TUNE_POSN, &astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningCondition.dMovePointion_2);
}

// IDC_COMBO_PARAMETER_B1W_CTRL_BLK
void MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTheme()
{
	astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].stTuningTheme.iThemeType = 
		((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_B1W_CTRL_BLK))->GetCurSel();

	UpdateB1W_ParaBndZ_TextEditBox(); // 20120522
}
// IDC_COMBO_PARAMETER_AXIS_TUNING_ALGO
void MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningAlgo()
{
	iServoParaTuningAlgoOption = ((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_ALGO))->GetCurSel();
}
// IDC_CHECK_PARAMETER_DLG_TUNE_DEBUG_FLAG
#include "DlgKeyInputPad.h"
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneDebugFlag()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_DEBUG_FLAG))->GetCheck();
	if(iTemp == TRUE)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_zzy()) // wb_mtn_tester_get_password_sg_8d_zzy()
			{
				iFlagFailurePass = FALSE;
			}
			else
			{
				iFlagFailurePass = TRUE;
			}
		}
		else
		{
			iFlagFailurePass = TRUE;
		}
		if(iFlagFailurePass == TRUE)
		{
			iTemp = FALSE;
			((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_DEBUG_FLAG))->SetCheck(FALSE);
		}
	}
	else
	{
		EnableTuningUI_Edit_Button_ByPass(iTemp);
	}
	iFlagDebug_SaveFile_UIProtPass = iTemp;
	astMtnTuneInput[mtn_wb_tune_get_curr_wb_axis_servo_para_tuning()][mtn_wb_tune_get_blk_move_posn_set()].iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
	stMtnTuneInputTableX_DRA.iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
	stMtnTuneInputTableY_DRA.iDebugFlag = iFlagDebug_SaveFile_UIProtPass;
}
// IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK, BondHead
void MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningPosnBlk()
{
	idxTuningBlockBondHead = ((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_POSN_BLK))->GetCurSel();
	UpdateTuningBlkCurrAxis();
	UpdateB1W_ParaBndZ_TextEditBox(); // 20120522

}
// IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK
void MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTablePosnBlk()
{
	idxTuningBlockTable = ((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_BLK))->GetCurSel();
	UpdateTuningBlkCurrAxis();
}

// IDC_STATIC_PARAMETER_DLG_SPEED_MAX_VEL
// IDC_STATIC_PARAMETER_DLG_SPEED_MAX_ACC
// IDC_STATIC_PARAMETER_DLG_SPEED_MAX_JERK
// IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE
void MtnParameterDlg::OnCbnSelchangeComboParameterAxisTuningTablePosnInRange()
{

	idxCurrSectorTable = ((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE))->GetCurSel();
	unsigned int uiMaxNumSector;
	if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_X)
	{
		mtn_wb_tune_set_max_sector_table_x(idxCurrSectorTable);  // 20110219
		uiMaxNumSector = mtn_wb_tune_get_max_sector_table_x();
	}
	else if(mtn_wb_tune_get_curr_wb_axis_servo_para_tuning() == WB_AXIS_TABLE_Y)
	{
		uiMaxNumSector = mtn_wb_tune_get_max_sector_table_y();
	}
	if(idxCurrSectorTable > uiMaxNumSector)
	{
		idxCurrSectorTable = uiMaxNumSector - 1;
		((CComboBox *)GetDlgItem(IDC_COMBO_PARAMETER_AXIS_TUNING_TABLE_POSN_IN_RANGE))->SetCurSel(idxCurrSectorTable);
	}
	UpdateTuningBlkCurrAxis();
}
// IDC_CHECK_TUNE_FLAG_TABLE_SECTOR
void MtnParameterDlg::OnBnClickedCheckTuneFlagTableSector()
{
	iFlagTableSectorTuning = ((CButton *)GetDlgItem(IDC_CHECK_TUNE_FLAG_TABLE_SECTOR))->GetCheck();
}

static BUFFER_DATA_WB_TUNE stBufferDataEmuB1W;
static WB_ONE_WIRE_PERFORMANCE_CALC astWbEmuB1WPerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE];
//extern WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE];

void MtnParameterDlg::PrepareStartThreadB1W()
{
	mtn_tune_make_output_folder(WB_AXIS_BOND_Z); // 20120522
	cFlagTuningRoutine = SERVO_PARA_TUNING_ROUTINE_AUTO_B1W;
	unsigned int ii;
	for(ii = 0; ii< MAX_BLK_PARAMETER; ii++)
	{
		astMtnTuneInput[WB_AXIS_BOND_Z][ii].stTuningTheme.iThemeType = THEME_WB_B1W;
	}
	acs_bufprog_0_run_();
	mtn_dll_music_logo_normal_start();  // 20120217 // Logo Normal Start 1-2-3, 20120217

	StartServoTuneThread();
}
void B1W_z_once_teach_contact_1st()
{
static int iFlag1stTimeRunB1W = 1;

//	mtnapi_dll_save_wb_speed_servo_parameter_acs(stServoControllerCommSet.Handle); // SaveCurrent Parameters to "C:\\WbData\\ParaBase\\" , 20110425
//	mtn_wb_dll_download_acs_servo_speed_parameter_acs(stServoControllerCommSet.Handle);
	acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();

	double dLowLimitPosn;
	if(iFlag1stTimeRunB1W == 1)
	{
		CString cstrTemp;
		iFlag1stTimeRunB1W = 0;
		//// Search Upper Limit and Home, // 20120802, AutoTuning B1W
		mtn_dll_axis_search_limit_go_home(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z());
		int iPosnUpperLimitBH = (int)mtn_wb_get_bh_upper_limit_position();

		if(aft_teach_lower_limit(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z(), 
			0, &dLowLimitPosn) != MTN_API_OK_ZERO)
		{
			return ;
		}
		
		cstrTemp.Format("Contact Position: %6.0f; UpperLimit: %d; Attention to protect capillary, OK to proceed;", 
			dLowLimitPosn, iPosnUpperLimitBH);
		AfxMessageBox(cstrTemp);

		mtn_wb_tune_b1w_stop_srch_init_cfg(stServoControllerCommSet.Handle);
		if(mtn_wb_tune_b1w_check_valid_search_height() == FALSE)  // 20121220
		{
			mtn_wb_tune_b1w_update_search_height_posn_by_low_limit(stServoControllerCommSet.Handle, (int)dLowLimitPosn);
			mtn_wb_tune_b1w_init_search_cfg();
		}  // 20121220
		mtn_wb_tune_b1w_stop_srch_download_cfg(); // 20121220

		MTUNE_OUT_POSN_COMPENSATION stPosnCompensationTuneOutputB1W; // 20120802, AutoTuning B1W
		MTN_TUNE_POSN_COMPENSATION stPosnCompensationTuneB1W;

		stPosnCompensationTuneB1W.dLowerPosnLimit = dLowLimitPosn + 1000.0;  // stTeachContactResult.iContactPosnReg
		stPosnCompensationTuneB1W.dUpperPosnLimit = iPosnUpperLimitBH - 1000.0;
		stPosnCompensationTuneB1W.uiTotalPoints = 8;
		stPosnCompensationTuneB1W.dPositionFactor = 0;
		stPosnCompensationTuneB1W.dCtrlOutOffset = 0;
		stPosnCompensationTuneB1W.uiGetStartingDateLen = 200;
		stPosnCompensationTuneB1W.uiSleepInterAction_ms = 250;
		mtune_position_compensation(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z(), &stPosnCompensationTuneB1W, &stPosnCompensationTuneOutputB1W);
	}
	else
	{
		Sleep(10);
	}

	mtn_wb_tune_b1w_stop_srch_trig_once(stServoControllerCommSet.Handle);
	while(qc_is_axis_still_acc_dec(stServoControllerCommSet.Handle, sys_get_acs_axis_id_bnd_z()))
	{
		Sleep(10); 	//Sleep(2);
	}
}
// IDC_BUTTON_PARAMETER_DLG_RUN_LAST_B1W
void MtnParameterDlg::OnBnClickedButtonParameterDlgRunLastB1w()
{
//	stBufferDataEmuB1W.iBufferDataWbTune_DRY_RUN_FLAG = mtn_wb_tune_b1w_get_flag_contact(); //iFlagIsActualContact; 20110521
//	astWbOneWirePerformance[0].iFlagIsDryRun = mtn_wb_tune_b1w_get_flag_contact(); // iFlagIsActualContact; // >=1: contact, 0: dry-run, 20110521
	// Variable structure defined in DlgServoTuning.cpp, update from Buff0
//	mtn_tune_calc_bond_prog_by_one_wire_info(&astWbOneWirePerformance[0]);
//	mtn_tune_set_buff_data_wb_tune_st(&stBufferDataEmuB1W);
//	acs_bufprog_write_data_emu_b1w_buff_8();

	B1W_z_once_teach_contact_1st();
//	mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

	// Prepare and start B1W routine
	//PrepareStartThreadB1W();
#ifdef TYE_B1W
	int nTotalNumWire;
	// Start EmuB1W
	acs_bufprog_start_buff_8();
	int iFlagDoingTest;
	acsc_ReadInteger(stServoControllerCommSet.Handle, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
	while(iFlagDoingTest)
	{
  		Sleep(20);
		acsc_ReadInteger(stServoControllerCommSet.Handle, BUFFER_ID_EMU_B1W, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
	}

	// Upload waveform
	mtn_tune_upload_latest_wb_waveform(stServoControllerCommSet.Handle);
	// Calculate performance index
	
	mtn_tune_ana_wb_waveform(astWbEmuB1WPerformance, &nTotalNumWire);

	// update parameter
	// execute buffer program
#endif // TYE_B1W


}

// IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT
void MtnParameterDlg::OnBnClickedCheckTuneFlagB1wActualContact()
{
	int iFlagIsActualContact = ((CButton *) GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT))->GetCheck();  // 20110521
	mtn_wb_tune_b1w_set_flag_contact(iFlagIsActualContact);  // 20110521
	UI_UpdateCheckStatusB1W();
}

// IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W
void MtnParameterDlg::OnBnClickedCheckParaTuneDlgEnableB1w()
{
	iFlagDoPosnCompBefB1W_ParaTuneDlg = ((CButton *) GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W))->GetCheck();
#ifdef __ENABLE_CHECK_B1W__
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W))->GetCheck();
	if(iTemp == TRUE)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_gxc()) // wb_mtn_tester_get_password_sg_8d_zzy()
			{
				iFlagFailurePass = FALSE;
			}
			else
			{
				iFlagFailurePass = TRUE;
			}
		}
		else
		{
			iFlagFailurePass = TRUE;
		}
		if(iFlagFailurePass == TRUE)
		{
			iTemp = FALSE;
			((CButton *)GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W))->SetCheck(FALSE);
		}
	}
	else
	{
		UI_Show_TuneB1W(iTemp);
	}

	int nTotalNumWire = 0;
	iFlagDoPosnCompBefB1W_ParaTuneDlg = ((CButton *) GetDlgItem(IDC_CHECK_PARA_TUNE_DLG_FLAG_DO_POSN_COMP_B1W))->GetCheck();

	int iRetUpld = MTN_API_OK_ZERO;
	if(stServoControllerCommSet.Handle == ACSC_INVALID)  //  || stCommHandleACS == 0)
	{
		nTotalNumWire = 0;
		for(int ii=0; ii<NUM_TOTAL_WIRE_IN_ONE_SCOPE; ii++)
		{
			astWbEmuB1WPerformance[ii] = astWbOneWirePerformance[ii];
			if(astWbEmuB1WPerformance[ii].iFlagHasWireInfo == 0)
			{
				nTotalNumWire = ii;
				break;
			}
		}
	}
	else
	{
		iRetUpld = mtn_tune_upload_latest_wb_waveform(stServoControllerCommSet.Handle);
		mtn_tune_ana_wb_waveform(&astWbEmuB1WPerformance[0], &nTotalNumWire);
		astWbOneWirePerformance[0] = astWbEmuB1WPerformance[0];
	}
	if(iRetUpld == MTN_API_OK_ZERO)
	{
		if(nTotalNumWire  >= 1 && nTotalNumWire <= 4) // Double verify the data-check-sum
		{
			UI_Show_TuneB1W(iFlagDoPosnCompBefB1W_ParaTuneDlg);
			UpdateB1W_PerformanceButton();
			mtn_tune_get_wb_table_tuning_position_set(0, &(astWbEmuB1WPerformance[0].stPosnsOfTable));// 20110425

		}
		else
		{
			GetDlgItem(IDC_STATIC_GROUP_PARA_TUNE_DLG_ENABLE_B1W)->SetWindowTextA(_T("Wb-Tune, ErrorB1W_Init"));
		}
	}
	else
	{
		GetDlgItem(IDC_STATIC_GROUP_PARA_TUNE_DLG_ENABLE_B1W)->SetWindowTextA(_T("Wb-Tune, ErrorB1W_Init"));
	}
#endif // __ENABLE_CHECK_B1W__
}

// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B
extern int iFlagB1W_TuneIdle;
extern int iFlagB1W_TuneSrch1B;
extern int iFlagB1W_TuneMoveTo1B;
extern int iFlagB1W_TuneLooping ;
extern int iFlagB1W_TuneLoopTop ;
extern int iFlagB1W_TuneTraj2B ;
extern int iFlagB1W_TuneTail ;
extern int iFlagB1W_TuneReset;
extern int iFlagB1W_TuneSrch2B;

void MtnParameterDlg::UI_UpdateCheckStatusB1W()
{
		((CButton *) GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT))->SetCheck(mtn_wb_tune_b1w_get_flag_contact());  // iFlagIsActualContact
		((CButton *)GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION))->SetCheck(mtn_wb_tune_b1w_get_flag_motion());  // 20110520
		//////
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE))->SetCheck(iFlagB1W_TuneIdle);  // 20110523
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B))->SetCheck(iFlagB1W_TuneSrch1B);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B))->SetCheck(iFlagB1W_TuneMoveTo1B);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING))->SetCheck(iFlagB1W_TuneLooping);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP))->SetCheck(iFlagB1W_TuneLoopTop); 
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B))->SetCheck(iFlagB1W_TuneTraj2B);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL))->SetCheck(iFlagB1W_TuneTail);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET))->SetCheck(iFlagB1W_TuneReset);  
		((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B))->SetCheck(iFlagB1W_TuneSrch2B);  // 20110523
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE
void MtnParameterDlg::UI_Show_TuneB1W(int iFlagEnable)
{
	GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_ACTUAL_CONTACT)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_LAST_B1W)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION)->ShowWindow(iFlagEnable);               // 20110521

	// B1W Detail OneByOne Tuning
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE)->ShowWindow(iFlagEnable);               // 20110521
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B)->ShowWindow(iFlagEnable);   
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL)->ShowWindow(iFlagEnable); 
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET)->ShowWindow(iFlagEnable);
	GetDlgItem(IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B)->ShowWindow(iFlagEnable);        
            // 20110521
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET)->ShowWindow(iFlagEnable);        
	GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B)->ShowWindow(iFlagEnable);        

	if(iFlagEnable)
	{
		UI_UpdateCheckStatusB1W();
	}

}
void MtnParameterDlg::OnBnClickedCancel()
{
//	StopTimer();
	iFlagDoPosnCompBefB1W_ParaTuneDlg = 1;
	iDlgSrvoParameterTuneShowFlag = 0;  // 20120202	iDlgSrvoParameterTuneShowFlag = 0;
	OnCancel();
}

// IDC_BUTTON_PARAMETER_DLG_LOAD_TUNE_CFG
void MtnParameterDlg::OnBnClickedButtonParameterDlgLoadTuneCfg()
{
	if(mtnapi_init_master_config_acs(strServoSpeedFilenameInitParameterACS, stServoControllerCommSet.Handle) == MTN_API_OK_ZERO)
	{
		mtnapi_init_tune_para_bound();
	}
}

#include "MtnTune.h"

// IDC_BUTTON_PARAMETER_DLG_RUN_SYS_B1W
void MtnParameterDlg::OnBnClickedButtonParameterDlgRunSysB1w()
{
	WB_TUNE_B1W_BH_OBJ stWbTuneB1wObj;

	gstSystemScope.uiNumData = 7;
	gstSystemScope.uiDataLen = 3000;
	gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();

#ifndef __TEACH_BY_BUFFER__
	B1W_z_once_teach_contact_1st();
#else
	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		//// Teach Contact
		TEACH_CONTACT_INPUT stTeachContactParameter;
		TEACH_CONTACT_OUTPUT stTeachContactResult;
		acsc_upload_search_contact_parameter(stServoControllerCommSet.Handle, &stTeachContactParameter);
		mtnapi_get_speed_profile(stServoControllerCommSet.Handle, stTeachContactParameter.iAxis, &(stTeachContactParameter.stSpeedProfileTeachContact), 0);
		stTeachContactParameter.iFlagSwitchToForceControl = 0;
		stTeachContactParameter.iResetPosition = (int)mtn_wb_init_bh_relax_position_from_sp(stServoControllerCommSet.Handle);
		stTeachContactParameter.iSearchHeightPosition = stTeachContactParameter.iResetPosition - 2000;
		stTeachContactParameter.iSearchTolPE = 30;
		stTeachContactParameter.iSearchVel = -15000;
		stTeachContactParameter.iMaxDist = 11000;   
		stTeachContactParameter.iStartVel = -200000;
		stTeachContactParameter.iAntiBounce = 12;
		stTeachContactParameter.iTimeOut= 20000;
		stTeachContactParameter.iFlagDebug= 0;
		stTeachContactParameter.iMaxJerkMoveSrchHt = 5;
		stTeachContactParameter.iMaxAccMoveSrchHt = 200;
		acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();
		Sleep(200);
		mtn_teach_contact_acs(stServoControllerCommSet.Handle, &stTeachContactParameter, &stTeachContactResult);

			gstSystemScope.uiNumData = 7;
			gstSystemScope.uiDataLen = 3000;
			gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();
			mtn_tune_contact_save_data(stServoControllerCommSet.Handle, &gstSystemScope, stTeachContactParameter.iAxis, 
				FILE_NAME_SEARCH_CONTACT_INI_SRCH, "%% ACSC Controller, Axis- %d: RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n");

		int iAxisTuningACS = stTeachContactParameter.iAxis;

		mtn_wb_tune_b1w_stop_srch_init_cfg(stServoControllerCommSet.Handle);
		mtn_wb_tune_b1w_stop_srch_download_cfg();

		if(qc_is_axis_not_safe(stServoControllerCommSet.Handle, iAxisTuningACS)) //mtnapi_wait_axis_settling(stServoControllerCommSet.Handle, iAxisTuningACS, POSN_SETTLE_TH_DURING_TUNING) != MTN_API_OK_ZERO)
		{
			AfxMessageBox(_T("Error Settling B1W"));
		}
		mtn_wb_tune_b1w_stop_srch_trig_once(stServoControllerCommSet.Handle);
		while(qc_is_axis_still_acc_dec(stServoControllerCommSet.Handle, iAxisTuningACS))
		{
			Sleep(10); 	//Sleep(2);
		}
	}
	else
	{		// load from file

		char strFilePathnameOffLineSimu[256], strFilenameOffLineSimu[256];
		int iMechCfg = get_sys_machine_type_flag();
		CFileDialog cLoadWaveform(TRUE, NULL, "*.m", OFN_HIDEREADONLY,"B1W Files(*.*)|*.m||*.dat",NULL); // 20111113
		int   iResult   = (int)cLoadWaveform.DoModal();
		unsigned int uiDataLen;
		sprintf_s(strFilePathnameOffLineSimu, 256, "%s", cLoadWaveform.GetPathName().GetString());
		sprintf_s(strFilenameOffLineSimu, 256, "%s", cLoadWaveform.GetFileName().GetString()); // 20110520
		if( iResult == IDOK)
		{
			mtn_debug_load_b1w_data_from_file(strFilePathnameOffLineSimu, &uiDataLen);
		}
		mtn_wb_tune_b1w_stop_srch_init_cfg(stServoControllerCommSet.Handle);
	}
#endif 

	mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

	CString cstrTemp;
	cstrTemp.Format("ObjB1W: %4.0f, 1S-%4.0f, M1B-%4.0f, %4.0f, %4.0f, M2B-%4.0f, %4.0f, R-%4.0f, 2S-%4.0f", 
		stWbTuneB1wObj.dObj[0], stWbTuneB1wObj.dObj[1], stWbTuneB1wObj.dObj[2], 
		stWbTuneB1wObj.dObj[3], stWbTuneB1wObj.dObj[4], stWbTuneB1wObj.dObj[5], stWbTuneB1wObj.dObj[6],
		stWbTuneB1wObj.dObj[7], stWbTuneB1wObj.dObj[8]);
	GetDlgItem(IDC_STATIC_SERVO_PARA_TUNE_STATUS)->SetWindowTextA(cstrTemp);

}

// IDC_CHECK_TUNE_FLAG_B1W_MOTION
void MtnParameterDlg::OnBnClickedCheckTuneFlagB1wMotion()
{
	int iFlag = ((CButton*)GetDlgItem(IDC_CHECK_TUNE_FLAG_B1W_MOTION))->GetCheck() ;
	mtn_wb_tune_b1w_set_flag_motion(iFlag);
	UI_UpdateCheckStatusB1W();
}

// 20110523
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdle()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneIdle = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH_1B
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wSrch1b()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneSrch1B = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_MOVE_1B
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdleB1wMove1b()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneMoveTo1B = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPING
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wLooping()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneLooping = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_LOOPTOP
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wLooptop()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneLoopTop = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_IDLE_B1W_TRAJ2B
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneIdleB1wTraj2b()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneTraj2B = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_TAIL
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wTail()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneTail = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}
// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_RESET
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wReset()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneReset = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}

// IDC_BUTTON_PARAMETER_DLG_TUNE_B1W_SRCH2B
void MtnParameterDlg::OnBnClickedButtonParameterDlgTuneB1wSrch2b()
{
	mtn_wb_tune_b1w_set_clear_all_section(); iFlagB1W_TuneSrch2B = 1; UI_UpdateCheckStatusB1W();
	PrepareStartThreadB1W();
}

/////////////////////////////////////
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wIdle()
{
	iFlagB1W_TuneIdle = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_IDLE))->GetCheck();
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wSrch1b()
{
	iFlagB1W_TuneSrch1B = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH_1B))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wMove1b()
{
	iFlagB1W_TuneMoveTo1B = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_MOVE_1B))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wLooping()
{
	iFlagB1W_TuneLooping = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPING))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wLooptop()
{
	iFlagB1W_TuneLoopTop = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_LOOPTOP))->GetCheck(); 
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wTraj2b()
{
	iFlagB1W_TuneTraj2B = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TRAJ2B))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wTail()
{
	iFlagB1W_TuneTail = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_TAIL))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wReset()
{
	iFlagB1W_TuneReset = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_RESET))->GetCheck();  
}
// IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B
void MtnParameterDlg::OnBnClickedCheckParameterDlgTuneB1wSrch2b()
{
	iFlagB1W_TuneSrch2B = ((CButton *)GetDlgItem(IDC_CHECK_PARAMETER_DLG_TUNE_B1W_SRCH2B))->GetCheck();  // 20110523
}

// IDC_FLAG_CHECK_TUNING_BY_DLL  // 20110920
void MtnParameterDlg::OnBnClickedFlagCheckTuningByDll()
{
	iFlagServoTuningByDLL = ((CButton*) GetDlgItem(IDC_FLAG_CHECK_TUNING_BY_DLL))->GetCheck();
}

// iDlgSrvoParameterTuneShowFlag = 0;  // 20120202
void MtnParameterDlg::OnTimer(UINT nIDEvent)
{
//	UpdateUI_ByTimer();
}

UINT MtnParameterDlg::StartTimer(UINT TimerDuration)
{
	iTimerIdParameterDlg = SetTimer(IDT_TIMER_PARAMETER_DLG, TimerDuration, 0);
	
	if (iTimerIdParameterDlg == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}

    return (UINT)iTimerIdParameterDlg;
}// end StartTimer

BOOL MtnParameterDlg::StopTimer()
{
	if (!KillTimer (iTimerIdParameterDlg))
	{
		return FALSE;
	}
	return TRUE;
}
// IDC_EDIT_PARAMETER_DLG_B1W_VKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wVkp()
{
	// read from UI
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_B1W_VKP , &stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain);

	MTN_TUNE_PARAMETER_SET stTuneParaSet;
	int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
	mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	stTuneParaSet.dVelLoopKP = stServoLoopAcsCtrlParaDlg.dVelocityLoopProportionalGain;

	// write to acs
	mtn_b1w_write_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);

}
// IDC_EDIT_PARAMETER_DLG_B1W_VKI
void MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wVki()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_B1W_VKI , &stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain);

	MTN_TUNE_PARAMETER_SET stTuneParaSet;
	int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
	mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	stTuneParaSet.dVelLoopKI = stServoLoopAcsCtrlParaDlg.dVelocityLoopIntegratorGain;
	// write to acs
	mtn_b1w_write_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
}
// IDC_EDIT_PARAMETER_DLG_B1W_PKP
void MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wPkp()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_B1W_PKP , &stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain);

	MTN_TUNE_PARAMETER_SET stTuneParaSet;
	int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
	mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	stTuneParaSet.dPosnLoopKP = stServoLoopAcsCtrlParaDlg.dPositionLoopProportionalGain;
	// write to acs
	mtn_b1w_write_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	
}
// IDC_EDIT_PARAMETER_DLG_B1W_ACCFFC
void MtnParameterDlg::OnEnKillfocusEditParameterDlgB1wAccffc()
{
	ReadDoubleFromEdit(IDC_EDIT_PARAMETER_DLG_B1W_ACCFFC , &stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward);

	MTN_TUNE_PARAMETER_SET stTuneParaSet;
	int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
	mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	stTuneParaSet.dAccFFC = stServoLoopAcsCtrlParaDlg.dAccelerationFeedforward;
	// write to acs
	mtn_b1w_write_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
}

void MtnParameterDlg::UpdateB1W_ParaBndZ_TextEditBox()
{
	MTN_TUNE_PARAMETER_SET stTuneParaSet;
	int iObjSectionFlagB1W = mtn_wb_tune_get_blk_move_posn_set();
	mtn_b1w_read_para_bh_servo(stServoControllerCommSet.Handle, &stTuneParaSet, iObjSectionFlagB1W);
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_B1W_VKP, stTuneParaSet.dVelLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_B1W_VKI, stTuneParaSet.dVelLoopKI, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_B1W_PKP, stTuneParaSet.dPosnLoopKP, "%5.1f");
	UpdateDoubleToEdit(IDC_EDIT_PARAMETER_DLG_B1W_ACCFFC, stTuneParaSet.dAccFFC, "%5.1f");
}

void MtnParameterDlg::EnableUI_B1W_EditOnly_ParaBndZ(int iEnaFlag)
{
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_B1W_VKP)->EnableWindow(iEnaFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_B1W_VKI)->EnableWindow(iEnaFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_B1W_PKP)->EnableWindow(iEnaFlag);
	GetDlgItem(IDC_EDIT_PARAMETER_DLG_B1W_ACCFFC)->EnableWindow(iEnaFlag);
}

