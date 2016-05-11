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


#ifndef __TWIN_SINE_H__
#define __TWIN_SINE_H__

#include "mtncalc.h"

//Structure Define for function prototype.
//Input
typedef struct
{
MOT_ALGO_DBL fDist;
MOT_ALGO_DBL fAccMax; // fAVmd_time;
MOT_ALGO_DBL fVelMax;
MOT_ALGO_DBL fJmax;
MOT_ALGO_DBL fT1;
MOT_ALGO_DBL fT2;
int   iTimeMoveFlag;

}CALC_TWINSINE_PRECAL_INPUT; 
//Structure for output
typedef struct
{
MOT_ALGO_DBL fT1;
MOT_ALGO_DBL fT2;
MOT_ALGO_DBL fC1;
MOT_ALGO_DBL fC2;
MOT_ALGO_DBL fC3;
MOT_ALGO_DBL fC4;
MOT_ALGO_DBL fC5;
MOT_ALGO_DBL fC2C4;
MOT_ALGO_DBL fC2C2C4;
MOT_ALGO_DBL fC5T1T1;
MOT_ALGO_DBL fDConstAtT1T2;
MOT_ALGO_DBL fAmax_real;
MOT_ALGO_DBL fVmax_real;
MOT_ALGO_DBL fJmax_real;
short sErrorCode;

}CALC_TWINSINE_PRECAL_OUTPUT; 

extern short twinsine_precal(
        CALC_TWINSINE_PRECAL_INPUT  *stpInput,
        CALC_TWINSINE_PRECAL_OUTPUT  *stpOutput);

//Structure Define for function prototype.
//Input
typedef struct
{
MOT_ALGO_DBL fT1;
MOT_ALGO_DBL fT2;
MOT_ALGO_DBL fC1;
MOT_ALGO_DBL fC2;
MOT_ALGO_DBL fC3;
MOT_ALGO_DBL fC4;
MOT_ALGO_DBL fC5;
MOT_ALGO_DBL fC2C4;
MOT_ALGO_DBL fC2C2C4;
MOT_ALGO_DBL fC5T1T1;
MOT_ALGO_DBL fDConstAtT1T2;
MOT_ALGO_DBL fAmax_real;
MOT_ALGO_DBL fVmax_real;
MOT_ALGO_DBL fJmax_real;
short sErrorCode;

}CALC_TWINSINE_PROFGEN_INPUT;

//Structure for output
typedef struct
{
MOT_ALGO_DBL *pfdist_prof;
MOT_ALGO_DBL *pfvel_prof;
MOT_ALGO_DBL *pfacc_prof;
MOT_ALGO_DBL *pfjerk_prof;

}CALC_TWINSINE_PROFGEN_OUTPUT; 

short twinsine_profgen(
        CALC_TWINSINE_PROFGEN_INPUT  *stpInput,
        CALC_TWINSINE_PROFGEN_OUTPUT  *stpOutput);


#endif // __TWIN_SINE_H__