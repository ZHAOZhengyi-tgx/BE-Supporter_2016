// MtnPosnCompensDlg.cpp : implementation file
// YYYYMMDD Author Notes
// 20081119 Zhengyi strange error
// 20090908 Replace Sleep to be high_precision_sleep_ms

#include "stdafx.h"
#include "MtnPosnCompensDlg.h"
#include "MtnApi.h"
#include "WinTiming.h"
#include "MtnInitAcs.h"

#include "acs_buff_prog.h"

// Define for communication with buffer program in controller
#include "ForceCali.h"

static TEACH_CONTACT_INPUT stTeachContactParameter;
static TEACH_CONTACT_OUTPUT stTeachContactResult;
static SEARCH_CONTACT_AND_FORCE_CONTROL_INPUT stSearchContactAndForceControl;
static FORCE_BLOCK_ACS stForceBlock;
static FORCE_CALIBRATION_INPUT stForceCalibrationInput;
static FORCE_CALIBRATION_OUTPUT stForceCalibrationOutput;
static char strDebugText[128];

static FORCE_VERIFY_INPUT stForceVerifyInput;
static FORCE_VERIFY_OUTPUT stForceVerifyOutput;

int giContactPositionReg;
//static FORCE_BLOCK_ACS stBakForceBlock; // 20081119
static int iContactTuneFlagB1W = 0;

#define FORCE_CONSTANT_GRAM_FULL_ADC          220
#define ACS_ADC_FULL_14BIT                    8192
static double fForceConstantGramPerAdcRead = FORCE_CONSTANT_GRAM_FULL_ADC/ACS_ADC_FULL_14BIT;

int iSearchHeight_um = 250; // 20121211

// 20121030
#define SP_DETECTION_NONE      0
#define SP_DETECTION_BY_DOUT   1
#define SP_DETECTION_BY_PE     2
#define SP_DETECTION_BY_AIN    3

static char cFlagContactDetectionSwitch = SP_DETECTION_BY_PE;
extern char *pstrDetectionSwitch[];
static char cFlagSearchSpd = 3;
extern char *pstrSearchVelSelection[];
static char cFlagSearchTolPE = 1;
extern char *pstrSearchTH_PE[];
static char cFlagSwitchDamp = 3;
extern char *pstrSwitchDamp[];
static char cFlagPreImpactForce = 3;
extern char *pstrPreImpactForce[];

#define DEF_FORCE_BLK 0

static int iFlagInitOnce;
static unsigned int uiCurrEditSeg;
//static 	unsigned int uiLoopContactForceTest;
static char *strStopString; // used in strtod

#define DEF_POSN_COMPENSATE_DAC_FACTOR    (0.045666)
#define DEF_POSN_COMPENSATE_DAC_OFFSET    (105.825112)

extern MTN_TUNE_POSN_COMPENSATION stPosnCompensationTune;
static MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;

// CMtnPosnCompensDlg dialog
static char strTemp[64];


#define THREAD_TEACH_LIMIT                         0
#define THREAD_SEARCH_CONTACT_WITH_FORCE_CONTROL   1
#define THREAD_FORCE_CALIBRATION                   2
#define THREAD_FORCE_VERIFICATION                  3
#define THREAD_FORCE_LOG_VERIFICATION              4
static char cFlagThreadInPosnForceCaliDlg = 0;

#define __DEF_IDX_COMBO_LOOP_CHECK__ 3
static short nIdxComboCheckPositionForce = __DEF_IDX_COMBO_LOOP_CHECK__; // 20 times by default
char *pstrComboCheckPositionForce[] = 
{
	"1",
	"2",
	"5",
	"10",
	"20",
	"50",
	"100",
	"200",
	"500"
};
static int aCountLoopCheckPositionForce[] = {1, 2, 5, 10, 20, 50, 100, 200, 500};
static unsigned int nCountLoopCheckPositionForce = aCountLoopCheckPositionForce[__DEF_IDX_COMBO_LOOP_CHECK__];
static unsigned int uiLoopContactForceTest = 10;

IMPLEMENT_DYNAMIC(CMtnPosnCompensDlg, CDialog)

CMtnPosnCompensDlg::CMtnPosnCompensDlg(HANDLE hInHandle, CWnd* pParent /*=NULL*/)
	: CDialog(CMtnPosnCompensDlg::IDD, pParent)
{
	m_hHandle = hInHandle;
	if( iFlagInitOnce == 0)
	{
		iFlagInitOnce = 1;
		stForceBlock.uiNumSegment = 4;
		stForceBlock.aiForceBlk_Rampcount[0] = 20;
		stForceBlock.aiForceBlk_LevelCount[0] = 50;
		stForceBlock.adForceBlk_LevelAmplitude[0] = -1.5;

		stForceBlock.aiForceBlk_Rampcount[1] = 20;
		stForceBlock.aiForceBlk_LevelCount[1] = 200;
		stForceBlock.adForceBlk_LevelAmplitude[1] = -3.0;

		stForceBlock.aiForceBlk_Rampcount[2] = 20;
		stForceBlock.aiForceBlk_LevelCount[2] = 200;
		stForceBlock.adForceBlk_LevelAmplitude[2] = -4.5;

		stForceBlock.aiForceBlk_Rampcount[3] = 20;
		stForceBlock.aiForceBlk_LevelCount[3] = 50;
		stForceBlock.adForceBlk_LevelAmplitude[3] = -1.5;
		stForceBlock.iInitForceHold_cnt = 5;

		uiCurrEditSeg = 0;

		stOutputPosnCompensationTune.dPositionFactor = DEF_POSN_COMPENSATE_DAC_FACTOR;
		stOutputPosnCompensationTune.dCtrlOutOffset = DEF_POSN_COMPENSATE_DAC_OFFSET;

		stForceCalibrationOutput.fForceFactor_Kf = DEF_FORCE_FACTOR_KF;
		stForceCalibrationOutput.fForceOffset_I0 = DEF_FORCE_OFFSET_I0;
		stForceCalibrationOutput.fPosnFactor_Ka = DEF_FORCE_POSN_FACTOR_KA;

		stForceCalibrationInput.stForceCaliPara.dDefForceFactor_Kf = DEF_FORCE_FACTOR_KF;
		stForceCalibrationInput.stForceCaliPara.dDefForceOffset_I0 = DEF_FORCE_OFFSET_I0;
		stForceCalibrationInput.stForceCaliPara.dFirstForce_gram = 30;
		stForceCalibrationInput.stForceCaliPara.dLastForce_gram = 200;
		stForceCalibrationInput.stForceCaliPara.uiNumSegment = 10;

		stForceCalibrationInput.stForceCaliPara.dStepDCOM = 0.4; // 20150124

		stForceVerifyInput.dForceVerifyDesiredGram = 50;
//		uiLoopContactForceTest = 1;
//stPosnCompensationTune ={10, -1000, -4000, 0, 0,};

	}
}

int iPosnForceCalibrateDlgFlagShow;
int CMtnPosnCompensDlg::UI_GetShowWindowFlag()
{
	return iPosnForceCalibrateDlgFlagShow;
}
CMtnPosnCompensDlg::~CMtnPosnCompensDlg()
{
}

void CMtnPosnCompensDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMtnPosnCompensDlg, CDialog)
	ON_BN_CLICKED(IDC_POSN_COMPENSATE_START_TUNE, &CMtnPosnCompensDlg::OnBnClickedPosnCompensateStartTune)
	ON_BN_CLICKED(IDC_POSNCOMPEN_DEBUG_FLAG, &CMtnPosnCompensDlg::OnBnClickedPosncompenDebugFlag)
	ON_BN_CLICKED(IDC_POSN_COMPENSATE_START_VERIFY, &CMtnPosnCompensDlg::OnBnClickedPosnCompensateStartVerify)
	ON_BN_CLICKED(IDC_BUTTON_TEACH_CONTACT, &CMtnPosnCompensDlg::OnBnClickedButtonTeachContact)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT, &CMtnPosnCompensDlg::OnEnChangeEditForceControlMaxSegment)
	ON_CBN_SELCHANGE(IDC_COMBO_FORCE_CONTROL_EDIT_SEGMENT, &CMtnPosnCompensDlg::OnCbnSelchangeComboForceControlEditSegment)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceControlRampCountCurrSeg)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceControlLevelCntCurrSeg)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceControlLevelAmpCurrSeg)
	//	ON_BN_CLICKED(IDC_BUTTON2, &CMtnPosnCompensDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_CONTACT_WITH_FORCE_CONTROL, &CMtnPosnCompensDlg::OnBnClickedButtonSearchContactWithForceControl)
//	ON_EN_CHANGE(IDC_EDIT_CONTACT_FORCE_LOOP_TEST, &CMtnPosnCompensDlg::OnEnChangeEditContactForceLoopTest)
	ON_EN_CHANGE(IDC_FORCE_CONTROL_HOLD_COUNT_INIT_FORCE, &CMtnPosnCompensDlg::OnEnChangeForceControlHoldCountInitForce)
	ON_BN_CLICKED(IDC_BUTTON_FORCE_CALIBRATION, &CMtnPosnCompensDlg::OnBnClickedButtonForceCalibration)
	ON_BN_CLICKED(IDC_BUTTON_FORCE_VERIFICATION, &CMtnPosnCompensDlg::OnBnClickedButtonForceVerification)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CALI_TOTAL_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceCaliTotalSeg)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceCaliGramFirstSeg)
	ON_EN_CHANGE(IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG, &CMtnPosnCompensDlg::OnEnChangeEditForceCaliGramLastSeg)
	ON_EN_CHANGE(IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM, &CMtnPosnCompensDlg::OnEnChangeEditForceVerifyDesiredGram)
	ON_EN_CHANGE(IDC_POSNCOMPN_UPPER_LIMIT, &CMtnPosnCompensDlg::OnEnChangePosncompnUpperLimit)
	ON_EN_CHANGE(IDC_POSNCOMPN_TOTAL_POINT, &CMtnPosnCompensDlg::OnEnChangePosncompnTotalPoint)
	ON_EN_CHANGE(IDC_POSNCOMPN_LOWER_LIMIT, &CMtnPosnCompensDlg::OnEnChangePosncompnLowerLimit)
	ON_BN_CLICKED(IDC_BUTTON_VERIFICATION_LOG, &CMtnPosnCompensDlg::OnBnClickedButtonVerificationLog)
	ON_BN_CLICKED(ID_OK_DIALOG_POSITION_COMPENSATION, &CMtnPosnCompensDlg::OnBnClickedOkDialogPositionCompensation)
	ON_BN_CLICKED(IDC_CHECK_CONTACT_TUNE_FLAG_B1W, &CMtnPosnCompensDlg::OnBnClickedCheckContactTuneFlagB1w)
	ON_CBN_SELCHANGE(IDC_COMBO_POSN_FORCE_COMPENSATION_DLG_FORCE_CONTROL_DETECTION_FLAG, &CMtnPosnCompensDlg::OnCbnSelchangeComboPosnForceCompensationDlgForceControlDetectionFlag)
	ON_EN_KILLFOCUS(IDC_POSNCOMPN_UPPER_LIMIT, &CMtnPosnCompensDlg::OnEnKillfocusPosncompnUpperLimit)
	ON_EN_KILLFOCUS(IDC_POSNCOMPN_LOWER_LIMIT, &CMtnPosnCompensDlg::OnEnKillfocusPosncompnLowerLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_START_DIST, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactStartDist)
	ON_EN_KILLFOCUS(IDC_POSNCOMPN_TOTAL_POINT, &CMtnPosnCompensDlg::OnEnKillfocusPosncompnTotalPoint)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactStartVel)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_SEARCH_VEL, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactSearchVel)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_END_POSN, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactEndPosn)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_MAX_DIST, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactMaxDist)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactAntiBounce)
	ON_EN_KILLFOCUS(IDC_EDIT_TEACH_CONTACT_TIME_OUT, &CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactTimeOut)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT, &CMtnPosnCompensDlg::OnEnKillfocusEditForceControlMaxSegment)
//	ON_EN_KILLFOCUS(IDC_EDIT_CONTACT_FORCE_LOOP_TEST, &CMtnPosnCompensDlg::OnEnKillfocusEditContactForceLoopTest)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceControlRampCountCurrSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceControlLevelCntCurrSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceControlLevelAmpCurrSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CALI_TOTAL_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliTotalSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliGramFirstSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG, &CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliGramLastSeg)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM, &CMtnPosnCompensDlg::OnEnKillfocusEditForceVerifyDesiredGram)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_VEL_POSN_FORCE_CALIBRATION_DLG, &CMtnPosnCompensDlg::OnCbnSelchangeComboSearchVelPosnForceCalibrationDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_TOL_PE_POSN_FORCE_CALIBRATION_DLG, &CMtnPosnCompensDlg::OnCbnSelchangeComboSearchTolPePosnForceCalibrationDlg)
	ON_CBN_KILLFOCUS(IDC_COMBO_DAMP_POSN_FORCE_CALIBRATION_DLG, &CMtnPosnCompensDlg::OnCbnKillfocusComboDampPosnForceCalibrationDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PRE_IMP_FORCE_POSITION_FORCE_CALI_DLG, &CMtnPosnCompensDlg::OnCbnSelchangeComboPreImpForcePositionForceCaliDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG, &CMtnPosnCompensDlg::OnEnKillfocusEditMaxJerkMoveSrchHtPosnForceCaliDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_LOOP_COUNT_CHECK_FORCE_POSITION_CALIBRATION_DLG, &CMtnPosnCompensDlg::OnCbnSelchangeComboLoopCountCheckForcePositionCalibrationDlg)
	ON_BN_CLICKED(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG, &CMtnPosnCompensDlg::OnBnClickedButtonCalibrateForceRatioPosnForceCalibrateDlg)
	ON_EN_SETFOCUS(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG, &CMtnPosnCompensDlg::OnEnSetfocusEditForceReadByGramGaugePosnForceDlg)
	ON_EN_KILLFOCUS(IDC_FORCE_VERIFY_TRIALS_PER_LOOP, &CMtnPosnCompensDlg::OnEnKillfocusForceVerifyTrialsPerLoop)
	ON_BN_CLICKED(IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG, &CMtnPosnCompensDlg::OnBnClickedCheckForceVerifyByGramJig)
	ON_EN_KILLFOCUS(IDC_EDIT_FORCE_CALI_DELTA_DCOM, &CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliDeltaDcom)
END_MESSAGE_MAP()

extern short acs_init_buff_prog_search_contact_force_control();
extern short acs_clear_buffer_prog_search_contact_force_control();

#include "MotAlgo_DLL.h"
#include "MtnWbDef.h"

char cFlagForceVerifyByGramGauge = 0; // 20130107
char cFlagDoingForceVerifyByGramGauge = 0;

extern void mtn_acs_convert_from_current_to_position_loop(HANDLE m_hHandle, int iAxisACS);

void CMtnPosnCompensDlg::UI_InitPositionForceCalibrationDlg()
{
	int ii;
	cFlagContactDetectionSwitch = (char)stTeachContactParameter.cFlagDetectionSP;
	for(ii=0; ii<4; ii++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_POSN_FORCE_COMPENSATION_DLG_FORCE_CONTROL_DETECTION_FLAG))->InsertString(ii, pstrDetectionSwitch[ii]);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_POSN_FORCE_COMPENSATION_DLG_FORCE_CONTROL_DETECTION_FLAG))->SetCurSel(cFlagContactDetectionSwitch);

	for(ii=0; ii<60; ii++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_VEL_POSN_FORCE_CALIBRATION_DLG))->InsertString(ii, pstrSearchVelSelection[ii]);
	}
	cFlagSearchSpd = (char)((abs(stTeachContactParameter.iSearchVel)/10000.0 - 0.1)/0.1);  // 20130205
	((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_VEL_POSN_FORCE_CALIBRATION_DLG))->SetCurSel(cFlagSearchSpd);

	for(ii=0; ii<12; ii++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_TOL_PE_POSN_FORCE_CALIBRATION_DLG))->InsertString(ii, pstrSearchTH_PE[ii]);
	}
	cFlagSearchTolPE = (char)((stTeachContactParameter.iSearchTolPE - 6)/6.0);
	((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_TOL_PE_POSN_FORCE_CALIBRATION_DLG))->SetCurSel(cFlagSearchTolPE);

	for(ii=0; ii<8; ii++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_LOOP_COUNT_CHECK_FORCE_POSITION_CALIBRATION_DLG))->InsertString(ii, pstrComboCheckPositionForce[ii]);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_LOOP_COUNT_CHECK_FORCE_POSITION_CALIBRATION_DLG))->SetCurSel(nIdxComboCheckPositionForce);

	int iDampSP;
	//mtnapi_upload_acs_sp_parameter_damp_switch(m_hHandle, &iDampSP);
	//cFlagSwitchDamp = iDampSP/5;
	for(ii=0; ii<13; ii++) // 
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_DAMP_POSN_FORCE_CALIBRATION_DLG))->InsertString(ii, pstrSwitchDamp[ii]);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_DAMP_POSN_FORCE_CALIBRATION_DLG))->SetCurSel(cFlagSwitchDamp);
	iDampSP = cFlagSwitchDamp * 5 + 5;
	mtnapi_download_acs_sp_parameter_damp_switch(m_hHandle, iDampSP); // 20121220

	for(ii=0; ii<11; ii++)
	{
		((CComboBox*)GetDlgItem(IDC_COMBO_PRE_IMP_FORCE_POSITION_FORCE_CALI_DLG))->InsertString(ii, pstrPreImpactForce[ii]);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO_PRE_IMP_FORCE_POSITION_FORCE_CALI_DLG))->SetCurSel(cFlagPreImpactForce);

CString cstrTemp;
//char strTextTemp[32];

	cstrTemp.Format("%d", stPosnCompensationTune.uiTotalPoints);
	GetDlgItem(IDC_POSNCOMPN_TOTAL_POINT)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.2f", stPosnCompensationTune.dUpperPosnLimit);
	GetDlgItem(IDC_POSNCOMPN_UPPER_LIMIT)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.2f", stPosnCompensationTune.dLowerPosnLimit);
	GetDlgItem(IDC_POSNCOMPN_LOWER_LIMIT)->SetWindowTextA(cstrTemp);
	
	cstrTemp.Format("%8.2f", stPosnCompensationTune.dPositionFactor);
	GetDlgItem(IDC_POSNCOMPN_POSN_FACTOR_LABEL)->SetWindowTextA(cstrTemp);
	
	cstrTemp.Format("%8.2f", stPosnCompensationTune.dCtrlOutOffset);
	GetDlgItem(IDC_POSNCOMPN_OFFSET_LABEL)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dPositionFactor);
	GetDlgItem(IDC_POSNCOMPN_POSN_FACTOR_LABEL)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dCtrlOutOffset);
	GetDlgItem(IDC_POSNCOMPN_OFFSET_LABEL)->SetWindowTextA(cstrTemp);

	((CButton *)GetDlgItem(IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG))->SetCheck(cFlagForceVerifyByGramGauge); // 20130107

	cstrTemp.Format("%4.2f", stForceCalibrationInput.stForceCaliPara.dStepDCOM);
	GetDlgItem(IDC_EDIT_FORCE_CALI_DELTA_DCOM)->SetWindowTextA(cstrTemp);

}
// CMtnPosnCompensDlg message handlers
BOOL CMtnPosnCompensDlg::OnInitDialog()
{
	CString cstrTemp;
	char strTextTemp[32];
//	int ii;

	mtn_tune_init_position_compensation_condition();

	InitForceFbAdc();

	acsc_RunBuffer(m_hHandle, BUFFER_ID_SEARCH_CONTACT, NULL, NULL);
	Sleep(20); // Wait for buffer program end initialization
	acsc_upload_search_contact_parameter(m_hHandle, &stTeachContactParameter);


	// cFlagDetectionSP = cFlagContactDetectionSwitch;
	UI_InitPositionForceCalibrationDlg();

	cstrTemp.Format("Teach Ctct Axis:%d", stTeachContactParameter.iAxis);
	GetDlgItem(IDC_BUTTON_TEACH_CONTACT)->SetWindowTextA(cstrTemp);
	
	cstrTemp.Format("%d", stForceBlock.iInitForceHold_cnt);
	GetDlgItem(IDC_FORCE_CONTROL_HOLD_COUNT_INIT_FORCE)->SetWindowTextA(cstrTemp);

	// IDC_STATIC_CURR_FORCE_CALI
	cstrTemp.Format("Ka: %10.7f, Kf: %8.5f, I0: %8.5f", 
		stForceCalibrationOutput.fPosnFactor_Ka, stForceCalibrationOutput.fForceFactor_Kf, stForceCalibrationOutput.fForceOffset_I0);
	GetDlgItem(IDC_STATIC_CURR_FORCE_CALI)->SetWindowTextA(cstrTemp);

	// Force Calibration input
	cstrTemp.Format("%d", stForceCalibrationInput.stForceCaliPara.uiNumSegment);
	GetDlgItem(IDC_EDIT_FORCE_CALI_TOTAL_SEG)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.2f", stForceCalibrationInput.stForceCaliPara.dFirstForce_gram);
	GetDlgItem(IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.2f", stForceCalibrationInput.stForceCaliPara.dLastForce_gram);
	GetDlgItem(IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG)->SetWindowTextA(cstrTemp);

	// Force Verification input	
	cstrTemp.Format("%8.2f", stForceVerifyInput.dForceVerifyDesiredGram);
	GetDlgItem(IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM)->SetWindowTextA(cstrTemp);

	UpdateDataFromVarToUI();

	// Force Control UI
	sprintf_s(strTextTemp, 32, "%d", stForceBlock.uiNumSegment);
	// Very strange command, it will change value in stForceBlock
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT)->SetWindowTextA(_T(strTextTemp));
//	stForceBlock = stBakForceBlock; 		// 20081119

	CComboBox *pSelectAxisForAxis1 = (CComboBox*) GetDlgItem(IDC_COMBO_FORCE_CONTROL_EDIT_SEGMENT);
	for(unsigned int ii=0; ii< stForceBlock.uiNumSegment; ii++)
	{
		sprintf_s(strTextTemp, 32, "%d", ii);
		pSelectAxisForAxis1->AddString(strTextTemp);
	}
	pSelectAxisForAxis1->SetCurSel(uiCurrEditSeg);
	UpdateEditCurrSegment(uiCurrEditSeg);

	m_pWinThreadPosnForceCali = NULL;
	mFlagStopPosnForceCaliThread = TRUE;

	return CDialog::OnInitDialog();
}
UINT PosnForceCaliThreadProc(LPVOID pParam )
{
    CMtnPosnCompensDlg* pObject = (CMtnPosnCompensDlg *)pParam;

	AfxMessageBox("Remember: \n --- Adjust Table s.t. BondHead on Force Sensor \n --- Adjust sensor-amplifier s.t. ForceFbAdc [-7000, -3000]");

	if(cFlagThreadInPosnForceCaliDlg == THREAD_SEARCH_CONTACT_WITH_FORCE_CONTROL)
	{
		pObject->ThreadDoSearchContactWithForceControl();
	}
	else if(cFlagThreadInPosnForceCaliDlg == THREAD_FORCE_CALIBRATION)
	{
		pObject->ThreadDoForceCalibration();
	}
	else if(cFlagThreadInPosnForceCaliDlg == THREAD_FORCE_VERIFICATION)
	{
		if(cFlagForceVerifyByGramGauge == 0)
		{
			pObject->ThreadDoForceVerification();
		}
		else
		{
			pObject->ThreadDoForceVerificationByGramJig();
		}
	}
	else if(cFlagThreadInPosnForceCaliDlg == THREAD_TEACH_LIMIT)
	{
	}
	else if(cFlagThreadInPosnForceCaliDlg ==  THREAD_FORCE_LOG_VERIFICATION) // 20130103
	{
		pObject->Thread_ForceLogVerification(); // 20130103
	}

	pObject->UI_SetEnableButton(TRUE);
	pObject->mFlagStopPosnForceCaliThread = TRUE;
	return MTN_API_OK_ZERO;
}

void CMtnPosnCompensDlg::RunPosnForceCaliThread()
{
	mFlagStopPosnForceCaliThread = FALSE;

	UI_SetEnableButton(FALSE); // 20130102
	acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z(); // 20120203
	Sleep(200);  // 20120203

static int iFlagRunTeachContactOnce;
	if(iFlagRunTeachContactOnce ==0)
	{
		OnBnClickedButtonTeachContact();
		iFlagRunTeachContactOnce = 1;
	}

	m_pWinThreadPosnForceCali = AfxBeginThread(PosnForceCaliThreadProc, this);
	SetPriorityClass(m_pWinThreadPosnForceCali->m_hThread, REALTIME_PRIORITY_CLASS);

	m_pWinThreadPosnForceCali->m_bAutoDelete = FALSE;
}

void CMtnPosnCompensDlg::StopPosnForceCaliThread()
{
	if (m_pWinThreadPosnForceCali)
	{
		mFlagStopPosnForceCaliThread = TRUE;
		WaitForSingleObject(m_pWinThreadPosnForceCali->m_hThread, 1000);
		// delete m_pWinThreadPosnForceCali;
		m_pWinThreadPosnForceCali = NULL;
	}
}

void CMtnPosnCompensDlg::UpdateEditCurrSegment(unsigned int uiCurrSeg)
{
	CString cstrTemp;

	cstrTemp.Format("%d", stForceBlock.aiForceBlk_Rampcount[uiCurrSeg]);
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%d", stForceBlock.aiForceBlk_LevelCount[uiCurrSeg]);
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%4.2f", stForceBlock.adForceBlk_LevelAmplitude[uiCurrSeg]);
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG)->SetWindowTextA(cstrTemp);
//	stForceBlock = stBakForceBlock; // 20081119

	UpdateData(TRUE);

}
void CMtnPosnCompensDlg::UI_SetEnableButton(BOOL bFlagEnable)
{
	GetDlgItem(IDC_POSN_COMPENSATE_START_TUNE)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_POSN_COMPENSATE_START_VERIFY)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_TEACH_CONTACT)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_SEARCH_CONTACT_WITH_FORCE_CONTROL)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_FORCE_VERIFICATION)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_VERIFICATION_LOG)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_FORCE_CALIBRATION)->EnableWindow(bFlagEnable);
}

void CMtnPosnCompensDlg::UpdateDataFromVarToUI()
{
	CString cstrTemp;
	// IDC_EDIT_TEACH_CONTACT_START_DIST
	cstrTemp.Format("%d",  iSearchHeight_um); // stTeachContactParameter.iSearchHeightPosition);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_START_DIST)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT
	cstrTemp.Format("%d", stTeachContactParameter.iMaxAccMoveSrchHt);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG
	cstrTemp.Format("%d", stTeachContactParameter.iMaxJerkMoveSrchHt);
	GetDlgItem(IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_SEARCH_VEL
	cstrTemp.Format("%d", stTeachContactParameter.iSearchVel);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_VEL)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_END_POSN
	cstrTemp.Format("%d", stTeachContactParameter.iResetPosition);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_END_POSN)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_MAX_DIST
	cstrTemp.Format("%d", stTeachContactParameter.iMaxDist);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_DIST)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE
	cstrTemp.Format("%d", stTeachContactParameter.iAntiBounce);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE)->SetWindowTextA(cstrTemp);

	//IDC_EDIT_TEACH_CONTACT_TIME_OUT
	cstrTemp.Format("%d", stTeachContactParameter.iTimeOut);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_TIME_OUT)->SetWindowTextA(cstrTemp);

	// IDC_EDIT_TEACH_CONTACT_SEARCH_TOL_PE
	cstrTemp.Format("%d", stTeachContactParameter.iSearchTolPE);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_TOL_PE)->SetWindowTextA(cstrTemp);

	// 20130102
	cstrTemp.Format("%d", uiLoopContactForceTest);
	GetDlgItem(IDC_FORCE_VERIFY_TRIALS_PER_LOOP)->SetWindowTextA(cstrTemp);

}

void CMtnPosnCompensDlg::UpdateDataFromUIToVar()
{
	char strTemp[64];

	// IDC_EDIT_TEACH_CONTACT_START_DIST
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_START_DIST)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &iSearchHeight_um); // stTeachContactParameter.iSearchHeightPosition);

	// IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxAccMoveSrchHt);

	// IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG
	GetDlgItem(IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxJerkMoveSrchHt);

	// IDC_EDIT_TEACH_CONTACT_SEARCH_VEL
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_VEL)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iSearchVel);

	// IDC_EDIT_TEACH_CONTACT_END_POSN
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_END_POSN)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iResetPosition);

	// IDC_EDIT_TEACH_CONTACT_MAX_DIST
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_DIST)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxDist);

	// IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iAntiBounce);

	//IDC_EDIT_TEACH_CONTACT_TIME_OUT
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_TIME_OUT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iTimeOut);

	// IDC_EDIT_TEACH_CONTACT_SEARCH_TOL_PE
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_TOL_PE)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iSearchTolPE);

}

//#include "MotAlgo_DLL.h"
extern char *astrMachineTypeNameLabel_en[];

void CMtnPosnCompensDlg::OnBnClickedPosnCompensateStartTune()
{
//	cstrTemp.Format("%d", stPosnCompensationTune.uiTotalPoints);
//	CDialog::UpdateData(TRUE);
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);

	int iMechCfg = get_sys_machine_type_flag();

	sprintf_s(stPosnCompensationTune.strDataFileName, MTN_API_MAX_STRLEN_FILENAME, "%s-%d_DataPosnCompn_%d.%d.%d_H%dM%d.m",
		astrMachineTypeNameLabel_en[iMechCfg],
		get_sys_machine_serial_num(),
		stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);

	if(mtune_position_compensation(m_hHandle, sys_get_acs_axis_id_bnd_z(), &stPosnCompensationTune, &stOutputPosnCompensationTune) == MTN_API_OK_ZERO)
	{
		// If successful ,it will update some user-interface
		CString cstrTemp;
		cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dPositionFactor);
		GetDlgItem(IDC_POSNCOMPN_POSN_FACTOR_LABEL)->SetWindowTextA(cstrTemp);
		cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dCtrlOutOffset);
		GetDlgItem(IDC_POSNCOMPN_OFFSET_LABEL)->SetWindowTextA(cstrTemp);

	}
}

void CMtnPosnCompensDlg::OnBnClickedPosncompenDebugFlag()
{
	// TODO: Add your control notification handler code here
	CButton *pCheckBox = (CButton *)GetDlgItem(IDC_POSNCOMPEN_DEBUG_FLAG);
	
	if( pCheckBox->GetCheck())
	{
		stPosnCompensationTune.iDebug = 1;
	}
	else
	{
		stPosnCompensationTune.iDebug = 0;
	}
}

void CMtnPosnCompensDlg::OnBnClickedPosnCompensateStartVerify()
{
	// TODO: Add your control notification handler code here
//	char strTemp[64];

//	CDialog::UpdateData(TRUE);
#ifdef __NOT_UPDATE_BY_KILL_FOCUS__
	GetDlgItem(IDC_POSNCOMPN_TOTAL_POINT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stPosnCompensationTune.uiTotalPoints);

	GetDlgItem(IDC_POSNCOMPN_UPPER_LIMIT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dUpperPosnLimit);

	GetDlgItem(IDC_POSNCOMPN_LOWER_LIMIT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dLowerPosnLimit);
#endif

	stPosnCompensationTune.uiGetStartingDateLen = 200;
	stPosnCompensationTune.uiSleepInterAction_ms = 500;
	sprintf_s(stPosnCompensationTune.strDataFileName, "VerifyPosnComp.m");
	if(mtune_position_compensation(m_hHandle, ACSC_AXIS_A, &stPosnCompensationTune, &stOutputPosnCompensationTune) == MTN_API_OK_ZERO)
	{
		CString cstrTemp;
		cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dPositionFactor);
		GetDlgItem(IDC_POSNCOMPN_VERIFY_POSN_FACTOR_LABEL)->SetWindowTextA(cstrTemp);
		cstrTemp.Format("%8.4f", stOutputPosnCompensationTune.dCtrlOutOffset);
		GetDlgItem(IDC_POSNCOMPN_VERIFY_OFFSET_LABEL)->SetWindowTextA(cstrTemp);
	}
}

void mtn_tune_contact_save_data(HANDLE m_hHandle, MTN_SCOPE *pstSystemScope, int uiAxisAcs, char *strFilename, char *strCommentTitleInFirstLine);

#include "AcsServo.h"
// IDC_BUTTON_TEACH_CONTACT
void CMtnPosnCompensDlg::OnBnClickedButtonTeachContact()
{

	//_mtnapi_dll_init_wb_servo_parameter_acs(m_hHandle);

	//if(_mtnapi_dll_init_servo_speed_para_acs(m_hHandle) == MTN_API_OK_ZERO)
	//{
	//	mtn_wb_acs_download_servo_ctrl(m_hHandle);
	//}

	GetDlgItem(IDC_BUTTON_TEACH_CONTACT)->EnableWindow(FALSE);

	int iResetLevel;
	//MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
	//mtn_api_get_spring_compensation_sp_para(m_hHandle, &stOutputPosnCompensationTune);
	iResetLevel = (int)mtn_wb_init_bh_relax_position_from_sp(m_hHandle); // stOutputPosnCompensationTune.iEncoderOffsetSP;
	if(m_hHandle != ACSC_INVALID   )
	{
		if(theAcsServo.GetServoOperationMode() == ONLINE_MODE)
		{
			mtnapi_get_speed_profile(m_hHandle, stTeachContactParameter.iAxis, &(stTeachContactParameter.stSpeedProfileTeachContact), 0);

			UpdateDataFromUIToVar();
			stTeachContactParameter.iFlagSwitchToForceControl = 0;

			stTeachContactParameter.iResetPosition = iResetLevel;
			stTeachContactParameter.iSearchHeightPosition = stTeachContactParameter.iResetPosition - 2000;
			stTeachContactParameter.iSearchTolPE = 50;
			stTeachContactParameter.iSearchVel = -15000;
			stTeachContactParameter.iMaxDist = 11000;
			stTeachContactParameter.iMaxJerkMoveSrchHt = 1; // 10 Km/s^3
			stTeachContactParameter.iAntiBounce = 10;
//			stTeachContactParameter.cFlagDetectionSP = 0;
			acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();
			Sleep(200);
			mtn_teach_contact_acs(m_hHandle, &stTeachContactParameter, &stTeachContactResult);

		#define FILE_NAME_SEARCH_CONTACT  "SrchContact.m"

			gstSystemScope.uiNumData = 7;
			gstSystemScope.uiDataLen = 3000;
			gstSystemScope.dSamplePeriod_ms = 1;
			mtn_tune_contact_save_data(m_hHandle, &gstSystemScope, stTeachContactParameter.iAxis, 
				FILE_NAME_SEARCH_CONTACT, "%% ACSC Controller, Axis- %d: RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n aTeachContactData = [    ");
			mtnapi_set_speed_profile(m_hHandle, stTeachContactParameter.iAxis, &(stTeachContactParameter.stSpeedProfileTeachContact), 0);

			CString cstrTemp;
			// IDC_SHOW_SEARCH_CONTACT_RESULT
			if(stTeachContactResult.iStatus == 0)
			{
				giContactPositionReg = stTeachContactResult.iContactPosnReg;
				cstrTemp.Format("OK: %d", stTeachContactResult.iContactPosnReg);
			}
			else if(stTeachContactResult.iStatus == 1)
			{
				cstrTemp.Format("TIME OUT");
			}
			else if(stTeachContactResult.iStatus == 2)
			{
				cstrTemp.Format("DISTANCE OUT");
			}
			else if(stTeachContactResult.iStatus == 3)
			{
				cstrTemp.Format("Motor Error");
			}
			GetDlgItem(IDC_SHOW_SEARCH_CONTACT_RESULT)->SetWindowTextA(cstrTemp);
		//	AfxMessageBox(cstrTemp);


			mtn_wb_tune_b1w_stop_srch_init_cfg(m_hHandle);
			mtn_wb_tune_b1w_stop_srch_download_cfg();
		}
		else if(theAcsServo.GetServoOperationMode()  == OFFLINE_MODE)
		{	
			iResetLevel = 5000;
			stTeachContactParameter.iResetPosition = iResetLevel;
			stTeachContactResult.iContactPosnReg = 0;
			if (!acsc_WriteInteger(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "CONTACT_POSN_REG", 0, 0, ACSC_NONE, ACSC_NONE, &stTeachContactResult.iContactPosnReg, NULL ))
			{
					// iRet = MTN_API_ERROR; 
			}
			mtnapi_enable_motor(m_hHandle, stTeachContactParameter.iAxis, NULL);
			Sleep(200);
			mtn_wb_tune_b1w_stop_srch_init_cfg(m_hHandle);
			mtn_wb_tune_b1w_stop_srch_download_cfg();
			Sleep(100);

		}


		if(iContactTuneFlagB1W == 1 &&
			mtn_qc_is_axis_locked_safe(m_hHandle, ACSC_AXIS_A) == TRUE )
		{

static CTRL_PARA_ACS stBakupServoControlPara;  // 20110720
			mtnapi_upload_servo_parameter_acs_per_axis(m_hHandle, ACSC_AXIS_A, &stBakupServoControlPara);

			mtn_wb_dll_download_acs_servo_speed_parameter_acs(m_hHandle);
			// Enable Axis
			mtnapi_enable_motor(m_hHandle, stTeachContactParameter.iAxis, NULL);
			// download speed profile
			
			mtnapi_axis_acs_move_to(m_hHandle, ACSC_AXIS_A, iResetLevel);
#ifdef   __PREV_
			while(qc_is_axis_still_acc_dec(m_hHandle, ACSC_AXIS_A))
			{
				Sleep(100); 	//Sleep(2);
			}

			Sleep(100);
			mtn_run_srch_contact_b1w(m_hHandle);
			while(qc_is_axis_still_acc_dec(m_hHandle, ACSC_AXIS_A))
			{
				Sleep(100); 	//Sleep(2);
			}
#else
			mtn_wb_tune_b1w_stop_srch_trig_once(m_hHandle);
#endif

			while(qc_is_axis_still_acc_dec(m_hHandle, ACSC_AXIS_A))
			{
				Sleep(100); 	//Sleep(2);
			}
			mtnapi_download_servo_parameter_acs_per_axis(m_hHandle, ACSC_AXIS_A, &stBakupServoControlPara);  // Restore to bakup
WB_TUNE_B1W_BH_OBJ stWbTuneB1wObj;
			mtn_wb_tune_b1w_stop_srch_calc_bh_obj(&gstSystemScope, &gdScopeCollectData[0], &stWbTuneB1wObj);

			CString cstrTemp;
			cstrTemp.Format("0: %4.1f, 1: %4.1f; 2: %4.1f; 6:%4.1f; 7:%4.1f ",
				stWbTuneB1wObj.dObj[0], stWbTuneB1wObj.dObj[1], stWbTuneB1wObj.dObj[2],
				stWbTuneB1wObj.dObj[6], stWbTuneB1wObj.dObj[7]);

			GetDlgItem(IDC_STATIC_POSN_TUNE_DLG_B1W_OBJ)->SetWindowTextA(cstrTemp);

			gstSystemScope.uiNumData = 7;
			gstSystemScope.uiDataLen = 3000;
			gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();
			#define FILE_NAME_TUNE_B1W_STOP_SRCH  "TuneB1W_StopSrch.m"
			mtn_tune_contact_save_data(m_hHandle, &gstSystemScope, stTeachContactParameter.iAxis, FILE_NAME_TUNE_B1W_STOP_SRCH,
					"%%%%  RPOS(AXIS), PE(AXIS), RVEL(AXIS), FVEL(AXIS), DCOM(AXIS), DOUT(AXIS), SLAFF(AXIS) \n ");

		}
//	mtnapi_dll_init_master_struct_ptr();
//	mtnapi_init_wb_def_servo_control_para_acs(m_hHandle);
//		_mtnapi_dll_init_servo_speed_para_acs(m_hHandle);
	}
	else  // ACSC_INVALID, PC-simulation mode 
	{

	}

	GetDlgItem(IDC_BUTTON_TEACH_CONTACT)->EnableWindow(TRUE);
}

static char strTextTemp[32];
// IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT
void CMtnPosnCompensDlg::OnEnChangeEditForceControlMaxSegment()
{
}
#include <math.h>
#include "direct.h"
void CMtnPosnCompensDlg::ThreadDoSearchContactWithForceControl()
{
	CString cstrTemp;

	// Setup parameter
	UpdateDataFromUIToVar();
	stTeachContactParameter.iFlagSwitchToForceControl = 1;
	stTeachContactParameter.iSearchHeightPosition = stTeachContactResult.iContactPosnReg + iSearchHeight_um; // 20121211
	stTeachContactParameter.iMaxDist = abs(stTeachContactResult.iContactPosnReg) + 500;

	stSearchContactAndForceControl.stpTeachContactPara = &stTeachContactParameter;
	stSearchContactAndForceControl.stpForceBlkPara = &stForceBlock;

////////////////////////////////////////////////// Folder
	char strFolderName[127];
	struct tm stTime;
	struct tm *stpTime = &stTime;
	__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	int iMechCfg = get_sys_machine_type_flag();
	sprintf_s(strFolderName, 127, "%s_%d_ContactForce_Y%dM%dD%d_H%dm%d_Srv%2.1f_Ht%d_PETh%d", 
		astrMachineTypeNameLabel_en[iMechCfg],
		get_sys_machine_serial_num(),
		stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, 
		fabs(stTeachContactParameter.iSearchVel/-10000.0),
		(stTeachContactParameter.iSearchHeightPosition - stTeachContactResult.iContactPosnReg),
		stTeachContactParameter.iSearchTolPE); // 20121211
	_mkdir(strFolderName);

	int iDampGA;
////////////////////////////////////////////////// Loop
	for(unsigned int ii = 0; ii<=nCountLoopCheckPositionForce; ii++)  
	{
		stForceBlock.dPreImpForce_DCOM = -1.8; //
//			(stOutputPosnCompensationTune.dPositionFactor * giContactPositionReg 
//				+ stOutputPosnCompensationTune.dCtrlOutOffset)
//					/32767 * 100; // unit of DCOM is percentage of 32767, DOUT

		mtn_search_contract_and_force_control(m_hHandle, &stSearchContactAndForceControl, &stTeachContactResult);
		// Update the contact position real-time
		giContactPositionReg = stTeachContactResult.iContactPosnReg;
		//IDC_SHOW_INIT_FORCE_CMD_READ_BACK
		cstrTemp.Format("InitForce: %8.4f", stTeachContactResult.dInitForceCommandReadBack);
		GetDlgItem(IDC_SHOW_INIT_FORCE_CMD_READ_BACK)->SetWindowTextA(cstrTemp);

////////////////////////////////////////////////// SAVE FILE
		char strFILE_NAME_CONTACT_FORCE_CONTROL[MTN_API_MAX_STRLEN_FILENAME];
			sprintf_s(strFILE_NAME_CONTACT_FORCE_CONTROL, MTN_API_MAX_STRLEN_FILENAME, "%s\\ContactForceControl_Y%dM%dD%d_H%dM%dS%d_Loop_%d.m",
				strFolderName,
				stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec, ii); 
			// 20121129
		extern 	double sys_get_controller_ts();
			// Upload data
			gstSystemScope.uiNumData = 7;
			gstSystemScope.uiDataLen = 4200;
			gstSystemScope.dSamplePeriod_ms = sys_get_controller_ts();
			if(ii == 0) 
			{
				mtnscope_upload_acsc_data_varname(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope");	continue;
			}
			if(mtnscope_upload_acsc_data_varname(m_hHandle, BUFFER_ID_SEARCH_CONTACT, "rAFT_Scope") 
				== MTN_API_OK_ZERO)
			{	
				FILE *fpData;
				fopen_s(&fpData, strFILE_NAME_CONTACT_FORCE_CONTROL, "w");
				if( fpData != NULL)
				{
					fprintf(fpData, "%% Position Calibration Output\n");
					fprintf(fpData, "dPositionFactor = %8.4f;", stOutputPosnCompensationTune.dPositionFactor);
					fprintf(fpData, "dCtrlOutOffset = %8.4f;\n", stOutputPosnCompensationTune.dCtrlOutOffset);
					mtnapi_upload_acs_sp_parameter_damp_switch(m_hHandle, &iDampGA);
					fprintf(fpData, "iDampACS = %d; \n", iDampGA);

					mtn_debug_print_to_file(fpData, &stSearchContactAndForceControl, &stTeachContactResult);
					fclose(fpData);
				}	
			}
///////////////////////////////////  SAVE FILE
			cstrTemp.Format("%d / Total %d", ii, nCountLoopCheckPositionForce);
			GetDlgItem(IDC_SHOW_FORCE_VERIFY_READBACK)->SetWindowTextA(cstrTemp);
	}
}
void CMtnPosnCompensDlg::ThreadDoForceCalibration()
{
	UpdateDataFromUIToVar();
//	stTeachContactParameter.iFlagSwitchToForceControl = 1;
	stTeachContactParameter.iSearchHeightPosition = stTeachContactResult.iContactPosnReg + iSearchHeight_um; // 20121211

	stForceCalibrationInput.stForceCaliPara.dDefForceFactor_Kf = DEF_FORCE_FACTOR_KF;//stForceCalibrationOutput.fForceFactor_Kf;
	stForceCalibrationInput.stForceCaliPara.dDefForceOffset_I0 = DEF_FORCE_OFFSET_I0;//stForceCalibrationOutput.fForceOffset_I0;
	stForceCalibrationInput.stForceCaliPara.dDefPosnFactor_Ka = DEF_FORCE_POSN_FACTOR_KA; // stForceCalibrationOutput.fPosnFactor_Ka;
	stForceCalibrationInput.stForceCaliPara.iInitForceHold_cnt = 10; // 10 count, 5 ms
	stForceCalibrationInput.stForceCaliPara.dPreImpForce_DCOM = 
		stForceCalibrationInput.stForceCaliPara.dPreImpForce_gram * DEF_FORCE_FACTOR_KF + DEF_FORCE_OFFSET_I0 +
		stTeachContactResult.iContactPosnReg * DEF_FORCE_POSN_FACTOR_KA;
		//-fabs(DEF_FORCE_OFFSET_I0);
	// stForceCalibrationInput.stForceCaliPara. {uiNumSegment, dFirstForce_gram, dLastForce_gram} from UI
	stForceCalibrationInput.stForceCaliPara.uiNumPointsSmoothing = 10; // After line-fitting to get Kf and I0, further test same force for this times to smooth I0

	stForceCalibrationInput.stPosnCompensate = stPosnCompensationTune; // from local file static variable

	// IDC_CHECK_FORCE_CALI_DEBUG_FLAG
	stForceCalibrationInput.iFlagDebug = ((CButton *)GetDlgItem(IDC_CHECK_FORCE_CALI_DEBUG_FLAG))->GetCheck();

	stForceCalibrationInput.stTeachContactPara = stTeachContactParameter;
	stForceCalibrationInput.stTeachContactPara.iFlagSwitchToForceControl = 1;
	stForceCalibrationInput.stTeachContactPara.iSearchVel = -10000;
	stForceCalibrationInput.stTeachContactPara.iSearchTolPE = 25;
	stForceCalibrationInput.stTeachContactPara.iMaxAccMoveSrchHt = 100;
	stForceCalibrationInput.stTeachContactPara.iMaxJerkMoveSrchHt = 1;

 	mtn_force_calibration(m_hHandle, &stForceCalibrationInput, &stForceCalibrationOutput);

	// IDC_STATIC_CURR_FORCE_CALI
	CString cstrTemp;
	cstrTemp.Format("Pa:%9.7f, Pb:%8.6f, Pc:%8.5f", 
		stForceCalibrationOutput.fForceCalibratePa, stForceCalibrationOutput.fForceCalibratePb, stForceCalibrationOutput.fForceCalibratePc);
		//stForceCalibrationOutput.fPosnFactor_Ka, stForceCalibrationOutput.fForceFactor_Kf, stForceCalibrationOutput.fForceOffset_I0);
	GetDlgItem(IDC_STATIC_CURR_FORCE_CALI)->SetWindowTextA(cstrTemp);
}

void CMtnPosnCompensDlg::ThreadDoForceVerificationByGramJig()  // 20130107
{

	stForceVerifyInput.fPosnFactor_Ka = stForceCalibrationOutput.fPosnFactor_Ka;
	stForceVerifyInput.fForceFactor_Kf = stForceCalibrationOutput.fForceFactor_Kf;
	stForceVerifyInput.fForceOffset_I0 = stForceCalibrationOutput.fForceOffset_I0;

	UpdateDataFromUIToVar();
	stTeachContactParameter.iFlagSwitchToForceControl = 1;
	stTeachContactParameter.iSearchHeightPosition = stTeachContactResult.iContactPosnReg + iSearchHeight_um; // 20121211
	stTeachContactParameter.iMaxDist = abs(stTeachContactResult.iContactPosnReg) + 500;

	stSearchContactAndForceControl.stpTeachContactPara = &stTeachContactParameter;
	stSearchContactAndForceControl.stpForceBlkPara = &stForceBlock;
	
	stForceBlock.dPreImpForce_DCOM = -1.8; //

	stForceBlock.uiNumSegment = 2;
	stForceBlock.aiForceBlk_Rampcount[0] = 50;
	stForceBlock.aiForceBlk_LevelCount[0] = 200;
	stForceBlock.adForceBlk_LevelAmplitude[0] = -fabs(stForceVerifyInput.dForceVerifyDesiredGram/2 * stForceVerifyInput.fForceFactor_Kf) 
						+ stTeachContactResult.iContactPosnReg * fabs(stForceVerifyInput.fPosnFactor_Ka) 
						+ stForceVerifyInput.fForceOffset_I0 ;

	stForceBlock.aiForceBlk_Rampcount[1] = 50;
	stForceBlock.aiForceBlk_LevelCount[1] = 2000 * 60 * 10; // around 10 minutes
	stForceBlock.adForceBlk_LevelAmplitude[1] = -fabs(stForceVerifyInput.dForceVerifyDesiredGram * stForceVerifyInput.fForceFactor_Kf) 
		+ stTeachContactResult.iContactPosnReg * fabs(stForceVerifyInput.fPosnFactor_Ka) 
		+ stForceVerifyInput.fForceOffset_I0;
		
//			(stOutputPosnCompensationTune.dPositionFactor * giContactPositionReg 
//				+ stOutputPosnCompensationTune.dCtrlOutOffset)
//					/32767 * 100; // unit of DCOM is percentage of 32767, DOUT

	cFlagDoingForceVerifyByGramGauge = TRUE;

	TEACH_CONTACT_OUTPUT stTeachContactResultTemp;
	mtn_search_contract_and_force_control(m_hHandle, &stSearchContactAndForceControl, &stTeachContactResultTemp);
		// Update the contact position real-time
//		giContactPositionReg = stTeachContactResult.iContactPosnReg;

	CString cstrTemp;
	cstrTemp.Format("EndVerify");
	GetDlgItem(IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("Unchecking the EndVerify to end the force verification");
	AfxMessageBox(cstrTemp);
}

void CMtnPosnCompensDlg::ThreadDoForceVerification() // 20130103
{
	CString cstrTemp;
	stForceVerifyInput.stTeachContactPara = stTeachContactParameter;
	stForceVerifyInput.stTeachContactPara.iFlagSwitchToForceControl = 1;
	stForceVerifyInput.fPosnFactor_Ka = stForceCalibrationOutput.fPosnFactor_Ka;
	stForceVerifyInput.fForceFactor_Kf = stForceCalibrationOutput.fForceFactor_Kf;
	stForceVerifyInput.fForceOffset_I0 = stForceCalibrationOutput.fForceOffset_I0;
	// stForceVerifyInput.iContactPosn, may be changed
	// IDC_CHECK_FORCE_CALI_DEBUG_FLAG
	stForceVerifyInput.iFlagDebug = ((CButton *)GetDlgItem(IDC_CHECK_FORCE_CALI_DEBUG_FLAG))->GetCheck();

	// stForceVerifyInput.dForceVerifyDesiredGram is from the UI

	double dSumForceReadBack_gram = 0, dSumI0 = 0;
	for(unsigned int ii = 0; ii<uiLoopContactForceTest; ii++)
	{
		if(ii == 0)  // Teach contact only for the first time
		{
			stForceVerifyInput.iFlagTeachContact = 1;
		}
		else
		{
			stForceVerifyInput.iFlagTeachContact = 0;
			stForceVerifyInput.iContactPosn = stForceVerifyOutput.iContactPosnReg;
		}
		mtn_force_verify(m_hHandle, &stForceVerifyInput, &stForceVerifyOutput);

		cstrTemp.Format("CP: %d, Read back(gram): %8.2f", stForceVerifyOutput.iContactPosnReg, stForceVerifyOutput.dForceVerifyReadBack);
		GetDlgItem(IDC_SHOW_FORCE_VERIFY_READBACK)->SetWindowTextA(cstrTemp);

		//stForceVerifyInput.fForceOffset_I0 = stForceVerifyInput.fForceOffset_I0 + (stForceVerifyInput.dForceVerifyDesiredGram - stForceVerifyOutput.dForceVerifyReadBack) * stForceVerifyInput.fForceFactor_Kf;

		dSumForceReadBack_gram = dSumForceReadBack_gram + stForceVerifyOutput.dForceVerifyReadBack;
		//dSumI0 = dSumI0 + stForceVerifyInput.fForceOffset_I0;
		Sleep(100);
	}

	if(nCountLoopCheckPositionForce > 0) // 20130102
	{
		cstrTemp.Format("CP: %d, Ave Force(gram): %8.2f", stForceVerifyOutput.iContactPosnReg, dSumForceReadBack_gram/uiLoopContactForceTest); // , 20130102
		GetDlgItem(IDC_SHOW_FORCE_VERIFY_READBACK)->SetWindowTextA(cstrTemp);
	}

	//stForceCalibrationOutput.fForceOffset_I0 = dSumI0/uiLoopContactForceTest;
}

// IDC_BUTTON_SEARCH_CONTACT_WITH_FORCE_CONTROL
void CMtnPosnCompensDlg::OnBnClickedButtonSearchContactWithForceControl()
{
	cFlagThreadInPosnForceCaliDlg = THREAD_SEARCH_CONTACT_WITH_FORCE_CONTROL;

	RunPosnForceCaliThread();

//	cFlagThreadInPosnForceCaliDlg == THREAD_TEACH_LIMIT

}

// IDC_FORCE_CONTROL_HOLD_COUNT_INIT_FORCE
void CMtnPosnCompensDlg::OnEnChangeForceControlHoldCountInitForce()
{
	GetDlgItem(IDC_FORCE_CONTROL_HOLD_COUNT_INIT_FORCE)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &stForceBlock.iInitForceHold_cnt);
}

//IDC_BUTTON_FORCE_VERIFICATION

// IDC_BUTTON_FORCE_CALIBRATION 20130102
void CMtnPosnCompensDlg::OnBnClickedButtonForceCalibration()
{

	cFlagThreadInPosnForceCaliDlg = THREAD_FORCE_CALIBRATION;
	RunPosnForceCaliThread();

}

void CMtnPosnCompensDlg::OnBnClickedButtonForceVerification()
{
	cFlagThreadInPosnForceCaliDlg = THREAD_FORCE_VERIFICATION;
	RunPosnForceCaliThread();
}

// IDC_BUTTON_VERIFICATION_LOG

void CMtnPosnCompensDlg::OnBnClickedButtonVerificationLog()
{
	cFlagThreadInPosnForceCaliDlg = THREAD_FORCE_LOG_VERIFICATION; // 20130103
	RunPosnForceCaliThread();
}

void CMtnPosnCompensDlg::OnBnClickedOkDialogPositionCompensation()
{
//	acs_clear_buffer_prog_search_contact_force_control();

	acs_stop_buffer_prog_srch_contact_f_cali_auto_tune_z(); // 20130107
	acs_compile_buffer_prog_srch_contact_f_cali_auto_tune_z();
	mtn_acs_convert_from_current_to_position_loop(m_hHandle, ACSC_AXIS_A); // 20130107

	// 20130107
	iPosnForceCalibrateDlgFlagShow = 0;
	CDialog::OnOK();
}
void CMtnPosnCompensDlg::ShowWindow(int nCmdShow)
{
	iPosnForceCalibrateDlgFlagShow = 1;
	CDialog::ShowWindow(nCmdShow);
}
// IDC_CHECK_CONTACT_TUNE_FLAG_B1W
void CMtnPosnCompensDlg::OnBnClickedCheckContactTuneFlagB1w()
{
	iContactTuneFlagB1W = ((CButton *)GetDlgItem(IDC_CHECK_CONTACT_TUNE_FLAG_B1W))->GetCheck();
}

// IDC_COMBO_POSN_FORCE_COMPENSATION_DLG_FORCE_CONTROL_DETECTION_FLAG, 20121030
void CMtnPosnCompensDlg::OnCbnSelchangeComboPosnForceCompensationDlgForceControlDetectionFlag()
{
	cFlagContactDetectionSwitch = ((CComboBox*)GetDlgItem(IDC_COMBO_POSN_FORCE_COMPENSATION_DLG_FORCE_CONTROL_DETECTION_FLAG))->GetCurSel();
	stTeachContactParameter.cFlagDetectionSP = cFlagContactDetectionSwitch;
}

void CMtnPosnCompensDlg::UI_UpdateEditByDetectionFlag(char cFlagDetectionSwitch)
{
	switch(cFlagContactDetectionSwitch)
	{
	case SP_DETECTION_NONE:
		break;
	case SP_DETECTION_BY_DOUT:
		break;
	case SP_DETECTION_BY_PE:
		break;
	case SP_DETECTION_BY_AIN:
		break;
	}
}

// IDC_POSNCOMPN_UPPER_LIMIT
void CMtnPosnCompensDlg::OnEnChangePosncompnUpperLimit()
{
}
// IDC_POSNCOMPN_TOTAL_POINT
void CMtnPosnCompensDlg::OnEnChangePosncompnTotalPoint()
{
}
// IDC_POSNCOMPN_LOWER_LIMIT
void CMtnPosnCompensDlg::OnEnChangePosncompnLowerLimit()
{
}

// IDC_COMBO_FORCE_CONTROL_EDIT_SEGMENT
void CMtnPosnCompensDlg::OnCbnSelchangeComboForceControlEditSegment()
{
	// TODO: Add your control notification handler code here
	CComboBox *pSelectAxisForAxis1 = (CComboBox*) GetDlgItem(IDC_COMBO_FORCE_CONTROL_EDIT_SEGMENT);

	uiCurrEditSeg = pSelectAxisForAxis1->GetCurSel();
	UpdateEditCurrSegment(uiCurrEditSeg);
}

// IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceControlRampCountCurrSeg()
{
}
// IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceControlLevelCntCurrSeg()
{
}
// IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceControlLevelAmpCurrSeg()
{
}
// IDC_EDIT_FORCE_CALI_TOTAL_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceCaliTotalSeg()
{
}
// IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceCaliGramFirstSeg()
{
}
// IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG
void CMtnPosnCompensDlg::OnEnChangeEditForceCaliGramLastSeg()
{
}
// IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM
void CMtnPosnCompensDlg::OnEnChangeEditForceVerifyDesiredGram()
{
}
///////////////////////////////////////////
//////////// Replace  by KillFocus
///////////////////////////////////////////
// IDC_POSNCOMPN_TOTAL_POINT
void CMtnPosnCompensDlg::OnEnKillfocusPosncompnTotalPoint()
{
	GetDlgItem(IDC_POSNCOMPN_TOTAL_POINT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stPosnCompensationTune.uiTotalPoints);
}
// IDC_POSNCOMPN_UPPER_LIMIT
void CMtnPosnCompensDlg::OnEnKillfocusPosncompnUpperLimit()
{
	GetDlgItem(IDC_POSNCOMPN_UPPER_LIMIT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dUpperPosnLimit);
}
// IDC_POSNCOMPN_LOWER_LIMIT
void CMtnPosnCompensDlg::OnEnKillfocusPosncompnLowerLimit()
{
	GetDlgItem(IDC_POSNCOMPN_LOWER_LIMIT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dLowerPosnLimit);
}
//////////////////////////// Teach Contact
// IDC_EDIT_TEACH_CONTACT_START_DIST
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactStartDist()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_START_DIST)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &iSearchHeight_um); // stTeachContactParameter.iSearchHeightPosition);
}

// IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactStartVel()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_ACC_MOVE_SRCH_HT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxAccMoveSrchHt);
}
// IDC_EDIT_TEACH_CONTACT_SEARCH_VEL
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactSearchVel()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_VEL)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iSearchVel);
}
// IDC_EDIT_TEACH_CONTACT_END_POSN
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactEndPosn()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_END_POSN)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iResetPosition);
}
// IDC_EDIT_TEACH_CONTACT_MAX_DIST
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactMaxDist()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_MAX_DIST)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxDist);
}
// IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactAntiBounce()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_ANTI_BOUNCE)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iAntiBounce);
}

//IDC_EDIT_TEACH_CONTACT_TIME_OUT
void CMtnPosnCompensDlg::OnEnKillfocusEditTeachContactTimeOut()
{
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_TIME_OUT)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iTimeOut);
}

// IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT
void CMtnPosnCompensDlg::OnEnKillfocusEditForceControlMaxSegment()
{
	unsigned int iPrevNumSeg;
	
	iPrevNumSeg = stForceBlock.uiNumSegment;
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &stForceBlock.uiNumSegment);
	if(stForceBlock.uiNumSegment > MAX_FORCE_SEG)
	{
		stForceBlock.uiNumSegment = MAX_FORCE_SEG;
		sprintf_s(strTextTemp, 32, "%d", stForceBlock.uiNumSegment);
		GetDlgItem(IDC_EDIT_FORCE_CONTROL_MAX_SEGMENT)->SetWindowTextA(_T(strTextTemp));
	}
	if(stForceBlock.uiNumSegment > iPrevNumSeg)
	{
		unsigned int ii;
		for(ii = iPrevNumSeg; ii<stForceBlock.uiNumSegment; ii++)
		{
			stForceBlock.aiForceBlk_Rampcount[ii] = 3;
			stForceBlock.aiForceBlk_LevelCount[ii] = 0;
			stForceBlock.adForceBlk_LevelAmplitude[ii] = stForceBlock.adForceBlk_LevelAmplitude[ii-1];
		}
		CComboBox *pSelectAxisForAxis1 = (CComboBox*) GetDlgItem(IDC_COMBO_FORCE_CONTROL_EDIT_SEGMENT);
		for(ii=iPrevNumSeg; ii< stForceBlock.uiNumSegment; ii++)
		{
			sprintf_s(strTextTemp, 32, "%d", ii);
			pSelectAxisForAxis1->AddString(strTextTemp);
		}
	}
	else
	{
		uiCurrEditSeg = 0;
		UpdateEditCurrSegment(uiCurrEditSeg);
	}
}
// IDC_EDIT_CONTACT_FORCE_LOOP_TEST
//void CMtnPosnCompensDlg::OnEnKillfocusEditContactForceLoopTest()
//{
//}
// IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceControlRampCountCurrSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_RAMP_COUNT_CURR_SEG)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &stForceBlock.aiForceBlk_Rampcount[uiCurrEditSeg]);
}
// IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceControlLevelCntCurrSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_LEVEL_CNT_CURR_SEG)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &stForceBlock.aiForceBlk_LevelCount[uiCurrEditSeg]);
}
// IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceControlLevelAmpCurrSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CONTROL_LEVEL_AMP_CURR_SEG)->GetWindowTextA(strTextTemp, 32);
	stForceBlock.adForceBlk_LevelAmplitude[uiCurrEditSeg] = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_FORCE_CALI_TOTAL_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliTotalSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CALI_TOTAL_SEG)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &stForceCalibrationInput.stForceCaliPara.uiNumSegment);
}
// IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliGramFirstSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CALI_GRAM_FIRST_SEG)->GetWindowTextA(strTextTemp, 32);
	stForceCalibrationInput.stForceCaliPara.dFirstForce_gram = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG
void CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliGramLastSeg()
{
	GetDlgItem(IDC_EDIT_FORCE_CALI_GRAM_LAST_SEG)->GetWindowTextA(strTextTemp, 32);
	stForceCalibrationInput.stForceCaliPara.dLastForce_gram = strtod(strTextTemp, &strStopString);
}
// IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM
void CMtnPosnCompensDlg::OnEnKillfocusEditForceVerifyDesiredGram()
{
	GetDlgItem(IDC_EDIT_FORCE_VERiFY_DESIRED_GRAM)->GetWindowTextA(strTextTemp, 32);
	stForceVerifyInput.dForceVerifyDesiredGram = strtod(strTextTemp, &strStopString);
}

void CMtnPosnCompensDlg::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*pdValue = strtod(tempChar, &strStopString);
//	sscanf_s(tempChar, "%f", pdValue);
}

// IDC_COMBO_SEARCH_VEL_POSN_FORCE_CALIBRATION_DLG
void CMtnPosnCompensDlg::OnCbnSelchangeComboSearchVelPosnForceCalibrationDlg()
{
	cFlagSearchSpd = ((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_VEL_POSN_FORCE_CALIBRATION_DLG))->GetCurSel();

	stTeachContactParameter.iSearchVel = (int)(-10000.0 * (cFlagSearchSpd * 0.1 + 0.1)); // 20130205
	CString cstrTemp; cstrTemp.Format("%d", stTeachContactParameter.iSearchVel);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_VEL)->SetWindowTextA(cstrTemp);
}

// IDC_COMBO_SEARCH_TOL_PE_POSN_FORCE_CALIBRATION_DLG
void CMtnPosnCompensDlg::OnCbnSelchangeComboSearchTolPePosnForceCalibrationDlg()
{
	cFlagSearchTolPE = ((CComboBox*)GetDlgItem(IDC_COMBO_SEARCH_TOL_PE_POSN_FORCE_CALIBRATION_DLG))->GetCurSel();

	stTeachContactParameter.iSearchTolPE = cFlagSearchTolPE * 6 + 6;
	CString cstrTemp; cstrTemp.Format("%d", stTeachContactParameter.iSearchTolPE);
	GetDlgItem(IDC_EDIT_TEACH_CONTACT_SEARCH_TOL_PE)->SetWindowTextA(cstrTemp);
}
// IDC_COMBO_DAMP_POSN_FORCE_CALIBRATION_DLG
void CMtnPosnCompensDlg::OnCbnKillfocusComboDampPosnForceCalibrationDlg()
{
	cFlagSwitchDamp = ((CComboBox*)GetDlgItem(IDC_COMBO_DAMP_POSN_FORCE_CALIBRATION_DLG))->GetCurSel();

	int iDampSP = cFlagSwitchDamp * 5 + 5;

	mtnapi_download_acs_sp_parameter_damp_switch(m_hHandle, iDampSP);
//	SETSPV(SP0:DampGA, )
}

// IDC_COMBO_PRE_IMP_FORCE_POSITION_FORCE_CALI_DLG
void CMtnPosnCompensDlg::OnCbnSelchangeComboPreImpForcePositionForceCaliDlg()
{
	cFlagPreImpactForce = ((CComboBox*)GetDlgItem(IDC_COMBO_PRE_IMP_FORCE_POSITION_FORCE_CALI_DLG))->GetCurSel();

	stForceCalibrationInput.stForceCaliPara.dPreImpForce_gram = cFlagPreImpactForce * 20.0;
}

// IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG
void CMtnPosnCompensDlg::OnEnKillfocusEditMaxJerkMoveSrchHtPosnForceCaliDlg()
{
	GetDlgItem(IDC_EDIT_MAX_JERK_MOVE_SRCH_HT_POSN_FORCE_CALI_DLG)->GetWindowTextA(&strTextTemp[0], 64);
	sscanf_s(strTemp, "%d", &stTeachContactParameter.iMaxJerkMoveSrchHt);
}

// IDC_STATIC_FORCE_FEEDBACK_POSITION_FORCE_CALIBRATION_DLG


// IDC_COMBO_LOOP_COUNT_CHECK_FORCE_POSITION_CALIBRATION_DLG
void CMtnPosnCompensDlg::OnCbnSelchangeComboLoopCountCheckForcePositionCalibrationDlg()
{
	nIdxComboCheckPositionForce = ((CComboBox*)GetDlgItem(IDC_COMBO_LOOP_COUNT_CHECK_FORCE_POSITION_CALIBRATION_DLG))->GetCurSel();
	nCountLoopCheckPositionForce = aCountLoopCheckPositionForce[nIdxComboCheckPositionForce];
}
// IDC_STATIC_CALI_FORCE_RATIO_DISPLAY
// IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG
// IDC_STATIC_FORCE_READ_ADC_POSN_FORCE_DLG
// IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG
#include "acsc.h"

#define __AVE_FB_LEN_UNIT_DELAYLINE__ 8
int aFbForceAdcTimeCheck[__AVE_FB_LEN_UNIT_DELAYLINE__];

#define __MOVING_AVERAGE_LEN_   4
double aFbForceAdcMovingAve[__MOVING_AVERAGE_LEN_];
static double dOldestFbForceAdc;

int iRealTimeSmoothForceFbAdc;

extern int mpi_calibrate_force_ratio(double *dRetForceRatio);
extern int aiForceRatioCalibrationFbAdc[];
extern double adForceRatioCalibrationFbGram[];

#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_IDLE              0
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_PREPARATION       1
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_CONTACT_F_SENSOR  2
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_1     3
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_2     4
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_3     5
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_4     6
#define FLAG_CALIBRATE_FORCE_RATIO_STAGE_CLEAR_FORCE_RST   7

int iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_IDLE;
void CMtnPosnCompensDlg::OnBnClickedButtonCalibrateForceRatioPosnForceCalibrateDlg()
{
	static double dInitForceDrvCmd, dCurrDrvCmd, dPrevDrvCmd;
	static double dForceRatio; // 20120109

	double dPrevFbPosition, dCurrFbPosition, dDeltaDrvCmd; // 20121224
	int aiCtrlOutput[128]; double dCurrCtrlOutput; // 20121224
	CString cstrTemp;
	static int iMotorFlags, ii;
	switch(iFlagStageCalibrateForceRatio)
	{
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_IDLE:
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_PREPARATION;
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(_T("Move Table s.t. Capillary above force sensor"));
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-1:>>"));
		GetDlgItem(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG)->EnableWindow(FALSE);
		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_PREPARATION:
		// Teach Contact, then stay at contact level
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_CONTACT_F_SENSOR;
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(_T("Teach contact, stay contact level "));
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-2:>>"));
		aiForceRatioCalibrationFbAdc[0] = iRealTimeSmoothForceFbAdc;
		adForceRatioCalibrationFbGram[0] = 0;
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(FALSE);
		OnBnClickedButtonTeachContact();
		Sleep(3000);
		stPosnCompensationTune.dLowerPosnLimit = stTeachContactResult.iContactPosnReg + 1000;
		stPosnCompensationTune.dUpperPosnLimit = stTeachContactResult.iContactPosnReg + 6000;
		mtune_position_compensation(m_hHandle, sys_get_acs_axis_id_bnd_z(), &stPosnCompensationTune, &stOutputPosnCompensationTune);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(TRUE);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_CONTACT_F_SENSOR:

		// Average of DOUT
		dCurrCtrlOutput = 0;
		for(ii = 0; ii< 128; ii++)
		{
			acsc_ReadInteger(m_hHandle, 0, "DOUT", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &aiCtrlOutput[ii], 0); // DOUT
			dCurrCtrlOutput = dCurrCtrlOutput + aiCtrlOutput[ii];
		}
		dInitForceDrvCmd = dCurrCtrlOutput/128.0 / 32767 * 100.0/8;
		//dInitForceDrvCmd = stTeachContactResult.dPreImpForce_DCOMReadBack - 0.3; //(stTeachContactResult.iContactPosnReg * stOutputPosnCompensationTune.dPositionFactor + stOutputPosnCompensationTune.dCtrlOutOffset)/100;
//		if(dInitForceDrvCmd >= -0.3) dInitForceDrvCmd = -0.3;
		if(dInitForceDrvCmd <= -1.5) dInitForceDrvCmd = -1.5;

		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_1;
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-3:>>"));
		GetDlgItem(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG)->EnableWindow(TRUE);

		// Get initial feedback position, and initialize the DCOM
		mtnapi_get_fb_position(m_hHandle, ACSC_AXIS_A, &dPrevFbPosition, 0);
		dDeltaDrvCmd = -fabs(dInitForceDrvCmd)/100.0;
		dCurrDrvCmd = dInitForceDrvCmd;
		acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOU

		acsc_ReadInteger(m_hHandle, 0, "MFLAGS", 
			ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &iMotorFlags, 0); // DOUT
		//	OpenLoop DAC mode, MFLAGS(iAFT_Axis).1=1; MFLAGS(iAFT_Axis).17=0;\n",
			// now is disabled, to enable it
			iMotorFlags = iMotorFlags | OR_SET_BIT_1;
			iMotorFlags = iMotorFlags & AND_CLEAR_BIT_17;
		acsc_WriteInteger(m_hHandle, 0, "MFLAGS", 
			ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &iMotorFlags, 0); // DOUT

		
		Sleep(10);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(0);
		Sleep(50);
		mtnapi_enable_motor(m_hHandle, ACSC_AXIS_A, 0);
		Sleep(50);
		// 
		mtn_acs_wait_axis_settle(m_hHandle, ACSC_AXIS_A);
		mtnapi_get_fb_position(m_hHandle, ACSC_AXIS_A, &dCurrFbPosition, 0);

		while(fabs(dCurrFbPosition - dPrevFbPosition) > 5.0)
		{
			dPrevFbPosition = dCurrFbPosition;
			dCurrDrvCmd -= dDeltaDrvCmd;
			acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
			Sleep(10);
			if(mtn_acs_wait_axis_settle(m_hHandle, ACSC_AXIS_A) == MTN_API_OK_ZERO)
			{
				mtnapi_get_fb_position(m_hHandle, ACSC_AXIS_A, &dCurrFbPosition, 0);
			}
			else
			{
				AfxMessageBox(_T("Error Settling"));
			}
		}
		//for(ii = 0; ii < 100; ii++)
		//{
		//	dCurrDrvCmd = ii/100.0 * dInitForceDrvCmd;
		//	acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
		//	Sleep(10);
		//}
		Sleep(2000);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(TRUE);
		aiForceRatioCalibrationFbAdc[1] = iRealTimeSmoothForceFbAdc; ///----- 1
		cstrTemp.Format("Get the gram gauge for recording, %4.2f:%d", dInitForceDrvCmd, aiForceRatioCalibrationFbAdc[1]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_1:
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_2;
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-4:>>"));
		ReadDoubleFromEdit(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG, &adForceRatioCalibrationFbGram[1]); ///----- 1

		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(0);
		acsc_ReadReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
		dPrevDrvCmd = dCurrDrvCmd;
		for(ii = 0; ii <= 100; ii++)
		{
			dCurrDrvCmd = dPrevDrvCmd - ii * 0.3 /100.0 ;
			acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
			Sleep(10);
		}

		Sleep(2000);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(TRUE);

		aiForceRatioCalibrationFbAdc[2] = iRealTimeSmoothForceFbAdc; ///----- 2
		cstrTemp.Format("%4.2f: %d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[2]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);
		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_2:
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_3;
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-5:>>"));
		ReadDoubleFromEdit(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG, &adForceRatioCalibrationFbGram[2]); ///----- 2

		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(0);
		acsc_ReadReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
		dPrevDrvCmd = dCurrDrvCmd;
		for(ii = 0; ii <= 100; ii++)
		{
			dCurrDrvCmd = dPrevDrvCmd - ii * 0.3 /100.0 ;
			acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
			Sleep(5);
		}
		Sleep(2000);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(TRUE);
		aiForceRatioCalibrationFbAdc[3] = iRealTimeSmoothForceFbAdc;  ///-----3
		cstrTemp.Format("%4.2f: %d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[3]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_3:
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_4;
//		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(_T("Measure by gram gauge, key in"));
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cali. Step-6:>>"));
		ReadDoubleFromEdit(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG, &adForceRatioCalibrationFbGram[3]);  ///-----3

		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(0);
		acsc_ReadReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
		dPrevDrvCmd = dCurrDrvCmd;
		for(ii = 0; ii <= 100; ii++)
		{
			dCurrDrvCmd = dPrevDrvCmd - ii * 0.3 /100.0 ;
			acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
			Sleep(5);
		}
		Sleep(2000);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->EnableWindow(TRUE);

		aiForceRatioCalibrationFbAdc[4] = iRealTimeSmoothForceFbAdc; ///------4
		cstrTemp.Format("%4.2f:%d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[4]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_4:
		ReadDoubleFromEdit(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG, &adForceRatioCalibrationFbGram[4]);  ///------4
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_CLEAR_FORCE_RST;
		
		cstrTemp.Format("%d, %2.0f; %d, %2.0f; %d, %2.0f; %d, %2.0f; %d, %2.0f;", aiForceRatioCalibrationFbAdc[0], adForceRatioCalibrationFbGram[0],
			aiForceRatioCalibrationFbAdc[1], adForceRatioCalibrationFbGram[1], aiForceRatioCalibrationFbAdc[2], adForceRatioCalibrationFbGram[2],
			aiForceRatioCalibrationFbAdc[3], adForceRatioCalibrationFbGram[3], aiForceRatioCalibrationFbAdc[4], adForceRatioCalibrationFbGram[4]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Calc F-Ratio >>"));
		GetDlgItem(IDC_EDIT_FORCE_READ_BY_GRAM_GAUGE_POSN_FORCE_DLG)->EnableWindow(FALSE);

		dPrevDrvCmd = dCurrDrvCmd;
		for(ii = 0; ii < 100; ii++)
		{
			dCurrDrvCmd = (100 - ii)/100.0 * dPrevDrvCmd;
			acsc_WriteReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT
			Sleep(5);
		}
		acsc_ReadInteger(m_hHandle, 0, "MFLAGS", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &iMotorFlags, 0); // DOUT
		iMotorFlags = iMotorFlags & AND_CLEAR_BIT_1;
		iMotorFlags = iMotorFlags | OR_SET_BIT_17;
		acsc_WriteInteger(m_hHandle, 0, "MFLAGS", ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &iMotorFlags, 0); // DOUT
		mtnapi_disable_motor(m_hHandle, ACSC_AXIS_A, 0);

		break;

	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_CLEAR_FORCE_RST:
		iFlagStageCalibrateForceRatio = FLAG_CALIBRATE_FORCE_RATIO_STAGE_IDLE;
		if(mpi_calibrate_force_ratio(&dForceRatio) == MTN_CALC_FORCE_SENSOR_ERROR)
		{
			cstrTemp.Format("fR:%8.5f, %d, %2.0f; %d, %2.0f; %d, %2.0f; %d, %2.0f; %d, %2.0f", dForceRatio,
				aiForceRatioCalibrationFbAdc[0], adForceRatioCalibrationFbGram[0],
				aiForceRatioCalibrationFbAdc[1], adForceRatioCalibrationFbGram[1], aiForceRatioCalibrationFbAdc[2], adForceRatioCalibrationFbGram[2],
				aiForceRatioCalibrationFbAdc[3], adForceRatioCalibrationFbGram[3], aiForceRatioCalibrationFbAdc[4], adForceRatioCalibrationFbGram[4]);
		}
		else
		{
			cstrTemp.Format("ForceRatio: %10.8f", dForceRatio);
			mtn_set_force_ratio(dForceRatio); // 20130109
		}
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);
		GetDlgItem(IDC_BUTTON_CALIBRATE_FORCE_RATIO_POSN_FORCE_CALIBRATE_DLG)->SetWindowTextA(_T("Cal. F-Ratio"));
		mtnapi_enable_motor(m_hHandle, ACSC_AXIS_A, 0);

		if(dForceRatio < DEF_FORCE_GRAM_PER_ADC_UPP && dForceRatio > DEF_FORCE_GRAM_PER_ADC_LOW)
		{
		}
		else
		{
			cstrTemp.Format("Force Ratio %8.5f ,out of range: [%5.3f, %5.3f]. Re-do calibration", dForceRatio,
				DEF_FORCE_GRAM_PER_ADC_LOW, DEF_FORCE_GRAM_PER_ADC_UPP);
			AfxMessageBox(cstrTemp);
		}
		break;
	}
}


void CMtnPosnCompensDlg::InitForceFbAdc()
{
	double dSumFbForce;
//	int iForceFbAdc;
	dSumFbForce = 0;
	for(int ii = 0; ii<__MOVING_AVERAGE_LEN_; ii++)
	{
		
		Sleep(50);
#ifdef __INIT_DELAY_LINE_BY_ADC__
		acsc_GetAnalogInput(m_hHandle, 0, &iForceFbAdc, 0);
		aFbForceAdcMovingAve[ii] = (double)iForceFbAdc;		
#else
		aFbForceAdcMovingAve[ii] = 0;
#endif // __CLEAR_DELAY_LINE__

		dSumFbForce = dSumFbForce + aFbForceAdcMovingAve[ii];
	}
	iRealTimeSmoothForceFbAdc = (int)(dSumFbForce /__MOVING_AVERAGE_LEN_ );
	for(int ii = 0; ii<__AVE_FB_LEN_UNIT_DELAYLINE__; ii++)
	{
		aFbForceAdcTimeCheck[ii] = iRealTimeSmoothForceFbAdc;
		//acsc_GetAnalogInput(m_hHandle, 0, &aFbForceAdcTimeCheck[ii], 0);
	}

	CString cstrTemp;
	cstrTemp.Format("ForceRatio: %10.8f", mtn_get_force_ratio());
	GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

}

static unsigned int idxMovingAveForceFb;
void CMtnPosnCompensDlg::DlgEventUpdateForceFbAdc()
{
int ii;
double dSumFbForce, dAveFbForceAdc;
unsigned int idxNextForceFbMovingAve;

	dSumFbForce = 0;
	for(ii = 0; ii<__AVE_FB_LEN_UNIT_DELAYLINE__; ii++)
	{
		acsc_GetAnalogInput(m_hHandle, 0, &aFbForceAdcTimeCheck[ii], 0);
		dSumFbForce = dSumFbForce + aFbForceAdcTimeCheck[ii];
	}
	dAveFbForceAdc = dSumFbForce/__AVE_FB_LEN_UNIT_DELAYLINE__;

	idxNextForceFbMovingAve = idxMovingAveForceFb + 1;
	if(idxNextForceFbMovingAve >= __MOVING_AVERAGE_LEN_)
	{
		idxNextForceFbMovingAve = 0;
	}
	dOldestFbForceAdc = aFbForceAdcMovingAve[idxNextForceFbMovingAve];
	aFbForceAdcMovingAve[idxNextForceFbMovingAve] = dAveFbForceAdc;

	iRealTimeSmoothForceFbAdc = iRealTimeSmoothForceFbAdc + (int)((dAveFbForceAdc - dOldestFbForceAdc)/__MOVING_AVERAGE_LEN_);

	idxMovingAveForceFb = idxNextForceFbMovingAve;

}

void CMtnPosnCompensDlg::DlgTimerEvent()
{
	DlgEventUpdateForceFbAdc();

	///// 
CString cstemp;
	cstemp.Format("%d", iRealTimeSmoothForceFbAdc);
	GetDlgItem(IDC_STATIC_FORCE_READ_ADC_POSN_FORCE_DLG)->SetWindowTextA(cstemp);

	/// Force Verification by GramGauge
	if(cFlagDoingForceVerifyByGramGauge == TRUE 
		|| mFlagStopPosnForceCaliThread == FALSE)
	{
		UI_SetEnableButton(FALSE); // 20130107
	}
	else
	{
		UI_SetEnableButton(TRUE); // 20130107
	}

}


void CMtnPosnCompensDlg::OnEnSetfocusEditForceReadByGramGaugePosnForceDlg()
{
	CString cstrTemp;
	double dCurrDrvCmd;
	acsc_ReadReal(m_hHandle, 0, "DCOM", 	ACSC_AXIS_A,ACSC_AXIS_A, 0, 0, &dCurrDrvCmd, 0); // DOUT

	switch(iFlagStageCalibrateForceRatio)
	{
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_1:
		aiForceRatioCalibrationFbAdc[1] = iRealTimeSmoothForceFbAdc; ///----- 1
		cstrTemp.Format("%4.2f:%d", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[1]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_2:
		aiForceRatioCalibrationFbAdc[2] = iRealTimeSmoothForceFbAdc; ///----- 2
		cstrTemp.Format("%4.2f: %d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[2]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_3:
		aiForceRatioCalibrationFbAdc[3] = iRealTimeSmoothForceFbAdc;  ///-----3
		cstrTemp.Format("%4.2f: %d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[3]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);

		break;
	case FLAG_CALIBRATE_FORCE_RATIO_STAGE_APPLY_FORCE_4:
		aiForceRatioCalibrationFbAdc[4] = iRealTimeSmoothForceFbAdc; ///------4
		cstrTemp.Format("%4.2f:%d, Measure by gram gauge, key in", dCurrDrvCmd, aiForceRatioCalibrationFbAdc[4]);
		GetDlgItem(IDC_STATIC_CALI_FORCE_RATIO_DISPLAY)->SetWindowTextA(cstrTemp);
		break;
	}
}

void CMtnPosnCompensDlg::Thread_ForceLogVerification()
{
	CString cstrTemp;
	char strText[128];
	FORCE_VERIFY_INPUT stLogForceVerifyInput;
	//stForceVerifyOutput;

	stLogForceVerifyInput.stTeachContactPara = stTeachContactParameter;
	stLogForceVerifyInput.stTeachContactPara.iFlagSwitchToForceControl = 1;
	stLogForceVerifyInput.fPosnFactor_Ka = stForceCalibrationOutput.fPosnFactor_Ka;
	stLogForceVerifyInput.fForceFactor_Kf = stForceCalibrationOutput.fForceFactor_Kf;
	stLogForceVerifyInput.fForceOffset_I0 = stForceCalibrationOutput.fForceOffset_I0;
	// stLogForceVerifyInput.iContactPosn, may be changed
	// IDC_CHECK_FORCE_CALI_DEBUG_FLAG
	stLogForceVerifyInput.iFlagDebug = ((CButton *)GetDlgItem(IDC_CHECK_FORCE_CALI_DEBUG_FLAG))->GetCheck();

	double dForceVerifyFirstGram = stForceCalibrationInput.stForceCaliPara.dFirstForce_gram;
	double dForceVerifyLastGram = stForceCalibrationInput.stForceCaliPara.dLastForce_gram;
	unsigned int nTotalGramPerContactLevel = stForceCalibrationInput.stForceCaliPara.uiNumSegment;
	double dDeltaForce = (dForceVerifyLastGram - dForceVerifyFirstGram)/(nTotalGramPerContactLevel - 1);
	double dSumForceReadBack_gram ;

	FILE *fpData = NULL;
    SYSTEMTIME stTime;
    GetSystemTime(&stTime);
	sprintf_s(strText, 128, "ForceLog_Y%dM%dD%d.txt", stTime.wYear, stTime.wMonth, stTime.wDay);
	fopen_s(&fpData, strText, "w");
	fprintf(fpData, "Force verification logging \n Year: %d, Month: %d, Date: %d \n", stTime.wYear, stTime.wMonth, stTime.wDay);
	fprintf(fpData, "Force calibration result: Ka = %10.7f, Kf = %10.7f, I0 = %10.7f, ForceRatio = %10.7f \n", 
		stForceCalibrationOutput.fPosnFactor_Ka, stForceCalibrationOutput.fForceFactor_Kf, stForceCalibrationOutput.fForceOffset_I0,
		mtn_get_force_ratio());  // 20130206
	unsigned int ii, pp, ff;
	for(pp = 0; pp<nCountLoopCheckPositionForce; pp++) // 20121129
	{
		fprintf(fpData, "%%%%%%%%  Contact Level No.%d %%%%%%%%\n", pp + 1);
		//sprintf_s(strText, 128, "Adjust %d / %d contact level", pp+1, nCountLoopCheckPositionForce);
		//int iRet = AfxMessageBox(strText, MB_OKCANCEL);
		//if(iRet == IDCANCEL) break;	
		for(ff = 0; ff < nTotalGramPerContactLevel; ff ++)
		{
			dSumForceReadBack_gram = 0;
			stLogForceVerifyInput.dForceVerifyDesiredGram = floor(dDeltaForce * ff + dForceVerifyFirstGram + 0.5); // 20130103
			for(ii =0; ii< uiLoopContactForceTest; ii++) // nCountLoopCheckPositionForce
			{
				if(ff == 0 && ii == 0) // Only teach for the first time after adjusting contact level
				{
					stLogForceVerifyInput.iFlagTeachContact = 1;
				}
				else
				{
					stLogForceVerifyInput.iFlagTeachContact = 0;
					stLogForceVerifyInput.iContactPosn = stForceVerifyOutput.iContactPosnReg;
				}
				Sleep(10);
				mtn_force_verify(m_hHandle, &stLogForceVerifyInput, &stForceVerifyOutput);

				cstrTemp.Format("%d/%d, %d/%d ->CP: %d, Desire(g): %6.2f, Read(g): %6.2f", 
					pp, nCountLoopCheckPositionForce, ii, uiLoopContactForceTest, // 20130107
					stForceVerifyOutput.iContactPosnReg, stLogForceVerifyInput.dForceVerifyDesiredGram,
					stForceVerifyOutput.dForceVerifyReadBack);
				GetDlgItem(IDC_SHOW_FORCE_VERIFY_READBACK)->SetWindowTextA(cstrTemp);

				dSumForceReadBack_gram = dSumForceReadBack_gram + stForceVerifyOutput.dForceVerifyReadBack;
				// Logging
				if(ii == 0)
				{
					fprintf(fpData, "Desired Force(gram): %6.2f, ContactPosition: %d \nReadback = [ %6.2f ",
						stLogForceVerifyInput.dForceVerifyDesiredGram, stForceVerifyOutput.iContactPosnReg, stForceVerifyOutput.dForceVerifyReadBack);
				}
				else
				{
					fprintf(fpData, ", %6.2f ",	stForceVerifyOutput.dForceVerifyReadBack);
				}
			}
			fprintf(fpData, "]\n Mean = %6.2f\n",	dSumForceReadBack_gram/uiLoopContactForceTest); // 
			fprintf(fpData, "\n");
			Sleep(20);
		}
		fprintf(fpData, "\n");
	}
	fclose(fpData);

	cstrTemp.Format("File save to %s", strText); // 20130107
	GetDlgItem(IDC_STATIC_POSN_TUNE_DLG_B1W_OBJ)->SetWindowTextA(cstrTemp); // 20130107
}

// IDC_FORCE_VERIFY_TRIALS_PER_LOOP
void CMtnPosnCompensDlg::OnEnKillfocusForceVerifyTrialsPerLoop()
{
	GetDlgItem(IDC_FORCE_VERIFY_TRIALS_PER_LOOP)->GetWindowTextA(strTextTemp, 32);
	sscanf_s(strTextTemp, "%d", &uiLoopContactForceTest);
}

// IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG
void CMtnPosnCompensDlg::OnBnClickedCheckForceVerifyByGramJig()
{
	cFlagForceVerifyByGramGauge = ((CButton *)GetDlgItem(IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG))->GetCheck();

	if(cFlagForceVerifyByGramGauge == 0)
	{
		acs_stop_buffer_prog_srch_contact_f_cali_auto_tune_z(); // 20130107
		acs_compile_buffer_prog_srch_contact_f_cali_auto_tune_z();
		Sleep(100);
		mtn_acs_convert_from_current_to_position_loop(m_hHandle, ACSC_AXIS_A); // 20130107
		Sleep(500);
		acs_run_buffer_prog_srch_contact_f_cali_auto_tune_z();

		CString cstrTemp;
		cstrTemp.Format("By GramJig");
		GetDlgItem(IDC_CHECK_FORCE_VERIFY_BY_GRAM_JIG)->SetWindowTextA(cstrTemp);

//		UI_SetEnableButton(TRUE); // 20130102
		cFlagDoingForceVerifyByGramGauge = 0;

	}
}
// IDC_EDIT_FORCE_CALI_DELTA_DCOM
void CMtnPosnCompensDlg::OnEnKillfocusEditForceCaliDeltaDcom()
{
	ReadDoubleFromEdit(IDC_EDIT_FORCE_CALI_DELTA_DCOM, &stForceCalibrationInput.stForceCaliPara.dStepDCOM);

}
