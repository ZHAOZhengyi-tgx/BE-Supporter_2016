// MtnSpectrumTestDlg.cpp : implementation file
//
// 20090908 Replace Sleep to be high_precision_sleep_ms

#include "stdafx.h"
#include "MtnSpectrumTestDlg.h"
#include "WinTiming.h"

#include "aftprbs.h"
#include "acs_buff_prog.h"
#include "MtnApi.h"
#include "MtnTesterResDef.h"

#define __MAX_NUM_FREQ_SINE_SWEEP__    10000

static SINE_SWEEP_SYS_CONFOG stSineSweepSystemCfg;
static SINE_SWEEP_AXIS_CONFIG astSineSweepAxisCfg[MAX_NUM_AXIS_GROUP_SPECTRUM];
static double adArrayFreqSineSweep[__MAX_NUM_FREQ_SINE_SWEEP__];
static unsigned short usTotalCaseFreqSineSweep;
static SINE_SWEEP_GROUP_CONFIG stSineSweepGroupConfig;

static unsigned short usCountLenPRBS;
static unsigned short usAmplitudePRBS;
static unsigned short usFreqFactorPRBS;
static int iSelChannel;
static int iFlagTuningDriver=0, iFlagTuningVelLoopGain=0;  // 20121018

#define __FLAG_BW_SCANNINT_LINEAR_UP  0
#define __FLAG_BW_SCANNING_MATRIX_UP_0  1
#define __FLAG_BW_SCANNING_MATRIX_UP_1  2
char cFlagBwVelLoopScanningList = __FLAG_BW_SCANNING_MATRIX_UP_1; // __FLAG_BW_SCANNINT_LINEAR_UP;  // 20121112
char cFlagBwCheckDriverAxis = 0;
char cFlagBwCheckDriverComPort = 0;
char cFlagBwCheckDriverType = 2;

static UINT nCurrCaseTuningVK, nTotalCaseTuningVK;

extern COMM_SETTINGS stServoControllerCommSet;
#define BUFFER_ID_AFT_SPECTRUM_TEST    8

#define SPEED_PROFILE_MAX_ACC_LOWER_RATIO_MOVING_AT_SPECTRUM_TEST   0.5
#define SPEED_PROFILE_MAX_JERK_LOWER_RATIO_MOVING_AT_SPECTRUM_TEST  0.125

static SPECTRUM_SYS_ANA_GROUP_CONFIG stSpectrumSysAna_GroupTestConfig;
extern UINT nTotalNumCasesGroupSpecTest;
extern UINT nCurrCaseIdxGroupSpecTest;

static PRBS_GROUP_TEST_INPUT *stpPrbsGroupTestCase;
static SINESWEEP_GROUP_TEST_INPUT *stpGroupSineSweepInputCases;

#define  DEF_ALLOC_MEMORY_CASES_GROUP_PRBS   625 // each axis has 5 points 
static PRBS_GROUP_TEST_INPUT astAllocMemoryGroupPrbsCases[DEF_ALLOC_MEMORY_CASES_GROUP_PRBS];
static SINESWEEP_GROUP_TEST_INPUT astAllocMemoryGroupSineSweepCases[DEF_ALLOC_MEMORY_CASES_GROUP_PRBS];

static double dFreqControl_Hz;
static	unsigned int uiAxis_CtrlCardId[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	int iLowerLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	int iUpperLimitPosition[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned int uiNumPoints[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned char ucFlagIsExciteAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisAmpPrbs[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisAmpVelPrbs[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisPrbsLen[MAX_NUM_AXIS_GROUP_SPECTRUM];
static	unsigned short usAxisPrbsFreqFactor[MAX_NUM_AXIS_GROUP_SPECTRUM];
static  int aiTuneMaxVKP[MAX_NUM_AXIS_GROUP_SPECTRUM];  // 20121018
static  int aiTuneMaxSOF[MAX_NUM_AXIS_GROUP_SPECTRUM];  // 20121018

static int iFlagInitFirstTime;
static int iFlagGotoVelStepAfterSpectrumTest = 0;
static UINT nTimerPeriod_ms;
static UINT_PTR m_iTimerVal;

// local declared functions not in the class
void aft_spec_test_dlg_save_response_memory_to_file(char *strFilename, PRBS_TEST_CONDITION stPrbsTestInput);

#define  BE_WB_SPECTRUM_TEST_X_PRBS_AMP    1000
#define  BE_WB_SPECTRUM_TEST_Y_PRBS_AMP    3000
#define  BE_WB_SPECTRUM_TEST_Z_PRBS_AMP    200

#include "MotAlgo_DLL.h"

static int iBondHeadRelaxPosn;
#include "_SpectrumDlg_init.c"

void aft_spectrum_init_sine_sweep_cfg()
{
	stSineSweepSystemCfg.dLowFreq_Hz = 5.0;
	stSineSweepSystemCfg.dUppFreq_Hz = 500;
	stSineSweepSystemCfg.usNumCaseDec = 20;

	for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM ; ii++)
	{
		astSineSweepAxisCfg[ii].pdFreqArray = &adArrayFreqSineSweep[0];
		astSineSweepAxisCfg[ii].uiTotalCaseFreq = 0;	
	}	
	astSineSweepAxisCfg[0].dAmpSineSweepRatio = 15.0;
	astSineSweepAxisCfg[1].dAmpSineSweepRatio = 15.0;
	astSineSweepAxisCfg[2].dAmpSineSweepRatio = 8.0;
	astSineSweepAxisCfg[3].dAmpSineSweepRatio = 10.0;
}
#include <math.h>
void aft_spectrum_calc_freq_array_from_cfg()
{
	// calculate total number of freqs for one axis
	double dNextDecLowLog = floor(log10(stSineSweepSystemCfg.dLowFreq_Hz)) + 1.0;
	double dNextDecLow = pow(10.0,  dNextDecLowLog);
	double dPrevDecUppLog = ceil(log10(stSineSweepSystemCfg.dUppFreq_Hz)) - 1.0;
	double dPrevDecUpp = pow(10.0, dPrevDecUppLog);

	unsigned short nTotalFreqCase = (int)(dPrevDecUppLog - dNextDecLowLog) * stSineSweepSystemCfg.usNumCaseDec + 2 * stSineSweepSystemCfg.usNumCaseDec;

	if(nTotalFreqCase > __MAX_NUM_FREQ_SINE_SWEEP__)
	{
		return;
	}
	else
	{
		usTotalCaseFreqSineSweep = nTotalFreqCase;
	}
	double dStepMultiFirst = pow(dNextDecLow/stSineSweepSystemCfg.dLowFreq_Hz, 1.0/(stSineSweepSystemCfg.usNumCaseDec - 1));
	adArrayFreqSineSweep[0] = stSineSweepSystemCfg.dLowFreq_Hz;
	for(int ii=1; ii<stSineSweepSystemCfg.usNumCaseDec; ii++)
	{
		adArrayFreqSineSweep[ii] = adArrayFreqSineSweep[ii - 1] * dStepMultiFirst;
	}
	int nTotalFreqCaseMiddle = (int)(dPrevDecUppLog - dNextDecLowLog) * stSineSweepSystemCfg.usNumCaseDec;
	double dStepMultiNext = pow((dPrevDecUpp / dNextDecLow), 1.0/((int)(dPrevDecUppLog - dNextDecLowLog) * stSineSweepSystemCfg.usNumCaseDec));
	for(int ii=stSineSweepSystemCfg.usNumCaseDec; ii<stSineSweepSystemCfg.usNumCaseDec + nTotalFreqCaseMiddle; ii++)
	{
		adArrayFreqSineSweep[ii] = adArrayFreqSineSweep[ii - 1] * dStepMultiNext;
	}

	double dStepMultiLast = pow(stSineSweepSystemCfg.dUppFreq_Hz/dPrevDecUpp,  1.0/(stSineSweepSystemCfg.usNumCaseDec));
	for(int ii=stSineSweepSystemCfg.usNumCaseDec + nTotalFreqCaseMiddle; ii<nTotalFreqCase; ii++)
	{
		adArrayFreqSineSweep[ii] = adArrayFreqSineSweep[ii - 1] * dStepMultiLast;
	}

	for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM ; ii++)
	{
		astSineSweepAxisCfg[ii].uiTotalCaseFreq = usTotalCaseFreqSineSweep;
	}	
}

// CMtnSpectrumTestDlg dialog
HANDLE stCommHandle;				// communication handle

IMPLEMENT_DYNAMIC(CMtnSpectrumTestDlg, CDialog)

CMtnSpectrumTestDlg::CMtnSpectrumTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMtnSpectrumTestDlg::IDD, pParent)
{
	
}

CMtnSpectrumTestDlg::~CMtnSpectrumTestDlg()
{

}

///// Timer related
UINT_PTR  CMtnSpectrumTestDlg::StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration)
{ // IDT_SPEC_TEST_DLG_TIMER
	m_iTimerVal = SetTimer(nIDT_TimerResMacro, TimerDuration, 0);
	
	if (m_iTimerVal == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}
    return m_iTimerVal;
} 
BOOL  CMtnSpectrumTestDlg::StopTimer(UINT_PTR nTimerVal)
{
	if (!KillTimer (nTimerVal))
	{
		return FALSE;
	}
	return TRUE;
}

void CMtnSpectrumTestDlg::OnTimer(UINT nTimerVal)
{
// IDC_STATIC_SHOW_INIT_CTIME_STATUS
    // 20120202
//	Update_DlgUI();
}
#include "MtnInitAcs.h"
void CMtnSpectrumTestDlg::Update_DlgUI(void)
{
	static UINT ii, uiCurrAxisId;
	static double dRefPosn;
	static CString cstrTemp;
	cstrTemp.Format("CTime(ms): %6.2f; RPosn:[", sys_get_controller_ts()); // aft_get_basic_time_unit());

	for(ii=0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		uiCurrAxisId = uiAxis_CtrlCardId[ii];
		if(uiCurrAxisId < MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			mtnapi_get_ref_position(stCommHandle,  uiCurrAxisId, &dRefPosn, 0);
			cstrTemp.AppendFormat("%7.1f", dRefPosn);
		}
		else
		{
			cstrTemp.AppendFormat("NA", dRefPosn);
		}
		if(ii < MAX_NUM_AXIS_GROUP_SPECTRUM - 1)
		{
			cstrTemp.AppendFormat(" ,");
		}
		else
		{
			cstrTemp.AppendFormat("]");
		}
	}

	if(aft_spectrum_get_thread_flag_stop() == FALSE && nCurrCaseIdxGroupSpecTest < nTotalNumCasesGroupSpecTest)
	{ // performing SPECTRUM Test
		if(aft_get_spectrum_sys_analysis_flag() == SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP) // int aft_spectrum_get_sine_sweep_current_case_freq()
		{
			cstrTemp.AppendFormat("  %d/%d  %d/%d", nCurrCaseIdxGroupSpecTest, nTotalNumCasesGroupSpecTest,
				aft_spectrum_get_sine_sweep_current_case_freq(), stpGroupSineSweepInputCases[nCurrCaseIdxGroupSpecTest].stSinSweepAxisCfg.uiTotalCaseFreq);
			GetDlgItem(IDC_BUTTON_STOP_SPECTRUM_TEST)->EnableWindow(TRUE);
		}
		else
		{
			if(iFlagTuningVelLoopGain == 1)
			{
				cstrTemp.AppendFormat("  %d/%d, VK*: %d/%d", nCurrCaseIdxGroupSpecTest, nTotalNumCasesGroupSpecTest,
					nCurrCaseTuningVK, nTotalCaseTuningVK); // 20121018
				
			}
			else
			{
				cstrTemp.AppendFormat("  %d/%d", nCurrCaseIdxGroupSpecTest, nTotalNumCasesGroupSpecTest);
			}
			GetDlgItem(IDC_BUTTON_STOP_SPECTRUM_TEST)->EnableWindow(TRUE);
		}

	}
	else
	{ // not doing
		GetDlgItem(IDC_BUTTON_START_GROUP_PRBS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_START_GROUP_SINE_SWEEP)->EnableWindow(TRUE);
		GetDlgItem(IDOK)->EnableWindow(TRUE);
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_STOP_SPECTRUM_TEST)->EnableWindow(FALSE);

	}
	GetDlgItem(IDC_STATIC_SHOW_INIT_CTIME_STATUS)->SetWindowTextA(cstrTemp);
}

//// Enter and exit
void CMtnSpectrumTestDlg::EndDialog(int nResult)
{
	CDialog::EndDialog(nResult);
}

// 20120202, move timer to global entry for better management
static int iDlgSpectrumFlagShowWindow = 0;

void CMtnSpectrumTestDlg::OnOK()
{
	spectrum_dlg_calculate_total_num_cases();
	aft_safe_exit_spectrum_test();
	if(aft_get_spectrum_test_bakup_flag() == TRUE)		aft_restore_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);
	iDlgSpectrumFlagShowWindow = 0;

	CDialog::OnOK();
}
void CMtnSpectrumTestDlg::OnCancel()
{
	aft_safe_exit_spectrum_test();
//	InitPrbsGroupTestConfigVar();
	if(aft_get_spectrum_test_bakup_flag() == TRUE)		aft_restore_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);
	iDlgSpectrumFlagShowWindow = 0;
	
	CDialog::OnCancel();
}
//#include "AcsServo.h" //"ServoController.h"

BOOL CMtnSpectrumTestDlg::OnInitDialog()
{
	nCurrCaseIdxGroupSpecTest = INT_MAX;
	if(iFlagInitFirstTime == 0)
	{
		iFlagInitFirstTime = 1;
		usCountLenPRBS = 3000;
		usAmplitudePRBS = 768; // over +-32767
		usFreqFactorPRBS = 1;

		iSelChannel = 0;

		InitPrbsGroupTestConfigVar();
		// Init PRBS Group test UI
		InitPrbsGroupTestConfigUI();
	}


	// Initialize Default parameters and firmware
	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		if(aft_spectrum_initialize() != MTN_API_OK_ZERO)
		{
			AfxMessageBox(_T("Error Initialize Environment Variables"));
		}
		aft_spectrum_init_sine_sweep_cfg();
		stCommHandle = stServoControllerCommSet.Handle;
	}

	stpPrbsGroupTestCase = &astAllocMemoryGroupPrbsCases[0];
	stpGroupSineSweepInputCases = &astAllocMemoryGroupSineSweepCases[0];

	double dBasicTimeUnit_ms = aft_get_basic_time_unit();
	CString cstrTemp; cstrTemp.Format("Control Time unit(ms): %8.4f", dBasicTimeUnit_ms);
	GetDlgItem(IDC_STATIC_SHOW_INIT_CTIME_STATUS)->SetWindowTextA(cstrTemp);

	// Timer & Thread
	nTimerPeriod_ms = 500;
	StartTimer(IDT_SPEC_TEST_DLG_TIMER, nTimerPeriod_ms);

	// Flags 
	((CButton*)GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP))->SetCheck(aft_spectrum_get_flag_velloop());
	((CButton*)GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_DSP_DATA))->SetCheck(aft_spectrum_get_flag_dsp_data());

	m_pWinThreadSpecTest = NULL;
	aft_spectrum_set_thread_flag_stop(TRUE); // mFlagStopSpectrumTestThread = TRUE;

	UI_EnableEditDebug(aft_spectrum_get_debug_flag()); // iFlagEnableDebugGroupPRBS);
	UI_EnableEditTuningMaxParameter(aft_spectrum_get_debug_flag()); // 20121018
	SetUserInterfaceLanguage(get_sys_language_option());

	/// TBA
	GetDlgItem(IDC_BUTTON_PLOT_SPECTRUM_TEST)->EnableWindow(FALSE);   // EnableWindow
	GetDlgItem(IDC_BUTTON_SAVE_GROUP_PRBS_CONFIG)->ShowWindow(FALSE); // EnableWindow
	GetDlgItem(IDC_BUTTON_LOAD_GROUP_PRBS_CONFIG)->ShowWindow(FALSE);  // EnableWindow
	return CDialog::OnInitDialog();
}

void CMtnSpectrumTestDlg::ShowWindow(int nCmdShow)
{
	StartTimer(IDT_SPEC_TEST_DLG_TIMER, nTimerPeriod_ms);
	// Init PRBS Group test UI
	InitPrbsGroupTestConfigUI();
	iDlgSpectrumFlagShowWindow = 1;
	CDialog::ShowWindow(nCmdShow);
}

int CMtnSpectrumTestDlg::UI_GetShowWindowFlag()
{
	return iDlgSpectrumFlagShowWindow;
}
void CMtnSpectrumTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMtnSpectrumTestDlg, CDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_SPECTRUM_PRBS_LEN, &CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsLen)
	ON_EN_CHANGE(IDC_EDIT_SPECTRUM_PRBS_AMPLITUDE, &CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsAmplitude)
	ON_EN_CHANGE(IDC_EDIT_SPECTRUM_PRBS_FREQ_FACTOR, &CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsFreqFactor)
	ON_BN_CLICKED(IDC_BUTTON_SPECTRUM_TEST, &CMtnSpectrumTestDlg::OnBnClickedButtonSpectrumTest)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECTRUM_TEST_AXIS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumTestAxis)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_TEST_DEBUG, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumTestDebug)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS1_GROUP_PRBS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis1GroupPrbs)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS2_GROUP_PRBS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis2GroupPrbs)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS3_GROUP_PRBS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis3GroupPrbs)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS4_GROUP_PRBS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis4GroupPrbs)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_AXIS1, &CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis1)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_AXIS2, &CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis2)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_AXIS3, &CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis3)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_AXIS4, &CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis4)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis4)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis4)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis4)
	ON_EN_CHANGE(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis4)
	ON_EN_CHANGE(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis4)
	ON_EN_CHANGE(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis4)
	ON_BN_CLICKED(IDC_BUTTON_START_GROUP_PRBS, &CMtnSpectrumTestDlg::OnBnClickedButtonStartGroupPrbs)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_GROUP_PRBS_CONFIG, &CMtnSpectrumTestDlg::OnBnClickedButtonSaveGroupPrbsConfig)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_GROUP_PRBS_CONFIG, &CMtnSpectrumTestDlg::OnBnClickedButtonLoadGroupPrbsConfig)
	ON_BN_CLICKED(IDC_BUTTON_GROUP_PRBS_ESTIMATE_TIME, &CMtnSpectrumTestDlg::OnBnClickedButtonGroupPrbsEstimateTime)
	ON_BN_CLICKED(IDC_BUTTON_STOP_SPECTRUM_TEST, &CMtnSpectrumTestDlg::OnBnClickedButtonStopSpectrumTest)
	ON_BN_CLICKED(IDC_BUTTON_PLOT_SPECTRUM_TEST, &CMtnSpectrumTestDlg::OnBnClickedButtonPlotSpectrumTest)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumFlagVelLoop)
	ON_EN_CHANGE(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS1, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis1)
	ON_EN_CHANGE(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS2, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis2)
	ON_EN_CHANGE(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS3, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis3)
	ON_EN_CHANGE(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS4, &CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis4)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_GROUP_TEST_DEBUG, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumGroupTestDebug)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_FLAG_DSP_DATA, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumFlagDspData)
	ON_EN_KILLFOCUS(IDC_EDIT_SINE_SWEEP_LOW_FREQ_HZ, &CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepLowFreqHz)
	ON_EN_KILLFOCUS(IDC_EDIT_SINE_SWEEP_UPP_FREQ_HZ, &CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepUppFreqHz)
	ON_EN_KILLFOCUS(IDC_EDIT_SINE_SWEEP_NUM_CASES_PER_DEC, &CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepNumCasesPerDec)
	ON_EN_KILLFOCUS(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS1, &CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS2, &CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS3, &CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS4, &CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis4)
	ON_BN_CLICKED(IDC_BUTTON_START_GROUP_SINE_SWEEP, &CMtnSpectrumTestDlg::OnBnClickedButtonStartGroupSineSweep)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgGotoVelStepAfter)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_DLG_TUNE_VEL_GAIN, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgTuneVelGain)
	ON_BN_CLICKED(IDC_CHECK_SPECTRUM_DLG_TUNE_DRV, &CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgTuneDrv)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_VKP_1_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp1SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_VKP_2_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp2SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_VKP_3_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp3SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_VKP_4_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp4SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_SOF_1_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof1SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_SOF_2_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof2SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_SOF_3_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof3SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_SOF_4_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof4SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKP_1_SPECTRUM_DLG2, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp1SpectrumDlg2)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKP_2_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp2SpectrumDlg)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKP_3_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp3SpectrumDlg)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKP_4_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp4SpectrumDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKI_1_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki1SpectrumDlg)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKI_2_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki2SpectrumDlg)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKI_3_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki3SpectrumDlg)
//	ON_EN_KILLFOCUS(IDC_EDIT_MAX_CKI_4_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki4SpectrumDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG, &CMtnSpectrumTestDlg::OnCbnSelchangeComboPathScanningVelLoopSpectrumDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS, &CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverAxis)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT, &CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverComport)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE, &CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverType)
END_MESSAGE_MAP()

// CMtnSpectrumTestDlg message handlers

// IDC_EDIT_SPECTRUM_PRBS_LEN
void CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsLen()
{
	char tempChar[32];
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_LEN)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &usCountLenPRBS);
}

// IDC_EDIT_SPECTRUM_PRBS_AMPLITUDE
void CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsAmplitude()
{
	char tempChar[32];
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_AMPLITUDE)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &usAmplitudePRBS);
}

// IDC_EDIT_SPECTRUM_PRBS_FREQ_FACTOR
void CMtnSpectrumTestDlg::OnEnChangeEditSpectrumPrbsFreqFactor()
{
	char tempChar[32];
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_FREQ_FACTOR)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &usFreqFactorPRBS);
}

// IDC_BUTTON_SPECTRUM_TEST
void CMtnSpectrumTestDlg::OnBnClickedButtonSpectrumTest()
{
	// TODO: Add your control notification handler code here
	PRBS_TEST_CONDITION stPrbsTestInput;
	unsigned int uiRandSeed;
	uiRandSeed = 0; // 20111024, Set the same test condition for all test, (unsigned int) time(NULL);

	stPrbsTestInput.uiExcitingAxis = iSelChannel;
	stPrbsTestInput.usAmplitudePRBS = usAmplitudePRBS;
	stPrbsTestInput.usCountLenPRBS = usCountLenPRBS;
	stPrbsTestInput.usFreqFactorPRBS = usFreqFactorPRBS;
	stPrbsTestInput.uiRandSeed = uiRandSeed;

	// Move to library

	// excite once
	if(aft_prbs_excite_once(&stPrbsTestInput) != MTN_API_OK_ZERO)
	{
		AfxMessageBox(_T("PRBS test failure"));
		goto label_return_prbs_test_once;
	}

	char strFilename[] = "PrbsRespAry.m";
	aft_spec_test_dlg_save_response_memory_to_file(strFilename, stPrbsTestInput);

label_return_prbs_test_once:

	return;
}

// IDC_COMBO_SPECTRUM_TEST_AXIS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumTestAxis()
{
	iSelChannel = (int) ((CComboBox*) GetDlgItem(IDC_COMBO_SPECTRUM_TEST_AXIS))->GetCurSel();
}

// IDC_CHECK_SPECTRUM_TEST_DEBUG
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumTestDebug()
{

	int iDebugFlag = ((CButton*)GetDlgItem(IDC_CHECK_SPECTRUM_TEST_DEBUG))->GetCheck();

	if(iDebugFlag)
	{
		aft_spectrum_enable_debug();
	}
	else
	{
		aft_spectrum_disable_debug();
	}
}

void CMtnSpectrumTestDlg::InitPrbsGroupTestConfigCombo_OneAxis(int nResId, int iAxis)
{
	CComboBox *pSelectChannelCombo = (CComboBox*) GetDlgItem(nResId);
	pSelectChannelCombo->ResetContent();
	pSelectChannelCombo->InsertString(0,"ACS-X");
	pSelectChannelCombo->InsertString(1,"ACS-Y");
	pSelectChannelCombo->InsertString(2,"ACS-Z");
	pSelectChannelCombo->InsertString(3,"ACS-T");
	pSelectChannelCombo->InsertString(4,"ACS-A");
	pSelectChannelCombo->InsertString(5,"ACS-B");
	pSelectChannelCombo->InsertString(6,"ACS-C");
	pSelectChannelCombo->InsertString(7,"ACS-D");
	pSelectChannelCombo->InsertString(MAX_CTRL_AXIS_PER_SERVO_BOARD,"Dummy");
	pSelectChannelCombo->SetCurSel(uiAxis_CtrlCardId[iAxis]);
}

void CMtnSpectrumTestDlg::InitPrbsGroupTestConfigEditOneAxis(int nResId, int iValue)
{
	static char tempChar[32];
	sprintf_s(tempChar, 32, "%d", iValue);
	GetDlgItem(nResId)->SetWindowTextA(_T(tempChar));
}

void CMtnSpectrumTestDlg::UpdateShortToEdit(int nResId, short sValue)
{
	CString cstrTemp;
	cstrTemp.Format("%d", sValue);
//	sprintf_s(tempChar, 32, "%d", sValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}
void CMtnSpectrumTestDlg::UpdateDoubleToEdit(int nResId, double dValue)
{
	CString cstrTemp;
	cstrTemp.Format("%5.1f", dValue);
//	sprintf_s(tempChar, 32, "%d", sValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}
// For reading double type variable
static char *strStopString;
void CMtnSpectrumTestDlg::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*pdValue = strtod(tempChar, &strStopString);
}

void CMtnSpectrumTestDlg::ReadUnsigndShortFromEdit(int nResId, unsigned short *pusValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", pusValue);
}

void CMtnSpectrumTestDlg::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}

void CMtnSpectrumTestDlg::ReadShortFromEdit(int nResId, short *psValue)
{
	static char tempChar[32];
	int iTemp;
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &iTemp);
	*psValue = (short)iTemp;
}

void CMtnSpectrumTestDlg::InitPrbsGroupTestConfigVar()
{

	ucFlagIsExciteAxis[0] = 1;
	ucFlagIsExciteAxis[1] = 1;
	ucFlagIsExciteAxis[2] = 0;
	ucFlagIsExciteAxis[3] = 0;

	MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
	mtn_api_get_spring_compensation_sp_para(stServoControllerCommSet.Handle, &stOutputPosnCompensationTune);
	iBondHeadRelaxPosn = stOutputPosnCompensationTune.iEncoderOffsetSP;

	int iTempMachCfg = get_sys_machine_type_flag();
	if(iTempMachCfg == WB_MACH_TYPE_VLED_FORK)   // machine type dependency Item-4.
	{
		aft_spectrum_set_test_condition_dual_track_20_vled();
	}
	else if(iTempMachCfg == WB_MACH_TYPE_HORI_LED || iTempMachCfg == BE_WB_HORI_20T_LED)  // 20120827
	{
		aft_spectrum_set_test_condition_hori_led();
	}
	else if(iTempMachCfg == BE_WB_ONE_TRACK_18V_LED)
	{
		aft_spectrum_set_test_condition_18v_led();
	}
	else if(iTempMachCfg == WB_STATION_XY_TOP)
	{
		aft_spectrum_set_test_condition_station_xy_hori();
	}
	else if(iTempMachCfg == WB_STATION_XY_VERTICAL)
	{
		aft_spectrum_set_test_condition_station_xy_vertical();
	}
	else if(iTempMachCfg == WB_STATION_BH)
	{
		aft_spectrum_set_test_condition_station_bh();
	}
	else if(iTempMachCfg == WB_STATION_EFO_BSD)
	{
		aft_spectrum_set_test_condition_station_dummy();
	}
	else //  if(get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)
	{
		aft_spectrum_set_test_condition_1_cup_vled();
	}

	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		usAxisAmpVelPrbs[0] = 500;
		usAxisAmpVelPrbs[1] = 500;
		usAxisAmpVelPrbs[2] = 1000;
		usAxisAmpVelPrbs[3] = 1000;
		usAxisPrbsLen[0] = 4096;
		usAxisPrbsLen[1] = 4096;
		usAxisPrbsLen[2] = 4096;
		usAxisPrbsLen[3] = 4096;

	}
	else
	{
		usAxisAmpVelPrbs[0] = 500;
		usAxisAmpVelPrbs[1] = 1000;
		usAxisAmpVelPrbs[2] = 1000;
		usAxisAmpVelPrbs[3] = 1000;

		usAxisPrbsLen[0] = 4096;
		usAxisPrbsLen[1] = 4096;
		usAxisPrbsLen[2] = 4096;
		usAxisPrbsLen[3] = 4096;

	}


	usAxisPrbsFreqFactor[0] = 1;
	usAxisPrbsFreqFactor[1] = 1;
	usAxisPrbsFreqFactor[2] = 1;
	usAxisPrbsFreqFactor[3] = 1;

	dFreqControl_Hz = 1000.0/aft_get_basic_time_unit();
	stSpectrumSysAna_GroupTestConfig.dFreqControl_Hz = dFreqControl_Hz;

	aiTuneMaxVKP[0] = 400;  // 20121018
	aiTuneMaxVKP[1] = 400;
	aiTuneMaxVKP[2] = 40;
	aiTuneMaxVKP[3] = 400;

	aiTuneMaxSOF[0] = 4000;
	aiTuneMaxSOF[1] = 4000;
	aiTuneMaxSOF[2] = 4000;
	aiTuneMaxSOF[3] = 4000; // 20121018

}

void CMtnSpectrumTestDlg::InitPrbsGroupTestConfigUI()
{
	CComboBox *pSelectChannelCombo = (CComboBox*) GetDlgItem(IDC_COMBO_SPECTRUM_TEST_AXIS);
	pSelectChannelCombo->ResetContent();
	pSelectChannelCombo->InsertString(0,"X");
	pSelectChannelCombo->InsertString(1,"Y");
	pSelectChannelCombo->InsertString(2,"Z");
	pSelectChannelCombo->InsertString(3,"T");
	pSelectChannelCombo->InsertString(4,"A");
	pSelectChannelCombo->InsertString(5,"B");
	pSelectChannelCombo->InsertString(6,"C");
	pSelectChannelCombo->InsertString(7,"D");
	pSelectChannelCombo->InsertString(MAX_CTRL_AXIS_PER_SERVO_BOARD,"Dummy");
	pSelectChannelCombo->SetCurSel(iSelChannel);

	char tempChar[32];
	sprintf_s(tempChar, 32, "%d", usCountLenPRBS);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_LEN)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 32, "%d", usAmplitudePRBS);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_AMPLITUDE)->SetWindowTextA(_T(tempChar));
	sprintf_s(tempChar, 32, "%d", usFreqFactorPRBS);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_FREQ_FACTOR)->SetWindowTextA(_T(tempChar));

	// Checks, 20121018
	((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER)) ->SetCheck(iFlagGotoVelStepAfterSpectrumTest);
	((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_TUNE_VEL_GAIN)) ->SetCheck(iFlagTuningVelLoopGain);
	((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_TUNE_DRV)) ->SetCheck(iFlagTuningDriver);

	// SineSweep Related
	UpdateDoubleToEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS1, astSineSweepAxisCfg[0].dAmpSineSweepRatio);
	UpdateDoubleToEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS2, astSineSweepAxisCfg[1].dAmpSineSweepRatio);
	UpdateDoubleToEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS3, astSineSweepAxisCfg[2].dAmpSineSweepRatio);
	UpdateDoubleToEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS4, astSineSweepAxisCfg[3].dAmpSineSweepRatio);
	
	UpdateDoubleToEdit(IDC_EDIT_SINE_SWEEP_LOW_FREQ_HZ, stSineSweepSystemCfg.dLowFreq_Hz);
	UpdateDoubleToEdit(IDC_EDIT_SINE_SWEEP_UPP_FREQ_HZ, stSineSweepSystemCfg.dUppFreq_Hz);
	UpdateShortToEdit(IDC_EDIT_SINE_SWEEP_NUM_CASES_PER_DEC, stSineSweepSystemCfg.usNumCaseDec);

	// PRBS
	InitPrbsGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS1_GROUP_PRBS, 0);
	InitPrbsGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS2_GROUP_PRBS, 1);
	InitPrbsGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS3_GROUP_PRBS, 2);
	InitPrbsGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS4_GROUP_PRBS, 3);

	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS1))->SetCheck(ucFlagIsExciteAxis[0]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS2))->SetCheck(ucFlagIsExciteAxis[1]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS3))->SetCheck(ucFlagIsExciteAxis[2]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS4))->SetCheck(ucFlagIsExciteAxis[3]);

	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS1, iLowerLimitPosition[0]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS2, iLowerLimitPosition[1]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS3, iLowerLimitPosition[2]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS4, iLowerLimitPosition[3]);

	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS1, iUpperLimitPosition[0]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS2, iUpperLimitPosition[1]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS3, iUpperLimitPosition[2]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS4, iUpperLimitPosition[3]);

	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS1, (int)uiNumPoints[0]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS2, (int)uiNumPoints[1]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS3, (int)uiNumPoints[2]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS4, (int)uiNumPoints[3]);

	UpdateShortToEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS1, (short)(usAxisAmpPrbs[0]));
	UpdateShortToEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS2, (short)(usAxisAmpPrbs[1]));
	UpdateShortToEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS3, (short)(usAxisAmpPrbs[2]));
	UpdateShortToEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS4, (short)(usAxisAmpPrbs[3]));

	UpdateShortToEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS1, (short)(usAxisAmpVelPrbs[0]));
	UpdateShortToEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS2, (short)(usAxisAmpVelPrbs[1]));
	UpdateShortToEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS3, (short)(usAxisAmpVelPrbs[2]));
	UpdateShortToEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS4, (short)(usAxisAmpVelPrbs[3]));

	UpdateShortToEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS1, (short)usAxisPrbsLen[0]);
	UpdateShortToEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS2, (short)usAxisPrbsLen[1]);
	UpdateShortToEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS3, (short)usAxisPrbsLen[2]);
	UpdateShortToEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS4, (short)usAxisPrbsLen[3]);

	UpdateShortToEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS1, (short)usAxisPrbsFreqFactor[0]);
	UpdateShortToEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS2, (short)usAxisPrbsFreqFactor[1]);
	UpdateShortToEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS3, (short)usAxisPrbsFreqFactor[2]);
	UpdateShortToEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS4, (short)usAxisPrbsFreqFactor[3]);

	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_VKP_1_SPECTRUM_DLG, aiTuneMaxVKP[0]); // 20121018
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_VKP_2_SPECTRUM_DLG, aiTuneMaxVKP[1]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_VKP_3_SPECTRUM_DLG, aiTuneMaxVKP[2]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_VKP_4_SPECTRUM_DLG, aiTuneMaxVKP[3]);

	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_SOF_1_SPECTRUM_DLG, aiTuneMaxSOF[0]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_SOF_2_SPECTRUM_DLG, aiTuneMaxSOF[1]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_SOF_3_SPECTRUM_DLG, aiTuneMaxSOF[2]);
	InitPrbsGroupTestConfigEditOneAxis(IDC_EDIT_MAX_SOF_4_SPECTRUM_DLG, aiTuneMaxSOF[3]);

	// Velocity Loop Bw Scanning
	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->ResetContent();
	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->InsertString(0, "Linear");
	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->InsertString(1, "Matrix-0");
	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->InsertString(2, "Matrix-1");
//	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->InsertString(3, "Matrix-2");
	((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->SetCurSel(cFlagBwVelLoopScanningList);

	// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->ResetContent();
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->InsertString(0, "COM1");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->InsertString(1, "COM2");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->InsertString(2, "COM3");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->InsertString(3, "COM4");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->SetCurSel(cFlagBwCheckDriverComPort);

	// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS))->ResetContent();
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS))->InsertString(0, "X");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS))->InsertString(1, "Y");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS))->SetCurSel(cFlagBwCheckDriverAxis);

	// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->ResetContent();
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->InsertString(0, "Copley");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->InsertString(1, "ELMO");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->InsertString(2, "Srvtrnx");
	((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->SetCurSel(cFlagBwCheckDriverType);
}

// IDC_COMBO_AXIS1_GROUP_PRBS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis1GroupPrbs()
{
	uiAxis_CtrlCardId[0] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS1_GROUP_PRBS))->GetCurSel();
}
// IDC_COMBO_AXIS2_GROUP_PRBS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis2GroupPrbs()
{
	uiAxis_CtrlCardId[1] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS2_GROUP_PRBS))->GetCurSel();
}
// IDC_COMBO_AXIS3_GROUP_PRBS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis3GroupPrbs()
{
	uiAxis_CtrlCardId[2] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS3_GROUP_PRBS))->GetCurSel();
}
// IDC_COMBO_AXIS4_GROUP_PRBS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboAxis4GroupPrbs()
{
	uiAxis_CtrlCardId[3] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS4_GROUP_PRBS))->GetCurSel();
}
// IDC_CHECK_EXCITE_FLAG_AXIS1
void CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis1()
{
	ucFlagIsExciteAxis[0] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS1))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_AXIS2
void CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis2()
{
	ucFlagIsExciteAxis[1] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS2))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_AXIS3
void CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis3()
{
	ucFlagIsExciteAxis[2] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS3))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_AXIS4
void CMtnSpectrumTestDlg::OnBnClickedCheckExciteFlagAxis4()
{
	ucFlagIsExciteAxis[3] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_AXIS4))->GetCheck();
}
// IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis1()
{
	ReadIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS1, (int*)&uiNumPoints[0]);
}
// IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis2()
{
	ReadIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS2, (int*)&uiNumPoints[1]);
}
// IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis3()
{
	ReadIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS3, (int*)&uiNumPoints[2]);
}
// IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditNumPointsGroupPrbsAxis4()
{
	ReadIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_PRBS_AXIS4, (int*)&uiNumPoints[3]);
}
// IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis1()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS1, (int*)&iLowerLimitPosition[0]);
}
// IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis2()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS2, (int*)&iLowerLimitPosition[1]);
}
// IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis3()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS3, (int*)&iLowerLimitPosition[2]);
}
// IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditLowLimitGroupPrbsAxis4()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS4, (int*)&iLowerLimitPosition[3]);
}
// IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis1()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS1, (int*)&iUpperLimitPosition[0]);
}
// IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis2()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS2, (int*)&iUpperLimitPosition[1]);
}
// IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis3()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS3, (int*)&iUpperLimitPosition[2]);
}
// IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditUppLimitGroupPrbsAxis4()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS4, (int*)&iUpperLimitPosition[3]);
}
// IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis1()
{ // 
	ReadShortFromEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS1, (short*)&usAxisAmpPrbs[0]);
}
// IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis2()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS2, (short*)&usAxisAmpPrbs[1]);
}
// IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis3()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS3, (short*)&usAxisAmpPrbs[2]);
}
// IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsAmpGroupPrbsAxis4()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS4, (short*)&usAxisAmpPrbs[3]);
}
// IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis1()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS1, (short*)&usAxisPrbsLen[0]);
}
// IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis2()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS2, (short*)&usAxisPrbsLen[1]);
}
// IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis3()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS3, (short*)&usAxisPrbsLen[2]);
}
// IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsLenGroupPrbsAxis4()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS4, (short*)&usAxisPrbsLen[3]);
}
// IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis1()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS1, (short*)&usAxisPrbsFreqFactor[0]);
}
// IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis2()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS2, (short*)&usAxisPrbsFreqFactor[1]);
}
// IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis3()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS3, (short*)&usAxisPrbsFreqFactor[2]);
}
// IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsFreqFactorGroupPrbsAxis4()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS4, (short*)&usAxisPrbsFreqFactor[3]);
}
// IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS1
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis1()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS1, (short*)&usAxisAmpVelPrbs[0]);
}
// IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS2
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis2()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS2, (short*)&usAxisAmpVelPrbs[1]);
}
// IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS3
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis3()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS3, (short*)&usAxisAmpVelPrbs[2]);
}
// IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS4
void CMtnSpectrumTestDlg::OnEnChangeEditPrbsVelAmpGroupPrbsAxis4()
{
	ReadShortFromEdit(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS4, (short*)&usAxisAmpVelPrbs[3]);
}

// IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumFlagVelLoop()
{
	char cFlag = ((CButton*)GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP))->GetCheck();
	aft_spectrum_set_flag_velocity_loop_excite(cFlag);
}

// IDC_BUTTON_START_GROUP_PRBS
void CMtnSpectrumTestDlg::OnBnClickedButtonStartGroupPrbs()
{
	OnBnClickedButtonGroupPrbsEstimateTime(); // 2010Feb9

	nCurrCaseIdxGroupSpecTest = 0; // 20120728
	nTotalNumCasesGroupSpecTest = spectrum_dlg_calculate_total_num_cases();

	CString cstrTemp;
	cstrTemp.Format("Total %d Cases", nTotalNumCasesGroupSpecTest);
	GetDlgItem(IDC_STATIC_GROUP_PRBS_TEST_STATUS)->SetWindowTextA(cstrTemp);

	// Generate PRBS_GROUP_TEST_INPUT for each case of Group-PRBS

	if(nTotalNumCasesGroupSpecTest > DEF_ALLOC_MEMORY_CASES_GROUP_PRBS)
	{
		stpPrbsGroupTestCase = (PRBS_GROUP_TEST_INPUT *) calloc(nTotalNumCasesGroupSpecTest, sizeof(PRBS_GROUP_TEST_INPUT));
	}

	unsigned int uiRandSeed;
	uiRandSeed = 0; // 20111024, Set the same test condition for all test, (unsigned int) time(NULL);

	aft_convert_group_prbs_cfg_to_test_input(stSpectrumSysAna_GroupTestConfig, 
											  stpPrbsGroupTestCase,
											  uiRandSeed);

	aft_spectrum_make_new_folder(stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[2]);  // 20121112
	aft_white_noise_spectrum_make_master_file(stSpectrumSysAna_GroupTestConfig, stpPrbsGroupTestCase); // 20121112

	// bakup motion speed profile
	aft_backup_servo_control_for_spec_axis(&stpPrbsGroupTestCase[0]); // 20110720
	aft_backup_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);

	// lower down the acc and jerk
	double dAccRatio = SPEED_PROFILE_MAX_ACC_LOWER_RATIO_MOVING_AT_SPECTRUM_TEST, dJerkRatio = SPEED_PROFILE_MAX_JERK_LOWER_RATIO_MOVING_AT_SPECTRUM_TEST;
	short sRet;
	sRet = aft_set_smaller_speed(dAccRatio, dJerkRatio, &stpPrbsGroupTestCase[0]);
	if(sRet != MTN_API_OK_ZERO)
	{
		AfxMessageBox(_T("Warning! Invalid Parameter"));
		aft_restore_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);
		goto label_return_click_button_group_prbs_test;
	}

	// Start Thread for spectrum test
	GetDlgItem(IDC_BUTTON_START_GROUP_SINE_SWEEP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_GROUP_PRBS)->EnableWindow(FALSE);
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

	nCurrCaseIdxGroupSpecTest = 0;
	acs_run_buffer_prog_prbs_prof_cfg_move();
	acs_run_buffer_prog_vel_loop_prof_cfg_move2();
	RunSpectrumTestThread();
	mtn_dll_music_logo_normal_start();  // 20120217

label_return_click_button_group_prbs_test:
	// restore the speed profile
	return;

}
// IDC_BUTTON_SAVE_GROUP_PRBS_CONFIG
void CMtnSpectrumTestDlg::OnBnClickedButtonSaveGroupPrbsConfig()
{
	// TODO: Add your control notification handler code here
}
// IDC_BUTTON_LOAD_GROUP_PRBS_CONFIG
void CMtnSpectrumTestDlg::OnBnClickedButtonLoadGroupPrbsConfig()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE);

	if(fileDlg.DoModal() == IDOK)
	{
		CString cstrSpecTestCfgFilename =  fileDlg.GetFileName();
		CString cstrSpecTestCfgPathname = fileDlg.GetPathName();
		cstrSpecTestCfgFilename.Find(_T("."));
	}
}

extern double afRatioRMS_DrvCmd[MAX_SERVO_AXIS_WIREBOND];

void spectrum_dlg_consolidate_group_sys_ana_config()
{
	int ii;
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		// 
		stSpectrumSysAna_GroupTestConfig.uiAxis_CtrlCardId[ii] = uiAxis_CtrlCardId[ii];
		stSpectrumSysAna_GroupTestConfig.iLowerLimitPosition[ii] = iLowerLimitPosition[ii];
		stSpectrumSysAna_GroupTestConfig.iUpperLimitPosition[ii] = iUpperLimitPosition[ii];
		stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[ii] = ucFlagIsExciteAxis[ii];
		stSpectrumSysAna_GroupTestConfig.uiNumPoints [ii] = uiNumPoints[ii];
		if(aft_spectrum_get_flag_velloop() == OPEN_LOOP_SPECTRUM_TEST)
		{
			stSpectrumSysAna_GroupTestConfig.usAxisAmpPrbs[ii] = usAxisAmpPrbs[ii];
		}
		else // VEL_LOOP_SPECTRUM_TEST
		{
			stSpectrumSysAna_GroupTestConfig.usAxisAmpPrbs[ii] = usAxisAmpVelPrbs[ii];
		}
		stSpectrumSysAna_GroupTestConfig.usAxisPrbsFreqFactor[ii] = usAxisPrbsFreqFactor[ii];
		stSpectrumSysAna_GroupTestConfig.usAxisPrbsLen[ii] = usAxisPrbsLen[ii];

		//
		if(astSineSweepAxisCfg[ii].dAmpSineSweepRatio < afRatioRMS_DrvCmd[ii])
		{
			stSpectrumSysAna_GroupTestConfig.usAmplitudeSineSweep16bDAC[ii] = (unsigned short)(astSineSweepAxisCfg[ii].dAmpSineSweepRatio * 32767.0);
		}
		else
		{
			stSpectrumSysAna_GroupTestConfig.usAmplitudeSineSweep16bDAC[ii] = (unsigned short)(afRatioRMS_DrvCmd[ii] * 32767.0);
		}
	}
}
unsigned int mtn_aft_spectrum_calculate_total_num_cases(SPECTRUM_SYS_ANA_GROUP_CONFIG stGroupSpectrumTestConfig)
{
	int ii;
	unsigned int nTotalNumCasesGroupPrbsTest;
//	spectrum_dlg_consolidate_group_sys_ana_config();

	// Convert structure SPECTRUM_SYS_ANA_GROUP_CONFIG to PRBS_GROUP_TEST_INPUT
	unsigned short nTotalEnvolvedAxis, nTotalExciteAxis, nTotalNumGroupPoints;
	nTotalEnvolvedAxis = 0; nTotalExciteAxis= 0; nTotalNumCasesGroupPrbsTest = 1; nTotalNumGroupPoints = 1;
	// spanning for nTotalEnvolvedAxis
	static int aiExciteAxisList[MAX_NUM_AXIS_GROUP_SPECTRUM], aiEnvolvedAxisList[MAX_NUM_AXIS_GROUP_SPECTRUM], aiNumPointsActiveAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	static int iDeltaDistance[MAX_NUM_AXIS_GROUP_SPECTRUM], iCurrAxis, iCurrPointIdInAxis, iCurrTestPosn[MAX_NUM_AXIS_GROUP_SPECTRUM];
	iCurrAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD; // dummy
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
			// init
		iDeltaDistance[ii] = 0;
		if(stGroupSpectrumTestConfig.uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && stGroupSpectrumTestConfig.uiNumPoints[ii] > 0)
		{
			nTotalNumGroupPoints = nTotalNumGroupPoints * stGroupSpectrumTestConfig.uiNumPoints[ii];

			aiEnvolvedAxisList[nTotalEnvolvedAxis] = stGroupSpectrumTestConfig.uiAxis_CtrlCardId[ii];
			aiNumPointsActiveAxis[nTotalEnvolvedAxis] = stGroupSpectrumTestConfig.uiNumPoints[ii];

			nTotalEnvolvedAxis ++;

		}
		if(stGroupSpectrumTestConfig.ucFlagIsExciteAxis[ii] >0 && stGroupSpectrumTestConfig.uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			aiExciteAxisList[nTotalExciteAxis] = stGroupSpectrumTestConfig.uiAxis_CtrlCardId[ii];
			nTotalExciteAxis = nTotalExciteAxis + 1;
		}
	}

	stGroupSpectrumTestConfig.usActualNumAxis = nTotalEnvolvedAxis;
	nTotalNumCasesGroupPrbsTest = nTotalNumGroupPoints * nTotalExciteAxis;

	return nTotalNumCasesGroupPrbsTest;
}

unsigned int CMtnSpectrumTestDlg::spectrum_dlg_calculate_total_num_cases()
{
	int ii;
	unsigned int nTotalNumCasesGroupPrbsTest;
	spectrum_dlg_consolidate_group_sys_ana_config();

	// Convert structure SPECTRUM_SYS_ANA_GROUP_CONFIG to PRBS_GROUP_TEST_INPUT
	unsigned short nTotalEnvolvedAxis, nTotalExciteAxis, nTotalNumGroupPoints;
	nTotalEnvolvedAxis = 0; nTotalExciteAxis= 0; nTotalNumCasesGroupPrbsTest = 1; nTotalNumGroupPoints = 1;
	// spanning for nTotalEnvolvedAxis
	static int aiExciteAxisList[MAX_NUM_AXIS_GROUP_SPECTRUM], aiEnvolvedAxisList[MAX_NUM_AXIS_GROUP_SPECTRUM], aiNumPointsActiveAxis[MAX_NUM_AXIS_GROUP_SPECTRUM];
	static int iDeltaDistance[MAX_NUM_AXIS_GROUP_SPECTRUM], iCurrAxis, iCurrPointIdInAxis, iCurrTestPosn[MAX_NUM_AXIS_GROUP_SPECTRUM];
	iCurrAxis = MAX_CTRL_AXIS_PER_SERVO_BOARD; // dummy
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
			// init
		iDeltaDistance[ii] = 0;
		if(uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && uiNumPoints[ii] > 0)
		{
			nTotalNumGroupPoints = nTotalNumGroupPoints * uiNumPoints[ii];

			aiEnvolvedAxisList[nTotalEnvolvedAxis] = uiAxis_CtrlCardId[ii];
			aiNumPointsActiveAxis[nTotalEnvolvedAxis] = uiNumPoints[ii];

			nTotalEnvolvedAxis ++;

		}
		if(ucFlagIsExciteAxis[ii] >0 && uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			aiExciteAxisList[nTotalExciteAxis] = uiAxis_CtrlCardId[ii];
			nTotalExciteAxis = nTotalExciteAxis + 1;
		}
	}

	stSpectrumSysAna_GroupTestConfig.usActualNumAxis = nTotalEnvolvedAxis;
	nTotalNumCasesGroupPrbsTest = nTotalNumGroupPoints * nTotalExciteAxis;

	// SinSweep Group Config
	for(ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		// 
		stSineSweepGroupConfig.uiAxis_CtrlCardId[ii] = uiAxis_CtrlCardId[ii];
		stSineSweepGroupConfig.iLowerLimitPosition[ii] = iLowerLimitPosition[ii];
		stSineSweepGroupConfig.iUpperLimitPosition[ii] = iUpperLimitPosition[ii];
		stSineSweepGroupConfig.ucFlagIsExciteAxis[ii] = ucFlagIsExciteAxis[ii];
		stSineSweepGroupConfig.uiNumPoints [ii] = uiNumPoints[ii];
		stSineSweepGroupConfig.usAxisAmpSineSweep[ii] = (unsigned short)astSineSweepAxisCfg[ii].dAmpSineSweepRatio;
//		stSpectrumSysAna_GroupTestConfig.usAxisPrbsLen[ii] = usAxisPrbsLen[ii];
	}

	return nTotalNumCasesGroupPrbsTest;
}

// IDC_BUTTON_GROUP_PRBS_ESTIMATE_TIME
void CMtnSpectrumTestDlg::OnBnClickedButtonGroupPrbsEstimateTime()
{
unsigned int ii;

	nTotalNumCasesGroupSpecTest = spectrum_dlg_calculate_total_num_cases();

	CString cstrTemp;
	cstrTemp.Format("Total %d Cases", nTotalNumCasesGroupSpecTest);
	GetDlgItem(IDC_STATIC_GROUP_PRBS_TEST_STATUS)->SetWindowTextA(cstrTemp);

	// Generate PRBS_GROUP_TEST_INPUT for each case of Group-PRBS
	static PRBS_GROUP_TEST_INPUT *stpVolatilePrbsGroupTestCase;
	stpVolatilePrbsGroupTestCase = (PRBS_GROUP_TEST_INPUT *) calloc(nTotalNumCasesGroupSpecTest, sizeof(PRBS_GROUP_TEST_INPUT));
//	int  jj;
	unsigned int uiRandSeed;
	uiRandSeed = 0; // 20111024, Set the same test condition for all test, (unsigned int) time(NULL);

	aft_convert_group_prbs_cfg_to_test_input(stSpectrumSysAna_GroupTestConfig, 
											  stpVolatilePrbsGroupTestCase,
											  uiRandSeed);

	double dGroupPRBS_ExpectTime_ms = 0;
	double dControlTimeUnit_ms = aft_get_basic_time_unit();

	// Time for PRBS not consider moving time
	for(ii = 0; ii<nTotalNumCasesGroupSpecTest; ii++)
	{
		dGroupPRBS_ExpectTime_ms = dGroupPRBS_ExpectTime_ms + 
			stpVolatilePrbsGroupTestCase[ii].stPrbsTestOnceInput.usCountLenPRBS 
			* dControlTimeUnit_ms 
			* stpVolatilePrbsGroupTestCase[ii].stPrbsTestOnceInput.usFreqFactorPRBS;
	}
	// IDC_GROUP_PRBS_ROUGH_TIME
	cstrTemp.Format("Exp. Time PRBS: %6.0f (sec)", dGroupPRBS_ExpectTime_ms/1000);
	GetDlgItem(IDC_GROUP_PRBS_ROUGH_TIME)->SetWindowTextA(cstrTemp);

	delete(stpVolatilePrbsGroupTestCase);

}

// Spectrum Test Thread
UINT SpectrumTestThreadProc( LPVOID pParam )
{
    CMtnSpectrumTestDlg* pObject = (CMtnSpectrumTestDlg *)pParam;

	// 20121018
	if(iFlagTuningDriver || iFlagTuningVelLoopGain)
	{
		return pObject->SpectrumTuningThread();
	}
	else
	{
		return pObject->SpectrumTestThread(); 	
	}
}


#include "MtnVelStepTestDlg.h"
#include "MtnInitAcs.h"

UINT CMtnSpectrumTestDlg::DoGroupSpectrumTest()
{
static short sRet = MTN_API_OK_ZERO;
CTRL_PARA_ACS astBakupServoPara[8];
unsigned int uiAxisCtrlCard;
int jj;

// Backup and enlarge limit, 20120717
	mtn_wb_bakup_software_limit(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z()); // ACS_CARD_AXIS_A
	mtn_wb_enlarge_software_limit_by_(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z(), 10.0); // 2012July6

//  Backup Servo Protection limit //  Enlarge protection limit
	mtn_wb_bakup_servo_protect_limit(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z());
	mtn_wb_enlarge_servo_protect_limit(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z(), 8.0);

	for(jj = 0; jj< stpPrbsGroupTestCase[0].usActualNumAxis; jj++)  // stSpectrumSysAna_GroupTestConfig.usActualNumAxis, debug, 20111026
	{
		uiAxisCtrlCard = stpPrbsGroupTestCase[0].uiAxis_CtrlCardId[jj]; // stSpectrumSysAna_GroupTestConfig.uiAxis_CtrlCardId[jj];
		mtnapi_upload_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisCtrlCard, &astBakupServoPara[uiAxisCtrlCard]);
	}
	mtnapi_set_bond_z_acs_axis(stServoControllerCommSet.Handle);

	while(aft_spectrum_get_thread_flag_stop() == FALSE && nCurrCaseIdxGroupSpecTest < nTotalNumCasesGroupSpecTest)
	{

		if(aft_get_spectrum_sys_analysis_flag() == SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE) // SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE
		{
			// Check Safety
			for(jj =0; jj<stpPrbsGroupTestCase[0].usActualNumAxis; jj++)  // stSpectrumSysAna_GroupTestConfig.usActualNumAxis , debug, 20111026
			{
				uiAxisCtrlCard = stpPrbsGroupTestCase[0].uiAxis_CtrlCardId[jj]; // stSpectrumSysAna_GroupTestConfig.uiAxis_CtrlCardId[jj];
				if(mtn_qc_is_axis_locked_safe(stServoControllerCommSet.Handle, uiAxisCtrlCard) == FALSE)
				{
					mtnapi_download_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisCtrlCard, &astBakupServoPara[uiAxisCtrlCard]);
					aft_spectrum_set_thread_flag_stop(TRUE); // mFlagStopSpectrumTestThread = TRUE; // Error return
					nCurrCaseIdxGroupSpecTest = 0;  // 20120728
					break;
				}
			}

			if(aft_spectrum_get_thread_flag_stop() == FALSE)
			{
				sRet = aft_group_prbs_move_and_excite_once(&stpPrbsGroupTestCase[nCurrCaseIdxGroupSpecTest]);
				if(sRet != MTN_API_OK_ZERO)
				{ // AFT_SPEC_TEST_ERR_MOTOR_DISABLED
					aft_spectrum_set_thread_flag_stop(TRUE); // mFlagStopSpectrumTestThread = TRUE; // Error return
					nCurrCaseIdxGroupSpecTest = 0;  // 20120728
				}
				else // 20120728
				{
					nCurrCaseIdxGroupSpecTest ++;
				}
			}
		}
		else if (aft_get_spectrum_sys_analysis_flag() == SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP)
		{ // for each checked axis

//			for(static int ii=0; ii<nTotalNumCasesGroupSpecTest;
			int iMechaTronicsAxis = 
				aft_get_mechatr_axis_from_ctrl_axis(stpGroupSineSweepInputCases[nCurrCaseIdxGroupSpecTest].stSineSweepExciteOnce.uiExcitingAxis,
				get_sys_machine_type_flag());

			aft_spectrum_make_new_folder_axis(ucFlagIsExciteAxis[2], iMechaTronicsAxis);
			
			sRet = aft_group_sine_sweep_move_and_excite_multi_freq(&stpGroupSineSweepInputCases[nCurrCaseIdxGroupSpecTest]);

			if(sRet != MTN_API_OK_ZERO)
			{ // AFT_SPEC_TEST_ERR_MOTOR_DISABLED
				aft_spectrum_set_thread_flag_stop(TRUE); // mFlagStopSpectrumTestThread = TRUE; // Error return
			}
			nCurrCaseIdxGroupSpecTest ++;
		}  
		Sleep(100);
	}
	aft_spectrum_save_servo_parameter_into_spectrum_folder(stServoControllerCommSet.Handle); //20120815

	aft_set_spectrum_sys_analysis_flag(SYSTEM_ANALYSIS_PSEUDO_WHITE_NOISE); // Set to default
	GetDlgItem(IDC_BUTTON_START_GROUP_SINE_SWEEP)->EnableWindow(TRUE);

	aft_restore_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);
	aft_restore_servo_control_for_spec_axis(&stpPrbsGroupTestCase[0]);  // 20110720
//Restore the limit
	mtn_wb_restore_software_limit(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z()); // ACS_CARD_AXIS_A
	mtn_wb_restore_servo_protect_limit(stServoControllerCommSet.Handle, ACS_CARD_AXIS_X, ACS_CARD_AXIS_Y, sys_get_acs_axis_id_bnd_z());

	if(nCurrCaseIdxGroupSpecTest != nTotalNumCasesGroupSpecTest)
	{
		Beep(523, 2000); Beep(587, 2000); Beep(659, 2000); if(sRet != MTN_API_OK_ZERO) AfxMessageBox(_T("Error motion."));
	}
	else
	{
		mtn_dll_music_logo_normal_stop(); // 20120217
	}

	return sRet;
}

typedef struct
{
	double dVKP;
	double dVKI;
	double dSOF;
}_TUNING_VEL_LOOP;

#define __TOTAL_NUM_CASE_VLOOP_GAIN__    5
#define __TOTAL_NUM_CASE_SOF__          8

#define __TOTAL_NUM_POINTS_TUNING_SPACE  (__TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_SOF__ * __TOTAL_NUM_CASE_VLOOP_GAIN__)
_TUNING_VEL_LOOP astTuningVelloop[__TOTAL_NUM_POINTS_TUNING_SPACE];


double adTuningVKP[__TOTAL_NUM_POINTS_TUNING_SPACE], adTuningVKI[__TOTAL_NUM_POINTS_TUNING_SPACE], adTuningSOF[__TOTAL_NUM_POINTS_TUNING_SPACE];


UINT CMtnSpectrumTestDlg::SpectrumTuningThread()
{
	if(uiNumPoints[2] >= 2)
	{
		AfxMessageBox("Remember to remove capillary, 注意保护磁嘴");
	}
	// if enable tuning driver check
	//   check driver communication
	//   for connected axis
	//   5 points from lower limit to upper limit
	// 
double adLowBoundVKP[MAX_NUM_AXIS_GROUP_SPECTRUM], adLowBoundVKI[MAX_NUM_AXIS_GROUP_SPECTRUM], adLowBoundSOF[MAX_NUM_AXIS_GROUP_SPECTRUM];
double adUppBoundVKP[MAX_NUM_AXIS_GROUP_SPECTRUM], adUppBoundVKI[MAX_NUM_AXIS_GROUP_SPECTRUM], adUppBoundSOF[MAX_NUM_AXIS_GROUP_SPECTRUM];
double adDeltaVKP[MAX_NUM_AXIS_GROUP_SPECTRUM], adDeltaVKI[MAX_NUM_AXIS_GROUP_SPECTRUM], adDeltaSOF[MAX_NUM_AXIS_GROUP_SPECTRUM];

// Hard coded, to be modified
	switch(cFlagBwVelLoopScanningList)
	{
	case __FLAG_BW_SCANNINT_LINEAR_UP:
	// Table X
		adLowBoundVKP[0] = 50; adUppBoundVKP[0]= 250;  
		adLowBoundVKI[0] = 100; adUppBoundVKI[0] = 1500;
		adLowBoundSOF[0] = 500; adUppBoundSOF[0] = 1500;

	// Table Y
		adLowBoundVKP[1] = 50; adUppBoundVKP[1]= 250;
		adLowBoundVKI[1] = 100; adUppBoundVKI[1] = 1500;
		adLowBoundSOF[1] = 500; adUppBoundSOF[1] = 1750;
	// Bnd-Z
		adLowBoundVKP[2] = 5; adUppBoundVKP[2]= aiTuneMaxVKP[2];
		adLowBoundVKI[2] = 100; adUppBoundVKI[2] = 2500;
		adLowBoundSOF[2] = 1000; adUppBoundSOF[2] = aiTuneMaxSOF[2];
		break;
	case __FLAG_BW_SCANNING_MATRIX_UP_0:
	// Table X
		adLowBoundVKP[0] = 50; adUppBoundVKP[0]= 250;  
		adLowBoundVKI[0] = 300; adUppBoundVKI[0] = 1500;
		adLowBoundSOF[0] = 500; adUppBoundSOF[0] = 1500;

	// Table Y
		adLowBoundVKP[1] = 50; adUppBoundVKP[1]= 250;
		adLowBoundVKI[1] = 300; adUppBoundVKI[1] = 1500;
		adLowBoundSOF[1] = 500; adUppBoundSOF[1] = 1500;
	// Bnd-Z
		adLowBoundVKP[2] = 5; adUppBoundVKP[2]= aiTuneMaxVKP[2];
		adLowBoundVKI[2] = 100; adUppBoundVKI[2] = 2500;
		adLowBoundSOF[2] = 1000; adUppBoundSOF[2] = aiTuneMaxSOF[2];
		break;

	case __FLAG_BW_SCANNING_MATRIX_UP_1:
	// Table X
		adLowBoundVKP[0] = 50; adUppBoundVKP[0]= aiTuneMaxVKP[0];  
		adLowBoundVKI[0] = 100; //adUppBoundVKI[0] = 1500;
		adLowBoundSOF[0] = 500; adUppBoundSOF[0] = aiTuneMaxSOF[0];

	// Table Y
		adLowBoundVKP[1] = 50; adUppBoundVKP[1]= aiTuneMaxVKP[1];
		adLowBoundVKI[1] = 100; //adUppBoundVKI[1] = 1500;
		adLowBoundSOF[1] = 500; adUppBoundSOF[1] = aiTuneMaxSOF[1];
	// Bnd-Z
		adLowBoundVKP[2] = 5; adUppBoundVKP[2]= aiTuneMaxVKP[2];
		adLowBoundVKI[2] = 100; //adUppBoundVKI[2] = 2500;
		adLowBoundSOF[2] = 1000; adUppBoundSOF[2] = aiTuneMaxSOF[2];
		break;
	}
//
//double adVKP[MAX_NUM_AXIS_GROUP_SPECTRUM], adVKI[MAX_NUM_AXIS_GROUP_SPECTRUM], adSOF[MAX_NUM_AXIS_GROUP_SPECTRUM];
int jj, iiVKP, iiVKI, iiSOF;
int uiAxisCtrlCard, iRet;
CTRL_PARA_ACS stServoParaTuning, stBakupServoParaTuning;

	for(jj = 0; jj< MAX_NUM_AXIS_GROUP_SPECTRUM; jj++)  // stSpectrumSysAna_GroupTestConfig.usActualNumAxis, debug, 20111026
	{
		if(stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[jj] == 1)
		{
			switch(cFlagBwVelLoopScanningList)
			{
			case __FLAG_BW_SCANNINT_LINEAR_UP:
			case __FLAG_BW_SCANNING_MATRIX_UP_0:
					adDeltaVKP[jj] = (adUppBoundVKP[jj] - adLowBoundVKP[jj])/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1);
					adDeltaVKI[jj] = (adUppBoundVKI[jj] - adLowBoundVKI[jj])/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1);
					adDeltaSOF[jj] = (adUppBoundSOF[jj] - adLowBoundSOF[jj])/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1);
				break;

			case __FLAG_BW_SCANNING_MATRIX_UP_1:
				for(jj =0; jj< MAX_NUM_AXIS_GROUP_SPECTRUM; jj++)
				{
					adDeltaVKP[jj] = (adUppBoundVKP[jj] - adLowBoundVKP[jj])/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1);
					adDeltaVKI[jj] = (adUppBoundVKI[jj] - adLowBoundVKI[jj])/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1);
					adDeltaSOF[jj] = (adUppBoundSOF[jj] - adLowBoundSOF[jj])/(__TOTAL_NUM_CASE_SOF__ - 1);
				}
				break;
			}
		}
	}	
//	nTotalCaseTuningVK = __TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_VLOOP_GAIN__ *
//		(stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[0] + stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[1] +
//			stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[2]);

	jj = 0; // For XY only
int iiCaseVelLoop, nTotalCaseVelLoopScanning;
static SPECTRUM_SYS_ANA_GROUP_CONFIG stSpectrumGroupTestConfigTemp;

	stSpectrumGroupTestConfigTemp = stSpectrumSysAna_GroupTestConfig;

double aVelWhiteNoiseAmplitudeWithPeak[MAX_NUM_AXIS_GROUP_SPECTRUM];
	aVelWhiteNoiseAmplitudeWithPeak[0] = aVelWhiteNoiseAmplitudeWithPeak[1] = 500 * 200; // XY Table
	aVelWhiteNoiseAmplitudeWithPeak[2] = 1000 * 15; // BondHead, 4212Z Driving 8A/10V

	for(jj = 0; jj< MAX_NUM_AXIS_GROUP_SPECTRUM; jj++)  // stSpectrumSysAna_GroupTestConfig.usActualNumAxis, debug, 20111026
	{
		iiVKP = 0, iiVKI = 0, iiSOF = 0;
		nCurrCaseTuningVK = 0;

		if(stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[jj] == 1)
		{
			// 20121112
			for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
			{
				if(ii == jj)
				{
					stSpectrumGroupTestConfigTemp.ucFlagIsExciteAxis[ii] = 1; 
				}
				else
				{
					stSpectrumGroupTestConfigTemp.ucFlagIsExciteAxis[ii] = 0; 
				}
			}
			nTotalNumCasesGroupSpecTest = mtn_aft_spectrum_calculate_total_num_cases(stSpectrumGroupTestConfigTemp); // 20121112

			uiAxisCtrlCard = stSpectrumSysAna_GroupTestConfig.uiAxis_CtrlCardId[jj]; 

			mtnapi_upload_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisCtrlCard, 
				&stServoParaTuning);
			stBakupServoParaTuning = stServoParaTuning;

			/////////////// Generate the tuning list
			iiCaseVelLoop = 0;
			switch(cFlagBwVelLoopScanningList)
			{
			case __FLAG_BW_SCANNINT_LINEAR_UP:
				stServoParaTuning.dVelocityLoopProportionalGain = adLowBoundVKP[jj];
				stServoParaTuning.dVelocityLoopIntegratorGain = adLowBoundVKI[jj];
				stServoParaTuning.dSecondOrderLowPassFilterBandwidth = adLowBoundSOF[jj];
				for(iiVKP = 0; iiVKP<__TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKP++)
				{
					stServoParaTuning.dVelocityLoopProportionalGain += adDeltaVKP[jj]; // + (iiVKP * adDeltaVKP[jj]);
					for(iiVKI = 0; iiVKI < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKI++)
					{
						stServoParaTuning.dVelocityLoopIntegratorGain += adDeltaVKI[jj]; // adLowBoundVKI[jj] * (__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1 - iiVKI)/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1) +

						for(iiSOF = 0; iiSOF < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiSOF++) // 20121023
						{
							stServoParaTuning.dSecondOrderLowPassFilterBandwidth += adDeltaSOF[jj]; // = adLowBoundSOF[jj] + (iiSOF * adDeltaSOF[jj]);
						
							astTuningVelloop[iiCaseVelLoop].dVKP = stServoParaTuning.dVelocityLoopProportionalGain;
							astTuningVelloop[iiCaseVelLoop].dSOF = stServoParaTuning.dSecondOrderLowPassFilterBandwidth;
							astTuningVelloop[iiCaseVelLoop].dVKI = stServoParaTuning.dVelocityLoopIntegratorGain;

							iiCaseVelLoop ++;
						}
					}
				}
				break;
			case __FLAG_BW_SCANNING_MATRIX_UP_0:
				// = ;
				//stServoParaTuning.dVelocityLoopIntegratorGain = ;
				//stServoParaTuning.dSecondOrderLowPassFilterBandwidth = ;
				for(iiVKP = 0; iiVKP<__TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKP++)
				{
					stServoParaTuning.dVelocityLoopProportionalGain = adLowBoundVKP[jj] + iiVKP * adDeltaVKP[jj]; // + (iiVKP * adDeltaVKP[jj]);
					for(iiVKI = 0; iiVKI < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKI++)
					{
						stServoParaTuning.dVelocityLoopIntegratorGain = adLowBoundVKI[jj] + iiVKI * adDeltaVKI[jj]; // adLowBoundVKI[jj] * (__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1 - iiVKI)/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1) +

						for(iiSOF = 0; iiSOF < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiSOF++) // 20121023
						{
							stServoParaTuning.dSecondOrderLowPassFilterBandwidth = adLowBoundSOF[jj] + iiSOF * adDeltaSOF[jj]; // = adLowBoundSOF[jj] + (iiSOF * adDeltaSOF[jj]);
						
							astTuningVelloop[iiCaseVelLoop].dVKP = stServoParaTuning.dVelocityLoopProportionalGain;
							astTuningVelloop[iiCaseVelLoop].dSOF = stServoParaTuning.dSecondOrderLowPassFilterBandwidth;
							astTuningVelloop[iiCaseVelLoop].dVKI = stServoParaTuning.dVelocityLoopIntegratorGain;

							iiCaseVelLoop ++;
						}
					}
				}
				break;
			case __FLAG_BW_SCANNING_MATRIX_UP_1:
				for(iiVKP = 0; iiVKP<__TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKP++)
				{
					stServoParaTuning.dVelocityLoopProportionalGain = adLowBoundVKP[jj] + (iiVKP * adDeltaVKP[jj]);
					for(iiSOF = 0; iiSOF < __TOTAL_NUM_CASE_SOF__; iiSOF++) // 20121023
					{
						stServoParaTuning.dSecondOrderLowPassFilterBandwidth = adLowBoundSOF[jj] + (iiSOF * adDeltaSOF[jj]);
						
						for(iiVKI = 0; iiVKI < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKI++)
						{
							stServoParaTuning.dVelocityLoopIntegratorGain = adLowBoundVKI[jj] * (__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1 - iiVKI)/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1) +
									stServoParaTuning.dSecondOrderLowPassFilterBandwidth * iiVKI/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1); // + (iiVKI * adDeltaVKI[jj]);
							
							astTuningVelloop[iiCaseVelLoop].dVKP = stServoParaTuning.dVelocityLoopProportionalGain;
							astTuningVelloop[iiCaseVelLoop].dSOF = stServoParaTuning.dSecondOrderLowPassFilterBandwidth;
							astTuningVelloop[iiCaseVelLoop].dVKI = stServoParaTuning.dVelocityLoopIntegratorGain;

							iiCaseVelLoop ++;
						}
					}
				}
				break;
			}

			nTotalCaseVelLoopScanning = iiCaseVelLoop;
			nTotalCaseTuningVK = nTotalCaseVelLoopScanning; // __TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_VLOOP_GAIN__ * __TOTAL_NUM_CASE_SOF__;

			iiCaseVelLoop =0;
			/////////////// Excite for each axis according to the scanning path
			//stServoParaTuning.dVelocityLoopProportionalGain = adLowBoundVKP[jj];
			//stServoParaTuning.dVelocityLoopIntegratorGain = adLowBoundVKI[jj];
			//stServoParaTuning.dSecondOrderLowPassFilterBandwidth = adLowBoundSOF[jj];

			for(iiCaseVelLoop = 0; iiCaseVelLoop < nTotalCaseVelLoopScanning; iiCaseVelLoop ++)
			{
			//for(iiVKP = 0; iiVKP<__TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKP++)
			//{
//				stServoParaTuning.dVelocityLoopProportionalGain = adLowBoundVKP[jj] + (iiVKP * adDeltaVKP[jj]);

				//for(iiSOF = 0; iiSOF < __TOTAL_NUM_CASE_SOF__; iiSOF++) // 20121023
				//{
//					stServoParaTuning.dSecondOrderLowPassFilterBandwidth = adLowBoundSOF[jj] + (iiSOF * adDeltaSOF[jj]);

					//for(iiVKI = 0; iiVKI < __TOTAL_NUM_CASE_VLOOP_GAIN__; iiVKI++)
					//{
//						stServoParaTuning.dVelocityLoopIntegratorGain = adLowBoundVKI[jj] * (__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1 - iiVKI)/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1) +
//							stServoParaTuning.dSecondOrderLowPassFilterBandwidth * iiVKI/(__TOTAL_NUM_CASE_VLOOP_GAIN__ - 1); // + (iiVKI * adDeltaVKI[jj]);
						stServoParaTuning.dVelocityLoopProportionalGain = astTuningVelloop[iiCaseVelLoop].dVKP;
						stServoParaTuning.dSecondOrderLowPassFilterBandwidth = astTuningVelloop[iiCaseVelLoop].dSOF;
						stServoParaTuning.dVelocityLoopIntegratorGain = astTuningVelloop[iiCaseVelLoop].dVKI;

						if(stServoParaTuning.dVelocityLoopProportionalGain > stBakupServoParaTuning.dVelocityLoopProportionalGain)
						{
							stSpectrumGroupTestConfigTemp.usAxisAmpPrbs[jj] = (unsigned short)(aVelWhiteNoiseAmplitudeWithPeak[jj]/stServoParaTuning.dVelocityLoopProportionalGain); // 20121112
						}
						aft_convert_group_prbs_cfg_to_test_input(stSpectrumGroupTestConfigTemp, stpPrbsGroupTestCase,  0); // 20121112

						mtnapi_disable_motor(stServoControllerCommSet.Handle, uiAxisCtrlCard, 0);
						Sleep(50);
						iRet = mtnapi_download_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisCtrlCard, 
							&stServoParaTuning);
						Sleep(50);
						iRet = mtnapi_enable_motor(stServoControllerCommSet.Handle, uiAxisCtrlCard, 0);
						Sleep(50);
						nCurrCaseIdxGroupSpecTest = 0;

						aft_spectrum_make_new_folder_tuning_vk(stSpectrumSysAna_GroupTestConfig.ucFlagIsExciteAxis[2], 
							nCurrCaseTuningVK, nTotalCaseTuningVK);

						aft_white_noise_spectrum_make_master_file(stSpectrumSysAna_GroupTestConfig, 
											  stpPrbsGroupTestCase);

						DoGroupSpectrumTest();
						nCurrCaseTuningVK ++;

				//	}
				//}
			}

			mtnapi_download_servo_parameter_acs_per_axis(stServoControllerCommSet.Handle, uiAxisCtrlCard, 
				&stBakupServoParaTuning);
			
		}
	}

	// if enable tuning velocity loop check
	//   for excited axis
	//   
	//   5 points from lower limit to upper limit, VKP
	//   5 points from lower limit to upper limit, VKI
	//   5 points from lower limit to upper limit, SOF

	acs_stop_buffer_prog_prbs_prof_cfg_move();
	acs_compile_buffer_prog_prbs_prof_cfg_move();
	acs_stop_buffer_prog_vel_loop_prof_cfg_move2();
	acs_compile_buffer_prog_vel_loop_prof_cfg_move2();
	UI_EnableEditTuningMaxParameter(aft_spectrum_get_debug_flag()); // 20121018

	return 0;
}

#include "AftVelLoopTest.h"
extern AFT_GROUP_VEL_STEP_TEST_CONFIG stVelStepGroupTestConfig;
UINT CMtnSpectrumTestDlg::SpectrumTestThread()
{
	short sRet = MTN_API_OK_ZERO;

	if(uiNumPoints[2] >= 2)
	{
		AfxMessageBox("Remember to remove capillary, 注意保护磁嘴");
	}

	sRet = DoGroupSpectrumTest();

	acs_stop_buffer_prog_prbs_prof_cfg_move();
	acs_compile_buffer_prog_prbs_prof_cfg_move();
	acs_stop_buffer_prog_vel_loop_prof_cfg_move2();
	acs_compile_buffer_prog_vel_loop_prof_cfg_move2();

	if(	iFlagGotoVelStepAfterSpectrumTest == 1 
		&& nCurrCaseIdxGroupSpecTest == nTotalNumCasesGroupSpecTest
		&& sRet == MTN_API_OK_ZERO)
	{
		if(cpVelStepGroupTest == NULL)
		{
			cpVelStepGroupTest = new CMtnVelStepTestDlg();
			cpVelStepGroupTest->Create(IDD_DLG_GROUP_VEL_STEP_TEST);
		}
		cpVelStepGroupTest->InitVelStepGroupTestCfg();
		cpVelStepGroupTest->ShowWindow(1);
int iFlagDoVelStepTestCheckOK = TRUE;
		if(stVelStepGroupTestConfig.uiAxis_CtrlCardId[0] < MAX_CTRL_AXIS_PER_SERVO_BOARD 
			&& mtn_qc_is_axis_locked_safe(stServoControllerCommSet.Handle, stVelStepGroupTestConfig.uiAxis_CtrlCardId[0]) == FALSE)
		{
			iFlagDoVelStepTestCheckOK = FALSE;
		}
		if(stVelStepGroupTestConfig.uiAxis_CtrlCardId[1] < MAX_CTRL_AXIS_PER_SERVO_BOARD
			&& mtn_qc_is_axis_locked_safe(stServoControllerCommSet.Handle, stVelStepGroupTestConfig.uiAxis_CtrlCardId[1]) == FALSE)
		{
			iFlagDoVelStepTestCheckOK = FALSE;
		}
		if(stVelStepGroupTestConfig.uiAxis_CtrlCardId[2] < MAX_CTRL_AXIS_PER_SERVO_BOARD
			&& mtn_qc_is_axis_locked_safe(stServoControllerCommSet.Handle, stVelStepGroupTestConfig.uiAxis_CtrlCardId[2]) == FALSE)
		{
			iFlagDoVelStepTestCheckOK = FALSE;
		}
		if(iFlagDoVelStepTestCheckOK == TRUE)
		{
			cpVelStepGroupTest->OnBnClickedButtonStartGroupVelStep();
		}
		// InitVelStepGroupTestCfg
	}

	return 0;
}

void CMtnSpectrumTestDlg::RunSpectrumTestThread()
{
	aft_spectrum_set_thread_flag_stop(FALSE); // mFlagStopSpectrumTestThread = FALSE;

	UI_EnableEditTuningMaxParameter(aft_spectrum_get_debug_flag()); // 20121018
	m_pWinThreadSpecTest = AfxBeginThread(SpectrumTestThreadProc, this);
	SetPriorityClass(m_pWinThreadSpecTest->m_hThread, REALTIME_PRIORITY_CLASS);

	m_pWinThreadSpecTest->m_bAutoDelete = FALSE;
}

void CMtnSpectrumTestDlg::StopSpectrumTestThread()
{
	if (m_pWinThreadSpecTest)
	{
		aft_spectrum_set_thread_flag_stop(TRUE); // mFlagStopSpectrumTestThread = TRUE;
		WaitForSingleObject(m_pWinThreadSpecTest->m_hThread, 1000);
		// delete m_pWinThreadSpecTest;
		m_pWinThreadSpecTest = NULL;
	}
}
void CMtnSpectrumTestDlg::OnBnClickedButtonStopSpectrumTest()
{
	// IDC_BUTTON_STOP_SPECTRUM_TEST
	StopSpectrumTestThread();
	if(aft_get_spectrum_test_bakup_flag() == TRUE)		aft_restore_speedprofile_for_spec_axis(&stpPrbsGroupTestCase[0]);
	aft_restore_servo_control_for_spec_axis(&stpPrbsGroupTestCase[0]);  // 20110720
	acs_stop_buffer_prog_prbs_prof_cfg_move();
	acs_compile_buffer_prog_prbs_prof_cfg_move();
	acs_stop_buffer_prog_vel_loop_prof_cfg_move2();
	acs_compile_buffer_prog_vel_loop_prof_cfg_move2();
}

void CMtnSpectrumTestDlg::SetUserInterfaceLanguage(int iLanguageOption)
{
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_STATIC_LABEL_GROUP_PRBS)->SetWindowTextA(_T("Group PRBS Test"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_AXIS_LIST)->SetWindowTextA(_T("Axis Envolved"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FLAG_IS_EXCITING_AXIS)->SetWindowTextA(_T("ExciteAxis"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_NUM_POINTS_PER_AXIS)->SetWindowTextA(_T("NumPoints"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_LOW_LIMIT_PER_AXIS)->SetWindowTextA(_T("LowLimit"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_UPP_LIMIT_PER_AXIS)->SetWindowTextA(_T("UppLimit"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_AMP_PER_AXIS)->SetWindowTextA(_T("PrbsAmp"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_LEN_PER_AXIS)->SetWindowTextA(_T("PrbsLen"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FREQ_FACTOR_PER_AXIS)->SetWindowTextA(_T("FreqFactor"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FLAG_HAS_OFFSET_PER_AXIS)->SetWindowTextA(_T("Offset"));
		GetDlgItem(IDC_BUTTON_GROUP_PRBS_ESTIMATE_TIME)->SetWindowTextA(_T("EstimateTime"));
		GetDlgItem(IDC_BUTTON_START_GROUP_PRBS)->SetWindowTextA(_T("StartGroupTest"));
		GetDlgItem(IDC_BUTTON_STOP_SPECTRUM_TEST)->SetWindowTextA(_T("StopTest"));
		GetDlgItem(IDC_BUTTON_SAVE_GROUP_PRBS_CONFIG)->SetWindowTextA(_T("SaveCfg"));
		GetDlgItem(IDC_BUTTON_LOAD_GROUP_PRBS_CONFIG)->SetWindowTextA(_T("LoadCfg"));
		GetDlgItem(IDC_STATIC_LABEL_PRBS_TEST_ONCE)->SetWindowTextA(_T("PseudoRandomBinarySequence"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_COUNT_LEN)->SetWindowTextA(_T("CountLen"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_AMP)->SetWindowTextA(_T("Amplitude"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_FREQ_FACTOR)->SetWindowTextA(_T("FreqFactor"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_EXCITE_AXIS)->SetWindowTextA(_T("ExciteAxis"));
		GetDlgItem(IDC_BUTTON_SPECTRUM_TEST)->SetWindowTextA(_T("StartOnce"));
		GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP)->SetWindowTextA(_T("VelLoop"));
		GetDlgItem(IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER)->SetWindowTextA(_T("->VelLpTest"));
		
	}
	else
	{
		GetDlgItem(IDC_STATIC_LABEL_GROUP_PRBS)->SetWindowTextA(_T("组合噪声测试"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_AXIS_LIST)->SetWindowTextA(_T("控制器轴"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FLAG_IS_EXCITING_AXIS)->SetWindowTextA(_T("激发噪声"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_NUM_POINTS_PER_AXIS)->SetWindowTextA(_T("总点数"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_LOW_LIMIT_PER_AXIS)->SetWindowTextA(_T("下限位置"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_UPP_LIMIT_PER_AXIS)->SetWindowTextA(_T("上限位置"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_AMP_PER_AXIS)->SetWindowTextA(_T("噪声幅度"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_LEN_PER_AXIS)->SetWindowTextA(_T("时间点数"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FREQ_FACTOR_PER_AXIS)->SetWindowTextA(_T("分频"));
		GetDlgItem(IDC_STATIC_GROUP_PRBS_FLAG_HAS_OFFSET_PER_AXIS)->SetWindowTextA(_T("偏置"));
		GetDlgItem(IDC_BUTTON_GROUP_PRBS_ESTIMATE_TIME)->SetWindowTextA(_T("估计时间"));
		GetDlgItem(IDC_BUTTON_START_GROUP_PRBS)->SetWindowTextA(_T("开始组合测试"));
		GetDlgItem(IDC_BUTTON_STOP_SPECTRUM_TEST)->SetWindowTextA(_T("停止测试"));
		GetDlgItem(IDC_BUTTON_SAVE_GROUP_PRBS_CONFIG)->SetWindowTextA(_T("保存配置"));
		GetDlgItem(IDC_BUTTON_LOAD_GROUP_PRBS_CONFIG)->SetWindowTextA(_T("加载配置"));
		GetDlgItem(IDC_STATIC_LABEL_PRBS_TEST_ONCE)->SetWindowTextA(_T("单次测试"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_COUNT_LEN)->SetWindowTextA(_T("时间点数"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_AMP)->SetWindowTextA(_T("噪声幅度"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_FREQ_FACTOR)->SetWindowTextA(_T("分频"));
		GetDlgItem(IDC_STATIC_PRBS_ONCE_EXCITE_AXIS)->SetWindowTextA(_T("激发源轴"));
		GetDlgItem(IDC_BUTTON_SPECTRUM_TEST)->SetWindowTextA(_T("开始测试"));
		GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_VEL_LOOP)->SetWindowTextA(_T("速度闭环"));
		GetDlgItem(IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER)->SetWindowTextA(_T("运行速度环"));
	}
}

// IDC_CHECK_SPECTRUM_GROUP_TEST_DEBUG
#include "DlgKeyInputPad.h"
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumGroupTestDebug()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_SPECTRUM_GROUP_TEST_DEBUG))->GetCheck();
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
			((CButton *)GetDlgItem(IDC_CHECK_SPECTRUM_GROUP_TEST_DEBUG))->SetCheck(FALSE);
		}
	}
	else
	{
		UI_EnableEditDebug(iTemp);
	}
	aft_spectrum_set_debug_flag(iTemp);  // iFlagEnableDebugGroupPRBS = iTemp;
	UI_EnableEditDebug(aft_spectrum_get_debug_flag()); // iFlagEnableDebugGroupPRBS);
	UI_EnableEditTuningMaxParameter(aft_spectrum_get_debug_flag());

}

void CMtnSpectrumTestDlg::UI_EnableEditDebug(int iEnableFlag)
{
	GetDlgItem(IDC_COMBO_AXIS1_GROUP_PRBS)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS2_GROUP_PRBS)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS3_GROUP_PRBS)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS4_GROUP_PRBS)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_AMP_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_LEN_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_FREQ_FACTOR_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_PRBS_VEL_AMP_GROUP_PRBS_AXIS4)->EnableWindow(iEnableFlag);
	
	GetDlgItem(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS1)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS2)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS3)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS4)->EnableWindow(iEnableFlag);

	GetDlgItem(IDC_EDIT_SINE_SWEEP_LOW_FREQ_HZ)->EnableWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SINE_SWEEP_UPP_FREQ_HZ)->EnableWindow(iEnableFlag);
	
	GetDlgItem(IDC_STATIC_LABEL_PRBS_TEST_ONCE)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_PRBS_ONCE_COUNT_LEN)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_LEN)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_PRBS_ONCE_AMP)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_AMPLITUDE)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_PRBS_ONCE_FREQ_FACTOR)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_EDIT_SPECTRUM_PRBS_FREQ_FACTOR)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_STATIC_PRBS_ONCE_EXCITE_AXIS)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_COMBO_SPECTRUM_TEST_AXIS)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_CHECK_SPECTRUM_TEST_DEBUG)->ShowWindow(iEnableFlag);
	GetDlgItem(IDC_BUTTON_SPECTRUM_TEST)->ShowWindow(iEnableFlag);

}

void CMtnSpectrumTestDlg::UI_EnableEditTuningMaxParameter(BOOL bEnableFlag)
{
	GetDlgItem(IDC_EDIT_MAX_VKP_1_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); // 20121018
	GetDlgItem(IDC_EDIT_MAX_VKP_2_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	GetDlgItem(IDC_EDIT_MAX_VKP_3_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	GetDlgItem(IDC_EDIT_MAX_VKP_4_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	
	GetDlgItem(IDC_EDIT_MAX_SOF_1_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	GetDlgItem(IDC_EDIT_MAX_SOF_2_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	GetDlgItem(IDC_EDIT_MAX_SOF_3_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); 
	GetDlgItem(IDC_EDIT_MAX_SOF_4_SPECTRUM_DLG)->EnableWindow(bEnableFlag && iFlagTuningVelLoopGain && aft_spectrum_get_thread_flag_stop() ); // 20121018
}

// IDC_CHECK_SPECTRUM_FLAG_DSP_DATA
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumFlagDspData()
{
	char cFlag = ((CButton*)GetDlgItem(IDC_CHECK_SPECTRUM_FLAG_DSP_DATA))->GetCheck();
	aft_spectrum_set_flag_dsp_data(cFlag);
}
// IDC_EDIT_SINE_SWEEP_LOW_FREQ_HZ
void CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepLowFreqHz()
{
	ReadDoubleFromEdit(IDC_EDIT_SINE_SWEEP_LOW_FREQ_HZ, &stSineSweepSystemCfg.dLowFreq_Hz);
	for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		astSineSweepAxisCfg[ii].stSineSweepSysCfg = stSineSweepSystemCfg;
	}

}
// IDC_EDIT_SINE_SWEEP_UPP_FREQ_HZ
void CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepUppFreqHz()
{
	ReadDoubleFromEdit(IDC_EDIT_SINE_SWEEP_UPP_FREQ_HZ, &stSineSweepSystemCfg.dUppFreq_Hz);
	for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		astSineSweepAxisCfg[ii].stSineSweepSysCfg = stSineSweepSystemCfg;
	}
}
// IDC_EDIT_SINE_SWEEP_NUM_CASES_PER_DEC
void CMtnSpectrumTestDlg::OnEnKillfocusEditSineSweepNumCasesPerDec()
{
	ReadUnsigndShortFromEdit(IDC_EDIT_SINE_SWEEP_NUM_CASES_PER_DEC, &stSineSweepSystemCfg.usNumCaseDec);
	for(int ii=0; ii<MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		astSineSweepAxisCfg[ii].stSineSweepSysCfg = stSineSweepSystemCfg;
	}
}
// IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS1
void CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis1()
{
	ReadDoubleFromEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS1, &astSineSweepAxisCfg[0].dAmpSineSweepRatio);
}
// IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS2
void CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis2()
{
	ReadDoubleFromEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS2, &astSineSweepAxisCfg[1].dAmpSineSweepRatio);
}
// IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS3
void CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis3()
{
	ReadDoubleFromEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS3, &astSineSweepAxisCfg[2].dAmpSineSweepRatio);
}
// IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS4
void CMtnSpectrumTestDlg::OnEnKillfocusEditSpectrumGroupSineSweepAmpAxis4()
{
	ReadDoubleFromEdit(IDC_EDIT_SPECTRUM_GROUP_SINE_SWEEP_AMP_AXIS4, &astSineSweepAxisCfg[3].dAmpSineSweepRatio);
}

unsigned short aft_spectrum_build_sinesweep_from_sys_ana_cfg(SPECTRUM_SYS_ANA_GROUP_CONFIG *stpSysAnaSpectrumCfg, SINE_SWEEP_AXIS_CONFIG *astSineSweepAxisCfg)
{
	unsigned short usActualNumAxis, usActualTotalExciteCase;
	usActualNumAxis = 0; usActualTotalExciteCase = 0;
	for(int ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		if(stpSysAnaSpectrumCfg->uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && stpSysAnaSpectrumCfg->uiNumPoints[ii] > 0)
		{
			usActualNumAxis ++;
			if(stpSysAnaSpectrumCfg->ucFlagIsExciteAxis[ii] == TRUE)
			{
				usActualTotalExciteCase ++;
			}
		}
	}

	// Each axis excite one
	int idxCurrExciteCase;
	idxCurrExciteCase = 0;
	for(int ii = 0; ii< MAX_NUM_AXIS_GROUP_SPECTRUM; ii++)
	{
		if(stpSysAnaSpectrumCfg->ucFlagIsExciteAxis[ii] == TRUE &&
			stpSysAnaSpectrumCfg->uiAxis_CtrlCardId[ii] < MAX_CTRL_AXIS_PER_SERVO_BOARD && stpSysAnaSpectrumCfg->uiNumPoints[ii] > 0)
		{
			stpGroupSineSweepInputCases[idxCurrExciteCase].stSineSweepExciteOnce.uiExcitingAxis = stpSysAnaSpectrumCfg->uiAxis_CtrlCardId[ii];	
			idxCurrExciteCase ++;
		}
	}

	for(unsigned int ii=0; ii<usActualTotalExciteCase; ii++)
	{
		stpGroupSineSweepInputCases[ii].usActualNumAxis = usActualNumAxis;
		for(unsigned int jj=0; jj<usActualNumAxis; jj++)
		{
			stpGroupSineSweepInputCases[ii].iAxisPosn[jj] = (int)(stpSysAnaSpectrumCfg->iUpperLimitPosition[jj]); // 20111209, (stpSysAnaSpectrumCfg->iLowerLimitPosition[jj] + stpSysAnaSpectrumCfg->iUpperLimitPosition[jj])/2;
			stpGroupSineSweepInputCases[ii].uiAxis_CtrlCardId[jj] = stpSysAnaSpectrumCfg->uiAxis_CtrlCardId[jj];
			stpGroupSineSweepInputCases[ii].stSineSweepExciteOnce.stSineSweepOnceExciteFreq.dNumCycle = 10; //astSineSweepAxisCfg[jj].stSineSweepSysCfg.usNumCaseDec;

		}

		for(unsigned int jj=0; jj<usActualNumAxis; jj++)
		{
			if(stpSysAnaSpectrumCfg->uiAxis_CtrlCardId[jj] == stpGroupSineSweepInputCases[ii].stSineSweepExciteOnce.uiExcitingAxis)
			{
				stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.dAmpSineSweepRatio = astSineSweepAxisCfg[jj].dAmpSineSweepRatio;
				stpGroupSineSweepInputCases[ii].stSineSweepExciteOnce.stSineSweepOnceExciteFreq.fAmplitude = stpSysAnaSpectrumCfg->usAmplitudeSineSweep16bDAC[jj];
				stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.stSineSweepSysCfg.dLowFreq_Hz = astSineSweepAxisCfg[jj].stSineSweepSysCfg.dLowFreq_Hz;
				stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.stSineSweepSysCfg.dUppFreq_Hz = astSineSweepAxisCfg[jj].stSineSweepSysCfg.dUppFreq_Hz;
				stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.stSineSweepSysCfg.usNumCaseDec = astSineSweepAxisCfg[jj].stSineSweepSysCfg.usNumCaseDec;
	
				stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.uiTotalCaseFreq = astSineSweepAxisCfg[jj].uiTotalCaseFreq;
				//for(int kk=0; kk<astSineSweepAxisCfg[jj].uiTotalCaseFreq; kk++)
				//{
					stpGroupSineSweepInputCases[ii].stSinSweepAxisCfg.pdFreqArray = &astSineSweepAxisCfg[jj].pdFreqArray[0];
				//}
			}
		}
	}

	return usActualTotalExciteCase;
}
// IDC_BUTTON_START_GROUP_SINE_SWEEP
void CMtnSpectrumTestDlg::OnBnClickedButtonStartGroupSineSweep()
{
// calculate
	aft_spectrum_calc_freq_array_from_cfg();
	spectrum_dlg_consolidate_group_sys_ana_config();

	// Reset counter
	nCurrCaseIdxGroupSpecTest = 0;
	nTotalNumCasesGroupSpecTest = aft_spectrum_build_sinesweep_from_sys_ana_cfg(&stSpectrumSysAna_GroupTestConfig, &astSineSweepAxisCfg[0]);

	if(nTotalNumCasesGroupSpecTest > 0)
	{
		aft_set_spectrum_sys_analysis_flag(SYSTEM_ANALYSIS_PSEUDO_SINE_SWEEP); // = TRUE;
	// Start the thread
		GetDlgItem(IDC_BUTTON_START_GROUP_SINE_SWEEP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_START_GROUP_PRBS)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);

		acs_run_buffer_prog_prbs_prof_cfg_move();
		acs_run_buffer_prog_vel_loop_prof_cfg_move2();
		RunSpectrumTestThread();
	}
	// 
}

#include "process.h"
// IDC_BUTTON_PLOT_SPECTRUM_TEST
void CMtnSpectrumTestDlg::OnBnClickedButtonPlotSpectrumTest()
{
	int iErrNo; 
	_flushall();

//	iErrNo = execv("test_ana_prbs_rest.exe", NULL);

 	iErrNo = system("test_ana_prbs_rest.exe");
	switch (iErrNo)
	{
	case ENOENT:
		AfxMessageBox("ENOENT");
		break;
	case E2BIG:
		AfxMessageBox("E2BIG");
		break;
	case ENOEXEC:
		AfxMessageBox("ENOEXEC");
		break;
	case ENOMEM:
		AfxMessageBox("ENOMEM");
		break;
	case EPERM:
		AfxMessageBox("EPERM");
		break;
	default :
		break;
	}
	aft_spectrum_set_debug_flag(3);
//	spawnve(_P_NOWAIT, "test_ana_prbs_rest.exe", NULL, NULL);  // _P_OVERLAY
}

// IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgGotoVelStepAfter()
{
	iFlagGotoVelStepAfterSpectrumTest = ((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_GOTO_VEL_STEP_AFTER)) ->GetCheck();
}

// IDC_CHECK_SPECTRUM_DLG_TUNE_VEL_GAIN
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgTuneVelGain()
{
	iFlagTuningVelLoopGain = ((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_TUNE_VEL_GAIN)) ->GetCheck();

	UI_EnableEditTuningMaxParameter(aft_spectrum_get_debug_flag());
}
// IDC_CHECK_SPECTRUM_DLG_TUNE_DRV
void CMtnSpectrumTestDlg::OnBnClickedCheckSpectrumDlgTuneDrv()
{
	iFlagTuningDriver = ((CButton *) GetDlgItem(IDC_CHECK_SPECTRUM_DLG_TUNE_DRV)) ->GetCheck();
}

// IDC_EDIT_MAX_VKP_1_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp1SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_VKP_1_SPECTRUM_DLG, &aiTuneMaxVKP[0]);
}
// IDC_EDIT_MAX_VKP_2_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp2SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_VKP_2_SPECTRUM_DLG, &aiTuneMaxVKP[1]);
}
// IDC_EDIT_MAX_VKP_3_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp3SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_VKP_3_SPECTRUM_DLG, &aiTuneMaxVKP[2]);
}
// IDC_EDIT_MAX_VKP_4_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxVkp4SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_VKP_4_SPECTRUM_DLG, &aiTuneMaxVKP[3]);
}
// IDC_EDIT_MAX_SOF_1_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof1SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_SOF_1_SPECTRUM_DLG, &aiTuneMaxSOF[0]);
}
// IDC_EDIT_MAX_SOF_2_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof2SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_SOF_2_SPECTRUM_DLG, &aiTuneMaxSOF[1]);
}
// IDC_EDIT_MAX_SOF_3_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof3SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_SOF_3_SPECTRUM_DLG, &aiTuneMaxSOF[2]);
}
// IDC_EDIT_MAX_SOF_4_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxSof4SpectrumDlg()
{
	ReadIntegerFromEdit(IDC_EDIT_MAX_SOF_4_SPECTRUM_DLG, &aiTuneMaxSOF[3]);
}

// IDC_EDIT_MAX_CKP_1_SPECTRUM_DLG2
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp1SpectrumDlg2()
{
}
//// IDC_EDIT_MAX_CKP_2_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp2SpectrumDlg()
//{
//}
//// IDC_EDIT_MAX_CKP_3_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp3SpectrumDlg()
//{
//}
//// IDC_EDIT_MAX_CKP_4_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCkp4SpectrumDlg()
//{
//}
// IDC_EDIT_MAX_CKI_1_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki1SpectrumDlg()
{
}
//// IDC_EDIT_MAX_CKI_2_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki2SpectrumDlg()
//{
//}
//// IDC_EDIT_MAX_CKI_3_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki3SpectrumDlg()
//{
//}
//// IDC_EDIT_MAX_CKI_4_SPECTRUM_DLG
//void CMtnSpectrumTestDlg::OnEnKillfocusEditMaxCki4SpectrumDlg()
//{
//}

// IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG
void CMtnSpectrumTestDlg::OnCbnSelchangeComboPathScanningVelLoopSpectrumDlg()
{
	cFlagBwVelLoopScanningList = ((CComboBox *) GetDlgItem(IDC_COMBO_PATH_SCANNING_VEL_LOOP_SPECTRUM_DLG))->GetCurSel();
}

// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS
void CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverAxis()
{
	cFlagBwCheckDriverAxis = ((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_AXIS))->GetCurSel();
}
// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT
void CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverComport()
{
	cFlagBwCheckDriverComPort = ((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_COMPORT))->GetCurSel();
}
// IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE
void CMtnSpectrumTestDlg::OnCbnSelchangeComboSpectrumDlgTuneDriverType()
{
	cFlagBwCheckDriverType = ((CComboBox *) GetDlgItem(IDC_COMBO_SPECTRUM_DLG_TUNE_DRIVER_TYPE))->GetCurSel();
}
	
