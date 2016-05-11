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


#ifndef __SS_PROF_H__
#define __SS_PROF_H__

#include "mtncalc.h"

#define MTN_CALC_SUCCESS        0
#define MTN_CALC_ERROR          1

#define pi 3.1415926536
#define __MIN_DEC_ACC_TIME__             2

#define __SS_PROF_NO_ERROR__             0
#define __SS_PROF_NEGATIVE_TIME__        1
#define __SS_PROF_V_INI_LARGER_MAX_V__   2
#define __SS_PROF_V_END_LARGER_MAX_V__   4
#define __SS_PROF_DIST_SMALLER_MTN_INI__ 8
#define __SS_PROF_OPPOSITE_DIR_INI__     16
#define __SS_PROF_OPPOSITE_DIR_END__     32
#define __SS_PROF_TOO_SHORT_DIST__       64
#define __SS_PROF_TOO_SHORT_TIME__       128

extern MOT_ALGO_DBL fsign(MOT_ALGO_DBL fInput);

//Structure Define for function prototype.
//Input
typedef struct
{
MOT_ALGO_DBL fDist;
MOT_ALGO_DBL fT_cnst;
MOT_ALGO_DBL fV_ini;
MOT_ALGO_DBL fMaxV;
MOT_ALGO_DBL fMaxA;
MOT_ALGO_DBL fV_end;
MOT_ALGO_DBL fplot_flag;

}CALC_SS_PROF_PRECALC_INPUT; 
//Structure for output
typedef struct
{
MOT_ALGO_DBL fDist_factor_acc_1;
MOT_ALGO_DBL fDist_factor_acc_2;
MOT_ALGO_DBL fDist_factor_dec_1;
MOT_ALGO_DBL fDist_factor_dec_2;
MOT_ALGO_DBL fOmega_factor_acc_1;
MOT_ALGO_DBL fOmega_factor_dec_1;
MOT_ALGO_DBL fVel_factor_acc1;
MOT_ALGO_DBL fVel_factor_dec1;
MOT_ALGO_DBL fT_cnst;
MOT_ALGO_DBL fT_acc_v;
MOT_ALGO_DBL fT_cnst_v;
MOT_ALGO_DBL fT_dec_v;
MOT_ALGO_DBL fV_max_actual;
MOT_ALGO_DBL fA_max_acc_actual;
MOT_ALGO_DBL fA_max_dec_actual;
MOT_ALGO_DBL fJ_max_acc_actual;
MOT_ALGO_DBL fJ_max_dec_actual;
MOT_ALGO_DBL fT_total;
MOT_ALGO_DBL fDist;
MOT_ALGO_DBL fV_ini;
MOT_ALGO_DBL fV_end;
MOT_ALGO_DBL fplot_flag;

short sErrorCode;
}CALC_SS_PROF_PRECALC_OUTPUT; 

typedef CALC_SS_PROF_PRECALC_OUTPUT CALC_SS_PROF_CMDGEN_INPUT ;

#ifdef __DUP_DEFINE__
//Structure Define for function prototype.
//Input
typedef struct
{
MOT_ALGO_DBL fDist_factor_acc_1;
MOT_ALGO_DBL fDist_factor_acc_2;
MOT_ALGO_DBL fDist_factor_dec_1;
MOT_ALGO_DBL fDist_factor_dec_2;
MOT_ALGO_DBL fOmega_factor_acc_1;
MOT_ALGO_DBL fOmega_factor_dec_1;
MOT_ALGO_DBL fVel_factor_acc1;
MOT_ALGO_DBL fVel_factor_dec1;
MOT_ALGO_DBL fT_cnst;
MOT_ALGO_DBL fT_acc_v;
MOT_ALGO_DBL fT_cnst_v;
MOT_ALGO_DBL fT_dec_v;
MOT_ALGO_DBL fV_max_actual;
MOT_ALGO_DBL fA_max_acc_actual;
MOT_ALGO_DBL fA_max_dec_actual;
MOT_ALGO_DBL fJ_max_acc_actual;
MOT_ALGO_DBL fJ_max_dec_actual;
MOT_ALGO_DBL fT_total;
MOT_ALGO_DBL fDist;
MOT_ALGO_DBL fV_ini;
MOT_ALGO_DBL fV_end;
MOT_ALGO_DBL fplot_flag;
}CALC_SS_PROF_CMDGEN_INPUT; 
#endif

//Structure for output
typedef struct
{
MOT_ALGO_DBL *pfdist_prof;
MOT_ALGO_DBL *pfvel_prof;
MOT_ALGO_DBL *pfacc_prof;
MOT_ALGO_DBL *pfjerk_prof;

}CALC_SS_PROF_CMDGEN_OUTPUT; 

typedef struct
{
MOT_ALGO_DBL fDist_ii;
MOT_ALGO_DBL fVel_ii;
MOT_ALGO_DBL fAcc_ii;
MOT_ALGO_DBL fJerk_ii;

}CALC_SS_PROF_CMDGEN_OUTPUT_ONE_SAMPLE;

short ss_prof_cmdgen(
        CALC_SS_PROF_CMDGEN_INPUT  *stpInput,
        CALC_SS_PROF_CMDGEN_OUTPUT  *stpOutput);

short ss_prof_precalc(
        CALC_SS_PROF_PRECALC_INPUT  *stpInput,
        CALC_SS_PROF_PRECALC_OUTPUT  *stpOutput);

short ss_prof_cmdgen_one_sample(
        CALC_SS_PROF_CMDGEN_INPUT  *stpInput,
        CALC_SS_PROF_CMDGEN_OUTPUT_ONE_SAMPLE  *stpOutput,
        unsigned short usSample );

#endif   //__SS_PROF_H__