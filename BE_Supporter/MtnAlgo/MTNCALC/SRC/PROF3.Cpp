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



 * Module       : prof3.c
 *-------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include "mtncalc.h"

// #define __DEBUG__

// To re-calculate k for non-symmetrical 3rd Order Profile Move
// Input  : Ratio, k (MOT_ALGO_DBL) in the range of -1 < k < 1
// Output : K (MOT_ALGO_DBL)
//
// Remark : a) Due to the constraint of the profile formulation,
//             this routine is needed to achieve mirrored profile for
//             +ve & -ve value counterpart, eg: -0.5 & 0.5.
//          b) Input Range of k:  -1 < k < 1
// 
// KKTan Added on 2ndOct2002,
// This routine is ported from CK Teo, created on Nov. 10, 1999
// Original: mtr_calculate_nonsymm_3odr_tmove()
short Calc_nonsymm_3odr_move(MOT_ALGO_DBL k, MOT_ALGO_DBL *pK)
{
    if (k < 0.00)
    {
       k = fabs(k);
       if (k == 1.00)
       {
          k = 0.99;
       }
       *pK = k / (k - 1.00);
    }
    return MTN_CALC_SUCCESS;
}

short Calc_3ordOrderTimeMove(MOT_ALGO_DBL Dist, MOT_ALGO_DBL k,
                             MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                             double *pJk1, double *pJk3,
                             double *pDeltVc, double *pDeltDf)
{
    double dDist;
    double w;

    // Calculate the 3rdOrder parameters based on T1, T2, T3
    dDist   = (double) Dist;
    w       = (2 - k) * T1 + T2;
    *pJk1   = dDist / (w * T1 * T1);
    *pJk3   = (dDist * (1 - k) * T1) / (w * T3 * T3 * T3);
    *pDeltVc = (dDist * ((1 - k) * T1 - T3)) / (w * T3);
    *pDeltDf = 0.00;

#ifdef __DEBUG__
    printf("T = [%f %f %f]\n", T1, T2, T3);
    printf("Jerk = [%f %f]\n", *pJk1, *pJk3);
    printf("Delta = [%f %f]\n", *pDeltVc, *pDeltDf);
#endif
    return MTN_CALC_SUCCESS;
}

short Calc3rdOrderProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL k,
                          MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                          double *pJk1, double *pJk3,
                          double *pDeltVc, double *pDeltDf)
{
    MOT_ALGO_DBL  SignDist, Dlimit, FixDlimit, D1act, D2act, Dact;
    MOT_ALGO_DBL  T1, T2, T3, AccT, Vc, Dcv;
    double dDist;
    double w;

    // Part I: Calculate T1, T2 & T3.
    *pT1 = *pT2 = *pT3 = 0.00;
    if (Dist < 0.0f)
    {
       SignDist = -1.0f;
    }
    else
    {
       SignDist = 1.0f;
    }

    Dlimit = Vmax * (Vmax/Amax);
#ifdef DEBUG

#endif
    if (fabs(Dist) <= (2 * Dlimit))
    {
       D1act = Dist/(2 - k);
       if (D1act > 0.0f)
       {
          D1act = floor(D1act);
       }
       else
       {
          D1act = ceil(D1act);
       }
       D2act = Dist - D1act;
    }
    else
    {
       if (Dlimit > 0.0f)
       {
          FixDlimit = floor(Dlimit);
       }
       else
       {
          FixDlimit = ceil(Dlimit);
       }
       Dact = SignDist * FixDlimit;
       if (k < 0.0f)
       {
          D1act = Dact;
          D2act = D1act * (1 - k);
          if (D2act > 0.0f)
          {
             D2act = floor(D2act);
          }
          else
          {
             D2act = ceil(D2act);
          }
       }
       else
       {
          D2act = Dact;
          D1act = D2act / (1 - k);
          if (D1act > 0.0f)
          {
             D1act = floor(D1act);
          }
          else
          {
             D1act = ceil(D1act);
          }
       }
    }

    if (k < 0.0f)
    {
       T1 = (sqrt( 1 + 4 * fabs( D1act )/Amax ) - 1.0f)/2.0f;
       T1 = floor(T1) + 1;
       T3 = (1 - k) * T1;
       if ((ceil(T3) - T3) > 0.5f)
       {
          T3 = ceil(T3) - 1.0f;
       }
       else
       {
          T3 = ceil(T3);
       }
       AccT = Amax * T1;
    }
    else
    {
       T3 = (sqrt( 1 + 4 * fabs( D2act )/Amax ) - 1.0f)/2.0f;
       T3 = floor(T3) + 1;
       T1 = T3 / (1 - k);
       if ((ceil(T1) - T1) > 0.5f)
       {
          T1 = ceil(T1) - 1.0f;
       }
       else
       {
          T1 = ceil(T1);
       }
       AccT = Amax * T3;
    }
    if ((ceil(AccT) - AccT) > 0.5f)
    {
       AccT = ceil(AccT) - 1.0f;
    }
    else
    {
       AccT = ceil(AccT);
    }
    Vc  = SignDist * AccT;
    Dcv = Dist - D1act - D2act;
    T2  = floor(Dcv / Vc);
    if (T2 < 0.0f)
    {
       T2 = 0.0f;
    }
    *pT1 = T1;
    *pT2 = T2;
    *pT3 = T3;

    // Part II: Calculate the other parameters based on T1, T2, T3
    dDist = (double) Dist;
    w = (2 - k) * T1 + T2;
    *pJk1 = dDist / (w * T1 * T1);
    *pJk3 = (dDist * (1 - k) * T1) / (w * T3 * T3 * T3);
    *pDeltVc = (dDist * ((1 - k) * T1 - T3)) / (w * T3);
    *pDeltDf = 0.00;
#ifdef __DEBUG__
    printf("Dlimit = %f\n", Dlimit);
    printf("Dact = [%f %f]\n", D1act, D2act);
    printf("T = [%f %f %f]\n", T1, T2, T3);
    printf("Jerk = [%f %f]\n", *pJk1, *pJk3);
    printf("Delta = [%f %f]\n", *pDeltVc, *pDeltDf);
#endif
    return(0);
}

short Gen3rdOrderProfile(MOT_ALGO_DBL Distance, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                         double Jk1, double Jk3,
                         double DeltVc, double DeltDf)
{
    MOT_ALGO_DBL  Dist;
    MOT_ALGO_DBL  Acc;
    MOT_ALGO_DBL  Vel;
    double Jerk;
    long   lT1, lT2, lT3;
    long   lTotalTime;
    long   lCntr;

    lT1 = (long) T1;
    lT2 = (long) T2;
    lT3 = (long) T3;
    lTotalTime = 2 * lT1 + lT2 + 2 * lT3;
#ifdef DEBUG
    printf("z=[\n");
#endif
    for (lCntr = 1; lCntr <= lTotalTime; lCntr++)
    {
        if (lCntr <= lT1)
        {
           Jerk = Jk1;
        }
        else if ((lCntr > lT1) && (lCntr <= (lT1 * 2)))
        {
           Jerk = -Jk1;
        }
        else if ((lCntr > (lT1 * 2)) && lCntr <= (lT1 * 2 + lT2))
        {
           Jerk = 0;
        }
        else if ((lCntr > (lT1 * 2 + lT2)) && lCntr <= (lT1 * 2 + lT2 + lT3))
        {
           Jerk = -Jk3;
        }
        else if ((lCntr > (lT1 * 2 + lT2 + lT3)) && lCntr < (lT1 * 2 + lT2 + lT3 * 2))
        {
           Jerk = Jk3;
        }
        else
        {
           Jerk = 0;
        }
        if (lCntr == 1)
        {
           Acc = Jerk;
           Vel = Acc;
           Dist = Vel;
        }
        else if (lCntr < lTotalTime)
        {
           Acc = Acc + Jerk;
           Vel = Vel + Acc;
           if (lCntr == (2 * lT1 + lT2))
           {
              Vel += DeltVc;
           }
           Dist = Dist + Vel;
           if (lCntr == (2 * lT1 + 1))
           {
              Dist += DeltDf;
           }
        }
        else
        {
           Acc = 0;
           Vel = 0;
           Dist = Distance;
        }
#ifdef DEBUG
        printf("%f %f %f\n", Acc, Vel, Dist);
#endif
    }
    printf("];\n");
    return 0;
}

#ifdef DEBUG
void main(void)
{
    MOT_ALGO_DBL T1, T2, T3;
    double Jk1, Jk3;
    double DeltVc, DeltDf;

    Calc3rdOrderProfile(1000, 25, 500, -10, &T1, &T2, &T3, &Jk1, &Jk3,
                        &DeltVc, &DeltDf);
    Gen3rdOrderProfile(1000, T1, T2, T3, Jk1, Jk3, DeltVc, DeltDf);

    Calc3rdOrderProfile(-1000, 25, 500, -10, &T1, &T2, &T3, &Jk1, &Jk3,
                        &DeltVc, &DeltDf);
    Gen3rdOrderProfile(-1000, T1, T2, T3, Jk1, Jk3, DeltVc, DeltDf);
}
#endif

#ifdef __DEBUG__
void main(void)
{
    double Jk1, Jk3;
    double DeltVc, DeltDf;
    MOT_ALGO_DBL  Dist, k, T1, T2, T3;
    MOT_ALGO_DBL  Amax, Vmax;

    printf("\nDistance = ");
    scanf("%f", &Dist);
    printf("\nk        = ");
    scanf("%f",&k );
    printf("\nAmax     = ");
    scanf("%f", &Amax);
    printf("\nVmax     = ");
    scanf("%f", &Vmax);

    Calc3rdOrderProfile(Dist, Amax, Vmax, k,
                        &T1, &T2, &T3,
                        &Jk1, &Jk3,
                        &DeltVc, &DeltDf);


    Calc_3ordOrderTimeMove(Dist, k, T1, T2, T3,
                           &Jk1, &Jk3,
                           &DeltVc, &DeltDf);

}

#endif
