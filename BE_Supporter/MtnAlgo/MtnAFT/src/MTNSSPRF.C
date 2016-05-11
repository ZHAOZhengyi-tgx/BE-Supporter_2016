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

//All the codes in the software are subject to the GNU General Public License("GPL") or 
//GNU Lesser General Public License("LGPL").
//Please see the GNU and LPGL Web sites to view the terms of each license.
/***************************************************************************
 * Filename:     MtnSSPrf.c
 * Programmer:   Zhao ZhengYi
 * Date:         1 Feb, 2006
 * Description : Super Sine Profile
 * History:
 * Date       Author    Notes
 * =========================================================================
 * 20060620   ZhengYi   Move file handler functions to mtngpf.c
 ****************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <math.h>

//#include "mtrintrf.h"
#include "ss_prof.h"
//#include "mtn_def.h"
//#include "mtrconfg.h"
#include "MtnSsPrf.h"

#define STR_CONFIG_SS_PROF_BLK    ("PROF_SS_CMD")

extern double pfProfDist[MAX_ARBITRARY_PROF];
extern double pfProfVel[MAX_ARBITRARY_PROF];
extern double pfProfAcc[MAX_ARBITRARY_PROF];
extern double pfProfJerk[MAX_ARBITRARY_PROF];

#ifdef A_MPI_
extern short mpi_set_posn_trajectory_2( MPI_MCB *const stpMotor,
                               unsigned short usStartIdx,
                               unsigned short uLength,
                               float *pfTraj);
extern short mpi_set_vel_trajectory_2( MPI_MCB *const stpMotor,
                               unsigned short usStartIdx,
                               unsigned short uLength,
                               float *pfTraj);
extern short mpi_set_acc_trajectory_2( MPI_MCB *const stpMotor,
                               unsigned short usStartIdx,
                               unsigned short uLength,
                               float *pfTraj);
extern short mpi_set_jerk_trajectory_2( MPI_MCB *const stpMotor,
                               unsigned short usStartIdx,
                               unsigned short uLength,
                               float *pfTraj);                               
extern short mpi_vaj_traj_move_2_srch_dyn_posn(MPI_MCB *const stpMotor,
                                 unsigned short uStartIndex,
                                 unsigned short uTotalTime,
                                 unsigned short uPosnTol,
                                 unsigned short uProcessBlk );
#endif // A_MPI_

static short sDebugLevel;
#define EPSLN        (1E-10)          //22102004

void __ss_prof_set_debug_level(short sLevel)
{
    sDebugLevel = sLevel;
}

#ifdef  __NOT_DEF__SS_PROF__
double fsign(double fIn)
{
	if(fIn >EPSLN )
	{
		return 1.0;
	}
	else if(fIn <-EPSLN )
	{
		return -1.0;
	}
	else
	{
		return 0.0;
	}
}
#endif // __NOT_DEF__SS_PROF__

static union
{
	CALC_SS_PROF_PRECALC_OUTPUT stSsProfOutput;
	CALC_SS_PROF_CMDGEN_INPUT stSsCmdGenInput;
}stPrecOutCmdGenIn[__MAX_SS_PROF_BLK__];

CALC_SS_PROF_PRECALC_INPUT stPrecalcIn[__MAX_SS_PROF_BLK__];
static CALC_SS_PROF_CMDGEN_OUTPUT stSsCmdGenOutput[__MAX_SS_PROF_BLK__];


double __ss_prof_get_dist_at_blk(unsigned short usBlkNo)
{
	if(usBlkNo <= __MAX_SS_PROF_BLK__)
	{
		return(stPrecalcIn[usBlkNo].fDist);
	}
	else
	{
		return 0;
	}
}

#include "mtnconfg.h" 

MTN_SHORT __init_ss_prof_blk_from_file__(char *strProfSsFile, unsigned short *pusTotalBlk)
{
short sMtnRet;	
char  strBuffer[BUFSIZ+1];
short ii;
short stat= 0;
static char strCmdID[64];
//static unsigned short usErrorCode, jj;
static float f01, f02, f03, f04, f05, f06;

    stat |= mtn_cfg_OpenConfigFile(strProfSsFile);  // mpi_cfg_OpenConfigFile
    sMtnRet = MTN_API_OK_ZERO;

    if(stat != OPENERR)
    {
        sMtnRet = MTN_API_OK_ZERO;
        stat |= mtn_cfg_ReadConfigString("SS_PROF_CONFIG", "TOTALMOTION", &strBuffer[0]); // mpi_cfg_ReadConfigString
        if (stat == MTN_API_OK_ZERO)
        {

            sscanf(strBuffer, "%d", pusTotalBlk);
            if(sDebugLevel >= DEBUG_LEVEL_1) //10052005
            {
                printf("%s", strBuffer);
                printf("Total Command: %d\n", (*pusTotalBlk));
//                getch();
            }
            if((*pusTotalBlk) > __MAX_SS_PROF_BLK__)
            {
            	sMtnRet = MTN_API_ERROR;
            }
            ii = 0;
            
            while(ii < (*pusTotalBlk))
            {
                sprintf(strCmdID, "SS_PROF_BLK_%d", ii);
                stat |= mtn_cfg_ReadConfigString(STR_CONFIG_SS_PROF_BLK, strCmdID, &strBuffer[0]); // mpi_cfg_ReadConfigString
                if (stat == MTN_API_OK_ZERO)
                {
                    if(sDebugLevel >= DEBUG_LEVEL_1) //10052005
                    {
                        printf("%s", strBuffer);
                    }
                    
                    sscanf(strBuffer, "%f,%f,%f,%f,%f,%f",
                    &f01, &f02, &f03, &f04, &f05, &f06);
                    
                    stPrecalcIn[ii].fDist  = floor(f01); 
                    stPrecalcIn[ii].fT_cnst= floor(f02);
                    stPrecalcIn[ii].fV_ini = f03;
                    stPrecalcIn[ii].fMaxV  = f04; 
                    stPrecalcIn[ii].fMaxA  = f05; 
                    stPrecalcIn[ii].fV_end = f06;
                }
                if( sDebugLevel >= DEBUG_LEVEL_1)
                {                
                    printf("Blk [%d]: %f, %f, %f, %f, %f, %f\n", 
                                ii,
                                stPrecalcIn[ii].fDist  ,
                                stPrecalcIn[ii].fT_cnst,
                                stPrecalcIn[ii].fV_ini ,
                                stPrecalcIn[ii].fMaxV  ,
                                stPrecalcIn[ii].fMaxA  ,
                                stPrecalcIn[ii].fV_end );
                }
                ii ++;
            }
            if( sDebugLevel >= DEBUG_LEVEL_1)
            {
//            	getch();
            }
        }                       
        mtn_cfg_CloseConfigFile();      // Close Filempi_cfg_CloseConfigFile
                                
    }
    else
    {
        if(sDebugLevel >= DEBUG_LEVEL_1) //10052005
        {
            printf("MotionJobList FileName: %s, FileOpen %d, OPENERR =%d Hardware =%d\n",
                strProfSsFile, stat, OPENERR, sDebugLevel);
        }
//        getch();
        sMtnRet = MTN_API_ERROR; // Open File Error
    }

    return sMtnRet;

}

MTN_SHORT __init_continuous_ss_prof_cmd__(short usTotalBlk, short sDebugFlag, unsigned short *pusTotalMotionSample)
{
unsigned short usTotalMovingSample, usCurrBlkSample;	
short ii, jj, sMtnRet = MTN_API_OK_ZERO;	
float fCurrTravDist;
    usTotalMovingSample = 0;
    for(ii = 0; ii<usTotalBlk; ii++)
    {
    	if(ss_prof_precalc(&stPrecalcIn[ii],
		               &stPrecOutCmdGenIn[ii].stSsProfOutput) == MTN_CALC_ERROR)
		{
		    fprintf(stderr, "\nCurrCmd: %d, Error Code: %d", ii, stPrecOutCmdGenIn[ii].stSsProfOutput.sErrorCode);
//		    getch();
		    sMtnRet |= MTN_API_ERROR;
		}
		usTotalMovingSample += (unsigned short)stPrecOutCmdGenIn[ii].stSsCmdGenInput.fT_total;
		
	}
	if(usTotalMovingSample + 10 > MAX_ARBITRARY_PROF)
	{
		fprintf(stderr, "\nViolate the total sample constraint %f ", usTotalMovingSample);
//		getch();
	    sMtnRet |= MTN_API_ERROR;
	}
	
	usCurrBlkSample = 0;
	if(sMtnRet == MTN_API_OK_ZERO)
	{
		fCurrTravDist = 0;
	    for(ii = 0; ii<usTotalBlk; ii++)
	    {
			stSsCmdGenOutput[ii].pfdist_prof = &pfProfDist[usCurrBlkSample];
			stSsCmdGenOutput[ii].pfvel_prof =  &pfProfVel [usCurrBlkSample];
			stSsCmdGenOutput[ii].pfacc_prof =  &pfProfAcc [usCurrBlkSample];
			stSsCmdGenOutput[ii].pfjerk_prof = &pfProfJerk[usCurrBlkSample];
	
			ss_prof_cmdgen(&stPrecOutCmdGenIn[ii].stSsCmdGenInput, &stSsCmdGenOutput[ii]);
			
		    if( sDebugFlag == 1)
		    {
				for(jj=0; jj<= 10; jj++)
				{
					fprintf(stderr, "Dist %d, %d, %f",
					   ii, jj, stSsCmdGenOutput[ii].pfdist_prof[jj]);
			    }
//			    getch();
			}
			for(jj = 0; jj<stPrecOutCmdGenIn[ii].stSsCmdGenInput.fT_total; jj++)
			{
					stSsCmdGenOutput[ii].pfdist_prof[jj] = stSsCmdGenOutput[ii].pfdist_prof[jj] + fCurrTravDist;
			}

		    if( sDebugFlag == 1)
		    {
				for(jj=0; jj<= 10; jj++)
				{
					fprintf(stderr, "\nDist %d, %d, %f",
					   ii, jj, stSsCmdGenOutput[ii].pfdist_prof[jj]);
			    }
//			    getch();
			}
			
			usCurrBlkSample = usCurrBlkSample + (unsigned short)stPrecOutCmdGenIn[ii].stSsCmdGenInput.fT_total;
			fCurrTravDist = fCurrTravDist + pfProfDist[usCurrBlkSample];
	        if( sDebugFlag == 1)
	        {
				fprintf(stderr, "\nusCurrBlkSample: %d,  %f", 
				        usCurrBlkSample, stPrecOutCmdGenIn[ii].stSsCmdGenInput.fT_total);
				fprintf(stderr, "\nfCurrTravDist: %f, Added %f",
				        fCurrTravDist, pfProfDist[usCurrBlkSample]);
				
	//			getch();        
			}
		}
		pfProfDist[usCurrBlkSample] = fCurrTravDist;
		pfProfVel[usCurrBlkSample] = 0;
		pfProfAcc[usCurrBlkSample] = 0;
		pfProfJerk[usCurrBlkSample]= 0;
    	(*pusTotalMotionSample) = usCurrBlkSample;

	}
	else
	{
		(*pusTotalMotionSample) = 0;
	}
	
	return sMtnRet;
	
}

#ifdef A_MPI_
MTN_SHORT mtn_mpi_ss_prof_setup_traj(MPI_MCB *stpMotor, unsigned short usCurrBlkSample)
{
	MTN_SHORT sMtnRet = MTN_API_OK_ZERO;	
	
	mpi_set_posn_trajectory(stpMotor, usCurrBlkSample + 1, pfProfDist);
	mpi_set_vel_trajectory(stpMotor, usCurrBlkSample + 1, pfProfVel);
	mpi_set_acc_trajectory(stpMotor, usCurrBlkSample + 1, pfProfAcc);
	mpi_set_jerk_trajectory(stpMotor, usCurrBlkSample + 1, pfProfJerk);
	return sMtnRet;
}
#endif // A_MPI_

#ifdef A_MPI_
MTN_SHORT mtn_mpi_ss_prof_trigger_move_once(MPI_MCB *stpMotor, 
                                        unsigned short usCurrBlkSample, 
                                        unsigned short uPosnTol,
                                        unsigned short uProcessBlk)
{
	short sMtnRet = MTN_API_OK_ZERO;
	
	mpi_setup_dsp_scope(stpMotor, DSP_SCP_MODE_ONE_SHOT);
	mpi_vaj_traj_move_srch_dyn_posn(stpMotor,
	     usCurrBlkSample + 1,
	     uPosnTol,
	     uProcessBlk );
	     
	if(mpi_sync_motion(stpMotor) != MTR_MBI_OKEY)
	{
		sMtnRet = MTN_API_ERROR;
	}

	return sMtnRet;
}
#endif // A_MPI_

#ifdef A_MPI_
MTN_SHORT mtn_mpi_ss_prof_trigger_move_twice(MPI_MCB *stpMotor, 
                                        unsigned short usCurrBlkSample, 
                                        unsigned short uPosnTol,
                                        unsigned short uProcessBlk)
{
	short sMtnRet = MTN_API_OK_ZERO;
	
	

	mpi_set_posn_trajectory(stpMotor, usCurrBlkSample + 1, pfProfDist);
	mpi_set_vel_trajectory(stpMotor, usCurrBlkSample + 1, pfProfVel);
	mpi_set_acc_trajectory(stpMotor, usCurrBlkSample + 1, pfProfAcc);
	mpi_set_jerk_trajectory(stpMotor, usCurrBlkSample + 1, pfProfJerk);

    if(mpi_sync_motion(stpMotor) != MTR_MBI_OKEY)
	{
		sMtnRet = MTN_API_ERROR;
	}
	if(sMtnRet == MTN_API_OK_ZERO)
	{
	    mpi_vaj_traj_move_srch_dyn_posn(stpMotor,
	        usCurrBlkSample + 1,
	        uPosnTol, uProcessBlk );
	}
    mpi_set_posn_trajectory_2(stpMotor, usCurrBlkSample + 5, usCurrBlkSample + 1, pfProfDist);
    mpi_set_vel_trajectory_2(stpMotor, usCurrBlkSample + 5, usCurrBlkSample + 1, pfProfVel);
    mpi_set_acc_trajectory_2(stpMotor, usCurrBlkSample + 5, usCurrBlkSample + 1, pfProfAcc);
    mpi_set_jerk_trajectory_2(stpMotor, usCurrBlkSample + 5, usCurrBlkSample + 1, pfProfJerk);

    if(mpi_sync_motion(stpMotor) != MTR_MBI_OKEY)
	{
		sMtnRet = MTN_API_ERROR;
	}
	if(sMtnRet == MTN_API_OK_ZERO)
	{
        mpi_vaj_traj_move_2_srch_dyn_posn(stpMotor,
             usCurrBlkSample + 5,
             usCurrBlkSample + 1,
	         uPosnTol, uProcessBlk );
	}

	return sMtnRet;

}
#endif // A_MPI_

