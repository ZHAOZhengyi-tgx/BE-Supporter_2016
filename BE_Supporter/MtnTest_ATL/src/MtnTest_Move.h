#pragma once


// CMtnTest_Move dialog

#include "resource.h"
#include "MtnApi.h"
#include "afxwin.h"

// Motion Burn-in test
#define MTN_TEST_BURN_IN_FLAG_NONE			  0
#define MTN_TEST_BURN_IN_ONE_AXIS_2_POINTS    1
#define MTN_TEST_BURN_IN_MULTI_AXIS_FIX_PTN   2
#define MTN_TEST_BURN_IN_MULTI_AXIS_RANDOM    3
#define MTN_TEST_BURN_IN_TUNING               4
#define MTN_TEST_CURR_LOOP_STEP_TEST		  5

// 
#define LEN_TEXT  128
#define MAX_NUM_AXES_MOVE_TEST 8

class CMtnTest_Move : public CDialog
{
	DECLARE_DYNAMIC(CMtnTest_Move)

public:
	CMtnTest_Move( CWnd* pParent = NULL);   // standard constructor
	virtual ~CMtnTest_Move();
	virtual void ShowWindow(int nCmdShow);

	HANDLE Handle;				// communication handle
//	unsigned int uiTotalAxes;
	int iAxisTargetPosn_cnt;
	double dRelDistance_mm;
	unsigned int uiTotalNumLoopRelMove; 
	CTRL_PARA_ACS stServoLoopAcsCtrlPara_CurrAxis; // 20090831
	int uiMotionInterDelay_ms[MAX_NUM_AXES_MOVE_TEST];

	CString cstrFbPosition[MAX_NUM_AXES_MOVE_TEST];
	CString cstrPosnErr[MAX_NUM_AXES_MOVE_TEST];
	double pdTimeClock_ms[2];
	CString cstrSamplePeriodLabel;
	CString cstrEditText;
	void InitAxisSetting();
	void UpdateUI_AxisCommandSetting();
	void InitTuneThemeOption();

	void DlgTimerEvent();
	int UI_GetShowWindowFlag();


	void SetUserInterfaceLanguage(int iLanguageOption);
// Dialog Data
	enum { IDD = IDD_MOTION_TEST_DIALOG };

// Thread
	UINT BurnInThread();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void UpdateAxisFeedback();
	void SaveTrajMoveScopeData(double fFirstDist, int nTrajSize, int nTrajSize2);

	virtual void OnClose();

	//UINT_PTR  StartTimer(UINT nIDT_TimerResMacro, UINT TimerDuration);
	//BOOL  StopTimer(UINT_PTR nTimerVal);

	// Related with UI
	void UI_InitDialog();
	void UI_Show_GroupWbSelectTuningPosition(BOOL bFlag);
	void UI_Show_GroupBurnInTest(BOOL bEnableFlag);
	void UI_Show_GroupCurrentLoopTest(BOOL bEnableFlag);
	void UI_Enable_GroupEditAcsParameter(BOOL bEnableFlag);

	void UpdateUShortToEdit(int nResId, unsigned short usValue);
	void UpdateDoubleToEdit(int nResId, double dValue);
	void ReadIntegerFromEdit(int nResId, int *iValue);
	void ReadUnsignedIntegerFromEdit(int nResId, unsigned int *uiValue);
	void ReadUShortFromEdit(int nResId, unsigned short *pusValue);
	void ReadDoubleFromEdit(int nResId, double *pdValue);
	void EnableButtonThreading(BOOL bFlagEnable);
	void EnableSet_AcsParameter(BOOL bFlagEnable);
	BOOL bIsThreadingButtonEnabled;
	void UpdateMovingThreadStatus();

	// ACS Parameter
	void UpdateUI_ServoLoopParameter();
	void DisableAcsParaUI_DuringTuning();
	void EnableAcsParaUI_DuringTuning(BOOL bFlagIsEnable);	BOOL bFlagIsEnabledAxisServoParaACS;
	void UpdateUI_SpeedProfile();

	// Current Loop Step Test
	void InitCurrLoopStepTestUI();
	void CurrentLoopStepMoveTestOneCycle();
	void ClearCurrentCommandAllAxis();

	//////// QC- Burn In
	// Related with Fix Burn In
	void InitFixBurnInGroupTestConfigUI();
	short FixBurnInOneCycle();

	// Related with Random Burn In
	short RandBurnInOneCycle();
	void RunBurnInThread();
	void StopBurnInThread();
	void InitBurnInGroupTestConfigCombo_OneAxis(int nResId, int iAxis);
	void InitRandBurnInGroupTestConfigUI();
	void UpdateIntToEdit(int nResId, int iValue);

	// Servo Tuning,
	void ServoTuneUploadSaveCalcIndex();
	void UpdateAxisTuningStatus();
	void InitComboTuningWB_TableBondHeadMotionIdx();

	// 1-Axis 2-Point Move
	short OneAxis2PointsMoveOneCycle();
	void ProtectionCommandPointOverLimit();

	BOOL m_fStopBurnInThread;
	CWinThread* m_pWinThread;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeTposnAxis1();
	int m_iFbPosition_Axis1;
	int m_iPosnErr;
//	afx_msg void OnTimer(UINT TimerVal);
	afx_msg void OnBnClickedMtnTestEnableMotor1();
	afx_msg void OnBnClickedButton3();
	CString m_strMotorEnaFlag;
	CString m_strFlagMove;
	CString m_strFlagAcc_Axis1;
	CString m_strFlagInPosn_Axis1;
	afx_msg void OnBnClickedUploadDataMoveTest();
	afx_msg void OnCbnSelchangeChanselMoveTestAxis1();
	CComboBox m_combSelectAxis0;
	afx_msg void OnBnClickedButtonMoveRelative();
	afx_msg void OnEnChangeEditRelDist1();
	afx_msg void OnEnChangeEditNumLoopRelMove();
	afx_msg void OnEnChangeEditMoveCmdGenTsMs();

	afx_msg void OnBnClickedButtonMoveTestRandomBurnInStart();
	afx_msg void OnBnClickedButtonMoveTestRandomBurnInStop();
	afx_msg void OnBnClickedCheckMoveTestRandomBurnInAxis1Envolved();
	afx_msg void OnEnChangeEditUppLmtMoveTestRandomBurnInAxis1();
	afx_msg void OnEnChangeEditUppLmtMoveTestRandomBurnInAxis2();
	afx_msg void OnEnChangeEditUppLmtMoveTestRandomBurnInAxis3();
	afx_msg void OnEnChangeEditUppLmtMoveTestRandomBurnInAxis4();
	afx_msg void OnEnChangeEditLowLmtMoveTestRandomBurnInAxis1();
	afx_msg void OnEnChangeEditLowLmtMoveTestRandomBurnInAxis2();
	afx_msg void OnEnChangeEditLowLmtMoveTestRandomBurnInAxis3();
	afx_msg void OnEnChangeEditLowLmtMoveTestRandomBurnInAxis4();
	afx_msg void OnEnChangeEditNumPointsPerCycleMoveTestRandomBurnIn();
	afx_msg void OnEnChangeEditTotalNumCycleMoveTestRandomBurnIn();
	afx_msg void OnBnClickedCheckMoveTestRandomBurnInAxis2Envolved();
	afx_msg void OnBnClickedCheckMoveTestRandomBurnInAxis3Envolved();
	afx_msg void OnBnClickedCheckMoveTestRandomBurnInAxis4Envolved();
	afx_msg void OnCbnSelchangeComboSelMoveTestRandomBurnInAxis1();
	afx_msg void OnCbnSelchangeComboSelMoveTestRandomBurnInAxis2();
	afx_msg void OnCbnSelchangeComboSelMoveTestRandomBurnInAxis3();
	afx_msg void OnCbnSelchangeComboSelMoveTestRandomBurnInAxis4();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnChangeEditFixBurnInWireLength();
	afx_msg void OnEnChangeEditFixBurnInBetweenUnit();
	afx_msg void OnCbnSelchangeComboFixBurnInNumSides();
	afx_msg void OnEnChangeEditFixBurnInNumUnitsPerSide();
	afx_msg void OnBnClickedCheckFixBurnInFlagIsClockWise();
	afx_msg void OnBnClickedButtonTestMoveFixBurnInStart();
	afx_msg void OnEnKillfocusMaxvelAxis1();
	afx_msg void OnEnKillfocusMaxaccAxis1();
	afx_msg void OnEnKillfocusMaxdecAxis1();
	afx_msg void OnEnKillfocusMaxjerkAxis1();
	afx_msg void OnEnKillfocusMaxKillAccAxis1();
	afx_msg void OnEnKillfocusEditAcsSlpkp();
	afx_msg void OnEnKillfocusEditAcsSlvkp();
	afx_msg void OnEnKillfocusEditAcsSlvki();
	afx_msg void OnEnKillfocusEditAcsSlvli();
	afx_msg void OnEnKillfocusEditAcsSof();
	afx_msg void OnEnKillfocusEditAcsSlaff();
	afx_msg void OnEnKillfocusEditMoveTest2PointsPoint1();
	afx_msg void OnEnKillfocusEditMoveTest2PointsPoint2();
	afx_msg void OnEnKillfocusEditMoveTest2PointsTotalIter();
	afx_msg void OnBnClickedButtonStart2PointsMove();
	afx_msg void OnEnKillfocusDwellAxis1();
	afx_msg void OnBnClickedCheckMoveTestSaveScopeDataToFile();
	afx_msg void OnBnClickedCheckMoveTestTuning();
	afx_msg void OnEnKillfocusEditMoveTestServoTunePosnerrSettleTh();
	afx_msg void OnBnClickedButtonMoveTestTuneAccFfc();
	afx_msg void OnBnClickedButtonMoveTestStopTuning();
	afx_msg void OnEnKillfocusEditMoveTestCurrStepUppLevel();
	afx_msg void OnEnKillfocusEditMoveTestCurrStepLowLevel();
	afx_msg void OnEnKillfocusEditMoveTestCurrStepTotalIter();
	afx_msg void OnBnClickedButtonMoveTestCurrStepStart();
	afx_msg void OnCbnSelchangeComboSelTuneWbBondHeadMotionIdx();
	afx_msg void OnCbnSelchangeComboSelTuneWbTableMotionIdx();
	afx_msg void OnEnKillfocusEditWireIdMoveTestGetTunePosnSet();
	afx_msg void OnCbnSelchangeComboTuningThemeOption();
	afx_msg void OnEnKillfocusEditAcsSpJerkff();
	afx_msg void OnEnKillfocusEditAcsSlvnfrq();
	afx_msg void OnEnKillfocusEditAcsSlvnwid();
	afx_msg void OnEnKillfocusEditAcsSlvnatt();
	afx_msg void OnBnClickedCheckAcsEnaNotch();
	afx_msg void OnBnClickedCheckAcsEnaSof();
	afx_msg void OnEnKillfocusEditAcsSldra();
	afx_msg void OnEnKillfocusEditAcsSldrx();
	afx_msg void OnEnKillfocusEditAcsSpKXFromA();
	afx_msg void OnEnKillfocusEditAcsSpKAFromX();
	afx_msg void OnBnClickedCheckShowGroupTestMoveMultiAxisRandBurnIn();
	afx_msg void OnBnClickedCheckShowGroupSetWbTunePosition();
	afx_msg void OnBnClickedCheckMovingTestDlgEnableEditAcsPara();
	afx_msg void OnBnClickedCheckShowMoveTestCurrentLoop();
	afx_msg void OnEnKillfocusEditAcsSlioffs();
	afx_msg void OnEnKillfocusEditAcsSlfrc();
	afx_msg void OnEnKillfocusEditAcsSlfrcd();
//	afx_msg void OnEnKillfocusEditAcsSlpki();
//	afx_msg void OnEnKillfocusEditAcsSlpli();
	afx_msg void OnEnKillfocusEditAcsSlvb0nf();
	afx_msg void OnEnKillfocusEditAcsSlvb0df();
	afx_msg void OnEnKillfocusEditAcsSlvb0nd();
	afx_msg void OnEnKillfocusEditAcsSlvb0dd();
	afx_msg void OnBnClickedCheckAcsEnableBiquadMovingDlg();
};
