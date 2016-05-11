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

#include "uturn_half_twinsine.h"
#include "math.h"
#include "mtncalc.h"

int axis_u_turn_half_ts_profgen(U_TURN_HALF_TWIN_SINE_PROFGEN_INPUT *stpInput, U_TURN_HALF_TWIN_SINE_PROFGEN_OUTPUT *stpOutput)
{
#define PI 3.14159265359

	double ActualAccMax = stpInput->ActualAccMax;
	double ActualVelMax = stpInput->ActualVelMax;
	double ActualJerkMax = stpInput->ActualJerkMax;
	double ActualJerkDecMax = stpInput->ActualJerkDecMax;
	double ActualAccDecMax = stpInput->ActualAccDecMax;

	// PlotFlag = 3;

	double Dist = stpInput->Dist;
	double T_acc = stpInput->T_acc;
	double T_dec = stpInput->T_dec;
	double T_cv = stpInput->T_cnst;

	double T_total = stpInput->T_total;

	double Vend = stpInput->Vend;
	double Vini = stpInput->Vini;

	// tAxis = 0:1:T_dec;
	double *j = stpOutput->pdJerk;
	double *a = stpOutput->pdAcc;
	double *v = stpOutput->pdVel;
	double *d = stpOutput->pdDist;
	double dThetaTemp;

	for(unsigned tt = 0; tt <= T_dec; tt ++) //  ii = 1:1: T_dec + 1
	{
	//    tt = tAxis(ii);
		if( tt == 0)
		{
			j[tt] = 0;
			a[tt] = 0;         
			v[tt] = Vini;
			d[tt] = 0;
		}
		else if(tt < T_dec)
		{
			dThetaTemp = tt * 2 * PI/T_dec;
			j[tt] = ActualJerkMax * sin(dThetaTemp);
			a[tt] = ActualAccMax/2 * (1 - cos(dThetaTemp));         // 
			v[tt] = ActualAccMax/2*tt + Vini  - ActualAccMax/2 * T_dec/2/PI * sin(dThetaTemp);
			d[tt] = Vini * tt + ActualAccMax/4*tt*tt + ActualAccMax/2 * (T_dec/2/PI) * (T_dec/2/PI) * (cos(dThetaTemp) - 1);
		}
		else
		{
			j[tt] = 0;
			a[tt] = 0;         
			v[tt] = Vend;
			d[tt] = 0;
		}
	}

//stUturn_tw_sin_profgen_output.d = d;
//stUturn_tw_sin_profgen_output.v = v;
//stUturn_tw_sin_profgen_output.a = a;
//stUturn_tw_sin_profgen_output.j = j;

//%01062005
//if(PlotFlag >= 3)
//    total_time = T_total
//    figure_index = 1:10;
//    str_display = 'U-turn by half TwinSine: ';
//    verify_motion_consistency(tAxis, d, v, a, j, figure_index, str_display);
//end

	#undef PI

return MTN_CALC_SUCCESS;

}
