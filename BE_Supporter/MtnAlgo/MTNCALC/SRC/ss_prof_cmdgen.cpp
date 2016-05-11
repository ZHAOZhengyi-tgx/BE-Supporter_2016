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



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "ss_prof.h"
#include "mtncalc.h"

short ss_prof_cmdgen(
        CALC_SS_PROF_CMDGEN_INPUT  *stpInput,
        CALC_SS_PROF_CMDGEN_OUTPUT  *stpOutput)
{

	MOT_ALGO_DBL fVelIni = stpInput->fV_ini;
	short ii, sIndexT_cnst, sIndexT_cnst_acc, sIndexT_cnst_acc_cnst;
	MOT_ALGO_DBL fTt;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%% Real time command generation for super-sine profile
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	if(stpInput->fT_cnst < 1.0)
	{
		sIndexT_cnst = 0;
	}
	else
	{
		sIndexT_cnst = (short)stpInput->fT_cnst - 1;
	}
	sIndexT_cnst_acc = (short)(stpInput->fT_cnst + stpInput->fT_acc_v -1);
	sIndexT_cnst_acc_cnst = (short)(stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v -1);
	
	for( ii=0; ii<=stpInput->fT_total; ii++)
	{
		fTt = (MOT_ALGO_DBL)ii;
		if( ii < stpInput->fT_cnst)
		{
			stpOutput->pfdist_prof[ii] = fVelIni * fTt + fVelIni;
			stpOutput->pfvel_prof[ii] = fVelIni;
			stpOutput->pfacc_prof[ii] = 0;
			stpOutput->pfjerk_prof[ii] = 0;
		}
		else if( ii< stpInput->fT_cnst + stpInput->fT_acc_v)
		{
			stpOutput->pfdist_prof[ii] = stpOutput->pfdist_prof[sIndexT_cnst] + stpInput->fDist_factor_acc_1* (fTt-stpInput->fT_cnst + 1) - stpInput->fDist_factor_acc_2 * sin((fTt - stpInput->fT_cnst+1)* stpInput->fOmega_factor_acc_1 );
			stpOutput->pfvel_prof[ii] = fVelIni + stpInput->fVel_factor_acc1 * (1 - cos((fTt-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1 ));
			stpOutput->pfacc_prof[ii] = stpInput->fA_max_acc_actual * sin((fTt-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1);
			stpOutput->pfjerk_prof[ii] = stpInput->fJ_max_acc_actual * cos((fTt-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1);
		}
		else if( ii< stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v)
		{
			stpOutput->pfdist_prof[ii] = stpOutput->pfdist_prof[sIndexT_cnst_acc] + stpInput->fV_max_actual * (fTt - stpInput->fT_cnst - stpInput->fT_acc_v + 1);
			stpOutput->pfvel_prof[ii] = stpInput->fV_max_actual;
			stpOutput->pfacc_prof[ii] = 0;
			stpOutput->pfjerk_prof[ii] = 0;
		}
		else if( ii< stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v + stpInput->fT_dec_v)
		{
			stpOutput->pfdist_prof[ii] = stpOutput->pfdist_prof[sIndexT_cnst_acc_cnst] + stpInput->fDist_factor_dec_1 * (fTt-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v +1) - stpInput->fDist_factor_dec_2 * sin((fTt-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v +1)* stpInput->fOmega_factor_dec_1 );
			stpOutput->pfvel_prof[ii] = stpInput->fV_max_actual + stpInput->fVel_factor_dec1 * (1 - cos((fTt-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1 ));
			stpOutput->pfacc_prof[ii] = stpInput->fA_max_dec_actual * sin((fTt-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1);
			stpOutput->pfjerk_prof[ii] = stpInput->fJ_max_dec_actual * cos((fTt-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1);
		}
		else
		{
			stpOutput->pfdist_prof[ii] = stpInput->fDist;
			stpOutput->pfvel_prof[ii] = stpInput->fV_end;
			stpOutput->pfacc_prof[ii] = 0;
			stpOutput->pfjerk_prof[ii] = 0;
		}
	}
	if( stpInput->fplot_flag >=1)
	{
	}
	return MTN_CALC_SUCCESS;
}


short ss_prof_cmdgen_one_sample(
        CALC_SS_PROF_CMDGEN_INPUT  *stpInput,
        CALC_SS_PROF_CMDGEN_OUTPUT_ONE_SAMPLE  *stpOutput,
        unsigned short usSample )
{
	MOT_ALGO_DBL fVelIni = stpInput->fV_ini;
	short sIndexT_cnst, sIndexT_cnst_acc, sIndexT_cnst_acc_cnst;
	MOT_ALGO_DBL fOutSampleT; //fTt,
	MOT_ALGO_DBL fDistIndexT_cnst, fDistIndexT_cnst_acc, fDistIndexT_cnst_acc_cnst;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%% Real time command generation for super-sine profile
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //// Calculation Sample Distance Command
    /// 1. Three special time points
	if(stpInput->fT_cnst < 1.0)
	{
		sIndexT_cnst = 0;
	}
	else
	{
		sIndexT_cnst = (short)stpInput->fT_cnst - 1;
	}
	sIndexT_cnst_acc = (short)(stpInput->fT_cnst + stpInput->fT_acc_v -1);
	sIndexT_cnst_acc_cnst = (short)(stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v -1);

    /// 2. Three special distance at above points
	fDistIndexT_cnst = fVelIni * (MOT_ALGO_DBL)sIndexT_cnst + fVelIni;
	fDistIndexT_cnst_acc = fDistIndexT_cnst 
	       + stpInput->fDist_factor_acc_1 * ((MOT_ALGO_DBL)sIndexT_cnst_acc -stpInput->fT_cnst + 1) 
	       - stpInput->fDist_factor_acc_2 * sin(((MOT_ALGO_DBL)sIndexT_cnst_acc - stpInput->fT_cnst+1)* stpInput->fOmega_factor_acc_1 );
	       
    fDistIndexT_cnst_acc_cnst = fDistIndexT_cnst_acc
           + stpInput->fV_max_actual * ((MOT_ALGO_DBL)sIndexT_cnst_acc_cnst - stpInput->fT_cnst - stpInput->fT_acc_v + 1);

    /// 3. Output distance command
//	fTt = (MOT_ALGO_DBL)usSample;

	fOutSampleT = (MOT_ALGO_DBL)usSample;
	if( usSample < stpInput->fT_cnst)
	{
		stpOutput->fDist_ii = fVelIni * fOutSampleT + fVelIni;
		stpOutput->fVel_ii = fVelIni;
		stpOutput->fAcc_ii = 0;
		stpOutput->fJerk_ii = 0;
	}
	else if( usSample< stpInput->fT_cnst + stpInput->fT_acc_v)
	{
		stpOutput->fDist_ii = fDistIndexT_cnst + stpInput->fDist_factor_acc_1* (fOutSampleT-stpInput->fT_cnst + 1) - stpInput->fDist_factor_acc_2 * sin((fOutSampleT - stpInput->fT_cnst+1)* stpInput->fOmega_factor_acc_1 );
		stpOutput->fVel_ii = fVelIni + stpInput->fVel_factor_acc1 * (1 - cos((fOutSampleT-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1 ));
		stpOutput->fAcc_ii = stpInput->fA_max_acc_actual * sin((fOutSampleT-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1);
		stpOutput->fJerk_ii = stpInput->fJ_max_acc_actual * cos((fOutSampleT-stpInput->fT_cnst)*stpInput->fOmega_factor_acc_1);
	}
	else if( usSample< stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v)
	{
		stpOutput->fDist_ii = fDistIndexT_cnst_acc + stpInput->fV_max_actual * (fOutSampleT - stpInput->fT_cnst - stpInput->fT_acc_v + 1);
		stpOutput->fVel_ii = stpInput->fV_max_actual;
		stpOutput->fAcc_ii = 0;
		stpOutput->fJerk_ii = 0;
	}
	else if( usSample< stpInput->fT_cnst + stpInput->fT_acc_v + stpInput->fT_cnst_v + stpInput->fT_dec_v)
	{
		stpOutput->fDist_ii = fDistIndexT_cnst_acc_cnst + stpInput->fDist_factor_dec_1 * (fOutSampleT-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v +1) - stpInput->fDist_factor_dec_2 * sin((fOutSampleT-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v +1)* stpInput->fOmega_factor_dec_1 );
		stpOutput->fVel_ii = stpInput->fV_max_actual + stpInput->fVel_factor_dec1 * (1 - cos((fOutSampleT-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1 ));
		stpOutput->fAcc_ii = stpInput->fA_max_dec_actual * sin((fOutSampleT-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1);
		stpOutput->fJerk_ii = stpInput->fJ_max_dec_actual * cos((fOutSampleT-stpInput->fT_cnst-stpInput->fT_acc_v - stpInput->fT_cnst_v)*stpInput->fOmega_factor_dec_1);
	}
	else
	{
		stpOutput->fDist_ii = stpInput->fDist;
		stpOutput->fVel_ii = stpInput->fV_end;
		stpOutput->fAcc_ii = 0;
		stpOutput->fJerk_ii = 0;
	}

	
	return MTN_CALC_SUCCESS;
}
