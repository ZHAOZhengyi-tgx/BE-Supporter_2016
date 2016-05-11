// Ultra Sonic Generation, Data Block
// History
// YYYYMMDD  Author  Notes
// 20081008  Zhengyi Move from USG_FSM.cpp

#ifndef CHIP_6412
#include "stdafx.h"
#endif

#include "USG_BLK.h"


MTN_USG_BLK stUltraSonicPowerBlk[_USG_MAX_NUM_BLK];
static int iDebugTriggerData = 0xAB;

// F:\Zhengyi\Mywork\MtnTest_ATL\src\USG_FSM.cpp : implementation file

#ifdef _OUT_BY_PCI_DIO_
char *pstrCmdTypeUSG[] = {
   "WAIT_TRIGGER",
   "RAMP_STEP",
   "END_GOTO_NEXT_BLK"
};

char *pstrStatusUSG[]  = {
   "NULL",
   "WAIT_TRIG_RAMP",
   "WAIT_TRIG_LEVEL",
   "RAMPSTEP_RAMP",
   "RAMPSTEP_LEVEL",
   "ENDSEG_GOTO_NEXT_BLK"
	};

char *pstrErrorFlagUSG[] = {
   "NO_ERR_ZERO",
   "WAIT_TRIGGER_TIME_OUT",
   "OUT_SEGMENT_RANGE"
};

int iaDebugTriggerRandDataRange[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0xAB, 12, 13, 14, 15, 16};

// extern int iUSG_Semaphor_DoingFSM;
#endif


void init_usg_blk_seg()
{
	unsigned int ii, jj;
	// initialise the parameter USG block and all the segments

	for(ii=0; ii<_USG_MAX_NUM_BLK; ii++)
	{
		stUltraSonicPowerBlk[ii].uiCurrStatusUSG = _USG_FLAG_NULL;
		stUltraSonicPowerBlk[ii].uiNextStatusUSG = _USG_FLAG_NULL;

		stUltraSonicPowerBlk[ii].iTriggerAddress = _DEF_DPRAM_TRIG_ADDRESS;

		stUltraSonicPowerBlk[ii].uiTotalNumActiveSegment = 10;
		for(jj = 0; jj<_USG_MAX_NUM_SEGMENT; jj++)
		{
			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_Amplitude = 0;
			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_Duration_ms = 0; 
			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_RampTime = 1;       // to avoid divided by zero
			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].iUSG_TriggerPattern = 0;
			stUltraSonicPowerBlk[ii].stUSG_Segment[jj].uiCmdType = _USG_CMD_END_GOTO_NEXT_BLK;
		}

		stUltraSonicPowerBlk[ii].uiMaxWaitTriggerProt_cnt = 1000;
		if(ii < _USG_MAX_NUM_BLK - 1)
		{
			stUltraSonicPowerBlk[ii].uiNextBlkIdx = ii + 1;
		}
		else
		{
			stUltraSonicPowerBlk[ii].uiNextBlkIdx = 0;
		}
	}
}

void usg_blk_set_segment(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg)
{
	if(uiBlk < _USG_MAX_NUM_BLK && uiSeg < _USG_MAX_NUM_SEGMENT)
	{
		// 20081008 there are chances that data is written, during reading in the USG_FSM, this should be avoided by the user
		//while(iUSG_Semaphor_DoingFSM == 1) 
		//{   ;// semaphore detection, set parameter only when not using
		//}
		// donot verify the validity of the parameters
		USG_UNIT_SEG *stpStaticUSGSeg = &stUltraSonicPowerBlk[uiBlk].stUSG_Segment[uiSeg];
		stpStaticUSGSeg->iUSG_Duration_ms = stpUSG_Seg->iUSG_Duration_ms ;
		stpStaticUSGSeg->iUSG_RampTime = stpUSG_Seg->iUSG_RampTime ;
		stpStaticUSGSeg->iUSG_TriggerPattern = stpUSG_Seg->iUSG_TriggerPattern ;
		stpStaticUSGSeg->iUSG_Amplitude = stpUSG_Seg->iUSG_Amplitude ;
		stpStaticUSGSeg->uiCmdType = stpUSG_Seg->uiCmdType ;
		// there is chances that above 5 lines of coding be interrupted by the USG_FSM
	}
}

void usg_blk_get_segment(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg)
{
	if(uiBlk < _USG_MAX_NUM_BLK && uiSeg < _USG_MAX_NUM_SEGMENT)
	{
		USG_UNIT_SEG *stpStaticUSGSeg = &stUltraSonicPowerBlk[uiBlk].stUSG_Segment[uiSeg];
		// donot verify the validity of the user's pointer
		stpUSG_Seg->iUSG_Duration_ms = stpStaticUSGSeg->iUSG_Duration_ms ;
		stpUSG_Seg->iUSG_RampTime = stpStaticUSGSeg->iUSG_RampTime ;
		stpUSG_Seg->iUSG_TriggerPattern = stpStaticUSGSeg->iUSG_TriggerPattern ;
		stpUSG_Seg->iUSG_Amplitude = stpStaticUSGSeg->iUSG_Amplitude ;
		stpUSG_Seg->uiCmdType = stpStaticUSGSeg->uiCmdType ;
	}
}

void usg_blk_set_trigger_address(unsigned int uiBlk, int iAddr)
{
	if(uiBlk < _USG_MAX_NUM_BLK)
	{
		// 20081008 there are chances that data is written, during reading in the USG_FSM, this should be avoided by the user
		//while(iUSG_Semaphor_DoingFSM == 1) 
		//{   ;// semaphore detection, set parameter only when not using
		//}
		// donot verify the validity of the parameters
		MTN_USG_BLK *stpStaticUSG_BLK = &stUltraSonicPowerBlk[uiBlk];
		stpStaticUSG_BLK->iTriggerAddress = iAddr;
	}
}

int usg_blk_get_trigger_address(unsigned int uiBlk)
{
	// donot verify the validity of parameter,
	return stUltraSonicPowerBlk[uiBlk].iTriggerAddress;
}

void usg_blk_set_nextblk_index(unsigned int uiBlk, unsigned int uiNextBlk)
{
	if(uiBlk < _USG_MAX_NUM_BLK)
	{
		// 20081008 there are chances that data is written, during reading in the USG_FSM, this should be avoided by the user
		//while(iUSG_Semaphor_DoingFSM == 1) 
		//{   ;// semaphore detection, set parameter only when not using
		//}
		// donot verify the validity of the parameters
		MTN_USG_BLK *stpStaticUSG_BLK = &stUltraSonicPowerBlk[uiBlk];
		stpStaticUSG_BLK->uiNextBlkIdx = uiNextBlk;
	}
}

unsigned int usg_blk_get_nextblk_index(unsigned int uiBlk)
{
	return stUltraSonicPowerBlk[uiBlk].uiNextBlkIdx;
}

void usg_blk_set_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int uiWaitCnt)
{
	if(uiBlk < _USG_MAX_NUM_BLK)
	{
		// 20081008 there are chances that data is written, during reading in the USG_FSM, this should be avoided by the user
		//while(iUSG_Semaphor_DoingFSM == 1) 
		//{   ;// semaphore detection, set parameter only when not using
		//}
		// donot verify the validity of the parameters
		MTN_USG_BLK *stpStaticUSG_BLK = &stUltraSonicPowerBlk[uiBlk];
		stpStaticUSG_BLK->uiMaxWaitTriggerProt_cnt = uiWaitCnt;
	}
}

unsigned int usg_blk_get_max_count_wait_trig_prot(unsigned int uiBlk)
{
	return stUltraSonicPowerBlk[uiBlk].uiMaxWaitTriggerProt_cnt;
}

