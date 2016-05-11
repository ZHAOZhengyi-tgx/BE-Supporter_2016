//The MIT License (MIT)
//
//Copyright (c) 2016 ZHAOZhengyi-tgx
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.
// (c)All right reserved, sg.LongRenE@gmail.com

// USG_FSM.cpp : implementation file
// Ultra Sonic Generation, Finite State Machine
// History
// YYYYMMDD  Author  Notes
// 20081008  Zhengyi Move structure define and some global variable to USG_BLK.h, USG_BLK.cpp
//                   donot use windows UI resource
/// !!!!!!!!!!!!!!!!! Make sure theDigitalIO is initialized in application
// 20081009  Zhengyi Start from segment 0, by default
// 20081021  Zhengyi Find a potential dangerous error in user's application, when calling PauseUSG_Thread and then ResumeUSG_Thread
//                   The timing interval between PauseUSG_Thread and ResumeUSG_Thread, should be enough s.t. 
//							USG_ThreadWin32wTiming has exited.
// 20090217  Zhengyi use ReadDpramInteger but not read Dpram directly
// 20090219  Zhengyi replace ServoController.h to AcsServo.h, follow ZhangDong's new release
// 20090520  Zhengyi Note down that m_iCountUSG is with the FSM frequence, iUSG_Duration_ms is with the same frequence, but not really milli-second
//                   Comparison Factor is (10/m_uiTimerFreqFactor_10KHz)
// 20090908 Replace Sleep to be high_precision_sleep_ms
// 20100111 Output USG Byte from LPT1
#include "stdafx.h"
#include "USG_FSM.h"
#include "mtndefin.h"
#include <MMSystem.h>
#include "AcsServo.h" // 20090219
//#include "MACDEF.h"
#include "WinTiming.h"

#define __USG_BY_DIGITAL_IO_1739U__

#ifndef __USG_BY_DIGITAL_IO_1739U__
#include "CPaPiC.h"
static CPaPiC mUSG_ParallelPort;
static 	lp_pin lpOutUSG;
#endif

extern char *pstrCmdTypeUSG[];
extern char *pstrStatusUSG[];
extern char *pstrErrorFlagUSG[];
extern MTN_USG_BLK stUltraSonicPowerBlk[_USG_MAX_NUM_BLK];
extern int iaDebugTriggerRandDataRange[];
int iUSG_Semaphor_DoingFSM = 0;
USG_FSM::USG_FSM() //, CWnd* pParent /*=NULL*/) // 20081007 
{
	// Initialize the communication handler
//	m_hHandle = //Handle;

	m_fptrDebug = NULL;  // 20110825

	// initialise the parameter USG block and all the segments
	init_usg_blk_seg();

	m_iCountUSG = 1;
	m_dDeltaAmp = 0;
	m_dCurrAmplitudeReg = 0;
	m_iCurrenAmplitudeOutUSG = 0;
	m_uiErrorFlagUSG = _USG_NO_ERR_ZERO;

	m_iDataReadTrigger = 0;
	m_uiIdxActiveBlk = 0;
	m_stpActiveBlk_USG = &stUltraSonicPowerBlk[m_uiIdxActiveBlk];
	m_stpCurrSegment_USG = &m_stpActiveBlk_USG->stUSG_Segment[0];
	m_uiIdxActiveSeg = 0;  // by default it is first segment

	m_uiPeriod_ms = 0;
	m_iCountTimer = 0;
	m_iCountSec = 0;

	//m_stpPCI_ADS_WritePortByte = &(m_stPCI_ADS_ByteOut_TimerUSG.stPCIADS_WritePortByte);
	//m_stpPCI_ADS_WriteBit = &(m_stPCI_ADS_BitOutTest.stPCIADS_WriteBit);

	m_iFlagWriteDebugFile = 0;
	m_iShowSecondCounter = 0;

	if(m_iFlagWriteDebugFile >= 1)
	{
		fopen_s(&m_fptrDebug, _USG_DEBUG_FILE_NAME, "w");
		if(m_fptrDebug == NULL)
		{
			sprintf_s(m_strTextDebug, 256, "Error Open File");
		}
		else
		{
			fprintf(m_fptrDebug, "Cnt,\tBlk,\tSeg,\tCmd,\tdAmp,\tiAmp,\tCurSta,\tNxtSta, \tTrgPtn, \tTrgDa \tErr\n");
		}
	}
	// Stop all threads
	m_fPauseUSG_Thread = TRUE;
	m_bRunningIntEventThread = FALSE;

	//m_ptCheckEvent.EventType = &m_usEventType;
	m_usEventType = 65535;


	// Setup 
	QueryPerformanceFrequency(&m_liFreqOS);
	m_uiTimerFreqFactor_10KHz = 1;
	timeBeginPeriod(1);

}

USG_FSM::~USG_FSM()
{
	if(m_fptrDebug != NULL)
	{
		fclose(m_fptrDebug);
	}
	timeEndPeriod(1);
}

void USG_FSM::InitLPT()
{
#ifndef __USG_BY_DIGITAL_IO_1739U__
	mUSG_ParallelPort.init_port();
	// 20100111
	lpOutUSG = 0;
#endif
}

void USG_FSM::cusg_warm_reset()
{
	m_iCountUSG = 1;
	m_dDeltaAmp = 0;
	m_dCurrAmplitudeReg = 0;
	m_iCurrenAmplitudeOutUSG = 0;
	m_uiErrorFlagUSG = _USG_NO_ERR_ZERO;

	m_iDataReadTrigger = 0;
	m_uiIdxActiveBlk = 0;
	m_uiIdxActiveSeg = 0;  // by default it is first segment
	m_stpActiveBlk_USG = &stUltraSonicPowerBlk[m_uiIdxActiveBlk];
	m_stpCurrSegment_USG = &m_stpActiveBlk_USG->stUSG_Segment[0];

	m_uiPeriod_ms = 0;
	m_iCountTimer = 0;
	m_iCountSec = 0;
}

void USG_FSM::cusg_set_active_blk(unsigned int uiBlkIdx)
{
	m_uiIdxActiveBlk = uiBlkIdx;
	m_stpActiveBlk_USG = &stUltraSonicPowerBlk[uiBlkIdx];
	// by default, start from segment 0 whenever set the block // 20081009 
	m_uiIdxActiveSeg = 0;  
	m_stpCurrSegment_USG = &m_stpActiveBlk_USG->stUSG_Segment[m_uiIdxActiveSeg];
}

void USG_FSM::cusg_get_curr_blk_seg(unsigned int *uiBlkIdx, unsigned int *uiSegIdx)
{
	*uiBlkIdx = m_uiIdxActiveBlk;
	*uiSegIdx = m_uiIdxActiveSeg;
}

unsigned int USG_FSM:: cusg_get_active_blk()
{
	return m_uiIdxActiveBlk;
}

inline void USG_FSM::cusg_goto_next_seg_w_prot()
{
	// match trigger pattern, goto next segment directly
	m_iCountUSG = 1;  // clear counter for next status
	m_uiIdxActiveSeg = m_uiIdxActiveSeg + 1;
	if(m_uiIdxActiveSeg >= m_stpActiveBlk_USG->uiTotalNumActiveSegment || m_uiIdxActiveSeg >= _USG_MAX_NUM_SEGMENT)
	{
		m_uiIdxActiveSeg = 0;
		m_uiErrorFlagUSG = _USG_ERR_OUT_SEGMENT_RANGE;
	}
	else
	{
		m_stpActiveBlk_USG->uiNextStatusUSG = _USG_FLAG_NULL;
	}
}
inline void USG_FSM::cusg_goto_next_block_seg0()
{
	m_uiIdxActiveBlk = m_stpActiveBlk_USG->uiNextBlkIdx;
	m_stpActiveBlk_USG = &stUltraSonicPowerBlk[m_uiIdxActiveBlk];
	m_stpActiveBlk_USG->uiNextStatusUSG = _USG_FLAG_NULL;
	m_dCurrAmplitudeReg = 0;
	m_iCurrenAmplitudeOutUSG = 0;
	m_uiIdxActiveSeg = 0;
	m_iCountUSG = 1;  // clear counter for next status
}
// cusg_calc_ramp_delta_w_prot_div0_to_level
inline void USG_FSM::cusg_calc_ramp_delta_w_prot_div0_to_level(unsigned int uiNextLevelStatus)
{
	if(m_stpCurrSegment_USG->iUSG_RampTime > 0)
	{
		m_dDeltaAmp = (m_stpCurrSegment_USG->iUSG_Amplitude - m_iCurrenAmplitudeOutUSG)
			/ (double) m_stpCurrSegment_USG->iUSG_RampTime;
		m_dCurrAmplitudeReg = m_dCurrAmplitudeReg + m_dDeltaAmp;
		m_iCurrenAmplitudeOutUSG = (int)m_dCurrAmplitudeReg;
	}
	else  // protection not to be divided by zero
	{ 
		m_dDeltaAmp = 0;
		m_stpActiveBlk_USG->uiNextStatusUSG = uiNextLevelStatus;
		m_iCurrenAmplitudeOutUSG = m_stpCurrSegment_USG->iUSG_Amplitude;
		m_dCurrAmplitudeReg = (double)m_iCurrenAmplitudeOutUSG;
	}
}

inline void USG_FSM::cusg_exec_ramp_outpower_xor_switolevel(unsigned int uiNextLevelStatus)
{
	if(m_iCountUSG <= m_stpCurrSegment_USG->iUSG_RampTime)
	{
		m_dCurrAmplitudeReg = m_dCurrAmplitudeReg + m_dDeltaAmp;
		m_iCurrenAmplitudeOutUSG = (int)m_dCurrAmplitudeReg;
		// TBA: output m_iCurrenAmplitudeOutUSG to m_stpActiveBlk_USG->uiUSG_ExciteAddress;
		//m_stpPCI_ADS_WritePortByte->state = m_iCurrenAmplitudeOutUSG;
		//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,	(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
#ifdef  __USG_BY_DIGITAL_IO_1739U__
		theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
		mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif
	}
	else
	{
		m_stpActiveBlk_USG->uiNextStatusUSG = uiNextLevelStatus;
	}
}
// OnTimer
//void USG_FSM::OnTimer(UINT uiTimerVal)
//{
//}
//
UINT USG_ThreadProc( LPVOID pParam )
{
    USG_FSM* pObject = (USG_FSM *)pParam;
	return pObject->USG_ThreadWin32wTiming(); 	
}

void USG_FSM::RunUSG_Thread()
{
	m_fPauseUSG_Thread = FALSE;
	m_pWinThreadTiming = AfxBeginThread(USG_ThreadProc, this);
	SetPriorityClass(m_pWinThreadTiming->m_hThread, REALTIME_PRIORITY_CLASS);
	SetThreadPriority(m_pWinThreadTiming->m_hThread, THREAD_PRIORITY_TIME_CRITICAL);
	m_pWinThreadTiming->m_bAutoDelete = FALSE;
}

void USG_FSM::StopUSG_Thread()
{
	if (m_fPauseUSG_Thread == FALSE)
	{
		m_fPauseUSG_Thread = TRUE;
		WaitForSingleObject(m_pWinThreadTiming->m_hThread, 10); // 20081001, 10000); This function must be called.
		// TerminateThread(m_pWinThreadTiming->m_hThread, 0); // 20081001 this function may cause trouble
		delete m_pWinThreadTiming;
		m_pWinThreadTiming = NULL;
	}
}

static unsigned int uiFlagExecutingUSG_Thread;  // 20081021
void USG_FSM::PauseUSG_Thread()
{
	if(m_fPauseUSG_Thread == FALSE) // 20081021
	{
		m_fPauseUSG_Thread = TRUE;
		while(uiFlagExecutingUSG_Thread == 1)
		{
			Sleep(10);
		}
	}
}
void USG_FSM::ResumeUSG_Thread()
{
	m_fPauseUSG_Thread = FALSE;
	AfxBeginThread(USG_ThreadProc, this);
}

static unsigned int uiCounterLast;
static unsigned int uiCounterCurr;
extern int GetPentiumTimeCount_per_100us(unsigned __int64 frequency);

UINT USG_FSM::USG_ThreadWin32wTiming()
{
	unsigned int ii;
	uiCounterLast = GetPentiumTimeCount_per_100us(m_liFreqOS.QuadPart); // timeGetTime();
	uiFlagExecutingUSG_Thread = 1;
	while(!m_fPauseUSG_Thread)
	{
		uiCounterCurr = GetPentiumTimeCount_per_100us(m_liFreqOS.QuadPart); // timeGetTime();
		if(uiCounterCurr - uiCounterLast >= m_uiTimerFreqFactor_10KHz 
			||	( (double) uiCounterCurr + UINT_MAX - uiCounterLast >= m_uiTimerFreqFactor_10KHz 
			     && (double)uiCounterLast + m_uiTimerFreqFactor_10KHz > UINT_MAX
				 )
		  ) // if 
		{
// Call USG finite state machine
			iUSG_Semaphor_DoingFSM = 1;
			USG_GenOnce();
			iUSG_Semaphor_DoingFSM = 0;
// Call USG finite state machine
			uiCounterLast = uiCounterCurr;
		}
		for(ii = 0; ii< 20; ii++)
		{
			Sleep(0);     // Get a slice of timing resource.
		}
// Never call Sleep() if precision is required for 2ms or below, //		Sleep(1);
	}
	// do we need to call system handler, in order to save timing, donot call at this moment

	uiFlagExecutingUSG_Thread = 0;
	return 0;
}

// Routines related with Interrupt Event Thread
UINT USG_IntEventThreadProc( LPVOID pParam )
{
    USG_FSM* pObject = (USG_FSM *)pParam;
	return pObject->IntEventThread(); 	
}

void USG_FSM::StartIntEventThread()
{
//	PT_EnableEvent	EventSetting;
	//EventSetting.EventType = ADS_EVT_INTERRUPT;
	//EventSetting.Enabled = TRUE;
	//EventSetting.Count = 1;
	//m_ErrCde = DRV_EnableEvent( m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle, &EventSetting );
//	if ( m_ErrCde != 0) {
//		DRV_GetErrorMessage( m_ErrCde, (LPSTR)m_szErrMsg );
////		MessageBox( (LPCSTR)m_szErrMsg, "Driver Message", MB_OK );
//		return;
//	}
	theDigitalIO.EnableInterruptEvent(TRUE); // 20081008, do not check OK or not, refer to DigitalIO.cpp
	// Using THREAD_PRIORITY_TIME_CRITICAL for interrupt event handling routine,
	// you can get about 20 KHz response ratio by using PENTIUM 200 MMX PC.
	m_pThreadIntPCI = AfxBeginThread(USG_IntEventThreadProc, this, THREAD_PRIORITY_TIME_CRITICAL);
//	SetPriorityClass(m_pThreadIntPCI->m_hThread, REALTIME_PRIORITY_CLASS);
//	SetThreadPriority(m_pThreadIntPCI->m_hThread);
	m_pThreadIntPCI->m_bAutoDelete = FALSE;

	m_bRunningIntEventThread = TRUE;

}

void USG_FSM::StopIntEventThread()
{
//	PT_EnableEvent	EventSetting;
	DWORD ExitCode ;
	//BOOL flag = FALSE;

	//EventSetting.EventType = ADS_EVT_INTERRUPT;
	//EventSetting.Enabled = FALSE;
	//EventSetting.Count = 1;
	//m_ErrCde = DRV_EnableEvent( m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle, &EventSetting );
	theDigitalIO.EnableInterruptEvent(FALSE); // 20081008 do not check OK or not, refer to DigitalIO.cpp

	if(m_bRunningIntEventThread)
	{
		WaitForSingleObject(m_pThreadIntPCI->m_hThread, 10000);
		delete m_pThreadIntPCI;

		GetExitCodeThread(m_pThreadIntPCI->m_hThread,&ExitCode);
		if(ExitCode==STILL_ACTIVE)
			TerminateThread(m_pThreadIntPCI->m_hThread,ExitCode);

		m_pThreadIntPCI = NULL;
		m_bRunningIntEventThread = FALSE;
	}
}


UINT USG_FSM::IntEventThread()
{
	while(m_bRunningIntEventThread)
	{
		if ( theDigitalIO.CheckInterruptEvent(&m_usEventType) == SUCCESS)// DRV_CheckEvent(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle , &m_ptCheckEvent ) == SUCCESS)
		{
			if(m_usEventType==ADS_EVT_INTERRUPT)
			{
				iUSG_Semaphor_DoingFSM = 1;
				USG_GenOnce();
				iUSG_Semaphor_DoingFSM = 0;
			}
		}
	}
	return 0;
}

void USG_FSM::PauseIntEventThread() // 20081009
{
	m_bRunningIntEventThread = FALSE;
}

void USG_FSM::ResumeIntEventThread() // 20081009
{
	m_bRunningIntEventThread = TRUE;
	AfxBeginThread(USG_IntEventThreadProc, this, THREAD_PRIORITY_TIME_CRITICAL);
}

//USG: Ultrasonic Generator, finite state machine
void USG_FSM::USG_GenOnce()
{

	//m_stpPCI_ADS_WriteBit->state = 0;
	//DRV_DioWriteBit(m_stPCI_ADS_BitOutTest.DriverHandle, m_stpPCI_ADS_WriteBit);
	theDigitalIO.UsgWriteOutputBit(0); // 20081008

	if(m_uiErrorFlagUSG == _USG_NO_ERR_ZERO)
	{ // if not equal to DISABLE
		m_stpCurrSegment_USG = &m_stpActiveBlk_USG->stUSG_Segment[m_uiIdxActiveSeg]; // setup pointer for fast access

		// According to CmdType
		switch(m_stpCurrSegment_USG->uiCmdType)
		{
		case _USG_CMD_WAIT_TRIGGER:
			if(m_stpActiveBlk_USG->uiCurrStatusUSG != _USG_FLAG_WAIT_TRIG_RAMP && m_stpActiveBlk_USG->uiCurrStatusUSG != _USG_FLAG_WAIT_TRIG_LEVEL)
			{
				m_stpActiveBlk_USG->uiNextStatusUSG = _USG_FLAG_WAIT_TRIG_RAMP;
				m_iCountUSG = 1;
			}
			break;
		case _USG_CMD_RAMP_STEP:
			if(m_stpActiveBlk_USG->uiCurrStatusUSG != _USG_FLAG_RAMPSTEP_RAMP && m_stpActiveBlk_USG->uiCurrStatusUSG != _USG_FLAG_RAMPSTEP_LEVEL)
			{
				m_stpActiveBlk_USG->uiNextStatusUSG = _USG_FLAG_RAMPSTEP_RAMP;
				m_iCountUSG = 1;
			}
			break;
		case _USG_CMD_END_GOTO_NEXT_BLK:
			m_stpActiveBlk_USG->uiNextStatusUSG = _USG_FLAG_ENDSEG_GOTO_NEXT_BLK;
			break;
		}

		unsigned int uiRandIdx = (unsigned int)(rand()/(double)RAND_MAX * 16) ;
		// According to Status, m_stpActiveBlk_USG->uiNextStatusUSG and m_stpActiveBlk_USG->uiCurrStatusUSG
		if(m_stpActiveBlk_USG->uiNextStatusUSG != m_stpActiveBlk_USG->uiCurrStatusUSG)
		{ // First sample in the state of current status
			m_stpActiveBlk_USG->uiCurrStatusUSG = m_stpActiveBlk_USG->uiNextStatusUSG;

			switch(m_stpActiveBlk_USG->uiCurrStatusUSG)
			{
				case _USG_FLAG_WAIT_TRIG_RAMP:         //  1
				// Read data from trigger address
				// TBA:  m_stpActiveBlk_USG->m_iDataReadTrigger = read_trigger_address() // from SPii 
					//m_iDataReadTrigger = iDebugTriggerData; // Only for off-line logic test
//#ifdef _DEBUG
					if(theAcsServo.GetServoOperationMode() == OFFLINE_MODE)
					{
						m_iDataReadTrigger = iaDebugTriggerRandDataRange[uiRandIdx];
					}
					else
					{
//#else
					// donot call //acsc_ReadDPRAMInteger(m_hHandle, m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
						theAcsServo.ReadDpramInteger(m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
					}
					
//#endif
				// compare with trigger pattern
					if(m_iDataReadTrigger  == m_stpCurrSegment_USG->iUSG_TriggerPattern)
					{
						cusg_goto_next_seg_w_prot();
					}
					else
					{
						cusg_calc_ramp_delta_w_prot_div0_to_level(_USG_FLAG_WAIT_TRIG_LEVEL);
						// TBA: output m_iCurrenAmplitudeOutUSG to m_stpActiveBlk_USG->uiUSG_ExciteAddress;
						//m_stpPCI_ADS_WritePortByte->state = m_iCurrenAmplitudeOutUSG;
						//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
#ifdef  __USG_BY_DIGITAL_IO_1739U__
						theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
						mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif

					}
					break;

				case _USG_FLAG_WAIT_TRIG_LEVEL:// 2
				// Read data from trigger address
				// TBA:  m_stpActiveBlk_USG->m_iDataReadTrigger = read_trigger_address() // from SPii 
					//m_iDataReadTrigger = iDebugTriggerData; // Only for off-line logic test
//#ifdef _DEBUG
					if(theAcsServo.GetServoOperationMode() == OFFLINE_MODE)
					{
						m_iDataReadTrigger = iaDebugTriggerRandDataRange[uiRandIdx];
					}
					else
					{
//#else
					// acsc_ReadDPRAMInteger(m_hHandle, m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
						theAcsServo.ReadDpramInteger(m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
					}
//#endif

				// compare with trigger pattern
					if(m_iDataReadTrigger  == m_stpCurrSegment_USG->iUSG_TriggerPattern)
					{
						cusg_goto_next_seg_w_prot();
					}
					else
					{
						m_iCurrenAmplitudeOutUSG = m_stpCurrSegment_USG->iUSG_Amplitude;
						m_dCurrAmplitudeReg = (double)m_iCurrenAmplitudeOutUSG;
						// TBA: output m_iCurrenAmplitudeOutUSG to m_stpActiveBlk_USG->uiUSG_ExciteAddress;
						//m_stpPCI_ADS_WritePortByte->state = m_iCurrenAmplitudeOutUSG;
						//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle, (LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
#ifdef  __USG_BY_DIGITAL_IO_1739U__
						theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
						mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif

					}
					break;
				case _USG_FLAG_RAMPSTEP_RAMP:          // 3
					cusg_calc_ramp_delta_w_prot_div0_to_level(_USG_FLAG_RAMPSTEP_LEVEL);
					// TBA: output m_iCurrenAmplitudeOutUSG to m_stpActiveBlk_USG->uiUSG_ExciteAddress;
					//m_stpPCI_ADS_WritePortByte->state = m_iCurrenAmplitudeOutUSG;
					//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,	(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
#ifdef  __USG_BY_DIGITAL_IO_1739U__
					theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
					mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif

					break;
				case _USG_FLAG_RAMPSTEP_LEVEL:		  // 4
					// output power, only once
					m_iCurrenAmplitudeOutUSG = m_stpCurrSegment_USG->iUSG_Amplitude;
					m_dCurrAmplitudeReg = (double)m_iCurrenAmplitudeOutUSG;
					// TBA: output m_dCurrAmplitudeReg to m_stpActiveBlk_USG->uiUSG_ExciteAddress;
					//m_stpPCI_ADS_WritePortByte->state = m_iCurrenAmplitudeOutUSG;
					//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
#ifdef  __USG_BY_DIGITAL_IO_1739U__
					theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
					mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif

					break;
				case _USG_FLAG_ENDSEG_GOTO_NEXT_BLK: // 5
					cusg_goto_next_block_seg0();
					break;
				case _USG_FLAG_NULL: // 0
				default:
					// TBA: output 0 to m_stpActiveBlk_USG->uiUSG_ExciteAddress; // output power only once
					//m_stpPCI_ADS_WritePortByte->state = 0;
					//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,	(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
			//		theDigitalIO.UsgWriteOutputByte(0); // 20081008
					m_iCurrenAmplitudeOutUSG = 0;
#ifdef  __USG_BY_DIGITAL_IO_1739U__
					theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
					mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif

					break;
			}
		}
		else 
		{// not the first time
			switch(m_stpActiveBlk_USG->uiCurrStatusUSG)
			{
				case _USG_FLAG_WAIT_TRIG_RAMP:          // 1
				// Read data from trigger address
				// TBA:  m_stpActiveBlk_USG->m_iDataReadTrigger = read_trigger_address() // from SPii 
					//m_iDataReadTrigger = iDebugTriggerData; // Only for off-line logic test
//#ifdef _DEBUG
					if(theAcsServo.GetServoOperationMode() == OFFLINE_MODE)
					{
						m_iDataReadTrigger = iaDebugTriggerRandDataRange[uiRandIdx]; // Only for off-line logic test
					}
					else
					{
//#else
					// acsc_ReadDPRAMInteger(m_hHandle, m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
						theAcsServo.ReadDpramInteger(m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
					}
//#endif

				// compare with trigger pattern
					if(m_iDataReadTrigger  == m_stpCurrSegment_USG->iUSG_TriggerPattern)
					{
						cusg_goto_next_seg_w_prot();
					}
					else
					{
						cusg_exec_ramp_outpower_xor_switolevel(_USG_FLAG_WAIT_TRIG_LEVEL);
					}
					break;
				case _USG_FLAG_WAIT_TRIG_LEVEL:		  // 2
				// Read data from trigger address
				// TBA:  m_stpActiveBlk_USG->m_iDataReadTrigger = read_trigger_address() // from SPii 
					//m_iDataReadTrigger = iDebugTriggerData; // Only for off-line logic test
//#ifdef _DEBUG
					if(theAcsServo.GetServoOperationMode() == OFFLINE_MODE)
					{
						m_iDataReadTrigger = iaDebugTriggerRandDataRange[uiRandIdx]; // Only for off-line logic test
					}
					else
					{
//#else
					// acsc_ReadDPRAMInteger(m_hHandle, m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
						theAcsServo.ReadDpramInteger(m_stpActiveBlk_USG->iTriggerAddress, &m_iDataReadTrigger);
					}
//#endif

				// compare with trigger pattern
					if(m_iDataReadTrigger  == m_stpCurrSegment_USG->iUSG_TriggerPattern)
					{
						cusg_goto_next_seg_w_prot();
					}
					else
					{
						if(m_iCountUSG >= m_stpCurrSegment_USG->iUSG_RampTime + m_stpCurrSegment_USG->iUSG_Duration_ms + (int)m_stpActiveBlk_USG->uiMaxWaitTriggerProt_cnt)
						{
							m_uiErrorFlagUSG = _USG_ERR_WAIT_TRIGGER_TIME_OUT;
							// TBA: output 0 to m_stpActiveBlk_USG->uiUSG_ExciteAddress; // output power only once
							//m_stpPCI_ADS_WritePortByte->state = 0;
							//DRV_DioWritePortByte(m_stPCI_ADS_ByteOut_TimerUSG.DriverHandle,	(LPT_DioWritePortByte)m_stpPCI_ADS_WritePortByte);
//							theDigitalIO.UsgWriteOutputByte(0);
							m_iCurrenAmplitudeOutUSG = 0;
#ifdef  __USG_BY_DIGITAL_IO_1739U__
							theDigitalIO.UsgWriteOutputByte(m_iCurrenAmplitudeOutUSG); // 20081008
#else
							mUSG_ParallelPort.pin_pattern(m_iCurrenAmplitudeOutUSG);   // 20100111
#endif
							sprintf_s(m_strTextDebug, 256, "%s", pstrErrorFlagUSG[m_uiErrorFlagUSG]);

						}
					}
					break;
				case _USG_FLAG_RAMPSTEP_RAMP:          // 3
					cusg_exec_ramp_outpower_xor_switolevel(_USG_FLAG_RAMPSTEP_LEVEL);
					break;
				case _USG_FLAG_RAMPSTEP_LEVEL:		  // 4
					if(m_iCountUSG >= m_stpCurrSegment_USG->iUSG_RampTime + m_stpCurrSegment_USG->iUSG_Duration_ms)
					{
						cusg_goto_next_seg_w_prot();
					}
					break;
				case _USG_FLAG_ENDSEG_GOTO_NEXT_BLK: // 5
					cusg_goto_next_block_seg0();
					break;
				case _USG_FLAG_NULL: // 0

					break;
				default: 
					break;
			}
		} // According to Status

		if(m_iFlagWriteDebugFile >= 1)
		{
			if(m_fptrDebug != NULL)
			{ //	fprintf(m_fptrDebug, "Cnt,\tBlk,\tSeg,\tCmd,\tdAmp,\tiAmp,\tCurrStatus,\tNextStatus \t \t \t \n");
				fprintf(m_fptrDebug, "%d, \t%d, \t%d, \t%s, \t%8.2f, \t%d, \t%s, \t%s,\t%d,\t%d, \t%s\n", 
					m_iCountUSG, m_uiIdxActiveBlk, m_uiIdxActiveSeg, pstrCmdTypeUSG[m_stpCurrSegment_USG->uiCmdType], 
					m_dCurrAmplitudeReg, m_iCurrenAmplitudeOutUSG, 
					pstrStatusUSG[m_stpActiveBlk_USG->uiCurrStatusUSG], pstrStatusUSG[m_stpActiveBlk_USG->uiNextStatusUSG], 
					m_stpCurrSegment_USG->iUSG_TriggerPattern, m_iDataReadTrigger,
					pstrErrorFlagUSG[m_uiErrorFlagUSG]);
			}
		}

		m_iCountUSG ++;  // increase counter for all cases

	}
	else // _USG_FLAG_NULL
	{ // do nothing
	}
	
	//m_stpPCI_ADS_WriteBit->state = 1;
	//DRV_DioWriteBit(m_stPCI_ADS_BitOutTest.DriverHandle, m_stpPCI_ADS_WriteBit);
	theDigitalIO.UsgWriteOutputBit(1); // 20081008

#ifdef _DEBUG_
	if(m_iShowSecondCounter >= 1)
	{
		m_iCountTimer ++;
		m_iCountSec = (int) (m_uiPeriod_ms * m_iCountTimer / 1000.0);
		switch( m_iCountSec % 2)
		{
		case 0:
			sprintf_s(m_strCounterFlag, 8, "|"); //GetDlgItem(IDC_USG_COUNTER)->SetWindowTextA(_T("|")); // 20081008
			break;
		case 1:
			sprintf_s(m_strCounterFlag, 8, "--"); // 20081008 //GetDlgItem(IDC_USG_COUNTER)->SetWindowTextA(_T("--"));
			break;
		}
		if(m_iCountSec == 2)
		{
			m_iCountSec = 0;
			m_iCountTimer = 0;
		}
	}
#endif

}


void USG_FSM::cusg_set_flag_write_file(int iFlag)
{
	m_iFlagWriteDebugFile = iFlag;
}

int USG_FSM::cusg_get_flag_write_file()
{
	return m_iFlagWriteDebugFile;
}

void USG_FSM::cusg_set_flag_second_counter(int iFlag)
{
	m_iShowSecondCounter = iFlag;
}

int USG_FSM::cusg_get_flag_second_counter()
{
	return m_iShowSecondCounter;
}

//void USG_FSM::DoDataExchange(CDataExchange* pDX) // 20081007 
//{
//	CWnd::DoDataExchange(pDX);
//}
//BEGIN_MESSAGE_MAP(USG_FSM, CWnd)
////		ON_WM_TIMER()
//END_MESSAGE_MAP()
//
//BOOL USG_FSM::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	int DlgWidth = GetSystemMetrics(SM_CXSCREEN);
//	int DlgHeight = GetSystemMetrics(SM_CYSCREEN);
//
//	SetWindowPos(&wndTopMost, DlgWidth - 150, DlgHeight -150, 50, 50, SWP_SHOWWINDOW);
//	
//	return TRUE; 
//}
// USG_FSM message handlers
//int USG_FSM::cusg_start_timer(int iTimerDuration_ms)
//{
//	
//	m_uiPeriod_ms = (unsigned int)iTimerDuration_ms;
//	m_uiTimerVal = SetTimer(_USG_IDT_TIMER_0, iTimerDuration_ms, 0);
//	
//	if (m_uiTimerVal == 0)
//	{
//		sprintf_s(m_strTextDebug, 256, "Unable to obtain timer");
//		return MTN_API_ERROR;
//	}
//	else
//	{
//		return MTN_API_OK_ZERO;
//	}
//}// end StartTimer

//void USG_FSM::cusg_stop_timer()
//{
//	// stop timer
//	if(m_uiTimerVal != 0)
//	{
//		KillTimer (m_uiTimerVal);
//	}
//}

//int USG_FSM::cusg_start_timer_from_blk(unsigned int uiTimerDuration_ms, unsigned int uiBlkIdx)
//{
//	m_uiIdxActiveBlk = uiBlkIdx;
//	
//	m_uiPeriod_ms = uiTimerDuration_ms;
//	m_uiTimerVal = SetTimer(_USG_IDT_TIMER_0, uiTimerDuration_ms, 0);
//	
//	if (m_uiTimerVal == 0)
//	{
//		sprintf_s(m_strTextDebug, 256, "Unable to obtain timer");
//		return MTN_API_ERROR;
//	}
//	else
//	{
//		return MTN_API_OK_ZERO;
//	}
//}  // 20081007 
