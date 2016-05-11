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


/////////////////////////////////////////////////////////////////////////////
//               旼컴컴컴컴컴컴커
//   Dist 컴컴컴?5th order    ?
// EndVel 컴컴컴?non-sym time 냐컴컴컴 C1, C2, C3, C4, C5, C6, C7
//     T1 컴컴컴?move calc.   ?
//     T2 컴컴컴?             냐컴컴컴 t1, t2 ,t3
//     T3 컴컴컴?             ?
//               읕컴컴컴컴컴컴켸
//
//
/////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include "mtncalc.h"

#define TOLERANCE   1.0e-30
#define PI 3.141592654

// #define __TEST__
//#define __TEST_GEN__
//#define __DEBUG__
extern short Calc__round(MOT_ALGO_DBL a);

/////////////////////////////////////////////////////////////////////
// *** Ported from MTRLIB -- OBWPCALC.C ***
// Function: Calc_Time_for_NonSymObwTimeMove
// Purpose : To calculate T1, T3 based on the user's ratio, k, and total time.
//           ONLY for non-symmetrical 5th Order Profile Move with ZERO
//           End Velocity.
// Input  : Total travel time,
//          Ratio, k in the range of 0 to 1;
// Output : T = [T1;T3];
//
// Remark : a) This routine is for Digital Bond-head application.
//
/////////////////////////////////////////////////////////////////////
short Calc_Time_for_NonSymObwTimeMove(MOT_ALGO_DBL fTotalTime,
                                      MOT_ALGO_DBL k,
                                      MOT_ALGO_DBL *pfT1,
                                      MOT_ALGO_DBL *pfT3)
{
    short sK;

    // This portion is added by CKTeo to patch DMU's problem.
    sK = (short) (k * 100.00);
    if (sK == 0)
    {
       k = 0.5;
    }

    // For protection against invalid ratio, k.
    if (k < 0.00)
    {
       k = 0.0001;
    }
    if (k > 1.00)
    {
       k = 0.9999;
    }

    *pfT1 = (MOT_ALGO_DBL) Calc__round(k * fTotalTime);
    *pfT3 = (fTotalTime - *pfT1);

    #ifdef __DEBUG__
    fprintf(stderr, "T1 %f T3 %f\n", *pfT1, *pfT3);
    #endif

    return MTN_CALC_SUCCESS;
}


////////////////////////////////////////////////////////////////////
// Function: CalcNonSymObwTimeMove
// Input:    Distance, EndVel, T1, T2, T3
// Output:   *pT1, *pT2, *pT3,
//           *pC1, *pC2, *pC3, *pC4, *pC5, *pC6, *pC7
// Notes:    This routine is divided into three major section:
//           1. Check the direction of the motion -- Error if bi-direction
//           2. Check the states of motion -- recalculate T1 & T3 if both
//              ramp up; if re-calculated (T1 < 0) || (T3 < 0), return Error
//           3. Re-calculate MaxVel
///////////////////////////////////////////////////////////////////
short   CalcNonSymObwTimeMove(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fEndVel,
                              MOT_ALGO_DBL T1,   MOT_ALGO_DBL T2,   MOT_ALGO_DBL T3,
                              MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                              MOT_ALGO_DBL *pC1, MOT_ALGO_DBL *pC2, MOT_ALGO_DBL *pC3,
                              MOT_ALGO_DBL *pC4, MOT_ALGO_DBL *pC5, MOT_ALGO_DBL *pC6, MOT_ALGO_DBL *pC7)
{
    MOT_ALGO_DBL       fMaxVel, fRampUp_Time, fRampDn_Time;
    MOT_ALGO_DBL       fD1;

   *pT1 = T1;
   *pT2 = 0;    // Temporary not Used 4 Oct 2002
   *pT3 = T3;

   if ((T1 < TOLERANCE) || (T3 < TOLERANCE))
   {
      return (MTN_CALC_ERROR);
   }

    fD1       = fEndVel * (T3);
    fMaxVel   = (2 * fDist - fD1) / (T1+T3);

    // Check the direction of motion
    if ( (fMaxVel * fDist) < TOLERANCE)
    {
        // The motion is bi-directional --> a Critical error!!
        return (MTN_CALC_ERROR);
    }

    // Check the 2 states of motion, is it both ramp up?
    if (fabs(fMaxVel) < fabs(fEndVel))
    {
        fRampUp_Time = floor(2*((T1+T3) - fDist/fEndVel));
        if (fRampUp_Time < 0)
        {
            // Error: Time < 0
            return (MTN_CALC_ERROR);
        }

        fRampDn_Time = (T1+T3) - fRampUp_Time;
        if (fRampDn_Time > 0)
        {
            // *pT1 and *pT3 are modified
            *pT1 = fRampUp_Time;
            *pT2 = 0;
            *pT3 = fRampDn_Time;
        }
        else        
        {
            // Error: T1 & T3 could not be changed -- Two steps ramp_up!
            // Critical Error!
            return (MTN_CALC_ERROR);
        }
    }

    // Re-calculate the motion parameters after the changes made to *pT1 and *pT3
    fD1             = fEndVel * (*pT3);
    fMaxVel         = (2 * fDist - fD1) / (*pT1 + *pT3);

    *pC1 = fMaxVel/2;   // VelConst_RampUp
    *pC2 = (fMaxVel - fEndVel)/2;  // VelConst_RampDn

    *pC3 = PI/2;

    *pC4 = PI/(*pT1);
    *pC5 = (*pT1)/PI;
    *pC6 = PI/(*pT3);
    *pC7 = (*pT3)/PI;

    #ifdef __TEST__
        printf("MaxVel %f\n", fMaxVel);
    #endif

    return (MTN_CALC_SUCCESS);
}

short Calc_generate_NonSymObwTimeMove_profile(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fEndVel,
                                       MOT_ALGO_DBL fT1,   MOT_ALGO_DBL fT2,   MOT_ALGO_DBL fT3,
                                       MOT_ALGO_DBL *pTraj)
{
    MOT_ALGO_DBL       fMaxVel;
    MOT_ALGO_DBL       fD1;
    MOT_ALGO_DBL       C1, C2, C3, C4, C5, C6, C7;
    unsigned long i;
    unsigned long T1, T2, T3;

    T1 = (unsigned long)fT1;
    T2 = (unsigned long)fT2;
    T3 = (unsigned long)fT3;

    if ((T1 < TOLERANCE) || (T3 < TOLERANCE))
    {
       *pTraj = 0;
       return (MTN_CALC_ERROR);
    }

    fD1             = fEndVel * (T3);
    fMaxVel         = (2 * fDist - fD1) / (T1 + T3);

    C1 = fMaxVel/2;   // VelConst_RampUp
    C2 = (fMaxVel - fEndVel)/2;  // VelConst_RampDn

    C3 = PI/2;

    C4 = PI/(T1);
    C5 = (T1)/PI;
    C6 = PI/(T3);
    C7 = (T3)/PI;

    for (i = 1; i <= (T1 + T3); i++)
    {
        if (i <= T1)
        {
           *(pTraj + i - 1) = C1 * (i - C5 * sin(C4 * i));
        }
        else
        {
           *(pTraj + i - 1) = C2 * ((i - T1) + C7 * sin(C6 * (i - T1))) +
                              fEndVel * (i - T1) + *(pTraj + T1 - 1);
        }
    }
    return MTN_CALC_SUCCESS;
}

short  Calc_NonSymOBWTimeMove_time_distance_relation(MOT_ALGO_DBL fTotalDist,
                                                     MOT_ALGO_DBL fDesiredDist,
                                                     MOT_ALGO_DBL fEndVel,
                                                     MOT_ALGO_DBL fT1,
                                                     MOT_ALGO_DBL fT2,
                                                     MOT_ALGO_DBL fT3,
                                                     MOT_ALGO_DBL *pfRequiredTime)

{
    unsigned int         i;
    MOT_ALGO_DBL       D, DT1;
    MOT_ALGO_DBL       fMaxVel;
    MOT_ALGO_DBL       fD1;
    MOT_ALGO_DBL       C1, C2, C3, C4, C5, C6, C7;
    unsigned long T1, T2, T3;

    // Initialize as 0
    *pfRequiredTime = 0;

    T1 = (unsigned long)fT1;
    T2 = (unsigned long)fT2;
    T3 = (unsigned long)fT3;
    if ((T1 < TOLERANCE) || (T3 < TOLERANCE))
    {
       return MTN_CALC_ERROR;
    }

    // Protection
    fTotalDist      = fabs(fTotalDist);
    fDesiredDist    = fabs(fDesiredDist);
    fEndVel         = fabs(fEndVel);
    if (fDesiredDist > fTotalDist)
    {
       fprintf(stderr, "Desired Dist = %f\n", fDesiredDist);
       fprintf(stderr, "Total   Dist = %f\n", fTotalDist);
       return MTN_CALC_ERROR;
    }
    if (fT1 == 0)
    {
       return MTN_CALC_SUCCESS;
    }

    // Calculate coefficients of time optimal BW profile
    fD1             = fEndVel * (T3);
    fMaxVel         = (2 * fTotalDist - fD1) / (T1 + T3);

    C1 = fMaxVel/2;   // VelConst_RampUp
    C2 = (fMaxVel - fEndVel)/2;  // VelConst_RampDn

    C3 = PI/2;

    C4 = PI/(T1);
    C5 = (T1)/PI;
    C6 = PI/(T3);
    C7 = (T3)/PI;

    for (i = 1; i <= (T1 + T3); i++)
    {
        if (i <= T1)
        {
           D = C1 * (i - C5 * sin(C4 * i));
           if (i == T1)
           {
                DT1 = D;
           }
        }
        else
        {
           D = C2 * ((i - T1) + C7 * sin(C6 * (i - T1))) +
               fEndVel * (i - T1) + DT1;
        }
        if (D >= fDesiredDist)
        {
           #ifdef __TEST_GEN__
           printf("%d: %f >= %f\n", (i-1), D, fDesiredDist);
           #endif

           *pfRequiredTime = i - 1;
           return MTN_CALC_SUCCESS;
        }
    }
    return MTN_CALC_ERROR;
}




#ifdef __TEST__
//#include <i86.h>
void main(void)
{
    MOT_ALGO_DBL   fDist, fEndVel;
    MOT_ALGO_DBL   T1, T2, T3;
    MOT_ALGO_DBL   t1, t2, t3;
    MOT_ALGO_DBL   C1, C2, C3, C4, C5, C6, C7;
    short   sStatus;

    printf("\n[OBW NonSymm Time Move]\n");
    printf("\nDistance =");
    scanf("%f", &fDist);
    printf("\nEndVel =");
    scanf("%f", &fEndVel);
    printf("\nT1 = ");
    scanf("%f", &T1);
    printf("\nT2 = ");
    scanf("%f", &T2);
    printf("\nT3 = ");
    scanf("%f", &T3);

    sStatus = CalcNonSymObwTimeMove(fDist, fEndVel,
                                    T1, T2, T3,
                                    &t1, &t2, &t3,
                                    &C1, &C2, &C3,
                                    &C4, &C5, &C6, &C7);

    if (sStatus == MTN_CALC_ERROR)
    {
        printf("Error!\n");
        return;
    }
    printf("\nT1 %f   T2 %f   T3 %f\n", T1, T2, T3);
    printf("t1 %f   t2 %f   t3 %f\n", t1, t2, t3);
    printf("C1 %f   C2 %f   C3 %f\n", C1, C2, C3);
    printf("C4 %f   C5 %f   C6 %f   c7 %f\n", C4, C5, C6, C7);
}
#endif

#ifdef __TEST_GEN__
short Calc__round(MOT_ALGO_DBL a)
{
    if ((ceil(a) - a) > 0.5f)
    {
       a = ceil(a) - 1.0f;
    }
    else
    {
       a = ceil(a);
    }
    return ((short) a);
}

void main(void)
{
    MOT_ALGO_DBL Traj[100];
    int   i;
    MOT_ALGO_DBL fRequiredTime;
    MOT_ALGO_DBL fT1;
    float fT3;
   /*
    Calc_generate_NonSymObwTimeMove_profile(100,
                                            20, // EndVel
                                            10, // T1
                                            0, // T2
                                            10, // T3
                                            Traj);
    for (i=0; i<20; i++)
    {
        printf("%d: Dist = %f\n", i, Traj[i]);
    }
    Calc_NonSymOBWTimeMove_time_distance_relation(100,
                                                  85,
                                                  20,
                                                  10,
                                                  0,
                                                  10,
                                                  &fRequiredTime);
    printf("Time %f\n", fRequiredTime);
  */
    Calc_Time_for_NonSymObwTimeMove(23,
                                    0.8,
                                    &fT1,
                                    &fT3);

}
#endif

