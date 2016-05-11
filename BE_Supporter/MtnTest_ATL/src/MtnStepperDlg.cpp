// MtnStepperDlg.cpp : implementation file
//
// History 
// YYYYMMDD Author		Notes
// 20090106 Zhengyi		Add external drive mode

#include "stdafx.h"
#include "math.h"
#include "MtnStepperDlg.h"
#include "ADS1240.h"
#include "MotionStepper.h"

// CMtnStepperDlg dialog
static unsigned int uiErrorLine = 0;

static BYTE uiCurrBoardId;
static long alMoveDistAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
static long aiTargetPosnAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
static DWORD aiFeedbackPosnAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
static USHORT ausDI_PosiLimitAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
static USHORT ausDI_NegLimitAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
static USHORT ausDI_HomeStatusAtAxis[MAX_STEPPER_AXIX_ON_BOARD];

static BOOL bStepperSelX[MAX_STEPPER_CTRL_BOARD], bStepperSelY[MAX_STEPPER_CTRL_BOARD], bStepperSelZ[MAX_STEPPER_CTRL_BOARD], bStepperSelU[MAX_STEPPER_CTRL_BOARD];
static BOOL bFirstInitFlag;
static char strCharAxisX[2] = "X";
static char strCharAxisY[2] = "Y";
static char strCharAxisZ[2] = "Z";
static char strCharAxisU[2] = "U";
static char strTextButton[128];
static CString cstrButtonText;
static char strTextTemp[128];
static BOOL abStepperCtrlBoardOpen[MAX_STEPPER_CTRL_BOARD];
static unsigned char ucStepperExtDriveMode; // 20090106
static unsigned long ulStepperExtDriveNumPulse;

static unsigned int uiMoveTotalIteration, uiCurrentIteration;
static unsigned int uiIterativeMoveDelay_ms;
static int iFlagIterativeMove;

char *pstrStepperExtDriveMode[] = {"JOGDisable",
		"JOGSelfAxis",
		"JOGSelect_XAxis",
		"JOGSelect_YAxis",
		"JOGConnect_XAxis",
		"JOGConnect_YAxis",
		"JOGConnect_ZAxis",
		"JOGConnect_UAxis",
		"HandWheelDisable",
		"HandWheelSelfAxis",
		"HandWheelSelect_XAxis",
		"HandWheelSelect_YAxis",
		"HandWheelFrom_XAxis",
		"HandWheelFrom_YAxis",
		"HandWheelFrom_ZAxis",
		"HandWheelFrom_UAxis"};

char *pstrStepperReturnError_1240[] = { 
		"Success",                // 0
		"BoardNumErr",            // 0x0001	
		"CreateKernelDriverFail", // 0x0002	//internal system error
		"CallKernelDriverFail  ", // 0x0003  //internal system error
		"RegistryOpenFail      ", // 0x0004	//Open registry file fail
		"RegistryReadFail      ", // 0x0005	//Read registry file fail
		"AxisNumErr            ", // 0x0006
		"UnderRGErr            ", // 0x0007
		"OverRGErr             ", // 0x0008
		"UnderSVErr            ", // 0x0009
		"OverSVErr             ", // 0x000a
		"OverMDVErr            ", // 0x000b
		"UnderDVErr            ", // 0x000c
		"OverDVErr             ", // 0x000d
		"UnderACErr            ", // 0x000e
		"OverACErr             ", // 0x000f
		"UnderAKErr            ", // 0x0010
		"OverAKErr             ", // 0x0011
		"OverPLmtErr           ", // 0x0012
		"OverNLmtErr           ", // 0x0013
		"MaxMoveDistErr        ", // 0x0014
		"AxisDrvBusy           ", // 0x0015
		"RegUnDefine           ", // 0x0016
		"ParaValueErr          ", // 0x0017
		"ParaValueOverRange    ", // 0x0018
		"ParaValueUnderRange   ", // 0x0019
		"AxisHomeBusy          ", // 0x001a
		"AxisExtBusy           ", // 0x001b
		"RegistryWriteFail     ", // 0x001c
		"ParaValueOverErr      ", // 0x001d
		"ParaValueUnderErr     ", // 0x001e
		"OverDCErr             ", // 0x001f
		"UnderDCErr            ", // 0x0020
		"UnderMDVErr           ", // 0x0021
		"RegistryCreateFail    ", // 0x0022
		"CreateThreadErr       ", // 0x0023		//internal system fail
		"HomeSwStop",             // 0x0024		//P1240HomeStatus
		"ChangeSpeedErr        ", // 0x0025
		"DOPortAsDriverStatus  ", // 0x0026
		"Not Defined Error",      // 0x0027
		"Not Defined Error",      // 0x0028
		"Not Defined Error",      // 0x0029
		"Not Defined Error",      // 0x002a
		"Not Defined Error",      // 0x002b
		"Not Defined Error",      // 0x002c
		"Not Defined Error",      // 0x002d
		"Not Defined Error",      // 0x002e
		"Not Defined Error",      // 0x002f
		"OpenEventFail         ", // 0x0030		//Internal system fail
		"Not Defined Error",      // 0x0031
		"DeviceCloseErr	",	  // 0x0032		//Internal system fail
		"Not Defined Error",      // 0x0033
		"Not Defined Error",      // 0x0034
		"Not Defined Error",      // 0x0035
		"Not Defined Error",      // 0x0036
		"Not Defined Error",      // 0x0037
		"Not Defined Error",      // 0x0038
		"Not Defined Error",      // 0x0039
		"Not Defined Error",      // 0x003a
		"Not Defined Error",      // 0x003b
		"Not Defined Error",      // 0x003c
		"Not Defined Error",      // 0x003d
		"Not Defined Error",      // 0x003e
		"Not Defined Error",      // 0x003f
		"HomeEMGStop           ", // 0x0040		//P1240HomeStatus
		"HomeLMTPStop          ", // 0x0041		//P1240HomeStatus
		"HomeLMTNStop          ", // 0x0042		//P1240HomeStatus
		"HomeALARMStop         ", // 0x0043		//P1240HomeStatus
		"Not Defined Error",      // 0x0044
		"Not Defined Error",      // 0x0045
		"Not Defined Error",      // 0x0046
		"Not Defined Error",      // 0x0047
		"Not Defined Error",      // 0x0048
		"Not Defined Error",      // 0x0049
		"Not Defined Error",      // 0x004a
		"Not Defined Error",      // 0x004b
		"Not Defined Error",      // 0x004c
		"Not Defined Error",      // 0x004d
		"Not Defined Error",      // 0x004e
		"Not Defined Error",      // 0x004f
		"AllocateBufferFail"    , // 0x0050
		"BufferReAllocate"      , // 0x0051
		"FreeBufferFail	"       , // 0x0052
		"FirstPointNumberFail"  , // 0x0053	
		"PointNumExceedAllocatedSize" , //		0x0054
		"BufferNoneAllocate", //				0x0055
		"SequenceNumberErr", //0x0056	
		"PathTypeErr", //0x0057
		"Not Defined Error",      // 0x0058
		"Not Defined Error",      // 0x0059
		"Not Defined Error",      // 0x005a
		"Not Defined Error",      // 0x005b
		"Not Defined Error",      // 0x005c
		"Not Defined Error",      // 0x005d
		"Not Defined Error",      // 0x005e
		"Not Defined Error",      // 0x005f
		"PathTypeMixErr", // 0x0060
		"BufferDataNotEnough"}; //				0x0061

IMPLEMENT_DYNAMIC(CMtnStepperDlg, CDialog)

CMtnStepperDlg::CMtnStepperDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMtnStepperDlg::IDD, pParent)
{
	if(bFirstInitFlag == 0)
	{
		bFirstInitFlag = 1;
		for(int ii = 0; ii<MAX_STEPPER_CTRL_BOARD; ii++)
		{
			bStepperSelX[ii] = 0;
			bStepperSelY[ii] = 0;
			bStepperSelZ[ii] = 0;
			bStepperSelU[ii] = 0;
			if(ii == 0)
			{
//				bStepperSelX[ii] = 1;
				bStepperSelY[ii] = 1;
				bStepperSelZ[ii] = 1;
			}
			
		}
		uiCurrBoardId = 0;
		ucStepperExtDriveMode = 0;
		ulStepperExtDriveNumPulse = 3200/4;
	}
}

char cFlagIsUsingLeeTro2812 = 0;
char cFlagIsUsingAd1240 = 0;

#include "Mpc2810.h"
long glTotalNumStepperCtrlCards;
long glTotalAxes;
void mtn_stepper_init_2812()
{
//	int Rtn;
	glTotalAxes = auto_set(); //自动设置
	if(glTotalAxes <= 0 )
	{//	返回错误代码
	}

	glTotalNumStepperCtrlCards = init_board(); //初始化板卡
	if(glTotalNumStepperCtrlCards <= 0 )
	{//	返回错误代码
	}
	else
	{
		cFlagIsUsingLeeTro2812 = 1;
	}
	int ii;
	for(ii=0;ii<glTotalAxes;ii++)
	{
		set_sd_logic(ii+1,0);
		set_el_logic(ii+1,0);
		set_org_logic(ii+1,0);
		set_alm_logic(ii+1,0);
		set_outmode(ii+1, 1 , 0);
		set_home_mode(ii+1, 0);
		set_dir(ii+1, 0);
	}

#ifdef __PORT_2812__
		set_maxspeed(ChX,atoi(m_sXHighSpeed));
		set_maxspeed(ChY,atoi(m_sYHighSpeed));

		set_conspeed(ChX,atoi(m_sXLowSpeed));
		set_conspeed(ChY,atoi(m_sYLowSpeed));

		set_profile(ChX,atoi(m_sXLowSpeed),atoi(m_sXHighSpeed),atoi(m_sXAccel));
		set_profile(ChY,atoi(m_sYLowSpeed),atoi(m_sYHighSpeed),atoi(m_sYAccel));

int WINAPI get_max_axe();
int WINAPI get_board_num();
int WINAPI get_axe(int cardno);
int WINAPI get_unit(int ch,double* dl);
int WINAPI check_IC(int cardno);
int WINAPI get_abs_pos(int ch,double *pos);
int WINAPI get_rel_pos(int ch,double *pos);
int WINAPI get_encoder(int ch,long *count);
int WINAPI get_done_source(int ch,long *src);
#endif // __PORT_2812__

}

#include "MtnTesterResDef.h"
static UINT_PTR iTimerIdStepper1240Dlg;

#define DEF_MOVING_PULSE_STEPPER 200
void CMtnStepperDlg::InitLocalVariable()
{
	int ii;
	for(ii = 0; ii< MAX_STEPPER_AXIX_ON_BOARD; ii++)
	{
		alMoveDistAtAxis[ii] = DEF_MOVING_PULSE_STEPPER;
	}
}

void CMtnStepperDlg::InitDetectStepperCtrlBord()
{
	CString cstrTemp;
	// Open 1240
	int ii;
	for(ii = 0; ii<MAX_STEPPER_CTRL_BOARD; ii++)
	{
		if((ULONG)P1240MotDevOpen(ii) == 0)
		{
			cFlagIsUsingAd1240 = 1;
			P1240MotDevClose(ii);
			glTotalNumStepperCtrlCards ++;
			uiCurrBoardId = ii;
		}
	}
	if(cFlagIsUsingAd1240 == 0) // Assumption, if there is any 1240 cards, no 2812 cards
	{
		mtn_stepper_init_2812();
	}
	else
	{
		(ULONG)P1240MotDevOpen(uiCurrBoardId); // 20120902
	}

	if(cFlagIsUsingAd1240 || cFlagIsUsingLeeTro2812)
	{
		abStepperCtrlBoardOpen[uiCurrBoardId] = 1 - abStepperCtrlBoardOpen[uiCurrBoardId];
		UploadSpeedProfileCurrBoard();
		UpdateButtonText();
		UpdateSpeedProfileText();
		UpdatePositionRegistrationText();
		if(cFlagIsUsingAd1240)
		{
			cstrTemp.Format("Ads1240");
		}
		else
		{
			cstrTemp.Format("Ltr2812");
		}
	}
	else
	{
		cstrTemp.Format("No Cards");
		
	}
	GetDlgItem(IDC_STATIC_STEPPER_CTRL_BD_TYPE)->SetWindowTextA(cstrTemp);
}
BOOL CMtnStepperDlg::OnInitDialog()
{

	BOOL bRet = CDialog::OnInitDialog();
	///////// Detection of PCI-Stepper Cards
	InitDetectStepperCtrlBord();  // 20120830

	// Edit Text
	InitLocalVariable();
	UpdateEditText();

	for(int ii = 0; ii<glTotalNumStepperCtrlCards; ii++)
	{
		cstrButtonText.Format("%d", ii);
		StepperBoardIdCombo.InsertString(ii, cstrButtonText);
		abStepperCtrlBoardOpen[ii] = 0;
	}
	StepperBoardIdCombo.SetCurSel(uiCurrBoardId);
	GetDlgItem(IDC_STATIC_STEPPER_EXT_MODE_SET_STATUS)->SetWindowTextA(_T("Unkown"));
	
	sprintf_s(strTextTemp, 128, "%d",ucStepperExtDriveMode);
	GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_MODE)->SetWindowTextA(_T(strTextTemp));
	sprintf_s(strTextTemp, 128, "%d",ulStepperExtDriveNumPulse);
	GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_NUM_PULSE)->SetWindowTextA(_T(strTextTemp));

	UpdateButtonText();
	UpdateWbAppName_CurrentStpCtrlBoard();

	UpdateUI_FromDigitalInput();
	((CButton *)GetDlgItem(IDC_CHECK_STEPPER1240_FLAG_ITERATIVE_MOVE))->SetCheck(iFlagIterativeMove);
	UpdateUnsignedIntToEdit(IDC_EDIT_STEPPER_1240_ITERATIVE_MOVE_DELAY, uiIterativeMoveDelay_ms);
	UpdateUnsignedIntToEdit(IDC_EDIT_STEPPER1240_MOVE_ITERATION, uiMoveTotalIteration);

//	StartTimer(50); // Duration is 100 ms
	// Only available on PCI-1240 card
	if(cFlagIsUsingAd1240 == 0)
	{
		GetDlgItem(IDC_CHECK_PCI1240_OUT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT7)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STEPPER_SET_EXT_MODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_STEPPER1240_FLAG_ITERATIVE_MOVE)->EnableWindow(FALSE);
//		GetDlgItem(IDC_BUTTON_STEPPER_HOME_AXIS)->EnableWindow(FALSE);
	}

	// TBA
	GetDlgItem(IDC_BUTTON_STEPPER_TO_TARGET_POSN)->EnableWindow(FALSE);
	// Init Thread
	m_pWinThreadDlgStepper1240 = NULL;
	m_fStopThreadDlgStepper1240 = TRUE;
	return bRet;
}


//UINT CMtnStepperDlg::StartTimer(UINT TimerDuration)
//{
//	iTimerIdStepper1240Dlg = SetTimer(IDT_STEPPER_CTRL_DLG_TIMER, TimerDuration, 0);
//	
//	if (iTimerIdStepper1240Dlg == 0)
//	{
//		AfxMessageBox("Unable to obtain timer");
//	}
//
//    return (UINT_PTR)iTimerIdStepper1240Dlg;
//}// end StartTimer
//
//BOOL CMtnStepperDlg::StopTimer()
//{
//	if (!KillTimer (iTimerIdStepper1240Dlg))
//	{
//		return FALSE;
//	}
//	return TRUE;
//}

CMtnStepperDlg::~CMtnStepperDlg()
{
}

void CMtnStepperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STEPPER_BOARD_ID, StepperBoardIdCombo);
}


BEGIN_MESSAGE_MAP(CMtnStepperDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_STEPPER_SEL_AXIS_X, &CMtnStepperDlg::OnBnClickedCheckStepperSelAxisX)
	ON_BN_CLICKED(IDC_CHECK_STEPPER_SEL_AXIS_Y, &CMtnStepperDlg::OnBnClickedCheckStepperSelAxisY)
	ON_BN_CLICKED(IDC_CHECK_STEPPER_SEL_AXIS_Z, &CMtnStepperDlg::OnBnClickedCheckStepperSelAxisZ)
	ON_BN_CLICKED(IDC_CHECK_STEPPER_SEL_AXIS_U, &CMtnStepperDlg::OnBnClickedCheckStepperSelAxisU)
	ON_CBN_SELCHANGE(IDC_COMBO_STEPPER_BOARD_ID, &CMtnStepperDlg::OnCbnSelchangeComboStepperBoardId)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_OPEN_CLOSE_BOARD, &CMtnStepperDlg::OnBnClickedButtonStepperOpenCloseBoard)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_MOVE_DIST, &CMtnStepperDlg::OnBnClickedButtonStepperMoveDist)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MOVE_DIST_X, &CMtnStepperDlg::OnEnChangeEditStepperMoveDistX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MOVE_DIST_Y, &CMtnStepperDlg::OnEnChangeEditStepperMoveDistY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MOVE_DIST_Z, &CMtnStepperDlg::OnEnChangeEditStepperMoveDistZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MOVE_DIST_U, &CMtnStepperDlg::OnEnChangeEditStepperMoveDistU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_TARGET_POSN_X, &CMtnStepperDlg::OnEnChangeEditStepperTargetPosnX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_TARGET_POSN_Y, &CMtnStepperDlg::OnEnChangeEditStepperTargetPosnY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_TARGET_POSN_Z, &CMtnStepperDlg::OnEnChangeEditStepperTargetPosnZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_TARGET_POSN_U, &CMtnStepperDlg::OnEnChangeEditStepperTargetPosnU)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_HOME_AXIS, &CMtnStepperDlg::OnBnClickedButtonStepperHomeAxis)
	ON_BN_CLICKED(IDC_BUTTON3, &CMtnStepperDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDCANCEL, &CMtnStepperDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_SV_X, &CMtnStepperDlg::OnEnChangeEditStepperSvX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_SV_Y, &CMtnStepperDlg::OnEnChangeEditStepperSvY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_SV_Z, &CMtnStepperDlg::OnEnChangeEditStepperSvZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_SV_U, &CMtnStepperDlg::OnEnChangeEditStepperSvU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_DV_X, &CMtnStepperDlg::OnEnChangeEditStepperDvX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_DV_Y, &CMtnStepperDlg::OnEnChangeEditStepperDvY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_DV_Z, &CMtnStepperDlg::OnEnChangeEditStepperDvZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_DV_U, &CMtnStepperDlg::OnEnChangeEditStepperDvU)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_SAVE_PROFILE, &CMtnStepperDlg::OnBnClickedButtonStepperSaveProfile)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MDV_X, &CMtnStepperDlg::OnEnChangeEditStepperMdvX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MDV_Y, &CMtnStepperDlg::OnEnChangeEditStepperMdvY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MDV_Z, &CMtnStepperDlg::OnEnChangeEditStepperMdvZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_MDV_U, &CMtnStepperDlg::OnEnChangeEditStepperMdvU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_AC_X, &CMtnStepperDlg::OnEnChangeEditStepperAcX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_AC_Y, &CMtnStepperDlg::OnEnChangeEditStepperAcY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_AC_Z, &CMtnStepperDlg::OnEnChangeEditStepperAcZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_AC_U, &CMtnStepperDlg::OnEnChangeEditStepperAcU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_JERK_X, &CMtnStepperDlg::OnEnChangeEditStepperJerkX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_JERK_Y, &CMtnStepperDlg::OnEnChangeEditStepperJerkY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_JERK_Z, &CMtnStepperDlg::OnEnChangeEditStepperJerkZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_JERK_U, &CMtnStepperDlg::OnEnChangeEditStepperJerkU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_POS_LIMIT_X, &CMtnStepperDlg::OnEnChangeEditStepperPosLimitX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_POS_LIMIT_Y, &CMtnStepperDlg::OnEnChangeEditStepperPosLimitY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_POS_LIMIT_Z, &CMtnStepperDlg::OnEnChangeEditStepperPosLimitZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_POS_LIMIT_U, &CMtnStepperDlg::OnEnChangeEditStepperPosLimitU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_EXTDRIVE_MODE, &CMtnStepperDlg::OnEnChangeEditStepperExtdriveMode)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_EXTDRIVE_NUM_PULSE, &CMtnStepperDlg::OnEnChangeEditStepperExtdriveNumPulse)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_SET_EXT_MODE, &CMtnStepperDlg::OnBnClickedButtonStepperSetExtMode)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_TYPE_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeTypeX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_TYPE_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeTypeY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_TYPE_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeTypeZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_TYPE_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeTypeU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_VEL_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_VEL_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_VEL_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_VEL_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_DIR_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_DIR_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_DIR_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P0_DIR_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_VEL_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_VEL_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_VEL_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_VEL_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_DIR_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_DIR_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_DIR_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_P1_DIR_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_OFFSET_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_OFFSET_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_OFFSET_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_OFFSET_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetU)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_MODE_X, &CMtnStepperDlg::OnEnChangeEditStepperHomeModeX)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_MODE_Y, &CMtnStepperDlg::OnEnChangeEditStepperHomeModeY)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_MODE_Z, &CMtnStepperDlg::OnEnChangeEditStepperHomeModeZ)
	ON_EN_CHANGE(IDC_EDIT_STEPPER_HOME_MODE_U, &CMtnStepperDlg::OnEnChangeEditStepperHomeModeU)
	ON_BN_CLICKED(IDC_CHECK_STEPPER1240_FLAG_ITERATIVE_MOVE, &CMtnStepperDlg::OnBnClickedCheckStepper1240FlagIterativeMove)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER1240_MOVE_ITERATION, &CMtnStepperDlg::OnEnKillfocusEditStepper1240MoveIteration)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_1240_ITERATIVE_MOVE_DELAY, &CMtnStepperDlg::OnEnKillfocusEditStepper1240IterativeMoveDelay)
	ON_BN_CLICKED(IDC_CHECK_PCI1240_OUT4, &CMtnStepperDlg::OnBnClickedCheckPci1240Out4)
	ON_BN_CLICKED(IDC_CHECK_PCI1240_OUT5, &CMtnStepperDlg::OnBnClickedCheckPci1240Out5)
	ON_BN_CLICKED(IDC_CHECK_PCI1240_OUT6, &CMtnStepperDlg::OnBnClickedCheckPci1240Out6)
	ON_BN_CLICKED(IDC_CHECK_PCI1240_OUT7, &CMtnStepperDlg::OnBnClickedCheckPci1240Out7)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_REVERSE_MOVE_DIST, &CMtnStepperDlg::OnBnClickedButtonStepperReverseMoveDist)
	ON_BN_CLICKED(IDC_BUTTON_STEPPER_TO_TARGET_POSN, &CMtnStepperDlg::OnBnClickedButtonStepperToTargetPosn)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_SV_X, &CMtnStepperDlg::OnEnKillfocusEditStepperSvX)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_SV_Y, &CMtnStepperDlg::OnEnKillfocusEditStepperSvY)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_SV_Z, &CMtnStepperDlg::OnEnKillfocusEditStepperSvZ)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_SV_U, &CMtnStepperDlg::OnEnKillfocusEditStepperSvU)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_DV_X, &CMtnStepperDlg::OnEnKillfocusEditStepperDvX)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_DV_Y, &CMtnStepperDlg::OnEnKillfocusEditStepperDvY)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_DV_Z, &CMtnStepperDlg::OnEnKillfocusEditStepperDvZ)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_DV_U, &CMtnStepperDlg::OnEnKillfocusEditStepperDvU)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_MDV_X, &CMtnStepperDlg::OnEnKillfocusEditStepperMdvX)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_MDV_Y, &CMtnStepperDlg::OnEnKillfocusEditStepperMdvY)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_MDV_Z, &CMtnStepperDlg::OnEnKillfocusEditStepperMdvZ)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_MDV_U, &CMtnStepperDlg::OnEnKillfocusEditStepperMdvU)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_AC_X, &CMtnStepperDlg::OnEnKillfocusEditStepperAcX)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_AC_Y, &CMtnStepperDlg::OnEnKillfocusEditStepperAcY)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_AC_Z, &CMtnStepperDlg::OnEnKillfocusEditStepperAcZ)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_AC_U, &CMtnStepperDlg::OnEnKillfocusEditStepperAcU)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_JERK_X, &CMtnStepperDlg::OnEnKillfocusEditStepperJerkX)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_JERK_Y, &CMtnStepperDlg::OnEnKillfocusEditStepperJerkY)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_JERK_Z, &CMtnStepperDlg::OnEnKillfocusEditStepperJerkZ)
	ON_EN_KILLFOCUS(IDC_EDIT_STEPPER_JERK_U, &CMtnStepperDlg::OnEnKillfocusEditStepperJerkU)
END_MESSAGE_MAP()


// CMtnStepperDlg message handlers

void CMtnStepperDlg::OnChangeComboStepperBoardId_Ad1240()
{
	unsigned int uiTemp, ulErrCode = 0;

	uiTemp = StepperBoardIdCombo.GetCurSel();

	if(uiCurrBoardId != uiTemp)
	{
		if(abStepperCtrlBoardOpen[uiCurrBoardId] == 1)
		{
			ulErrCode = (ULONG)P1240MotDevClose(uiCurrBoardId);
		}
		if(ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		else
		{
			abStepperCtrlBoardOpen[uiCurrBoardId] = 0;
			uiCurrBoardId = uiTemp;
		}
	}
}
	
// IDC_COMBO_STEPPER_BOARD_ID
void CMtnStepperDlg::OnCbnSelchangeComboStepperBoardId()
{
	if(cFlagIsUsingAd1240)
	{
		OnChangeComboStepperBoardId_Ad1240();
	}
	else if(cFlagIsUsingLeeTro2812 == 1)
	{
		uiCurrBoardId = StepperBoardIdCombo.GetCurSel();
	}
	UpdateButtonText();
	UpdateWbAppName_CurrentStpCtrlBoard();
}

// IDC_EDIT_STEPPER_MOVE_DIST_X
// IDC_EDIT_STEPPER_MOVE_DIST_Y
// IDC_EDIT_STEPPER_MOVE_DIST_Z
// IDC_EDIT_STEPPER_MOVE_DIST_U
void CMtnStepperDlg::UpdateEditText()
{
//	alMoveDistAtAxis[ii] = DEF_MOVING_PULSE_STEPPER;
	CString cstrEditText;

	cstrEditText.Format("%d", alMoveDistAtAxis[0]);
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_X)->SetWindowTextA(cstrEditText);
	cstrEditText.Format("%d", alMoveDistAtAxis[1]);
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Y)->SetWindowTextA(cstrEditText);
	cstrEditText.Format("%d", alMoveDistAtAxis[2]);
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Z)->SetWindowTextA(cstrEditText);
	cstrEditText.Format("%d", alMoveDistAtAxis[3]);
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_U)->SetWindowTextA(cstrEditText);
}
// IDC_EDIT_STEPPER_SV_X // IDC_EDIT_STEPPER_SV_Y // IDC_EDIT_STEPPER_SV_Z // IDC_EDIT_STEPPER_SV_U
// IDC_EDIT_STEPPER_DV_X // IDC_EDIT_STEPPER_DV_Y // IDC_EDIT_STEPPER_DV_Z // IDC_EDIT_STEPPER_DV_U
// IDC_EDIT_STEPPER_MDV_X // IDC_EDIT_STEPPER_MDV_Y // IDC_EDIT_STEPPER_MDV_Z // IDC_EDIT_STEPPER_MDV_U
// IDC_EDIT_STEPPER_AC_X // IDC_EDIT_STEPPER_AC_Y // IDC_EDIT_STEPPER_AC_Z // IDC_EDIT_STEPPER_AC_U
// IDC_EDIT_STEPPER_JERK_X // IDC_EDIT_STEPPER_JERK_Y // IDC_EDIT_STEPPER_JERK_Z // IDC_EDIT_STEPPER_JERK_U
// IDC_BUTTON_STEPPER_MOVE_DIST // IDC_BUTTON_STEPPER_TO_TARGET_POSN // IDC_BUTTON_STEPPER_HOME_AXIS // IDC_BUTTON_STEPPER_SAVE_PROFILE
void CMtnStepperDlg::UpdateButtonText()
{
	// check box
	((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_X))->SetCheck(bStepperSelX[uiCurrBoardId]);
	((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Y))->SetCheck(bStepperSelY[uiCurrBoardId]);
	((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Z))->SetCheck(bStepperSelZ[uiCurrBoardId]);
	((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_U))->SetCheck(bStepperSelU[uiCurrBoardId]);

	if(abStepperCtrlBoardOpen[uiCurrBoardId] == 0)
	{
		GetDlgItem(IDC_BUTTON_STEPPER_OPEN_CLOSE_BOARD)->SetWindowTextA(_T("Open Board"));
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_U)->EnableWindow(FALSE);

		GetDlgItem(IDC_BUTTON_STEPPER_MOVE_DIST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STEPPER_TO_TARGET_POSN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STEPPER_HOME_AXIS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STEPPER_SAVE_PROFILE)->EnableWindow(FALSE);

		GetDlgItem(IDC_CHECK_PCI1240_OUT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_PCI1240_OUT7)->EnableWindow(FALSE);		
	}
	else
	{
		GetDlgItem(IDC_BUTTON_STEPPER_OPEN_CLOSE_BOARD)->SetWindowTextA(_T("Close Board"));
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_U)->EnableWindow(TRUE);

		GetDlgItem(IDC_BUTTON_STEPPER_MOVE_DIST)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_STEPPER_HOME_AXIS)->EnableWindow(TRUE);

		if(cFlagIsUsingAd1240 == 1)
		{
			GetDlgItem(IDC_BUTTON_STEPPER_TO_TARGET_POSN)->EnableWindow(TRUE);
			
			GetDlgItem(IDC_BUTTON_STEPPER_SAVE_PROFILE)->EnableWindow(TRUE);

			GetDlgItem(IDC_CHECK_PCI1240_OUT4)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_PCI1240_OUT5)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_PCI1240_OUT6)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_PCI1240_OUT7)->EnableWindow(TRUE);
		}
	}

	// Button text
	if(bStepperSelX[uiCurrBoardId] == 1)
	{
		sprintf_s(strCharAxisX, 2, "X");
		GetDlgItem(IDC_EDIT_STEPPER_SV_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_X)->EnableWindow(TRUE);
	}
	else
	{
		sprintf_s(strCharAxisX, 2, "");
		GetDlgItem(IDC_EDIT_STEPPER_SV_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_X)->EnableWindow(FALSE);
	}

	if(bStepperSelY[uiCurrBoardId] == 1)
	{
		sprintf_s(strCharAxisY, 2, "Y");
		GetDlgItem(IDC_EDIT_STEPPER_SV_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Y)->EnableWindow(TRUE);
	}
	else
	{
		sprintf_s(strCharAxisY, 2, "");
		GetDlgItem(IDC_EDIT_STEPPER_SV_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Y)->EnableWindow(FALSE);
	}

	if(bStepperSelZ[uiCurrBoardId] == 1)
	{
		sprintf_s(strCharAxisZ, 2, "Z");
		GetDlgItem(IDC_EDIT_STEPPER_SV_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Z)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Z)->EnableWindow(TRUE);
	}
	else
	{
		sprintf_s(strCharAxisZ, 2, "");
		GetDlgItem(IDC_EDIT_STEPPER_SV_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Z)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Z)->EnableWindow(FALSE);
	}

	if(bStepperSelU[uiCurrBoardId] == 1)
	{
		sprintf_s(strCharAxisU, 2, "U");
		GetDlgItem(IDC_EDIT_STEPPER_SV_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_U)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_U)->EnableWindow(TRUE);
	}
	else
	{
		sprintf_s(strCharAxisU, 2, "");
		GetDlgItem(IDC_EDIT_STEPPER_SV_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_DV_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MDV_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_AC_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_JERK_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_U)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_U)->EnableWindow(FALSE);
	}

	cstrButtonText.Format("Move %s%s%s%s RelDist", strCharAxisX, strCharAxisY, strCharAxisZ, strCharAxisU);
	GetDlgItem(IDC_BUTTON_STEPPER_MOVE_DIST)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("Move %s%s%s%s ToTarget", strCharAxisX, strCharAxisY, strCharAxisZ, strCharAxisU);
	GetDlgItem(IDC_BUTTON_STEPPER_TO_TARGET_POSN)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("Home %s%s%s%s", strCharAxisX, strCharAxisY, strCharAxisZ, strCharAxisU);
	GetDlgItem(IDC_BUTTON_STEPPER_HOME_AXIS)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("Update %s%s%s%s Para", strCharAxisX, strCharAxisY, strCharAxisZ, strCharAxisU);
	GetDlgItem(IDC_BUTTON_STEPPER_SAVE_PROFILE)->SetWindowTextA(cstrButtonText);

	// Text on 

}
#include "MtnTesterResDef.h"
void CMtnStepperDlg::UI_SetAppNameForOneTrackVerLED(unsigned int uiBoardId)
{
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		switch(uiBoardId)
		{
		case 0:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("Injecter"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("LeftClaw"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("RightClaw"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("OffloadKicker"));
			break;
		case 1:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("OnloadPicker"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("Clamp"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("-"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("-"));
			break;
		}
	}
	else
	{
		switch(uiBoardId)
		{
		case 0:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("进料"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("左拨爪"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("右拨爪"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("下料踢片"));
			break;
		case 1:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("上料取片"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("夹片"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("-"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("-"));
			break;
		}
	}
}

void CMtnStepperDlg::UI_SetAppNameForHoriLED(unsigned int uiBoardId)
{
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		switch(uiBoardId)
		{
		case 0:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("Onloader Elevator"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("Clamp motor"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("Index motor"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("Offloader Elevator"));
			break;
		case 1:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("L.F.InnerTrack"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("L.F.OuterTrack"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("WindowClamp(Upper)"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("TopPlate (Lower)"));
			break;
		case 2:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("Offloader Elevator-Y"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("Offloader Elevator-Z"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("Offloader Magazine Holder"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T(""));
			break;
		}
	}
	else
	{
		switch(uiBoardId)
		{
		case 0:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("上料升降机"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("焊台夹具"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("焊台送片"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("下料升降机"));
			break;
		case 1:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("内侧轨道"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("外侧轨道"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("焊台上夹"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T("焊台下夹"));
			break;
		case 2:
			GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("下料水平"));
			GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("下料垂直"));
			GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("下料夹具"));
			GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T(""));
			break;
		}
	}
}
void CMtnStepperDlg::UI_SetAppNameForVerticalLED_Fork()
{
	if(get_sys_language_option() == LANGUAGE_UI_EN)
	{
		GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("InnerTrackClamper"));
		GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("OuterTrackClamper"));
		GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("Onloader Pusher"));
		GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T(""));
	}
	else
	{
		GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T("内导轨夹具"));
		GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T("外导轨夹具"));
		GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T("上料推进器"));
		GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T(""));
	}
}

void CMtnStepperDlg::UI_SetAppNameNull()
{
	GetDlgItem(IDC_STATIC_APP_NAME_X_CURR_BD)->SetWindowTextA(_T(""));
	GetDlgItem(IDC_STATIC_APP_NAME_Y_CURR_BD)->SetWindowTextA(_T(""));
	GetDlgItem(IDC_STATIC_APP_NAME_Z_CURR_BD)->SetWindowTextA(_T(""));
	GetDlgItem(IDC_STATIC_APP_NAME_U_CURR_BD)->SetWindowTextA(_T(""));
}

#include "MotAlgo_DLL.h"

void CMtnStepperDlg::UpdateWbAppName_CurrentStpCtrlBoard()
{
	int iFlagSysMachineType;

	iFlagSysMachineType = get_sys_machine_type_flag();
	switch(iFlagSysMachineType)
	{
	case WB_MACH_TYPE_VLED_MAGAZINE:
			UI_SetAppNameNull();
		break;
	case WB_MACH_TYPE_VLED_FORK:
		if( uiCurrBoardId == 0)
		{
			UI_SetAppNameForVerticalLED_Fork();
		}
		else
		{
			UI_SetAppNameNull();
		}
		break;
	case WB_MACH_TYPE_HORI_LED:
	case BE_WB_HORI_20T_LED:
		if( uiCurrBoardId <= 2)
		{
			UI_SetAppNameForHoriLED(uiCurrBoardId);
		}
		else
		{
			UI_SetAppNameNull();
		}
		break;
	case WB_MACH_TYPE_ONE_TRACK_13V_LED:  // 20110504
	case BE_WB_ONE_TRACK_18V_LED:
		if(uiCurrBoardId <= 1)
		{
			UI_SetAppNameForOneTrackVerLED(uiCurrBoardId);
		}
		else
		{
			UI_SetAppNameNull();
		}
		break;
	}
}

void CMtnStepperDlg::UpdateSpeedProfileText()
{
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperStartVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_SV_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperDriveVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_DV_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_MDV_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxAcceleration);
	GetDlgItem(IDC_EDIT_STEPPER_AC_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxJerk);
	GetDlgItem(IDC_EDIT_STEPPER_JERK_X)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperStartVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_SV_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperDriveVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_DV_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_MDV_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxAcceleration);
	GetDlgItem(IDC_EDIT_STEPPER_AC_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxJerk);
	GetDlgItem(IDC_EDIT_STEPPER_JERK_Y)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperStartVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_SV_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperDriveVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_DV_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_MDV_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxAcceleration);
	GetDlgItem(IDC_EDIT_STEPPER_AC_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxJerk);
	GetDlgItem(IDC_EDIT_STEPPER_JERK_Z)->SetWindowTextA(cstrButtonText);

	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperStartVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_SV_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperDriveVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_DV_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxVelocity);
	GetDlgItem(IDC_EDIT_STEPPER_MDV_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxAcceleration);
	GetDlgItem(IDC_EDIT_STEPPER_AC_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%10.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxJerk);
	GetDlgItem(IDC_EDIT_STEPPER_JERK_U)->SetWindowTextA(cstrButtonText);

}

void CMtnStepperDlg::UI_UpdateDigitalInputStatusFilling(unsigned int uiStaticId, unsigned short usFlagLighting)
{
	CWnd *pWnd; // , *pWnd1;
	CDC *pDC; // , *pDC1;
	CRect Rect;//, Rect1;
	COLORREF liColourRGB;

	CString cstrTemp;
	cstrTemp.Format("%d", usFlagLighting);
	GetDlgItem(uiStaticId)->SetWindowTextA(cstrTemp);
	pWnd = GetDlgItem(uiStaticId);
	
	pDC = pWnd->GetDC();
	pWnd->GetClientRect(&Rect);
	if(usFlagLighting == 0)
	{
		liColourRGB = RGB(0, 127, 0);
	}
	else
	{
		liColourRGB = RGB(127, 0, 0);
	}
	pDC->FillSolidRect(Rect, liColourRGB);
	ReleaseDC(pDC);

}

// IDC_STATIC_STEPPER_DI_PL_X, IDC_STATIC_STEPPER_DI_PL_Y, IDC_STATIC_STEPPER_DI_PL_Z, IDC_STATIC_STEPPER_DI_PL_U
// IDC_STATIC_STEPPER_DI_NL_X, IDC_STATIC_STEPPER_DI_NL_Y, IDC_STATIC_STEPPER_DI_NL_Z, IDC_STATIC_STEPPER_DI_NL_U
// IDC_STATIC_STEPPER_DI_HOME_X, IDC_STATIC_STEPPER_DI_HOME_Y, IDC_STATIC_STEPPER_DI_HOME_Z, IDC_STATIC_STEPPER_DI_HOME_U
void CMtnStepperDlg::UpdateUI_FromDigitalInput()
{
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_PL_X, ausDI_PosiLimitAtAxis[0]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_PL_Y, ausDI_PosiLimitAtAxis[1]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_PL_Z, ausDI_PosiLimitAtAxis[2]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_PL_U, ausDI_PosiLimitAtAxis[3]);

	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_NL_X, ausDI_NegLimitAtAxis[0]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_NL_Y, ausDI_NegLimitAtAxis[1]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_NL_Z, ausDI_NegLimitAtAxis[2]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_NL_U, ausDI_NegLimitAtAxis[3]);

	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_HOME_X, ausDI_HomeStatusAtAxis[0]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_HOME_Y, ausDI_HomeStatusAtAxis[1]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_HOME_Z, ausDI_HomeStatusAtAxis[2]);
	UI_UpdateDigitalInputStatusFilling(IDC_STATIC_STEPPER_DI_HOME_U, ausDI_HomeStatusAtAxis[3]);
}

void CMtnStepperDlg::UpdateDigitalInput_StepperCtrlLeetro2812()
{
	int iMinAxisOnBoard = uiCurrBoardId * 4 + 1;
	int iRetCheckLimit;
	int ii;

	for(ii = 0; ii<=3; ii++)
	{
		ausDI_HomeStatusAtAxis[ii] = check_home(iMinAxisOnBoard + ii);
		iRetCheckLimit = check_limit(iMinAxisOnBoard + ii);
		if(iRetCheckLimit == 1)
		{
			ausDI_PosiLimitAtAxis[ii] = 1; ausDI_NegLimitAtAxis[ii] = 0;
		}
		else if(iRetCheckLimit == -1)
		{
			ausDI_PosiLimitAtAxis[ii] = 0; ausDI_NegLimitAtAxis[ii] = 1;
		}
		else
		{
			ausDI_PosiLimitAtAxis[ii] = 0; ausDI_NegLimitAtAxis[ii] = 0;
		}
	}
}

#define PCI1240_BIT_HOME_IN0_FROM_0   0x100
#define PCI1240_BIT_HOME_IN3_FROM_0   0x800
#define PCI1240_BIT_HOME_POS_LMT_FROM_0   0x1000
#define PCI1240_BIT_HOME_NEG_LMT_FROM_0   0x2000

#define PCI1240_BIT_HOME_XIN3_FROM_RR4  0x8
#define PCI1240_BIT_HOME_YIN3_FROM_RR4  0x800
#define PCI1240_BIT_HOME_ZIN3_FROM_RR5  0x8
#define PCI1240_BIT_HOME_UIN3_FROM_RR5  0x800

#define PCI1240_BIT_HOME_H_POS_LMT_FROM_RR2   0x4
#define PCI1240_BIT_HOME_H_NEG_LMT_FROM_RR2   0x8

void CMtnStepperDlg::UpdateDigitalInput_StepperCtrl1240()
{
//static DWORD ausDI_PosiLimitAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
//static DWORD ausDI_NegLimitAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
//static DWORD ausDI_HomeStatusAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
DWORD ausMotionStatus[MAX_STEPPER_AXIX_ON_BOARD];
ULONG   ulErrCode;

		// Read P1240 4 axes logical posiition
		ulErrCode = (ULONG) P1240MotRdReg(uiCurrBoardId, X_Axis, RR4, &ausMotionStatus[0]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ausDI_HomeStatusAtAxis[0] = ausMotionStatus[0] & PCI1240_BIT_HOME_XIN3_FROM_RR4;  // PCI1240_BIT_HOME_IN3_FROM_0


		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, Y_Axis, RR4, &ausMotionStatus[1]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ausDI_HomeStatusAtAxis[1] = ausMotionStatus[1] & PCI1240_BIT_HOME_YIN3_FROM_RR4; // PCI1240_BIT_HOME_IN3_FROM_0;

		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, Z_Axis, RR5, &ausMotionStatus[2]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ausDI_HomeStatusAtAxis[2] = ausMotionStatus[2] & PCI1240_BIT_HOME_ZIN3_FROM_RR5; //PCI1240_BIT_HOME_IN3_FROM_0;

		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, U_Axis, RR5, &ausMotionStatus[3]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ausDI_HomeStatusAtAxis[3] = ausMotionStatus[3] & PCI1240_BIT_HOME_UIN3_FROM_RR5; //PCI1240_BIT_HOME_IN3_FROM_0;

		// Limit Status
		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, X_Axis, RR2, &ausMotionStatus[0]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, Y_Axis, RR2, &ausMotionStatus[1]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, Z_Axis, RR2, &ausMotionStatus[2]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}
		ulErrCode = (ULONG)P1240MotRdReg(uiCurrBoardId, U_Axis, RR2, &ausMotionStatus[3]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
			return;
		}

		ausDI_PosiLimitAtAxis[0] = ausMotionStatus[0] & PCI1240_BIT_HOME_H_POS_LMT_FROM_RR2; // PCI1240_BIT_HOME_POS_LMT_FROM_0;
		ausDI_NegLimitAtAxis[0] = ausMotionStatus[0] & PCI1240_BIT_HOME_H_NEG_LMT_FROM_RR2; // PCI1240_BIT_HOME_NEG_LMT_FROM_0;
		ausDI_PosiLimitAtAxis[1] = ausMotionStatus[1] & PCI1240_BIT_HOME_H_POS_LMT_FROM_RR2; // PCI1240_BIT_HOME_POS_LMT_FROM_0;
		ausDI_NegLimitAtAxis[1] = ausMotionStatus[1] & PCI1240_BIT_HOME_H_NEG_LMT_FROM_RR2; // PCI1240_BIT_HOME_NEG_LMT_FROM_0;
		ausDI_PosiLimitAtAxis[2] = ausMotionStatus[2] & PCI1240_BIT_HOME_H_POS_LMT_FROM_RR2; // PCI1240_BIT_HOME_POS_LMT_FROM_0;
		ausDI_NegLimitAtAxis[2] = ausMotionStatus[2] & PCI1240_BIT_HOME_H_NEG_LMT_FROM_RR2; // PCI1240_BIT_HOME_NEG_LMT_FROM_0;
		ausDI_PosiLimitAtAxis[3] = ausMotionStatus[3] & PCI1240_BIT_HOME_H_POS_LMT_FROM_RR2; // PCI1240_BIT_HOME_POS_LMT_FROM_0;
		ausDI_NegLimitAtAxis[3] = ausMotionStatus[3] & PCI1240_BIT_HOME_H_NEG_LMT_FROM_RR2; // PCI1240_BIT_HOME_NEG_LMT_FROM_0;

}

// IDC_EDIT_STEPPER_POS_LIMIT_X // IDC_EDIT_STEPPER_POS_LIMIT_Y // IDC_EDIT_STEPPER_POS_LIMIT_Z // IDC_EDIT_STEPPER_POS_LIMIT_U
// IDC_EDIT_STEPPER_NEG_LIMIT_X // IDC_EDIT_STEPPER_NEG_LIMIT_Y // IDC_EDIT_STEPPER_NEG_LIMIT_Z // IDC_EDIT_STEPPER_NEG_LIMIT_U
// IDC_EDIT_STEPPER_HOME_TYPE_X // IDC_EDIT_STEPPER_HOME_TYPE_Y // IDC_EDIT_STEPPER_HOME_TYPE_Z // IDC_EDIT_STEPPER_HOME_TYPE_U
// IDC_EDIT_STEPPER_HOME_MODE_X // IDC_EDIT_STEPPER_HOME_MODE_Y // IDC_EDIT_STEPPER_HOME_MODE_Z // IDC_EDIT_STEPPER_HOME_MODE_U
// IDC_EDIT_STEPPER_HOME_P0_VEL_X // IDC_EDIT_STEPPER_HOME_P0_VEL_Y // IDC_EDIT_STEPPER_HOME_P0_VEL_Z // IDC_EDIT_STEPPER_HOME_P0_VEL_U
// IDC_EDIT_STEPPER_HOME_P0_DIR_X // IDC_EDIT_STEPPER_HOME_P0_DIR_Y // IDC_EDIT_STEPPER_HOME_P0_DIR_Z // IDC_EDIT_STEPPER_HOME_P0_DIR_U
// IDC_EDIT_STEPPER_HOME_OFFSET_X // IDC_EDIT_STEPPER_HOME_OFFSET_Y // IDC_EDIT_STEPPER_HOME_OFFSET_Z // IDC_EDIT_STEPPER_HOME_OFFSET_U
// IDC_EDIT_STEPPER_HOME_P1_VEL_X // IDC_EDIT_STEPPER_HOME_P1_VEL_Y // IDC_EDIT_STEPPER_HOME_P1_VEL_Z // IDC_EDIT_STEPPER_HOME_P1_VEL_U
// IDC_EDIT_STEPPER_HOME_P1_DIR_X // IDC_EDIT_STEPPER_HOME_P1_DIR_Y // IDC_EDIT_STEPPER_HOME_P1_DIR_Z // IDC_EDIT_STEPPER_HOME_P1_DIR_U
void CMtnStepperDlg::UpdatePositionRegistrationText()
{
	// X
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dPositiveLimit);
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dNegativeLimit);
	GetDlgItem(IDC_EDIT_STEPPER_NEG_LIMIT_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeType);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeMode);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeOffset);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_X)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_X)->SetWindowTextA(cstrButtonText);

	// Y
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dPositiveLimit);
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dNegativeLimit);
	GetDlgItem(IDC_EDIT_STEPPER_NEG_LIMIT_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeType);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeMode);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeOffset);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_Y)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_Y)->SetWindowTextA(cstrButtonText);

	// Z
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dPositiveLimit);
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dNegativeLimit);
	GetDlgItem(IDC_EDIT_STEPPER_NEG_LIMIT_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeType);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeMode);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeOffset);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_Z)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_Z)->SetWindowTextA(cstrButtonText);

	// U
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dPositiveLimit);
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dNegativeLimit);
	GetDlgItem(IDC_EDIT_STEPPER_NEG_LIMIT_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeType);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeMode);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%8.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeOffset);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%6.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Vel);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_U)->SetWindowTextA(cstrButtonText);
	cstrButtonText.Format("%2.0f", astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Dir);
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_U)->SetWindowTextA(cstrButtonText);
}
// IDC_CHECK_STEPPER_SEL_AXIS_X
// IDC_CHECK_STEPPER_SEL_AXIS_Y
// IDC_CHECK_STEPPER_SEL_AXIS_Z
// IDC_CHECK_STEPPER_SEL_AXIS_U
void CMtnStepperDlg::OnBnClickedCheckStepperSelAxisX()
{
	bStepperSelX[uiCurrBoardId] = ((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_X))->GetCheck();
	UpdateButtonText();
}
void CMtnStepperDlg::OnBnClickedCheckStepperSelAxisY()
{
	bStepperSelY[uiCurrBoardId] = ((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Y))->GetCheck();
	UpdateButtonText();
}
void CMtnStepperDlg::OnBnClickedCheckStepperSelAxisZ()
{
	bStepperSelZ[uiCurrBoardId] = ((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_Z))->GetCheck();
	UpdateButtonText();
}
void CMtnStepperDlg::OnBnClickedCheckStepperSelAxisU()
{
	bStepperSelU[uiCurrBoardId] = ((CButton*)GetDlgItem(IDC_CHECK_STEPPER_SEL_AXIS_U))->GetCheck();
	UpdateButtonText();
}

void CMtnStepperDlg::MyShowErrorMsg(long ulErrCode)
{
	CString s1,s2;
	char buffer[128];

	_itoa((int)ulErrCode, buffer, 10 );
	s1.Format("Line:%d, Error ", uiErrorLine); 
	s2 = s1 +  buffer + ": " + pstrStepperReturnError_1240[ulErrCode]; 
	MessageBox( s2, "ContiDrive", MB_OK );
}
// IDC_BUTTON_STEPPER_OPEN_CLOSE_BOARD
void CMtnStepperDlg::OnBnClickedButtonStepperOpenCloseBoard()
{
	ULONG   ulErrCode = 0;

	if(cFlagIsUsingAd1240)
	{
		if(abStepperCtrlBoardOpen[uiCurrBoardId] == 0)
		{ // now is close, to be open
			ulErrCode = (ULONG)P1240MotDevOpen(uiCurrBoardId);
		}
		else
		{ // now is open, to be close
			ulErrCode = (ULONG)P1240MotDevClose(uiCurrBoardId);
		}
	}

	if(ulErrCode != 0)
	{
		uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
		return;
	}
	else
	{
//		cFlagIsUsingAd1240 = 1;  // 20120830
		abStepperCtrlBoardOpen[uiCurrBoardId] = 1 - abStepperCtrlBoardOpen[uiCurrBoardId];
		if(abStepperCtrlBoardOpen[uiCurrBoardId] == 1)
		{
			UploadSpeedProfileCurrBoard();
			Sleep(20);
//			mtnstp_wb_set_home_para(uiCurrBoardId, &ulErrCode);
			Sleep(20);
		}
		UpdateButtonText();
		UpdateSpeedProfileText();
		UpdatePositionRegistrationText();
	}
}

void CMtnStepperDlg::UploadSpeedProfileCurrBoard()
{
	ULONG   ulErrCode;

	if(cFlagIsUsingAd1240)
	{
		if(mtnstp_upload_parameters_ads1240(uiCurrBoardId, &ulErrCode))
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
		}
	}
	else if(cFlagIsUsingLeeTro2812)
	{
		mtnstp_upload_parameters_leetro_2812(uiCurrBoardId, &ulErrCode);
	}
}

void CMtnStepperDlg::OnBnClickedButtonStepperMoveDist()
{
	ULONG   ulErrCode;
    BYTE	byDriveAxes = 0;

	if(iFlagIterativeMove)
	{
		RunThreadDlgStepper1240();
	}
	else
	{
		if(cFlagIsUsingAd1240)
		{
			if(bStepperSelX[uiCurrBoardId])
				byDriveAxes = byDriveAxes | X_Axis;

			if(bStepperSelY[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Y_Axis;

			if(bStepperSelZ[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Z_Axis;

			if(bStepperSelU[uiCurrBoardId])
				byDriveAxes = byDriveAxes | U_Axis;

			ulErrCode = (ULONG)P1240MotPtp(uiCurrBoardId, byDriveAxes, 0, 
				alMoveDistAtAxis[0], alMoveDistAtAxis[1], alMoveDistAtAxis[2], alMoveDistAtAxis[3]);
			if( ulErrCode != 0)
			{
				uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
				return;
			}
		}
		else if(cFlagIsUsingLeeTro2812)
		{
			int idxMinAxis = uiCurrBoardId * 4 + 1;
			mtnstp_download_parameters_leetro_2812(uiCurrBoardId, &ulErrCode);
			if(bStepperSelX[uiCurrBoardId])
			{
//				set_maxspeed(idxMinAxis); set_conspeed(idxMinAxis); set_profile(idxMinAxis);
				fast_pmove(idxMinAxis, ((int)alMoveDistAtAxis[0])); // (double)
			}
			if(bStepperSelY[uiCurrBoardId])
			{
				fast_pmove(idxMinAxis + 1, ((int)alMoveDistAtAxis[1])); // (double)
			}
			if(bStepperSelZ[uiCurrBoardId])
			{
				fast_pmove(idxMinAxis + 2, ((int)alMoveDistAtAxis[2])); // (double)
			}
			if(bStepperSelU[uiCurrBoardId])
			{
				fast_pmove(idxMinAxis + 3, ((int)alMoveDistAtAxis[3])); // (double)
			}
		}
	}
}


// IDC_EDIT_STEPPER_MOVE_DIST_X
// IDC_EDIT_STEPPER_MOVE_DIST_Y
// IDC_EDIT_STEPPER_MOVE_DIST_Z
// IDC_EDIT_STEPPER_MOVE_DIST_U
void CMtnStepperDlg::OnEnChangeEditStepperMoveDistX()
{
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_X)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp, "%ld", &alMoveDistAtAxis[0]);
}

void CMtnStepperDlg::OnEnChangeEditStepperMoveDistY()
{
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Y)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp, "%ld", &alMoveDistAtAxis[1]);
}

void CMtnStepperDlg::OnEnChangeEditStepperMoveDistZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_Z)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp, "%ld", &alMoveDistAtAxis[2]);
}

void CMtnStepperDlg::OnEnChangeEditStepperMoveDistU()
{
	GetDlgItem(IDC_EDIT_STEPPER_MOVE_DIST_U)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%ld", &alMoveDistAtAxis[3]);
}

// IDC_EDIT_STEPPER_TARGET_POSN_X
// IDC_EDIT_STEPPER_TARGET_POSN_Y
// IDC_EDIT_STEPPER_TARGET_POSN_Z
// IDC_EDIT_STEPPER_TARGET_POSN_U
void CMtnStepperDlg::OnEnChangeEditStepperTargetPosnX()
{
	GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_X)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%ld", &aiTargetPosnAtAxis[0]);
}

void CMtnStepperDlg::OnEnChangeEditStepperTargetPosnY()
{
	GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Y)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%ld", &aiTargetPosnAtAxis[1]);
}

void CMtnStepperDlg::OnEnChangeEditStepperTargetPosnZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_Z)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%ld", &aiTargetPosnAtAxis[2]);
}

void CMtnStepperDlg::OnEnChangeEditStepperTargetPosnU()
{
	GetDlgItem(IDC_EDIT_STEPPER_TARGET_POSN_U)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp, "%ld", &aiTargetPosnAtAxis[3]);
}
static int iFlagDlgStepperCtrlRunningTimer = FALSE;
int CMtnStepperDlg::DlgGetFlagTimerRunning()
{
	return iFlagDlgStepperCtrlRunningTimer;
}
BOOL CMtnStepperDlg::ShowWindow(int nCmdShow)
{
	iFlagDlgStepperCtrlRunningTimer = TRUE;
	return CDialog::ShowWindow(nCmdShow);
}

void  CMtnStepperDlg::DlgTimerEvent()
{
	ULONG   ulErrCode;

	if(abStepperCtrlBoardOpen[uiCurrBoardId] == 1)
	{
		// Read P1240 4 axes logical posiition
		if(cFlagIsUsingAd1240)
		{
			ulErrCode = (ULONG)P1240MotRdMutiReg(uiCurrBoardId,        // Board ID
											 XYZU_Axis,      // Which axes
											 Lcnt,           // Logical counter
											 &aiFeedbackPosnAtAxis[0],        // X axis position
											 &aiFeedbackPosnAtAxis[1],        // Y axis position
											 &aiFeedbackPosnAtAxis[2],        // Z axis position
											 &aiFeedbackPosnAtAxis[3]);       // U axis position

			if( ulErrCode != 0)
			{
				uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
				return;
			}
			UpdateDigitalInput_StepperCtrl1240();
		}
		else if(cFlagIsUsingLeeTro2812)
		{
			int iMinAxisOnBoard, iMaxAxisOnBoard;
			double dAbsolutePosn;
			iMinAxisOnBoard = uiCurrBoardId * 4 + 1;
			iMaxAxisOnBoard = iMinAxisOnBoard + 3;

			for(int ii = iMinAxisOnBoard; ii<= iMaxAxisOnBoard; ii++)
			{
				get_abs_pos(ii,&dAbsolutePosn);

				aiFeedbackPosnAtAxis[ii - iMinAxisOnBoard] = (int)dAbsolutePosn;

			}
			UpdateDigitalInput_StepperCtrlLeetro2812();
		}


		cstrButtonText.Format("%ld", aiFeedbackPosnAtAxis[0]);
		GetDlgItem(IDC_EDIT_SHOW_FD_POSN_X)->SetWindowTextA(cstrButtonText);
		cstrButtonText.Format("%ld", aiFeedbackPosnAtAxis[1]);
		GetDlgItem(IDC_EDIT_SHOW_FD_POSN_Y)->SetWindowTextA(cstrButtonText);
		cstrButtonText.Format("%ld", aiFeedbackPosnAtAxis[2]);
		GetDlgItem(IDC_EDIT_SHOW_FD_POSN_Z)->SetWindowTextA(cstrButtonText);
		cstrButtonText.Format("%ld", aiFeedbackPosnAtAxis[3]);
		GetDlgItem(IDC_EDIT_SHOW_FD_POSN_U)->SetWindowTextA(cstrButtonText);

		// Update Motion Status DigitalInput, and UI
		UpdateUI_FromDigitalInput();

		// Update Iterative motion status, IDC_STATIC_STEPPER_1240_ITERATIVE_STATUS
		CString cstrTemp;
		cstrTemp.Format("%d / %d", uiCurrentIteration, uiMoveTotalIteration);
		GetDlgItem(IDC_STATIC_STEPPER_1240_ITERATIVE_STATUS)->SetWindowTextA(cstrTemp);		
	}
}
// IDC_EDIT_SHOW_FD_POSN_X
// IDC_EDIT_SHOW_FD_POSN_Y
// IDC_EDIT_SHOW_FD_POSN_Z
// IDC_EDIT_SHOW_FD_POSN_U
//void CMtnStepperDlg::OnTimer(UINT nIDEvent) 
//{
////	DlgTimerEvent();
//	CDialog::OnTimer(nIDEvent);
//}

// Search Home for multiple axis
void CMtnStepperDlg::OnBnClickedButtonStepperHomeAxis()
{	
	ULONG   ulErrCode;
    BYTE	byDriveAxes = 0;
	if(cFlagIsUsingAd1240) // 20120902
	{
		if(bStepperSelX[uiCurrBoardId])
			byDriveAxes = byDriveAxes | X_Axis;

		if(bStepperSelY[uiCurrBoardId])
			byDriveAxes = byDriveAxes | Y_Axis;

		if(bStepperSelZ[uiCurrBoardId])
			byDriveAxes = byDriveAxes | Z_Axis;

		if(bStepperSelU[uiCurrBoardId])
			byDriveAxes = byDriveAxes | U_Axis;

		ulErrCode = (ULONG)P1240MotHome(uiCurrBoardId, byDriveAxes);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
			return;
		}
	}
	else if(cFlagIsUsingLeeTro2812 == 1)
	{
		double dAbsolutePosn, dPrevAbsolutePosn;

		int idxMinAxis = uiCurrBoardId * 4 + 1;
		if(bStepperSelX[uiCurrBoardId])
		{
			fast_hmove(idxMinAxis, 1);
		}
		get_abs_pos(idxMinAxis,&dPrevAbsolutePosn);
		Sleep(100);
		get_abs_pos(idxMinAxis,&dAbsolutePosn);
		while(fabs(dAbsolutePosn - dPrevAbsolutePosn) > 1.0)
		{
			dPrevAbsolutePosn = dAbsolutePosn;
			Sleep(100);
			get_abs_pos(idxMinAxis,&dAbsolutePosn);
		}
		reset_pos(idxMinAxis);

		if(bStepperSelY[uiCurrBoardId])
		{
			fast_hmove(idxMinAxis + 1, 1);
		}
		get_abs_pos(idxMinAxis+1,&dPrevAbsolutePosn);
		Sleep(100);
		get_abs_pos(idxMinAxis+1,&dAbsolutePosn);
		while(fabs(dAbsolutePosn - dPrevAbsolutePosn) > 1.0)
		{
			dPrevAbsolutePosn = dAbsolutePosn;
			Sleep(100);
			get_abs_pos(idxMinAxis+1,&dAbsolutePosn);
		}
		reset_pos(idxMinAxis+1);

		if(bStepperSelZ[uiCurrBoardId])
		{
			fast_hmove(idxMinAxis + 2, 1);
		}
		get_abs_pos(idxMinAxis+2,&dPrevAbsolutePosn);
		Sleep(100);
		get_abs_pos(idxMinAxis+2,&dAbsolutePosn);
		while(fabs(dAbsolutePosn - dPrevAbsolutePosn) > 1.0)
		{
			dPrevAbsolutePosn = dAbsolutePosn;
			Sleep(100);
			get_abs_pos(idxMinAxis+2,&dAbsolutePosn);
		}
		reset_pos(idxMinAxis+2);

		if(bStepperSelU[uiCurrBoardId])
		{
			fast_hmove(idxMinAxis + 3, 1);
		}
		get_abs_pos(idxMinAxis+3,&dPrevAbsolutePosn);
		Sleep(100);
		get_abs_pos(idxMinAxis+3,&dAbsolutePosn);
		while(fabs(dAbsolutePosn - dPrevAbsolutePosn) > 1.0)
		{
			dPrevAbsolutePosn = dAbsolutePosn;
			Sleep(100);
			get_abs_pos(idxMinAxis+3,&dAbsolutePosn);
		}
		reset_pos(idxMinAxis+3);

	}
}

void CMtnStepperDlg::OnBnClickedButton3()
{
	ULONG   ulErrCode;

	if(cFlagIsUsingAd1240) // 20120902
	{
		ulErrCode = (ULONG)P1240MotStop(uiCurrBoardId,        // Board ID
							   XYZU_Axis,           // Which axis to stop
							   0                    // Buffer ID
							   );
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
			return;
		}
	}
	else if(cFlagIsUsingLeeTro2812 == 1)
	{
		int idxMinAxis = uiCurrBoardId * 4 + 1;
		sudden_stop4(idxMinAxis, idxMinAxis+1, idxMinAxis+2, idxMinAxis+3);
	}
	

}

void CMtnStepperDlg::OnBnClickedCancel()
{
	StopThreadDlgStepper1240();
	EndDialog(IDCANCEL);	//OnCancel();
}

void CMtnStepperDlg::DownloadWithErrorPrompt()
{
	unsigned long ulErrCode;
	if(mtnstp_download_parameters_ads1240(uiCurrBoardId, &ulErrCode))
	{
		uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);
	}
	UploadSpeedProfileCurrBoard();
	UpdateSpeedProfileText();

}
static char *strStopString;
// IDC_EDIT_STEPPER_SV_X // IDC_EDIT_STEPPER_SV_Y // IDC_EDIT_STEPPER_SV_Z // IDC_EDIT_STEPPER_SV_U
void CMtnStepperDlg::OnEnChangeEditStepperSvX()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}
void CMtnStepperDlg::OnEnChangeEditStepperSvY()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}
void CMtnStepperDlg::OnEnChangeEditStepperSvZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}
void CMtnStepperDlg::OnEnChangeEditStepperSvU()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_DV_X // IDC_EDIT_STEPPER_DV_Y // IDC_EDIT_STEPPER_DV_Z // IDC_EDIT_STEPPER_DV_U
void CMtnStepperDlg::OnEnChangeEditStepperDvX()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperDvY()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
	//DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperDvZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperDvU()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_MDV_X // IDC_EDIT_STEPPER_MDV_Y // IDC_EDIT_STEPPER_MDV_Z // IDC_EDIT_STEPPER_MDV_U
void CMtnStepperDlg::OnEnChangeEditStepperMdvX()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperMdvY()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperMdvZ()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperMdvU()
{
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_AC_X // IDC_EDIT_STEPPER_AC_Y // IDC_EDIT_STEPPER_AC_Z // IDC_EDIT_STEPPER_AC_U
void CMtnStepperDlg::OnEnChangeEditStepperAcX()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperAcY()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperAcZ()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperAcU()
{
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_JERK_X // IDC_EDIT_STEPPER_JERK_Y // IDC_EDIT_STEPPER_JERK_Z // IDC_EDIT_STEPPER_JERK_U
void CMtnStepperDlg::OnEnChangeEditStepperJerkX()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperJerkY()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperJerkZ()
{
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperJerkU()
{
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_POS_LIMIT_X // IDC_EDIT_STEPPER_POS_LIMIT_Y // IDC_EDIT_STEPPER_POS_LIMIT_Z // IDC_EDIT_STEPPER_POS_LIMIT_U
void CMtnStepperDlg::OnEnChangeEditStepperPosLimitX()
{
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dPositiveLimit = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperPosLimitY()
{
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dPositiveLimit = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperPosLimitZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dPositiveLimit = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnChangeEditStepperPosLimitU()
{
	GetDlgItem(IDC_EDIT_STEPPER_POS_LIMIT_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dPositiveLimit = strtod(strTextTemp, &strStopString);
//	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_NEG_LIMIT_X // IDC_EDIT_STEPPER_NEG_LIMIT_Y // IDC_EDIT_STEPPER_NEG_LIMIT_Z // IDC_EDIT_STEPPER_NEG_LIMIT_U

void CMtnStepperDlg::OnBnClickedButtonStepperSaveProfile()
{
//	ULONG   ulErrCode;

	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_EXTDRIVE_MODE
void CMtnStepperDlg::OnEnChangeEditStepperExtdriveMode()
{
	GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_MODE)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%d", &ucStepperExtDriveMode);
	if(ucStepperExtDriveMode > 15)
	{
		ucStepperExtDriveMode = ucStepperExtDriveMode & 0xF;
		sprintf_s(strTextTemp, 128, "%d",ucStepperExtDriveMode);
		GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_MODE)->SetWindowTextA(_T(strTextTemp));
	}
}
// IDC_EDIT_STEPPER_EXTDRIVE_NUM_PULSE
void CMtnStepperDlg::OnEnChangeEditStepperExtdriveNumPulse()
{
	GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_NUM_PULSE)->GetWindowTextA( &strTextTemp[0], 128);
	sscanf_s(strTextTemp,  "%d", &ulStepperExtDriveNumPulse);
	if(ulStepperExtDriveNumPulse > 10000)
	{
		ulStepperExtDriveNumPulse = 10000;
		sprintf_s(strTextTemp, 128, "%d",ulStepperExtDriveNumPulse);
		GetDlgItem(IDC_EDIT_STEPPER_EXTDRIVE_NUM_PULSE)->SetWindowTextA(_T(strTextTemp));
	}
}

//IDC_BUTTON_STEPPER_SET_EXT_MODE, IDC_STATIC_STEPPER_EXT_MODE_SET_STATUS
void CMtnStepperDlg::OnBnClickedButtonStepperSetExtMode()
{
	ULONG   ulErrCode;
    BYTE	byDriveAxes = 0;

    if(bStepperSelX[uiCurrBoardId])
        byDriveAxes = byDriveAxes | X_Axis;

    if(bStepperSelY[uiCurrBoardId])
        byDriveAxes = byDriveAxes | Y_Axis;

    if(bStepperSelZ[uiCurrBoardId])
        byDriveAxes = byDriveAxes | Z_Axis;

    if(bStepperSelU[uiCurrBoardId])
        byDriveAxes = byDriveAxes | U_Axis;

	ulErrCode =  (ULONG) P1240MotExtMode(uiCurrBoardId, // 
									   byDriveAxes,
									  ucStepperExtDriveMode,
									  ulStepperExtDriveNumPulse);
    if( ulErrCode != 0)
	{
		uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
		return;
	}
	else
	{
		GetDlgItem(IDC_STATIC_STEPPER_EXT_MODE_SET_STATUS)->SetWindowTextA(_T(pstrStepperExtDriveMode[ucStepperExtDriveMode]));
	}

}

//////////////////
// IDC_EDIT_STEPPER_HOME_TYPE_X // IDC_EDIT_STEPPER_HOME_TYPE_Y // IDC_EDIT_STEPPER_HOME_TYPE_Z // IDC_EDIT_STEPPER_HOME_TYPE_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeTypeX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeType = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeTypeY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeType = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeTypeZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeType = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeTypeU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_TYPE_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeType = strtod(strTextTemp, &strStopString);
}

//////////////////
// IDC_EDIT_STEPPER_HOME_P0_VEL_X // IDC_EDIT_STEPPER_HOME_P0_VEL_Y // IDC_EDIT_STEPPER_HOME_P0_VEL_Z // IDC_EDIT_STEPPER_HOME_P0_VEL_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0VelU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_VEL_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Vel = strtod(strTextTemp, &strStopString);
}


//////////////////
// IDC_EDIT_STEPPER_HOME_P0_DIR_X // IDC_EDIT_STEPPER_HOME_P0_DIR_Y // IDC_EDIT_STEPPER_HOME_P0_DIR_Z // IDC_EDIT_STEPPER_HOME_P0_DIR_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP0_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP0_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP0_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP0DirU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P0_DIR_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP0_Dir = strtod(strTextTemp, &strStopString);
}

//////////////////
// IDC_EDIT_STEPPER_HOME_P1_VEL_X // IDC_EDIT_STEPPER_HOME_P1_VEL_Y // IDC_EDIT_STEPPER_HOME_P1_VEL_Z // IDC_EDIT_STEPPER_HOME_P1_VEL_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Vel = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1VelU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_VEL_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Vel = strtod(strTextTemp, &strStopString);
}

//////////////////
// IDC_EDIT_STEPPER_HOME_P1_DIR_X // IDC_EDIT_STEPPER_HOME_P1_DIR_Y // IDC_EDIT_STEPPER_HOME_P1_DIR_Z // IDC_EDIT_STEPPER_HOME_P1_DIR_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeP1_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeP1_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeP1_Dir = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeP1DirU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_P1_DIR_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeP1_Dir = strtod(strTextTemp, &strStopString);
}
//////////////////
// IDC_EDIT_STEPPER_HOME_OFFSET_X // IDC_EDIT_STEPPER_HOME_OFFSET_Y // IDC_EDIT_STEPPER_HOME_OFFSET_Z // IDC_EDIT_STEPPER_HOME_OFFSET_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeOffset = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeOffset = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeOffset = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeOffsetU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_OFFSET_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeOffset = strtod(strTextTemp, &strStopString);
}

//////////////////
// IDC_EDIT_STEPPER_HOME_MODE_X // IDC_EDIT_STEPPER_HOME_MODE_Y // IDC_EDIT_STEPPER_HOME_MODE_Z // IDC_EDIT_STEPPER_HOME_MODE_U
//////////////////
void CMtnStepperDlg::OnEnChangeEditStepperHomeModeX()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[0].dHomeMode = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeModeY()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[1].dHomeMode = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeModeZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[2].dHomeMode = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnChangeEditStepperHomeModeU()
{
	GetDlgItem(IDC_EDIT_STEPPER_HOME_MODE_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astPositionRegistration[3].dHomeMode = strtod(strTextTemp, &strStopString);
}
// IDC_CHECK_STEPPER1240_FLAG_ITERATIVE_MOVE
void CMtnStepperDlg::OnBnClickedCheckStepper1240FlagIterativeMove()
{
	iFlagIterativeMove = ((CButton *)GetDlgItem(IDC_CHECK_STEPPER1240_FLAG_ITERATIVE_MOVE))->GetCheck();
}
unsigned int CMtnStepperDlg::ReadUnsignedIntegerFromEdit(int nResId)
{
	static char tempChar[32];
	unsigned int uiValue;
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &uiValue);
	return uiValue;
}
void CMtnStepperDlg::UpdateUnsignedIntToEdit(int nResId, unsigned int uiValue)
{
	CString cstrTemp;
	cstrTemp.Format("%d", uiValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}
// IDC_EDIT_STEPPER1240_MOVE_ITERATION
void CMtnStepperDlg::OnEnKillfocusEditStepper1240MoveIteration()
{
	uiMoveTotalIteration = ReadUnsignedIntegerFromEdit(IDC_EDIT_STEPPER1240_MOVE_ITERATION);
	uiCurrentIteration = uiMoveTotalIteration; 
}
// IDC_EDIT_STEPPER_1240_ITERATIVE_MOVE_DELAY
void CMtnStepperDlg::OnEnKillfocusEditStepper1240IterativeMoveDelay()
{
	uiIterativeMoveDelay_ms = ReadUnsignedIntegerFromEdit(IDC_EDIT_STEPPER_1240_ITERATIVE_MOVE_DELAY);
}
UINT ThreadDlgStepper1240Proc( LPVOID pParam )
{
    CMtnStepperDlg* pObject = (CMtnStepperDlg *)pParam;
	return pObject->ThreadDlgStepper1240(); 	
}

void CMtnStepperDlg::RunThreadDlgStepper1240()
{
	m_fStopThreadDlgStepper1240 = FALSE;
	m_pWinThreadDlgStepper1240 = AfxBeginThread(ThreadDlgStepper1240Proc, this, THREAD_PRIORITY_NORMAL);
	m_pWinThreadDlgStepper1240->m_bAutoDelete = FALSE;
}

void CMtnStepperDlg::StopThreadDlgStepper1240()
{
	if (m_pWinThreadDlgStepper1240 != NULL)
	{
		m_fStopThreadDlgStepper1240 = TRUE;
		WaitForSingleObject(m_pWinThreadDlgStepper1240->m_hThread, 1500);
//		delete m_pWinThread;
		m_pWinThreadDlgStepper1240 = NULL;
	}
}
UINT CMtnStepperDlg::ThreadDlgStepper1240()
{
	ULONG   ulErrCode;
    BYTE	byDriveAxes = 0;

	if(m_fStopThreadDlgStepper1240 == FALSE)
	{

		if(cFlagIsUsingAd1240) // 20120902
		{
			if(bStepperSelX[uiCurrBoardId])
				byDriveAxes = byDriveAxes | X_Axis;

			if(bStepperSelY[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Y_Axis;

			if(bStepperSelZ[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Z_Axis;

			if(bStepperSelU[uiCurrBoardId])
				byDriveAxes = byDriveAxes | U_Axis;

			for(uiCurrentIteration = 0; uiCurrentIteration< uiMoveTotalIteration; uiCurrentIteration++)
			{
				Sleep(uiIterativeMoveDelay_ms);
				ulErrCode = (ULONG)P1240MotPtp(uiCurrBoardId, byDriveAxes, 0, 
					alMoveDistAtAxis[0], alMoveDistAtAxis[1], alMoveDistAtAxis[2], alMoveDistAtAxis[3]);
				if( ulErrCode != 0)
				{
					uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
					goto label_return_ThreadDlgStepper1240;
				}
				// 20110621
				ulErrCode = AxisDrvBusy;
				while(ulErrCode == AxisDrvBusy)
				{
					ulErrCode = (ULONG)P1240MotAxisBusy(uiCurrBoardId, byDriveAxes);
					Sleep(10);
				}// 20110621
				// Negative Dir
				Sleep(uiIterativeMoveDelay_ms);
				ulErrCode = (ULONG)P1240MotPtp(uiCurrBoardId, byDriveAxes, 0, 
					-alMoveDistAtAxis[0], -alMoveDistAtAxis[1], -alMoveDistAtAxis[2], -alMoveDistAtAxis[3]);
				if( ulErrCode != 0)
				{
					uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
					goto label_return_ThreadDlgStepper1240;
				}
				ulErrCode = AxisDrvBusy;  // 20110621
				while(ulErrCode == AxisDrvBusy)
				{
					ulErrCode = (ULONG)P1240MotAxisBusy(uiCurrBoardId, byDriveAxes);
					Sleep(10);
				} // 20110621
			}
		}
	}

label_return_ThreadDlgStepper1240:
	return 0;
}

ULONG mtnstp_set_clear_out_ads1240(unsigned int uiBoardId, BYTE uBytAxis, int iFlagEnableBit, 
								 unsigned int uiShiftBit, BYTE uBytAndClear, BYTE uBytOrSet)
{
ULONG   ulErrCode = MTN_API_OK_ZERO;
DWORD ulRegOut;
BYTE  uByteOutVal;
//ULONG ulRet = MTN_API_OK_ZERO;

		ulErrCode = (ULONG) P1240MotRdReg(uiBoardId, uBytAxis, WR3, &ulRegOut);
		if( ulErrCode != 0)
		{
//			MyShowErrorMsg(ulErrCode);
			return ulErrCode;
		}
		ulRegOut = (ulRegOut>>uiShiftBit) & 0x0f;
		uByteOutVal = (BYTE) ulRegOut;

		if(iFlagEnableBit)
		{
			uByteOutVal = uByteOutVal | uBytOrSet;
		}
		else
		{
			uByteOutVal = uByteOutVal & uBytAndClear;
		}

		ulErrCode = (ULONG)P1240MotDO(uiBoardId, uBytAxis, uByteOutVal);
		if (ulErrCode != ERROR_SUCCESS)
		{
//			iRet = MTN_API_ERROR;
//			MyShowErrorMsg(ulErrCode);
		}

	return ulErrCode;

}
#define PCI1240_HIGH_ACTIVE_OUT4 0x1
#define PCI1240_LOW_ACTIVE_OUT4  0xE
#define PCI1240_HIGH_ACTIVE_OUT5 0x2
#define PCI1240_LOW_ACTIVE_OUT5  0xD
#define PCI1240_HIGH_ACTIVE_OUT6 0x4
#define PCI1240_LOW_ACTIVE_OUT6  0xB
#define PCI1240_HIGH_ACTIVE_OUT7 0x8
#define PCI1240_LOW_ACTIVE_OUT7  0x7

// IDC_CHECK_PCI1240_OUT4
void CMtnStepperDlg::OnBnClickedCheckPci1240Out4()
{
ULONG   ulErrCode;
//DWORD ulRegOut;
//BYTE  uByteOutVal;
int iFlagEnableOut4; 

	if(cFlagIsUsingAd1240) // 20120902
	{
		iFlagEnableOut4 = ((CButton *) GetDlgItem(IDC_CHECK_PCI1240_OUT4))->GetCheck();

		// Channel-X
		if(bStepperSelX[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, X_Axis, iFlagEnableOut4, 8, PCI1240_LOW_ACTIVE_OUT4, PCI1240_HIGH_ACTIVE_OUT4);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}

		// Channel-Y
		if(bStepperSelY[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Y_Axis, iFlagEnableOut4, 8, PCI1240_LOW_ACTIVE_OUT4, PCI1240_HIGH_ACTIVE_OUT4);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}

		// Channel-Z
		if(bStepperSelZ[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Z_Axis, iFlagEnableOut4, 8, PCI1240_LOW_ACTIVE_OUT4, PCI1240_HIGH_ACTIVE_OUT4);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-U
		if(bStepperSelU[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, U_Axis, iFlagEnableOut4, 8, PCI1240_LOW_ACTIVE_OUT4, PCI1240_HIGH_ACTIVE_OUT4);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
	}
}

// IDC_CHECK_PCI1240_OUT5
void CMtnStepperDlg::OnBnClickedCheckPci1240Out5()
{
ULONG   ulErrCode;
int iFlagEnableOut5; 

	if(cFlagIsUsingAd1240) // 20120902
	{
		iFlagEnableOut5 = ((CButton *) GetDlgItem(IDC_CHECK_PCI1240_OUT5))->GetCheck();
		// Channel-X
		if(bStepperSelX[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, X_Axis, iFlagEnableOut5, 8, PCI1240_LOW_ACTIVE_OUT5, PCI1240_HIGH_ACTIVE_OUT5);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Y
		if(bStepperSelY[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Y_Axis, iFlagEnableOut5, 8, PCI1240_LOW_ACTIVE_OUT5, PCI1240_HIGH_ACTIVE_OUT5);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Z
		if(bStepperSelZ[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Z_Axis, iFlagEnableOut5, 8, PCI1240_LOW_ACTIVE_OUT5, PCI1240_HIGH_ACTIVE_OUT5);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-U
		if(bStepperSelU[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, U_Axis, iFlagEnableOut5, 8, PCI1240_LOW_ACTIVE_OUT5, PCI1240_HIGH_ACTIVE_OUT5);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
	}
}
// IDC_CHECK_PCI1240_OUT6
void CMtnStepperDlg::OnBnClickedCheckPci1240Out6()
{
ULONG   ulErrCode;
int iFlagEnableOut6; 

	if(cFlagIsUsingAd1240) // 20120902
	{
		iFlagEnableOut6 = ((CButton *) GetDlgItem(IDC_CHECK_PCI1240_OUT6))->GetCheck();
		// Channel-X
		if(bStepperSelX[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, X_Axis, iFlagEnableOut6, 8, PCI1240_LOW_ACTIVE_OUT6, PCI1240_HIGH_ACTIVE_OUT6);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Y
		if(bStepperSelY[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Y_Axis, iFlagEnableOut6, 8, PCI1240_LOW_ACTIVE_OUT6, PCI1240_HIGH_ACTIVE_OUT6);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Z
		if(bStepperSelZ[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Z_Axis, iFlagEnableOut6, 8, PCI1240_LOW_ACTIVE_OUT6, PCI1240_HIGH_ACTIVE_OUT6);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-U
		if(bStepperSelU[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, U_Axis, iFlagEnableOut6, 8, PCI1240_LOW_ACTIVE_OUT6, PCI1240_HIGH_ACTIVE_OUT6);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
	}
}
// IDC_CHECK_PCI1240_OUT7
void CMtnStepperDlg::OnBnClickedCheckPci1240Out7()
{
ULONG   ulErrCode;
int iFlagEnableOut7; 

	if(cFlagIsUsingAd1240) // 20120902
	{
		iFlagEnableOut7 = ((CButton *) GetDlgItem(IDC_CHECK_PCI1240_OUT7))->GetCheck();
		// Channel-X
		if(bStepperSelX[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, X_Axis, iFlagEnableOut7, 8, PCI1240_LOW_ACTIVE_OUT7, PCI1240_HIGH_ACTIVE_OUT7);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Y
		if(bStepperSelY[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Y_Axis, iFlagEnableOut7, 8, PCI1240_LOW_ACTIVE_OUT7, PCI1240_HIGH_ACTIVE_OUT7);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-Z
		if(bStepperSelZ[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, Z_Axis, iFlagEnableOut7, 8, PCI1240_LOW_ACTIVE_OUT7, PCI1240_HIGH_ACTIVE_OUT7);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
		// Channel-U
		if(bStepperSelU[uiCurrBoardId])
		{
			ulErrCode = mtnstp_set_clear_out_ads1240(uiCurrBoardId, U_Axis, iFlagEnableOut7, 8, PCI1240_LOW_ACTIVE_OUT7, PCI1240_HIGH_ACTIVE_OUT7);
			if( ulErrCode != 0)
			{
				MyShowErrorMsg(ulErrCode);
			}
		}
	}
}
#include "math.h"
// IDC_BUTTON_STEPPER_REVERSE_MOVE_DIST
void CMtnStepperDlg::OnBnClickedButtonStepperReverseMoveDist()
{
ULONG   ulErrCode;
BYTE byDriveAxes = 0;
long aiMoveNegDistAtAxis[MAX_STEPPER_AXIX_ON_BOARD];
double adMoveNegDistAtAxis[MAX_STEPPER_AXIX_ON_BOARD];

	for(int ii =0; ii<MAX_STEPPER_AXIX_ON_BOARD; ii++)
	{
		adMoveNegDistAtAxis[ii] = - fabs((double)alMoveDistAtAxis[ii]);
		aiMoveNegDistAtAxis[ii] = (long)(adMoveNegDistAtAxis[ii]);  // aiMoveNegDistAtAxis[ii]
	}

	if(cFlagIsUsingAd1240) // 20120902
	{
		if(bStepperSelX[uiCurrBoardId])
				byDriveAxes = byDriveAxes | X_Axis;

		if(bStepperSelY[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Y_Axis;

		if(bStepperSelZ[uiCurrBoardId])
				byDriveAxes = byDriveAxes | Z_Axis;

		if(bStepperSelU[uiCurrBoardId])
				byDriveAxes = byDriveAxes | U_Axis;

		ulErrCode = (ULONG)P1240MotPtp(uiCurrBoardId, byDriveAxes, 0, 
				aiMoveNegDistAtAxis[0], 
				aiMoveNegDistAtAxis[1], 
				aiMoveNegDistAtAxis[2], 
				aiMoveNegDistAtAxis[3]);
		if( ulErrCode != 0)
		{
			uiErrorLine = __LINE__; MyShowErrorMsg(ulErrCode);	
			return;
		}
	}
	else if(cFlagIsUsingLeeTro2812 == 1)
	{
		int idxMinAxis = uiCurrBoardId * 4 + 1;
		if(bStepperSelX[uiCurrBoardId])
		{
			fast_pmove(idxMinAxis, adMoveNegDistAtAxis[0]);
		}
		if(bStepperSelY[uiCurrBoardId])
		{
			fast_pmove(idxMinAxis + 1, adMoveNegDistAtAxis[1]);
		}
		if(bStepperSelZ[uiCurrBoardId])
		{
			fast_pmove(idxMinAxis + 2, adMoveNegDistAtAxis[2]);
		}
		if(bStepperSelU[uiCurrBoardId])
		{
			fast_pmove(idxMinAxis + 3, adMoveNegDistAtAxis[3]);
		}
	}
}
// IDC_BUTTON_STEPPER_TO_TARGET_POSN
void CMtnStepperDlg::OnBnClickedButtonStepperToTargetPosn()
{
	// TODO: Add your control notification handler code here
}

// IDC_EDIT_STEPPER_SV_X // IDC_EDIT_STEPPER_SV_Y // IDC_EDIT_STEPPER_SV_Z // IDC_EDIT_STEPPER_SV_U
void CMtnStepperDlg::OnEnKillfocusEditStepperSvX()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperSvY()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_Y)->GetWindowTextA( &strTextTemp[1], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperSvZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_Z)->GetWindowTextA( &strTextTemp[2], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperSvU()
{
	GetDlgItem(IDC_EDIT_STEPPER_SV_U)->GetWindowTextA( &strTextTemp[3], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperStartVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_DV_X // IDC_EDIT_STEPPER_DV_Y // IDC_EDIT_STEPPER_DV_Z // IDC_EDIT_STEPPER_DV_U
void CMtnStepperDlg::OnEnKillfocusEditStepperDvX()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperDvY()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_Y)->GetWindowTextA( &strTextTemp[1], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperDvZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_Z)->GetWindowTextA( &strTextTemp[2], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

void CMtnStepperDlg::OnEnKillfocusEditStepperDvU()
{
	GetDlgItem(IDC_EDIT_STEPPER_DV_U)->GetWindowTextA( &strTextTemp[3], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dStepperDriveVelocity = strtod(strTextTemp, &strStopString);
	DownloadWithErrorPrompt();
}

// IDC_EDIT_STEPPER_MDV_X // IDC_EDIT_STEPPER_MDV_Y // IDC_EDIT_STEPPER_MDV_Z // IDC_EDIT_STEPPER_MDV_U
void CMtnStepperDlg::OnEnKillfocusEditStepperMdvX()
{
	GetDlgItem(IDC_EDIT_STEPPER_MDV_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxVelocity = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperMdvY()
{
	GetDlgItem(IDC_EDIT_STEPPER_MDV_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxVelocity = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperMdvZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_MDV_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxVelocity = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperMdvU()
{
	GetDlgItem(IDC_EDIT_STEPPER_MDV_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxVelocity = strtod(strTextTemp, &strStopString);
}

// IDC_EDIT_STEPPER_AC_X // IDC_EDIT_STEPPER_AC_Y // IDC_EDIT_STEPPER_AC_Z // IDC_EDIT_STEPPER_AC_U
void CMtnStepperDlg::OnEnKillfocusEditStepperAcX()
{
	GetDlgItem(IDC_EDIT_STEPPER_AC_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxAcceleration = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperAcY()
{
	GetDlgItem(IDC_EDIT_STEPPER_AC_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxAcceleration = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperAcZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_AC_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxAcceleration = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperAcU()
{
	GetDlgItem(IDC_EDIT_STEPPER_AC_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxAcceleration = strtod(strTextTemp, &strStopString);
}

// IDC_EDIT_STEPPER_JERK_X // IDC_EDIT_STEPPER_JERK_Y // IDC_EDIT_STEPPER_JERK_Z // IDC_EDIT_STEPPER_JERK_U
void CMtnStepperDlg::OnEnKillfocusEditStepperJerkX()
{
	GetDlgItem(IDC_EDIT_STEPPER_JERK_X)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[0].dMaxJerk = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperJerkY()
{
	GetDlgItem(IDC_EDIT_STEPPER_JERK_Y)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[1].dMaxJerk = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperJerkZ()
{
	GetDlgItem(IDC_EDIT_STEPPER_JERK_Z)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[2].dMaxJerk = strtod(strTextTemp, &strStopString);
}

void CMtnStepperDlg::OnEnKillfocusEditStepperJerkU()
{
	GetDlgItem(IDC_EDIT_STEPPER_JERK_U)->GetWindowTextA( &strTextTemp[0], 128);
	astStepperSpeedProfilePerBoard[uiCurrBoardId].astSpeedProfileStepperAxis[3].dMaxJerk = strtod(strTextTemp, &strStopString);
}
