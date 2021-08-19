// ChildJudgeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditDlg.h"

#include "Judge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildJudgeDlg dialog


CChildJudgeDlg::CChildJudgeDlg(CWnd* pParent /*=NULL*/) :
	CTabPage(CChildJudgeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChildJudgeDlg)
	m_nAnswer = -1;
	//}}AFX_DATA_INIT
}


void CChildJudgeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildJudgeDlg)
	DDX_Control(pDX, IDC_RADIO_TRUE, m_cRadioTrue);
	DDX_Control(pDX, IDC_RADIO_FALSE, m_cRadioFalse);
	DDX_Radio(pDX, IDC_RADIO_TRUE, m_nAnswer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChildJudgeDlg, CTabPage)
	//{{AFX_MSG_MAP(CChildJudgeDlg)
	ON_BN_CLICKED(IDC_RADIO_TRUE, OnRadio)
	ON_BN_CLICKED(IDC_RADIO_FALSE, OnRadio)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildJudgeDlg message handlers

BOOL CChildJudgeDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChildJudgeDlg::SetShow(const CItem *pJudge)
{
	if (!pJudge)
	{
		m_nAnswer = -1;
	}
	else
	{
		CJudge *ptr = (CJudge*)pJudge;
		
		switch(((CJudge*)pJudge)->GetAnswer())
		{
		case -1:
			m_nAnswer = -1;
			break;

		case FALSE:
			m_nAnswer = 1;
			break;

		case TRUE:
			m_nAnswer = 0;
			break;
		}
	}
	
	UpdateData(FALSE);
	m_bModify = FALSE;
}

void CChildJudgeDlg::GetInfo(CJudge *pJudge)
{
	if (!pJudge)
	{
		return;
	}
	
	UpdateData();

	switch(m_nAnswer)
	{
	case -1:
		pJudge->SetAnswer(-1);
		break;

	case 0:
		pJudge->SetAnswer(TRUE);
		break;

	case 1:
		pJudge->SetAnswer(FALSE);
		break;
	}
}

void CChildJudgeDlg::OnRadio() 
{
	m_bModify = TRUE;
}
