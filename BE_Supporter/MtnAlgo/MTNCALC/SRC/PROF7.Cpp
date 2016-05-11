/*---------------------------------------------------------------------------
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



 * Module       : prof7.c
 * Programmers  : ZhengYi(John)
 * Created      : 05052003
 * Description:   7th order profile generation.
 *--------------------------------------------------------------------------
 * Revision History:
 * DDMMYYYY     Programmer      Changes
 * 07052003     ZhengYi         Improve algorithm for a better timing.
 * 09052003     ZhengYi         Improve to double precision calculation
 * 16052003     ZhengYi         Correct the implementation for EcoreDSP
 *                                  Interpolation
 * 21052003     ZhengYi         Implement the InterFactor for interpolation
 *                                  Simulation.
 * 14012004     ZhengYi         Add CalcProf7OrderTimeMove
 * 23082004     ZhebgYi         Add Jerk Constraints
 * 01022005     ZhengYi         Add MaximumJerkPoint Calculation
 * 19122005     ZhengYi         Fix bug
 *-------------------------------------------------------------------------*/
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include "mtncalc.h"
//#define __TEST__

extern int brinv(double a[],int n);
// Matrix multiplication a * b = c, a: m by n, b: n by k, c: m by k
extern void brmul(double *a, double *b, int m, int n, int k, double *c);

#define TOLERANCE   1.0e-30
#define ONE_THIRD   0.3333333333333
#define CONST_ACC     7.5131884
#define CONST_VEL     2.1875
#define CONST_JERK    52.5
/*-----------------------------------------------------------------------------
 * Function: CalcProf7OrderTimeMove
 * Date:     14012004
 *----------------------------------------------------------------------------*/
short CalcProf7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        double *dConstVel, double *dConstAcc, double *dConstJerk,
                        MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal)
{
double dTf_Inv;
double dTf_Inv_Order2;

    if(fTf <= 4.0)
    {
        fTf = 4.0;
    }

    dTf_Inv = 1/(fTf);
    dTf_Inv_Order2 = dTf_Inv * dTf_Inv;
    *dConstVel = fDist * 140 * dTf_Inv;
    *dConstAcc = fDist * 420 * dTf_Inv_Order2;
    *dConstJerk= fDist * 840 * dTf_Inv_Order2 * dTf_Inv;

    *AmaxReal = (MOT_ALGO_DBL) CONST_ACC * fDist * dTf_Inv_Order2;
    *VmaxReal = (MOT_ALGO_DBL) (CONST_VEL / sqrt(fabs(CONST_ACC)) )
                        * sqrt(fabs(fDist * (*AmaxReal) ));

    return 0;
}
/*-----------------------------------------------------------------------------
 * Function: CalcMaxJerk_Prof7OrderTimeMove
 * Date:     01022005
 *----------------------------------------------------------------------------*/
short CalcMaxJerk_Prof7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        MOT_ALGO_DBL *JmaxReal)
{

    if(fTf <= 4.0)
    {
        fTf = 4.0;
    }
  
    *JmaxReal = CONST_JERK* fabs(fDist)/ (fTf * fTf * fTf);

    return 0;
}

/*-----------------------------------------------------------------------------
 * Function: CalcRealTimeMotionLimits_Prof7OrderTimeMove
 * Date:     01022005
 *----------------------------------------------------------------------------*/
short CalcRealTimeMotionLimits_Prof7OrderTimeMove (MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal,  MOT_ALGO_DBL *JmaxReal)
{
double dTf_Inv;
double dTf_Inv_Order2;

    if(fTf <= 4.0)
    {
        fTf = 4.0;
    }

    dTf_Inv = 1/(fTf);
    dTf_Inv_Order2 = dTf_Inv * dTf_Inv;

    *AmaxReal = (MOT_ALGO_DBL) CONST_ACC * fDist * dTf_Inv_Order2;
    *VmaxReal = (MOT_ALGO_DBL) (CONST_VEL / sqrt(fabs(CONST_ACC)) )
                        * sqrt(fabs(fDist * (*AmaxReal) ));
    *JmaxReal = (MOT_ALGO_DBL)CONST_JERK* fabs(fDist)/ (fTf * fTf * fTf);

    return 0;
}

short CalcProf7thOrder(MOT_ALGO_DBL fDist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Jmax,
                  MOT_ALGO_DBL *fTf, double *dConstVel, double *dConstAcc, double *dConstJerk,
                  MOT_ALGO_DBL *AmaxReal, MOT_ALGO_DBL *VmaxReal)
{
    MOT_ALGO_DBL Tfv;
    MOT_ALGO_DBL Tfa;
    MOT_ALGO_DBL Tfj;
    double dTf_Inv;
    double dTf_Inv_Order2;

    if ((Amax < TOLERANCE) || (Vmax < TOLERANCE) || (Jmax < TOLERANCE))
    {
         *fTf = 0;
         return 0;
    }
    Tfv = floor(CONST_VEL*fabs(fDist/Vmax)) + 1.0;
    Tfa = floor(sqrt(fabs(CONST_ACC * fDist/Amax))) + 1.0;
    Tfj = floor(pow(fabs(CONST_JERK * fDist/Jmax) , ONE_THIRD)) + 1.0;

    if((Tfv > Tfa) && (Tfv > Tfj) )        // 23082004
    {
        *fTf = Tfv;
    }
    else if(Tfa > Tfj)
    {
        *fTf = Tfa;
    }
    else
    {
        *fTf = Tfj;
    }
    dTf_Inv = 1/(*fTf);
    dTf_Inv_Order2 = dTf_Inv * dTf_Inv;
    *dConstVel = fDist * 140 * dTf_Inv;
    *dConstAcc = fDist * 420 * dTf_Inv_Order2;
    *dConstJerk= fDist * 840 * dTf_Inv_Order2 * dTf_Inv;

    *AmaxReal = (MOT_ALGO_DBL) CONST_ACC * fDist * dTf_Inv_Order2;
    *VmaxReal = (MOT_ALGO_DBL) (CONST_VEL / sqrt(fabs(CONST_ACC)) )
                        * sqrt(fabs(fDist * (*AmaxReal) ));

    return 0;
}

short  Calc_generate_7thOrder_profile(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        double dConstVel, double fConstAcc, double fConstJerk,
                        MOT_ALGO_DBL *pfPos, MOT_ALGO_DBL *pfVel, MOT_ALGO_DBL *pfAcc, MOT_ALGO_DBL *pfJerk)
{
    unsigned long   i;
    unsigned long   lTf;
    double   dDeltT, dDeltT_Order2;

    if (pfPos == NULL || pfVel==NULL || pfAcc == NULL || pfJerk == NULL)
    {
        return MTN_CALC_ERROR;
    }
    if (fTf == 0.0)
    {
        *pfPos = *pfVel = *pfAcc = *pfJerk = 0;
        return MTN_CALC_SUCCESS;
    }
    lTf = (long) fTf;
    for (i = 0; i <= lTf; i++)  // counter should start from 0 but not 1, bug fixed on 19122005
    {
        dDeltT = i/fTf;
        dDeltT_Order2 = dDeltT * dDeltT;
        pfPos[i] =(MOT_ALGO_DBL) ( fDist * (__D3 + dDeltT*(__D2 + dDeltT*(__D1 + dDeltT*__D0 )))
                                        * dDeltT_Order2 * dDeltT_Order2 );
        pfVel[i] =(MOT_ALGO_DBL) ( dConstVel * (__V3 + dDeltT*(__V2 + dDeltT*(__V1 + dDeltT*__V0 )))
                                        * dDeltT_Order2 *dDeltT );
        pfAcc[i] =(MOT_ALGO_DBL) ( fConstAcc * (__A3 + dDeltT*(__A2 + dDeltT*(__A1 + dDeltT*__A0 )))
                                        * dDeltT_Order2 );
        pfJerk[i]=(MOT_ALGO_DBL) ( fConstJerk* (__J3 + dDeltT*(__J2 + dDeltT*(__J1 + dDeltT*__J0 )))
                                        * dDeltT );
    }

    return MTN_CALC_SUCCESS;
}
// 16052003
short  Calc_generate_7thOrder_profile_for_interpolator(
                        MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf, MOT_ALGO_DBL InterFactor,
                        double dConstVel, double dConstAcc, double dConstJerk,
                        MOT_ALGO_DBL *pfPos, MOT_ALGO_DBL *pfVel, MOT_ALGO_DBL *pfAcc, MOT_ALGO_DBL *pfJerk)
{
    unsigned long   i;
    unsigned long   lTf;
    double   dDeltT, dDeltT_Order2;

    if (pfPos == NULL || pfVel==NULL || pfAcc == NULL || pfJerk == NULL)
    {
        return MTN_CALC_ERROR;
    }
    if (fTf == 0.0)
    {
        *pfPos = *pfVel = *pfAcc = *pfJerk = 0;
        return MTN_CALC_SUCCESS;
    }
    lTf = (long) fTf;
    for (i = 0; i <= lTf; i++)
    {
        dDeltT = i/fTf;
        dDeltT_Order2 = dDeltT * dDeltT;
        pfPos[i] =(MOT_ALGO_DBL) ( fDist * (__D3 + dDeltT*(__D2 + dDeltT*(__D1 + dDeltT*__D0 )))
                                * dDeltT_Order2 * dDeltT_Order2) ;
        pfVel[i] =(MOT_ALGO_DBL) ( dConstVel * (__V3 + dDeltT*(__V2 + dDeltT*(__V1 + dDeltT*__V0 )))
                                * dDeltT_Order2 *dDeltT )/ InterFactor;
        pfAcc[i] =(MOT_ALGO_DBL) ( dConstAcc * (__A3 + dDeltT*(__A2 + dDeltT*(__A1 + dDeltT*__A0 )))
                                * dDeltT_Order2 )/ (InterFactor*InterFactor);
        pfJerk[i]=(MOT_ALGO_DBL) ( dConstJerk* (__J3 + dDeltT*(__J2 + dDeltT*(__J1 + dDeltT*__J0 )))
                                * dDeltT ) /(InterFactor *InterFactor *InterFactor);
    }

    return MTN_CALC_SUCCESS;
}

#ifdef __ERR_EMULATE_EC__

short  Calc_generate_7thOrder_profile_for_interpolator(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fTf,
                        double dConstVel, double dConstAcc, double dConstJerk,
                        MOT_ALGO_DBL *pfPos, MOT_ALGO_DBL *pfVel, MOT_ALGO_DBL *pfAcc, MOT_ALGO_DBL *pfJerk)
{
    unsigned long   i;
    unsigned long   lTf;
    double   dDeltT, dDeltT_Order2;

    if (pfPos == NULL || pfVel==NULL || pfAcc == NULL || pfJerk == NULL)
    {
        return MTN_CALC_ERROR;
    }
    if (fTf == 0.0)
    {
        *pfPos = *pfVel = *pfAcc = *pfJerk = 0;
        return MTN_CALC_SUCCESS;
    }
    lTf = (long) (fTf/2);
    for (i = 1; i <= lTf; i++)
    {
        dDeltT = 2*i/fTf;
        dDeltT_Order2 = dDeltT * dDeltT;
        pfPos[i] =(MOT_ALGO_DBL) ( fDist * (__D3 + dDeltT*(__D2 + dDeltT*(__D1 + dDeltT*__D0 )))
                                        * dDeltT_Order2 * dDeltT_Order2) ;
        pfVel[i] =(MOT_ALGO_DBL) ( dConstVel * (__V3 + dDeltT*(__V2 + dDeltT*(__V1 + dDeltT*__V0 )))
                                        * dDeltT_Order2 *dDeltT );
        pfAcc[i] =(MOT_ALGO_DBL) ( dConstAcc * (__A3 + dDeltT*(__A2 + dDeltT*(__A1 + dDeltT*__A0 )))
                                        * dDeltT_Order2 );
        pfJerk[i]=(MOT_ALGO_DBL) ( dConstJerk* (__J3 + dDeltT*(__J2 + dDeltT*(__J1 + dDeltT*__J0 )))
                                        * dDeltT );
    }

    return MTN_CALC_SUCCESS;
}
#endif
#ifdef __TEST__
void main(void)
{

    MOT_ALGO_DBL   *pfPos, *pfAcc, *pfVel, *pfJerk;
    int     i;
    MOT_ALGO_DBL   fDist, fVmax, fAmax, fJmax;
    MOT_ALGO_DBL   fTf, fAmaxReal, fVmaxReal, fInterFactor;
    double  dConstVel, dConstAcc, dConstJerk;
    long    lTf;
    char    cTypeCmd;
    FILE    *fp;

    fprintf(stderr, "Input travelling distance:");
    scanf("%f", &fDist);
    fprintf(stderr, "%f\n", fDist);
    fprintf(stderr, "Input Max Vel:");
    scanf("%f", &fVmax);
    fprintf(stderr, "%f\n", fVmax);
    fprintf(stderr, "Input Max Acc:");
    scanf("%f", &fAmax);
    fprintf(stderr, "%f\n", fAmax);
    fprintf(stderr, "Input Max Jerk:");
    scanf("%f", &fJmax);
    fprintf(stderr, "%f\n", fJmax);
    fprintf(stderr, "Input Interpolation Factor[CmdFreq/CtrlFreq]:");
    scanf("%f", &fInterFactor);
    fprintf(stderr, "Interpolation Factor[CmdFreq/CtrlFreq]: %f\n", fInterFactor);


    CalcProf7thOrder(fDist, fAmax, fVmax, fJmax,
                      &fTf, &dConstVel, &dConstAcc, &dConstJerk,
                        &fAmaxReal, &fVmaxReal);

    printf("Actual Amax: %f,  Actualy Vmax: %f,  Time: %f\n",
                            fAmaxReal, fVmaxReal, fTf);
    printf("\nConstVel: %f, ConstAcc: %f, ConstJerk: %f \n",
                            dConstVel, dConstAcc, dConstJerk);
    printf("[F] Generate final profile, [I] Generate profile for interpolator\n");
    cTypeCmd = getch();
    if(cTypeCmd == 'f' || cTypeCmd == 'F')
    {
        lTf = (long)fTf;
        pfPos = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfVel = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfAcc = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfJerk= (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        Calc_generate_7thOrder_profile(fDist, fTf,
                                dConstVel, dConstAcc, dConstJerk,
                                pfPos, pfVel, pfAcc, pfJerk);
    }
    else if(cTypeCmd == 'i' || cTypeCmd == 'I')
    {
        lTf = ceil(fTf);
        pfPos = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfVel = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfAcc = (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        pfJerk= (MOT_ALGO_DBL*) malloc( lTf*sizeof(MOT_ALGO_DBL));
        Calc_generate_7thOrder_profile_for_interpolator(
                            fDist, fTf, fInterFactor,
                            dConstVel, dConstAcc, dConstJerk,
                            pfPos, pfVel, pfAcc, pfJerk);

    }

    fp = fopen("Prof7ord.m", "w");
    fprintf(fp, "%%Condition:\n");
    fprintf(fp, "Dist = %f\n Vmax=%f\n Amax=%f\n Jmax=%f\n InterFactor=%f\n",
                        fDist, fVmax, fAmax, fJmax, fInterFactor);
    fprintf(fp, "%%Output: \n");
    fprintf(fp, "Dist = %f\n Tf =%f\n Amax=%f\n Vmax=%f\n",
                        fDist, fTf, fAmaxReal, fVmaxReal);

    fprintf(fp, "%%Pos  Vel     Acc     Jerk\n");
    fprintf(fp, "prof7order = [");
    for(i =0; i<=lTf; i++)
    {
        fprintf(fp, "%f     %f      %f      %f\n",
                    pfPos[i], pfVel[i], pfAcc[i], pfJerk[i]);
    }
    fprintf(fp, "];\n");
    flushall();
    free(pfPos);
    free(pfVel);
    free(pfAcc);
    free(pfJerk);

    fclose(fp);
}
#endif
