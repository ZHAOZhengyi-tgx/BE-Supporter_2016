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


 * Revision History:
 * DDMMYYYY     Programmer      Changes
 * 22072003     ZhengYI         First Created fPoly11Sin, Divide 2*pi into  4 regions
 * 24072003     ZhengYI, Peter  fPoly5Sin_QuarterPI, fPolySin5Cos,
 *                              Divide 2*pi into 8 regions
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include "mtncalc.h"

#define PI              3.141592654
#define PI_1_OVER_4     0.785398163
#define PI_1_OVER_2     1.570796327
#define PI_3_OVER_2     4.71238898
#define PI_2            6.283185307
#define NE_1_OVER_FACT3     (-1.666666667E-1)
#define ONE_OVER_FACT5        (8.333333333E-3)
#define NE_1_OVER_FACT7     (-1.984126984E-4)
#define ONE_OVER_FACT9        (2.755731922E-6)
#define NE_1_OVER_FACT11    (-2.505210839E-8)

extern MOT_ALGO_DBL fPoly6Cos_QuarterPI(MOT_ALGO_DBL x);

MOT_ALGO_DBL   fPoly11Sin(MOT_ALGO_DBL x)
{
MOT_ALGO_DBL xx;
MOT_ALGO_DBL x_less_pi_over_2;
MOT_ALGO_DBL y;

    if(x <= PI_1_OVER_2)
    {
        xx = x*x;
        y = x *(1 + xx *(NE_1_OVER_FACT3 + xx *(ONE_OVER_FACT5 + xx *(NE_1_OVER_FACT7 + xx *(ONE_OVER_FACT9 + xx * NE_1_OVER_FACT11)))));

    }
    else if( x>PI_1_OVER_2 && x<= PI )
    {
        x_less_pi_over_2 = PI - x;
        xx = x_less_pi_over_2 * x_less_pi_over_2;
        y = x_less_pi_over_2 * (1 + xx *(NE_1_OVER_FACT3 + xx *(ONE_OVER_FACT5 + xx *(NE_1_OVER_FACT7 + xx *(ONE_OVER_FACT9 + xx * NE_1_OVER_FACT11)))));
    }
    else if( x>PI && x<=PI_3_OVER_2 )
    {
        x_less_pi_over_2 = x - PI;
        xx = x_less_pi_over_2 * x_less_pi_over_2;
        y = - x_less_pi_over_2 * (1 + xx *(NE_1_OVER_FACT3 + xx *(ONE_OVER_FACT5 + xx *(NE_1_OVER_FACT7 + xx *(ONE_OVER_FACT9 + xx * NE_1_OVER_FACT11)))));
    }
    else if( x>PI_3_OVER_2 && x < PI_2)
    {
        x_less_pi_over_2 = PI_2 - x ;
        xx = x_less_pi_over_2 * x_less_pi_over_2;
        y = - x_less_pi_over_2 * (1 + xx *(NE_1_OVER_FACT3 + xx *(ONE_OVER_FACT5 + xx *(NE_1_OVER_FACT7 + xx *(ONE_OVER_FACT9 + xx * NE_1_OVER_FACT11)))));

    }
    else
    {
        y = 0;
        fprintf(stderr, "Error! x exceeds 2*pi ");
    }
    return y;
}

MOT_ALGO_DBL   fPoly5Sin_QuarterPI(MOT_ALGO_DBL x)
{
MOT_ALGO_DBL xx;
MOT_ALGO_DBL y;

    if(x>= -PI_1_OVER_4 && x <= PI_1_OVER_4)
    {
        xx = x*x;
        y = x *(1 + xx *(NE_1_OVER_FACT3 + xx *ONE_OVER_FACT5 ));
    }
    else
    {
        y = 0;
        fprintf(stderr, "Error! x exceeds pi/4 ");
    }
    return y;
}

MOT_ALGO_DBL fPolySin5Cos(MOT_ALGO_DBL x)
{
MOT_ALGO_DBL x_less_pi_over_4;
MOT_ALGO_DBL y;
short sRegion;

        sRegion = (short) ( x/PI_1_OVER_4 );
        switch(sRegion)
        {
            case 0:
				y = fPoly5Sin_QuarterPI(x);
            break;
            case 1:
                x_less_pi_over_4 = PI_1_OVER_2 - x;
				y = fPoly6Cos_QuarterPI(x_less_pi_over_4);
            break;
            case 2:
                x_less_pi_over_4 = x - PI_1_OVER_2;
				y = fPoly6Cos_QuarterPI(x_less_pi_over_4);
            break;
            case 3:
                x_less_pi_over_4 = PI - x;
				y = fPoly5Sin_QuarterPI(x_less_pi_over_4);
            break;

            case 4:
                x_less_pi_over_4 = x - PI;
				y = -fPoly5Sin_QuarterPI(x_less_pi_over_4);
            break;
            case 5:
                x_less_pi_over_4 = PI_3_OVER_2 - x;
				y = -fPoly6Cos_QuarterPI(x_less_pi_over_4);
            break;
            case 6:
                x_less_pi_over_4 = x - PI_3_OVER_2;
				y = - fPoly6Cos_QuarterPI(x_less_pi_over_4);
            break;
            case 7:
				x_less_pi_over_4 = PI_2 - x;
				y = - fPoly5Sin_QuarterPI(x_less_pi_over_4);
            break;
            default:
				y = 0;
				fprintf(stderr, "Error! x exceeds 2*pi ");

            break;
        }
		return y;
}
