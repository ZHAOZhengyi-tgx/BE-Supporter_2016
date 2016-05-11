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


#include <math.h>
#include <stdio.h>
#include "mtncalc.h"

#define TOLERANCE   1.0e-30
#define ONE_HALF    0.50
#define ONE_THIRD   1.00/3.00
#define SQUARE      2.00
#define CUBE        3.00
#define PI          3.141592654

//#define __TEST__
//#define DEBUG
//#define __EAGLE_FOR_REFERENCE_ONLY__

#ifdef __REMOVE_LATER__
// KKTAN
// 4 Oct 2002  NOT USED ANY MORE!!!
short CalcObwMinTime(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Acck, MOT_ALGO_DBL Dmax,
                     MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3)
{
    MOT_ALGO_DBL Dk;
    MOT_ALGO_DBL DaccMax;
    MOT_ALGO_DBL k1;
    MOT_ALGO_DBL k2;
    MOT_ALGO_DBL Acc;
    MOT_ALGO_DBL Dnorm;
    MOT_ALGO_DBL Tm;
    MOT_ALGO_DBL Tcv;
    MOT_ALGO_DBL Temp;

    if ((Amax < TOLERANCE) || (Vmax < TOLERANCE) || (Acck < TOLERANCE))
    {
    #ifdef __TEST__
        printf("T1 = T2 = T3 = 0\n");
    #endif
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;
       return 0;
    }

    DaccMax = PI * pow(Vmax, SQUARE) / (Amax * 2.00);
    Temp = 3.00 * Amax - 2.00 * Acck;
    if (Temp > 0)
    {
       Dk = Acck * Dmax / Temp;
    }
    else
    {
       return 1;
    }
    k1 = sqrt(2.00 * PI * pow(Dk, ONE_THIRD) / Acck);
    k2 = Acck / (3.00 * Dk);

    if (Dist > DaccMax)
    {
       Tcv = (Dist - DaccMax) / Vmax;
       Dnorm = DaccMax;
    }
    else
    {
       Tcv = 0;
       Dnorm = Dist;
    }

    if ((Dnorm - Dk) < TOLERANCE)
    {
       Acc = 2.00 * PI * pow(Dnorm, ONE_THIRD) / (k1 * k1);
    }
    else if ((Dnorm - Dmax) < TOLERANCE)
    {
       Acc = k2 * (Dnorm - Dk) + Acck;
    }
    else
    {
       Acc = Amax;
    }

    if (Acc > 0.00)
    {
       Tm = sqrt(2.00 * PI * Dnorm / Acc);
    }
    else
    {
       return 1;
    }

    *pT1 = *pT3 = Tm / 2.00;
    *pT2 = Tcv;

    return 0;
}
#endif


///////////////////////////////////////////////////////////////////
// Function:    CalcObwMinTime
// Purpose:     Routine to calculate T1, T2, T3 for OBW Min Time.
// Former name: HLCalcObwMinTime() to patch bug in older CalcObwMinTime()
// Input:       Amax, Vmax, Ak_Dk, Dmax, Distance
// Output:      *pT1, *pT2 , *pT3
// Notes:       Previously this routine uses Acck as one of the condition
//              for calculation of T1, T2, T3. When Amax is smaller than
//              the Acck, it violate the calculation condition.
//              Hence the limiting condition Acck is changed to Dk (Ak_Dk)
//              (from Acceleration to Distance).
//              In former implemetation, Acck is 55.
//              With the corrected implementation, Ak_Dk is suggested 1170.
///////////////////////////////////////////////////////////////////
short CalcObwMinTime(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Ak_Dk, MOT_ALGO_DBL Dmax,
                     MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3)
{
    MOT_ALGO_DBL Acck;
    MOT_ALGO_DBL Dk;
    MOT_ALGO_DBL DaccMax;
    MOT_ALGO_DBL k1;
    MOT_ALGO_DBL k2;
    MOT_ALGO_DBL Acc;
    MOT_ALGO_DBL Dnorm;
    MOT_ALGO_DBL Tm;
    MOT_ALGO_DBL Tcv;
//    MOT_ALGO_DBL Temp;

    if ((Amax < TOLERANCE) || (Vmax < TOLERANCE) )
    {
    #ifdef __TEST__
        printf("T1 = T2 = T3 = 0\n");
    #endif
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;
       return 0;
    }

    DaccMax = PI * pow(Vmax, SQUARE) / (Amax * 2.00);

    Dk = Ak_Dk;         // Ak_Dk pass in Dk of default 1170;
    Acck = 3 * Dk * Amax / (Dmax + 2.00 * Dk);

    k1 = sqrt(2.00 * PI * pow(Dk, ONE_THIRD) / Acck);
    k2 = Acck / (3.00 * Dk);

    #ifdef __DEBUG__
    printf("DaccMax %f Acck %f \nk1 %f k2 %f\n", DaccMax, Acck, k1, k2);
    #endif

    if (Dist > DaccMax)
    {
       Tcv = (Dist - DaccMax) / Vmax;
       Dnorm = DaccMax;
    }
    else
    {
       Tcv = 0;
       Dnorm = Dist;
    }

    if ((Dnorm - Dk) < TOLERANCE)
    {
       Acc = 2.00 * PI * pow(Dnorm, ONE_THIRD) / (k1 * k1);
    }
    else if ((Dnorm - Dmax) < TOLERANCE)
    {
       Acc = k2 * (Dnorm - Dk) + Acck;
    }
    else
    {
       Acc = Amax;
    }
    #ifdef __TEST__
    printf("Acc = %f\n", Acc);
    #endif

    if (Acc > 0.00)
    {
       Tm = sqrt(2.00 * PI * Dnorm / Acc);
    }
    else
    {
       // KKTan Added 22Nov2002
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;

       return 1;
    }

    *pT1 = *pT3 = Tm / 2.00;
    *pT2 = Tcv;

    return 0;
}

////////////////////////////////////////////////////////////////
// 5Mar2003
// KKTan Added to return Actual Acceleration in the MinTime Move.
////////////////////////////////////////////////////////////////
short CalcObwMinTime_2(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Ak_Dk, MOT_ALGO_DBL Dmax,
                       MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                       MOT_ALGO_DBL *fAcc)
{
    MOT_ALGO_DBL Acck;
    MOT_ALGO_DBL Dk;
    MOT_ALGO_DBL DaccMax;
    MOT_ALGO_DBL k1;
    MOT_ALGO_DBL k2;
    MOT_ALGO_DBL Acc;
    MOT_ALGO_DBL Dnorm;
    MOT_ALGO_DBL Tm;
    MOT_ALGO_DBL Tcv;
//    MOT_ALGO_DBL Temp;

    if ((Amax < TOLERANCE) || (Vmax < TOLERANCE) )
    {
    #ifdef __TEST__
        printf("T1 = T2 = T3 = 0\n");
    #endif
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;
       return 0;
    }

    DaccMax = PI * pow(Vmax, SQUARE) / (Amax * 2.00);

    Dk = Ak_Dk;         // Ak_Dk pass in Dk of default 1170;
    Acck = 3 * Dk * Amax / (Dmax + 2.00 * Dk);

    k1 = sqrt(2.00 * PI * pow(Dk, ONE_THIRD) / Acck);
    k2 = Acck / (3.00 * Dk);

    #ifdef __DEBUG__
    printf("DaccMax %f Acck %f \nk1 %f k2 %f\n", DaccMax, Acck, k1, k2);
    #endif

    if (Dist > DaccMax)
    {
       Tcv = (Dist - DaccMax) / Vmax;
       Dnorm = DaccMax;
    }
    else
    {
       Tcv = 0;
       Dnorm = Dist;
    }

    if ((Dnorm - Dk) < TOLERANCE)
    {
       Acc = 2.00 * PI * pow(Dnorm, ONE_THIRD) / (k1 * k1);
    }
    else if ((Dnorm - Dmax) < TOLERANCE)
    {
       Acc = k2 * (Dnorm - Dk) + Acck;
    }
    else
    {
       Acc = Amax;
    }
    #ifdef __TEST__
    printf("Acc = %f\n", Acc);
    #endif

    if (Acc > 0.00)
    {
       Tm = sqrt(2.00 * PI * Dnorm / Acc);
    }
    else
    {
       // KKTan Added 22Nov2002
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;

       return 1;
    }

    *pT1 = *pT3 = Tm / 2.00;
    *pT2 = Tcv;

    // KKTan Added to return the achievable Acceleration
    *fAcc = Acc;

    return 0;
}

////////////////////////////////////////////////////////////////
// 30Mar2004:
// KKTan added to provide a mintime calculation that is exactly same
// as that of Eagle platform.
// This calculation does not match with the original m-file as the
// "floor()" is not required. However, the Eagle platform
// motion calculation made this mistake. So, the ECORE(destiny) platform
// provide this function to promote portability from Eagle platform to
// ECORE platform, specially for the time move commands.
// The ECORE library does NOT use this function for its mintime move
// calculation.
////////////////////////////////////////////////////////////////
short CalcObwMinTime_3(MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL Ak_Dk, MOT_ALGO_DBL Dmax,
                       MOT_ALGO_DBL Dist, MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3)
{
    MOT_ALGO_DBL Acck;
    MOT_ALGO_DBL Dk;
    MOT_ALGO_DBL DaccMax;
    MOT_ALGO_DBL k1;
    MOT_ALGO_DBL k2;
    MOT_ALGO_DBL Acc;
    MOT_ALGO_DBL Dnorm;
    MOT_ALGO_DBL Tm;
    MOT_ALGO_DBL Tcv;
//    MOT_ALGO_DBL Temp;

    if ((Amax < TOLERANCE) || (Vmax < TOLERANCE) )
    {
    #ifdef __TEST__
        printf("T1 = T2 = T3 = 0\n");
    #endif
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;
       return 0;
    }

    DaccMax = PI * pow(Vmax, SQUARE) / (Amax * 2.00);

    Dk = Ak_Dk;         // Ak_Dk pass in Dk of default 1170;
    Acck = floor(3 * Dk * Amax / (Dmax + 2.00 * Dk)); // this discrepancy is intended.

    k1 = sqrt(2.00 * PI * pow(Dk, ONE_THIRD) / Acck);
    k2 = Acck / (3.00 * Dk);

    #ifdef __DEBUG__
    printf("DaccMax %f Acck %f \nk1 %f k2 %f\n", DaccMax, Acck, k1, k2);
    #endif

    if (Dist > DaccMax)
    {
       Tcv = (Dist - DaccMax) / Vmax;
       Dnorm = DaccMax;
    }
    else
    {
       Tcv = 0;
       Dnorm = Dist;
    }

    if ((Dnorm - Dk) < TOLERANCE)
    {
       Acc = 2.00 * PI * pow(Dnorm, ONE_THIRD) / (k1 * k1);
    }
    else if ((Dnorm - Dmax) < TOLERANCE)
    {
       Acc = k2 * (Dnorm - Dk) + Acck;
    }
    else
    {
       Acc = Amax;
    }
    #ifdef __TEST__
    printf("Acc = %f\n", Acc);
    #endif

    if (Acc > 0.00)
    {
       Tm = sqrt(2.00 * PI * Dnorm / Acc);
    }
    else
    {
       // KKTan Added 22Nov2002
       *pT1 = 0.00;
       *pT2 = 0.00;
       *pT3 = 0.00;

       return 1;
    }

    *pT1 = *pT3 = Tm / 2.00;
    *pT2 = Tcv;

    return 0;
}


#ifdef __EAGLE_FOR_REFERENCE_ONLY__
//////////////////////////////////////////////////////////////////////
// KKTan 7 Oct 2002
// Due to the legacy, definition of T1 & T3 in Eagle
// is different from Twin Eagle.
// The relation between them is as below:
// T1_twineagle = 2 * T1_eagle
// T3_twineagle = 2 * T3_eagle
//////////////////////////////////////////////////////////////////////
//#define DEBUG
//#define DEBUG1

#define DEF_AKDK    1170.0f
#define DEF_AK_DK   55.0f
#define DEF_DMAX    4000.0f
//#define ONE_HALF    0.500000f
//#define ONE_THIRD   1.0f/3.0f
//#define SQUARE      2.000000f
//#define CUBE        3.000000f

#ifdef DEBUG1
    static short sDebugFlagX = 0;
    static short sDebugFlagY = 0;
#endif
static MOT_ALGO_DBL Amax;
static MOT_ALGO_DBL Dmax = DEF_DMAX;   // Distance at which maximum acceleration
                                // can be reached.
static MOT_ALGO_DBL Acck = DEF_AK_DK;
static MOT_ALGO_DBL AkDk = DEF_AKDK;   // 19122001

void mtr_precalculate_motiontime_constant(MOT_ALGO_DBL fAmax, MOT_ALGO_DBL fVmax,
                                          MOT_ALGO_DBL *fDk, MOT_ALGO_DBL *fDacc_max,
                                          MOT_ALGO_DBL *fK1, MOT_ALGO_DBL *fK2)
{
#define PI 3.141592654
    Amax = fAmax;
    *fDk = AkDk;
    Acck = floor(3.00 * AkDk * Amax / (Dmax + 2.00 * AkDk));
    *fDacc_max = PI * pow(fVmax, SQUARE) / (fAmax * 2.00);
    *fK1 = sqrt(2.00 * PI * pow(*fDk, ONE_THIRD) / Acck);
    *fK2 = Acck / (3.00 * *fDk);

    printf("Acck = %f\n", Acck);
#ifdef DEBUG
    printf("Dacc Max       = %f\n", *fDacc_max);
    printf("Dk             = %f\n", *fDk);
    printf("k1             = %f\n", *fK1);
    printf("k2             = %f\n", *fK2);
    printf("2 * pi / k1 ^2 = %f\n", 2.00 * PI / (*fK1 * *fK1));
#endif
#undef PI
}

void mtr_calculate_motiontime(MOT_ALGO_DBL fDist, MOT_ALGO_DBL fVmax,
                              MOT_ALGO_DBL fDacc_max, MOT_ALGO_DBL fDk,
                              MOT_ALGO_DBL fK1, MOT_ALGO_DBL fK2,
                              MOT_ALGO_DBL *fT1, MOT_ALGO_DBL *fT2, MOT_ALGO_DBL *fT3)
{
#define PI 3.141592654
    MOT_ALGO_DBL Acc;
    MOT_ALGO_DBL Dnorm;
    MOT_ALGO_DBL Tm, Tcv;

    if (fDist > fDacc_max)
    {
       Tcv = (fDist - fDacc_max) / fVmax;
       Dnorm = fDacc_max;
    }
    else
    {
       Tcv = 0;
       Dnorm = fDist;
    }

    if (Dnorm <= fDk)
    {
       Acc = 2.00 * PI * pow(Dnorm, ONE_THIRD) / (fK1 * fK1);
    }
    else if (Dnorm <= Dmax)
    {
       Acc = fK2 * (Dnorm - fDk) + Acck;
    }
    else
    {
       Acc = Amax;
    }
#ifdef DEBUG
    printf("Acc = %f\n", Acc);
#endif
    if (Acc > 0.00)
    {
       Tm = sqrt(2.00 * PI * Dnorm / Acc);
    }
    else
    {
       Tm = 0.00;
    }
#ifdef DEBUG
    printf("Tm = %f\n", Tm);
#endif

    *fT1 = *fT3 = Tm / 4.0f;
    *fT2 = Tcv;
#undef PI
}
#endif


#ifdef __TEST__

MOT_ALGO_DBL ConvertTimeToDiscrete(MOT_ALGO_DBL Fs, MOT_ALGO_DBL Tms)
{
    return (ceil(Fs * Tms));
}

// Unit of return value is um
MOT_ALGO_DBL ConvertDToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Dist)
{
    return (Dist * EncResolution);
}

// Unit of return value is mm/s
MOT_ALGO_DBL ConvertVToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Vmax)
{
    return (Vmax * EncResolution * Fs);
}

// Unit of return value is m/s^2
MOT_ALGO_DBL ConvertAToSI(MOT_ALGO_DBL EncResolution, MOT_ALGO_DBL Fs, MOT_ALGO_DBL Amax)
{
    return (Amax * EncResolution * Fs * Fs);
}

short CalcTObw(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
               MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3)
{
#define PI 3.141592654

   if (T1 < TOLERANCE)
   {
      *c1 = 0.00;
      *c2 = 0.00;
      *c3 = 0.00;
      return 0;
   }
   *c1 = Dist / ( 2 * (T1 + T2));
   *c2 = T1 / PI;
   *c3 = PI / T1;

#undef PI
    return 0;
}

void main(void)
{
    MOT_ALGO_DBL   fAmax, fVmax, fDmax, fAcck, fDist;
    MOT_ALGO_DBL   T1, T2, T3;
    MOT_ALGO_DBL fFbResolution;
    MOT_ALGO_DBL fFs;
    MOT_ALGO_DBL fDistanceSI, fMaxVelSI, fMaxAccSI;
    MOT_ALGO_DBL fConvT1, fConvT2, fConvT3;
    MOT_ALGO_DBL c1, c2, c3;
    MOT_ALGO_DBL fDk, fDacc_max;
    MOT_ALGO_DBL fK1, fK2;
    unsigned long  ulT1, ulT2;

    printf("\n[Calc Min Time]\n");
    printf("\nAmax[25] =");
    scanf("%f", &fAmax);
    printf("\nVmax[1000] =");
    scanf("%f", &fVmax);
    printf("\nDmax[4000] =");
    scanf("%f", &fDmax);
    printf("\nAcck[1170] =");
    scanf("%f", &fAcck);
    printf("\nDistance =");
    scanf("%f", &fDist);

    fFbResolution = 0.2;
    fFs = 4.0;

    /* Step (1) Forward conversion */
    fDistanceSI = ConvertDToSI(fFbResolution, fDist);
    fMaxVelSI = ConvertVToSI(fFbResolution, fFs, fVmax);
    fMaxAccSI = ConvertAToSI(fFbResolution, fFs, fAmax);

    printf("Distance %f MaxVel %f MaxAcc %f\n", fDistanceSI, fMaxVelSI, fMaxAccSI);

    /* Step (2) MinTime calculation */
    CalcObwMinTime(fMaxAccSI, fMaxVelSI, fAcck, fDmax, fDistanceSI,
                   &T1, &T2, &T3);
    printf("Step 2:");
    printf("\nT1 %f   T2 %f   T3 %f\n", T1, T2, T3);

    /* Step (3) Backward conversion */
    fConvT1 = ConvertTimeToDiscrete(fFs, T1);
    fConvT2 = ConvertTimeToDiscrete(fFs, T2);
    fConvT3 = ConvertTimeToDiscrete(fFs, T3);
    printf("Step 3:");
    printf("\nT1 %f   T2 %f   T3 %f\n", fConvT1, fConvT2, fConvT3);

    printf("Step 4: Profile Generation\n");
    CalcTObw(fDist, fConvT1, fConvT2, fConvT3, &c1, &c2, &c3);
    ulT1 = (unsigned long)fConvT1;
    ulT2 = (unsigned long)fConvT2;
    printf("\nT1 %ld   T2 %ld   c1 %f c2 %f c3 %f\n",
            ulT1, ulT2, c1, c2, c3);

#ifdef __NANTI__
    printf("CKTeo's Lib Calculation:\n");

    /* Step (1) Forward conversion */
    fDistanceSI = ConvertDToSI(fFbResolution, fDist);
    fMaxVelSI = ConvertVToSI(fFbResolution, 4, fVmax);
    fMaxAccSI = ConvertAToSI(fFbResolution, 4, fAmax);

    printf("Distance %f MaxVel %f MaxAcc %f\n", fDistanceSI, fMaxVelSI, fMaxAccSI);


    // Step (2) Mintime Calculation
    mtr_precalculate_motiontime_constant(fMaxAccSI, fMaxVelSI,
                                         &fDk, &fDacc_max,
                                         &fK1, &fK2);
    mtr_calculate_motiontime(fDistanceSI, fMaxVelSI,
                             fDacc_max, fDk,
                             fK1, fK2,
                             &T1, &T2, &T3);
    printf("Step 2:");
    printf("\nT1 %f   T2 %f   T3 %f\n", T1, T2, T3);

    /* Step (3) Backward conversion */
    fConvT1 = ConvertTimeToDiscrete(fFs, T1);
    fConvT2 = ConvertTimeToDiscrete(fFs, T2);
    fConvT3 = ConvertTimeToDiscrete(fFs, T3);
    printf("Step 3:");
    printf("\nT1 %f   T2 %f   T3 %f\n", fConvT1, fConvT2, fConvT3);
#endif
}
#endif


