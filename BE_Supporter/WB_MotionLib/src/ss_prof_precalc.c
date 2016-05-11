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

//
// precalculation for super-sine profile
//Dist:   Travelling Distance, unit as feedback_count
//fT_cnst: Time for initial constant speed move, unit as sample count
//V_ini:  Velocity at initially constant speed move, unit as feedback_count per sample_count
//MaxV:   Maximum Velocity, unit as feedback_count per sample_count
//MaxA:   Maximum Acceleration, unit as feedback_count per sample_count square
//V_end:  Velocity when motion end, unit as feedback_count per sample_count

short ss_prof_precalc(
        CALC_SS_PROF_PRECALC_INPUT  *stpInput,
        CALC_SS_PROF_PRECALC_OUTPUT  *stpOutput)
{
double fDist_long_1, fDist_long_2;
double fV_max_adj;
	stpOutput->sErrorCode = __SS_PROF_NO_ERROR__;

	if( stpInput->fT_cnst < 0)
	{
		fprintf(stderr, "negative time parameter for constant speed motion");
		stpOutput->sErrorCode |= __SS_PROF_NEGATIVE_TIME__;
	}
	if( fabs(stpInput->fV_ini) > stpInput->fMaxV )
	{
		fprintf(stderr, "stpInput->fV_ini (%f) violates the maximum velocity constraint (%f)", stpInput->fV_ini, stpInput->fMaxV);
		stpOutput->sErrorCode |= __SS_PROF_V_INI_LARGER_MAX_V__;
	}
	if( fabs((double)stpInput->fV_end) > (double)stpInput->fMaxV )
	{
		fprintf(stderr, "stpInput->fV_end (%f) violates the maximum velocity constraint (%f)", stpInput->fV_end, stpInput->fMaxV);
		stpOutput->sErrorCode |= __SS_PROF_V_END_LARGER_MAX_V__;
	}
	if( fabs(stpInput->fDist) < fabs( stpInput->fT_cnst * stpInput->fV_ini))
	{
		fprintf(stderr, "Distance (%f) violates the initial velocity (%f) and const speed move condition (%f)", stpInput->fDist, stpInput->fT_cnst, stpInput->fV_ini);
		stpOutput->sErrorCode |= __SS_PROF_DIST_SMALLER_MTN_INI__;
	}

	if( stpInput->fDist * stpInput->fV_ini < 0)
	{
		fprintf(stderr, "Reverse travelling distance, oppositie direction stpInput->fDist (%f) and stpOutput->fV_ini (%f)", stpInput->fDist, stpInput->fV_ini);
		stpOutput->sErrorCode |= __SS_PROF_OPPOSITE_DIR_INI__;
	}
	if( stpInput->fDist * stpInput->fV_end < 0)
	{
		fprintf(stderr, "Reverse travelling distance, oppositie direction stpInput->fDist (%f) and stpOutput->fV_end (%f)", stpInput->fDist, stpInput->fV_end);
		stpOutput->sErrorCode |= __SS_PROF_OPPOSITE_DIR_END__;
	}
	if(stpOutput->sErrorCode != __SS_PROF_NO_ERROR__)
	{
		return MTN_CALC_ERROR;
	}

	/*
	Dist_long_1 = sign(Dist) * (sign(Dist) * MaxV - V_ini)/MaxA * pi/2 * (sign(Dist) * MaxV + V_ini)/2 + ...
    sign(Dist) * (sign(Dist)* MaxV - V_end)/MaxA * pi/2 * (sign(Dist)*MaxV + V_end)/2 + ...
    V_ini * T_cnst;
	*/


	fDist_long_1 = fsign(stpInput->fDist) *(fsign(stpInput->fDist) * stpInput->fMaxV - stpInput->fV_ini)/stpInput->fMaxA * pi/2 * (fsign(stpInput->fDist) * stpInput->fMaxV + stpInput->fV_ini)/2 + 
		 fsign(stpInput->fDist) * (fsign(stpInput->fDist)* stpInput->fMaxV - stpInput->fV_end)/stpInput->fMaxA * pi/2 * (fsign(stpInput->fDist)*stpInput->fMaxV + stpInput->fV_end)/2 + 
		 stpInput->fV_ini * stpInput->fT_cnst;
	//
	//Dist_long_2 = V_ini * stpInput->fT_cnst + fsign(V_ini) * abs(V_ini - V_end)/MaxA * pi/2 * (abs(V_ini + V_end))/2;
	//
	//Dist_long_2 = V_ini * T_cnst + sign(Dist) * abs(V_ini)/MaxA * pi/2 * V_ini/2 + sign(Dist) * abs(V_end)/MaxA * pi/2 * V_end/2;

	fDist_long_2 = stpInput->fV_ini * stpInput->fT_cnst + 
		fsign(stpInput->fDist) * fabs(stpInput->fV_ini)/stpInput->fMaxA * pi/2 * stpInput->fV_ini/2 + 
		fsign(stpInput->fDist) * fabs(stpInput->fV_end)/stpInput->fMaxA * pi/2 * stpInput->fV_end/2;

	if( stpInput->fplot_flag >= 3)
	{
		fprintf(stderr, "fDist_long_1: %f, fDist_long_2: %f\n", fDist_long_1, fDist_long_2);
	}

	if((fabs(stpInput->fDist) <= fabs(fDist_long_2)))
	{
		fprintf(stderr, "Too short stpInput->fDist (%f), has to reverse velocity direction ");
		stpOutput->sErrorCode |= __SS_PROF_TOO_SHORT_DIST__;
	}
	
	if( fabs(stpInput->fDist) > fabs(fDist_long_1))
	{
//    T_acc_v = abs(fix((sign(Dist) * MaxV - V_ini)/MaxA * pi /2)) + 1;

		stpOutput->fT_acc_v = floor(fabs((fsign(stpInput->fDist) * stpInput->fMaxV - stpInput->fV_ini)/stpInput->fMaxA * pi /2)) 
								+ 1;
//    T_dec_v = abs(fix((sign(Dist) * MaxV - V_end)/MaxA * pi /2)) + 1;
//    T_cnst_v = abs(fix((Dist - T_acc_v * (sign(Dist) * MaxV + V_ini)/2 - T_dec_v * (sign(Dist)*MaxV + V_end)/2)/MaxV)) + 1;
		stpOutput->fT_dec_v = floor(fabs((fsign(stpInput->fDist) * stpInput->fMaxV - stpInput->fV_end)/stpInput->fMaxA * pi /2))
								+ 1;
        //T_cnst_v = floor((Dist - stpOutput->fT_acc_v * (sign(V_ini) * MaxV + V_ini)/2 - stpOutput->fT_dec_v * (sign(V_ini)*MaxV + V_end)/2)/MaxV) + 1;

		stpOutput->fT_cnst_v = 
					floor(fabs((stpInput->fDist 
					     - stpOutput->fT_acc_v * (fsign(stpInput->fDist) * stpInput->fMaxV + stpInput->fV_ini)/2 
						 - stpOutput->fT_dec_v * (fsign(stpInput->fDist) * stpInput->fMaxV + stpInput->fV_end)/2)
						 /stpInput->fMaxV
						      )
						  ) + 1;
	}
	else if( fabs(stpInput->fDist) > fabs(fDist_long_2))
	{
//  V_max_adj = sign(Dist)*sqrt((abs(Dist - V_ini*T_cnst) * 4/pi*MaxA + V_ini*V_ini + V_end*V_end)/2);
		fV_max_adj = fsign(stpInput->fDist)*
			                 sqrt( (fabs(stpInput->fDist - stpInput->fV_ini*stpInput->fT_cnst) * 4/pi*stpInput->fMaxA 
							         + stpInput->fV_ini * stpInput->fV_ini 
									 + stpInput->fV_end * stpInput->fV_end
								   )/2.0
						         );
//    T_cnst_v = 0;
//    T_acc_v = abs(fix((V_max_adj - V_ini)/MaxA * pi /2)) + 1;
//    T_dec_v = abs(fix((V_max_adj - V_end)/MaxA * pi /2)) + 1;
		stpOutput->fT_cnst_v = 0;
		stpOutput->fT_acc_v = floor(fabs((fV_max_adj - stpInput->fV_ini)/stpInput->fMaxA * pi /2))
			                  + 1;
		stpOutput->fT_dec_v = floor(fabs((fV_max_adj - stpInput->fV_end)/stpInput->fMaxA * pi /2))
			                  + 1;
	}
	
	if( stpOutput->fT_acc_v <= __MIN_DEC_ACC_TIME__ || stpOutput->fT_dec_v <= __MIN_DEC_ACC_TIME__)
	{
		fprintf(stderr, "Too short distance or too large vel and/or acc causing too short time, incomplete profile, T_acc: %f, T_dec: %f", stpOutput->fT_acc_v, stpOutput->fT_dec_v);
		stpOutput->sErrorCode |= __SS_PROF_TOO_SHORT_TIME__;
	}
	
	if(stpOutput->sErrorCode != __SS_PROF_NO_ERROR__)
	{
		return MTN_CALC_ERROR;
	}

	// stpOutput->fV_max_actual = (Dist - V_ini * T_cnst - V_ini/2*stpOutput->fT_acc_v - V_end/2*stpOutput->fT_dec_v)/(stpOutput->fT_acc_v/2 + T_cnst_v + stpOutput->fT_dec_v/2);
	stpOutput->fV_max_actual = (stpInput->fDist - stpInput->fV_ini * stpInput->fT_cnst - stpInput->fV_ini/2*stpOutput->fT_acc_v - stpInput->fV_end/2*stpOutput->fT_dec_v)
		                          /(stpOutput->fT_acc_v/2 + stpOutput->fT_cnst_v + stpOutput->fT_dec_v/2);
	stpOutput->fA_max_acc_actual = (stpOutput->fV_max_actual - stpInput->fV_ini) *pi / 2/stpOutput->fT_acc_v;
	stpOutput->fA_max_dec_actual = (stpInput->fV_end- stpOutput->fV_max_actual) *pi / 2/stpOutput->fT_dec_v;
	stpOutput->fJ_max_acc_actual = stpOutput->fA_max_acc_actual * pi/stpOutput->fT_acc_v;
	stpOutput->fJ_max_dec_actual = stpOutput->fA_max_dec_actual * pi/stpOutput->fT_dec_v;
	stpOutput->fT_total = stpInput->fT_cnst + stpOutput->fT_acc_v + stpOutput->fT_cnst_v + stpOutput->fT_dec_v;

	if( stpInput->fplot_flag >= 3)
	{
//		stpOutput->fT_cnst, stpOutput->fT_acc_v, stpOutput->fT_cnst_v, stpOutput->fT_dec_v
//			stpOutput->fV_max_actual, stpOutput->fA_max_acc_actual, stpOutput->fA_max_dec_actual, stpOutput->fJ_max_acc_actual, stpOutput->fJ_max_dec_actual, stpOutput->fT_total,    
	}
	
	stpOutput->fDist_factor_acc_1 = (stpOutput->fV_max_actual + stpInput->fV_ini )/2;
	stpOutput->fDist_factor_acc_2 = (stpOutput->fV_max_actual - stpInput->fV_ini)/2* stpOutput->fT_acc_v / pi;
	stpOutput->fDist_factor_dec_1 = (stpInput->fV_end+ stpOutput->fV_max_actual )/2;
	stpOutput->fDist_factor_dec_2 = (stpInput->fV_end- stpOutput->fV_max_actual )/2* stpOutput->fT_dec_v / pi;
	stpOutput->fOmega_factor_acc_1 = pi/stpOutput->fT_acc_v;
	stpOutput->fOmega_factor_dec_1 = pi/stpOutput->fT_dec_v;
	stpOutput->fVel_factor_acc1 = (stpOutput->fV_max_actual - stpInput->fV_ini)/2;
	stpOutput->fVel_factor_dec1 = (stpInput->fV_end- stpOutput->fV_max_actual)/2;

	stpOutput->fDist = stpInput->fDist;
	stpOutput->fV_ini= stpInput->fV_ini;
	stpOutput->fV_end= stpInput->fV_end;
	stpOutput->fT_cnst = stpInput->fT_cnst;


	return MTN_CALC_SUCCESS;

}
