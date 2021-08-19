// ChildSChoiseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"

#include "Choise.h"
#include "EditDlg.h"
#include "EditFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildSChoiseDlg dialog


CChildSChoiseDlg::CChildSChoiseDlg(CWnd* pParent /*=NULL*/)
	: CTabPage(CChildSChoiseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChildSChoiseDlg)
	m_nAnswer = -1;
	//}}AFX_DATA_INIT
}


void CChildSChoiseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildSChoiseDlg)
	DDX_Control(pDX, IDC_BUTTON_AUTO, m_cBtnAuto);
	DDX_Control(pDX, IDC_RADIO_ANSWER1, m_cRadioAnswer1);
	DDX_Control(pDX, IDC_RADIO_ANSWER2, m_cRadioAnswer2);
	DDX_Control(pDX, IDC_RADIO_ANSWER3, m_cRadioAnswer3);
	DDX_Control(pDX, IDC_RADIO_ANSWER4, m_cRadioAnswer4);
	DDX_Control(pDX, IDC_RADIO_ANSWER5, m_cRadioAnswer5);
	DDX_Control(pDX, IDC_RADIO_ANSWER6, m_cRadioAnswer6);
	DDX_Control(pDX, IDC_RADIO_ANSWER7, m_cRadioAnswer7);
	DDX_Control(pDX, IDC_RADIO_ANSWER8, m_cRadioAnswer8);
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
	DDX_Radio(pDX, IDC_RADIO_ANSWER1, m_nAnswer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChildSChoiseDlg, CTabPage)
	//{{AFX_MSG_MAP(CChildSChoiseDlg)
	ON_EN_CHANGE(IDC_EDIT_ANSWER1, OnChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER1, OnRadioChange)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_EN_CHANGE(IDC_EDIT_ANSWER2, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER3, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER4, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER5, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER6, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER7, OnChange)
	ON_EN_CHANGE(IDC_EDIT_ANSWER8, OnChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER2, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER3, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER4, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER5, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER6, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER7, OnRadioChange)
	ON_BN_CLICKED(IDC_RADIO_ANSWER8, OnRadioChange)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_SET1, IDC_BUTTON_SET8, OnSet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildSChoiseDlg message handlers

BOOL CChildSChoiseDlg::OnInitDialog() 
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

void CChildSChoiseDlg::SetShow(const CItem *pChoise)
{
	int i;
	
	m_nAnswer = -1;
	if (!pChoise)
	{
		for (i = 0; i < 8; i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, _T(""));
		}
	}
	else
	{
		CChoise *ptr = (CChoise*)pChoise;
		
		for (i = 0; i < ptr->GetOptionCount(); i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, ptr->GetOption(i));
			if (ptr->GetAnswer(i))
			{
				m_nAnswer = i;
			}
		}
		for (; i < 8; i++)
		{
			SetDlgItemText(IDC_EDIT_ANSWER1 + i, _T(""));
		}
	}
	
	m_bModify = FALSE;
	UpdateData(FALSE);
}

int CChildSChoiseDlg::CheckSaveable()
{
	int i, nOptionCount = 0;
	CString str;

	UpdateData();

	for (i = 0; i < MAXOPTIONNUM; i++)
	{
		GetDlgItemText(IDC_EDIT_ANSWER1 + i, str);
		if (CTextManager::CheckValidCharactor(str))
		{
			nOptionCount++;
		}
		else
		{
			if (m_nAnswer == i)
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

	return SAVEABLE_OK;
}

void CChildSChoiseDlg::GetInfo(CChoise *pChoise)
{
	if (!pChoise)
	{
		return;
	}
	
	int i;
	CString string;
	
	pChoise->ClearAllOptions();
	UpdateData();
	
	for (i = 0; i < MAXOPTIONNUM; i++)
	{
		GetDlgItemText(IDC_EDIT_ANSWER1 + i, string);
		if (!string.GetLength())
		{
			continue;
		}
		
		pChoise->AddOption(string, (i == m_nAnswer));
	}
}

void CChildSChoiseDlg::OnChange()
{
	m_bModify = TRUE;
}

void CChildSChoiseDlg::OnRadioChange()
{
	int nSave = m_nAnswer;

	UpdateData();
	if (m_nAnswer != nSave)
	{
		m_bModify = TRUE;
	}
}

void CChildSChoiseDlg::OnSet(UINT uID)
{
	LPTSTR ptr = ((CEditDlg*)(GetParent()->GetParent()))->GetEditFrmSel();
	if (ptr)
	{
		SetDlgItemText(uID - IDC_BUTTON_SET1 + IDC_EDIT_ANSWER1, ptr);
		free(ptr);
	}

	m_bModify = TRUE;
}

void CChildSChoiseDlg::OnButtonAuto() 
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
