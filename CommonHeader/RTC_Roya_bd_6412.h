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
// Project header file, access both by host and by Roya-Board-6412
// Real Time Communication via ethernet
// 
#ifndef __RTC_ROYA_BD_6412__
#define __RTC_ROYA_BD_6412__

#define PERIOD_2KHZ_ON_ROYA_BOARD_6412    30000
#define PERIOD_5KHZ_ON_ROYA_BOARD_6412    30000
#define PERIOD_20KHZ_ON_ROYA_BOARD_6412   3000
#define PERIOD_150KHZ_ON_ROYA_BOARD_6412  533
#define PERIOD_250KHZ_ON_ROYA_BOARD_6412  320


#define _RTC_ETH_MAX_BUFF_LEN_BYTE       256
#define _RTC_ROYA_MAX_NUM_CMD            0xFF

#define  __MAX_FLASH_READ_LEN    128

//// ETH Macro Define
#define _RTC_ROYA_OFST_CMD_TYPE          0x0
#define _RTC_ROYA_OFST_CMD_CODE          0x1
#define _RTC_ROYA_OFST_CMD_LEN           0x2
#define _RTC_ROYA_OFST_RESV_1            0x3
#define _RTC_ROYA_OFST_RESV_2            0x4
#define _RTC_ROYA_OFST_DATA_START        0x5

// Command Type List
#define _RTC_ROYA_CMD_TYPE_TEST_COMM    0x0
#define _RTC_ROYA_CMD_TYPE_USG_PROF     0x1
#define _RTC_ROYA_CMD_TYPE_BSD          0x2
#define _RTC_ROYA_CMD_TYPE_SIGNAL_GEN   0x3
#define _RTC_ROYA_CMD_TYPE_BQM          0x4


// Command Code list
// 1. Communication Test
#define _RTC_ROYA_TEST_COMMUNICATION     0x00
#define _RTC_ROYA_GET_VERSION            0x01
#define _RTC_ROYA_TEST_DIGITAL_OUT_LCD   0x02
#define _RTC_ROYA_TEST_DAC               0x03
#define _RTC_ROYA_TEST_ADC               0x04
#define _RTC_ROYA_SET_TIMER_PERIOD       0x05
#define _RTC_ROYA_GET_RT_CLOCK           0x06
#define _RTC_ROYA_SET_RT_CLOCK           0x07
#define _RTC_ROYA_GET_TIMER_PERIOD       0x08
#define _RTC_ROYA_GET_KEY_INT_COUNTER    0x09
#define _RTC_ROYA_READ_FLASH             0x0A
#define _RTC_ROYA_WRITE_FLASH            0x0B


// Command Code list
// 2. USG Profile command
#define _RTC_ROYA_USG_BLK_SET_SEGMENT         0x0
#define _RTC_ROYA_USG_BLK_GET_SEGMENT         0x1
#define _RTC_ROYA_USG_BLK_SET_TRIGGER_ADDRESS 0x2
#define _RTC_ROYA_USG_BLK_GET_TRIGGER_ADDRESS 0x3
#define _RTC_ROYA_USG_BLK_SET_NEXTBLK_INDEX   0x4
#define _RTC_ROYA_USG_BLK_GET_NEXTBLK_INDEX   0x5
#define _RTC_ROYA_USG_BLK_SET_MAX_COUNT_WAIT_TRIG_PROT     0x6
#define _RTC_ROYA_USG_BLK_GET_MAX_COUNT_WAIT_TRIG_PROT     0x7
#define _RTC_ROYA_USG_BLK_SET_START_WITH_BLOCK             0x8
#define _RTC_ROYA_USG_BLK_GET_START_WITH_STATUS            0x9
#define _RTC_ROYA_USG_BLK_WARM_RESET                       0xA

// Command Code list
// 3. BSD command (Non-Stick Detection)
#define _RTC_ROYA_BSD_SET_PARAMETER          0x0
#define _RTC_ROYA_BSD_GET_PARAMETER          0x1
#define _RTC_ROYA_BSD_START_BSD              0x2
#define _RTC_ROYA_BSD_WARM_RESET             0x3
#define _RTC_ROYA_BSD_GET_STATUS             0x4
#define _RTC_ROYA_BSD_GET_DATA               0x5
#define _RTC_ROYA_BSD_GET_DATA_FROM_LEN      0x6
#define _RTC_ROYA_BSD_EFO_GET_GOOD_FLAG      0x7
#define _RTC_ROYA_BSD_FIRMWARE_ANA_STICKNESS 0x8
#define _RTC_ROYA_BSD_SET_PARA_FIRMWARE_ANA_STICKNESS 0x9
#define _RTC_ROYA_BSD_GET_PARA_FIRMWARE_ANA_STICKNESS 0xA
#define _RTC_ROYA_BSD_GET_OUTPUT_FIRMWARE_ANA_STICKNESS 0xB
//#define _RTC_ROYA_BSD_GET_COUNTERS           0xC

// Command Code for Signal Generator

#define _RTC_SIGGEN_SET_NEW_CYCLE            0x0
#define _RTC_SIGGEN_GET_CURR_CYCLE           0x1
#define _RTC_SIGGEN_SET_NEW_AMPLITUDE        0x2
#define _RTC_SIGGEN_GET_CURR_AMPLITUDE       0x3
#define _RTC_SIGGEN_SET_START_FLAG_CALC_SINE 0x4
#define _RTC_SIGGEN_GET_CALC_SINE_FLAG		 0x5
#define _RTC_SIGGEN_SET_NEW_FRAC_CYCLE	 	 0x6
#define _RTC_SIGGEN_GET_CURR_SINE_OUT_CFG	 0x7


//// Command Code for BQM: Bonding Qualify Monitoring
#define _RTC_BQM_SET_PRE_COUNTER_TH            0x0
#define _RTC_BQM_SET_MAX_COUNTER_ADC           0x1
#define _RTC_BQM_GET_STATUS                    0x2
#define _RTC_BQM_GET_BUFFER_BH_C_BLK_FROM_LEN   0x3
#define _RTC_BQM_GET_BUFFER_PWR_V_BLK_FROM_LEN   0x4
#define _RTC_BQM_GET_BUFFER_PWR_C_BLK_FROM_LEN   0x5

#include "Roya_Version.h"

#endif // __RTC_ROYA_BD_6412__

