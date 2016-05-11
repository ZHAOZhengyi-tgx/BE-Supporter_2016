
// Variables for Random Burn-in Test
#define  AXIS_BONDING_CONTROL_IN_BURN_IN    2

#define  MAX_AXIS_ENVOLVE_BURN_IN_TEST      4
#define  NUM_POINTS_PER_CYCLE_RAND_BURN_IN      100
#define  TOTAL_NUM_CYCLE_RAND_BURN_IN           1

static BOOL abQcBurnInAxisEnvolveFlag[MAX_AXIS_ENVOLVE_BURN_IN_TEST];
static short asQcBurnInAxisIdAcs[MAX_AXIS_ENVOLVE_BURN_IN_TEST];
static int aiRandBurnInAxisUpperLimit[MAX_AXIS_ENVOLVE_BURN_IN_TEST];
static int aiRandBurnInAxisLowerLimit[MAX_AXIS_ENVOLVE_BURN_IN_TEST];
static unsigned short usRandBurnInNumPointsPerCycle;
static unsigned int uiCurrBurnInCycle, uiRandBurnInTotalNumCycle;
static unsigned int uiTotalNumCycleBurnIn;
static char cFlagInitRandBurnInDefPara;
static int aiTargetPointRandBurnInPerCycle[MAX_AXIS_ENVOLVE_BURN_IN_TEST][USHRT_MAX];

void mtn_update_burn_in_position_limit_wb_axis(int iWbAxis, int iLowerLimit, int iUpperLimit)
{
	aiRandBurnInAxisLowerLimit[iWbAxis] = iLowerLimit;
	aiRandBurnInAxisUpperLimit[iWbAxis] = iUpperLimit;
}

//// // 2010Feb05
static	char cFlagStartCollectingData = 0;
void mtn_move_test_set_flag_collecting_data()
{
    cFlagStartCollectingData = 1;
}

void mtn_move_test_clear_flag_collecting_data()
{
    cFlagStartCollectingData = 0;
}

void aft_rand_burn_in_init_def_var()
{
	abQcBurnInAxisEnvolveFlag[0] = TRUE;
	abQcBurnInAxisEnvolveFlag[1] = TRUE;
	abQcBurnInAxisEnvolveFlag[2] = FALSE;
	abQcBurnInAxisEnvolveFlag[3] = FALSE;

	asQcBurnInAxisIdAcs[0] = APP_X_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[1] = APP_Y_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
	asQcBurnInAxisIdAcs[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis;

	aiRandBurnInAxisUpperLimit[0] = EFSIKA_X_TABLE_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[2] = (int)mtn_wb_get_bh_upper_limit_position();  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[3] = 0;

	aiRandBurnInAxisLowerLimit[0] = EFSIKA_X_TABLE_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[2] = (int)mtn_wb_get_bh_upper_limit_position() - 3000;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[3] = 0;

	usRandBurnInNumPointsPerCycle = NUM_POINTS_PER_CYCLE_RAND_BURN_IN;
	uiRandBurnInTotalNumCycle = TOTAL_NUM_CYCLE_RAND_BURN_IN;

}

void aft_rand_burn_in_init_def_var_1cup_v_led_bonder()
{
	abQcBurnInAxisEnvolveFlag[0] = TRUE;
	abQcBurnInAxisEnvolveFlag[1] = TRUE;
	abQcBurnInAxisEnvolveFlag[2] = FALSE;
	abQcBurnInAxisEnvolveFlag[3] = FALSE;

	asQcBurnInAxisIdAcs[0] = APP_X_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[1] = APP_Y_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
	asQcBurnInAxisIdAcs[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis;

	aiRandBurnInAxisUpperLimit[0] = EFSIKA_X_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	aiRandBurnInAxisUpperLimit[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[2] = (int)mtn_wb_get_bh_upper_limit_position();  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[3] = 0;

	aiRandBurnInAxisLowerLimit[0] = EFSIKA_X_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	aiRandBurnInAxisLowerLimit[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[2] = (int)mtn_wb_get_bh_upper_limit_position() - 3000;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[3] = 0;

	usRandBurnInNumPointsPerCycle = NUM_POINTS_PER_CYCLE_RAND_BURN_IN;
	uiRandBurnInTotalNumCycle = TOTAL_NUM_CYCLE_RAND_BURN_IN;

}

void aft_rand_burn_in_init_def_var_hori_led_bonder()
{
	abQcBurnInAxisEnvolveFlag[0] = TRUE;
	abQcBurnInAxisEnvolveFlag[1] = TRUE;
	abQcBurnInAxisEnvolveFlag[2] = FALSE;
	abQcBurnInAxisEnvolveFlag[3] = FALSE;

	asQcBurnInAxisIdAcs[0] = APP_Y_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[1] = APP_X_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
	asQcBurnInAxisIdAcs[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis;

	aiRandBurnInAxisUpperLimit[0] = EFSIKA_X_TABLE_UPP_LIMIT_HORI_BONDER_CNT;
	aiRandBurnInAxisUpperLimit[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[2] = (int)mtn_wb_get_bh_upper_limit_position() - 2000;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED;  // 20110625
	aiRandBurnInAxisUpperLimit[3] = 0;

	aiRandBurnInAxisLowerLimit[0] = EFSIKA_X_TABLE_LOW_LIMIT_HORI_BONDER_CNT;
	aiRandBurnInAxisLowerLimit[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[2] = (int)mtn_wb_get_bh_upper_limit_position() - 4000;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;   // 20110625
	aiRandBurnInAxisLowerLimit[3] = 0;

	usRandBurnInNumPointsPerCycle = NUM_POINTS_PER_CYCLE_RAND_BURN_IN;
	uiRandBurnInTotalNumCycle = TOTAL_NUM_CYCLE_RAND_BURN_IN;
}

void aft_rand_burn_in_init_def_var_1_cup_vled_bonder()
{
	abQcBurnInAxisEnvolveFlag[0] = TRUE;
	abQcBurnInAxisEnvolveFlag[1] = TRUE;
	abQcBurnInAxisEnvolveFlag[2] = FALSE;
	abQcBurnInAxisEnvolveFlag[3] = FALSE;

	asQcBurnInAxisIdAcs[0] = APP_X_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[1] = APP_Y_TABLE_ACS_ID;
	asQcBurnInAxisIdAcs[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
	asQcBurnInAxisIdAcs[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis;

	aiRandBurnInAxisUpperLimit[0] = EFSIKA_X_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	aiRandBurnInAxisUpperLimit[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[2] = (int)mtn_wb_get_bh_upper_limit_position();  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT;
	aiRandBurnInAxisUpperLimit[3] = 0;

	aiRandBurnInAxisLowerLimit[0] = EFSIKA_X_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	aiRandBurnInAxisLowerLimit[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[2] = (int)mtn_wb_get_bh_upper_limit_position() - 3000;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT;
	aiRandBurnInAxisLowerLimit[3] = 0;

	usRandBurnInNumPointsPerCycle = NUM_POINTS_PER_CYCLE_RAND_BURN_IN;
	uiRandBurnInTotalNumCycle = TOTAL_NUM_CYCLE_RAND_BURN_IN;

}
void aft_rand_burn_in_init_target_point_per_cycle(unsigned int uiRandSeed)
{
	int ii, jj;
	double dUppLimit, dLowLimit;

	srand(uiRandSeed);

	for(jj = 0; jj<MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
	{
		if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD))
		{
			if(aiRandBurnInAxisUpperLimit[jj] < aiRandBurnInAxisLowerLimit[jj])
			{
				dUppLimit = (double)aiRandBurnInAxisLowerLimit[jj];
				dLowLimit = (double)aiRandBurnInAxisUpperLimit[jj];
			}
			else
			{
				dLowLimit = (double)aiRandBurnInAxisLowerLimit[jj];
				dUppLimit = (double)aiRandBurnInAxisUpperLimit[jj];
			}
			for(ii = 0; ii<usRandBurnInNumPointsPerCycle; ii++)
			{
				aiTargetPointRandBurnInPerCycle[jj][ii] = (int)(((double)rand())/RAND_MAX * (dUppLimit - dLowLimit) + dLowLimit);
			}
		}
	}
}

// Variables and function related with fix burn in
#define MAX_PATTERN_SIDES      3
static double dFixBurnInWireLength_mm;
static double dFixBurnInPitchPerUnit_mm;
static unsigned short usFixBurnInNumUnitsPerSide, usNumSide[MAX_PATTERN_SIDES], usSidePattern;
static BOOL bFixBurnInFlagIsClockWise;
static unsigned short usActualNumPointsPerCycle_FixBurnIn;
void aft_fix_burn_in_init_def_var()
{
	dFixBurnInWireLength_mm = 1;
	dFixBurnInPitchPerUnit_mm = 3;
	usNumSide[0] = 1;
	usNumSide[1] = 2;
	usNumSide[2] = 4;
	usSidePattern = 2;
	usFixBurnInNumUnitsPerSide = 1;
	bFixBurnInFlagIsClockWise = TRUE;
}

void mtn_move_test_config_burn_in_condition()
{
	if(get_sys_machine_type_flag() == WB_MACH_TYPE_VLED_FORK)
	{
		aft_rand_burn_in_init_def_var();
		aft_fix_burn_in_init_def_var();
	}
	else if(get_sys_machine_type_flag() == WB_MACH_TYPE_HORI_LED)
	{
		aft_rand_burn_in_init_def_var_hori_led_bonder();
		aft_fix_burn_in_init_def_var();
	}
	else
	{
		aft_rand_burn_in_init_def_var_1_cup_vled_bonder();
		aft_fix_burn_in_init_def_var();
	}
}

void aft_fix_burn_in_calc_target_point_per_cycle_1_side()
{
	if(bFixBurnInFlagIsClockWise)
	{ // Y-LowLimit, From Right to Left, X-LowLimit to X-UppLimit
	}
	else
	{ // Y-LowLimit, From Left to Right, X-UppLimit to X-LowLimit
	}
}


void aft_fix_burn_in_calc_target_point_per_cycle_2_side()
{
	// Machine Front Side
	if(bFixBurnInFlagIsClockWise)
	{ // Y-LowLimit, From Right to Left, X-LowLimit to X-UppLimit
	}
	else
	{ // Y-LowLimit, From Left to Right, X-UppLimit to X-LowLimit
	}

	// Machine Rear Side
	if(bFixBurnInFlagIsClockWise)
	{ // Y-UppLimit, From Left to Right, X-UppLimit to X-LowLimit
	}
	else
	{ // Y-UppLimit, From Right to Left, X-LowLimit to X-UppLimit
	}

}

void aft_fix_burn_in_calc_target_point_per_cycle_4_side()
{
	int iDistWireLength_cnt;
	iDistWireLength_cnt = (int)(fabs(dFixBurnInWireLength_mm) * 1000 / X_ENC_UNIT_UM);

	int ii;
	int iDistPitch_X_cnt = (int)(fabs((double)(aiRandBurnInAxisUpperLimit[0] - aiRandBurnInAxisLowerLimit[0])) / (usFixBurnInNumUnitsPerSide-1) );
	int iDistPitch_Y_cnt; 
	iDistPitch_Y_cnt = (int)(fabs((double)(aiRandBurnInAxisUpperLimit[1] - aiRandBurnInAxisLowerLimit[1])) / (usFixBurnInNumUnitsPerSide-1) + 0.5);

	int iUnitStartPosn_X, iUnitStartPosn_Y, iUnitStartPosn_Z;
	int idxPoint = 0;
	if(bFixBurnInFlagIsClockWise)
	{ 	// CW-1: Machine Front Side
		// Y-LowLimit, From Right to Left, X-LowLimit to X-UppLimit, Wire Dir in Y
		iUnitStartPosn_X = aiRandBurnInAxisLowerLimit[0];
		iUnitStartPosn_Y = aiRandBurnInAxisLowerLimit[1] + iDistWireLength_cnt;
		iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];

		for(ii = 0; ii < usFixBurnInNumUnitsPerSide; ii++)
		{
			// First Point in a unit, FireLevel
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = iUnitStartPosn_Z;

			// 2nd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// 3rd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y - iDistWireLength_cnt;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// Update the start point for next unit (wire)
			iUnitStartPosn_X += iDistPitch_X_cnt;
			iUnitStartPosn_Y = aiRandBurnInAxisLowerLimit[1] + iDistWireLength_cnt;
			iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];
		}

		// CW-2: Left Side
		// X-UpperLimit, From Front to Rear, Y-LowLimit to Y-UppLimit, Wire Dir in X
		iUnitStartPosn_X = aiRandBurnInAxisUpperLimit[0] - iDistWireLength_cnt;
		iUnitStartPosn_Y = aiRandBurnInAxisLowerLimit[1];
		iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];
		for(ii = 0; ii < usFixBurnInNumUnitsPerSide; ii++)
		{
			// First Point in a unit, FireLevel
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = iUnitStartPosn_Z;

			// 2nd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// 3rd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X + iDistWireLength_cnt;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// Update the start point for next unit (wire)
			iUnitStartPosn_X = aiRandBurnInAxisUpperLimit[0] - iDistWireLength_cnt;
			iUnitStartPosn_Y += iDistPitch_Y_cnt;
			iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];
		}

		// CW-3: Machine Rear Side
		// Y-UppLimit, From Left to Right, X-UppLimit to X-LowLimit, Wire-Dir in Y
		iUnitStartPosn_X = aiRandBurnInAxisUpperLimit[0];
		iUnitStartPosn_Y = aiRandBurnInAxisUpperLimit[1] - iDistWireLength_cnt;
		iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];

		for(ii = 0; ii < usFixBurnInNumUnitsPerSide; ii++)
		{
			// First Point in a unit, FireLevel
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = iUnitStartPosn_Z;

			// 2nd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// 3rd Point in a unit, teach contact, after wir-len in wire-dir
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y + iDistWireLength_cnt;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// Update the start point for next unit (wire)
			iUnitStartPosn_X -= iDistPitch_X_cnt;
			iUnitStartPosn_Y = aiRandBurnInAxisUpperLimit[1] - iDistWireLength_cnt;
			iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];
		}

		// CW-4: Right Side
		// X-LowLimit, From Rear to Front, Y-UppLimit to Y-LowLimit, Wire-Dir in X
		iUnitStartPosn_X = aiRandBurnInAxisLowerLimit[0] + iDistWireLength_cnt;
		iUnitStartPosn_Y = aiRandBurnInAxisUpperLimit[1];
		iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];

		for(ii = 0; ii < usFixBurnInNumUnitsPerSide; ii++)
		{
			// First Point in a unit, FireLevel
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = iUnitStartPosn_Z;

			// 2nd Point in a unit, teach contact
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// 3rd Point in a unit, teach contact, after wir-len in wire-dir
			aiTargetPointRandBurnInPerCycle[0][idxPoint] = iUnitStartPosn_X - iDistWireLength_cnt;
			aiTargetPointRandBurnInPerCycle[1][idxPoint] = iUnitStartPosn_Y;
			aiTargetPointRandBurnInPerCycle[2][idxPoint ++] = aiRandBurnInAxisLowerLimit[2];

			// Update the start point for next unit (wire)
			iUnitStartPosn_X = aiRandBurnInAxisLowerLimit[0] + iDistWireLength_cnt;
			iUnitStartPosn_Y -= iDistPitch_Y_cnt;
			iUnitStartPosn_Z = aiRandBurnInAxisUpperLimit[2];
		}

		usActualNumPointsPerCycle_FixBurnIn = idxPoint;
	}
	else
	{   // Machine Front Side
		// Y-LowLimit, From Left to Right, X-UppLimit to X-LowLimit

		// Machine Rear Side
		// Y-UppLimit, From Right to Left, X-LowLimit to X-UppLimit
	}
}

void aft_fix_burn_in_init_target_point_per_cycle()
{
	if(usNumSide[usSidePattern] == 1)
	{
		aft_fix_burn_in_calc_target_point_per_cycle_1_side();
	}
	else if(usNumSide[usSidePattern] == 2)
	{
		aft_fix_burn_in_calc_target_point_per_cycle_2_side();
	}
	else if(usNumSide[usSidePattern] == 4)
	{
		aft_fix_burn_in_calc_target_point_per_cycle_4_side();
	}
	else
	{
	}

}

// Quality Control, to ensure that all controller axis(motors , electrical & mechanical)are in safe condition
short qc_is_any_motor_not_safe(HANDLE hCommunicationHandle)
{
	short sRet = MTN_API_OK_ZERO;
	int jj;
	int iMotorStateReg;
	for(jj = 0; jj< MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
	{
		if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD))
		{
			mtnapi_get_motor_state(hCommunicationHandle, asQcBurnInAxisIdAcs[jj], &iMotorStateReg, NULL);
			if(iMotorStateReg & ACSC_MST_ENABLE)
			{
			}
			else
			{
				sRet = MTN_API_ERROR; // error happens
				break;
			}
		}
	}
	return sRet;
}

short qc_is_any_motor_still_moving(HANDLE hCommunicationHandle)
{
	short sRet = 0;
	int jj;
	int iMotorStateReg;
	for(jj = 0; jj< MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
	{
		if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD))
		{
			mtnapi_get_motor_state(hCommunicationHandle, asQcBurnInAxisIdAcs[jj], &iMotorStateReg, NULL);
			if(iMotorStateReg & ACSC_MST_MOVE)  // 
			{
				sRet = 1;
				goto label_qc_is_any_motor_still_moving; // or break;
			}
		}
	}

label_qc_is_any_motor_still_moving:
	return sRet;
}
//
//short qc_is_axis_still_acc_dec(HANDLE hCommunicationHandle, int iAxis)
//{
//	short sRet = 0;
//	int iTempMotorState;
//	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iTempMotorState, NULL);
//	if(iTempMotorState & ACSC_MST_ACC ||
//		iTempMotorState & ACSC_MST_MOVE) //
//	{
//		sRet = 1;
//	}
//	return sRet;
//}
//
//short qc_is_axis_not_safe(HANDLE hCommunicationHandle, int iAxis)
//{
//	short sRet;
//	int iTempMotorState;
//	mtnapi_get_motor_state(hCommunicationHandle, iAxis, &iTempMotorState, NULL);
//	if(iTempMotorState & ACSC_MST_ENABLE) //
//	{
//		sRet = 0;
//	}
//	else
//	{
//		sRet = 1;
//	}
//	return sRet;
//}