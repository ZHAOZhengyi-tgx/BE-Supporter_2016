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

// Module: Motion Stepper
// Template structure is based on ADS-PCI-1240
// History
// YYYYMMDD  Author    Notes
// 

#ifndef __MOTION_STEPPER__
#define __MOTION_STEPPER__

#include "mtnapi.h"

typedef struct
{
	MTN_SPEED_PROFILE astSpeedProfileStepperAxis[MAX_STEPPER_AXIX_ON_BOARD];
	MTN_POSITION_REGISTRATION astPositionRegistration[MAX_STEPPER_AXIX_ON_BOARD];
}STEPPER_SPEED_PROFILE_PER_BOARD;


int mtnstp_upload_parameters_ads1240(BYTE uiCurrBoardId, DWORD *ulErrCode);
int mtnstp_download_parameters_ads1240(BYTE uiCurrBoardId, DWORD *ulErrCode);

int mtnstp_upload_parameters_leetro_2812(BYTE uiCurrBoardId, DWORD *ulErrCode);
int mtnstp_download_parameters_leetro_2812(BYTE uiCurrBoardId, DWORD *ulErrCode);

#define WB_WH_INNER_CLAMPER_STEPPER   0
#define WB_WH_OUTER_CLAMPER_STEPPER   1
#define WB_WH_ONLOADER_STEPPER   2
void mtnstp_wb_set_home_para(unsigned int uiCurrBoardId, unsigned long *ulErrCode);

extern STEPPER_SPEED_PROFILE_PER_BOARD astStepperSpeedProfilePerBoard[];

#endif // __MOTION_STEPPER__