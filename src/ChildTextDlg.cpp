// ChildTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditDlg.h"

#include "Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildTextDlg dialog


CChildTextDlg::CChildTextDlg(CWnd* pParent /*=NULL*/) :
	CTabPage(CChildTextDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChildTextDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChildTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildTextDlg)
	DDX_Control(pDX, IDC_BUTTON_ANSWER, m_cBtnAsw);
	DDX_Control(pDX, IDC_EDIT_ANSWER, m_cEditAnswer);
	DDX_Control(pDX, IDC_STATIC_GROUP1, m_cGroup1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChildTextDlg, CTabPage)
	//{{AFX_MSG_MAP(CChildTextDlg)
	ON_EN_CHANGE(IDC_EDIT_ANSWER, OnChange)
	ON_BN_CLICKED(IDC_BUTTON_ANSWER, OnButtonAnswer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildTextDlg message handlers

void CChildTextDlg::SetShow(const CItem *pItem)
{
	if (!pItem)
	{
		SetDlgItemText(IDC_EDIT_ANSWER, _T(""));
	}
	else
	{
		SetDlgItemText(IDC_EDIT_ANSWER, ((CText*)pItem)->GetAnswer());
	}

	m_bModify = FALSE;
}

int CChildTextDlg::CheckSaveable()
{
	if (!GetDlgItem(IDC_EDIT_ANSWER)->GetWindowTextLength())
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, IDS_EDITDLGT_NOASW, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
		case IDYES:
			break;

		case IDNO:
			return SAVEABLE_DENY;

		default:
			ASSERT(FALSE);
		}
	}

	return SAVEABLE_OK;
}

void CChildTextDlg::GetInfo(CText *pText)
{
	if (!pText || !m_bModify)
	{
		return;
	}
	
	CString string;
	GetDlgItemText(IDC_EDIT_ANSWER, string);
	pText->SetAnswer(string, -1);
}

void CChildTextDlg::OnChange() 
{
	m_bModify = TRUE;
}

void CChildTextDlg::OnButtonAnswer() 
{
	LPTSTR ptr = ((CEditDlg*)(GetParent()->GetParent()))->GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(IDC_EDIT_ANSWER, ptr);
		free(ptr);
	}

	m_bModify = TRUE;
}
