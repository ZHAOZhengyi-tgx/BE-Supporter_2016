// 2010Feb05   call by func, remove //extern unsigned int uiCurrMoveAxis_ACS; 

#include "stdafx.h"

#include "MtnInitAcs.h"
#include "MtnTest_Move.h"
// from MtnTest_Move.cpp
unsigned int mtn_test_get_current_moving_axis();

extern double fThresholdSettleTime;
extern void mtn_test_set_burn_in_flag(int iSetFlagBurnIn);
extern int mtn_test_get_2_point_move_1st_pt();
extern int mtn_test_get_2_point_move_2nd_pt();
#include "MtnTune.h"

MTN_TUNE_SEARCH_AFF_INPUT stTuneAffInput;
MTN_TUNE_SEARCH_AFF_OUTPUT stTuneAffOutput;

// IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC
void CMtnTest_Move::OnBnClickedButtonMoveTestTuneAccFfc()
{
	// Disable ParameterACS UI
	DisableAcsParaUI_DuringTuning();
	EnableButtonThreading(FALSE);//	GetDlgItem(IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC)->EnableWindow();

	int iAxisACS_Tuning = mtn_test_get_current_moving_axis();
	// Update the parameters 
	mMotionTuning.mtn_tune_initialize_class_var(fThresholdSettleTime, Handle, iAxisACS_Tuning,uiMotionInterDelay_ms[iAxisACS_Tuning]);
//	mMotionTuning.fThresholdSettleTime = fThresholdSettleTime;
	mMotionTuning.mtn_tune_set_tune_axis(iAxisACS_Tuning);
//	mMotionTuning.iDelayInterMove_ms = uiMotionInterDelay_ms[iAxisACS_Tuning];

	//int idxDlgUI = aiIdxMapFromAxisACS[iAxisACS_Tuning];
	//if(idxDlgUI < MAX_AXIS_ENVOLVE_BURN_IN_TEST)
	//{
// UI: input lower point: iPoint1stIn2PointsOneAxisMove;
// UI: input upper point: iPoint2ndIn2PointsOneAxisMove;
//		mMotionTuning.iMovePoint1 = mtn_test_get_2_point_move_1st_pt();
//		mMotionTuning.iMovePoint2 = mtn_test_get_2_point_move_2nd_pt();
	//}

	// Input through interface
	mtnapi_upload_servo_parameter_acs_per_axis(Handle, iAxisACS_Tuning, &stServoLoopAcsCtrlPara_CurrAxis); // 20090831
	stTuneAffInput.fInitSLAFF = stServoLoopAcsCtrlPara_CurrAxis.dAccelerationFeedforward; // initial value from current one
	stTuneAffInput.fWeightDiffDPE = 1.0;
	stTuneAffInput.fWeightNegativeDPE = 1.0;
	stTuneAffInput.fWeightPositiveDPE = 1.0;
	stTuneAffInput.iMovePoint1 = mtn_test_get_2_point_move_1st_pt();
	stTuneAffInput.iMovePoint2 = mtn_test_get_2_point_move_2nd_pt();
	stTuneAffInput.iFlagEnableDebug = 1; //cFlagSaveScopeDataInFile; // read from IDC_CHECK_MOVE_TEST_SAVE_SCOPE_DATA_TO_FILE

	mtn_test_set_burn_in_flag(MTN_TEST_BURN_IN_TUNING);  // iFlagBurnIn = MTN_TEST_BURN_IN_TUNING;
	stTuneAffOutput.iErrorCode = __MTN_TUNE_SLAFF_ERROR_RET;
	mMotionTuning.mtn_tune_search_aff(&stTuneAffInput, &stTuneAffOutput);

	if(stTuneAffOutput.iErrorCode == __MTN_TUNE_SLAFF_SUCCESS_RET)
	{
		stServoLoopAcsCtrlPara_CurrAxis.dAccelerationFeedforward = stTuneAffOutput.fServoLoopAFF;
//		mtnapi_download_servo_parameter_acs_per_axis(Handle, iAxisACS_Tuning, &stServoLoopAcsCtrlPara_CurrAxis); // 20090831
	}
	else
	{
	}
}

// IDC_BUTTON_MOVE_TEST_STOP_TUNING
void CMtnTest_Move::OnBnClickedButtonMoveTestStopTuning()
{
	if(mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == TRUE)
	{
		mMotionTuning.mtn_tune_stop_tuning_thread_one_group();
	}

	if(mMotionTuning.mtn_tuning_get_flag_doing_thread_one_group() == TRUE)
	{
		mMotionTuning.mtn_tune_aff_stop_groups_thread();
	}

	Sleep(500);
	if(mMotionTuning.mtn_tuning_get_flag_doing_thread_one_group() == FALSE
		&& mMotionTuning.mtn_tune_aff_get_flag_doing_group_thread() == FALSE)
	{
		EnableButtonThreading(TRUE); // GetDlgItem(IDC_BUTTON_MOVE_TEST_TUNE_ACC_FFC)->EnableWindow(TRUE);	//		bIsThreadingButtonEnabled = TRUE;
		EnableAcsParaUI_DuringTuning(TRUE);
	}
}

