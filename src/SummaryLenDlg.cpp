// SummaryLenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "SummaryLenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSummaryLenDlg dialog


CSummaryLenDlg::CSummaryLenDlg(int nLen, CWnd* pParent /*=NULL*/) :
	CClrDialog(CSummaryLenDlg::IDD, pParent),
	m_nLen(nLen)
{
	//{{AFX_DATA_INIT(CSummaryLenDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSummaryLenDlg::DoDataExchange(CDataExchange* pDX)
{
	CClrDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSummaryLenDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_EDIT_SUMMARYLEN, m_cEditLen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSummaryLenDlg, CClrDialog)
	//{{AFX_MSG_MAP(CSummaryLenDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSummaryLenDlg message handlers

void CSummaryLenDlg::OnOK() 
{
	m_nLen = GetDlgItemInt(IDC_EDIT_SUMMARYLEN);
	m_nLen = max(20, min(120, m_nLen));
	
	CClrDialog::OnOK();
}

BOOL CSummaryLenDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	m_cEditLen.SetLimitText(3);
	SetDlgItemInt(IDC_EDIT_SUMMARYLEN, m_nLen);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
