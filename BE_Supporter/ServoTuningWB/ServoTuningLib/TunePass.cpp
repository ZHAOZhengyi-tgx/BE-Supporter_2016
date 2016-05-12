
#include "stdafx.h"

#include "mtndefin.h"

const int PASSWORD_1 = 96685352;
const int PASSWORD_2 = 666888;
const int PASSWORD_3 = 661644;

int wb_mtn_tester_get_password_sg_8d_zzy()
{
	return PASSWORD_1;
}

int wb_mtn_tester_get_password_brightlux_6d_gxc()
{
	return PASSWORD_2;
}

int wb_mtn_tester_get_password_brightlux_6d_zzy()
{
	return PASSWORD_3;
}

int iFlagLanguage = LANGUAGE_UI_CN;  // Default
int get_sys_language_option()
{
	return iFlagLanguage;
}

int set_sys_language_option(int iFlag)
{
	int iRet = MTN_API_OK_ZERO;
	int iTempFlag;
	if(iFlag > LANGUAGE_UI_CN)
	{
		iTempFlag = LANGUAGE_UI_CN;
		iRet = MTN_API_ERROR;
	}
	else if (iFlag < LANGUAGE_UI_EN)
	{
		iTempFlag = LANGUAGE_UI_EN;
		iRet = MTN_API_ERROR;
	}
	else
	{
		iTempFlag = iFlag;
	}
	iFlagLanguage = iTempFlag;

	return iRet;
}

static char cSystemDebugLevel = __SYSTEM_PROTECTING_OPERATOR__;
char get_sys_pass_protect_level()
{
	return cSystemDebugLevel;
}
void set_sys_pass_protect_level(char cSysPassLevel)
{
	cSystemDebugLevel = cSysPassLevel;
}


int wb_security_calc_eng_pass_from_service(int iServicePass)
{
	int iTemp, iTemp1d, iTemp2d, iTemp3d, iTemp4d;

	iTemp = iServicePass + 8;
	iTemp1d = iTemp % 10;
	iTemp2d = ((iTemp - iTemp1d) % 100) / 10;
	iTemp3d = (((iTemp - iTemp1d - iTemp2d * 10) % 1000))/100;
	iTemp4d = (iTemp - iTemp1d - iTemp2d * 10 - iTemp3d * 100)/1000;

	int iPasswordEngineer;
	iPasswordEngineer = iTemp4d + iTemp2d * 10 + iTemp3d * 100 + iTemp1d * 1000;

	return iPasswordEngineer;
}
