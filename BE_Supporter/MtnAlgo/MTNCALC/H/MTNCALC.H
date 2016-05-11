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

/*---------------------------------------------------------------------------
//All the codes in the software are subject to the GNU General Public License("GPL") or 
//GNU Lesser General Public License("LGPL").
//Please see the GNU and LPGL Web sites to view the terms of each license.

 * Module       : mtncalc.h
 // History
 // YYYYMMDD  Author   Notes
 // 20081013  Zhengyi  Add constrained 7th order profile calculation
 *-------------------------------------------------------------------------*/
//#ifndef __MTNCALC_H__
//#define __MTNCALC_H__

#pragma once

#ifdef __cplusplus      /* For the mentally challenged */
extern "C" {
#endif

#include "mtndefin.h"


///////////////////////////////////////////////////////////////////////
//  4 Oct 2002
// Unit Conversion utilities for Cycloidal MinTime Move
extern MOT_ALGO_DBL ConvertTimeToDiscrete(MOT_ALGO_DBL Fs, MOT_ALGO_DBL Tms);
extern MOT_ALGO_DBL ConvertDToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Dist);
extern MOT_ALGO_DBL ConvertVToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Vmax);
extern MOT_ALGO_DBL ConvertAToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Amax);

extern MOT_ALGO_DBL ConvertDToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Dist);
extern MOT_ALGO_DBL ConvertVToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Vmax);
extern MOT_ALGO_DBL ConvertAToDiscrete(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Amax);

extern short CalcCycloidalMinTime(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Acck, MOT_ALGO_DBL Dmax,
                            MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3);
extern short CalcCycloidalMinTime_2(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Acck, MOT_ALGO_DBL Dmax,
                              MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                              MOT_ALGO_DBL *fAcc);
extern short CalcCycloidalMinTime_3(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Ak_Dk, MOT_ALGO_DBL Dmax,
                       MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3);
///////////////////////////////////////////////////////////////////////
//  4 Oct 2002
// To re-calculate symmetry factor k (-1<k<1) into *pK (-n<*pk<1)
// Used by ECORE motion library
extern short Calc_nonsymm_3odr_move(MOT_ALGO_DBL k, MOT_ALGO_DBL *pK);

extern short Calc3rdOrderProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL k,
                                 MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                                 double *pJk1, double *pJk3,
                                 double *pDeltVc, double *pDeltDf);

extern short Calc_3ordOrderTimeMove(MOT_ALGO_DBL Dist, MOT_ALGO_DBL k,
                                    MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                                    double *pJk1, double *pJk3,
                                    double *pDeltVc, double *pDeltDf);
///////////////////////////////////////////////////////////////////////

extern short Calc3rd2ndNsyProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T, MOT_ALGO_DBL k,
                                  MOT_ALGO_DBL *pT1st, MOT_ALGO_DBL *pT2nd,
                                  MOT_ALGO_DBL *pJerk1st, MOT_ALGO_DBL *pDec,
                                  MOT_ALGO_DBL *pDeltVc, MOT_ALGO_DBL *pDeltDf);

///////////////////////////////////////////////////////////////////////
typedef struct {
	short sTxy;
	short sT1xy;
	short sT1xyOverlapped;
	short sT3xyOverlapped;
} CALC_CYC_LINEAR_TRAJ_INPUT;	

typedef struct {
	short sT1z;
	short sT3z;
} CALC_CYC_LINEAR_TRAJ_OUTPUT;	

extern short CalcCycloidalLinearTrajectory(
      CALC_CYC_LINEAR_TRAJ_INPUT *stpInput,
      CALC_CYC_LINEAR_TRAJ_OUTPUT *stpOutput);

extern short CalcCycloidalAmax(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2,
                                 MOT_ALGO_DBL *pCalcAmax);

///////////////////////////////////////////////////////////////////////
extern short CalcProfCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL k,
                         MOT_ALGO_DBL *T1, MOT_ALGO_DBL *T2, MOT_ALGO_DBL *T3,
                         MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3);

///////////////////////////////////////////////////////////////////////
extern short CalcTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                      MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3);

extern short  Calc_generate_OBW_profile(MOT_ALGO_DBL fDist,
                                        MOT_ALGO_DBL fT1, MOT_ALGO_DBL fT2, MOT_ALGO_DBL fT3,
                                        MOT_ALGO_DBL *pTraj);
extern short  Calc_OBWMove_time_distance_relation(MOT_ALGO_DBL fTotalDist,
                                                  MOT_ALGO_DBL fDesiredDist,
                                                  MOT_ALGO_DBL fT1,
                                                  MOT_ALGO_DBL fT2,
                                                  MOT_ALGO_DBL fT3,
                                                  MOT_ALGO_DBL *pfRequiredTime);
///////////////////////////////////////////////////////////////////////
extern short CalcGTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel,
                       MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                       MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3, MOT_ALGO_DBL *c4, MOT_ALGO_DBL *c5);

extern short CheckGTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel,
                        MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                        MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3);

extern short   CalcNonSymCycloidalTimeMove(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fEndVel,
                                     MOT_ALGO_DBL T1,   MOT_ALGO_DBL T2,   MOT_ALGO_DBL T3,
                                     MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                                     MOT_ALGO_DBL *pC1, MOT_ALGO_DBL *pC2, MOT_ALGO_DBL *pC3,
                                     MOT_ALGO_DBL *pC4, MOT_ALGO_DBL *pC5, MOT_ALGO_DBL *pC6, MOT_ALGO_DBL *pC7);

extern short  Calc_Time_for_NonSymCycloidalTimeMove(MOT_ALGO_DBL fTotalTime,
                                              MOT_ALGO_DBL k,
                                              MOT_ALGO_DBL *pfT1,
                                              MOT_ALGO_DBL *pfT3);

extern short  Calc_generate_NonSymCycloidalTimeMove_profile( MOT_ALGO_DBL fDist,
                                                       MOT_ALGO_DBL fEndVel,
                                                       MOT_ALGO_DBL fT1,
                                                       MOT_ALGO_DBL fT2,
                                                       MOT_ALGO_DBL fT3,
                                                       MOT_ALGO_DBL *pTraj);
extern short  Calc_NonSymOBWTimeMove_time_distance_relation(MOT_ALGO_DBL fTotalDist,
                                                            MOT_ALGO_DBL fDesiredDist,
                                                            MOT_ALGO_DBL fEndVel,
                                                            MOT_ALGO_DBL fT1,
                                                            MOT_ALGO_DBL fT2,
                                                            MOT_ALGO_DBL fT3,
                                                            MOT_ALGO_DBL *pfRequiredTime);

//////////////////////////////////////////////////////////////////////////////////
#define __D0  -20
#define __D1  70
#define __D2  -84
#define __D3  35
#define __V0  -1
#define __V1  3
#define __V2  -3
#define __V3  1
#define __A0  -2
#define __A1  5
#define __A2  -4
#define __A3  1
#define __J0  -5
#define __J1  10
#define __J2  -6
#define __J3  1

extern short CalcProf7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        double *dConstVel, double *dConstAcc, double *dConstJerk,
                        MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal);
extern short CalcProf7thOrder(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Jmax,
                  MOT_ALGO_DBL *Tf, double *fConstVel, double *fConstAcc, double *fConstJerk,
                  MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal);

extern short  Calc_generate_7thOrder_profile(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        double fConstVel, double fConstAcc, double fConstJerk,
                        MOT_ALGO_DBL *pfPos, MOT_ALGO_DBL *pfVel, MOT_ALGO_DBL *pfAcc, MOT_ALGO_DBL *pfJerk);
extern short CalcMaxJerk_Prof7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        MOT_ALGO_DBL *JmaxReal);
extern short CalcRealTimeMotionLimits_Prof7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal,  MOT_ALGO_DBL *JmaxReal);

#ifdef __SINGLE_PRECISION__
extern short CalcProf7thOrder(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Jmax,
                  MOT_ALGO_DBL *Tf, MOT_ALGO_DBL *fConstVel, MOT_ALGO_DBL *fConstAcc, MOT_ALGO_DBL *fConstJerk,
                  MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal);

extern short  Calc_generate_7thOrder_profile(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        MOT_ALGO_DBL fConstVel, MOT_ALGO_DBL fConstAcc, MOT_ALGO_DBL fConstJerk,
                        MOT_ALGO_DBL *pfPos, MOT_ALGO_DBL *pfVel, MOT_ALGO_DBL *pfAcc, MOT_ALGO_DBL *pfJerk);
#endif // __SINGLE_PRECISION__

///////////////////////////////////////////////////////////////////////
extern short Calc2ndOrderProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax,
                                 MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                                 MOT_ALGO_DBL *pActAcc, MOT_ALGO_DBL *pDeltV);

#ifdef __cplusplus
}
#endif

// #endif
