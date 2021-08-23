// ItemShowWnd.cpp : implementation file
//

/*
## ������Ϣ

	��Ŀ��ʾ����

	���ʱ�䣺2021/06/13

	�޶�����������

	���ߣ��Ͻ���ʿ

## �������

	1�����ո���Ŀ�ĸ�ʽ��ʾ��Ŀ����������Ŀչʾ�Ϳ��ԣ�����Ŀչʾ������ѡ�к͸�������

	2�����Ըı���ʾ����Ĵ�С

	3���������û���ק���ڴ�С��������ק��ɺ�Ž������²��֣������ʾЧ��

## ����˵��

	�����ڰ�������ṹ��

	��һ�㣺���ڱ�������������ʾ��Դ����������������ڵ�ͨ�š�

	�ڶ��㣺ShowManager������������ÿһ����Ŀ����Ӧһ����������������Ŀ�飬��Ŀ��������Ӧһ��
��������ÿ������Ŀ��Ӧһ������������

	�����㣺ÿ���������ж���飨Block����ÿ�����ڰ���һ�����������ֻ�һ��ͼƬ��������ָ������
��ͼƬ��ָ�룬ͬʱ��������ʾ��Ϣ�����壬��������ʾ��������Ϣ�ȣ���

	ÿ�����ֿ��ڶ������˻�����Ϣ�������ڵ������֣�����һ��������Ϣ��������û�л�����Ϣ����Ȼ
�ᱻʶ��ΪͼƬ����

	�������ṩ��HitTest���ܣ����Է��������λ�ö�Ӧ�Ĺ�������ţ�����ţ������ͣ����������
��ŵ���Ϣ������Ҫ�ر�ע����ǡ��������λ����Ŀ���ĳ��λ��ʱ�����ٶ��������Ŀ���һ�е�ĳ
��λ�ã�������Ϣ������ٶ����أ��������Ϣ�᷵�ء�SHOWHITTEST_NONE���������ô���Ϣ�жϷ��ص�
�����Ϣ�Ƿ���Ч��

	����HitTest�Ƕ�������Ŀ�Ĺ��������ν��в��ԣ��䷵��ֵ�������ָ��ǰ��Ŀ��ǰ���Ǻ����
�������λ��ĳ����Ŀ����������в��ԣ������ָ��ĳ����Ŀǰ������ζ�Ų���Ҫ�ټ�������������
�ˡ�

## �޶���¼

	��
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
	// ����һ��û��ͼ��Ŀ����
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
���ܣ�	��ʾ��ǰ�б��е�����
����ֵ����
��ע��	��nIndexС��0ʱ����ʾ���е�����
		nStep���ڿ���ѧϰģʽ�µ�����ʾ����nIndexС��0ʱ��Ч
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

	// ��������ֻ�ڿ��Խ���������
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

	// ����Ҫ�ƶ�
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

	// �򸸴���������ɫ
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
���ܣ�	�趨��Ŀ����ʾ����
����ֵ����
��ע��	
*/
void CItemShowWnd::SetShowInfo(HDC hDC)
{
	CShowManager *pMgr;
	long lyOffset = 0, lWidth, lHeight; // ���ܸ߶ȳ�������ʾ�߶ȣ�����ʾ������
	CItem *pItem;
	int i;
	int nIndex; // ��Ŀ���
	int nMgrIndex; // ���������

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

		// ����Ŀ������Ŀ��ʱ����Ҫ����Ƿ�Ϊ��Ŀ�������Ŀ
		if (pItem->GetType() != TYPE_GROUP)
		{
			pParent = pItem->GetItemFrom()->GetParent();
		}

		// ����Ŀ�������Ŀ
		// ��ifΪ�����������������ֽ������
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

		// �������Ŀ�飬�˶�Ϊ��Ŀ������
		// �����һ����Ŀ���˶�Ϊ��Ŀ����
		pMgr = GetShowManager(nMgrIndex, pItem);
		if (!pMgr)
		{
			return;
		}
		lHeight = pMgr->SetItemShow(hDC, lyOffset, lWidth, m_dwShowStyle, nIndex, m_nShowStep);
		m_pInfo->SetData(nMgrIndex++, pMgr);
		lyOffset += lHeight;

		// �������Ŀ�飬�����Ŀ�������Ŀ
		// ��ifΪ���Խ��桢���Խ���Ͳ鿴�𰸽������
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
			// ���¼���
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

	// ���ù������Ĳ���
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
���ܣ�	���û���������󣬸�����ʾ
����ֵ����
��ע��	nStart����Ϊ�ܵ�Ӱ�����ʾ���������
*/
void CItemShowWnd::UpdateShowInfo(HDC hDC, int nStart)
{
	CShowManager *pMgr;
	long lyOffset, lWidth, lHeight; // ���ܸ߶ȳ�������ʾ�߶ȣ�����ʾ������
	long lHoleHeight, lNewHeight;
	int i;
	DWORD dwShow = m_dwShowStyle;
	SCROLLINFO si = {0};

	// ��ȡ�ͻ������ܿ��
	GetClientRect(m_rcClient);
	lWidth = m_rcClient.Width();

	// ����Ƿ��й������ı仯
	pMgr = (CShowManager*)m_pInfo->GetData(nStart);
	lHeight = pMgr->GetShowHeight();
	lyOffset = pMgr->GetYOffset();

	// �����µĸ߶�
	lNewHeight = pMgr->RecalcItemShow(hDC, lyOffset, lWidth);
	lyOffset += lNewHeight + pMgr->GetMetrix(SHOWMETRIX_QUESTIONGAP);

	// �����޸�ǰ����ܸ߶�
	lHoleHeight = m_lTotalHeight - lHeight + lNewHeight;

	// ��������������˱仯������Ҫ���²���������ʾ
	if ((m_lTotalHeight <= m_rcClient.Height() && lHoleHeight > m_rcClient.Height()) ||
		(m_lTotalHeight > m_rcClient.Height() && lHoleHeight <= m_rcClient.Height()))
	{
		RefreshShowInfo();
		return;
	}
	// ���������û�з����仯�������¼���������ݵ�Yƫ����
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

	// ���ù������Ĳ���
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
				// ������ģʽʱ������ʾ����
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
���ܣ�	�����ָ���λ�ý��в���
����ֵ�����Խ��
��ע��	��pHit��ΪNULLʱ����Ӱ�����²�����
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

	// ��ֻ��һ����Ŀʱ���������Ŀ�·��հ��ƶ�ʱ
	// Ҳ��Ϊ����Ŀ���ƶ�������û��Ŀ��
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

	// ��Ŀ����ת��ֻ���������ܣ�
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
	
	// ��Ŀ����ת��ֻ���������ܣ�
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

	// ��Ŀ����ת
	if (m_nShowIndex >= 0)
	{
		// Ϊ����ģʽʱ����Ŀ�б�Ϊ����Ŀ�б�����������Ŀ��
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
		// ����ģʽʱ��ֻ��ͨ����ʾ��Ϣ��ת
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

	// �������е�ѡ����Ϣ
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
	
	// �������е�ѡ����Ϣ
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
