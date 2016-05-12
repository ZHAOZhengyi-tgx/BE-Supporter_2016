// History
// YYYYDDMM  Author  Notes
// 20081005  Zhengyi Create, first release, OS-Timing and Thread soft interrupt
// 20081007  Zhengyi class inherits from CObject
// 20100122  Add Init_Port
#pragma once

//	unsigned int m_uiErrorFlagHoldTrig_Null;
#include "DigitalIO.h"
#include "MtnApi.h"
#include "WB_USG_BLK.h"

//#include "resource.h"
// release for global access

#define _USG_DEBUG_FILE_NAME   "MtnUSG_DEBUG.txt"

// WB_USG_FSM dialog

class WB_USG_FSM : public CObject
{
//	DECLARE_DYNAMIC(WB_USG_FSM),  // 20081007

public:
	WB_USG_FSM(); // HANDLE Handle);   // standard constructor, , CWnd* pParent = NULL), // 20081007 
	virtual ~WB_USG_FSM();

	int m_iCountTimer;
	int m_iCountSec;
	int m_uiPeriod_ms;
	UINT_PTR    m_uiTimerVal;

	int m_iCountUSG;
	int m_iDataReadTrigger;
	double m_dDeltaAmp, m_dCurrAmplitudeReg;
	int m_iCurrenAmplitudeOutUSG;  // only lower 8 bits are valid, 0 to 255
	unsigned int m_uiErrorFlagUSG;

	char m_strTextDebug[256];
	FILE *m_fptrDebug;
	char m_strCounterFlag[8];

	MTN_USG_BLK* m_stpActiveBlk_USG;
	USG_UNIT_SEG *m_stpCurrSegment_USG; 

	// Excitation Address Through PCI
//	PCI_ADS_IO_WRITE_BYTE  m_stPCI_ADS_ByteOut_TimerUSG;
//	PT_DioWritePortByte *m_stpPCI_ADS_WritePortByte;

	// Bit for timing test
//	PCI_ADS_IO_WRITE_BIT m_stPCI_ADS_BitOutTest;
//	PT_DioWriteBit *m_stpPCI_ADS_WriteBit;

	void InitLPT();  // 20100122
	void cusg_set_active_blk(unsigned int uiBlkIdx);
	unsigned int cusg_get_active_blk();
	void cusg_get_curr_blk_seg(unsigned int *uiBlkIdx, unsigned int *uiSegIdx);
	void cusg_warm_reset();

	// inline function, called by Finite State Machine, OnTimer
	inline void cusg_goto_next_seg_w_prot();
	inline void cusg_goto_next_block_seg0();
	inline void cusg_calc_ramp_delta_w_prot_div0_to_level(unsigned int uiNextLevelStatus);
	inline void cusg_exec_ramp_outpower_xor_switolevel(unsigned int uiNextLevelStatus);

	void USG_GenOnce();

	// Thread
	CWinThread* m_pWinThreadTiming;
	BOOL m_fPauseUSG_Thread;
	void RunUSG_Thread();
	void StopUSG_Thread();
	UINT USG_ThreadWin32wTiming();
	void PauseUSG_Thread();
	void ResumeUSG_Thread();

	// Interrupt Thread
	CWinThread*     m_pThreadIntPCI;
	void StartIntEventThread();
	void StopIntEventThread();
	UINT IntEventThread();
	void PauseIntEventThread();
	void ResumeIntEventThread();
	BOOL m_bRunningIntEventThread;
	unsigned short m_usEventType;


	// OS Timing // windows
	LARGE_INTEGER m_liFreqOS; 
	unsigned int m_uiTimerFreqFactor_10KHz;

	// debug purpose
	void cusg_set_flag_write_file(int iFlag);
	int cusg_get_flag_write_file();
	void cusg_set_flag_second_counter(int iFlag);
	int cusg_get_flag_second_counter();

// Dialog Data
	//enum { IDD = IDD_USG_TIMER_DIALOG }; donot use any windows UI resource
	//virtual BOOL OnInitDialog();

protected:
	LRESULT m_ErrCde;				// Return error code
	char	m_szErrMsg[80];			// Use for MESSAGEBOX function


	unsigned int m_uiIdxActiveBlk;
	unsigned int m_uiIdxActiveSeg;

	int m_iFlagWriteDebugFile;
	int m_iShowSecondCounter;

};

// Donot use windows Timer
	//int cusg_start_timer(int iTimerDuration_ms);
	//int cusg_start_timer_from_blk(unsigned int uiTimerDuration_ms, unsigned int uiBlkIdx);
	//void cusg_stop_timer();
//	DECLARE_MESSAGE_MAP()
//public:
//	afx_msg void OnTimer(UINT TimerVal);
