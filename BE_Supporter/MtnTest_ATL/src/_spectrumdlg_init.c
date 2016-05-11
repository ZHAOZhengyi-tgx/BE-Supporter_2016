
//extern char sys_acs_communication_get_flag_sc_udi();

void aft_spectrum_set_test_condition_1_cup_vled()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn - 500; // (int)mtn_wb_get_bh_upper_limit_position() - 2500; // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_1CUP; // BE_WB_Z_BOND_LOW_LIMIT_CNT;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn + 1000; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_1CUP; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 2;
	uiNumPoints[1] = 2;
	uiNumPoints[2] = 3;  // 20111123
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_PCI_ID;
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	}
	else
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_SC_UDI_ID;
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	} // 20121203

	usAxisAmpPrbs[0] = BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = BE_WB_SPECTRUM_TEST_Y_PRBS_AMP;
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_station_xy_vertical()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_ONE_TRACK_V_LED_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500; // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_1CUP; // BE_WB_Z_BOND_LOW_LIMIT_CNT;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_1CUP; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 2;
	uiNumPoints[1] = 2;
	uiNumPoints[2] = 1;
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	uiAxis_CtrlCardId[2] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis

	usAxisAmpPrbs[0] = BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = BE_WB_SPECTRUM_TEST_Y_PRBS_AMP;
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_hori_led()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_HORI_BONDER_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn - 500; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_HORI_BONDER_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn + 1000; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 3;
	uiNumPoints[1] = 3;
	uiNumPoints[2] = 2; // 20111123
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	}
	else
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_SC_UDI_ID;
		uiAxis_CtrlCardId[3] = APP_WIRE_CLAMP_ACS_SC_UDI_ID; // Dummy Axis
	} // 20121203

	usAxisAmpPrbs[0] = 1500; //BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = 1500; //
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_18v_led()
{
	iLowerLimitPosition[0] = 5000;  // 20120826
	iUpperLimitPosition[0] = 35000; // EFSIKA_X_TABLE_UPP_LIMIT_ONE_TRACK_V_LED_CNT;

	iLowerLimitPosition[1] = 5000; // EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iUpperLimitPosition[1] = 15000;// 2012July6

	iLowerLimitPosition[3] = 0;

	iLowerLimitPosition[2] = iBondHeadRelaxPosn - 500; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn + 1000; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 2;
	uiNumPoints[1] = 2;
	uiNumPoints[2] = 2; // 20111123
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	}
	else
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_SC_UDI_ID;
		uiAxis_CtrlCardId[3] = APP_WIRE_CLAMP_ACS_SC_UDI_ID; // Dummy Axis
	} // 20121203

	usAxisAmpPrbs[0] = 1000; //BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = 1000; //
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_station_xy_hori()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_HORI_BONDER_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_HORI_BONDER_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 3;
	uiNumPoints[1] = 3;
	uiNumPoints[2] = 0;
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_Y_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_X_TABLE_ACS_ID;
	uiAxis_CtrlCardId[2] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // APP_Z_BOND_ACS_ID;
	uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis

	usAxisAmpPrbs[0] = 1500; //BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = 1500; //
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_station_bh()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_HORI_BONDER_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn - 4000; // (int)mtn_wb_get_bh_upper_limit_position() - 4500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_HORI_BONDER_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn + 1000; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 0;
	uiNumPoints[1] = 0;
	uiNumPoints[2] = 5;
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	uiAxis_CtrlCardId[1] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	}
	else
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_SC_UDI_ID;
		uiAxis_CtrlCardId[3] = APP_WIRE_CLAMP_ACS_SC_UDI_ID; // Dummy Axis
	} // 20121203

	usAxisAmpPrbs[0] = 1500; //BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = 1500; //
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

	ucFlagIsExciteAxis[0] = 0;
	ucFlagIsExciteAxis[1] = 0;
	ucFlagIsExciteAxis[2] = 1;
	ucFlagIsExciteAxis[3] = 0;

}

void aft_spectrum_set_test_condition_dual_track_20_vled()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 5;
	uiNumPoints[1] = 2;
	uiNumPoints[2] = 1;
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = APP_X_TABLE_ACS_ID;
	uiAxis_CtrlCardId[1] = APP_Y_TABLE_ACS_ID;
	if(sys_acs_communication_get_flag_sc_udi() == 0)  // 20121203
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_PCI_ID;  // 20130311
		uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis
	}
	else
	{
		uiAxis_CtrlCardId[2] = APP_Z_BOND_ACS_SC_UDI_ID;
		uiAxis_CtrlCardId[3] = APP_WIRE_CLAMP_ACS_SC_UDI_ID; // Dummy Axis
	} // 20121203

	usAxisAmpPrbs[0] = BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = BE_WB_SPECTRUM_TEST_Y_PRBS_AMP;
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}

void aft_spectrum_set_test_condition_station_dummy()
{
	iLowerLimitPosition[0] = EFSIKA_X_TABLE_LOW_LIMIT_HORI_BONDER_CNT;
	iLowerLimitPosition[1] = EFSIKA_Y_TABLE_LOW_LIMIT_CNT;
	iLowerLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 4500;  // 20110711, BE_WB_Z_BOND_LOW_LIMIT_CNT_HORI_LED;
	iLowerLimitPosition[3] = 0;

	iUpperLimitPosition[0] = EFSIKA_X_TABLE_UPP_LIMIT_HORI_BONDER_CNT;
	iUpperLimitPosition[1] = EFSIKA_Y_TABLE_UPP_LIMIT_CNT;
	iUpperLimitPosition[2] = iBondHeadRelaxPosn; // (int)mtn_wb_get_bh_upper_limit_position() - 2500;  // 20110711, BE_WB_Z_BOND_UPP_LIMIT_CNT_HORI_LED; // BE_WB_Z_BOND_UPP_LIMIT_CNT;
	iUpperLimitPosition[3] = 0;

	uiNumPoints[0] = 0;
	uiNumPoints[1] = 0;
	uiNumPoints[2] = 0;
	uiNumPoints[3] = 0;

	uiAxis_CtrlCardId[0] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	uiAxis_CtrlCardId[1] = MAX_CTRL_AXIS_PER_SERVO_BOARD;
	uiAxis_CtrlCardId[2] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // APP_Z_BOND_ACS_ID;
	uiAxis_CtrlCardId[3] = MAX_CTRL_AXIS_PER_SERVO_BOARD; // Dummy Axis

	usAxisAmpPrbs[0] = 1500; //BE_WB_SPECTRUM_TEST_X_PRBS_AMP;
	usAxisAmpPrbs[1] = 1500; //
	usAxisAmpPrbs[2] = BE_WB_SPECTRUM_TEST_Z_PRBS_AMP;
	usAxisAmpPrbs[3] = 0;

}