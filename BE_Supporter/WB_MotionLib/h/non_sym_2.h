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
// History
// YYYYMMDD Author      Notes
// 20081205 JohnZHAO    Release micro definition

#pragma once

#define PI   (3.14159265358979)
#define NON_SYM_LOCAL_VAR_LENGTH   1024                // 20081021
#define NON_SYM_FITTING_DEF_CASES  (1024 * 5)          // 20081021

typedef struct
{
	unsigned int uiT1;
	unsigned int uiT2;
	unsigned int uiT3;
	double dActualMaxVel;
	double dActualMaxAccT1;
	double dActualMaxAccT3;
	double dDist;
	double dVend;
	double dVelFactor1_T1;
	double dJerkFactor_T1;
	double dDistFactor_T1;
	double dOmegaFactor_T1;
	double dDistFactor_T3;
	double dVelFactor1_T3;
	double dJerkFactor_T3;
	double dOmegaFactor_T3;
	int	iPlotFlag;
}NON_SYM_GEN_PROF_INPUT;

typedef struct
{
	double *pdDistProf;
	double *pdVelProf;
	double *pdAccProf;
	double *pdJerkProf;
}NON_SYM_GEN_PROF_OUTPUT;
int non_sym_gen_prof_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput);

#define NON_SYM_FIT_RULE_WEIGHTED_RMS_ERR      0
#define NON_SYM_FIT_RULE_RMS_ERR               1
#define NON_SYM_FIT_RULE_WEIGHTED_INV_RMS_ERR  2

#define NON_SYM_FIT_CURVE_LOOPING_ERROR_INVERSE_DIST_VEND  1
#define NON_SYM_FIT_CURVE_LOOPING_ERROR_T3_LOWER_BOUND     2
#define NON_SYM_CYCLOIDAL_FIT_CURVE_DEBUG_FILE  "NonSymCycloidalFitCurveDebug.txt"
typedef struct
{
double dDist        ;
double dMaxVel      ;
double dMaxAcc      ;
double dVend        ;
double *pdTargetCurve ;
unsigned int nTargetLen;
unsigned int nFitLen;
int iFlagObjRule;
int iPlotFlag;   // if >= 3 , generate file NON_SYM_CYCLOIDAL_FIT_CURVE_DEBUG_FILE, for debugging
}NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_INPUT; 

typedef struct
{
double dObjValue;
NON_SYM_GEN_PROF_INPUT stNonSymGenProfIn;
}NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_OUTPUT;

typedef struct
{
	NON_SYM_GEN_PROF_INPUT stNonSymGenProfInputCases;
	double pdObjValueCases;
}NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE;

extern int non_sym_cycloidal_calc_t_fit_curve_2(NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_INPUT * stpInput, 
									   NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_OUTPUT *stpOutput);
