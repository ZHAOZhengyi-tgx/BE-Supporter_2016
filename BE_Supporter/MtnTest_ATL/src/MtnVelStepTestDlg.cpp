// MtnVelStepTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MtnVelStepTestDlg.h"

#include "MtnApi.h"
#include "AftVelLoopTest.h"

AFT_GROUP_VEL_STEP_TEST_CONFIG stVelStepGroupTestConfig;
AFT_GROUP_VEL_STEP_TEST_INPUT stVelStepGroupTestCase[MAX_NUM_CASE_VEL_STEP_GROUP_TEST];
AFT_VEL_LOOP_TEST_OUTPUT astVelLoopTestOutput[MAX_NUM_CASE_VEL_STEP_GROUP_TEST];
AFT_VEL_LOOP_TEST_INPUT astVelLoopTestInput[MAX_NUM_CASE_VEL_STEP_GROUP_TEST];

static unsigned int iCaseVelStepGroupTest;
static unsigned int uiTotalTestCasesVelStepGroup;
static int iFlagEnableEditVelStepGroupDlg = FALSE;
static int iFlagTeachLimitBeforeTest = 1; // 20121215
static char cFlagSaveWaveform = 1;
static char cFlagPlotChart = 0;
// CMtnVelStepTestDlg dialog
extern COMM_SETTINGS stServoControllerCommSet;
static HANDLE stCommHandleACS;	// communication handle

static char cFlagTuningVelLoopByStepResponse;  // 20120801

IMPLEMENT_DYNAMIC(CMtnVelStepTestDlg, CDialog)

CMtnVelStepTestDlg::CMtnVelStepTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMtnVelStepTestDlg::IDD, pParent)
{

}

CMtnVelStepTestDlg::~CMtnVelStepTestDlg()
{
}

#include "MtnTesterResDef.h"
static int iFlagInitOnce = 0;
BOOL CMtnVelStepTestDlg::OnInitDialog()
{
	// Init ACS communication
	stCommHandleACS = stServoControllerCommSet.Handle;

	SetUserInterfaceLanguage(get_sys_language_option());

	if(iFlagInitOnce == 0)
	{
		iFlagInitOnce = 1;
		InitVelStepGroupTestCfg();
	}

	InitVelStepGroupTestConfigUI();

	((CButton*)GetDlgItem(IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST))->SetCheck(cFlagSaveWaveform);
	((CButton*)GetDlgItem(IDC_CHECK_FLAG_PLOT_CHART_VEL_STEP_GROUP_TEST))->SetCheck(cFlagPlotChart);

	//StartTimer(IDT_VEL_STEP_GROUP_TEST_DLG_TIMER, 200);
	return CDialog::OnInitDialog();
}

static int iFlagRunningTimer = FALSE;
int CMtnVelStepTestDlg::DlgGetFlagTimerRunning()
{
	return iFlagRunningTimer;
}
void CMtnVelStepTestDlg::PauseTimer()
{
	iFlagRunningTimer = FALSE;
}
void CMtnVelStepTestDlg::RestartTimer()
{
	iFlagRunningTimer = TRUE;
}

// IDC_CHECK_TUNING_VEL_LOOP_BY_STEP_RESPONSE
void CMtnVelStepTestDlg::OnBnClickedCheckTuningVelLoopByStepResponse()
{
	cFlagTuningVelLoopByStepResponse = ((CButton *)GetDlgItem(IDC_CHECK_TUNING_VEL_LOOP_BY_STEP_RESPONSE))->GetCheck();
}

BOOL CMtnVelStepTestDlg::ShowWindow(int nCmdShow)
{
	iFlagRunningTimer = TRUE;
	InitVelStepGroupTestConfigUI();
	return CDialog::ShowWindow(nCmdShow);
}

void CMtnVelStepTestDlg::DlgTimerEvent()
{
	CString cstrTemp;
	if(cFlagDlgVelStepGroputTestThreadRunning == TRUE 
		&& iFlagRunningTimer == TRUE
		&& cFlagTuningVelLoopByStepResponse == 0)
	{
		cstrTemp.Format("Curr: %d/ Total %d", iCaseVelStepGroupTest, uiTotalTestCasesVelStepGroup);
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TEST_STATUS)->SetWindowTextA(cstrTemp);
		cstrTemp.Format("Posn: [%d, %d, %d]", stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[0], stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[1], stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[2]);
		GetDlgItem(IDC_GROUP_VEL_STEP_ROUGH_TIME)->SetWindowTextA(cstrTemp);
	}
}
// IDC_STATIC_GROUP_VEL_STEP_TEST_STATUS
// IDC_GROUP_VEL_STEP_ROUGH_TIME
void CMtnVelStepTestDlg::OnTimer(UINT nTimerVal)
{
	// DlgTimerEvent();
}

static UINT_PTR m_iTimerVal;
UINT_PTR  CMtnVelStepTestDlg::StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration)
{ // IDT_SPEC_TEST_DLG_TIMER
	m_iTimerVal = SetTimer(nIDT_TimerResMacro, TimerDuration, 0);
	
	if (m_iTimerVal == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}
    return m_iTimerVal;
} 

void CMtnVelStepTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMtnVelStepTestDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis1)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis2)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis3)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis4)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis1)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis2)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis3)
	ON_EN_CHANGE(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis4)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis1)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis2)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis3)
	ON_EN_CHANGE(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis4)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis1)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis2)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis3)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis4)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis1)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis2)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis3)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis4)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS1_GROUP_VEL_STEP, &CMtnVelStepTestDlg::OnCbnSelchangeComboAxis1GroupVelStep)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS2_GROUP_VEL_STEP, &CMtnVelStepTestDlg::OnCbnSelchangeComboAxis2GroupVelStep)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS3_GROUP_VEL_STEP, &CMtnVelStepTestDlg::OnCbnSelchangeComboAxis3GroupVelStep)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS4_GROUP_VEL_STEP, &CMtnVelStepTestDlg::OnCbnSelchangeComboAxis4GroupVelStep)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis1)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis2)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis3)
	ON_BN_CLICKED(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis4)
	ON_BN_CLICKED(IDC_BUTTON_START_GROUP_VEL_STEP, &CMtnVelStepTestDlg::OnBnClickedButtonStartGroupVelStep)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis1)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis2)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis3)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis4)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis4)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis3)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis2)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis1)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS1, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis1)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS2, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis2)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS3, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis3)
	ON_EN_CHANGE(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS4, &CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis4)
	ON_BN_CLICKED(IDC_BUTTON_STOP_VEL_STEP_TEST, &CMtnVelStepTestDlg::OnBnClickedButtonStopVelStepTest)
	ON_BN_CLICKED(IDOK, &CMtnVelStepTestDlg::OnBnClickedOk)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST, &CMtnVelStepTestDlg::OnBnClickedCheckFlagSaveChartVelStepGroupTest)
	ON_BN_CLICKED(IDC_CHECK_FLAG_PLOT_CHART_VEL_STEP_GROUP_TEST, &CMtnVelStepTestDlg::OnBnClickedCheckFlagPlotChartVelStepGroupTest)
	ON_BN_CLICKED(IDC_CHECK_VEL_STEP_DLG_ENABLE_EDIT, &CMtnVelStepTestDlg::OnBnClickedCheckVelStepDlgEnableEdit)
	ON_BN_CLICKED(IDC_CHECK_TUNING_VEL_LOOP_BY_STEP_RESPONSE, &CMtnVelStepTestDlg::OnBnClickedCheckTuningVelLoopByStepResponse)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis4)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis4)
	ON_EN_KILLFOCUS(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis4)
	ON_EN_KILLFOCUS(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis4)
	ON_EN_KILLFOCUS(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1, &CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2, &CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis2)
	ON_EN_KILLFOCUS(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3, &CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis3)
	ON_EN_KILLFOCUS(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4, &CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis4)
	ON_BN_CLICKED(IDC_CHECK_VEL_STEP_DLG_TEACH_LIMIT, &CMtnVelStepTestDlg::OnBnClickedCheckVelStepDlgTeachLimit)
END_MESSAGE_MAP()

void CMtnVelStepTestDlg::InitVelStepGroupTestConfigEditOneAxis(int nResId, int iValue)
{
	static char tempChar[32];
	sprintf_s(tempChar, 32, "%d", iValue);
	GetDlgItem(nResId)->SetWindowTextA(_T(tempChar));
}

void CMtnVelStepTestDlg::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}
void CMtnVelStepTestDlg::ReadUnsignIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

static char *strStopString;
void CMtnVelStepTestDlg::ReadDoubleFromEdit(int nResId, double *dValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*dValue = strtod(tempChar, &strStopString);
//	sscanf_s(tempChar, "%f", dValue);
}

void CMtnVelStepTestDlg::VelStepGroupTestUpdateDoubleToEdit(int nResId, double dValue, const char * strFormat)
{
//	static char tempChar[32];
//	sprintf_s(tempChar, 32, strFormat, dValue);
//	GetDlgItem(nResId)->SetWindowTextA(_T(tempChar));
	CString cstrTemp;
	cstrTemp.Format(strFormat, dValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp);
}

void CMtnVelStepTestDlg::InitVelStepGroupTestConfigUI()
{
	((CButton *)GetDlgItem(IDC_CHECK_TUNING_VEL_LOOP_BY_STEP_RESPONSE))->SetCheck(cFlagTuningVelLoopByStepResponse); // 20120801

	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1, stVelStepGroupTestConfig.uiNumPoints[0]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2, stVelStepGroupTestConfig.uiNumPoints[1]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3, stVelStepGroupTestConfig.uiNumPoints[2]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4, stVelStepGroupTestConfig.uiNumPoints[3]);

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1, stVelStepGroupTestConfig.dLowerLimitPosition_mm[0], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2, stVelStepGroupTestConfig.dLowerLimitPosition_mm[1], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3, stVelStepGroupTestConfig.dLowerLimitPosition_mm[2], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4, stVelStepGroupTestConfig.dLowerLimitPosition_mm[3], "%6.2f");

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1, stVelStepGroupTestConfig.dUpperLimitPosition_mm[0], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2, stVelStepGroupTestConfig.dUpperLimitPosition_mm[1], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3, stVelStepGroupTestConfig.dUpperLimitPosition_mm[2], "%6.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4, stVelStepGroupTestConfig.dUpperLimitPosition_mm[3], "%6.2f");

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1, stVelStepGroupTestConfig.fVelStepTestDistance_mm[0], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2, stVelStepGroupTestConfig.fVelStepTestDistance_mm[1], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3, stVelStepGroupTestConfig.fVelStepTestDistance_mm[2], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4, stVelStepGroupTestConfig.fVelStepTestDistance_mm[3], "%8.1f");

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, stVelStepGroupTestConfig.fMaxVelocityPercent[0]*100, "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, stVelStepGroupTestConfig.fMaxVelocityPercent[1]*100, "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, stVelStepGroupTestConfig.fMaxVelocityPercent[2]*100, "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4, stVelStepGroupTestConfig.fMaxVelocityPercent[3]*100, "%8.1f");

	InitVelStepGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS1_GROUP_VEL_STEP, stVelStepGroupTestConfig.uiAxis_CtrlCardId[0]);
	InitVelStepGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS2_GROUP_VEL_STEP, stVelStepGroupTestConfig.uiAxis_CtrlCardId[1]);
	InitVelStepGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS3_GROUP_VEL_STEP, stVelStepGroupTestConfig.uiAxis_CtrlCardId[2]);
	InitVelStepGroupTestConfigCombo_OneAxis(IDC_COMBO_AXIS4_GROUP_VEL_STEP, stVelStepGroupTestConfig.uiAxis_CtrlCardId[3]);

	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS1))->SetCheck(stVelStepGroupTestConfig.ucFlagIsExciteAxis[0]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS2))->SetCheck(stVelStepGroupTestConfig.ucFlagIsExciteAxis[1]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS3))->SetCheck(stVelStepGroupTestConfig.ucFlagIsExciteAxis[2]);
	((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS4))->SetCheck(stVelStepGroupTestConfig.ucFlagIsExciteAxis[3]);

	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS1, stVelStepGroupTestConfig.uiLoopNum[0]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS2, stVelStepGroupTestConfig.uiLoopNum[1]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS3, stVelStepGroupTestConfig.uiLoopNum[2]);
	InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS4, stVelStepGroupTestConfig.uiLoopNum[3]);

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS1, stVelStepGroupTestConfig.fPercentErrorThresholdSettleTime[0]*100, "%8.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS2, stVelStepGroupTestConfig.fPercentErrorThresholdSettleTime[1]*100, "%8.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS3, stVelStepGroupTestConfig.fPercentErrorThresholdSettleTime[2]*100, "%8.2f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS4, stVelStepGroupTestConfig.fPercentErrorThresholdSettleTime[3]*100, "%8.2f");

	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS1, stVelStepGroupTestConfig.fEncResolutionCntPerMM[0], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS2, stVelStepGroupTestConfig.fEncResolutionCntPerMM[1], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS3, stVelStepGroupTestConfig.fEncResolutionCntPerMM[2], "%8.1f");
	VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS4, stVelStepGroupTestConfig.fEncResolutionCntPerMM[3], "%8.1f");

	((CButton *) GetDlgItem(IDC_CHECK_VEL_STEP_DLG_ENABLE_EDIT))->SetCheck(iFlagEnableEditVelStepGroupDlg);
	((CButton *)GetDlgItem(IDC_CHECK_VEL_STEP_DLG_TEACH_LIMIT))->SetCheck(iFlagTeachLimitBeforeTest);
	UI_EnableEditTestCondition(iFlagEnableEditVelStepGroupDlg);
}

void CMtnVelStepTestDlg::InitVelStepGroupTestConfigCombo_OneAxis(int nResId, int iAxisCtrlCard)
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
	pSelectChannelCombo->SetCurSel(iAxisCtrlCard);
}

#include "MotAlgo_DLL.h"
// CMtnVelStepTestDlg message handlers
void CMtnVelStepTestDlg::InitVelStepGroupTestCfg()
{
	MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;  // 20110722
	mtn_api_get_spring_compensation_sp_para(stServoControllerCommSet.Handle, &stOutputPosnCompensationTune);
	aft_vel_loop_set_bond_head_relax_position((int)(stOutputPosnCompensationTune.iEncoderOffsetSP/1000));  // 20110722

	unsigned int ii, jj, uiTotalNumCase_VelStepTest;
	int iFlagMachType = get_sys_machine_type_flag();
	if(iFlagMachType == WB_MACH_TYPE_VLED_FORK)
	{
		aft_vel_loop_test_init_default_cfg(&stVelStepGroupTestConfig);
	}
	else if(iFlagMachType == WB_MACH_TYPE_HORI_LED 
		|| iFlagMachType == BE_WB_HORI_20T_LED ) // 20120826)
	{
		aft_vel_loop_test_hori_bonder_init_default_cfg(&stVelStepGroupTestConfig);
	}
	else if(iFlagMachType == WB_STATION_XY_VERTICAL)
	{
		aft_vel_loop_test_station_xy_init_VLED_cfg(&stVelStepGroupTestConfig);
	}
	else if(iFlagMachType == WB_STATION_XY_TOP)
	{
		aft_vel_loop_test_station_xy_init_HoriLED_cfg(&stVelStepGroupTestConfig);
	}
	else if(iFlagMachType == WB_STATION_BH)
	{
		aft_vel_loop_test_station_bh_init_default_cfg(&stVelStepGroupTestConfig);
	}
	else if(iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
	{  // aft_vel_loop_test_wb_18V_init_default_cfg
		aft_vel_loop_test_wb_18V_init_default_cfg(&stVelStepGroupTestConfig);
	}
	else
	{
		aft_vel_loop_test_13v_init_default_cfg(&stVelStepGroupTestConfig);
	}
	aft_convert_group_vel_step_cfg_to_test_input(&stVelStepGroupTestConfig, 
											  &stVelStepGroupTestCase[0],
											  &uiTotalNumCase_VelStepTest);
FILE *fpDataVelStepTestCases;
static	char strFilename[128];

	if(iFlagEnableEditVelStepGroupDlg == TRUE)  // 20120510
	{
		sprintf_s(strFilename, 128, "VelStepTestCases.txt");
		fopen_s(&fpDataVelStepTestCases, strFilename, "w");  // FILENAME_TRAJ_TEMP_DATA

		if(fpDataVelStepTestCases != NULL)
		{
			for(ii = 0; ii<uiTotalNumCase_VelStepTest; ii++)
			{
				fprintf(fpDataVelStepTestCases, "%%%% Case - %d\n", ii);
				fprintf(fpDataVelStepTestCases, "aAxisId = [");
				for(jj = 0; jj < stVelStepGroupTestCase[ii].usActualNumAxis - 1; jj ++)
				{
					fprintf(fpDataVelStepTestCases, "%d, ", stVelStepGroupTestCase[ii].uiAxis_CtrlCardId[jj]);
				}
				fprintf(fpDataVelStepTestCases, "%d]; \n", stVelStepGroupTestCase[ii].uiAxis_CtrlCardId[jj]);

				fprintf(fpDataVelStepTestCases, "aAxisPosn = [");
				for(jj = 0; jj < stVelStepGroupTestCase[ii].usActualNumAxis - 1; jj ++)
				{
					fprintf(fpDataVelStepTestCases, "%d, ", stVelStepGroupTestCase[ii].iAxisPosn[jj]);
				}
				fprintf(fpDataVelStepTestCases, "%d]; \n", stVelStepGroupTestCase[ii].iAxisPosn[jj]);
				fprintf(fpDataVelStepTestCases, "iExciteAxis = %d; \n", stVelStepGroupTestCase[ii].stVelStepTestOnceInput.iAxisCtrlCard);
				fprintf(fpDataVelStepTestCases, "VelTestPosnRange = [%d, %d]; \n\n", stVelStepGroupTestCase[ii].stVelStepTestOnceInput.iPositionLower, stVelStepGroupTestCase[ii].stVelStepTestOnceInput.iPositionUpper);


			}
			fclose(fpDataVelStepTestCases);
		}
	}  // 20120510
}
#define MAX_NUM_POINT_VEL_STEP_GROUP_TEST_X_TABLE			10
#define MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Y_TABLE			10
#define MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Z_BOND			5
#define MAX_NUM_POINT_VEL_STEP_GROUP_TEST_DEF_AXIS			3

// IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis1()
{
}
// IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis2()
{
}
// IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis3()
{
}
// IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditNumPointsGroupVelStepAxis4()
{
}
// IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis1()
{
}
// IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis2()
{
}
// IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis3()
{
}
// IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditLowLimitGroupVelStepAxis4()
{
}
// IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis1()
{
}
// IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis2()
{
}
// IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis3()
{
}
// IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditUppLimitGroupVelStepAxis4()
{
}
// IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis1()
{
}
// IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis2()
{
}
// IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis3()
{
}
// IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditVelStepDistGroupVelStepAxis4()
{
}

#define UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE			40
#define LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE			2
#define UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_BONDHEAD		40
#define LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_BONDHEAD		1

// IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis1()
{
}
// IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis2()
{
}
// IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis3()
{
}
// IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditVelStepMaxVelGroupAxis4()
{
}
// IDC_COMBO_AXIS1_GROUP_VEL_STEP
void CMtnVelStepTestDlg::OnCbnSelchangeComboAxis1GroupVelStep()
{
	stVelStepGroupTestConfig.uiAxis_CtrlCardId[0] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS1_GROUP_VEL_STEP))->GetCurSel();
}
// IDC_COMBO_AXIS2_GROUP_VEL_STEP
void CMtnVelStepTestDlg::OnCbnSelchangeComboAxis2GroupVelStep()
{
	stVelStepGroupTestConfig.uiAxis_CtrlCardId[1] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS2_GROUP_VEL_STEP))->GetCurSel();
}
// IDC_COMBO_AXIS3_GROUP_VEL_STEP
void CMtnVelStepTestDlg::OnCbnSelchangeComboAxis3GroupVelStep()
{
	stVelStepGroupTestConfig.uiAxis_CtrlCardId[2] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS3_GROUP_VEL_STEP))->GetCurSel();
}
// IDC_COMBO_AXIS4_GROUP_VEL_STEP
void CMtnVelStepTestDlg::OnCbnSelchangeComboAxis4GroupVelStep()
{
	stVelStepGroupTestConfig.uiAxis_CtrlCardId[3] = ((CComboBox*) GetDlgItem(IDC_COMBO_AXIS4_GROUP_VEL_STEP))->GetCurSel();
}
// IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS1
void CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis1()
{
	stVelStepGroupTestConfig.ucFlagIsExciteAxis[0] = ((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS1))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS2
void CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis2()
{
	stVelStepGroupTestConfig.ucFlagIsExciteAxis[1] = ((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS2))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS3
void CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis3()
{
	stVelStepGroupTestConfig.ucFlagIsExciteAxis[2] = ((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS3))->GetCheck();
}
// IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS4
void CMtnVelStepTestDlg::OnBnClickedCheckExciteFlagVelStepAxis4()
{
	stVelStepGroupTestConfig.ucFlagIsExciteAxis[3] = ((CButton*) GetDlgItem(IDC_CHECK_EXCITE_FLAG_VEL_STEP_AXIS4))->GetCheck();
}

#include "MtnSpectrumTestDlg.h"
extern CMtnSpectrumTestDlg *cpTestSpectrum;
extern void mtn_update_burn_in_position_limit_wb_axis(int iWbAxis, int iLowerLimit, int iUpperLimit); // 20130208

// IDC_BUTTON_START_GROUP_VEL_STEP
void CMtnVelStepTestDlg::OnBnClickedButtonStartGroupVelStep()
{
	if(iFlagTeachLimitBeforeTest == TRUE)
	{
	// 20121003
	mtn_wb_bakup_software_limit(stCommHandleACS, 0, 1, 4);
	mtn_wb_enlarge_software_limit_by_(stCommHandleACS, 0, 1, 4, 10.0);

		double dUppLimitPosn, dLowLimitPosn;
		for(int ii = 0; ii< 4; ii++)
		{
			int iAxisOnACS = stVelStepGroupTestConfig.uiAxis_CtrlCardId[ii];
			if(iAxisOnACS !=MAX_CTRL_AXIS_PER_SERVO_BOARD && stVelStepGroupTestConfig.ucFlagIsExciteAxis[ii])
			{
				if(aft_teach_upper_limit(stCommHandleACS, iAxisOnACS, iFlagEnableEditVelStepGroupDlg, &dUppLimitPosn) == MTN_API_OK_ZERO)
				{
					stVelStepGroupTestConfig.dUpperLimitPosition_mm[ii] = dUppLimitPosn/stVelStepGroupTestConfig.fEncResolutionCntPerMM[ii] - 1.0;
				}
				else
				{
					return;
				}

				if(aft_teach_lower_limit(stCommHandleACS, iAxisOnACS, iFlagEnableEditVelStepGroupDlg, &dLowLimitPosn) == MTN_API_OK_ZERO)
				{
					stVelStepGroupTestConfig.dLowerLimitPosition_mm[ii] = dLowLimitPosn/stVelStepGroupTestConfig.fEncResolutionCntPerMM[ii] + 1.0;
				}
				else
				{
					return;
				}
				acsc_ToPoint(stCommHandleACS, 0, // start up the motion immediately
						iAxisOnACS, (dUppLimitPosn + dLowLimitPosn)/2.0, NULL);
				CString cstrTemp;
				cstrTemp.Format("Upper Limit: %8.1f, Lower Limit: %8.1f", dUppLimitPosn, dLowLimitPosn);
				mtn_update_burn_in_position_limit_wb_axis(ii, (int)dLowLimitPosn, (int)dUppLimitPosn); // 20130208

				AfxMessageBox(cstrTemp);
			}
		}
	mtn_wb_restore_software_limit(stCommHandleACS, 0, 1, 4);  // 20121003

		iFlagTeachLimitBeforeTest = 0; // 20121215
		((CButton *)GetDlgItem(IDC_CHECK_VEL_STEP_DLG_TEACH_LIMIT))->SetCheck(iFlagTeachLimitBeforeTest);
	}
	aft_convert_group_vel_step_cfg_to_test_input(&stVelStepGroupTestConfig, 
											  &stVelStepGroupTestCase[0],
											  &uiTotalTestCasesVelStepGroup);


//	RestartTimer();
	StartVelStepGroupTestThread();
	mtn_dll_music_logo_normal_start();  // 20120217   // Logo Normal Start 1-2-3, 20120217
	if(cpTestSpectrum->UI_GetShowWindowFlag() == TRUE)
	{
		cpTestSpectrum->OnCancel();
	}
}
// IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis1()
{
}
// IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis2()
{
}
// IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis3()
{
}

// IDC_EDIT_VEL_STEP_GROUP_LOOP_NUM_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupLoopNumAxis4()
{
}
// IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis4()
{
}
// IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis3()
{
}
// IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis2()
{
}
// IDC_EDIT_VEL_STEP_GROUP_TOL_PC_SETTLE_TIME_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupTolPcSettleTimeAxis1()
{
}
// IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS1
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis1()
{
}
// IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS2
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis2()
{
}
// IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS3
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis3()
{
}
// IDC_EDIT_VEL_STEP_GROUP_ENC_PER_MM_AXIS4
void CMtnVelStepTestDlg::OnEnChangeEditVelStepGroupEncPerMmAxis4()
{
}

UINT VelStepGroupTestThreadInDialog( LPVOID pParam )
{
    CMtnVelStepTestDlg* pObject = (CMtnVelStepTestDlg *)pParam;

	if(cFlagTuningVelLoopByStepResponse == 1)
	{
		return pObject->VelStepTestTuningThread();
	}
	else
	{
		return pObject->VelStepGroupTestThread(); 	
	}
}

void CMtnVelStepTestDlg::StartVelStepGroupTestThread()
{
	cFlagDlgVelStepGroputTestThreadRunning = TRUE;
	pDlgVelStepGroupTestWinThread = AfxBeginThread(VelStepGroupTestThreadInDialog, this); // , THREAD_PRIORITY_TIME_CRITICAL);
	pDlgVelStepGroupTestWinThread->m_bAutoDelete = FALSE;
}
#include "math.h"
#include "MtnAft.h"
#include "MtnInitAcs.h"
void CMtnVelStepTestDlg::DlgVelStepTuningPerAxis(AFT_VEL_LOOP_TEST_INPUT *stpVelLoopTuningInput)
{
// stpVelLoopTuningInput
int iCurrentAxis_Ctrl = stpVelLoopTuningInput->iAxisCtrlCard; // stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj];
static AFT_VEL_LOOP_TEST_OUTPUT stVelLoopTestOutput;

		// Display // IDC_GROUP_VEL_STEP_ROUGH_TIME

		int iFlagExitTuning = 0, nCurrentRoundTuning = 0;
		CTRL_PARA_ACS stCurrentCtrlACS;
		// stVelLoopTuningInput.fMaxAbsVel = 
		// Set VKP, VKI,


#define __MAX_CTRL_OUT_16b__           32766
#define __MAX_FACTOR_VKP_INCREMENT__   (1.08)
#define __MIN_FACTOR_VKP_INCREMENT__   (1.01)
#define __MAX_NUM_ACS_AXIS_TUNING__    4

		while(iFlagExitTuning == 0 && nCurrentRoundTuning <= 100
			&& cFlagDlgVelStepGroputTestThreadRunning == TRUE)
		{
			// Start Excitation
			mtn_aft_vel_loop_test(stCommHandleACS, stpVelLoopTuningInput, &stVelLoopTestOutput);
			// Check DAC, VelSettleTime, VelOS, AccSettleTime
			int iMaxDrvCmd = 32000;
			double dAccSettleTime = 0, dVelOvershoot_Plus=0, dVelOvershoot_Minus, dMaxAbsOS, dMinAbsOS;
			double dRiseTime = 0;
			double fSamplePositivePeakDrvCmd = stVelLoopTestOutput.fSamplePositivePeakDrvCmd;
			double fSampleNegativePeakDrvCmd = stVelLoopTestOutput.fSampleNegativePeakDrvCmd;

			iMaxDrvCmd = (int)fabs(stVelLoopTestOutput.fMaxDriveCmd);
			dVelOvershoot_Plus = stVelLoopTestOutput.fPositiveStepPercentOS;
			dVelOvershoot_Minus = stVelLoopTestOutput.fNegativeStepPercentOS;
			if(fabs(dVelOvershoot_Plus) > fabs(dVelOvershoot_Minus))
			{
				dMaxAbsOS = fabs(dVelOvershoot_Plus);
				dMinAbsOS = fabs(dVelOvershoot_Minus);
			}
			else
			{
				dMinAbsOS = fabs(dVelOvershoot_Plus);
				dMaxAbsOS = fabs(dVelOvershoot_Minus);
			}
			dRiseTime = (stVelLoopTestOutput.fNegativeStepRiseTime + stVelLoopTestOutput.fPositiveStepRiseTime)/2.0;

			double dDeltaVelByAcc = dRiseTime * stVelLoopTestOutput.fAveMaxAccEstimateFullDAC_mpss * 1E-3;
			double dVelStep = stpVelLoopTuningInput->fMaxAbsVel * 2.0/ stpVelLoopTuningInput->fEncResolutionCntPerMM / 1000;
			double dSettleTime = (stVelLoopTestOutput.fNegativeStepSettleTime + stVelLoopTestOutput.fPositiveStepSettleTime)/2.0;
			double fPositiveReturnTime = stVelLoopTestOutput.fPositiveReturnTime;
			double fNegativeReturnTime = stVelLoopTestOutput.fNegativeReturnTime;
			double fReturnTime = (fPositiveReturnTime + fNegativeReturnTime)/2.0;

			// Display
			CString cstrTemp;
			cstrTemp.Format("Ax:%d, Mdrv:%d@%d, R.T:%3.1f, RtnT:%3.1f, S.T.:%3.1f, V: %4.2f, %4.2f, S.T: %5.1f, O.S: %4.2f, %4.2f,", 
				stpVelLoopTuningInput->iAxisCtrlCard, iMaxDrvCmd, (int)((fSamplePositivePeakDrvCmd + fSampleNegativePeakDrvCmd)/2.0),
				dRiseTime, fReturnTime, dSettleTime,
				dDeltaVelByAcc, dVelStep, dAccSettleTime, dVelOvershoot_Plus, dVelOvershoot_Minus);
			GetDlgItem(IDC_GROUP_VEL_STEP_ROUGH_TIME)->SetWindowTextA(cstrTemp);

			// Decision whether to exit, calculate VKP VKI increment or exit
			mtnapi_upload_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, 	&stCurrentCtrlACS);
			cstrTemp.Format("VKP: %4.1f, KI:%4.1f", stCurrentCtrlACS.dVelocityLoopProportionalGain,
				stCurrentCtrlACS.dVelocityLoopIntegratorGain);
			GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TEST_STATUS)->SetWindowTextA(cstrTemp);

			double dFactorIncrementVKP = 1.0;
			if(iMaxDrvCmd < __MAX_CTRL_OUT_16b__ ||
				((fSamplePositivePeakDrvCmd + fSampleNegativePeakDrvCmd) < (dRiseTime * 1.2) )
				&& (stpVelLoopTuningInput->iAxisCtrlCard == 0 || stpVelLoopTuningInput->iAxisCtrlCard == 1) // Only for XY in WB
				)
			{
				dFactorIncrementVKP = (double)__MAX_CTRL_OUT_16b__/iMaxDrvCmd;
				if(dFactorIncrementVKP > __MAX_FACTOR_VKP_INCREMENT__) dFactorIncrementVKP = __MAX_FACTOR_VKP_INCREMENT__;
				if(dFactorIncrementVKP < __MIN_FACTOR_VKP_INCREMENT__) dFactorIncrementVKP = __MIN_FACTOR_VKP_INCREMENT__;

				stCurrentCtrlACS.dVelocityLoopProportionalGain = 
					stCurrentCtrlACS.dVelocityLoopProportionalGain * dFactorIncrementVKP;
				mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			}
			//else if(dMaxAbsOS > 0.08)
			//{
			//	stCurrentCtrlACS.dVelocityLoopIntegratorGain = stCurrentCtrlACS.dVelocityLoopIntegratorGain / 1.05;
			//	mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			//}
			else if(dMinAbsOS < 0.01)
			{
				stCurrentCtrlACS.dVelocityLoopIntegratorGain = stCurrentCtrlACS.dVelocityLoopIntegratorGain * 1.05;
				mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			}
			else if(fReturnTime > dRiseTime)
			{
				stCurrentCtrlACS.dVelocityLoopIntegratorGain = stCurrentCtrlACS.dVelocityLoopIntegratorGain * 1.05;
				mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			}
			else if(dSettleTime > 1.5 * fReturnTime)
			{
				stCurrentCtrlACS.dVelocityLoopProportionalGain = 
					stCurrentCtrlACS.dVelocityLoopProportionalGain / 1.02;
				mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			}
			//else if(dSettleTime < 1.2 * fReturnTime)
			//{
			//	stCurrentCtrlACS.dVelocityLoopProportionalGain = 
			//		stCurrentCtrlACS.dVelocityLoopProportionalGain * 1.02;
			//	mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			//}
			else if(dDeltaVelByAcc > dVelStep * 1.6)
			{
				dFactorIncrementVKP = dDeltaVelByAcc / dVelStep / 1.6;
				if(dFactorIncrementVKP > __MAX_FACTOR_VKP_INCREMENT__) dFactorIncrementVKP = __MAX_FACTOR_VKP_INCREMENT__;
				if(stCurrentCtrlACS.dVelocityLoopProportionalGain * dFactorIncrementVKP - 
					stCurrentCtrlACS.dVelocityLoopProportionalGain < 1.0)
				{
					stCurrentCtrlACS.dVelocityLoopProportionalGain = stCurrentCtrlACS.dVelocityLoopProportionalGain + 1;
				}
				else
				{
					stCurrentCtrlACS.dVelocityLoopProportionalGain = stCurrentCtrlACS.dVelocityLoopProportionalGain * dFactorIncrementVKP;
				}
				
				mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iCurrentAxis_Ctrl, &stCurrentCtrlACS);
			}
			else
			{
				iFlagExitTuning = 1;
			}
			nCurrentRoundTuning ++;
		}
}

extern char strVeLoopTestNewFolderName[128];

UINT CMtnVelStepTestDlg::VelStepTestTuningThread()
{
unsigned int jj;
static double dMaxVel;
AFT_VEL_LOOP_TEST_INPUT stVelLoopTuningInput;
static AFT_VEL_LOOP_TEST_OUTPUT stVelLoopTestOutput;

	// Start tuning 
	GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->EnableWindow(FALSE);

	// Move group axes to desired position
	for(jj =0; jj<__MAX_NUM_ACS_AXIS_TUNING__; jj++)
	{
		if(stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			int iAxisTestPosn = (int)((stVelStepGroupTestConfig.dLowerLimitPosition_mm[jj] + 
				stVelStepGroupTestConfig.dUpperLimitPosition_mm[jj])/2.0 * stVelStepGroupTestConfig.fEncResolutionCntPerMM[jj]);

			acsc_ToPoint(stCommHandleACS, 0, // start up immediately the motion
				stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj], iAxisTestPosn, NULL);
			while(mtn_qc_is_axis_still_moving(stCommHandleACS, stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj]) == TRUE)
			{
				Sleep(5);
			}
			if(mtn_qc_is_axis_locked_safe(stCommHandleACS, stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj]) == FALSE)
			{
				goto label_return_run_vel_tuning_by_step_response;
			}
		}
	}

	// Excite the axis
	for(jj =0; jj<__MAX_NUM_ACS_AXIS_TUNING__; jj++)
	{
		int iAxisTestPosn = 
			(int)((stVelStepGroupTestConfig.dLowerLimitPosition_mm[jj] + 
			stVelStepGroupTestConfig.dUpperLimitPosition_mm[jj])/2.0  * stVelStepGroupTestConfig.fEncResolutionCntPerMM[jj]);


		stVelLoopTuningInput = stVelStepGroupTestCase[jj].stVelStepTestOnceInput; /// First several axis
		int iCurrentAxis_Ctrl = stVelLoopTuningInput.iAxisCtrlCard;
		double dMaxStepVelocity = stVelLoopTuningInput.fMaxAbsVel; 
		stVelLoopTuningInput.iPositionLower = iAxisTestPosn - 
			(int)(stVelStepGroupTestConfig.fVelStepTestDistance_mm[jj] / 2.0 * stVelStepGroupTestConfig.fEncResolutionCntPerMM[jj]);
		stVelLoopTuningInput.iPositionUpper = iAxisTestPosn + 
			(int)(stVelStepGroupTestConfig.fVelStepTestDistance_mm[jj] / 2.0 * stVelStepGroupTestConfig.fEncResolutionCntPerMM[jj]);


		if(iCurrentAxis_Ctrl == MAX_CTRL_AXIS_PER_SERVO_BOARD)
		{
			continue;  // skip to next axis			
		}
		// Set VelStep
		double dMaxVel;
		acsc_ReadReal(stCommHandleACS, 0, "XVEL", 
				iCurrentAxis_Ctrl, 
				iCurrentAxis_Ctrl, 0, 0, &dMaxVel, 0); // DOUT

		stVelLoopTuningInput.fMaxAbsVel = stVelStepGroupTestConfig.fMaxVelocityPercent[jj] * dMaxVel;

		DlgVelStepTuningPerAxis(&stVelLoopTuningInput);

		// Check Safety, exit if cut-off
		while(mtn_qc_is_axis_still_moving(stCommHandleACS, stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj]) == TRUE)
		{
			Sleep(5);
		}
		if(mtn_qc_is_axis_locked_safe(stCommHandleACS, stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj]) == FALSE)
		{
			goto label_return_run_vel_tuning_by_step_response;
		}

	}

label_return_run_vel_tuning_by_step_response:
	// End
	cFlagDlgVelStepGroputTestThreadRunning = FALSE;
	mtn_dll_music_logo_normal_stop(); // 20120217  // Logo Normal Exit 3-2-1,
	GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->EnableWindow(TRUE);
	return 0;

}

UINT CMtnVelStepTestDlg::VelStepGroupTestThread()
{
unsigned int jj;
static double dMaxVel;

AFT_VEL_LOOP_TEST_INPUT stVelLoopTestGroupCaseInput;
AFT_VEL_LOOP_TEST_OUTPUT stVelLoopTestOutput;
FILE *fpDataVelStepTestCases;
static	char strFilename[512];
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
CTRL_PARA_ACS astBakupServoPara[8];
unsigned int uiAxisCtrlCard;

	// 20121003
	mtn_wb_bakup_software_limit(stCommHandleACS, 0, 1, 4);
	mtn_wb_enlarge_software_limit_by_(stCommHandleACS, 0, 1, 4, 10.0);

	for(jj = 0; jj< stVelStepGroupTestConfig.usActualNumAxis; jj++)
	{
		uiAxisCtrlCard = stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj];
		mtnapi_upload_servo_parameter_acs_per_axis(stCommHandleACS, uiAxisCtrlCard, &astBakupServoPara[uiAxisCtrlCard]);
	}
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	aft_veloop_step_test_make_new_folder();
	sprintf_s(strFilename, 512, "%s\\VelStepGroupTestCases_%d_%d_%d_H%dm%d.m", strVeLoopTestNewFolderName,
				stpTime->tm_year + 1900, stpTime->tm_mon + 1, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);  // 20110722
	fopen_s(&fpDataVelStepTestCases, strFilename, "w");  // FILENAME_TRAJ_TEMP_DATA
	if(fpDataVelStepTestCases != NULL)
	{
		for(jj = 0; jj< stVelStepGroupTestConfig.usActualNumAxis; jj++)
		{
			fprintf(fpDataVelStepTestCases, "fEncResolution_mm(%d) = %8.4f;\n", jj + 1, stVelStepGroupTestConfig.fEncResolutionCntPerMM[jj]);
		}

		for(jj = 0; jj< stVelStepGroupTestConfig.usActualNumAxis; jj++)  // Application name, 20110917
		{
			fprintf(fpDataVelStepTestCases, "stApplication(%d).strName = \'%s\';\n", jj + 1, stVelStepGroupTestConfig.strServoAxisNameWb[jj]);
		}

		fprintf(fpDataVelStepTestCases, "YYYYMMDDHHMMSS = [%d, %d, %d, %d, %d, %d];\n", stpTime->tm_year + 1900, stpTime->tm_mon + 1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
		fprintf(fpDataVelStepTestCases, "iFlagSaveWaveform = %d;\n", cFlagSaveWaveform);
		fprintf(fpDataVelStepTestCases, "iMachineType = %d;\n", get_sys_machine_type_flag());  // 20120901
		fprintf(fpDataVelStepTestCases, "nTotalCase = %d\n", uiTotalTestCasesVelStepGroup);
	}

	iCaseVelStepGroupTest = 0;

	GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->EnableWindow(FALSE);
	for(iCaseVelStepGroupTest= 0; iCaseVelStepGroupTest< uiTotalTestCasesVelStepGroup; iCaseVelStepGroupTest++)
	{
		if(cFlagDlgVelStepGroputTestThreadRunning == TRUE)
		{
			// read input
			stVelLoopTestGroupCaseInput = stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput;
			stVelLoopTestGroupCaseInput.strServoAxisNameWb = stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.strServoAxisNameWb;

			// Move group axes to desired position
			for(jj =0; jj<stVelStepGroupTestConfig.usActualNumAxis; jj++)
			{
				if(stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD)
				{
					acsc_ToPoint(stCommHandleACS, 0, // start up immediately the motion
						stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj], 
						stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[jj], NULL);
					while(mtn_qc_is_axis_still_moving(stCommHandleACS, stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj]) == TRUE)
					{
						Sleep(5);
					}
					if(mtn_qc_is_axis_locked_safe(stCommHandleACS, stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj]) == FALSE)
					{
						iCaseVelStepGroupTest = 0;
						goto label_return_run_vel_step_group_test;
					}
				}
			}

			acsc_ReadReal(stCommHandleACS, 0, "XVEL", 
				stVelLoopTestGroupCaseInput.iAxisCtrlCard, 
				stVelLoopTestGroupCaseInput.iAxisCtrlCard, 0, 0, &dMaxVel, 0); // DOUT

			stVelLoopTestGroupCaseInput.fMaxAbsVel = stVelStepGroupTestCase[iCaseVelStepGroupTest].fMaxVelocityPercent * dMaxVel;
			if(iCaseVelStepGroupTest < stVelStepGroupTestConfig.usActualNumAxis)  // 20110729
			{
				stVelLoopTestGroupCaseInput.iFlagSaveFile = 1;
			}
			else
			{
				stVelLoopTestGroupCaseInput.iFlagSaveFile = iFlagEnableEditVelStepGroupDlg;  // 20100923
			}   // 20110729

			Sleep(200);
			// 20120913
			SAFETY_PARA_ACS stSafetyParaBak, stSafetyParaCurr;
			mtnapi_upload_safety_parameter_acs_per_axis(stCommHandleACS, stVelLoopTestGroupCaseInput.iAxisCtrlCard, &stSafetyParaBak);
			stSafetyParaCurr = stSafetyParaBak;
			stSafetyParaCurr.dCriticalPosnErrAcc = 10000;
			stSafetyParaCurr.dCriticalPosnErrIdle = 10000;	
			stSafetyParaCurr.dCriticalPosnErrVel = 10000;
			if(stVelLoopTestGroupCaseInput.iAxisCtrlCard == ACSC_AXIS_A)
			{
				stSafetyParaCurr.dRMS_DrvCmdX = 25;  // 20121215
			}
			else
			{
				stSafetyParaCurr.dRMS_DrvCmdX = 40;  // 20121215
			}
			stSafetyParaCurr.dRMS_TimeConst = 6000;
			stSafetyParaCurr.dRMS_DrvCmdIdle = stSafetyParaCurr.dRMS_DrvCmdIdle * 2;
			stSafetyParaCurr.dRMS_DrvCmdMtn = 100;  // 20120717, must release to 100% for motion tuning
			mtnapi_download_safety_parameter_acs_per_axis(stCommHandleACS, stVelLoopTestGroupCaseInput.iAxisCtrlCard, &stSafetyParaCurr);

			mtn_aft_vel_loop_test(stCommHandleACS, &stVelLoopTestGroupCaseInput, &stVelLoopTestOutput);
			astVelLoopTestOutput[iCaseVelStepGroupTest] = stVelLoopTestOutput;  // 20100922
			astVelLoopTestInput[iCaseVelStepGroupTest] = stVelLoopTestGroupCaseInput; // 20100922

			mtnapi_download_safety_parameter_acs_per_axis(stCommHandleACS, 
				stVelLoopTestGroupCaseInput.iAxisCtrlCard, &stSafetyParaBak);
			
			// Check Safety
			for(jj =0; jj<stVelStepGroupTestConfig.usActualNumAxis; jj++)
			{
				if(mtn_qc_is_axis_locked_safe(stCommHandleACS, stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj]) == FALSE)
				{
					uiAxisCtrlCard = stVelStepGroupTestConfig.uiAxis_CtrlCardId[jj];
					mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, uiAxisCtrlCard, &astBakupServoPara[uiAxisCtrlCard]);

					goto label_return_run_vel_step_group_test;
				}
			}

			if(fpDataVelStepTestCases != NULL)
			{
				// Write to file
				fprintf(fpDataVelStepTestCases, "%%%% Case - %d\n", iCaseVelStepGroupTest);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).aAxisId = [", iCaseVelStepGroupTest +1);
				for(jj = 0; jj < stVelStepGroupTestCase[iCaseVelStepGroupTest].usActualNumAxis - 1; jj ++)
				{
					fprintf(fpDataVelStepTestCases, "%d, ", stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj]);
				}
				fprintf(fpDataVelStepTestCases, "%d]; \n", stVelStepGroupTestCase[iCaseVelStepGroupTest].uiAxis_CtrlCardId[jj]);

				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).aAxisPosn = [", iCaseVelStepGroupTest +1);
				for(jj = 0; jj < stVelStepGroupTestCase[iCaseVelStepGroupTest].usActualNumAxis - 1; jj ++)
				{
					fprintf(fpDataVelStepTestCases, "%d, ", stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[jj]);
				}
				fprintf(fpDataVelStepTestCases, "%d]; \n", stVelStepGroupTestCase[iCaseVelStepGroupTest].iAxisPosn[jj]);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).iExciteAxis = %d; \n", iCaseVelStepGroupTest +1, stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.iAxisCtrlCard);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).VelTestPosnRange = [%d, %d]; \n", iCaseVelStepGroupTest +1, stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.iPositionLower, stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.iPositionUpper);
				fprintf(fpDataVelStepTestCases, "%%%% Test-output: \n", stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.iPositionLower, stVelStepGroupTestCase[iCaseVelStepGroupTest].stVelStepTestOnceInput.iPositionUpper);

				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).MaxPosAccEstimateFullDAC = %8.1f; %%(m/s/s) \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fMaxAccEstimateFullDAC_mpss);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).MaxNegAccEstimateFullDAC = %8.1f; %%(m/s/s) \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fMinAccEstimateFullDAC_mpss);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).MaxAccEstimateFullDAC = %8.1f; %%(m/s/s); \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fAveMaxAccEstimateFullDAC_mpss);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).MaxFbAcc = %8.1f; %%(m/s/s); \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fAveMaxFeedBackAcc_mpss);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).ExpAccAtRMS = %8.1f; %%(m/s/s); \n\n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fExpectedAccAtRMS_mpss);
				fprintf(fpDataVelStepTestCases, "%%% Velocity Performance \n");
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).fPosiNegStepPercentOS = [%8.3f, %8.3f]; \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fPositiveStepPercentOS, stVelLoopTestOutput.fNegativeStepPercentOS);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).fPosiNegStepSettleTime = [%8.1f, %8.3f]; \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fPositiveStepSettleTime, stVelLoopTestOutput.fNegativeStepSettleTime);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).fPosiNegStepRiseTime = [%8.1f, %8.3f]; \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fPositiveStepRiseTime, stVelLoopTestOutput.fNegativeStepRiseTime);

				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).fMeanDrvCmd_PosiNegConstVel = [%8.1f, %8.1f]; \n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fMeanDrvCmd_PositiveConstVel, 
					stVelLoopTestOutput.fMeanDrvCmd_NegativeConstVel);
				fprintf(fpDataVelStepTestCases, "stVelStepGroupTestOut(%d).fStdDrvCmd_PosiNegConstVel = [%8.1f, %8.1f]; \n\n", iCaseVelStepGroupTest +1, stVelLoopTestOutput.fStdDrvCmd_PositiveConstVel, 
					stVelLoopTestOutput.fStdDrvCmd_NegativeConstVel);

				Sleep(800);
			}
		}
		else
		{
			break;
		}
	}

	if(iCaseVelStepGroupTest == uiTotalTestCasesVelStepGroup)
	{
		aft_ana_vel_step_group_test(&astVelLoopTestInput[0], &astVelLoopTestOutput[0], uiTotalTestCasesVelStepGroup);
	}

label_return_run_vel_step_group_test:
	if(fpDataVelStepTestCases != NULL)
	{
		fclose(fpDataVelStepTestCases);
	}
	
	mtn_wb_restore_software_limit(stCommHandleACS, 0, 1, 4);  // 20121003

	cFlagDlgVelStepGroputTestThreadRunning = FALSE;
	mtn_dll_music_logo_normal_stop(); // 20120217  // Logo Normal Exit 3-2-1,
	GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->EnableWindow(TRUE);
	return 0;
}

void CMtnVelStepTestDlg::OnBnClickedButtonStopVelStepTest()
{
	// TODO: Add your control notification handler code here
	cFlagDlgVelStepGroputTestThreadRunning = FALSE;
}

void CMtnVelStepTestDlg::OnBnClickedOk()
{
	PauseTimer();  //KillTimer(m_iTimerVal); 20100922
//	iFlagRunningTimer = FALSE;
	OnOK();
}
// IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST
void CMtnVelStepTestDlg::OnBnClickedCheckFlagSaveChartVelStepGroupTest()
{
	cFlagSaveWaveform = ((CButton*)GetDlgItem(IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST))->GetCheck();
}
// IDC_CHECK_FLAG_PLOT_CHART_VEL_STEP_GROUP_TEST
void CMtnVelStepTestDlg::OnBnClickedCheckFlagPlotChartVelStepGroupTest()
{
	cFlagPlotChart = ((CButton*)GetDlgItem(IDC_CHECK_FLAG_PLOT_CHART_VEL_STEP_GROUP_TEST))->GetCheck();
}
void CMtnVelStepTestDlg::UI_EnableEditTestCondition(int bEnableFlag)
{
	GetDlgItem(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4)->EnableWindow(bEnableFlag);
	
	GetDlgItem(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4)->EnableWindow(bEnableFlag);

	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4)->EnableWindow(bEnableFlag);
	
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4)->EnableWindow(bEnableFlag);

	// 20110801
	GetDlgItem(IDC_COMBO_AXIS1_GROUP_VEL_STEP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS2_GROUP_VEL_STEP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS3_GROUP_VEL_STEP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_AXIS4_GROUP_VEL_STEP)->EnableWindow(bEnableFlag);
}

void CMtnVelStepTestDlg::SetUserInterfaceLanguage(int iLanguageOption)
{
	m_iLanguageOption = iLanguageOption;

	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_STATIC_LABEL_GROUP_VEL_STEP_TEST)->SetWindowTextA(_T("Group VelocityStep test"));
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_AXIS_LIST)->SetWindowTextA(_T("Axis Involved")); // 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_FLAG_IS_EXCITING_AXIS)->SetWindowTextA(_T("Excite")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_NUM_POINTS_PER_AXIS)->SetWindowTextA(_T("NumPoints")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_LOW_LIMIT_PER_AXIS)->SetWindowTextA(_T("LowLimit:(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_UPP_LIMIT_PER_AXIS)->SetWindowTextA(_T("UppLimit(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TEST_DISTANCE)->SetWindowTextA(_T("Distance(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_MAX_VEL_PER_AXIS)->SetWindowTextA(_T("% x MaxVel")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_LOOP_NUM)->SetWindowTextA(_T("LoopNum")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TOLERANCE_SETTLE_TIME)->SetWindowTextA(_T("Tol%, S.T")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_ENC_RES_MM_PER_AXIS)->SetWindowTextA(_T("Enc/mm")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_FLAG_HAS_OFFSET_PER_AXIS)->SetWindowTextA(_T("Offset")); 
		GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->SetWindowTextA(_T("StartGroupTest")); 
		GetDlgItem(IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST)->SetWindowTextA(_T("SaveChart")); 
	}
	else
	{
		GetDlgItem(IDC_STATIC_LABEL_GROUP_VEL_STEP_TEST)->SetWindowTextA(_T("速度阶跃响应-组合阵列测试"));
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_AXIS_LIST)->SetWindowTextA(_T("包含轴")); // 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_FLAG_IS_EXCITING_AXIS)->SetWindowTextA(_T("激发轴")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_NUM_POINTS_PER_AXIS)->SetWindowTextA(_T("总点数")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_LOW_LIMIT_PER_AXIS)->SetWindowTextA(_T("下限(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_UPP_LIMIT_PER_AXIS)->SetWindowTextA(_T("上限(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TEST_DISTANCE)->SetWindowTextA(_T("距离(mm)")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_MAX_VEL_PER_AXIS)->SetWindowTextA(_T("最大速度%")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_LOOP_NUM)->SetWindowTextA(_T("总周数")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_TOLERANCE_SETTLE_TIME)->SetWindowTextA(_T("安定时间误差阈值")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_ENC_RES_MM_PER_AXIS)->SetWindowTextA(_T("编码器精度")); 
		GetDlgItem(IDC_STATIC_GROUP_VEL_STEP_FLAG_HAS_OFFSET_PER_AXIS)->SetWindowTextA(_T("偏置")); 
		GetDlgItem(IDC_BUTTON_START_GROUP_VEL_STEP)->SetWindowTextA(_T("开始测试")); 
		GetDlgItem(IDC_CHECK_FLAG_SAVE_CHART_VEL_STEP_GROUP_TEST)->SetWindowTextA(_T("分析后自动保存图像")); 
	}
}

#include "DlgKeyInputPad.h"
// IDC_CHECK_VEL_STEP_DLG_ENABLE_EDIT
void CMtnVelStepTestDlg::OnBnClickedCheckVelStepDlgEnableEdit()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_VEL_STEP_DLG_ENABLE_EDIT))->GetCheck();
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
			((CButton *)GetDlgItem(IDC_CHECK_VEL_STEP_DLG_ENABLE_EDIT))->SetCheck(FALSE);
		}
	}
	else
	{
			UI_EnableEditTestCondition(iTemp);
	}

	iFlagEnableEditVelStepGroupDlg = iTemp;
	UI_EnableEditTestCondition(iFlagEnableEditVelStepGroupDlg);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis3()
{
	double dTemp;
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, &(dTemp));
	if(dTemp > UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_BONDHEAD)
	{
		dTemp = UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_BONDHEAD;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, dTemp, "%8.1f");
	}
	else if(dTemp < LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE)
	{
		dTemp = LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_BONDHEAD;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS3, dTemp, "%8.1f");
	}
	stVelStepGroupTestConfig.fMaxVelocityPercent[2] = dTemp / 100.0;
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis4()
{
	double dTemp;
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS4, &(dTemp));
	stVelStepGroupTestConfig.fMaxVelocityPercent[3] = dTemp / 100.0;
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis1()
{
	double dTemp;
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, &(dTemp));
	if(dTemp > UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE)
	{
		dTemp = UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, dTemp, "%8.1f");
	}
	else if(dTemp < LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE)
	{
		dTemp = LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS1, dTemp, "%8.1f");
	}

	stVelStepGroupTestConfig.fMaxVelocityPercent[0] = dTemp / 100.0;
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepMaxVelGroupAxis2()
{
	double dTemp;
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, &(dTemp));
	if(dTemp > UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE)
	{
		dTemp = UPP_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, dTemp, "%8.1f");
	}
	else if(dTemp < LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE)
	{
		dTemp = LOW_MAX_PC_VEL_IN_VEL_STEP_GROUP_TEST_TABLE;
		VelStepGroupTestUpdateDoubleToEdit(IDC_EDIT_VEL_STEP_MAX_VEL_GROUP_AXIS2, dTemp, "%8.1f");
	}

	stVelStepGroupTestConfig.fMaxVelocityPercent[1] = dTemp / 100.0;
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis1()
{
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS1, &(stVelStepGroupTestConfig.fVelStepTestDistance_mm[0]));
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis2()
{
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS2, &(stVelStepGroupTestConfig.fVelStepTestDistance_mm[1]));
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis3()
{
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS3, &(stVelStepGroupTestConfig.fVelStepTestDistance_mm[2]));
}

void CMtnVelStepTestDlg::OnEnKillfocusEditVelStepDistGroupVelStepAxis4()
{
	ReadDoubleFromEdit(IDC_EDIT_VEL_STEP_DIST_GROUP_VEL_STEP_AXIS4, &(stVelStepGroupTestConfig.fVelStepTestDistance_mm[3]));
}

void CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis1()
{
	ReadDoubleFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS1, &stVelStepGroupTestConfig.dUpperLimitPosition_mm[0]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis2()
{
	ReadDoubleFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS2, &stVelStepGroupTestConfig.dUpperLimitPosition_mm[1]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis3()
{
	ReadDoubleFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS3, &stVelStepGroupTestConfig.dUpperLimitPosition_mm[2]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditUppLimitGroupVelStepAxis4()
{
	ReadDoubleFromEdit(IDC_EDIT_UPP_LIMIT_GROUP_VEL_STEP_AXIS4, &stVelStepGroupTestConfig.dUpperLimitPosition_mm[3]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis1()
{
	ReadDoubleFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS1, &stVelStepGroupTestConfig.dLowerLimitPosition_mm[0]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis2()
{
	ReadDoubleFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS2, &stVelStepGroupTestConfig.dLowerLimitPosition_mm[1]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis3()
{
	ReadDoubleFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS3, &stVelStepGroupTestConfig.dLowerLimitPosition_mm[2]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditLowLimitGroupVelStepAxis4()
{
	ReadDoubleFromEdit(IDC_EDIT_LOW_LIMIT_GROUP_VEL_STEP_AXIS4, &stVelStepGroupTestConfig.dLowerLimitPosition_mm[3]);
}

void CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis1()
{
	ReadUnsignIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1, &(stVelStepGroupTestConfig.uiNumPoints[0]));
	if(stVelStepGroupTestConfig.uiNumPoints[0] > MAX_NUM_POINT_VEL_STEP_GROUP_TEST_X_TABLE)
	{
		stVelStepGroupTestConfig.uiNumPoints[0] = MAX_NUM_POINT_VEL_STEP_GROUP_TEST_X_TABLE;
		InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS1, stVelStepGroupTestConfig.uiNumPoints[0]);
	}
}

void CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis2()
{
	ReadUnsignIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2, &(stVelStepGroupTestConfig.uiNumPoints[1]));
	if(stVelStepGroupTestConfig.uiNumPoints[1] > MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Y_TABLE)
	{
		stVelStepGroupTestConfig.uiNumPoints[1] = MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Y_TABLE;
		InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS2, stVelStepGroupTestConfig.uiNumPoints[1]);
	}
}

void CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis3()
{
	ReadUnsignIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3, &(stVelStepGroupTestConfig.uiNumPoints[2]));
	if(stVelStepGroupTestConfig.uiNumPoints[2] > MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Z_BOND)
	{
		stVelStepGroupTestConfig.uiNumPoints[2] = MAX_NUM_POINT_VEL_STEP_GROUP_TEST_Z_BOND;
		InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS3, stVelStepGroupTestConfig.uiNumPoints[2]);
	}
}

void CMtnVelStepTestDlg::OnEnKillfocusEditNumPointsGroupVelStepAxis4()
{
	ReadUnsignIntegerFromEdit(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4, &(stVelStepGroupTestConfig.uiNumPoints[3]));
	if(stVelStepGroupTestConfig.uiNumPoints[3] > MAX_NUM_POINT_VEL_STEP_GROUP_TEST_DEF_AXIS)
	{
		stVelStepGroupTestConfig.uiNumPoints[3] = MAX_NUM_POINT_VEL_STEP_GROUP_TEST_DEF_AXIS;
		InitVelStepGroupTestConfigEditOneAxis(IDC_EDIT_NUM_POINTS_GROUP_VEL_STEP_AXIS4, stVelStepGroupTestConfig.uiNumPoints[3]);
	}
}

// IDC_CHECK_VEL_STEP_DLG_TEACH_LIMIT
void CMtnVelStepTestDlg::OnBnClickedCheckVelStepDlgTeachLimit()
{
	iFlagTeachLimitBeforeTest = ((CButton *)GetDlgItem(IDC_CHECK_VEL_STEP_DLG_TEACH_LIMIT))->GetCheck();
}
