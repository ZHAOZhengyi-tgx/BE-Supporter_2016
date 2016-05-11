// TimerDlg_TestUSG.cpp : implementation file
// History 
// YYYYMMDD Author  Notes
// 20081008 Zhengyi 
// 20100122 Initialize LPT
#include "stdafx.h"
#include "TimerDlg_TestUSG.h"
#include "CPaPiC.h"

static CPaPiC mParallelPort;

#ifndef NO_IO_PCI_CARD
#include "WB_USG_FSM.h"
#else
#include "MtnApi.h"
#include "USG_BLK.h"
#endif

/// PCI I/O Board
#ifndef NO_IO_PCI_CARD
#include "DigitalIO.h"

PCI_ADS_IO_WRITE_BYTE  stPCI_ADC_ByteOut_DlgTest;
static int iPCI_ADS_ByteOut_Channel = 4;

CDigitalIO theDigitalIO;
WB_USG_FSM *cpUSG_FSM;
#endif // NO_IO_PCI_CARD

extern int GetPentiumTimeCount_per_100us(unsigned __int64 frequency);
extern bool GetPentiumClockEstimateFromRegistry(unsigned __int64 *frequency);
extern bool GetComputerClockFromReg(unsigned __int64 *frequency);

// TimerDlg_TestUSG dialog
IMPLEMENT_DYNAMIC(TimerDlg_TestUSG, CDialog)

TimerDlg_TestUSG::TimerDlg_TestUSG(HANDLE Handle,CWnd* pParent /*=NULL*/)
	: CDialog(TimerDlg_TestUSG::IDD, pParent)
{
	m_hHandle = Handle;
	m_iCountTimer = 0;
}

#include "MtnTesterResDef.h"
//#define  IDT_TIMER_1  IDT_TIMER_USG_PROF_DIO_DLG + 1  

TimerDlg_TestUSG::~TimerDlg_TestUSG()
{
#ifndef NO_IO_PCI_CARD
	if(cpUSG_FSM != NULL)
	{
		cpUSG_FSM = NULL;
	}
	mParallelPort.init_port();

//	delete cpUSG_FSM;
#endif // NO_IO_PCI_CARD
}

void TimerDlg_TestUSG::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(TimerDlg_TestUSG, CDialog)
	ON_WM_TIMER()
ON_BN_CLICKED(IDC_BUTTON_TIMER_STOP, &TimerDlg_TestUSG::OnBnClickedButtonTimerStop)
ON_BN_CLICKED(IDC_BUTTON_TIMER_START, &TimerDlg_TestUSG::OnBnClickedButtonTimerStart)
ON_BN_CLICKED(IDC_TIMER_CHECK_DEBUG_FILE, &TimerDlg_TestUSG::OnBnClickedTimerCheckDebugFile)
ON_BN_CLICKED(IDC_TIMER_VISIBLE_COUNTER_CHECK, &TimerDlg_TestUSG::OnBnClickedTimerVisibleCounterCheck)
ON_CBN_SELCHANGE(IDC_TIMER_USG_BLK_START_COMBO, &TimerDlg_TestUSG::OnCbnSelchangeTimerUspBlkStartCombo)
ON_BN_CLICKED(IDC_BUTTON_INIT_USG, &TimerDlg_TestUSG::OnBnClickedButtonInitUsp)
ON_BN_CLICKED(IDC_BUTTON_WARM_RESET, &TimerDlg_TestUSG::OnBnClickedButtonWarmReset)
ON_BN_CLICKED(IDC_USG_TIMER_WRITE_BYTE_BTN, &TimerDlg_TestUSG::OnBnClickedUsgTimerWriteByteBtn)
ON_BN_CLICKED(IDC_BUTTON_USG_BY_INT_PCI, &TimerDlg_TestUSG::OnBnClickedButtonUsgByIntPci)
ON_BN_CLICKED(IDC_USG_TRIGGER_ALL, &TimerDlg_TestUSG::OnBnClickedUsgTriggerAll)
ON_BN_CLICKED(IDC_BUTTON_PAUSE_USG, &TimerDlg_TestUSG::OnBnClickedButtonPauseUsg)
ON_BN_CLICKED(IDC_BUTTON_RESUME_USG, &TimerDlg_TestUSG::OnBnClickedButtonResumeUsg)
	ON_BN_CLICKED(IDC_USG_LPT_BIT7, &TimerDlg_TestUSG::OnBnClickedUsgLptBit7)
	ON_BN_CLICKED(IDC_USG_LPT_BIT6, &TimerDlg_TestUSG::OnBnClickedUsgLptBit6)
	ON_BN_CLICKED(IDC_USG_LPT_BIT5, &TimerDlg_TestUSG::OnBnClickedUsgLptBit5)
	ON_BN_CLICKED(IDC_USG_LPT_BIT4, &TimerDlg_TestUSG::OnBnClickedUsgLptBit4)
	ON_BN_CLICKED(IDC_USG_LPT_BIT3, &TimerDlg_TestUSG::OnBnClickedUsgLptBit3)
	ON_BN_CLICKED(IDC_USG_LPT_BIT2, &TimerDlg_TestUSG::OnBnClickedUsgLptBit2)
	ON_BN_CLICKED(IDC_USG_LPT_BIT1, &TimerDlg_TestUSG::OnBnClickedUsgLptBit1)
	ON_BN_CLICKED(IDC_USG_LPT_BIT0, &TimerDlg_TestUSG::OnBnClickedUsgLptBit0)
	ON_BN_CLICKED(IDC_USG_LPT_BUTTON_SET, &TimerDlg_TestUSG::OnBnClickedUsgLptButtonSet)
	ON_BN_CLICKED(IDC_TIMER_USG_CHECK_PATH_BY_LPT1, &TimerDlg_TestUSG::OnBnClickedTimerUsgCheckPathByLpt1)

	ON_BN_CLICKED(IDC_BUTTON_USG_DEBUG_TIMER, &TimerDlg_TestUSG::OnBnClickedButtonUsgDebugTimer)
END_MESSAGE_MAP()


// TimerDlg_TestUSG message handlers

UINT  TimerDlg_TestUSG::StartTimer (UINT TimerDuration)
{
	m_iTimerVal = SetTimer(IDT_TIMER_USG_PROF_DIO_DLG, TimerDuration, 0);
	
	if (m_iTimerVal == 0)
	{
		AfxMessageBox("Unable to obtain timer");
	}

    return m_iTimerVal;
}// end StartTimer

BOOL TimerDlg_TestUSG::StopTimer (UINT TimerVal)
{
	if (!KillTimer (m_iTimerVal))
	{
		return FALSE;
	}
	return TRUE;
} // end StopTimer

#include <Windows.h>
#include <Mmsystem.h>
//  Library: Use Winmm.lib.

BOOL TimerDlg_TestUSG::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rectNow;
	GetClientRect(&rectNow);
	// Update the member variable, indicating the plot frame
	m_iX_LeftBottom = rectNow.left;
	m_iY_LeftBottom = rectNow.bottom;
	m_iX_RightTop = rectNow.right;
	m_iY_RightTop = rectNow.top;

#ifndef NO_IO_PCI_CARD
	cpUSG_FSM = new WB_USG_FSM();

	if(cpUSG_FSM->cusg_get_flag_write_file())
	{
		((CButton*)GetDlgItem(IDC_TIMER_CHECK_DEBUG_FILE))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_TIMER_CHECK_DEBUG_FILE))->SetCheck(0);
	}

#ifdef _DEBUG
	if(cpUSG_FSM->cusg_get_flag_second_counter())
	{
		((CButton*)GetDlgItem(IDC_TIMER_VISIBLE_COUNTER_CHECK))->SetCheck(1);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_TIMER_VISIBLE_COUNTER_CHECK))->SetCheck(0);
	}
#else
	((CButton*)GetDlgItem(IDC_TIMER_VISIBLE_COUNTER_CHECK))->EnableWindow(FALSE);

#endif 

	// Default 10 KHz, 
	sprintf_s(m_strText, 32, "%d", cpUSG_FSM->m_uiTimerFreqFactor_10KHz);
	GetDlgItem(IDC_USG_TIMER_FACTOR_10KHZ)->SetWindowTextA(_T(m_strText));
	// Default, IDC_USG_TRIGGER_ADDRESS
	sprintf_s(m_strText, 32, "%d", cpUSG_FSM->m_stpActiveBlk_USG->iTriggerAddress);
	GetDlgItem(IDC_USG_TRIGGER_ADDRESS)->SetWindowTextA(_T(m_strText));
	// IDC_USG_TRIG_PATTERN
	sprintf_s(m_strText, 32, "%x", cpUSG_FSM->m_stpCurrSegment_USG->iUSG_TriggerPattern);
	GetDlgItem(IDC_USG_TRIG_PATTERN)->SetWindowTextA(_T(m_strText));

	// Init PCI Cards
	theDigitalIO.InitializeIoCard();
#endif // NO_IO_PCI_CARD

	((CButton*)GetDlgItem(IDC_USG_TRIGGER_ALL))->EnableWindow(FALSE);

	for(unsigned int ii=0; ii<_USG_MAX_NUM_BLK; ii++)
	{
		sprintf_s(m_strText, 256, "%d", ii);
		((CComboBox*)GetDlgItem(IDC_TIMER_USG_BLK_START_COMBO))->InsertString(ii, _T(m_strText));
	}

	// 20100122
	mParallelPort.init_port();
	char cFlagUsgFSM_HardwarePath = wb_usg_fsm_get_flag_hardware_path();
	((CButton*)GetDlgItem(IDC_TIMER_USG_CHECK_PATH_BY_LPT1))->SetCheck((int)cFlagUsgFSM_HardwarePath);
	if(cFlagUsgFSM_HardwarePath == USG_FSM_CMD_HARDWARE_VIA_LPT1)
	{
		cpUSG_FSM->InitLPT(); // 20100122
	}

	// Start Timer, local as a monitor update
	StartTimer(100);

	// Init Debug Text
	CString cstrTemp;
	unsigned __int64 m_liFreqByRegistry; 
	GetComputerClockFromReg(&m_liFreqByRegistry);  // GetPentiumClockEstimateFromRegistry

	cstrTemp.Format("Freq: %u \r\n", cpUSG_FSM->m_liFreqOS.QuadPart);
	cstrTemp.AppendFormat("FreqByReg: %u \r\n", m_liFreqByRegistry);
	GetDlgItem(IDC_EDIT_USG_DEBUG_TEXT)->SetWindowTextA(cstrTemp);


	return TRUE; // return TRUE unless you set the focus to a control
}

void TimerDlg_TestUSG::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CClientDC dc(this);
	CRect rectNow;
	GetClientRect(&rectNow);
	// Update the member variable, indicating the plot frame
	m_iX_LeftBottom = rectNow.left;
	m_iY_LeftBottom = rectNow.bottom;
	m_iX_RightTop = rectNow.right;
	m_iY_RightTop = rectNow.top;

//	m_iCountTimer ++;
//	sprintf_s(m_strText, 256, "%d", m_iCountTimer);
#ifndef NO_IO_PCI_CARD
	unsigned int uiIdxActiveBlk, uiIdxActiveSeg;
	cpUSG_FSM->cusg_get_curr_blk_seg(&uiIdxActiveBlk, &uiIdxActiveSeg);

	sprintf_s(m_strText, 256, "Blk:%d, Seg:%d, Cmd:%s, Status:%s, Error:%s", 
					uiIdxActiveBlk, uiIdxActiveSeg, 
					wb_usg_blk_get_text_cmd(cpUSG_FSM->m_stpCurrSegment_USG->uiCmdType), 
					wb_usg_blk_get_text_status(cpUSG_FSM->m_stpActiveBlk_USG->uiCurrStatusUSG), 
					wb_usg_blk_get_text_error_flag(cpUSG_FSM->m_uiErrorFlagUSG)
					);

	dc.TextOutA((m_iX_LeftBottom *9 + m_iX_RightTop)/10, (m_iY_LeftBottom * 9 + m_iY_RightTop)/10, m_strText);

	static char strDeviceName[128];
	theDigitalIO.GetCardsName(0, strDeviceName, 32);
	sprintf_s(m_strText, 256, "DevName: %s; Channel: %d", 
		strDeviceName, // theDigitalIO.m_IoCardInfo.stDeviceList[0].szDeviceName, 
		theDigitalIO.usGetUsgCardsChannel()); // stPCI_ADC_ByteOut_DlgTest.stPCIADS_WritePortByte.port);
	dc.TextOutA((m_iX_LeftBottom *9 + m_iX_RightTop)/10, (m_iY_LeftBottom + m_iY_RightTop * 15)/16, 
			m_strText);

	sprintf_s(m_strText, 32, "%x", cpUSG_FSM->m_stpCurrSegment_USG->iUSG_TriggerPattern);
	GetDlgItem(IDC_USG_TRIG_PATTERN)->SetWindowTextA(_T(m_strText));
#endif // NO_IO_PCI_CARD

	CDialog::OnTimer(nIDEvent);
}
void TimerDlg_TestUSG::OnShowWindow(BOOL bShow, UINT nStatus)
{	
	CDialog::OnShowWindow(bShow, nStatus);
}

// IDC_BUTTON_PAUSE_USG
void TimerDlg_TestUSG::OnBnClickedButtonPauseUsg()
{
#ifndef NO_IO_PCI_CARD
	//	cpUSG_FSM->PauseIntEventThread();
	cpUSG_FSM->PauseUSG_Thread();
	((CButton*)GetDlgItem(IDC_BUTTON_RESUME_USG))->EnableWindow(TRUE);
#endif // NO_IO_PCI_CARD
}

// IDC_BUTTON_RESUME_USG
void TimerDlg_TestUSG::OnBnClickedButtonResumeUsg()
{
#ifndef NO_IO_PCI_CARD
	//	cpUSG_FSM->ResumeIntEventThread();
	cpUSG_FSM->ResumeUSG_Thread();
	((CButton*)GetDlgItem(IDC_BUTTON_RESUME_USG))->EnableWindow(FALSE);
#endif // NO_IO_PCI_CARD

}

void TimerDlg_TestUSG::OnBnClickedButtonTimerStop()
{
//	StopTimer(m_iTimerVal);
#ifndef NO_IO_PCI_CARD
//	cpUSG_FSM->cusg_stop_timer();

	cpUSG_FSM->StopUSG_Thread();
	cpUSG_FSM->StopIntEventThread();
#endif // NO_IO_PCI_CARD
	((CButton*)GetDlgItem(IDC_BUTTON_USG_BY_INT_PCI))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_TIMER_START))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_USG_TRIGGER_ALL))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_PAUSE_USG))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_RESUME_USG))->EnableWindow(FALSE);

}

//IDC_BUTTON_TIMER_START
void TimerDlg_TestUSG::OnBnClickedButtonTimerStart()
{
	USG_UNIT_SEG stStaticUSGSeg;

#ifndef NO_IO_PCI_CARD
	GetDlgItem(IDC_USG_TIMER_FACTOR_10KHZ)->GetWindowTextA(m_strText, 32);
	cpUSG_FSM->m_uiTimerFreqFactor_10KHz = atoi(m_strText);
#endif // NO_IO_PCI_CARD

	GetDlgItem(IDC_USG_TRIGGER_ADDRESS)->GetWindowTextA(m_strText, 32);
	int iTriggerAddress = atoi(m_strText);

	GetDlgItem(IDC_USG_TRIG_PATTERN)->GetWindowTextA(m_strText, 32);
	int iTriggerPattern = atoi(m_strText);

	for(unsigned int ii=0; ii<_USG_MAX_NUM_BLK; ii++)
	{
		wb_usg_blk_set_trigger_address(ii, iTriggerAddress); // stUltraSonicPowerBlk[ii].iTriggerAddress = iTriggerAddress;
		for(unsigned int jj=0; jj< _USG_MAX_NUM_SEGMENT; jj++) // stUltraSonicPowerBlk[ii].uiTotalNumActiveSegment
		{
			//stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_TriggerPattern = iTriggerPattern;
			wb_usg_blk_get_segment(ii, jj, &stStaticUSGSeg);
			stStaticUSGSeg.iUSG_TriggerPattern = iTriggerPattern;
			wb_usg_blk_set_segment(ii, jj, &stStaticUSGSeg);
		}
	}
	
#ifndef NO_IO_PCI_CARD
	cpUSG_FSM->RunUSG_Thread();//m_fStopUSG_Thread = FALSE;
#endif // NO_IO_PCI_CARD

	((CButton*)GetDlgItem(IDC_BUTTON_USG_BY_INT_PCI))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_TIMER_START))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_USG_TRIGGER_ALL))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_PAUSE_USG))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_RESUME_USG))->EnableWindow(FALSE);
	// 
}
// IDC_USG_TRIGGER_ALL
void TimerDlg_TestUSG::OnBnClickedUsgTriggerAll()
{
	USG_UNIT_SEG stStaticUSGSeg;
	// TODO: Add your control notification handler code here
	for(unsigned int ii=0; ii<_USG_MAX_NUM_BLK; ii++)
	{
		for(unsigned int jj=0; jj< _USG_MAX_NUM_SEGMENT; jj++)
		{
			// acsc_WriteDPRAMInteger(m_hHandle, stUltraSonicPowerBlk[ii].iTriggerAddress, stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_TriggerPattern);
			wb_usg_blk_get_segment(ii, jj, &stStaticUSGSeg);
			acsc_WriteDPRAMInteger(m_hHandle, wb_usg_blk_get_trigger_address(ii), stStaticUSGSeg.iUSG_TriggerPattern);
				//stUltraSonicPowerBlk[ii].iTriggerAddress, stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_TriggerPattern);
//			high_precision_sleep_ms(20);
		}
	}

}

// IDC_BUTTON_USG_BY_INT_PCI
void TimerDlg_TestUSG::OnBnClickedButtonUsgByIntPci()
{
	GetDlgItem(IDC_USG_TRIGGER_ADDRESS)->GetWindowTextA(m_strText, 32);
	int iTriggerAddress = atoi(m_strText);

	GetDlgItem(IDC_USG_TRIG_PATTERN)->GetWindowTextA(m_strText, 32);
	int iTriggerPattern = atoi(m_strText);

	USG_UNIT_SEG stStaticUSGSeg;
	for(unsigned int ii=0; ii<_USG_MAX_NUM_BLK; ii++)
	{
//		stUltraSonicPowerBlk[ii].iTriggerAddress = iTriggerAddress;
		wb_usg_blk_set_trigger_address(ii, iTriggerAddress); // stUltraSonicPowerBlk[ii].iTriggerAddress = iTriggerAddress;

		for(unsigned int jj=0; jj< _USG_MAX_NUM_SEGMENT; jj++)
		{
//			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_TriggerPattern = iTriggerPattern;
			wb_usg_blk_get_segment(ii, jj, &stStaticUSGSeg);
			stStaticUSGSeg.iUSG_TriggerPattern = iTriggerPattern;
			wb_usg_blk_set_segment(ii, jj, &stStaticUSGSeg);
		}
	}
#ifndef NO_IO_PCI_CARD
	cpUSG_FSM->StartIntEventThread();
#endif // NO_IO_PCI_CARD
	((CButton*)GetDlgItem(IDC_BUTTON_TIMER_START))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_USG_BY_INT_PCI))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_USG_TRIGGER_ALL))->EnableWindow(TRUE);

}

// IDC_TIMER_CHECK_DEBUG_FILE
void TimerDlg_TestUSG::OnBnClickedTimerCheckDebugFile()
{
#ifndef NO_IO_PCI_CARD
	if(	((CButton*)GetDlgItem(IDC_TIMER_CHECK_DEBUG_FILE))->GetCheck() == 1)
	{
		cpUSG_FSM->cusg_set_flag_write_file(1); // Enable
	}
	else
	{
		cpUSG_FSM->cusg_set_flag_write_file(0); // Disable
	}
#endif // NO_IO_PCI_CARD
}

// IDC_TIMER_VISIBLE_COUNTER_CHECK
void TimerDlg_TestUSG::OnBnClickedTimerVisibleCounterCheck()
{
	// TODO: Add your control notification handler code here
#ifdef _DEBUG
#ifndef NO_IO_PCI_CARD

	if(	((CButton*)GetDlgItem(IDC_TIMER_VISIBLE_COUNTER_CHECK))->GetCheck() == 1)
	{
		cpUSG_FSM->cusg_set_flag_second_counter(1); // Enable
	}
	else
	{
		cpUSG_FSM->cusg_set_flag_second_counter(0); // Disable
	}
#endif // NO_IO_PCI_CARD
#endif // _DEBUG
}

// IDC_TIMER_USG_BLK_START_COMBO
void TimerDlg_TestUSG::OnCbnSelchangeTimerUspBlkStartCombo()
{
	// TODO: Add your control notification handler code here
#ifndef NO_IO_PCI_CARD
	unsigned int idxCombo = ((CComboBox*)GetDlgItem(IDC_TIMER_USG_BLK_START_COMBO))->GetCurSel();
	if(idxCombo >0 && idxCombo <_USG_MAX_NUM_BLK)
	{
		cpUSG_FSM->cusg_set_active_blk(idxCombo);
	}
#endif // NO_IO_PCI_CARD
}

void TimerDlg_TestUSG::OnBnClickedButtonInitUsp()
{

	USG_UNIT_SEG stStaticUSGSeg;
	stStaticUSGSeg.iUSG_Amplitude = 100;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	wb_usg_blk_set_segment(1, 0, &stStaticUSGSeg);

	//// From BLk 1 
	stStaticUSGSeg.iUSG_Amplitude = 200;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_WAIT_TRIGGER;
	wb_usg_blk_set_segment(1, 1, &stStaticUSGSeg);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	wb_usg_blk_set_segment(1, 2, &stStaticUSGSeg);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_END_GOTO_NEXT_BLK;
	wb_usg_blk_set_segment(1, 3, &stStaticUSGSeg);
	//stUltraSonicPowerBlk[1].uiTotalNumActiveSegment = 128;  

	//// From BLk 2 
	stStaticUSGSeg.iUSG_Amplitude = 255;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 6;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	wb_usg_blk_set_segment(2, 0, &stStaticUSGSeg);

	stStaticUSGSeg.iUSG_Amplitude = 100;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 7;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_WAIT_TRIGGER;
	wb_usg_blk_set_segment(2, 1, &stStaticUSGSeg);

	// Set for testing protection
	// wb_usg_blk_set_max_count_wait_trig_prot(2, 1000);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 9;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_RAMP_STEP;
	wb_usg_blk_set_segment(2, 2, &stStaticUSGSeg);

	stStaticUSGSeg.iUSG_Amplitude = 50;
	stStaticUSGSeg.iUSG_Duration_ms = 10; 
	stStaticUSGSeg.iUSG_RampTime = 4;       // to avoid divided by zero
	stStaticUSGSeg.iUSG_TriggerPattern = 0xAB;
	stStaticUSGSeg.uiCmdType = _USG_CMD_END_GOTO_NEXT_BLK;
	wb_usg_blk_set_segment(2, 3, &stStaticUSGSeg);

}

void TimerDlg_TestUSG::OnBnClickedButtonWarmReset()
{
#ifndef NO_IO_PCI_CARD
	cpUSG_FSM->cusg_warm_reset();
#endif // NO_IO_PCI_CARD
}

void TimerDlg_TestUSG::OnBnClickedUsgTimerWriteByteBtn()
{
	char m_strTextTemp[32];
	int gwData;
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_USG_TIMER_OUT_TEST)->GetWindowTextA( &m_strTextTemp[0], 32);
	sscanf_s(m_strTextTemp, "%d", &gwData);

#ifndef NO_IO_PCI_CARD
	theDigitalIO.UsgWriteOutputByte(gwData);
#endif // NO_IO_PCI_CARD
//	stPCI_ADC_ByteOut_DlgTest.stPCIADS_WritePortByte.state = gwData;

	//LRESULT         ErrCode; 
 //   WCHAR           strErrMsg[80];
	//if((ErrCde = DRV_DioWritePortByte(stPCI_ADC_ByteOut_DlgTest.DriverHandle,
	//	(LPT_DioWritePortByte)&stPCI_ADC_ByteOut_DlgTest.stPCIADS_WritePortByte)) != 0)
	//{
	//	DRV_GetErrorMessage(ErrCde,(LPSTR)strErrMsg);
	//	AfxMessageBox(_T(strErrMsg));
	//}

}

// generate random seeds
   	//srand((unsigned)time(NULL));

// random plot shapes for debugging
	//switch(rand() % 5)
	//{
	//	case 0:
	//		dc.Ellipse(x, abs(y-200), abs(y-x), y);
	//		break;
	//	case 1:
	//		dc.Rectangle(y, x, abs(y-x), (x+y)%255);
	//		break;
	//	case 2:
	//		dc.RoundRect(y, x, y, x, abs(x-y), x+y);
	//		break;
	//	case 3:
	//		dc.Ellipse(y, x, abs(x-y), x+y);
	//		break;
	//	case 4:
	//		dc.Rectangle(x, y, abs(x-y), x+y);
	//		break;
	//}
	//dc.SelectObject(pOldBrush);
	//dc.SelectObject(pOldPen);

static BOOL bDataLPT_Bit7, bDataLPT_Bit6, bDataLPT_Bit5, bDataLPT_Bit4, bDataLPT_Bit3, bDataLPT_Bit2, bDataLPT_Bit1, bDataLPT_Bit0;
static unsigned char ucNumByteLPT;

// IDC_USG_LPT_BIT7
void TimerDlg_TestUSG::OnBnClickedUsgLptBit7()
{
	bDataLPT_Bit7 = !bDataLPT_Bit7;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT6
void TimerDlg_TestUSG::OnBnClickedUsgLptBit6()
{
	bDataLPT_Bit6 = !bDataLPT_Bit6;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT5
void TimerDlg_TestUSG::OnBnClickedUsgLptBit5()
{
	bDataLPT_Bit5 = !bDataLPT_Bit5;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT4
void TimerDlg_TestUSG::OnBnClickedUsgLptBit4()
{
	bDataLPT_Bit4 = !bDataLPT_Bit4;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT3
void TimerDlg_TestUSG::OnBnClickedUsgLptBit3()
{
	bDataLPT_Bit3 = !bDataLPT_Bit3;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT2
void TimerDlg_TestUSG::OnBnClickedUsgLptBit2()
{
	bDataLPT_Bit2 = !bDataLPT_Bit2;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT1
void TimerDlg_TestUSG::OnBnClickedUsgLptBit1()
{
	bDataLPT_Bit1 = !bDataLPT_Bit1;
	UpdateLPT_Data();
}
// IDC_USG_LPT_BIT0
void TimerDlg_TestUSG::OnBnClickedUsgLptBit0()
{
	bDataLPT_Bit0 = !bDataLPT_Bit0;
	UpdateLPT_Data();
}

void TimerDlg_TestUSG::UpdateLPT_Data()
{
	if(bDataLPT_Bit7)
	{
		GetDlgItem(IDC_USG_LPT_BIT7)->SetWindowTextA(_T("7"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT7)->SetWindowTextA(_T("0"));
	}

	if(bDataLPT_Bit6)
	{
		GetDlgItem(IDC_USG_LPT_BIT6)->SetWindowTextA(_T("6"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT6)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit5)
	{
		GetDlgItem(IDC_USG_LPT_BIT5)->SetWindowTextA(_T("5"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT5)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit4)
	{
		GetDlgItem(IDC_USG_LPT_BIT4)->SetWindowTextA(_T("4"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT4)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit3)
	{
		GetDlgItem(IDC_USG_LPT_BIT3)->SetWindowTextA(_T("3"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT3)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit2)
	{
		GetDlgItem(IDC_USG_LPT_BIT2)->SetWindowTextA(_T("2"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT2)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit1)
	{
		GetDlgItem(IDC_USG_LPT_BIT1)->SetWindowTextA(_T("1"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT1)->SetWindowTextA(_T("0"));
	}
	if(bDataLPT_Bit0)
	{
		GetDlgItem(IDC_USG_LPT_BIT0)->SetWindowTextA(_T("0"));
	}
	else
	{
		GetDlgItem(IDC_USG_LPT_BIT0)->SetWindowTextA(_T(""));
	}

	ucNumByteLPT = 128 * bDataLPT_Bit7 + 64 * bDataLPT_Bit6 + 32 * bDataLPT_Bit5 + 16 * bDataLPT_Bit4 + 8 * bDataLPT_Bit3 + 4 * bDataLPT_Bit2 + 2 * bDataLPT_Bit1 + bDataLPT_Bit0;

	char tempChar[8];
	sprintf_s(tempChar, 256, "%d", ucNumByteLPT);
	GetDlgItem(IDC_USG_LPT_NUM_STATIC)->SetWindowTextA(_T(tempChar));

}
#include <conio.h>
int _outp( unsigned short port, int databyte );

#define __PARALLEL_PORT_ADDRESS_		0x378

// IDC_USG_LPT_BUTTON_SET
void TimerDlg_TestUSG::OnBnClickedUsgLptButtonSet()
{
	lp_pin lpOut;
	// TODO: Add your control notification handler code here
	//_outp(0x378, ucNumByteLPT);  // __PARALLEL_PORT_ADDRESS_
	lpOut = ucNumByteLPT;
	mParallelPort.pin_pattern(lpOut);
}

// IDC_TIMER_USG_CHECK_PATH_BY_LPT1
void TimerDlg_TestUSG::OnBnClickedTimerUsgCheckPathByLpt1()
{
	int iFlagGetUSG_FSM_FlagPathViaLPT1;
	iFlagGetUSG_FSM_FlagPathViaLPT1 = ((CButton*)GetDlgItem(IDC_TIMER_USG_CHECK_PATH_BY_LPT1))->GetCheck();

	if(iFlagGetUSG_FSM_FlagPathViaLPT1)
	{
		wb_usg_fsm_set_flag_via_lpt1();
	}
	else
	{
		wb_usg_fsm_set_flag_via_1739u();
	}

}
// IDC_EDIT_USG_DEBUG_TEXT
// IDC_BUTTON_USG_DEBUG_TIMER
void TimerDlg_TestUSG::OnBnClickedButtonUsgDebugTimer()
{
	CString cstrTemp;
	unsigned __int64 m_liFreqByRegistry; 
	unsigned int uiCounterPrev, uiCounterCurr;
	double dTickPrev, dTickCurr;
	LARGE_INTEGER liTickPrev, liTickCurr;
	GetComputerClockFromReg(&m_liFreqByRegistry);  // GetPentiumClockEstimateFromRegistry

	GetDlgItem(IDC_BUTTON_USG_DEBUG_TIMER)->EnableWindow(FALSE);

	cstrTemp.Format("Freq: %u \r\n", cpUSG_FSM->m_liFreqOS.QuadPart);
	cstrTemp.AppendFormat("FreqByReg: %u \r\n", m_liFreqByRegistry);
	uiCounterPrev = GetPentiumTimeCount_per_100us(cpUSG_FSM->m_liFreqOS.QuadPart); // timeGetTime();
	Sleep(100);
	uiCounterCurr = GetPentiumTimeCount_per_100us(cpUSG_FSM->m_liFreqOS.QuadPart); // timeGetTime();

	cstrTemp.AppendFormat("Counter = [%u, \r\n %u] - %u;\r\n", uiCounterPrev, uiCounterCurr, uiCounterCurr - uiCounterPrev);

	uiCounterPrev = GetPentiumTimeCount_per_100us(m_liFreqByRegistry); // timeGetTime();
	Sleep(100);
	uiCounterCurr = GetPentiumTimeCount_per_100us(m_liFreqByRegistry); // timeGetTime();
	if(uiCounterCurr < uiCounterPrev)
	{
		uiCounterCurr = uiCounterCurr + 10000;
	}
	cstrTemp.AppendFormat("CounterByReg = [%u, \r\n %u] - %u;\r\n", uiCounterPrev, uiCounterCurr, uiCounterCurr - uiCounterPrev);

	QueryPerformanceCounter(&liTickPrev);
	Sleep(100);
	QueryPerformanceCounter(&liTickCurr);

	dTickPrev = (double)(liTickPrev.QuadPart % m_liFreqByRegistry);
	uiCounterPrev = (unsigned int)(dTickPrev/(double)m_liFreqByRegistry * 10000);
	dTickCurr = (double)(liTickCurr.QuadPart % m_liFreqByRegistry);
	uiCounterCurr = (unsigned int)(dTickCurr/(double)m_liFreqByRegistry * 10000);

	cstrTemp.AppendFormat("TickByReg = [%u, \r\n %u] - %u;\r\n", uiCounterPrev, uiCounterCurr, uiCounterCurr - uiCounterPrev);

	GetDlgItem(IDC_EDIT_USG_DEBUG_TEXT)->SetWindowTextA(cstrTemp);

	Sleep(100);
	GetDlgItem(IDC_BUTTON_USG_DEBUG_TIMER)->EnableWindow(TRUE);

}
