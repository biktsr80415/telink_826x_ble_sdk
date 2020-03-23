// tl_ble_moduleDlg.h : header file
//

#if !defined(AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_)
#define AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTl_ble_moduleDlg dialog

class CTl_ble_moduleDlg : public CDialog
{
// Construction
public:
	CTl_ble_moduleDlg(CWnd* pParent = NULL);	// standard constructor

	CString		m_device_vidpid;
	CString		m_InitFile, m_InitStr, m_ExeDir, m_AppCmd, m_CurrentDir;
	void		CTl_ble_module_Init ();
	int		LoadIniFile ();
	void	DoCmd(LPCTSTR cmd, LPSTR cmdline, LPCTSTR curdir, int nownd);
	int		ExecCmd(LPCTSTR cmd);
	int		LogMsg (LPSTR sz,...);
	int		m_nTextLen;
	void	SetHostInterface();
	int		m_i2c_id;
	CScanDlg	*m_pScanDlg;
	int		m_status;
	format_parse (unsigned char *ps, int len, unsigned char *pd);
// Dialog Data
	//{{AFX_DATA(CTl_ble_moduleDlg)
	enum { IDD = IDD_TL_BLE_MODULE_DIALOG };
	CEdit	m_textBO;
	CEdit	m_Log;
	int		m_conn_interval;
	int		m_conn_timeout;
	CString	m_conn_chnmask;
	BOOL	m_bLogEn;
	BOOL	m_bHex;
	CString	m_cmd_input;
	BOOL	m_btStack;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTl_ble_moduleDlg)
	protected:
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTl_ble_moduleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeInifile();
	afx_msg void OnSelchangeCommand();
	afx_msg void OnDblclkCommand();
	afx_msg void OnOpen();
	afx_msg void OnClosedlg();
	afx_msg void OnLogclear();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnScan();
	afx_msg void OnStop();
	afx_msg void OnLogsave();
	afx_msg void OnLogsavefile();
	afx_msg void OnLogen();
	afx_msg void OnBulkout();
	afx_msg void OnHex();
	afx_msg void OnCmdsel();
	afx_msg void OnDevice();
	afx_msg void OnBtstack();
	//}}AFX_MSG
	afx_msg LRESULT OnAppendLog (WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnExecCmd (WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TL_BLE_MODULEDLG_H__882B3038_F6FB_43FD_9579_899D142C510B__INCLUDED_)
