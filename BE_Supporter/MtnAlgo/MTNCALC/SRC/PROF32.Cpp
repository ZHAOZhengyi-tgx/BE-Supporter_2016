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



 * Module       : prof32
 *-------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include "mtncalc.h"

//#define DEBUG

short Calc3rd2ndNsyProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T, MOT_ALGO_DBL k,
                           MOT_ALGO_DBL *pT1st, MOT_ALGO_DBL *pT2nd,
                           MOT_ALGO_DBL *pJerk1st, MOT_ALGO_DBL *pDec,
                           MOT_ALGO_DBL *pDeltVc, MOT_ALGO_DBL *pDeltDf)
{
   MOT_ALGO_DBL T1st, T2nd;
   MOT_ALGO_DBL D1st, D2nd;
   MOT_ALGO_DBL Jerk1st;
   MOT_ALGO_DBL V1st, V2nd;
   MOT_ALGO_DBL Dec;
   MOT_ALGO_DBL DeltVc, DeltDf;

   T2nd = floor((2.00 * (T + 2.00) * (1 - k) + 3.00 * k) / (2.00 + k)) + 1.00;
   D1st = Dist * k;
   T1st = T - T2nd;

   Jerk1st = 6.00 * D1st / (T1st * (T1st + 1.00) * (T1st + 2.00));
   D1st    = T1st * (T1st + 1) * (T1st + 2) * Jerk1st / 6.00;
   V1st    = T1st * (T1st + 1) * Jerk1st / 2.00;

   Dec    = 2.00 * (Dist - D1st) / (T2nd * (T2nd - 1));
   V2nd   = Dec * T2nd;
   DeltVc = V2nd - V1st;
   V2nd   = V1st + DeltVc;
   D2nd   = V2nd * T2nd - 0.50 * T2nd * (T2nd + 1) * Dec;
   DeltDf = Dist - D1st - D2nd;

   *pT1st = T1st;
   *pT2nd = T2nd;
   *pJerk1st = Jerk1st;
   *pDec = Dec;
   *pDeltVc = DeltVc;
   *pDeltDf = DeltDf;
#ifdef DEBUG
   printf("[%f %f %f]\n", Jerk1st, D1st, V1st);
   printf("[%f %f]\n", T1st, T2nd);
   printf("Dec = %f\n", Dec);
   printf("V2nd = %f\n", V2nd);
   printf("D2nd = %f\n", D2nd);
   printf("delt Vc = %f\n", DeltVc);
   printf("delt Df = %f\n", DeltDf);
#endif

   return 0;
}

short Gen3rd2ndNsyProfile(MOT_ALGO_DBL T1st, MOT_ALGO_DBL T2nd,
                          MOT_ALGO_DBL Jerk1st, MOT_ALGO_DBL Dec,
                          MOT_ALGO_DBL DeltVc, MOT_ALGO_DBL DeltDf)
{
   long  lCntr;
   long  lT1st, lT2nd;
   long  lTotalTime;
   MOT_ALGO_DBL Jerk;
   MOT_ALGO_DBL Acc0, Acc;
   MOT_ALGO_DBL Vel0, Vel;
   MOT_ALGO_DBL Dist0, Dist;

   lTotalTime = (long) (T1st + T2nd);
   lT1st = (long) T1st;
   lT2nd = (long) T2nd;
#ifdef DEBUG
   printf("z = [\n");
#endif
   for (lCntr = 0; lCntr < lTotalTime; lCntr++)
   {
       if (lCntr <= T1st)
       {
          Jerk = Jerk1st;
       }
       else
       {
          Jerk = 0.00;
       }

       if (lCntr == 0)
       {
          // Initial Values
          Acc0 = Jerk;
          Vel0 = Acc0;
          Dist0 = Vel0;
       }
       else
       {
          // Generate Command Acceleration
          if (lCntr <= (lT1st - 1))
          {
             Acc = Acc0 + Jerk;
          }
          else
          {
             Acc = -Dec;
          }

          // Generate Command Velocity
          if (lCntr == lT1st)
          {
             Vel = Vel0 + Acc + DeltVc;
          }
          else
          {
             Vel = Vel0 + Acc;
          }

          // Generate Command Distance
          Dist = Dist0 + Vel;

          if (lCntr == lT1st + 1)
          {
             Dist = Dist + DeltDf;
          }

          Acc0 = Acc;
          Vel0 = Vel;
          Dist0 = Dist;
#ifdef DEBUG
          printf("%f %f %f\n", Acc0, Vel0, Dist0);
#endif
       }
   }
#ifdef DEBUG
   printf("];\n");
#endif

   return 0;
}

/*****Removed [mb2001]
void main(void)
{
    MOT_ALGO_DBL T1, T2;
    MOT_ALGO_DBL Jk1, Dec;
    MOT_ALGO_DBL DeltVc, DeltDf;

    Calc3rd2ndNsyProfile(-1000, 100, 0.8, &T1, &T2, &Jk1, &Dec,
                         &DeltVc, &DeltDf);
    Gen3rd2ndNsyProfile(T1, T2, Jk1, Dec, DeltVc, DeltDf);
    Calc3rd2ndNsyProfile(1000, 100, 0.8, &T1, &T2, &Jk1, &Dec,
                         &DeltVc, &DeltDf);
    Gen3rd2ndNsyProfile(T1, T2, Jk1, Dec, DeltVc, DeltDf);
}
*****/
