// Multithreaded; use /MT

// for compability with MFC
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <conio.h>
#include <windows.h>

#include "CSerialPort2.h"

// History
// 20091110		Z_John	Integrate and test

// for Com Port Debug
char *pstrCommPortErrorMsg[] = {
	"OK", // 0
	"SET_COM_PORT_MASK",	 //			-1
	"SET_COM_PORT_TIME_OUT", //		-2
	"SET_COM_PORT_STATE",    //    -3
	"CREATE_PORT_FILE"		//	-4
};

// Constructor
CSerialPort2::CSerialPort2()
{
    m_RXSize               = 1;
    m_isReady              = false;
    m_hSerial              = INVALID_HANDLE_VALUE;
    m_hThread              = INVALID_HANDLE_VALUE;
    m_isTXing              = false;
    m_isRXing              = false;
    m_Buffer_TX_len        = 0;
    m_Buffer_RX_len        = 0;
    m_isCheckingModemEvent = false;

    m_Handler              = NULL;

    // Initializing events
    {
        int i;

        for(i = 0; i <=  EVENT_CHECKMODEMEVENT; i++)
            m_Events[i] = CreateEvent(NULL, FALSE, FALSE, NULL); // Auto reset

        for(i = EVENT_READ_DONE; i < EVENT_NUM; i++)
            m_Events[i] = CreateEvent(NULL, TRUE,  FALSE, NULL); // Manual reset
    }
}


// Destructor
CSerialPort2::~CSerialPort2()
{
    // Terminate thread
    if(m_hThread != INVALID_HANDLE_VALUE)
    {
        SetEvent(m_Events[EVENT_DISCONNECT]);

        WaitForSingleObject(m_hThread, 2000);
        m_hThread = INVALID_HANDLE_VALUE;
    }

    // Finalizing events
    for(int i = 0; i < EVENT_NUM; i++)
    {
        if(m_Events[i] != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_Events[i]);
            m_Events[i] = INVALID_HANDLE_VALUE;
        }
    }

    // Close port
    if(m_hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
    }
}

void CSerialPort2::GetErrorCodeStr(char *strCode, int nStrLen, int iErrorCode)
{
	sprintf_s(strCode, nStrLen, "%s", pstrCommPortErrorMsg[ - iErrorCode]);
}

// int portno, int rate, BYTE parity, char bytesize, bool usemodemevents
int CSerialPort2::ConnectNormal(COM_PORT_NORMAL_SET *stpNormalSetting)
{

	    // Initialize basic members
    {
        ZeroMemory(&m_ovRead,      sizeof(OVERLAPPED));
        ZeroMemory(&m_ovWrite,     sizeof(OVERLAPPED));
        ZeroMemory(&m_ovWaitEvent, sizeof(OVERLAPPED));

        m_ovRead.hEvent      = m_Events[EVENT_READ_DONE];
        m_ovWrite.hEvent     = m_Events[EVENT_WRITE_DONE];
        m_ovWaitEvent.hEvent = m_Events[EVENT_MODEM_DONE];

        m_isCheckingModemEvent = stpNormalSetting->usemodemevents;
    }

    if(m_hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;
    }

    // Open port & settings
    try
    {
        {
            char port[10];
            sprintf_s(port, 10, "COM%d", stpNormalSetting->portno);

            m_hSerial = CreateFile(port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        }

        if(m_hSerial != INVALID_HANDLE_VALUE)
        {
            if(!SetCommMask(m_hSerial, m_isCheckingModemEvent ? (EV_RING | EV_RLSD) : 0)) throw -1; // ERROR_SET_COM_PORT_MASK, 20091110
            
            // set timeouts
            {
                COMMTIMEOUTS cto = { 0, 0, 0, 0, 0 };

                if(!SetCommTimeouts(m_hSerial, &cto)) throw -2;  // ERROR_SET_COM_PORT_TIME_OUT, 20091110
            }

            // set DCB
            {
                DCB dcb;

                memset(&dcb,0,sizeof(dcb));

                dcb.DCBlength       = sizeof(dcb);                   
                dcb.BaudRate        = stpNormalSetting->rate;
                dcb.Parity          = stpNormalSetting->parity;

                if(stpNormalSetting->parity) dcb.fParity = 1;

				dcb.StopBits        = stpNormalSetting->StopBits; // ONESTOPBIT; // ONE5STOPBITS; 20091110
                dcb.ByteSize        = (BYTE)(stpNormalSetting->bytesize);

                dcb.fInX            = FALSE;
                dcb.fOutX           = FALSE;

                dcb.fErrorChar      = 0;
                dcb.fBinary         = 1;
                dcb.fNull           = 0;
                dcb.fAbortOnError   = 0;
                dcb.XonLim          = 2;
                dcb.XoffLim         = 4;
                dcb.XonChar         = 0x13;
                dcb.XoffChar        = 0x19;
                dcb.EvtChar         = 0;

                /* Hardware handshake
                {
                    dcb.fOutxDsrFlow    = TRUE;
                    dcb.fOutxCtsFlow    = TRUE;
                    dcb.fRtsControl     = RTS_CONTROL_HANDSHAKE;
                    dcb.fDtrControl     = DTR_CONTROL_HANDSHAKE;
                }
                */
                {
                    dcb.fOutxDsrFlow    = FALSE;
                    dcb.fOutxCtsFlow    = FALSE;

					dcb.fRtsControl     = stpNormalSetting->fRtsControl; // RTS_CONTROL_DISABLE; // RTS_CONTROL_ENABLE;
					dcb.fDtrControl     = stpNormalSetting->fDtrControl; // DTR_CONTROL_DISABLE; // DTR_CONTROL_ENABLE;
                }

                if(!SetCommState(m_hSerial, &dcb)) throw -3;   // ERROR_SET_COM_PORT_STATE, 20091110
            }
        }
        else throw -4; // ERROR_CREATE_PORT_FILE
    }
    catch(int errorcode)
    {
        CloseHandle(m_hSerial);
        m_hSerial = INVALID_HANDLE_VALUE;

        return errorcode;
    }

    // if, CONNECTION OK, start thread, using one thread, checked on 20110630
    {
        unsigned int thread_id;
        m_hThread = (HANDLE)_beginthreadex(NULL, 0, CSerialPort2::ThreadLoop, this, 0, &thread_id);
    }
    
    return 0;

}
// Connect
int CSerialPort2::Connect(int portno, int rate, BYTE parity, char bytesize, bool usemodemevents)
{
static	COM_PORT_NORMAL_SET stComPortNormalSet;

	stComPortNormalSet.portno = portno;
	stComPortNormalSet.rate = rate;
	stComPortNormalSet.parity = parity;
	stComPortNormalSet.bytesize = bytesize;
	stComPortNormalSet.usemodemevents = usemodemevents;
	stComPortNormalSet.fDtrControl = DTR_CONTROL_DISABLE;
	stComPortNormalSet.fRtsControl = RTS_CONTROL_DISABLE;
	stComPortNormalSet.StopBits = ONESTOPBIT;

	return ConnectNormal(& stComPortNormalSet);
}


void CSerialPort2::Disconnect(void)
{
    m_isReady = false;

    if(m_hThread != INVALID_HANDLE_VALUE)
    {
        SetEvent(m_Events[EVENT_DISCONNECT]);

        WaitForSingleObject(m_hThread, 2000);
        m_hThread = INVALID_HANDLE_VALUE;
    }
}


// read buffer (for event handler)
void CSerialPort2::GetBuffer(BYTE **buf, size_t *size)
{
    *buf  = m_Buffer_RX;
    *size = m_Buffer_RX_len;
}


// mark flag as readed (for event handler)
void CSerialPort2::SetBufferReaded(void)
{
    SetEvent(m_Events[EVENT_BUFFER_READED]);
}


// bytes once reading
void CSerialPort2::SetRXSize(size_t size)
{
    m_RXSize = size > SERIAL_MAX_RX ? SERIAL_MAX_RX : size;
}


// Send bytes to port
void CSerialPort2::Send(BYTE *buffer, size_t size)
{
    if(!m_isTXing && (size < SERIAL_MAX_TX))
    {
        m_isTXing = true;

        memcpy(m_Buffer_TX, buffer, size);
        m_Buffer_TX_len = (int)size;

        SetEvent(m_Events[EVENT_SEND]);
    }
}


// Main Thread Loop
unsigned __stdcall CSerialPort2::ThreadLoop(void *arg1)
{
    CSerialPort2 *arg = (CSerialPort2 *)arg1;

    arg->m_isReady            = true;
    arg->m_isTXing            = false;
    arg->m_isRXing            = false;
    arg->m_isWaitCommEventing = false;

    // send CONNECTED signal to handler
    if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_CONNECTED, arg->m_UserArg);

    GetLastError(); // clear pending error if any
    SetEvent(arg->m_Events[EVENT_BUFFER_READED]);
    if(arg->m_isCheckingModemEvent) SetEvent(arg->m_Events[EVENT_CHECKMODEMEVENT]);

    DWORD ret;

    while(1)
    {
        ret = WaitForMultipleObjects(EVENT_NUM, arg->m_Events, FALSE, INFINITE);

        if((ret < WAIT_OBJECT_0) || (ret > WAIT_OBJECT_0 + EVENT_NUM - 1))
            goto threadloop_exit;

        switch(ret - WAIT_OBJECT_0)
        {
            case EVENT_DISCONNECT: // DISCONNECT requested
                                   goto threadloop_exit;

            case EVENT_BUFFER_READED: // Marked as read buffer is done; start new reading
                                      if(!(arg->m_isRXing))
                                      {
                                          arg->m_isRXing = true;

                                          if(!ReadFile(arg->m_hSerial, &(arg->m_Buffer_RX), (DWORD)arg->m_RXSize, &ret, &(arg->m_ovRead)))
                                          {
                                              // ReadFile() failed.
                                              if(GetLastError() != ERROR_IO_PENDING) goto threadloop_exit;
                                          }
                                      }
                                      break;

            case EVENT_READ_DONE: // ReadFile() notified reading operation is done; process result
                              if(GetOverlappedResult(arg->m_hSerial, &(arg->m_ovRead), &ret, FALSE))
                              {
                                  ResetEvent(arg->m_Events[EVENT_READ_DONE]);

                                  arg->m_Buffer_RX_len = ret;
                                  arg->m_isRXing       = false;

                                  // send DATA_ARRIVAL signal to handler
                                  if((ret != 0) && (arg->m_Handler != NULL)) arg->m_Handler(arg, SERIAL_DATA_ARRIVAL, arg->m_UserArg);
                              }
                              else
                                  if(GetLastError()!= ERROR_IO_PENDING) goto threadloop_exit;

                              break;

            case EVENT_SEND: // send bytes to port
                             if(!WriteFile(arg->m_hSerial, arg->m_Buffer_TX, arg->m_Buffer_TX_len, &ret, &(arg->m_ovWrite)))
                             {
                                 // WriteFile() failed.
                                 if(GetLastError() != ERROR_IO_PENDING) goto threadloop_exit;
                             }
                             break;

            case EVENT_WRITE_DONE: // WriteFile() notified reading operation is done; process result
                             if(GetOverlappedResult(arg->m_hSerial, &(arg->m_ovWrite), &ret, FALSE))
                             {
                                 ResetEvent(arg->m_Events[EVENT_WRITE_DONE]);
                                 arg->m_isTXing = false;

                                 // send DATA_SENT signal to handler
                                 if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_DATA_SENT, arg->m_UserArg);
                             }
                             else
                                 if(GetLastError() != ERROR_IO_PENDING) goto threadloop_exit;

                             break;

            case EVENT_CHECKMODEMEVENT: // modem event
                                        if(!(arg->m_isWaitCommEventing) && arg->m_isCheckingModemEvent)
                                        // if no wait is in progress I start a new one
                                        {            
                                            arg->m_isWaitCommEventing = true;

                                            // reading one byte only to have immediate answer on each byte
                                            if(!WaitCommEvent(arg->m_hSerial, &(arg->m_dwCommEvent), &(arg->m_ovWaitEvent)))
                                            {
                                                if(GetLastError() != ERROR_IO_PENDING) goto threadloop_exit;
                                            }
                                        }
                                        break;

            case EVENT_MODEM_DONE: // read WaitCommEvent() result
                                   if(GetOverlappedResult(arg->m_hSerial, &(arg->m_ovWaitEvent), &ret, FALSE))
                                   {
                                       ResetEvent(arg->m_Events[EVENT_MODEM_DONE]);
                                       arg->m_isWaitCommEventing = false;

                                       // process modem events
                                       {
                                           unsigned long ModemStat;
                                           GetCommModemStatus(arg->m_hSerial, &ModemStat);

                                           if((arg->m_dwCommEvent & EV_RING)!=0)
                                           {
                                               if((ModemStat &  MS_RING_ON)!= 0)
                                               {
                                                   if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_RING, arg->m_UserArg);
                                               }
                                           }

                                           if((arg->m_dwCommEvent & EV_RLSD)!=0)
                                           {
                                               if((ModemStat &  MS_RLSD_ON)!= 0)
                                               {
                                                   if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_CD_ON, arg->m_UserArg);
                                               }
                                               else
                                               {
                                                   if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_CD_OFF, arg->m_UserArg);
                                               }
                                           }
                                       }
                    
                                       // automatically starting a new check
                                       SetEvent(arg->m_Events[EVENT_CHECKMODEMEVENT]);
                                   }
                                   else
                                       if(GetLastError() != ERROR_IO_PENDING) goto threadloop_exit;
                                   
                                   break;
        }
    }


threadloop_exit:
    
    // Finalize
    arg->m_isReady = false;

    if(arg->m_hSerial != INVALID_HANDLE_VALUE)
    {
        CloseHandle(arg->m_hSerial);
        arg->m_hSerial = INVALID_HANDLE_VALUE;
    }

    // send DISCONNECTED signal to handler
    if(arg->m_Handler != NULL) arg->m_Handler(arg, SERIAL_DISCONNECTED, arg->m_UserArg);

    return 0;
}


/*
// get number of bytes pending in port
int CSerialPort2::PendingBytes(void)  
{
    struct COMSTAT status;
    DWORD          errorcode;

    if(m_hSerial != INVALID_HANDLE_VALUE)
    {
        ClearCommError(m_hSerial, &errorcode, &status);
        return status.cbInQue;
    }
}
*/
