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


//function [sam1_1234_precal_output]= sam1_1234_precal(sam1_1234_precal_input)
//sam1_1234_precal_output]= sam1_1234_precal(sam1_1234_precal_input)
//The profile generation is used to calculate the 3rd order position profile
//
// Other outputs are for software debugging.
// 03June2004  ZhengYi   Convert "!=" to be "~="

#include "stdafx.h"

#include "mtn_cmd_gen_sam.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

short sam1_1234_precal(SAM1_1234_PRECAL_INPUT *stpInput, 
					   SAM1_1234_PRECAL_OUTPUT *stpOutput)
{
double Dist = stpInput->Dist;
double Amax = stpInput->Amax;
double Vmax = stpInput->Vmax;
double Jmax = stpInput->Jmax;

// initialize the output
//T=zeros(3,1);

// check input condition
if(Vmax < 0)   Vmax = fabs(Vmax);		   		

if(Amax < 0)   Amax = fabs(Amax);	

if(Jmax < 0)   Jmax = fabs(Jmax);	

////////////// pre-calculation
double AbsDist = fabs(Dist);
double k_a = 0.5;
double k_dist_over_jmax = 64;
double k_vel_over_jmax = 32;

////// package //// Speed constraint
SAM1_CALCT_BY_VAJ_D stSam1_CalcT_By_vaj_d;
SAM1_CALCT_OUT stSam1_Calc_Output;

stSam1_CalcT_By_vaj_d.AbsDist = AbsDist;
stSam1_CalcT_By_vaj_d.k_a = k_a;
stSam1_CalcT_By_vaj_d.k_dist_over_jmax = k_dist_over_jmax;
stSam1_CalcT_By_vaj_d.k_vel_over_jmax = k_vel_over_jmax;
stSam1_CalcT_By_vaj_d.Jmax = Jmax;

stSam1_CalcT_By_vaj_d.Amax = Amax;
stSam1_CalcT_By_vaj_d.Vmax = Vmax;
stSam1_CalcT_By_vaj_d.Dist = Dist;

sam1_calc_t_acc_cv_dec_by_vaj_d(&stSam1_CalcT_By_vaj_d, &stSam1_Calc_Output);

int T_acc = stSam1_Calc_Output.T_acc; // ceil(/4.0) * 4;
int T_dec = stSam1_Calc_Output.T_dec; // ceil(/4) * 4;
int T_total = stSam1_Calc_Output.T_acc + stSam1_Calc_Output.T_cv + stSam1_Calc_Output.T_dec;
int T_cv = stSam1_Calc_Output.T_cv;

//// Time Move
double V_max_a = Dist/(T_acc + T_cv);
double A_max_a = V_max_a / k_a/T_acc;
double J_max_a = V_max_a * k_vel_over_jmax/ 4 / T_acc/T_acc;

double AbsD_cv = V_max_a * T_cv;
double D_cv = AbsD_cv * f_get_sign(Dist);  // //    D_cv = 0; // if T_cv = 0
double D_vv = (AbsDist - AbsD_cv) * f_get_sign(Dist);  //   D_vv = Dist;  // if T_cv = 0 

//// output real-time parameter
stpOutput->T_total = T_total;
stpOutput->T_acc = T_acc;
stpOutput->T_cnst = T_cv;
stpOutput->ActualJerkMax = J_max_a;
stpOutput->ActualAccMax = A_max_a;
stpOutput->ActualVelMax = V_max_a;
// stpOutput->T = [T_acc, T_cv, T_dec];
stpOutput->ActAcc = A_max_a;
stpOutput->AdjVmax = V_max_a - Vmax;
stpOutput->ActVmax = V_max_a;
stpOutput->Dramp = D_vv;
stpOutput->Dconst = D_cv;
stpOutput->AbsDist = AbsDist;
stpOutput->Dist = Dist;

	return 0;
}

//function [sam1_1234_profgen_output] = sam1_1234_profgen(sam1_1234_profgen_input)

short sam1_1234_profgen(SAM1_1234_PROFGEN_INPUT *stpInput,
						SAM1_1234_PROFGEN_OUTPUT *stpOutput)
{
double ActualAccMax = stpInput->ActualAccMax;
double ActualVelMax = stpInput->ActualVelMax;
double ActualJerkMax = stpInput->ActualJerkMax;
// PlotFlag = 3;

double Dist = stpInput->Dist;
int T_acc = stpInput->T_acc;
int T_cv = stpInput->T_cv;

int T_total = stpInput->T_total;
// T = stpInput->T;

	int TT_JerkPos_1 = (int)(T_acc/4);
	int TT_JerkPos_2 = (int)(T_acc/2);
	int TT_JerkNeg_1 = (int)(T_acc/4 * 3);
	int TT_JerkNeg_2 = T_acc;
	int TT_JerkZero = T_acc + T_cv;
	int TT_JerkNeg_3 = T_acc + T_cv + (int)(T_acc/4);
	int TT_JerkNeg_4 = T_acc + T_cv + (int)(T_acc/2);
	int TT_JerkPos_3 = T_acc + T_cv + (int)(T_acc/4*3);
	int TT_JerkPos_4 = T_acc + T_cv + T_acc;

	// tAxis = 0:1:TT_JerkPos_4;
	// // // //        The later function state machine is calculated by ActualVelMax and ActualAccMax
	////////        03June2004, ActAcc -> ActualAccMax,  ConstantSpeed, ActualVelMax

	double *d = stpOutput->d;
	double *v = stpOutput->v;
	double *a = stpOutput->a;
	double *j = stpOutput->j;

	double tt;
	for(int ii = 0; ii<= T_total; ii++)
	{
		tt = ii;
		if(fabs(tt) < 0.1)
		{
			j[ii] = 0;
			a[ii] = 0;         //// 03June2004
			v[ii] = 0;
			d[ii] = 0;
		}
		else if( tt < T_total)
		{
			if(tt < TT_JerkPos_1)
			{
				j[ii] = ActualJerkMax * tt * 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //ActualJerkMax * tt;               //  
				v[ii] = v[ii-1] + a[ii];  //ActualJerkMax * tt * tt/2;        // 
				d[ii] = d[ii-1] + v[ii];  //ActualJerkMax * tt * tt * tt/6;   //  
			}
			else if( tt == TT_JerkPos_1)
			{
				j[ii] = ActualJerkMax;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}            
			else if( tt < TT_JerkPos_2)
			{
				j[ii] = ActualJerkMax - ActualJerkMax * (tt - TT_JerkPos_1)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}   
			else if( tt == TT_JerkPos_2)
			{
				j[ii] = 0;
				a[ii] = ActualAccMax;  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}            
			else if( tt < TT_JerkNeg_1)
			{
				j[ii] = - ActualJerkMax * (tt - TT_JerkPos_2)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}   
			else if( tt == TT_JerkNeg_1)
			{
				j[ii] = - ActualJerkMax;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}            
			else if( tt < TT_JerkNeg_2)
			{
				j[ii] = - ActualJerkMax + ActualJerkMax * (tt - TT_JerkNeg_1)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}
			else if( tt == TT_JerkNeg_2)
			{
				j[ii] = 0;
				a[ii] = 0;  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = ActualVelMax;  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}
			else if( tt <= T_acc + T_cv) // TT_JerkZero
			{
				j[ii] = 0;
				a[ii] = 0;
				v[ii] = ActualVelMax;
				d[ii] = d[ii-1] + ActualVelMax;
			}            
			else if( tt < TT_JerkNeg_3)
			{
				j[ii] = - ActualJerkMax * (tt - T_acc - T_cv)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}
			else if( tt == TT_JerkNeg_3)
			{
				j[ii] = - ActualJerkMax;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}
			else if( tt < TT_JerkNeg_4)
			{
				j[ii] = - ActualJerkMax + ActualJerkMax * (tt - TT_JerkNeg_3)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}            
			else if( tt == TT_JerkNeg_4)
			{
				j[ii] = 0;
				a[ii] = - ActualAccMax;  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}
			else if( tt < TT_JerkPos_3)
			{
				j[ii] = ActualJerkMax * (tt - TT_JerkNeg_4)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}            
			else if( tt == TT_JerkPos_3)
			{
				j[ii] = ActualJerkMax;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}            
			else if( tt < TT_JerkPos_4)
			{
				j[ii] = ActualJerkMax - ActualJerkMax * (tt - TT_JerkPos_3)* 4 /T_acc;
				a[ii] = a[ii-1] + j[ii];  //a[ii-1] + j[ii];  //  ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax + T_acc * ActualJerkMax *(tt - TT_JerkZero) - ActualJerkMax * (tt - TT_JerkZero) * (tt - TT_JerkZero)/2; // 
				d[ii] = d[ii-1] + v[ii];  
			}
			else
			{
				j[ii] = 0;
				a[ii] = a[ii-1] + j[ii];  // -T_acc * ActualJerkMax + ActualJerkMax * (tt - TT_JerkNeg_2); //a[ii-1] + j[ii];  // ActualJerkMax * tt;
				v[ii] = v[ii-1] + a[ii];  // T_acc * T_acc/2 * ActualJerkMax - T_acc * ActualJerkMax *(tt - TT_JerkNeg_2) + ActualJerkMax * (tt - TT_JerkNeg_2) * (tt - TT_JerkNeg_2)/2; // v[ii-1] + a[ii];  // ActualJerkMax * tt * tt/2;
				d[ii] = d[ii-1] + v[ii];  // ActualJerkMax * tt * tt * tt/6;
			}
			}
		else //// ii == T_total 
		{
		  j[ii] = 0;
		  a[ii] = 0;
		  v[ii] = 0;
		  d[ii] = Dist;	
		}
	}

// figure(1)
// clf
// plot([1:T_total],d);
// title('Cmd Distance vs Time(sample)');
// xlabel('Time(sample)')
// legend('CmdDist');
// grid
// zoom
// 
// figure(2)
// clf
// //plot([1:T_total],v, [1:T_total],a, [2:T_total], diff(d));
// plot([1:T_total],v, [1:T_total],a);
// title('Cmd Vel & Cmd Acc vs Time');
// xlabel('Time(sample)')
// legend('CmdVel', 'CmdAcc');
// grid
// zoom
// 
// total_time = T_total


//01062005
//if(PlotFlag >= 3)
//    total_time = T_total
//    figure_index = 1:10;
//    str_display = 'SAM-1 Order 1-2-3-4: ';
//    verify_motion_consistency(tAxis, d, v, a, j, figure_index, str_display);
//end
	return 0;
}
