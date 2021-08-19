// TestFrame.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "TestFrame.h"

#include "Blank.h"
#include "View.h"
#include "Doc.h"
#include "ShowManager.h"

#include "TestSetDlg.h"
#include "BlkTxtCheckDlg.h"

#include "TestMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TESTFRAME_LVIEWMINSIZE		300
#define TESTFRAME_RVIEWMINSIZE		250
#define TESTFRAME_SPLITTERWIDTH		4

#define TESTFRAMESTEP_TESTING		0
#define TESTFRAMESTEP_VIEWTEST		1

static LPCTSTR classname		= _T("QQTestFrame");
static LPCTSTR lpszTestWindow	= _T("Test Window");
static LPCTSTR lpszX			= _T("x");
static LPCTSTR lpszY			= _T("y");
static LPCTSTR lpszCX			= _T("cx");
static LPCTSTR lpszCY			= _T("cy");
static LPCTSTR lpszFont			= _T("font");

/////////////////////////////////////////////////////////////////////////////
// CTestFrame

CTestFrame::CTestFrame() :
	m_pShow(NULL), m_pList(NULL), m_nShowStep(TESTFRAMESTEP_TESTING),
	m_nSplitterPos(0),
	m_bSplitterMoving(FALSE),
	m_bShowSingle(FALSE), m_nCurShow(0),
	m_bOnlyFalse(FALSE)
{
	ZeroMemory(&m_Para, sizeof(m_Para));
	ZeroMemory(&m_rcClient, sizeof(m_rcClient));
}

CTestFrame::~CTestFrame()
{
	if (m_pShow)
	{
		delete m_pShow;
	}
	if (m_pList)
	{
		delete m_pList;
	}
}

BEGIN_MESSAGE_MAP(CTestFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CTestFrame)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_GETMINMAXINFO()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
	ON_MESSAGE(WM_ITEMSHOWWNDNOTIFY, OnItemShowWndNotify)
	ON_MESSAGE(WM_TESTBARNOTIFY, OnTestBarNotify)
	ON_MESSAGE(WM_QLISTNOTIFY, OnQListNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestFrame message handlers

BOOL CTestFrame::StartTest(CDoc *pDoc)
{
	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);
	
	CTestSetDlg SetDlg(IDS_TEST_TESTSET, pDoc->GetAllPapersList(), pPaper);
	if (IDOK != SetDlg.DoModal())
	{
		return FALSE;
	}

	CTMessageBox Box;
	CTestPara *pCPara = SetDlg.GetTestPara();
	if (!pCPara->GetTotalCount())
	{
		return FALSE;
	}
	memcpy(&m_Para, pCPara->GetTestPara(), sizeof(m_Para));

	if (!m_Para.pPaper->CreateTestList(pCPara, &m_List))
	{
		Box.SuperMessageBox(NULL, IDS_TEST_CREATETESTFAIL, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);
		return FALSE;
	}
	m_SingleList.ClearAllItems(TRUE);
	CTestMaker::MakeAtomList(&m_List, &m_SingleList);

	if (IDYES != Box.SuperMessageBox(NULL, IDS_TEST_BEGINTESTASK, IDS_MSGCAPTION_ASK, MB_YESNO | MB_ICONQUESTION, 0, 0))
	{
		return FALSE;
	}

	CWinApp *pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	
	int x = pApp->GetProfileInt(lpszTestWindow, lpszX, 100);
	int y = pApp->GetProfileInt(lpszTestWindow, lpszY, 100);
	if (x > cx || x < 0)
	{
		x = 100;
	}
	if (y > cy || y < 0)
	{
		y = 100;
	}
	
	int nWidth = pApp->GetProfileInt(lpszTestWindow, lpszCX, cx / 2);
	int nHeight = pApp->GetProfileInt(lpszTestWindow, lpszCY, cy / 3 * 2);
	if (nWidth > cx || nWidth <= 0)
	{
		nWidth = cx / 2;
	}
	if (nHeight > cy || nHeight <= 0)
	{
		nHeight = cy / 2;
	}

	CString string;
	string.Format(IDS_TESTFRAME_TITLE, m_Para.pPaper->GetDescription());
	if (!CreateFrame(string, CRect(x, y, nWidth, nHeight), AfxGetMainWnd(), 0))
	{
		return FALSE;
	}

	int nFont = pApp->GetProfileInt(lpszTestWindow, lpszFont, ISP_NORMALFONT);
	if (nFont < ISP_SMALLFONT || nFont > ISP_GIANTFONT)
	{
		nFont = ISP_NORMALFONT;
	}
	m_cBar.SetFontSize(nFont);
	m_pShow->SetFontSize(nFont, FALSE);

	SetList(TESTFRAMESTEP_TESTING);
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	return TRUE;
}

BOOL CTestFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	// 创建一个没有图标的框架类
	WNDCLASS wndcls = {0};
	
	wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndcls.lpfnWndProc = ::DefWindowProc;
	wndcls.hInstance = AfxGetInstanceHandle();
	wndcls.hIcon = NULL;
	wndcls.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_SIZEWE), IMAGE_CURSOR, 0, 0, LR_SHARED);
	wndcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndcls.lpszMenuName = NULL;
	wndcls.lpszClassName = classname;
	
	VERIFY(AfxRegisterClass(&wndcls));
	
	cs.lpszClass = classname;
	cs.style &= ~WS_CLIPCHILDREN;
	cs.dwExStyle |= (WS_EX_APPWINDOW | WS_EX_DLGMODALFRAME);
	return TRUE;
}

BOOL CTestFrame::CreateFrame(LPCTSTR lpszWindowName,
							 const RECT& rect /* = rectDefault */,
							 CWnd* pParentWnd /* = NULL */,
							 DWORD dwExStyle /* = 0 */)
{
	if (!m_pShow)
	{
		m_pShow = new CItemShowWnd;
		if (!m_pShow)
		{
			return -1;
		}
	}
	if (!m_pShow->RegisterClass(AfxGetInstanceHandle()))
	{
		return -1;
	}
	
	if (!m_pList)
	{
		m_pList = new CQList;
		if (!m_pList)
		{
			return -1;
		}
	}

	return CreateEx(dwExStyle, classname, lpszWindowName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, rect, pParentWnd, NULL);
}

int CTestFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nSplitterPos = lpCreateStruct->cx / 2;
	
	RECT rcClient = {0};
	ASSERT(m_pShow);
	if (!m_pShow->CreateEx(WS_EX_CLIENTEDGE, m_pShow->GetClassName(), NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		return -1;
	}

	if (!m_pList->Create(WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL | LVS_NOSORTHEADER | LVS_SHOWSELALWAYS | WS_CLIPSIBLINGS,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST + 1))
	{
		return -1;
	}
	m_pList->ModifyStyleEx(NULL, WS_EX_CLIENTEDGE, 0);

	if (!m_cBar.Create(this, IDD_TEST_BAR, WS_VISIBLE | WS_CHILD | CBRS_TOP, IDD_TEST_BAR))
	{
		return -1;
	}
	
	ShowWindow(SW_SHOW);
	m_pShow->SetFocus();
	return 0;
}

BOOL CTestFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->wParam == VK_PROCESSKEY)
    {
		m_pShow->CheckImmInput(pMsg);
    }
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

HBRUSH CTestFrame::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	// 首先交给子窗口处理
	LRESULT lResult;
	if (pWnd->SendChildNotifyLastMsg(&lResult))
	{
		return (HBRUSH)lResult;
	}
	
	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		return CWnd::OnCtlColor(pDC, pWnd, nCtlColor);
		
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

void CTestFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);
	
	switch(nType)
	{
	case SIZE_MINIMIZED:
		return;
	}

	RECT rc = {0}, rcBar = {0};
	GetClientRect(&rc);

	m_cBar.GetWindowRect(&rcBar);
	ScreenToClient(&rcBar);

	if (rc.right - rc.left - m_nSplitterPos < TESTFRAME_LVIEWMINSIZE)
	{
		m_nSplitterPos = rc.right - rc.left - TESTFRAME_LVIEWMINSIZE;
	}
	else if (m_nSplitterPos < TESTFRAME_RVIEWMINSIZE)
	{
		m_nSplitterPos = TESTFRAME_RVIEWMINSIZE;
	}

	m_pShow->MoveWindow(
		0, rcBar.bottom,
		rc.right - rc.left - m_nSplitterPos, rc.bottom - rcBar.bottom,
		TRUE);
	m_pList->MoveWindow(
		rc.right - rc.left - m_nSplitterPos + TESTFRAME_SPLITTERWIDTH, rcBar.bottom,
		m_nSplitterPos - TESTFRAME_SPLITTERWIDTH, rc.bottom - rcBar.bottom,
		TRUE);

	switch(nType)
	{
	case SIZE_MAXIMIZED:
	case SIZE_MAXSHOW:
		m_pShow->RefreshShowInfo();
		break;
		
	default:
		break;
	}
}

void CTestFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);

	if (SC_RESTORE == nID)
	{
		m_pShow->RefreshShowInfo();
	}
}

void CTestFrame::OnSetFocus(CWnd* pOldWnd)
{
	m_pShow->SetFocus();
}

void CTestFrame::SetList(int nShowStep)
{
	m_nShowStep = nShowStep;
	m_nCurShow = 0;

	CList *pList = GetList();
	if (TESTFRAMESTEP_TESTING == m_nShowStep)
	{
		m_pList->SetList(&m_List, LISTFRM_TYPE_TESTING, 0);
		m_pShow->SetShowType(SHOWWNDTYPE_TEST);

		m_cBar.SetType(TESTBARTYPE_TEST);
		m_cBar.SetTime(m_Para.nTime * 60);
	}
	else
	{
		m_pList->SetList(m_bOnlyFalse ? &m_WrongList : &m_List, LISTFRM_TYPE_REVIEW, 0);
		m_pShow->SetShowType(SHOWWNDTYPE_TESTVIEW);

		m_cBar.SetType(TESTBARTYPE_TESTVIEW);
		m_cBar.UpdateList(m_WrongList.GetItemCount());
	}
	
	m_pShow->SetList(pList);
	if (m_bShowSingle)
	{
		m_pShow->ShowItem(m_nCurShow, GetShowStyle());
	}
	else
	{
		m_pShow->ShowItem(-1, GetShowStyle());
	}
}

void CTestFrame::MarkItem(CItem *pItem)
{
	if (!pItem || pItem->GetType() == TYPE_GROUP)
	{
		return;
	}

	if (pItem->GetItemState(ITEMMARK_UNCERTAIN))
	{
		pItem->SetItemState(ITEMMARK_NULL, ITEMMARK_UNCERTAIN);
	}
	else
	{
		pItem->SetItemState(ITEMMARK_UNCERTAIN, ITEMMARK_UNCERTAIN);
	}

	m_pShow->UpdateShowInfo();
	m_pList->Invalidate(TRUE);
}

void CTestFrame::StoreItem(CItem *pItem)
{
	if (!pItem)
	{
		return;
	}

	pItem = pItem->GetItemFrom();
	pItem->SetStore(!pItem->IsStored());

	m_pShow->UpdateShowInfo();
	m_pList->Invalidate(TRUE);
}

void CTestFrame::OnClose() 
{
	CTMessageBox Box;
	if (TESTFRAMESTEP_TESTING == m_nShowStep)
	{
		if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_TESTFRAME_CANCEL, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
		{
			return;
		}
	}
	else
	{
		if (IDYES == Box.SuperMessageBox(NULL, IDS_TEST_RETESTASK, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONINFORMATION | MB_DEFBUTTON2, 0, 0))
		{
			m_List.ClearUserInfo();
			m_List.Random(0, 0, 0);

			m_SingleList.ClearAllItems(TRUE);
			CTestMaker::MakeAtomList(&m_List, &m_SingleList);

			CString string;
			string.Format(IDS_TESTFRAME_TITLE, m_Para.pPaper->GetDescription());
			SetWindowText(string);
			
			SetList(TESTFRAMESTEP_TESTING);
			return;
		}
	}

	if (!IsIconic() && !IsZoomed())
	{
		RECT rc = {0};
		GetWindowRect(&rc);
		
		CWinApp *pApp = AfxGetApp();
		pApp->WriteProfileInt(lpszTestWindow, lpszX, rc.left);
		pApp->WriteProfileInt(lpszTestWindow, lpszY, rc.top);
		pApp->WriteProfileInt(lpszTestWindow, lpszCX, rc.right);
		pApp->WriteProfileInt(lpszTestWindow, lpszCY, rc.bottom);
		pApp->WriteProfileInt(lpszTestWindow, lpszFont, m_pShow->GetFontSize());
	}
	
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
	CFrameWnd::OnClose();
}

void CTestFrame::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	GetClientRect(&m_rcClient);

	CWnd::OnNcLButtonDown(nHitTest, point);
}

LRESULT CTestFrame::OnExitSizeMove(WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	GetClientRect(&rc);
	
	if (rc.bottom - rc.top != m_rcClient.bottom - m_rcClient.top ||
		rc.right - rc.left != m_rcClient.right - m_rcClient.left)
	{
		m_pShow->RefreshShowInfo();
	}

	return Default();
}

void CTestFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	lpMMI->ptMinTrackSize.x = 640;
    lpMMI->ptMinTrackSize.y = 480;
	
	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CTestFrame::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ModifyStyle(WS_CLIPCHILDREN, 0, 0);
	
	m_bSplitterMoving = TRUE;
	SetCapture();

	RECT rcTracker = {0}, rcBar = {0};
	GetClientRect(&rcTracker);

	m_cBar.GetWindowRect(&rcBar);
	ScreenToClient(&rcBar);

	rcTracker.top = rcBar.bottom;
	rcTracker.left = rcTracker.right - rcTracker.left - m_nSplitterPos;
	rcTracker.right = rcTracker.left + TESTFRAME_SPLITTERWIDTH;
	InvertTracker(&rcTracker);
	
	CFrameWnd::OnLButtonDown(nFlags, point);
}

void CTestFrame::OnLButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	m_bSplitterMoving = FALSE;

	ModifyStyle(0, WS_CLIPCHILDREN, 0);
	
	RECT rc = {0};
	GetClientRect(&rc);
	SendMessage(WM_SIZE, SIZE_RESTORED, MAKELPARAM(rc.right, rc.bottom));

	m_pShow->RefreshShowInfo();
	Invalidate(FALSE);

	CFrameWnd::OnLButtonUp(nFlags, point);
}

void CTestFrame::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bSplitterMoving)
	{
		RECT rc = {0}, rcBar = {0}, rcTracker = {0};
		GetClientRect(&rc);

		m_cBar.GetWindowRect(&rcBar);
		ScreenToClient(&rcBar);

		rcTracker.left = rc.right - rc.left - m_nSplitterPos;
		rcTracker.top = rcBar.bottom;
		rcTracker.right = rcTracker.left + TESTFRAME_SPLITTERWIDTH;
		rcTracker.bottom = rc.bottom;
		InvertTracker(&rcTracker);

		if (point.x < TESTFRAME_LVIEWMINSIZE)
		{
			m_nSplitterPos = rc.right - rc.left - TESTFRAME_LVIEWMINSIZE;
		}
		else if (point.x > rc.right - TESTFRAME_RVIEWMINSIZE)
		{
			m_nSplitterPos = TESTFRAME_RVIEWMINSIZE;
		}
		else
		{
			m_nSplitterPos = rc.right - rc.left - point.x;
		}

		rcTracker.left = rc.right - rc.left - m_nSplitterPos;
		rcTracker.right = rcTracker.left + TESTFRAME_SPLITTERWIDTH;
		InvertTracker(&rcTracker);
	}
	else
	{
		CFrameWnd::OnMouseMove(nFlags, point);
	}
}

void CTestFrame::InvertTracker(LPRECT rc)
{
	HDC hDC = ::GetDC(GetSafeHwnd());
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, CDC::GetHalftoneBrush()->GetSafeHandle());
	PatBlt(hDC, rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, PATINVERT);
	if (hOldBrush)
	{
		SelectObject(hDC, hOldBrush);
	}
	::ReleaseDC(GetSafeHwnd(), hDC);
}

BOOL CTestFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (m_cBar.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

LRESULT CTestFrame::OnItemShowWndNotify(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case ISN_USERSET:
		m_pList->Invalidate(TRUE);
		break;
		
	case ISN_STOREITEM:
		MarkItem((CItem*)lParam);
		break;
		
	case ISN_SELECTITEM:
		m_cBar.UpdateState((CItem*)lParam);
		m_pList->SetCurSel((CItem*)lParam);
		break;
		
	case ISN_PRE:
	case ISN_NEXT:
		ASSERT(m_bShowSingle);
		m_nCurShow = lParam;
		m_pList->SetCurSel(m_nCurShow);
		break;
	}
	
	return 0;
}

LRESULT CTestFrame::OnTestBarNotify(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	CList* pList = m_pShow->GetList();

	switch(wParam)
	{
	case TBN_TIMEOUT:
		Score(&m_List, &m_Para);
		break;

	case TBN_USERHANDIN:
		{
			CTMessageBox Box;
			CItem *pItem = m_List.GetFirstUnAnswerd();
			if (pItem)
			{
				if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_TESTFRAME_NOTALLDONEASK, IDS_MSGCAPTION_ASK,
					MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
				{
					m_pShow->SetItemSelect(pItem, TRUE);
					return FALSE;
				}
			}
			else
			{
				if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_TESTFRAME_DONEASK, IDS_MSGCAPTION_ASK,
					MB_YESNO | MB_ICONQUESTION, 0, 0))
				{
					return FALSE;
				}
			}

			Score(&m_List, &m_Para);
			return TRUE;
		}
		break;

	case TBN_MARKITEM:
		MarkItem(m_pShow->GetSelection());
		m_cBar.UpdateState(m_pShow->GetSelection());
		m_pShow->UpdateShowInfo();
		m_pList->Invalidate(TRUE);
		break;

	case TBN_TOPMOST:
		if (lParam)
		{
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		break;

	case TBN_SETFONTSIZE:
		{
			long lFontSize;
			switch(lParam)
			{
			case TBP_SMALLFONT:
				lFontSize = ISP_SMALLFONT;
				break;

			case TBP_NORMALFONT:
			default:
				lFontSize = ISP_NORMALFONT;
				break;

			case TBP_LARGEFONT:
				lFontSize = ISP_LARGEFONT;
				break;
				
			case TBP_GIANTFONT:
				lFontSize = ISP_GIANTFONT;
				break;
			}
			m_pShow->SetFontSize(lFontSize, TRUE);
			m_pShow->SetFocus();
		}
		break;

	case TBN_SAVEITEM:
		StoreItem(m_pShow->GetSelection());
		break;

	case TBN_ONLYFALSE:
		OnlyFalse(lParam);
		break;

	case TBN_SHOWSINGLE:
		ShowSingle(lParam);
		break;

	case TBN_PRE:
		ASSERT(m_bShowSingle);
		--m_nCurShow;
		if (m_nCurShow < 0)
		{
			m_nCurShow = pList->GetItemCount() - 1;
		}
		m_pShow->ShowItem(m_nCurShow, GetShowStyle());
		m_pList->SetCurSel(m_nCurShow);
		break;

	case TBN_NEXT:
		ASSERT(m_bShowSingle);
		++m_nCurShow;
		if (m_nCurShow >= pList->GetItemCount())
		{
			m_nCurShow = 0;
		}
		m_pShow->ShowItem(m_nCurShow, GetShowStyle());
		m_pList->SetCurSel(m_nCurShow);
		break;
	}

	m_pShow->SetFocus();
	return lRet;
}

LRESULT CTestFrame::OnQListNotify(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	switch(wParam)
	{
	case QLN_LBUTTONDBLCLK:
		if (m_bShowSingle)
		{
			CList *pList = m_pShow->GetList();

			m_nCurShow = lParam;
			m_pShow->ShowItem(m_nCurShow, GetShowStyle());
		}
		else
		{
			CItem *pItem = m_pList->GetItem(lParam);

			m_pShow->ScrollToItem(pItem, 0);
			m_pShow->SetItemSelect(pItem, TRUE);
		}
		m_pShow->SetFocus();
		break;
	}

	return 0;
}

void CTestFrame::OnlyFalse(BOOL bOnlyFalse)
{
	// 只有在题目浏览时才能到这里
	if (m_bOnlyFalse == bOnlyFalse)
	{
		return;
	}
	m_bOnlyFalse = bOnlyFalse;

	// 此时只显示错题
	if (m_bOnlyFalse)
	{
		m_pList->SetList(&m_WrongList, LISTFRM_TYPE_REVIEW, 0);
		if (m_bShowSingle)
		{
			CItem *pItem = m_SingleList.GetItem(m_nCurShow);

			m_pShow->SetList(&m_WrongSingleList);
			m_nCurShow = m_WrongSingleList.GetIndex(pItem);
			if (m_nCurShow < 0)
			{
				m_nCurShow = 0;
			}
			m_pShow->ShowItem(m_nCurShow, GetShowStyle());
			m_pList->SetCurSel(m_nCurShow);
		}
		else
		{
			CItem *pItem = m_pShow->GetSelection();
			if (m_WrongList.GetIndex(pItem) < 0)
			{
				pItem = m_WrongList.GetItem(0);
			}

			m_pShow->SetList(&m_WrongList);
			m_pShow->ShowItem(-1, GetShowStyle());
			m_pShow->ScrollToItem(pItem, 0);
			m_pShow->SetItemSelect(pItem, TRUE);
			m_pList->SetCurSel(pItem);
		}
	}
	else
	{
		m_pList->SetList(&m_List, LISTFRM_TYPE_REVIEW, 0);
		if (m_bShowSingle)
		{
			CItem *pItem = m_WrongSingleList.GetItem(m_nCurShow);
			
			m_pShow->SetList(&m_SingleList);
			m_nCurShow = m_SingleList.GetIndex(pItem);
			if (m_nCurShow < 0)
			{
				m_nCurShow = 0;
			}
			m_pShow->ShowItem(m_nCurShow, GetShowStyle());
			m_pList->SetCurSel(m_nCurShow);
		}
		else
		{
			CItem *pItem = m_pShow->GetSelection();
			
			m_pShow->SetList(&m_List);
			m_pShow->ShowItem(-1, GetShowStyle());
			m_pShow->ScrollToItem(pItem, 0);
			m_pShow->SetItemSelect(pItem, TRUE);
			m_pList->SetCurSel(pItem);
		}
	}

	m_pShow->SetFocus();
}

void CTestFrame::ShowSingle(BOOL bShowSingle)
{
	// 显示单个题目不会影响列表窗口的内容
	if (m_bShowSingle == bShowSingle)
	{
		return;
	}
	m_bShowSingle = bShowSingle;

	// 由多显变为单显
	// 需要获得此时选中的题目
	if (m_bShowSingle)
	{
		CItem *pItem = m_pShow->GetSelection();
		CList *pList;

		if (TESTFRAMESTEP_VIEWTEST == m_nShowStep && m_bOnlyFalse)
		{
			pList = &m_WrongSingleList;
		}
		else
		{
			pList = &m_SingleList;
		}

		m_pShow->SetList(pList);
		m_nCurShow = pList->GetIndex(pItem);
		if (m_nCurShow < 0)
		{
			m_nCurShow = 0;
		}

		m_pShow->ShowItem(m_nCurShow, GetShowStyle());
		m_pList->SetCurSel(m_nCurShow);
	}
	// 由单显变为多显
	// m_nCurShow必然有内容
	else
	{
		CItem *pItem;
		CList *pList;
		if (TESTFRAMESTEP_VIEWTEST == m_nShowStep && m_bOnlyFalse)
		{
			pList = &m_WrongList;
		}
		else
		{
			pList = &m_List;
		}

		m_pShow->SetList(pList);
		m_pShow->ShowItem(-1, GetShowStyle());

		pItem = m_pList->GetItem(m_nCurShow);
		m_pShow->ScrollToItem(pItem, 0);
		m_pShow->SetItemSelect(pItem, TRUE);
		m_pList->SetCurSel(m_nCurShow);
	}

	m_pShow->SetFocus();
}

DWORD CTestFrame::GetShowStyle()
{
	switch(m_nShowStep)
	{
	case TESTFRAMESTEP_TESTING:
		return SHOWSTYLE_WITHINDEX | SHOWSTYLE_WITHUSERANSWER;

	case TESTFRAMESTEP_VIEWTEST:
		return SHOWSTYLE_WITHINDEX | SHOWSTYLE_WITHUSERANSWER | SHOWSTYLE_WITHANSWER | SHOWSTYLE_WITHRESOLVE | SHOWSTYLE_WITHSTORE;

	default:
		return 0;
	}
}

CList* CTestFrame::GetList()
{
	if (m_bShowSingle)
	{
		if (m_bOnlyFalse)
		{
			return &m_WrongSingleList;
		}
		else
		{
			return &m_SingleList;
		}
	}
	else
	{
		if (m_bOnlyFalse)
		{
			return &m_WrongList;
		}
		else
		{
			return &m_List;
		}
	}
}

void CTestFrame::Score(CList *pList, PTESTPARA pPara)
{
	int nAll, nGet;
	CTestPara Para(pPara);
	int i = 0;
	CTMessageBox Box;

	ShowWindow(SW_HIDE);
	
	CItem *pItem = pList->GetItem(i);
	while(pItem)
	{
		if (pItem->GetType() == TYPE_BLANK &&
			(!(pPara->dwBlkOptions & BLKOPTION_ALLRIGHT)))
		{
			break;
		}
		else if (pItem->GetType() == TYPE_TEXT &&
			(!(pPara->dwTxtOptions & TXTOPTION_ALLRIGHT)))
		{
			break;
		}
		
		++i;
		pItem = pList->GetItem(i);
	}
	// 如果有主观题，而且不要求全部正确
	if (pItem)
	{
		Box.SuperMessageBox(NULL, IDS_TESTFRAME_BLKTXT, IDS_MSGCAPTION_INFOR,
			MB_OK | MB_ICONINFORMATION, 0, 0);
		
		CBlkTxtCheckDlg Dlg(pList, &Para);
		Dlg.DoModal();
	}
	
	if (MARKTYPE_AVERAGE == pPara->nMarkType)
	{
		nGet = 0;
		
		// 评分按全对、半对、不对来区分
		// 总分为题目总数×100，为了照顾百分制简答题
		nAll = Para.GetTotalAtomCount(pList) * 100;
		for (i = 0; i < pList->GetItemCount(); ++i)
		{
			CItem *pItem = pList->GetItem(i);
			nGet += pItem->Score(&Para);
		}
		
		pPara->fScoreR = ((float)nGet / nAll) * pPara->nFullMark;
	}
	else
	{
		nGet = nAll = 0;
		
		// 评分按全对、半对、不对来区分
		// 总分为设定总分×2，为了照顾半对
		for (i = 0; i < pList->GetItemCount(); ++i)
		{
			CItem *pItem = pList->GetItem(i);
			switch(pItem->GetType())
			{
			case TYPE_SCHOISE:
				nAll += pPara->nSChPoints * 2;
				break;
				
			case TYPE_MCHOISE:
				nAll += pPara->nMChPoints * 2;
				break;
				
			case TYPE_JUDGE:
				nAll += pPara->nJdgPoints * 2;
				break;
				
			case TYPE_BLANK:
				nAll += pPara->nBlkPoints * ((CBlank*)pItem)->GetBlankCount() * 2;
				break;
				
			case TYPE_TEXT:
				nAll += pPara->nTxtPoints * 2;
				break;

			case TYPE_GROUP:
				nAll += ((CItemGroup*)pItem)->GetFullMark(&Para) * 2;
				break;
			}
			nGet += pItem->Score(&Para);
		}
		
		pPara->fScoreR = nGet * 0.5;
	}
	
	pPara->fScoreH = (nGet * 100.f) / nAll;

	// 填充错误列表
	m_WrongList.ClearAllItems(TRUE);
	for (i = 0; i < m_List.GetItemCount(); ++i)
	{
		pItem = m_List.GetItem(i);
		if (pItem->CheckAnswer() != ALLRIGHT)
		{
			m_WrongList.AddItem(pItem);
		}
	}

	m_WrongSingleList.ClearAllItems(TRUE);
	CTestMaker::MakeAtomList(&m_WrongList, &m_WrongSingleList);

	Box.SuperMessageBox(NULL, IDS_TEST_SHOWSCORE, IDS_MSGCAPTION_RESULT, MB_OK | MB_ICONINFORMATION, 0, 0,
		pPara->fScoreR, pPara->fScoreH);

	CString string;
	string.Format(IDS_TESTVIEW_TITLE, pPara->fScoreR, pPara->fScoreH);
	SetWindowText(string);

	SetList(TESTFRAMESTEP_VIEWTEST);
	ShowWindow(SW_SHOW);
}
