// DlgServoTuning.cpp : implementation file
//
// 20100426  From Thomas, disable clear zero position

#include "stdafx.h"
#include "DlgServoTuning.h"
CDlgServoTuning *pDlgServoCtrlTuning;

#include "math.h"
#include "MtnTune.h"
// CDlgServoTuning dialog
#include "MtnTesterResDef.h"

#include "AftVelLoopTest.h"
AFT_VEL_LOOP_TEST_INPUT stVelLoopTestInput;

#include "MtnApi.h"
extern COMM_SETTINGS stServoControllerCommSet;
static HANDLE stCommHandleACS = ACSC_INVALID;	// communication handle

#define MAX_X_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE  1080
#define MAX_Y_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE  520

// XLS class
//#include "XLEzAutomation.h"
//class CXLEzAutomation;
//static CXLEzAutomation * m_pXLS_ServoTuningDlg;

double *afRefPosn;
double *afFeedVel;
double *afRefVel;
double *afFeedAcc;
double *afDac;

extern void mtn_aft_vel_loop_add_data_into_sheet(); // CXLEzAutomation * m_pXLS_ServoTuningDlg

IMPLEMENT_DYNAMIC(CDlgServoTuning, CDialog)

CDlgServoTuning::CDlgServoTuning(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServoTuning::IDD, pParent)
{

}

CDlgServoTuning::~CDlgServoTuning()
{
}


#include "MtnInitAcs.h"
static char strWbServoParaBaseFilename[] = "C:\\WbData\\ParaBase\\ServoMaster.ini";
static char cFlagSafetyCheckOpenLoopForceAndFeedback = 1; // IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_, 20121012


void CDlgServoTuning::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

char *strServoSetupCtrlPageTextEn[] =
{
	"Encoder",
	"OpenLoop",
	"VelLoop",
	"Spectrum",
	"MoveTest",
//  "RandPtn",
//	"FixedPtn",
	"ForceCtrl",
	"Looping",
	"WB_Motion"
};

char *strServoSetupCtrlPageTextCn[] =
{
	"调编码器",
	"开环测试",
	"调速度环",
	"频谱测试",
	"运动测试",
//	"随机模式",
//	"固定模式",
	"压力测试",
	"线弧测试",
	"焊线应用"
};

extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

extern double afRatioRMS_DrvCmd[MAX_SERVO_AXIS_WIREBOND];

static int iEnaFlagSaveFileWbWaveformPerformance;
static int iOptionShownWbWaveformPerformance;
#define OPT_SHOW_MULTI_WIRE_STAT    0
#define OPT_SHOW_1ST_WIRE			1
#define OPT_SHOW_2ND_WIRE			2
#define OPT_SHOW_3RD_WIRE			3
#define OPT_SHOW_4TH_WIRE			4
char *astrComboOptionShowWbWaveformPerformance[] =
{
	"MultiWireStat",
	"1st Wire",
	"2nd Wire",
	"3rd Wire",
	"4th Wire"
};

#define __IDX_COMBO_WB_PERFORM_BND_Z_SRCH_CONTACT   0
#define __IDX_COMBO_WB_PERFORM_BND_Z_LOOPING		1
#define __IDX_COMBO_WB_PERFORM_XY_SETTLING_1ST_B	2
#define __IDX_COMBO_WB_PERFORM_XY_SETTLING_2ND_B	3
#define __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_Z		4
#define __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_XY		5
#define __IDX_COMBO_WB_PERFORM_MULTI_WIRE_STAT__    6
#define __IDX_COMBO_WB_PERFORM_MULTI_ALL            7

char *astrComboShowWbPerformOption[] =
{
	"BND_Z_SRCH_CONTACT",
	"BND_Z_LOOPING",
	"XY_SETTLING_1ST_B",
	"XY_SETTLING_2ND_B",
	"ACTUAL_SPEED_Z",
	"ACTUAL_SPEED_XY",
	"MULTI_WIRE_STAT__",
	"ALL"
};

static int iFlagTuneWB_ServoOption2 = __IDX_COMBO_WB_PERFORM_MULTI_ALL;

extern char *astrWireBondServoAxisNameEn[];
extern char *astrWireBondServoAxisNameCn[];

extern RANGE_DRIVE_OUT astRangeDriveOut[];
extern char *strDriveRangeText_OLT[];

static int iCurrSelAxisServoCtrlTuning = WB_AXIS_TABLE_X;  // X
static int iCurrServoCtrlTuneTabId;
static int iFlagPlotDlgServoCtrlTuning = 0;

static double dActualDriveOut;
static int iDriveOutRangeCurrSel = 3;  // 1%

BEGIN_MESSAGE_MAP(CDlgServoTuning, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SERVO_SETUP_TUNE_TAB, &CDlgServoTuning::OnTcnSelchangeServoSetupTuneTab)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT, &CDlgServoTuning::OnNMCustomdrawServoCtrlTuningSliderDriverOut)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP, &CDlgServoTuning::OnCbnSelchangeComboAxisServoCtrlTuningOpenLoop)
	ON_CBN_SELCHANGE(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP, &CDlgServoTuning::OnCbnSelchangeComboOutRangeServoCtrlTuningOpenLoop)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningOpenLoopEnable)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningZeroDriverOut)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningZeroFeedbackPosn)
	ON_BN_CLICKED(IDOK, &CDlgServoTuning::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetEncIdxOnce)
	ON_EN_CHANGE(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnEnChangeEdit1stPosnServoCtrlVelLoop)
	ON_EN_CHANGE(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnEnChangeEdit2ndPosnServoCtrlVelLoop)
	ON_BN_CLICKED(IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnBnClickedButtonRead1stPosnServoCtrlVelLoop)
	ON_BN_CLICKED(IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnBnClickedButtonRead2ndPosnServoCtrlVelLoop)
	ON_BN_CLICKED(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnBnClickedButtonStartTestPosnServoCtrlVelLoop)
	ON_EN_CHANGE(IDC_EDIT_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP, &CDlgServoTuning::OnEnChangeEditTotalNumLoopServoCtrlVelLoop)
	ON_BN_CLICKED(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING, &CDlgServoTuning::OnBnClickedCheckFlagPlotServoCtrlTuning)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetServicePass)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_GROUP_TEST, &CDlgServoTuning::OnBnClickedButtonServoCtrlGroupTest)
	ON_BN_CLICKED(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE, &CDlgServoTuning::OnBnClickedCheckServoCtrlTuningEnaWaveform)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetWbPerformIdx)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningHomeCurrAxis)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ, &CDlgServoTuning::OnBnClickedButtonServoCtrlTuningHomeWbXyz)
	ON_CBN_SELCHANGE(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP, &CDlgServoTuning::OnCbnSelchangeComboShowPerformOptServoCtrlTuningWbApp)
	ON_BN_CLICKED(IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE, &CDlgServoTuning::OnBnClickedButtonGenAftReportServoTune)
	ON_BN_CLICKED(IDC_CHECK_FLAG_INVERT_ENCODER, &CDlgServoTuning::OnBnClickedCheckFlagInvertEncoder)
	ON_BN_CLICKED(IDC_CHECK_FLAG_INVERT_DRIVER_OUT, &CDlgServoTuning::OnBnClickedCheckFlagInvertDriverOut)
	ON_BN_CLICKED(IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE, &CDlgServoTuning::OnBnClickedCheckServoAdjustPositionSpringCompensateEnable)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN, &CDlgServoTuning::OnEnKillfocusEditServoAdjustCalibrateUppPosn)
	ON_EN_KILLFOCUS(IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN, &CDlgServoTuning::OnEnKillfocusEditServoAdjustCalibrateLowPosn)
	ON_BN_CLICKED(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN, &CDlgServoTuning::OnBnClickedButtonServoAdjustStartCalibratePosn)
//	ON_BN_CLICKED(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSITION_SPRING_GRAVITY, &CDlgServoTuning::OnBnClickedButtonServoAdjustVerifyPositionSpringGravity)
    ON_BN_CLICKED(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY, &CDlgServoTuning::OnBnClickedButtonServoAdjustVerifyPosnSpringGravity)
//ON_STN_CLICKED(IDC_STATIC_SERVO_ADJUST_LABEL_BH_UPP_LIMIT_POSN, &CDlgServoTuning::OnStnClickedStaticServoAdjustLabelBhUppLimitPosn)
ON_CBN_SELCHANGE(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL, &CDlgServoTuning::OnCbnSelchangeComboServoCtrlTuningWbDebugLevel)
ON_EN_KILLFOCUS(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING, &CDlgServoTuning::OnEnKillfocusEditEncResServoCtrlTuning)
ON_BN_CLICKED(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_, &CDlgServoTuning::OnBnClickedCheckSafetyForceFeedbackServoCtrlTuning)
END_MESSAGE_MAP()

// Need to update with ZD
int mtn_tune_get_wb_waveform_ena_flag()
{
	int iEnaWbWaveformDebug = 0;

    acsc_ReadInteger(stCommHandleACS, 0, "IntData", 33, 33, 0, 0, &iEnaWbWaveformDebug, NULL);

	return iEnaWbWaveformDebug;

}
// Need to update with ZD
void mtn_tune_set_wb_waveform_ena_flag(int iEnaFlag)
{
	int iEnaWbWaveformDebug = iEnaFlag;
    acsc_WriteInteger(stCommHandleACS, 0, "IntData", 33, 33, 0, 0, &iEnaWbWaveformDebug, NULL);
}

#include "mtndefin.h"

void InitDriveOutRangeStruct()
{
	for(int ii=0; ii<MAX_DRIVE_OUT_RANGE_CASES; ii++)
	{
		astRangeDriveOut[ii].iRangeId = ii;
		astRangeDriveOut[ii].strRangeText = strDriveRangeText_OLT[ii];
	}
	astRangeDriveOut[0].dRangeNum = 1;
	astRangeDriveOut[1].dRangeNum = 2;
	astRangeDriveOut[2].dRangeNum = 5;
	astRangeDriveOut[3].dRangeNum = 10;
	astRangeDriveOut[4].dRangeNum = 20;
	astRangeDriveOut[5].dRangeNum = 50;
	astRangeDriveOut[6].dRangeNum = 100;

}
//extern MOTALGO_DLL_API int get_sys_machine_type_flag();

void mtn_aft_vel_loop_add_data_into_sheet()
{
	CString czTemp;
	int iCountVelChangeSign = 0, iCountColumn=1;

#ifdef __XLS_BY_OLE__
	for(unsigned int ii=0; ii<gstSystemScope.uiDataLen; ii++)
	{
		czTemp.Format("%6.1f", afRefPosn[ii]);
		m_pXLS_ServoTuningDlg->SetCellValue(iCountColumn, ii+1, czTemp); iCountColumn ++;
		czTemp.Format("%6.1f", afFeedVel[ii]);
		m_pXLS_ServoTuningDlg->SetCellValue(iCountColumn, ii+1, czTemp); iCountColumn ++;
		czTemp.Format("%6.1f", afRefVel[ii]);
		m_pXLS_ServoTuningDlg->SetCellValue(iCountColumn, ii+1, czTemp); iCountColumn ++;
		czTemp.Format("%6.1f", afFeedAcc[ii]);
		m_pXLS_ServoTuningDlg->SetCellValue(iCountColumn, ii+1, czTemp); iCountColumn ++;
		czTemp.Format("%6.1f", afDac[ii]);
		m_pXLS_ServoTuningDlg->SetCellValue(iCountColumn, ii+1, czTemp); iCountColumn = 1;

////////
		if((afRefVel[ii+1] * afRefVel[ii]) < 0)
		{
			iCountVelChangeSign ++;
		}
		if(iCountVelChangeSign >= 3) break;
	}
#endif
}

static UINT_PTR m_iTimerVal;
static int iCheckAxisEncoderGuideStage = 0;
static int iCountPassIndexPosn = 0;
static double dIndexPositionCurr;
static char cFlagAxisEncoderOK = TRUE;

CString aszCheckEncoderErrorIndexAlignment[2] =
{
	_T("Check Encoder Alignment, Index Count Error"),
	_T("检查编码器对准，刻线标尺错误")
};

CString aszCmdButtonEnableMotor[2] =
{
	_T("Enable"),
	_T("使能")
};
CString aszCmdButtonDisableMotor[2] =
{
	_T("Disable"),
	_T("松开")
};

void CDlgServoTuning::SetUserInterfaceLanguage(int iLanguageOption)
{
	m_iLanguageOption = iLanguageOption;
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING)->SetWindowTextA(_T("ShowData"));
		GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("Axis"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->SetWindowTextA(_T("Enable"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->SetWindowTextA(_T("Zero"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(_T("CheckEncIdx"));
		GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("Fb Posn:"));
		GetDlgItem(IDC_STATIC_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("1st Posn:"));
		GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("Last IdxPosn"));
		GetDlgItem(IDC_STATIC_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("2nd Posn:"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN)->SetWindowTextA(_T("Zero Posn"));
		GetDlgItem(IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("Read 1st"));
		GetDlgItem(IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("Read 2nd"));
		GetDlgItem(IDC_STATIC_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("Num. Loops"));
		GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("VeLoopStep"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS)->SetWindowTextA(_T("Get ServicePass"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->SetWindowTextA(_T("Group Test"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->SetWindowTextA(_T("WbPerform"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->SetWindowTextA(_T("WB_HomeAll"));	
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->SetWindowTextA(_T("HomeCurrAxis"));
		GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->SetWindowTextA(_T("#INVENC"));   // 20110131
		GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->SetWindowTextA(_T("#INVDOUT")); // 20110131
		GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->SetWindowTextA(_T("SAFE")); // 20121012

		GetDlgItem(IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE)->SetWindowTextA(_T("GenReport"));		
		
		if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_OPEN_LOOP)
		{
			GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("Output Range(%):"));
		}
		else if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_VELOCITY_LOOP)
		{
			GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("VelCmd(%):"));
		}
		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->SetWindowTextA(_T("P.Cali"));		
		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->SetWindowTextA(_T("P.Veri"));		
		
	}
	else if(iLanguageOption == LANGUAGE_UI_CN)
	{
		GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING)->SetWindowTextA(_T("显示数据"));
		GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("调控对象"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->SetWindowTextA(_T("使能"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->SetWindowTextA(_T("归零"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(_T("检查编码器"));
		GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("反馈位置"));
		GetDlgItem(IDC_STATIC_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("第一点"));
		GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("刻标位置"));
		GetDlgItem(IDC_STATIC_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("第二点"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN)->SetWindowTextA(_T("位置清零"));
		GetDlgItem(IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("读第一点"));
		GetDlgItem(IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("读第二点"));
		GetDlgItem(IDC_STATIC_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("总共往返周数"));
		GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(_T("开始速度环测试"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS)->SetWindowTextA(_T("维护口令"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->SetWindowTextA(_T("组合测试"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->SetWindowTextA(_T("焊线伺服性能"));
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->SetWindowTextA(_T("焊线机初始化"));	
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->SetWindowTextA(_T("当前轴归零"));	
		GetDlgItem(IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE)->SetWindowTextA(_T("装机报告"));		
		GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->SetWindowTextA(_T("位移反向"));    // 20110131
		GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->SetWindowTextA(_T("出力反向")); // 20110131
		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->SetWindowTextA(_T("位置校准"));		
		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->SetWindowTextA(_T("验证"));		
		// 焊线机初始化
		GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->SetWindowTextA(_T("安检")); // 20121012
		
		if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_OPEN_LOOP)
		{
			GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("驱动输出范围(%):"));
		}
		else if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_VELOCITY_LOOP)
		{
			GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("指令速度(%)"));
		}
	}
	else
	{
	}
	SetCtrlTabLanguage();
	SetAxisNameLanguage();
}

void CDlgServoTuning::SetCtrlTabLanguage()
{
	// IDC_SERVO_SETUP_TUNE_TAB
	CTabCtrl *pcTabCtrl = (CTabCtrl*)GetDlgItem(IDC_SERVO_SETUP_TUNE_TAB);

	pcTabCtrl->DeleteAllItems();

	if(m_iLanguageOption == LANGUAGE_UI_EN)
	{
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_ENCODER, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_ENCODER]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_OPEN_LOOP, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_OPEN_LOOP]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_VELOCITY_LOOP, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_VELOCITY_LOOP]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_SPECTRUM, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_SPECTRUM]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_MOVE_TEST, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_MOVE_TEST]);
//		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_RANDOM_POINTS, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_RANDOM_POINTS]);
//		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_FIXED_PATTERN, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_FIXED_PATTERN]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_FORCE_CTRL, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_FORCE_CTRL]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_LOOPING, strServoSetupCtrlPageTextEn[SERVO_AFT_VERIFY_LOOPING]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_PASS, strServoSetupCtrlPageTextEn[SERVO_AFT_PASS]);
		
	}
	else if(m_iLanguageOption == LANGUAGE_UI_CN)
	{
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_ENCODER, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_ENCODER]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_OPEN_LOOP, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_OPEN_LOOP]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_VELOCITY_LOOP, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_VELOCITY_LOOP]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_SPECTRUM, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_SPECTRUM]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_MOVE_TEST, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_MOVE_TEST]);
//		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_RANDOM_POINTS, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_RANDOM_POINTS]);
//		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_FIXED_PATTERN, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_FIXED_PATTERN]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_FORCE_CTRL, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_FORCE_CTRL]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_VERIFY_LOOPING, strServoSetupCtrlPageTextCn[SERVO_AFT_VERIFY_LOOPING]);
		InitTabCtrlServoSetup(pcTabCtrl, SERVO_AFT_PASS, strServoSetupCtrlPageTextCn[SERVO_AFT_PASS]);
	}
	else
	{
	}
	pcTabCtrl->SetCurSel(iCurrServoCtrlTuneTabId);

}

void CDlgServoTuning::SetAxisNameLanguage()
{

	CComboBox *pAxisComboAxisWB = (CComboBox*)GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP);
	
	//for(int ii = ii<pAxisComboAxisWB->GetCount(); ii >0; ii--)
	//{
	//	pAxisComboAxisWB->DeleteString(ii-1);
	//}
	pAxisComboAxisWB->ResetContent();
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
	pAxisComboAxisWB->SetCurSel(iCurrSelAxisServoCtrlTuning);
}


// CDlgServoTuning message handlers
BOOL CDlgServoTuning::OnInitDialog()
{
	// 0.0 Language Option
	SetUserInterfaceLanguage(get_sys_language_option()); //	m_iLanguageOption = get_sys_language_option();

	// 1.1. Initialize static Local Variable
	SetAxisNameLanguage();
	// Init ACS communication
	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		stCommHandleACS = stServoControllerCommSet.Handle;
	}
	iCurrServoCtrlTuneTabId = SERVO_AFT_VERIFY_ENCODER; //SERVO_AFT_VERIFY_OPEN_LOOP;
	SetUserInterfaceByTab(iCurrServoCtrlTuneTabId);

	// 1.2. Init Velocity Loop Test
	stVelLoopTestInput.iPositionUpper = 5000;
	stVelLoopTestInput.iPositionLower = 3000;
	stVelLoopTestInput.fMaxAbsDAC = 0;
	stVelLoopTestInput.fMaxAbsVel = 0;
	stVelLoopTestInput.uiLoopNum = 15;
	stVelLoopTestInput.iAxisCtrlCard = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS; // VelLoopTest
	stVelLoopTestInput.fDriveCmdRatioRMS = afRatioRMS_DrvCmd[iCurrSelAxisServoCtrlTuning];
	stVelLoopTestInput.fEncResolutionCntPerMM = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm;
	stVelLoopTestInput.strServoAxisNameWb = astrWireBondServoAxisNameEn[iCurrSelAxisServoCtrlTuning];
	stVelLoopTestInput.fPercentErrorThresholdSettleTime = 0.05;   // DEF_THRESHOLD_CALC_SETTLE_TIME

	CString cstrTemp;
	cstrTemp.Format("%8.2f", afRatioRMS_DrvCmd[iCurrSelAxisServoCtrlTuning]);
	GetDlgItem(IDC_EDIT_RATIO_RMS_SERVO_CTRL_TUNING_VEL_LOOP)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%8.1f", astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm); // [iCurrSelAxisServoCtrlTuning]);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%d", stVelLoopTestInput.iPositionLower);
	GetDlgItem(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stVelLoopTestInput.iPositionUpper);
	GetDlgItem(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stVelLoopTestInput.uiLoopNum);
	GetDlgItem(IDC_EDIT_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(cstrTemp);

	//// 2 Initialize user interface
	// 2.1. Init Range and Slider
	InitDriveOutRangeStruct();
	InitRangeComboOpenLoopTest();
	InitSliderRangeDriveOutput();
	UpdateActualDriveOutBySlide();
	UpdateLabelMaxMinRange();

	// 2.2. Initialize Current target axis
	InitAxisForWireBond();

	// 2.3. Initialize checking encoder guide
	iCheckAxisEncoderGuideStage = 0;
	cFlagAxisEncoderOK = TRUE;
	UpdateCheckEncoderGuide();

	// 2.4. Flag for plotting
	((CButton*)GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING))->SetCheck(iFlagPlotDlgServoCtrlTuning);
	((CButton*)GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_))->SetCheck(cFlagSafetyCheckOpenLoopForceAndFeedback);

	// 2.5. Update the flags of inverting DrvOut and Encoder
	Update_UI_FlagInvertEncoderDrvOut_CtrlCard();  // 20110131
	// IDC_SERVO_SETUP_TUNE_TAB
//	SetCtrlTabLanguage();
//	CTabCtrl *pcTabCtrl = (CTabCtrl*)GetDlgItem(IDC_SERVO_SETUP_TUNE_TAB);

	// Start XLS application
	if(iFlagPlotDlgServoCtrlTuning)
	{
	//	if(NULL == m_pXLS_ServoTuningDlg)
	//	{
	////		MessageBox("Press 'Stop' to end automatic updating of the chart","Guide", MB_OK);
	////		m_pXLS_ServoTuningDlg = new CXLEzAutomation();  // (FALSE); // FALSE not visible
	//	}
	//	m_pXLS_ServoTuningDlg->CreateXYChart(2);
	}

	((CButton *) GetDlgItem(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE))->SetCheck(iEnaFlagSaveFileWbWaveformPerformance);
	((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP))->SetCurSel(iOptionShownWbWaveformPerformance);
	InitComboShowWbPerformOpt_2();

	mtn_tune_init_servo_para_tuning_config();
	mtn_tune_set_initial_tuning_para();

	StartTimer(IDT_SERVO_CTRL_TUNING_DLG_TIMER, 50);
	return CDialog::OnInitDialog();

}

UINT_PTR  CDlgServoTuning::StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration)
{ // IDT_SPEC_TEST_DLG_TIMER
	m_iTimerVal = SetTimer(nIDT_TimerResMacro, TimerDuration, 0);
	
	if (m_iTimerVal == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}
    return m_iTimerVal;
} 

BOOL CDlgServoTuning::StopTimer()
{
	if (!KillTimer (m_iTimerVal))
	{
		return FALSE;
	}
	return TRUE;
} // end StopTimer

CString aszCheckEncoderGuide[7][2] =
{
	{_T("Move target to one limit, then press StartCheckEnc"),	_T("移动调控对象到一侧极限，按键开始")},
	{_T("Move target to the other limit, then press Check 1st"),	_T("移动调控对象到另一侧极限，按键开始一次")},
	{_T("Move target to the other limit, then press Check 2nd"),	_T("移动调控对象到另一侧极限，按键开始二次")},
	{_T("Move target to the other limit, then press Check 3rd"),	_T("移动调控对象到另一侧极限，按键开始三次")},
	{_T("Move target to the other limit, then press Check 4th"),	_T("移动调控对象到另一侧极限，按键开始四次")},
	{_T("Move target to the other limit, then press Check 5th"),	_T("移动调控对象到另一侧极限，按键开始五次")},
	{_T("Move target to the other limit, then press Check more"),	_T("移动调控对象到另一侧极限，按键开始再次")}
};

CString aszCheckEncoderButtonText[7][2] =
{
	{_T("StartCheckEnc"),	_T("开始检查")},
	{_T("Check 1st"),	_T("检查一次")},
	{_T("Check 2nd"),	_T("检查二次")},
	{_T("Check 3rd"),	_T("检查三次")},
	{_T("Check 4th ..."),	_T("检查四次")},
	{_T("Check 5th..."),	_T("检查五次")},
	{_T("Check more..."),	_T("检查再次")}
};

static char *strStopString;
void CDlgServoTuning::ReadDoubleFromEdit(int nResId, double *dValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*dValue = strtod(tempChar, &strStopString);
}

void CDlgServoTuning::UpdateCheckEncoderGuide()
{
	switch(iCheckAxisEncoderGuideStage)
	{
	case 0:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[0][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[0][m_iLanguageOption]);
		break;
	case 1:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[1][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[1][m_iLanguageOption]);
		break;
	case 2:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[2][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[2][m_iLanguageOption]);
		break;
	case 3:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[3][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[3][m_iLanguageOption]);
		break;
	case 4:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[4][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[4][m_iLanguageOption]);
		break;
	case 5:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[5][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[5][m_iLanguageOption]);
		break;
	default:
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->SetWindowTextA(aszCheckEncoderButtonText[6][m_iLanguageOption]);
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(aszCheckEncoderGuide[6][m_iLanguageOption]);
		break;

	}
}

void CDlgServoTuning::InitTabCtrlServoSetup(CTabCtrl *pcTabCtrl, int iTabCtrlId, char *strTabText)
{
	TCITEMA cTabCtrlItem;
//	TCITEMA *pTempTabCtrlItem;
	CString cstrTemp;

	cstrTemp = strTabText;
	if(pcTabCtrl != NULL)
	{
		//pTempTabCtrlItem = NULL;
		//pcTabCtrl->GetItem(iTabCtrlId, pTempTabCtrlItem);
		//if(pTempTabCtrlItem != NULL)
		//{
		//	pcTabCtrl->DeleteItem(iTabCtrlId);
		//}
		cTabCtrlItem.mask = TCIF_TEXT;
		cTabCtrlItem.pszText = (LPSTR)(LPCTSTR)cstrTemp;
		cTabCtrlItem.cchTextMax = cstrTemp.GetLength();
		pcTabCtrl->InsertItem(iTabCtrlId, &cTabCtrlItem);
	}
}
// IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT
void CDlgServoTuning::InitSliderRangeDriveOutput()
{
	((CSliderCtrl*)GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT))->SetRangeMax(100);
	((CSliderCtrl*)GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT))->SetRange(0, 100);
	((CSliderCtrl*)GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT))->SetPos(50);
}

// IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT
// IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE
void CDlgServoTuning::OnTcnSelchangeServoSetupTuneTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	CTabCtrl *pcTabCtrl = (CTabCtrl*)GetDlgItem(IDC_SERVO_SETUP_TUNE_TAB);
	iCurrServoCtrlTuneTabId = pcTabCtrl->GetCurSel();
	SetUserInterfaceByTab(iCurrServoCtrlTuneTabId);
	// Language Option
//	SetUserInterfaceLanguage(get_sys_language_option());

	*pResult = 0;
}

#include "MtnSpectrumTestDlg.h"
extern CMtnSpectrumTestDlg *cpTestSpectrum;
#include "MtnTest_Move.h"
extern CMtnTest_Move *cpMovingTest;
#include "DlgKeyInputPad.h"

int iFlagEnableEditEncResolution = 0;

void CDlgServoTuning::UI_ShowWindowsOpenLoopTest()
{
	GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->EnableWindow(TRUE); // 20111013  (CComboBox*)

	GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("OutRange(DRV%):"));
	GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_COUNTER_PASS_INDEX)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MIN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MAX)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_ENC_RESOLUTION)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->ShowWindow(TRUE); // 20121012
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->EnableWindow(iFlagEnableEditEncResolution);
}

void CDlgServoTuning::UI_ShowWindowWbAppTunePass()
{
	// Window Component Size
	GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowPos(NULL, 10, 160, 
		MAX_X_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE, 
		MAX_Y_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE, SWP_NOZORDER); // 10, 15, 720, 400,  SWP_NOMOVE
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS)->ShowWindow(TRUE);
	GetDlgItem(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE)->ShowWindow(TRUE);	
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->ShowWindow(TRUE);	
	GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->ShowWindow(TRUE);	
	GetDlgItem(IDC_STATIC_TOTAL_NUM_WIRE_SERVO_CTRL_TUNING)->ShowWindow(TRUE);	
	GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->ShowWindow(TRUE);	

	GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL)->ShowWindow(TRUE);		
//	((CComboBox *) GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL))->SetCurSel(iFlagTuneWB_ServoOption2);
}
extern char get_sys_pass_protect_level();
void CDlgServoTuning::SetUserInterfaceByTab(int iCurrTabId)
{
	CDlgKeyInputPad cDlgInputPad;

	UI_HideWindowPartsAll();
	switch(iCurrTabId)
	{
		case ((int)SERVO_AFT_VERIFY_ENCODER):
			SetUserInterfaceCheckEncoder();
			iCheckAxisEncoderGuideStage = 0;
			cFlagAxisEncoderOK = TRUE;
			UpdateCheckEncoderGuide();
			break;
		case ((int)SERVO_AFT_VERIFY_OPEN_LOOP):
			EnableTabOpenLoopTest();
			break;
		case ((int)SERVO_AFT_VERIFY_VELOCITY_LOOP):
			SetUserInterfaceVelocityLoopCheck();
			break;
		case ((int)SERVO_AFT_VERIFY_SPECTRUM):
			if(stServoControllerCommSet.Handle != ACSC_INVALID)
			{
				DisableTabOpenLoopTest();
				if(cpTestSpectrum == NULL)
				{
					cpTestSpectrum = new CMtnSpectrumTestDlg;
					cpTestSpectrum->Create(IDD_MTN_SPECTRUM_TEST_DLG);
				}	
				cpTestSpectrum->ShowWindow(1);
			}
			break;
		//case ((int)SERVO_AFT_VERIFY_RANDOM_POINTS):
		//case ((int)SERVO_AFT_VERIFY_FIXED_PATTERN):
		case ((int)SERVO_AFT_VERIFY_MOVE_TEST):
			DisableTabOpenLoopTest();
			if(stServoControllerCommSet.Handle != ACSC_INVALID)
			{
				if(cpMovingTest == NULL)
				{
					cpMovingTest = new CMtnTest_Move;
					cpMovingTest->Create(IDD_MOTION_TEST_DIALOG);
				}
				cpMovingTest->ShowWindow(1);
			}
			break;
		case (int)SERVO_AFT_VERIFY_FORCE_CTRL:  // 20110710
			if(iCurrSelAxisServoCtrlTuning == WB_AXIS_BOND_Z)
			{
				UI_ShowWindowPartsPositionForceCalibration(TRUE);
				UI_ShowWindowPartsPositionForceCalibrationVarSP(TRUE);
				if(stServoControllerCommSet.Handle != ACSC_INVALID)
				{
					UI_Wb_UpdateParameterPositionForceCalibration();
					UI_EnableWindowPartsPositionForceCalibration(TRUE);
				}
			}
//			DisableTabOpenLoopTest();
			break;
		case (int)SERVO_AFT_VERIFY_LOOPING:
			DisableTabOpenLoopTest();
			break;
		case (int)SERVO_AFT_PASS:
			// Verify PASSWORD_1 96685352
			if(stCommHandleACS != ACSC_INVALID 
				&& get_sys_pass_protect_level() != __SYSTEM_PROTECTING_ENGINEER__)  //  || stCommHandleACS == 0)
			{
				cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
				cDlgInputPad.SetInputNumber(0);
	//			cDlgInputPad.SetWindowTextA(_T("Input Password"));
				if(cDlgInputPad.DoModal() == IDOK)
				{
					if(cDlgInputPad.GetReturnNumber() == wb_mtn_tester_get_password_brightlux_6d_gxc() )  // PASSWORD_2
					{
						SetUserInterfaceTabPass();
					}
					else
					{
						// DisableTabOpenLoopTest();
					}
				}
			}
			else
			{  // ACS-System Offline mode, 20110221
				SetUserInterfaceTabPass();
			}
		default :
			break;
	}
}
void CDlgServoTuning::EnableTabOpenLoopTest()
{
	UI_ShowWindowsOpenLoopTest();

	iDriveOutRangeCurrSel = 1;  // 2%
	((CComboBox*)GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP))->SetCurSel(iDriveOutRangeCurrSel);
	UpdateLabelMaxMinRange();

	GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING)->ShowWindow(FALSE);
	
	if(iCurrSelAxisServoCtrlTuning == WB_AXIS_BOND_Z)
	{
		UI_ShowWindowPartsPositionForceCalibration(TRUE);
		if(stServoControllerCommSet.Handle != ACSC_INVALID)
		{
			UI_Wb_UpdateParameterPositionForceCalibration();
		}
	}

	UI_EnableWindowPartsPositionForceCalibration(FALSE);
	RestartTimer(); // StartTimer(IDT_SERVO_CTRL_TUNING_DLG_TIMER, 200);
}

void CDlgServoTuning::DisableTabOpenLoopTest()
{

	UI_HideWindowPartsAll();
	UI_ShowWindowPartsPositionForceCalibration(FALSE);
	UI_EnableWindowPartsPositionForceCalibration(FALSE);
	UI_ShowWindowPartsPositionForceCalibrationVarSP(FALSE);

	PauseTimer();// StopTimer();

}

void CDlgServoTuning::UI_HideWindowPartsAll()
{
	GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_COUNTER_PASS_INDEX)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MIN)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MAX)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_RMS_RATIO)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_RATIO_RMS_SERVO_CTRL_TUNING_VEL_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_ENC_RESOLUTION)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE)->ShowWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->ShowWindow(FALSE);	
	GetDlgItem(IDC_STATIC_TOTAL_NUM_WIRE_SERVO_CTRL_TUNING)->ShowWindow(FALSE);	
	GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->ShowWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING)->ShowWindow(FALSE);

	GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->ShowWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->ShowWindow(FALSE);	
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->ShowWindow(FALSE);	

	GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL)->ShowWindow(FALSE);
	GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->ShowWindow(FALSE); // 20121012

	UI_ShowWindowPartsPositionForceCalibration(FALSE);
	UI_ShowWindowPartsPositionForceCalibrationVarSP(FALSE);
}

void CDlgServoTuning::SetUserInterfaceTabPass()
{

	UI_ShowWindowWbAppTunePass();
	PauseTimer();// StopTimer();
}

void CDlgServoTuning::SetUserInterfaceCheckEncoder()
{

	UI_ShowWindowsCheckEncoder();
	RestartTimer(); // StartTimer(IDT_SERVO_CTRL_TUNING_DLG_TIMER, 200);
}

void CDlgServoTuning::UI_ShowWindowsCheckEncoder()
{
	GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->EnableWindow(FALSE); // 20111013  (CComboBox*)

	GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_COUNTER_PASS_INDEX)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_ENC_RESOLUTION)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->EnableWindow(iFlagEnableEditEncResolution);

	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN)->ShowWindow(TRUE);

	GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->ShowWindow(FALSE); // 20121012
}

void CDlgServoTuning::SetUserInterfaceVelocityLoopCheck()
{
	GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_COUNTER_PASS_INDEX)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT)->ShowWindow(FALSE);

	GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_OUTPUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(_T("OutRange(XVEL%):"));
	GetDlgItem(IDC_STATIC_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->ShowWindow(TRUE);	
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->ShowWindow(TRUE);	
	GetDlgItem(IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE)->ShowWindow(TRUE);

	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->EnableWindow(TRUE); // 20111013  (CComboBox*)
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->ShowWindow(TRUE);

	GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MIN)->ShowWindow(FALSE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MAX)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_SERVICE_PASS)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->ShowWindow(FALSE);	
	GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->ShowWindow(FALSE);	
	GetDlgItem(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE)->ShowWindow(FALSE);	
	GetDlgItem(IDC_STATIC_TOTAL_NUM_WIRE_SERVO_CTRL_TUNING)->ShowWindow(FALSE);	
	GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->ShowWindow(FALSE);	

	GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->ShowWindow(FALSE);     // 20110131
	GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->ShowWindow(FALSE);  // 20110131
	// Special for Velocity Loop Test
	GetDlgItem(IDC_STATIC_CTRL_TUNING_RMS_RATIO)->ShowWindow(TRUE);
	GetDlgItem(IDC_STATIC_CTRL_TUNING_ENC_RESOLUTION)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_RATIO_RMS_SERVO_CTRL_TUNING_VEL_LOOP)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->ShowWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->EnableWindow(iFlagEnableEditEncResolution);
	GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING)->ShowWindow(TRUE);

	GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->EnableWindow(FALSE);

	iDriveOutRangeCurrSel = 3; // 5%
	((CComboBox*)GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP))->SetCurSel(iDriveOutRangeCurrSel);

	UI_ShowWindowPartsPositionForceCalibration(FALSE);
	UI_EnableWindowPartsPositionForceCalibration(FALSE);
	UI_ShowWindowPartsPositionForceCalibrationVarSP(FALSE);

	GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_)->ShowWindow(TRUE); // 20121012

	RestartTimer(); // StartTimer(IDT_SERVO_CTRL_TUNING_DLG_TIMER, 200);
}

void CDlgServoTuning::OnNMCustomdrawServoCtrlTuningSliderDriverOut(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	UpdateActualDriveOutBySlide();
	*pResult = 0;
}

void CDlgServoTuning::InitAxisForWireBond()
{
//	SetAxisNameLanguage();
	CComboBox *pAxisComboAxisWB = (CComboBox*)GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP);
	pAxisComboAxisWB->SetCurSel(iCurrSelAxisServoCtrlTuning);
		// 20110807
	CString cstrTemp;
	cstrTemp.Format("Axis(ACS-%d): ", astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(cstrTemp);  // 20110807

}

void CDlgServoTuning::InitRangeComboOpenLoopTest()
{
	CComboBox *pAxisComboDriveOutRangeOLT = (CComboBox*)GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP);
	for(int ii=0; ii<MAX_DRIVE_OUT_RANGE_CASES; ii++)
	{
		pAxisComboDriveOutRangeOLT->InsertString(ii, _T(astRangeDriveOut[ii].strRangeText));
	}
	pAxisComboDriveOutRangeOLT->SetCurSel(iDriveOutRangeCurrSel);

}
void CDlgServoTuning::OnCbnSelchangeComboAxisServoCtrlTuningOpenLoop()
{
	iCurrSelAxisServoCtrlTuning = ((CComboBox*)GetDlgItem(IDC_COMBO_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP))->GetCurSel();
	iCountPassIndexPosn = 0;
	iCheckAxisEncoderGuideStage = 0;
	cFlagAxisEncoderOK = TRUE;
	UpdateCheckEncoderGuide();

	stVelLoopTestInput.iAxisCtrlCard = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS; // VelLoopTest
	stVelLoopTestInput.fDriveCmdRatioRMS = afRatioRMS_DrvCmd[iCurrSelAxisServoCtrlTuning];
	stVelLoopTestInput.fEncResolutionCntPerMM = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm; // [iCurrSelAxisServoCtrlTuning];
	CString cstrTemp;
	cstrTemp.Format("%8.2f", afRatioRMS_DrvCmd[iCurrSelAxisServoCtrlTuning]);
	GetDlgItem(IDC_EDIT_RATIO_RMS_SERVO_CTRL_TUNING_VEL_LOOP)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%8.1f", astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm); //[iCurrSelAxisServoCtrlTuning]);
	GetDlgItem(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING)->SetWindowTextA(cstrTemp);
	stVelLoopTestInput.strServoAxisNameWb = astrWireBondServoAxisNameEn[iCurrSelAxisServoCtrlTuning];

	// 20110807
	cstrTemp.Format("Axis(ACS-%d): ", astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS);
	GetDlgItem(IDC_STATIC_AXIS_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowTextA(cstrTemp);  // 20110807
	// Update the flags of inverting DrvOut and Encoder
	Update_UI_FlagInvertEncoderDrvOut_CtrlCard();  // 20110131
}

void CDlgServoTuning::OnCbnSelchangeComboOutRangeServoCtrlTuningOpenLoop()
{
	iDriveOutRangeCurrSel = ((CComboBox*)GetDlgItem(IDC_COMBO_OUT_RANGE_SERVO_CTRL_TUNING_OPEN_LOOP))->GetCurSel();
	UpdateActualDriveOutBySlide();

	UpdateLabelMaxMinRange();
}

void CDlgServoTuning::UpdateLabelMaxMinRange()
{
	CString cstrTemp;
	cstrTemp.Format("-%4.1f", astRangeDriveOut[iDriveOutRangeCurrSel].dRangeNum);
	cstrTemp = cstrTemp; //  + _T("%");
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MIN)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%4.1f", astRangeDriveOut[iDriveOutRangeCurrSel].dRangeNum);
	cstrTemp = cstrTemp; // + _T("%");
	GetDlgItem(IDC_STATIC_CTRL_TUNING_DRIVE_RANGE_MAX)->SetWindowTextA(cstrTemp);
	
}

// IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT
void CDlgServoTuning::UpdateActualDriveOutBySlide()
{
	int iSliderPosn = ((CSliderCtrl*)GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT))->GetPos();

	dActualDriveOut = astRangeDriveOut[iDriveOutRangeCurrSel].dRangeNum * (iSliderPosn - 50)/ 50;

	CString cstrTemp;
	cstrTemp.Format("%4.5f", dActualDriveOut);
	GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_ACT_DRIVER_OUT)->SetWindowTextA(cstrTemp);

	static int iDriveCommand;
	iDriveCommand = (int)dActualDriveOut; // (dActualDriveOut * 100);  // /100 * 32767  // iDriveCommand
	acsc_WriteReal(stCommHandleACS, 0, "DCOM", 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, &dActualDriveOut, 0); // DOUT

}
static int iDebug = 0;
static int iFlagRunningTimer = 1;
void CDlgServoTuning::PauseTimer()
{
	iFlagRunningTimer = FALSE;
}
void CDlgServoTuning::RestartTimer()
{
	iFlagRunningTimer = TRUE;
}

#include "MtnTune.h"
#include "acs_buff_prog.h"
#include <direct.h>
#include "MotAlgo_DLL.h"
void CDlgServoTuning::OnTimerRoutine()
{
		if(stServoControllerCommSet.Handle == ACSC_INVALID)  // Handle
		{
			UpdateServoCtrlUI_ACS(FALSE);
		}
		else
		{
			UpdateServoCtrlUI_ACS(TRUE);
			UpdateServoFbStatusInTimer();
		}

		if(mtn_tune_get_flag_stopping_position_compensation_thread() == TRUE)
		{
			GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->EnableWindow(FALSE);
		}

		// 20110801
		if(mtn_dll_home_get_flag_stopping_thread() == FALSE)
		{
			//if(mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_X_TABLE_ACS_ID) == FALSE 
			//	|| mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_Y_TABLE_ACS_ID) == FALSE 
			//	|| mtn_qc_is_axis_locked_safe(stCommHandleACS, sys_get_acs_axis_id_bnd_z()) == FALSE 
			//	)
			//{
			//	mtn_dll_home_stop_thread();
			//}


			GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(FALSE);
//			GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->EnableWindow(FALSE);
		}
		else
		{
			if(stServoControllerCommSet.Handle != ACSC_INVALID)
			{
				if(mtn_qc_is_axis_locked_safe(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS))
				{
					GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->EnableWindow(TRUE);
//					GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->EnableWindow(TRUE);
				}
				else
				{
					GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->EnableWindow(FALSE);
				}

				if(mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_X_TABLE_ACS_ID) == FALSE 
					|| mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_Y_TABLE_ACS_ID) == FALSE 
					|| mtn_qc_is_axis_locked_safe(stCommHandleACS, sys_get_acs_axis_id_bnd_z()) == FALSE )
				{
					
					static int iFlagMachType = get_sys_machine_type_flag();
					// Only for one case in XY station
					if( (iFlagMachType == WB_STATION_XY_VERTICAL ||
						iFlagMachType == WB_STATION_XY_TOP)
						&& 
						(mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_X_TABLE_ACS_ID) == TRUE 
							&& mtn_qc_is_axis_locked_safe(stCommHandleACS, APP_Y_TABLE_ACS_ID) == TRUE )
					  )
					{
						GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(TRUE);
					}
					else
					{
						GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(FALSE);
					}

				}
				else
				{
					GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(TRUE);
				}
			}
		}
}
void CDlgServoTuning::OnTimer(UINT nTimerVal)
{
	if(iFlagRunningTimer == TRUE)
	{
		OnTimerRoutine();
	}
}
static int iFlagFirstTimeCreatChart = 1;
void aft_vel_test_port_data_to_extern_xls_file()
{
	//if(m_pXLS_ServoTuningDlg != NULL)
	//{
	//	mtn_aft_vel_loop_add_data_into_sheet(); // m_pXLS_ServoTuningDlg
	//}
	//if(iFlagFirstTimeCreatChart && m_pXLS_ServoTuningDlg != NULL)
	//{
	//	m_pXLS_ServoTuningDlg->CreateXYChart(2);
	//	iFlagFirstTimeCreatChart = 0;
	//}

	//	if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_VELOCITY_LOOP && m_pXLS_ServoTuningDlg != NULL)
	//	{
	//		if(iFlagServoNormal && (uiCurrLoopCount % 4 == 0))
	//		{
	//			m_pXLS_ServoTuningDlg->UpdateXYChart(2);
	//		}
	//	}
}

// 
// $(ProjectDir)Release\ServoTuningWB.lib
extern int wb_mtn_tester_get_password_sg_8d_zzy();
extern int wb_mtn_tester_get_password_brightlux_6d_gxc();
extern int wb_mtn_tester_get_password_brightlux_6d_zzy();

int iFlagModeIsVerifyingPosnCompensate = FALSE;

static int iMotorFlags;
static int *piMotorFlag = &iMotorFlags;
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningOpenLoopEnable()
{
	int iMotorState;

	mtnapi_get_motor_state(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
		&iMotorState, iDebug);

	if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_OPEN_LOOP)
	{
		acsc_ReadInteger(stCommHandleACS, 0, "MFLAGS", 
			astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
			astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, piMotorFlag, 0); // DOUT
		if(iMotorState & ACSC_MST_ENABLE) 
		{
			// now is enable, to disable it
			iMotorFlags = iMotorFlags & AND_CLEAR_BIT_1;
			iMotorFlags = iMotorFlags | OR_SET_BIT_17;
		}
		else
		{
		//	OpenLoop DAC mode, MFLAGS(iAFT_Axis).1=1; MFLAGS(iAFT_Axis).17=0;\n",
			// now is disabled, to enable it
			iMotorFlags = iMotorFlags | OR_SET_BIT_1;
			iMotorFlags = iMotorFlags & AND_CLEAR_BIT_17;
		}
		acsc_WriteInteger(stCommHandleACS, 0, "MFLAGS", 
			astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
			astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, piMotorFlag, 0); // DOUT
	}

	if(iMotorState & ACSC_MST_ENABLE) 
	{
		// now is enable, to disable it
		mtnapi_disable_motor(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
			iDebug);
		InitSliderRangeDriveOutput();
	}
	else
	{
		// now is disabled, to enable it
		int iRetCheckPolarity = MTN_API_OK_ZERO;
		if(cFlagSafetyCheckOpenLoopForceAndFeedback == TRUE
			&& iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_VELOCITY_LOOP)
		{
			iRetCheckPolarity = aft_acs_routine_check_force_feedback_direction(astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS);

		}
		if(iRetCheckPolarity == MTN_API_OK_ZERO)
		{
			mtnapi_enable_motor(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
				iDebug);
		}
		else
		{
			AfxMessageBox(_T("Check driver output, CANNOT close loop"));
		}
		InitSliderRangeDriveOutput();

		if(cFlagSafetyCheckOpenLoopForceAndFeedback == TRUE
			&& iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_OPEN_LOOP)
		{
			StartServoTuneThread();
		}

	}

}

// IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_DRIVER_OUT
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningZeroDriverOut()
{
	((CSliderCtrl*)GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT))->SetPos(50);
	UpdateActualDriveOutBySlide();
}

// IDC_BUTTON_SERVO_CTRL_TUNING_ZERO_FEEDBACK_POSN
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningZeroFeedbackPosn()
{
	acsc_SetFPosition(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0);
}

void CDlgServoTuning::OnShowWindow(BOOL bShow, UINT nStatus)
{
	
	RestartTimer(); // 

	CDialog::OnShowWindow(bShow, nStatus);
}

void CDlgServoTuning::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	acsc_ReadInteger(stCommHandleACS, 0, "MFLAGS", 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, piMotorFlag, 0); // DOUT
	iMotorFlags = iMotorFlags & AND_CLEAR_BIT_1;
	iMotorFlags = iMotorFlags | OR_SET_BIT_17;

	acsc_WriteInteger(stCommHandleACS, 0, "MFLAGS", 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
		astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, piMotorFlag, 0); // DOUT

	PauseTimer();// KillTimer(m_iTimerVal);

	//if(NULL != m_pXLS_ServoTuningDlg)
	//{
	//	m_pXLS_ServoTuningDlg->ReleaseExcel();
	//	delete m_pXLS_ServoTuningDlg;
	//	m_pXLS_ServoTuningDlg = NULL;
	//}

	OnOK();
}

static int iSumCounterPassIndex2ndPress;
static int iSumCounterPassIndex3rdPress;
static int iSumCounterPassIndex4thPress;
static int iSumCounterPassIndex5thPress;
static double dPrevIndexPosition2ndPress;
static double dPrevIndexPosition3rdPress;
static double dPrevIndexPosition4thPress;
static double dPrevIndexPosition5thPress;
void CDlgServoTuning::ResetCheckEncIdxCounterReg()
{
	iSumCounterPassIndex2ndPress = 0;
	iSumCounterPassIndex3rdPress = 0;
	iSumCounterPassIndex4thPress = 0;
	iSumCounterPassIndex5thPress = 0;
	dPrevIndexPosition2ndPress = 0;
	dPrevIndexPosition3rdPress = 0;
	dPrevIndexPosition4thPress = 0;
	dPrevIndexPosition5thPress = 0;
}

CString aszCheckEncoderErrorLossCount[2] =
{
	_T("Check Encoder Alignment, Encoder Loss Count \r\n Enter <OpenLoop> or <VelLoop> and select next testing axis"),
	_T("检查编码器对准，计数遗失  \r\n 从【开环测试】或【调速度环】进入，选择下一个测试轴")
};

CString aszCheckEncoderNormalLowSpeedPass[2] =
{
	_T("PASS: Low Speed Check Encoder, OK to ShowReport \r\n Enter <OpenLoop> or <VelLoop> and select next testing axis \r\n After completing velocity step response, in <VelLoop> Click <GenReport> will generate AFT Report"),
	_T("编码器通过低速检查, OK则显示报告 \r\n 从【开环测试】或【调速度环】进入，选择下一个测试轴  \r\n 完毕后，在【调速度环】页面点击【装机报告】生成报告")
};

#include "MtnAft.h"
static AFT_CHECKING_AXIS astAssemblyFuncTestCheckingAxis[MAX_SERVO_AXIS_WIREBOND];
static double dLowerLimitPosn_cnt, dUpperLimitPosn_cnt;
void aft_axis_update_limit_position(int iAxis)
{
	double dTempCurrLimitPosn;
	mtnapi_get_fb_position(stCommHandleACS, astAxisInfoWireBond[iAxis].iAxisInCtrlCardACS, &dTempCurrLimitPosn, iDebug);
	if(dTempCurrLimitPosn > 0)
	{
		if(dTempCurrLimitPosn > dUpperLimitPosn_cnt)
		{
			dUpperLimitPosn_cnt = dTempCurrLimitPosn;
		}
	}
	else
	{
		if(dTempCurrLimitPosn < dLowerLimitPosn_cnt)
		{
			dLowerLimitPosn_cnt = dTempCurrLimitPosn;
		}
	}

}
extern CString aft_gen_report_string();
// IDC_BUTTON_SERVO_CTRL_TUNING_GET_ENC_IDX_ONCE
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetEncIdxOnce()
{
	double dAverageIndexPosn;
	iCheckAxisEncoderGuideStage ++;
	UpdateCheckEncoderGuide();
	if(iCheckAxisEncoderGuideStage == 1)
	{
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistIndexUppLow_cnt = 0;
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistLimitToLimit_cnt = 0;
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistLowLimitToIndex_cnt = 0;
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistUppLimitToIndex_cnt = 0;
		dLowerLimitPosn_cnt = 0;
		dUpperLimitPosn_cnt = 0;
		GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->ShowWindow(FALSE);
	}
	else if(iCheckAxisEncoderGuideStage == 2)
	{
		iSumCounterPassIndex2ndPress = iCountPassIndexPosn; // dIndexPositionCurr
		dPrevIndexPosition2ndPress = dIndexPositionCurr;
		aft_axis_update_limit_position(iCurrSelAxisServoCtrlTuning);

	}
	else if(iCheckAxisEncoderGuideStage == 3)
	{
		iSumCounterPassIndex3rdPress = iCountPassIndexPosn;
		dPrevIndexPosition3rdPress = dIndexPositionCurr;
		if(iSumCounterPassIndex3rdPress < iSumCounterPassIndex2ndPress + 1)
		{
			cFlagAxisEncoderOK = FALSE;
			AfxMessageBox(aszCheckEncoderErrorIndexAlignment[m_iLanguageOption]);
		}
		aft_axis_update_limit_position(iCurrSelAxisServoCtrlTuning);

	}
	else if(iCheckAxisEncoderGuideStage == 4)
	{
		iSumCounterPassIndex4thPress = iCountPassIndexPosn;
		dPrevIndexPosition4thPress = dIndexPositionCurr;
		if(iSumCounterPassIndex4thPress < iSumCounterPassIndex3rdPress + 1)
		{
			cFlagAxisEncoderOK = FALSE;
			AfxMessageBox(aszCheckEncoderErrorIndexAlignment[m_iLanguageOption]);
		}
		aft_axis_update_limit_position(iCurrSelAxisServoCtrlTuning);
	}
	else if(iCheckAxisEncoderGuideStage == 5)
	{
		iSumCounterPassIndex5thPress = iCountPassIndexPosn;
		dPrevIndexPosition5thPress = dIndexPositionCurr;
		if(iSumCounterPassIndex5thPress < iSumCounterPassIndex4thPress + 1)
		{
			cFlagAxisEncoderOK = FALSE;
			AfxMessageBox(aszCheckEncoderErrorIndexAlignment[m_iLanguageOption]);
			ResetCheckEncIdxCounterReg();
		}
		if( (fabs(dPrevIndexPosition5thPress - dPrevIndexPosition3rdPress) > 5)
			|| (fabs(dPrevIndexPosition4thPress - dPrevIndexPosition2ndPress) > 5)
			)
		{
			cFlagAxisEncoderOK = FALSE;
			AfxMessageBox(aszCheckEncoderErrorLossCount[m_iLanguageOption]);
		}
		aft_axis_update_limit_position(iCurrSelAxisServoCtrlTuning);
	}
	else
	{
		int iRetMsgBox;
		if(cFlagAxisEncoderOK)
		{
			iRetMsgBox = AfxMessageBox(aszCheckEncoderNormalLowSpeedPass[m_iLanguageOption], MB_OKCANCEL);
		}
		dAverageIndexPosn = (dPrevIndexPosition5thPress + dPrevIndexPosition3rdPress + dPrevIndexPosition4thPress + dPrevIndexPosition2ndPress)/4.0;
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistIndexUppLow_cnt = 
			(fabs(dPrevIndexPosition5thPress - dPrevIndexPosition4thPress) + fabs(dPrevIndexPosition3rdPress - dPrevIndexPosition2ndPress))/2.0;
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistLimitToLimit_cnt = fabs(dLowerLimitPosn_cnt - dUpperLimitPosn_cnt);
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistLowLimitToIndex_cnt = fabs(dLowerLimitPosn_cnt - dAverageIndexPosn);
		astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning].dDistUppLimitToIndex_cnt = fabs(dUpperLimitPosn_cnt - dAverageIndexPosn);

		aft_axis_calc_output_from_checking(iCurrSelAxisServoCtrlTuning, &astAssemblyFuncTestCheckingAxis[iCurrSelAxisServoCtrlTuning]);
		aft_axis_verify_output_by_spec(iCurrSelAxisServoCtrlTuning);
		CString cstrAftReport = aft_gen_report_string();

		if(cFlagAxisEncoderOK == TRUE && iRetMsgBox == IDOK) // IDYES MB_OK
		{
			GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrAftReport);
			GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowPos(NULL, 10, 160, 
				MAX_X_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE, 
				MAX_Y_IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE, 
				SWP_NOZORDER);	
			GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->ShowWindow(TRUE);
		}

	}
	if(cFlagAxisEncoderOK == FALSE)
	{
		ResetCheckEncIdxCounterReg();
		iCheckAxisEncoderGuideStage = 0;
		UpdateCheckEncoderGuide();
//		cFlagAxisEncoderOK = TRUE;  // 2010Nov19
	}
}

void CDlgServoTuning::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}

// IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP
void CDlgServoTuning::OnEnChangeEdit1stPosnServoCtrlVelLoop()
{
	ReadIntegerFromEdit(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP, &(stVelLoopTestInput.iPositionLower)); //  // VelLoopTest
}
// IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP
void CDlgServoTuning::OnEnChangeEdit2ndPosnServoCtrlVelLoop()
{
	ReadIntegerFromEdit(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP, &(stVelLoopTestInput.iPositionUpper)); //  // VelLoopTest
}
// IDC_BUTTON_READ_1ST_POSN_SERVO_CTRL_VEL_LOOP
void CDlgServoTuning::OnBnClickedButtonRead1stPosnServoCtrlVelLoop()
{
	ReadIntegerFromEdit(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP, &(stVelLoopTestInput.iPositionLower)); //  // VelLoopTest
	CString cstrTemp;
	cstrTemp.Format("%d", stVelLoopTestInput.iPositionLower);
	GetDlgItem(IDC_EDIT_1ST_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(cstrTemp);
}
// IDC_BUTTON_READ_2ND_POSN_SERVO_CTRL_VEL_LOOP
void CDlgServoTuning::OnBnClickedButtonRead2ndPosnServoCtrlVelLoop()
{
	ReadIntegerFromEdit(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP, &(stVelLoopTestInput.iPositionUpper)); //  // VelLoopTest
	CString cstrTemp;
	cstrTemp.Format("%d", stVelLoopTestInput.iPositionUpper);
	GetDlgItem(IDC_EDIT_2ND_POSN_SERVO_CTRL_VEL_LOOP)->SetWindowTextA(cstrTemp);
}

// IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP
void CDlgServoTuning::OnBnClickedButtonStartTestPosnServoCtrlVelLoop()
{
static double dMaxVel;

	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		acsc_ReadReal(stCommHandleACS, 0, "XVEL", 
			stVelLoopTestInput.iAxisCtrlCard, 
			stVelLoopTestInput.iAxisCtrlCard, 0, 0, &dMaxVel, 0); // DOUT

		stVelLoopTestInput.fMaxAbsVel = (float)(astRangeDriveOut[iDriveOutRangeCurrSel].dRangeNum * dMaxVel/100);

		StartServoTuneThread();
	}
}

void CDlgServoTuning::OnEnChangeEditTotalNumLoopServoCtrlVelLoop()
{
	ReadIntegerFromEdit(IDC_EDIT_TOTAL_NUM_LOOP_SERVO_CTRL_VEL_LOOP, (int*)&(stVelLoopTestInput.uiLoopNum)); //  // VelLoopTest
}

extern int mtn_acs_routine_check_axis_force_feedback_direction(HANDLE hHandle, int iAcsAxis);
int aft_acs_routine_check_force_feedback_direction(int iAcsAxis)
{
	return mtn_acs_routine_check_axis_force_feedback_direction(stCommHandleACS, iAcsAxis);
}

UINT ServoTuningThreadInDialog( LPVOID pParam )
{
    CDlgServoTuning* pObject = (CDlgServoTuning *)pParam;
	return pObject->DlgServoTuningThread(); 	
}

void CDlgServoTuning::StartServoTuneThread()
{
	cFlagDlgServoTuningThreadRunning = TRUE;
	pDlgServoTuningWinThread = AfxBeginThread(ServoTuningThreadInDialog, this); // , );
	pDlgServoTuningWinThread->m_bAutoDelete = FALSE;
}

extern void mtn_update_burn_in_position_limit_wb_axis(int iWbAxis, int iLowerLimit, int iUpperLimit); // 20130208

UINT CDlgServoTuning::DlgServoTuningThread()
{
static AFT_VEL_LOOP_TEST_OUTPUT stVelLoopTestOutput;
//char strSystemCommand[128];
int iAcsAxis, iRetCheckPolarity;
CString cstrTemp;
double dUppLimitPosn, dLowLimitPosn, dMiddlePosn, dEncResolution_CntPerMM;
int iOpenLoopCheckPolarityStartFlag;
	switch(iCurrServoCtrlTuneTabId)
	{
	case SERVO_AFT_VERIFY_OPEN_LOOP:  // 20121012
		//prompt to locate object at center of stroke
		UpdateServoCtrlUI_ACS(FALSE);
		iOpenLoopCheckPolarityStartFlag = AfxMessageBox(_T("Please move object at center of stroke"), MB_YESNO);

		if(iOpenLoopCheckPolarityStartFlag == IDYES)
		{
			iAcsAxis = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
			iRetCheckPolarity = aft_acs_routine_check_force_feedback_direction(iAcsAxis);
			if(iRetCheckPolarity == MTN_API_OK_ZERO)
			{
				AfxMessageBox(_T("Polarity Check OK"));
			}
			else if(iRetCheckPolarity == MTN_API_ERROR_WRONG_POLARITY)
			{
				AfxMessageBox(_T("Polarity Error"));
			}
			else
			{
				AfxMessageBox(_T("Driver has no force output"));
			}
		}
		UpdateServoCtrlUI_ACS(TRUE);

		break;
	case SERVO_AFT_VERIFY_VELOCITY_LOOP:
//		if(NULL != m_pXLS_ServoTuningDlg)
//		{
//			m_pXLS_ServoTuningDlg->CreateXYChart(2);
//			m_pXLS_ServoTuningDlg = new CXLEzAutomation();  // (FALSE); // FALSE not visible
//		}
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->ShowWindow(TRUE);
//		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(_T(""));
		iAcsAxis = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
		dEncResolution_CntPerMM = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm;

#define   DEF_DISTANCE_VEL_STEP_TEST_20MM   (20.0)
			if(iAcsAxis !=MAX_CTRL_AXIS_PER_SERVO_BOARD)
			{
				if(aft_teach_upper_limit(stCommHandleACS, iAcsAxis, 0, &dUppLimitPosn) == MTN_API_OK_ZERO)
				{
				}
				else
				{
					goto label_return_thread_servo_tuning;
				}

				if(aft_teach_lower_limit(stCommHandleACS, iAcsAxis, 0, &dLowLimitPosn) == MTN_API_OK_ZERO)
				{
					
				}
				else
				{
					goto label_return_thread_servo_tuning;
				}
				stVelLoopTestInput.iPositionUpper = (int)(dUppLimitPosn - (dUppLimitPosn - dLowLimitPosn)/4.0);
				stVelLoopTestInput.iPositionLower = (int)(dLowLimitPosn + (dUppLimitPosn - dLowLimitPosn)/4.0);
				dMiddlePosn = (dUppLimitPosn + dLowLimitPosn)/2.0; // 20130203
				if((stVelLoopTestInput.iPositionUpper - stVelLoopTestInput.iPositionLower) / dEncResolution_CntPerMM 
					>= DEF_DISTANCE_VEL_STEP_TEST_20MM) // 20mm
				{
					stVelLoopTestInput.iPositionUpper = (int)(dMiddlePosn + dEncResolution_CntPerMM * DEF_DISTANCE_VEL_STEP_TEST_20MM/2.0);
					stVelLoopTestInput.iPositionLower = (int)(dMiddlePosn - dEncResolution_CntPerMM * DEF_DISTANCE_VEL_STEP_TEST_20MM/2.0);
				}

				acsc_ToPoint(stCommHandleACS, 0, // start up the motion immediately
						iAcsAxis, dMiddlePosn, NULL);
				CString cstrTemp;
				cstrTemp.Format("Upper Limit: %8.1f, Lower Limit: %8.1f, TestBetween: [%d, %d]", 
					dUppLimitPosn, dLowLimitPosn, stVelLoopTestInput.iPositionLower, stVelLoopTestInput.iPositionUpper);
				mtn_update_burn_in_position_limit_wb_axis(iCurrSelAxisServoCtrlTuning, (int)dLowLimitPosn, (int)dUppLimitPosn); // 20130208

				AfxMessageBox(cstrTemp);
			}
extern char strVeLoopTestNewFolderName[128];

		stVelLoopTestInput.iFlagSaveFile = iFlagPlotDlgServoCtrlTuning;  // 20100923
		sprintf_s(strVeLoopTestNewFolderName, 128, ".\\");  // 20110722

		mtn_aft_vel_loop_test(stCommHandleACS, &stVelLoopTestInput, &stVelLoopTestOutput);
		cstrTemp.Format("Mean-PeakAcc: %5.1f;", 
			stVelLoopTestOutput.fAveMaxAccEstimateFullDAC_mpss); // stVelLoopTestOutput.fMaxAccEstimateFullDAC_mpss
		// afMaxFdAccAtFullDriveCmd[0], afMinFdAccAtFullDriveCmd[0]
		//if(iCurrServoCtrlTuneTabId == SERVO_AFT_VERIFY_VELOCITY_LOOP && m_pXLS_ServoTuningDlg != NULL)
		//{
		//	m_pXLS_ServoTuningDlg->UpdateXYChart(2);
		//}
		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(cstrTemp);
//		GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->ShowWindow(FALSE);

//		
//		if(iFlagPlotDlgServoCtrlTuning)
//		{
//			Sleep(1500);
//			sprintf_s(strSystemCommand, 128, "notepad %s", stVelLoopTestOutput.strOutputFilename);
//			sprintf_s(strSystemCommand, 128, ".\\Leetro2812\\VcDemo3");
//			system(strSystemCommand);
//		}

		break;
	default :
		break;
	}

label_return_thread_servo_tuning:
	cFlagDlgServoTuningThreadRunning = FALSE;

	return 0;
}
// IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING
void CDlgServoTuning::OnBnClickedCheckFlagPlotServoCtrlTuning()
{
	iFlagPlotDlgServoCtrlTuning = ((CButton*)GetDlgItem(IDC_CHECK_FLAG_PLOT_SERVO_CTRL_TUNING))->GetCheck();
}

#include "MtnTesterEntry.h"
extern MtnTesterEntry cMainDialog;  // void MtnTesterEntry_EnableProtectButtons();

extern struct tm *stpTimeGlobalVar;
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetServicePass()
{
	CDlgKeyInputPad cDlgInputPad;
	cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
	cDlgInputPad.SetInputNumber(0);
//	cDlgInputPad.SetWindowTextA(_T("Input Password"));
	if(cDlgInputPad.DoModal() == IDOK)
	{
		if(cDlgInputPad.GetReturnNumber() == wb_mtn_tester_get_password_brightlux_6d_zzy() )
		{
			int iPasswordService;//
			CString cstrTemp;

				iPasswordService = (stpTimeGlobalVar->tm_mday + 60) * (stpTimeGlobalVar->tm_mon + 1 + 60);
				cstrTemp.Format("Service Pass: %d, Bef6.25", iPasswordService);
				AfxMessageBox(cstrTemp) ;
				//if( == IDNO) MB_YESNO
				//{
				//  int iPasswordEngineer;
				//	iPasswordEngineer = wb_security_calc_eng_pass_from_service(iPasswordService);
				//	cstrTemp.Format("Engineer. Pass: %d", iPasswordEngineer);
				//	AfxMessageBox(cstrTemp);

//					cMainDialog.EnableProtectButtons(); // MtnTesterEntry_EnableProtectButtons();
				//}
			sys_set_flag_debug_mode(1);
			iFlagEnableEditEncResolution = 1;
		}
	}
}

#include "MtnVelStepTestDlg.h"
extern CMtnVelStepTestDlg *cpVelStepGroupTest;

// IDC_BUTTON_SERVO_CTRL_GROUP_TEST
void CDlgServoTuning::OnBnClickedButtonServoCtrlGroupTest()
{
	if(stServoControllerCommSet.Handle != ACSC_INVALID)
	{
		if(cpVelStepGroupTest == NULL)
		{
			cpVelStepGroupTest = new CMtnVelStepTestDlg();
			cpVelStepGroupTest->Create(IDD_DLG_GROUP_VEL_STEP_TEST);
//			cpVelStepGroupTest = NULL;
		}
		cpVelStepGroupTest->ShowWindow(1);
	}
}

// Buffer Program #0, IntData(33) = 1 <=> enable debug
// IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE
void CDlgServoTuning::OnBnClickedCheckServoCtrlTuningEnaWaveform()
{
	iEnaFlagSaveFileWbWaveformPerformance = ((CButton *) GetDlgItem(IDC_CHECK_SERVO_CTRL_TUNING_ENA_SAVEFILE))->GetCheck();
}

extern WB_ONE_WIRE_PERFORMANCE_CALC astWbOneWirePerformance[NUM_TOTAL_WIRE_IN_ONE_SCOPE];

// static MULTI_WIRE_PERFORMANCE_STATISTIC astWbMWBondHeadPerformance[]
static MULTI_WIRE_WB_PERFORMANCE stWbMultiWirePerformStat;
static int nTotalNumWire = 0;
extern char *astrTextContactOrNot[];
extern char *astrTextBondWireOrBall[];
extern char *astrMachineTypeNameLabel_en[];

// IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningGetWbPerformIdx()
{
//WB_ONE_WIRE_PERFORMANCE_CALC stWbWaveformPerformanceCalc;
CString cstrWbWaveformPerformIndex;
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
char strPathFoldername[512];  // 20110520
char strFilenameWbPerformIndex[1024];
char strFilenameWbWaveform[256];
FILE *fptr;
int iRet = MTN_API_ERROR;
char strCommandCopyFileToFolder[1024];

		char strFilePathnameOffLineSimu[256], strFilenameOffLineSimu[256];
	int iMechCfg = get_sys_machine_type_flag();

	CFileDialog cLoadWaveform(TRUE, NULL, "*.m", OFN_HIDEREADONLY,"Waveform Files(*.*)|*.m||*.dat",NULL); // 20111113

	nTotalNumWire = 0;
	if(stCommHandleACS == ACSC_INVALID)  //  || stCommHandleACS == 0)
	{
		
		int   iResult   = (int)cLoadWaveform.DoModal();
		unsigned int uiDataLen;
		sprintf_s(strFilePathnameOffLineSimu, 256, "%s", cLoadWaveform.GetPathName().GetString());
		sprintf_s(strFilenameOffLineSimu, 256, "%s", cLoadWaveform.GetFileName().GetString()); // 20110520
		if( iResult == IDOK)
		{
			iRet = mtn_debug_load_waveform_from_file(strFilePathnameOffLineSimu, &uiDataLen);
		}
	}
	else
	{
		mtnapi_upload_wb_servo_speed_parameter_acs(stCommHandleACS); // stServoControllerCommSet.Handle);
		iRet = mtn_tune_upload_latest_wb_waveform(stCommHandleACS);
	}
	if(iRet == MTN_API_OK_ZERO)
	{
		mtn_tune_ana_wb_waveform(&astWbOneWirePerformance[0], &nTotalNumWire);
//
		if(iEnaFlagSaveFileWbWaveformPerformance)
		{
			_time64(&stLongTime);
			_localtime64_s(stpTime, &stLongTime);
				// Make a folder, 20110520
			sprintf_s(strPathFoldername, 512, "D:\\MT\\%s-%d_WbParaWaveform_Y%d.M%d.D%d_H%d.m%d", 
					astrMachineTypeNameLabel_en[iMechCfg],
					get_sys_machine_serial_num(),
					stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);
			_mkdir(strPathFoldername);
		}

		if(nTotalNumWire >= 8 || nTotalNumWire <= 0) // Double verify the data-check-sum
		{
			cstrWbWaveformPerformIndex.Format("ERROR data, do bonding and capture data again");
			GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrWbWaveformPerformIndex);
		}
		else
		{
			mtn_tune_ana_wb_multi_wire_stat(&stWbMultiWirePerformStat, astWbOneWirePerformance, nTotalNumWire);

	//		mtn_tune_calc_performance_wb_waveform(&astWbOneWirePerformance[0]);
			for(int ii =0; ii< nTotalNumWire; ii++)
			{
				mtn_tune_get_wb_bondhead_tuning_position_set(ii, &(astWbOneWirePerformance[ii].stPosnsOfBondHeadZ));
				mtn_tune_get_wb_table_tuning_position_set(ii, &(astWbOneWirePerformance[ii].stPosnsOfTable));
			}

			double dCtrlSampleTime_ms = sys_get_controller_ts();
			cstrWbWaveformPerformIndex.Format("TotalWire = %d, fMeanTimeB1W_ms = %3.1f; \r\n", 
				nTotalNumWire, stWbMultiWirePerformStat.dMeanTimeB1W_cnt * dCtrlSampleTime_ms);
			cstrWbWaveformPerformIndex.Append(mtn_wb_tune_multi_wire_perform(&stWbMultiWirePerformStat));
			for(int ii = 0; ii<nTotalNumWire; ii++)
			{
				cstrWbWaveformPerformIndex.AppendFormat("%%%%====%%%%  WireNo.%d, %s, %s, B1W_Time_ms: %3.1f\r\n", 
					ii, astrTextContactOrNot[astWbOneWirePerformance[ii].iFlagIsDryRun], // 20110531
					astrTextBondWireOrBall[astWbOneWirePerformance[ii].iFlagBallBond], 
					astWbOneWirePerformance[ii].nTimeB1W_cnt * dCtrlSampleTime_ms);
				cstrWbWaveformPerformIndex.Append(mtn_tune_output_performance_string(&astWbOneWirePerformance[ii])); // stWbWaveformPerformanceCalc));
			}

//			GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrWbWaveformPerformIndex);
			InitComboShowWbPerformOpt(nTotalNumWire);
			UI_ShowPerformOptServoCtrlTuningWbApp();
			((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP))->SetCurSel(iOptionShownWbWaveformPerformance);

//			mtn_tune_calc_bond_prog_by_one_wire_info(&astWbOneWirePerformance[0]);
			///////////// Save performance index
			if(stCommHandleACS == ACSC_INVALID)
			{
				sprintf_s(strFilenameWbPerformIndex, 1024, "%s\\%s.txt", strPathFoldername, strFilenameOffLineSimu);  // strFilePathnameOffLineSimu
			}
			else
			{
				sprintf_s(strFilenameWbPerformIndex, 1024, "%s\\Wb_%d-%d-%d_%d-%d-%d.txt", 
					strPathFoldername, stpTime->tm_year +1900, stpTime->tm_mon +1, 
					stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
			}
			fopen_s(&fptr, strFilenameWbPerformIndex, "w");
			if(fptr != NULL)
			{
				fprintf_s(fptr, "%s", cstrWbWaveformPerformIndex.GetString());
				fclose(fptr);
			}
			if(iFlagPlotDlgServoCtrlTuning)
			{
				static char strSystemCommand[128];
				Sleep(500);
				sprintf_s(strSystemCommand, 128, "notepad %s", strFilenameWbPerformIndex);
				system(strSystemCommand);
			}
		}
		if(iEnaFlagSaveFileWbWaveformPerformance)
		{
//				if(stCommHandleACS != ACSC_INVALID)
//				{
			if(nTotalNumWire >= 4 || nTotalNumWire <= 0)
			{
				sprintf_s(strFilenameWbWaveform, 256, "%s\\ErrWvfm", strPathFoldername);
			}
			else
			{
				if(astWbOneWirePerformance[0].iFlagIsDryRun == 1)
				{
					sprintf_s(strFilenameWbWaveform, 256, "%s\\WvfmDryRun", strPathFoldername);
				}
				else
				{
					sprintf_s(strFilenameWbWaveform, 256, "%s\\WvfmBond", strPathFoldername);
				}				
			}
			//mtn_tune_save_waveform(strFilenameWbWaveform); //
			mtn_wb_save_waveform_with_path(strFilenameWbWaveform); //
//				}
			Sleep(500);
			mtnapi_dll_save_wb_speed_servo_parameter_acs(stCommHandleACS);
			if(_chdir("C:\\WbData\\ParaBase\\") == FALSE) // 0: OK, folder exist
			{			
				sprintf_s(strCommandCopyFileToFolder, "%s\\ServoMaster.ini", strPathFoldername); // 20130127
				CopyFile("C:\\WbData\\ParaBase\\ServoMaster.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_x.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_x.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_y.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_y.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_z.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_z.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\ctrl_acsc_w.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\ctrl_acsc_w.ini", strCommandCopyFileToFolder, 0);
				//
				sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_x.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_x.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_y.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_y.ini", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\MtnTune_acs_z.ini", strPathFoldername);
				CopyFile("C:\\WbData\\ParaBase\\MtnTune_acs_z.ini", strCommandCopyFileToFolder, 0);
				// 20110801
				sprintf_s(strCommandCopyFileToFolder, "%s\\McData.dat", strPathFoldername);
				CopyFile("C:\\WbData\\McData.dat", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\PowerOn.dat", strPathFoldername);
				CopyFile("C:\\WbData\\PowerOn.dat", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\CtrlData.dat", strPathFoldername);
				CopyFile("C:\\WbData\\CtrlData.dat", strCommandCopyFileToFolder, 0);
				sprintf_s(strCommandCopyFileToFolder, "%s\\WhData.dat", strPathFoldername);
				CopyFile("C:\\WbData\\WhData.dat", strCommandCopyFileToFolder, 0);
				//
				sprintf_s(strCommandCopyFileToFolder, "%s\\McConfig.ini", strPathFoldername);
				CopyFile("C:\\WbData\\McConfig.ini", strCommandCopyFileToFolder, 0);
			}
		}


		CString cstrTemp;
		if(stCommHandleACS == ACSC_INVALID)  //  || stCommHandleACS == 0)
		{
			 cstrTemp.Format("Wires:%d, %s, %s", nTotalNumWire, astrTextContactOrNot[astWbOneWirePerformance[0].iFlagIsDryRun],
				&strFilenameOffLineSimu[cLoadWaveform.GetFileName().GetLength() - 20]); // 20110602
		}
		else
		{
			cstrTemp.Format("Wires:%d, %s", nTotalNumWire, astrTextContactOrNot[astWbOneWirePerformance[0].iFlagIsDryRun]); // 20110602
		}
		GetDlgItem(IDC_STATIC_TOTAL_NUM_WIRE_SERVO_CTRL_TUNING)->SetWindowTextA(cstrTemp);	
		GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->ShowWindow(TRUE);	

	}
	else
	{
		cstrWbWaveformPerformIndex.Format("ERROR data, do bonding (Enable Waveform) and capture data again");
		GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrWbWaveformPerformIndex);
		GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->ShowWindow(FALSE);	

#ifdef  __DEBUG__
		cstrWbWaveformPerformIndex.Format("aaaaaaaaaaaaaaaa \r\n bbbbbbbbbbbb, \r\n test");
		//GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowPos(NULL, 20, 20, 500, 350, SWP_NOMOVE);	
		GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrWbWaveformPerformIndex);
		if(iEnaFlagSaveFileWbWaveformPerformance)
		{
			_time64(&stLongTime);
			_localtime64_s(stpTime, &stLongTime);
			sprintf_s(strFilenameWbPerformIndex, 64, "Wb%d_%d_%d_%d-%d-%d.txt", stpTime->tm_year, stpTime->tm_mon, 
				stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
			fptr = fopen(strFilenameWbPerformIndex, "w");
			if(fptr != NULL)
			{
				fprintf_s(fptr, "%s", cstrWbWaveformPerformIndex.GetString());
			}
			fclose(fptr);
		}
#endif  // __DEBUG__
	}

}

// IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL
void CDlgServoTuning::OnCbnSelchangeComboServoCtrlTuningWbDebugLevel()
{
	iFlagTuneWB_ServoOption2 = 	((CComboBox *) GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL))->GetCurSel();

	UI_ShowPerformOptServoCtrlTuningWbApp();
}

void CDlgServoTuning::InitComboShowWbPerformOpt_2()
{
	CComboBox *pWbAppPerformCheckCombo_2 =((CComboBox *) GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL));
	pWbAppPerformCheckCombo_2->ResetContent();

	int ii;
	for(ii = 0; ii<= __IDX_COMBO_WB_PERFORM_MULTI_ALL; ii++)
	{
		pWbAppPerformCheckCombo_2->InsertString(ii, astrComboShowWbPerformOption[ii]);
	}

	((CComboBox *) GetDlgItem(IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL))->SetCurSel(iFlagTuneWB_ServoOption2);

}

static char strDebugMessage[512];
static char strDebugText[768];
extern int mtester_fb_search_home_acs_axis(HANDLE stCommHandleACS, int iAcsAxis); // Implemented in MotionTester "MtnDialog_FbMonitor.cpp"
extern void mtnapi_get_debug_message_home_acs(char strDebugMessageHomeAcs[512]);
void mtnapi_get_debug_message_ptr_home_acs(char *ptrDebugMessageHomeAcs);

#define HOME_AXIS_TABLE_X 0
#define HOME_AXIS_TABLE_Y 1
#define HOME_AXIS_BND_Z   4 

// IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningHomeCurrAxis()
{
	int iAcsAxis = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;

//	if(mtester_fb_search_home_acs_axis(stCommHandleACS, iAcsAxis))
//	{
	int iMachType = get_sys_machine_type_flag();
	if(iMachType == WB_MACH_TYPE_VLED_FORK)
	{
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iMachType == WB_MACH_TYPE_HORI_LED 
		|| iMachType  == WB_STATION_XY_TOP
		|| iMachType == BE_WB_HORI_20T_LED)  // 20120731
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iMachType  == BE_WB_ONE_TRACK_18V_LED)  // 20120801
	{
		mtn_dll_init_def_para_search_index_18v_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else  // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
	{
		mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	

// For XY, BH need to go to fire level first
		if((iCurrSelAxisServoCtrlTuning == WB_AXIS_TABLE_X|| iCurrSelAxisServoCtrlTuning == WB_AXIS_TABLE_Y)
			&& iMachType != WB_STATION_XY_VERTICAL
			&& iMachType != WB_STATION_XY_TOP
			)
		{
			acsc_ToPoint(stCommHandleACS, 0, // start up the motion immediately
				sys_get_acs_axis_id_bnd_z(), 
				mtn_wb_init_bh_relax_position_from_sp(stCommHandleACS), // mtn_dll_init_home_get_z_bondhead_firelevel(), 
				NULL);
			while(mtn_qc_is_axis_still_moving(stCommHandleACS, sys_get_acs_axis_id_bnd_z()) == TRUE){		Sleep(10);}
		}

	mtn_dll_home_set_acs_ctrl_handle(stCommHandleACS);
	switch(iCurrSelAxisServoCtrlTuning)
	{
	case WB_AXIS_TABLE_X:
		mtn_dll_home_set_current_axis(HOME_AXIS_TABLE_X);
		break;
	case WB_AXIS_TABLE_Y:
		mtn_dll_home_set_current_axis(HOME_AXIS_TABLE_Y);
		break;
	case WB_AXIS_BOND_Z:
		mtn_dll_home_set_current_axis(HOME_AXIS_BND_Z);
		break;
	default:
		mtn_dll_home_set_current_axis(HOME_AXIS_TABLE_X);
		break;
	}
CTRL_PARA_ACS stBakupServoControlPara;
MTN_SPEED_PROFILE stBakupParaSpeedProf;

	mtnapi_upload_servo_parameter_acs_per_axis(stCommHandleACS, iAcsAxis, &stBakupServoControlPara); // 20130128
	mtnapi_get_speed_profile(stCommHandleACS, iAcsAxis, &stBakupParaSpeedProf, 0);  // 20130128

	mtn_dll_trigger_axis_home_thread();
	Sleep(100);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(FALSE);

	while(mtn_dll_home_get_flag_stopping_thread() == FALSE)
	{
		Sleep(100);
		if(mtn_qc_is_axis_locked_safe(stCommHandleACS, iAcsAxis) == FALSE)
		{
			mtn_dll_home_stop_thread();
		}
	}

	mtnapi_download_servo_parameter_acs_per_axis(stCommHandleACS, iAcsAxis, &stBakupServoControlPara);// 20130128
	mtnapi_set_speed_profile(stCommHandleACS, iAcsAxis, &stBakupParaSpeedProf, 0);  // 20130128

	if(iCurrSelAxisServoCtrlTuning == WB_AXIS_BOND_Z
			&& iMachType != WB_STATION_XY_VERTICAL
			&& iMachType != WB_STATION_XY_TOP)
	{
		acsc_ToPoint(stCommHandleACS, 0, // start up the motion immediately
				sys_get_acs_axis_id_bnd_z(), 
				mtn_wb_init_bh_relax_position_from_sp(stCommHandleACS), // mtn_dll_init_home_get_z_bondhead_firelevel(), 
				NULL);
		while(mtn_qc_is_axis_still_moving(stCommHandleACS, sys_get_acs_axis_id_bnd_z()) == TRUE){		Sleep(10);}
	}
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(TRUE);

}
// IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ
void CDlgServoTuning::OnBnClickedButtonServoCtrlTuningHomeWbXyz()
{
	int iMachType = get_sys_machine_type_flag();
	if(iMachType == WB_MACH_TYPE_VLED_FORK)
	{
		mtn_dll_init_def_para_search_index_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iMachType == WB_MACH_TYPE_HORI_LED 
		|| iMachType  == WB_STATION_XY_TOP
		|| iMachType == BE_WB_HORI_20T_LED)  // 20120731
	{
		mtn_dll_init_def_para_search_index_hori_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else if(iMachType  == BE_WB_ONE_TRACK_18V_LED)  // 20120801
	{
		mtn_dll_init_def_para_search_index_18v_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}
	else  // if (get_sys_machine_type_flag() == WB_MACH_TYPE_ONE_TRACK_13V_LED)  // default
	{
		mtn_dll_init_def_para_search_index_13v_vled_bonder_xyz(APP_X_TABLE_ACS_ID, APP_Y_TABLE_ACS_ID, sys_get_acs_axis_id_bnd_z());
	}

	mtn_dll_home_set_acs_ctrl_handle(stCommHandleACS);
	mtn_dll_trigger_wb_xyz_home_thread();
	Sleep(100);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(FALSE);

}

// IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP
void CDlgServoTuning::OnCbnSelchangeComboShowPerformOptServoCtrlTuningWbApp()
{
	iOptionShownWbWaveformPerformance = ((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP))->GetCurSel();
	switch(iFlagTuneWB_ServoOption2)
	{
	case __IDX_COMBO_WB_PERFORM_BND_Z_SRCH_CONTACT:
	case __IDX_COMBO_WB_PERFORM_BND_Z_LOOPING:
	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_1ST_B:
	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_2ND_B:
	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_Z:
	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_XY:
		if(iOptionShownWbWaveformPerformance - 1 < 0) 
		{
			iOptionShownWbWaveformPerformance =  1;
			((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP))->SetCurSel(iOptionShownWbWaveformPerformance);
		}
		break;
	}

	UI_ShowPerformOptServoCtrlTuningWbApp();
}

CString CDlgServoTuning::WB_TuneGetString_PerformOpt_All()
{
	CString csTemp;
	double dCtrlSampleTime_ms = sys_get_controller_ts();

	if(iOptionShownWbWaveformPerformance == 0)
	{
		csTemp.Format("TotalWire = %d, fMeanTimeB1W_ms = %5.1f; \r\n", 
			nTotalNumWire, stWbMultiWirePerformStat.dMeanTimeB1W_cnt * dCtrlSampleTime_ms);
		csTemp.Append(mtn_wb_tune_multi_wire_perform(&stWbMultiWirePerformStat));

//		break;
	}
	else if(iOptionShownWbWaveformPerformance <= nTotalNumWire)
	{

		csTemp.Format("%%%%====%%%%  WireNo.%d, %s, %s, B1W_Time: %3.1f\r\n", 
				iOptionShownWbWaveformPerformance, 
				astrTextContactOrNot[astWbOneWirePerformance[iOptionShownWbWaveformPerformance - 1].iFlagIsDryRun],   // 20110602
				astrTextBondWireOrBall[astWbOneWirePerformance[iOptionShownWbWaveformPerformance - 1].iFlagBallBond],   // 20120915
				astWbOneWirePerformance[iOptionShownWbWaveformPerformance - 1].nTimeB1W_cnt * dCtrlSampleTime_ms);
		csTemp.Append(mtn_tune_output_performance_string(&astWbOneWirePerformance[iOptionShownWbWaveformPerformance - 1])); // stWbWaveformPerformanceCalc));

//		break;
	}
	else if(iOptionShownWbWaveformPerformance == nTotalNumWire + 1)
	{
		csTemp.Format("%%%%====%%%%  Last Wire Position, Distance and Speed Analysis\r\n");
		csTemp.Append(mtn_wb_tune_multi_wire_motion_cmd_dist_acc(&astWbOneWirePerformance[nTotalNumWire - 1]));

	}
	else if(iOptionShownWbWaveformPerformance == nTotalNumWire + 2)
	{
static BUFFER_DATA_WB_TUNE stBufferDataEmuB1W;

		mtn_tune_calc_bond_prog_by_one_wire_info(&astWbOneWirePerformance[0]);
		mtn_tune_set_buff_data_wb_tune_st(&stBufferDataEmuB1W);
	}

	return csTemp;
}

void CDlgServoTuning::UI_ShowPerformOptServoCtrlTuningWbApp()
{
	CString cstrWbWaveformOpt;
	double dCtrlSampleTime_ms = sys_get_controller_ts();
	int iWireNo;

	switch(iFlagTuneWB_ServoOption2)
	{
	case __IDX_COMBO_WB_PERFORM_BND_Z_SRCH_CONTACT:
	case __IDX_COMBO_WB_PERFORM_BND_Z_LOOPING:
	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_1ST_B:
	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_2ND_B:
	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_Z:
	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_XY:
		iWireNo = iOptionShownWbWaveformPerformance - 1;
		if(iWireNo < 0) 
		{
			iWireNo = 0; iOptionShownWbWaveformPerformance =  iWireNo + 1;
			((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP))->SetCurSel(iOptionShownWbWaveformPerformance);
		}
		break;
	}
	switch (iFlagTuneWB_ServoOption2)
	{
	case __IDX_COMBO_WB_PERFORM_MULTI_ALL:
		cstrWbWaveformOpt = WB_TuneGetString_PerformOpt_All();
		break;
//#define __IDX_COMBO_WB_PERFORM_BND_Z_LOOPING		1
//#define __IDX_COMBO_WB_PERFORM_XY_SETTLING			2
//#define __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED			3
//#define __IDX_COMBO_WB_PERFORM_MULTI_WIRE_STAT__    4

	case __IDX_COMBO_WB_PERFORM_BND_Z_SRCH_CONTACT:
		cstrWbWaveformOpt.Format("%%%%====%%%%  WireNo.%d, %s, %s, B1W_Time: %3.1f\r\n", 
				iOptionShownWbWaveformPerformance, 
				astrTextContactOrNot[astWbOneWirePerformance[iWireNo].iFlagIsDryRun],   // 20110602
				astrTextBondWireOrBall[astWbOneWirePerformance[iWireNo].iFlagBallBond],   // 20120915
				astWbOneWirePerformance[iWireNo].nTimeB1W_cnt * dCtrlSampleTime_ms);
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_bh_srch_reset(&astWbOneWirePerformance[iWireNo]));
		break;
	case __IDX_COMBO_WB_PERFORM_BND_Z_LOOPING:
		cstrWbWaveformOpt.Format("%%%%====%%%%  WireNo.%d, %s, %s, B1W_Time: %3.1f\r\n", 
				iOptionShownWbWaveformPerformance, 
				astrTextContactOrNot[astWbOneWirePerformance[iWireNo].iFlagIsDryRun],   // 20110602
				astrTextBondWireOrBall[astWbOneWirePerformance[iWireNo].iFlagBallBond],   // 20120915
				astWbOneWirePerformance[iWireNo].nTimeB1W_cnt * dCtrlSampleTime_ms);
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_bh_looping(&astWbOneWirePerformance[iWireNo]));
		break;

	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_1ST_B:
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_xy_settling_1st_bond(&astWbOneWirePerformance[iWireNo]));
		break;

	case __IDX_COMBO_WB_PERFORM_XY_SETTLING_2ND_B:
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_xy_settling_2nd_bond(&astWbOneWirePerformance[iWireNo]));
		break;

	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_Z:
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_max_speed_bnd_z(&astWbOneWirePerformance[iWireNo]));
		break;

	case __IDX_COMBO_WB_PERFORM_ACTUAL_SPEED_XY:
		cstrWbWaveformOpt.Append( _mtn_tune_opt_wire_max_speed_xy(&astWbOneWirePerformance[iWireNo]));
		break;

	case __IDX_COMBO_WB_PERFORM_MULTI_WIRE_STAT__:
		cstrWbWaveformOpt.Format("TotalWire = %d, fMeanTimeB1W_ms = %5.1f; \r\n", 
			nTotalNumWire, stWbMultiWirePerformStat.dMeanTimeB1W_cnt * dCtrlSampleTime_ms);
		cstrWbWaveformOpt.Append(mtn_wb_tune_multi_wire_perform(&stWbMultiWirePerformStat));

		break;
	}
	GetDlgItem(IDC_EDIT_SERVO_TUNING_SHOW_WB_WAVEFORM_PERFORMANCE)->SetWindowTextA(cstrWbWaveformOpt);
}
void CDlgServoTuning::InitComboShowWbPerformOpt(int nTotalNumWire)
{
	CComboBox *pWbAppPerformCheckCombo =((CComboBox *) GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP));
	CString cstrTemp;

	//pWbAppPerformCheckCombo->Clear(); clear current item
	pWbAppPerformCheckCombo->ResetContent();
	pWbAppPerformCheckCombo->InsertString(0, _T("Multi Wire Stat."));
	for(int ii = 1; ii<= nTotalNumWire; ii++)
	{
		cstrTemp.Format("Wire- %d", ii);
		pWbAppPerformCheckCombo->InsertString(ii, cstrTemp.GetString());
	}
	pWbAppPerformCheckCombo->InsertString(nTotalNumWire + 1, _T("Actual Speed"));
	pWbAppPerformCheckCombo->InsertString(nTotalNumWire + 2, _T("Emu B1W"));
}

// IDC_BUTTON_GEN_AFT_REPORT_SERVO_TUNE
void CDlgServoTuning::OnBnClickedButtonGenAftReportServoTune()
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
FILE *fptr;
char strFilenameReportAFT[256];

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);  // 20111015
	int iMechCfg = get_sys_machine_type_flag();

	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
	}

	sprintf_s(strFilenameReportAFT, 64, "D:\\MT\\%s-%d_RptAFT_%d%d%d_H%dm%d.txt", 
		astrMachineTypeNameLabel_en[iMechCfg],
		get_sys_machine_serial_num(),
		stpTime->tm_year +1900, stpTime->tm_mon +1, 
			stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min);
	fopen_s(&fptr, strFilenameReportAFT, "w");


	if(fptr != NULL)
	{
		aft_axis_verify_output_by_spec(WB_AXIS_TABLE_X);
		aft_axis_verify_output_by_spec(WB_AXIS_TABLE_Y);
		aft_axis_verify_output_by_spec(WB_AXIS_BOND_Z);
		CString cstrAftReport = aft_gen_report_string();
		fprintf_s(fptr, "%s", cstrAftReport.GetString());
		fclose(fptr);
	}
}

void CDlgServoTuning::UpdateServoCtrlUI_ACS(BOOL bEnableFlag)
{
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->EnableWindow(bEnableFlag);
//	GetDlgItem(IDC_COMBO_SHOW_PERFORM_OPT_SERVO_CTRL_TUNING_WB_APP)->EnableWindow(bEnableFlag);
//	GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_GET_WB_PERFORM_IDX)->EnableWindow(bEnableFlag);
	if(bEnableFlag == FALSE)
	{
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_CURR_AXIS)->EnableWindow(bEnableFlag);
		GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_HOME_WB_XYZ)->EnableWindow(bEnableFlag);
		GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->EnableWindow(bEnableFlag);
	}
	GetDlgItem(IDC_BUTTON_SERVO_CTRL_GROUP_TEST)->EnableWindow(bEnableFlag);
//	GetDlgItem(IDC_BUTTON_START_TEST_POSN_SERVO_CTRL_VEL_LOOP)->EnableWindow(bEnableFlag);	
}

void CDlgServoTuning::UpdateServoFbStatusInTimer()
{
		static int iMotorState;
			mtnapi_get_motor_state(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, &iMotorState, iDebug);
			if(iMotorState & ACSC_MST_ENABLE) 
			{
				// now is enable
				CString cstrEnaDisMotorButton = "Disable";
				GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->SetWindowText(aszCmdButtonDisableMotor[m_iLanguageOption]);
				GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->EnableWindow(TRUE);

				GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->EnableWindow(FALSE);

			}
			else
			{   // Now is disabled
		//		CString cstrEnaDisMotorButton = "Enable";
				GetDlgItem(IDC_BUTTON_SERVO_CTRL_TUNING_OPEN_LOOP_ENABLE)->SetWindowText(aszCmdButtonEnableMotor[m_iLanguageOption]);
				GetDlgItem(IDC_SERVO_CTRL_TUNING_SLIDER_DRIVER_OUT)->EnableWindow(FALSE);
				// Polarity of  Encoder and DrvOut could be able to checked only when motor is OFF
				GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER)->EnableWindow(get_sys_flag_debug_mode());  // According to the system debug mode, 2011Feb07
				GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT)->EnableWindow(get_sys_flag_debug_mode());
			}

		static double dblFbPosition;
			mtnapi_get_fb_position(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, &dblFbPosition, iDebug);
			CString cstrTemp;
			cstrTemp.Format("%8.2f", dblFbPosition); // iMonitorFlag * 1000 +
			GetDlgItem(IDC_EDIT_FB_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowText(cstrTemp);

		static int iIndexStateCurr;
		// index count 20090119
			acsc_GetIndexState(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
					&iIndexStateCurr, NULL);
			if(iIndexStateCurr & ACSC_IST_IND)
			{
				acsc_ResetIndexState(stCommHandleACS, astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS,
					ACSC_IST_IND, NULL); 				// Clear index state
				iCountPassIndexPosn ++;
			}
			cstrTemp.Format("%d", iCountPassIndexPosn);
			GetDlgItem(IDC_STATIC_SERVO_CTRL_TUNING_COUNTER_PASS_INDEX)->SetWindowTextA(cstrTemp);

			acsc_ReadReal(stCommHandleACS, ACSC_NONE, "IND", 
				astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 
				astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS, 0, 0, &dIndexPositionCurr, NULL);
			cstrTemp.Format("%6.0f", dIndexPositionCurr);
			GetDlgItem(IDC_EDIT_LAST_IDX_POSN_SERVO_CTRL_TUNING_OPEN_LOOP)->SetWindowText(cstrTemp);

			// If During the thread of tuning velocity loop, update the XLS chart
			if(cFlagDlgServoTuningThreadRunning == TRUE)
			{
				static	unsigned int uiCurrLoopCount;
				static	int iFlagServoNormal;
				CString czTestVelLoopTuning;
				mtn_aft_vel_loop_get_status(&uiCurrLoopCount, &iFlagServoNormal);
				czTestVelLoopTuning.Format("CurrLoop: %d/ Total Loop%d", uiCurrLoopCount, stVelLoopTestInput.uiLoopNum);
				GetDlgItem(IDC_STATIC_CHECK_ENC_GUIDE_SERVO_CTRL_TUNING)->SetWindowTextA(czTestVelLoopTuning);
			}

			//
}

#define _BIT_INV_ENC_CONTROL_CARD_          12
#define _BIT_INV_DRV_OUT_CONTROL_CARD_      13

// IDC_CHECK_FLAG_INVERT_ENCODER
void CDlgServoTuning::OnBnClickedCheckFlagInvertEncoder()
{
	int iCtrlAxisACS = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
	int iControlCardMotorFlag;
	acsc_ReadInteger(stCommHandleACS, ACSC_NONE, "MFLAGS", 
				iCtrlAxisACS, iCtrlAxisACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagEnc;
	iTempFlagEnc = ((CButton * ) GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER))->GetCheck();

	unsigned int uiTempFlagBitAnd, uiTempBitFullOne32Bit;
	uiTempFlagBitAnd = (unsigned int)pow(2.0, (double)_BIT_INV_ENC_CONTROL_CARD_);  

	if(iTempFlagEnc == 1)
	{ // OR the bit to enable
		iControlCardMotorFlag = iControlCardMotorFlag | uiTempFlagBitAnd;
	}
	else 
	{ // clear the bit to disable
		uiTempBitFullOne32Bit = _FULL_BITS_INTEGER_32b_; // (int)(pow(2.0, 32) - 1);
		uiTempFlagBitAnd = uiTempBitFullOne32Bit - uiTempFlagBitAnd;
		iControlCardMotorFlag = iControlCardMotorFlag & uiTempFlagBitAnd;

	}
	acsc_WriteInteger(stCommHandleACS, ACSC_NONE, "MFLAGS", 
				iCtrlAxisACS, iCtrlAxisACS, 0, 0, &iControlCardMotorFlag, NULL);
}

// IDC_CHECK_FLAG_INVERT_DRIVER_OUT
void CDlgServoTuning::OnBnClickedCheckFlagInvertDriverOut()
{
	int iCtrlAxisACS = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
	int iControlCardMotorFlag;
	acsc_ReadInteger(stCommHandleACS, ACSC_NONE, "MFLAGS", 
				iCtrlAxisACS, iCtrlAxisACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagDrvOut = 0;
	iTempFlagDrvOut = ((CButton * ) GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT))->GetCheck();

	unsigned int uiTempFlagBitAnd, uiTempBitFullOne32Bit;
	uiTempFlagBitAnd = (unsigned int)pow(2.0, (double)_BIT_INV_DRV_OUT_CONTROL_CARD_);  
	if(iTempFlagDrvOut == 1)
	{ // OR the bit to enable
		iControlCardMotorFlag = iControlCardMotorFlag | uiTempFlagBitAnd;
	}
	else 
	{ // clear the bit to disable
		uiTempBitFullOne32Bit = _FULL_BITS_INTEGER_32b_; // (int)(pow(2.0, 32) - 1);
		uiTempFlagBitAnd = uiTempBitFullOne32Bit - uiTempFlagBitAnd;
		iControlCardMotorFlag = iControlCardMotorFlag & uiTempFlagBitAnd;

	}
	acsc_WriteInteger(stCommHandleACS, ACSC_NONE, "MFLAGS", 
				iCtrlAxisACS, iCtrlAxisACS, 0, 0, &iControlCardMotorFlag, NULL);

}

//#define _BIT_AND_GET_INV_ENC_CONTROL_CARD_      ((int)pow(2, _BIT_INV_ENC_CONTROL_CARD_))
//#define _BIT_AND_GET_INV_DRV_OUT_CONTROL_CARD_  ((int)pow(2, _BIT_INV_DRV_OUT_CONTROL_CARD_))

void CDlgServoTuning::Update_UI_FlagInvertEncoderDrvOut_CtrlCard()
{
	int iCtrlAxisACS = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
	
	int iControlCardMotorFlag;
	acsc_ReadInteger(stCommHandleACS, ACSC_NONE, "MFLAGS", 
				iCtrlAxisACS, iCtrlAxisACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagEnc = 0;
	int iTempFlagDrvOut = 0;

	unsigned int uiTempFlagBitAnd;
	uiTempFlagBitAnd = (int)pow(2.0, (double)_BIT_INV_ENC_CONTROL_CARD_); // _BIT_AND_GET_INV_ENC_CONTROL_CARD_;
	iTempFlagEnc = iControlCardMotorFlag & uiTempFlagBitAnd;
	uiTempFlagBitAnd = (int)pow(2.0, (double)_BIT_INV_DRV_OUT_CONTROL_CARD_);  // _BIT_AND_GET_INV_DRV_OUT_CONTROL_CARD_;
	iTempFlagDrvOut = iControlCardMotorFlag & uiTempFlagBitAnd;

	((CButton * ) GetDlgItem(IDC_CHECK_FLAG_INVERT_ENCODER))->SetCheck(iTempFlagEnc);
	((CButton * ) GetDlgItem(IDC_CHECK_FLAG_INVERT_DRIVER_OUT))->SetCheck(iTempFlagDrvOut);

}

// IDC_COMBO_SERVO_CTRL_TUNING_WB_DEBUG_LEVEL

void CDlgServoTuning::UI_ShowWindowPartsPositionForceCalibration(BOOL bFlagShow)
{
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KF)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KCP)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_OF)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KF)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KCP)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_OF)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE)->ShowWindow(bFlagShow);	

	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->ShowWindow(bFlagShow);

}

void CDlgServoTuning::UI_ShowWindowPartsPositionForceCalibrationVarSP(BOOL bFlagShow)
{
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_OS)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_GAIN)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_FACTOR)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_EOFFS)->ShowWindow(bFlagShow);	
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_BH_UPP_LIMIT_POSN)->ShowWindow(bFlagShow);	
	
}

void CDlgServoTuning::UI_EnableWindowPartsPositionForceCalibration(BOOL bEnableFlag)
{
	//
	GetDlgItem(IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE)->EnableWindow(bEnableFlag);	

	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN)->EnableWindow(bEnableFlag);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN)->EnableWindow(bEnableFlag);	
	GetDlgItem(IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN)->EnableWindow(bEnableFlag);	
	// 
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KF)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KCP)->EnableWindow(FALSE);	
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_OF)->EnableWindow(FALSE);	

}

// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS
// IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP
// IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS
// IDC_STATIC_SERVO_ADJUST_LABEL_BH_UPP_LIMIT_POSN

// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KF
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KCP
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_OF
// IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KF
// IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_KCP
// IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_FORCE_OF

// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_OS
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_GAIN
// IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_FACTOR
extern MTN_TUNE_POSN_COMPENSATION stPosnCompensationTune;

void CDlgServoTuning::UI_Wb_UpdateParameterPositionForceCalibration()
{
//	int 
	MTUNE_OUT_POSN_COMPENSATION stOutputPosnCompensationTune;
	mtn_api_get_spring_compensation_sp_para(stCommHandleACS, &stOutputPosnCompensationTune);

	CString cstrTemp;
	cstrTemp.Format("%10.6f", stOutputPosnCompensationTune.dPositionFactor);
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_KP)->SetWindowTextA(cstrTemp); // EnableWindow(bEnableFlag);	

	cstrTemp.Format("%10.6f", stOutputPosnCompensationTune.dCtrlOutOffset);
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_OS)->SetWindowTextA(cstrTemp);	

	cstrTemp.Format("%d", stOutputPosnCompensationTune.iEncoderOffsetSP);
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_EOFFS)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%d", stOutputPosnCompensationTune.iOffset_SP_ACS);
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_OS)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stOutputPosnCompensationTune.iGain_SP_ACS);
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_GAIN)->SetWindowTextA(cstrTemp);
	cstrTemp.Format("%d", stOutputPosnCompensationTune.iFactor_SP_ACS);
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_CALIBRATION_POSITION_SP_KP_FACTOR)->SetWindowTextA(cstrTemp);

	((CButton*)GetDlgItem(IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE))->SetCheck(stOutputPosnCompensationTune.iFlagEnable);

	cstrTemp.Format("%d", (int)mtn_wb_get_bh_upper_limit_position());
	GetDlgItem(IDC_STATIC_SERVO_ADJUST_LABEL_BH_UPP_LIMIT_POSN)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%5.1f", stPosnCompensationTune.dUpperPosnLimit);
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%5.1f", stPosnCompensationTune.dLowerPosnLimit);
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN)->SetWindowTextA(cstrTemp);

}

// IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE
void CDlgServoTuning::OnBnClickedCheckServoAdjustPositionSpringCompensateEnable()
{
	int iFlagEnable = ((CButton*)GetDlgItem(IDC_CHECK_SERVO_ADJUST_POSITION_SPRING_COMPENSATE_ENABLE))->GetCheck();

	if(iFlagEnable == 1)
	{
		mtn_api_enable_spring_compensation_sp_para(stCommHandleACS);
	}
	else
	{
		mtn_api_disable_spring_compensation_sp_para(stCommHandleACS);
	}
}

static char strTemp[64];
// IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN
void CDlgServoTuning::OnEnKillfocusEditServoAdjustCalibrateUppPosn()
{
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_UPP_POSN)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dUpperPosnLimit);
}
// IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN
void CDlgServoTuning::OnEnKillfocusEditServoAdjustCalibrateLowPosn()
{
	GetDlgItem(IDC_EDIT_SERVO_ADJUST_CALIBRATE_LOW_POSN)->GetWindowTextA(&strTemp[0], 64);
	sscanf_s(strTemp, "%lf", &stPosnCompensationTune.dLowerPosnLimit);
}
// IDC_BUTTON_SERVO_ADJUST_START_CALIBRATE_POSN
void CDlgServoTuning::OnBnClickedButtonServoAdjustStartCalibratePosn()
{
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
	stPosnCompensationTune.iDebug = 1;

	mtn_thread_trigger_start_position_compensation();

}

// IDC_BUTTON_SERVO_ADJUST_VERIFY_POSITION_SPRING_GRAVITY
void CDlgServoTuning::OnBnClickedButtonServoAdjustVerifyPositionSpringGravity()
{
}
// IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY
void CDlgServoTuning::OnBnClickedButtonServoAdjustVerifyPosnSpringGravity()
{
	if(iFlagModeIsVerifyingPosnCompensate == FALSE)
	{
		iFlagModeIsVerifyingPosnCompensate = TRUE;

		mtnapi_disable_motor_current_mode(stCommHandleACS, ACSC_AXIS_A);
		mtnapi_disable_motor(stCommHandleACS, ACSC_AXIS_A, 0);

		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->SetWindowTextA(_T("R"));
	}
	else
	{
		iFlagModeIsVerifyingPosnCompensate = FALSE;

		mtnapi_enable_motor_current_mode(stCommHandleACS, ACSC_AXIS_A);
		mtnapi_enable_motor(stCommHandleACS, ACSC_AXIS_A, 0);

		GetDlgItem(IDC_BUTTON_SERVO_ADJUST_VERIFY_POSN_SPRING_GRAVITY)->SetWindowTextA(_T("P.V."));
	}
}
//// IDC_STATIC_SERVO_ADJUST_LABEL_BH_UPP_LIMIT_POSN
//void CDlgServoTuning::OnStnClickedStaticServoAdjustLabelBhUppLimitPosn()
//{
//}

// 	IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING
void CDlgServoTuning::OnEnKillfocusEditEncResServoCtrlTuning()
{
	double dEncRes_CntPmm;
	ReadDoubleFromEdit(IDC_EDIT_ENC_RES_SERVO_CTRL_TUNING, &dEncRes_CntPmm);
	astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].afEncoderResolution_cnt_p_mm = dEncRes_CntPmm;

	cpVelStepGroupTest->InitVelStepGroupTestCfg();
	cpVelStepGroupTest->InitVelStepGroupTestConfigUI(); // 
}

// IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_
void CDlgServoTuning::OnBnClickedCheckSafetyForceFeedbackServoCtrlTuning()
{
	cFlagSafetyCheckOpenLoopForceAndFeedback = ((CButton*)GetDlgItem(IDC_CHECK_SAFETY_FORCE_FEEDBACK_SERVO_CTRL_TUNING_))->GetCheck();
}
