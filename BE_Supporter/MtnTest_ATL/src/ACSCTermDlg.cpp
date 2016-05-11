// ACSCTermDlg.cpp : implementation file
//
// 20090908 Replace Sleep to be high_precision_sleep_ms
// 20100723 Debug ACS-Terminal
#include "stdafx.h"
#include "ACSCTerm.h"
#include "ACSCTermDlg.h"
//#include "CommunicationDialog.h"
#include "WinTiming.h"

static int iFlagDebug_SaveView_GroupBuffProg = FALSE;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT TermThreadProc( LPVOID pParam )
{
    CACSCTermDlg* pObject = (CACSCTermDlg *)pParam;
	return pObject->TerminalThread(); 	
}

/////////////////////////////////////////////////////////////////////////////
// CACSCTermDlg dialog

CACSCTermDlg::CACSCTermDlg(COMM_SETTINGS stInCommSet, CWnd* pParent /*=NULL*/)
	: CDialog(CACSCTermDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CACSCTermDlg)
//	m_ConnectionString = _T("Communication: OFF");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	CommSet = stInCommSet;
//	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_fStopTermThread = FALSE;
//	m_fConnect = TRUE;
	m_pWinThread = NULL;
	m_nEditPos = 0;
}

void CACSCTermDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CACSCTermDlg)
	DDX_Control(pDX, IDC_EDIT1, m_editTerminal);
	DDX_Control(pDX, IDC_SENDCOMBO, m_comboSend);
//	DDV_MaxChars(pDX, m_ConnectionString, 256);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CACSCTermDlg, CDialog)
	//{{AFX_MSG_MAP(CACSCTermDlg)
	ON_WM_SYSCOMMAND()
//	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_CONNECTBUTTON, OnConnect)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID0, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId0)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID1, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId1)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID2, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId2)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID3, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId3)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID4, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId4)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID5, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId5)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID6, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId6)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID7, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId7)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID8, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId8)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID9, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId9)
	ON_BN_CLICKED(IDC_CHECK_ACS_PROG_BUFF_ID10, &CACSCTermDlg::OnBnClickedCheckAcsProgBuffId10)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_ACS_BUFFER_PROG_TERM_DLG, &CACSCTermDlg::OnBnClickedButtonSaveAcsBufferProgTermDlg)
	ON_BN_CLICKED(IDC_CHECK_FLAG_VIEW_ACS_BUFFER_PROG_TERM_DLG, &CACSCTermDlg::OnBnClickedCheckFlagViewAcsBufferProgTermDlg)
	ON_BN_CLICKED(IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG, &CACSCTermDlg::OnBnClickedCheckFlagDebugAcsBufferProgTermDlg)
	ON_BN_CLICKED(IDC_BUTTON_ACS_TERMINAL_SAVE_PARAMETER, &CACSCTermDlg::OnBnClickedButtonAcsTerminalSaveParameter)
	ON_BN_CLICKED(IDC_BUTTON_ACS_TERMINAL_SAVE_PROGRAM, &CACSCTermDlg::OnBnClickedButtonAcsTerminalSaveProgram)
	ON_BN_CLICKED(IDC_BUTTON_ACS_TERMINAL_DLG_ENABLE_PS_LOST_CHECK, &CACSCTermDlg::OnBnClickedButtonAcsTerminalDlgEnablePsLostCheck)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACSCTermDlg message handlers

BOOL CACSCTermDlg::OnInitDialog()
{

#ifdef  __USE__
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
#endif  // __USE__

	// UI
	CFont* pFont = new CFont();
	pFont->CreatePointFont(80, "Courier New");
	m_editTerminal.SetFont(pFont);
	// UI for BuffProg
	InitUI_ForBuffProg();

	acsc_OpenHistoryBuffer(CommSet.Handle, 100000);
	RunTermThread();
	// Debug Protect by Pass
	UI_Enable_Group_ACS_BuffProg(iFlagDebug_SaveView_GroupBuffProg);
	((CButton *)GetDlgItem(IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG))->SetCheck(iFlagDebug_SaveView_GroupBuffProg);
//	GetDlgItem(IDC_SENDCOMBO)->SetFocus();
//	return TRUE;  // return TRUE  unless you set the focus to a control
	return 	CDialog::OnInitDialog();
//	return 	TRUE;
}

void CACSCTermDlg::ShowWindow(int nCmdShow)
{
	InitUI_ForBuffProg();
	acsc_OpenHistoryBuffer(CommSet.Handle, 100000);
	RunTermThread();
	CDialog::ShowWindow(nCmdShow);
}

#include "acs_buff_prog.h"
static int iFlagEnableCheckBuffProgACS[MAX_NUM_ACS_BUFFERS];
static int iFlagViewAfterSaveAcsBuffProg;
static int iTotalNumLinesPerBuffer[MAX_NUM_ACS_BUFFERS];
static int iTotalBytesPerBuffer[MAX_NUM_ACS_BUFFERS];
static int iTransactionReceived;
static CString m_strSentCommand;

char *strProgramStateACS[] =
{
	"C",
	"R",
	"A",
	"D",
	"S"
};

void CACSCTermDlg::InitUI_ForBuffProg()
{
	int ii;

	for(ii=0; ii<MAX_NUM_ACS_BUFFERS; ii++)
	{
		iTotalNumLinesPerBuffer[ii] = 0;
		iTotalBytesPerBuffer[ii] = 0;
		if(acs_buffprog_upload_prog_get_line(ii) == MTN_API_OK_ZERO)
		{
			iTotalNumLinesPerBuffer[ii] = acs_buffprog_get_num_lines_at_buff(ii);
			iTotalBytesPerBuffer[ii] = acs_buffprog_get_num_bytes_at_buff(ii);
		}
	}

CString cstrTemp;
int iProgState;
// Buff0
	acsc_GetProgramState(CommSet.Handle, 0, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[0], iTotalBytesPerBuffer[0], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID0))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID0))->EnableWindow(iTotalBytesPerBuffer[0]);

// Buff1
	acsc_GetProgramState(CommSet.Handle, 1, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[1], iTotalBytesPerBuffer[1], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID1))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID1))->EnableWindow(iTotalBytesPerBuffer[1]);
// Buff2
	acsc_GetProgramState(CommSet.Handle, 2, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[2], iTotalBytesPerBuffer[2], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID2))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID2))->EnableWindow(iTotalBytesPerBuffer[2]);
// Buff3
	acsc_GetProgramState(CommSet.Handle, 3, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[3], iTotalBytesPerBuffer[3], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID3))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID3))->EnableWindow(iTotalBytesPerBuffer[3]);
// Buff4
	acsc_GetProgramState(CommSet.Handle, 4, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[4], iTotalBytesPerBuffer[4], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID4))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID4))->EnableWindow(iTotalBytesPerBuffer[4]);
// Buff5
	acsc_GetProgramState(CommSet.Handle, 5, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[5], iTotalBytesPerBuffer[5], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID5))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID5))->EnableWindow(iTotalBytesPerBuffer[5]);
// Buff6
	acsc_GetProgramState(CommSet.Handle, 6, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[6], iTotalBytesPerBuffer[6], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID6))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID6))->EnableWindow(iTotalBytesPerBuffer[6]);
// Buff7
	acsc_GetProgramState(CommSet.Handle, 7, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[7], iTotalBytesPerBuffer[7], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID7))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID7))->EnableWindow(iTotalBytesPerBuffer[7]);
// Buff8
	acsc_GetProgramState(CommSet.Handle, 8, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[8], iTotalBytesPerBuffer[8], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID8))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID8))->EnableWindow(iTotalBytesPerBuffer[8]);
// Buff9
	acsc_GetProgramState(CommSet.Handle, 9, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[9], iTotalBytesPerBuffer[9], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID9))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID9))->EnableWindow(iTotalBytesPerBuffer[9]);
// Buff10
	acsc_GetProgramState(CommSet.Handle, 10, &iProgState, NULL);
	cstrTemp.Format("L%d,B%d,%s", iTotalNumLinesPerBuffer[10], iTotalBytesPerBuffer[10], strProgramStateACS[iProgState]);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID10))->SetWindowTextA(cstrTemp);
	((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID10))->EnableWindow(iTotalBytesPerBuffer[10]);

}

void CACSCTermDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
#ifdef __USE__

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CACSCTermDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
#endif // __USE__

void CACSCTermDlg::OnSend() 
{
	// TODO: Add your control notification handler code here
	// function sends command to the controller
	char strTemp[128];
	UpdateData(TRUE);
	m_comboSend.GetWindowTextA(strTemp, 128); // m_comboSend.GetWindowTextLengthA()); //  m_comboSend.GetWindowText(m_strSentCommand);
	m_strSentCommand.Format("%s", strTemp);
	int ind = m_comboSend.FindStringExact(-1, m_strSentCommand);
	if (ind != CB_ERR) m_comboSend.DeleteString(ind);
	m_comboSend.InsertString(0, m_strSentCommand);
	// every sent command should be concluded by '\r' character
	m_strSentCommand += '\r';
	// send command to the controller
//	if (!acsc_Send(CommSet.Handle, (char *)LPCTSTR(m_strSentCommand), m_strSentCommand.GetLength(), NULL))
	int iRet = acsc_Transaction(CommSet.Handle, (char *)LPCTSTR(m_strSentCommand), m_strSentCommand.GetLength(),
		m_HistBuf, HISTBUF_SIZE - 1, &iTransactionReceived, NULL);  // strUploadMessage, 256, 
	if(!iRet)
	{
		char str1[300], str2[256];
		int Received;
		// retrieve last error
		acsc_GetErrorString(CommSet.Handle, acsc_GetLastError(), str2, 256, &Received);
		str2[Received] = '\0';
		sprintf_s(str1, "Error: %s", str2);
		MessageBox(str1, "ACSC Terminal", MB_ICONWARNING);
	}
	m_comboSend.SetEditSel(0, -1);
}

void CACSCTermDlg::OnClose() 
{
	// TODO: Add your control notification handler code here
	StopTermThread();
	acsc_CloseHistoryBuffer(CommSet.Handle);
	CDialog::OnCancel();
}

void CACSCTermDlg::OnOK() 
{
	// TODO: Add extra validation here
//	CDialog::OnOK();
}

void CACSCTermDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
}

UINT CACSCTermDlg::TerminalThread()
{
	// every 100 ms communication history is updated
	while (!m_fStopTermThread)
	{
		// get all communication history
		// acsc_GetHistory(CommSet.Handle, m_HistBuf, HISTBUF_SIZE - 1, &o, TRUE)
		//if ()
		//{   // 20110709
			if (iTransactionReceived > 0)
			{
				m_HistBuf[iTransactionReceived] = '\0';
				// insert '\n' after each line
				FormatBuffer();
				// show communication history
				m_editTerminal.SetWindowText(m_EditBuf);  // m_HistBuf
				int lines = m_editTerminal.GetLineCount();
				m_editTerminal.LineScroll(lines); 
				//	Clear iTransactionReceived, 20110709
				iTransactionReceived = 0;
			}
		//}  // 20110709
		Sleep(100);
	}
	return 0;
}

void CACSCTermDlg::RunTermThread()
{
	m_fStopTermThread = FALSE;
	m_pWinThread = AfxBeginThread(TermThreadProc, this);
	m_pWinThread->m_bAutoDelete = FALSE;
}

void CACSCTermDlg::StopTermThread()
{
	if (m_pWinThread)
	{
		m_fStopTermThread = TRUE;
		WaitForSingleObject(m_pWinThread->m_hThread, 1000);
		delete m_pWinThread;
		m_pWinThread = NULL;
	}
}

void CACSCTermDlg::FormatBuffer()
{
	// each line of the controller replies are concluded by '\r'
	// function adds '\n' in the end of the lines after '\r'
	static char tmpbuf[2*HISTBUF_SIZE];

	sprintf_s(tmpbuf, 2 * HISTBUF_SIZE, "%s", m_strSentCommand.GetString());  // 20110709
	
	int hbuf_pos = m_strSentCommand.GetLength();
	int tmpbuf_pos = m_strSentCommand.GetLength();  // 20110709
	while (m_HistBuf[hbuf_pos] != '\0')
	{
		tmpbuf[tmpbuf_pos++] = m_HistBuf[hbuf_pos];
		if (m_HistBuf[hbuf_pos] == '\r')
			tmpbuf[tmpbuf_pos++] = '\n';
		hbuf_pos++;
	}
	tmpbuf[tmpbuf_pos++] = '\0';    // 20100723, only keep the last single '/0'
	if (EDITBUF_SIZE - m_nEditPos < tmpbuf_pos)
	{
		int shortage = tmpbuf_pos - (HISTBUF_SIZE - m_nEditPos);
		m_nEditPos -= shortage;
		memcpy(m_EditBuf, &m_EditBuf[shortage], m_nEditPos);
	}
	memcpy(&m_EditBuf[m_nEditPos], tmpbuf, tmpbuf_pos); // strcpy_s, run-time error, 20100723
	m_nEditPos += (tmpbuf_pos - 1);  // 20100723, only keep the last single '/0'
}

// IDC_CHECK_ACS_PROG_BUFF_ID0
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId0()
{
	iFlagEnableCheckBuffProgACS[0] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID0))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID1
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId1()
{
	iFlagEnableCheckBuffProgACS[1] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID1))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID2
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId2()
{
	iFlagEnableCheckBuffProgACS[2] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID2))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID3
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId3()
{
	iFlagEnableCheckBuffProgACS[3] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID3))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID4
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId4()
{
	iFlagEnableCheckBuffProgACS[4] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID4))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID5
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId5()
{
	iFlagEnableCheckBuffProgACS[5] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID5))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID6
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId6()
{
	iFlagEnableCheckBuffProgACS[6] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID6))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID7
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId7()
{
	iFlagEnableCheckBuffProgACS[7] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID7))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID8
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId8()
{
	iFlagEnableCheckBuffProgACS[8] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID8))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID9
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId9()
{
	iFlagEnableCheckBuffProgACS[9] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID9))->GetCheck();
}
// IDC_CHECK_ACS_PROG_BUFF_ID10
void CACSCTermDlg::OnBnClickedCheckAcsProgBuffId10()
{
	iFlagEnableCheckBuffProgACS[10] = ((CButton*)GetDlgItem(IDC_CHECK_ACS_PROG_BUFF_ID10))->GetCheck();
}
extern char astrBuffProgramACS[MAX_NUM_ACS_BUFFERS][MAX_NUM_BYTES_PER_BUFFER];
extern CString acs_buffprog_get_buffer_string(unsigned int idxBuffer);
#define __FILE_NAME_ACS_BUFF_PROG__		"ACS_BuffProg"
#include "MtnTesterResDef.h"
// IDC_BUTTON_SAVE_ACS_BUFFER_PROG_TERM_DLG
void CACSCTermDlg::OnBnClickedButtonSaveAcsBufferProgTermDlg()
{
struct tm stTime;
struct tm *stpTime = &stTime;
__time64_t stLongTime;
char strFilenameAcsBuffProg[64];
FILE *fptr;

	_time64(&stLongTime);
	_localtime64_s(stpTime, &stLongTime);
	sprintf_s(strFilenameAcsBuffProg, 64, "%s_%d-%d-%d_%d-%d-%d.txt", __FILE_NAME_ACS_BUFF_PROG__, stpTime->tm_year +1900, stpTime->tm_mon +1, 
		stpTime->tm_mday, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
	fopen_s(&fptr, strFilenameAcsBuffProg, "w");
	if(fptr != NULL)
	{
		fprintf(fptr, "#/ Controller version  = 5.20\n");
		fprintf(fptr, "#/ Date[YYYY/MM/DD] = %d/%d/%d, Time= %d:%d:%d\n", stpTime->tm_mday, stpTime->tm_mon +1, stpTime->tm_year +1900, stpTime->tm_hour, stpTime->tm_min, stpTime->tm_sec);
		for(int ii=0; ii< MAX_NUM_ACS_BUFFERS; ii++) // 
		{
			if((iFlagEnableCheckBuffProgACS[ii] == TRUE) && (iTotalBytesPerBuffer[ii] > 0))
			{
				CString cstrTemp;
				fprintf(fptr, "#%d\n", ii);
//				cstrTemp = acs_buffprog_get_buffer_string(ii);
				fprintf(fptr, "%s", astrBuffProgramACS[ii]);  // cstrTemp.GetString()
			}
		}

		fclose(fptr);
	}
	if(iFlagViewAfterSaveAcsBuffProg)
	{
		char strSystemCmdViewBuffProg[512];
		sprintf(strSystemCmdViewBuffProg, "notepad %s", strFilenameAcsBuffProg);
		system(strSystemCmdViewBuffProg);
	}
	sys_set_flag_clean_env_on_exit(TRUE);
}
// IDC_CHECK_FLAG_VIEW_ACS_BUFFER_PROG_TERM_DLG
void CACSCTermDlg::OnBnClickedCheckFlagViewAcsBufferProgTermDlg()
{
	iFlagViewAfterSaveAcsBuffProg = ((CButton*)GetDlgItem(IDC_CHECK_FLAG_VIEW_ACS_BUFFER_PROG_TERM_DLG))->GetCheck();
}
// IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG, IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG
#include "DlgKeyInputPad.h"
void CACSCTermDlg::OnBnClickedCheckFlagDebugAcsBufferProgTermDlg()
{
	int iTemp;
	static int iFlagFailurePass = FALSE;
	iTemp = ((CButton *)GetDlgItem(IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG))->GetCheck();
	if(iTemp == TRUE)
	{
		static CDlgKeyInputPad cDlgInputPad;
		cDlgInputPad.SetFlagShowNumberOnKeyPad(0);
		cDlgInputPad.SetInputNumber(0);
		if(cDlgInputPad.DoModal() == IDOK)
		{
			if(cDlgInputPad.GetReturnNumber()== wb_mtn_tester_get_password_brightlux_6d_zzy())
			{
				iFlagFailurePass = FALSE;
			}
			else
			{
				iFlagFailurePass = TRUE;
			}
		}
		else
		{
			iFlagFailurePass = TRUE;
		}
		if(iFlagFailurePass == TRUE)
		{
			iTemp = FALSE;
			((CButton *)GetDlgItem(IDC_CHECK_FLAG_DEBUG_ACS_BUFFER_PROG_TERM_DLG))->SetCheck(FALSE);
		}
		else
		{
			UI_Enable_Group_ACS_BuffProg(iTemp);
		}

	}
	else
	{
		UI_Enable_Group_ACS_BuffProg(iTemp);
	}
	iFlagDebug_SaveView_GroupBuffProg = iTemp;
	sys_set_flag_debug_mode(iTemp);

}

void CACSCTermDlg::UI_Enable_Group_ACS_BuffProg(BOOL bEnableFlag)
{
	GetDlgItem(IDC_BUTTON_SAVE_ACS_BUFFER_PROG_TERM_DLG)->ShowWindow(bEnableFlag);
	GetDlgItem(IDC_CHECK_FLAG_VIEW_ACS_BUFFER_PROG_TERM_DLG)->ShowWindow(bEnableFlag);
}
// IDC_BUTTON_ACS_TERMINAL_SAVE_PARAMETER
void CACSCTermDlg::OnBnClickedButtonAcsTerminalSaveParameter()
{
	acs_command_save_parameter();
	GetDlgItem(IDC_STATIC_EDIT_CMD_ACS_TERM_DLG)->SetWindowTextA(_T("#SAVEPAR"));
}

#include "MotAlgo_Dll.h"
// IDC_BUTTON_ACS_TERMINAL_SAVE_PROGRAM
void CACSCTermDlg::OnBnClickedButtonAcsTerminalSaveProgram()
{
	acs_clear_buffer_7_from_ram_para_(); // 20120906

	int iMachType = get_sys_machine_type_flag();
	short sRet = acs_update_buffer_7_from_ram_para_(iMachType);

	acs_command_save_program();
	// #SAVEPROG 7
	GetDlgItem(IDC_STATIC_EDIT_CMD_ACS_TERM_DLG)->SetWindowTextA(_T("#SAVEPROG 7"));
	char strSaveBuffer7[16] = "#SAVEPROG 7\r";
	for(int ii=0; ii<3; ii++)
	{
		if(acsc_Send(CommSet.Handle, strSaveBuffer7, strlen(strSaveBuffer7), NULL))
		{
			Sleep(500);
		}
	}
	acs_run_buffer_prog_rms_emerg_stop();
}

// IDC_STATIC_EDIT_CMD_ACS_TERM_DLG
// IDC_BUTTON_ACS_TERMINAL_DLG_ENABLE_PS_LOST_CHECK
void CACSCTermDlg::OnBnClickedButtonAcsTerminalDlgEnablePsLostCheck()
{
	acs_enable_check_in3_flag();
}
