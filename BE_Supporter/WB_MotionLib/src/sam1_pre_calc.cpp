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

//function [T_acc, T_cv, T_dec, T_total] = sam1_calc_t_acc_cv_dec_by_vaj_d(stSam1_CalcT_By_vaj_d)
// SAM1: symmetry and mean, 1st condition
//       a(t) = a(T_acc - t), t \in [0, T_acc], 
//            or a(t) is even-symmetry in [0, T_acc]
//       T_total = T_acc + T_cv + T_dec
//       a(t) = - a(T_total - t), t \in [0, T_total]
//            or a(t) is odd-symmetry in [0, T_total]

#include "stdafx.h"

#include "mtn_cmd_gen_sam.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int f_get_sign(double dIn)
{
	if(dIn >= 0)
		return 1;
	else
		return -1;
}

double f_get_max(double dIn1, double dIn2)
{
	if(dIn1 >= dIn2)
		return dIn1;
	else
		return dIn2;
}

double round(double dIn1)
{
	if(dIn1 >= 0)
		return floor(dIn1 + 0.5);
	else
		return floor(dIn1 - 0.5);
}

short sam1_calc_t_acc_cv_dec_by_vaj_d(SAM1_CALCT_BY_VAJ_D *stpSam1_CalcT_By_vaj_d,
									  SAM1_CALCT_OUT *stpOutput)
{
double AbsDist = stpSam1_CalcT_By_vaj_d->AbsDist;
double Jmax = stpSam1_CalcT_By_vaj_d->Jmax;
double k_dist_over_jmax = stpSam1_CalcT_By_vaj_d->k_dist_over_jmax;
double k_vel_over_jmax = stpSam1_CalcT_By_vaj_d->k_vel_over_jmax;
double Amax = stpSam1_CalcT_By_vaj_d->Amax;
double k_a = stpSam1_CalcT_By_vaj_d->k_a;
double Vmax = stpSam1_CalcT_By_vaj_d->Vmax;
double Dist = stpSam1_CalcT_By_vaj_d->Dist;

double AbsDist_VelAcc = Vmax * Vmax/k_a/Amax;

double T_maxj = pow(k_dist_over_jmax * AbsDist/Jmax, 1.0/3);
double T_maxa = 2.0 * sqrt( AbsDist/(k_a * Amax));
double T_maxv = 2.0 * AbsDist/Vmax;
double T_maxvaj = f_get_max(f_get_max(T_maxj, T_maxa), T_maxv);
// double T_max_j_a_v = [T_maxj, T_maxa, T_maxv]
// double 
T_maxvaj = ceil(T_maxvaj);

int T_cv, T_acc, T_dec, T_total;

	if( T_maxvaj <= 8)
	{
		T_maxvaj = 8;
	}
	if( AbsDist_VelAcc >= AbsDist) // Small Distance
	{
		T_maxvaj = ceil(f_get_max(f_get_max(T_maxj, T_maxa), 8));
		T_cv = 0;
		T_acc = (int)T_maxvaj/2;
		T_acc = (int)round(T_acc/4.0) * 4; 
		T_dec = T_acc;
		T_total = T_acc * 2;
	}
	else
	{
		T_cv = (int)ceil((AbsDist - AbsDist_VelAcc)/Vmax);

double		AbsD_cv = Vmax * T_cv;
double		D_cv = AbsD_cv * f_get_sign(Dist);  // //    D_cv = 0; // if T_cv = 0
double		D_vv = (AbsDist - AbsD_cv) * f_get_sign(Dist);  //   D_vv = Dist;  // if T_cv = 0 
	    
double		T_acc_vj = sqrt(Vmax * k_vel_over_jmax / Jmax)/2;
double		T_acc_dj = pow(k_dist_over_jmax * abs(D_vv)/Jmax, 1.0/3);
double		T_acc_va = Vmax/k_a /Amax;
double		T_acc_da = 2 * sqrt( fabs(D_vv)/(k_a * Amax));
double		T_vv = f_get_max(f_get_max(D_vv/Vmax/2, T_acc_vj), f_get_max(f_get_max(T_acc_va, T_acc_dj), T_acc_da));
		T_vv = ceil(T_vv/4.0) * 4;
		T_acc = (int)T_vv/2;
		T_dec = T_acc;
		T_total = T_acc * 2 + T_cv;
	    
	}

	stpOutput->T_acc = T_acc;
	stpOutput->T_cv = T_cv;
	stpOutput->T_dec = T_dec;
	stpOutput->T_total = T_total;

	return 0;
}


