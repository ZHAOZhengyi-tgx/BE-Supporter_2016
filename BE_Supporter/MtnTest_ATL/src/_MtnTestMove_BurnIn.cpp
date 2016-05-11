


// MTN_TEST_BURN_IN_ONE_AXIS_2_POINTS    1
short CMtnTest_Move::OneAxis2PointsMoveOneCycle()
{
	unsigned int uiAxisTemp = mtn_test_get_current_moving_axis();
	static int iTempMotorState;
	short sRet = MTN_API_OK_ZERO;

	static CTRL_PARA_ACS astBakupServoPara;

	Sleep(uiMotionInterDelay_ms[uiAxisTemp]); 	//Sleep(uiMotionInterDelay_ms[uiAxisTemp]);

	acsc_ToPoint(Handle, 0, // start up immediately the motion
					uiAxisTemp, iPoint1stIn2PointsOneAxisMove, NULL);
	while(qc_is_axis_still_acc_dec(Handle, (int)uiAxisTemp))
	{
		Sleep(1); 	//Sleep(2);
		if(qc_is_axis_not_safe(Handle, (int)uiAxisTemp))
		{
			mtn_api_prompt_acs_error_code(Handle);
			sRet = MTN_API_ERROR; // error happens
			goto label_ret_one_axis_2_points_move_cycle;
		}
	}

	// 20130218
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&astBakupServoPara);
	mtnapi_download_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&stServoAxis_ACS[uiCurrMoveAxis_AppWB].stServoParaACS[0]); // 20130218


	Sleep(uiMotionInterDelay_ms[uiAxisTemp]); 	//Sleep(uiMotionInterDelay_ms[uiAxisTemp]);
	mtnapi_download_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&astBakupServoPara); // 20130218

	acsc_ToPoint(Handle, 0, // start up immediately the motion
					uiAxisTemp, iPoint2ndIn2PointsOneAxisMove, NULL);
	mtnapi_get_motor_state(Handle, uiAxisTemp, &iTempMotorState, NULL);

	while(qc_is_axis_still_acc_dec(Handle, (int)uiAxisTemp))
	{
		Sleep(1); 	//Sleep(2);
		if(qc_is_axis_not_safe(Handle, (int)uiAxisTemp))
		{
			mtn_api_prompt_acs_error_code(Handle);
			sRet = MTN_API_ERROR; // error happens
			goto label_ret_one_axis_2_points_move_cycle;
		}
	}
	// 20130218
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&astBakupServoPara);
	mtnapi_download_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&stServoAxis_ACS[uiCurrMoveAxis_AppWB].stServoParaACS[0]); // 20130218


	Sleep(uiMotionInterDelay_ms[uiAxisTemp]); 	//Sleep(uiMotionInterDelay_ms[uiAxisTemp]);
	mtnapi_download_servo_parameter_acs_per_axis(Handle, astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS, 
		&astBakupServoPara); // 20130218

label_ret_one_axis_2_points_move_cycle:

	return sRet;
}

short CMtnTest_Move::FixBurnInOneCycle()
{
	int jj;
	short sRet = 0;

	iFlagErrorInServoControl = 0;
	// Setup scope for motionn, 20100305
	mtnscope_set_acsc_var_collecting_move(uiCurrMoveAxis_ACS);
	// Start collection
	if (!acsc_CollectB(Handle, 0, // system data collection
		gstrScopeArrayName, // name of data collection array
		gstSystemScope.uiDataLen, // number of samples to be collected
		1, // sampling period 1 millisecond
		strACSC_VarName, // variables to be collected
		NULL)
		)
	{
		mtn_api_prompt_acs_error_code(Handle);
		//printf("Collecting data, transaction error: %d\n", acsc_GetLastError());
		sRet = MTN_API_ERROR; // error happens
	}
	else
	{
		mtn_move_test_set_flag_collecting_data(); // 2010Feb05
	}

	iiBurnInPointInCycle = 0;
	while(iiBurnInPointInCycle<usActualNumPointsPerCycle_FixBurnIn &&
		!m_fStopBurnInThread )
	{
		// 3 points per unit
		// Move to 1st point
		for(jj = 0; jj<MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
		{
			if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD) )
			{// Move axis asQcBurnInAxisIdAcs[jj], to 
				if(abQcBurnInAxisEnvolveFlag[jj]) // 20100305
				{
					acsc_ToPoint(Handle, 0, // start up immediately the motion
							asQcBurnInAxisIdAcs[jj], aiTargetPointRandBurnInPerCycle[jj][iiBurnInPointInCycle], NULL);
				}
			}
		}
		while(qc_is_any_motor_still_moving(Handle))
		{
			Sleep(1);
		}// synchronize motion for starting all motion together, can be optimized. TBA

		// Move to 2nd point, actually only z-motor
		if(sRet = qc_is_any_motor_not_safe(Handle))
		{
			mtn_api_prompt_acs_error_code(Handle);
			iFlagErrorInServoControl = 1;  // 20100305
			goto label_FixBurnInOneCycle;
		}
		iiBurnInPointInCycle ++;
		if(abQcBurnInAxisEnvolveFlag[AXIS_BONDING_CONTROL_IN_BURN_IN]) // 20100305
		{
			acsc_ToPoint(Handle, 0, // start up immediately the motion
							asQcBurnInAxisIdAcs[AXIS_BONDING_CONTROL_IN_BURN_IN], 
							aiTargetPointRandBurnInPerCycle[AXIS_BONDING_CONTROL_IN_BURN_IN][iiBurnInPointInCycle], NULL);
		}
		while(qc_is_any_motor_still_moving(Handle))
		{
			Sleep(1);
		} // synchronization of motion for there should be search contact, and force control, and looping.
		if(abQcBurnInAxisEnvolveFlag[AXIS_BONDING_CONTROL_IN_BURN_IN]) // 20100305
		{
			acsc_ToPoint(Handle, 0, // start up immediately the motion
							asQcBurnInAxisIdAcs[AXIS_BONDING_CONTROL_IN_BURN_IN], 
							aiRandBurnInAxisUpperLimit[AXIS_BONDING_CONTROL_IN_BURN_IN], NULL);
		}

		// Start Z to upper limit, move to 3rd point
		iiBurnInPointInCycle ++;
		for(jj = 0; jj<MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
		{
			if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD) )
			{// Move axis asQcBurnInAxisIdAcs[jj], to 
				if(abQcBurnInAxisEnvolveFlag[jj]) // 20100305
				{
					acsc_ToPoint(Handle, 0, // start up immediately the motion
							asQcBurnInAxisIdAcs[jj], aiTargetPointRandBurnInPerCycle[jj][iiBurnInPointInCycle], NULL);
				}
			}
		}

		// Synchronize motion
		Sleep(uiMotionInterDelay_ms[0]); // 20101119

		// Check no error happens, s.t. all motor-axis are enabled
		if(sRet = qc_is_any_motor_not_safe(Handle))
		{
//			iiBurnInPointInCycle = usActualNumPointsPerCycle_FixBurnIn;
			mtn_api_prompt_acs_error_code(Handle);
			iFlagErrorInServoControl = 1;  // 20100305
			goto label_FixBurnInOneCycle;
		}
		iiBurnInPointInCycle ++;

		if(cFlagTuningCurrentAxis == TRUE
			&& iiBurnInPointInCycle % 4 == 0) // Upload data every 4 units
		{
			ServoTuneUploadSaveCalcIndex();
			mtn_move_test_clear_flag_collecting_data(); // 20100305
		}
	}

label_FixBurnInOneCycle:

	return sRet;

}
short CMtnTest_Move::RandBurnInOneCycle()
{
	int jj, iMotorStateReg;
	short sRet = 0;
	double adRegStartPoints[MAX_AXIS_ENVOLVE_BURN_IN_TEST];
	int iNumPointsError;

//	// Setup scope for motionn, 20100305
//	mtnscope_set_acsc_var_collecting_move(uiCurrMoveAxis_ACS);
//	// Start collection
//	if (!acsc_CollectB(Handle, 0, // system data collection
//		gstrScopeArrayName, // name of data collection array
//		gstSystemScope.uiDataLen, // number of samples to be collected
//		1, // sampling period 1 millisecond
//		strACSC_VarName, // variables to be collected
//		NULL)
//		)
//	{
//		printf("Collecting data, transaction error: %d\n", acsc_GetLastError());
////				sRet = MTN_API_ERROR; // error happens
//	}
//	else
//	{
//		mtn_move_test_set_flag_collecting_data(); // 2010Feb05
//	}

	for(iiBurnInPointInCycle = 0; iiBurnInPointInCycle<usRandBurnInNumPointsPerCycle; iiBurnInPointInCycle ++)
	{
		for(jj = 0; jj< MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
		{
			if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD) )
			{// Move axis asQcBurnInAxisIdAcs[jj], to 
				mtnapi_get_ref_position(Handle, asQcBurnInAxisIdAcs[jj], &adRegStartPoints[jj], 0);
				acsc_ToPoint(Handle, 0, // start up immediately the motion
						asQcBurnInAxisIdAcs[jj], aiTargetPointRandBurnInPerCycle[jj][iiBurnInPointInCycle], NULL);
			}
		}
		while(qc_is_any_motor_still_moving(Handle))
		{
			Sleep(5);
		} // synchronization of motion for there should be search contact, and force control, and looping.
		for(jj = 0; jj< MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
		{
			if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD))
			{
				mtnapi_get_motor_state(Handle, asQcBurnInAxisIdAcs[jj], &iMotorStateReg, NULL);
				if(iMotorStateReg & ACSC_MST_ENABLE)
				{
				}
				else
				{
					sRet = MTN_API_ERROR; // error happens
					iNumPointsError = iiBurnInPointInCycle;
					iiBurnInPointInCycle = usRandBurnInNumPointsPerCycle;
					mtn_api_prompt_acs_error_code(Handle);

					break;
				}
			}
		}
		Sleep(uiMotionInterDelay_ms[0]);

		if(m_fStopBurnInThread) // if press stop burn-in button
		{
			break;
		}

// Scope capture data, update the Performance Index
		//if(cFlagTuningCurrentAxis == TRUE
		//	&& iiBurnInPointInCycle % 4 == 0) // Upload data every 4 units
		//{
		//	ServoTuneUploadSaveCalcIndex();
		//	mtn_move_test_clear_flag_collecting_data(); // 20100305

		//	// Setup scope for motionn, 20100305
		//	mtnscope_set_acsc_var_collecting_move(uiCurrMoveAxis_ACS);
		//	// Start collection
		//	if (!acsc_CollectB(Handle, 0, // system data collection
		//		gstrScopeArrayName, // name of data collection array
		//		gstSystemScope.uiDataLen, // number of samples to be collected
		//		1, // sampling period 1 millisecond
		//		strACSC_VarName, // variables to be collected
		//		NULL)
		//		)
		//	{
		//		printf("Collecting data, transaction error: %d\n", acsc_GetLastError());
		////				sRet = MTN_API_ERROR; // error happens
		//	}
		//	else
		//	{
		//		mtn_move_test_set_flag_collecting_data(); // 2010Feb05
		//	}
		//}
	}

	if(sRet)
	{
		FILE *fptr;
		fopen_s(&fptr, strBurnInDebugFile, "w");
		fprintf(fptr, "BurnIn Error Axis: %d\n", jj);
		fprintf(fptr, "Error point: %d\n", iNumPointsError);
		fprintf(fptr, "Axis, FromPosn, ToPosn\n");
		for(jj = 0; jj< MAX_AXIS_ENVOLVE_BURN_IN_TEST; jj++)
		{
			if(abQcBurnInAxisEnvolveFlag[jj] && (asQcBurnInAxisIdAcs[jj] != MAX_CTRL_AXIS_PER_SERVO_BOARD))
			{
				fprintf(fptr, "%d, %8.1f, %d\n", jj, adRegStartPoints[jj], aiTargetPointRandBurnInPerCycle[jj][iNumPointsError]);
			}
		}

		fclose(fptr);

	}
	return sRet;
}


void CMtnTest_Move::InitRandBurnInGroupTestConfigUI()
{
	InitBurnInGroupTestConfigCombo_OneAxis(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, 0);
	InitBurnInGroupTestConfigCombo_OneAxis(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, 1);
	InitBurnInGroupTestConfigCombo_OneAxis(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, 2);
	InitBurnInGroupTestConfigCombo_OneAxis(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, 3);

	((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_1_ENVOLVED))->SetCheck(abQcBurnInAxisEnvolveFlag[0]);
	((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_2_ENVOLVED))->SetCheck(abQcBurnInAxisEnvolveFlag[1]);
	((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_3_ENVOLVED))->SetCheck(abQcBurnInAxisEnvolveFlag[2]);
	((CButton*) GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_4_ENVOLVED))->SetCheck(abQcBurnInAxisEnvolveFlag[3]);

	UpdateIntToEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, aiRandBurnInAxisUpperLimit[0]);
	UpdateIntToEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, aiRandBurnInAxisUpperLimit[1]);
	UpdateIntToEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, aiRandBurnInAxisUpperLimit[2]);
	UpdateIntToEdit(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, aiRandBurnInAxisUpperLimit[3]);

	UpdateIntToEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1, aiRandBurnInAxisLowerLimit[0]);
	UpdateIntToEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2, aiRandBurnInAxisLowerLimit[1]);
	UpdateIntToEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3, aiRandBurnInAxisLowerLimit[2]);
	UpdateIntToEdit(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4, aiRandBurnInAxisLowerLimit[3]);

	UpdateUShortToEdit(IDC_EDIT_NUM_POINTS_PER_CYCLE_MOVE_TEST_RANDOM_BURN_IN, usRandBurnInNumPointsPerCycle);

	UpdateIntToEdit(IDC_EDIT_TOTAL_NUM_CYCLE_MOVE_TEST_RANDOM_BURN_IN, uiRandBurnInTotalNumCycle);

}
// Fixed Burn In
void CMtnTest_Move::InitFixBurnInGroupTestConfigUI()
{
	UpdateDoubleToEdit(IDC_EDIT_FIX_BURN_IN_WIRE_LENGTH, dFixBurnInWireLength_mm);
	UpdateDoubleToEdit(IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT, dFixBurnInPitchPerUnit_mm);
	UpdateUShortToEdit(IDC_EDIT_FIX_BURN_IN_NUM_UNITS_PER_SIDE, usFixBurnInNumUnitsPerSide);
	((CButton*) GetDlgItem(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE))->SetCheck(bFixBurnInFlagIsClockWise);

	CComboBox *pSelectFixBurnInSidePatternCombo = (CComboBox*) GetDlgItem(IDC_COMBO_FIX_BURN_IN_NUM_SIDES);
	pSelectFixBurnInSidePatternCombo->InsertString(0,"1");
	pSelectFixBurnInSidePatternCombo->InsertString(1,"2");
	pSelectFixBurnInSidePatternCombo->InsertString(2,"4");
	pSelectFixBurnInSidePatternCombo->SetCurSel(usSidePattern);

}
