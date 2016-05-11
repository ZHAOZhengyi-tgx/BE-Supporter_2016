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


#pragma once


#define POLY_7(k0, k1, k2, k3, k4, k5, k6, k7, x) (k0 + x*(k1 + x*(k2 + x*(k3 + x*(k4 + x* (k5 + x* (k6 + x * k7)))))))

// 20081013
typedef struct
{
	unsigned int TimeT_ini;
	double dDist;   
	double dVelIni; 
	double dVelEnd; 
	double dAccIni; 
	double dAccEnd; 
	double dJerkIni;
	double dJerkEnd;
}POLY7_CALC_K_BY_TIME_INPUT;

typedef struct
{
	unsigned int TimeT_ini;
	double dDist;   
	double dVelIni; 
	double dVelEnd; 
	double dAccIni; 
	double dAccEnd; 
	double dJerkIni;
	double dJerkEnd;
	double dK4;
	double dK5;
	double dK6;
	double dK7;
}POLY7_CALC_K_BY_TIME_OUTPUT;

#define NUM_ROW_MATRIX  4
#define NUM_COL_MATRIX  4
typedef struct
{
	double aMatrix[NUM_ROW_MATRIX * NUM_COL_MATRIX];
	double aInvMatrix[NUM_ROW_MATRIX * NUM_COL_MATRIX];
	double aRHS[NUM_ROW_MATRIX];
	double aSolution[NUM_ROW_MATRIX];
}POLY7_CALC_K_BY_TIME_DEBUG;

int poly7_calc_k_by_time(POLY7_CALC_K_BY_TIME_INPUT *stpInput, POLY7_CALC_K_BY_TIME_OUTPUT *stpOutput, POLY7_CALC_K_BY_TIME_DEBUG *stpDebug);

typedef struct
{
	unsigned int TimeT_ini;
	double dDist;   
	double dVelIni; 
	double dVelEnd; 
	double dAccIni; 
	double dAccEnd; 
	double dJerkIni;
	double dJerkEnd;
	double dK4;
	double dK5;
	double dK6;
	double dK7;
}POLY7_GEN_PROF_INPUT;

typedef struct
{
	double *pdDistProf;
	double *pdVelProf;
	double *pdAccProf;
	double *pdJerkProf;
}POLY7_GEN_PROF_OUTPUT;
void poly7_generate_prof(POLY7_GEN_PROF_INPUT *stpInput, POLY7_GEN_PROF_OUTPUT *stpOutput);
void poly7_generate_vel_prof(POLY7_GEN_PROF_INPUT *stpInput, POLY7_GEN_PROF_OUTPUT *stpOutput);
unsigned int get_time_prof7_verify_looping_vel_not_change_sign(POLY7_CALC_K_BY_TIME_INPUT *stpInput, unsigned int uiTimeT_min, unsigned int uiTimeT_max);

