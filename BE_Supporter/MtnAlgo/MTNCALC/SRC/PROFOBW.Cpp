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


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mtncalc.h"

#define TOLERANCE   1.0e-30

// #define __TEST__

short CalcCycloidalLinearTrajectory(
      CALC_CYC_LINEAR_TRAJ_INPUT *stpInput,
      CALC_CYC_LINEAR_TRAJ_OUTPUT *stpOutput)
{
	short sTz;
	
	if ((stpInput->sTxy < 0) ||
	    (stpInput->sT1xy < 0) ||
	    (stpInput->sT1xyOverlapped < 0) ||
	    (stpInput->sT3xyOverlapped < 0))
	{
		return 1;
	}
	
	sTz = stpInput->sTxy - stpInput->sT1xyOverlapped - stpInput->sT3xyOverlapped;
	if (sTz < 0)
	{
		return 1;
    }
	if ((stpOutput->sT1z = stpInput->sT1xy - stpInput->sT1xyOverlapped) < 0)
	{
		return 1;
	}
	if ((stpOutput->sT3z = sTz - stpOutput->sT1z) < 0)
	{
		return 1;
	}	
	return 0;
}

short CalcCycloidalAmax(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2,
                                 MOT_ALGO_DBL *pCalcAmax)
{
#define PI 3.141592654
    MOT_ALGO_DBL C1;
    MOT_ALGO_DBL C3;

    if ((short) T1 <= 0)
    {
       return 1;
    }
    if ((short) T2 < 0)
    {
       return 1;
    }
    C1 = Dist / (2.00 * (T1 + T2));
    C3 = PI / T1;
    *pCalcAmax = C1 * C3;

#undef PI
    return 0;
}


short CalcProfCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax, MOT_ALGO_DBL k,
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

short CalcTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
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

short CheckGTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
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

short CalcGTCycloidal(MOT_ALGO_DBL Dist, MOT_ALGO_DBL EndVel, MOT_ALGO_DBL T1, MOT_ALGO_DBL T2, MOT_ALGO_DBL T3,
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


short  Calc_generate_OBW_profile(MOT_ALGO_DBL fDist,
                                 MOT_ALGO_DBL fT1, MOT_ALGO_DBL fT2, MOT_ALGO_DBL fT3,
                                 MOT_ALGO_DBL *pTraj)
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

    // Calculate Co-efficients for OBW generation:
    CalcTCycloidal(fDist, fT1, fT2, fT3, &C1, &C2, &C3);

    for (i = 1; i <= (2 * T1 + T2); i++)
    {
        if (i <= T1)
        {
           *(pTraj + i - 1) = C1 * (i - C2 * sin(C3 * i));
        }
        else if ((i > T1) && (i <= (T1 + T2)))
        {
           *(pTraj + i - 1) = 2 * C1 * i - C1 * T1;
        }
        else if ( (i > (T1+T2)) && (i < (2*T1 + T2)) )
        {
           *(pTraj + i - 1) = C1 * (i + T2 + C2 * sin(C3 * (i- T1 - T2)));
        }
        else // Last sample
        {
            *(pTraj + i - 1) = fDist;
        }
    }
#undef PI
    return MTN_CALC_SUCCESS;
}

short  Calc_OBWMove_time_distance_relation(MOT_ALGO_DBL fTotalDist,
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
    CalcTCycloidal(fTotalDist, fT1, fT2, fT1,
             &C1, &C2, &C3);

    for (i = 1; i <= (2 * T1 + T2); i++)
    {
        if (i <= T1)
        {
           D = C1 * (i - C2 * sin(C3 * i));
        }
        else if ((i > T1) && (i <= (T1 + T2)))
        {
           D = 2 * C1 * i - C1 * T1;
        }
        else if ( (i > (T1+T2)) && (i < (2*T1 + T2)) )
        {
           D = C1 * (i + T2 + C2 * sin(C3 * (i- T1 - T2)));
        }
        else // last sample
        {
            D = fTotalDist;
        }

        if (D >= fDesiredDist)
        {
           #ifdef __TEST__
           printf("%d: %f >= %f\n", (i-1), D, fDesiredDist);
           #endif
           *pfRequiredTime = i - 1;
           return MTN_CALC_SUCCESS;
        }
    }
    return MTN_CALC_ERROR;
#undef PI
}


#ifdef __TEST__
void main(void)
{

    MOT_ALGO_DBL   Traj[100];
    int     i;
    MOT_ALGO_DBL   fRequiredTime;

    Calc_generate_OBW_profile(500,
                              5,
                              10,
                              5,
                              Traj);

    for (i=0; i<20; i++)
    {
        printf("%d Dist = %f\n", i, Traj[i]);
    }
    Calc_OBWMove_time_distance_relation(500,
                                        45,
                                        5,
                                        10,
                                        5,
                                        &fRequiredTime);
    printf("Time = %f\n", fRequiredTime);
}
#endif
