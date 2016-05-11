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



#include "poly7_bnd.h"
#include "malloc.h"
#include "math.h"

extern int brinv(double a[],int n);
// Matrix multiplication a * b = c, a: m by n, b: n by k, c: m by k
extern void brmul(double *a, double *b, int m, int n, int k, double *c);

// function [stOutput, stDebug] = poly7_calc_k_by_time(stPoly7CalcK_ByTime)
// Constraint is such that
// T, CmdDist, VelIni, VelEnd, AccIni, AccEnd, JerkIni, JerkEnd
//   Dist(0) = 0
//   Dist(T) = CmdDist
//   Vel(0) = V_ini
//   Vel(T) = V_end
//   Acc(0) = AccIni
//   Acc(T) = AccEnd
//   Jerk(0) = J_ini
//   Jerk(T) = J_end
int poly7_calc_k_by_time(POLY7_CALC_K_BY_TIME_INPUT *stpInput, POLY7_CALC_K_BY_TIME_OUTPUT *stpOutput, POLY7_CALC_K_BY_TIME_DEBUG *stpDebug)
{

	double *pMat, *pRHS, *aInvMatrix, *pSolution;
	static double dT = (double)stpInput->TimeT_ini;
	static double CmdDist = stpInput->dDist;
	static double VelIni  = stpInput->dVelIni; 
	static double VelEnd  = stpInput->dVelEnd; 
	static double AccIni  = stpInput->dAccIni; 
	static double AccEnd  = stpInput->dAccEnd; 
	static double JerkIni = stpInput->dJerkIni; 
	static double JerkEnd = stpInput->dJerkEnd;

	static double dT_sq = dT * dT;
	static double dTcube = dT * dT_sq;
	static double dTp4 = dT_sq * dT_sq;
	static double dTp5 = dT_sq * dTcube;
	static double dTp6 = dT_sq * dTp4;

	pMat = stpDebug->aMatrix;
	pRHS = stpDebug->aRHS;
	aInvMatrix = stpDebug->aInvMatrix;
	pSolution = stpDebug->aSolution;

	pMat[0* NUM_COL_MATRIX + 0] =  dTp4;
	pMat[0* NUM_COL_MATRIX + 1] =  dTp5;
	pMat[0* NUM_COL_MATRIX + 2] =  dTp6;
	pMat[0* NUM_COL_MATRIX + 3] =  dTp4 * dTcube;

	pMat[1* NUM_COL_MATRIX + 0] =  4 * dTcube;
	pMat[1* NUM_COL_MATRIX + 1] =  5 * dTp4;
	pMat[1* NUM_COL_MATRIX + 2] =  6 * dTp5;
	pMat[1* NUM_COL_MATRIX + 3] =  7 * dTp6;

	pMat[2* NUM_COL_MATRIX + 0] =  12  *dT_sq;
	pMat[2* NUM_COL_MATRIX + 1] =  20 * dTcube;
	pMat[2* NUM_COL_MATRIX + 2] =  30 * dTp4;
	pMat[2* NUM_COL_MATRIX + 3] =  42 * dTp5;

	pMat[3* NUM_COL_MATRIX + 0] =  24 * dT;
	pMat[3* NUM_COL_MATRIX + 1] =  60 * dT_sq;
	pMat[3* NUM_COL_MATRIX + 2] =  120 *dTcube;
	pMat[3* NUM_COL_MATRIX + 3] =  210 *dTp4;

	pRHS[0] = CmdDist - (VelIni * dT + AccIni /2* dT_sq + JerkIni/6 * dTcube); 
	pRHS[1] = VelEnd - (VelIni + AccIni * dT + JerkIni/2 * dT_sq); 
	pRHS[2] = AccEnd - (AccIni + JerkIni * dT); 
	pRHS[3] = (JerkEnd - JerkIni);

	for(unsigned int ii = 0; ii< NUM_ROW_MATRIX * NUM_COL_MATRIX; ii++)
	{
		aInvMatrix[ii] = pMat[ii];
	}

	if(brinv(aInvMatrix, NUM_ROW_MATRIX) == 0)// inv(M)
	{
		return 1; // ERROR
	}

	brmul(aInvMatrix, pRHS, NUM_ROW_MATRIX, NUM_ROW_MATRIX, 1, pSolution);  // K = inv(M) * RHS;

	stpOutput->dK4 = pSolution[0];
	stpOutput->dK5 = pSolution[1];
	stpOutput->dK6 = pSolution[2];
	stpOutput->dK7 = pSolution[3];
	
	stpOutput->dDist     = stpInput->dDist    ;
	stpOutput->dVelIni   = stpInput->dVelIni  ;
	stpOutput->dVelEnd   = stpInput->dVelEnd  ;
	stpOutput->dAccIni   = stpInput->dAccIni  ;
	stpOutput->dAccEnd   = stpInput->dAccEnd  ;
	stpOutput->dJerkIni  = stpInput->dJerkIni ;
	stpOutput->dJerkEnd  = stpInput->dJerkEnd ;
	stpOutput->TimeT_ini = stpInput->TimeT_ini;

	return 0;
}


// [DistProf, VelProf, AccProf, JerkProf]= poly7_generate_prof(stPoly7GenProfInput)
//  Constraint is such that
//    Dist(0) = 0
//    Dist(T) = CmdDist
//    Vel(0) = V_ini
//    Vel(T) = V_end
//    Acc(0) = A_ini
//    Acc(T) = A_end
//    Jerk(0) = J_ini
//    Jerk(T) = J_end
void poly7_generate_prof(POLY7_GEN_PROF_INPUT *stpInput, POLY7_GEN_PROF_OUTPUT *stpOutput)
{

	static double k4       = stpInput->dK4       ; 
	static double k5       = stpInput->dK5       ; 
	static double k6       = stpInput->dK6       ; 
	static double k7       = stpInput->dK7       ; 
	static double dT = (double)stpInput->TimeT_ini;
	static double CmdDist = stpInput->dDist;
	static double VelIni  = stpInput->dVelIni; 
	static double VelEnd  = stpInput->dVelEnd; 
	static double AccIni  = stpInput->dAccIni; 
	static double AccEnd  = stpInput->dAccEnd; 
	static double JerkIni = stpInput->dJerkIni; 
	static double JerkEnd = stpInput->dJerkEnd;
	
	for(unsigned int ii = 0; ii<= stpInput->TimeT_ini; ii ++)
	{
		if( ii == 0)
		{
			stpOutput->pdDistProf[ii] = 0;
			stpOutput->pdVelProf[ii] = VelIni;
			stpOutput->pdAccProf[ii] = AccIni;
			stpOutput->pdJerkProf[ii] = JerkIni;
		}   
		else if(ii == stpInput->TimeT_ini) // TimeT + 1
		{
			stpOutput->pdDistProf[ii] = CmdDist;
			stpOutput->pdVelProf[ii] = VelEnd;
			stpOutput->pdAccProf[ii] = AccEnd;
			stpOutput->pdJerkProf[ii] = JerkEnd;
		}   
		else
		{
			stpOutput->pdDistProf[ii] = POLY_7(0,     VelIni, AccIni/2, JerkIni/6, k4,    k5,    k6,   k7, (ii));
			stpOutput->pdVelProf[ii] = POLY_7(VelIni,  AccIni, JerkIni/2, 4*k4,    5*k5,  6*k6,  7*k7, 0.0,  (ii));
			stpOutput->pdAccProf[ii] = POLY_7(AccIni,  JerkIni, 12*k4,   20*k5,   30*k6, 42*k7, 0,    0.0,  (ii));
			stpOutput->pdJerkProf[ii] = POLY_7(JerkIni, 24*k4, 60*k5,   120*k6,  210*k7,0,     0,    0.0,  (ii));
		}
	}
}



// [DistProf, VelProf, AccProf, JerkProf]= poly7_generate_prof(stPoly7GenProfInput)
//  Constraint is such that
//    Dist(0) = 0
//    Dist(T) = CmdDist
//    Vel(0) = V_ini
//    Vel(T) = V_end
//    Acc(0) = A_ini
//    Acc(T) = A_end
//    Jerk(0) = J_ini
//    Jerk(T) = J_end
void poly7_generate_vel_prof(POLY7_GEN_PROF_INPUT *stpInput, POLY7_GEN_PROF_OUTPUT *stpOutput)
{

	static double k4       = stpInput->dK4       ; 
	static double k5       = stpInput->dK5       ; 
	static double k6       = stpInput->dK6       ; 
	static double k7       = stpInput->dK7       ; 
	static double dT = (double)stpInput->TimeT_ini;
	static double CmdDist = stpInput->dDist;
	static double VelIni  = stpInput->dVelIni; 
	static double VelEnd  = stpInput->dVelEnd; 
	static double AccIni  = stpInput->dAccIni; 
	static double AccEnd  = stpInput->dAccEnd; 
	static double JerkIni = stpInput->dJerkIni; 
	static double JerkEnd = stpInput->dJerkEnd;
	
	for(unsigned int ii = 0; ii<= stpInput->TimeT_ini; ii ++)
	{
		if( ii == 0)
		{
			stpOutput->pdVelProf[ii] = VelIni;
		}   
		else if(ii == stpInput->TimeT_ini) // TimeT + 1
		{
			stpOutput->pdVelProf[ii] = VelEnd;
		}   
		else
		{
			stpOutput->pdVelProf[ii] = POLY_7(VelIni,  AccIni, JerkIni/2, 4*k4,    5*k5,  6*k6,  7*k7, 0.0,  (ii));
		}
	}
}

#define DEF_MEM_SIZE 128
static double gadDistProf[DEF_MEM_SIZE];
static double gadVelProf[DEF_MEM_SIZE];
static double gadAccProf[DEF_MEM_SIZE];
static double gadJerkProf[DEF_MEM_SIZE];

unsigned int get_time_prof7_verify_looping_vel_not_change_sign(POLY7_CALC_K_BY_TIME_INPUT *stpInput, unsigned int uiTimeT_min, unsigned int uiTimeT_max)
{
	unsigned int ii, uiTimeT = 0, uiTimeTotal, uiFlagVelChangSign;

	static POLY7_CALC_K_BY_TIME_DEBUG stPoly7CalcK_ByTimeDebug;
	static union
	{
		POLY7_CALC_K_BY_TIME_OUTPUT stPoly7CalcK_ByTimeOutput;
		POLY7_GEN_PROF_INPUT stPoly7GenProfInput;
	}uCalcKOutGenProfInput;
	static POLY7_GEN_PROF_OUTPUT stPoly7Prof;


// Input uiTimeT_min, uiTimeT_max
	for(unsigned int tt = uiTimeT_min; tt <= uiTimeT_max; tt++)
	{
		stpInput->TimeT_ini = tt;
		poly7_calc_k_by_time(stpInput, &uCalcKOutGenProfInput.stPoly7CalcK_ByTimeOutput, &stPoly7CalcK_ByTimeDebug);

		uiTimeTotal = uCalcKOutGenProfInput.stPoly7CalcK_ByTimeOutput.TimeT_ini + 1;

		if(uiTimeTotal > DEF_MEM_SIZE)
		{
			stPoly7Prof.pdVelProf = (double *) calloc(uiTimeTotal, sizeof(double));
		}
		else
		{
			stPoly7Prof.pdVelProf = gadVelProf;
		}
		poly7_generate_vel_prof(&uCalcKOutGenProfInput.stPoly7GenProfInput, &stPoly7Prof);
		uiFlagVelChangSign = 0;
		for(ii = 1; ii<uiTimeTotal ; ii++)
		{
			if(stpInput->dVelIni * stPoly7Prof.pdVelProf[ii] < 0 && fabs(stPoly7Prof.pdVelProf[ii]) > 1)
			{
				uiFlagVelChangSign = 1;
				ii = uiTimeTotal;
			}
		}
		if(uiFlagVelChangSign == 0)
		{
			uiTimeT = tt;
			break;
		}
	}

	return uiTimeT;
}
