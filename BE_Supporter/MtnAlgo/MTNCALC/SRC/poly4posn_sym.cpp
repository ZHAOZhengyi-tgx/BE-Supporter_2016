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


#include "stdafx.h"

// #include "poly4posn_sym.c"
#include "math.h"

typedef struct
{
	double dDist_m;  // p
	double dMaxVel_si;
	double dMaxAcc_si; 
	double dMaxJerk_si;
	double dMaxDerJerk_si;
	double dSampleTime_s;  // <=0: Continuoust time
	double dPosnUnit_m;
	int nNumDecimal;  // 
	double dEpslon;

}CMDGEN_POLY4POSN_PRECAL_INPUT;

typedef struct
{
	double dTimeConstDerJerk;
	double dTimeConstJerk;
	double dTimeConstAcc;
	double dTimeConstVel;

	double dDerJerk_1;
	double dDerJerk_2;
	double dDerJerk_3;
	double dDerJerk_4;
}CMDGEN_POLY4POSN_PRECAL_OUTPUT;

#define ERROR_NEGATIVE_VALUES_FOUND   20
#define DOUBLE_EPSLON     2.220446049250313E-16

double f_round(double dIn) 
{
	if(dIn > 0)
	{
		return floor(dIn + 0.5);
	}
	else
	{
		return floor(dIn + 0.5);   /// ?
	}
}

double f_sign(double fIn)
{
	if(fIn >= 0)
	{
		return 1.0;
	}
	else
	{
		return -1.0;
	}
}

int mtn_api_poly4posn_precal(CMDGEN_POLY4POSN_PRECAL_INPUT *stpInput, CMDGEN_POLY4POSN_PRECAL_OUTPUT *stpOutput)
{
//// Checking validity of inputs
//if nargin < 5 | nargin > 8
//   help make4
//   return
//else
int iRet = 0;

	double   fDist = stpInput->dDist_m; // abs(varargin{1}); p
	double   fMaxVel = fabs(stpInput->dMaxVel_si); // abs(varargin{2}); v
	double   fMaxAcc =fabs(stpInput->dMaxAcc_si); // abs(varargin{3}); a
	double   fMaxJerk = fabs(stpInput->dMaxJerk_si); // abs(varargin{4}); j
	double   fMaxDerJerk = fabs(stpInput->dMaxDerJerk_si);  // abs(varargin{5}); d
	double   dTs = stpInput->dSampleTime_s;
	if( dTs < 0)
	{
		dTs = 0;
	}
	double   dEps = stpInput->dEpslon;
	if( dEps < 0 || dEps > 1E-6)
	{
		dEps = DOUBLE_EPSLON;
	}
		
	int nNumDecimal = stpInput->nNumDecimal;
	if(nNumDecimal <0 || nNumDecimal >15)
	{
		nNumDecimal = 15;
	}
//   if nargin == 5
//      Ts=0; r=eps; s=15;
//   elseif nargin == 6
//      Ts=abs(varargin{6});
//      r=eps; s=15;
//  elseif nargin == 7
//      Ts=abs(varargin{6});
//      r=abs(varargin{7});
//      s=15;
//  elseif nargin == 8
//      Ts=abs(varargin{6});
//      r=abs(varargin{7});
//      s=abs(varargin{8});
//   end
//end

//if length(fDist)==0 | length(fMaxVel)==0 | length(fMaxAcc)==0 | length(fMaxJerk)==0 | length(fMaxDerJerk)==0 | ...
//   length(Ts)==0 | length(r)==0 | length(s)==0 
//   disp('ERROR: insufficient input for trajectory calculation')
//   return
//end

	double tol = DOUBLE_EPSLON;   //// tolerance required for continuous time calculations
	double dd = fMaxDerJerk;      //// required for discrete time calculations

	//// Calculation constant djerk phase duration: t1
	double t1  = pow((1/8.0 * fDist / fMaxDerJerk), (1.0/4)) ;  //// largest t1 with bound on derivative of jerk
	if( dTs>0 )
	{
	   t1 = ceil(t1/dTs)*dTs;
	   dd  = 1.0/8*fDist/(t1*t1*t1*t1);  // 1.0/8*fDist/(t1^4);
	}
	//// velocity test
	if( fMaxVel < 2*dd*t1*t1*t1)    // t1^3       //// fMaxVel bound violated ?
	{
	   t1 = pow((1.0/2*fMaxVel/fMaxDerJerk), 1.0/3); // (1.0/2*fMaxVel/fMaxDerJerk)^(1.0/3) ;  //// t1 with bound on velocity not violated
	   if(dTs>0)
	   {
		  t1 = ceil(t1/dTs)*dTs;
		  dd  = 1/2*fMaxVel/(t1*t1*t1);  // t1^3
	   }
	}
	//// acceleration test
	if( fMaxAcc < dd*t1*t1)  //  t1^2         //// fMaxAcc bound violated ?
	{
	   t1 = pow((fMaxAcc/fMaxDerJerk), (1.0/2));  // (fMaxAcc/fMaxDerJerk)^(1/2) ;  //// t1 with bound on acceleration not violated
	   if(dTs>0)
	   {
		  t1 = ceil(t1/dTs)*dTs;
		  dd  = fMaxAcc/(t1*t1);  // t1^2
	   }
	}
	//// jerk test
	if( fMaxJerk < dd * t1)    //// fMaxJerk bound violated ?
	{
	   t1 = fMaxJerk/fMaxDerJerk ;    //// t1 with bound on jerk not violated
	   if( dTs>0 )
	   {
		  t1 = ceil(t1/dTs)*dTs; 
		  dd  = fMaxJerk/t1;
	   }
	}
	fMaxDerJerk = dd;  //// as t1 is now fixed, dd is the new bound on derivative of jerk

	//// Calculation constant jerk phase duration: t2
	double P = -1/9  * t1 * t1;                 //// calculations to determine   //// -1/9  * t1^2;
	double Q = -1/27 * t1 * t1 * t1  -  fDist/(4*fMaxDerJerk*t1);  //// positive real solution of 
	double D = P*P*P + Q*Q;                    //// third order polynomial... P^3 + Q^2
	double R = pow(( -Q + sqrt(D) ), (1.0/3));       //// ( -Q + sqrt(D) )^(1/3); 
	double t2 = R - P/R - 5.0/3*t1 ;           //// largest t2 with bound on jerk
	if( dTs>0 )
	{
	   t2 = ceil(t2/dTs)*dTs;
	   dd  = fDist/( 8*t1*t1*t1*t1 + 16*t1*t1*t1*t2 + 10*t1*t1*t1*t2*t2 + 2*t1*t2*t2*t2 );   //// ( 8*t1^4 + 16*t1^3*t2 + 10*t1^2*t2^2 + 2*t1*t2^3 )
	}
	if( abs(t2)<tol) { t2=0; } //// for continuous time case
	//// velocity test
	if( fMaxVel < (2*dd*t1*t1*t1 + 3*dd*t1*t1*t2 + dd*t1*t2*t2) )  //// fMaxVel bound violated ?
	{
	   t2 = sqrt( t1*t1/4 + fMaxVel/fMaxDerJerk/t1 ) - 3.0/2*t1 ;     //// t2 with bound on velocity not violated, ( t1^2/4 + fMaxVel/fMaxDerJerk/t1 )^(1/2) - 3/2*t1 ;  
	   if( dTs>0  )
	   {
		  t2 = ceil(t2/dTs)*dTs;
		  dd = fMaxVel/( 2*t1*t1*t1 + 3*t1*t1*t1*t2 + t1*t2*t2 );    //// ( 2*t1^3 + 3*t1^2*t2 + t1*t2^2 )
	   }
	}
	if(abs(t2)<tol) { t2=0; } //// for continuous time case
	//// acceleration test
	if( fMaxAcc < (dd*t1*t1 + dd*t1*t2))  //// fMaxAcc bound violated ?  (dd*t1^2 + dd*t1*t2)
	{
	   t2 = fMaxAcc/(fMaxDerJerk*t1) - t1 ;      //// t2 with bound on acceleration not violated
	   if( dTs>0  )
	   {
		  t2 = ceil(t2/dTs)*dTs;
		  dd  = fMaxAcc/( t1*t1 + t1*t2 );   //// fMaxAcc/( t1^2 + t1*t2 );
	   }
	}
	if( abs(t2)<tol) { t2=0; } //// for continuous time case
	fMaxDerJerk = dd;  //// as t2 is now fixed, dd is the new bound on derivative of jerk

	//// Calculation constant acceleration phase duration: t3
	double c1 = t1 * t1 + t1*t2 ;                                       //// t1^2
	double c2 = 6*t1*t1*t1 + 9*t1*t1*t2 + 3*t1*t2*t2 ;                   ////  t1^3 + 9*t1^2*t2 + 3*t1*t2^2
	double c3 = 8*t1*t1*t1*t1 + 16*t1*t1*t1*t2 + 10*t1*t1*t2*t2 + 2*t1*t2*t2*t2 ;   ////  8*t1^4 + 16*t1^3*t2 + 10*t1^2*t2^2 + 2*t1*t2^3
	double t3 = (-c2 + sqrt(c2*c2 - 4*c1*(c3 - fDist/fMaxDerJerk)))/(2*c1) ;          //// largest t3 with bound on acceleration
		// (c2^2-4*c1*(c3-fDist/fMaxDerJerk)
	if( dTs>0 )
	{
	   t3 = ceil(t3/dTs)*dTs;
	   dd = fDist/( c1*t3 * t3 + c2*t3 + c3 );  // t3^2
	}
	if( abs(t3)<tol) { t3=0; } //// for continuous time case
	//// velocity test
	if( fMaxVel < dd*(2*t1*t1*t1 + 3*t1*t1*t2 + t1*t2*t2 + t1*t1*t3 + t1*t2*t3) )  //// fMaxVel bound violated ?  dd*(2*t1^3 + 3*t1^2*t2 + t1*t2^2 + t1^2*t3 + t1*t2*t3)
	{
		//// t3, bound on velocity not violated
	   t3 = -(2*t1*t1*t1 + 3*t1*t1*t2 + t1*t2*t2 - fMaxVel/fMaxDerJerk)/(t1*t1 + t1*t2);  /// -(2*t1^3 + 3*t1^2*t2 + t1*t2^2 - fMaxVel/fMaxDerJerk)/(t1^2 + t1*t2);  
	   if( dTs>0 )
	   {
		  t3 = ceil(t3/dTs)*dTs;
		  dd = fMaxVel/( 2*t1*t1*t1 + 3*t1*t1*t2 + t1*t2*t2 + t1*t1*t3 + t1*t2*t3 );  /// fMaxVel/( 2*t1^3 + 3*t1^2*t2 + t1*t2^2 + t1^2*t3 + t1*t2*t3 )
	   }
	}
	if( abs(t3)<tol) { t3=0; } //// for continuous time case
	fMaxDerJerk = dd;  //// as t3 is now fixed, dd is the new bound on derivative of jerk

	//// Calculation constant velocity phase duration: t4 
	 //// t4 with bound on velocity
	double t4 = ( fDist - fMaxDerJerk*(c1*t3*t3 + c2*t3 + c3) )/fMaxVel ;   //// ( fDist - fMaxDerJerk*(c1*t3^2 + c2*t3 + c3) )/fMaxVel
	if( dTs>0 )
	{
	   t4 = ceil(t4/dTs)*dTs;
	   dd = fDist/( c1*t3*t3 + c2*t3 + c3 + t4*(2*t1*t1*t1 + 3*t1*t1*t2 + t1*t2*t2 + t1*t1*t3 + t1*t2*t3) ) ;  //// dd = fDist/( c1*t3^2 + c2*t3 + c3 + t4*(2*t1^3 + 3*t1^2*t2 + t1*t2^2 + t1^2*t3 + t1*t2*t3) ) ;
	}
	if( abs(t4)<tol) { t4=0; } //// for continuous time case

	//// All time intervals are now calculated
	double t[4]; // = [t1 t2 t3 t4] ;
	t[0] = t1, t[1] = t2, t[2] = t3, t[3] = t4;
	//// This error should never occur !!
	if( min(t1, min(t2, min(t3, t4)))<0 )
	{
	//    disp('ERROR: negative values found');
		iRet = ERROR_NEGATIVE_VALUES_FOUND;  //  
	}

	//// Quantization of dd and calculation of required position correction (decimal scaling)
	if( dTs>0 )
	{
	double   x=ceil(log10(dd));          //// determine exponent of dd
	double   ddq=dd/pow(10.0, x);                //// scale to 0-1  //// ddq=dd/10^x
	   ddq = f_round(ddq* pow(10.0, (double)(nNumDecimal)))/(pow(10, (double)(nNumDecimal)));   //// round to nNumDecimal  decimals  ddq=round(ddq* 10^nNumDecimal)/10^nNumDecimal;   
	   ddq=ddq*pow(10, x);   //// ddq*10^x
	   //// actual displacement obtained with quantized dd
	double  pp = ddq*( c1*t3*t3 + c2*t3 + c3 + t4*(2*t1*t1*t1 + 3*t1*t1*t2 + t1*t2*t2 + t1*t1*t3 + t1*t2*t3) ) ;  ///  pp = ddq*( c1*t3^2 + c2*t3 + c3 + t4*(2*t1^3 + 3*t1^2*t2 + t1*t2^2 + t1^2*t3 + t1*t2*t3) ) ;
	double  dif=fDist-pp;          //// position error due to quantization of dd
	double  cnt= f_round (dif/dEps);  //// divided by resolution gives 'number of increments' 
						  //// of required position correction
	   //// smooth correction obtained by dividing over entire trajectory duration
	double   tt = 8*t1 + 4*t2 + 2*t3 + t4;
	double   ti = tt/dTs;        //// should be integer number of samples
	double cor1= f_sign(cnt)*floor(abs(cnt/ti))*ti;   //// we need cor1/ti increments correction at each 
											   //// ... sample during trajectory
	double cor2=cnt-cor1;                          //// remaining correction: 1 increment per sample 
											   //// ... during first part of trajectory
	   /// dd=[ddq cor1 cor2 dd];
		stpOutput->dDerJerk_1 = ddq;
		stpOutput->dDerJerk_2 = cor1;
		stpOutput->dDerJerk_3 = cor2;
		stpOutput->dDerJerk_4 = dd;
	}
	else
	{
	//   dd=[dd 0 0 dd]; //// continuous time result in same format
		stpOutput->dDerJerk_1 = dd;
		stpOutput->dDerJerk_2 = 0; // cor1;
		stpOutput->dDerJerk_3 = 0; //cor2;
		stpOutput->dDerJerk_4 = dd;
	}

	return iRet;
}

//// Finished.
////////////////////////////////////////////////////////////////////////////%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
typedef struct
{
	double dTimeConstDerJerk;
	double dTimeConstJerk;
	double dTimeConstAcc;
	double dTimeConstVel;

	double dMaxDerJerk;

	double dAccuracy;
}MTN_API_POLY4POSN_PROFGEN_INPUT;

typedef struct
{
	double *adProfileDerJerk;
	double *adProfileJerk;
	double *adProfileAcc;
	double *adProfileVel;
	double *adProfileDist;
	double adSwitchTimes[16];
}MTN_API_POLY4POSN_PROFGEN_OUTPUT;

//function [dj,tx,d,j,a,v,p,tt]=profile4(t,d,acc,plt)
int mtn_api_poly4posn_profgen(MTN_API_POLY4POSN_PROFGEN_INPUT *stpInput, MTN_API_POLY4POSN_PROFGEN_OUTPUT *stpOutput)
{
	int iRet = 0;

// function [dj,tx,d,j,a,v,p,tt]=profile4(t,d,acc)
//
// Calculate symmetrical fourth order profiles from times: 
//
//  Inputs:
//
//      t(1) = constant djerk phase duration
//      t(2) = constant jerk phase duration (default 0)
//      t(3) = constant acceleration phase duration (default 0)
//      t(4) = constant velocity phase duration (default 0)
// 
//      d    = bound on djerk
//      acc  = continuous time: accuracy for profiles: t(1)*acc = minimal timestep
//             discrete time:   sample time
//
//  Outputs:
//
//      dj  = derivative of jerk profile suitable for simulink 
//
//      tx  = time sequence for plotting profiles
//      d   = derivative of jerk profile
//      j   = jerk profile
//      a   = acceleration profile
//      v   = velocity profile
//      p   = position profile
//
//      tt  = 16 switching times for profile:
//
//       0 1              6 7             10 11  12 13
//       .-.              .-.              .-.    .-.
//       | |              | |              | |    | |
//       | |  2 3    4 5  | |         8 9  | |    | |  14 15
//       '-'--.-.----.-.--' '---------.-.--'-'----'-'--.-.--
//            | |    | |              | |              | |
//            | |    | |              | |              | |
//            '-'    '-'              '-'              '-'
//
//  Note: coinciding switching times are not removed 

// Experimental version: no guarantees
//
// Paul Lambrechts, TUE fac. WTB, last modified: Jan. 7, 2003.
//

//if nargin~=3 & nargin~=4
//    help profile4
//    return
//end
//if nargin==3
//    plt=1;
//end

static double adTimeIndexMinDistWithMaxDerJerk[16] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8};

static double adTimeIndexConstJerk_Phase1[16] ={0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8};
static double adTimeIndexConstJerk_Phase2[16] ={0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4};

static double adTimeIndexConstAcc_Phase1[16] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8};
static double adTimeIndexConstAcc_Phase2[16] = {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4};
static double adTimeIndexConstAcc_Phase3[16] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};

static double adTimeIndexConstVel_Phase1[16] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8};
static double adTimeIndexConstVel_Phase2[16] = {0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4};
static double adTimeIndexConstVel_Phase3[16] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
static double adTimeIndexConstVel_Phase4[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

	double dT1 = stpInput->dTimeConstDerJerk;
	double dT2 = stpInput->dTimeConstJerk;
	double dT3 = stpInput->dTimeConstAcc;
	double dT4 = stpInput->dTimeConstVel;
int ii;
double tt[16];

	if( fabs(stpInput->dTimeConstVel) < 0.5   // length(t)==1  // min distance with max djerk
		&& fabs(stpInput->dTimeConstAcc) < 0.5
		&& fabs(stpInput->dTimeConstJerk) < 0.5)
	{
		for(ii=0; ii<16; ii++)
		{
			tt[ii] = adTimeIndexMinDistWithMaxDerJerk[ii] * stpInput->dTimeConstDerJerk;
		}
//    tt=   [0 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8]*t;  //    tt=   [0 1 1 2 2 3 4 4 4 4 4 5 5 6 6 7]*t;
	}
	else if( fabs(stpInput->dTimeConstVel) < 0.5   // length(t)==1  // min distance with max djerk
		&& fabs(stpInput->dTimeConstAcc) < 0.5)
	{
		for(ii=0; ii<16; ii++)
		{
			tt[ii] = adTimeIndexConstJerk_Phase1[ii] * stpInput->dTimeConstDerJerk
				+ adTimeIndexConstJerk_Phase2[ii] * stpInput->dTimeConstJerk;
		}
	}
	else if( fabs(stpInput->dTimeConstVel) < 0.5 )
	{
		for(ii=0; ii<16; ii++)
		{
			tt[ii] = adTimeIndexConstAcc_Phase1[ii] * stpInput->dTimeConstDerJerk
				+ adTimeIndexConstAcc_Phase2[ii] * stpInput->dTimeConstJerk
				+ adTimeIndexConstAcc_Phase3[ii] * stpInput->dTimeConstAcc;
		}
	}
	else
	{
		for(ii=0; ii<16; ii++)
		{
			tt[ii] = adTimeIndexConstVel_Phase1[ii] * stpInput->dTimeConstDerJerk
				+ adTimeIndexConstVel_Phase2[ii] * stpInput->dTimeConstJerk
				+ adTimeIndexConstVel_Phase3[ii] * stpInput->dTimeConstAcc
				+ adTimeIndexConstVel_Phase4[ii] * stpInput->dTimeConstVel;
		}
	}

//elseif length(t)==2 // constant jerk phase
    //tt=   [0 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8]*t(1) ...
    //    + [0 0 1 1 1 1 2 2 2 2 3 3 3 3 4 4]*t(2);
    
//elseif length(t)==3 // constant acceleration phase
//    tt=   [0 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8]*t(1) ...
//        + [0 0 1 1 1 1 2 2 2 2 3 3 3 3 4 4]*t(2) ...
//        + [0 0 0 0 1 1 1 1 1 1 1 1 2 2 2 2]*t(3) ;
//
//elseif length(t)==4 // constant velocity phase
//    tt=   [0 1 1 2 2 3 3 4 4 5 5 6 6 7 7 8]*t(1) ...
//        + [0 0 1 1 1 1 2 2 2 2 3 3 3 3 4 4]*t(2) ...
//        + [0 0 0 0 1 1 1 1 1 1 1 1 2 2 2 2]*t(3) ...
//        + [0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1]*t(4) ;
//    
//else
//    return
//end

////////////////////////////////////%%%%%%%%%%%%%%%
// Generate Simulink look-up table
//dt=[];
//for i=1:16
//    dt =  [dt   [1 1]*tt(i) ];
//end
//dt  = [dt 1.5*tt(16)];
//
//dd = [0 d d 0];
//dd = [ dd -dd -dd dd -dd dd dd -dd 0];
//
//dj = [dt ; dd] ;
//
//if plt==0  // no plot required
//    tx=[];d=[];j=[];a=[];v=[];p=[];    // dummy outputs
//    return
//end

//////////////////////////////////////////%%%%%%%%%%%
// Generate profiles for plotting

// Determine continuous or discrete
//if max(abs( round(t/acc)-t/acc )) > 1e-12 // continuous
//   disp('Calculating continuous time profiles')
//   step = t(1)*acc;
//   tx=[0:step:1.2*tt(16)]';
//   x=[];
//   for i=0:step:1.2*tt(16)
//       j=find(i<=dj(1,:));
//       x=[x ; dj(2,j(1))];
//   end
//   d=x;
//   j=cumsum(d)*step;
//   a=cumsum(j)*step; 
//   v=cumsum(a)*step;
//   p=cumsum(v)*step;
//
//else // discrete

    
//   disp('Calculating discrete time profiles')
	double Ts = stpInput->dAccuracy; // acc;
    double ttest[17];
	for(ii=0; ii<16; ii++)
	{
		ttest[ii] = tt[ii];
	}
	ttest[16] = 1.5*tt[15];
//	=[tt 1.5*tt[15]];
int   len = (int)f_round(1.2*tt[15]/Ts + 1); // length of profiles
//   xd = zeros(len,1);
//   xj = xd;
//   xa = xd;
//   xv = xd;
//   xp = xd;
//   xd(1) = d;
//   tx=[0:Ts:1.2*tt[15]+Ts/2]';
//   for time=Ts:Ts:1.2*tt[15]+Ts/2
//      j = find( (time + Ts/2) <= ttest ); j = j(1)-1;
//      k = round(time/Ts);
//      if j==1 | j==7 | j==11 | j==13
//          xd(k+1) =  d;
//      elseif j==3 | j==5 | j==9 | j==15
//          xd(k+1) = -d;
//      else
//          xd(k+1) =  0;
//      end
//      xj(k+1) = xj(k) + xd(k)*Ts;
//      xa(k+1) = xa(k) + xj(k)*Ts;
//      xv(k+1) = xv(k) + xa(k)*Ts;
//      xp(k+1) = xp(k) + xv(k)*Ts;
//   end
//   d=xd;j=xj;a=xa;v=xv;p=xp;
//
//   dj(1,:)=dj(1,:)+Ts/2 ; // add Ts/2 to avoid numerical problems
//end

// dj=dj'; // to be compatible with Simulink 'From Workspace' block

#ifdef  __TBA

#endif  // __TBA
// figure
//close all
//subplot(511);plot(tx,d,'k','LineWidth',1.5);hold on;plot([0 0],[-1 1]*max(d),'k--',[1 1]*max(tt),[-1 1]*max(d),'k--','LineWidth',1.5); grid on; axis([ [-0.01 1]*max(tx) [-1.1 1.1]*max(d)]);
//title('Fourth order trajectory profiles');ylabel('d [m/s4]');
//subplot(512);plot(tx,j,'k','LineWidth',1.5);hold on;plot([0 0],[-1 1]*max(j),'k--',[1 1]*max(tt),[-1 1]*max(j),'k--','LineWidth',1.5);grid on; axis([ [-0.01 1]*max(tx) [-1.1 1.1]*max(j)]);
//ylabel('j [m/s3]');
//subplot(513);plot(tx,a,'k','LineWidth',1.5);hold on;plot([0 0],[-1 1]*max(a),'k--',[1 1]*max(tt),[-1 1]*max(a),'k--','LineWidth',1.5);grid on; axis([ [-0.01 1]*max(tx) [-1.1 1.1]*max(a)]);
//ylabel('a [m/s2]');
//subplot(514);plot(tx,v,'k','LineWidth',1.5);hold on;plot([0 0],[0 1]*max(v),'k--',[1 1]*max(tt),[0 1]*max(v),'k--','LineWidth',1.5);grid on; axis([ [-0.01 1]*max(tx) [-0.1 1.1]*max(v)]);
//ylabel('v [m/s]');
//subplot(515);plot(tx,p,'k','LineWidth',1.5);hold on;plot([0 0],[0 1]*max(p),'k--',[1 1]*max(tt),[0 1]*max(p),'k--','LineWidth',1.5);grid on; axis([ [-0.01 1]*max(tx) [-0.1 1.1]*max(p)]);
//xlabel('time [s]');ylabel('x [m]');
//set(1,'position',[700 100 500 860])
//set(1,'paperposition',[0 0 5 8.6])
//
//return
//subplot(511);
//text(tt( 1)+tt(16)/200,-max(d)/5,'t_0');
//text(tt( 2)-tt(16)/200,-max(d)/5,'t_1');
//text(tt( 3)-tt(16)/200, max(d)/5,'t_2');
//text(tt( 4)-tt(16)/200, max(d)/5,'t_3');
//text(tt( 5)-tt(16)/200, max(d)/5,'t_4');
//text(tt( 6)-tt(16)/200, max(d)/5,'t_5');
//text(tt( 7)-tt(16)/200,-max(d)/5,'t_6');
//text(tt( 8)-tt(16)/200,-max(d)/5,'t_7');
//text(tt( 9)-tt(16)/200, max(d)/5,'t_8');
//text(tt(10)-tt(16)/200, max(d)/5,'t_9');
//text(tt(11)-tt(16)/100,-max(d)/5,'t_1_0');
//text(tt(12)-tt(16)/200,-max(d)/5,'t_1_1');
//text(tt(13)-tt(16)/100,-max(d)/5,'t_1_2');
//text(tt(14)-tt(16)/200,-max(d)/5,'t_1_3');
//text(tt(15)-tt(16)/100, max(d)/5,'t_1_4');
//text(tt(16)-tt(16)/200, max(d)/5,'t_1_5');


	return iRet;
}
