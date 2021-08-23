// ItemShowWnd.cpp : implementation file
//

/*
## 基本信息

	题目显示窗口

	完成时间：2021/06/13

	修订次数：初版

	作者：南郊居士

## 功能设计

	1、按照各题目的格式显示题目，可用于题目展示和考试，在题目展示中允许选中和复制文字

	2、可以改变显示字体的大小

	3、允许由用户拖拽窗口大小，且在拖拽完成后才进行重新布局，提高显示效率

## 开发说明

	本窗口包含三层结构：

	第一层：窗口本身，管理所有显示资源，并完成与其他窗口的通信。

	第二层：ShowManager（管理器），每一道题目都对应一个管理器（对于题目组，题目组描述对应一个
管理器，每个子题目对应一个管理器）。

	第三层：每个管理器有多个块（Block），每个块内包含一段连续的文字或一张图片，块内有指向文字
或图片的指针，同时保存了显示信息（字体，在哪里显示，换行信息等）。

	每个文字块内都保存了换行信息，（对于单行文字，则有一条换行信息，不允许没有换行信息，不然
会被识别为图片）。

	本窗口提供了HitTest功能，可以返回鼠标点击位置对应的管理器序号，块序号，块类型，点击的文字
序号等信息。【需要特别注意的是】，当鼠标位于题目后的某个位置时，将假定点击在题目最后一行的某
个位置，所有信息按这个假定返回，但点击信息会返回“SHOWHITTEST_NONE”，可以用此信息判断返回的
点击信息是否有效。

	窗口HitTest是对所有题目的管理器依次进行测试，其返回值是鼠标是指向当前题目的前还是后，如果
发现鼠标位于某个题目后，则继续进行测试；当鼠标指向某个题目前，则意味着不需要再继续向下做测试
了。

## 修订记录

	空
*/

#include "stdafx.h"
#include "ItemShowWnd.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"
#include "ShowManager.h"

#include <Imm.h>
#pragma comment(lib, "Imm32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MINWIDTH					250
static LPCTSTR classname			= _T("QQItemShowWnd");

static LPCTSTR jpeg_lpszImageExt	= _T("jpeg");
static LPCTSTR bmp_lpszImageExt		= _T("bmp");
static LPCTSTR png_lpszImageExt		= _T("png");

#define ISW_TIMER_AUTOSCROLL		1
#define ISW_AUTOSCROLLDEFAULT		0
#define ISW_AUTOSCROLLUP			1
#define ISW_AUTOSCROLLDOWN			(-1)
#define ISW_AUTOSCROLLTIME			20
#define ISW_AUTOSCROLLSTEP			20

BEGIN_MESSAGE_MAP(CItemShowWnd, CWnd)
	//{{AFX_MSG_MAP(CItemShowWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SAVEIMG, OnUpdateCmdUI)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_SHOW_SAVEIMG, OnSaveImg)
	ON_WM_NCMOUSEMOVE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_QEDITNOTIFY, OnQEditNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemShowWnd

CItemShowWnd::CItemShowWnd() :
	m_nShowType(SHOWWNDTYPE_PROPERTY), m_hAccelTable(NULL),
	m_lTotalHeight(0),
	m_lYScroll(0), m_lXScroll(0), m_bXScroll(FALSE), m_bYScroll(FALSE),
	m_bSizing(FALSE), m_nAutoScroll(ISW_AUTOSCROLLDEFAULT),
	m_pList(NULL), m_pInfo(NULL), m_nInfoCount(0),
	m_lFontHeight(0),
	m_nShowIndex(-1), m_nShowStep(0), m_dwShowStyle(0),
	m_nSel(-1), m_uHitTest(0),
	m_bEditing(FALSE), m_bClick(FALSE)
{
	ZeroMemory(m_hFont, sizeof(m_hFont));

	ZeroMemory(&m_HitInfo, sizeof(m_HitInfo));
	ZeroMemory(&m_EditInfo, sizeof(m_EditInfo));

	ZeroMemory(&m_SelInfo, sizeof(m_SelInfo));
}

CItemShowWnd::~CItemShowWnd()
{
	int i;
	for (i = 0; i < 3; ++i)
	{
		DeleteObject(m_hFont[i]);
	}
	
	if (m_pInfo)
	{
		for (i = 0; i < m_pInfo->GetDataCount(); ++i)
		{
			ClearShowManager(m_pInfo->GetData(i));
		}
		delete m_pInfo;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CItemShowWnd message handlers

BOOL CItemShowWnd::RegisterClass(HINSTANCE hInstance /* = NULL */)
{
	// 创建一个没有图标的框架类
	WNDCLASS wndcls;
	ZeroMemory(&wndcls, sizeof(WNDCLASS));

	if (!hInstance)
	{
		hInstance = AfxGetInstanceHandle();
	}
	
	wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = NULL;
	wndcls.hCursor = afxData.hcurArrow;
	wndcls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = classname;
	
	if (!AfxRegisterClass(&wndcls))
	{
		AfxThrowResourceException();
		return FALSE;
	}

	m_hAccelTable = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	return TRUE;
}

LPCTSTR CItemShowWnd::GetClassName()
{
	return classname;
}

BOOL CItemShowWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	RegisterClass(AfxGetInstanceHandle());
	
	cs.style |= (WS_VSCROLL | WS_HSCROLL);
	cs.lpszClass = classname;
	return TRUE;
}

void CItemShowWnd::PreSubclassWindow() 
{
	GetClientRect(m_rcClient);
	SetFontSize(ISP_NORMALFONT, FALSE);
	
	CWnd::PreSubclassWindow();
}

BOOL CItemShowWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (!::TranslateAccelerator(GetSafeHwnd(), m_hAccelTable, pMsg))
	{
		return CWnd::PreTranslateMessage(pMsg);
	}
	return TRUE;
}

/*
功能：	显示当前列表中的内容
返回值：无
备注：	当nIndex小于0时，显示所有的内容
		nStep用于控制学习模式下的逐步显示，在nIndex小于0时无效
*/
void CItemShowWnd::ShowItem(int nIndex, DWORD dwShowStyle, int nStep /* = -1 */)
{
	if (!m_pList)
	{
		return;
	}

	m_nShowIndex = nIndex;
	m_nShowStep = nStep;
	m_dwShowStyle = dwShowStyle;
	RefreshShowInfo();

	InitSelection();
	InitScroll();
}

void CItemShowWnd::SetItemSelect(int nSel, BOOL bRedraw)
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(nSel);
	if (pMgr->GetItem()->GetType() == TYPE_GROUP)
	{
		return;
	}

	m_nSel = nSel;
	if (bRedraw)
	{
		Invalidate(FALSE);
	}
	GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_SELECTITEM, (LPARAM)GetSelection());
}

void CItemShowWnd::SetItemSelect(CItem *pItem, BOOL bRedraw)
{
	int i = 0;
	CShowManager *pMgr;

	ASSERT(pItem->GetType() != TYPE_GROUP);
	while(1)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (!pMgr)
		{
			break;
		}
		
		if (pMgr->GetItem() == pItem)
		{
			break;
		}
		++i;
	}
	if (!pMgr)
	{
		return;
	}
	
	SetItemSelect(i, bRedraw);
}

void CItemShowWnd::InitSelection()
{
	int i = 0;
	CShowManager *pMgr;
	
	while(1)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (!pMgr)
		{
			break;
		}
		
		if (pMgr->GetItem()->GetType() != TYPE_GROUP)
		{
			break;
		}
		++i;
	}
	if (!pMgr)
	{
		return;
	}
	
	SetItemSelect(i, TRUE);
}

CItem* CItemShowWnd::GetSelection()
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(m_nSel);
	if (!pMgr)
	{
		return NULL;
	}
	return pMgr->GetItem();
}

void CItemShowWnd::CheckImmInput(MSG *pMsg)
{
	CItem *pItem = GetSelection();
	if (!pItem)
	{
		return;
	}

	switch(pItem->GetType())
	{
	case TYPE_SCHOISE:
	case TYPE_MCHOISE:
	case TYPE_JUDGE:
		pMsg->wParam = ImmGetVirtualKey(pMsg->hwnd);
		break;

	default:
		break;
	}
}

void CItemShowWnd::SetFontSize(long lFontSize, BOOL bRebuild)
{
	CloseEdit();

	if (lFontSize == m_lFontHeight)
	{
		return;
	}
	m_lFontHeight = lFontSize;

	int i;
	for (i = 0; i < 3; ++i)
	{
		DeleteObject(m_hFont[i]);
	}

	LOGFONT lf = {0};
	if (!GetObject(g_hBigFont, sizeof(LOGFONT), &lf))
	{
		return;
	}
	lf.lfHeight = -lFontSize;
	m_hFont[0] = CreateFontIndirect(&lf);
	
	if (!GetObject(g_hNormalFont, sizeof(LOGFONT), &lf))
	{
		return;
	}
	lf.lfHeight = -lFontSize;
	m_hFont[1] = CreateFontIndirect(&lf);
	
	if (!GetObject(g_hBigBoldFont, sizeof(LOGFONT), &lf))
	{
		return;
	}
	lf.lfHeight = -lFontSize;
	m_hFont[2] = CreateFontIndirect(&lf);

	if (IsWindow(m_cBlankEdit.GetSafeHwnd()))
	{
		m_cBlankEdit.SendMessage(WM_SETFONT, (WPARAM)m_hFont[0], TRUE);
	}
	if (IsWindow(m_cTextEdit.GetSafeHwnd()))
	{
		m_cTextEdit.SendMessage(WM_SETFONT, (WPARAM)m_hFont[0], TRUE);
	}

	if (bRebuild)
	{
		RefreshShowInfo();
	}
}

void CItemShowWnd::RefreshShowInfo()
{
	CWaitCursor wc;

	if (!m_pList)
	{
		return;
	}
	if (!m_pInfo)
	{
		m_pInfo = new CDataList;
		if (!m_pInfo)
		{
			return;
		}
	}

	CloseEdit();

	HDC hDC = ::GetDC(GetSafeHwnd());
	SetShowInfo(hDC);
	::ReleaseDC(GetSafeHwnd(), hDC);
}

void CItemShowWnd::UpdateShowInfo()
{
	HDC hDC = ::GetDC(GetSafeHwnd());
	UpdateShowInfo(hDC, m_nSel);
	::ReleaseDC(GetSafeHwnd(), hDC);
}

int CItemShowWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 以下内容只在考试界面中生成
	if (!m_cBlankEdit.Create(WS_CHILD | ES_AUTOHSCROLL | ES_LEFT | ES_NOHIDESEL,
		CRect(0, 0, 0, 0), this, IDC_EDIT_ANSWER1))
	{
		return -1;
	}
	m_cBlankEdit.SendMessage(WM_SETFONT, (WPARAM)m_hFont[0], TRUE);
	
	if (!m_cTextEdit.Create(WS_CHILD | WS_BORDER | WS_VSCROLL | ES_LEFT | ES_NOHIDESEL | ES_MULTILINE | ES_WANTRETURN,
		CRect(0, 0, 0, 0), this, IDC_EDIT_ANSWER2))
	{
		return -1;
	}
	m_cTextEdit.SendMessage(WM_SETFONT, (WPARAM)m_hFont[0], TRUE);
	
	return 0;
}

void CItemShowWnd::ScrollToItem(int nIndex, int nSub)
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(nIndex);
	ASSERT(pMgr);

	CItem *pItem = pMgr->GetItem();
	int nStart, nEnd;

	GetClientRect(m_rcClient);
	nStart = pMgr->GetYOffset();
	nEnd = nStart + pMgr->GetShowHeight();

	// 不需要移动
	if (m_lYScroll < nStart && m_lYScroll + m_rcClient.Height() > nEnd)
	{
		return;
	}
	else if (m_lYScroll > nStart)
	{
		if(pItem->GetType() == TYPE_BLANK)
		{
			RECT rc = {0};
			if (pMgr->GetEditRect(nSub, &rc))
			{
				m_lYScroll = rc.top - 10;
			}
			else
			{
				m_lYScroll = nStart - 10;
			}
		}
		else
		{
			m_lYScroll = nStart - 10;
		}
	}
	else if (m_lYScroll < nEnd)
	{
		if(pItem->GetType() == TYPE_BLANK)
		{
			RECT rc = {0};
			if (pMgr->GetEditRect(nSub, &rc))
			{
				m_lYScroll = rc.bottom - m_rcClient.Height() + 30;
			}
			else
			{
				m_lYScroll = nEnd - m_rcClient.Height() + 10;
				if (nStart < m_lYScroll)
				{
					m_lYScroll = nStart - 10;
				}
			}
		}
		else
		{
			m_lYScroll = nEnd - m_rcClient.Height() + 10;
			if (nStart < m_lYScroll)
			{
				m_lYScroll = nStart - 10;
			}
		}
	}

	SCROLLINFO si = {0};
	si.fMask = SIF_POS;
	si.nPos = m_lYScroll;
	SetScrollInfo(SB_VERT, &si, TRUE);
	GetScrollInfo(SB_VERT, &si, SIF_POS);
	m_lYScroll = si.nPos;
}

void CItemShowWnd::ScrollToItem(CItem *pItem, int nSub)
{
	int i = 0;
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(i);
	
	while(pMgr)
	{
		if (pMgr->GetItem() == pItem)
		{
			break;
		}
		++i;
		pMgr = (CShowManager*)m_pInfo->GetData(i);
	}
	if (!pMgr)
	{
		return;
	}

	ScrollToItem(i, nSub);
}

void CItemShowWnd::InitScroll()
{
	SCROLLINFO si = {0};
	si.fMask = SIF_POS;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE);

	GetScrollInfo(SB_VERT, &si, SIF_POS);
	m_lYScroll = si.nPos;
}

void CItemShowWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);
	
	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}
	
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.
		
		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}
		
		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void CItemShowWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// 向父窗口请求颜色
	GetParent()->SendMessage(WM_CTLCOLORSTATIC, (WPARAM)dc.GetSafeHdc(), (LPARAM)GetSafeHwnd());
	
	GetClientRect(m_rcClient);
	m_Bmp.CreateBmp(dc.GetSafeHdc(), m_rcClient.Width(), m_rcClient.Height(), TRUE, dc.GetBkColor());

	HDC hBmpDC = m_Bmp.GetDC();
	COLORREF clrOld = SetTextColor(hBmpDC, dc.GetTextColor());

	DrawItem(m_Bmp.GetDC());
	m_Bmp.Draw(dc.GetSafeHdc(), 0, 0);

	SetTextColor(hBmpDC, clrOld);
}

void CItemShowWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CloseEdit();

	SCROLLINFO si = {0};
	GetScrollInfo(SB_HORZ, &si, SIF_ALL);
	
	switch(nSBCode)
	{
	case SB_LEFT:
		si.nPos = si.nMin;
		break;
		
	case SB_RIGHT:
		si.nPos = si.nMax;
		break;
		
	case SB_LINELEFT:
		si.nPos -= m_lFontHeight;
		break;
		
	case SB_LINERIGHT:
		si.nPos += m_lFontHeight;
		break;
		
	case SB_PAGELEFT:
		si.nPos += si.nPage;
		break;
		
	case SB_PAGERIGHT:
		si.nPos -= si.nPage;
		break;
		
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(SB_HORZ, &si, TRUE);
	GetScrollInfo(SB_HORZ, &si);
	if (si.nPos != m_lXScroll)
	{
		m_lXScroll = si.nPos;
		Invalidate(FALSE);
	}
}

void CItemShowWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CloseEdit();

	SCROLLINFO si = {0};
	GetScrollInfo(SB_VERT, &si, SIF_ALL);

	switch(nSBCode)
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;

	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;

	case SB_LINEUP:
		si.nPos -= m_lFontHeight;
		break;

	case SB_LINEDOWN:
		si.nPos += m_lFontHeight;
		break;

	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;

	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	}
	
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si, TRUE);
	GetScrollInfo(SB_VERT, &si);
	if (si.nPos != m_lYScroll)
	{
		m_lYScroll = si.nPos;
		Invalidate(FALSE);
	}
}

BOOL CItemShowWnd::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	CloseEdit();

	SCROLLINFO si = {0};
	GetScrollInfo(SB_VERT, &si, SIF_ALL);

	si.nPos -= (zDelta * 2 / 3);
	si.fMask = SIF_POS;
	SetScrollInfo(SB_VERT, &si, TRUE);
	GetScrollInfo(SB_VERT, &si);
	if (si.nPos != m_lYScroll)
	{
		m_lYScroll = si.nPos;
		Invalidate(FALSE);
	}

	POINT point = {pt.x, pt.y};
	ScreenToClient(&point);
	ISWSetCursor(point);

	return TRUE;
}

/*
功能：	设定题目的显示参数
返回值：无
备注：	
*/
void CItemShowWnd::SetShowInfo(HDC hDC)
{
	CShowManager *pMgr;
	long lyOffset = 0, lWidth, lHeight; // 当总高度超过了显示高度，则显示滚动条
	CItem *pItem;
	int i;
	int nIndex; // 题目序号
	int nMgrIndex; // 管理器序号

	m_bSizing = TRUE;

	::ShowScrollBar(GetSafeHwnd(), SB_HORZ, FALSE);
	::ShowScrollBar(GetSafeHwnd(), SB_VERT, FALSE);
	m_bXScroll = m_bYScroll = FALSE;
	
	GetClientRect(m_rcClient);
	lWidth = m_rcClient.Width();
	if (lWidth < MINWIDTH)
	{
		lWidth = MINWIDTH;
		m_bXScroll = TRUE;
	}

	if (m_nShowIndex < 0)
	{
		i = 0;
	}
	else
	{
		i = m_nShowIndex;
	}
	nIndex = i + 1;
	for (nMgrIndex = 0; i < m_pList->GetItemCount(); )
	{
		CItem *pParent = NULL;

		pItem = m_pList->GetItem(i);
		if (!pItem)
		{
			break;
		}

		// 当题目不是题目组时，需要检查是否为题目组的子题目
		if (pItem->GetType() != TYPE_GROUP)
		{
			pParent = pItem->GetItemFrom()->GetParent();
		}

		// 是题目组的子题目
		// 此if为浏览界面和主观题评分界面可用
		if (pParent && pParent->GetType() == TYPE_GROUP)
		{
			CItemGroup *pGroup = (CItemGroup*)pParent;
			if (lstrlen(pGroup->GetDescription()))
			{
				pMgr = GetShowManager(nMgrIndex, pGroup);
				if (!pMgr)
				{
					return;
				}
				lHeight = pMgr->SetItemShow(hDC, lyOffset, lWidth, m_dwShowStyle, nIndex, m_nShowIndex);
				m_pInfo->SetData(nMgrIndex++, pMgr);
				lyOffset += lHeight + pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);
			}
		}

		// 如果是题目组，此段为题目组描述
		// 如果是一般题目，此段为题目内容
		pMgr = GetShowManager(nMgrIndex, pItem);
		if (!pMgr)
		{
			return;
		}
		lHeight = pMgr->SetItemShow(hDC, lyOffset, lWidth, m_dwShowStyle, nIndex, m_nShowStep);
		m_pInfo->SetData(nMgrIndex++, pMgr);
		lyOffset += lHeight;

		// 如果是题目组，添加题目组的子题目
		// 此if为属性界面、考试界面和查看答案界面可用
		if (pItem->GetType() == TYPE_GROUP)
		{
			int j;
			int nSubIndex;
			DWORD dwShow = m_dwShowStyle | SHOWSTYLE_WITHINDEX;
			CItemGroup *pGroup = (CItemGroup*)pItem;

			for (j = 0; j < pGroup->GetItemCount(); ++j)
			{
				lyOffset += pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);

				pItem = pGroup->GetItem(j);
				if (!pItem)
				{
					break;
				}

				pMgr = GetShowManager(nMgrIndex, pItem);
				if (!pMgr)
				{
					return;
				}
				
				if (m_dwShowStyle & SHOWSTYLE_WITHINDEX)
				{
					nSubIndex = nIndex;
				}
				else
				{
					nSubIndex = j + 1;
				}
				lHeight = pMgr->SetItemShow(hDC, lyOffset, lWidth, dwShow, nSubIndex, m_nShowStep);
				m_pInfo->SetData(nMgrIndex++, pMgr);
				++nIndex;
				lyOffset += lHeight;
			}
		}
		else
		{
			++nIndex;
		}
		lyOffset += pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);

		if (!m_bYScroll && lyOffset > m_rcClient.Height())
		{
			// 重新计算
			m_bYScroll = TRUE;
			lyOffset = 0;
			lWidth -= GetSystemMetrics(SM_CXVSCROLL);
			
			if (m_nShowIndex < 0)
			{
				i = 0;
			}
			else
			{
				i = m_nShowIndex;
			}
			nIndex = i + 1;
			nMgrIndex = 0;
			continue;
		}

		if (m_nShowIndex < 0)
		{
			++i;
		}
		else
		{
			break;
		}
	}
	lyOffset -= pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);
	m_nInfoCount = nMgrIndex;

	for (i = 0; i < m_nInfoCount; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (lWidth < pMgr->GetShowWidth())
		{
			m_bXScroll = TRUE;
			lWidth = max(lWidth, pMgr->GetShowWidth());
		}
	}

	if (m_lTotalHeight)
	{
		m_lYScroll = m_lYScroll * lyOffset / m_lTotalHeight;
	}
	else
	{
		m_lYScroll = 0;
	}
	m_lTotalHeight = lyOffset;

	// 设置滚动条的参数
	SCROLLINFO si = {0};
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nPos = 0;
	si.nMin = 0;
	si.nMax = lWidth;
	si.nPage = m_rcClient.Width() - (m_bYScroll ? GetSystemMetrics(SM_CXVSCROLL) : 0);
	SetScrollInfo(SB_HORZ, &si, TRUE);

	si.nPos = m_lYScroll;
	si.nMax = m_lTotalHeight;
	si.nPage = m_rcClient.Height() - (m_bXScroll ? GetSystemMetrics(SM_CYHSCROLL) : 0);
	SetScrollInfo(SB_VERT, &si, TRUE);

	si.fMask = SIF_POS;
	GetScrollInfo(SB_VERT, &si);
	if (si.nPos != m_lYScroll)
	{
		m_lYScroll = si.nPos;
	}

	::ShowScrollBar(GetSafeHwnd(), SB_HORZ, m_bXScroll);
	::ShowScrollBar(GetSafeHwnd(), SB_VERT, m_bYScroll);
	m_lXScroll = 0;

	m_bSizing = FALSE;
	Invalidate(TRUE);
}

CShowManager* CItemShowWnd::GetShowManager(int nIndex, CItem *pItem)
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(nIndex);
	if (!pMgr)
	{
		pMgr = new CShowManager(pItem, m_nShowType, m_hFont);
	}
	else if (pMgr->GetItem() != pItem)
	{
		ClearShowManager(pMgr);
		pMgr = new CShowManager(pItem, m_nShowType, m_hFont);
	}
	return pMgr;
}

/*
功能：	在用户进行输入后，更新显示
返回值：无
备注：	nStart参数为受到影响的显示管理器序号
*/
void CItemShowWnd::UpdateShowInfo(HDC hDC, int nStart)
{
	CShowManager *pMgr;
	long lyOffset, lWidth, lHeight; // 当总高度超过了显示高度，则显示滚动条
	long lHoleHeight, lNewHeight;
	int i;
	DWORD dwShow = m_dwShowStyle;
	SCROLLINFO si = {0};

	// 获取客户区的总宽度
	GetClientRect(m_rcClient);
	lWidth = m_rcClient.Width();

	// 检查是否有滚动条的变化
	pMgr = (CShowManager*)m_pInfo->GetData(nStart);
	lHeight = pMgr->GetShowHeight();
	lyOffset = pMgr->GetYOffset();

	// 计算新的高度
	lNewHeight = pMgr->RecalcItemShow(hDC, lyOffset, lWidth);
	lyOffset += lNewHeight + pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);

	// 计算修改前后的总高度
	lHoleHeight = m_lTotalHeight - lHeight + lNewHeight;

	// 如果滚动条发生了变化，则需要重新布置所有显示
	if ((m_lTotalHeight <= m_rcClient.Height() && lHoleHeight > m_rcClient.Height()) ||
		(m_lTotalHeight > m_rcClient.Height() && lHoleHeight <= m_rcClient.Height()))
	{
		RefreshShowInfo();
		return;
	}
	// 如果滚动条没有发生变化，则重新计算后续内容的Y偏移量
	else
	{
		lNewHeight -= lHeight;
	}
	
	for (i = nStart + 1; i < m_nInfoCount; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (!pMgr)
		{
			return;
		}

		lHeight = pMgr->AddOffset(lNewHeight);
		lyOffset += lHeight + pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);
	}

	// 设置滚动条的参数
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nPos = 0;
	si.nMin = 0;
	si.nMax = lyOffset;
	si.nPage = m_rcClient.Height() - (m_bXScroll ? GetSystemMetrics(SM_CYHSCROLL) : 0);
	SetScrollInfo(SB_VERT, &si, FALSE);

	si.fMask = SIF_POS;
	GetScrollInfo(SB_VERT, &si);
	if (si.nPos != m_lYScroll)
	{
		m_lYScroll = si.nPos;
	}

	Invalidate(FALSE);
}

void CItemShowWnd::DrawItem(HDC hDC)
{
	int nOldMode = ::SetBkMode(hDC, TRANSPARENT);
	COLORREF clrOld = ::SetBkColor(hDC, AfxGetQColor(QCOLOR_TEXTSELECTBACK));
	int i = 0;
	BOOL bSel = FALSE;

	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(i);
	while(pMgr)
	{
		if (pMgr->IsVisible(m_lYScroll, m_rcClient.Height()))
		{
			break;
		}
		++i;
		pMgr = (CShowManager*)m_pInfo->GetData(i);
	}

	for (; i < m_nInfoCount; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (!pMgr->IsVisible(m_lYScroll, m_rcClient.Height()))
		{
			break;
		}

		switch(m_nShowType)
		{
		case SHOWWNDTYPE_TEST:
		case SHOWWNDTYPE_TESTVIEW:
			if (m_nShowIndex >= 0)
			{
				// 当单题模式时，不显示高亮
				bSel = FALSE;
			}
			else
			{
				bSel = (m_nSel == i);
			}
			break;

		case SHOWWNDTYPE_PROPERTY:
			if (m_nInfoCount > 1)
			{
				bSel = (m_nSel == i);
			}
			break;

		default:
			break;
		}
		pMgr->DrawItem(hDC, m_lXScroll, m_lYScroll, bSel);
	}
	
	::SetBkMode(hDC, nOldMode);
	::SetBkColor(hDC, clrOld);
}

/*
功能：	对鼠标指向的位置进行测试
返回值：测试结果
备注：	当pHit不为NULL时，将影响以下参数：
		nMgrIndex, nBlkType
*/
int CItemShowWnd::HitTest(int x, int y, PISWHITTEST pHit)
{
	int i = 0;
	int nHeight = m_rcClient.Height();
	ISWHITTEST Hit = {0};
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(i);
	HDC hDC = ::GetDC(GetSafeHwnd());
	
	while(pMgr)
	{
		if (pMgr->IsVisible(m_lYScroll, nHeight))
		{
			break;
		}
		++i;
		pMgr = (CShowManager*)m_pInfo->GetData(i);
	}
	if (!pMgr)
	{
		goto _HitTest_Return;
	}

	while(pMgr)
	{
		if (!pMgr->IsVisible(m_lYScroll, nHeight))
		{
			break;
		}
		if (!pMgr->HitTest(hDC, m_lXScroll + x, m_lYScroll + y, &Hit))
		{
			break;
		}

		Hit.nMgrIndex = i;

		switch(Hit.nHitType)
		{
		case SHOWHITTEST_TEXT:
		case SHOWHITTEST_OPTION:
		case SHOWHITTEST_BLANK:
		case SHOWHITTEST_TEXTANS:
		case SHOWHITTEST_INITEM:
		case SHOWHITTEST_IMAGE:
			if (pHit)
			{
				memcpy(pHit, &Hit, sizeof(Hit));
			}
			goto _HitTest_Return;

		default:
			break;
		}

		++i;
		pMgr = (CShowManager*)m_pInfo->GetData(i);
	}

	// 当只有一道题目时，鼠标在题目下方空白移动时
	// 也视为在题目中移动，但是没有目标
	if (pHit)
	{
		memcpy(pHit, &Hit, sizeof(Hit));
	}

_HitTest_Return:
	::ReleaseDC(GetSafeHwnd(), hDC);
	return Hit.nHitType;
}

int CItemShowWnd::ClearShowManager(PVOID p)
{
	CShowManager *pInfo = (CShowManager*)p;
	delete pInfo;
	return 0;
}

void CItemShowWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	ISWSetCursor(point);

	if (SHOWWNDTYPE_PROPERTY == m_nShowType ||
		SHOWWNDTYPE_TESTVIEW == m_nShowType ||
		SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
	{
		m_point = point;
		if (m_point.y < 0)
		{
			m_point.y = 0;
			m_nAutoScroll = ISW_AUTOSCROLLUP;
		}
		else if (m_point.y > m_rcClient.Height())
		{
			m_point.y = m_rcClient.Height();
			m_nAutoScroll = ISW_AUTOSCROLLDOWN;
		}
		else
		{
			m_nAutoScroll = ISW_AUTOSCROLLDEFAULT;
		}
		
		if (m_bClick)
		{
			ISWHITTEST Hit = {0};
			HitTest(m_point.x, m_point.y, &Hit);

			SetTextSelect(&Hit, FALSE);
			Invalidate(FALSE);
		}
		return;
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CItemShowWnd::ISWSetCursor(CPoint point)
{
	if (SHOWWNDTYPE_TEST == m_nShowType)
	{
		switch(HitTest(point.x, point.y, NULL))
		{
		case SHOWHITTEST_OPTION:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			break;
			
		case SHOWHITTEST_BLANK:
		case SHOWHITTEST_TEXTANS:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			break;
			
		default:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			break;
		}
	}
	else if (SHOWWNDTYPE_PROPERTY == m_nShowType ||
			 SHOWWNDTYPE_TESTVIEW == m_nShowType ||
			 SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
	{
		switch(HitTest(point.x, point.y, NULL))
		{
		case SHOWHITTEST_NONE:
		case SHOWHITTEST_INITEM:
		case SHOWHITTEST_IMAGE:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			break;
			
		case SHOWHITTEST_BLANK:
			if (SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
			{
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
			}
			else
			{
				SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			}
			break;
			
		default:
			SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
			break;
		}
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CItemShowWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	switch(m_nShowType)
	{
	case SHOWWNDTYPE_TEST:
		if (HTCLIENT == nHitTest)
		{
			return TRUE;
		}
		break;

	case SHOWWNDTYPE_PROPERTY:
	case SHOWWNDTYPE_TESTVIEW:
	case SHOWWNDTYPE_SUBJECTIVEVIEW:
		return TRUE;

	default:
		break;
	}

	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CItemShowWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_uHitTest = HitTest(point.x, point.y, &m_HitInfo);

	switch(m_nShowType)
	{
	case SHOWWNDTYPE_PROPERTY:
	case SHOWWNDTYPE_TESTVIEW:
	case SHOWWNDTYPE_SUBJECTIVEVIEW:
		SetTimer(ISW_TIMER_AUTOSCROLL, ISW_AUTOSCROLLTIME, NULL);
		SetTextSelect(&m_HitInfo, TRUE);
		break;

	default:
		break;
	}
	
	m_bClick = TRUE;
	SetCapture();
	SetFocus();
	CWnd::OnLButtonDown(nFlags, point);
}

void CItemShowWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ISWHITTEST Hit = {0};
	UINT uHit = HitTest(point.x, point.y, &Hit);

	if (uHit != SHOWHITTEST_NONE && uHit == m_uHitTest &&
		Hit.nMgrIndex == m_HitInfo.nMgrIndex)
	{
		if (SHOWWNDTYPE_TEST == m_nShowType)
		{
			CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(Hit.nMgrIndex);
			CItem *pItem = pMgr->GetItem();
			HDC hDC = ::GetDC(GetSafeHwnd());

			switch(uHit)
			{
			case SHOWHITTEST_OPTION:
				if (pItem->GetType() == TYPE_SCHOISE ||
					pItem->GetType() == TYPE_MCHOISE)
				{
					((CChoise*)pItem)->SetUserAnswer(Hit.nInfo);
				}
				else if (pItem->GetType() == TYPE_JUDGE)
				{
					((CJudge*)pItem)->SetUserAnswer(Hit.nInfo);
				}
				UpdateShowInfo(hDC, Hit.nMgrIndex);
				GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_USERSET, 0);
				break;
				
			case SHOWHITTEST_BLANK:
			case SHOWHITTEST_TEXTANS:
				ScrollToItem(Hit.nMgrIndex, 0);
				ShowEdit(Hit.nMgrIndex, Hit.nInfo);
				GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_USERSET, 0);
				break;
				
			default:
				break;
			}
			
			::ReleaseDC(GetSafeHwnd(), hDC);
		}
		else if (SHOWWNDTYPE_PROPERTY == m_nShowType ||
				 SHOWWNDTYPE_TESTVIEW == m_nShowType)
		{
			SetTextSelect(&Hit, FALSE);
		}
		else if (SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
		{
			if (SHOWHITTEST_BLANK == uHit)
			{
				CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(Hit.nMgrIndex);
				CItem *pItem = pMgr->GetItem();
				CBlank *pBlank = (CBlank*)pItem;
				BOOL bCheck = pBlank->GetUserAnswerCheck(Hit.nInfo);

				pBlank->SetUserAnswerCheck(Hit.nInfo, !bCheck);
				Invalidate(FALSE);
				GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_BLANKCHECK, (LPARAM)pBlank);
			}
			SetTextSelect(&Hit, FALSE);
		}

		SetItemSelect(Hit.nMgrIndex, TRUE);
	}
	
	KillTimer(ISW_TIMER_AUTOSCROLL);
	m_nAutoScroll = ISW_AUTOSCROLLDEFAULT;

	m_bClick = FALSE;
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CItemShowWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ISWHITTEST Hit = {0};
	UINT uHit = HitTest(point.x, point.y, &Hit);
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(Hit.nMgrIndex);
	CItem *pItem = pMgr->GetItem();

	switch(uHit)
	{
	case SHOWHITTEST_NONE:
		break;

	default:
		GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_STOREITEM, (LPARAM)pItem);
		break;
	}
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CItemShowWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (SHOWWNDTYPE_PROPERTY == m_nShowType ||
		SHOWWNDTYPE_TESTVIEW == m_nShowType)
	{
		HitTest(point.x, point.y, &m_HitInfo);

		CMenu cMenu;
		cMenu.LoadMenu(IDR_SHOWMENU);
		CMenu *pDelMenu = cMenu.GetSubMenu(0);
		
		ClientToScreen(&point);
		pDelMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CItemShowWnd::OnTimer(UINT nIDEvent) 
{
	switch(nIDEvent)
	{
	case ISW_TIMER_AUTOSCROLL:
		{
			SCROLLINFO si = {0};
			ISWHITTEST Hit = {0};

			ASSERT(SHOWWNDTYPE_PROPERTY == m_nShowType ||
				   SHOWWNDTYPE_TESTVIEW == m_nShowType ||
				   SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType);

			si.fMask = SIF_POS;
			switch(m_nAutoScroll)
			{
			case ISW_AUTOSCROLLDOWN:
				si.nPos = m_lYScroll + ISW_AUTOSCROLLSTEP;
				break;
				
			case ISW_AUTOSCROLLUP:
				si.nPos = m_lYScroll - ISW_AUTOSCROLLSTEP;
				break;
				
			default:
				goto _OnTimer_Return;
			}
			SetScrollInfo(SB_VERT, &si, TRUE);
			GetScrollInfo(SB_VERT, &si, SIF_POS);
			m_lYScroll = si.nPos;

			HitTest(m_point.x, m_point.y, &Hit);
			SetTextSelect(&Hit, FALSE);
			Invalidate(FALSE);
		}
		break;

	default:
		break;
	}
	
_OnTimer_Return:
	CWnd::OnTimer(nIDEvent);
}

void CItemShowWnd::ShowEdit(int nMgrIndex, int nBlkIndex)
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(nMgrIndex);
	CItem *pItem = pMgr->GetItem();
	RECT rc = {0};

	if (pMgr->GetEditRect(nBlkIndex, &rc))
	{
		if (pItem->GetType() == TYPE_BLANK)
		{
			m_cTextEdit.ShowWindow(SW_HIDE);
			m_cBlankEdit.MoveWindow(rc.left - m_lXScroll, rc.top - m_lYScroll,
				rc.right - rc.left, rc.bottom - rc.top);

			m_cBlankEdit.SetWindowText(((CBlank*)pItem)->GetUserAnswer(nBlkIndex));
			m_cBlankEdit.ShowWindow(SW_SHOW);
			m_cBlankEdit.SetSel(0, -1);
			m_cBlankEdit.SetFocus();

			pMgr->ShowBlankText(nBlkIndex, FALSE);
			Invalidate(FALSE);
		}
		else
		{
			m_cBlankEdit.ShowWindow(SW_HIDE);
			
			GetClientRect(m_rcClient);
			if (rc.bottom - m_lYScroll > m_rcClient.Height())
			{
				rc.bottom = m_lYScroll + m_rcClient.Height();
			}
			m_cTextEdit.MoveWindow(rc.left - m_lXScroll, rc.top - m_lYScroll,
				rc.right - rc.left, rc.bottom - rc.top);

			m_cTextEdit.SetWindowText(((CText*)pItem)->GetUserAnswer());
			m_cTextEdit.ShowWindow(SW_SHOW);
			m_cTextEdit.SetSel(0, -1);
			m_cTextEdit.SetFocus();
		}
	}

	m_EditInfo.dwPara1 = nMgrIndex;
	m_EditInfo.dwPara2 = nBlkIndex;
	m_bEditing = TRUE;
}

void CItemShowWnd::CloseEdit()
{
	if (SHOWWNDTYPE_TEST == m_nShowType)
	{
		if (!m_bEditing)
		{
			return;
		}

		CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(m_EditInfo.dwPara1);
		CItem *pItem = pMgr->GetItem();
		CString string;
		HDC hDC = ::GetDC(GetSafeHwnd());

		if (pItem->GetType() == TYPE_BLANK)
		{
			m_cBlankEdit.GetWindowText(string);
			((CBlank*)pItem)->SetUserAnswer(m_EditInfo.dwPara2, string);
		}
		else
		{
			m_cTextEdit.GetWindowText(string);
			((CText*)pItem)->SetUserAnswer(string);
		}
		UpdateShowInfo(hDC, m_EditInfo.dwPara1);
		::ReleaseDC(GetSafeHwnd(), hDC);

		m_cBlankEdit.ShowWindow(SW_HIDE);
		m_cTextEdit.ShowWindow(SW_HIDE);

		m_bEditing = FALSE;
		pMgr->ShowBlankText(m_EditInfo.dwPara2, TRUE);
		Invalidate(FALSE);
		GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_USERSET, 0);
	}
}

LRESULT CItemShowWnd::OnQEditNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case QEN_ESCAPE:
		CloseEdit();
		break;

	case QEN_NEXT:
		SelectNextItem();
		break;

	case QEN_PREV:
		SelectPrevItem();
		break;

	default:
		break;
	}

	return 0;
}

void CItemShowWnd::SelectNextItem()
{
	CShowManager *pShow;
	CItem *pItem = GetSelection();

	CloseEdit();

	// 题目内跳转（只有填空题可能）
	if (pItem->GetType() == TYPE_BLANK)
	{
		CBlank *pBlk = (CBlank*)pItem;
		if (m_EditInfo.dwPara2 < pBlk->GetBlankCount() - 1)
		{
			++m_EditInfo.dwPara2;
			ScrollToItem(m_nSel, m_EditInfo.dwPara2);
			ShowEdit(&m_EditInfo);
			return;
		}
	}
	
	if (m_nShowIndex >= 0)
	{
		++m_nShowIndex;
		if (m_nShowIndex >= m_pList->GetItemCount())
		{
			m_nShowIndex = 0;
		}

		pItem = m_pList->GetItem(m_nShowIndex);

		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
			ShowItem(m_nShowIndex, m_dwShowStyle);
			break;
			
		case TYPE_BLANK:
		case TYPE_TEXT:
			ShowItem(m_nShowIndex, m_dwShowStyle);
			m_EditInfo.dwPara1 = m_nSel;
			m_EditInfo.dwPara2 = 0;
			ShowEdit(&m_EditInfo);
			break;
		}

		GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_NEXT, m_nShowIndex);
	}
	else
	{
		++m_nSel;
		if (m_nSel >= m_nInfoCount)
		{
			m_nSel = 0;
		}

		pShow = (CShowManager*)m_pInfo->GetData(m_nSel);
		pItem = pShow->GetItem();
		while(pItem->GetType() == TYPE_GROUP)
		{
			++m_nSel;
			pShow = (CShowManager*)m_pInfo->GetData(m_nSel);
			pItem = pShow->GetItem();
		}

		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
			SetItemSelect(m_nSel, TRUE);
			ScrollToItem(m_nSel, 0);
			break;

		case TYPE_BLANK:
		case TYPE_TEXT:
			m_EditInfo.dwPara1 = m_nSel;
			m_EditInfo.dwPara2 = 0;
			SetItemSelect(m_nSel, TRUE);
			ScrollToItem(m_nSel, 0);
			ShowEdit(&m_EditInfo);
			break;
		}
	}
}

void CItemShowWnd::SelectPrevItem()
{
	CShowManager *pShow;
	CItem *pItem = GetSelection();
	
	CloseEdit();
	
	// 题目内跳转（只有填空题可能）
	if (pItem->GetType() == TYPE_BLANK)
	{
		CBlank *pBlk = (CBlank*)pItem;
		if (m_EditInfo.dwPara2 > 0)
		{
			--m_EditInfo.dwPara2;
			ScrollToItem(m_nSel, m_EditInfo.dwPara2);
			ShowEdit(&m_EditInfo);
			return;
		}
	}

	// 题目间跳转
	if (m_nShowIndex >= 0)
	{
		// 为单题模式时，题目列表为子题目列表，不可能有题目组
		--m_nShowIndex;
		if (m_nShowIndex < 0)
		{
			m_nShowIndex = m_pList->GetItemCount() - 1;
		}

		pItem = m_pList->GetItem(m_nShowIndex);

		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
			ShowItem(m_nShowIndex, m_dwShowStyle);
			break;

		case TYPE_BLANK:
			ShowItem(m_nShowIndex, m_dwShowStyle);
			m_EditInfo.dwPara1 = 0;
			m_EditInfo.dwPara2 = ((CBlank*)pItem)->GetBlankCount() - 1;
			ShowEdit(&m_EditInfo);
			break;

		case TYPE_TEXT:
			ShowItem(m_nShowIndex, m_dwShowStyle);
			m_EditInfo.dwPara1 = m_nSel;
			m_EditInfo.dwPara2 = 0;
			ShowEdit(&m_EditInfo);
			break;

		default:
			ASSERT(FALSE);
		}

		GetParent()->SendMessage(WM_ITEMSHOWWNDNOTIFY, ISN_PRE, m_nShowIndex);
	}
	else
	{
		// 多题模式时，只能通过显示信息跳转
		--m_nSel;
		if (m_nSel < 0)
		{
			m_nSel = m_nInfoCount - 1;
		}

		pShow = (CShowManager*)m_pInfo->GetData(m_nSel);
		pItem = pShow->GetItem();

		while(pItem->GetType() == TYPE_GROUP)
		{
			--m_nSel;
			pShow = (CShowManager*)m_pInfo->GetData(m_nSel);
			pItem = pShow->GetItem();
		}
		
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
			SetItemSelect(m_nSel, TRUE);
			ScrollToItem(m_nSel, 0);
			break;
			
		case TYPE_BLANK:
			SetItemSelect(m_nSel, TRUE);
			ScrollToItem(m_nSel, m_EditInfo.dwPara2);
			m_EditInfo.dwPara1 = m_nSel;
			m_EditInfo.dwPara2 = ((CBlank*)pItem)->GetBlankCount() - 1;
			ShowEdit(&m_EditInfo);
			break;
			
		case TYPE_TEXT:
			SetItemSelect(m_nSel, TRUE);
			ScrollToItem(m_nSel, 0);
			m_EditInfo.dwPara1 = m_nSel;
			m_EditInfo.dwPara2 = 0;
			ShowEdit(&m_EditInfo);
			break;
			
		default:
			ASSERT(FALSE);
		}
	}
}

BOOL CItemShowWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	WORD wCode = HIWORD(wParam);
	WORD wID = LOWORD(wParam);
	
	if (EN_KILLFOCUS == wCode)
	{
		CloseEdit();
		return TRUE;
	}
	
	return CWnd::OnCommand(wParam, lParam);
}

void CItemShowWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (SHOWWNDTYPE_TEST == m_nShowType)
	{
		switch(nChar)
		{
		case VK_TAB:
			if (GetKeyState(VK_CONTROL) < 0)
			{
				if (GetKeyState(VK_SHIFT) < 0)
				{
					SelectPrevItem();
				}
				else
				{
					SelectNextItem();
				}
			}
			break;
			
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
			{
				CItem *pItem = GetSelection();
				if (pItem)
				{
					switch(pItem->GetType())
					{
					case TYPE_SCHOISE:
					case TYPE_MCHOISE:
						((CChoise*)pItem)->SetUserAnswer(nChar - 'A');
						UpdateShowInfo();
						break;

					case TYPE_JUDGE:
						((CJudge*)pItem)->SetUserAnswer(('A' == nChar) ? TRUE : FALSE);
						UpdateShowInfo();
						break;
					}
				}
			}
			break;

		case 'Y':
		case 'V':
		case 'T':
			{
				CItem *pItem = GetSelection();
				if (pItem)
				{
					switch(pItem->GetType())
					{
					case TYPE_JUDGE:
						((CJudge*)pItem)->SetUserAnswer(TRUE);
						UpdateShowInfo();
						break;
					}
				}
			}
			break;

		case 'N':
		case 'X':
			{
				CItem *pItem = GetSelection();
				if (pItem)
				{
					switch(pItem->GetType())
					{
					case TYPE_JUDGE:
						((CJudge*)pItem)->SetUserAnswer(FALSE);
						UpdateShowInfo();
						break;
					}
				}
			}
			break;
		}
	}
	
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CItemShowWnd::SetTextSelect(PISWHITTEST pHit, BOOL bIsStart)
{
	int i;
	CShowManager *pMgr;
	int nStartMgr, nStartBlock, nStartPos, nEndMgr, nEndBlock, nEndPos;

	// 清理所有的选中信息
	for (i = 0; i < m_nInfoCount; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		pMgr->SetTextSelect(0, 0, 0, 0);
	}

	if (bIsStart)
	{
		m_SelInfo.nStartMgr = pHit->nMgrIndex;
		m_SelInfo.nStartBlock = pHit->nBlockIndex;
		m_SelInfo.nStartPos = pHit->nTextPos;
	}
	m_SelInfo.nEndMgr = pHit->nMgrIndex;
	m_SelInfo.nEndBlock = pHit->nBlockIndex;
	m_SelInfo.nEndPos = pHit->nTextPos;

	if (m_SelInfo.nStartMgr == m_SelInfo.nEndMgr)
	{
		if (m_SelInfo.nStartBlock == m_SelInfo.nEndBlock)
		{
			nStartBlock = nEndBlock = m_SelInfo.nStartBlock;
			nStartPos = min(m_SelInfo.nStartPos, m_SelInfo.nEndPos);
			nEndPos = max(m_SelInfo.nStartPos, m_SelInfo.nEndPos);
		}
		else if (m_SelInfo.nStartBlock < m_SelInfo.nEndBlock)
		{
			nStartBlock = m_SelInfo.nStartBlock;
			nStartPos = m_SelInfo.nStartPos;
			nEndBlock = m_SelInfo.nEndBlock;
			nEndPos = m_SelInfo.nEndPos;
		}
		else
		{
			nStartBlock = m_SelInfo.nEndBlock;
			nStartPos = m_SelInfo.nEndPos;
			nEndBlock = m_SelInfo.nStartBlock;
			nEndPos = m_SelInfo.nStartPos;
		}

		pMgr = (CShowManager*)m_pInfo->GetData(m_SelInfo.nStartMgr);
		if (!pMgr)
		{
			return;
		}
		pMgr->SetTextSelect(nStartBlock, nStartPos, nEndBlock, nEndPos);
	}
	else
	{
		if (m_SelInfo.nStartMgr < m_SelInfo.nEndMgr)
		{
			nStartMgr = m_SelInfo.nStartMgr;
			nStartBlock = m_SelInfo.nStartBlock;
			nStartPos = m_SelInfo.nStartPos;
			nEndMgr = m_SelInfo.nEndMgr;
			nEndBlock = m_SelInfo.nEndBlock;
			nEndPos = m_SelInfo.nEndPos;
		}
		else
		{
			nStartMgr = m_SelInfo.nEndMgr;
			nStartBlock = m_SelInfo.nEndBlock;
			nStartPos = m_SelInfo.nEndPos;
			nEndMgr = m_SelInfo.nStartMgr;
			nEndBlock = m_SelInfo.nStartBlock;
			nEndPos = m_SelInfo.nStartPos;
		}

		pMgr = (CShowManager*)m_pInfo->GetData(nStartMgr);
		if (!pMgr)
		{
			return;
		}
		pMgr->SetTextSelect(nStartBlock, nStartPos, -1, -1);

		for (i = nStartMgr + 1; i < nEndMgr; ++i)
		{
			pMgr = (CShowManager*)m_pInfo->GetData(i);
			if (!pMgr)
			{
				return;
			}
			pMgr->SetTextSelect(0, 0, -1, -1);
		}

		pMgr = (CShowManager*)m_pInfo->GetData(nEndMgr);
		if (!pMgr)
		{
			return;
		}
		pMgr->SetTextSelect(0, 0, nEndBlock, nEndPos);
	}

	Invalidate(FALSE);
}

void CItemShowWnd::Copy(CString &str)
{
	int a = min(m_SelInfo.nStartMgr, m_SelInfo.nEndMgr);
	int b = max(m_SelInfo.nStartMgr, m_SelInfo.nEndMgr);
	int i;
	CShowManager *pMgr;

	str.Empty();
	for (i = a; i <= b; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		if (!pMgr)
		{
			return;
		}

		pMgr->Copy(str);
	}

	for (i = str.GetLength() - 1; i >= 0; --i)
	{
		if (str[i] == _T('\r') ||
			str[i] == _T('\n'))
		{
			str.Delete(i);
		}
		else
		{
			break;
		}
	}
}

void CItemShowWnd::OnCopy() 
{
	CString str;
	HGLOBAL hGlobal;
	PTSTR pGlobal;

	switch(m_nShowType)
	{
	case SHOWWNDTYPE_PROPERTY:
	case SHOWWNDTYPE_TESTVIEW:
	case SHOWWNDTYPE_SUBJECTIVEVIEW:
		break;

	default:
		return;
	}
	Copy(str);

	hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (str.GetLength() + 1) * sizeof (TCHAR));
	pGlobal = (PTSTR)GlobalLock(hGlobal);
	
	lstrcpy(pGlobal, str);
	GlobalUnlock(hGlobal);
	
	if (::OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_TCHAR, hGlobal);
		CloseClipboard();
	}
}

void CItemShowWnd::OnSelectAll() 
{
	switch(m_nShowType)
	{
	case SHOWWNDTYPE_PROPERTY:
	case SHOWWNDTYPE_TESTVIEW:
	case SHOWWNDTYPE_SUBJECTIVEVIEW:
		break;
		
	default:
		return;
	}

	int i;
	CShowManager *pMgr;

	m_SelInfo.nStartMgr = 0;
	m_SelInfo.nStartBlock = 0;
	m_SelInfo.nStartPos = 0;

	m_SelInfo.nEndMgr = m_nInfoCount - 1;
	pMgr = (CShowManager*)m_pInfo->GetData(m_SelInfo.nEndMgr);
	m_SelInfo.nEndBlock = pMgr->GetBlockCount();
	m_SelInfo.nEndPos = pMgr->GetBlockLength(m_SelInfo.nEndBlock);
	
	// 清理所有的选中信息
	for (i = 0; i < m_nInfoCount; ++i)
	{
		pMgr = (CShowManager*)m_pInfo->GetData(i);
		pMgr->SetTextSelect(0, 0, -1, -1);
	}
	
	Invalidate(FALSE);
}

void CItemShowWnd::OnSaveImg() 
{
	CShowManager *pMgr = (CShowManager*)m_pInfo->GetData(m_HitInfo.nMgrIndex);
	if (!pMgr)
	{
		return;
	}

	PITEMIMAGE pImage = pMgr->GetHitImage(&m_HitInfo);
	if (!pImage)
	{
		return;
	}

	OPENFILENAME ofn;
	TCHAR strFilePath[MAX_PATH] = {0};
	_tcscpyn(strFilePath, pImage->szImageTitle, MAX_PATH);
	
	CString strTitle;
	strTitle.LoadString(IDS_SHOWWND_SAVETITLE);
	
	UINT nID;
	LPCTSTR pExt;
	switch(pImage->nImageType)
	{
	case ITEMIMAGETYPE_JPEG:
		nID = IDS_SHOWWND_JPEGFILTER;
		pExt = jpeg_lpszImageExt;
		break;

	case ITEMIMAGETYPE_DIB:
		nID = IDS_SHOWWND_BMPFILTER;
		pExt = bmp_lpszImageExt;
		break;

	case ITEMIMAGETYPE_PNG:
		nID = IDS_SHOWWND_PNGFILTER;
		pExt = png_lpszImageExt;
		break;

	default:
		ASSERT(FALSE);
	}
	LPTSTR strFilter = CTextManager::LoadFileFilter(nID);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_JPEGCOVERDLG_OPENFAIL);
		return;
	}
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = strFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = strFilePath;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = strTitle;
	ofn.Flags             = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = jpeg_lpszImageExt;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	
	if (!GetSaveFileName(&ofn))
	{
		free(strFilter);
		return;
	}
	free(strFilter);

	FILE *fp = _tfopen(strFilePath, _T("wb"));
	if (!fp)
	{
		return;
	}

	fwrite(pImage->pImageData, pImage->ulImageSize, 1, fp);
	fclose(fp);
}

void CItemShowWnd::OnUpdateCmdUI(CCmdUI* pCmdUI) 
{
	switch(pCmdUI->m_nID)
	{
	case ID_SHOW_SAVEIMG:
		pCmdUI->Enable(SHOWHITTEST_IMAGE == m_HitInfo.nHitType);
		break;

	default:
		break;
	}
}

void CItemShowWnd::OnNcMouseMove(UINT nHitTest, CPoint point) 
{
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
	
	CWnd::OnNcMouseMove(nHitTest, point);
}
