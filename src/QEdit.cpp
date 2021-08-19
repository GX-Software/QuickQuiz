// QEdit.cpp : implementation file
//

#include "stdafx.h"
#include "QEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQEdit

CQEdit::CQEdit() :
	m_clrTxt(RGB(0, 0, 0)),
	m_hbrReadOnly(NULL),
	m_bFocus(FALSE),
	m_bEnableCopy(TRUE),
	m_bCheckSelChange(FALSE),
	m_nStart(-1), m_nEnd(-1)
{
}

CQEdit::~CQEdit()
{
	if (m_hbrReadOnly)
	{
		DeleteObject(m_hbrReadOnly);
	}
}


BEGIN_MESSAGE_MAP(CQEdit, CEdit)
	//{{AFX_MSG_MAP(CQEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCPAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_DROPFILES()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_COPY, OnCopy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQEdit message handlers

HBRUSH CQEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (CLRTYPE_DARK == g_nColorType)
	{
		pDC->SetTextColor(AfxGetQColor(QCOLOR_TEXT));
	}
	else
	{
		pDC->SetTextColor(m_clrTxt);
	}
	
	if (GetStyle() & ES_READONLY || GetStyle() & WS_DISABLED)
	{
		switch(g_nColorType)
		{
		case CLRTYPE_DEFAULT:
			pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
			return m_hbrReadOnly;

		case CLRTYPE_GREEN:
		case CLRTYPE_DARK:
			pDC->SetBkColor(AfxGetQColor(QCOLOR_FACE));
			return AfxGetQBrush();

		default:
			ASSERT(FALSE);
		}
	}
	else
	{
		switch(g_nColorType)
		{
		case CLRTYPE_DEFAULT:
			pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
			return (HBRUSH)GetStockObject(WHITE_BRUSH);

		case CLRTYPE_GREEN:
			pDC->SetBkColor(AfxGetQColor(QCOLOR_EDITFACE));
			return AfxGetQBrush();

		case CLRTYPE_DARK:
			pDC->SetBkColor(AfxGetQColor(QCOLOR_EDITFACE));
			return (HBRUSH)GetStockObject(BLACK_BRUSH);

		default:
			ASSERT(FALSE);
		}
	}
	
	return NULL;
}

void CQEdit::SetTextColor(COLORREF clrTxt)
{
	m_clrTxt = clrTxt;
	Invalidate(TRUE);
}

long CQEdit::GetText(int nStart, int nEnd, LPTSTR lpBuf)
{
	if (nEnd < nStart)
	{
		return 0;
	}

	int nLen = 0;
	LPTSTR pText = NULL;
	
	nLen = GetWindowTextLength();
	pText = (LPTSTR)malloc((nLen + 1) * sizeof(TCHAR));
	if (!pText)
	{
		return 0;
	}
	GetWindowText(pText, nLen + 1);
	
	nStart = CTextManager::GetRealIndex(pText, nStart);
	nEnd = CTextManager::GetRealIndex(pText, nEnd);
	nLen = nEnd - nStart;
	
	if (lpBuf)
	{
		_tcsncpy(lpBuf, pText + nStart, nLen);
		lpBuf[nLen] = 0;
	}
	
	free(pText);
	return nLen;
}

long CQEdit::GetSelText(LPTSTR lpBuf)
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	
	return GetText(nStart, nEnd, lpBuf);
}

LRESULT CQEdit::OnCopy(WPARAM wParam, LPARAM lParam)
{
	if (m_bEnableCopy)
	{
		return Default();
	}
	else
	{
		return 0;
	}
}

void CQEdit::PreSubclassWindow() 
{
	if (m_hbrReadOnly)
	{
		DeleteObject(m_hbrReadOnly);
		m_hbrReadOnly = NULL;
	}
	m_hbrReadOnly = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	
	CEdit::PreSubclassWindow();
}

void CQEdit::OnNcPaint() 
{
	CEdit::OnNcPaint();
	if (CLRTYPE_DEFAULT != g_nColorType)
    {
		DrawBorder();
	}
}

void CQEdit::DrawBorder()
{
	if (!(GetStyle() & WS_BORDER) && !(GetExStyle() & WS_EX_CLIENTEDGE))
	{
		return;
	}

    HDC hDC = ::GetWindowDC(m_hWnd);
    CRect rc;
    ::GetWindowRect(m_hWnd, rc);
	rc.OffsetRect(-rc.left, -rc.top);

	HBRUSH hbrBorder = CreateSolidBrush(AfxGetQColor(QCOLOR_BORDER));
	HBRUSH hbrClientEdge = CreateSolidBrush(GetClientEdgeClr());

    ::FrameRect(hDC,&rc, hbrBorder);
    rc.DeflateRect(1, 1);
    ::FrameRect(hDC,&rc,hbrClientEdge);

	DeleteObject(hbrBorder);
	DeleteObject(hbrClientEdge);

    ::ReleaseDC(m_hWnd, hDC);
}

COLORREF CQEdit::GetClientEdgeClr()
{
	switch(g_nColorType)
	{
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		if (m_bFocus)
		{
			return AfxGetQColor(QCOLOR_HIGHLIGHT);
		}
		else
		{
			return AfxGetQColor(QCOLOR_FACE);
		}
		
	default:
		ASSERT(FALSE);
	}
	return 0;
}

void CQEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);
	
	m_bFocus = TRUE;
	if (CLRTYPE_DEFAULT != g_nColorType)
    {
		DrawBorder();
	}
}

void CQEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	m_bFocus = FALSE;
	if (CLRTYPE_DEFAULT != g_nColorType)
    {
		DrawBorder();
	}
}

// 防止在有色彩的情况下窗口再绘制导致闪烁
LRESULT CQEdit::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		return Default();
	}
	return 0;
}

LRESULT CQEdit::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		Default();
	}
	return 0;
}

void CQEdit::OnDropFiles(HDROP hDropInfo) 
{
	GetParent()->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo, 0);
}

void CQEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonDown(nFlags, point);

	// 监视选区的变化
	if (m_bCheckSelChange)
	{
		NotifySelChange();
	}
}

void CQEdit::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonUp(nFlags, point);

	// 监视选区的变化
	if (m_bCheckSelChange)
	{
		NotifySelChange();
	}
}

void CQEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_ESCAPE:
		GetParent()->SendMessage(WM_QEDITNOTIFY, QEN_ESCAPE, 0);
		break;
		
	case VK_TAB:
		if (GetKeyState(VK_CONTROL) < 0)
		{
			if (GetKeyState(VK_SHIFT) < 0)
			{
				GetParent()->SendMessage(WM_QEDITNOTIFY, QEN_PREV, 0);
			}
			else
			{
				GetParent()->SendMessage(WM_QEDITNOTIFY, QEN_NEXT, 0);
			}
			return;
		}
		break;
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	// 监视选区的变化
	if (m_bCheckSelChange)
	{
		NotifySelChange();
	}
}

LRESULT CQEdit::NotifySelChange()
{
	int nStart, nEnd;
	GetSel(nStart, nEnd);
	if (nStart != m_nStart || nEnd != m_nEnd)
	{
		m_nStart = nStart;
		m_nEnd = nEnd;

		SELCHANGE SelInfo;
		SelInfo.nmhdr.hwndFrom = m_hWnd;
		SelInfo.nmhdr.idFrom = GetDlgCtrlID();
		SelInfo.nmhdr.code = EN_SELCHANGE;

		SelInfo.chrg.cpMin = nStart;
		SelInfo.chrg.cpMax = nEnd;
		SelInfo.seltyp = SEL_TEXT;

		return GetParent()->SendMessage(WM_NOTIFY, SelInfo.nmhdr.idFrom, (LPARAM)&SelInfo);
	}

	return 0;
}

void CQEdit::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (m_bEnableCopy)
	{
		CEdit::OnContextMenu(pWnd, point);
	}
}

