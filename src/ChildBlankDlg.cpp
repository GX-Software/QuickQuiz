// ChildBlankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditDlg.h"

#include "Blank.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildBlankDlg dialog


CChildBlankDlg::CChildBlankDlg(CWnd* pParent /*=NULL*/) :
	CTabPage(CChildBlankDlg::IDD, pParent),
	m_pDescEdit(NULL), m_pSetBtn(NULL), m_pBlank(NULL)
{
	//{{AFX_DATA_INIT(CChildBlankDlg)
	//}}AFX_DATA_INIT
}

CChildBlankDlg::~CChildBlankDlg()
{
	if (m_pBlank)
	{
		delete m_pBlank;
	}
}

void CChildBlankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChildBlankDlg)
	DDX_Control(pDX, IDC_EDIT_ANSWER, m_cEditAnswer);
	DDX_Control(pDX, IDC_LIST_BLANKS, m_cList);
	DDX_Control(pDX, IDC_BUTTON_DELWITHASW, m_cBtnDelWithAsw);
	DDX_Control(pDX, IDC_BUTTON_DELNOASW, m_cBtnDelNoAsw);
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_cBtnEdit);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_cBtnAdd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChildBlankDlg, CTabPage)
	//{{AFX_MSG_MAP(CChildBlankDlg)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELWITHASW, OnButtonDelWithAsw)
	ON_BN_CLICKED(IDC_BUTTON_DELNOASW, OnButtonDelNoAsw)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_BLANKS, OnGetDispInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BLANKS, OnDblClkListBlanks)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildBlankDlg message handlers

BOOL CChildBlankDlg::OnInitDialog() 
{
	CTabPage::OnInitDialog();

	m_cEditAnswer.SetLimitText(0);

	m_cList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	if(CLRTYPE_DEFAULT != g_nColorType)
	{
		m_cList.SetBkColor(AfxGetQColor(QCOLOR_FACE));
		m_cList.SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		m_cList.SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
	}

	CString string;
	string.LoadString(IDS_BLKDLG_INDEX);
	m_cList.InsertColumn(0, string, LVCFMT_LEFT, 40);

	string.LoadString(IDS_BLKDLG_START);
	m_cList.InsertColumn(1, string, LVCFMT_LEFT, 40);

	string.LoadString(IDS_BLKDLG_END);
	m_cList.InsertColumn(2, string, LVCFMT_LEFT, 40);

	string.LoadString(IDS_BLKDLG_ASW);
	m_cList.InsertColumn(3, string, LVCFMT_LEFT, 150);

	m_pBlank = new CBlank;
	ASSERT(m_pBlank);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CChildBlankDlg::OnButtonAdd() 
{
	CString string;
	if (!m_pBlank->GetBlankCount())
	{
		m_pDescEdit->GetWindowText(string);
		m_pBlank->SetDescription(string);
	}

	int nStart, nEnd;
	m_pDescEdit->GetSel(nStart, nEnd);

	GetDlgItemText(IDC_EDIT_ANSWER, string);
	int nInsertPos = m_pBlank->AddBlank(nStart, nEnd, string);

	if (nInsertPos < 0)
	{
		ShowErrorInfo();
		return;
	}

	m_bModify = TRUE;
	m_pDescEdit->SetWindowText(m_pBlank->GetDescription());
	UpdatePage();
	m_pDescEdit->SetSel(0, 0, TRUE);
}

void CChildBlankDlg::OnButtonEdit() 
{
	CTMessageBox Box;
	int nSelIndex = m_cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSelIndex < 0)
	{
		Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_NOSEL, IDS_MSGCAPTION_INFOR, MB_YESNO | MB_ICONINFORMATION, 0, 0);
		return;
	}

	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_EDITASK, IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
	{
		return;
	}

	CString str;
	GetDlgItemText(IDC_EDIT_ANSWER, str);
	if (!m_pBlank->EditBlank(nSelIndex, str, -1))
	{
		ShowErrorInfo();
		return;
	}

	m_bModify = TRUE;
	UpdatePage();
}

void CChildBlankDlg::OnButtonDelWithAsw() 
{
	CTMessageBox Box;
	int nSelIndex = m_cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSelIndex < 0)
	{
		Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_NOSEL, IDS_MSGCAPTION_INFOR, MB_YESNO | MB_ICONINFORMATION, 0, 0);
		return;
	}

	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_DELASK, IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
	{
		return;
	}

	if (!m_pBlank->DelBlank(nSelIndex, TRUE))
	{
		ShowErrorInfo();
	}
	else
	{
		m_bModify = TRUE;
	}

	m_pDescEdit->SetWindowText(m_pBlank->GetDescription());
	UpdatePage();
}

void CChildBlankDlg::OnButtonDelNoAsw() 
{
	CTMessageBox Box;
	int nSelIndex = m_cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSelIndex < 0)
	{
		Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_NOSEL, IDS_MSGCAPTION_INFOR, MB_YESNO | MB_ICONINFORMATION, 0, 0);
		return;
	}

	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_DELASK, IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
	{
		return;
	}
	
	if (!m_pBlank->DelBlank(nSelIndex, FALSE))
	{
		ShowErrorInfo();
	}
	else
	{
		m_bModify = TRUE;
	}
	
	m_pDescEdit->SetWindowText(m_pBlank->GetDescription());
	UpdatePage();
}

void CChildBlankDlg::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem = &pDispInfo->item;

	if (pItem->mask & LVIF_TEXT)
	{
		int n = 0;
		switch(pItem->iSubItem)
		{
		case 0:
			n = pItem->iItem + 1;
			break;

		case 1:
			if (m_pBlank)
			{
				m_pBlank->GetBlankRange(pItem->iItem, &n, NULL);
			}
			break;

		case 2:
			if (m_pBlank)
			{
				m_pBlank->GetBlankRange(pItem->iItem, NULL, &n);
			}
			break;

		case 3:
			if (m_pBlank)
			{
				lstrcpyn(pItem->pszText, m_pBlank->GetBlank(pItem->iItem), pItem->cchTextMax);
				*pResult = 0;
				return;
			}
			break;
		}

		_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%d"), n);
	}
	
	*pResult = 0;
}

void CChildBlankDlg::OnDblClkListBlanks(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nSelIndex = m_cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nSelIndex >= 0)
	{
		int nStart, nEnd;
		m_pBlank->GetBlankRange(nSelIndex, &nStart, &nEnd);
		m_pDescEdit->SetSel(nStart, nEnd);

		CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_ANSWER);
		pEdit->SetWindowText(m_pBlank->GetBlank(nSelIndex));
		pEdit->SetFocus();
		pEdit->SetSel(0, -1);
	}
	
	*pResult = 0;
}

void CChildBlankDlg::UpdatePage()
{
	SetDlgItemText(IDC_EDIT_ANSWER, _T(""));

	m_cBtnAdd.EnableWindow(m_pBlank->GetBlankCount() < MAXBLANKS);
	if (!m_pBlank->GetBlankCount())
	{
		m_pDescEdit->SetReadOnly(FALSE);
		m_pSetBtn->EnableWindow(TRUE);

		m_cBtnEdit.EnableWindow(FALSE);
		m_cBtnDelNoAsw.EnableWindow(FALSE);
		m_cBtnDelWithAsw.EnableWindow(FALSE);
	}
	else
	{
		m_pDescEdit->SetReadOnly(TRUE);
		m_pSetBtn->EnableWindow(FALSE);

		m_cBtnEdit.EnableWindow(TRUE);
		m_cBtnDelNoAsw.EnableWindow(TRUE);
		m_cBtnDelWithAsw.EnableWindow(TRUE);
	}
	
	m_cList.SetItemCountEx(m_pBlank->GetBlankCount(), LVSICF_NOSCROLL);
}

BOOL CChildBlankDlg::OnKillActive(BOOL bIsDestroy)
{
	ASSERT(m_pBlank);

	int nCount = m_pBlank->GetBlankCount();
	// 如果没有做任何修改，则可以直接改变题目类型
	if (!nCount || !m_bModify)
	{
		m_pDescEdit->SetReadOnly(FALSE);
		m_pSetBtn->EnableWindow(TRUE);

		return TRUE;
	}

	if (!bIsDestroy)
	{
		CTMessageBox Box;
		if (IDNO == Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_SWITCHPAGE, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
			return FALSE;
		}

		// 保留答案内容的清除
		int i;
		for (i = nCount - 1; i >= 0; --i)
		{
			m_pBlank->DelBlank(i, TRUE);
		}
		
		m_pDescEdit->SetWindowText(m_pBlank->GetDescription());
		UpdatePage();

		m_bModify = FALSE;
	}
	
	return TRUE;
}

void CChildBlankDlg::ShowErrorInfo()
{
	CTMessageBox Box;
	UINT strCode = 0;
	switch(m_pBlank->GetLastError())
	{
	case ERROR_OK:
		return;

	case ERROR_NODESCRIPTION:
		strCode = IDS_BLKDLG_ERRNODESC;
		break;
		
	case ERROR_RANGECONFLICT:
		strCode = IDS_BLKDLG_ERRRANGECONF;
		break;
		
	case ERROR_ALLOCFAIL:
		strCode = IDS_BLKDLG_ERRALLOCFAIL;
		break;
		
	case ERROR_NOANSWER:
		strCode = IDS_BLKDLG_ERRNOASW;
		break;
		
	case ERROR_INDEXOVERFLOW:
		strCode = IDS_BLKDLG_ERREDITOVER;
		break;

	case ERROR_TRANSCODEFAIL:
		strCode = IDS_BLKDLG_ERRTRANSCODEFAIL;
		break;
		
	default:
		ASSERT(FALSE);
	}
	
	Box.SuperMessageBox(m_hWnd, strCode, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);
}

void CChildBlankDlg::SetShow(const CItem *pItem)
{
	ASSERT(m_pBlank);

	if (!pItem)
	{
		m_pBlank->ClearAllInfo();
	}
	else
	{
		CBlank* ptr = (CBlank*)pItem;
		m_pBlank->GetInfo(ptr, TRUE);

		m_pDescEdit->SetWindowText(m_pBlank->GetDescription());
	}

	m_bModify = FALSE;
	UpdatePage();
}

int CChildBlankDlg::CheckSaveable()
{
	if (!m_pBlank->GetBlankCount())
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, IDS_BLKDLG_NOBLOCKS, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0))
		{
		case IDYES:
			// 禁止不设空白
			return SAVEABLE_CONTINUE;

		case IDNO:
			return SAVEABLE_DENY;

		default:
			ASSERT(FALSE);
			break;
		}
	}

	return SAVEABLE_OK;
}

void CChildBlankDlg::GetInfo(CBlank *pBlank)
{
	if (!pBlank)
	{
		return;
	}

	pBlank->ClearAllInfo();
	pBlank->GetInfo(m_pBlank, TRUE);

	m_bModify = FALSE;
}
