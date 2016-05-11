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


//***************************************************************************
// Revision History:
// YYYYMMDD     Programmer      Changes
//***************************************************************************
// 20081009     Zhengyi         rename Obw -> Cycloidal
//***************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mtncalc.h"

#define TOLERANCE   1.0e-30

// #define __POLY_SIN_OBW_TEST__
#define __PolyMutualSinCos__

extern MOT_ALGO_DBL fPoly12Cos(MOT_ALGO_DBL x);
extern MOT_ALGO_DBL fPoly11Sin(MOT_ALGO_DBL x);
extern MOT_ALGO_DBL fPolySin5Cos(MOT_ALGO_DBL x);
extern MOT_ALGO_DBL fPolyCos6Sin(MOT_ALGO_DBL x);

short CalcProfPolyCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL k,
                  MOT_ALGO_DBL *T1, MOT_ALGO_DBL *T2, MOT_ALGO_DBL *T3,
                  MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3)
{
#define PI 3.141592654
   MOT_ALGO_DBL T1d;
   MOT_ALGO_DBL Dmv;
   MOT_ALGO_DBL Dcv;

   if ((Amax < TOLERANCE) || (Vmax < TOLERANCE))
   {
      *T1 = 0.00;
      *T2 = 0.00;
      *T3 = 0.00;
      *c1 = 0.00;
      *c2 = 0.00;
      *c3 = 0.00;
      return 0;
   }

   Dmv = 0.50 * PI * Vmax * Vmax / Amax;
   Dcv = fabs(Dist) - Dmv;
   if (Dcv > 0.00)
   {
      *T2 = floor(Dcv / Vmax) + 1.00;
      T1d = Dmv / Vmax;
   }
   else
   {
      *T2 = 0.00;
      T1d = sqrt(fabs(Dist) * 2.00 * PI / Amax) / 2.00;
   }
   *T1 = floor(T1d) + 1.00;

   *T3 = *T1;

   *c1 = Dist / ( 2 * (*T1 + *T2));
   *c2 = *T1 / PI;
   *c3 = PI / *T1;

#undef PI
   return 0;
}

short CalcTPolyCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
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

short CheckGTPolyCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                 MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3)
{
   MOT_ALGO_DBL D1;
   MOT_ALGO_DBL MaxVel;
   MOT_ALGO_DBL RampUpTime;
   MOT_ALGO_DBL RampDownTime;

   *pT1 = T1;
   *pT2 = T2;
   *pT3 = T3;

   if ((T1 < TOLERANCE) || (T3 < TOLERANCE))
   {
      *pT1 = 0.00;
      *pT2 = 0.00;
      *pT3 = 0.00;
      return 3;
   }
   D1 = EndVel * T3;
   MaxVel = (2.00 * Dist - D1) / (T1 + T3);
   if ((MaxVel * Dist) < TOLERANCE)
   {
      *pT1 = 0.00;
      *pT2 = 0.00;
      *pT3 = 0.00;
      return 3;
   }
   if (fabs(MaxVel) < fabs(EndVel))
   {
      RampUpTime = floor(2.00 * ((T1 + T3) - Dist / EndVel));
      RampDownTime = (T1 + T3) - RampUpTime;
      if (RampDownTime > 0.00)
      {
         *pT1 = RampUpTime;
         *pT2 = 0.00;
         *pT3 = RampDownTime;
         return 1;
      }
      else
      {
         return 2;
      }
   }
   return 0;
}

short CalcGTPolyCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
                MOT_ALGO_DBL *c1, MOT_ALGO_DBL *c2, MOT_ALGO_DBL *c3, MOT_ALGO_DBL *c4, MOT_ALGO_DBL *c5)
{
#define PI 3.141592654

   MOT_ALGO_DBL D1;
   MOT_ALGO_DBL MaxVel;

   if ((T1 < TOLERANCE) || (T3 < TOLERANCE))
   {
      *c1 = 0.00;
      *c2 = 0.00;
      *c3 = 0.00;
      *c4 = 0.00;
      *c5 = 0.00;
      return 0;
   }

   D1 = EndVel * T3;
   MaxVel = (2.00 * Dist - D1) / (T1 + T3);
   *c1 = MaxVel / 2.00;
   *c2 = T1 / PI;
   *c3 = PI / T1;
   *c4 = T3 / PI;
   *c5 = PI / T3;

#undef PI
    return 0;
}


short  Calc_generate_PolyCycloidal_profile(MOT_ALGO_DBL fDist,
                                 MOT_ALGO_DBL fT1, MOT_ALGO_DBL fT2, MOT_ALGO_DBL fT3,
                                 MOT_ALGO_DBL *pTraj, MOT_ALGO_DBL *pVel, MOT_ALGO_DBL *pAcc)
{
#define PI 3.141592654
    MOT_ALGO_DBL           C1, C2, C3;
    unsigned long   i;
    unsigned long   T1, T2, T3;

    T1 = (unsigned long)fT1;
    T2 = (unsigned long)fT2;
    T3 = (unsigned long)fT3;

    if (pTraj == NULL)
    {
        return MTN_CALC_ERROR;
    }
    if (fT1 == 0)
    {
        *pTraj = 0;
        return MTN_CALC_SUCCESS;
    }

    // Calculate Co-efficients for PolyCycloidal generation:
    CalcTPolyCycloidal(fDist, fT1, fT2, fT3, &C1, &C2, &C3);

    for (i = 1; i <= (2*T1 + T2 ); i++)
    {
        if (i <= T1)
        {
#ifdef __PolySingleSinCos__
           *(pTraj + i - 1) = C1 * (i - C2 * fPoly11Sin(C3 * i));
           *(pVel + i - 1)  = C1 * (1 - fPoly12Cos(C3 * i));
           *(pAcc + i - 1)  = C3 * C1 * fPoly11Sin(C3 * i);
#endif
#ifdef __PolyMutualSinCos__
           *(pTraj + i - 1) = C1 * (i - C2 * fPolySin5Cos(C3 * i));
           *(pVel + i - 1)  = C1 * (1 - fPolyCos6Sin(C3 * i));
           *(pAcc + i - 1)  = C3 * C1 * fPolySin5Cos(C3 * i);
#endif
        }
        else if ((i > T1) && (i <= (T1 + T2)))
        {
           *(pTraj + i - 1) = 2 * C1 * i - C1 * T1;
           *(pVel + i - 1)  = 2 * C1;
           *(pAcc + i - 1)  = 0;

        }
        else if ( (i > (T1+T2)) && (i < (2*T1 + T2)) )
        {
#ifdef __PolySingleSinCos__
           *(pTraj + i - 1) = C1 * (i + T2 + C2 * fPoly11Sin(C3 * (i- T1 - T2)));
           *(pVel + i - 1)  = C1 * (1 - fPoly12Cos(C3 * (i - T2)));
           *(pAcc + i - 1)  = C3 * C1 * fPoly11Sin(C3 * (i - T2));
#endif

#ifdef __PolyMutualSinCos__
           *(pTraj + i - 1) = C1 * (i + T2 + C2 * fPolySin5Cos(C3 * (i- T1 - T2)));
           *(pVel + i - 1)  = C1 * (1 - fPolyCos6Sin(C3 * (i - T2)));
           *(pAcc + i - 1)  = C3 * C1 * fPolySin5Cos(C3 * (i - T2));
#endif
        }
        else // Last sample
        {
            *(pTraj + i - 1) = fDist;
            *(pVel + i - 1)  = 0;
            *(pAcc + i - 1)  = 0;
        }
    }
#undef PI
    return MTN_CALC_SUCCESS;
}

short  Calc_PolyCycloidalMove_time_distance_relation(MOT_ALGO_DBL fTotalDist,
                                       MOT_ALGO_DBL fDesiredDist,
                                       MOT_ALGO_DBL fT1,
                                       MOT_ALGO_DBL fT2,
                                       MOT_ALGO_DBL fT3,
                                       MOT_ALGO_DBL *pfRequiredTime)
{
#define PI 3.141592654
    unsigned int   i;
    MOT_ALGO_DBL D;
    MOT_ALGO_DBL C1, C2, C3;
    unsigned long T1, T2, T3;

    // Initialize as 0
    *pfRequiredTime = 0;

    T1 = (unsigned long)fT1;
    T2 = (unsigned long)fT2;
    T3 = (unsigned long)fT3;

    // Protection
    fTotalDist = fabs(fTotalDist);
    fDesiredDist = fabs(fDesiredDist);
    if (fDesiredDist > fTotalDist)
    {
       fprintf(stderr, "Desired Dist = %f\n", fDesiredDist);
       fprintf(stderr, "Total   Dist = %f\n", fTotalDist);
       return MTN_CALC_ERROR;
    }
    if (T1 == 0)
    {
       return MTN_CALC_SUCCESS;
    }

    // Calculate coefficients of optimal BW profile
    CalcTPolyCycloidal(fTotalDist, fT1, fT2, fT1,
             &C1, &C2, &C3);

    for (i = 1; i <= (2 * T1 + T2); i++)
    {
        if (i <= T1)
        {
           D = C1 * (i - C2 * fPoly11Sin(C3 * i));
        }
        else if ((i > T1) && (i <= (T1 + T2)))
        {
           D = 2 * C1 * i - C1 * T1;
        }
        else if ( (i > (T1+T2)) && (i < (2*T1 + T2)) )
        {
           D = C1 * (i + T2 + C2 * fPoly11Sin(C3 * (i- T1 - T2)));
        }
        else // last sample
        {
            D = fTotalDist;
        }

        if (D >= fDesiredDist)
        {
           #ifdef __POLY_SIN_Cycloidal_TEST__
           printf("\nNo.%d: %f >= %f\n", (i-1), D, fDesiredDist);
           #endif
           *pfRequiredTime = i - 1;
           return MTN_CALC_SUCCESS;
        }
    }
    return MTN_CALC_ERROR;
#undef PI
}


#ifdef __POLY_SIN_Cycloidal_TEST__
void main(void)
{

    MOT_ALGO_DBL   *Traj, *fVel, *fAcc;
    int     i;
    MOT_ALGO_DBL   fRequiredTime;
    MOT_ALGO_DBL fT1, fT2, fT3, fDist, fInterFactor = 1;
    MOT_ALGO_DBL fC1, fC2, fC3;
    MOT_ALGO_DBL fMaxAcc, fMaxVel;
    MOT_ALGO_DBL fRate;
    short sTotalTime;
    FILE    *fp;

    fprintf(stderr, "Polynormial SinProfile Simulation\n");
    fprintf(stderr, "Distance [Encoder]: ");
    scanf("%f", &fDist);
    fprintf(stderr, "Input: %f\n\n", fDist);
    fprintf(stderr, "Sampling Rate[Hz]: ");
    scanf("%f", &fRate);
    fprintf(stderr, "Input: %f\n\n", fRate);
    fprintf(stderr, "AccMax [Enc/Smpl/Smpl]: ");
    scanf("%f", &fMaxAcc);
    fprintf(stderr, "Input: %f\n\n", fMaxAcc);
    fprintf(stderr, "VelMax [Enc/Smpl]: ");
    scanf("%f", &fMaxVel);
    fprintf(stderr, "Input: %f\n\n", fMaxVel);

    CalcProfPolyCycloidal(fDist, fMaxAcc, fMaxVel, 1,
                  &fT1, &fT2, &fT3,
                  &fC1, &fC2, &fC3);

    fprintf(stderr, "T1: %f\n",fT1);
    fprintf(stderr, "T2: %f\n", fT2);
    fprintf(stderr, "T3: %f\n", fT3);
    fprintf(stderr, "C1: %f\n", fC1);
    fprintf(stderr, "C2: %f\n", fC2);
    fprintf(stderr, "C3: %f\n", fC3);

    Traj = (MOT_ALGO_DBL*) malloc( (short)(fT1+fT2+fT3) * sizeof(MOT_ALGO_DBL));
    fVel = (MOT_ALGO_DBL*) malloc( (short)(fT1+fT2+fT3) * sizeof(MOT_ALGO_DBL));
    fAcc = (MOT_ALGO_DBL*) malloc( (short)(fT1+fT2+fT3) * sizeof(MOT_ALGO_DBL));

    Calc_generate_PolyCycloidal_profile(fDist,
                              fT1,
                              fT2,
                              fT3,
                              Traj,
                              fVel,
                              fAcc);

    sTotalTime = (short)(fT1 + fT2 + fT3);
    *(Traj + sTotalTime) = fDist;
    for (i=0; i<sTotalTime; i++)
    {
        printf("%d Dist = %f\n", i, Traj[i]);
    }
    Calc_PolyCycloidalMove_time_distance_relation(fDist,
                                        45,
                                        fT1,
                                        fT2,
                                        fT3,
                                        &fRequiredTime);
    fp = fopen("Plysinpf.m", "w");
    fprintf(fp, "%%Condition:\n");
    fprintf(fp, "%%Dist = %f\n ", fDist);
    fprintf(fp, "%%MaxAcc: %f\n", fMaxAcc);
    fprintf(fp, "%%MaxVel: %f\n", fMaxVel);
    fprintf(fp, "%%T1=%f\n %%T2=%f\n %%T3=%f\n %%InterFactor=%f\n",
                        fT1, fT2, fT3, fInterFactor);
    fprintf(fp, "%%SampleRate: %f\n", fRate);
    fprintf(fp, "%%C1: %f\n", fC1);
    fprintf(fp, "%%C2: %f\n", fC2);
    fprintf(fp, "%%C3: %f\n", fC3);

    fprintf(fp, "%%No, Pos  Vel     Acc \n");
    fprintf(fp, "profpolysin = [");
    sTotalTime = (short)(fT1 + fT2 + fT3);
    for(i =0; i<=sTotalTime; i++)
    {
        fprintf(fp, "%d     %f  %f  %f\n",
                    i, Traj[i], fVel[i], fAcc[i]);
    }
    fprintf(fp, "];\n");
    flushall();
    fclose(fp);
    fprintf(stderr, "Time = %f\n", fRequiredTime);
    free(Traj);
    free(fVel);
    free(fAcc);

}
#endif
