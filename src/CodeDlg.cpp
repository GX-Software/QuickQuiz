// CodeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "CodeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCodeDlg dialog


CCodeDlg::CCodeDlg(CWnd* pParent /*=NULL*/) :
	CClrDialog(CCodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCodeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

#ifdef _UNICODE
	m_nCS = CS_UTF16;
#else
	m_nCS = CS_ANSI;
#endif
}


void CCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_COMBO_CODE, m_cCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCodeDlg, CClrDialog)
	//{{AFX_MSG_MAP(CCodeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeDlg message handlers

BOOL CCodeDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	((CComboBox*)GetDlgItem(IDC_COMBO_CODE))->SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CCodeDlg::OnOK() 
{
	m_nCS = ((CComboBox*)GetDlgItem(IDC_COMBO_CODE))->GetCurSel();
	
	CDialog::OnOK();
}

