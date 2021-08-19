// ListFrame.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "ListFrame.h"

#include "EditDlg.h"
#include "ViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SUMMARYLENGTH		60

static LPCTSTR classname = _T("QQListFrame");

/////////////////////////////////////////////////////////////////////////////
// CListFrame

CListFrame::CListFrame(CDialog *pParent) :
	m_pListCtrl(NULL),
	m_pParent(pParent),
	m_pList(NULL), m_nListType(-1)
{
}

CListFrame::~CListFrame()
{
}


BEGIN_MESSAGE_MAP(CListFrame, CWnd)
	//{{AFX_MSG_MAP(CListFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_EDIT_CLEAR, OnClear)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_QLISTNOTIFY, OnQListNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListFrame message handlers

BOOL CListFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// 创建一个没有图标的框架类
	WNDCLASS wndcls;
	ZeroMemory(&wndcls, sizeof(WNDCLASS));
	
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = NULL;
	wndcls.hCursor = afxData.hcurArrow;
	wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = classname;
	
	VERIFY(AfxRegisterClass(&wndcls));
	
	cs.lpszClass = classname;
	cs.dwExStyle |= WS_EX_DLGMODALFRAME;

	// 这里非常之恶心
	// 用Create创建的框架窗口，默认会加上一个非常宽的边框
	// 但是用LoadFrame创建的窗口默认则没有
	// 这是因为LoadFrame使用的是Create而Create用的是CreateEx
	// 这里不使用默认PreCreateWindow，直接返回TRUE
	return TRUE;
}

BOOL CListFrame::CreateFrame(LPCTSTR lpszWindowName,
							 const RECT& rect /* = rectDefault */,
							 CWnd* pParentWnd /* = NULL */,
							 DWORD dwExStyle /* = 0 */)
{
	CString strTitle;
	if (!lpszWindowName)
	{
		strTitle.LoadString(IDS_LISTFRM_TITLE);
	}
	else
	{
		strTitle = lpszWindowName;
	}

	// 禁止点右上角退出
	return CreateEx(dwExStyle, classname, strTitle, WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME, rect, pParentWnd, NULL);
}

int CListFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ASSERT(!m_pListCtrl);
	m_pListCtrl = new CQList;
	if (!m_pListCtrl)
	{
		return -1;
	}

	if (!m_pListCtrl->Create(WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		return -1;
	}
	
	return 0;
}

void CListFrame::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RECT rc;
	GetClientRect(&rc);
	m_pListCtrl->MoveWindow(&rc, TRUE);
}

void CListFrame::OnSetFocus(CWnd* pOldWnd) 
{
	ASSERT(m_pListCtrl);
	m_pListCtrl->SetFocus();
}

void CListFrame::PostNcDestroy() 
{
	CWnd::PostNcDestroy();

	// 没有自行清理的功能，需要手动清理
	if (IsWindow(m_pListCtrl->GetSafeHwnd()))
	{
		m_pListCtrl->DestroyWindow();
	}
	if (m_pListCtrl)
	{
		delete m_pListCtrl;
	}
	
	delete this;
}

BOOL CListFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (m_pListCtrl->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CListFrame::SetWndPos(HWND hWnd)
{
	CRect Wnd;
	::GetWindowRect(hWnd, Wnd);

	SetWindowPos(NULL, Wnd.right + 3, Wnd.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CListFrame::SetList(CList *pList, int nListType, int nSel)
{
	if (!IsWindow(m_pListCtrl->GetSafeHwnd()))
	{
		return;
	}

	m_pList = pList;
	m_nListType = nListType;
	m_pListCtrl->SetList(pList, nListType, nSel);
}

void CListFrame::UpdateList(int nSel /* = -1 */)
{
	m_pListCtrl->SetList(m_pList, m_nListType, nSel);
}

void CListFrame::RefreshList()
{
	m_pListCtrl->Invalidate();
}

void CListFrame::SetCurSel(int nIndex)
{
	m_pListCtrl->SetCurSel(nIndex);
}

void CListFrame::OnClear()
{
	ASSERT(m_nListType == LISTFRM_TYPE_EDIT);

	CTMessageBox Box;
	if (IDOK != Box.SuperMessageBox(m_hWnd, IDS_QVIEW_DELASK, IDS_MSGCAPTION_ASK,
		MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	int nCurSelIndex = ((CEditDlg*)m_pParent)->GetCurSel();
	CItem *pItem = m_pList->GetItem(nCurSelIndex);

	int nSelIndex = m_pListCtrl->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	while (nSelIndex >= 0)
	{
		if (nCurSelIndex == nSelIndex)
		{
			do
			{
				pItem = pItem->GetPrev();
			} while(pItem && pItem->GetItemState(ITEMMARK_DELETE));
		}
		((CEditDlg*)m_pParent)->Delete(DELTYPE_MARK, nSelIndex);
		
		nSelIndex = m_pListCtrl->GetNextItem(nSelIndex, LVNI_ALL | LVNI_SELECTED);
	}

	((CEditDlg*)m_pParent)->Delete(DELTYPE_DO);
	((CEditDlg*)m_pParent)->SetCurSel(m_pList->GetIndex(pItem));
	m_pListCtrl->Invalidate();
}

LRESULT CListFrame::OnQListNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case QLN_LBUTTONDBLCLK:
		if (m_pParent)
		{
			switch(m_nListType)
			{
			case LISTFRM_TYPE_VIEW:
				((CViewDlg*)m_pParent)->SetCurSel(lParam);
				break;
				
			case LISTFRM_TYPE_EDIT:
				if (!((CEditDlg*)m_pParent)->SetCurSel(lParam))
				{
					SetCurSel(m_nCurShowIndex);
				}
				else
				{
					m_nCurShowIndex = lParam;
				}
				break;
				
			default:
				ASSERT(FALSE);
			}
		}
		break;

	case QLN_RBUTTONUP:
		if (m_nListType == LISTFRM_TYPE_EDIT && m_pListCtrl->GetSelectedCount() > 0)
		{
			CMenu cMenu;
			cMenu.LoadMenu(IDR_LISTMENU);
			CMenu *pDelMenu = cMenu.GetSubMenu(0);
			
			CPoint point(lParam);
			ClientToScreen(&point);
			pDelMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
		break;
	}

	return 0;
}
