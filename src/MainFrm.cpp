// MainFrm.cpp : implementation of the CMain class
//

#include "stdafx.h"
#include "QuickQuiz.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "View.h"
#include "Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry entry names:
static LPCTSTR lpszMainWindow	= _T("Main Window");
static LPCTSTR lpszX			= _T("x");
static LPCTSTR lpszY			= _T("y");
static LPCTSTR lpszCX			= _T("cx");
static LPCTSTR lpszCY			= _T("cy");
static LPCTSTR lpszState		= _T("State");
static LPCTSTR lpszLeft			= _T("Left Pane");
static LPCTSTR lpszColor		= _T("Color");
static LPCTSTR lpszToolBar		= _T("Tool Bar");
static LPCTSTR lpszStatusBar	= _T("Status Bar");
static LPCTSTR lpszShowFont		= _T("Show Font");

/////////////////////////////////////////////////////////////////////////////
// CMain

IMPLEMENT_DYNCREATE(CMain, CFrameWnd)

BEGIN_MESSAGE_MAP(CMain, CFrameWnd)
	//{{AFX_MSG_MAP(CMain)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateCmdUI)
	ON_COMMAND(ID_VIEW_COLORNORMAL, OnColorNormal)
	ON_COMMAND(ID_VIEW_COLORGREEN, OnColorGreen)
	ON_COMMAND(ID_VIEW_COLORDARK, OnColorDark)
	ON_COMMAND(ID_VIEW_RESET, OnReset)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_ALL, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEARMRU, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_EDITABLE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_NEWQUESTION, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_NEWGROUP, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_TEST_VIEW, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_TEST_MAKE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_TEST_COMPETITIVE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORNORMAL, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORGREEN, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLORDARK, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESET, OnUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_ITEMCOUNT,
};

/////////////////////////////////////////////////////////////////////////////
// CMain construction/destruction

CMain::CMain() :
	m_bLoaded(FALSE), m_pEditWnd(NULL)
{
	// TODO: add member initialization code here
}

CMain::~CMain()
{
}

int CMain::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//设置查找标记。
	::SetProp(m_hWnd, g_szPropName, (HANDLE)1);

	return 0;
}

BOOL CMain::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	CRect rect;
	GetClientRect(rect);

	// create splitter window
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLView), CSize(rect.right / 5, rect.bottom), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CQView), CSize(rect.right / 5 * 4, rect.bottom), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	GetPaperView()->UpdateColor();
	GetItemView()->UpdateColor();

	return TRUE;
}

BOOL CMain::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	
	cs.style &= ~FWS_ADDTOTITLE ;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMain diagnostics

#ifdef _DEBUG
void CMain::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMain::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMain message handlers

CQView* CMain::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	CQView* pView = DYNAMIC_DOWNCAST(CQView, pWnd);
	return pView;
}

void CMain::LoadSettings()
{
	CWinApp *pApp = AfxGetApp();
	ASSERT(pApp != NULL);
	
	CString strSection = lpszMainWindow;
	
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	
	int x = pApp->GetProfileInt(strSection, lpszX, 10);
	int y = pApp->GetProfileInt(strSection, lpszY, 10);
	if (x > cx || x < 0)
	{
		x = 10;
	}
	if (y > cy || y < 0)
	{
		y = 10;
	}
	
	int nWidth = pApp->GetProfileInt(strSection, lpszCX, cx / 3 * 2);
	int nHeight = pApp->GetProfileInt(strSection, lpszCY, cy / 3 * 2);
	if (nWidth > cx || nWidth <= 0)
	{
		nWidth = cx / 2;
	}
	if (nHeight > cy || nHeight <= 0)
	{
		nHeight = cy / 2;
	}
	
	int nState = pApp->GetProfileInt(strSection, lpszState, 0) % 2;
	
	m_wndSplitter.SetColumnInfo(0, pApp->GetProfileInt(strSection, lpszLeft, nWidth / 4), 10);

	g_nColorType = pApp->GetProfileInt(strSection, lpszColor, CLRTYPE_DEFAULT);
	GetPaperView()->UpdateColor();
	GetItemView()->UpdateColor();

	ShowControlBar(&m_wndToolBar, pApp->GetProfileInt(strSection, lpszToolBar, 1), FALSE);
	ShowControlBar(&m_wndStatusBar, pApp->GetProfileInt(strSection, lpszStatusBar, 1), FALSE);

	g_nShowFont = pApp->GetProfileInt(strSection, lpszShowFont, SW_NORMALFONT);
	
	if (nState == 1)
	{
		ShowWindow(SW_SHOWMAXIMIZED);
	}
	else
	{
		MoveWindow (x, y, nWidth, nHeight);
		ShowWindow(SW_SHOWNORMAL);
	}
}

void CMain::SaveSettings()
{
	if (!g_bReset)
	{
		CWinApp *pApp = AfxGetApp();
		ASSERT(pApp != NULL);
		
		CString strSection = lpszMainWindow;
		
		if (IsZoomed())
		{
			pApp->WriteProfileInt(strSection, lpszState, 1);
		}
		else
		{
			RECT rc;
			GetWindowRect (&rc);
			
			pApp->WriteProfileInt(strSection, lpszX, rc.left);
			pApp->WriteProfileInt(strSection, lpszY, rc.top);
			
			pApp->WriteProfileInt(strSection, lpszCX, rc.right - rc.left);
			pApp->WriteProfileInt(strSection, lpszCY, rc.bottom - rc.top);
			
			pApp->WriteProfileInt(strSection, lpszState, 0);
		}
		
		pApp->WriteProfileInt(strSection, lpszColor, g_nColorType);
		
		pApp->WriteProfileInt(strSection, lpszToolBar, m_wndToolBar.IsWindowVisible());
		pApp->WriteProfileInt(strSection, lpszStatusBar, m_wndStatusBar.IsWindowVisible());

		pApp->WriteProfileInt(strSection, lpszShowFont, g_nShowFont);
		
		int cxCol;
		int cxColMin;
		m_wndSplitter.GetColumnInfo(0, cxCol, cxColMin);
		pApp->WriteProfileInt(strSection, lpszLeft, cxCol);
	}
}

void CMain::ActivateFrame(int nCmdShow) 
{
	if (!m_bLoaded)
	{
		LoadSettings();
		m_bLoaded = true;
	}

	CFrameWnd::ActivateFrame(-1);
}

void CMain::OnClose() 
{
	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->CheckSaveable(FALSE);
	}

	CTMessageBox Box;
	if (IDOK != Box.SuperMessageBox(m_hWnd, IDS_MAIN_QUIT, IDS_MSGCAPTION_ASK,
		MB_OKCANCEL | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	CDoc *pDoc = (CDoc*)GetActiveDocument();
	if (pDoc)
	{
		// 在保存时选中了取消
		if (!pDoc->SaveAllPapers(TRUE))
		{
			return;
		}
	}

	//删除所设置的标记。
	::RemoveProp(m_hWnd, g_szPropName);
	
	SaveSettings();
	CFrameWnd::OnClose();
}

BOOL CMain::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (CFrameWnd::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	CDoc *pDoc = (CDoc *) GetActiveDocument ();
	if (pDoc != NULL)
	{
		if (pDoc->RouteCmdMsg (GetActiveView (), nID, nCode, pExtra, pHandlerInfo))
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CMain::OnUpdateCmdUI(CCmdUI* pCmdUI) 
{
	CDoc *pDoc = (CDoc*)GetActiveDocument();
	ASSERT(pDoc);
	
	switch(pCmdUI->m_nID)
	{
	case ID_FILE_SAVE:
		{
			CPaper *pPaper = pDoc->GetSelectedPaper();
			if (pPaper)
			{
				LPCTSTR str = pPaper->GetFilePath();
				pCmdUI->Enable(pPaper->IsModify() || !str);
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
		break;

	case ID_FILE_SAVE_ALL:
		pCmdUI->Enable(pDoc->GetPaperCount());
		break;

	case ID_FILE_SAVE_AS:
	case ID_FILE_CLOSE:
		pCmdUI->Enable(pDoc->GetSelectedPaper() ? TRUE : FALSE);
		break;

	case ID_FILE_MRU_FILE1:
		if (pCmdUI->m_pMenu)
		{
			((CApp*)AfxGetApp())->OnUpdateMRUFile(pCmdUI);
		}
		break;

	case ID_FILE_CLEARMRU:
		pCmdUI->Enable(((CApp*)AfxGetApp())->IsAnyRecentFile());
		break;

	case ID_PAPER_EDITABLE:
	case ID_PAPER_NEWQUESTION:
	case ID_PAPER_NEWGROUP:
		{
			BOOL b1 = FALSE, b2 = FALSE;
			CPaper *pPaper = pDoc->GetSelectedPaper();
			if (pPaper)
			{
				b1 = pPaper->CanEdit();
				b2 = pPaper->CanAddItems();
			}

			if (ID_PAPER_EDITABLE == pCmdUI->m_nID)
			{
				pCmdUI->SetCheck(b1);
				pCmdUI->Enable(b2);
			}
			else if (ID_PAPER_NEWQUESTION == pCmdUI->m_nID)
			{
				CList *pList = pDoc->GetSelectedType();
				if (pList)
				{
					pCmdUI->Enable(!pDoc->IsSelectStore() && b1 && b2 && TYPE_GROUP != pList->GetType());
				}
				else
				{
					pCmdUI->Enable((pPaper ? TRUE : FALSE) && b1 && b2);
				}
			}
			else
			{
				CList *pList = pDoc->GetSelectedType();
				if (pList)
				{
					pCmdUI->Enable(!pDoc->IsSelectStore() && b1 && b2 && TYPE_GROUP == pList->GetType());
				}
				else
				{
					pCmdUI->Enable((pPaper ? TRUE : FALSE) && b1 && b2);
				}
			}
		}
		break;

	case ID_TEST_VIEW:
	case ID_TEST_COMPETITIVE:
		pCmdUI->Enable(pDoc->GetPaperCount() && pDoc->GetSelectedPaper()->GetItemCount());
		break;

	case ID_TEST_MAKE:
		pCmdUI->Enable(pDoc->GetPaperCount());
		break;

	case ID_VIEW_COLORNORMAL:
		pCmdUI->SetCheck(CLRTYPE_DEFAULT == g_nColorType);
		break;

	case ID_VIEW_COLORGREEN:
		pCmdUI->SetCheck(CLRTYPE_GREEN == g_nColorType);
		break;

	case ID_VIEW_COLORDARK:
		pCmdUI->SetCheck(CLRTYPE_DARK == g_nColorType);
		break;

	case ID_VIEW_RESET:
		pCmdUI->SetCheck(g_bReset);
		break;
	}
}

void CMain::SetItemCount(int nCount)
{
	CString str;
	str.Format(IDS_ITEMCOUNTFORMAT, nCount);

	UINT dwTemp[2] = {0};
	int cxWidth;
	int nIndex = m_wndStatusBar.CommandToIndex(IDS_ITEMCOUNT);
	
	// 设置宽度
	m_wndStatusBar.GetPaneInfo(nIndex, dwTemp[0], dwTemp[1], cxWidth);
	
	CDC *pDC = m_wndStatusBar.GetDC();
	CFont *pOldFont = NULL;
	CFont *pCurFont = m_wndStatusBar.GetFont();
	pOldFont = pDC->SelectObject(pCurFont);
	
	CSize sz = pDC->GetTextExtent(str);
	pDC->SelectObject(pOldFont);
	
	m_wndStatusBar.ReleaseDC(pDC);
	
	if (cxWidth != sz.cx)
	{
		m_wndStatusBar.SetPaneInfo(nIndex, dwTemp[0], dwTemp[1], sz.cx);
	}
	m_wndStatusBar.SetPaneText(nIndex, str);
}

void CMain::SetShow(LPCTSTR strShow, UINT uTitle)
{
	CString string;
	string.LoadString(uTitle);

	if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
	{
		m_pEditWnd->CheckSaveable(FALSE);
	}
	else
	{
		CTMessageBox Box;

		int cx = GetSystemMetrics(SM_CXSCREEN);
		int cy = GetSystemMetrics(SM_CYSCREEN);
		
		RECT rc;
		rc.left = max(0, (cx - 640) / 2);
		rc.top = max(0, (cy - 480) / 2);
		rc.right = min(cx, 640 + rc.left);
		rc.bottom = min(cy, 480 + rc.top);

		m_pEditWnd = new CEditFrame(EDITFRAMETYPE_SHOWPAPER);
		if (!m_pEditWnd)
		{
			Box.SuperMessageBox(GetSafeHwnd(), IDS_EDITFRM_INITFAIL, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);
			return;
		}
		m_pEditWnd->CreateFrame(string, rc, this, 0);
		if (!IsWindow(m_pEditWnd->GetSafeHwnd()))
		{
			Box.SuperMessageBox(GetSafeHwnd(), IDS_EDITFRM_INITFAIL, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);
			return;
		}
	}

	m_pEditWnd->SetWindowText(string);
	m_pEditWnd->UpdateColor();
	m_pEditWnd->SetContent(strShow);
	m_pEditWnd->ShowWindow(SW_SHOWNORMAL);
}

void CMain::OnColorNormal() 
{
	if (g_nColorType != CLRTYPE_DEFAULT)
	{
		g_nColorType = CLRTYPE_DEFAULT;

		GetPaperView()->UpdateColor();
		GetItemView()->UpdateColor();
		if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
		{
			m_pEditWnd->UpdateColor();
		}
	}
}

void CMain::OnColorGreen() 
{
	if (g_nColorType != CLRTYPE_GREEN)
	{
		g_nColorType = CLRTYPE_GREEN;
		
		GetPaperView()->UpdateColor();
		GetItemView()->UpdateColor();
		if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
		{
			m_pEditWnd->UpdateColor();
		}
	}
}

void CMain::OnColorDark() 
{
	if (g_nColorType != CLRTYPE_DARK)
	{
		g_nColorType = CLRTYPE_DARK;
		
		GetPaperView()->UpdateColor();
		GetItemView()->UpdateColor();
		if (m_pEditWnd && IsWindow(m_pEditWnd->GetSafeHwnd()))
		{
			m_pEditWnd->UpdateColor();
		}
	}
}

void CMain::OnReset() 
{
	if (!g_bReset)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_MAIN_RESET, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);
	}

	g_bReset = !g_bReset;
}
