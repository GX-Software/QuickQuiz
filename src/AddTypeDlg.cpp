// AddTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "AddTypeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddTypeDlg dialog


CAddTypeDlg::CAddTypeDlg(CWnd* pParent /*=NULL*/) :
	CClrDialog(CAddTypeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddTypeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAddTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddTypeDlg)
	DDX_Control(pDX, IDOK, m_cBtnOK);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_cCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddTypeDlg, CClrDialog)
	//{{AFX_MSG_MAP(CAddTypeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddTypeDlg message handlers

void CAddTypeDlg::OnOK() 
{
	EndDialog(m_cCombo.GetCurSel() + TYPE_SCHOISE);
}

void CAddTypeDlg::OnCancel()
{
	EndDialog(TYPE_DEFAULT);
}

BOOL CAddTypeDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	m_cCombo.SetCurSel(0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
