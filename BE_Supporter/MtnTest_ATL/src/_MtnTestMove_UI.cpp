/// _MtnTestMove_UI.cpp
/// Embedded in _MtnTestMove.cpp


// IDC_EDIT_ACS_SP_JERKFF
void CMtnTest_Move::OnEnKillfocusEditAcsSpJerkff()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SP_JERKFF, &(stServoLoopAcsCtrlPara_CurrAxis.dJerkFf));
	mtnapi_download_acs_sp_parameter_jerk_ff(Handle, uiCurrMoveAxis_ACS, stServoLoopAcsCtrlPara_CurrAxis.dJerkFf);

	// 20110603
	acs_clear_buffer_7_from_ram_para_();

	int iMechCfg = get_sys_machine_type_flag();		// 20111214
	acs_update_buffer_7_from_ram_para_(iMechCfg);	// 20111214
	acs_compile_start_buffer_7();  // 20110603
}

//////////
void CMtnTest_Move::UI_Show_GroupCurrentLoopTest(BOOL bEnableFlag)
{
	GetDlgItem(IDC_GROUP_MOVE_TEST_CURR_STEP_TEST)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_LOW_LEVEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_MOVE_TEST_CURR_STEP_LOW_LEVEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_MOVE_TEST_CURR_STEP_UPP_LEVEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_MOVE_TEST_CURR_STEP_UPP_LEVEL)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_MOVE_TEST_TOTAL_CURRENT_LOOP_ITER)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_MOVE_TEST_CURR_STEP_TOTAL_ITER)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_MOVE_TEST_CURR_STEP_START)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_STATIC_MOVE_CURR_STEP_TEST_HIGH_LEVEL_I)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_MOVE_CURR_STEP_TEST_LOW_LEVEL_I)->ShowWindow(bEnableFlag);
}

// 
void CMtnTest_Move::UI_Show_GroupBurnInTest(BOOL bEnableFlag)
{
	GetDlgItem(IDC_STATIC_RAND_BURN_IN_ENVOLVE_AXIS)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_1_ENVOLVED)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_2_ENVOLVED)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_3_ENVOLVED)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_MOVE_TEST_RANDOM_BURN_IN_AXIS_4_ENVOLVED)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_RAND_BURN_IN_SELECT_AXIS)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_RAND_BURN_IN_NUM_POINTS_PER_CYCLE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_GROUP_LABEL_RANDOM_BURN_IN)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_NUM_POINTS_PER_CYCLE_MOVE_TEST_RANDOM_BURN_IN)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_MOVE_TEST_RANDOM_BURN_IN_START)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_GROUP_LABEL_FIX_BURN_IN)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FIX_BURN_IN_WIRE_LENGTH)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_FIX_BURN_IN_WIRE_LENGTH)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FIXED_BURNIN_PITCH_WITHIN_WIRE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_FIX_BURN_IN_BETWEEN_UNIT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FIX_BURN_IN_SIDES_PATTERN)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_FIX_BURN_IN_NUM_SIDES)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_FIX_BURN_IN_NUM_UNITS_PER_SIDE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_FIX_BURN_IN_NUM_UNITS_PER_SIDE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FIX_BURN_IN_FLAG_IS_CLOCK_WISE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_BUTTON_TEST_MOVE_FIX_BURN_IN_START)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_RAND_BURN_IN_TOTAL_NUM_CYCLE)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_TOTAL_NUM_CYCLE_MOVE_TEST_RANDOM_BURN_IN)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_STATIC_RAND_BURN_IN_UPP_LIMIT)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_RAND_BURN_IN_LOW_LIMIT)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_3)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_UPP_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_LOW_LMT_MOVE_TEST_RANDOM_BURN_IN_AXIS_4)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_2)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_4)->ShowWindow(bEnableFlag);

}

void CMtnTest_Move::UI_Show_GroupWbSelectTuningPosition(BOOL bEnableFlag)
{
	GetDlgItem(IDC_STATIC_GROUP_SELECT_WB_XYZ_TUNE_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SELECT_WB_TABLE_TUNE_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SELECT_WB_BH_TUNE_POSITION)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_SEL_TUNE_WB_BOND_HEAD_MOTION_IDX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_COMBO_SEL_TUNE_WB_TABLE_MOTION_IDX)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SELECT_WB_WIRE_NUMBER)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_WIRE_ID_MOVE_TEST_GET_TUNE_POSN_SET)->ShowWindow(bEnableFlag);
}

void CMtnTest_Move::UI_Enable_GroupEditAcsParameter(BOOL bEnableFlag)
{
	GetDlgItem(IDC_MAXVEL_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_MAXACC_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_MAXDEC_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_MAXJERK_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_MAX_KILL_ACC_AXIS1)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVKI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLPKP)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVLI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLAFF)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLDRX)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLDRA)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SP_K_A_FROM_X)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SP_K_X_FROM_A)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SP_JERKFF)->EnableWindow(bEnableFlag);

	GetDlgItem(IDC_EDIT_ACS_SLIOFFS)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLPLI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLPKI)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLFRCD)->EnableWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLFRC)->EnableWindow(bEnableFlag);

//
	GetDlgItem(IDC_STATIC_ACS_NFRQ)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_ACS_NWID)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_ACS_NATT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_ACS_MFLAGS_ENA_NTCH)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SLVSOF)->ShowWindow(bEnableFlag);

//
	GetDlgItem(IDC_EDIT_ACS_SLVNFRQ)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVNWID)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVNATT)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_ACS_ENA_NOTCH)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_ACS_ENA_SOF)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SOF)->ShowWindow(bEnableFlag);

	// BIQUAD
	GetDlgItem(IDC_EDIT_ACS_SLVB0NF)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DF)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVB0ND)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DD)->ShowWindow(bEnableFlag);

	GetDlgItem(IDC_STATIC_SLVB0NF)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SLVB0DF)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SLVB0ND)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_STATIC_SLVB0DD)->ShowWindow(bEnableFlag);

}

void CMtnTest_Move::InitCurrLoopStepTestUI()
{
	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_LOW_LEVEL, stMoveTestCurrLoopStep.iMoveCurrLoopTestLowLevel);
	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_UPP_LEVEL, stMoveTestCurrLoopStep.iMoveCurrLoopTestUppLevel);
	UpdateIntToEdit(IDC_EDIT_MOVE_TEST_CURR_STEP_TOTAL_ITER, stMoveTestCurrLoopStep.uiTotalIter);

}

void CMtnTest_Move::UI_InitDialog()
{
	// ShowFlags for Groups
	((CButton*)GetDlgItem(IDC_CHECK_SHOW_GROUP_TEST_MOVE_MULTI_AXIS_RAND_BURN_IN))->SetCheck(bFlagEnableShowGroupBurnInTest);
	UI_Show_GroupBurnInTest(bFlagEnableShowGroupBurnInTest);

	((CButton*)GetDlgItem(IDC_CHECK_SHOW_GROUP_SET_WB_TUNE_POSITION))->SetCheck(bFlagEnableShowGroupWbTunePosn);
	UI_Show_GroupWbSelectTuningPosition(bFlagEnableShowGroupWbTunePosn);

	((CButton*)GetDlgItem(IDC_CHECK_SHOW_MOVE_TEST_CURRENT_LOOP))->SetCheck(bFlagEnableShowGroupCurrentLoopTest);
	UI_Show_GroupCurrentLoopTest(bFlagEnableShowGroupCurrentLoopTest);

	((CButton*)GetDlgItem(IDC_CHECK_MOVING_TEST_DLG_ENABLE_EDIT_ACS_PARA))->SetCheck(bFlagEnableEditAcsParameter);
	UI_Enable_GroupEditAcsParameter(bFlagEnableEditAcsParameter);

	// Move Test Current Loop
	InitCurrLoopStepTestUI();
	InitRandBurnInGroupTestConfigUI();

	// Special Axis for Burn-in, AXIS_BONDING_CONTROL_IN_BURN_IN = 2, 3rd axis over 4
	GetDlgItem(IDC_COMBO_SEL_MOVE_TEST_RANDOM_BURN_IN_AXIS_3)->EnableWindow(FALSE);
	InitFixBurnInGroupTestConfigUI();

	// Speed Profile
	UpdateUI_SpeedProfile();

	// Servo Parameters
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis); // 20090930
	mtnapi_upload_acs_sp_parameter_decouple_x_from_a(Handle, &dAcsControlDecoupleY_FromZ); // 20110701
	mtnapi_upload_acs_sp_parameter_decouple_a_from_x(Handle, &dAcsControlDecoupleZ_FromY); // 20110701
	UpdateUI_ServoLoopParameter();

	// Axis config
	UpdateUI_AxisCommandSetting();
}

void CMtnTest_Move::UpdateUI_SpeedProfile()
{
//	MtnTestMoveDlg_UpdateSpeedProfile // 20130218
	mtnapi_get_speed_profile(Handle, astAxisInfoWireBond[0].iAxisInCtrlCardACS, &stSpeedProfilePerAxis[0], 0);
	mtnapi_get_speed_profile(Handle, astAxisInfoWireBond[1].iAxisInCtrlCardACS, &stSpeedProfilePerAxis[1], 0);
	mtnapi_get_speed_profile(Handle, astAxisInfoWireBond[2].iAxisInCtrlCardACS, &stSpeedProfilePerAxis[2], 0);


//	unsigned int uiAxisTemp;
//	uiAxisTemp = astAxisInfoWireBond[uiCurrMoveAxis_AppWB].iAxisInCtrlCardACS;//uiCurrMoveAxis_AppWB; //20120921, mtn_test_get_current_moving_axis();
	cstrEditText.Format("%8.2f", stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxVelocity);
	GetDlgItem(IDC_MAXVEL_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%8.2f", stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxAcceleration);
	GetDlgItem(IDC_MAXACC_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%8.2f", stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxDeceleration);
	GetDlgItem(IDC_MAXDEC_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%8.2f", stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxJerk);
	GetDlgItem(IDC_MAXJERK_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%8.2f", stSpeedProfilePerAxis[uiCurrMoveAxis_AppWB].dMaxKillDeceleration);
	GetDlgItem(IDC_MAX_KILL_ACC_AXIS1)->SetWindowTextA(cstrEditText);

}


void CMtnTest_Move::UpdateUI_AxisCommandSetting()
{
	unsigned int uiAxisTemp = mtn_test_get_current_moving_axis();
	cstrEditText.Format("%d", uiMotionInterDelay_ms[uiAxisTemp]);
	GetDlgItem(IDC_DWELL_AXIS1)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%f", dRelDistance_mm);
	GetDlgItem(IDC_EDIT_REL_DIST_1)->SetWindowTextA(cstrEditText);

	uiTotalNumLoopRelMove = 2;
	cstrEditText.Format("%d", uiTotalNumLoopRelMove);
	GetDlgItem(IDC_EDIT_NUM_LOOP_REL_MOVE)->SetWindowText(cstrEditText);
}

void CMtnTest_Move::DisableAcsParaUI_DuringTuning()
{
	EnableAcsParaUI_DuringTuning(FALSE);
}

void CMtnTest_Move::EnableAcsParaUI_DuringTuning(BOOL bFlagIsEnable)
{
	GetDlgItem(IDC_EDIT_ACS_SLPKP)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVKP)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVKI)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_ACS_SLVLI)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_ACS_SOF)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_ACS_SLAFF)->EnableWindow(bFlagIsEnable);
	GetDlgItem(IDC_EDIT_MOVE_TEST_SERVO_TUNE_POSNERR_SETTLE_TH)->EnableWindow(bFlagIsEnable);

	bFlagIsEnabledAxisServoParaACS = bFlagIsEnable;
}

void CMtnTest_Move::UpdateIntToEdit(int nResId, int iValue)
{
	static char tempChar[32];
	sprintf_s(tempChar, 32, "%d", iValue);
	GetDlgItem(nResId)->SetWindowTextA(_T(tempChar));
}

void CMtnTest_Move::UpdateUShortToEdit(int nResId, unsigned short usValue)
{
	CString cstrTemp;
	cstrTemp.Format("%d", usValue);
//	sprintf_s(tempChar, 32, "%d", sValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}

void CMtnTest_Move::UpdateDoubleToEdit(int nResId, double dValue)
{
	CString cstrTemp;
	cstrTemp.Format("%6.3f", dValue);
	GetDlgItem(nResId)->SetWindowTextA(cstrTemp); // _T(tempChar));
}

void CMtnTest_Move::ReadIntegerFromEdit(int nResId, int *iValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", iValue);
}

void CMtnTest_Move::ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", uiValue);
}

void CMtnTest_Move::ReadUShortFromEdit(int nResId, unsigned short *pusValue)
{
	static char tempChar[32];
	int iTemp;
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	sscanf_s(tempChar, "%d", &iTemp);
	*pusValue = (short)iTemp;
}

void CMtnTest_Move::ReadDoubleFromEdit(int nResId, double *pdValue)
{
	static char tempChar[32];
	GetDlgItem(nResId)->GetWindowTextA(&tempChar[0], 32);
	*pdValue = strtod(tempChar, &strStopString);
//	sscanf_s(tempChar, "%f", pdValue);
}

// IDC_STATIC_SLPKP
// IDC_STATIC_SLVKP
// IDC_STATIC_SLVKI
// IDC_STATIC_SLVLI
// IDC_STATIC_SLVSOF
// IDC_STATIC_SLAFF

// IDC_EDIT_ACS_SLPKP
void CMtnTest_Move::OnEnKillfocusEditAcsSlpkp()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLPKP, &(stServoLoopAcsCtrlPara_CurrAxis.dPositionLoopProportionalGain));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVKP
void CMtnTest_Move::OnEnKillfocusEditAcsSlvkp()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVKP, &(stServoLoopAcsCtrlPara_CurrAxis.dVelocityLoopProportionalGain));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVKI
void CMtnTest_Move::OnEnKillfocusEditAcsSlvki()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVKI, &(stServoLoopAcsCtrlPara_CurrAxis.dVelocityLoopIntegratorGain));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVLI
void CMtnTest_Move::OnEnKillfocusEditAcsSlvli()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVLI, &(stServoLoopAcsCtrlPara_CurrAxis.dVelocityLoopIntegratorLimit));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SOF
void CMtnTest_Move::OnEnKillfocusEditAcsSof()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SOF, &(stServoLoopAcsCtrlPara_CurrAxis.dSecondOrderLowPassFilterBandwidth));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLAFF
void CMtnTest_Move::OnEnKillfocusEditAcsSlaff()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLAFF, &(stServoLoopAcsCtrlPara_CurrAxis.dAccelerationFeedforward));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLDRA
void CMtnTest_Move::OnEnKillfocusEditAcsSldra()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLDRA, &(stServoLoopAcsCtrlPara_CurrAxis.dDRA));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLDRX
void CMtnTest_Move::OnEnKillfocusEditAcsSldrx()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLDRX, &(stServoLoopAcsCtrlPara_CurrAxis.dDRX));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}

// IDC_EDIT_ACS_SP_K_X_FROM_A
void CMtnTest_Move::OnEnKillfocusEditAcsSpKXFromA()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SP_K_X_FROM_A, &(dAcsControlDecoupleY_FromZ));
	mtnapi_download_acs_sp_parameter_k_x_from_a(Handle, dAcsControlDecoupleY_FromZ);
	
	acs_clear_buffer_7_from_ram_para_(); // 20121017
	int iMechCfg = get_sys_machine_type_flag();		
	acs_update_buffer_7_from_ram_para_(iMechCfg);	
	acs_compile_start_buffer_7();  // 20121017

}
// IDC_EDIT_ACS_SP_K_A_FROM_X
void CMtnTest_Move::OnEnKillfocusEditAcsSpKAFromX()
{
	// TODO: Add your control notification handler code here
	ReadDoubleFromEdit(IDC_EDIT_ACS_SP_K_A_FROM_X, &(dAcsControlDecoupleZ_FromY));
	mtnapi_download_acs_sp_parameter_k_a_from_x(Handle, dAcsControlDecoupleZ_FromY);

	acs_clear_buffer_7_from_ram_para_(); // 20121017
	int iMechCfg = get_sys_machine_type_flag();		
	acs_update_buffer_7_from_ram_para_(iMechCfg);	
	acs_compile_start_buffer_7();  // 20121017
}
// IDC_EDIT_ACS_SLIOFFS
void CMtnTest_Move::OnEnKillfocusEditAcsSlioffs()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLIOFFS, &(stServoLoopAcsCtrlPara_CurrAxis.dCurrentLoopOffset));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLFRC
void CMtnTest_Move::OnEnKillfocusEditAcsSlfrc()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLFRC, &(stServoLoopAcsCtrlPara_CurrAxis.dVelocityFrictionPc));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLFRCD
void CMtnTest_Move::OnEnKillfocusEditAcsSlfrcd()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLFRCD, &(stServoLoopAcsCtrlPara_CurrAxis.dVelocityFrictionMax));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLPKI
//void CMtnTest_Move::OnEnKillfocusEditAcsSlpki()
//{
//	ReadDoubleFromEdit(IDC_EDIT_ACS_SLPKI, &(stServoLoopAcsCtrlPara_CurrAxis.dPositionIntegralGain));
//	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
//}
//// IDC_EDIT_ACS_SLPLI
//void CMtnTest_Move::OnEnKillfocusEditAcsSlpli()
//{
//	ReadDoubleFromEdit(IDC_EDIT_ACS_SLPLI, &(stServoLoopAcsCtrlPara_CurrAxis.dPositionIntegralLimit));
//	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
//}
// IDC_EDIT_ACS_SLVNFRQ
void CMtnTest_Move::OnEnKillfocusEditAcsSlvnfrq()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVNFRQ, &(stServoLoopAcsCtrlPara_CurrAxis.dNotchFilterFrequency));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVNWID
void CMtnTest_Move::OnEnKillfocusEditAcsSlvnwid()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVNWID, &(stServoLoopAcsCtrlPara_CurrAxis.dNotchFilterWidth));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVNATT
void CMtnTest_Move::OnEnKillfocusEditAcsSlvnatt()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVNATT, &(stServoLoopAcsCtrlPara_CurrAxis.dNotchFilterAttenuation));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}

// _BIT_INV_ENC_CONTROL_CARD_
#define _BIT_SET_ENABLE_NOTH_  14
#define _BIT_SET_DISABLE_SOF_  15
#define _BIT_SET_ENABLE_BIQUAD_  16
// IDC_CHECK_ACS_ENA_NOTCH
void CMtnTest_Move::OnBnClickedCheckAcsEnaNotch()
{
//	int uiCurrMoveAxis_ACS = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
	int iControlCardMotorFlag;
	acsc_ReadInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagEnc;
	iTempFlagEnc = ((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENA_NOTCH))->GetCheck();

	unsigned int uiTempFlagBitAnd, uiTempBitFullOne32Bit;
	uiTempFlagBitAnd = (unsigned int)pow(2.0, (double)_BIT_SET_ENABLE_NOTH_);  

	if(iTempFlagEnc == 1)
	{ // OR the bit to enable
		iControlCardMotorFlag = iControlCardMotorFlag | uiTempFlagBitAnd;
	}
	else 
	{ // clear the bit to disable
		uiTempBitFullOne32Bit = _FULL_BITS_INTEGER_32b_; // (int)(pow(2.0, 32) - 1);
		uiTempFlagBitAnd = uiTempBitFullOne32Bit - uiTempFlagBitAnd;
		iControlCardMotorFlag = iControlCardMotorFlag & uiTempFlagBitAnd;

	}
	acsc_WriteInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

}

// IDC_CHECK_ACS_ENA_SOF
void CMtnTest_Move::OnBnClickedCheckAcsEnaSof()
{
	int iControlCardMotorFlag;
	acsc_ReadInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagEnc;
	iTempFlagEnc = ((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENA_SOF))->GetCheck();

	unsigned int uiTempFlagBitAnd, uiTempBitFullOne32Bit;
	uiTempFlagBitAnd = (unsigned int)pow(2.0, (double)_BIT_SET_DISABLE_SOF_);  

	if(iTempFlagEnc == 1)
	{ // OR the bit to enable
		iControlCardMotorFlag = iControlCardMotorFlag | uiTempFlagBitAnd;
	}
	else 
	{ // clear the bit to disable
		uiTempBitFullOne32Bit = _FULL_BITS_INTEGER_32b_; // (int)(pow(2.0, 32) - 1);
		uiTempFlagBitAnd = uiTempBitFullOne32Bit - uiTempFlagBitAnd;
		iControlCardMotorFlag = iControlCardMotorFlag & uiTempFlagBitAnd;

	}
	acsc_WriteInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);
}
void CMtnTest_Move::UpdateUI_ServoLoopParameter()
{
//	unsigned int uiAxisTemp;
	static CTRL_PARA_ACS stServoLoopAcsCtrlPara_CurrTimeCurrAxis; // 20090831
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrTimeCurrAxis); // 2009093

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dPositionLoopProportionalGain);
	GetDlgItem(IDC_EDIT_ACS_SLPKP)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dVelocityLoopProportionalGain);
	GetDlgItem(IDC_EDIT_ACS_SLVKP)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dVelocityLoopIntegratorGain);
	GetDlgItem(IDC_EDIT_ACS_SLVKI)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dVelocityLoopIntegratorLimit);
	GetDlgItem(IDC_EDIT_ACS_SLVLI)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dSecondOrderLowPassFilterBandwidth);
	GetDlgItem(IDC_EDIT_ACS_SOF)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dAccelerationFeedforward);
	GetDlgItem(IDC_EDIT_ACS_SLAFF)->SetWindowTextA(cstrEditText);

// Notch Filter
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dNotchFilterFrequency);
	GetDlgItem(IDC_EDIT_ACS_SLVNFRQ)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dNotchFilterWidth);
	GetDlgItem(IDC_EDIT_ACS_SLVNWID)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dNotchFilterAttenuation);
	GetDlgItem(IDC_EDIT_ACS_SLVNATT)->SetWindowTextA(cstrEditText);

// Jerk FF
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dJerkFf);
	GetDlgItem(IDC_EDIT_ACS_SP_JERKFF)->SetWindowTextA(cstrEditText);

// DRA
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dDRA);
	GetDlgItem(IDC_EDIT_ACS_SLDRA)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dDRX);
	GetDlgItem(IDC_EDIT_ACS_SLDRX)->SetWindowTextA(cstrEditText);
	//
	cstrEditText.Format("%4.0f", fThresholdSettleTime);
	GetDlgItem(IDC_EDIT_MOVE_TEST_SERVO_TUNE_POSNERR_SETTLE_TH)->SetWindowTextA(cstrEditText);

	// 20110701, Decouple
	cstrEditText.Format("%4.1f", dAcsControlDecoupleY_FromZ);
	GetDlgItem(IDC_EDIT_ACS_SP_K_X_FROM_A)->SetWindowTextA(cstrEditText);

	cstrEditText.Format("%4.1f", dAcsControlDecoupleZ_FromY);
	GetDlgItem(IDC_EDIT_ACS_SP_K_A_FROM_X)->SetWindowTextA(cstrEditText);

// IDC_EDIT_ACS_SLIOFFS
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dCurrentLoopOffset);
	GetDlgItem(IDC_EDIT_ACS_SLIOFFS)->SetWindowTextA(cstrEditText);

// IDC_EDIT_ACS_SLFRC
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dVelocityFrictionPc);
	GetDlgItem(IDC_EDIT_ACS_SLFRC)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLFRCD
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dVelocityFrictionMax);
	GetDlgItem(IDC_EDIT_ACS_SLFRCD)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLPKI
//	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dPositionIntegralGain);
//	GetDlgItem(IDC_EDIT_ACS_SLPKI)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLPLI
//	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dPositionIntegralLimit);
//	GetDlgItem(IDC_EDIT_ACS_SLPLI)->SetWindowTextA(cstrEditText);

// IDC_EDIT_ACS_SLVB0NF  // 20120921
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dBiquadNumFreq);
	GetDlgItem(IDC_EDIT_ACS_SLVB0NF)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLVB0DF
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dBiquadDenFreq);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DF)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLVB0ND
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dBiquadNumDamp);
	GetDlgItem(IDC_EDIT_ACS_SLVB0ND)->SetWindowTextA(cstrEditText);
// IDC_EDIT_ACS_SLVB0DD
	cstrEditText.Format("%6.1f", stServoLoopAcsCtrlPara_CurrTimeCurrAxis.dBiquadDenDamp);
	GetDlgItem(IDC_EDIT_ACS_SLVB0DD)->SetWindowTextA(cstrEditText);

///////////////
// ACS-Config
//	int uiCurrMoveAxis_ACS = astAxisInfoWireBond[iCurrSelAxisServoCtrlTuning].iAxisInCtrlCardACS;
	
	int iControlCardMotorFlag;
	acsc_ReadInteger(Handle, ACSC_NONE, "MFLAGS", 
				uiCurrMoveAxis_ACS, uiCurrMoveAxis_ACS, 0, 0, &iControlCardMotorFlag, NULL);

	int iTempFlagSOF = 0;
	int iTempFlagEnaNotch = 0;
	int iTempFlagEnaBiquad = 0;

	unsigned int uiTempFlagBitAnd;
	uiTempFlagBitAnd = (int)pow(2.0, (double)_BIT_SET_DISABLE_SOF_); // 
	iTempFlagSOF = iControlCardMotorFlag & uiTempFlagBitAnd;
	uiTempFlagBitAnd = (int)pow(2.0, (double)_BIT_SET_ENABLE_NOTH_);  // 
	iTempFlagEnaNotch = iControlCardMotorFlag & uiTempFlagBitAnd;

	uiTempFlagBitAnd = (int)pow(2.0, (double)_BIT_SET_ENABLE_BIQUAD_);  // 
	iTempFlagEnaBiquad = iControlCardMotorFlag & uiTempFlagBitAnd;  // 20121106

	((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENA_SOF))->SetCheck(iTempFlagSOF);
	((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENA_NOTCH))->SetCheck(iTempFlagEnaNotch);
	((CButton * ) GetDlgItem(IDC_CHECK_ACS_ENABLE_BIQUAD_MOVING_DLG))->SetCheck(iTempFlagEnaBiquad); // 20121106

}

// IDC_EDIT_ACS_SLVB0NF
void CMtnTest_Move::OnEnKillfocusEditAcsSlvb0nf()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVB0NF, &(stServoLoopAcsCtrlPara_CurrAxis.dBiquadNumFreq));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVB0DF
void CMtnTest_Move::OnEnKillfocusEditAcsSlvb0df()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVB0DF, &(stServoLoopAcsCtrlPara_CurrAxis.dBiquadDenFreq));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVB0ND
void CMtnTest_Move::OnEnKillfocusEditAcsSlvb0nd()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVB0ND, &(stServoLoopAcsCtrlPara_CurrAxis.dBiquadNumDamp));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
// IDC_EDIT_ACS_SLVB0DD
void CMtnTest_Move::OnEnKillfocusEditAcsSlvb0dd()
{
	ReadDoubleFromEdit(IDC_EDIT_ACS_SLVB0DD, &(stServoLoopAcsCtrlPara_CurrAxis.dBiquadDenDamp));
	mtnapi_download_servo_parameter_acs_per_axis(Handle, uiCurrMoveAxis_ACS, &stServoLoopAcsCtrlPara_CurrAxis);
}
