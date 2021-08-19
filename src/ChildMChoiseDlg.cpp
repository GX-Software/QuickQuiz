// ChildChoiseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditDlg.h"

#include "Choise.h"
#include "EditFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildMChoiseDlg dialog


CChildMChoiseDlg::CChildMChoiseDlg(CWnd* pParent /*=NULL*/) :
	CTabPage(CChildMChoiseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChildMChoiseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CChildMChoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildMChoiseDlg)
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_cBtnAuto);
	DDX_Control(pDX, IDC_CHECK_ANSWER1, m_cCheckAnswer1);
	DDX_Control(pDX, IDC_CHECK_ANSWER2, m_cCheckAnswer2);
	DDX_Control(pDX, IDC_CHECK_ANSWER3, m_cCheckAnswer3);
	DDX_Control(pDX, IDC_CHECK_ANSWER4, m_cCheckAnswer4);
	DDX_Control(pDX, IDC_CHECK_ANSWER5, m_cCheckAnswer5);
	DDX_Control(pDX, IDC_CHECK_ANSWER6, m_cCheckAnswer6);
	DDX_Control(pDX, IDC_CHECK_ANSWER7, m_cCheckAnswer7);
	DDX_Control(pDX, IDC_CHECK_ANSWER8, m_cCheckAnswer8);
	DDX_Control(pDX, IDC_BUTTON_SET1, m_cBtnSet1);
	DDX_Control(pDX, IDC_BUTTON_SET2, m_cBtnSet2);
	DDX_Control(pDX, IDC_BUTTON_SET3, m_cBtnSet3);
	DDX_Control(pDX, IDC_BUTTON_SET4, m_cBtnSet4);
	DDX_Control(pDX, IDC_BUTTON_SET5, m_cBtnSet5);
	DDX_Control(pDX, IDC_BUTTON_SET6, m_cBtnSet6);
	DDX_Control(pDX, IDC_BUTTON_SET7, m_cBtnSet7);
	DDX_Control(pDX, IDC_BUTTON_SET8, m_cBtnSet8);
	DDX_Control(pDX, IDC_EDIT_ANSWER1, m_cEditAnswer1);
	DDX_Control(pDX, IDC_EDIT_ANSWER2, m_cEditAnswer2);
	DDX_Control(pDX, IDC_EDIT_ANSWER3, m_cEditAnswer3);
	DDX_Control(pDX, IDC_EDIT_ANSWER4, m_cEditAnswer4);
	DDX_Control(pDX, IDC_EDIT_ANSWER5, m_cEditAnswer5);
	DDX_Control(pDX, IDC_EDIT_ANSWER6, m_cEditAnswer6);
	DDX_Control(pDX, IDC_EDIT_ANSWER7, m_cEditAnswer7);
	DDX_Control(pDX, IDC_EDIT_ANSWER8, m_cEditAnswer8);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChildMChoiseDlg, CTabPage)
	//{{AFX_MSG_MAP(CChildMChoiseDlg)
	ON_EN_CHANGE(IDC_EDIT_ANSWER1, OnChange)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_EN_CHANGE(IDC_EDIT_ANSWER2, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER3, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER4, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER5, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER6, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER7, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER8, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER1, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER2, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER3, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER4, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER5, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER6, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER7, OnChange)
	ON_BN_CLICKED(IDC_CHECK_ANSWER8, OnChange)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_SET1, IDC_BUTTON_SET8, OnSet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildMChoiseDlg message handlers

BOOL CChildMChoiseDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();
	
	int i;
	for (i = 0; i < 8; i++)
	{
		((CEdit*)GetDlgItem(IDC_EDIT_ANSWER1 + i))->SetLimitText(MAX_OPTION_LEN);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChildMChoiseDlg::SetShow(const CItem *pChoise)
{
	int i;
	
	if (!pChoise)
	{
		for (i = 0; i < 8; i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, _T(""));
			((CButton*)GetDlgItem(IDC_CHECK_ANSWER1 + i))->SetCheck(FALSE);
		}
	}
	else
	{
		CChoise *ptr = (CChoise*)pChoise;
		
		for (i = 0; i < ptr->GetOptionCount(); i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, ptr->GetOption(i));
			((CButton*)GetDlgItem(IDC_CHECK_ANSWER1 + i))->SetCheck(ptr->GetAnswer(i));
		}
		for (; i < 8; i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, _T(""));
			((CButton*)GetDlgItem(IDC_CHECK_ANSWER1 + i))->SetCheck(FALSE);
		}
	}
	
	m_bModify = FALSE;
}

int CChildMChoiseDlg::CheckSaveable()
{
	int i, nOptionCount = 0, nAnswerCount = 0;
	BOOL bIsValid = 0;
	BOOL bEmptyAnswer = FALSE;
	CString str;

	for (i = 0; i < MAXOPTIONNUM; i++)
	{
		GetDlgItemText(IDC_EDIT_ANSWER1 + i, str);

		bIsValid = CTextManager::CheckValidCharactor(str);
		if (bIsValid)
		{
			nOptionCount++;
		}

		if (((CButton*)GetDlgItem(IDC_CHECK_ANSWER1 + i))->GetCheck())
		{
			if (bIsValid)
			{
				nAnswerCount++;
			}
			else
			{
				bEmptyAnswer = TRUE;
			}
		}
	}

	if (nOptionCount < 2)
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, IDS_EDITDLGC_ONLYOPTION, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
		case IDYES:
			break;

		case IDNO:
			return SAVEABLE_DENY;

		default:
			ASSERT(FALSE);
		}
	}

	if (bEmptyAnswer)
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, IDS_EDITDLGC_INVALID, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
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

void CChildMChoiseDlg::GetInfo(CChoise *pChoise)
{
	if (!pChoise)
	{
		return;
	}
	
	int i;
	CString string;
	
	pChoise->ClearAllOptions();
	
	for (i = 0; i < MAXOPTIONNUM; i++)
	{
		GetDlgItemText(IDC_EDIT_ANSWER1 + i, string);
		if (!string.GetLength())
		{
			continue;
		}
		
		pChoise->AddOption(string, ((CButton*)GetDlgItem(IDC_CHECK_ANSWER1 + i))->GetCheck());
	}
}

void CChildMChoiseDlg::OnChange()
{
	m_bModify = TRUE;
}

void CChildMChoiseDlg::OnSet(UINT uID)
{
	LPTSTR ptr = ((CEditDlg*)(GetParent()->GetParent()))->GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(uID - IDC_BUTTON_SET1 + IDC_EDIT_ANSWER1, ptr);
		free(ptr);
	}

	m_bModify = TRUE;
}

void CChildMChoiseDlg::OnButtonAuto() 
{
	LPTSTR ptr = ((CEditDlg*)(GetParent()->GetParent()))->GetEditFrmSel();
	if (!ptr)
	{
		return;
	}

	LPWSTR wptr = (LPWSTR)CTextManager::SwitchSave(ptr, CS_UTF16, NULL);
	if (!wptr)
	{
		free(ptr);
		return;
	}
	
	// 开始分析字串了
	CChoise Choise(TRUE);
	if (!Choise.AutoOptions(wptr, 0))
	{
		free(ptr);
		return;
	}

	free(ptr);
	free(wptr);
	
	int i;
	for (i = 0; i < Choise.GetOptionCount(); i++)
	{
		SetDlgItemText(IDC_EDIT_ANSWER1 + i, Choise.GetOption(i));
	}
	for (; i < MAXOPTIONNUM; i++)
	{
		SetDlgItemText(IDC_EDIT_ANSWER1 + i, _T(""));
	}
}

