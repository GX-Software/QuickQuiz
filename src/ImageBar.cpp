// ImageBar.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ImageBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageBar dialog


CImageBar::CImageBar(CWnd* pParent /* = NULL */) :
	m_nWidth(0), m_nHeight(0), m_bTitleModify(FALSE)
{
	//{{AFX_DATA_INIT(CImageBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CImageBar::~CImageBar()
{
}

void CImageBar::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CImageBar)
	DDX_Control(pDX, IDC_EDIT_TITLE, m_cEditTitle);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_cBtnDelete);
	DDX_Control(pDX, IDC_BUTTON_SET, m_cBtnSet);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_cBtnNew);
	DDX_Control(pDX, IDC_BUTTON_PRE, m_cBtnPre);
	DDX_Control(pDX, IDC_BUTTON_NEXT, m_cBtnNext);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CImageBar, CDialogBar)
	//{{AFX_MSG_MAP(CImageBar)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_PRE, OnPre)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnNext)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnNew)
	ON_BN_CLICKED(IDC_BUTTON_SET, OnSet)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnDelete)
	ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageBar message handlers

BOOL CImageBar::OnInitDialog(UINT wParam, LONG lParam) 
{
	BOOL bRet = HandleInitDialog(wParam, lParam);
	if (!UpdateData(FALSE))
	{
		TRACE(_T("InitCDataStatus Failed!"));
	}

	RECT rcDlg;
	GetWindowRect(&rcDlg);
	m_nWidth = rcDlg.right - rcDlg.left;
	m_nHeight = rcDlg.bottom - rcDlg.top;

	m_cEditTitle.SetLimitText(32);
	
	return TRUE; // return TRUE unless you set the focus to a control
				 // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CImageBar::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
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

BOOL CImageBar::CreateBar(CWnd* pParentWnd, UINT uID)
{
	return Create(pParentWnd, uID, CBRS_BOTTOM, uID);
}

void CImageBar::SetStatus(size_t ulCount, size_t ulCur, size_t ulMax, LPCTSTR lpTitle)
{
	m_cBtnPre.EnableWindow(ulCur > 0);
	m_cBtnNext.EnableWindow(ulCount && ulCur < ulCount - 1);
	m_cBtnSet.EnableWindow(ulCount > 0);
	m_cBtnNew.EnableWindow(ulCount < ulMax);
	m_cBtnDelete.EnableWindow(ulCount > 0);

	m_cEditTitle.SetWindowText(lpTitle);
	m_cEditTitle.SetFocus();
	m_cEditTitle.SetModify(FALSE);
}

void CImageBar::GetImageTitle(CString &string)
{
	m_cEditTitle.GetWindowText(string);
}

BOOL CImageBar::IsModified()
{
	return m_cEditTitle.GetModify();
}

void CImageBar::OnPre() 
{
	CWnd *pParent = GetParent();
	if (!pParent)
	{
		return;
	}

	CString str;
	m_cEditTitle.GetWindowText(str);
	LPCTSTR pStr;
	if (m_cEditTitle.GetModify())
	{
		pStr = str;
	}
	else
	{
		pStr = NULL;
	}
	pParent->SendMessage(WM_IMAGEBARNOTIFY, IMAGEBARNOTIFY_PRE, (LPARAM)pStr);
	m_cEditTitle.SetModify(FALSE);
}

void CImageBar::OnNext() 
{
	CWnd *pParent = GetParent();
	if (!pParent)
	{
		return;
	}
	
	CString str;
	m_cEditTitle.GetWindowText(str);
	LPCTSTR pStr;
	if (m_cEditTitle.GetModify())
	{
		pStr = str;
	}
	else
	{
		pStr = NULL;
	}
	pParent->SendMessage(WM_IMAGEBARNOTIFY, IMAGEBARNOTIFY_NEXT, (LPARAM)pStr);
	m_cEditTitle.SetModify(FALSE);
}

void CImageBar::OnNew() 
{
	CWnd *pParent = GetParent();
	if (!pParent)
	{
		return;
	}
	
	CString str;
	m_cEditTitle.GetWindowText(str);
	LPCTSTR pStr;
	if (m_cEditTitle.GetModify())
	{
		pStr = str;
	}
	else
	{
		pStr = NULL;
	}
	pParent->SendMessage(WM_IMAGEBARNOTIFY, IMAGEBARNOTIFY_NEW, (LPARAM)pStr);
	m_cEditTitle.SetModify(FALSE);
}

void CImageBar::OnSet() 
{
	CWnd *pParent = GetParent();
	if (!pParent)
	{
		return;
	}
	
	CString str;
	m_cEditTitle.GetWindowText(str);
	LPCTSTR pStr;
	if (m_cEditTitle.GetModify())
	{
		pStr = str;
	}
	else
	{
		pStr = NULL;
	}
	pParent->SendMessage(WM_IMAGEBARNOTIFY, IMAGEBARNOTIFY_SET, (LPARAM)pStr);
	m_cEditTitle.SetModify(FALSE);
}

void CImageBar::OnDelete() 
{
	CWnd *pParent = GetParent();
	if (!pParent)
	{
		return;
	}
	
	pParent->SendMessage(WM_IMAGEBARNOTIFY, IMAGEBARNOTIFY_DELETE, NULL);
}

