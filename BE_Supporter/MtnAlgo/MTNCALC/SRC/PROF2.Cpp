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



 * Module       : prof2.c
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "mtncalc.h"

#include <limits.h>

//#define __STUB__
//#define __DEBUG__
/////////////////////////////////////////////////////////////////////////
// Function:    Calc2ndOrderProfile()
// Purpose:     To calculate the parameters for 2nd order profile generation
// Two cases:
// 1. With constant speed move, i.e. Trapezoidal Velocity profile
// 2. Without constant speed move, i.e. Triangular Velocity profile
/////////////////////////////////////////////////////////////////////////
short Calc2ndOrderProfile(MOT_ALGO_DBL Dist, MOT_ALGO_DBL Amax, MOT_ALGO_DBL Vmax,
                          MOT_ALGO_DBL *pT1, MOT_ALGO_DBL *pT2, MOT_ALGO_DBL *pT3,
                          MOT_ALGO_DBL *pActAcc, MOT_ALGO_DBL *pDeltV)

{
    MOT_ALGO_DBL AbsDist, Dconst, Dramp, ActVmax;
    unsigned long ulRampSample;
	double dblRampSample;

    // initialize the output.
    *pT1 = 0.0;
    *pT2 = 0.0;
    *pT3 = 0.0;
    *pDeltV = 0.0;
    *pActAcc = 0.0;

    // check the input
    if(Vmax<=0)
    {
        return MTN_CALC_ERROR;
    }
    if(Amax<=0)
    {
        return MTN_CALC_ERROR;
    }
    if(Dist==0)
    {
        return MTN_CALC_SUCCESS;
    }

    // get the absoulte value of distance
    AbsDist = fabs(Dist);

    // Based on the input parameter, calculate the intermediate parameters.
    if(Amax < Vmax)
    {
        // Vmax is achieved in >= 1 sample
		dblRampSample = ceil(Vmax / Amax);
		if( dblRampSample > UINT_MAX)
		{
			printf("Error, too long motion time, exceeding maximum unsigned integer %d", UINT_MAX);
		}
		else
		{
	        ulRampSample = (unsigned long ) dblRampSample; //ramp up only, get integer
		}
        ActVmax = Amax * dblRampSample;
        Dramp = floor(ActVmax * ulRampSample) + ActVmax; // including ramp up & ramp down
    }
    else
    {
        // Vmax is achieved in 1 sample.
        ulRampSample = 1; //ramp up only, get integer
        ActVmax = Vmax;
        Dramp = Vmax; //including ramp up & ramp down
    }

    Dconst = AbsDist - Dramp;

    if (AbsDist > Dramp)
    {
        if (Dconst >= ActVmax)
        {
            // Case 1: Trapezoidal
            *pT1 = ulRampSample;
            *pT3 = *pT1;
            *pT2 = ceil(Dconst/ActVmax)+1;
            *pActAcc = Amax;

            if (Amax < Vmax)
            {
                *pDeltV = AbsDist/(*pT1) - Amax* *pT2;
            }
            else
            {
                *pActAcc = ActVmax;
                if (AbsDist == (ActVmax* *pT1 * *pT2))
                {
                    *pT2 = *pT2 - 1;
                    *pDeltV = ActVmax/2;
                }
                else
                {
                    *pT2 = *pT2 - 1;
                    *pDeltV = ActVmax - (AbsDist/(*pT1) - ActVmax* *pT2);
                }
            }
            #ifdef __DEBUG__
                fprintf(stderr, "Trapezoidal: T1 %f T2 %f T3 %f ActAcc %f DeltV %f\n",
                                 *pT1, *pT2, *pT3, *pActAcc, *pDeltV);
            #endif
        }
        else
        {
            // Dconst < ActVmax
            // Case 2: Triangular
            *pT1 = ceil(sqrt(AbsDist/Amax)) + 1;
            *pT3 = *pT1;
            *pT2 = 0;
            *pActAcc = AbsDist/(*pT1* *pT1);
            //ActDistance = *pActAcc * *pT1* *pT1;
            //ActVmax = AbsDist/(*pT1);
            #ifdef __DEBUG__
                fprintf(stderr, "Triangular: T1 %f T2 %f T3 %f ActAcc %f DeltV %f\n",
                                 *pT1, *pT2, *pT3, *pActAcc, *pDeltV);
            #endif

        }
    }
    else
    {
        // Case 2: Triangular
        // TotalTime = T(1) +T(3) = 2* sqrt(AbsDist/Amax);
        *pT1 = ceil(sqrt(AbsDist/Amax));
        *pT3 = *pT1 ;
        *pT2 = 0;
        *pActAcc = AbsDist/(*pT1* *pT1);
        // ActDistance = *pActAcc * *pT1 * *pT1;
        // ActVmax = AbsDist/(*pT1);
        #ifdef __DEBUG__
            fprintf(stderr, "Triangular: T1 %f T2 %f T3 %f ActAcc %f DeltV %f\n",
                             *pT1, *pT2, *pT3, *pActAcc, *pDeltV);
        #endif

    }

    if(Dist < 0)
    {
        *pActAcc = - *pActAcc;
        *pDeltV = - *pDeltV;
    }

    return MTN_CALC_SUCCESS;
}

#ifdef __STUB__
void main(void)
{
    MOT_ALGO_DBL Dist, Amax, Vmax;
    MOT_ALGO_DBL T1, T2, T3, ActAcc, DeltV;

    fprintf(stderr, "Enter Dist, Amax, Vmax:  ");
    scanf("%f %f %f", &Dist, &Amax, &Vmax);

    Calc2ndOrderProfile(Dist, Amax, Vmax,
                        &T1, &T2, &T3, &ActAcc, &DeltV);
    fprintf(stderr, "Dist %f Amax %f Vmax %f\n", Dist, Amax, Vmax);
    fprintf(stderr, "T1 %f T2 %f T3 %f\n", T1, T2, T3);
    fprintf(stderr, "ActAcc %f DeltV %f\n", ActAcc, DeltV);

}
#endif



