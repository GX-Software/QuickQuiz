// TestBar.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TestBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestBar dialog


CTestBar::CTestBar(CWnd* pParent /*=NULL*/) :
	m_lTime(0), m_cProgress(SetProgressText),
	m_bNoTimeLimit(FALSE)
{
	//{{AFX_DATA_INIT(CTestBar)
	//}}AFX_DATA_INIT
}


void CTestBar::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CTestBar)
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	DDX_Control(pDX, IDC_CHECK_SHOWSINGLE, m_cCheckSingle);
	DDX_Control(pDX, IDC_CHECK_ONLYFALSE, m_cCheckOnlyFalse);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_cBtnSave);
	DDX_Control(pDX, IDC_COMBO_FONTSIZE, m_cComboFont);
	DDX_Control(pDX, IDC_PROGRESS_TIME, m_cProgress);
	DDX_Control(pDX, IDC_CHECK_TOPMOST, m_cCheckTopMost);
	DDX_Control(pDX, IDC_BUTTON_MARK, m_cBtnMark);
	DDX_Control(pDX, IDC_BUTTON_HANDIN, m_cBtnHandIn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestBar, CDialogBar)
	//{{AFX_MSG_MAP(CTestBar)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_HANDIN, OnHandIn)
	ON_BN_CLICKED(IDC_BUTTON_MARK, OnMark)
	ON_BN_CLICKED(IDC_CHECK_TOPMOST, OnTopMost)
	ON_CBN_SELCHANGE(IDC_COMBO_FONTSIZE, OnSelChangeFontSize)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_BN_CLICKED(IDC_CHECK_ONLYFALSE, OnOnlyFalse)
	ON_BN_CLICKED(IDC_CHECK_SHOWSINGLE, OnShowSingle)
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnButtonPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnButtonNext)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestBar message handlers

BOOL CTestBar::OnInitDialog(UINT wParam, LONG lParam) 
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{
		TRACE(_T("InitCDataStatus Failed!"));
	}
	
	SetType(TESTBARTYPE_TEST);
	m_cBtnPre.ShowWindow(SW_HIDE);
	m_cBtnNext.ShowWindow(SW_HIDE);

	SendMessage(WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
	SendMessageToDescendants(WM_SETFONT, (WPARAM)g_hNormalFont, MAKELPARAM(FALSE, 0), TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTestBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	LRESULT lResult;
	if (pWnd->SendChildNotifyLastMsg(&lResult))
	{
		return (HBRUSH)lResult;
	}
	
	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		return CDialogBar::OnCtlColor(pDC, pWnd, nCtlColor);
		
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		pDC->SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		pDC->SetBkColor(AfxGetQColor(QCOLOR_FACE));
		return AfxGetQBrush();
		
	default:
		ASSERT(FALSE);
	}
	
	return NULL;
}

void CTestBar::OnSize(UINT nType, int cx, int cy) 
{
	CDialogBar::OnSize(nType, cx, cy);
	
	if (!IsWindow(m_cProgress.GetSafeHwnd()))
	{
		return;
	}

	RECT rcProgress = {0}, rcHandin = {0};
	m_cProgress.GetWindowRect(&rcProgress);
	ScreenToClient(&rcProgress);
	rcProgress.right = cx - rcProgress.left;
	m_cProgress.MoveWindow(&rcProgress, TRUE);

	m_cBtnHandIn.GetWindowRect(&rcHandin);
	ScreenToClient(&rcHandin);
	rcHandin.left += rcProgress.right - rcHandin.right;
	rcHandin.right = rcProgress.right;
	m_cBtnHandIn.MoveWindow(&rcHandin, TRUE);
}

void CTestBar::SetTime(long lTime)
{
	m_lTime = lTime;

	m_cProgress.SetRange(0, m_lTime);
	if (m_lTime)
	{
		m_bNoTimeLimit = FALSE;

		m_cProgress.SetAlarm1Percent(0.25f);
		m_cProgress.SetAlarm2Percent(0.1f);
		
		m_cProgress.SetPos(m_lTime);
	}
	else
	{
		m_bNoTimeLimit = TRUE;
	}

	SetTimer(1, 1000, NULL);
}

void CTestBar::SetType(int nType)
{
	switch(nType)
	{
	case TESTBARTYPE_TEST:
		m_cBtnSave.ShowWindow(SW_HIDE);
		m_cCheckOnlyFalse.ShowWindow(SW_HIDE);
		m_cBtnMark.ShowWindow(SW_SHOW);
		m_cCheckTopMost.ShowWindow(SW_SHOW);
		m_cBtnHandIn.ShowWindow(SW_SHOW);
		break;

	case TESTBARTYPE_TESTVIEW:
		m_cBtnSave.ShowWindow(SW_SHOW);
		m_cCheckOnlyFalse.ShowWindow(SW_SHOW);
		m_cBtnMark.ShowWindow(SW_HIDE);
		m_cCheckTopMost.ShowWindow(SW_HIDE);
		m_cBtnHandIn.ShowWindow(SW_HIDE);
		break;
	}
}

void CTestBar::UpdateState(CItem *pItem)
{
	if (!pItem)
	{
		return;
	}

	CString string;
	if (pItem->GetItemState(ITEMMARK_UNCERTAIN))
	{
		string.LoadString(IDS_TESTBAR_UNMARK);
		GetDlgItem(IDC_BUTTON_MARK)->SetWindowText(string);
	}
	else
	{
		string.LoadString(IDS_TESTBAR_MARK);
		GetDlgItem(IDC_BUTTON_MARK)->SetWindowText(string);
	}

	if (pItem->GetItemFrom()->IsStored())
	{
		string.LoadString(IDS_QVIEW_BUTTONUNSTORE);
		GetDlgItem(IDC_BUTTON_SAVE)->SetWindowText(string);
	}
	else
	{
		string.LoadString(IDS_VIEWDLG_BUTTONSTORE);
		SetDlgItemText(IDC_BUTTON_SAVE, string);
	}
}

void CTestBar::UpdateList(int nWrongCount)
{
	m_cCheckOnlyFalse.EnableWindow(nWrongCount);
}

void CTestBar::SetFontSize(long lFont)
{
	int nSel = 0;
	switch(lFont)
	{
	case ISP_SMALLFONT:
		nSel = TBP_SMALLFONT;
		break;

	case ISP_NORMALFONT:
		nSel = TBP_NORMALFONT;
		break;

	case ISP_LARGEFONT:
		nSel = TBP_LARGEFONT;
		break;

	case ISP_GIANTFONT:
		nSel = TBP_GIANTFONT;
		break;

	default:
		nSel = -1;
	}
	m_cComboFont.SetCurSel(nSel);
}

void CTestBar::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case 1:
		if (!(--m_lTime))
		{
			m_cProgress.SetPos(m_lTime);
			
			KillTimer(1);
			
			CTMessageBox Box;
			Box.SuperMessageBox(m_hWnd, IDS_TESTBAR_TIMEOUT, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);

			GetParent()->PostMessage(WM_TESTBARNOTIFY, TBN_TIMEOUT, 0);
		}
		m_cProgress.SetPos(m_lTime);
		break;
	}
	
	CDialogBar::OnTimer(nIDEvent);
}

void FUNCCALL CTestBar::SetProgressText(CWnd *pDlg, CString &string)
{
	CTestBar *pThis = (CTestBar*)pDlg;
	
	if (pThis->m_bNoTimeLimit)
	{
		string.LoadString(IDS_TESTBAR_NOTIMELMT);
	}
	else
	{
		int nHour = pThis->m_lTime / 3600;
		int nMinute = (pThis->m_lTime % 3600) / 60;
		int nSecond = pThis->m_lTime % 60;
		
		if (nHour)
		{
			string.Format(IDS_TESTBAR_TIMELEFT_H, nHour, nMinute, nSecond);
		}
		else if (nMinute)
		{
			string.Format(IDS_TESTBAR_TIMELEFT_M, nMinute, nSecond);
		}
		else
		{
			string.Format(IDS_TESTBAR_TIMELEFT_S, nSecond);
		}
	}
}

void CTestBar::OnHandIn() 
{
	if (GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_USERHANDIN, 0))
	{
		KillTimer(1);
	}
}

void CTestBar::OnMark() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_MARKITEM, 0);
}

void CTestBar::OnTopMost() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_TOPMOST,
		((CButton*)GetDlgItem(IDC_CHECK_TOPMOST))->GetCheck());
}

void CTestBar::OnSelChangeFontSize() 
{
	int nSel = m_cComboFont.GetCurSel();
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_SETFONTSIZE, nSel);
}

void CTestBar::OnSave() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_SAVEITEM, 0);
}

void CTestBar::OnOnlyFalse() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_ONLYFALSE, m_cCheckOnlyFalse.GetCheck());
}

void CTestBar::OnShowSingle() 
{
	BOOL bSingle = m_cCheckSingle.GetCheck();

	m_cBtnPre.ShowWindow(bSingle);
	m_cBtnNext.ShowWindow(bSingle);

	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_SHOWSINGLE, bSingle);
}

void CTestBar::OnButtonPre() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_PRE, 0);
}

void CTestBar::OnButtonNext() 
{
	GetParent()->SendMessage(WM_TESTBARNOTIFY, TBN_NEXT, 0);
}
