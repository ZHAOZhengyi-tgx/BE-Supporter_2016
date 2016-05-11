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
// Precalculation for U-turn motion, profile type is Half Twin-Sine
// It meets strict condition s.t. Vend = -Vini, Dist = 0, and Dist overshoot is in the same direction of Vini;
int axis_u_turn_half_ts_precal(U_TURN_HALF_TWIN_SINE_PRECAL_INPUT *stpInput, U_TURN_HALF_TWIN_SINE_PRECAL_OUTPUT *stpOutput)
{
#define PI 3.14159265359

	double Dist_end = stpInput->dDist_end;
	double Vend = stpInput->dVend;
	double Vini = stpInput->dVini;
	double Amax = stpInput->dAmax;
	double Jmax = stpInput->dJmax;

	if( fabs(Dist_end) > 0)
	{
	//    Dist_end
	//    disp('warning, Dist_end is set to be 0 for U-turn.');
		Dist_end = 0;
	}

	if( fabs(Vend + Vini) > 0)
	{
	//    V_end_ini = [Vend, Vini]
	//    disp('warning, set Vend = -Vini for U-turn');
		Vend = -Vini;
	}

	if( Amax < 0)
	{
		Amax = - Amax;
	}
	if( Jmax <0)
	{
		Jmax = -Jmax;
	}

	double Aave = Amax/2;
	double Tdec_by_a = fabs(Vend - Vini)/Aave;
	double Tdec_by_j = sqrt(fabs(Vend - Vini) * 2 *PI/Jmax);
	double T_dec;

	if( Tdec_by_a < Tdec_by_j)
	{
		T_dec = ceil(Tdec_by_j);
	}
	else
	{
		T_dec = ceil(Tdec_by_a);
	}
	double A_max_a = (Vend - Vini) / T_dec * 2;
	double J_max_a = A_max_a * PI/T_dec;
	double Dist_max_os = Vini * T_dec/2 +  A_max_a * T_dec * T_dec/16 - 1.0/4/PI/PI * A_max_a * T_dec * T_dec;

	//// output real-time parameter
	stpOutput->T_total = T_dec;
	stpOutput->T_acc = 0;
	stpOutput->T_dec = T_dec;
	stpOutput->T_cnst = 0;
	stpOutput->ActualJerkMax = J_max_a;
	stpOutput->ActualAccMax = A_max_a;
	if( Vini > Vend)
	{
		stpOutput->ActualVelMax = Vini;
	}
	else
	{
		stpOutput->ActualVelMax = Vend;
	}
	stpOutput->ActualJerkDecMax = J_max_a;
	stpOutput->ActualAccDecMax = A_max_a;
	//stpOutput->T = [0, 0, T_dec];
	stpOutput->ActAcc = A_max_a;
	stpOutput->AdjVmax = 0;
	stpOutput->ActVmax = stpOutput->ActualVelMax; // max([Vini, Vend]);
	stpOutput->Dramp = Dist_max_os;
	stpOutput->Dconst = 0;
	stpOutput->AbsDist = Dist_max_os;
	stpOutput->Dist = Dist_end;
	stpOutput->Vend = Vend;
	stpOutput->Vini = Vini;

#undef PI

return MTN_CALC_SUCCESS;

}