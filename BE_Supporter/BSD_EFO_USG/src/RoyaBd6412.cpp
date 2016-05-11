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

// 20091113 reduce time_out from 20 sec to 3 sec, CANNOT be less
// 20091210 add semaphore s.t. at any time there is only one communication between host and roya-dsp board. delay between send command to and get reply from Roya-BSD box
//			later each time of eth command, there must be a pair of roya_6412_send_to(int iDataLen) and roya_clear_flag_hold_commu_eth()
// 20101014 Semaphore of holding ETH, by MACRO
// 20100119 Change ADC to 16 bit short
// 20100217 Consolidate checking ETH before each time of communication, debug for sizeof(BSD_STATUS)
// 20100223 Bind Socket
#include "stdafx.h"
#include "RTC_Roya_bd_6412.h"
#include "mtndefin.h"
#include "RoyaBd6412.h"
#include "BSD_API.h"
#include "rt_clock_def.h"
#include "ROYA_DEF.h"  // 20100120

#define  BSD_COMMU_DELAY_GET_REPLY    5
#define  BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS    3

static ROYA_ETH_INFO stRoyaEth;

char strSendBuffer[_RTC_ETH_MAX_BUFF_LEN_BYTE];
char strReceiveBuffer[_RTC_ETH_MAX_BUFF_LEN_BYTE];
int RecvfromNetwork(SOCKET S,SOCKADDR_IN Remote, char * rBuf);
int roya_6412_send_to(int iDataLen);
void roya_set_flag_hold_commu_eth();
void roya_clear_flag_hold_commu_eth();

extern unsigned int uiDataAdcBSD[_BSD_MAX_BLK_PARAMETER_][_BSD_MAX_DATA_LEN_];
//#define __BINDING_ROYA_ETH__

static int iFlagNumByteADC = ROYA_BSD_ADC_2_BYTE;
static int iFlagRealTimeClockFormat = ROYA_BSD_YEAR_4_BYTE;

static int iFlagNumDAC_bit = ROYA_BSD_DAC_12b;
static int iFlagPolarityADC = ROYA_BSD_ADC_POLARITY_SINGLE;

void roya_6412_set_flag_dac_bit(int iFlag)
{
	iFlagNumDAC_bit = iFlag;
}
int roya_6412_get_flag_dac_bit()
{
	return iFlagNumDAC_bit;
}

void roya_6412_set_flag_adc_byte(int iFlag)
{
	iFlagNumByteADC = iFlag;
}

void roya_6412_set_flag_real_time_clock_year_byte(int iFlag)
{
	iFlagRealTimeClockFormat = iFlag;
}

int roya_6412_get_flag_adc_byte()
{
	return iFlagNumByteADC;
}

int roya_6412_get_flag_real_time_clock_year_byte()
{
	return iFlagRealTimeClockFormat;
}

void roya_6412_set_flag_adc_polarity(int iFlag)
{
	iFlagPolarityADC = iFlag;
}

int roya_6412_get_flag_adc_polarity()
{
	return iFlagPolarityADC;
}

short roya_6412_init_eth_socket(char *strLocalIP, char *strRemoteIP)
{
	sprintf_s(stRoyaEth.strLocalIP, 16, "%s", strLocalIP);
	sprintf_s(stRoyaEth.strRemoteIP, 16, "%s", strRemoteIP);

	if (WSAStartup(MAKEWORD(1,1),&(stRoyaEth.wsd)) !=0)
	{
		return MTN_ROYA_WSA_STARTUP_ERR;
	}
	
	if (WSAStartup(MAKEWORD(2,2),&(stRoyaEth.wsd)) !=0)
	{
		return MTN_ROYA_WSA_STARTUP_ERR;
	}
	
	stRoyaEth.stSocket =socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if (stRoyaEth.stSocket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("本机网络故障,或者防火墙遮挡,或者IP错误,或者网络端口3800被占用！"));
		return MTN_ROYA_GET_SOCKET_ERR;
	}

	char * tempstrIP = (char *)LPCSTR(stRoyaEth.strLocalIP);
	unsigned int  s_Port = 3800;

	stRoyaEth.Local.sin_family=AF_INET;
	stRoyaEth.Local.sin_port= htons(s_Port);
	stRoyaEth.Local.sin_addr.S_un.S_addr= inet_addr(tempstrIP);
		

	// setup remote IP
	tempstrIP = (char *)LPCSTR(stRoyaEth.strRemoteIP);
	stRoyaEth.Remote.sin_family=AF_INET;
	stRoyaEth.Remote.sin_port= htons(s_Port);
	stRoyaEth.Remote.sin_addr.S_un.S_addr= inet_addr(tempstrIP); 

	// bind socket, 20100223
#ifdef  __BINDING_ROYA_ETH__
	stRoyaEth.iRoyaEthSocketBind = bind (stRoyaEth.stSocket,  (struct sockaddr *) &stRoyaEth.Local, sizeof(stRoyaEth.Local));
	if(stRoyaEth.iRoyaEthSocketBind<0) 
	{
		CString cstrTemp;
		cstrTemp.Format("Server: cannot bind port");
		AfxMessageBox(cstrTemp);
		return MTN_ROYA_ETH_UDP_BIND_ERROR;
    }
#endif // __BINDING_ROYA_ETH__

	// Initialization, 
	efo_api_init();
	bsd_api_check_wire_spool_motor_init();

	short sRet =  MTN_API_OK_ZERO;
	// Get version
	VERSION_INFO stRoyaVer;
	sRet = bsd_api_get_version_roya_6412(&stRoyaVer); // roya_6412_get_version(&stRoyaVer); 20100403

 	return sRet; // MTN_API_OK_ZERO;
}

// 20091210
static int iFlagRoyaEthCommuHold;
void roya_set_flag_hold_commu_eth()
{
	iFlagRoyaEthCommuHold = 1;
}

#define ROYA_SET_FLAG_HOLD_COMMU_ETH()  {iFlagRoyaEthCommuHold = 1;}

void roya_clear_flag_hold_commu_eth()
{
	iFlagRoyaEthCommuHold = 0;
}

int roya_6412_send_to(int iDataLen)
{
	int iSentLen;
	while(iFlagRoyaEthCommuHold == 1)
	{
		Sleep(1);
	}
	ROYA_SET_FLAG_HOLD_COMMU_ETH()    // use MACRO instead of roya_set_flag_hold_commu_eth();  20101014

	iSentLen = sendto(stRoyaEth.stSocket,strSendBuffer, iDataLen, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	iSentLen = send(stRoyaEth.stSocket, strSendBuffer, iDataLen, 0);

	return iSentLen;
} // 20091210

///  Check socket, used each time before communication
static 	BYTE TTL = (BYTE) 8;
short roya_6412_check_eth_socket()
{
	short sRet = MTN_API_OK_ZERO;
	//单播
#ifdef __BINDING_ROYA_ETH__
	if(stRoyaEth.iRoyaEthSocketBind<0) // 20100223
	{
#endif // __BINDING_ROYA_ETH__

#ifdef __CHECK_BY_SET_SOCKOPT__
		int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
		if (ret!=0)
		{
			Sleep(10);
			ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE)); // Check once more
			if (ret!=0)
			{
				sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
			}
		}
#endif  // __CHECK_BY_SET_SOCKOPT__
#ifdef __BINDING_ROYA_ETH__
	}
#endif // __BINDING_ROYA_ETH__
	return sRet;
}

static struct  fd_set  stFdSet_Roya6412; 
static struct  timeval FAR stEthTimeOut_Roya6412 = {1,0}; // 20091210 reduce time_out from 20 sec to 1 sec, CANNOT be less
int RecvfromNetwork(SOCKET S,SOCKADDR_IN Remote, char * rBuf)
{
//接收	
		unsigned int nLength = 0;
		int	nRecvAddrLen = sizeof(Remote);
		FD_ZERO(&stFdSet_Roya6412); //每次循环都要清空集合，否则不能检测描述符变化
		FD_SET(S,&stFdSet_Roya6412); //添加描述符 
		int sRetSelect = select((int)S+1,&stFdSet_Roya6412,  NULL, NULL,&stEthTimeOut_Roya6412);
		switch(sRetSelect) //select使用 
		{ 

			case -1: exit(-1);break; //select错误，退出程序 
			case 0:break; //再次轮询, Time-out
			default:
					ULONG bytes;
					if(ioctlsocket(S, FIONREAD, &bytes)!=0) {
						break;
					}

					if(FD_ISSET(S, &stFdSet_Roya6412)) //测试sock是否可读，即是否网络上有数据
					{ 		
//						nLength = recvfrom(S, (char*)rBuf, bytes, 0, (SOCKADDR *) &Remote, &nRecvAddrLen);
						nLength = recv(S, (char*)rBuf, bytes, 0);
						if (nLength > 0)
							break;
					}// end if break; 
				break;

		}// end switch
	return nLength;
}

// 20100310
int RecvfromNetwork_w_TimeOut(SOCKET S,SOCKADDR_IN Remote, char * rBuf, int iTimeOutSec)
{
//接收	
		static struct  timeval FAR stEthTimeOut;
		unsigned int nLength = 0;
		int	nRecvAddrLen = sizeof(Remote);

		stEthTimeOut.tv_sec = iTimeOutSec;
		FD_ZERO(&stFdSet_Roya6412); //每次循环都要清空集合，否则不能检测描述符变化
		FD_SET(S,&stFdSet_Roya6412); //添加描述符 
		int sRetSelect = select((int)S+1,&stFdSet_Roya6412,  NULL, NULL,&stEthTimeOut_Roya6412);
		switch(sRetSelect) //select使用 
		{ 

			case -1: exit(-1);break; //select错误，退出程序 
			case 0:break; //再次轮询, Time-out
			default:
					ULONG bytes;
					if(ioctlsocket(S, FIONREAD, &bytes)!=0) {
						break;
					}

					if(FD_ISSET(S, &stFdSet_Roya6412)) //测试sock是否可读，即是否网络上有数据
					{ 		
						nLength = recvfrom(S, (char*)rBuf, bytes, 0, (SOCKADDR *) &Remote, &nRecvAddrLen);
						if (nLength > 0)
							break;
					}// end if break; 
				break;

		}// end switch
	return nLength;
}


////////////////////////////////////////////////////////////////////
// ROYA, BSD applications (Non-Stick Detection)
short roya_6412_bsd_set_parameters(BSD_PARAMETER *stpBSD_Para, unsigned char ucBlkBSD)
{
	short sRet = MTN_API_OK_ZERO;
	int ii;
	
	union 
	{
		BSD_PARAMETER stParameterBSD;
		char cByteOut[sizeof(BSD_PARAMETER)];
	}uConvertBSDToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_SET_PARAMETER;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1 + sizeof(BSD_PARAMETER);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	uConvertBSDToByte.stParameterBSD.cAdcIdx			 = stpBSD_Para->cAdcIdx;
	uConvertBSDToByte.stParameterBSD.cDacIdx			 = stpBSD_Para->cDacIdx;
	uConvertBSDToByte.stParameterBSD.cDetectFactorIsr	 = stpBSD_Para->cDetectFactorIsr;
	uConvertBSDToByte.stParameterBSD.usDetectSampleLength = stpBSD_Para->usDetectSampleLength;
	uConvertBSDToByte.stParameterBSD.uiDacValue			 = stpBSD_Para->uiDacValue;
	uConvertBSDToByte.stParameterBSD.cDataBufferIdx      = stpBSD_Para->cDataBufferIdx;
	uConvertBSDToByte.stParameterBSD.cNextParaBlkIdx     = stpBSD_Para->cNextParaBlkIdx;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = ucBlkBSD;
	for(ii = 0; ii < sizeof(BSD_PARAMETER) ; ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii] = uConvertBSDToByte.cByteOut[ii];
	}

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1 + ii); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1 + ii, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;

}

short roya_6412_bsd_get_parameters(BSD_PARAMETER *stpBSD_Para, unsigned char ucBlkBSD)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		BSD_PARAMETER stParameterBSD;
		char cByteOut[sizeof(BSD_PARAMETER)];
	}uConvertBSDToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_GET_PARAMETER;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1 + _RTC_ROYA_OFST_DATA_START;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = ucBlkBSD;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(BSD_PARAMETER) + 6) )
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(BSD_PARAMETER); ii++)
		{
			uConvertBSDToByte.cByteOut[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii];
		}
		stpBSD_Para->cAdcIdx             = uConvertBSDToByte.stParameterBSD.cAdcIdx;
		stpBSD_Para->cDacIdx             = uConvertBSDToByte.stParameterBSD.cDacIdx;
		stpBSD_Para->cDetectFactorIsr    = uConvertBSDToByte.stParameterBSD.cDetectFactorIsr;
		stpBSD_Para->usDetectSampleLength = uConvertBSDToByte.stParameterBSD.usDetectSampleLength;
		stpBSD_Para->uiDacValue          = uConvertBSDToByte.stParameterBSD.uiDacValue;
		stpBSD_Para->cDataBufferIdx      = uConvertBSDToByte.stParameterBSD.cDataBufferIdx;
		stpBSD_Para->cNextParaBlkIdx     = uConvertBSDToByte.stParameterBSD.cNextParaBlkIdx;
	}

	return sRet;
}

short roya_6412_bsd_start_detect(unsigned char ucBlkIdx)
{
	short sRet = MTN_API_OK_ZERO;
#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_START_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = ucBlkIdx;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_bsd_warm_reset()
{
	short sRet = MTN_API_OK_ZERO;
#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_WARM_RESET;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_bsd_get_status(BSD_STATUS *pstStatusBSD)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_GET_STATUS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	int nExpectRetLen =  3 * sizeof(char) + sizeof(unsigned int) + sizeof(unsigned short); // sizeof(BSD_STATUS); // 20100217
	nExpectRetLen += _RTC_ROYA_OFST_DATA_START;
	if(nLength < nExpectRetLen)
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		union
		{
			unsigned int uiDataBSD;
			char cByte[sizeof(int)];
		}uConvertByteToInt;
		union
		{	unsigned short usDataBSD;
			char cByte[sizeof(Uint16)];
		}uConvertShort2Byte;

		uConvertShort2Byte.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START ];
		uConvertShort2Byte.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];

		pstStatusBSD->usBsdSampleCnt = uConvertShort2Byte.usDataBSD;

		pstStatusBSD ->cFlagStartBSD = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 2];
		pstStatusBSD ->cBlkIdxBSD = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 3];
		pstStatusBSD ->cFlagTriggerBSD = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 4];

		uConvertByteToInt.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 5];
		uConvertByteToInt.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 6];
		uConvertByteToInt.cByte[2] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 7];
		uConvertByteToInt.cByte[3] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 8];
		pstStatusBSD ->uiTrigCnt = uConvertByteToInt.uiDataBSD;

	}

	return sRet;
}


short roya_6412_get_efo_good_flag(unsigned int *pEfoGoodFlag)
{
short sRet = MTN_API_OK_ZERO;
union
{
	unsigned int uiEfoGoodFlag;
	char cByte[sizeof(int)];
}uConvertIntByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_EFO_GET_GOOD_FLAG;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(int) + _RTC_ROYA_OFST_DATA_START))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		uConvertIntByte.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START ];
		uConvertIntByte.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
		uConvertIntByte.cByte[2] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 2];
		uConvertIntByte.cByte[3] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 3];
		*pEfoGoodFlag = uConvertIntByte.uiEfoGoodFlag;
	}

	return sRet;
}

/////////////////////////////////////////////////////////////////////
/// Communicate Related
/////////////////////////////////////////////////////////////////////
short roya_6412_test_communication(char *strSendChar, char cLen)
{
//	CString  strSended;
	short sRet;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_TEST_COMMUNICATION;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = cLen;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	for(int ii = 0; ii< cLen; ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + ii] = strSendChar[ii];
	}
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + cLen] = '\0';

//	strSended = _T(strSendBuffer);

	int intSize = _RTC_ROYA_OFST_DATA_START + cLen; 
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	char * Sended =new char(intSize);
   //发送到DSP, (char * ) LPCSTR(strSended)
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
   
   if (retsenSize > 0)
   {
		memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
		//从DSP接收刚才发送的内容
//		Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
		int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
		strcpy_s(Sended, intSize, strSendBuffer); //(char * ) LPCSTR(strSended));
		//比较, strnicmp
		if  ((nLength > 0) && (!_strnicmp(strReceiveBuffer,(char *)Sended,256)))
		{
			sRet = MTN_API_OK_ZERO;
		}else
		{
			sRet = MTN_ROYA_GET_WRONG_DATA;
		}
   }
   else
   {
	   sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
//		AfxMessageBox("通讯失败！");
   }
	roya_clear_flag_hold_commu_eth(); // 20091210

//	closesocket(stRoyaEth.stSocket);
	WSACleanup();

	return sRet;
}

short roya_6412_get_version(VERSION_INFO *stpVersionRoya6412)
{
	short sRet = MTN_API_OK_ZERO;
	
	union 
	{
		VERSION_INFO stVerOut;
		char cByteIn[sizeof(VERSION_INFO)];
	}uConvertVersionToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_GET_VERSION;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = '\0';

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(VERSION_INFO) + _RTC_ROYA_OFST_DATA_START))
	{
		return MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(VERSION_INFO); ii++)
		{
			uConvertVersionToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}

		stpVersionRoya6412->usVerMajor = uConvertVersionToByte.stVerOut.usVerMajor;
		stpVersionRoya6412->usVerMinor = uConvertVersionToByte.stVerOut.usVerMinor;
		stpVersionRoya6412->usVerYear = uConvertVersionToByte.stVerOut.usVerYear;
		stpVersionRoya6412->usVerMonth = uConvertVersionToByte.stVerOut.usVerMonth;
		stpVersionRoya6412->usVerDate = uConvertVersionToByte.stVerOut.usVerDate;
	}
	return sRet;
}

short roya_6412_comm_test_set_lcd(char cByteLCD)
{
	short sRet = MTN_API_OK_ZERO;
#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_TEST_DIGITAL_OUT_LCD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = cByteLCD;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = '\0';

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return MTN_API_OK_ZERO;

}

short roya_6412_comm_test_set_dac(unsigned int uiDacIdx, unsigned int uiDacValue)
{
	union
	{
		unsigned int uiDacValue;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
    int ii;
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	uConvertIntToByte.uiDacValue = uiDacValue;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_TEST_DAC;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1 + sizeof(int);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiDacIdx;
	for( ii=0; ii< sizeof(int); ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii] = uConvertIntToByte.cByteIn[ii];
	}

	unsigned int uiNumSendByte = ii + 1 + _RTC_ROYA_OFST_DATA_START;
	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(uiNumSendByte); // sendto(stRoyaEth.stSocket,strSendBuffer, uiNumSendByte, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return MTN_API_OK_ZERO;

}

short roya_6412_comm_test_get_adc_4B(unsigned int *uiAdcValue, short nLength)
{
short sRet = MTN_API_OK_ZERO;
union 
{
	int uiAdcValue; // 20100119, unsigned int uiAdcValue;
	char cByteIn[sizeof(int)];
}uConvertIntToByte;

	if(nLength < (sizeof(int) + 1 + _RTC_ROYA_OFST_DATA_START))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(int); ii++)  // 20100119
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii];
		}
		*uiAdcValue = (unsigned int)uConvertIntToByte.uiAdcValue;
	}

	return sRet;
}

short roya_6412_comm_test_get_adc_2B(unsigned int *uiAdcValue, short nLength)
{
short sRet = MTN_API_OK_ZERO;
union 
{
	unsigned short usAdcValue; // 20100119, unsigned int uiAdcValue, 20110822
	char cByteIn[sizeof(short)];
}uConvertIntToByte;

	if(nLength < (sizeof(short) + 1 + _RTC_ROYA_OFST_DATA_START))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(short); ii++)  // 20100119
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii];
		}
		*uiAdcValue = (unsigned int)(uConvertIntToByte.usAdcValue);  // 20110822
	}

	return sRet;
}

short roya_6412_comm_test_get_adc(unsigned int uiAdcIdx, unsigned int *uiAdcValue)
{
	short sRet = MTN_API_OK_ZERO;
	

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_TEST_ADC;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiAdcIdx;

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

static int nLength;
	nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210


	if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)
	{
		sRet = roya_6412_comm_test_get_adc_4B(uiAdcValue, nLength);
	}
	else 
	{
		sRet = roya_6412_comm_test_get_adc_2B(uiAdcValue, nLength);
	}

	return sRet;
}

short roya_6412_comm_test_set_timer_period(char cTimerId, unsigned int uiTimerPeriod)
{
	union 
	{
		unsigned int uiTimerPeriod;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	uConvertIntToByte.uiTimerPeriod = uiTimerPeriod;
	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_SET_TIMER_PERIOD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 5;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = cTimerId;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = uConvertIntToByte.cByteIn[3];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+5); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return MTN_API_OK_ZERO;

}


short roya_6412_comm_test_get_timer_period_clock_cnt(char cTimerId, unsigned int *uiTimerPeriod)
{
	short sRet = MTN_API_OK_ZERO;
	union 
	{
		unsigned int uiTimerPeriod;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_GET_TIMER_PERIOD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = cTimerId;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
	roya_clear_flag_hold_commu_eth(); // 20091210

   if (retsenSize > 0)
   {
		memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
		//从DSP接收刚才发送的内容
		int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
		if(nLength >= sizeof(int) + _RTC_ROYA_OFST_DATA_START)
		{
			for(int ii=0; ii<sizeof(int); ii++)
			{
				uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
			}
			*uiTimerPeriod = uConvertIntToByte.uiTimerPeriod;
		}
		else
		{
		   sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
		}
   }
   else
   {
	   sRet = MTN_ROYA_COMM_SENT_LEN_ERROR;
   }

	return sRet;

}


typedef struct rtc_tm_year2B {  
        unsigned short  year; /* xxxx 4 digits :) // 20100311 */                          
        unsigned short   secs;                                                 
        unsigned short   mins;                                                 
        unsigned short   hours;                                                
        unsigned short   mday;                                                 
        unsigned short   mon;   
        unsigned short   wday;                                                 
        unsigned short   vl;     
}RTC_TM_YEAR2B;
short roya_6412_get_rt_clock_year2B(RTC_TM *stpRealTimeClock, unsigned short nLength)
{
union
{
	RTC_TM_YEAR2B stRealTimeClock;
	char cByte[sizeof(RTC_TM_YEAR2B)];
}uConvertRTC_TM_Byte;
static short sRet = MTN_API_OK_ZERO;

	if(nLength >= sizeof(RTC_TM_YEAR2B) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(RTC_TM_YEAR2B); ii++)
		{
			uConvertRTC_TM_Byte.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		stpRealTimeClock->year = uConvertRTC_TM_Byte.stRealTimeClock.year;
		stpRealTimeClock->mon = uConvertRTC_TM_Byte.stRealTimeClock.mon;
		stpRealTimeClock->mday = uConvertRTC_TM_Byte.stRealTimeClock.mday;
		stpRealTimeClock->wday = uConvertRTC_TM_Byte.stRealTimeClock.wday;
		stpRealTimeClock->hours = uConvertRTC_TM_Byte.stRealTimeClock.hours;
		stpRealTimeClock->mins = uConvertRTC_TM_Byte.stRealTimeClock.mins;
		stpRealTimeClock->secs = uConvertRTC_TM_Byte.stRealTimeClock.secs;
	}
	else
	{
	   sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	return sRet;
}


short roya_6412_get_rt_clock_year4B(RTC_TM *stpRealTimeClock, unsigned short nLength)
{
union
{
	RTC_TM stRealTimeClock;
	char cByte[sizeof(RTC_TM)];
}uConvertRTC_TM_Byte;
static short sRet = MTN_API_OK_ZERO;

	if(nLength >= sizeof(RTC_TM) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(RTC_TM); ii++)
		{
			uConvertRTC_TM_Byte.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		stpRealTimeClock->year = uConvertRTC_TM_Byte.stRealTimeClock.year;
		stpRealTimeClock->mon = uConvertRTC_TM_Byte.stRealTimeClock.mon;
		stpRealTimeClock->mday = uConvertRTC_TM_Byte.stRealTimeClock.mday;
		stpRealTimeClock->wday = uConvertRTC_TM_Byte.stRealTimeClock.wday;
		stpRealTimeClock->hours = uConvertRTC_TM_Byte.stRealTimeClock.hours;
		stpRealTimeClock->mins = uConvertRTC_TM_Byte.stRealTimeClock.mins;
		stpRealTimeClock->secs = uConvertRTC_TM_Byte.stRealTimeClock.secs;
	}
	else
	{
	   sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	return sRet;
}

short roya_6412_get_rt_clock(RTC_TM *stpRealTimeClock)
{
short sRet= MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	//单播
BYTE TTL = (BYTE) 8;
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_GET_RT_CLOCK;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int intSize = _RTC_ROYA_OFST_DATA_START; 
	char * Sended =new char(intSize);
   //发送到DSP, (char * ) LPCSTR(strSended)
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
   
	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(iFlagRealTimeClockFormat == ROYA_BSD_YEAR_2_BYTE)
	{
		sRet = roya_6412_get_rt_clock_year2B(stpRealTimeClock, nLength);
	}
	else
	{
		sRet = roya_6412_get_rt_clock_year4B(stpRealTimeClock, nLength);
	}

	return sRet;
}

short roya_6412_set_rt_clock(RTC_TM *stpRealTimeClock)
{
short sRet = MTN_API_OK_ZERO;
BYTE TTL = (BYTE) 8;
union
{
	RTC_TM stRealTimeClock;
	char cByte[sizeof(RTC_TM)];
}uConvertRTC_TM_Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	uConvertRTC_TM_Byte.stRealTimeClock.year = stpRealTimeClock->year;
	uConvertRTC_TM_Byte.stRealTimeClock.mon = stpRealTimeClock->mon;
	uConvertRTC_TM_Byte.stRealTimeClock.mday = stpRealTimeClock->mday;
	uConvertRTC_TM_Byte.stRealTimeClock.hours = stpRealTimeClock->hours;
	uConvertRTC_TM_Byte.stRealTimeClock.mins = stpRealTimeClock->mins;
	uConvertRTC_TM_Byte.stRealTimeClock.secs = stpRealTimeClock->secs;
	uConvertRTC_TM_Byte.stRealTimeClock.wday = stpRealTimeClock->wday;
	uConvertRTC_TM_Byte.stRealTimeClock.vl = stpRealTimeClock->vl;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_SET_RT_CLOCK;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(RTC_TM);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	for(int ii=0; ii<sizeof(RTC_TM); ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + ii] = uConvertRTC_TM_Byte.cByte[ii];
	}

	int intSize = _RTC_ROYA_OFST_DATA_START + sizeof(RTC_TM); 
   //发送到DSP
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
	roya_clear_flag_hold_commu_eth(); // 20091210
   
	return sRet;
}

short roya_6412_set_rt_clock_year2B(RTC_TM *stpRealTimeClock)
{
short sRet = MTN_API_OK_ZERO;
BYTE TTL = (BYTE) 8;
union
{
	RTC_TM_YEAR2B stRealTimeClock;
	char cByte[sizeof(RTC_TM_YEAR2B)];
}uConvertRTC_TM_Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	uConvertRTC_TM_Byte.stRealTimeClock.year = (unsigned short)(stpRealTimeClock->year);
	uConvertRTC_TM_Byte.stRealTimeClock.mon = stpRealTimeClock->mon;
	uConvertRTC_TM_Byte.stRealTimeClock.mday = stpRealTimeClock->mday;
	uConvertRTC_TM_Byte.stRealTimeClock.hours = stpRealTimeClock->hours;
	uConvertRTC_TM_Byte.stRealTimeClock.mins = stpRealTimeClock->mins;
	uConvertRTC_TM_Byte.stRealTimeClock.secs = stpRealTimeClock->secs;
	uConvertRTC_TM_Byte.stRealTimeClock.wday = stpRealTimeClock->wday;
	uConvertRTC_TM_Byte.stRealTimeClock.vl = stpRealTimeClock->vl;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_SET_RT_CLOCK;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(RTC_TM);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	for(int ii=0; ii<sizeof(RTC_TM_YEAR2B); ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + ii] = uConvertRTC_TM_Byte.cByte[ii];
	}

	int intSize = _RTC_ROYA_OFST_DATA_START + sizeof(RTC_TM_YEAR2B); 
   //发送到DSP
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
	roya_clear_flag_hold_commu_eth(); // 20091210
   
	return sRet;
}

// 20100127, Get Key Interrupt Counter
short roya_6412_get_key_interrupt_counter(KEY_INTERRUPT_COUNTER *stpRoyaKeyIntCounter)
{
short sRet= MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_GET_KEY_INT_COUNTER;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int intSize = _RTC_ROYA_OFST_DATA_START; 
	// char * Sended =new char(intSize);
   //发送到DSP, (char * ) LPCSTR(strSended)
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
   
	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

union
{
	KEY_INTERRUPT_COUNTER stRoyaKeyIntCounter;
	char cByte[sizeof(KEY_INTERRUPT_COUNTER)];
}uConvertKeyIntCounter2Char;

	if(nLength >= sizeof(KEY_INTERRUPT_COUNTER) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(KEY_INTERRUPT_COUNTER); ii++)
		{
			uConvertKeyIntCounter2Char.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		stpRoyaKeyIntCounter->uiCounterKeyBSD = uConvertKeyIntCounter2Char.stRoyaKeyIntCounter.uiCounterKeyBSD;
		stpRoyaKeyIntCounter->uiWireFeedSensorTriggerCounter = uConvertKeyIntCounter2Char.stRoyaKeyIntCounter.uiWireFeedSensorTriggerCounter;
		stpRoyaKeyIntCounter->uiCounterKeyInt = uConvertKeyIntCounter2Char.stRoyaKeyIntCounter.uiCounterKeyInt;

	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	
	
	return sRet;
}

short roya_6412_read_flash(unsigned int uiAddressFlashRelative, unsigned char ucReadLen, unsigned char *strReadBuffer)
{
short sRet= MTN_API_OK_ZERO;
union
{
	unsigned int uiAddressFlashRelative;
	char cByte[sizeof(int)];
}uConvertInt_Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	//单播
BYTE TTL = (BYTE) 8;
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_READ_FLASH;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) + 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	uConvertInt_Byte.uiAddressFlashRelative = uiAddressFlashRelative;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertInt_Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertInt_Byte.cByte[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertInt_Byte.cByte[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertInt_Byte.cByte[3];

	if(ucReadLen > __MAX_FLASH_READ_LEN)
	{
		ucReadLen = __MAX_FLASH_READ_LEN;
	}

	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = ucReadLen;

	int intSize = _RTC_ROYA_OFST_DATA_START + sizeof(int) + 1; 
//	char * Sended =new char(intSize);
   //发送到DSP, (char * ) LPCSTR(strSended)
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210
   
	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength >= (ucReadLen + _RTC_ROYA_OFST_DATA_START + 1))
	{
		for(int ii=0; ii< ucReadLen; ii++)
		{
			strReadBuffer[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii + 1];
		}
	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	
	return sRet;

}

short roya_6412_write_flash(unsigned int uiAddressFlashRelative, unsigned char ucWriteLen, unsigned char *strWriteBuffer)
{
short sRet= MTN_API_OK_ZERO;
union
{
	unsigned int uiAddressFlashRelative;
	char cByte[sizeof(int)];
}uConvertInt_Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	//单播
BYTE TTL = (BYTE) 8;
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_TEST_COMM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_WRITE_FLASH;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) + 1 + ucWriteLen;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	uConvertInt_Byte.uiAddressFlashRelative = uiAddressFlashRelative;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertInt_Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertInt_Byte.cByte[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertInt_Byte.cByte[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertInt_Byte.cByte[3];

	if(ucWriteLen > __MAX_FLASH_READ_LEN)
	{
		ucWriteLen = __MAX_FLASH_READ_LEN;
	}

	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = ucWriteLen;

	for(int ii =0; ii<ucWriteLen; ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + 5 + ii] = strWriteBuffer[ii];
	}
	int intSize = _RTC_ROYA_OFST_DATA_START + sizeof(int) + 1 + ucWriteLen; 
//	char * Sended =new char(intSize);
   //发送到DSP, (char * ) LPCSTR(strSended)
	int retsenSize = roya_6412_send_to(intSize); // sendto(stRoyaEth.stSocket, strSendBuffer, intSize, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));	
	Sleep(500); // Must fail if <= 300, cannot always start if 500
	int nLength = RecvfromNetwork_w_TimeOut(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer, 10); // 10 second timeout

	if(nLength >= (_RTC_ROYA_OFST_DATA_START + 1))
	{
		unsigned char cWriteFlashLen;
		cWriteFlashLen = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];

		if(cWriteFlashLen != ucWriteLen)
		{
//			Sleep(500);
//			nLength = RecvfromNetwork_w_TimeOut(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer, 10); // 10 second timeout
//			cWriteFlashLen = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
			sRet = MTN_ROYA_ETH_WRITE_FLASH_ERR;

		}

	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	
	roya_clear_flag_hold_commu_eth(); // 20091210
	
	return sRet;

}
/////////////////////////////////////////////////////////////////////
/// USG Related: Ultra-Sonic Generator
/////////////////////////////////////////////////////////////////////

short roya_6412_usg_prof_set_blk_seg(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg)
{
	short sRet = MTN_API_OK_ZERO;
	int ii;
	
	union 
	{
		USG_UNIT_SEG stUsgUnitSegIn;
		char cByteOut[sizeof(USG_UNIT_SEG)];
	}uConvertUsgToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_SET_SEGMENT;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 2 + sizeof(USG_UNIT_SEG);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	uConvertUsgToByte.stUsgUnitSegIn.iUSG_Amplitude = stpUSG_Seg->iUSG_Amplitude;
	uConvertUsgToByte.stUsgUnitSegIn.iUSG_Duration_ms = stpUSG_Seg->iUSG_Duration_ms;
	uConvertUsgToByte.stUsgUnitSegIn.iUSG_RampTime = stpUSG_Seg->iUSG_RampTime;
	uConvertUsgToByte.stUsgUnitSegIn.iUSG_TriggerPattern = stpUSG_Seg->iUSG_TriggerPattern;
	uConvertUsgToByte.stUsgUnitSegIn.uiCmdType = stpUSG_Seg->uiCmdType;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = (char) uiSeg;
	for(ii = 0; ii < sizeof(USG_UNIT_SEG) ; ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + 2 + ii] = uConvertUsgToByte.cByteOut[ii];
	}

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 2 + ii); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 2 + ii, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;

}

short roya_6412_usg_prof_get_blk_seg(unsigned int uiBlk, unsigned int uiSeg, USG_UNIT_SEG *stpUSG_Seg)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		USG_UNIT_SEG stUsgUnitSegOut;
		char cByteIn[sizeof(USG_UNIT_SEG)];
	}uConvertUsgToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_GET_SEGMENT;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = (char) uiSeg;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 2); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 2, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(USG_UNIT_SEG) + 6))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(USG_UNIT_SEG); ii++)
		{
			uConvertUsgToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 2 + ii];
		}
		stpUSG_Seg->iUSG_Amplitude   = uConvertUsgToByte.stUsgUnitSegOut.iUSG_Amplitude;
		stpUSG_Seg->iUSG_Duration_ms = uConvertUsgToByte.stUsgUnitSegOut.iUSG_Duration_ms;
		stpUSG_Seg->iUSG_RampTime    = uConvertUsgToByte.stUsgUnitSegOut.iUSG_RampTime;
		stpUSG_Seg->iUSG_TriggerPattern = uConvertUsgToByte.stUsgUnitSegOut.iUSG_TriggerPattern;
		stpUSG_Seg->uiCmdType        = uConvertUsgToByte.stUsgUnitSegOut.uiCmdType;

	}

	return sRet;

}

short roya_6412_usg_profblk_set_trigger_address(unsigned int uiBlk, int iAddr)
{
	short sRet = MTN_API_OK_ZERO;
	union 
	{
		unsigned int iAddr;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	uConvertIntToByte.iAddr = iAddr;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_SET_TRIGGER_ADDRESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) + 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[1];

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+3); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+3, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_usg_profblk_get_trigger_address(unsigned int uiBlk, int *iTrigAddress)
{
	short sRet = MTN_API_OK_ZERO;
	union 
	{
		int iAddr;
		char cByteIn[sizeof(int)];
	}uConvertUsgToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_GET_TRIGGER_ADDRESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(int) + 1 + _RTC_ROYA_OFST_DATA_START))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(int); ii++)
		{
			uConvertUsgToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii];
		}

		(*iTrigAddress) = uConvertUsgToByte.iAddr;
	}

	return sRet;
}

short roya_6412_usg_profblk_set_nextblk_index(unsigned int uiBlk, unsigned int uiNextBlk)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_SET_NEXTBLK_INDEX;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = (char)uiNextBlk;

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+2); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+2, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_usg_profblk_get_nextblk_index(unsigned int uiBlk, unsigned int *uiNextBlk)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_GET_NEXTBLK_INDEX;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + 1))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		*uiNextBlk = (int)strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
	}

	return sRet;
}

short roya_6412_usg_profblk_set_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int uiWaitCnt)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		return MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	union 
	{
		unsigned int uiWaitCnt;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
	uConvertIntToByte.uiWaitCnt = uiWaitCnt;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_SET_MAX_COUNT_WAIT_TRIG_PROT;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) + 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[1];

	//int intSize = strSended.GetLength();
	//int retsenSize = sock.SendTo(Sended, intSize, sock.m_Port, strIP);
	//char * Sended =new char(intSize);
   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+3); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+3, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_usg_profblk_get_max_count_wait_trig_prot(unsigned int uiBlk, unsigned int *uiMaxCountTrigProt)
{
	short sRet = MTN_API_OK_ZERO;
	union 
	{
		unsigned int uiWaitCnt;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_GET_MAX_COUNT_WAIT_TRIG_PROT;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (sizeof(int) + 6))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii < sizeof(int); ii++)
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1 + ii];
		}
		(*uiMaxCountTrigProt) = uConvertIntToByte.uiWaitCnt;
	}

	return sRet;
}

short roya_6412_usg_profblk_set_start_flag_with_blk(unsigned int uiBlk, unsigned int uiStartFlag)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_SET_START_WITH_BLOCK;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char) uiBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = (char) uiStartFlag;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 2); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START + 2, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;

}

short roya_6412_usg_profblk_get_start_with_status(unsigned int *uiBlk, unsigned int *uiSegIdx, 
												  unsigned int *uiStartFlag, int *iErrorFlag)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_GET_START_WITH_STATUS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_LITTLE_DATA_GET_REPLY_MS);  // 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (4 + _RTC_ROYA_OFST_DATA_START))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		*uiBlk       = (int)strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 0];
		*uiSegIdx    = (int)strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
		*uiStartFlag = (int)strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 2];
		*iErrorFlag  = (int)strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 3];
	}

	return sRet;
}

short roya_6412_usg_profblk_warm_reset()
{
	short sRet = MTN_API_OK_ZERO;
 
#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_USG_PROF;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_USG_BLK_WARM_RESET;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
	roya_clear_flag_hold_commu_eth(); // 20091210
	return sRet;

}



short roya_6412_bsd_get_data(unsigned int uiCurrBlkIdx, unsigned int *uiSampleCnt)
{
	short sRet = MTN_API_OK_ZERO;
	unsigned short usActualDataLen, usFlagGetDataMultiRound = 0; 
	unsigned int uiSampleCntBSD, uiByteCnt;


#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else		BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_GET_DATA;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 1;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiCurrBlkIdx;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+1); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+1, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY);  // Add 10ms delay in 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + 1))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		union
		{
			ADC_TYPE uiDataBSD;  // 20100119 unsigned int uiDataBSD;
			char cByte[sizeof(ADC_TYPE)];  // 20100119
		}uConvertByteToInt;
		union
		{	unsigned short usDataBSD;
			char cByte[sizeof(Uint16)];
		}uConvertShort2Byte;

		uConvertShort2Byte.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		uConvertShort2Byte.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
		uiSampleCntBSD = uConvertShort2Byte.usDataBSD; // strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		*uiSampleCnt = uiSampleCntBSD;

static int nAdcSize = 2; // 20100403
		if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
		{
			nAdcSize = 4;
		}
		else
		{
			nAdcSize = 2;
		}

		// 20100119
		if(uiSampleCntBSD * nAdcSize + _RTC_ROYA_OFST_DATA_START + 2 >= _RTC_ETH_MAX_BUFF_LEN_BYTE)
		{
			usFlagGetDataMultiRound = 1;
			usActualDataLen = (unsigned char)strReceiveBuffer[_RTC_ROYA_OFST_CMD_LEN] - (unsigned char)_RTC_ROYA_OFST_DATA_START - 2;
			usActualDataLen = usActualDataLen / sizeof(ADC_TYPE); // 20100119
		}
		else
		{
			usFlagGetDataMultiRound = 0;
			usActualDataLen = (unsigned short)uiSampleCntBSD;
		}

		uiByteCnt = _RTC_ROYA_OFST_DATA_START + 2;

		for(int ii = 0; ii < usActualDataLen; ii++)
		{
			uConvertByteToInt.cByte[0] = strReceiveBuffer[uiByteCnt];
			uiByteCnt ++;
			uConvertByteToInt.cByte[1] = strReceiveBuffer[uiByteCnt];
			uiByteCnt ++;
			if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
			{
					uConvertByteToInt.cByte[2] = strReceiveBuffer[uiByteCnt];
					uiByteCnt ++;
					uConvertByteToInt.cByte[3] = strReceiveBuffer[uiByteCnt];
					uiByteCnt ++;
			}
			uiDataAdcBSD[uiCurrBlkIdx][ii] = uConvertByteToInt.uiDataBSD;
		}
	}

	// Get remaining data iteratively
	if(usFlagGetDataMultiRound == 1)
	{
		unsigned short usRemainingLen, usStartCntLen, usLoadLen;
		unsigned int uiReadCnt;
		usStartCntLen = usActualDataLen;
		usRemainingLen = uiSampleCntBSD - usActualDataLen;
		while(usRemainingLen > 0)
		{			
			if(usRemainingLen > usActualDataLen)
			{
				usLoadLen = usActualDataLen;
			}
			else
			{
				usLoadLen = usRemainingLen;
			}
			roya_6412_bsd_get_data_from_len(uiCurrBlkIdx, usStartCntLen, usLoadLen, &uiReadCnt);
			usRemainingLen = usRemainingLen - usLoadLen;
			usStartCntLen = usStartCntLen + usLoadLen;
		}
	}


	return sRet;
}

// get data from specific start-cnt for some length
short roya_6412_bsd_get_data_from_len(unsigned int uiCurrBlkIdx, unsigned short usFromCnt, unsigned short usLenData, unsigned int *uiSampleCnt)
{
	short sRet = MTN_API_OK_ZERO;
	unsigned short usActualDataLen; 
	union
	{	unsigned short usDataBSD;
		char cByte[sizeof(Uint16)];
	}uConvertShort2Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_GET_DATA_FROM_LEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 5;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiCurrBlkIdx;

	uConvertShort2Byte.usDataBSD = usFromCnt;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = (char)uConvertShort2Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 2] = (char)uConvertShort2Byte.cByte[1];

	uConvertShort2Byte.usDataBSD = usLenData;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 3] = (char)uConvertShort2Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 4] = (char)uConvertShort2Byte.cByte[1];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+5); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + 1))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		unsigned int uiSampleCntBSD, uiByteCnt;
		union
		{
			ADC_TYPE uiDataBSD;  // 20100119  unsigned int uiDataBSD;
			char cByte[sizeof(ADC_TYPE)]; // 20100119
		}uConvertByteToInt;

		uConvertShort2Byte.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		uConvertShort2Byte.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
		uiSampleCntBSD = uConvertShort2Byte.usDataBSD; // strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		*uiSampleCnt = uiSampleCntBSD;
		// 20100119
static int nAdcSize = 2; // 20100403
		if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
		{
			nAdcSize = 4;
		}
		else
		{
			nAdcSize = 2;
		}

		if(uiSampleCntBSD * nAdcSize + _RTC_ROYA_OFST_DATA_START + 2 >= _RTC_ETH_MAX_BUFF_LEN_BYTE)
		{
			sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
		}
		else
		{
			usActualDataLen = (unsigned short)uiSampleCntBSD;
			uiByteCnt = _RTC_ROYA_OFST_DATA_START + 2;

			for(int ii = 0; ii < usActualDataLen; ii++)
			{
				uConvertByteToInt.cByte[0] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
				uConvertByteToInt.cByte[1] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
	if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
	{
			
				uConvertByteToInt.cByte[2] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
				uConvertByteToInt.cByte[3] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
	}
				uiDataAdcBSD[uiCurrBlkIdx][usFromCnt + ii] = uConvertByteToInt.uiDataBSD;
			}
		}

	}

	return sRet;
}

//static BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER stBSD_InputFirmwareAnalyzeStickness;
//static BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT stBSD_OutputFirmwareAnalyzeStickness;

short roya_6412_bsd_set_firmware_analyze_parameter(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER *stpInput)
{
union
{
BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER stInputAnalyzeBSD;
char cByte[sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER)];
}uConvertByteInputAnalyzeBSD;

	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc = stpInput->us1stBondNumSamplesConsecGreaterDieThAdc;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc = stpInput->us1stBondNumSamplesConsecLowerLeadThAdc;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondSkipSamples = stpInput->us1stBondSkipSamples;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher = stpInput->us2ndBondNumSampleConsecHigher;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc = stpInput->us2ndBondNumSamplesConsecLowerLeadThAdc;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondSkipSamples = stpInput->us2ndBondSkipSamples;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD1stBondStickDieThAdc = stpInput->usBSD1stBondStickDieThAdc;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD2ndBondNonStickThAdc = stpInput->usBSD2ndBondNonStickThAdc;
	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD2ndBondStickLeadThAdc = stpInput->usBSD2ndBondStickLeadThAdc;

	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_SET_PARA_FIRMWARE_ANA_STICKNESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	for(int ii = 0; ii<sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER); ii++)
	{
		strSendBuffer[_RTC_ROYA_OFST_DATA_START + ii] = uConvertByteInputAnalyzeBSD.cByte[ii];
	}

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER)); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

short roya_6412_bsd_get_firmware_analyze_parameter(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER *stpInput)
{

	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_GET_PARA_FIRMWARE_ANA_STICKNESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER)))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
	union
	{
	BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER stInputAnalyzeBSD;
	char cByte[sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER)];
	}uConvertByteInputAnalyzeBSD;


		for(int ii = 0; ii<sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_PARAMETER); ii++)
		{
			uConvertByteInputAnalyzeBSD.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}

//		stpInput->ucParaBlk1stBSD							= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.ucParaBlk1stBSD;
//		stpInput->ucParaBlk2ndBSD							= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.ucParaBlk2ndBSD;
		stpInput->us1stBondNumSamplesConsecGreaterDieThAdc	= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondNumSamplesConsecGreaterDieThAdc;
		stpInput->us1stBondNumSamplesConsecLowerLeadThAdc	= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondNumSamplesConsecLowerLeadThAdc;
		stpInput->us1stBondSkipSamples						= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us1stBondSkipSamples;
		stpInput->us2ndBondNumSampleConsecHigher			= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondNumSampleConsecHigher;
		stpInput->us2ndBondNumSamplesConsecLowerLeadThAdc	= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondNumSamplesConsecLowerLeadThAdc;
		stpInput->us2ndBondSkipSamples					= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.us2ndBondSkipSamples;
		stpInput->usBSD1stBondStickDieThAdc			= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD1stBondStickDieThAdc;
		stpInput->usBSD2ndBondNonStickThAdc			= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD2ndBondNonStickThAdc;
		stpInput->usBSD2ndBondStickLeadThAdc			= uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD.usBSD2ndBondStickLeadThAdc;
	}
	return sRet;
}

short roya_6412_bsd_get_firmware_analyze_output(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutput)
{
union
{
BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT stOutputAnalyzeBSD;
char cByte[sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT)];
}uConvertByteOutputAnalyzeBSD;

	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif
	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_FIRMWARE_ANA_STICKNESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT)))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii<sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT); ii++)
		{
			uConvertByteOutputAnalyzeBSD.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		//stBSD_OutputFirmwareAnalyzeStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD;

		stpOutput->us1stBondStickStatus = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us1stBondStickStatus;
		stpOutput->us2ndBondOpenSticknessFirstRiseCount = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondOpenSticknessFirstRiseCount;
		stpOutput->us2ndBondStartSampleStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondStartSampleStickness;
		stpOutput->us2ndBondStickStatus = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondStickStatus;

	}
	return sRet;
}

short roya_6412_bsd_firmware_analyze_stickness(unsigned char ucBSD1stBlk, unsigned char ucBSD2ndBlk, BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT *stpOutputAnalyzeBSD)
{

union
{
BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT stOutputAnalyzeBSD;
char cByte[sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT)];
}uConvertByteOutputAnalyzeBSD;

//	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD = stBSD_InputFirmwareAnalyzeStickness;

	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif
	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BSD;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_ROYA_BSD_FIRMWARE_ANA_STICKNESS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START ] = ucBSD1stBlk;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = ucBSD2ndBlk;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START + 2); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	Sleep(5);
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT)) )
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii<sizeof(BSD_FIRMWARE_ANALYZE_STICKNESS_OUTPUT); ii++)
		{
			uConvertByteOutputAnalyzeBSD.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		//stBSD_OutputFirmwareAnalyzeStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD;

		stpOutputAnalyzeBSD->us1stBondStickStatus = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us1stBondStickStatus;
		stpOutputAnalyzeBSD->us2ndBondOpenSticknessFirstRiseCount = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondOpenSticknessFirstRiseCount;
		stpOutputAnalyzeBSD->us2ndBondStartSampleStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondStartSampleStickness;
		stpOutputAnalyzeBSD->us2ndBondStickStatus = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD.us2ndBondStickStatus;

	}
	return sRet;

}

/////////////////////////////////////////////////////////////////////
/// Signal Generator Related
/////////////////////////////////////////////////////////////////////
//// _RTC_SIGGEN_SET_NEW_CYCLE
short roya_6412_siggen_set_new_cycle(unsigned int uiCycle)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		unsigned int uiCycle;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
	uConvertIntToByte.uiCycle = uiCycle;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_SET_NEW_CYCLE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int);
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int)); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

//// _RTC_SIGGEN_GET_CURR_CYCLE
short roya_6412_siggen_get_curr_cycle(unsigned int *uiCurrCycle)
{
	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_GET_CURR_CYCLE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+3, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));

	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	union 
	{
		unsigned int uiCycle;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

	if(nLength >= sizeof(int) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(int); ii++)
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}

		*uiCurrCycle = uConvertIntToByte.uiCycle;
	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}

	return sRet;
}

//// _RTC_SIGGEN_SET_NEW_AMPLITUDE
short roya_6412_siggen_set_new_amplitude(unsigned int uiAmp)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		unsigned int uiAmp;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
	uConvertIntToByte.uiAmp = uiAmp;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_SET_NEW_AMPLITUDE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int)); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

//// _RTC_SIGGEN_GET_CURR_AMPLITUDE
short roya_6412_siggen_get_curr_amplitude(unsigned int *uiCurrAmp)
{
	short sRet = MTN_API_OK_ZERO;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_GET_CURR_AMPLITUDE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+3, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));

	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	union 
	{
		unsigned int uiCurrAmp;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

	if(nLength >= sizeof(int) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(int); ii++)
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}

		*uiCurrAmp = uConvertIntToByte.uiCurrAmp;
	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	return sRet;
}

//// _RTC_SIGGEN_SET_START_FLAG_CALC_SINE
short roya_6412_siggen_set_start_flag_calc_sine(unsigned int uiFlag)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		unsigned int uiFlag;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;
	uConvertIntToByte.uiFlag = uiFlag;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_SET_START_FLAG_CALC_SINE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int)); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210
	return sRet;
}

//// _RTC_SIGGEN_GET_CALC_SINE_FLAG
short roya_6412_siggen_get_calc_sine_flag(unsigned int *uiFlag)
{
	short sRet = MTN_API_OK_ZERO;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_GET_CALC_SINE_FLAG;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

   //发送到DSP
	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+3, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));

	memset(strReceiveBuffer, 0, sizeof(strReceiveBuffer));
	//从DSP接收刚才发送的内容
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	union 
	{
		unsigned int uiFlag;
		char cByteIn[sizeof(int)];
	}uConvertIntToByte;

	if(nLength >= sizeof(int) + _RTC_ROYA_OFST_DATA_START)
	{
		for(int ii=0; ii<sizeof(int); ii++)
		{
			uConvertIntToByte.cByteIn[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}

		*uiFlag = uConvertIntToByte.uiFlag;
	}
	else
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	return sRet;
}

//#define _RTC_SIGGEN_SET_NEW_FRAC_CYCLE	 	 0x6
short roya_6412_siggen_set_new_frac_cycle(unsigned int uiCycle, unsigned int uiFracCycle)
{
	short sRet = MTN_API_OK_ZERO;

	union 
	{
		unsigned int uiCycle[2];
		char cByteIn[sizeof(int) * 2];
	}uConvertIntToByte;
	uConvertIntToByte.uiCycle[0] = uiCycle;
	uConvertIntToByte.uiCycle[1] = uiFracCycle;

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_SET_NEW_FRAC_CYCLE;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) * 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = uConvertIntToByte.cByteIn[4];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+5] = uConvertIntToByte.cByteIn[5];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+6] = uConvertIntToByte.cByteIn[6];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+7] = uConvertIntToByte.cByteIn[7];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int) * 2); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

//#define _RTC_SIGGEN_GET_CURR_SINE_OUT_CFG	 0x7
short roya_6412_siggen_get_curr_sine_gen_cfg(SIG_GEN_SINE_OUT_CFG *stpSigGenSineOutCfg)
{

union
{
SIG_GEN_SINE_OUT_CFG stSigGenSineOutCfg;
char cByte[sizeof(SIG_GEN_SINE_OUT_CFG)];
}uConvertByteSigGenSineOutCfg;

//	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD = stBSD_InputFirmwareAnalyzeStickness;

	short sRet = MTN_API_OK_ZERO;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif
	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_SIGNAL_GEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_SIGGEN_GET_CURR_SINE_OUT_CFG;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BSD_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	Sleep(5);
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + sizeof(SIG_GEN_SINE_OUT_CFG)) )
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii<sizeof(SIG_GEN_SINE_OUT_CFG); ii++)
		{
			uConvertByteSigGenSineOutCfg.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		//stBSD_OutputFirmwareAnalyzeStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD;

		stpSigGenSineOutCfg->dFreqISR_Hz = uConvertByteSigGenSineOutCfg.stSigGenSineOutCfg.dFreqISR_Hz;
		stpSigGenSineOutCfg->nCycleISR = uConvertByteSigGenSineOutCfg.stSigGenSineOutCfg.nCycleISR;
		stpSigGenSineOutCfg->nRefSignalCycle = uConvertByteSigGenSineOutCfg.stSigGenSineOutCfg.nRefSignalCycle;

	}
	return sRet;

}
/////////////////////////////////////       BQM related
//#define _RTC_BQM_SET_PRE_COUNTER_TH            0x0
short roya_6412_bqm_set_pre_counter_th(int *iPreCounterTH)
{

	short sRet = MTN_API_OK_ZERO;

	union 
	{
		int iPreCounterTH[2];
		char cByteIn[sizeof(int) * 2];
	}uConvertIntToByte;
	uConvertIntToByte.iPreCounterTH[0] = iPreCounterTH[0];
	uConvertIntToByte.iPreCounterTH[1] = iPreCounterTH[1];

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BQM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_BQM_SET_PRE_COUNTER_TH;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) * 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = uConvertIntToByte.cByteIn[4];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+5] = uConvertIntToByte.cByteIn[5];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+6] = uConvertIntToByte.cByteIn[6];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+7] = uConvertIntToByte.cByteIn[7];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int) * 2); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

//#define _RTC_BQM_SET_MAX_COUNTER_ADC           0x1
short roya_6412_bqm_set_max_counter_adc(int *iMaxCounter)
{

	short sRet = MTN_API_OK_ZERO;

	union 
	{
		int iTemp[2];
		char cByteIn[sizeof(int) * 2];
	}uConvertIntToByte;
	uConvertIntToByte.iTemp[0] = iMaxCounter[0];
	uConvertIntToByte.iTemp[1] = iMaxCounter[1];

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BQM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_BQM_SET_MAX_COUNTER_ADC;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = sizeof(int) * 2;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = uConvertIntToByte.cByteIn[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+1] = uConvertIntToByte.cByteIn[1];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+2] = uConvertIntToByte.cByteIn[2];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+3] = uConvertIntToByte.cByteIn[3];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+4] = uConvertIntToByte.cByteIn[4];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+5] = uConvertIntToByte.cByteIn[5];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+6] = uConvertIntToByte.cByteIn[6];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START+7] = uConvertIntToByte.cByteIn[7];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+ sizeof(int) * 2); //发送到DSP
	roya_clear_flag_hold_commu_eth(); // 20091210

	return sRet;
}

#include "BSD_Def.h"
//#define _RTC_BQM_GET_STATUS                    0x2
short roya_6412_bqm_get_curr_status(BQM_STATUS *stpBQM_Status)
{

union
{
BQM_STATUS stBQM_Status;
char cByte[sizeof(BQM_STATUS)];
}uConvertByteStatusBQM;

//	uConvertByteInputAnalyzeBSD.stInputAnalyzeBSD = stBSD_InputFirmwareAnalyzeStickness;

	short sRet = MTN_API_OK_ZERO;

	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BQM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_BQM_GET_STATUS;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 0;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));

	Sleep(5);
	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + sizeof(BQM_STATUS)) )
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		for(int ii = 0; ii<sizeof(BQM_STATUS); ii++)
		{
			uConvertByteStatusBQM.cByte[ii] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + ii];
		}
		//stBSD_OutputFirmwareAnalyzeStickness = uConvertByteOutputAnalyzeBSD.stOutputAnalyzeBSD;

		stpBQM_Status->cIdxCurrBlkBQM = uConvertByteStatusBQM.stBQM_Status.cIdxCurrBlkBQM;
		stpBQM_Status->iCounterBQM = uConvertByteStatusBQM.stBQM_Status.iCounterBQM;
		stpBQM_Status->iFlagStartBQM = uConvertByteStatusBQM.stBQM_Status.iFlagStartBQM;
		stpBQM_Status->iMaxCountBQM_Adc[0] = uConvertByteStatusBQM.stBQM_Status.iMaxCountBQM_Adc[0];
		stpBQM_Status->iMaxCountBQM_Adc[1] = uConvertByteStatusBQM.stBQM_Status.iMaxCountBQM_Adc[1];
		stpBQM_Status->iPreCountBQM_TH[0] = uConvertByteStatusBQM.stBQM_Status.iPreCountBQM_TH[0];
		stpBQM_Status->iPreCountBQM_TH[1] = uConvertByteStatusBQM.stBQM_Status.iPreCountBQM_TH[1];

	}
	return sRet;

}

unsigned int uiDataAdd_BH_C[_BQM_MAX_BLK_][_BQM_MAX_DATA_LEN_];

//#define _RTC_BQM_GET_BUFFER_BH_C_BLK_FROM_LEN   0x3
short roya_6412_bqm_get_adc_bh_c_from_len(unsigned int uiCurrBlkIdx, unsigned short usFromCnt, unsigned short usLenData, unsigned int *uiSampleCnt)
{
	short sRet = MTN_API_OK_ZERO;
	unsigned short usActualDataLen; 
	union
	{	unsigned short usDataBQM;
		char cByte[sizeof(Uint16)];
	}uConvertShort2Byte;

#ifndef __USE_DISCRETE_CODE__
	sRet = roya_6412_check_eth_socket();
	if(sRet != MTN_API_OK_ZERO)
	{
		return sRet;
	}
#else	
	BYTE TTL = (BYTE) 8;
	//单播
	int ret = setsockopt(stRoyaEth.stSocket, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(BYTE));
	if (ret!=0)
	{
		sRet = MTN_ROYA_COMM_PRE_CHECK_ERROR;
	}
#endif

	strSendBuffer[_RTC_ROYA_OFST_CMD_TYPE] = _RTC_ROYA_CMD_TYPE_BQM;
	strSendBuffer[_RTC_ROYA_OFST_CMD_CODE] = _RTC_BQM_GET_BUFFER_BH_C_BLK_FROM_LEN;
	strSendBuffer[_RTC_ROYA_OFST_CMD_LEN] = 5;
	strSendBuffer[_RTC_ROYA_OFST_RESV_1] = (char)0xAB;
	strSendBuffer[_RTC_ROYA_OFST_RESV_2] = (char)0xAB;

	strSendBuffer[_RTC_ROYA_OFST_DATA_START] = (char)uiCurrBlkIdx;

	uConvertShort2Byte.usDataBQM = usFromCnt;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 1] = (char)uConvertShort2Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 2] = (char)uConvertShort2Byte.cByte[1];

	uConvertShort2Byte.usDataBQM = usLenData;
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 3] = (char)uConvertShort2Byte.cByte[0];
	strSendBuffer[_RTC_ROYA_OFST_DATA_START + 4] = (char)uConvertShort2Byte.cByte[1];

	int retsenSize = roya_6412_send_to(_RTC_ROYA_OFST_DATA_START+5); // sendto(stRoyaEth.stSocket,strSendBuffer, _RTC_ROYA_OFST_DATA_START+5, 0 , (SOCKADDR *) &(stRoyaEth.Remote),sizeof(stRoyaEth.Remote));
//	Sleep(BQM_COMMU_DELAY_GET_REPLY); // Add 10ms delay in 20091210

	int nLength = RecvfromNetwork(stRoyaEth.stSocket,stRoyaEth.Remote, strReceiveBuffer);
	roya_clear_flag_hold_commu_eth(); // 20091210

	if(nLength < (_RTC_ROYA_OFST_DATA_START + 1))
	{
		sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
	}
	else
	{
		unsigned int uiSampleCntBQM, uiByteCnt;
		union
		{
			ADC_TYPE uiDataBQM;  // 20100119  unsigned int uiDataBQM;
			char cByte[sizeof(ADC_TYPE)]; // 20100119
		}uConvertByteToInt;

		uConvertShort2Byte.cByte[0] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		uConvertShort2Byte.cByte[1] = strReceiveBuffer[_RTC_ROYA_OFST_DATA_START + 1];
		uiSampleCntBQM = uConvertShort2Byte.usDataBQM; // strReceiveBuffer[_RTC_ROYA_OFST_DATA_START];
		*uiSampleCnt = uiSampleCntBQM;
		// 20100119
static int nAdcSize = 2; // 20100403
		if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
		{
			nAdcSize = 4;
		}
		else
		{
			nAdcSize = 2;
		}

		if(uiSampleCntBQM * nAdcSize + _RTC_ROYA_OFST_DATA_START + 2 >= _RTC_ETH_MAX_BUFF_LEN_BYTE)
		{
			sRet = MTN_ROYA_COMM_REPLY_LEN_ERROR;
		}
		else
		{
			usActualDataLen = (unsigned short)uiSampleCntBQM;
			uiByteCnt = _RTC_ROYA_OFST_DATA_START + 2;

			for(int ii = 0; ii < usActualDataLen; ii++)
			{
				uConvertByteToInt.cByte[0] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
				uConvertByteToInt.cByte[1] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
	if(iFlagNumByteADC == ROYA_BSD_ADC_4_BYTE)  // 20100403
	{
			
				uConvertByteToInt.cByte[2] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
				uConvertByteToInt.cByte[3] = strReceiveBuffer[uiByteCnt];
				uiByteCnt ++;
	}
				uiDataAdd_BH_C[uiCurrBlkIdx][usFromCnt + ii] = uConvertByteToInt.uiDataBQM;
			}
		}

	}

	return sRet;
}
