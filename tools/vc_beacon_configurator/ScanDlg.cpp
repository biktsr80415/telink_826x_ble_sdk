// ScanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tl_ble_module.h"
#include "ScanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScanDlg dialog


CScanDlg::CScanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScanDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_mac[0] = 1;
	nmac = 0;
}


void CScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScanDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScanDlg, CDialog)
	//{{AFX_MSG_MAP(CScanDlg)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	ON_LBN_DBLCLK(IDC_SCANLIST, OnDblclkScanlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScanDlg message handlers

void CScanDlg::OnConnect() 
{
	// TODO: Add your control notification handler code here
	int n = ((CListBox *) GetDlgItem (IDC_SCANLIST))->GetCurSel ();
	char buff[64];
	int d0, d1, d2, d3, d4, d5;
	if (n >= 0) {
			((CListBox *) GetDlgItem (IDC_SCANLIST))->GetText (n, buff);
			sscanf (buff, "%x %x %x %x %x %x", &d0, &d1, &d2, &d3, &d4, &d5);
			m_mac[1] = d0;
			m_mac[2] = d1;
			m_mac[3] = d2;
			m_mac[4] = d3;
			m_mac[5] = d4;
			m_mac[6] = d5;
			OnOK ();
	}
	else
	{
		OnCancel ();
	}
	
}


void CScanDlg::AddMac(unsigned char * p_mac, int rssi) 
{
	p_mac[6] = 0;
	for (int i=0; i<nmac; i++) {
		if (strcmp (amac+i*8, (char *) p_mac) == 0) {
			return;
		}
	}
	nmac++;
	memcpy (amac+i*8, p_mac, 6);
	amac[i*8+6] = 0;
	CString str;
	str.Format ("%02x %02x %02x %02x %02x %02x %d dBm", 
		p_mac[0], p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], rssi);

	((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
}

void CScanDlg::AddDevice(unsigned char * p, int n) 
{
	unsigned char p_mac[8];
	signed short dc = p[n-2] | (p[n-1] << 8);
	signed char rssi = p[n-3];
	char scan_req[] = "scan request";
	char connect_req[] = "connect request";
	char *ps;

	if (p[0] != 0x04 || p[1] != 0x3e || p[3] != 0x02)
	{
		return;
	}
	memcpy (p_mac, p + 7, 6);

	p_mac[6] = 0;
	for (int j=0; j<nmac; j++) {
		if (strcmp (amac+j*8, (char *) p_mac) == 0) {
			return;
		}
	}
	nmac++;
	memcpy (amac+j*8, p_mac, 6);
	amac[j*8+6] = 0;

	//-------------- find device name -------------------------

	for (int i = 14; i < n; )
	{
		if (p[i + 1] == 9)
		{
			p[i + p[i] + 1] = 0;
			break;
		}
		i += p[i] + 1;
		if (!p[i])
		{
			i = n;
		}
	}

	if (i >= n)
	{
		p[i + 2] = 0;
	}

	if (p[5] == 0x03)
		ps = scan_req;
	else if (p[5] == 0x05)
		ps = connect_req;
	else
		ps = (char *)p + i + 2;
	//-------------- device name ------------------------------
	CString str;
	str.Format ("%02x %02x %02x %02x %02x %02x %d dBm %d K (%s)", 
		p_mac[0], p_mac[1], p_mac[2], p_mac[3], p_mac[4], p_mac[5], rssi, dc, ps);

	((CListBox *) GetDlgItem (IDC_SCANLIST))->AddString (str);
}

void CScanDlg::OnDblclkScanlist() 
{
	// TODO: Add your control notification handler code here
	OnConnect ();
}
