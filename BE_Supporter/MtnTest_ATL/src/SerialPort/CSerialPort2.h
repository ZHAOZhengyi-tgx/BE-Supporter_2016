//
// Serial port communication class library
//
//
// Usage:
//
//     CSerialPort2 *port = new CSerialPort2();
//     port->SetHandler(Serial_Handler, this);
//     m_SerialPort2->Connect(1, 9600, 2, 8, true);
//
//     void Serial_Handler(CSerialPort2 *object, serial_event_id_t event, void *user_arg)
//     {
//         switch(event)
//             case SERIAL_DATA_ARRIVAL:
//                 object->GetBuffer();
//                 object->SetBufferReaded();
//     }
//

#ifndef CSERIALPORT2_H
#define CSERIALPORT2_H

#ifdef _MFC_VER
#include "stdafx.h"
#endif

#include <stdio.h>
#include <windows.h>


#define SERIAL_MAX_RX     256       // Input buffer max size
#define SERIAL_MAX_TX     256       // output buffer max size

#define EVENT_NUM             7

#define EVENT_DISCONNECT      0
#define EVENT_SEND            1    // data waiting to be sent
#define EVENT_BUFFER_READED   2    // handler notifies buffer is readed
#define EVENT_CHECKMODEMEVENT 3
#define EVENT_READ_DONE       4
#define EVENT_WRITE_DONE      5
#define EVENT_MODEM_DONE      6

#define COM_PORT_CONNECT_OK				0
#define ERROR_SET_COM_PORT_MASK			-1
#define ERROR_SET_COM_PORT_TIME_OUT		-2
#define ERROR_SET_COM_PORT_STATE        -3
#define ERROR_CREATE_PORT_FILE			-4


typedef enum
{
    SERIAL_CONNECTED,
    SERIAL_DISCONNECTED,
    SERIAL_DATA_SENT,
    SERIAL_DATA_ARRIVAL,
    SERIAL_RING,
    SERIAL_CD_ON,
    SERIAL_CD_OFF
} serial_event_id_t;


class CSerialPort2;
typedef void (*CSerialPort2_Callback_t) (CSerialPort2 *object, serial_event_id_t event, void *user_arg);

typedef struct
{
	int portno;
	int rate;
	BYTE parity;
	char bytesize;
	bool usemodemevents;
	DWORD fRtsControl;
	DWORD fDtrControl;
	BYTE StopBits;
}COM_PORT_NORMAL_SET;

class CSerialPort2
{
    private:
        HANDLE     m_Events[EVENT_NUM];   // events to wait on
        HANDLE     m_hSerial;                     // ...
        HANDLE     m_hThread;                     // ...

        OVERLAPPED m_ovRead;                      // for ReadFile
        OVERLAPPED m_ovWrite;                     // for WriteFile
        OVERLAPPED m_ovWaitEvent;                 // for WaitCommEvent

        BYTE       m_Buffer_RX[SERIAL_MAX_RX];
        int        m_Buffer_RX_len;
        BYTE       m_Buffer_TX[SERIAL_MAX_TX];
        int        m_Buffer_TX_len;

        size_t     m_RXSize;                      // setting; max bytes once read

        bool       m_isReady;

        bool       m_isTXing;
        bool       m_isRXing;
        bool       m_isWaitCommEventing;
        bool       m_isCheckingModemEvent;

        DWORD      m_dwCommEvent;                 // to store the result of the wait

        CSerialPort2_Callback_t m_Handler;
        void                   *m_UserArg;

        static unsigned __stdcall ThreadLoop(void *arg1);      // main thread loop


    public:
        CSerialPort2();
        ~CSerialPort2();

		int ConnectNormal(COM_PORT_NORMAL_SET *stpNormalSetting);

        int  Connect(int portno, int rate, BYTE parity, char bytesize, bool usemodemevents);      // parity: EVENPARITY MARKPARITY NOPARITY ODDPARITY SPACEPARITY

        void Disconnect(void);

        void SetRXSize(size_t size);   // size of bytes once reading, default 1

        void Send(BYTE *buffer, size_t size);

        bool isConnected(void) { return m_isReady; }

        // Set Handler
        void SetHandler(CSerialPort2_Callback_t handler, void *user_arg) { m_Handler = handler; m_UserArg = user_arg; }

        // call from handler
        void GetBuffer(BYTE **buf, size_t *size);
        void SetBufferReaded(void);    // Notify CSerialPort to buffer readed; buffer reset

		// Get error code
		void GetErrorCodeStr(char *strCode, int nStrLen, int iErrorCode);
};

#endif
