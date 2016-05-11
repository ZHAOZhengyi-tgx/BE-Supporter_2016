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


#ifndef __U_TURN_HALF_TWIN_SINE__
#define __U_TURN_HALF_TWIN_SINE__

// U-turn half twin sine module
// 
// It meets strict condition s.t. Vend = -Vini, Dist = 0, and Dist overshoot is in the same direction of Vini;


typedef struct
{
double dDist_end;
double dVini;
double dVend;
double dAmax;
double dJmax;
}U_TURN_HALF_TWIN_SINE_PRECAL_INPUT;

typedef struct
{
double T_total;
double T_acc;
double T_dec;
double T_cnst;
double ActualJerkMax;
double ActualAccMax;
double ActualVelMax;
double ActualJerkDecMax;
double ActualAccDecMax;
double ActAcc;
double AdjVmax;
double ActVmax;
double Dramp;
double Dconst;
double AbsDist;
double Dist;
double Vend;
double Vini;
}U_TURN_HALF_TWIN_SINE_PRECAL_OUTPUT;

int axis_u_turn_half_ts_precal(U_TURN_HALF_TWIN_SINE_PRECAL_INPUT *stpInput, U_TURN_HALF_TWIN_SINE_PRECAL_OUTPUT *stpOutput);

typedef U_TURN_HALF_TWIN_SINE_PRECAL_OUTPUT U_TURN_HALF_TWIN_SINE_PROFGEN_INPUT;

typedef struct
{
	double *pdDist;
	double *pdVel;
	double *pdAcc;
	double *pdJerk;
}U_TURN_HALF_TWIN_SINE_PROFGEN_OUTPUT;
int axis_u_turn_half_ts_profgen(U_TURN_HALF_TWIN_SINE_PROFGEN_INPUT *stpInput, U_TURN_HALF_TWIN_SINE_PROFGEN_OUTPUT *stpOutput);

#endif // __U_TURN_HALF_TWIN_SINE__