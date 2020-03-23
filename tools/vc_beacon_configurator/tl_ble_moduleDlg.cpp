// tl_ble_moduleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"
#include "tl_ble_moduleDlg.h"
#include "usbprt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_APPENDLOG (WM_APP + 104)

//#define	DONGLE_BLE_MODULE_ID		0x82bd

int	DONGLE_BLE_MODULE_ID =		0xffff;

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg dialog

CTl_ble_moduleDlg::CTl_ble_moduleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTl_ble_moduleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTl_ble_moduleDlg)
	m_conn_interval = 24;
	m_conn_timeout = 1000;
	m_conn_chnmask = _T("ff ff ff ff 1f");
	m_bLogEn = 1;
	m_bHex = 1;
	m_cmd_input = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pScanDlg = NULL;
	m_status = 0;
}

void CTl_ble_moduleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTl_ble_moduleDlg)
	DDX_Control(pDX, IDC_TEXT_BO, m_textBO);
	DDX_Control(pDX, IDC_LOG, m_Log);
	DDX_Text(pDX, IDC_CONN_INTERVAL, m_conn_interval);
	DDX_Text(pDX, IDC_CONN_TIMEOUT, m_conn_timeout);
	DDX_Text(pDX, IDC_CONN_CHNMASK, m_conn_chnmask);
	DDX_Check(pDX, IDC_LOGEN, m_bLogEn);
	DDX_Check(pDX, IDC_HEX, m_bHex);
	DDX_Text(pDX, IDC_CMDINPUT, m_cmd_input);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTl_ble_moduleDlg, CDialog)
	//{{AFX_MSG_MAP(CTl_ble_moduleDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_INIFILE, OnSelchangeInifile)
	ON_LBN_SELCHANGE(IDC_COMMAND, OnSelchangeCommand)
	ON_LBN_DBLCLK(IDC_COMMAND, OnDblclkCommand)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_BN_CLICKED(IDC_CLOSEDLG, OnClosedlg)
	ON_BN_CLICKED(IDC_LOGCLEAR, OnLogclear)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SCAN, OnScan)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_LOGSAVE, OnLogsave)
	ON_BN_CLICKED(IDC_LOGSAVEFILE, OnLogsavefile)
	ON_BN_CLICKED(IDC_LOGEN, OnLogen)
	ON_BN_CLICKED(IDC_BULKOUT, OnBulkout)
	ON_BN_CLICKED(IDC_HEX, OnHex)
	ON_BN_CLICKED(IDC_CMDSEL, OnCmdsel)
	ON_BN_CLICKED(IDC_Device, OnDevice)
	ON_BN_CLICKED(IDC_OTA1, OnOta1)
	ON_BN_CLICKED(IDC_OTA2, OnOta2)
	ON_BN_CLICKED(IDC_OTA1SART, OnOta1sart)
	ON_BN_CLICKED(IDC_OTA2START, OnOta2start)
	//}}AFX_MSG_MAP

	ON_MESSAGE(WM_APPENDLOG, OnAppendLog)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
int		device_ok = 0;
HANDLE	m_hDev = NULL;
HANDLE	m_hDevW = NULL;
HANDLE NULLEVENT2 = CreateEvent( NULL, TRUE, FALSE, NULL ) ;
int		buff_bi_sel = 0;

UINT ThreadBulkIn ( void* pParams )
{
	BYTE		buff[32*1024];
	DWORD		nB;
    while ( 1 )    { 
		if (m_hDev != NULL) {
			int bok =  ReadFile(m_hDev, buff, 32*1024, &nB, NULL);
			if (bok) {	// send to log window
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_APPENDLOG, nB, (LPARAM)buff);
			}
			else {
				m_hDev = NULL;
			}
		}
		else {
				WaitForSingleObject (NULLEVENT2, 200);
				
				m_hDev = GetPrintDeviceHandle(DONGLE_BLE_MODULE_ID);
				m_hDevW = GetPrintDeviceHandle(DONGLE_BLE_MODULE_ID);
		}
	}
	return 1;
}

UINT ThreadBulkIn2 ( void* pParams )
{
	BYTE		buff[256];
	DWORD		nB;
    while ( 1 )    { 
		if (m_hDev != NULL) {
			int bok =  ReadFile(m_hDev, buff, 256, &nB, NULL);
			if (bok) {	// send to log window
				::SendMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_APPENDLOG, nB, (LPARAM)buff);
			}
			else {
				m_hDev = NULL;
			}
		}
		else {
				WaitForSingleObject (NULLEVENT2, 200);
				
				m_hDev = GetPrintDeviceHandle(DONGLE_BLE_MODULE_ID);
				m_hDevW = GetPrintDeviceHandle(DONGLE_BLE_MODULE_ID);
		}
	}
	return 1;
}
#define MAXB2TSIZE  1024*64
char arrb2t[] = "0123456789abcdef";
char arrh2i[256] = {
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		 0,  1,  2,  3,  4,  5,  6,  7,   8,  9, 16, 16, 16, 16, 16, 16,
		
		16, 10, 11, 12, 13, 14, 15, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16, 
		16, 10, 11, 12, 13, 14, 15, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,

		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,

		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16,  16, 16, 16, 16, 16, 16, 16, 16
	};


int Hex2Text (LPBYTE lpD, LPBYTE lpS, int n)
{
    int i = 0;
	int d = 0;
	
	if (n == 2) {
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 5;

	}
	else if (n == 3) {
		lpD[d++] = arrb2t [(lpS[2]>>4) & 15];
		lpD[d++] = arrb2t [lpS[2] & 15];
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 7;

	}
	else if (n == 4) {
		lpD[d++] = arrb2t [(lpS[3]>>4) & 15];
		lpD[d++] = arrb2t [lpS[3] & 15];
		lpD[d++] = arrb2t [(lpS[2]>>4) & 15];
		lpD[d++] = arrb2t [lpS[2] & 15];
		lpD[d++] = arrb2t [(lpS[1]>>4) & 15];
		lpD[d++] = arrb2t [lpS[1] & 15];
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 9;

	}
	else
	{
		lpD[d++] = arrb2t [(lpS[0]>>4) & 15];
		lpD[d++] = arrb2t [lpS[0] & 15];
		lpD[d++] = ' ';
		return 3;
	}
}

int Bin2Text (LPBYTE lpD, LPBYTE lpS, int n)
{
    int i = 0;
	int m = n;
	int d = 0;
	if (m>MAXB2TSIZE) m = MAXB2TSIZE;

	for (i=0; i<m; i++) {

		lpD[d++] = arrb2t [(lpS[i]>>4) & 15];
		lpD[d++] = arrb2t [lpS[i] & 15];
		lpD[d++] = ' ';

		if ((i&15)==7) lpD[d++] = ' ';
		else if ((i&15)==15) {
			lpD[d++] = '\r';
			lpD[d++] = '\n';
		}
		
	}
	//lpD[d++] = '\r';
	//lpD[d++] = '\n';
	lpD[d++] = '\0';
	return m;
}

int Text2Bin(LPBYTE lpD, LPBYTE lpS)
{
	// TODO: Add your control notification handler code here
		
	int n = 0;
	
	while (*lpS) {
		
		if (*lpS < 48) {
			lpS++;
		}
		else {
			
			int dh = *lpS++;
			int dl = *lpS++;	
			*lpD++ = ( arrh2i[dh] << 4) + arrh2i[dl];
			n++;
		}		
	}

	return n;
}

int Time2Text (LPBYTE lpD, unsigned int d)
{
   d = d / 32;
   int us = d % 1000;
   d = (d - us) / 1000;
   int ms = d % 1000;
   d = (d - ms) / 1000;
   
   sprintf ((char *)lpD, "%03d:%03d:%03d ", d, ms, us);
   return 12;
}

int		evt_mask[1024];
int		evt_val[1024];
int		evt_no = 0;
CString	evt_str[1024];

#define				MAX_FORMAT			256

int		format_buff[16*MAX_FORMAT];
CString	format_str[MAX_FORMAT];
int		format_num = 0;
/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg message handlers

BOOL CTl_ble_moduleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strOta1 = "";
	m_strOta2 = "";

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	GetCurrentDirectory (512, m_CurrentDir.GetBuffer(512));
	m_CurrentDir.ReleaseBuffer();

	m_nTextLen = 0x100000;
	m_Log.SetLimitText (m_nTextLen);

	// load ini file
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	CString strDir = m_CurrentDir + "\\tl_*.ini";
	hFind = FindFirstFile(strDir, &ffd);
	int ns = 0;

	if (hFind != INVALID_HANDLE_VALUE) {
      
		((CComboBox *) GetDlgItem (IDC_INIFILE))->ResetContent();
		do	{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
				//filesize.LowPart = ffd.nFileSizeLow;
				//filesize.HighPart = ffd.nFileSizeHigh;
				((CComboBox *) GetDlgItem (IDC_INIFILE))->AddString (ffd.cFileName);
				if (strcmp (ffd.cFileName, "wtcdb.ini") == 0) { 
					((CComboBox *) GetDlgItem (IDC_INIFILE))->SetCurSel(ns); 
				}
				ns++;
			}
		}	while (FindNextFile(hFind, &ffd) != 0);

		FindClose(hFind);
	}
	((CComboBox *) GetDlgItem (IDC_INIFILE))->SetCurSel (0);
	OnSelchangeInifile ();

	((CComboBox *) GetDlgItem (IDC_CHANNEL))->SetCurSel (1);

	AfxBeginThread( ThreadBulkIn, this, THREAD_PRIORITY_HIGHEST);

	//AfxBeginThread( ThreadBulkIn2, this, THREAD_PRIORITY_HIGHEST);

	SetTimer (1, 100, NULL);

	OnCmdsel ();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTl_ble_moduleDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTl_ble_moduleDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

#define INIBUFFSIZE 1024*32

int CTl_ble_moduleDlg::LoadIniFile() 
{
	
	int case_no = 0;
	CString str_last;
	CString str_pk;
	CString str;
	char key[INIBUFFSIZE]="default";
	char desc[128];
	DWORD nd;

	GetPrivateProfileString ("SET", NULL, "", key, INIBUFFSIZE, m_InitFile);

	format_num = 0;
	evt_no = 0;
	if (1) {
		nd = GetPrivateProfileString ("SET", NULL, "", key, INIBUFFSIZE, m_InitFile);
		char *pk = key;
		((CListBox  *) GetDlgItem (IDC_COMMAND))->ResetContent ();
		while (nd>0) {
			char * pdir = strstr (pk, "CMD-");

			if (pdir == pk) {
				str.Format ("%s", pk + 4);
				((CListBox  *) GetDlgItem (IDC_COMMAND))->AddString (str);
			}
			pdir = strstr (pk, "EVT-");
			if (pdir == pk) {

				format_str[format_num++].Format ("%s", pk+4);
			}

			while (nd>0 && *pk!=0) {pk++; nd--;}
			while (nd>0 && *pk==0) {pk++; nd--;}
		}

		((CComboBox  *) GetDlgItem (IDC_COMMAND))->SetCurSel (0);
		OnSelchangeCommand();
	}

#if 1
		//CString str;
		memset (format_buff, 0, sizeof (format_buff));
		for (int k=0; k<format_num; k++) {
			str.Format ("EVT-%s", format_str[k]);
			GetPrivateProfileString ("SET", str, "", key, 1024, m_InitFile);
			
			int pos = 0;
			int id = 0;
			int len = strlen (key);
			while (pos < len && id < 16) {
				while ((key[pos]==' ' || key[pos] == '\t') && pos < len) {pos++;}
				if (sscanf (key + pos, "%x", format_buff +id + k * 16) == 1)
				{
					id++;
					while ((key[pos]!=' ' && key[pos] != '\t') && pos < len) {pos++;}
				}
				else
				{
					break;
				} 
			}
			//sscanf (key, "%x %x", evt_mask + k, evt_val + k);
		}
#endif
	return 1;
}

void CTl_ble_moduleDlg::CTl_ble_module_Init() 
{
//	m_hDev = NULL;

	char buff[128];

	GetPrivateProfileString ("SET", "prnid", "ffff", buff, 1024, m_InitFile);
	sscanf (buff, "%x", &DONGLE_BLE_MODULE_ID); 
	m_hDev = NULL;

	GetPrivateProfileString ("SET", "i2c_id", "6e", buff, 1024, m_InitFile);
	sscanf (buff, "%x", &m_i2c_id);

	GetPrivateProfileString ("SET", "vidpid", "248a82bd", m_device_vidpid.GetBuffer(1024), 1024, m_InitFile);
	m_device_vidpid.ReleaseBuffer ();


	//////////////////////////////////////////////
	// Load BIN setting
	//////////////////////////////////////////////
	LoadIniFile ();

}


void CTl_ble_moduleDlg::OnSelchangeInifile() 
{
	// TODO: Add your control notification handler code here
	int ns = ((CComboBox *) GetDlgItem (IDC_INIFILE))->GetCurSel(); 
	((CComboBox *) GetDlgItem (IDC_INIFILE))->GetLBText (ns, m_InitStr.GetBuffer(256));
	m_InitStr.ReleaseBuffer ();
	m_InitFile = m_CurrentDir + "\\" + m_InitStr;
	if (ns >= 0) {
		m_cmd_input = "";
		CTl_ble_module_Init ();
	}	
}

void CTl_ble_moduleDlg::OnSelchangeCommand() 
{
	// TODO: Add your control notification handler code here
	UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);
		
		str.Format ("CMD-%s", cmd);
		GetPrivateProfileString ("SET", str, "", cmd, 1024, m_InitFile);

		m_cmd_input = cmd;
		UpdateData (FALSE);
	}				
}

void CTl_ble_moduleDlg::OnDblclkCommand() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	int step = 0;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);

		LogMsg (".. %s\r\n", cmd);
		
		str.Format ("CMD-%s", cmd);
		GetPrivateProfileString ("SET", str, "", cmd, 1024, m_InitFile);

		m_cmd_input = cmd;
		UpdateData (FALSE);

		ExecCmd (cmd);
	}			
}

int CTl_ble_moduleDlg::LogMsg (LPSTR sz,...)
{
    char ach[2048];

	if (!m_bLogEn)
		return 0;
    int n = wvsprintf(ach, sz, (LPSTR)(&sz+1));   /* Format the string */
    int len = m_Log.GetWindowTextLength();
	int nstep = m_nTextLen  / 8;
	if (len >= nstep * 7) {
		m_Log.SetSel (0, nstep*4);
		m_Log.ReplaceSel ("");
		len = m_Log.GetWindowTextLength();
	}
	m_Log.SetSel (len, len);
	m_Log.ReplaceSel (ach);

    return n;
}

int CTl_ble_moduleDlg::ExecCmd(LPCTSTR cmd) 
{
	BYTE	buff[1024];
	DWORD	nB;
	int n = Text2Bin (buff, (LPBYTE) cmd);
	
	int bok =  WriteFile(m_hDevW, buff, n, &nB, NULL);

	return 1;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void CTl_ble_moduleDlg::OnOpen() 
{
	// TODO: Add your control notification handler code here
	// TODO: Add your control notification handler code here
	char cmd[] = "C:\\Windows\\notepad.exe";
	char cmdline[1024];
	sprintf (cmdline, "C:\\Windows\\notepad.exe %s", m_InitStr);

	DoCmd (cmd, cmdline, m_CurrentDir, 0);
}

void CTl_ble_moduleDlg::DoCmd(LPCTSTR cmd, LPSTR cmdline, LPCTSTR curdir, int nownd = 0)
{
	PROCESS_INFORMATION pif;  //Gives info on the thread and..
                           //..process for the new process
	STARTUPINFO si;          //Defines how to start the program

	ZeroMemory(&si,sizeof(si)); //Zero the STARTUPINFO struct
	si.cb = sizeof(si);         //Must set size of structure
	//si.dwFlags = STARTF_USESHOWWINDOW;

	BOOL bRet = CreateProcess(
		cmd,  
		cmdline, //NULL,   //Command string - not needed here
		NULL,   //Process handle not inherited
		NULL,   //Thread handle not inherited
		FALSE,  //No inheritance of handles
		nownd,      //No special flags
		NULL,   //Same environment block as this prog
		curdir,   //Current directory - no separate path
		&si,    //Pointer to STARTUPINFO
		&pif);   //Pointer to PROCESS_INFORMATION

	if(bRet == FALSE)  {
		AfxMessageBox("Unable to start program");
		return;
	}
	//TerminateProcess (pif.hProcess, -2);
	CloseHandle(pif.hProcess);   //Close handle to process
	CloseHandle(pif.hThread);    //Close handle to thread	
}

void CTl_ble_moduleDlg::OnOK() 
{
	// TODO: Add extra validation here
	//CDialog::OnOK();
	UpdateData (TRUE);

	LogMsg (".. %s\r\n", m_cmd_input);
	ExecCmd (m_cmd_input);
}


void CTl_ble_moduleDlg::OnClosedlg() 
{
	// TODO: Add your control notification handler code here
	KillTimer (1);
	CDialog::OnOK ();
}


int sn = 0;

int CTl_ble_moduleDlg::format_parse (unsigned char *ps, int len, unsigned char *pd)
{
	if (ps[3] == 3)
	{
		ps[3] = 3;
	}
	unsigned char *po, *pi;
	for (int i=0; i<format_num; i++) 
	{
		int match = 1;
		int *pw = format_buff + i * 16;
		pi = ps; //+ 1;
		po = pd;
		for (int j=0; j<16 && pw[j]; j++)
		{
			////////////////// format check ////////////////
			unsigned char *pb = (unsigned char *)(pw + j);
			unsigned int d = pi[0] + pi[1] * 256;
			int nb = pb[3] & 15;
			if (nb >= 3)
				d += pi[2] << 16;
			if (nb == 4)
				d += pi[3] << 24;
			int s = pb[2] & 15;
			int e = pb[2] >> 4;
			int m = e < s ? 0 : ((1 << (e - s + 1)) - 1) << s ;
			//LogMsg ("%x %x %d %d %d\r\n", pw[j], d, s, e, m);
			if ( (d & m) != ((pb[0] + pb[1] * 256) & m) )
			{
				match = 0;
				break;
			}
			if (pb[3] == 0x84)			//time stamp
			{
				po += Time2Text (po, d);
			}
			else
			{
				po += Hex2Text (po, pi, nb);
			}

			pi += nb;
		}
		if (match)
		{
			int n = len - (pi - ps);
			if (n > 0) {
				Bin2Text (po, pi, n);
			}
			else {
				*po = 0;
			}
			return i;
		}
	
	}
	
	Bin2Text (pd, ps, len);
	return -1;
}

LRESULT  CTl_ble_moduleDlg::OnAppendLog (WPARAM wParam, LPARAM lParam )
{

	int		n = (int) wParam;
	LPBYTE	pu = (unsigned char *) lParam;
	BYTE	buff[1024*32];
	int		offset;

	if (!m_bHex)
	{
		pu[n] = 0;
		LogMsg ("%s", pu);
		return 0;
	}

	if (1)
	{

#if 0
		int c = n > 4096 ? 4096 : n;
		Bin2Text (buff, pu, c);
		LogMsg ("%s\r\n", buff);
		n -= c;
#else
		if (1) {

			if (m_pScanDlg)
			{
				m_pScanDlg->AddDevice (pu, n);
			}
			int id = format_parse(pu, n, buff);
			if (id >= 0)
			{
				LogMsg ("%s\r\n", format_str[id]);
			}
			else
			{
				LogMsg (" ..data..\r\n");
			}
			//Bin2Text (buff, pu+1, pu[0]);
			LogMsg (" <%d>  %s\r\n", sn, buff);
			offset = pu[0] + 1;
		}
		sn++;
		n -= offset;
		pu += offset;
#endif
	}

	return 0;
}


void CTl_ble_moduleDlg::OnLogclear() 
{
	// TODO: Add your control notification handler code here
	sn = 0;
	m_Log.SetWindowText ("");	
}

void CTl_ble_moduleDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1) {
		if (m_hDev != NULL) {
			if (1 || !device_ok) {
				device_ok = 1;
				SetWindowText ("Telink Bulk Out/In -- Found");
				((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (1);

			}
		}
		else {
			if (1 || device_ok) {
				device_ok = 0;
				SetWindowText ("Telink Bulk Out/In -- Not Found");
			//	((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (0);
				UpdateData (FALSE);
			}
		}

	}
	CDialog::OnTimer(nIDEvent);
}


void CTl_ble_moduleDlg::SetHostInterface() 
{
	UpdateData ();
//	int dat = 0x80 | (m_bif_poll << 6) | m_bif_spi | (m_i2c_id<<8);
//	WriteMem (m_hDevW, 0x8008, (LPBYTE) &dat, 2, 2);
	((CListBox  *) GetDlgItem (IDC_COMMAND))->EnableWindow (1);
}

unsigned	char buff_mac[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned	char buff_start[] = {0x02, 0x00};
unsigned	char buff_stop[] = {0x03, 0x00};
unsigned	char buff_conn[16] = {0x04, 38, 0x06, 0x00, 0x00, 0x04, 0xff, 0xff, 0xff, 0xff, 0x1f};


void CTl_ble_moduleDlg::OnScan() 
{
	// TODO: Add your control notification handler code here
	char cmd[1024];
	BYTE	conn[16] = {0xfe, 0xff, 0x07, 0x00, 0x00};
	DWORD nB;
	CScanDlg dlg;
	m_pScanDlg = &dlg;
	
	{
		UpdateData ();
		int ns = ((CComboBox *) GetDlgItem (IDC_CHANNEL))->GetCurSel ();
		buff_conn[1] = 37 + ns;
		buff_conn[2] = m_conn_interval;
		buff_conn[3] = m_conn_interval >> 8;
		buff_conn[4] = m_conn_timeout;
		buff_conn[5] = m_conn_timeout >> 8;
		
		char buff_mask[32];
		sprintf (buff_mask, "%s", m_conn_chnmask);
		Text2Bin (buff_conn + 6, (LPBYTE) buff_mask);
		
	//	WriteFile(m_hDevW, buff_mac, 6, &nB, NULL);
	//	WaitForSingleObject (NULLEVENT2, 10);
		
	//	WriteFile(m_hDevW, buff_conn, 11, &nB, NULL);
	//	WaitForSingleObject (NULLEVENT2, 10);
		
	//	WriteFile(m_hDevW, buff_start, 1, &nB, NULL);

			LogMsg ("..scan %s\r\n");
			m_bLogEn = 0;
		if (dlg.DoModal () == IDOK)
		{
			memcpy (conn + 5, dlg.m_mac + 1, 6);
			WriteFile(m_hDevW, conn, 11, &nB, NULL);
			WaitForSingleObject (NULLEVENT2, 10);
			
			m_status = 1;
			m_bLogEn = 1;
			LogMsg (".. %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", conn[0], conn[1], conn[2], conn[3],
				conn[4], conn[5], conn[6], conn[7], conn[8], conn[9], conn[10]);
			//GetDlgItem (IDC_SCAN)->SetWindowText ("Stop");
			m_bLogEn = 0;
		}
	}

	m_pScanDlg = NULL;
}

void CTl_ble_moduleDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	DWORD nB;
	WriteFile(m_hDevW, buff_stop, 1, &nB, NULL);
	WaitForSingleObject (NULLEVENT2, 10);
	m_status = 0;	
}

void CTl_ble_moduleDlg::OnLogsave() 
{
	// TODO: Add your control notification handler code here

		UpdateData ();
	short kc = GetKeyState(VK_LCONTROL) >> 8;
	short ks = GetKeyState(VK_LSHIFT) >> 8;

	char cmd[1024];
	int ns = ((CListBox *) GetDlgItem (IDC_COMMAND))->GetCurSel ();
	CString str;
	int step = 0;
	if (ns >= 0) {
		((CListBox *) GetDlgItem (IDC_COMMAND))->GetText (ns, cmd);
		
		str = cmd;
		int n = str.Find("__");

		sscanf (cmd + n + 2, "%d", &step);
		str = str.Left (n);
		str = str.Right (str.GetLength () - str.Find(" ") - 1);
		str.Replace ('/', '.');
		
		CFile file;
		CFileException fe;

		if (!file.Open(m_CurrentDir + "\\log\\" + str + ".txt", CFile::modeCreate |
		  CFile::modeReadWrite | CFile::shareExclusive, &fe))
		{
			AfxMessageBox ("Cannot open output file");
			return;
		}

		CString text;
		m_Log.GetWindowText(text);
		file.Write(text, text.GetLength());
		file.Close();

	}			


		
}

void CTl_ble_moduleDlg::OnLogsavefile() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "Log Files (*.txt)|*.txt|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, NULL, NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

 	if (dlg.DoModal()!=IDOK) return;

	CFile file;
	CFileException fe;

	if (!file.Open(dlg.GetPathName(), CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		AfxMessageBox ("Cannot open output file");
		return;
	}

	CString str;
	m_Log.GetWindowText(str);
	file.Write(str, str.GetLength());
	file.Close();		
}

void CTl_ble_moduleDlg::OnLogen() 
{
	// TODO: Add your control notification handler code here
	UpdateData ();

}

void CTl_ble_moduleDlg::OnBulkout() 
{
	// TODO: Add your control notification handler code here

	//UpdateData ();
	CString str;
	m_textBO.GetWindowText(str);
	
	//LogMsg ("%s", str);
	DWORD nB;
	BYTE   buff[32*1024];
	nB = str.GetLength();
	if (nB > 32 * 1024)
		nB = 32 * 1024;
	memcpy (buff, str, nB);
	int bok =  WriteFile(m_hDevW, buff, nB, &nB, NULL);

}

void CTl_ble_moduleDlg::OnHex() 
{
	// TODO: Add your control notification handler code here
	UpdateData ();
}

void CTl_ble_moduleDlg::OnCmdsel() 
{
	// TODO: Add your control notification handler code here
	static int bcmd = 0;
	bcmd = !bcmd;
	if (bcmd)
	{
		m_textBO.ShowWindow(FALSE);
		((CListBox *) GetDlgItem (IDC_COMMAND))->ShowWindow(TRUE);
		GetDlgItem (IDC_CMDSEL)->SetWindowText("CMD");
	}
	else
	{
		m_textBO.ShowWindow(TRUE);
		((CListBox *) GetDlgItem (IDC_COMMAND))->ShowWindow(FALSE);
		GetDlgItem (IDC_CMDSEL)->SetWindowText("ASCII");
	}
}

void CTl_ble_moduleDlg::OnDevice() 
{
	// TODO: Add your control notification handler code here
	char cmd[1024];
	BYTE	conn[16] = {0xfe, 0xff, 0x07, 0x00, 0x00};
	DWORD nB;
	CScanDlg dlg;
	m_pScanDlg = &dlg;
	
	{
		UpdateData ();

		if (dlg.DoModal () == IDOK)
		{
			memcpy (conn + 5, dlg.m_mac + 1, 6);
			WriteFile(m_hDevW, conn, 11, &nB, NULL);
			WaitForSingleObject (NULLEVENT2, 10);
			WaitForSingleObject (NULLEVENT2, 10);
			
			m_status = 1;
			//GetDlgItem (IDC_SCAN)->SetWindowText ("Stop");
		}
	}

	m_pScanDlg = NULL;	
}

void CTl_ble_moduleDlg::CheckOtaFile(CString strfile, int no) 
{
	CFile file;
	CFileException fe;

	if (!file.Open(strfile, CFile::modeCreate |
	  CFile::modeReadWrite | CFile::shareExclusive, &fe))
	{
		LogMsg ("Cannot open file %s\r\n", strfile);
		return;
	}

	if (file.GetLength () < 1024)
	{
		AfxMessageBox ("File size incorrect");
	}
	file.Close();	
	if (no == 1)
		m_strOta1 = strfile;
	else
		m_strOta2 = strfile;
}


void CTl_ble_moduleDlg::OnOta1() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, NULL, m_CurrentDir,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

 	if (dlg.DoModal()!=IDOK) return;

	CheckOtaFile (dlg.GetPathName(), 1);
	
}

void CTl_ble_moduleDlg::OnOta2() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "BIN Files (*.bin)|*.bin|All Files (*.*)|*.*||";
	CFileDialog dlg(FALSE, NULL, m_CurrentDir,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

 	if (dlg.DoModal()!=IDOK) return;

	CheckOtaFile (dlg.GetPathName(), 2);
	
}

void CTl_ble_moduleDlg::OnOta1sart() 
{
	// TODO: Add your control notification handler code here
	if (m_strOta1 == "")
	{
		return;
	}
}

void CTl_ble_moduleDlg::OnOta2start() 
{
	// TODO: Add your control notification handler code here
	if (m_strOta2 == "")
	{
		return;
	}	
}
