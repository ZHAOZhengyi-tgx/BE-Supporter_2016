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
// YYYYDDMM  Author		Note
// 20081021  Zhengyi    debug release memory
// 20100320				Add protection not to count initial zero-velocities
#include "stdafx.h"
#include "math.h"
#include "malloc.h"
#include "stdio.h"
#include "non_sym_2.h"

// Function non_sym_gen_prof_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput)
// Utility:
//      to generate non-symmetric cycloidal profile, 
// Given: 
//      Dist, Vend, T1, T2, T3
// Matlab Prototype
//function [DistProf, VelProf, AccProf, JerkProf] = non_sym_sin_gen_prof(stInput)
int non_sym_gen_prof_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput)
{
	unsigned int uiT1      = stpInput->uiT1;
	unsigned int uiT2      = stpInput->uiT2;
	unsigned int uiT3      = stpInput->uiT3;
	double dActualMaxVel   = stpInput->dActualMaxVel;
	double dActualMaxAccT1 = stpInput->dActualMaxAccT1;
	double dActualMaxAccT3 = stpInput->dActualMaxAccT3;
	double dDist           = stpInput->dDist;
	double dVend           = stpInput->dVend;
	double dVelFactor1_T1  = stpInput->dVelFactor1_T1;
	double dJerkFactor_T1  = stpInput->dJerkFactor_T1;
	double dDistFactor_T1  = stpInput->dDistFactor_T1;
	double dOmegaFactor_T1 = stpInput->dOmegaFactor_T1;
	double dDistFactor_T3  = stpInput->dDistFactor_T3;
	double dVelFactor1_T3  = stpInput->dVelFactor1_T3;
	double dJerkFactor_T3  = stpInput->dJerkFactor_T3;
	double dOmegaFactor_T3 = stpInput->dOmegaFactor_T3;
	int	PlotFlag       = stpInput->iPlotFlag;

	double *DistProf = stpOutput->pdDistProf;
	double *VelProf = stpOutput->pdVelProf;
	double *AccProf = stpOutput->pdAccProf;
	double *JerkProf = stpOutput->pdJerkProf;

// aTimeList = 0:1:(uiT1 + uiT2 + uiT3);
	unsigned int uiTotalTime = uiT1 + uiT2 + uiT3 + 1, tDec;
	double dTempSin, dTempCos;


	for(unsigned int tt = 0; tt<=uiTotalTime; tt++) // :1:uiTotalTime
	{
//		tt = aTimeList(tt);
	    
		if( tt <= uiT1)
		{
			dTempSin = sin(tt * dOmegaFactor_T1);
			dTempCos = cos(tt * dOmegaFactor_T1);
			DistProf[tt]= dVelFactor1_T1 * tt - dDistFactor_T1 * dTempSin;
			VelProf[tt] = dVelFactor1_T1 * (1 - dTempCos);
			AccProf[tt] = dActualMaxAccT1 * dTempSin;
			JerkProf[tt] = dJerkFactor_T1 * dTempCos;
		}
		else if (tt <= uiT1 + uiT2)
		{
			DistProf[tt]= dVelFactor1_T1 * uiT1 + dActualMaxVel * (tt - uiT1);
			VelProf[tt] = dActualMaxVel;
			AccProf[tt] = 0;
			JerkProf[tt] = 0;
		}
		else if( tt < uiT1 + uiT2 + uiT3)
		{
			tDec = (tt - uiT1 - uiT2);
			dTempSin = sin(tDec * dOmegaFactor_T3);
			dTempCos = cos(tDec * dOmegaFactor_T3);
			DistProf[tt] = dVelFactor1_T1 * uiT1 + dActualMaxVel * uiT2 + dActualMaxVel * tDec + dVelFactor1_T3 * tDec
				- dDistFactor_T3 * dTempSin ;
			VelProf[tt] = dActualMaxVel + dVelFactor1_T3 * (1 - dTempCos);
			AccProf[tt] = dActualMaxAccT3 * dTempSin;
			JerkProf[tt] = dJerkFactor_T3 * dTempCos;
		}
		else
		{
			DistProf[tt] = dDist;
			VelProf[tt] = dVend; 
			AccProf[tt] = 0;
			JerkProf[tt] = 0;
		}
	}

	return 0;

//figure_index = 1:10;
//str_display = 'Non-Sym-Sin, w V_e--  ';
//if PlotFlag >= 1
//    verify_motion_consistency(aTimeList, DistProf, VelProf, AccProf, JerkProf, figure_index, str_display);
//end
}

// Function non_sym_gen_prof_dist_vel_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput)
// only generate distance and velocity profile
int non_sym_gen_prof_dist_vel_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput)
{
	unsigned int uiT1      = stpInput->uiT1;
	unsigned int uiT2      = stpInput->uiT2;
	unsigned int uiT3      = stpInput->uiT3;
	double dActualMaxVel   = stpInput->dActualMaxVel;
	double dActualMaxAccT1 = stpInput->dActualMaxAccT1;
	double dActualMaxAccT3 = stpInput->dActualMaxAccT3;
	double dDist           = stpInput->dDist;
	double dVend           = stpInput->dVend;
	double dVelFactor1_T1  = stpInput->dVelFactor1_T1;
	double dJerkFactor_T1  = stpInput->dJerkFactor_T1;
	double dDistFactor_T1  = stpInput->dDistFactor_T1;
	double dOmegaFactor_T1 = stpInput->dOmegaFactor_T1;
	double dDistFactor_T3  = stpInput->dDistFactor_T3;
	double dVelFactor1_T3  = stpInput->dVelFactor1_T3;
	double dJerkFactor_T3  = stpInput->dJerkFactor_T3;
	double dOmegaFactor_T3 = stpInput->dOmegaFactor_T3;
	int	PlotFlag       = stpInput->iPlotFlag;

	double *DistProf = stpOutput->pdDistProf;
	double *VelProf = stpOutput->pdVelProf;

// aTimeList = 0:1:(uiT1 + uiT2 + uiT3);
	unsigned int uiTotalTime = uiT1 + uiT2 + uiT3 + 1, tDec;


	for(unsigned int tt = 0; tt<=uiTotalTime; tt++) // :1:uiTotalTime
	{
//		tt = aTimeList(tt);
	    
		if( tt <= uiT1)
		{
			DistProf[tt]= dVelFactor1_T1 * tt - dDistFactor_T1 * sin(tt * dOmegaFactor_T1);
			VelProf[tt] = dVelFactor1_T1 * (1 - cos(tt * dOmegaFactor_T1));
		}
		else if (tt <= uiT1 + uiT2)
		{
			DistProf[tt]= dVelFactor1_T1 * uiT1 + dActualMaxVel * (tt - uiT1);
			VelProf[tt] = dActualMaxVel;
		}
		else if( tt < uiT1 + uiT2 + uiT3)
		{
			tDec = (tt - uiT1 - uiT2);
			DistProf[tt] = dVelFactor1_T1 * uiT1 + dActualMaxVel * uiT2 + dActualMaxVel * tDec + dVelFactor1_T3 * tDec
				- dDistFactor_T3 * sin(tDec * dOmegaFactor_T3) ;
			VelProf[tt] = dActualMaxVel + dVelFactor1_T3 * (1 - cos(tDec * dOmegaFactor_T3));
		}
		else
		{
			DistProf[tt] = dDist;
			VelProf[tt] = dVend; 
		}
	}

	return 0;

//figure_index = 1:10;
//str_display = 'Non-Sym-Sin, w V_e--  ';
//if PlotFlag >= 1
//    verify_motion_consistency(aTimeList, DistProf, VelProf, AccProf, JerkProf, figure_index, str_display);
//end
}


double adTargetAccLocalNonSymFitting[NON_SYM_LOCAL_VAR_LENGTH];  // array of double, 20100727
double adDistProfLocal[NON_SYM_LOCAL_VAR_LENGTH];
double adVelProfLocal[NON_SYM_LOCAL_VAR_LENGTH];
double adVelWeight[NON_SYM_LOCAL_VAR_LENGTH];   // 20081021, 20100727
//function 
//
//Matlab Prototype
// fSosErr = calc_weighted_rms_err(DistProf, VelProf, TargetCurve, nFitLen)
double non_sym_calc_weighted_rms_err_2(double *pdDistProf, double *pdVelProf, double *pdTargetCurve, unsigned int nFitLen)
{
	double *pdVelWeight, dErrRMS, dSumWeight = 0, tTemp;
	unsigned int ii;

	if (nFitLen > NON_SYM_LOCAL_VAR_LENGTH)   // 20081021
	{
		pdVelWeight = (double *) calloc(nFitLen, sizeof(double) );
	}
	else
	{
		pdVelWeight = &adVelWeight[0];
	}
	for(ii = 0; ii< nFitLen; ii++)
	{
		pdVelWeight[ii] = fabs(pdVelProf[ii]);
		dSumWeight += pdVelWeight[ii];
	}

	dErrRMS = 0;
	for(ii = 0; ii< nFitLen; ii++)
	{
		tTemp = pdDistProf[ii] - pdTargetCurve[ii];
		dErrRMS = dErrRMS + tTemp * tTemp * pdVelWeight[ii]/dSumWeight;
	}

	dErrRMS = sqrt(dErrRMS/nFitLen);

	if (nFitLen > NON_SYM_LOCAL_VAR_LENGTH)  // 20081021
	{
		free(pdVelWeight); //  = (double *) calloc(nFitLen, sizeof(double) );
	}
return dErrRMS;
}

//function 
//
//Matlab Prototype
// fSosErr = calc_rms_err(DistProf, VelProf, TargetCurve, nFitLen)
double non_sym_calc_rms_err_2(double *pdDistProf, double *pdVelProf, double *pdTargetCurve, unsigned int nFitLen)
{
	double dErrRMS, tTemp;
	unsigned int ii;

	dErrRMS = 0;
	for(ii = 0; ii< nFitLen; ii++)
	{
		tTemp = pdDistProf[ii] - pdTargetCurve[ii];
		dErrRMS = dErrRMS + tTemp * tTemp;
	}

	dErrRMS = sqrt(dErrRMS/nFitLen);

return dErrRMS;
}

//function 
//
//Matlab Prototype
// fSosErr = calc_weighted_inv_rms_err(DistProf, VelProf, TargetCurve, nFitLen)
double non_sym_calc_weighted_inv_rms_err_2(double *pdDistProf, double *pdVelProf, double *pdTargetCurve, unsigned int nFitLen)
{
	double *pdVelWeight, dErrRMS, dSumWeight = 0, tTemp;
	unsigned int ii;

	if (nFitLen > NON_SYM_LOCAL_VAR_LENGTH)  // 20081021
	{
		pdVelWeight = (double *) calloc(nFitLen, sizeof(double) );
	}
	else
	{
		pdVelWeight = &adVelWeight[0];
	}
	for(ii = 0; ii< nFitLen; ii++)
	{
		pdVelWeight[ii] = 1/(fabs(pdVelProf[ii]) + 1);
		dSumWeight += pdVelWeight[ii];
	}

	dErrRMS = 0;
	for(ii = 0; ii< nFitLen; ii++)
	{
		tTemp = pdDistProf[ii] - pdTargetCurve[ii];
		dErrRMS = dErrRMS + tTemp * tTemp * pdVelWeight[ii]/dSumWeight;
	}

	dErrRMS = sqrt(dErrRMS/nFitLen);
	if (nFitLen > NON_SYM_LOCAL_VAR_LENGTH)   // 20081021
	{
		free(pdVelWeight); //  = (double *) calloc(nFitLen, sizeof(double) );
	}

return dErrRMS;
}



//function [stOutput, stDebug] = non_sym_sin_calc_t_fit_curve(stInput)
//
// INPUT:
// Dist
// MaxVel
// MaxAcc
// Vend
// TargetCurve
// iFlagObjRule
static NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE astNonSymCalcFitCase[NON_SYM_FITTING_DEF_CASES];

int non_sym_cycloidal_calc_t_fit_curve_2(NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_INPUT * stpInput, 
									   NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_OUTPUT *stpOutput)
{
double Dist        = stpInput->dDist        ;
double MaxVel      = stpInput->dMaxVel      ;
double MaxAcc      = stpInput->dMaxAcc      ;
double Vend        = stpInput->dVend        ;
double *TargetCurve = stpInput->pdTargetCurve ;
unsigned int nFitLen     = stpInput->nFitLen;
int iPlotFlag   = stpInput->iPlotFlag;
int iRet = 0;
double *pdTargetAcc;
unsigned int ii;
unsigned int T1_nominal, nT1_Lower, nT1_Upper;
unsigned int uiNumTotalCases;
FILE *fptrDebug;

	if(iPlotFlag >= 3)
	{
		fopen_s(&fptrDebug, NON_SYM_CYCLOIDAL_FIT_CURVE_DEBUG_FILE, "w");
	}
	if(Vend * Dist < 0)
	{
		iRet = NON_SYM_FIT_CURVE_LOOPING_ERROR_INVERSE_DIST_VEND;
		return iRet; // error('Vend * Dist < 0')
	}

	if(nFitLen > NON_SYM_LOCAL_VAR_LENGTH)
	{
		pdTargetAcc = (double*) calloc(nFitLen, sizeof(double));
	}
	else
	{
		pdTargetAcc = adTargetAccLocalNonSymFitting;
	}
int iSkippingZeros = 0; // 20100320
	for(ii = 0; ii < (nFitLen - 1); ii++)
	{
		if(fabs(TargetCurve[ii+1] - TargetCurve[ii]) > 0.00001)
		{
			iSkippingZeros = ii;
			break;
		}
	}

	//TargetVel = [diff(TargetCurve); 0]; //[prediv_get_diff(TargetCurve), 0];
	//TargetAcc = [0; diff(TargetVel)];//[0, prediv_get_diff(TargetVel)];
	for(ii = 0; ii<nFitLen; ii++)
	{
		if(ii == 0)
		{
			pdTargetAcc[ii] = 0;
		}
		else if(ii == nFitLen - 1)
		{
			pdTargetAcc[ii] = 0;
		}
		else
		{
			pdTargetAcc[ii] = TargetCurve[ii+1] + TargetCurve[ii-1] - 2 * TargetCurve[ii];
		}
	}
	
//	double dTempAccChangeSign;
	T1_nominal = nFitLen/2;
	for(ii = 1; ii<(nFitLen-1); ii++)  // ii = 1:1: length(TargetAcc) - 1
	{
//		dTempAccChangeSign = (pdTargetAcc[ii] * pdTargetAcc[ii + 1]); dTempAccChangeSign < 0
		if( (pdTargetAcc[ii] <0 && pdTargetAcc[ii + 1] >=0) ||
			(pdTargetAcc[ii] >0 && pdTargetAcc[ii + 1] <=0)
			)
		{
			T1_nominal = ii+1;
			break;
		}
	}

	double fMaxVel = TargetCurve[T1_nominal] - TargetCurve[T1_nominal-1];
	//// 
	////T3 = length(TargetAcc) - T1 + ceil((Vend - TargetVel(end))/TargetAcc(end)/2);
	//// Tuning Range of T1: {T1_nominal-2, T1_nominal-1, T1_nominal, T1_nominal+1, T1_nominal +2}
#ifdef __PROTECT_BY_T3__
unsigned int T3_min_1, T3_min_2, T3_min;
	T3_min_1 = (unsigned int)(fabs(fMaxVel - Vend) * PI/2 / MaxAcc);   // DONOT use MaxVel
	T3_min_2 = stpInput->nTargetLen - T1_nominal - iSkippingZeros; // 20100320
	if(T3_min_1 >= T3_min_2)   // T3_min = max([T3_min_1, T3_min_2])
	{
		T3_min = T3_min_1;
	}
	else
	{
		T3_min = T3_min_2;
	}
	if(T3_min > T1_nominal)
	{
		iRet = NON_SYM_FIT_CURVE_LOOPING_ERROR_T3_LOWER_BOUND;
		return iRet;
	}
	uiNumTotalCases = (T1_nominal + 2 - T3_min) * 5;
#endif  // __PROTECT_BY_T3__
//// Tuning Range of T3: [T3_min , T1], T1: [T1_nominal-2, T1_nominal+2]
#define __SEARCH_RANGE_HALF__    11
	nT1_Lower = T1_nominal - __SEARCH_RANGE_HALF__;
	nT1_Upper = T1_nominal + __SEARCH_RANGE_HALF__;
static unsigned int uiHalfT1Nominal, uiHalfT1FitLen;

	uiHalfT1Nominal = T1_nominal/2;
	if(nT1_Lower < uiHalfT1Nominal)
	{
		nT1_Lower = uiHalfT1Nominal;  // T1_nominal/2;
	}
	uiHalfT1FitLen = (T1_nominal + nFitLen)/2;
	if(nT1_Upper > uiHalfT1FitLen)
	{
		nT1_Upper = uiHalfT1FitLen;
	}
	uiNumTotalCases = nT1_Upper - nT1_Lower + 1;

	//NON_SYM_GEN_PROF_INPUT **stpNonSymGenProfInputCases;
	//double *pdObjValueCases;
	NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE *pstFitCases;
	if(uiNumTotalCases > NON_SYM_FITTING_DEF_CASES)
	{
		//stpNonSymGenProfInputCases = (NON_SYM_GEN_PROF_INPUT **)calloc(uiNumTotalCases, sizeof(NON_SYM_GEN_PROF_INPUT));
		//pdObjValueCases = (double *)calloc(uiNumTotalCases, sizeof(double));
		pstFitCases = (NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE *)calloc(uiNumTotalCases, sizeof(NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE));
	}
	else
	{
		pstFitCases = (NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE *) &astNonSymCalcFitCase[0];
		//for(ii = 0; ii<uiNumTotalCases; ii++)
		//{
		//	pstFitCases[ii] = &(astNonSymCalcFitCase[ii]);
		//}
		//stpNonSymGenProfInputCases = &astNonSymGenProfCases[0];
		//pdObjValueCases = &adObjValue[0];
	}

	NON_SYM_GEN_PROF_OUTPUT stNonSymProfOut;
	if((T1_nominal+2)*2 + 1 > NON_SYM_LOCAL_VAR_LENGTH)
	{
		stNonSymProfOut.pdDistProf = (double *) calloc((T1_nominal+2)*2 + 1, sizeof(double));
		stNonSymProfOut.pdVelProf  = (double *) calloc((T1_nominal+2)*2 + 1, sizeof(double));
	}
	else
	{
		stNonSymProfOut.pdDistProf = &adDistProfLocal[0];
		stNonSymProfOut.pdVelProf  = &adVelProfLocal[0];
	}
	//time_list = 1:1:nFitLen; for plotting

	unsigned int T2_trial = 0, T1_trial, T3_trial;  //// always no T2
	double dMinObjValue = 0;
	unsigned int uiMinObjIdx = 0;  // 20120517
	//stGenProfInput.Dist = Dist;
	//stGenProfInput.Vend = Vend;
	unsigned int nCase = 0;
	double fActualMaxVel, fActualMaxAccT1, fActualMaxAccT3;
	for(T1_trial = nT1_Lower;  T1_trial <= nT1_Upper; T1_trial ++)  // 20100728
	{
//		for( T3_trial = T3_min;  T3_trial <= T1_trial; T3_trial ++)
//		{
		T3_trial = nFitLen - T1_trial;
			fActualMaxVel = (Dist - Vend/2 * T3_trial) * 2/(T1_trial + T3_trial);
			fActualMaxAccT1 = fActualMaxVel * PI/2/T1_trial;
			fActualMaxAccT3 = (Vend - fActualMaxVel)*PI/2/T3_trial;
	        
			if(abs(fActualMaxVel) > MaxVel || abs(fActualMaxAccT1) > MaxAcc || abs(fActualMaxAccT3) > MaxAcc)
			{
				continue;
			}
			else
			{
				pstFitCases[nCase].stNonSymGenProfInputCases.dDist = Dist;
				pstFitCases[nCase].stNonSymGenProfInputCases.dVend = Vend;
				pstFitCases[nCase].stNonSymGenProfInputCases.uiT1 = T1_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.uiT2 = T2_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.uiT3 = T3_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.dActualMaxVel = fActualMaxVel;
				pstFitCases[nCase].stNonSymGenProfInputCases.dActualMaxAccT1 = fActualMaxAccT1;
				pstFitCases[nCase].stNonSymGenProfInputCases.dActualMaxAccT3 = fActualMaxAccT3;
				pstFitCases[nCase].stNonSymGenProfInputCases.dJerkFactor_T1 = fActualMaxAccT1 * PI/T1_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.dVelFactor1_T1 = fActualMaxAccT1 * T1_trial/PI;
				pstFitCases[nCase].stNonSymGenProfInputCases.dDistFactor_T1 = pstFitCases[nCase].stNonSymGenProfInputCases.dVelFactor1_T1 * T1_trial/PI;
				pstFitCases[nCase].stNonSymGenProfInputCases.dOmegaFactor_T1 = PI/T1_trial;

				pstFitCases[nCase].stNonSymGenProfInputCases.dVelFactor1_T3 = fActualMaxAccT3 * T3_trial/PI;
				pstFitCases[nCase].stNonSymGenProfInputCases.dJerkFactor_T3 = fActualMaxAccT3 *  PI/T3_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.dOmegaFactor_T3 = PI/T3_trial;
				pstFitCases[nCase].stNonSymGenProfInputCases.dDistFactor_T3 = pstFitCases[nCase].stNonSymGenProfInputCases.dVelFactor1_T3 * T3_trial/PI;

				pstFitCases[nCase].stNonSymGenProfInputCases.iPlotFlag = 0;
				//stCasesFindT(nCase).stGenProfInput = stGenProfInput;
				//[DistProf, VelProf, AccProf, JerkProf] = non_sym_sin_gen_prof(stGenProfInput);
				non_sym_gen_prof_dist_vel_2(&pstFitCases[nCase].stNonSymGenProfInputCases, &stNonSymProfOut);
				//stCasesFindT(nCase).DistProf = DistProf;
				//stCasesFindT(nCase).VelProf = VelProf;
				//stCasesFindT(nCase).AccProf = AccProf;
				//stCasesFindT(nCase).JerkProf = JerkProf;
				
				switch(stpInput->iFlagObjRule)
				{  
				case NON_SYM_FIT_RULE_WEIGHTED_RMS_ERR: 
				default:
					// stCasesFindT(nCase).fObj = calc_weighted_rms_err(DistProf, VelProf, TargetCurve, nFitLen);
					pstFitCases[nCase].pdObjValueCases = non_sym_calc_weighted_rms_err_2(stNonSymProfOut.pdDistProf, stNonSymProfOut.pdVelProf,
						TargetCurve, nFitLen);
					break;
				case NON_SYM_FIT_RULE_RMS_ERR:
					// stCasesFindT(nCase).fObj = calc_rms_err(DistProf, VelProf, TargetCurve, nFitLen);
					pstFitCases[nCase].pdObjValueCases = non_sym_calc_rms_err_2(stNonSymProfOut.pdDistProf, stNonSymProfOut.pdVelProf,
						TargetCurve, nFitLen);
					break;
				case NON_SYM_FIT_RULE_WEIGHTED_INV_RMS_ERR:

					// stCasesFindT(nCase).fObj = calc_weighted_inv_rms_err(DistProf, VelProf, TargetCurve, nFitLen);
					pstFitCases[nCase].pdObjValueCases = non_sym_calc_weighted_inv_rms_err_2(stNonSymProfOut.pdDistProf, stNonSymProfOut.pdVelProf,
						TargetCurve, nFitLen);
					break;
				}
				
				if(nCase == 0)
				{
					dMinObjValue = pstFitCases[nCase].pdObjValueCases;
					uiMinObjIdx = 0;
				}
				else
				{
					if(dMinObjValue > pstFitCases[nCase].pdObjValueCases)
					{
						dMinObjValue = pstFitCases[nCase].pdObjValueCases;
						uiMinObjIdx = nCase;
					}
				}

				if(iPlotFlag >= 4)
				{
					fprintf(fptrDebug, "Case: %d -- T1: %d, T2: %d, ObjValue: %10.6f\n",
						nCase, pstFitCases[nCase].stNonSymGenProfInputCases.uiT1, pstFitCases[nCase].stNonSymGenProfInputCases.uiT3,
						pstFitCases[nCase].pdObjValueCases);
				}
				nCase = nCase + 1;

				//if iPlotFlag >= 3
				//    figure(101);
				//    clf;
				//    plot(time_list, DistProf(time_list), time_list, TargetCurve(time_list), time_list, DistProf(time_list) - TargetCurve(time_list)', time_list, VelProf(time_list))
				//    legend('Dist', 'Target', 'Error', 'Velocity');
				//    if iPlotFlag >= 4
				//        strText = sprintf('T1: %d, T2: %d, T3: %d, Error: %f, any key...', T1_trial, T2_trial, T3_trial, aObjList(nCase));
				//        input(strText)
				//    else
				//        strText = sprintf('T1: %d, T2: %d, T3: %d, Error: %f', T1_trial, T2_trial, T3_trial, aObjList(nCase));
				//        disp(strText)
				//    end
				//end
			}
//		}
	}

//[fMinObj, idxMinObj] = min(aObjList);
//T1 = stCasesFindT(idxMinObj).stGenProfInput.T1;
//T3 = stCasesFindT(idxMinObj).stGenProfInput.T3;

	//pstFitCases[nCase].pdObjValueCases
	stpOutput->stNonSymGenProfIn = pstFitCases[uiMinObjIdx].stNonSymGenProfInputCases;
	stpOutput->dObjValue = dMinObjValue;

	if(iPlotFlag >= 3)
	{
		fprintf(fptrDebug, "CaseMinimum: %d -- T1: %d, T2: %d, ObjValue: %10.6f\n",
			uiMinObjIdx, stpOutput->stNonSymGenProfIn.uiT1, stpOutput->stNonSymGenProfIn.uiT3,
			stpOutput->dObjValue);
	}


//if iPlotFlag >= 1
//	figure(101);
//	clf;
//	plot(time_list, stCasesFindT(idxMinObj).DistProf(time_list), time_list, TargetCurve(time_list), ...
//        time_list, stCasesFindT(idxMinObj).DistProf(time_list) - TargetCurve(time_list)', time_list, stCasesFindT(idxMinObj).VelProf(time_list))
//	legend('Dist', 'Target', 'Error', 'Velocity');
//	strText = sprintf('Minimum Error Case -- T1: %d, T2: %d, T3: %d, Error: %f', T1, T2_trial, T3, stCasesFindT(idxMinObj).fObj);
//	title(strText);
//end
//stOutput = stCasesFindT(idxMinObj).stGenProfInput;
//stOutput.iPlotFlag = 1;
//stDebug.TargetVel = TargetVel;
//stDebug.TargetAcc = TargetAcc;
//stDebug.stCasesFindT= stCasesFindT;

	if(nFitLen > NON_SYM_LOCAL_VAR_LENGTH)
	{
		free(pdTargetAcc); //  = (double*) calloc(nFitLen, sizeof(double));
	}
	if(uiNumTotalCases > NON_SYM_FITTING_DEF_CASES)
	{
		//free(stpNonSymGenProfInputCases); // = (NON_SYM_GEN_PROF_INPUT *)calloc(uiNumTotalCases, sizeof(NON_SYM_GEN_PROF_INPUT));
		//free(pdObjValueCases); // = (double *)calloc(uiNumTotalCases, sizeof(double));
		free(pstFitCases); //  = (NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE **)calloc(uiNumTotalCases, sizeof(NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_CASE));
	}
	if((T1_nominal+2)*2 + 1 > NON_SYM_LOCAL_VAR_LENGTH)
	{
		free(stNonSymProfOut.pdDistProf); // = (double *) calloc((T1_nominal+2)*2 + 1, sizeof(double));
		free(stNonSymProfOut.pdVelProf); //   = (double *) calloc((T1_nominal+2)*2 + 1, sizeof(double));
	}

	if(iPlotFlag >= 3)
	{
		fclose(fptrDebug);
	}
	return 0; // OK

}
#ifdef __IMPLEMENT_LATER__
#endif // __IMPLEMENT_LATER__