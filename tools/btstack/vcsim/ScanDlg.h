#if !defined(AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_)
#define AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScanDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanDlg dialog

class CScanDlg : public CDialog
{
// Construction
public:
	CScanDlg(CWnd* pParent = NULL);   // standard constructor
	char m_mac[8];
	void AddMac (unsigned char * pmac, int rssi);
	void AddDevice (unsigned char * p, int n);
	char amac[1024];
	int	 nmac;

// Dialog Data
	//{{AFX_DATA(CScanDlg)
	enum { IDD = IDD_TL_SCAN_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanDlg)
	afx_msg void OnConnect();
	afx_msg void OnDblclkScanlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANDLG_H__459B6637_BEE5_457A_973D_D1DEFD32026D__INCLUDED_)
