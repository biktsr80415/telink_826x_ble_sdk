#if !defined(AFX_SCANDIALOG_H__258AF0F5_0B51_445A_A77D_320047EB8230__INCLUDED_)
#define AFX_SCANDIALOG_H__258AF0F5_0B51_445A_A77D_320047EB8230__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScanDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScanDialog dialog

class CScanDialog : public CDialog
{
// Construction
public:
	CScanDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CScanDialog)
	enum { IDD = IDD_TL_BLE_SCAN };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScanDialog)
	afx_msg void OnDblclkScanList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANDIALOG_H__258AF0F5_0B51_445A_A77D_320047EB8230__INCLUDED_)
