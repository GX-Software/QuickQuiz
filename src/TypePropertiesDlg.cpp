// TypePropertiesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "PropertiesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTypePropertiesDlg dialog


CTypePropertiesDlg::CTypePropertiesDlg(CPaper *pPaper, CWnd* pParent /*=NULL*/) :
	CClrDialog(CTypePropertiesDlg::IDD, pParent),
	m_pPaper(pPaper)
{
	//{{AFX_DATA_INIT(CTypePropertiesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTypePropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTypePropertiesDlg)
	DDX_Control(pDX, IDC_EDIT_QNUMBER, m_cEditNumber);
	DDX_Control(pDX, IDOK, m_cBtnOK);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTypePropertiesDlg, CClrDialog)
	//{{AFX_MSG_MAP(CTypePropertiesDlg)
	ON_EN_KILLFOCUS(IDC_EDIT_QNUMBER, OnKillFocusQNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTypePropertiesDlg message handlers

BOOL CTypePropertiesDlg::OnInitDialog() 
{
	CClrDialog::OnInitDialog();
	
	if (m_pList)
	{
		SetDlgItemText(IDC_EDIT_TYPE, m_pList->GetDescription());

		CPaper *pPaper = (CPaper*)m_pList->GetParent();
		if (pPaper)
		{
			SetDlgItemText(IDC_EDIT_PARENT, pPaper->GetDescription());
		}
		else
		{
			CString string;
			string.LoadString(IDS_PROPS_NOPARENT);
			SetDlgItemText(IDC_EDIT_PARENT, string);
		}
		
		SetDlgItemInt(IDC_EDIT_ITEMCOUNT, m_pList->GetItemCount());

		if (m_pList->IsDefaultType())
		{
			CString string;
			string.LoadString(IDS_TYPEPROPT_DEFAULT);
			m_cEditNumber.SetWindowText(string);
			m_cEditNumber.SetReadOnly(TRUE);
		}
		else
		{
			SetDlgItemInt(IDC_EDIT_QNUMBER, m_pList->GetQNumber(), FALSE);
			m_cEditNumber.SetReadOnly(!m_pPaper->CanEdit());
		}
		m_cEditNumber.SetLimitText(3);
		m_cEditNumber.SetModify(FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTypePropertiesDlg::OnKillFocusQNumber() 
{
	int n = GetDlgItemInt(IDC_EDIT_QNUMBER);
	n = min(LIST_MAXQNUMBER, max(0, n));
}

void CTypePropertiesDlg::OnOK() 
{
	if (m_cEditNumber.GetModify())
	{
		// 核对题号是否有效
		int n = GetDlgItemInt(IDC_EDIT_QNUMBER);
		CList *pList = m_pPaper->CheckQNumberValid(n, m_pList);
		if (pList)
		{
			CTMessageBox Box;
			Box.SuperMessageBox(GetSafeHwnd(), IDS_TYPEPROPT_DUPL, IDS_MSGCAPTION_ERROR,
				MB_OK | MB_ICONERROR, 0, 0, pList->GetTypeDesc());
			m_cEditNumber.SetFocus();
			m_cEditNumber.SetSel(0, -1);
			return;
		}

		m_pList->SetQNumber(n);
	}

	CClrDialog::OnOK();
}
