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



#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "twinsine.h"
#include "mtncalc.h"

short twinsine_profgen(
        CALC_TWINSINE_PROFGEN_INPUT  *stpInput,
        CALC_TWINSINE_PROFGEN_OUTPUT  *stpOutput)
{

MOT_ALGO_DBL T1, T2, C1, C2, C3, C4, C5, C2C4, C2C2C4, C5T1T1, DConstAtT1T2;
int ii;
MOT_ALGO_DBL *D, *V, *A, *J;
MOT_ALGO_DBL fTemp;

C1 = stpInput->fC1;
T1 = stpInput->fT1;
T2 = stpInput->fT2;
C1 = stpInput->fC1;
C2 = stpInput->fC2;
C3 = stpInput->fC3;
C4 = stpInput->fC4;
C5 = stpInput->fC5;
C2C4 = stpInput->fC2C4;
C2C2C4 = stpInput->fC2C2C4;
C5T1T1 = stpInput->fC5T1T1;
DConstAtT1T2 = stpInput->fDConstAtT1T2;

D = stpOutput->pfdist_prof;
V = stpOutput->pfvel_prof;
A = stpOutput->pfacc_prof;
J = stpOutput->pfjerk_prof;

for( ii=0; ii <= 2*T1+T2 ; ii++)
{
    if( ii <= T1)
	{
		D[ii] = C5*ii*ii + C2C2C4*cos(C3 * ii) - C2C2C4;
		V[ii] = C4*ii - C2C4*sin(C3 * ii);
		A[ii] = C4 *( 1 - cos(C3 * ii));
		J[ii] = C4 * C3 * sin(C3 * ii);
   }
   else if( (ii > T1) && (ii <= T1+T2) )
   {

      D[ii] = C5T1T1 + C1*(ii-T1);
      V[ii] = C1;
      A[ii] = 0;
      J[ii] = 0;
   }
   else if( ii >(T1+T2))
   {
      fTemp = (ii-T1-T2);
      D[ii] = C1*fTemp - C5*fTemp * fTemp - C2C2C4*cos(C3 * fTemp) + DConstAtT1T2;
      V[ii] = C1 -C4*fTemp + C2C4*sin(C3 * fTemp); 
      A[ii] = -C4*( 1 - cos(C3 * fTemp));
      J[ii] = -C4 * C3 * sin(C3 * fTemp);
   }
}



//if((twinsine_profgen_input.PlotFlag >= 3))
//{
//
//    actual_t_acc_dec = twinsine_profgen_input.T1
//    actual_t_cnst = twinsine_profgen_input.T2
//    total_time = actual_t_cnst + actual_t_acc_dec * 2
//    twinsine_profgen_input
//    figure_index = 1:10;
//    str_display = "Twin Sine - ";
//    ver if(y_motion_consistency(t, D, V, A, J, figure_index, str_display);)
//    ver{
//
//}


return MTN_CALC_SUCCESS;
}
