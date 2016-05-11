// ProfCalcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProfCalcDlg.h"
#include "MtnApi.h"

static double dDesDist = 10000;
static double dDesireMaxVel = 1E6;
static double dDesireMaxAcc = 100;
static double dDesireMaxJerk = 10;
static double dDesireInitVel = 100;
static double dDesireEndVel;
static double dDesireInitAcc;
static double dDesireEndAcc;
static double dDesireInitJerk;
static double dDesireEndJerk;

static double dActualDist;
static double dActualMaxVel;
static double dActualMaxAcc;
static double dActualMaxJerk;
static double dActualInitVel;
static double dActualInitAcc;
static double dActualInitJerk;
static double dActualEndVel;
static double dActualEndAcc;
static double dActualEndJerk;

static double dActualTimeAcc;
static double dActualTimeCnstV;
static double dActualTimeDec;

// Variables to Execute Moving
static unsigned int uiTotalNumProfileMove = 1;
static unsigned int uiCurrIterProfileMove;
static HANDLE stCtrlCardHandle;				// communication handle
static unsigned int uiMotionInterDelay_ms = 20;
static int iAxisCtrlCardProfCalcuTest = 0; // By default ACS-X
static int iFlagSaveScopeDataInFile;

// Temp String, char
static CString cstrTextUI;
static CString cstrTemp;
static char strTextTemp[128];
static char *strStopString;  // for reading double strtod

static char strFilenameOutProfile[128];
static BOOL bFlagSaveFile = 1;

static double *pdDistProf, *pdVelProf, *pdAccProf, *pdJerkProf;
#define DEF_PROFILE_LENGTH   10000
static double adDistProf[DEF_PROFILE_LENGTH];
static double adVelProf[DEF_PROFILE_LENGTH];
static double adAccProf[DEF_PROFILE_LENGTH];
static double adJerkProf[DEF_PROFILE_LENGTH];

// For SuperSine (OfstSine)
//double *pfProfJerk, *pfProfAcc, *pfProfVel, *pfProfDist;
double pfProfJerk[DEF_PROFILE_LENGTH], pfProfAcc[DEF_PROFILE_LENGTH], pfProfVel[DEF_PROFILE_LENGTH], pfProfDist[DEF_PROFILE_LENGTH];

extern COMM_SETTINGS stServoControllerCommSet;

// CProfCalcDlg dialog
IMPLEMENT_DYNAMIC(CProfCalcDlg, CDialog)

CProfCalcDlg::CProfCalcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProfCalcDlg::IDD, pParent)
{

}

CProfCalcDlg::~CProfCalcDlg()
{
}

void CProfCalcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CProfCalcDlg::UpdateUShortToEdit(int nResId, unsigned short usValue)
{
	static CString cstrTemp;
	cstrTemp.Format("%d", usValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp);
}

void CProfCalcDlg::UpdateIntToEdit(int nResId, int iValue)
{
	static CString cstrTemp;
	cstrTemp.Format("%d", iValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp);
}

void CProfCalcDlg::InitProfCalcDlgUI()
{
	CComboBox *pSelectMoveCtrlAxisCombo = (CComboBox*) GetDlgItem(IDC_PROF_CALC_TESTER_COMBO_CTRL_AXIS);
	pSelectMoveCtrlAxisCombo->InsertString(0,"X");
	pSelectMoveCtrlAxisCombo->InsertString(1,"Y");
	pSelectMoveCtrlAxisCombo->InsertString(2,"Z");
	pSelectMoveCtrlAxisCombo->InsertString(3,"T");
	pSelectMoveCtrlAxisCombo->InsertString(4,"A");
	pSelectMoveCtrlAxisCombo->InsertString(5,"B");
	pSelectMoveCtrlAxisCombo->InsertString(6,"C");
	pSelectMoveCtrlAxisCombo->InsertString(7,"D");
	pSelectMoveCtrlAxisCombo->InsertString(MAX_CTRL_AXIS_PER_SERVO_BOARD,"Dummy");
	pSelectMoveCtrlAxisCombo->SetCurSel(iAxisCtrlCardProfCalcuTest);

	UpdateIntToEdit(IDC_EDIT_PROF_CALC_TEST_MOVE_ITER, (int)uiTotalNumProfileMove);

	((CButton*) GetDlgItem(IDC_PROF_CALC_TEST_SAVE_SCOPE_DATA_CHECK))->SetCheck(iFlagSaveScopeDataInFile);

}

BEGIN_MESSAGE_MAP(CProfCalcDlg, CDialog)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_PROF_CALC_DES_DIST, &CProfCalcDlg::OnEnChangeProfCalcDesDist)
	ON_BN_CLICKED(IDC_UTURN_HALF_TWIN_SINE_BTN, &CProfCalcDlg::OnBnClickedUturnHalfTwinSineBtn)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_MAX_VEL, &CProfCalcDlg::OnEnChangeProfCalcDesMaxVel)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_MAX_ACC, &CProfCalcDlg::OnEnChangeProfCalcDesMaxAcc)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_MAX_JERK, &CProfCalcDlg::OnEnChangeProfCalcDesMaxJerk)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_INIT_VEL, &CProfCalcDlg::OnEnChangeProfCalcDesInitVel)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_END_VEL, &CProfCalcDlg::OnEnChangeProfCalcDesEndVel)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_INIT_ACC, &CProfCalcDlg::OnEnChangeProfCalcDesInitAcc)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_END_ACC, &CProfCalcDlg::OnEnChangeProfCalcDesEndAcc)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_INIT_JERK, &CProfCalcDlg::OnEnChangeProfCalcDesInitJerk)
	ON_EN_CHANGE(IDC_PROF_CALC_DES_END_JERK, &CProfCalcDlg::OnEnChangeProfCalcDesEndJerk)
	ON_BN_CLICKED(IDC_PROF_CALC_DLG_SAVE_PROF_FILE_CHECK, &CProfCalcDlg::OnBnClickedProfCalcDlgSaveProfFileCheck)
	ON_BN_CLICKED(IDC_OFST_SINE_BTN, &CProfCalcDlg::OnBnClickedOfstSineBtn)
	ON_CBN_SELCHANGE(IDC_PROF_CALC_TESTER_COMBO_CTRL_AXIS, &CProfCalcDlg::OnCbnSelchangeProfCalcTesterComboCtrlAxis)
	ON_EN_CHANGE(IDC_EDIT_PROF_CALC_TEST_MOVE_ITER, &CProfCalcDlg::OnEnChangeEditProfCalcTestMoveIter)
	ON_BN_CLICKED(IDC_BUTTON_PROF_CALC_TEST_MOVE_START, &CProfCalcDlg::OnBnClickedButtonProfCalcTestMoveStart)
	ON_BN_CLICKED(IDCANCEL, &CProfCalcDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_PROF_CALC_TEST_SAVE_SCOPE_DATA_CHECK, &CProfCalcDlg::OnBnClickedProfCalcTestSaveScopeDataCheck)
	ON_BN_CLICKED(IDC_POLY_SAM1_1234_BTN, &CProfCalcDlg::OnBnClickedPolySam11234Btn)
END_MESSAGE_MAP()

#include "MtnTesterResDef.h"

BOOL CProfCalcDlg::OnInitDialog()
{
	BOOL bRet = CDialog::OnInitDialog(); 

#ifdef __USE_STATIC_COMMON_ARRAY__
	pfProfJerk = &adJerkProf[0];
	pfProfAcc = &adAccProf[0];
	pfProfVel = &adVelProf[0];
	pfProfDist = &adDistProf[0];
#endif // __USE_STATIC_COMMON_ARRAY__

	ProfCalcDlg_UpdateDesiredUIFromLocalVar();

	stCtrlCardHandle = stServoControllerCommSet.Handle;

	InitProfCalcDlgUI();

	// Timer and Thread
	StartTimer(IDT_PROF_CALC_TEST_DLG_TIMER, 200);
	m_pWinThread = NULL; m_fStopTestMoveThread = TRUE;

	return bRet;
}
void CProfCalcDlg::ShowWindow(int nCmdShow)
{
	StartTimer(IDT_PROF_CALC_TEST_DLG_TIMER, 200);
	m_pWinThread = NULL; m_fStopTestMoveThread = TRUE;
	CDialog::ShowWindow(nCmdShow);
}
void CProfCalcDlg::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}

void CProfCalcDlg::ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

void CProfCalcDlg::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[128];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 128);
	*pdValue = strtod(tempChar, &strStopString);
}

// IDC_PROF_CALC_DES_DIST
// IDC_PROF_CALC_DES_MAX_VEL
// IDC_PROF_CALC_DES_MAX_ACC
// IDC_PROF_CALC_DES_MAX_JERK
// IDC_PROF_CALC_DES_INIT_VEL
// IDC_PROF_CALC_DES_END_VEL
// IDC_PROF_CALC_DES_INIT_ACC
// IDC_PROF_CALC_DES_END_ACC
// IDC_PROF_CALC_DES_INIT_JERK
// IDC_PROF_CALC_DES_END_JERK

// IDC_PROF_CALC_ACT_DIST
// IDC_PROF_CALC_ACT_MAX_VEL
// IDC_PROF_CALC_ACT_MAX_ACC
// IDC_PROF_CALC_ACT_MAX_JERK
// IDC_PROF_CALC_ACT_INIT_VEL
// IDC_PROF_CALC_ACT_END_VEL
// IDC_PROF_CALC_ACT_INIT_ACC
// IDC_PROF_CALC_ACT_END_ACC
// IDC_PROF_CALC_ACT_INIT_JERK
// IDC_PROF_CALC_ACT_END_JERK

// IDC_PROF_CALC_ACT_T_ACC
// IDC_PROF_CALC_ACT_T_CNST_VEL
// IDC_PROF_CALC_ACT_T_DEC

// CProfCalcDlg message handlers
void CProfCalcDlg::ProfCalcDlg_UpdateDesiredUIFromLocalVar()
{
	// Desired value
	cstrTextUI.Format("%8.0f", dDesDist);
	GetDlgItem(IDC_PROF_CALC_DES_DIST)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireMaxVel);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireMaxAcc);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireMaxJerk);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_JERK)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireInitVel);
	GetDlgItem(IDC_PROF_CALC_DES_INIT_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireEndVel);
	GetDlgItem(IDC_PROF_CALC_DES_END_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireInitAcc);
	GetDlgItem(IDC_PROF_CALC_DES_INIT_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireEndAcc);
	GetDlgItem(IDC_PROF_CALC_DES_END_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireInitJerk);
	GetDlgItem(IDC_PROF_CALC_DES_INIT_JERK)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dDesireEndJerk);
	GetDlgItem(IDC_PROF_CALC_DES_END_JERK)->SetWindowTextA(cstrTextUI);

	((CButton *)GetDlgItem(IDC_PROF_CALC_DLG_SAVE_PROF_FILE_CHECK))->SetCheck(bFlagSaveFile);
}
void CProfCalcDlg::OnEnChangeProfCalcDesDist()
{
	//GetDlgItem(IDC_PROF_CALC_DES_DIST)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesDist = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_DIST, &dDesDist);
}

void CProfCalcDlg::OnEnChangeProfCalcDesMaxVel()
{
	//GetDlgItem(IDC_PROF_CALC_DES_MAX_VEL)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireMaxVel = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_MAX_VEL, &dDesireMaxVel);
}

void CProfCalcDlg::OnEnChangeProfCalcDesMaxAcc()
{
	//GetDlgItem(IDC_PROF_CALC_DES_MAX_ACC)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireMaxAcc = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_MAX_ACC, &dDesireMaxAcc);
}

void CProfCalcDlg::OnEnChangeProfCalcDesMaxJerk()
{
	//GetDlgItem(IDC_PROF_CALC_DES_MAX_JERK)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireMaxJerk = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_MAX_JERK, &dDesireMaxJerk);
}

void CProfCalcDlg::OnEnChangeProfCalcDesInitVel()
{
	//GetDlgItem(IDC_PROF_CALC_DES_INIT_VEL)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireInitVel = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_INIT_VEL, &dDesireInitVel);
}

void CProfCalcDlg::OnEnChangeProfCalcDesEndVel()
{
	//GetDlgItem(IDC_PROF_CALC_DES_END_VEL)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireEndVel = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_END_VEL, &dDesireEndVel);
}

void CProfCalcDlg::OnEnChangeProfCalcDesInitAcc()
{
	//GetDlgItem(IDC_PROF_CALC_DES_INIT_ACC)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireInitAcc = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_INIT_ACC, &dDesireInitAcc);
}

void CProfCalcDlg::OnEnChangeProfCalcDesEndAcc()
{
	//GetDlgItem(IDC_PROF_CALC_DES_END_ACC)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireEndAcc = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_END_ACC, &dDesireEndAcc);
}

void CProfCalcDlg::OnEnChangeProfCalcDesInitJerk()
{
	//GetDlgItem(IDC_PROF_CALC_DES_INIT_JERK)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireInitJerk = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_INIT_JERK, &dDesireInitJerk);
}

void CProfCalcDlg::OnEnChangeProfCalcDesEndJerk()
{
	//GetDlgItem(IDC_PROF_CALC_DES_END_JERK)->GetWindowTextA( &strTextTemp[0], 128);
	//dDesireEndJerk = strtod(strTextTemp, &strStopString);
	ReadDoubleFromEdit(IDC_PROF_CALC_DES_END_JERK, &dDesireEndJerk);
}
void CProfCalcDlg::ProfCalcDlg_UpdateActualUIFromLocalVar()
{
	// Actual value
	cstrTextUI.Format("%8.0f", dActualDist);
	GetDlgItem(IDC_PROF_CALC_ACT_DIST)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualMaxVel);
	GetDlgItem(IDC_PROF_CALC_ACT_MAX_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualMaxAcc);
	GetDlgItem(IDC_PROF_CALC_ACT_MAX_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualMaxJerk);
	GetDlgItem(IDC_PROF_CALC_ACT_MAX_JERK)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualInitVel);
	GetDlgItem(IDC_PROF_CALC_ACT_INIT_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualEndVel);
	GetDlgItem(IDC_PROF_CALC_ACT_END_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualInitAcc);
	GetDlgItem(IDC_PROF_CALC_ACT_INIT_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualEndAcc);
	GetDlgItem(IDC_PROF_CALC_ACT_END_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualInitJerk);
	GetDlgItem(IDC_PROF_CALC_ACT_INIT_JERK)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualEndJerk);
	GetDlgItem(IDC_PROF_CALC_ACT_END_JERK)->SetWindowTextA(cstrTextUI);

	cstrTextUI.Format("%8.2f", dActualTimeAcc);
	GetDlgItem(IDC_PROF_CALC_ACT_T_ACC)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualTimeCnstV);
	GetDlgItem(IDC_PROF_CALC_ACT_T_CNST_VEL)->SetWindowTextA(cstrTextUI);
	cstrTextUI.Format("%8.2f", dActualTimeDec);
	GetDlgItem(IDC_PROF_CALC_ACT_T_DEC)->SetWindowTextA(cstrTextUI);

}
#define FILENAME_SAVE_PROFILE "ProfDVAJ_Uturn_HalfTwinSine.txt"

#include "uturn_half_twinsine.h"
void CProfCalcDlg::OnBnClickedUturnHalfTwinSineBtn()
{
	U_TURN_HALF_TWIN_SINE_PRECAL_INPUT stUturnHalfTwinSinePrecalInput;
	U_TURN_HALF_TWIN_SINE_PRECAL_OUTPUT stUturnHalfTwinSinePrecalOutputProfGenInput;
	U_TURN_HALF_TWIN_SINE_PROFGEN_OUTPUT stUturnHalfTwinSineProfGenOutput;

	stUturnHalfTwinSinePrecalInput.dDist_end = dDesDist;
	stUturnHalfTwinSinePrecalInput.dVini = dDesireInitVel;
	stUturnHalfTwinSinePrecalInput.dVend = dDesireEndVel;
	stUturnHalfTwinSinePrecalInput.dAmax = dDesireMaxAcc;
	stUturnHalfTwinSinePrecalInput.dJmax = dDesireMaxJerk;

	axis_u_turn_half_ts_precal(&stUturnHalfTwinSinePrecalInput, &stUturnHalfTwinSinePrecalOutputProfGenInput);

	dActualDist = stUturnHalfTwinSinePrecalOutputProfGenInput.Dist;
	dActualMaxVel = stUturnHalfTwinSinePrecalOutputProfGenInput.ActVmax;
	dActualMaxAcc = stUturnHalfTwinSinePrecalOutputProfGenInput.ActualAccMax;
	dActualMaxJerk = stUturnHalfTwinSinePrecalOutputProfGenInput.ActualJerkMax;
	dActualInitVel = stUturnHalfTwinSinePrecalOutputProfGenInput.Vini;
	dActualEndVel = stUturnHalfTwinSinePrecalOutputProfGenInput.Vend;

	dActualTimeAcc = stUturnHalfTwinSinePrecalOutputProfGenInput.T_acc;
	dActualTimeCnstV = stUturnHalfTwinSinePrecalOutputProfGenInput.T_cnst;
	dActualTimeDec = stUturnHalfTwinSinePrecalOutputProfGenInput.T_dec;

	BOOL bAllocDynMemory;

	unsigned int uiTotalTime = (unsigned int)(stUturnHalfTwinSinePrecalOutputProfGenInput.T_total + 1);
	if(uiTotalTime <= DEF_PROFILE_LENGTH)
	{
		pdDistProf = adDistProf;
		pdVelProf = adVelProf;
		pdAccProf = adAccProf;
		pdJerkProf = adJerkProf;
		bAllocDynMemory = FALSE;
	}
	else
	{
		pdDistProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdVelProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdAccProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdJerkProf = (double*)calloc(uiTotalTime, sizeof(double));
		bAllocDynMemory = TRUE;
	}
	stUturnHalfTwinSineProfGenOutput.pdDist = pdDistProf;
	stUturnHalfTwinSineProfGenOutput.pdVel = pdVelProf;
	stUturnHalfTwinSineProfGenOutput.pdAcc = pdAccProf;
	stUturnHalfTwinSineProfGenOutput.pdJerk = pdJerkProf;

	axis_u_turn_half_ts_profgen(&stUturnHalfTwinSinePrecalOutputProfGenInput, 
		&stUturnHalfTwinSineProfGenOutput);

	dActualInitAcc = pdAccProf[0]; dActualEndAcc = pdAccProf[uiTotalTime]; 
	dActualInitJerk = pdJerkProf[0]; dActualEndJerk = pdJerkProf[uiTotalTime];
	ProfCalcDlg_UpdateActualUIFromLocalVar();

	if(bFlagSaveFile == TRUE)
	{
		sprintf_s(strFilenameOutProfile, 128, "%s", FILENAME_SAVE_PROFILE);
		ProfCalcDlg_SaveProfileToFile();
	}

	if( bAllocDynMemory == TRUE)
	{
		free(pdDistProf); free(pdVelProf); free(pdAccProf); free(pdJerkProf);
	}

}

// IDC_PROF_CALC_DLG_SAVE_PROF_FILE_CHECK
void CProfCalcDlg::OnBnClickedProfCalcDlgSaveProfFileCheck()
{
	bFlagSaveFile = ((CButton *)GetDlgItem(IDC_PROF_CALC_DLG_SAVE_PROF_FILE_CHECK))->GetCheck();
}

void CProfCalcDlg::ProfCalcDlg_SaveProfileToFile()
{

FILE *fptrDebug;
unsigned int uiTotalTime = (unsigned int)(dActualTimeAcc + dActualTimeCnstV + dActualTimeDec + 1);

		fopen_s(&fptrDebug, strFilenameOutProfile, "w");

		fprintf(fptrDebug, "%% Motion planning input, desired values and system limits\n");
		fprintf(fptrDebug, "fDesiredDist = %8.1f; \n", dDesDist);
		fprintf(fptrDebug, "fDesiredVmax = %8.4f; \n", dDesireMaxVel);
		fprintf(fptrDebug, "fDesiredAmax = %8.4f; \n", dDesireMaxAcc);
		fprintf(fptrDebug, "fDesiredJmax = %8.4f; \n", dDesireMaxJerk);
		fprintf(fptrDebug, "fDesiredVini = %8.4f; \n", dDesireInitVel);
		fprintf(fptrDebug, "fDesiredVend = %8.4f; \n", dDesireEndVel);
		fprintf(fptrDebug, "fDesiredAini = %8.4f; \n", dDesireInitAcc);
		fprintf(fptrDebug, "fDesiredAend = %8.4f; \n", dDesireEndAcc);
		fprintf(fptrDebug, "fDesiredJini = %8.4f; \n", dDesireInitJerk);
		fprintf(fptrDebug, "fDesiredJend = %8.4f; \n", dDesireEndJerk);

		fprintf(fptrDebug, "\n%% Motion planning output, actual values\n");
		fprintf(fptrDebug, "fActualDist = %8.1f; \n", dActualDist);
		fprintf(fptrDebug, "fActualVmax = %8.4f; \n", dActualMaxVel);
		fprintf(fptrDebug, "fActualAmax = %8.4f; \n", dActualMaxAcc);
		fprintf(fptrDebug, "fActualJmax = %8.4f; \n", dActualMaxJerk);
		fprintf(fptrDebug, "fActualVini = %8.4f; \n", dActualInitVel);
		fprintf(fptrDebug, "fActualVend = %8.4f; \n", dActualEndVel);
		fprintf(fptrDebug, "fActualAini = %8.4f; \n", dActualInitAcc);
		fprintf(fptrDebug, "fActualAend = %8.4f; \n", dActualEndAcc);
		fprintf(fptrDebug, "fActualJini = %8.4f; \n", dActualInitJerk);
		fprintf(fptrDebug, "fActualJend = %8.4f; \n", dActualEndJerk);

		fprintf(fptrDebug, "fActualTacc = %8.4f; \n", dActualTimeAcc);
		fprintf(fptrDebug, "fActualTcnst = %8.4f; \n", dActualTimeCnstV);
		fprintf(fptrDebug, "fActualTdec = %8.4f; \n", dActualTimeDec);

		fprintf(fptrDebug, "%%%% Profile Count, Dist, Vel, Acc, Jerk\n");

		fprintf(fptrDebug, "matCalculateProfile = [ ");
		for(unsigned int ii = 0; ii<uiTotalTime; ii ++)
		{
			if(ii == uiTotalTime - 1)
			{
				fprintf(fptrDebug, "%d, %8.2f,  %8.5f, %8.5f, %8.5f", 
					ii,
					pdDistProf[ii],
					pdVelProf[ii],
					pdAccProf[ii],
					pdJerkProf[ii]);
			}
			else
			{
				fprintf(fptrDebug, "%d, %8.2f,  %8.5f, %8.5f, %8.5f \n", 
					ii,
					pdDistProf[ii],
					pdVelProf[ii],
					pdAccProf[ii],
					pdJerkProf[ii]);

			}
		}
		fprintf(fptrDebug, "];\n\n");

		fclose(fptrDebug);

}

#include "MtnSsPrf.h"
#include "acs_buff_prog.h"
#include "MtnInitAcs.h"

void CProfCalcDlg::OnBnClickedOfstSineBtn()
{
	CFileDialog mcFileDlg(TRUE);
	char strProfSsFile[260];
	CString cstrFilename;
	unsigned short usTotalBlk;
	short sDebugFlag = 0;
	unsigned short usTotalMotionSample;
	if(mcFileDlg.DoModal() == IDOK)
	{
//		cstrFilename = mcFileDlg.GetFolderPath().GetString() + mcFileDlg.GetFileName().GetString();
//		cstrFilename.Append();
		CString cstrFileName = mcFileDlg.GetFileName();
		CString cstrFullPathName = mcFileDlg.GetPathName(); // GetFolderPath();

		sprintf_s(strProfSsFile, 128, "%s", cstrFullPathName.GetString()); // cstrFileName.GetString()
		//cstrFilename.GetString()); // mcFileDlg.m_szFileName
	}
	__init_ss_prof_blk_from_file__(strProfSsFile, &usTotalBlk);
	__init_continuous_ss_prof_cmd__(usTotalBlk, sDebugFlag, &usTotalMotionSample);
	// Update UI
	cstrTemp.Format("TotalBlk: %d, TotalSample: %d", usTotalBlk, usTotalMotionSample);
	GetDlgItem(IDC_STATIC_PROFILE_CALC_OUTPUT)->SetWindowTextA(cstrTemp);

	// Download profile
	if(acs_bufprog_download_vel_prof_buff_8(iAxisCtrlCardProfCalcuTest, 
		usTotalMotionSample, pfProfVel) != MTN_API_OK_ZERO)
	{
		sprintf_s(strTextTemp, 128, "download trajectory error, Error Code: %d", acsc_GetLastError());
		AfxMessageBox(_T(strTextTemp));
	}
	acs_bufprog_write_traj_len_buff_8(usTotalMotionSample); // acsc_WriteDPRAMInteger(Handle, DPRAM_ADDR_PATH_LEN_BUF_8, nTrajSize);
	acs_bufprog_write_motion_flag_buff_8(ACS_BUFPROG_MTN_FLAG_CLOSE_LOOP); // either ProfMove in CloseLoop or PWN in OpenLoop
	// The command generation time should be <= CTIME
	double dControllerSampleTime = sys_get_controller_ts();
	acs_bufprog_write_traj_move_time_unit_ms(dControllerSampleTime); // 1 ms command // 20110426

}

void CProfCalcDlg::OnCbnSelchangeProfCalcTesterComboCtrlAxis()
{
	iAxisCtrlCardProfCalcuTest = ((CComboBox*) GetDlgItem(IDC_PROF_CALC_TESTER_COMBO_CTRL_AXIS))->GetCurSel();
}

// IDC_EDIT_PROF_CALC_TEST_MOVE_ITER
void CProfCalcDlg::OnEnChangeEditProfCalcTestMoveIter()
{
	ReadUnsignedIntegerFromEdit(IDC_EDIT_PROF_CALC_TEST_MOVE_ITER, &uiTotalNumProfileMove);
}

void CProfCalcDlg::EnableDlgUserInput(BOOL bFlagEnable)
{
//	((CButton *) GetDlgItem(IDC_BUTTON_PROF_CALC_TEST_MOVE_START))->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_EDIT_PROF_CALC_TEST_MOVE_ITER)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_DIST)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_VEL)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_ACC)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_MAX_JERK)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_INIT_VEL)->EnableWindow(bFlagEnable);
	GetDlgItem(IDC_PROF_CALC_DES_END_VEL)->EnableWindow(bFlagEnable);
}
// IDC_BUTTON_PROF_CALC_TEST_MOVE_START
void CProfCalcDlg::OnBnClickedButtonProfCalcTestMoveStart()
{
	//EnableDlgUserInput(FALSE);

	if(m_fStopTestMoveThread) // NOT started any thread
	{
		// 20110426
		acs_run_buffer_prog_prbs_prof_cfg_move();
//		acs_run_buffer_prog_vel_loop_prof_cfg_move2();
		Sleep(100);  		// 20110426

		RunTestMoveThread();
		GetDlgItem(IDC_BUTTON_PROF_CALC_TEST_MOVE_START)->SetWindowTextA(_T("Stop"));
	}
	else
	{
		StopTestMoveThread();
		GetDlgItem(IDC_BUTTON_PROF_CALC_TEST_MOVE_START)->SetWindowTextA(_T("Start"));

	}
}

UINT ProfCalcDlg_TestMoveThreadProc( LPVOID pParam )
{
    CProfCalcDlg* pObject = (CProfCalcDlg *)pParam;
	return pObject->TestMoveThread(); 	
}

void CProfCalcDlg::RunTestMoveThread()
{
	m_fStopTestMoveThread = FALSE;
	m_pWinThread = AfxBeginThread(ProfCalcDlg_TestMoveThreadProc, this, THREAD_PRIORITY_NORMAL);
	m_pWinThread->m_bAutoDelete = FALSE;
}

void CProfCalcDlg::StopTestMoveThread()
{
	if (m_pWinThread)
	{
		m_fStopTestMoveThread = TRUE;
		WaitForSingleObject(m_pWinThread->m_hThread, 2000);
//		delete m_pWinThread;
		m_pWinThread = NULL;
	}
}

UINT CProfCalcDlg::TestMoveThread()
{
	static int iFlagDoingTest;
	if(!m_fStopTestMoveThread)
	{
		for(unsigned int ii=0; ii<uiTotalNumProfileMove; ii++)
		{
			uiCurrIterProfileMove = ii;
			acsc_ReadInteger(stCtrlCardHandle, BUFFER_ID_AFT_SPECTRUM_TEST, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);
			while(iFlagDoingTest)
			{
       			Sleep(5);
				acsc_ReadInteger(stCtrlCardHandle, BUFFER_ID_AFT_SPECTRUM_TEST, "iFlagRunningBufferProg", 0, 0, 0, 0, &iFlagDoingTest, NULL);

				if(mtn_qc_is_axis_locked_safe(stCtrlCardHandle, iAxisCtrlCardProfCalcuTest) == FALSE)		break;
			}

			// Capture data

			if((uiCurrIterProfileMove%2 == 0))
			{
				// Setup scope for motionn
				mtnscope_set_acsc_var_collecting_move(iAxisCtrlCardProfCalcuTest);
				// Start collection
				if (!acsc_CollectB(stCtrlCardHandle, 0, // system data collection
					gstrScopeArrayName, // name of data collection array
					gstSystemScope.uiDataLen, // number of samples to be collected
					1, // sampling period 1 millisecond
					strACSC_VarName, // variables to be collected
					NULL)
					)
				{
					cstrTemp.Format("Collecting data, transaction error: %d", acsc_GetLastError());
					AfxMessageBox(cstrTemp);					break;
				}
			}
			Sleep(uiMotionInterDelay_ms);
			acs_bufprog_start_buff_8();
			if(m_fStopTestMoveThread) break;
		}
	}

	if(iFlagSaveScopeDataInFile)
	{
		mtnscope_upload_acsc_data(stCtrlCardHandle);
		mtn_save_curr_axis_traj_data_to_file(iAxisCtrlCardProfCalcuTest);
	}

	EnableDlgUserInput(TRUE);
	m_fStopTestMoveThread = TRUE;

	//Sleep(100);  		// 20110426
	//acs_stop_buffer_prog_prbs_prof_cfg_move();
	//acs_compile_buffer_prog_prbs_prof_cfg_move();

//	acs_stop_buffer_prog_vel_loop_prof_cfg_move2();
//	acs_compile_buffer_prog_vel_loop_prof_cfg_move2();  // 20110426

	return 0;
}


//	
static UINT_PTR m_iTimerVal;

///// Timer related
UINT_PTR  CProfCalcDlg::StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration)
{ // IDT_SPEC_TEST_DLG_TIMER
	m_iTimerVal = SetTimer(nIDT_TimerResMacro, TimerDuration, 0);
	
	if (m_iTimerVal == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}
    return m_iTimerVal;
} 
BOOL  CProfCalcDlg::StopTimer(UINT_PTR nTimerVal)
{
	if (!KillTimer (nTimerVal))
	{
		return FALSE;
	}
	return TRUE;
}
void CProfCalcDlg::OnTimer(UINT nTimerVal)
{
	if(!m_fStopTestMoveThread)
	{
		// Update UI
		cstrTemp.Format("Curr/Total Iter: %d /%d", uiCurrIterProfileMove, uiTotalNumProfileMove);
		GetDlgItem(IDC_STATIC_PROFILE_CALC_OUTPUT)->SetWindowTextA(cstrTemp);		
	}
	else
	{
		GetDlgItem(IDC_BUTTON_PROF_CALC_TEST_MOVE_START)->SetWindowTextA(_T("Start"));
	}
}

void CProfCalcDlg::OnBnClickedCancel()
{
	if (m_pWinThread)
	{
		StopTestMoveThread();
	}
	StopTimer(m_iTimerVal);

	OnCancel();
}

void CProfCalcDlg::OnBnClickedProfCalcTestSaveScopeDataCheck()
{
	iFlagSaveScopeDataInFile = ((CButton*) GetDlgItem(IDC_PROF_CALC_TEST_SAVE_SCOPE_DATA_CHECK))->GetCheck();
}

#include "mtn_cmd_gen_sam.h"

void CProfCalcDlg::OnBnClickedPolySam11234Btn()
{
	SAM1_1234_PRECAL_INPUT stSam1Prof_1234_Input;
	SAM1_1234_PRECAL_OUTPUT stSam1Precal_Output;

	SAM1_1234_PROFGEN_INPUT stSam1ProfGen_1234_Input;
	SAM1_1234_PROFGEN_OUTPUT stSam1ProfGen_1234_Output;

	stSam1Prof_1234_Input.Amax = dDesireMaxAcc;
	stSam1Prof_1234_Input.Dist = dDesDist;
	stSam1Prof_1234_Input.Jmax = dDesireMaxJerk;
	stSam1Prof_1234_Input.Vmax = dDesireMaxVel;

	sam1_1234_precal(&stSam1Prof_1234_Input,   &stSam1Precal_Output);

	stSam1ProfGen_1234_Input.ActualAccMax = stSam1Precal_Output.ActualAccMax;
	stSam1ProfGen_1234_Input.ActualJerkMax = stSam1Precal_Output.ActualJerkMax;
	stSam1ProfGen_1234_Input.ActualVelMax = stSam1Precal_Output.ActualVelMax;
	stSam1ProfGen_1234_Input.Dist = stSam1Precal_Output.Dist;
	stSam1ProfGen_1234_Input.T_acc = stSam1Precal_Output.T_acc;
	stSam1ProfGen_1234_Input.T_cv = stSam1Precal_Output.T_cnst;
	stSam1ProfGen_1234_Input.T_total = stSam1Precal_Output.T_total;

	dActualTimeAcc = stSam1Precal_Output.T_acc;
	dActualTimeCnstV = stSam1Precal_Output.T_cnst;
	dActualTimeDec = stSam1Precal_Output.T_acc;

	unsigned int uiTotalTime = (unsigned int)stSam1ProfGen_1234_Input.T_total;
	if(stSam1ProfGen_1234_Input.T_total <= DEF_PROFILE_LENGTH)
	{
		pdDistProf = adDistProf;
		pdVelProf = adVelProf;
		pdAccProf = adAccProf;
		pdJerkProf = adJerkProf;
//		bAllocDynMemory = FALSE;
	}
	else
	{
		pdDistProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdVelProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdAccProf = (double*)calloc(uiTotalTime, sizeof(double));
		pdJerkProf = (double*)calloc(uiTotalTime, sizeof(double));
//		bAllocDynMemory = TRUE;
	}
	stSam1ProfGen_1234_Output.d = pdDistProf;
	stSam1ProfGen_1234_Output.v = pdVelProf;
	stSam1ProfGen_1234_Output.a = pdAccProf;
	stSam1ProfGen_1234_Output.j = pdJerkProf;


	sam1_1234_profgen(&stSam1ProfGen_1234_Input,
						&stSam1ProfGen_1234_Output);


	sprintf_s(strFilenameOutProfile, 128, "Sam1_1234.txt");
	ProfCalcDlg_SaveProfileToFile();


}
