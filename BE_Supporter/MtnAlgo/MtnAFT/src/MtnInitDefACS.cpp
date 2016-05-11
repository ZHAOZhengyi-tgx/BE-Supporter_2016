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
#include "MtnInitAcs.h"

#define DEF_AXIS_X     0
#define DEF_AXIS_Y     1
#define DEF_AXIS_Z     2
#define DEF_AXIS_W     3

#define TABLE_X_SLPKP               	(150  )
#define TABLE_X_SLVKP               	(180  )
#define TABLE_X_SLVKI               	(400  )
#define TABLE_X_SLVLI               	(50   )
#define TABLE_X_SLVSOF              	(500  )
#define TABLE_X_SLVSOFD             	(0.707)
#define TABLE_X_SLVNFRQ             	(500  )
#define TABLE_X_SLVNWID             	(30   )
#define TABLE_X_SLVNATT             	(5    )
#define TABLE_X_SLAFF               	(7200 )
#define TABLE_X_SLFRC               	(0    )

#define TABLE_Y_SLPKP  					(50   )
#define TABLE_Y_SLVKP  	                (180  )
#define TABLE_Y_SLVKI  	                (250 )
#define TABLE_Y_SLVLI  	                (50   )
#define TABLE_Y_SLVSOF 	                (300  )
#define TABLE_Y_SLVSOFD	                (0.707)
#define TABLE_Y_SLVNFRQ	                (300  )
#define TABLE_Y_SLVNWID	                (30   )
#define TABLE_Y_SLVNATT	                (5    )
#define TABLE_Y_SLAFF  	                (9000 )
#define TABLE_Y_SLFRC  	                (0    )

#define BOND_Z_SLPKP  					(50   )
#define BOND_Z_SLVKP  	                (50   )
#define BOND_Z_SLVKI  	                (150  )
#define BOND_Z_SLVLI  	                (50   )
#define BOND_Z_SLVSOF 	                (3000 )
#define BOND_Z_SLVSOFD	                (0.707)
#define BOND_Z_SLVNFRQ	                (500  )
#define BOND_Z_SLVNWID	                (50   )
#define BOND_Z_SLVNATT	                (5    )
#define BOND_Z_SLAFF  	                (650  )
#define BOND_Z_SLFRC  	                (0    )

#define WIRE_CLAMP_SLPKP                 (0)
#define WIRE_CLAMP_SLVKP                 (0)
#define WIRE_CLAMP_SLVKI                 (0)
#define WIRE_CLAMP_SLVLI                 (0)
#define WIRE_CLAMP_SLVSOF                (0)
#define WIRE_CLAMP_SLVSOFD               (0)
#define WIRE_CLAMP_SLVNFRQ               (0)
#define WIRE_CLAMP_SLVNWID               (0)
#define WIRE_CLAMP_SLVNATT               (0)
#define WIRE_CLAMP_SLAFF                 (0)
#define WIRE_CLAMP_SLFRC                 (0)


#define TABLE_X_MAX_VEL                  (1900000  )
#define TABLE_X_MAX_ACC                  (50000000 )
#define TABLE_X_MAX_DEC                  (50000000 )
#define TABLE_X_MAX_KILL_DEC             (500000000)
#define TABLE_X_MAX_JERK                 (600000000)
#define TABLE_X_PROF_TYPE                (0        )

#define TABLE_Y_MAX_VEL                   (1900000  )
#define TABLE_Y_MAX_ACC                   (30000000 )
#define TABLE_Y_MAX_DEC                   (30000000 )
#define TABLE_Y_MAX_KILL_DEC              (30000000 )
#define TABLE_Y_MAX_JERK                  (600000000)
#define TABLE_Y_PROF_TYPE                 (0        )

#define BOND_Z_MAX_VEL                     (1950000   )
#define BOND_Z_MAX_ACC                     (100000000 )
#define BOND_Z_MAX_DEC                     (100000000 )
#define BOND_Z_MAX_KILL_DEC                (100000000 )
#define BOND_Z_MAX_JERK                    (5000000000)
#define BOND_Z_PROF_TYPE                   (0         )

#define WIRE_CLAMP_MAX_VEL                  (0)
#define WIRE_CLAMP_MAX_ACC                  (0)
#define WIRE_CLAMP_MAX_DEC                  (0)
#define WIRE_CLAMP_MAX_KILL_DEC             (0)
#define WIRE_CLAMP_MAX_JERK                 (0)
#define WIRE_CLAMP_PROF_TYPE                (0)


#define TABLE_X_POSN_ERR_IDLE                (1000     )   
#define TABLE_X_POSN_ERR_VEL                 (1000     )   
#define TABLE_X_POSN_ERR_ACC                 (1000     )   
#define TABLE_X_CRITICALPOSNERR_IDLE         (200      )   
#define TABLE_X_CRITICALPOSNERR_VEL          (1000     )   
#define TABLE_X_CRITICALPOSNERR_ACC          (1000     )   
#define TABLE_X_DYNAMICBRAKETHRESHOLDVEL     (0        )   
#define TABLE_X_MAX_VELX                     (2100000  )   
#define TABLE_X_MAX_ACCX                     (100000000)   
#define TABLE_X_RMS_DRVCMDX                  (33       )   
#define TABLE_X_RMS_DRVCMDIDLE               (50       )   
#define TABLE_X_RMS_DRVCMDMTN                (100      )    
#define TABLE_X_RMS_TIMECONST                (3200     )    
#define TABLE_X_SOFTWAREPOSNLIMIT_LOW        (180000   )   
#define TABLE_X_SOFTWAREPOSNLIMIT_UPP        (-180000  )   
                                                           
#define TABLE_Y_POSN_ERR_IDLE                 (1000     )  
#define TABLE_Y_POSN_ERR_VEL                  (1000     )  
#define TABLE_Y_POSN_ERR_ACC                  (1000     )  
#define TABLE_Y_CRITICALPOSNERR_IDLE          (200      )  
#define TABLE_Y_CRITICALPOSNERR_VEL           (1000     )  
#define TABLE_Y_CRITICALPOSNERR_ACC           (1000     )  
#define TABLE_Y_DYNAMICBRAKETHRESHOLDVEL      (0        )  
#define TABLE_Y_MAX_VELX                      (2100000  )  
#define TABLE_Y_MAX_ACCX                      (100000000)  
#define TABLE_Y_RMS_DRVCMDX                   (33       )  
#define TABLE_Y_RMS_DRVCMDIDLE                (50       )  
#define TABLE_Y_RMS_DRVCMDMTN                 (100      )   
#define TABLE_Y_RMS_TIMECONST                 (3200     )   
#define TABLE_Y_SOFTWAREPOSNLIMIT_LOW         (90000    )  
#define TABLE_Y_SOFTWAREPOSNLIMIT_UPP         (-90000   )  
                                                           
#define BOND_Z_POSN_ERR_IDLE                   (300      ) 
#define BOND_Z_POSN_ERR_VEL                    (500      ) 
#define BOND_Z_POSN_ERR_ACC                    (1000     ) 
#define BOND_Z_CRITICALPOSNERR_IDLE            (300      ) 
#define BOND_Z_CRITICALPOSNERR_VEL             (500      ) 
#define BOND_Z_CRITICALPOSNERR_ACC             (1000     ) 
#define BOND_Z_DYNAMICBRAKETHRESHOLDVEL        (0        ) 
#define BOND_Z_MAX_VELX                        (2100000  ) 
#define BOND_Z_MAX_ACCX                        (100000000) 
#define BOND_Z_RMS_DRVCMDX                     (25       ) 
#define BOND_Z_RMS_DRVCMDIDLE                  (25       ) 
#define BOND_Z_RMS_DRVCMDMTN                   (100      ) 
#define BOND_Z_RMS_TIMECONST                   (3200      ) 
#define BOND_Z_SOFTWAREPOSNLIMIT_LOW           (10000    ) 
#define BOND_Z_SOFTWAREPOSNLIMIT_UPP           (-8000    ) 
                                                           
#define WIRE_CLAMP_POSN_ERR_IDLE                 (100     )
#define WIRE_CLAMP_POSN_ERR_VEL                  (100     )
#define WIRE_CLAMP_POSN_ERR_ACC                  (100     )
#define WIRE_CLAMP_CRITICALPOSNERR_IDLE          (1000    )
#define WIRE_CLAMP_CRITICALPOSNERR_VEL           (1000    )
#define WIRE_CLAMP_CRITICALPOSNERR_ACC           (1000    )
#define WIRE_CLAMP_DYNAMICBRAKETHRESHOLDVEL      (0       )
#define WIRE_CLAMP_MAX_VELX                      (2000000 )
#define WIRE_CLAMP_MAX_ACCX                      (10000000)
#define WIRE_CLAMP_RMS_DRVCMDX                   (40      )
#define WIRE_CLAMP_RMS_DRVCMDIDLE                (15      )
#define WIRE_CLAMP_RMS_DRVCMDMTN                 (100     )
#define WIRE_CLAMP_RMS_TIMECONST                 (200     )
#define WIRE_CLAMP_SOFTWAREPOSNLIMIT_LOW         (2E+12   )
#define WIRE_CLAMP_SOFTWAREPOSNLIMIT_UPP         (-2E+12  )


#define TABLE_X_MFLAGS                            (0x2a0000 )
#define TABLE_X_AFLAGS                            (0x0      )
#define TABLE_X_BRAKEOFF_TIME                     (50       )
#define TABLE_X_BRAKEON_TIME                      (50       )
#define TABLE_X_ENABLE_TIME                       (50       )
#define TABLE_X_ENCODER_FACTOR                    (1        )
#define TABLE_X_SETTLING_TIME                     (0        )
#define TABLE_X_TARGET_RADIX                      (1000     )
#define TABLE_X_ENCODER_FREQ                      (0        )
#define TABLE_X_ENCODER_TYPE                      (0        )

#define TABLE_Y_MFLAGS                             (0x2a3000)
#define TABLE_Y_AFLAGS                             (0x0     )
#define TABLE_Y_BRAKEOFF_TIME                      (50      )
#define TABLE_Y_BRAKEON_TIME                       (50      )
#define TABLE_Y_ENABLE_TIME                        (50      )
#define TABLE_Y_ENCODER_FACTOR                     (1       )
#define TABLE_Y_SETTLING_TIME                      (0       )
#define TABLE_Y_TARGET_RADIX                       (1000    )
#define TABLE_Y_ENCODER_FREQ                       (0       )
#define TABLE_Y_ENCODER_TYPE                       (0       )

#define BOND_Z_MFLAGS                              (0x228000)
#define BOND_Z_AFLAGS                              (0x0     )
#define BOND_Z_BRAKEOFF_TIME                       (50      )
#define BOND_Z_BRAKEON_TIME                        (50      )
#define BOND_Z_ENABLE_TIME                         (50      )
#define BOND_Z_ENCODER_FACTOR                      (1       )
#define BOND_Z_SETTLING_TIME                       (0       )
#define BOND_Z_TARGET_RADIX                        (1000    )
#define BOND_Z_ENCODER_FREQ                        (0       )
#define BOND_Z_ENCODER_TYPE                        (0       )

#define WIRE_CLAMP_MFLAGS                          (0x20000)
#define WIRE_CLAMP_AFLAGS                          (0x0    )
#define WIRE_CLAMP_BRAKEOFF_TIME                   (50     )
#define WIRE_CLAMP_BRAKEON_TIME                    (50     )
#define WIRE_CLAMP_ENABLE_TIME                     (50     )
#define WIRE_CLAMP_ENCODER_FACTOR                  (1      )
#define WIRE_CLAMP_SETTLING_TIME                   (0      )
#define WIRE_CLAMP_TARGET_RADIX                    (1      )
#define WIRE_CLAMP_ENCODER_FREQ                    (0      )
#define WIRE_CLAMP_ENCODER_TYPE                    (0      )

void mtnapi_init_def_servo_acs()
{
	// init default parameter for x
	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dPositionLoopProportionalGain      = TABLE_X_SLPKP   ;  // 70;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dVelocityLoopProportionalGain      = TABLE_X_SLVKP   ;  // 300.0;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dVelocityLoopIntegratorGain        = TABLE_X_SLVKI   ;  // 1200.0;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dVelocityLoopIntegratorLimit       = TABLE_X_SLVLI   ;  // 9.0; // in percent
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth = TABLE_X_SLVSOF  ;  // 300.;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dSecondOrderLowPassFilterDamping   = TABLE_X_SLVSOFD ;  // 0.707;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dNotchFilterFrequency              = TABLE_X_SLVNFRQ ;  // 6.0;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dNotchFilterWidth                  = TABLE_X_SLVNWID ;  // 3.0;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dNotchFilterAttenuation            = TABLE_X_SLVNATT ;  // 2.0;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dAccelerationFeedforward           = TABLE_X_SLAFF   ;  // 19000;
		stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dDynamicFrictionFeedforward        = TABLE_X_SLFRC   ;  // 0;
		//stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].iConstantDriveCommand              = 0;
		//stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dTorqueLimitIdle                   = 50; // per cent of full-DAC
		//stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dTorqueLimitMoving                 = 100;
		//stServoAxis_ACS[DEF_AXIS_X].stServoParaACS[ii].dTorqueLimitRMS                    = 80;
	}

	// init default parameter for y
	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dPositionLoopProportionalGain      = TABLE_Y_SLPKP   ;  //20;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dVelocityLoopProportionalGain      = TABLE_Y_SLVKP   ;  //300.0;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dVelocityLoopIntegratorGain        = TABLE_Y_SLVKI   ;  //1100.0;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dVelocityLoopIntegratorLimit       = TABLE_Y_SLVLI   ;  //10.0; // in percent
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth = TABLE_Y_SLVSOF  ;  //200.;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dSecondOrderLowPassFilterDamping   = TABLE_Y_SLVSOFD ;  //0.707;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dNotchFilterFrequency              = TABLE_Y_SLVNFRQ ;  //300.0;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dNotchFilterWidth                  = TABLE_Y_SLVNWID ;  //30.0;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dNotchFilterAttenuation            = TABLE_Y_SLVNATT ;  //5.0;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dAccelerationFeedforward           = TABLE_Y_SLAFF   ;  //11000;
		stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dDynamicFrictionFeedforward        = TABLE_Y_SLFRC   ;  //0;
		//stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].iConstantDriveCommand              = 0;
		//stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dTorqueLimitIdle                   = 50; // per cent of full-DAC
		//stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dTorqueLimitMoving                 = 100;
		//stServoAxis_ACS[DEF_AXIS_Y].stServoParaACS[ii].dTorqueLimitRMS                    = 80;
	}

	// init default parameter for Z
	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dPositionLoopProportionalGain      = BOND_Z_SLPKP  ; //	30;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dVelocityLoopProportionalGain      = BOND_Z_SLVKP  ; //	20.0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dVelocityLoopIntegratorGain        = BOND_Z_SLVKI  ; //	900.0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dVelocityLoopIntegratorLimit       = BOND_Z_SLVLI  ; //	2.0; // in percent
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dSecondOrderLowPassFilterBandwidth = BOND_Z_SLVSOF ; //	700.;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dSecondOrderLowPassFilterDamping   = BOND_Z_SLVSOFD; //	0.707;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dNotchFilterFrequency              = BOND_Z_SLVNFRQ; //	300.0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dNotchFilterWidth                  = BOND_Z_SLVNWID; //	30.0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dNotchFilterAttenuation            = BOND_Z_SLVNATT; //	5.0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dAccelerationFeedforward           = BOND_Z_SLAFF  ; //	0;
		stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dDynamicFrictionFeedforward        = BOND_Z_SLFRC  ; //	0;
		//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].iConstantDriveCommand              = 0;
		//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dTorqueLimitIdle                   = 80; // per cent of full-DAC
		//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dTorqueLimitMoving                 = 100;
		//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[ii].dTorqueLimitRMS                    = 80;
	}
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dPositionLoopProportionalGain      = BOND_Z_SLPKP  ; //	30;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dVelocityLoopProportionalGain      = BOND_Z_SLVKP  ; //	20.0;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dVelocityLoopIntegratorGain        = BOND_Z_SLVKI  ; //	1200.0;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dVelocityLoopIntegratorLimit       = BOND_Z_SLVLI  ; //	5.0; // in percent
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dSecondOrderLowPassFilterBandwidth = BOND_Z_SLVSOF ; //	700.;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dSecondOrderLowPassFilterDamping   = BOND_Z_SLVSOFD; //	0.707;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dNotchFilterFrequency              = BOND_Z_SLVNFRQ; //	300.0;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dNotchFilterWidth                  = BOND_Z_SLVNWID; //	30.0;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dNotchFilterAttenuation            = BOND_Z_SLVNATT; //	5.0;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dAccelerationFeedforward           = BOND_Z_SLAFF  ; //	750;
	stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dDynamicFrictionFeedforward        = BOND_Z_SLFRC  ; //	0;
	//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].iConstantDriveCommand              = 0;
	//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dTorqueLimitIdle                   = 80; // per cent of full-DAC
	//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dTorqueLimitMoving                 = 100;
	//stServoAxis_ACS[DEF_AXIS_Z].stServoParaACS[DEF_BLK_ID_SERVO_PARA_Z_SEARCH_HOME].dTorqueLimitRMS                    = 80;

	// Speed Profile
	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].dMaxVelocity		= TABLE_X_MAX_VEL      ;
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].dMaxAcceleration	= TABLE_X_MAX_ACC      ;
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].dMaxDeceleration	= TABLE_X_MAX_DEC      ;
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].dMaxKillDeceleration = TABLE_X_MAX_KILL_DEC ;
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].dMaxJerk             = TABLE_X_MAX_JERK      ;
		stServoAxis_ACS[DEF_AXIS_X].stSpeedProfile[ii].uiProfileType        = TABLE_X_PROF_TYPE    ;
	}	

	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].dMaxVelocity		= TABLE_Y_MAX_VEL       ;                  
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].dMaxAcceleration	= TABLE_Y_MAX_ACC       ;                  
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].dMaxDeceleration	= TABLE_Y_MAX_DEC       ;                  
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].dMaxKillDeceleration = TABLE_Y_MAX_KILL_DEC  ;                  
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].dMaxJerk             = TABLE_Y_MAX_JERK      ;
		stServoAxis_ACS[DEF_AXIS_Y].stSpeedProfile[ii].uiProfileType        = TABLE_Y_PROF_TYPE     ;
	}

	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].dMaxVelocity		= BOND_Z_MAX_VEL       ;
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].dMaxAcceleration	= BOND_Z_MAX_ACC       ;
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].dMaxDeceleration	= BOND_Z_MAX_DEC       ;
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].dMaxKillDeceleration = BOND_Z_MAX_KILL_DEC  ;
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].dMaxJerk             = BOND_Z_MAX_JERK      ;
		stServoAxis_ACS[DEF_AXIS_Z].stSpeedProfile[ii].uiProfileType        = BOND_Z_PROF_TYPE     ;
	}

	for(int ii= 0; ii<MAX_BLK_PARAMETER; ii++)
	{
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].dMaxVelocity		= WIRE_CLAMP_MAX_VEL     ;
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].dMaxAcceleration	= WIRE_CLAMP_MAX_ACC     ;
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].dMaxDeceleration	= WIRE_CLAMP_MAX_DEC     ;
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].dMaxKillDeceleration = WIRE_CLAMP_MAX_KILL_DEC;
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].dMaxJerk             = WIRE_CLAMP_MAX_JERK    ;
		stServoAxis_ACS[DEF_AXIS_W].stSpeedProfile[ii].uiProfileType        = WIRE_CLAMP_PROF_TYPE   ;
	}	
	// Safety Parameter
	stSafetyParaAxis_ACS[DEF_AXIS_X].dPosnErrIdle			= TABLE_X_POSN_ERR_IDLE            ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dPosnErrVel			= TABLE_X_POSN_ERR_VEL             ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dPosnErrAcc			= TABLE_X_POSN_ERR_ACC             ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dCriticalPosnErrIdle		= TABLE_X_CRITICALPOSNERR_IDLE     ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dCriticalPosnErrVel		= TABLE_X_CRITICALPOSNERR_VEL      ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dCriticalPosnErrAcc		= TABLE_X_CRITICALPOSNERR_ACC      ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dDynamicBrakeThresholdVel	= TABLE_X_DYNAMICBRAKETHRESHOLDVEL ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dMaxVelX			= TABLE_X_MAX_VELX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dMaxAccX			= TABLE_X_MAX_ACCX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dRMS_DrvCmdX			= TABLE_X_RMS_DRVCMDX              ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dRMS_DrvCmdIdle		= TABLE_X_RMS_DRVCMDIDLE           ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dRMS_DrvCmdMtn			= TABLE_X_RMS_DRVCMDMTN            ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dRMS_TimeConst			= TABLE_X_RMS_TIMECONST            ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dSoftwarePosnLimitLow		= TABLE_X_SOFTWAREPOSNLIMIT_LOW    ;
	stSafetyParaAxis_ACS[DEF_AXIS_X].dSoftwarePosnLimitUpp		= TABLE_X_SOFTWAREPOSNLIMIT_UPP    ;
	 
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dPosnErrIdle			= TABLE_Y_POSN_ERR_IDLE            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dPosnErrVel			= TABLE_Y_POSN_ERR_VEL             ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dPosnErrAcc			= TABLE_Y_POSN_ERR_ACC             ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dCriticalPosnErrIdle		= TABLE_Y_CRITICALPOSNERR_IDLE     ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dCriticalPosnErrVel		= TABLE_Y_CRITICALPOSNERR_VEL      ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dCriticalPosnErrAcc		= TABLE_Y_CRITICALPOSNERR_ACC      ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dDynamicBrakeThresholdVel	= TABLE_Y_DYNAMICBRAKETHRESHOLDVEL ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dMaxVelX			= TABLE_Y_MAX_VELX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dMaxAccX			= TABLE_Y_MAX_ACCX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dRMS_DrvCmdX			= TABLE_Y_RMS_DRVCMDX              ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dRMS_DrvCmdIdle		= TABLE_Y_RMS_DRVCMDIDLE           ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dRMS_DrvCmdMtn			= TABLE_Y_RMS_DRVCMDMTN            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dRMS_TimeConst			= TABLE_Y_RMS_TIMECONST            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dSoftwarePosnLimitLow		= TABLE_Y_SOFTWAREPOSNLIMIT_LOW    ;
	stSafetyParaAxis_ACS[DEF_AXIS_Y].dSoftwarePosnLimitUpp		= TABLE_Y_SOFTWAREPOSNLIMIT_UPP    ;

	stSafetyParaAxis_ACS[DEF_AXIS_Z].dPosnErrIdle			= BOND_Z_POSN_ERR_IDLE            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dPosnErrVel			= BOND_Z_POSN_ERR_VEL             ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dPosnErrAcc			= BOND_Z_POSN_ERR_ACC             ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dCriticalPosnErrIdle		= BOND_Z_CRITICALPOSNERR_IDLE     ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dCriticalPosnErrVel		= BOND_Z_CRITICALPOSNERR_VEL      ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dCriticalPosnErrAcc		= BOND_Z_CRITICALPOSNERR_ACC      ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dDynamicBrakeThresholdVel	= BOND_Z_DYNAMICBRAKETHRESHOLDVEL ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dMaxVelX			= BOND_Z_MAX_VELX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dMaxAccX			= BOND_Z_MAX_ACCX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dRMS_DrvCmdX			= BOND_Z_RMS_DRVCMDX              ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dRMS_DrvCmdIdle		= BOND_Z_RMS_DRVCMDIDLE           ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dRMS_DrvCmdMtn			= BOND_Z_RMS_DRVCMDMTN            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dRMS_TimeConst			= BOND_Z_RMS_TIMECONST            ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dSoftwarePosnLimitLow		= BOND_Z_SOFTWAREPOSNLIMIT_LOW    ;
	stSafetyParaAxis_ACS[DEF_AXIS_Z].dSoftwarePosnLimitUpp		= BOND_Z_SOFTWAREPOSNLIMIT_UPP    ;

	stSafetyParaAxis_ACS[DEF_AXIS_W].dPosnErrIdle			= WIRE_CLAMP_POSN_ERR_IDLE            ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dPosnErrVel			= WIRE_CLAMP_POSN_ERR_VEL             ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dPosnErrAcc			= WIRE_CLAMP_POSN_ERR_ACC             ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dCriticalPosnErrIdle		= WIRE_CLAMP_CRITICALPOSNERR_IDLE     ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dCriticalPosnErrVel		= WIRE_CLAMP_CRITICALPOSNERR_VEL      ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dCriticalPosnErrAcc		= WIRE_CLAMP_CRITICALPOSNERR_ACC      ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dDynamicBrakeThresholdVel	= WIRE_CLAMP_DYNAMICBRAKETHRESHOLDVEL ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dMaxVelX			= WIRE_CLAMP_MAX_VELX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dMaxAccX			= WIRE_CLAMP_MAX_ACCX                 ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dRMS_DrvCmdX			= WIRE_CLAMP_RMS_DRVCMDX              ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dRMS_DrvCmdIdle		= WIRE_CLAMP_RMS_DRVCMDIDLE           ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dRMS_DrvCmdMtn			= WIRE_CLAMP_RMS_DRVCMDMTN            ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dRMS_TimeConst			= WIRE_CLAMP_RMS_TIMECONST            ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dSoftwarePosnLimitLow		= WIRE_CLAMP_SOFTWAREPOSNLIMIT_LOW    ;
	stSafetyParaAxis_ACS[DEF_AXIS_W].dSoftwarePosnLimitUpp		= WIRE_CLAMP_SOFTWAREPOSNLIMIT_UPP    ;
	 
	 //Basic Parameters             
	stBasicParaAxis_ACS[DEF_AXIS_X].iMotorFlags    = TABLE_X_MFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_X].iAxisFlags     = TABLE_X_AFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dBrakeOffTime  = TABLE_X_BRAKEOFF_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dBrakeOnTime   = TABLE_X_BRAKEON_TIME   ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dEnableTime    = TABLE_X_ENABLE_TIME    ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dEncoderFactor = TABLE_X_ENCODER_FACTOR ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dSettlingTime  = TABLE_X_SETTLING_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_X].dTargetRadix   = TABLE_X_TARGET_RADIX   ;
	stBasicParaAxis_ACS[DEF_AXIS_X].iEncoderFreq   = TABLE_X_ENCODER_FREQ   ;
	stBasicParaAxis_ACS[DEF_AXIS_X].iEncoderType   = TABLE_X_ENCODER_TYPE   ;

	stBasicParaAxis_ACS[DEF_AXIS_Y].iMotorFlags    = TABLE_Y_MFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].iAxisFlags     = TABLE_Y_AFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dBrakeOffTime  = TABLE_Y_BRAKEOFF_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dBrakeOnTime   = TABLE_Y_BRAKEON_TIME   ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dEnableTime    = TABLE_Y_ENABLE_TIME    ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dEncoderFactor = TABLE_Y_ENCODER_FACTOR ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dSettlingTime  = TABLE_Y_SETTLING_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].dTargetRadix   = TABLE_Y_TARGET_RADIX   ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].iEncoderFreq   = TABLE_Y_ENCODER_FREQ   ;
	stBasicParaAxis_ACS[DEF_AXIS_Y].iEncoderType   = TABLE_Y_ENCODER_TYPE   ;

	stBasicParaAxis_ACS[DEF_AXIS_Z].iMotorFlags    = BOND_Z_MFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].iAxisFlags     = BOND_Z_AFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dBrakeOffTime  = BOND_Z_BRAKEOFF_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dBrakeOnTime   = BOND_Z_BRAKEON_TIME   ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dEnableTime    = BOND_Z_ENABLE_TIME    ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dEncoderFactor = BOND_Z_ENCODER_FACTOR ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dSettlingTime  = BOND_Z_SETTLING_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].dTargetRadix   = BOND_Z_TARGET_RADIX   ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].iEncoderFreq   = BOND_Z_ENCODER_FREQ   ;
	stBasicParaAxis_ACS[DEF_AXIS_Z].iEncoderType   = BOND_Z_ENCODER_TYPE   ;

	stBasicParaAxis_ACS[DEF_AXIS_W].iMotorFlags    = WIRE_CLAMP_MFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_W].iAxisFlags     = WIRE_CLAMP_AFLAGS         ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dBrakeOffTime  = WIRE_CLAMP_BRAKEOFF_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dBrakeOnTime   = WIRE_CLAMP_BRAKEON_TIME   ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dEnableTime    = WIRE_CLAMP_ENABLE_TIME    ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dEncoderFactor = WIRE_CLAMP_ENCODER_FACTOR ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dSettlingTime  = WIRE_CLAMP_SETTLING_TIME  ;
	stBasicParaAxis_ACS[DEF_AXIS_W].dTargetRadix   = WIRE_CLAMP_TARGET_RADIX   ;
	stBasicParaAxis_ACS[DEF_AXIS_W].iEncoderFreq   = WIRE_CLAMP_ENCODER_FREQ   ;
	stBasicParaAxis_ACS[DEF_AXIS_W].iEncoderType   = WIRE_CLAMP_ENCODER_TYPE   ;
 
}       

