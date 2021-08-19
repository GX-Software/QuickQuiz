// QTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "QTabCtrl.h"

#include "TabPage.h"

#include "MemBmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQTabCtrl

CQTabCtrl::CQTabCtrl() :
	m_nPageCount(0), m_nShowingPage(-1),
	m_bCanChangeType(TRUE)
{
	ZeroMemory(m_pPageList, sizeof(m_pPageList));
}

CQTabCtrl::~CQTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CQTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CQTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQTabCtrl message handlers

void CQTabCtrl::PreSubclassWindow() 
{
	switch(g_nColorType)
	{
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		ModifyStyle(0, TCS_OWNERDRAWFIXED);
		break;
	}
	
	CTabCtrl::PreSubclassWindow();
}

BOOL CQTabCtrl::AddPage(UINT nIDTemplate, CTabPage *pPage, LPCTSTR szTitle)
{
	if (m_nPageCount >= MAXPAGENUM)
	{
		return FALSE;
	}

	ASSERT(!m_pPageList[m_nPageCount]);
	
	if (!IsWindow(pPage->GetSafeHwnd()))
	{
		if (!pPage->Create(nIDTemplate, this))
		{
			return FALSE;
		}
	}
	m_pPageList[m_nPageCount] = pPage;
	SetChildPos(pPage);

	InsertItem(m_nPageCount, szTitle);
	m_nPageCount++;

	if (m_nShowingPage < 0)
	{
		m_nShowingPage = 0;
		m_pPageList[m_nShowingPage]->OnSetActive();
	}
	m_pPageList[m_nShowingPage]->ShowWindow(SW_SHOW);
	return TRUE;
}

CTabPage* CQTabCtrl::GetPage(int nIndex)
{
	if (nIndex >= MAXPAGENUM || nIndex < 0)
	{
		return NULL;
	}

	return m_pPageList[nIndex];
}

void CQTabCtrl::SetChildPos(CTabPage *pPage)
{
	CRect rcClient, rcItem;
	UINT uFlag = (m_pPageList[m_nShowingPage] == pPage) ? SWP_SHOWWINDOW : SWP_HIDEWINDOW;

	GetClientRect(rcClient);
	GetItemRect(0, rcItem);

	pPage->SetWindowPos(&wndTop,
						rcClient.left + 1, rcItem.Height() + 3,
						rcClient.Width() - 4, rcClient.Height() - rcItem.Height() - 5,
						uFlag);
}

void CQTabCtrl::SwitchPage(int nIndex)
{
	if (nIndex >= m_nPageCount)
	{
		nIndex = 0;
	}
	if (nIndex == m_nShowingPage)
	{
		return;
	}

	SetCurSel(nIndex);
	SwitchShow(nIndex);
}

BOOL CQTabCtrl::SwitchShow(int nIndex)
{
	if (!m_pPageList[m_nShowingPage]->OnKillActive(FALSE))
	{
		return FALSE;
	}

	// 这样设计可以防止闪烁
	m_pPageList[nIndex]->ShowWindow(SW_SHOW);
	m_pPageList[m_nShowingPage]->ShowWindow(SW_HIDE);

	m_nShowingPage = nIndex;
	m_pPageList[m_nShowingPage]->OnSetActive();

	return TRUE;
}

void CQTabCtrl::OnSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCur = GetCurSel();

	if (nCur == m_nShowingPage)
	{
		return;
	}

	// 在某些情况下禁止翻页
	if (!m_bCanChangeType)
	{
		SetCurSel(m_nShowingPage);
		return;
	}
	else if (!SwitchShow(nCur))
	{
		SetCurSel(m_nShowingPage);
		return;
	}

	GetParent()->SendMessage(WM_NOTIFY, MAKELONG(TCN_SELCHANGE, 0), (LPARAM)pNMHDR);
	
	*pResult = 0;
}

void CQTabCtrl::OnPaint() 
{
	if (CLRTYPE_DEFAULT == g_nColorType)
	{
		CTabCtrl::OnPaint();
	}
	else
	{
		CPaintDC paintDC(this);
		
		Paint(paintDC.m_hDC);
	}
}

BOOL CQTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	switch(g_nColorType)
	{
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		return TRUE;

	default:
		return CTabCtrl::OnEraseBkgnd(pDC);
	}
}

void CQTabCtrl::Paint(HDC hDC)
{
	ASSERT(CLRTYPE_DEFAULT != g_nColorType);

	CRect rcClient;
	GetClientRect(rcClient);
	CRect rcWork(rcClient);

	CMemBmp Mem;
	HDC dc = Mem.CreateBmp(hDC, rcClient, TRUE, AfxGetQColor(QCOLOR_FACE));
	ASSERT(dc);
	
	COLORREF clrOld = SetTextColor(dc, AfxGetQColor(QCOLOR_TEXT));
	int nOldMode = SetBkMode(dc, TRANSPARENT);
	HPEN hOldPen = (HPEN)SelectObject(dc, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_BORDER)));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(dc, AfxGetQBrush());
	HBRUSH hbrShadow = CreateSolidBrush(AfxGetQColor(QCOLOR_SHADOW));
	ASSERT(hbrShadow);
	
	int nCount = GetItemCount();
	int nIndex = GetCurSel();
	int i = 0;
	
	TCHAR szLabel[64] = {0};
	TC_ITEM iItemTC;
	iItemTC.mask = TCIF_TEXT;
	iItemTC.pszText = szLabel;
	iItemTC.cchTextMax = sizeof(szLabel);
	
	HFONT hFont = (HFONT)SendMessage(WM_GETFONT, 0, 0);
	if (!hFont)
	{
		hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	}
	HFONT hOldFont = (HFONT)SelectObject(dc, hFont);

	CRect rcItem, rcShadow;
	while(i < nCount)
	{
		if (i == nIndex)
		{
			i++;
			continue;
		}
		
		GetItemRect(i, rcItem);
		rcShadow = rcItem;
		rcShadow.DeflateRect(1, 1, 1, 0);
		rcItem.bottom++;
		
		Rectangle(dc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);

		// 绘制阴影
		switch(g_nColorType)
		{
		case CLRTYPE_GREEN:
			rcShadow.top += rcShadow.Height() / 2;
			break;

		case CLRTYPE_DARK:
			rcShadow.bottom -= rcShadow.Height() / 2;
			break;
		}
		FillRect(dc, rcShadow, hbrShadow);
		
		GetItem(i, &iItemTC);
		TextOut(dc, rcItem.left + 6, rcItem.top + 4, iItemTC.pszText, lstrlen(iItemTC.pszText));
		
		i++;
	}
	DeleteObject(hbrShadow);
	
	// 最后绘制选中的那个
	GetItemRect(nIndex, rcItem);
	rcItem.bottom++;
	rcItem.top = 0;
	rcItem.left -= 2;
	rcItem.right += 2;
	
	Rectangle(dc, rcItem.left, rcItem.top, rcItem.right, rcItem.bottom);
	
	GetItem(nIndex, &iItemTC);
	TextOut(dc, rcItem.left + 6, rcItem.top + 4, iItemTC.pszText, lstrlen(iItemTC.pszText));

	// 绘制下面的显示区
	rcWork.top += rcItem.Height() - 1;
	Rectangle(dc, rcWork.left, rcWork.top, rcWork.right, rcWork.bottom);

	// 重新处理选中的标签
	SelectObject(dc, CreatePen(PS_SOLID, 1, AfxGetQColor(QCOLOR_FACE)));

	MoveToEx(dc, rcItem.left + 1, rcItem.bottom - 1, NULL);
	LineTo(dc, rcItem.right - 1, rcItem.bottom - 1);
	
	Mem.Draw(hDC, rcClient.left, rcClient.top);
	
	SelectObject(dc, hOldBrush);
	DeleteObject(SelectObject(dc, hOldPen));
	SetTextColor(dc, clrOld);
	SetBkMode(dc, nOldMode);
	SelectObject(dc, hOldFont);
}

void CQTabCtrl::OnDestroy() 
{
	int i;
	for (i = 0; i < m_nPageCount; i++)
	{
		m_pPageList[i]->OnKillActive(TRUE);
	}

	CTabCtrl::OnDestroy();
}
