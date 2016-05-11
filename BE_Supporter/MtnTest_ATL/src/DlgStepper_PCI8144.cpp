// DlgStepper_PCI8144.cpp : implementation file
//

#include "stdafx.h"
#include "DlgStepper_PCI8144.h"

#include "pci_8144.h"

// CDlgStepper_PCI8144 dialog

IMPLEMENT_DYNAMIC(CDlgStepper_PCI8144, CDialog)

CDlgStepper_PCI8144::CDlgStepper_PCI8144(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgStepper_PCI8144::IDD, pParent)
{

}

CDlgStepper_PCI8144::~CDlgStepper_PCI8144()
{
}

void CDlgStepper_PCI8144::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgStepper_PCI8144, CDialog)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_BOARD_ID_STP8144_DLG, &CDlgStepper_PCI8144::OnCbnSelchangeComboBoardIdStp8144Dlg)
	ON_CBN_SELCHANGE(IDC_COMBO_AXIS_ID_STP8144_DLG, &CDlgStepper_PCI8144::OnCbnSelchangeComboAxisIdStp8144Dlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PROFILE_TYPE_TV_SV_STP8144_DLG, &CDlgStepper_PCI8144::OnCbnSelchangeComboProfileTypeTvSvStp8144Dlg)
	ON_CBN_SELCHANGE(IDC_COMBO_MOTION_MODE_STP8144_DLG, &CDlgStepper_PCI8144::OnCbnSelchangeComboMotionModeStp8144Dlg)
	ON_BN_CLICKED(IDC_EDIT_STP8144_DLG_AXIS_MOVE_START, &CDlgStepper_PCI8144::OnBnClickedEditStp8144DlgAxisMoveStart)
	ON_EN_KILLFOCUS(IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST, &CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveDist)
	ON_EN_KILLFOCUS(IDC_EDIT_STP8144_DLG_AXIS_MOVE_START_VEL, &CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveStartVel)
	ON_EN_KILLFOCUS(IDC_EDIT_STP8144_DLG_AXIS_MOVE_MAX_VEL, &CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveMaxVel)
	ON_EN_KILLFOCUS(IDC_EDIT_STP8144_DLG_AXIS_MOVE_T_ACC, &CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveTAcc)
END_MESSAGE_MAP()

#define __MAX_NUM_CARD_PCI_8144 16
#define __MAX_AXIS_STEPPER_PCI_8144 (__MAX_NUM_CARD_PCI_8144 * 4)
I16 usTotalCard = 0;
I16 usCardID = 0;
I16 Card_Pci_8144_Status[__MAX_NUM_CARD_PCI_8144];
I16 Now_Max_Card_Pci_8144=0;
I16 ALL_Max_PCI_8144_Card = __MAX_NUM_CARD_PCI_8144;
I16 Do_Status, DI_Status, Rtn_Msg, Motion_Counter;

static double	dMoveStartVelocity[__MAX_AXIS_STEPPER_PCI_8144];  // m_TvStr;
static double	dMoveMaxVelocity[__MAX_AXIS_STEPPER_PCI_8144];    // m_TvMaxVel;
static double	dMoveAcceleration[__MAX_AXIS_STEPPER_PCI_8144];   // m_TvTacc;
static int	iMoveRelDistance[__MAX_AXIS_STEPPER_PCI_8144];    // m_TrDis;

static I16 usCurrAxisId;
static I16 usAxisNO_CurrCardStart, usAxisNO_CurrCardEnd;  // AxisNum=0,

bool Int_Flag = false;

static I16 usFlagProfileType;
#define __STEPPER_MOVE_PROFILE_TRAP__  0
#define __STEPPER_MOVE_PROFILE_SINE__  1

static I16 usStepperMovingMode;
#define __STEPPER_MOVE_REL_DIST__  0
#define __STEPPER_MOVE_JOGGING__   1

void CDlgStepper_PCI8144::UI_InitCombo()
{
	((CComboBox*)GetDlgItem(IDC_COMBO_PROFILE_TYPE_TV_SV_STP8144_DLG))->InsertString(__STEPPER_MOVE_PROFILE_TRAP__, "TV");
	((CComboBox*)GetDlgItem(IDC_COMBO_PROFILE_TYPE_TV_SV_STP8144_DLG))->InsertString(__STEPPER_MOVE_PROFILE_SINE__, "SV");


	((CComboBox*)GetDlgItem(IDC_COMBO_MOTION_MODE_STP8144_DLG))->InsertString(__STEPPER_MOVE_REL_DIST__, "RelDist");
	((CComboBox*)GetDlgItem(IDC_COMBO_MOTION_MODE_STP8144_DLG))->InsertString(__STEPPER_MOVE_JOGGING__, "Jogging");	
}

void CDlgStepper_PCI8144::UI_UpdateAxisMovingCondition()
{
	CString cstrTemp;

	cstrTemp.Format("%d", iMoveRelDistance[usCurrAxisId]);
	GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.1f", dMoveStartVelocity[usCurrAxisId]);
	GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_START_VEL)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.1f", dMoveMaxVelocity[usCurrAxisId]);
	GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_MAX_VEL)->SetWindowTextA(cstrTemp);

	cstrTemp.Format("%8.1f", dMoveAcceleration[usCurrAxisId]);
	GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_T_ACC)->SetWindowTextA(cstrTemp);

}

void CDlgStepper_PCI8144::UI_UpdateAxisIdByCardId()
{
	CString cstrTemp;
	usAxisNO_CurrCardStart = 4 * usCardID;
	usAxisNO_CurrCardEnd = usAxisNO_CurrCardStart + 3;
	CComboBox *pCombo_AxisIdStepper8144Dlg =((CComboBox *) GetDlgItem(IDC_COMBO_AXIS_ID_STP8144_DLG));
	pCombo_AxisIdStepper8144Dlg->ResetContent();
	for(I16 ii = usAxisNO_CurrCardStart; ii<= usAxisNO_CurrCardEnd; ii++)
	{
		cstrTemp.Format("Axis# %d", ii);
		pCombo_AxisIdStepper8144Dlg->InsertString(ii - usAxisNO_CurrCardStart, cstrTemp);
	}
}

#include "MtnDefin.h"
//__declspec(dllimport) I16 _8144_config_from_file();
#define BE_WB_STEPPER_8144_CTRL_DEF_LIMIT_LOGIC      0
#define BE_WB_STEPPER_8144_CTRL_DEF_IO_SENSITIVITY   1
#define BE_WB_STEPPER_8144_CTRL_DEF_PULSE_OUT_LOGIC  0
#define BE_WB_STEPPER_8144_CTRL_DEF_ORG_ENABLE_LOGIC 0
#define BE_WB_STEPPER_8144_CTRL_DEF_SD_ENABLE        0
#define BE_WB_STEPPER_8144_CTRL_DEF_PULSE_OUTMODE    1

int mtn_stp_config_pci_8144(U16 iAxisId)
{
	_8144_set_limit_logic(iAxisId, BE_WB_STEPPER_8144_CTRL_DEF_LIMIT_LOGIC);
	_8144_set_mio_sensitivity(iAxisId, BE_WB_STEPPER_8144_CTRL_DEF_IO_SENSITIVITY);
	_8144_set_pls_outmode(iAxisId, BE_WB_STEPPER_8144_CTRL_DEF_PULSE_OUT_LOGIC);
	_8144_set_pls_outmode2(iAxisId, BE_WB_STEPPER_8144_CTRL_DEF_PULSE_OUTMODE, BE_WB_STEPPER_8144_CTRL_DEF_PULSE_OUT_LOGIC);

	return MTN_API_OK_ZERO;
}

int mtn_stp_config_pci_8144_card(U16 usCardId)
{
U16 usStartAxis, usEndAxis, ii;
	usStartAxis = 4 * usCardId;
	usEndAxis = usStartAxis + 3;

	for(ii = usStartAxis; ii<=usEndAxis; ii++)
	{
		mtn_stp_config_pci_8144(ii);
	}

	return MTN_API_OK_ZERO;
}

BOOL CDlgStepper_PCI8144::OnInitDialog()
{
// CDlgStepper_PCI8144 message handlers

	I32 CardID_InBit;				
	I16 RET=0;
	I16 ii=0;
	usTotalCard=0;

	UI_InitCombo();
	UI_UpdateAxisMovingCondition();

	RET = _8144_initial(&CardID_InBit, 0);			// Initial PCI-8144 card
	
	for (ii = 0; ii < ALL_Max_PCI_8144_Card; ii++)     // Check how many card on PC
	{
		if (CardID_InBit & (1<<ii))
		{
			usTotalCard = usTotalCard+1;
			Card_Pci_8144_Status[ii] = true;
			Now_Max_Card_Pci_8144 = ii+1;
			usCardID = ii;
		}
		else
			Card_Pci_8144_Status[ii] = false;
	}

	if (usTotalCard == 0)                            // If PCI-8144 is not exist, then close program 
	{
		_8144_close();
		MessageBox("No PCI - 8144 Card Exist", "ERROR");
		CDialog::OnOK();
		return FALSE;  // Return error;
	}
	else
	{

		CString cstrTemp;

		CComboBox *pCombo_CardIdStepper8144Dlg =((CComboBox *) GetDlgItem(IDC_COMBO_BOARD_ID_STP8144_DLG));
		pCombo_CardIdStepper8144Dlg->ResetContent();
		for(ii = 0; ii<usTotalCard; ii++)
		{
			cstrTemp.Format("Card# %d", ii);
			pCombo_CardIdStepper8144Dlg->InsertString(ii, cstrTemp);

			//
			mtn_stp_config_pci_8144_card(ii);
			Sleep(20);
		}
		pCombo_CardIdStepper8144Dlg->SetCurSel(usCardID);

		// Update axis once the card is changed
		UI_UpdateAxisIdByCardId();
		usCurrAxisId = usAxisNO_CurrCardEnd;
		CComboBox *pCombo_AxisIdStepper8144Dlg =((CComboBox *) GetDlgItem(IDC_COMBO_AXIS_ID_STP8144_DLG));
		pCombo_AxisIdStepper8144Dlg->SetCurSel(usCurrAxisId - usAxisNO_CurrCardStart);

		_8144_config_from_file();  // configurate the default setting from file, 20110201
		// 
		// Only start time for valid card and axis
		StartTimer(100); // SetTimer(1, 100, NULL);

	}
    
	//SetDlgItemInt(IDC_Tbox_AxisStart, usAxisNO_CurrCardStart);
	//SetDlgItemInt(IDC_Tbox_AxisEnd, usAxisNO_CurrCardEnd);
	//SetDlgItemInt(IDC_EDIT6, 0);

	//_8144_set_remaining_pulse(I16(GetDlgItemInt(IDC_Tbox_AxisNO)), 0);


	return CDialog::OnInitDialog();
}

#include "MtnTesterResDef.h"
static UINT_PTR iTimerIdStepper8144Dlg;

UINT_PTR CDlgStepper_PCI8144::StartTimer(UINT TimerDuration)
{
	iTimerIdStepper8144Dlg = SetTimer(IDT_STEPPER_CTRL_DLG_TIMER, TimerDuration, 0);
	
	if (iTimerIdStepper8144Dlg == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}

    return (UINT_PTR)iTimerIdStepper8144Dlg;
}// end StartTimer

BOOL CDlgStepper_PCI8144::StopTimer()
{
	if (!KillTimer (iTimerIdStepper8144Dlg))
	{
		return FALSE;
	}
	return TRUE;
}

void CDlgStepper_PCI8144::UI_TimerEventUpdateMotionIO_Status()
{
	CWnd *pWnd; // , *pWnd1;
	CDC *pDC; // , *pDC1;
	CRect Rect;//, Rect1;

	I16 ii, MotionIO_Status;
	_8144_get_mio_status(usCurrAxisId, &MotionIO_Status);

	if( (MotionIO_Status & 0x80) == 128 )
	{
		MotionIO_Status = MotionIO_Status - 128;
	}
	for (ii = 0; ii < 7; ii++)
	{
		switch(ii)
		{
			case 0:
				pWnd = GetDlgItem(IDC_LightNEL);
				break;
			case 1:
				pWnd = GetDlgItem(IDC_LightPEL);
				break;
			case 2:
				pWnd = GetDlgItem(IDC_LightORG);
				break;
			case 3:
				pWnd = GetDlgItem(IDC_LightSTP);
				break;
			case 4:
				pWnd = GetDlgItem(IDC_LightSTA);
				break;
			case 5:
				pWnd = GetDlgItem(IDC_LightPSD);
				break;
			case 6:
			default:
				pWnd = GetDlgItem(IDC_LightNSD);
				break;
		}
		if ( MotionIO_Status & (1<<ii) )
		{
			pDC = pWnd->GetDC();
			pWnd->GetClientRect(&Rect);
			pDC->FillSolidRect(Rect, 0xff00);
			ReleaseDC(pDC);
		}
		else
		{
//			pWnd = GetDlgItem(IDC_LightNEL+i);
			pDC = pWnd->GetDC();
			pWnd->GetClientRect(&Rect);
			pDC->FillSolidRect(Rect, 0x00);
			ReleaseDC(pDC);
		}
	}
}

void CDlgStepper_PCI8144::OnTimer(UINT nIDEvent)
{

//	F64 Remain_Pulse;

	I16 Motion_Status;
	//_8144_motion_status(I16(GetDlgItemInt(IDC_Tbox_AxisNO)), &Motion_Status);

	UI_TimerEventUpdateMotionIO_Status();

	CDialog::OnTimer(nIDEvent);

}
// IDC_COMBO_BOARD_ID_STP8144_DLG
void CDlgStepper_PCI8144::OnCbnSelchangeComboBoardIdStp8144Dlg()
{
	usCardID = ((CComboBox*)GetDlgItem(IDC_COMBO_BOARD_ID_STP8144_DLG))->GetCurSel();

	UI_UpdateAxisIdByCardId(); // Update axis once the card is changed
	usCurrAxisId = ((CComboBox*)GetDlgItem(IDC_COMBO_AXIS_ID_STP8144_DLG))->GetCurSel() + usAxisNO_CurrCardStart;
	UI_UpdateAxisMovingCondition();
}
// IDC_COMBO_AXIS_ID_STP8144_DLG
void CDlgStepper_PCI8144::OnCbnSelchangeComboAxisIdStp8144Dlg()
{
	usCurrAxisId = ((CComboBox*)GetDlgItem(IDC_COMBO_AXIS_ID_STP8144_DLG))->GetCurSel() + usAxisNO_CurrCardStart;
	UI_UpdateAxisMovingCondition();
}
// IDC_COMBO_PROFILE_TYPE_TV_SV_STP8144_DLG
void CDlgStepper_PCI8144::OnCbnSelchangeComboProfileTypeTvSvStp8144Dlg()
{
	usFlagProfileType = ((CComboBox*)GetDlgItem(IDC_COMBO_PROFILE_TYPE_TV_SV_STP8144_DLG))->GetCurSel();
}

// IDC_COMBO_MOTION_MODE_STP8144_DLG
void CDlgStepper_PCI8144::OnCbnSelchangeComboMotionModeStp8144Dlg()
{
	usStepperMovingMode = ((CComboBox*)GetDlgItem(IDC_COMBO_MOTION_MODE_STP8144_DLG))->GetCurSel();
	// __STEPPER_MOVE_REL_DIST__  0
	if(usStepperMovingMode == __STEPPER_MOVE_JOGGING__)
	{
		GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST)->EnableWindow(TRUE);
	}
}

// IDC_STATIC_STEPPER_CTRL_PCI8144_CARD_ID
// IDC_STATIC_STEPPER_CTRL_PCI8144_AXIS_ID
// IDC_STATIC_STEPPER_CTRL_PCI8144_NEL, Negative Eletr. Limit
// IDC_STATIC_STEPPER_CTRL_PCI8144_PEL,
// IDC_STATIC_STEPPER_CTRL_PCI8144_ORG, Original
// IDC_STATIC_STEPPER_CTRL_PCI8144_STOP, stop
// IDC_STATIC_STEPPER_CTRL_PCI8144_START
// IDC_STATIC_STEPPER_CTRL_PCI8144_PSD
// IDC_STATIC_STEPPER_CTRL_PCI8144_NSD

// IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_START_VEL
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_MAX_VEL
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_T_ACC

// IDC_STATIC_STP8144_DLG_AXIS_MOVE_DIST
// IDC_STATIC_STP8144_DLG_AXIS_MOVE_START_VEL
// IDC_STATIC_STP8144_DLG_AXIS_MOVE_MAX_VEL
// IDC_STATIC_STP8144_DLG_AXIS_MOVE_T_ACC

//Motion Interface I/O  Section:4
//I16 FNTYPE _8144_set_limit_logic(I16 AxisNo, I16 LimitLogic );
//I16 FNTYPE _8144_get_limit_logic(I16 AxisNo, I16 *LimitLogic );
//I16 FNTYPE _8144_get_mio_status(I16 AxisNo, I16 *MotionIoStatusInBit );
//I16 FNTYPE _8144_set_mio_sensitivity( I16 AxisNo, I16 HighOrLow );
//I16 FNTYPE _8144_set_pls_outmode( I16 AxisNo, I16 PulseLogic );

// Motion  Section:5
//I16 FNTYPE _8144_tv_move( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 Tacc );
//I16 FNTYPE _8144_sv_move( I16 AxisNo, F64 StrVel, F64 MaxVel, F64 TSacc );
//I16 FNTYPE _8144_start_tr_move( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc );
//I16 FNTYPE _8144_start_sr_move( I16 AxisNo, F64 Distance, F64 StrVel, F64 MaxVel, F64 Tacc );
//I16 FNTYPE _8144_set_external_start( I16 AxisNo, I16 Enable );
//I16 FNTYPE _8144_emg_stop( I16 AxisNo );
//I16 FNTYPE _8144_dec_stop( I16 AxisNo );
//I16 FNTYPE _8144_slow_down( I16 AxisNo );
//I16 FNTYPE _8144_enable_org_stop( I16 AxisNo, I16 Enable );
//I16 FNTYPE _8144_enable_sd_signal( I16 AxisNo, I16 Enable );
//I16 FNTYPE _8144_hold_current_speed( I16 AxisNo, I16 HoldSpeed );
//I16 FNTYPE _8144_get_remaining_pulse( I16 AxisNo, F64 *RemainingPulse );
//I16 FNTYPE _8144_set_remaining_pulse( I16 AxisNo, I32 RemainingPulse ); 

// IDC_EDIT_STP8144_DLG_AXIS_MOVE_START
void CDlgStepper_PCI8144::OnBnClickedEditStp8144DlgAxisMoveStart()
{

	_8144_set_gpio_output(usCardID, 9);
	_8144_set_limit_logic(usCurrAxisId, 1);

	if(usStepperMovingMode == __STEPPER_MOVE_JOGGING__)
	{
		_8144_tv_move(usCurrAxisId, 
			 dMoveStartVelocity[usCurrAxisId], dMoveMaxVelocity[usCurrAxisId], dMoveAcceleration[usCurrAxisId]);
	}
	else
	{
		_8144_start_tr_move(usCurrAxisId, 
			iMoveRelDistance[usCurrAxisId], dMoveStartVelocity[usCurrAxisId], dMoveMaxVelocity[usCurrAxisId], dMoveAcceleration[usCurrAxisId]);
	}
}

// IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST
void CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveDist()
{
	iMoveRelDistance[usCurrAxisId] = (int)GetDlgItemInt(IDC_EDIT_STP8144_DLG_AXIS_MOVE_DIST);
}
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_START_VEL
void CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveStartVel()
{
//	dMoveStartVelocity[usCurrAxisId]
	ReadDoubleFromEdit(IDC_EDIT_STP8144_DLG_AXIS_MOVE_START_VEL, &dMoveStartVelocity[usCurrAxisId]);
}
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_MAX_VEL
void CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveMaxVel()
{
//	dMoveMaxVelocity[usCurrAxisId]
	ReadDoubleFromEdit(IDC_EDIT_STP8144_DLG_AXIS_MOVE_MAX_VEL, &dMoveMaxVelocity[usCurrAxisId]);
}
// IDC_EDIT_STP8144_DLG_AXIS_MOVE_T_ACC
void CDlgStepper_PCI8144::OnEnKillfocusEditStp8144DlgAxisMoveTAcc()
{
//	dMoveAcceleration[usCurrAxisId]
	ReadDoubleFromEdit(IDC_EDIT_STP8144_DLG_AXIS_MOVE_T_ACC, &dMoveAcceleration[usCurrAxisId]);
}

static char *strStopString;
void CDlgStepper_PCI8144::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*pdValue = strtod(tempChar, &strStopString);
//	sscanf_s(tempChar, "%f", pdValue);
}
