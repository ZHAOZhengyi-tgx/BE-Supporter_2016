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

// 20100301 Port from *USG_BLK.h, consolidate, WB_USG_BLK.h and USG_BLK.h

#ifndef __USG_BLK_DEF
#define __USG_BLK_DEF

#define _USG_MAX_NUM_BLK      2  // Usually one block for one point

#define _USG_MAX_NUM_SEGMENT  4 //128 // Each block has maxiumum 128 segments

#define _USG_IDT_TIMER_0  WM_USER + 200  
#define _USG_IDT_TIMER_1  (_USG_IDT_TIMER_0 + 1)

#define _DEF_DPRAM_TRIG_ADDRESS   0x90
// USG Status
#define _USG_FLAG_NULL                  0
#define _USG_FLAG_WAIT_TRIG_RAMP        1
#define _USG_FLAG_WAIT_TRIG_LEVEL       2
#define _USG_FLAG_RAMPSTEP_RAMP 	    3
#define _USG_FLAG_RAMPSTEP_LEVEL	    4
#define _USG_FLAG_ENDSEG_GOTO_NEXT_BLK  5

// Error flag
#define _USG_NO_ERR_ZERO                 0
#define _USG_ERR_WAIT_TRIGGER_TIME_OUT   1
#define _USG_ERR_OUT_SEGMENT_RANGE       2

// USG Command, set in the uiCmdType
#define _USG_CMD_WAIT_TRIGGER		0
#define _USG_CMD_RAMP_STEP			1
#define _USG_CMD_END_GOTO_NEXT_BLK  2

// Hardware path
#define USG_FSM_CMD_HARDWARE_VIA_1739U    0
#define USG_FSM_CMD_HARDWARE_VIA_LPT1     1

// For any block, always start from seg#0 --> seg#uiTotalNumActiveSegment
// USG Segment Defination
typedef struct
{
	int iUSG_Duration_ms; //
	int iUSG_RampTime;
	unsigned int iUSG_TriggerPattern; //
	int iUSG_Amplitude;
	unsigned int uiCmdType;  //  one of the following USG_CMD_
}USG_UNIT_SEG;

// One wire has one MTN_USG_BLK,
// Totally two MTN_USG_BLKs for current wire and next wire,
// There is _USG_MAX_NUM_SEGMENT number of units for each MTN_USG_BLK
//  
//#include "PCI_ADS.h"

typedef struct
{
	unsigned int uiNextBlkIdx;
	unsigned int uiTotalNumActiveSegment;

	int iTriggerAddress;

	unsigned int uiCurrStatusUSG;
	unsigned int uiNextStatusUSG;
	
//	unsigned int uiUSG_ExciteAddress;

	USG_UNIT_SEG stUSG_Segment[_USG_MAX_NUM_SEGMENT];

	unsigned int uiMaxWaitTriggerProt_cnt; //
}MTN_USG_BLK;


#endif  // __USG_BLK_DEF
