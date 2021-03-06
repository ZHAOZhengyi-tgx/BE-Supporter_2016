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

#include "limits.h"
#include "twinsine.h"
#include "mtncalc.h"

short twinsine_precal(
        CALC_TWINSINE_PRECAL_INPUT  *stpInput,
        CALC_TWINSINE_PRECAL_OUTPUT  *stpOutput)
{

//function [D,Vdiff,Adiff,T,Vmax_real,C2,Amax_real]=twinsine(Dist,Amd_time,tmove_flag, Jmax);
//The function is used to calculate the motion time when the Twin Sinusoildal 
//Jerk profile is used in the profile generation.
//It should be noticed that the actual acceleration and maximum velocity 
//may not be excactly as the desire setting, especially when the distance 
//is very long.
//
//Dist : The distance to be traveled, [Enc_step]
//AVmd_time : AVmd_time = [Amd Vmd] or = [T1 T2] depends on the tmove_flag
//Amd  : The desired Acceleration to be used, [Enc_step/sampple^2]
//Vmd  : The desired maximum velocity limitation, [Enc_step/sampl]
//tmove_flag: 1 ==> it is a time move, AVmd_time = [T1, T2] = [Taccelerating, Tconst_v]
//            0 ==> it is a profile move.
//Jmax : The desired maximum jerk.
// Note:
//       The maximum velocity is contrained by encoder loss count
//       The maximum acceleration is constrained by system band-width (driver and mechanics)
//       The maximum jerk  = Amax * 2000 Hz (by Ou Gang)
//
//Output
//T    : T = [T1, T2]. [samples]
//	where T1 is the time used for both acceleration and deceleration, symmetrically
// 	      T2 is the time used for constant velocity. 
//             If T2 =0, the acc profile will be combined by Twin Cosine.
//D    : The distance sequence generate for simulation
//Vdiff: The velocity sequence generated by differenciation on distance
//Adiff: The acceleration sequence generated by differenciation on the velocity
//The default Amd and Vmd :
//Amd = 6.25 ; % = 80 m/sec^2. [enc_step/sample^2], Enc_step=0.2 micron; sample = 0.125 msec;
//Vmd = 625;  % = 1.0 m/sec.  [enc_step/sample].
//
// ZhengYi, Created on 27 Oct 2003. 
// 21062004 ZhengYi     Add constraints on Maximum Jerk, and minimum time

#define PI 3.141592654

MOT_ALGO_DBL Amd, Vmd, Dmv_va, Dmv_vj, Dmv, Dcv, T1d,  T1d_a, T1d_j, fDist;
int AchieveVmax, AchieveAmax, AchieveJmax;
unsigned int T2, T1;
MOT_ALGO_DBL C1, C2, C3, C4, C5, C2C4, C2C2C4; // , C5T1T1, DConstAtT1T2, Vmax_real, Amax_real, Jmax_real;
MOT_ALGO_DBL dblT2;
MOT_ALGO_DBL fJerkMax = stpInput->fJmax;

fDist = stpInput->fDist;

#define ONE_THIRD       (0.333333333333333333333)
	if( stpInput->iTimeMoveFlag ==0)
	{
	//Calculate the motion time
		Amd = stpInput->fAccMax;
		Vmd = stpInput->fVelMax;
		Dmv_va = 2*Vmd * Vmd/Amd;
		Dmv_vj = Vmd * sqrt(Vmd * PI * 2/  fJerkMax);
		//%%   Dmv = max(Dmv_va, Dmv_vj);
		if(Dmv_va > Dmv_vj)       //%% 21062004
		{
			AchieveAmax = 1;       //%% Achieve Maximum Acceleration, don't get maximum Jerk
			AchieveJmax = 0;
			Dmv = Dmv_va;
		}
		else
		{
			AchieveAmax = 0;       //%% Achieve maximum jerk, donot get maximum acceleration
			AchieveJmax = 1;
			Dmv = Dmv_vj;
		}                       //%% 21062004
		Dcv = fabs(fDist) - Dmv;

		if( Dcv >  0)
		{
			AchieveVmax = 1;      //%% Achieve maximum velocity
			dblT2 = floor(Dcv/Vmd)+1;
			if (dblT2 > UINT_MAX)
			{
				printf("Error, too long motion time T2, exceeding maximum unsigned integer %d, %s, %d", UINT_MAX, __FILE__, __LINE__ );
			}
			else
			{
				T2 = (unsigned int) dblT2;
			}
			T1d = Dmv/Vmd;
		}
		else
		{
			AchieveVmax = 0;      //%% Donot Achieve maximum velocity
			T2 = 0;
			T1d_a = sqrt(fabs(fDist)*2/Amd);            //%% 21062004
			T1d_j = pow(fabs(fDist * 2 * PI/fJerkMax), ONE_THIRD);  // 28102008
			if((T1d_a > T1d_j))
			{
				T1d = T1d_a;
				AchieveAmax = 1;      
				AchieveJmax = 0;
			}
			else
			{
				T1d = T1d_j;
				AchieveAmax = 0;
				AchieveJmax = 1;
			}
			if((T1d < 2))
			{
				T1d = 2;
			}                                       //%% 21062004
		}

		if (T1d > UINT_MAX)
		{
			printf("Error, too long motion time T1, exceeding maximum unsigned integer %d, %s, %d", UINT_MAX, __FILE__, __LINE__ );
		}
		else
		{
			T1= (unsigned int) (floor(T1d)+1);
		}
	}
	else
	{
		if (stpInput->fT1 > UINT_MAX)
		{
			printf("Error, too long motion time T1, exceeding maximum unsigned integer %d, %s, %d", UINT_MAX, __FILE__, __LINE__ );
		}
		else
		{
			T1 = (unsigned int) ceil(stpInput->fT1); 
		}
		if (stpInput->fT2 > UINT_MAX)
		{
			printf("Error, too long motion time T2, exceeding maximum unsigned integer %d, %s, %d", UINT_MAX, __FILE__, __LINE__ );
		}
		else
		{
			T2 = (unsigned int) ceil(stpInput->fT2);
		}
	}

	C1= fDist/(T1 + T2);	//This constant is from the condition of both Vel and Distance	
	C2 = T1/PI/2;
	C3= 2*PI/T1;
	C4 = C1 /T1;
	C5 = C4/2;
	C2C4 = C2*C4;
	C2C2C4 = C2*C2C4;

	// Output
	stpOutput->fC5T1T1 = C5*T1*T1;
	stpOutput->fDConstAtT1T2 = C2C2C4 + C5*T1*T1 + C1*T2;

	stpOutput->fVmax_real = C1;
	stpOutput->fAmax_real = C4 *2;
	stpOutput->fJmax_real = fabs(C3 * C4);

	stpOutput->fT1 = T1;
	stpOutput->fT2 = T2;
	stpOutput->fC1 = C1;
	stpOutput->fC2 = C2;
	stpOutput->fC3 = C3;
	stpOutput->fC4 = C4;
	stpOutput->fC5 = C5;
	stpOutput->fC2C4 = C2C4;
	stpOutput->fC2C2C4 = C2C2C4;
//	stpOutput->fC5T1T1 = C5T1T1;
//	stpOutput->fDConstAtT1T2 = DConstAtT1T2;
//	stpOutput->fAmax_real = Amax_real;
//	stpOutput->fVmax_real = Vmax_real;
//	stpOutput->fJmax_real = Jmax_real;

#undef PI
#undef ONE_THIRD

return MTN_CALC_SUCCESS;
}
