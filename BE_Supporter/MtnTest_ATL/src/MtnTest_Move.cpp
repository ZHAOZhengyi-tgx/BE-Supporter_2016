// MtnTest_Move.cpp : implementation file
// TBA: To be added
// History
// YYYYMMDD  Author  Notes
// 20081028  Zhengyi Debug buffer programming
// 20090219  Zhengyi replace ServoController.h to AcsServo.h, follow ZhangDong's new release
// 20090831			 Servo Loop Parameter, 2-Point Move
// 2010Feb05 protect local static variables
// 20100305  Fixed Pattern Burn-In, data-capture
#include "stdafx.h"
#include "MtnTest_Move.h"
#include "MtnScopeDlg.h"
#include "MtnInitAcs.h"
#include "AcsServo.h" //"ServoController.h"
//#include "MACDEF.h"
#include "math.h"
//#include "mmsystem.h"
#include "WinTiming.h"

#include "MtnTesterResDef.h"

//#include "MotAlgo_DLL.h"

extern COMM_SETTINGS stServoControllerCommSet;

// CMtnTest_Move dialog
extern SERVO_AXIS_BLK stServoAxis_ACS[];
// 20081028
#include "acs_buff_prog.h"

static char strDebugText[128];
void ClearAxisProfCmdBuffer(int iAxis);

#define CMD_GEN_UNIT_TIME_IN_SEC  0.0001
static double dCmdGenTs_ms = (CMD_GEN_UNIT_TIME_IN_SEC*1000);

// For reading double type variable
static char *strStopString;

#include "MotAlgo_DLL.h"

#include "BurnIn.c"

extern int iiOneAxis2PointsMoveCycle;
extern int iiBurnInPointInCycle;

static int iFlagBurnIn = MTN_TEST_BURN_IN_FLAG_NONE;
static char cFlagSaveScopeDataInFile = 0;
static char cFlagTuningCurrentAxis = 0;
IMPLEMENT_DYNAMIC(CMtnTest_Move, CDialog)

// extern CServoController theAcsServo; // external Class, declared from CommunicationDialog.cpp
static unsigned int uiCurrMoveAxis_ACS; // uiAxisPerMoveItem[MAX_NUM_AXES_MOVE_TEST]; 20090930
static unsigned int uiCurrMoveAxis_AppWB = WB_AXIS_TABLE_X;
unsigned int mtn_test_get_current_moving_axis()
{
    return uiCurrMoveAxis_ACS;
}

/////////
#include "MtnTune.h"

static MOVE_CURR_LOOP_STEP_TEST stMoveTestCurrLoopStep;
static MTN_TIME_IDX_2_LOOP_MOVE stMtnTimeIdx2LoopMove;
static MTN_SERVO_TUNE_INDEX  stMtnServoTuneIndex;
double fThresholdSettleTime = 5.0;
extern CMtnTune mMotionTuning;
extern MTN_TUNE_SEARCH_AFF_OUTPUT stTuneAffOutput;

//extern void mtn_api_prompt_acs_error_code(HANDLE stCommHandleAcs);
static int iMoveDlgFlagShow = 0;

int mtn_test_get_burn_in_flag()
{
	return iFlagBurnIn;
}

void mtn_test_set_burn_in_flag(int iSetFlagBurnIn)
{
	iFlagBurnIn = iSetFlagBurnIn;
}

#define TRAJECTORY_BUFFER_SIZE  2000
#define AXIS_Z_MAPPING_ACS_A    4

double dDistProf[MAX_NUM_AXES_MOVE_TEST][TRAJECTORY_BUFFER_SIZE];
double dVelProf[MAX_NUM_AXES_MOVE_TEST][TRAJECTORY_BUFFER_SIZE];
double dAccProf[MAX_NUM_AXES_MOVE_TEST][TRAJECTORY_BUFFER_SIZE];
double dJerkProf[MAX_NUM_AXES_MOVE_TEST][TRAJECTORY_BUFFER_SIZE];
double aTime_ms[TRAJECTORY_BUFFER_SIZE];

void ClearAxisProfCmdBuffer(int iAxis)
{
	for(int ii = 0; ii<TRAJECTORY_BUFFER_SIZE; ii++)
	{
		dDistProf[iAxis][ii] = 0;
		dVelProf[iAxis][ii] = 0;
		dAccProf[iAxis][ii] = 0;
		dJerkProf[iAxis][ii] = 0;
	}
}
#include "TwinSine.h"

static CALC_TWINSINE_PRECAL_INPUT lstTwinSinePreCalInput[MAX_NUM_AXES_MOVE_TEST];
static union 
{
	CALC_TWINSINE_PRECAL_OUTPUT stTwinSinePreCalOutput;
	CALC_TWINSINE_PROFGEN_INPUT stTwinSingProfGenInput;
}luCalcTwinSinePrecOutProfGenIn[MAX_NUM_AXES_MOVE_TEST];
static CALC_TWINSINE_PROFGEN_OUTPUT lstTwinSineProfGenOutput[MAX_NUM_AXES_MOVE_TEST];

// Unit for MTN_SPEED_PROFILE is: Enc/Sec, Enc/Sec/Sec,
// Unit for CALC_TWINSINE_PRECAL_INPUT is: Enc/Sample
void mtn_twin_sine_set_speed_limit_(CALC_TWINSINE_PRECAL_INPUT *stpTwinSineSpeed, MTN_SPEED_PROFILE *stpMtnSpeedProf, double dUnitCmdGen_sec, double dDist)
{
	stpTwinSineSpeed->fDist = dDist;
	stpTwinSineSpeed->fVelMax = stpMtnSpeedProf->dMaxVelocity * dUnitCmdGen_sec;
	stpTwinSineSpeed->fAccMax = stpMtnSpeedProf->dMaxAcceleration * dUnitCmdGen_sec * dUnitCmdGen_sec;
	stpTwinSineSpeed->fJmax = stpMtnSpeedProf->dMaxJerk * dUnitCmdGen_sec * dUnitCmdGen_sec * dUnitCmdGen_sec; 
	stpTwinSineSpeed->iTimeMoveFlag = 0;
}

void mtn_twin_sine_init_profile_data_buffer()
{
	// Init pointer to memory buffer
	for(unsigned int ii = 0; ii<MAX_NUM_AXES_MOVE_TEST; ii++)
	{
		lstTwinSineProfGenOutput[ii].pfdist_prof = &dDistProf[ii][0];
		lstTwinSineProfGenOutput[ii].pfvel_prof = &dVelProf[ii][0];
		lstTwinSineProfGenOutput[ii].pfacc_prof = &dAccProf[ii][0];
		lstTwinSineProfGenOutput[ii].pfjerk_prof = &dJerkProf[ii][0];
	}
}


static MTN_SPEED_PROFILE stSpeedProfilePerAxis[MAX_NUM_AXES_MOVE_TEST];
static double dAcsControlDecoupleY_FromZ;  // 20110701
static double dAcsControlDecoupleZ_FromY;  // 20110701

static int iMotorState[MAX_NUM_AXES_MOVE_TEST], iDebug;
static char strTextTemp[LEN_TEXT];
static double dblFbPosition, dblRefPosition;

extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

void MtnTestMoveDlg_UpdateSpeedProfile()   // 20120921
{
//	int idxAcsAxisWbApp = astAxisInfoWireBond[WB_AXIS_TABLE_X].iAxisInCtrlCardACS;
	stSpeedProfilePerAxis[WB_AXIS_TABLE_X] = stServoAxis_ACS[WB_AXIS_TABLE_X].stSpeedProfile[3];

//	idxAcsAxisWbApp = astAxisInfoWireBond[WB_AXIS_TABLE_Y].iAxisInCtrlCardACS;
	stSpeedProfilePerAxis[WB_AXIS_TABLE_Y] = stServoAxis_ACS[WB_AXIS_TABLE_Y].stSpeedProfile[3];

//	idxAcsAxisWbApp = astAxisInfoWireBond[WB_AXIS_BOND_Z].iAxisInCtrlCardACS;
	stSpeedProfilePerAxis[WB_AXIS_BOND_Z] = stServoAxis_ACS[WB_AXIS_BOND_Z].stSpeedProfile[5]; // For reset motion, fastest speed

}

#include <direct.h>
extern char *astrMachineTypeNameLabel_en[];
extern AXIS_INFO_WIRE_BOND astAxisInfoWireBond[MAX_SERVO_AXIS_WIREBOND];

static	char strFolderNameMoveTest[1024];
static	char strFilenameMoveTest[512];

//#define FILE_NAME "DataTraj.m"
short mtn_save_curr_axis_traj_data_to_file(unsigned int uiAxisCtrlCard)
{
	short sRet = MTN_API_OK_ZERO;

struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
//static char strMechaTronicsConfig[32];
	// Mechanical configuration
	int iMechCfg = get_sys_machine_type_flag();

	int idxAxisWb;
	if(uiAxisCtrlCard == ACSC_AXIS_X)
	{
		if(iMechCfg == WB_MACH_TYPE_HORI_LED ||
			iMechCfg == WB_STATION_XY_TOP ||
			iMechCfg == BE_WB_HORI_20T_LED || // 20120826
			iMechCfg == BE_WB_ONE_TRACK_18V_LED)
		{
			idxAxisWb = WB_AXIS_TABLE_Y;
		}
		else
		{
			idxAxisWb = WB_AXIS_TABLE_X;
		}
	}
	else if(uiAxisCtrlCard == ACSC_AXIS_Y)
	{
		if(iMechCfg == WB_MACH_TYPE_HORI_LED  ||
			iMechCfg == WB_STATION_XY_TOP)
		{
			idxAxisWb = WB_AXIS_TABLE_X;
		}
		else
		{
			idxAxisWb = WB_AXIS_TABLE_Y;
		}
	}
	else if(uiAxisCtrlCard == sys_get_acs_axis_id_bnd_z())
	{
		idxAxisWb = WB_AXIS_BOND_Z;
	}
	else if(uiAxisCtrlCard == sys_get_acs_axis_id_wire_clamp())
	{
		idxAxisWb = WB_AXIS_WIRE_CLAMP;
	}
	else
	{
		idxAxisWb = 0;
	}

	if(_chdir("D:\\MT"))
	{
		_mkdir("D:\\MT");
		_chdir("D:\\MT");
	}

	// 20130218
	sprintf_s(strFolderNameMoveTest, 512, "_MoveTest_%s-%d_%s_%d.%d.%d.H%d_A-%5.1f-g_J-%5.1f-Kmps3",
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num(),
				astAxisInfoWireBond[idxAxisWb].strAxisNameEn,
				stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour,
				stSpeedProfilePerAxis[idxAxisWb].dMaxAcceleration / astAxisInfoWireBond[idxAxisWb].afEncoderResolution_cnt_p_mm/1000.0/10, // unit in g: 10m/s^2
				stSpeedProfilePerAxis[idxAxisWb].dMaxJerk / astAxisInfoWireBond[idxAxisWb].afEncoderResolution_cnt_p_mm/1000.0/1000);      // unit in km/s^3
	_mkdir(strFolderNameMoveTest);
	// 20130218

	sprintf_s(strFilenameMoveTest, 512, "D:\\MT\\%s\\_MoveTest_%s-%d_%s_%d.%d.%d.H%d.m",
				strFolderNameMoveTest, 
				astrMachineTypeNameLabel_en[iMechCfg],
				get_sys_machine_serial_num(),
				astAxisInfoWireBond[idxAxisWb].strAxisNameEn,
				stpTime->tm_year +1900, stpTime->tm_mon +1, stpTime->tm_mday, stpTime->tm_hour);

	sRet = mtn_save_curr_axis_traj_data_to_filename(strFilenameMoveTest, uiAxisCtrlCard); // fopen_s(&fpData, FILE_NAME, "w");

	return sRet;
}

CMtnTest_Move::CMtnTest_Move( CWnd* pParent /*=NULL*/)
	: CDialog(CMtnTest_Move::IDD, pParent)
	, m_iFbPosition_Axis1(0)
	, m_iPosnErr(0)
	, m_strMotorEnaFlag(_T(""))
	, m_strFlagMove(_T(""))
	, m_strFlagAcc_Axis1(_T(""))
	, m_strFlagInPosn_Axis1(_T(""))
{
	Handle = stServoControllerCommSet.Handle;
//	uiTotalAxes = 2;

	iDebug = 0;

//unsigned int ii;

	mtn_twin_sine_init_profile_data_buffer();
	InitAxisSetting();
}

CMtnTest_Move::~CMtnTest_Move()
{
}

static int iPoint1stIn2PointsOneAxisMove = 1000;
static int iPoint2ndIn2PointsOneAxisMove = 5000;
static unsigned short usTotalCycle2PointsOneAxisMove = 1;
static unsigned int uiCurrWireNo;

int mtn_test_get_2_point_move_1st_pt()
{
	return iPoint1stIn2PointsOneAxisMove;
}
int mtn_test_get_2_point_move_2nd_pt()
{
	return iPoint2ndIn2PointsOneAxisMove;
}

void CMtnTest_Move::InitAxisSetting()
{
	unsigned int ii;
	iAxisTargetPosn_cnt = 0;
	dRelDistance_mm = 5.0;

	for(ii = 0; ii<MAX_NUM_AXES_MOVE_TEST; ii++)
	{
		uiMotionInterDelay_ms[ii] = 100;
	}
}

static BOOL bFlagEnableShowGroupBurnInTest = 0;
// IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN
void CMtnTest_Move::OnBnClickedCheckShowGroupTestMoveMultiAxisRandBurnIn()
{
	bFlagEnableShowGroupBurnInTest = ((CButton*)GetDlgItem(IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN))->GetCheck();
	UI_Show_GroupBurnInTest(bFlagEnableShowGroupBurnInTest);
}

static BOOL bFlagEnableShowGroupWbTunePosn = 0;
// IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION
void CMtnTest_Move::OnBnClickedCheckShowGroupSetWbTunePosition()
{
	bFlagEnableShowGroupWbTunePosn = ((CButton*)GetDlgItem(IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION))->GetCheck();
	UI_Show_GroupWbSelectTuningPosition(bFlagEnableShowGroupWbTunePosn);
}

static BOOL bFlagEnableEditAcsParameter;
// IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA
#include "DlgKeyInputPad.h"
void CMtnTest_Move::OnBnClickedCheckMovingTestDlgEnableEditAcsPara()
{
BOOL	bTemp = ((CButton*)GetDlgItem(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA))->GetCheck();
	static int iFlagFailurePass = FALSE;

	if(bTemp == TRUE)
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
			bTemp = FALSE;
			((CButton *)GetDlgItem(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA))->SetCheck(FALSE);
		}
	}
	else
	{
		UI_Enable_GroupEditAcsParameter(bTemp);
	}

	bFlagEnableEditAcsParameter = bTemp;
	UI_Enable_GroupEditAcsParameter(bFlagEnableEditAcsParameter);
}

// IDC_CHECK_SHOW_MOVE_TEST_CURRENT_LOOP
static BOOL bFlagEnableShowGroupCurrentLoopTest = 0;
void CMtnTest_Move::OnBnClickedCheckShowMoveTestCurrentLoop()
{
	bFlagEnableShowGroupCurrentLoopTest = ((CButton*)GetDlgItem(IDC_CHECK_SHOW_MOVE_TEST_CURRENT_LOOP))->GetCheck();
	UI_Show_GroupCurrentLoopTest(bFlagEnableShowGroupCurrentLoopTest);
}

// Dialog UIs related with Random Burn-In
void CMtnTest_Move::InitBurnInGroupTestConfigCombo_OneAxis(int nResId, int iAxis)
{
	CComboBox *pSelectChannelCombo = (CComboBox*) GetDlgItem(nResId);
	pSelectChannelCombo->InsertString(0,"ACS-X");
	pSelectChannelCombo->InsertString(1,"ACS-Y");
	pSelectChannelCombo->InsertString(2,"ACS-Z");
	pSelectChannelCombo->InsertString(3,"ACS-T");
	pSelectChannelCombo->InsertString(4,"ACS-A");
	pSelectChannelCombo->InsertString(5,"ACS-B");
	pSelectChannelCombo->InsertString(6,"ACS-C");
	pSelectChannelCombo->InsertString(7,"ACS-D");
	pSelectChannelCombo->InsertString(MAX_CTRL_AXIS_PER_SERVO_BOARD,"Dummy");
	pSelectChannelCombo->SetCurSel(asQcBurnInAxisIdAcs[iAxis]);
}


void CMtnTest_Move::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FPOSN_MOVE_TEST_AXIS1, m_iFbPosition_Axis1);
	DDX_Text(pDX, IDC_POSNERR_MOVE_TEST_AXIS1, m_iPosnErr);
	DDX_Text(pDX, IDC_MOTOR_ENA_FLAG_MOVE_TEST_AXIS1, m_strMotorEnaFlag);
	DDX_Text(pDX, IDC_MOVING_FLAG_MOVE_TEST_AXIS1, m_strFlagMove);
	DDX_Text(pDX, IDC_ACC_FLAG_MOVE_TEST_AXIS1, m_strFlagAcc_Axis1);
	DDX_Text(pDX, IDC_INPOSN_FLAG_MOVE_TEST_AXIS1, m_strFlagInPosn_Axis1);
	DDX_Control(pDX, IDC_CHANSEL_MOVE_TEST_AXIS1, m_combSelectAxis0);
}

BEGIN_MESSAGE_MAP(CMtnTest_Move, CDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_TPOSN_AXIS1, &CMtnTest_Move::OnEnChangeTposnAxis1)
	ON_BN_CLICKED(IDC_MTN_TEST_ENABLE_MOTOR_1, &CMtnTest_Move::OnBnClickedMtnTestEnableMotor1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMtnTest_Move::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_UPLOAD_DATA_MOVE_TEST, &CMtnTest_Move::OnBnClickedUploadDataMoveTest)
	ON_CBN_SELCHANGE(IDC_CHANSEL_MOVE_TEST_AXIS1, &CMtnTest_Move::OnCbnSelchangeChanselMoveTestAxis1)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_RELATIVE, &CMtnTest_Move::OnBnClickedButtonMoveRelative)
	ON_EN_CHANGE(IDC_EDIT_REL_DIST_1, &CMtnTest_Move::OnEnChangeEditRelDist1)
	ON_EN_CHANGE(IDC_EDIT_NUM_LOOP_REL_MOVE, &CMtnTest_Move::OnEnChangeEditNumLoopRelMove)
	ON_EN_CHANGE(IDC_EDIT_MOVE_CMD_GEN_TS_MS, &CMtnTest_Move::OnEnChangeEditMoveCmdGenTsMs)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START, &CMtnTest_Move::OnBnClickedButtonMoveTestRandomBurnInStart)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_STOP, &CMtnTest_Move::OnBnClickedButtonMoveTestRandomBurnInStop)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_1_ENVOLVED, &CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis1Envolved)
	ON_EN_CHANGE(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, &CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis1)
	ON_EN_CHANGE(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, &CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis2)
	ON_EN_CHANGE(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, &CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis3)
	ON_EN_CHANGE(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, &CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis4)
	ON_EN_CHANGE(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, &CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis1)
	ON_EN_CHANGE(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, &CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis2)
	ON_EN_CHANGE(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, &CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis3)
	ON_EN_CHANGE(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, &CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis4)
	ON_EN_CHANGE(IDC_EDIT_NUM_POINTS_PER_CYCLE_MOVE_TEST_RANDOM_BURN_IN, &CMtnTest_Move::OnEnChangeEditNumPointsPerCycleMoveTestRandomBurnIn)
	ON_EN_CHANGE(IDC_EDIT_TOTAL_NUM_CYCLE_MOVE_TEST_RANDOM_BURN_IN, &CMtnTest_Move::OnEnChangeEditTotalNumCycleMoveTestRandomBurnIn)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_2_ENVOLVED, &CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis2Envolved)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_3_ENVOLVED, &CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis3Envolved)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_4_ENVOLVED, &CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis4Envolved)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, &CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis1)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, &CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis2)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, &CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis3)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, &CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis4)
	ON_BN_CLICKED(IDOK, &CMtnTest_Move::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMtnTest_Move::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_FIX_BURN_IN_WIRE_LENGTH, &CMtnTest_Move::OnEnChangeEditFixBurnInWireLength)
	ON_EN_CHANGE(IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT, &CMtnTest_Move::OnEnChangeEditFixBurnInBetweenUnit)
	ON_CBN_SELCHANGE(IDC_COMBO_FIX_BURN_IN_NUM_SIDES, &CMtnTest_Move::OnCbnSelchangeComboFixBurnInNumSides)
	ON_EN_CHANGE(IDC_EDIT_FIX_BURN_IN_NUM_UNITS_PER_SIDE, &CMtnTest_Move::OnEnChangeEditFixBurnInNumUnitsPerSide)
	ON_BN_CLICKED(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE, &CMtnTest_Move::OnBnClickedCheckFixBurnInFlagIsClockWise)
	ON_BN_CLICKED(IDC_BUTTON_TEST_MOVE_FIX_BURN_IN_START, &CMtnTest_Move::OnBnClickedButtonTestMoveFixBurnInStart)
	ON_EN_KILLFOCUS(IDC_MAXVEL_AXIS1, &CMtnTest_Move::OnEnKillfocusMaxvelAxis1)
	ON_EN_KILLFOCUS(IDC_MAXACC_AXIS1, &CMtnTest_Move::OnEnKillfocusMaxaccAxis1)
	ON_EN_KILLFOCUS(IDC_MAXDEC_AXIS1, &CMtnTest_Move::OnEnKillfocusMaxdecAxis1)
	ON_EN_KILLFOCUS(IDC_MAXJERK_AXIS1, &CMtnTest_Move::OnEnKillfocusMaxjerkAxis1)
	ON_EN_KILLFOCUS(IDC_MAX_KILL_ACC_AXIS1, &CMtnTest_Move::OnEnKillfocusMaxKillAccAxis1)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLPKP, &CMtnTest_Move::OnEnKillfocusEditAcsSlpkp)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVKP, &CMtnTest_Move::OnEnKillfocusEditAcsSlvkp)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVKI, &CMtnTest_Move::OnEnKillfocusEditAcsSlvki)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVLI, &CMtnTest_Move::OnEnKillfocusEditAcsSlvli)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SOF, &CMtnTest_Move::OnEnKillfocusEditAcsSof)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLAFF, &CMtnTest_Move::OnEnKillfocusEditAcsSlaff)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, &CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsPoint1)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, &CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsPoint2)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_2_POINTS_TOTAL_ITER, &CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsTotalIter)
	ON_BN_CLICKED(IDC_BUTTON_START_2_POINTS_MOVE, &CMtnTest_Move::OnBnClickedButtonStart2PointsMove)
	ON_EN_KILLFOCUS(IDC_DWELL_AXIS1, &CMtnTest_Move::OnEnKillfocusDwellAxis1)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_SAVE_SCOPE_DATA_TO_FILE, &CMtnTest_Move::OnBnClickedCheckMoveTestSaveScopeDataToFile)
	ON_BN_CLICKED(IDC_CHECK_MOVE_TEST_TUNING, &CMtnTest_Move::OnBnClickedCheckMoveTestTuning)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_SERVO_TUNE_POSNERR_SETTLE_TH, &CMtnTest_Move::OnEnKillfocusEditMoveTestServoTunePosnerrSettleTh)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC, &CMtnTest_Move::OnBnClickedButtonMoveTestTuneAccFfc)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEST_STOP_TUNING, &CMtnTest_Move::OnBnClickedButtonMoveTestStopTuning)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_CURR_STEP_UPP_LEVEL, &CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepUppLevel)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_CURR_STEP_LOW_LEVEL, &CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepLowLevel)
	ON_EN_KILLFOCUS(IDC_EDIT_MOVE_TEST_CURR_STEP_TOTAL_ITER, &CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepTotalIter)
	ON_BN_CLICKED(IDC_BUTTON_MOVE_TEST_CURR_STEP_START, &CMtnTest_Move::OnBnClickedButtonMoveTestCurrStepStart)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_TUNE_WB_BOND_HEAD_MOTION_IDX, &CMtnTest_Move::OnCbnSelchangeComboSelTuneWbBondHeadMotionIdx)
	ON_CBN_SELCHANGE(IDC_COMBO_SEL_TUNE_WB_TABLE_MOTION_IDX, &CMtnTest_Move::OnCbnSelchangeComboSelTuneWbTableMotionIdx)
	ON_EN_KILLFOCUS(IDC_EDIT_WIRE_ID_MOVE_TEST_GET_TUNE_POSN_SET, &CMtnTest_Move::OnEnKillfocusEditWireIdMoveTestGetTunePosnSet)
	ON_CBN_SELCHANGE(IDC_COMBO_TUNING_THEME_OPTION, &CMtnTest_Move::OnCbnSelchangeComboTuningThemeOption)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SP_JERKFF, &CMtnTest_Move::OnEnKillfocusEditAcsSpJerkff)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVNFRQ, &CMtnTest_Move::OnEnKillfocusEditAcsSlvnfrq)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVNWID, &CMtnTest_Move::OnEnKillfocusEditAcsSlvnwid)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVNATT, &CMtnTest_Move::OnEnKillfocusEditAcsSlvnatt)
	ON_BN_CLICKED(IDC_CHECK_ACS_ENA_NOTCH, &CMtnTest_Move::OnBnClickedCheckAcsEnaNotch)
	ON_BN_CLICKED(IDC_CHECK_ACS_ENA_SOF, &CMtnTest_Move::OnBnClickedCheckAcsEnaSof)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLDRA, &CMtnTest_Move::OnEnKillfocusEditAcsSldra)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLDRX, &CMtnTest_Move::OnEnKillfocusEditAcsSldrx)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SP_K_X_FROM_A, &CMtnTest_Move::OnEnKillfocusEditAcsSpKXFromA)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SP_K_A_FROM_X, &CMtnTest_Move::OnEnKillfocusEditAcsSpKAFromX)
	ON_BN_CLICKED(IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN, &CMtnTest_Move::OnBnClickedCheckShowGroupTestMoveMultiAxisRandBurnIn)
	ON_BN_CLICKED(IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION, &CMtnTest_Move::OnBnClickedCheckShowGroupSetWbTunePosition)
	ON_BN_CLICKED(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA, &CMtnTest_Move::OnBnClickedCheckMovingTestDlgEnableEditAcsPara)
	ON_BN_CLICKED(IDC_CHECK_SHOW_MOVE_TEST_CURRENT_LOOP, &CMtnTest_Move::OnBnClickedCheckShowMoveTestCurrentLoop)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLIOFFS, &CMtnTest_Move::OnEnKillfocusEditAcsSlioffs)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLFRC, &CMtnTest_Move::OnEnKillfocusEditAcsSlfrc)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLFRCD, &CMtnTest_Move::OnEnKillfocusEditAcsSlfrcd)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVB0NF, &CMtnTest_Move::OnEnKillfocusEditAcsSlvb0nf)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVB0DF, &CMtnTest_Move::OnEnKillfocusEditAcsSlvb0df)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVB0ND, &CMtnTest_Move::OnEnKillfocusEditAcsSlvb0nd)
	ON_EN_KILLFOCUS(IDC_EDIT_ACS_SLVB0DD, &CMtnTest_Move::OnEnKillfocusEditAcsSlvb0dd)
	ON_BN_CLICKED(IDC_CHECK_ACS_ENABLE_BIQUAD_MOVING_DLG, &CMtnTest_Move::OnBnClickedCheckAcsEnableBiquadMovingDlg)
END_MESSAGE_MAP()
// CMtnTest_Move message handlers

///// Timer related
//static UINT_PTR m_iTimerVal = NULL;
//UINT_PTR  CMtnTest_Move::StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration)
//{ // IDT_SPEC_TEST_DLG_TIMER
//	m_iTimerVal = SetTimer(nIDT_TimerResMacro, TimerDuration, 0);
//	
//	if (m_iTimerVal == 0)
//	{
//		AfxMessageBox("Unable to obtain timer");
//	}
//    return m_iTimerVal;
//} 
//BOOL  CMtnTest_Move::StopTimer(UINT_PTR nTimerVal)
//{
//	if (!KillTimer (nTimerVal))
//	{
//		return FALSE;
//	}
//	return TRUE;
//}
void CMtnTest_Move::OnClose()
{
	StopBurnInThread();
//	StopTimer(m_iTimerVal);
//	m_iTimerVal = NULL;
	// Start Buffer Program, 20081028
	acsc_StopBuffer(Handle, BUFFER_ID_POSITIVE_MOVE, NULL);
	acsc_StopBuffer(Handle, BUFFER_ID_NEGATIVE_MOVE, NULL);
	iMoveDlgFlagShow = 0;  // 20120202
}

void move_test_random_burn_in_variables()
{
	// Random Burn-In Test related
	if(cFlagInitRandBurnInDefPara == 0)
	{
		int iFlagMachType = get_sys_machine_type_flag();
		if(iFlagMachType == WB_MACH_TYPE_VLED_FORK
			|| iFlagMachType == WB_STATION_XY_VERTICAL)
		{
			aft_rand_burn_in_init_def_var();
			aft_fix_burn_in_init_def_var();
		}
		else if(iFlagMachType == WB_MACH_TYPE_HORI_LED
			|| iFlagMachType == WB_STATION_XY_TOP
			|| iFlagMachType == BE_WB_HORI_20T_LED  // 20120826
			|| iFlagMachType == BE_WB_ONE_TRACK_18V_LED)
		{
			aft_rand_burn_in_init_def_var_hori_led_bonder();
			aft_fix_burn_in_init_def_var();
		}
		else
		{
			aft_rand_burn_in_init_def_var_1_cup_vled_bonder();
			aft_fix_burn_in_init_def_var();
		}
		cFlagInitRandBurnInDefPara = 1;

		//// Move Test Current Loop, 1% of full range, 32768
		stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel = -328;
		stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel = 328;
		stMoveTestCurrLoopStep.uiTotalIter = 100;
	}

}

BOOL CMtnTest_Move::OnInitDialog()
{

	CComboBox *pSelectAxisForAxis1 = (CComboBox*) GetDlgItem(IDC_CHANSEL_MOVE_TEST_AXIS1);
	for(int ii=0; ii<MAX_SERVO_AXIS_WIREBOND; ii++)
	{
		pSelectAxisForAxis1->InsertString(ii, _T(astAxisInfoWireBond[ii].strAxisNameEn));
	}
	pSelectAxisForAxis1->SetCurSel(uiCurrMoveAxis_AppWB);
	uiCurrMoveAxis_ACS = astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS;
// 	pSelectNumAxesCombo->SetCurSel(uiTotalAxes - 1);
	cstrEditText.Format("%d", iAxisTargetPosn_cnt);
	GetDlgItem(IDC_TPOSN_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%8.2f", dCmdGenTs_ms);
	GetDlgItem(IDC_EDIT_MOVE_CMD_GEN_TS_MS)->SetWindowText(cstrEditText);

	int iDebug = 0;
	double dblFbPosition, dblRefPosition;
	mtnapi_get_fb_position(Handle, uiCurrMoveAxis_ACS, &dblFbPosition, iDebug);
	m_iFbPosition_Axis1 = (int)dblFbPosition;

	mtnapi_get_ref_position(Handle, uiCurrMoveAxis_ACS, &dblRefPosition, iDebug);
	m_iPosnErr = (int)(dblRefPosition - dblFbPosition);

	mtnapi_get_motor_state(Handle, uiCurrMoveAxis_ACS, &iMotorState[0], iDebug);
	if(iMotorState[0] & ACSC_MST_ENABLE) 
	{
		// now is enable
		CString cstrEnaDisMotorButton = "Disable";
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
		m_strMotorEnaFlag = "OK";
//		GetDlgItem(IDC_MOTOR_ENA_FLAG_MOVE_TEST_AXIS1)->SetWindowText(cstrEnaDisMotorButton);
	}
	else
	{
		CString cstrEnaDisMotorButton = "Enable";
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
		m_strMotorEnaFlag = "NO";
//		GetDlgItem(IDC_MOTOR_ENA_FLAG_MOVE_TEST_AXIS1)->SetWindowText(cstrEnaDisMotorButton);
	}
	// , IDC_SAMPLE_PERIOD_MS_LABEL
	acsc_ReadReal( Handle, ACSC_NONE, "CTIME", ACSC_NONE, ACSC_NONE, ACSC_NONE, ACSC_NONE, pdTimeClock_ms, NULL);
	cstrSamplePeriodLabel.Format("SamplePeriod (ms): %8.2f, Freq (Hz): %8.1f", pdTimeClock_ms[0], 1000/pdTimeClock_ms[0]);
	GetDlgItem(IDC_SAMPLE_PERIOD_MS_LABEL)->SetWindowText(cstrSamplePeriodLabel);

	// 20130208
	dCmdGenTs_ms = sys_get_controller_ts();


	// 20110721
	UI_InitDialog();

	// Later TBA
	GetDlgItem(IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_FIX_BURN_IN_NUM_SIDES)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE)->EnableWindow(FALSE);

	// 
	m_fStopBurnInThread = TRUE;

	// 2-Point One Axis
	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, iPoint1stIn2PointsOneAxisMove);
	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, iPoint2ndIn2PointsOneAxisMove);
	UpdateUShortToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_TOTAL_ITER, usTotalCycle2PointsOneAxisMove);

	// Language option
	SetUserInterfaceLanguage(get_sys_language_option());
	// DONOT enable upload data
	GetDlgItem(IDC_BUTTON_MOVE_RELATIVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_UPLOAD_DATA_MOVE_TEST)->EnableWindow(FALSE);
//	GetDlgItem(IDC_TOTAL_NUM_AXES_SEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE)->EnableWindow(FALSE);

	InitTuneThemeOption();
	// mtn tuning
	GetDlgItem(IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC)->EnableWindow(TRUE);
	bIsThreadingButtonEnabled = TRUE;
	mMotionTuning.mtn_tune_initialize_class_var(fThresholdSettleTime,  Handle, uiCurrMoveAxis_ACS, 
		uiMotionInterDelay_ms[uiCurrMoveAxis_ACS]);
	// 
	((CButton *)GetDlgItem(IDC_CHECK_MOVE_TEST_TUNING))->SetCheck(cFlagTuningCurrentAxis);

	// 
	InitComboTuningWB_TableBondHeadMotionIdx();
	UpdateIntToEdit(IDC_EDIT_WIRE_ID_MOVE_TEST_GET_TUNE_POSN_SET, uiCurrWireNo);

#ifdef __TBA_LATER__
	// Start Buffer Program, 20081028
	if (!acsc_RunBuffer(Handle, BUFFER_ID_POSITIVE_MOVE, NULL, NULL))
	{
		sprintf_s(strDebugText, 128, "Warning: error start buffer program, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	acsc_RunBuffer(Handle, BUFFER_ID_NEGATIVE_MOVE, NULL, NULL);
#endif // InitRandBurnInGroupTestConfigUI

//	StartTimer(IDT_MOVE_TEST_DLG_TIMER, 200);
	return CDialog::OnInitDialog();

//	return TRUE;
}

// BurnIn Routines
int iiOneAxis2PointsMoveCycle;
int iiBurnInPointInCycle;
char strBurnInDebugFile[64] = "BurnInDebug.txt";
static int iFlagErrorInServoControl = 0;  // 20100305

#include "_MtnTestMove_BurnIn.cpp"
#include "_MtnTestMove_UI.cpp"
static double adServoProtLmtRMS[8];

void CMtnTest_Move::ShowWindow(int nCmdShow)
{
	//if(m_iTimerVal == NULL)
	//{
	//	StartTimer(IDT_MOVE_TEST_DLG_TIMER, 200);
	//}
	int idxAcsAxisWbApp; // 20120715
	for(int ii = 0; ii<= WB_AXIS_WIRE_CLAMP; ii++)
	{
		idxAcsAxisWbApp = astAxisInfoWireBond[ii].iAxisInCtrlCardACS;
		mtnapi_get_speed_profile(Handle, idxAcsAxisWbApp, &stSpeedProfilePerAxis[ii], 0);  // 20120921
	}

	UpdateUI_SpeedProfile();
	InitRandBurnInGroupTestConfigUI();
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis); // 20090930
	UpdateUI_ServoLoopParameter();
	iMoveDlgFlagShow = 1; // 20120202

	acs_read_current_rms_prot_lmt_array_wb13v_cfg(adServoProtLmtRMS); // 20130124

	CDialog::ShowWindow(nCmdShow);
}

void CMtnTest_Move::OnEnChangeTposnAxis1()
{
	GetDlgItem(IDC_TPOSN_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	sscanf_s(strTextTemp, "%d", &iAxisTargetPosn_cnt);
}

void CMtnTest_Move::OnBnClickedMtnTestEnableMotor1()
{
	int iDebug = 0;

	mtnapi_get_motor_state(Handle, uiCurrMoveAxis_ACS, &iMotorState[0], iDebug);
	if(iMotorState[0] & ACSC_MST_ENABLE) 
	{
		// now is enable, to disable it
		mtnapi_disable_motor(Handle, uiCurrMoveAxis_ACS, iDebug);
	}
	else
	{
		// now is disabled, to enable it
		mtnapi_enable_motor(Handle, uiCurrMoveAxis_ACS, iDebug);
	}
}

void CMtnTest_Move::OnBnClickedButton3()
{
	unsigned int uiSleep_ms = 2000;

	// Setup scope for motion
	mtnscope_set_acsc_var_collecting_move(uiCurrMoveAxis_ACS);
	// Start collection
	if (!acsc_CollectB(Handle, 0, // system data collection
		gstrScopeArrayName, // name of data collection array
		gstSystemScope.uiDataLen, // number of samples to be collected
		(int)dCmdGenTs_ms, // sampling period 1 millisecond, dCmdGenTs_ms, 20130208
		strACSC_VarName, // variables to be collected
		NULL)
		)
	{
		printf("transaction error: %d\n", acsc_GetLastError());
	}
	// Start motion
	acsc_ToPoint(Handle, 0, // start up immediately the motion
		uiCurrMoveAxis_ACS, iAxisTargetPosn_cnt, NULL);
	
	Sleep(uiSleep_ms);
	if (!acsc_StopCollect(Handle, NULL))
	{
		printf("transaction error: %d\n", acsc_GetLastError());
	}
	while(qc_is_axis_still_acc_dec(Handle, uiCurrMoveAxis_ACS))
	{
		Sleep(10); 	//Sleep(2);
	}
	if(qc_is_axis_not_safe(Handle, uiCurrMoveAxis_ACS))
	{
		mtn_api_prompt_acs_error_code(Handle);
	}
}

//#include "MtnDrawCurve.h"

void CMtnTest_Move::OnBnClickedUploadDataMoveTest()
{
	// TODO: Add your control notification handler code here
	mtnscope_upload_acsc_data(Handle);

	mtn_save_curr_axis_traj_data_to_file(uiCurrMoveAxis_ACS);

	// Allocate a new frame window object
//	CWndDrawCurveMtnTest* pFrame = new CWndDrawCurveMtnTest;

	// Initialize the frame window
//   pFrame->Create(0, 
//      "A simple curve for scope",
//      WS_POPUPWINDOW | WS_DLGFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX, // WS_BORDER |
//      CRect(0, 0, GRAPH_DEF_800_PIXEL, GRAPH_DEF_600_PIXEL));

	// Assign the frame window as the app's main frame window
//   this->m_pMainWnd = pFrame;

	// Show the frame window maximized
//   pFrame->ShowWindow(1);
//   pFrame->UpdateWindow();

//   return TRUE;

}

extern char *strTuningOption[];
extern char *strTuningStageGroupThread[];
void CMtnTest_Move::UpdateMovingThreadStatus()
{
	// Random Burn-In Related
	CString cstrTemp;
	if(	iFlagBurnIn == MTN_TEST_BURN_IN_MULTI_AXIS_FIX_PTN)
	{
		cstrTemp.Format("%d-F:CurPt %d/%d, CurCyc %d / Total %d", 
			iFlagBurnIn,
			iiBurnInPointInCycle, usActualNumPointsPerCycle_FixBurnIn, 
			uiCurrBurnInCycle, uiTotalNumCycleBurnIn);
	}
	else if(iFlagBurnIn == MTN_TEST_BURN_IN_MULTI_AXIS_RANDOM )
	{
		cstrTemp.Format("%d-R:CurPt %d/%d, CurCyc %d / Total %d", 
			iFlagBurnIn,
			iiBurnInPointInCycle, usRandBurnInNumPointsPerCycle, 
			uiCurrBurnInCycle, uiTotalNumCycleBurnIn);
	}
	else if( iFlagBurnIn == MTN_TEST_BURN_IN_ONE_AXIS_2_POINTS
		|| iFlagBurnIn == MTN_TEST_CURR_LOOP_STEP_TEST)
	{
		cstrTemp.Format("%d-2P-Curr, CurCyc %d / Total %d", 
			iFlagBurnIn,
			uiCurrBurnInCycle, uiTotalNumCycleBurnIn);
	}
	else if(mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == TRUE)
	{
		int iTuningOption = mMotionTuning.mtn_tune_get_option();
		int iTuningStage = mMotionTuning.mtn_tune_get_stage_group_thread();
		cstrTemp.Format("%s-[%d, %d]; %s - %d; %d/%d", strTuningOption[iTuningOption], mMotionTuning.mtn_tune_get_move_point_1(), mMotionTuning.mtn_tune_get_move_point_1(),
			strTuningStageGroupThread[iTuningStage], mMotionTuning.mtn_tune_get_group_loop_iter(), 
			mMotionTuning.mtn_tune_get_curr_iter_within_one_group(),	mMotionTuning.mtn_tune_get_total_num_iter_for_one_group());
	}
	else
	{
	}
	GetDlgItem(IDC_STATIC_TEST_MOVING_THREAD_STATUS)->SetWindowTextA(cstrTemp);
}
int CMtnTest_Move::UI_GetShowWindowFlag()
{
	return iMoveDlgFlagShow;
}
void CMtnTest_Move::EnableButtonThreading(BOOL bFlagEnable)
{
	GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_MOVE_RELATIVE)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_START_2_POINTS_MOVE)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_TEST_MOVE_FIX_BURN_IN_START)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_BUTTON_MOVE_TEST_CURR_STEP_START)->EnableWindow(bFlagEnable); // 20100305
	bIsThreadingButtonEnabled = bFlagEnable;

}

void CMtnTest_Move::EnableSet_AcsParameter(BOOL bFlagEnable)
{
	GetDlgItem(IDC_EDIT_ACS_SLVNFRQ)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVNWID)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVNATT)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_CHECK_ACS_ENA_NOTCH)->EnableWindow(bFlagEnable);

	// SOF
	GetDlgItem(IDC_EDIT_ACS_SOF)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_CHECK_ACS_ENA_SOF)->EnableWindow(bFlagEnable);

	// BIQUAD
	GetDlgItem(IDC_EDIT_ACS_SLVB0NF)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DF)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVB0ND)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DD)->EnableWindow(bFlagEnable);

	GetDlgItem(IDC_CHECK_ACS_ENABLE_BIQUAD_MOVING_DLG)->EnableWindow(bFlagEnable); // 20121106
}

void CMtnTest_Move::DlgTimerEvent()
{
	UpdateAxisFeedback();
	// ACS-Parameter

	// Axis-Tuning
	UpdateAxisTuningStatus();

	// Thread: Burn-in, Tuning
	if(mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == TRUE ||
		iFlagBurnIn != MTN_TEST_BURN_IN_FLAG_NONE)
	{
		UpdateMovingThreadStatus();
		UpdateUI_ServoLoopParameter();
	}

	if(!m_fStopBurnInThread ||
		mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == TRUE)
	{
		EnableButtonThreading(FALSE);
	}
	else
	{
		EnableButtonThreading(TRUE);
		if(iFlagBurnIn != MTN_TEST_BURN_IN_FLAG_NONE)
		{
			//mtn_test_set_burn_in_flag(MTN_TEST_BURN_IN_FLAG_NONE);
			//if(stTuneAffOutput.iErrorCode == __MTN_TUNE_SLAFF_SUCCESS_RET)
			//{
			//	stServoLoopAcsCtrlPara_CurrAxis.dAccelerationFeedforward = stTuneAffOutput.fServoLoopAFF;
			//	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis); // 20090831
			//}
			if(stTuneAffOutput.iErrorCode == __MTN_TUNE_SLAFF_SUCCESS_RET)
			{
				CString cstrTemp; 
				cstrTemp.Format("%4.0f", stTuneAffOutput.fServoLoopAFF);
				GetDlgItem(IDC_STATIC_MOVE_TEST_BEST_FFC_PARA_TUNE)->SetWindowTextA(cstrTemp);
			}
		}
	}

	// Update UI Enable Flag
	mtnapi_get_motor_state(Handle, uiCurrMoveAxis_ACS, &iMotorState[0], iDebug);
	if(iMotorState[0] & ACSC_MST_ENABLE) 
	{
		// now is enable
		CString cstrEnaDisMotorButton = "Disable";
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
		m_strMotorEnaFlag = "OK";

		// Enable Setting Some Parameters
		EnableSet_AcsParameter(FALSE);
	}
	else
	{
		CString cstrEnaDisMotorButton = "Enable";
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
		m_strMotorEnaFlag = "NO";
		// Enable Setting Some Parameters
		EnableSet_AcsParameter(bFlagEnableEditAcsParameter);  // TRUE, 20110721
	}
	GetDlgItem(IDC_MOTOR_ENA_FLAG_MOVE_TEST_AXIS1)->SetWindowText(m_strMotorEnaFlag);

}

//void CMtnTest_Move::OnTimer(UINT nTimerVal)
//{
//// DlgTimerEvent();  // 20120202
//}

void CMtnTest_Move::UpdateAxisTuningStatus()
{
	if(mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == TRUE) // only freq update when tuning process going (not stopped)
	{
		UpdateUI_ServoLoopParameter();
		// IDC_STATIC_TEST_MOVING_THREAD_STATUS
	}
	else
	{
		if(!bFlagIsEnabledAxisServoParaACS)
		{
			EnableAcsParaUI_DuringTuning(bFlagEnableEditAcsParameter);  // TRUE, 20110721
		}
	}
}
UINT MoveTest_BurnInThreadProc( LPVOID pParam )
{
    CMtnTest_Move* pObject = (CMtnTest_Move *)pParam;
	return pObject->BurnInThread(); 	
}

void CMtnTest_Move::RunBurnInThread()
{
	m_fStopBurnInThread = FALSE;
	m_pWinThread = AfxBeginThread(MoveTest_BurnInThreadProc, this, THREAD_PRIORITY_NORMAL); // THREAD_PRIORITY_TIME_CRITICAL);
	m_pWinThread->m_bAutoDelete = FALSE;
}

void CMtnTest_Move::StopBurnInThread()
{
	if (m_pWinThread)
	{
		m_fStopBurnInThread = TRUE;
		WaitForSingleObject(m_pWinThread->m_hThread, 1500);
//		delete m_pWinThread;
		m_pWinThread = NULL;
	}
}

void CMtnTest_Move::UpdateAxisFeedback()
{
	// IDC_STATIC_TEST_MOVE_SINGLE_AXIS
	CString cstrAcsAxisLabel;
	cstrAcsAxisLabel.Format("AxisACS(%d):", uiCurrMoveAxis_ACS);
	GetDlgItem(IDC_STATIC_TEST_MOVE_SINGLE_AXIS)->SetWindowTextA(cstrAcsAxisLabel);

		mtnapi_get_fb_position(Handle, uiCurrMoveAxis_ACS, &dblFbPosition, iDebug);
//		m_iFbPosition_Axis1 = (int)dblFbPosition;
		mtnapi_get_ref_position(Handle, uiCurrMoveAxis_ACS, &dblRefPosition, iDebug);
//		m_iPosnErr = (int)(dblRefPosition - dblFbPosition);

		cstrFbPosition[0].Format("%d", (int)dblFbPosition);
		GetDlgItem(IDC_FPOSN_MOVE_TEST_AXIS1)->SetWindowText(cstrFbPosition[0]);
		cstrPosnErr[0].Format("%d", (int)(dblRefPosition - dblFbPosition));
		GetDlgItem(IDC_POSNERR_MOVE_TEST_AXIS1)->SetWindowText(cstrPosnErr[0]);

		mtnapi_get_motor_state(Handle, uiCurrMoveAxis_ACS, &iMotorState[0], iDebug);
		if(iMotorState[0] & ACSC_MST_ENABLE) 
		{
			// now is enable
			CString cstrEnaDisMotorButton = "Disable";
			GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
			m_strMotorEnaFlag = "OK";
		}
		else
		{
			CString cstrEnaDisMotorButton = "Enable";
			GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowText(cstrEnaDisMotorButton);
			m_strMotorEnaFlag = "NO";
		}
		GetDlgItem(IDC_MOTOR_ENA_FLAG_MOVE_TEST_AXIS1)->SetWindowText(m_strMotorEnaFlag);

		// ACSC_MST_MOVE
		if(iMotorState[0] & ACSC_MST_MOVE)
		{
			m_strFlagMove = "Y";
		}
		else
		{
			m_strFlagMove = "N";
		}
		GetDlgItem(IDC_MOVING_FLAG_MOVE_TEST_AXIS1)->SetWindowText(m_strFlagMove);

		// ACSC_MST_ACC
		if(iMotorState[0] & ACSC_MST_ACC)
		{
			m_strFlagAcc_Axis1 = "Y";
		}
		else
		{
			m_strFlagAcc_Axis1 = "N";
		}
		GetDlgItem(IDC_ACC_FLAG_MOVE_TEST_AXIS1)->SetWindowText(m_strFlagAcc_Axis1);

		// ACSC_MST_INPOS
		if(iMotorState[0] & ACSC_MST_INPOS)
		{
			m_strFlagInPosn_Axis1 = "Y";
		}
		else
		{
			m_strFlagInPosn_Axis1 = "N";
		}
		GetDlgItem(IDC_INPOSN_FLAG_MOVE_TEST_AXIS1)->SetWindowText(m_strFlagInPosn_Axis1);

		//CMtnTest_Move::UpdateData(TRUE);

}
void CMtnTest_Move::ServoTuneUploadSaveCalcIndex()
{
	CString cstrTemp;

	// Upload data
	mtnscope_upload_acsc_data(Handle);

	// init data buffer point and position error array
	mtn_tune_init_array_ptr();
	mtn_tune_calc_time_idx_2_loop_move(&stMtnTimeIdx2LoopMove);

	cstrTemp.Format("[%d, %d; %d, %d; %d, %d; %d, %d]", stMtnTimeIdx2LoopMove.idx1stMoveFrontStart, stMtnTimeIdx2LoopMove.idx1stMoveFrontEnd,
		stMtnTimeIdx2LoopMove.idx1stMoveBackStart, stMtnTimeIdx2LoopMove.idx1stMoveBackEnd,
		stMtnTimeIdx2LoopMove.idx2ndMoveFrontStart, stMtnTimeIdx2LoopMove.idx2ndMoveFrontEnd,
		stMtnTimeIdx2LoopMove.idx2ndMoveBackStart, stMtnTimeIdx2LoopMove.idx2ndMoveBackEnd);

	// IDC_STATIC_MTN_TEST_TIME_INDEX
	GetDlgItem(IDC_STATIC_MTN_TEST_TIME_INDEX)->SetWindowTextA(cstrTemp);

	// Calculate servo tuning index
	mtn_tune_calc_servo_perform_index(fThresholdSettleTime, &stMtnTimeIdx2LoopMove, &stMtnServoTuneIndex);

// IDC_STATIC_MOVE_TEST_TUNE_SPEC_SETTLING_TIME
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_OVER_UNDER_SHOOT
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_CMD_END_OU_SHOOT
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_MAX_DPE
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_RMS_DAC
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_ERROR_REPEATIBLE
	cstrTemp.Format("F%4.0f, B%4.0f        ", stMtnServoTuneIndex.fSettleTimeFrontDir, stMtnServoTuneIndex.fSettleTimeBackDir);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_SETTLING_TIME)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("F%5.0f, B%5.0f        ", stMtnServoTuneIndex.fCmdEndFrontOUS, stMtnServoTuneIndex.fCmdEndBackOUS);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_CMD_END_OU_SHOOT)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("FOU-%3.0f, %3.0f; BOU-%3.0f, %3.0f", stMtnServoTuneIndex.fMaxFrontOS, stMtnServoTuneIndex.fMaxFrontUS,
		stMtnServoTuneIndex.fMaxBackOS, stMtnServoTuneIndex.fMaxBackUS);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_OVER_UNDER_SHOOT)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("F%4.0f, B%4.0f    ", stMtnServoTuneIndex.fMaxFrontMoveDPE, stMtnServoTuneIndex.fMaxBackMoveDPE);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_MAX_DPE)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%6.2f    ", stMtnServoTuneIndex.fRptErrorRMS);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_ERROR_REPEATIBLE)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%4.1f <%3.1f, %4.1f <85%%   ", 
		stMtnServoTuneIndex.fRMS_Over32767_DAC * 100.0, adServoProtLmtRMS[uiCurrMoveAxis_ACS],
		stMtnServoTuneIndex.fPeakPercentDAC * 100.0);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TUNE_SPEC_RMS_DAC)->SetWindowTextA(cstrTemp);
}

//////////////////////////////////////////////////////////////////////////
UINT CMtnTest_Move::BurnInThread()
{

char strCmdTextACS[512];
// Setup scope for motionn
mtnscope_set_acsc_var_collecting_move(uiCurrMoveAxis_ACS);
// DC rAFT_Scope, 4200, rAFT_PrbsSampleTime_ms, X_FVEL, X_RVEL, Y_FVEL, Y_RVEL, A_FVEL, A_RVEL
sprintf_s(strCmdTextACS, 512, "DC %s, %d, %5.2f, %s\n\r", gstrScopeDataVarName, gstSystemScope.uiDataLen, dCmdGenTs_ms, strACSC_VarName);

//
	// every 100 ms communication history is updated
	while (!m_fStopBurnInThread && (uiCurrBurnInCycle <= uiTotalNumCycleBurnIn)) // uiRandBurnInTotalNumCycle
	{
		if(cFlagStartCollectingData == 0 && (uiCurrBurnInCycle % 2 == 0)
			&& iFlagBurnIn != MTN_TEST_CURR_LOOP_STEP_TEST) // Donot waist time to capture data
		{
			// Start collection
			if (!acsc_Send(Handle, strCmdTextACS, strlen(strCmdTextACS)+2, NULL)
				//acsc_CollectB(Handle, 0, // system data collection
				//gstrScopeArrayName, // name of data collection array
				//gstSystemScope.uiDataLen, // number of samples to be collected
				//dCmdGenTs_ms, // sampling period 1 millisecond, dCmdGenTs_ms, 20130208
				//strACSC_VarName, // variables to be collected
				//NULL)
				)
			{
				mtn_api_prompt_acs_error_code(Handle);
			}
			else
			{
				mtn_move_test_set_flag_collecting_data(); // 2010Feb05
			}
		}

		if(iFlagBurnIn == MTN_TEST_BURN_IN_MULTI_AXIS_RANDOM)
		{
			if(RandBurnInOneCycle() )
			{
				// error happens motor cutoff
				m_fStopBurnInThread = 1;
			}
			else
			{
				uiCurrBurnInCycle ++;
			}
		}
		else if(iFlagBurnIn == MTN_TEST_BURN_IN_MULTI_AXIS_FIX_PTN)
		{
			if(FixBurnInOneCycle() )
			{
				// error happens motor cutoff
				m_fStopBurnInThread = 1;
			}
			else
			{
				uiCurrBurnInCycle ++;
			}
		}
		else if(iFlagBurnIn == MTN_TEST_BURN_IN_ONE_AXIS_2_POINTS)
		{
			if(OneAxis2PointsMoveOneCycle())
			{
				// error happens motor cutoff
				m_fStopBurnInThread = 1;
			}
			else
			{
				uiCurrBurnInCycle ++;
			}
		}
		else if(iFlagBurnIn == MTN_TEST_CURR_LOOP_STEP_TEST)
		{
			CurrentLoopStepMoveTestOneCycle();
			uiCurrBurnInCycle ++;
			if(uiCurrBurnInCycle >= uiTotalNumCycleBurnIn)	ClearCurrentCommandAllAxis();
		}
		else
		{
			m_fStopBurnInThread = 1;
		}

		if(cFlagStartCollectingData == 1 && (uiCurrBurnInCycle % 2 == 0))
		{
			ServoTuneUploadSaveCalcIndex();
			mtn_move_test_clear_flag_collecting_data(); // 2010Feb05
		}
		// Normal exit, have reached total number of cycles
		if(uiCurrBurnInCycle >= uiTotalNumCycleBurnIn + 1)
		{
			m_fStopBurnInThread = 1;

			// Only Save the last motion
			if(cFlagSaveScopeDataInFile)
			{
				mtn_save_curr_axis_traj_data_to_file(uiCurrMoveAxis_ACS); 
			}

		}
		else if(uiCurrBurnInCycle == 2)
		{
			// Only Save the 1st motion
			if(cFlagSaveScopeDataInFile)
			{
				mtn_save_curr_axis_traj_data_to_file(uiCurrMoveAxis_ACS);
			}

		}
//		Sleep(10);
	}
	iFlagBurnIn = MTN_TEST_BURN_IN_FLAG_NONE;
	UpdateUI_ServoLoopParameter(); // 20130218
	return 0;
}

void CMtnTest_Move::OnCbnSelchangeChanselMoveTestAxis1()
{
	// TODO: Add your control notification handler code here
//	CDialog::UpdateData(FALSE); // From view to the class

	// Clear current prof cmd buffer
	ClearAxisProfCmdBuffer(uiCurrMoveAxis_ACS);
	// IDC_CHANSEL_MOVE_TEST_AXIS1
	CComboBox *pSelectAxisForAxis1 = (CComboBox*) GetDlgItem(IDC_CHANSEL_MOVE_TEST_AXIS1);
//	>SetCurSel(uiCurrMoveAxis_AppWB);
	uiCurrMoveAxis_AppWB = pSelectAxisForAxis1->GetCurSel();
	uiCurrMoveAxis_ACS = astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS;
	UpdateUI_SpeedProfile();
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis); // 20090930
	UpdateUI_ServoLoopParameter();
	UpdateUI_AxisCommandSetting();
	ProtectionCommandPointOverLimit();


	if(uiCurrMoveAxis_AppWB == WB_AXIS_WIRE_CLAMP)
	{
		stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel = -8328;
		stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel = 8328;
	}
	else
	{
		stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel = -328;
		stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel = 328;		
	}
	InitCurrLoopStepTestUI();
	OnEnKillfocusEditMoveTestCurrStepUppLevel();
	OnEnKillfocusEditMoveTestCurrStepLowLevel();

}

void CMtnTest_Move::OnBnClickedButtonMoveRelative()
{
	// TODO: Add your control notification handler code here
	double 		fFirstDist;
	unsigned int nTrajSize, nTrajSize2;
	unsigned int uiAxisTemp;

	switch(uiCurrMoveAxis_ACS)
	{
	case 0:
		fFirstDist = dRelDistance_mm /1000/X_ENC_UNIT_UM * 1E+6;
		break;
	case 1:
		fFirstDist = dRelDistance_mm /1000/Y_ENC_UNIT_UM * 1E+6;
		break;
	case 4:
		fFirstDist = dRelDistance_mm /1000/A_ENC_UNIT_UM * 1E+6;
		break;
	}

	mtn_twin_sine_set_speed_limit_(&lstTwinSinePreCalInput[uiCurrMoveAxis_ACS], &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], 
		dCmdGenTs_ms/1000.0, fFirstDist);

	if(acs_bufprog_write_traj_move_time_unit_ms(dCmdGenTs_ms) != MTN_API_OK_ZERO)
	{
		sprintf_s(strDebugText, 128, "Write CmdGenPeriod_ms error, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strDebugText));
	}

	// calculate profile total time
	// Positive distance move
	uiAxisTemp = uiCurrMoveAxis_ACS;
	lstTwinSinePreCalInput[uiAxisTemp].fDist = fFirstDist;
	twinsine_precal(&lstTwinSinePreCalInput[uiAxisTemp],
		(&luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput));
	nTrajSize = (unsigned int)(luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput.fT1 * 2 + luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput.fT2 + 1 );
	if(nTrajSize > TRAJECTORY_BUFFER_SIZE)
	{
		AfxMessageBox(_T("Error, exceeding maximum trajectory length"));
		return;
	}

	//////////////////////////////
	// command generation
	twinsine_profgen(&luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSingProfGenInput, &lstTwinSineProfGenOutput[uiAxisTemp]);
	// download 3 axis -- hard-coded at buffer program
	if(acs_bufprog_download_vel_prof_buff_8(0, nTrajSize, lstTwinSineProfGenOutput[0].pfvel_prof) != MTN_API_OK_ZERO)
	{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
	}
	if(acs_bufprog_download_vel_prof_buff_8(1, nTrajSize, lstTwinSineProfGenOutput[1].pfvel_prof) != MTN_API_OK_ZERO)
	{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
	}
	if(uiAxisTemp == AXIS_Z_MAPPING_ACS_A)
	{
		if(acs_bufprog_download_vel_prof_buff_8(2, nTrajSize, lstTwinSineProfGenOutput[uiAxisTemp].pfvel_prof) != MTN_API_OK_ZERO)
		{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
		}
	}

	acs_bufprog_write_traj_len_buff_8(nTrajSize); // acsc_WriteDPRAMInteger(Handle, DPRAM_ADDR_PATH_LEN_BUF_8, nTrajSize);

	// Negative distance move
	uiAxisTemp = uiCurrMoveAxis_ACS;
	lstTwinSinePreCalInput[uiAxisTemp].fDist = - fFirstDist;
	twinsine_precal(&lstTwinSinePreCalInput[uiAxisTemp],
		(&luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput));
	nTrajSize2 = (unsigned int)(luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput.fT1 * 2 
		+ luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSinePreCalOutput.fT2 + 1);
	if(nTrajSize > TRAJECTORY_BUFFER_SIZE)
	{
		AfxMessageBox(_T("Error, exceeding maximum trajectory length"));
//		return;
	}
	// command generation
	twinsine_profgen(&luCalcTwinSinePrecOutProfGenIn[uiAxisTemp].stTwinSingProfGenInput, &lstTwinSineProfGenOutput[uiAxisTemp]);


	////////////////////////////////
	// download 3 axis- Hard-coded from buffer program

	if(acs_bufprog_download_vel_prof_buff_9(0, nTrajSize, lstTwinSineProfGenOutput[0].pfvel_prof) != MTN_API_OK_ZERO)
	{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
	}
	if(acs_bufprog_download_vel_prof_buff_9(1, nTrajSize, lstTwinSineProfGenOutput[1].pfvel_prof) != MTN_API_OK_ZERO)
	{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
	}
	if(uiAxisTemp == AXIS_Z_MAPPING_ACS_A)
	{
		if(acs_bufprog_download_vel_prof_buff_9(2, nTrajSize, lstTwinSineProfGenOutput[uiAxisTemp].pfvel_prof) != MTN_API_OK_ZERO)
		{
			sprintf_s(strDebugText, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
			AfxMessageBox(_T(strDebugText));
		}
	}
	acs_bufprog_write_traj_len_buff_9(nTrajSize2); // acsc_WriteDPRAMInteger(Handle, DPRAM_ADDR_PATH_LEN_BUF_9, nTrajSize2);

//	for(unsigned int ii = 0; ii< nTrajSize2; ii++)
//	{
//		aTime_ms[ii] = dCmdGenTs_ms * ii; // ; //
//	}
//	if (!acsc_WriteReal(Handle, BUFFER_ID_NEGATIVE_MOVE, "rAFT_TrajectoryPoints_buffer9", 3, 3, 0, nTrajSize2 - 1, aTime_ms, NULL ))
//	{
//		sprintf_s(strDebugText, 128, "download time error, Error Code: %d", acsc_GetLastError());
//		AfxMessageBox(_T(strDebugText));
////			return;
//	}

	acs_bufprog_write_motion_flag_buff_8(ACS_BUFPROG_MTN_FLAG_CLOSE_LOOP); // either ProfMove in CloseLoop or PWN in OpenLoop
	// IDC_STATIC_SHOW_CMD_LEN
	sprintf_s(strDebugText, 128, "CmdLen:%d <> %d", nTrajSize2, TRAJECTORY_BUFFER_SIZE);
	GetDlgItem(IDC_STATIC_SHOW_CMD_LEN)->SetWindowTextA(_T(strDebugText));

	for(unsigned int ii=0; ii<uiTotalNumLoopRelMove; ii++)
	{
		// wait until finishing current motion
		while(mtn_qc_is_axis_still_moving(Handle, uiCurrMoveAxis_ACS))
		{
			Sleep(2);
		}
		if(mtn_qc_is_axis_locked_safe(Handle, uiCurrMoveAxis_ACS) == FALSE)
		{
			break;
		}
		Sleep(uiMotionInterDelay_ms[uiCurrMoveAxis_ACS]);
		acs_bufprog_start_buff_8();
		// downloading next buffer can be done here to save timing

		while(mtn_qc_is_axis_still_moving(Handle, uiCurrMoveAxis_ACS))
		{
			Sleep(2);
		}
		if(mtn_qc_is_axis_locked_safe(Handle, uiCurrMoveAxis_ACS) == FALSE)
		{
			break;
		}
		// run buffer, path move
		Sleep(uiMotionInterDelay_ms[uiCurrMoveAxis_ACS]);
		acs_bufprog_start_buff_9(); //acsc_WriteDPRAMInteger(Handle, DPRAM_ADDR_START_MOVE_BUF_9, 1);
		// downloading next buffer can be done here to save timing
	}

	SaveTrajMoveScopeData(fFirstDist, nTrajSize, nTrajSize2);
}

void CMtnTest_Move::SaveTrajMoveScopeData(double fFirstDist, int nTrajSize, int nTrajSize2)
{
	// Upload data
#define FILE_NAME_PROF_MOVE "TestMove.m"
	FILE *fpData;
	CString cstrTemp;

	gstSystemScope.uiNumData = 6;
	gstSystemScope.uiDataLen = 5000;
	gstSystemScope.dSamplePeriod_ms = 0.5;
	//mtnscope_upload_acsc_data(Handle);
	if (!acsc_ReadReal( Handle, BUFFER_ID_POSITIVE_MOVE, "rAFT_Scope", 0, 5, 0, 3000 - 1, gdScopeCollectData, NULL))
	{
		cstrTemp.Format("transaction error: %d\n", acsc_GetLastError());
		AfxMessageBox(cstrTemp);
	}
	else
	{	
		fopen_s(&fpData, FILE_NAME_PROF_MOVE, "w");

		int ii, jj;
		fprintf(fpData, "%% Motion Calculation in Host\n");
		fprintf(fpData, "tCmdGen_ms = %6.2f\n", dCmdGenTs_ms);
		fprintf(fpData, "aDist1_2 = [%10.1f, %10.1f]  \n", fFirstDist, - fFirstDist);
		fprintf(fpData, "anTrajSize = [%d,  %d] \n", nTrajSize, nTrajSize2);
		fprintf(fpData, "matCalculateProfile = [ %%%% Dist, Vel, Acc, Jerk \n ");
		for(ii = 0; ii<nTrajSize2; ii++)
		{
			fprintf(fpData, "%8.2f,  %8.5f, %8.5f, %8.5f \n", 
				lstTwinSineProfGenOutput[uiCurrMoveAxis_ACS].pfdist_prof[ii],
				lstTwinSineProfGenOutput[uiCurrMoveAxis_ACS].pfvel_prof[ii],
				lstTwinSineProfGenOutput[uiCurrMoveAxis_ACS].pfacc_prof[ii],
				lstTwinSineProfGenOutput[uiCurrMoveAxis_ACS].pfjerk_prof[ii]);
		}
		fprintf(fpData, "];\n\n");
//		fprintf(fpData, ""
		fprintf(fpData, "%% ACSC Controller, Axis- %d:  X_FVEL, X_RVEL, Y_FVEL, Y_RVEL, A_FVEL, A_RVEL\n aProfileMotion = [    ", 
			uiCurrMoveAxis_ACS);
		for(ii = 0; ii<3000; ii++)
		{
			for(jj = 0; jj<6; jj++)
			{
				if(jj == 6 - 1)
				{	
					fprintf(fpData, "%8.2f", gdScopeCollectData[jj* 3000 + ii]);
				}
				else
				{
					fprintf(fpData, "%8.2f,  ", gdScopeCollectData[jj* 3000 + ii]);
				}
			}
			fprintf(fpData, "\n");
		}
		fprintf(fpData, "];\n");
		fclose(fpData);

	}
}

void CMtnTest_Move::OnEnChangeEditRelDist1()
{
	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_EDIT_REL_DIST_1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//sscanf(strTextTemp, "%f", &);
	dRelDistance_mm = strtod(strTextTemp, &strStopString);
}

void CMtnTest_Move::OnEnChangeEditNumLoopRelMove()
{
	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_EDIT_NUM_LOOP_REL_MOVE)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	sscanf_s(strTextTemp, "%d", &uiTotalNumLoopRelMove);
}
// IDC_EDIT_MOVE_CMD_GEN_TS_MS <=> dCmdGenTs_ms
void CMtnTest_Move::OnEnChangeEditMoveCmdGenTsMs()
{
	GetDlgItem(IDC_EDIT_MOVE_CMD_GEN_TS_MS)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//sscanf(strTextTemp, "%f", &);
	dCmdGenTs_ms = strtod(strTextTemp, &strStopString);
}


// IDC_STATIC_GROUP_SINGLE_AXIS

#include "MtnAft.h"
// IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START
// IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_STOP
void CMtnTest_Move::OnBnClickedButtonMoveTestRandomBurnInStart()
{
	unsigned int uiRandSeed;
	uiRandSeed = 0; // 20111024, Set the same test condition for all test, (unsigned int) time(NULL);
	aft_rand_burn_in_init_target_point_per_cycle(uiRandSeed);
	uiCurrBurnInCycle = 0;
	mtn_test_set_burn_in_flag(MTN_TEST_BURN_IN_MULTI_AXIS_RANDOM); // 2010Feb05
	uiTotalNumCycleBurnIn = uiRandBurnInTotalNumCycle;

	mtn_aft_init_random_burn_in_condition(Handle);  // 20111019
	RunBurnInThread();
}

void CMtnTest_Move::OnBnClickedButtonMoveTestRandomBurnInStop()
{
	mtn_aft_restore_speed_profile(Handle);   // 20111019
	StopBurnInThread();
}

// IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1
void CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis1()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, &aiRandBurnInAxisUpperLimit[0]);
}

// IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2
void CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis2()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, &aiRandBurnInAxisUpperLimit[1]);
}

// IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3
void CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis3()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, &aiRandBurnInAxisUpperLimit[2]);
}

// IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4
void CMtnTest_Move::OnEnChangeEditUppLmtMoveTestRandomBurnInAxis4()
{
	ReadIntegerFromEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, &aiRandBurnInAxisUpperLimit[3]);
}

// IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1
void CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis1()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, &aiRandBurnInAxisLowerLimit[0]);
}

// IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2
void CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis2()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, &aiRandBurnInAxisLowerLimit[1]);
}

// IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3
void CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis3()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, &aiRandBurnInAxisLowerLimit[2]);
}

// IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4
void CMtnTest_Move::OnEnChangeEditLowLmtMoveTestRandomBurnInAxis4()
{
	ReadIntegerFromEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, &aiRandBurnInAxisLowerLimit[3]);
}
// IDC_EDIT_NUM_POINTS_PER_CYCLE_MOVE_TEST_RANDOM_BURN_IN
void CMtnTest_Move::OnEnChangeEditNumPointsPerCycleMoveTestRandomBurnIn()
{
	ReadUShortFromEdit(IDC_EDIT_NUM_POINTS_PER_CYCLE_MOVE_TEST_RANDOM_BURN_IN, &usRandBurnInNumPointsPerCycle);
}

// IDC_EDIT_TOTAL_NUM_CYCLE_MOVE_TEST_RANDOM_BURN_IN
void CMtnTest_Move::OnEnChangeEditTotalNumCycleMoveTestRandomBurnIn()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_TOTAL_NUM_CYCLE_MOVE_TEST_RANDOM_BURN_IN, &uiRandBurnInTotalNumCycle);
}

// IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_1_ENVOLVED
void CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis1Envolved()
{
	abQcBurnInAxisEnvolveFlag[0] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_1_ENVOLVED))->GetCheck();
}

// IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_2_ENVOLVED
void CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis2Envolved()
{
	abQcBurnInAxisEnvolveFlag[1] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_2_ENVOLVED))->GetCheck();
}

// IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_3_ENVOLVED
void CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis3Envolved()
{
	abQcBurnInAxisEnvolveFlag[2] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_3_ENVOLVED))->GetCheck();
}

// IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_4_ENVOLVED
void CMtnTest_Move::OnBnClickedCheckMoveTestRandomBurnInAxis4Envolved()
{
	abQcBurnInAxisEnvolveFlag[3] = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_4_ENVOLVED))->GetCheck();
}

// IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_1
void CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis1()
{
	asQcBurnInAxisIdAcs[0] = ((CComboBox*) GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_1))->GetCurSel();
}

// IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_2
void CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis2()
{
	asQcBurnInAxisIdAcs[1] = ((CComboBox*) GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_2))->GetCurSel();
}

// IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3
void CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis3()
{
	asQcBurnInAxisIdAcs[2] = ((CComboBox*) GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3))->GetCurSel();
}

// IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_4
void CMtnTest_Move::OnCbnSelchangeComboSelMoveTestRandomBurnInAxis4()
{
	asQcBurnInAxisIdAcs[3] = ((CComboBox*) GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_4))->GetCurSel();
}

void CMtnTest_Move::SetUserInterfaceLanguage(int iLanguageOption)
{
//	
	if(iLanguageOption == LANGUAGE_UI_EN)
	{
//		GetDlgItem(IDC_SAMPLE_PERIOD_MS_LABEL)->SetWindowTextA(_T("ProfGenSample(ms)"));
		GetDlgItem(IDC_STATIC_GROUP_SINGLE_AXIS)->SetWindowTextA(_T("SingleAxis"));
		GetDlgItem(IDC_STATIC_TEST_MOVE_SINGLE_AXIS)->SetWindowTextA(_T("Axis"));
		GetDlgItem(IDC_STATIC_TEST_MOVE_SINGLE_AXIS_TARGET_POSN)->SetWindowTextA(_T("TargetPosn(cnt)"));
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowTextA(_T("Enable"));
		GetDlgItem(IDC_BUTTON3)->SetWindowTextA(_T("MoveToTarget"));
		GetDlgItem(IDC_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN)->SetWindowTextA(_T("MultiAxis BurnIn"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_ENVOLVE_AXIS)->SetWindowTextA(_T("Envolved"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_SELECT_AXIS)->SetWindowTextA(_T(""));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_UPP_LIMIT)->SetWindowTextA(_T("UppLimit"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_LOW_LIMIT)->SetWindowTextA(_T("LowLimit"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_NUM_POINTS_PER_CYCLE)->SetWindowTextA(_T("Num.Points per Cycle"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_TOTAL_NUM_CYCLE)->SetWindowTextA(_T("Total Num.Cycles"));
		GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START)->SetWindowTextA(_T("Start Rand"));
		GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_STOP)->SetWindowTextA(_T("StopBI"));
		GetDlgItem(IDC_STATIC_FIX_BURN_IN_WIRE_LENGTH)->SetWindowTextA(_T("WireLen(mm)"));
		GetDlgItem(IDC_STATIC_FIX_BURN_IN_NUM_UNITS_PER_SIDE)->SetWindowTextA(_T("UnitsPerSides"));
		GetDlgItem(IDC_GROUP_LABEL_FIX_BURN_IN)->SetWindowTextA(_T("Fix Pattern BurnIn(WB)"));
		GetDlgItem(IDC_GROUP_LABEL_RANDOM_BURN_IN)->SetWindowTextA(_T("Random BurnIn"));
		GetDlgItem(IDC_BUTTON3)->SetWindowTextA(_T("Move To Target"));
		GetDlgItem(IDC_STATIC_GROUP_2_POINTS_MOVE)->SetWindowTextA(_T("2-Points Move"));
		GetDlgItem(IDC_BUTTON_START_2_POINTS_MOVE)->SetWindowTextA(_T("Start2Pnts"));

		GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL)->SetWindowTextA(_T("Low Level"));
		GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL)->SetWindowTextA(_T("Upp Level"));
		GetDlgItem(IDC_GROUP_MOVE_TEST_CURR_STEP_TEST)->SetWindowTextA(_T("CurrentLoop Step Test"));

		GetDlgItem(IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN)->SetWindowTextA(_T("Do BurnIn Test"));
		GetDlgItem(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA)->SetWindowTextA(_T("Edit"));
		GetDlgItem(IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION)->SetWindowTextA(_T("Show WB TunePosnSet"));
		
	}
	else
	{
//		GetDlgItem(IDC_SAMPLE_PERIOD_MS_LABEL)->SetWindowTextA(_T("生成指令(ms)"));
		GetDlgItem(IDC_STATIC_GROUP_SINGLE_AXIS)->SetWindowTextA(_T("单轴")); // 單軸
		GetDlgItem(IDC_STATIC_TEST_MOVE_SINGLE_AXIS)->SetWindowTextA(_T("Axis"));
		GetDlgItem(IDC_STATIC_TEST_MOVE_SINGLE_AXIS_TARGET_POSN)->SetWindowTextA(_T("目的位置（刻度）"));
		GetDlgItem(IDC_MTN_TEST_ENABLE_MOTOR_1)->SetWindowTextA(_T("使能"));
		GetDlgItem(IDC_BUTTON3)->SetWindowTextA(_T("移到目的"));
		GetDlgItem(IDC_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN)->SetWindowTextA(_T("多轴运动测试")); // 多軸運動測試
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_ENVOLVE_AXIS)->SetWindowTextA(_T("包含"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_SELECT_AXIS)->SetWindowTextA(_T("选择"));// 選擇
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_UPP_LIMIT)->SetWindowTextA(_T("位置上限"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_LOW_LIMIT)->SetWindowTextA(_T("位置下限"));
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_NUM_POINTS_PER_CYCLE)->SetWindowTextA(_T("每周期点数")); // 每周期點數
		GetDlgItem(IDC_STATIC_RAND_BURN_IN_TOTAL_NUM_CYCLE)->SetWindowTextA(_T("总共周期数")); // 總共周期數
		GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START)->SetWindowTextA(_T("开始随机")); // 開始隨機
		GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_STOP)->SetWindowTextA(_T("停止"));
		GetDlgItem(IDC_STATIC_FIX_BURN_IN_WIRE_LENGTH)->SetWindowTextA(_T("线长(mm)"));
		GetDlgItem(IDC_STATIC_FIX_BURN_IN_NUM_UNITS_PER_SIDE)->SetWindowTextA(_T("每边线数"));
		GetDlgItem(IDC_GROUP_LABEL_FIX_BURN_IN)->SetWindowTextA(_T("定模式测试(焊线)"));
		GetDlgItem(IDC_GROUP_LABEL_RANDOM_BURN_IN)->SetWindowTextA(_T("随机测试"));
		GetDlgItem(IDC_BUTTON3)->SetWindowTextA(_T("移向定点"));
		GetDlgItem(IDC_STATIC_GROUP_2_POINTS_MOVE)->SetWindowTextA(_T("2点跑测试"));
		GetDlgItem(IDC_BUTTON_START_2_POINTS_MOVE)->SetWindowTextA(_T("开始2点跑"));
		GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL)->SetWindowTextA(_T("低电平"));
		GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL)->SetWindowTextA(_T("高电平"));
		GetDlgItem(IDC_GROUP_MOVE_TEST_CURR_STEP_TEST)->SetWindowTextA(_T("电流环阶越测试"));

		GetDlgItem(IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN)->SetWindowTextA(_T("做伺服空跑"));
		GetDlgItem(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA)->SetWindowTextA(_T("编辑"));
		GetDlgItem(IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION)->SetWindowTextA(_T("显示WB 调节位置"));
		
	}
}

void CMtnTest_Move::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CMtnTest_Move::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	cFlagInitRandBurnInDefPara = 0;
	CDialog::OnCancel();
}

// IDC_EDIT_FIX_BURN_IN_WIRE_LENGTH
void CMtnTest_Move::OnEnChangeEditFixBurnInWireLength()
{
	ReadDoubleFromEdit(IDC_EDIT_FIX_BURN_IN_WIRE_LENGTH, &dFixBurnInWireLength_mm);
}

// IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT
void CMtnTest_Move::OnEnChangeEditFixBurnInBetweenUnit()
{
	ReadDoubleFromEdit(IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT, &dFixBurnInPitchPerUnit_mm);
}

// IDC_COMBO_FIX_BURN_IN_NUM_SIDES
void CMtnTest_Move::OnCbnSelchangeComboFixBurnInNumSides()
{
	usSidePattern = ((CComboBox*) GetDlgItem(IDC_COMBO_FIX_BURN_IN_NUM_SIDES))->GetCurSel();
}

// IDC_EDIT_FIX_BURN_IN_NUM_UNITS_PER_SIDE
void CMtnTest_Move::OnEnChangeEditFixBurnInNumUnitsPerSide()
{
	ReadUShortFromEdit(IDC_EDIT_FIX_BURN_IN_NUM_UNITS_PER_SIDE, &usFixBurnInNumUnitsPerSide);
}

// IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE
void CMtnTest_Move::OnBnClickedCheckFixBurnInFlagIsClockWise()
{
	bFixBurnInFlagIsClockWise = (unsigned char)((CButton*) GetDlgItem(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE))->GetCheck();
}

// IDC_BUTTON_TEST_MOVE_FIX_BURN_IN_START
void CMtnTest_Move::OnBnClickedButtonTestMoveFixBurnInStart()
{
	// TODO: Add your control notification handler code here
	aft_fix_burn_in_calc_target_point_per_cycle_4_side();
	mtn_test_set_burn_in_flag(MTN_TEST_BURN_IN_MULTI_AXIS_FIX_PTN);  // 2010Feb05
	uiCurrBurnInCycle = 0;
	uiTotalNumCycleBurnIn = uiRandBurnInTotalNumCycle;

	RunBurnInThread();
}

// IDC_MAXVEL_AXIS1
void CMtnTest_Move::OnEnKillfocusMaxvelAxis1()
{
	//unsigned int uiAxisTemp;
	//uiAxisTemp = uiCurrMoveAxis_ACS;
	//GetDlgItem(IDC_MAXVEL_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//stSpeedProfilePerAxis[uiAxisTemp].dMaxVelocity = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_MAXVEL_AXIS1, &(stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxVelocity)); // 20130218
	mtnapi_set_speed_profile(Handle, uiCurrMoveAxis_ACS, &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], NULL);

}
// IDC_MAXACC_AXIS1
void CMtnTest_Move::OnEnKillfocusMaxaccAxis1()
{
	//unsigned int uiAxisTemp;
	//uiAxisTemp = uiCurrMoveAxis_ACS;
	//GetDlgItem(IDC_MAXACC_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//stSpeedProfilePerAxis[uiAxisTemp].dMaxAcceleration  = strtod(strTextTemp, &strStopString);
	//mtnapi_set_speed_profile(Handle, uiAxisTemp, &stSpeedProfilePerAxis[uiAxisTemp], NULL);
	ReadDoubleFromEdit(IDC_MAXACC_AXIS1, &(stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxAcceleration)); // 20130218
	mtnapi_set_speed_profile(Handle, uiCurrMoveAxis_ACS, &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], NULL);
}
// IDC_MAXDEC_AXIS1
void CMtnTest_Move::OnEnKillfocusMaxdecAxis1()
{
	//unsigned int uiAxisTemp = uiCurrMoveAxis_ACS;
	//GetDlgItem(IDC_MAXDEC_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//stSpeedProfilePerAxis[uiAxisTemp].dMaxDeceleration = strtod(strTextTemp, &strStopString);
	//mtnapi_set_speed_profile(Handle, uiAxisTemp, &stSpeedProfilePerAxis[uiAxisTemp], NULL);
	ReadDoubleFromEdit(IDC_MAXDEC_AXIS1, &(stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxDeceleration)); // 20130218
	mtnapi_set_speed_profile(Handle, uiCurrMoveAxis_ACS, &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], NULL);
}

// IDC_MAXJERK_AXIS1
void CMtnTest_Move::OnEnKillfocusMaxjerkAxis1()
{
	//unsigned int uiAxisTemp = uiCurrMoveAxis_ACS;
	//GetDlgItem(IDC_MAXJERK_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//stSpeedProfilePerAxis[uiAxisTemp].dMaxJerk = strtod(strTextTemp, &strStopString);
	//mtnapi_set_speed_profile(Handle, uiAxisTemp, &stSpeedProfilePerAxis[uiAxisTemp], NULL);
	ReadDoubleFromEdit(IDC_MAXJERK_AXIS1, &(stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxJerk));  // 20130218
	mtnapi_set_speed_profile(Handle, uiCurrMoveAxis_ACS, &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], NULL);
}
// IDC_MAX_KILL_ACC_AXIS1
void CMtnTest_Move::OnEnKillfocusMaxKillAccAxis1()
{
	//unsigned int uiAxisTemp = uiCurrMoveAxis_ACS;
	//GetDlgItem(IDC_MAX_KILL_ACC_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	//stSpeedProfilePerAxis[uiAxisTemp].dMaxKillDeceleration  = strtod(strTextTemp, &strStopString);
	//mtnapi_set_speed_profile(Handle, uiAxisTemp, &stSpeedProfilePerAxis[uiAxisTemp], NULL);
	ReadDoubleFromEdit(IDC_MAX_KILL_ACC_AXIS1, &(stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxKillDeceleration));
	mtnapi_set_speed_profile(Handle, uiCurrMoveAxis_ACS, &stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB], NULL);
}


int aiIdxMapFromAxisACS[MAX_CTRL_AXIS_PER_SERVO_BOARD] = {0, 1, MAX_AXIS_ENVOLVE_BURN_IN_TEST, MAX_AXIS_ENVOLVE_BURN_IN_TEST, 2, MAX_AXIS_ENVOLVE_BURN_IN_TEST, MAX_AXIS_ENVOLVE_BURN_IN_TEST, MAX_AXIS_ENVOLVE_BURN_IN_TEST};

void CMtnTest_Move::ProtectionCommandPointOverLimit()
{
// UI: input lower point: iPoint1stIn2PointsOneAxisMove;
// UI: input upper point: iPoint2ndIn2PointsOneAxisMove;
	// Protection
	int iLowerPointAxisMoving, iUpperPointAxisMoving;
	int iAxisACS_Tuning = uiCurrMoveAxis_ACS;
	int idxDlgUI = aiIdxMapFromAxisACS[iAxisACS_Tuning];
	if(idxDlgUI < MAX_AXIS_ENVOLVE_BURN_IN_TEST)
	{
		iLowerPointAxisMoving = aiRandBurnInAxisLowerLimit[idxDlgUI];
		iUpperPointAxisMoving = aiRandBurnInAxisUpperLimit[idxDlgUI];
	}
	if(iPoint1stIn2PointsOneAxisMove > iPoint2ndIn2PointsOneAxisMove) // by default, 1stPoint < 2ndPoint
	{
		int iTemp = iPoint1stIn2PointsOneAxisMove;
		iPoint1stIn2PointsOneAxisMove = iPoint2ndIn2PointsOneAxisMove;
		iPoint2ndIn2PointsOneAxisMove = iTemp;
	}

	if(iPoint1stIn2PointsOneAxisMove < iLowerPointAxisMoving)
	{
		iPoint1stIn2PointsOneAxisMove = iLowerPointAxisMoving;
		UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, iPoint1stIn2PointsOneAxisMove);

	}
	if(iPoint2ndIn2PointsOneAxisMove > iUpperPointAxisMoving)
	{
		iPoint2ndIn2PointsOneAxisMove = iUpperPointAxisMoving;
		UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, iPoint2ndIn2PointsOneAxisMove);
	}
}

// IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1
void CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsPoint1()
{
	ReadIntegerFromEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, &iPoint1stIn2PointsOneAxisMove);
	ProtectionCommandPointOverLimit();
}
// IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2
void CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsPoint2()
{
	ReadIntegerFromEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, &iPoint2ndIn2PointsOneAxisMove);
	ProtectionCommandPointOverLimit();
}
// IDC_EDIT_MOVE_TEST_2_POINTS_TOTAL_ITER
void CMtnTest_Move::OnEnKillfocusEditMoveTest2PointsTotalIter()
{
	ReadUShortFromEdit(IDC_EDIT_MOVE_TEST_2_POINTS_TOTAL_ITER, &usTotalCycle2PointsOneAxisMove);
}
// IDC_BUTTON_START_2_POINTS_MOVE
void CMtnTest_Move::OnBnClickedButtonStart2PointsMove()
{
	uiCurrBurnInCycle = 0;
	uiTotalNumCycleBurnIn = usTotalCycle2PointsOneAxisMove;
	mtn_test_set_burn_in_flag( MTN_TEST_BURN_IN_ONE_AXIS_2_POINTS);  // 2010Feb05
	RunBurnInThread();
}

void CMtnTest_Move::OnEnKillfocusDwellAxis1()
{
	unsigned int uiAxisTemp = uiCurrMoveAxis_ACS;
	GetDlgItem(IDC_DWELL_AXIS1)->GetWindowTextA( &strTextTemp[0], LEN_TEXT);
	sscanf_s(strTextTemp, "%d", &uiMotionInterDelay_ms[uiAxisTemp]);
}
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_SETTLING_TIME
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_OVER_UNDER_SHOOT
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_MAX_DPE
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_RMS_DAC
// IDC_STATIC_MOVE_TEST_TUNE_SPEC_ERROR_REPEATIBLE

// IDC_CHECK_MOVE_TEST_SAVE_SCOPE_DATA_TO_FILE
void CMtnTest_Move::OnBnClickedCheckMoveTestSaveScopeDataToFile()
{
	cFlagSaveScopeDataInFile = ((CButton *)GetDlgItem(IDC_CHECK_MOVE_TEST_SAVE_SCOPE_DATA_TO_FILE))->GetCheck();
}

// IDC_CHECK_MOVE_TEST_TUNING
void CMtnTest_Move::OnBnClickedCheckMoveTestTuning()
{
	cFlagTuningCurrentAxis = ((CButton *)GetDlgItem(IDC_CHECK_MOVE_TEST_TUNING))->GetCheck();
}

// IDC_EDIT_MOVE_TEST_SERVO_TUNE_POSNERR_SETTLE_TH
void CMtnTest_Move::OnEnKillfocusEditMoveTestServoTunePosnerrSettleTh()
{
	ReadDoubleFromEdit(IDC_EDIT_MOVE_TEST_SERVO_TUNE_POSNERR_SETTLE_TH, &(fThresholdSettleTime));
}

// IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL
// IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL
// IDC_GROUP_MOVE_TEST_CURR_STEP_TEST
// IDC_EDIT_MOVE_TEST_CURR_STEP_LOW_LEVEL
// IDC_EDIT_MOVE_TEST_CURR_STEP_UPP_LEVEL
// IDC_EDIT_MOVE_TEST_CURR_STEP_TOTAL_ITER
#define MAX_DRIVER_COMMAND_BY_CTRL			(32767.0)
extern AFT_GROUP_VEL_STEP_TEST_CONFIG stVelStepGroupTestConfig;
void CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepUppLevel()
{
	CString cstrTemp;
	char strText[32];
	ReadIntegerFromEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_UPP_LEVEL, &(stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel));
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL)->GetWindowTextA(strText, 10);
	cstrTemp.Format("%s: %4.1f%%, %3.1fV", strText, 
		((float)stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel)/MAX_DRIVER_COMMAND_BY_CTRL * 100.0,
		stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel  * 10.0/MAX_DRIVER_COMMAND_BY_CTRL);
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL)->SetWindowTextA(cstrTemp);
	// IDC_STATIC_MOVE_CURR_STEP_TEST_HIGH_LEVEL_I

	cstrTemp.Format("Drv: %3.1fA", 
		stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel/MAX_DRIVER_COMMAND_BY_CTRL * stVelStepGroupTestConfig.afPeakDriverCurrent[uiCurrMoveAxis_AppWB]);
	GetDlgItem(IDC_STATIC_MOVE_CURR_STEP_TEST_HIGH_LEVEL_I)->SetWindowTextA(cstrTemp);

}

void CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepTotalIter()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_TOTAL_ITER, &(stMoveTestCurrLoopStep.uiTotalIter));
}

void CMtnTest_Move::OnEnKillfocusEditMoveTestCurrStepLowLevel()
{
	CString cstrTemp;
	char strText[32];
	ReadIntegerFromEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_LOW_LEVEL, &(stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel));
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL)->GetWindowTextA(strText, 10);
	cstrTemp.Format("%s: %4.1f%%, %3.1fV", strText, 
		((float)stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel)/MAX_DRIVER_COMMAND_BY_CTRL * 100.0,
		stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel * 10.0/MAX_DRIVER_COMMAND_BY_CTRL);
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL)->SetWindowTextA(cstrTemp);
	// IDC_STATIC_MOVE_CURR_STEP_TEST_LOW_LEVEL_I
	cstrTemp.Format("Drv: %3.1fA", 
		stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel/MAX_DRIVER_COMMAND_BY_CTRL * stVelStepGroupTestConfig.afPeakDriverCurrent[uiCurrMoveAxis_AppWB]);
	GetDlgItem(IDC_STATIC_MOVE_CURR_STEP_TEST_LOW_LEVEL_I)->SetWindowTextA(cstrTemp);
}


// IDC_BUTTON_MOVE_TEST_CURR_STEP_START
void CMtnTest_Move::OnBnClickedButtonMoveTestCurrStepStart()
{
	uiCurrBurnInCycle = 0;
	uiTotalNumCycleBurnIn = stMoveTestCurrLoopStep.uiTotalIter;
	mtn_test_set_burn_in_flag(MTN_TEST_CURR_LOOP_STEP_TEST);  // 2010Feb05
	for(int ii = 0; ii < 8; ii++)
	{
		stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand[ii] = 0;
	}
	RunBurnInThread();
}

void CMtnTest_Move::CurrentLoopStepMoveTestOneCycle()
{
	unsigned int uiAxisTemp = uiCurrMoveAxis_ACS;
	static int iTempMotorState;
	short sRet = 0;

	Sleep(uiMotionInterDelay_ms[uiAxisTemp]); 	//Sleep(uiMotionInterDelay_ms[uiAxisTemp]);

	// Set Current Command to Upp Level, DCOM
	stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand[uiAxisTemp] = stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel * 100/32767;
	// acsc_WriteInteger(HANDLE Handle, int NBuf, char* Var, int From1, int To1, int From2, int To2, int* Values, ACSC_WAITBLOCK* Wait);
	acsc_WriteInteger(Handle, 0, "DCOM", 0, 7, 0, 0, stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand, 0); // DOUT

	Sleep(uiMotionInterDelay_ms[uiAxisTemp]); 	//Sleep(uiMotionInterDelay_ms[uiAxisTemp]);

	// Set Current Command to Low Level, DCOM
	stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand[uiAxisTemp] = stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel * 100/32767;
	acsc_WriteInteger(Handle, 0, "DCOM", 0, 7, 0, 0, stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand, 0); // DOUT

}

void CMtnTest_Move::ClearCurrentCommandAllAxis()
{
	for(int ii = 0; ii < 8; ii++)
	{
		stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand[ii] = 0;
	}
	acsc_WriteInteger(Handle, 0, "DCOM", 0, 7, 0, 0, stMoveTestCurrLoopStep.aiControllerAxisCurrentCommand, 0); // DOUT
}

#include "MtnTune.h"
// IDC_COMBO_SEL_TUNE_WB_BOND_HEAD_MOTION_IDX
void CMtnTest_Move::OnCbnSelchangeComboSelTuneWbBondHeadMotionIdx()
{
	//if(uiCurrMoveAxis_AppWB == WB_AXIS_BOND_Z)
	//{
	//	CComboBox *pTuneWbBondHeadMotionIdxCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SEL_TUNE_WB_BOND_HEAD_MOTION_IDX);
	//	int uiBlkMovePosnSet = pTuneWbBondHeadMotionIdxCombo->GetCurSel();
	//	double dPosn1, dPosn2;
	//	mtn_tune_get_bond_head_tuning_position(uiCurrWireNo, uiBlkMovePosnSet,  &dPosn1, &dPosn2);
	//	if(dPosn1 < dPosn2)
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn1;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn2;
	//	}
	//	else
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn2;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn1;
	//	}
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, iPoint1stIn2PointsOneAxisMove);
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, iPoint2ndIn2PointsOneAxisMove);

	//}
}

void CMtnTest_Move::InitComboTuningWB_TableBondHeadMotionIdx()
{
	CComboBox *pTuneWbBondHeadMotionIdxCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SEL_TUNE_WB_BOND_HEAD_MOTION_IDX);

	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_1ST_BOND_SEARCH_HEIGHT, _T("1ST_B_SEARCH_HEIGHT"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_REVERSE_HEIGHT, _T("REVERSE_HEIGHT"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_KINK_HEIGHT, _T("KINK_HEIGHT"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_LOOP_TOP, _T("LOOP_TOP"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_TRAJ_2ND_BOND_SEARCH_H, _T("2ND_B_SEARCH_H"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_TAIL, _T("TAIL"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(IDX_BH_MOTION_RESET_FIRE_LEVEL, _T("RESET_FIRE_LEVEL"));

	pTuneWbBondHeadMotionIdxCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SEL_TUNE_WB_TABLE_MOTION_IDX);

	pTuneWbBondHeadMotionIdxCombo->InsertString(WB_TBL_MOTION_IDX_MOVE_BTO, _T("1ST_B_MOVE_BTO"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(WB_TBL_MOTION_IDX_MOVE_REVERSE_DIST, _T("KINK"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(WB_TBL_MOTION_IDX_MOVE_TRAJ, _T("TRAJ_2ND_B"));
	pTuneWbBondHeadMotionIdxCombo->InsertString(WB_TBL_MOTION_IDX_MOVE_NEXT_PR, _T("NEXT_PR"));

}

// IDC_COMBO_SEL_TUNE_WB_TABLE_MOTION_IDX
void CMtnTest_Move::OnCbnSelchangeComboSelTuneWbTableMotionIdx()
{
	//CComboBox *pTuneWbTableMotionIdxCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SEL_TUNE_WB_TABLE_MOTION_IDX);
	//if(uiCurrMoveAxis_AppWB == WB_AXIS_TABLE_X)
	//{
	//	int uiBlkMovePosnSet = pTuneWbTableMotionIdxCombo->GetCurSel();
	//	double dPosn1, dPosn2;
	//	mtn_tune_get_table_x_tuning_position(uiCurrWireNo, uiBlkMovePosnSet,  &dPosn1, &dPosn2);
	//	if(dPosn1 < dPosn2)
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn1;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn2;
	//	}
	//	else
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn2;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn1;
	//	}
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, iPoint1stIn2PointsOneAxisMove);
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, iPoint2ndIn2PointsOneAxisMove);

	//}
	//else if(uiCurrMoveAxis_AppWB == WB_AXIS_TABLE_Y)
	//{
	//	int uiBlkMovePosnSet = pTuneWbTableMotionIdxCombo->GetCurSel();
	//	double dPosn1, dPosn2;
	//	mtn_tune_get_table_y_tuning_position(uiCurrWireNo, uiBlkMovePosnSet,  &dPosn1, &dPosn2);
	//	if(dPosn1 < dPosn2)
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn1;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn2;
	//	}
	//	else
	//	{
	//		iPoint1stIn2PointsOneAxisMove = (int)dPosn2;
	//		iPoint2ndIn2PointsOneAxisMove = (int)dPosn1;
	//	}
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_1, iPoint1stIn2PointsOneAxisMove);
	//	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_2_POINTS_POINT_2, iPoint2ndIn2PointsOneAxisMove);

	//}
}

// global REAL arMotorXProfile(4)(3); [Blk0:Blk3] [MaxVel, MaxAcc, MaxJerk]
// global REAL arMotorAProfile(5)(3); [Blk0:Blk4] [MaxVel, MaxAcc, MaxJerk]
// global REAL arACtrl(9)(4); [Blk0:Blk8] [KPV, KIV, KPP, SLAFF]
// Load Speed Profile, Table: arMotorXProfile; BondHead: arMotorAProfile
// Ctrl: arACtrl


// IDC_EDIT_WIRE_ID_MOVE_TEST_GET_TUNE_POSN_SET
void CMtnTest_Move::OnEnKillfocusEditWireIdMoveTestGetTunePosnSet()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_WIRE_ID_MOVE_TEST_GET_TUNE_POSN_SET, &uiCurrWireNo);
	if(uiCurrWireNo > mtn_wb_tune_get_num_total_wire())
	{
		uiCurrWireNo = mtn_wb_tune_get_num_total_wire();
	}	 
}
// IDC_COMBO_TUNING_THEME_OPTION
void CMtnTest_Move::OnCbnSelchangeComboTuningThemeOption()
{
	CComboBox *pTuneThemeOptionCombo = (CComboBox*) GetDlgItem(IDC_COMBO_TUNING_THEME_OPTION);
	int iTuneThemeOpt = pTuneThemeOptionCombo->GetCurSel();
	mtn_tune_set_tune_theme_option(iTuneThemeOpt);
}

//IDC_COMBO_TUNING_THEME_OPTION
void CMtnTest_Move::InitTuneThemeOption()
{
	CComboBox *pTuneThemeOptionCombo = (CComboBox*) GetDlgItem(IDC_COMBO_TUNING_THEME_OPTION);
	pTuneThemeOptionCombo->InsertString(TUNE_THEME_OPT_MIN_DPE_DIFF_DPE, "MIN_DPE");
	pTuneThemeOptionCombo->InsertString(TUNE_THEME_OPT_MIN_CEOUS, "MIN_CEOUS");
	pTuneThemeOptionCombo->SetCurSel(mtn_tune_get_tune_theme_option());
}

// IDC_CHECK_ACS_ENABLE_BIQUAD_MOVING_DLG, 20121106
void CMtnTest_Move::OnBnClickedCheckAcsEnableBiquadMovingDlg()
{
	int iControlCardMotorFlag;
	acsc_ReadInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagEnc;
	iTempFlagEnc = ((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENABLE_BIQUAD_MOVING_DLG))->GetCheck();

	unsigned int uiTempFlagBitAnd, uiTempBitFullOne32Bit;
	uiTempFlagBitAnd = (unsigned int)pow(2.0, (double)_BIT_SET_ENABLE_BIQUAD_);  

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
	acsc_WriteInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

}
