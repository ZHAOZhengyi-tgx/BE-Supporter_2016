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

// Test_non_sym_looping.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "malloc.h"
#include "conio.h"

//double adTargetCurve[] = { 0, -0.5, -3.94,  -12.66,  -29.95,  -59.47, -104.85, -169.23, -254.83, -362.67, -492.35, -642.03, -808.47,
//-987.2, -1172.8,   -1359, -1539.1, -1705.9, -1851.7, -1966.7, -2030.1, -2065.1, -2082.6};

//double adTargetCurve[] = {  0,  -5.92,  -14.51, -24.59, -36.16, -49.26, -63.91, -80.12, -97.87, -117.18, -138.03, -160.39, -184.25, -209.56, -236.29, -264.39, -293.81, -324.49, -356.35, -389.33, -423.36, -458.34,  -494.2, -530.83, 
//-568.14, -606.03,  -644.4, -683.12, -722.09, -761.18, -800.27, -839.24, -877.93, -916.22, -953.95, -990.97, -1027.1, -1062.2,   -1096, -1128.3, -1158.8, -1187.4, -1213.7, -1237.5, -1258.3, -1276.1, -1290.4, -1301.4, -1309.2, -1314.1, -1316.6, -1317.6, -1317.7};

//double adTargetCurve[] = { 0,  -5.92, -14.51, -24.59, -36.16, -49.26, -63.91, -80.12, -97.87, -117.18, -138.03, -160.39, -184.25, -209.56, -236.29, -264.39, 
//-293.81, -324.49, -356.35, -389.33, -423.36, -458.34, -494.2, -530.83, -568.14, -606.03, -644.4, -683.12, -722.09, -761.18, -800.27, -839.24, -877.93, -916.22, 
//-953.95, -990.97, -1027.1, -1062.2,  -1096, -1128.3, -1158.8, -1187.4, -1213.7, -1237.5, -1258.3, -1276.1, -1290.4, -1301.4, -1309.2, -1314.1, -1316.6, -1317.6};

//double adTargetCurve[] = {0, -1.51, -8.41, -16.52, -25.88, -36.5, -48.39, -61.56, -75.96, -91.58, -108.36, -126.23, -145.12, -164.94, -185.57, -206.9, 
//-228.8, -251.12, -273.72, -296.42, -319.08, -341.5, -363.54, -385.01, -405.74, -425.59, -444.4, -462.05, -478.41, -493.4, -506.95, -519.01, -529.57, 
//-538.64, -546.26, -552.5, -557.45, -561.24, -563.98, -565.84, -566.97, -567.56, -567.77};

//double adTargetCurve[] = {0, -0.41, -6.35, -14.35, -24.43, -36.59, -50.79, -67.01, -85.2, -105.29, -127.21, -150.86, -176.15, -202.95, -231.15, -260.6, 
//-291.15, -322.65, -354.92, -387.79, -421.07, -454.56, -488.05, -521.32, -554.13, -586.23, -617.35, -647.21, -675.48, -701.82, -725.88, -747.27, -765.66, 
//-780.76, -792.42, -800.73, -805.98, -808.76, -809.79};

// Data_traj_11_2010-08-01_TrajData-1.txt, XinDa from FuJian, 
//double adTargetCurve[] = {0, -0.6, -2.93, -6.21, -10.48, -15.8, -22.23, -29.8, -38.57, -48.56, -59.8, -72.32, -86.13, -101.24, -117.66, -135.38, -154.4, 
//-174.69, -196.24, -219.01, -242.97, -268.07, -294.27, -321.51, -349.72, -378.84, -408.8, -439.5, -470.86, -502.78, -535.17, -567.92, -600.92, -634.04, 
//-667.16, -700.15, -732.87, -765.17, -796.9, -827.91, -858.03, -887.1, -914.94, -941.38, -966.25, -989.39, -1010.7, -1029.9, -1047, -1062, -1074.7, 
//-1085.3, -1093.8, -1100.3, -1105.1, -1108.3, -1110.3, -1111.3, -1111.7};

// Data_traj_10_2010-08-01_TrajData.txt
//double adTargetCurve[] = {0, -0.83, -3.13, -6.36, -10.6, -15.89, -22.29, -29.84, -38.58, -48.54, -59.76, -72.26, -86.05, -101.15, -117.55, -135.26, -154.27,
//-174.55, -196.09, -218.86, -242.82, -267.93, -294.13, -321.37, -349.59,  -378.72, -408.69, -439.4, -470.77, -502.71, -535.12, -567.88, -600.89, -634.03, -667.17, 
//-700.17, -732.91, -765.22, -796.97, -827.99, -858.13, -887.2, -915.05, -941.5, -966.38, -989.52, -1010.8, -1030, -1047.2, -1062.1, -1074.9, -1085.4, -1093.9, -1100.4,
//-1105.2, -1108.5, -1110.4, -1111.5, -1111.8};

// Data_traj_12_2010-10-08_TrajData.txt
//double adTargetCurve[] = {0, -1.53, -4.85, -9.28, -14.82, -21.47, -29.23, -38.07, -47.99, -58.94, -70.92, -83.87, -97.78, -112.61, -128.31, -144.86, -162.22, -180.36, -199.25, -218.85, -239.13, -260.06, -281.6, 
//-303.73, -326.4, -349.58, -373.22, -397.29, -421.72, -446.46, -471.45, -496.62, -521.87, -547.13, -572.28, -597.21, -621.79, -645.88, -669.31, -691.91, -713.48, -733.81, -752.69, -769.88, -785.18,
// -798.4, -809.43, -818.21, -824.8, -829.38, -832.23, -833.71, -834.26};

double adTargetCurve[] = {0, -1.53, -4.85, -9.28, -14.82, -21.47, -29.23, -38.07, -47.99, -58.94, -70.92, -83.87, -97.78, -112.61, -128.31, -144.86, -180.36, -218.85, -260.06, 
-303.73, -349.58, -382.19};

unsigned int nLenDouble = sizeof(double);
unsigned int nLenArray = sizeof(adTargetCurve)/nLenDouble; //sizeof(double);

#include "non_sym_2.h"

NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_INPUT stNonSymCycFitCurveInput;
NON_SYM_CYCLOIDAL_CALC_T_FIT_CURVE_OUTPUT stNonSymCycFitCurveOutput;

int non_sym_gen_prof_2(NON_SYM_GEN_PROF_INPUT *stpInput, NON_SYM_GEN_PROF_OUTPUT *stpOutput);

NON_SYM_GEN_PROF_OUTPUT stNonSymProf;

#define DEF_LOCAL_VAR_LENGTH   1024
double DistProf[DEF_LOCAL_VAR_LENGTH];
double VelProf[DEF_LOCAL_VAR_LENGTH];
double AccProf[DEF_LOCAL_VAR_LENGTH];
double JerkProf[DEF_LOCAL_VAR_LENGTH];

#define DDIST		(-382.9)
#define DMAXACC		(28.125)
#define DMAXVEL		(487.5)
#define DVEND		(-6.25)
#define NTARGETLEN (22)

#define SAMPLE_FREQ  2000
#define ENC_RES_M    (0.2E-6)
int _tmain(int argc, _TCHAR* argv[])
{

	unsigned int uiTotalPoint;
	FILE *fptrDebug;

//	nLenArray = sizeof(adTargetCurve);
//	nLenArray = nLenArray/8;

	stNonSymCycFitCurveInput.dDist = adTargetCurve[nLenArray-1];
	stNonSymCycFitCurveInput.dMaxAcc = DMAXACC; // 1000 / ENC_RES_M / SAMPLE_FREQ /SAMPLE_FREQ;
	stNonSymCycFitCurveInput.dMaxVel = DMAXVEL; // 1 / ENC_RES_M / SAMPLE_FREQ ;
	stNonSymCycFitCurveInput.dVend = DVEND; // adTargetCurve[nLenArray -1] - adTargetCurve[nLenArray -2];
	stNonSymCycFitCurveInput.iFlagObjRule = NON_SYM_FIT_RULE_WEIGHTED_INV_RMS_ERR;
	stNonSymCycFitCurveInput.iPlotFlag = 3;
	stNonSymCycFitCurveInput.nFitLen = NTARGETLEN;
	stNonSymCycFitCurveInput.pdTargetCurve = &adTargetCurve[0];
	stNonSymCycFitCurveInput.nTargetLen = NTARGETLEN;

	if(non_sym_cycloidal_calc_t_fit_curve_2(&stNonSymCycFitCurveInput, 
									   &stNonSymCycFitCurveOutput) != 0)
	{
		fprintf(stderr, "Error");
		getch();
	}
	else
	{
		uiTotalPoint = stNonSymCycFitCurveOutput.stNonSymGenProfIn.uiT1 + stNonSymCycFitCurveOutput.stNonSymGenProfIn.uiT3;
		if(uiTotalPoint + 1 > DEF_LOCAL_VAR_LENGTH)
		{
			stNonSymProf.pdAccProf = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			stNonSymProf.pdDistProf = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			stNonSymProf.pdJerkProf = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			stNonSymProf.pdVelProf = (double*) calloc(uiTotalPoint + 1, sizeof(double));
		}
		else
		{
			stNonSymProf.pdAccProf = &AccProf[0];
			stNonSymProf.pdDistProf = &DistProf[0];
			stNonSymProf.pdJerkProf = &JerkProf[0];
			stNonSymProf.pdVelProf = &VelProf[0];
		}
		non_sym_gen_prof_2(&(stNonSymCycFitCurveOutput.stNonSymGenProfIn), &stNonSymProf);


		fopen_s(&fptrDebug, NON_SYM_CYCLOIDAL_FIT_CURVE_DEBUG_FILE, "a");
		fprintf(fptrDebug, "\n\n Final Profile, Total Time: %d\n", uiTotalPoint + 1);
		fprintf(fptrDebug, "Index, \tDistProf,  \tVelProf,   \tAccProf,  \tJerkProf \n");
		for(unsigned int ii=0; ii<= uiTotalPoint; ii++)
		{
			fprintf(fptrDebug, "%d,\t%12.6f,  \t%10.6f, \t%10.6f,  \t%8.6f\n", ii,
				stNonSymProf.pdDistProf[ii], stNonSymProf.pdVelProf[ii], stNonSymProf.pdAccProf[ii], stNonSymProf.pdJerkProf[ii]);

		}

		fclose(fptrDebug);


		if(uiTotalPoint + 1 > DEF_LOCAL_VAR_LENGTH)
		{
			free(stNonSymProf.pdAccProf); // = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			free(stNonSymProf.pdDistProf); // = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			free(stNonSymProf.pdJerkProf); // = (double*) calloc(uiTotalPoint + 1, sizeof(double));
			free(stNonSymProf.pdVelProf); // = (double*) calloc(uiTotalPoint + 1, sizeof(double));
		}

	}

	return 0;
}

