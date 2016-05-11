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

#ifndef MTN_CMD_GEN_SAM_H
#define MTN_CMD_GEN_SAM_H

typedef struct 
{
	double AbsDist;
	double Jmax;
	double k_dist_over_jmax;
	double k_vel_over_jmax;
	double Amax;
	double k_a;
	double Vmax;
	double Dist;
}SAM1_CALCT_BY_VAJ_D;

typedef struct
{
	int T_acc;
	int T_cv;
	int T_dec;
	int T_total;
}SAM1_CALCT_OUT;
short sam1_calc_t_acc_cv_dec_by_vaj_d(SAM1_CALCT_BY_VAJ_D *stpSam1_CalcT_By_vaj_d,
									  SAM1_CALCT_OUT *stpOutput);
int f_get_sign(double dIn);
double f_get_max(double dIn1, double dIn2);
double round(double dIn1);

typedef struct
{
	double Dist;
	double Amax;
	double Vmax;
	double Jmax;
}SAM1_1234_PRECAL_INPUT;

typedef struct
{
int T_total;
int T_acc;
int T_cnst;
double ActualJerkMax;
double ActualAccMax;
double ActualVelMax;
// double T = [T_acc, T_cv, T_dec];
double ActAcc;
double AdjVmax;
double ActVmax;
double Dramp;
double Dconst;
double AbsDist;
double Dist;

}SAM1_1234_PRECAL_OUTPUT;


typedef struct
{
double ActualAccMax; // = sam1_1234_profgen_input.ActualAccMax;
double ActualVelMax; // = sam1_1234_profgen_input.ActualVelMax;
double ActualJerkMax; // = sam1_1234_profgen_input.ActualJerkMax;
// PlotFlag = 3;

double Dist;  // = sam1_1234_profgen_input.Dist;
int T_acc; // = sam1_1234_profgen_input.T_acc;
int T_cv;  // = sam1_1234_profgen_input.T_cnst;

int T_total; // = sam1_1234_profgen_input.T_total;
//T = sam1_1234_profgen_input.T;
}SAM1_1234_PROFGEN_INPUT;

typedef struct
{
double *d;
double *v;
double *a;
double *j;
}SAM1_1234_PROFGEN_OUTPUT;

short sam1_1234_precal(SAM1_1234_PRECAL_INPUT *stpInput, 
					   SAM1_1234_PRECAL_OUTPUT *stpOutput);
short sam1_1234_profgen(SAM1_1234_PROFGEN_INPUT *stpInput,
						SAM1_1234_PROFGEN_OUTPUT *stpOutput);

#endif  // MTN_CMD_GEN_SAM_H