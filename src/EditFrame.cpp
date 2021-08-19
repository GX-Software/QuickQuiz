// EditFrame.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "EditFrame.h"
#include "QEdit.h"

#include "resource.h"
#include "CodeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Registry entry names:
static LPCTSTR lpszFileSection	= _T("Recent Text List");
static LPCTSTR lpszFileEntry	= _T("File%d");
static LPCTSTR lpszEditWindow	= _T("Edit Window");
static LPCTSTR lpszFont			= _T("Font");
static LPCTSTR lpszColor		= _T("Color");

static LPCTSTR classname		= _T("QQEditFrame");

/////////////////////////////////////////////////////////////////////////////
// CEditFrame

BEGIN_MESSAGE_MAP(CEditFrame, CWnd)
	//{{AFX_MSG_MAP(CEditFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_CLEARMRU, OnClearMRU)
	ON_COMMAND(ID_EX_EXIT, OnExit)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnClear)
	ON_COMMAND(ID_EDIT_FIND, OnFind)
	ON_COMMAND(ID_EDIT_FINDNEXT, OnFindNext)
	ON_COMMAND(ID_EDIT_REPLACE, OnReplace)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_COMMAND(ID_EX_AUTOSEP, OnAutoSep)
	ON_COMMAND(ID_EX_FONT, OnFont)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateCmdUI)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_FILE_CLEARMRU, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FINDNEXT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EX_AUTOSEP, OnUpdateCmdUI)
	//}}AFX_MSG_MAP
	ON_COMMAND_EX_RANGE (ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnFileOpenMRU)
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
END_MESSAGE_MAP()

CEditFrame::CEditFrame(int nType) :
	m_nType(nType),
	m_pRecentFileList(NULL),
	m_hAccelTable(NULL), m_pEdit(NULL),
	m_pFindReplaceDlg(NULL), m_bDown(TRUE), m_bMatchCase(TRUE),
	m_bAutoSep(TRUE), m_hFont(NULL)
{
	ZeroMemory(m_strFilePath, sizeof(m_strFilePath));
	CheckUnicode();

	ZeroMemory(m_strFind, sizeof(m_strFind));
	ZeroMemory(m_strRepl, sizeof(m_strRepl));
	ZeroMemory(&m_lf, sizeof(m_lf));
}

CEditFrame::~CEditFrame()
{
	if (m_pRecentFileList)
	{
		delete m_pRecentFileList;
	}

	if (m_hFont)
	{
		DeleteObject(m_hFont);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CEditFrame message handlers

BOOL CEditFrame::PreCreateWindow(CREATESTRUCT& cs) 
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
	if (EDITFRAMETYPE_SHOWDESC == m_nType)
	{
		wndcls.lpszMenuName = NULL;
	}
	else
	{
		wndcls.lpszMenuName = MAKEINTRESOURCE(IDR_EDITMENU);
	}
	wndcls.lpszClassName = classname;

	VERIFY(AfxRegisterClass(&wndcls));

	cs.lpszClass = classname;
	cs.dwExStyle |= WS_EX_DLGMODALFRAME;
	return TRUE;
}

BOOL CEditFrame::CreateFrame(LPCTSTR lpszWindowName,
							 const RECT& rect /* = rectDefault */,
							 CWnd* pParentWnd /* = NULL */,
							 DWORD dwExStyle /* = 0 */)
{
	CString strTitle;
	if (!lpszWindowName)
	{
		strTitle.LoadString((EDITFRAMETYPE_SHOWDESC == m_nType) ? IDS_EDITFRM_GROUPTITLE : IDS_EDITFRM_TITLE);
	}
	else
	{
		strTitle = lpszWindowName;
	}

	return CreateEx(dwExStyle, classname, strTitle, WS_OVERLAPPEDWINDOW, rect, pParentWnd, NULL);
}

int CEditFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (EDITFRAMETYPE_SHOWDESC != m_nType)
	{
		// 载入快捷键
		m_hAccelTable = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	}
	if (EDITFRAMETYPE_SHOWDESC == m_nType)
	{
		// 禁止点右上角退出
		CMenu* menu = this->GetSystemMenu(FALSE);
		menu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);
	}
	
	// 创建视图
	if (!m_pEdit)
	{
		m_pEdit = new CQEdit;
		if (!m_pEdit)
		{
			return -1;
		}
	}

	DWORD Style = WS_CHILD | WS_VISIBLE | WS_VSCROLL |
				  ES_WANTRETURN | ES_NOHIDESEL | ES_MULTILINE;
	if (!m_bAutoSep)
	{
		Style |= ES_AUTOHSCROLL | WS_HSCROLL;
	}
	DWORD ExStyle = WS_EX_CLIENTEDGE;
	if (EDITFRAMETYPE_NEWOREDIT == m_nType)
	{
		ExStyle |= WS_EX_ACCEPTFILES;
	}
	if (!m_pEdit->CreateEx(ExStyle, _T("EDIT"), NULL, Style,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}

	// 最大显示字数
	m_pEdit->SetLimitText(0);
	
	if (EDITFRAMETYPE_SHOWDESC == m_nType)
	{
		m_pEdit->EnableCopy(FALSE);
		m_pEdit->SetReadOnly(TRUE);
	}

	if (EDITFRAMETYPE_SHOWPAPER == m_nType)
	{
		m_strFilePath[0] = 0;
		m_pEdit->SetModify(TRUE);
	}
	else
	{
		m_strFilePath[0] = 0;
		m_pEdit->SetModify(FALSE);
	}

	// 初始化字体
	if (m_hFont)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
	LPBYTE ptr = NULL;
	UINT n;
	if (EDITFRAMETYPE_SHOWDESC == m_nType)
	{
		GetObject(g_hBigFont, sizeof(LOGFONT), &m_lf);
	}
	else if (!AfxGetApp()->GetProfileBinary(lpszEditWindow, lpszFont, (LPBYTE*)&ptr, &n))
	{
		GetObject(g_hNormalFont, sizeof(LOGFONT), &m_lf);
	}
	else
	{
		ASSERT(n);
		memcpy(&m_lf, ptr, sizeof(LOGFONT));
	}
	if (ptr)
	{
		delete[] ptr;
	}

	m_hFont = CreateFontIndirect(&m_lf);
	ASSERT(m_hFont);
	::SendMessage(m_pEdit->GetSafeHwnd(), WM_SETFONT, (WPARAM)m_hFont, TRUE);

	COLORREF clr = AfxGetApp()->GetProfileInt(lpszEditWindow, lpszColor, RGB(0, 0, 0));
	m_pEdit->SetTextColor(clr);

	// 创建最近打开的文档
	if (!m_pRecentFileList && EDITFRAMETYPE_NEWOREDIT == m_nType)
	{
		m_pRecentFileList = new CFixedRecentFileList(0, lpszFileSection, lpszFileEntry, 8);
	}
	if (m_pRecentFileList)
	{
		m_pRecentFileList->ReadList();
	}
	
	return 0;
}

void CEditFrame::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RECT rc;
	GetClientRect(&rc);
	m_pEdit->MoveWindow(&rc, TRUE);
}

void CEditFrame::OnSetFocus(CWnd* pOldWnd) 
{
	ASSERT(m_pEdit);
	m_pEdit->SetFocus();
}

BOOL CEditFrame::PreTranslateMessage(MSG* pMsg) 
{
	if (!::TranslateAccelerator(GetSafeHwnd(), m_hAccelTable, pMsg))
	{
		return CWnd::PreTranslateMessage(pMsg);
	}
	return TRUE;
}

BOOL CEditFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (m_pEdit->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEditFrame::OnClose() 
{
	switch(m_nType)
	{
	case EDITFRAMETYPE_NEWOREDIT:
	case EDITFRAMETYPE_SHOWPAPER:
		ShowWindow(SW_HIDE);
		return;

	case EDITFRAMETYPE_SHOWDESC:
		return;
	}

	// 禁止再点了
	CMenu *pMenu = GetSystemMenu(FALSE);
	pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

	if (!CheckSaveable(TRUE))
	{
		pMenu->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
		return;
	}
	
	CWnd::OnClose();
}

void CEditFrame::OnDestroy() 
{
	DestroyAcceleratorTable(m_hAccelTable);

	CWnd::OnDestroy();
	
	// 保存文档记录
	if (m_pRecentFileList)
	{
		m_pRecentFileList->WriteList();
	}

	if (EDITFRAMETYPE_NEWOREDIT == m_nType ||
		EDITFRAMETYPE_SHOWPAPER == m_nType)
	{
		// 保存字体信息
		AfxGetApp()->WriteProfileBinary(lpszEditWindow, lpszFont, (LPBYTE)&m_lf, sizeof(LOGFONT));
		AfxGetApp()->WriteProfileInt(lpszEditWindow, lpszColor, (int)m_pEdit->GetTextColor());
	}
}

void CEditFrame::PostNcDestroy() 
{
	CWnd::PostNcDestroy();

	// 没有自行清理的功能，需要手动清理
	if (IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->DestroyWindow();
	}
	if (m_pEdit)
	{
		delete m_pEdit;
	}
	
	delete this;
}

void CEditFrame::OnFileNew() 
{
	if (!CheckSaveable(TRUE))
	{
		return;
	}

	m_strFilePath[0] = 0;
	m_pEdit->SetWindowText(_T(""));
	m_pEdit->SetModify(FALSE);

	CheckUnicode();
}

void CEditFrame::OnFileOpen() 
{
	if (!CheckSaveable(TRUE))
	{
		return;
	}

	TCHAR tmpFilePath[MAX_PATH] = {0};
	OPENFILENAME ofn;
	LPTSTR strFilter = CTextManager::LoadFileFilter(IDS_EDITFRM_LOADFILTER);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_EDITFRM_READFAIL);
		return;
	}
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = strFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = tmpFilePath;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = _T("txt");
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;

	if (!GetOpenFileName(&ofn))
	{
		free(strFilter);
		return;
	}
	free(strFilter);

	// 分析文件的扩展名
	if (ofn.nFileExtension)
	{
		CWaitCursor wc;
		ReadTxtFile(tmpFilePath);
	}
}

void CEditFrame::OnFileSave() 
{
	if (m_strFilePath[0])
	{
		WriteTxtFile(m_strFilePath);
	}
	else
	{
		OnFileSaveAs();
	}
}

void CEditFrame::OnFileSaveAs() 
{
	TCHAR tmpFilePath[MAX_PATH] = {0};
	OPENFILENAME ofn;
	LPTSTR strFilter = CTextManager::LoadFileFilter(IDS_EDITFRM_SAVEFILTER);
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_EDITFRM_SAVEFAIL);
		return;
	}
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = GetSafeHwnd();
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = strFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 1;
	ofn.lpstrFile         = tmpFilePath;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = OFN_OVERWRITEPROMPT;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = _T("txt");
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	
	if (!GetSaveFileName(&ofn))
	{
		free(strFilter);
		return;
	}
	free(strFilter);

	if (!WriteTxtFile(tmpFilePath))
	{
		return;
	}
	if (m_pRecentFileList)
	{
		m_pRecentFileList->Add(tmpFilePath);
	}
}

BOOL CEditFrame::CheckSaveable(BOOL bCanCancel)
{
	ASSERT(m_pEdit && IsWindow(m_pEdit->GetSafeHwnd()));

	UINT nType = MB_ICONQUESTION;
	nType |= bCanCancel ? MB_YESNOCANCEL : MB_YESNO;
	UINT nMsgTxt = (EDITFRAMETYPE_SHOWPAPER == m_nType) ? IDS_EDITFRM_CLOSEASK : IDS_EDITFRM_CLOSEASKNEW;
	if (m_pEdit->GetModify())
	{
		CTMessageBox Box;
		switch(Box.SuperMessageBox(m_hWnd, nMsgTxt, IDS_MSGCAPTION_ASK, nType, 0, 0))
		{
		case IDYES:
			OnFileSave();
			return TRUE;

		case IDNO:
			return TRUE;

		case IDCANCEL:
			return FALSE;

		default:
			ASSERT(FALSE);
		}
	}

	return TRUE;
}

void CEditFrame::UpdateColor()
{
	// 只有在预览时才会涉及到颜色的改变
	if (EDITFRAMETYPE_SHOWPAPER != m_nType)
	{
		return;
	}

	if (IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->Invalidate(TRUE);
	}
}

BOOL CEditFrame::ReadTxtFile(LPCTSTR strFilePath)
{
	CTextManager TMng;
	if (!TMng.LoadFile(strFilePath))
	{
		Q_SHOWERRORMSG_CODE(m_hWnd, IDS_EDITFRM_READFAIL);
		return FALSE;
	}

	// 检查编码情况
	if (CS_UNKNOWN == TMng.GetCS())
	{
		CCodeDlg Dlg;
		Dlg.DoModal();

		if (!TMng.SetDecode(Dlg.GetCS()))
		{
			Q_SHOWERRORMSG_CODE(m_hWnd, IDS_EDITFRM_READFAIL);
			return FALSE;
		}
	}

	m_pEdit->SetWindowText(TMng.GetText());
	
	lstrcpyn(m_strFilePath, strFilePath, MAX_PATH);
	m_pEdit->SetModify(FALSE);
	m_nCS = TMng.GetCS();

	if (m_pRecentFileList)
	{
		m_pRecentFileList->Add(strFilePath);
	}

	SetFocus();
	return TRUE;
}

BOOL CEditFrame::WriteTxtFile(LPCTSTR strFilePath)
{
	if (!m_strFilePath[0])
	{
		// 新保存文档时，只能存为程序当前的编码方式
		CheckUnicode();
	}

	ASSERT(IsWindow(m_pEdit->GetSafeHwnd()));

	CTextManager TMng(m_nCS);
	CString str;

	m_pEdit->GetWindowText(str);
	if (!TMng.SaveFile(strFilePath, str, str.GetLength()))
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_EDITFRM_SAVEFAIL);
		return FALSE;
	}

	lstrcpyn(m_strFilePath, strFilePath, MAX_PATH);
	m_pEdit->SetModify(FALSE);

	SetFocus();
	return TRUE;
}

void CEditFrame::CheckUnicode()
{
#ifdef _UNICODE
	m_nCS = CS_UTF16;
#else
	m_nCS = CS_ANSI;
#endif
}

BOOL CEditFrame::IsAnyRecentFile()
{
	if (!m_pRecentFileList)
	{
		return FALSE;
	}

	int i, nSize;
	
	nSize = m_pRecentFileList->GetSize();
	for (i = 0; i < nSize; i++)
	{
		if (m_pRecentFileList->m_arrNames->GetLength())
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CEditFrame::OnFileOpenMRU(UINT nID)
{
	if (!CheckSaveable(TRUE) || EDITFRAMETYPE_NEWOREDIT != m_nType)
	{
		return TRUE;
	}

	ASSERT_VALID(this);
	ASSERT(m_pRecentFileList != NULL);
	
	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_pRecentFileList->GetSize());
	int nIndex = nID - ID_FILE_MRU_FILE1;
	ASSERT((*m_pRecentFileList)[nIndex].GetLength() != 0);
	
	// 分析文件扩展名
	CString &string = (*m_pRecentFileList)[nIndex];
	int i = string.GetLength() - 1;
	while(i)
	{
		if (_T('.') == string[i])
		{
			i++;
			break;
		}

		i--;
	}

	CWaitCursor wc;
	if (!ReadTxtFile(string))
	{
		m_pRecentFileList->Remove(nIndex);
	}
	
	return TRUE;
}

void CEditFrame::OnClearMRU() 
{
	CTMessageBox Box;
	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_DOC_CLEARMRU, IDS_MSGCAPTION_ASK,
		MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	m_pRecentFileList->ClearList();
}

void CEditFrame::OnExit() 
{
	OnClose();
}

void CEditFrame::OnUndo() 
{
	m_pEdit->Undo();
}

void CEditFrame::OnCut() 
{
	m_pEdit->Cut();
}

void CEditFrame::OnCopy() 
{
	m_pEdit->Copy();
}

void CEditFrame::OnPaste() 
{
	m_pEdit->Paste();
}

void CEditFrame::OnClear() 
{
	m_pEdit->ReplaceSel(_T(""), TRUE);
}

void CEditFrame::OnFind() 
{
	if (m_pFindReplaceDlg)
	{
		m_pFindReplaceDlg->SetActiveWindow();
		return;
	}

	DWORD dwFind = FR_HIDEWHOLEWORD;
	if (m_bDown)
	{
		dwFind |= FR_DOWN;
	}
	if (m_bMatchCase)
	{
		dwFind |= FR_MATCHCASE;
	}
	m_pFindReplaceDlg = new CFindReplaceDialog;
	if (m_pFindReplaceDlg)
	{
		m_pFindReplaceDlg->Create(TRUE, m_strFind, NULL, dwFind, this);
	}
}

void CEditFrame::OnFindNext() 
{
	int nStart, nEnd;
	m_pEdit->GetSel(nStart, nEnd);
	
	CString str;
	m_pEdit->GetWindowText(str);
	
#ifdef _UNICODE
	if (m_dwFindFlags & FR_DOWN)
	{
		if (nStart != nEnd)
		{
			nStart++;
		}
	}
	else
	{
		if (nStart != nEnd)
		{
			if (nEnd < 2)
			{
				nStart = 0;
			}
			else
			{
				nStart = nEnd - 2;
			}
		}
		else
		{
			nStart = nEnd - 1;
		}
	}
#else
	if (m_dwFindFlags & FR_DOWN)
	{
		if (nStart != nEnd)
		{
			nStart = CTextManager::GetRealIndex(str, nStart + 1);
		}
		else
		{
			nStart = CTextManager::GetRealIndex(str, nStart);
		}
	}
	else
	{
		if (nStart != nEnd)
		{
			if (nEnd < 2)
			{
				nStart = 0;
			}
			else
			{
				nStart = CTextManager::GetRealIndex(str, nEnd - 2);
			}
		}
		else
		{
			nStart = CTextManager::GetRealIndex(str, nEnd - 1);
		}
	}
#endif
	
	BOOL ret = m_Find.FindNext(str, nStart, nStart, nEnd);
	if (!ret)
	{
		if (m_dwFindFlags & FR_DOWN)
		{
			ret = m_Find.FindNext(str, 0, nStart, nEnd);
		}
		else
		{
			ret = m_Find.FindNext(str, str.GetLength() - 1, nStart, nEnd);
		}

		if (!ret)
		{
			CTMessageBox Box;
			Box.SuperMessageBox(m_hWnd, IDS_EDITFRM_FINDFAIL, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);

			if (m_pFindReplaceDlg)
			{
				m_pFindReplaceDlg->SetActiveWindow();
			}
			return;
		}
	}
	
#ifndef _UNICODE
	nStart = MultiByteToWideChar(CP_ACP, 0, str, nStart, NULL, 0);
	nEnd = MultiByteToWideChar(CP_ACP, 0, str, nEnd, NULL, 0);
#endif
	m_pEdit->SetSel(nStart, nEnd);
}

void CEditFrame::OnReplace() 
{
	if (m_pFindReplaceDlg)
	{
		m_pFindReplaceDlg->SetActiveWindow();
		return;
	}
	
	DWORD dwFind = FR_HIDEWHOLEWORD;
	if (m_bDown)
	{
		dwFind |= FR_DOWN;
	}
	if (m_bMatchCase)
	{
		dwFind |= FR_MATCHCASE;
	}
	m_pFindReplaceDlg = new CFindReplaceDialog;
	if (m_pFindReplaceDlg)
	{
		m_pFindReplaceDlg->Create(FALSE, m_strFind, m_strRepl, dwFind, this);
	}
}

LRESULT CEditFrame::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE ptr = (LPFINDREPLACE)lParam;

	if (!ptr->lpstrReplaceWith)
	{
		m_bDown = (ptr->Flags & FR_DOWN) ? TRUE : FALSE;
	}
	m_bMatchCase = (ptr->Flags & FR_MATCHCASE) ? TRUE : FALSE;
	
	if (ptr->Flags & FR_DIALOGTERM)
	{
		m_pFindReplaceDlg = NULL;
	}
	else if (ptr->Flags & FR_FINDNEXT)
	{
		// 尽可能少地设置查找参数
		// 当查找的内容和原来保存的内容不一样时，重新设置查找参数
		if (_tcscmp(m_strFind, ptr->lpstrFindWhat))
		{
			if (!m_Find.SetPara(ptr->lpstrFindWhat, 0, ptr->Flags & FR_DOWN, ptr->Flags & FR_MATCHCASE))
			{
				return 0;
			}

			_tcscpyn(m_strFind, ptr->lpstrFindWhat, MAX_STRING_LEN);
		}

		m_dwFindFlags = ptr->Flags;
		OnFindNext();

		// 查找设置结束后，对话框关闭
		// 替换不关闭
		if (m_pFindReplaceDlg && !ptr->lpstrReplaceWith)
		{
			m_pFindReplaceDlg->DestroyWindow();
			m_pFindReplaceDlg = NULL;
		}
	}
	else if (ptr->Flags & FR_REPLACE)
	{
		m_pEdit->ReplaceSel(ptr->lpstrReplaceWith, TRUE);
		m_pEdit->Invalidate(FALSE);

		OnFindNext();

		_tcscpyn(m_strFind, ptr->lpstrFindWhat, MAX_STRING_LEN);
		_tcscpyn(m_strRepl, ptr->lpstrReplaceWith, MAX_STRING_LEN);
	}
	else if (ptr->Flags & FR_REPLACEALL)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_EDITFRM_REPLACEDONE, IDS_MSGCAPTION_INFOR,
			MB_OK | MB_ICONINFORMATION, 0, 0, ReplaceAll(ptr));

		_tcscpyn(m_strFind, ptr->lpstrFindWhat, MAX_STRING_LEN);
		_tcscpyn(m_strRepl, ptr->lpstrReplaceWith, MAX_STRING_LEN);
	}

	return 0;
}

int CEditFrame::ReplaceAll(LPFINDREPLACE ptr)
{
	if (!ptr)
	{
		return 0;
	}

	// 如果替换的内容和原来一样，就不替换
	if (m_bMatchCase)
	{
		if (!lstrcmp(ptr->lpstrFindWhat, ptr->lpstrReplaceWith))
		{
			return 0;
		}
	}
	else
	{
		if (!lstrcmpi(ptr->lpstrFindWhat, ptr->lpstrReplaceWith))
		{
			return 0;
		}
	}

	CString string;
	m_pEdit->GetWindowText(string);

	CFindText Find;
	// 开始查找
	Find.SetPara(ptr->lpstrFindWhat, 0, TRUE, ptr->Flags & FR_MATCHCASE);

	int ret = TRUE, i = 0, nCount = 0;
	int nEnd;
	int nLen = lstrlen(ptr->lpstrFindWhat);
	while(1)
	{
		ret = Find.FindNext(string, i, i, nEnd);
		if (!ret)
		{
			break;
		}

		string.Delete(i, nLen);
		string.Insert(i, ptr->lpstrReplaceWith);

		i += _tcslen(ptr->lpstrReplaceWith);
		nCount++;
	}

	if (nCount)
	{
		m_pEdit->SetWindowText(string);
		m_pEdit->SetModify();
	}
	return nCount;
}

void CEditFrame::OnSelectAll() 
{
	m_pEdit->SetSel(0, -1);
}

void CEditFrame::OnAutoSep() 
{
	CString str;
	m_pEdit->GetWindowText(str);

	LONG Style = GetWindowLong(m_pEdit->GetSafeHwnd(), GWL_STYLE);
	m_bAutoSep = !m_bAutoSep;
	if (m_bAutoSep)
	{
		Style &= ~WS_HSCROLL;
		Style &= ~ES_AUTOHSCROLL;
	}
	else
	{
		Style |= ES_AUTOHSCROLL;
		Style |= WS_HSCROLL;
	}
	Style |= WS_VSCROLL;

	int nStart, nEnd;
	m_pEdit->GetSel(nStart, nEnd);
	m_pEdit->DestroyWindow();

	if (!m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), NULL, Style,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		ASSERT(FALSE);
	}
	::SendMessage(m_pEdit->GetSafeHwnd(), WM_SETFONT, (WPARAM)m_hFont, TRUE);

	RECT rc;
	GetWindowRect(&rc);
	OnSize(SIZE_RESTORED, rc.right - rc.left, rc.bottom - rc.top);
	
	m_pEdit->SetWindowText(str);
	m_pEdit->SetSel(nStart, nStart);

	m_pEdit->SetFocus();
}

void CEditFrame::OnFont() 
{
	CHOOSEFONT cf ;
	
	cf.lStructSize    = sizeof (CHOOSEFONT);
	cf.hwndOwner      = m_hWnd;
	cf.hDC            = NULL;
	cf.lpLogFont      = &m_lf;
	cf.iPointSize     = 0;
	cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.rgbColors      = m_pEdit->GetTextColor();
	cf.lCustData      = 0;
	cf.lpfnHook       = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance      = NULL;
	cf.lpszStyle      = NULL;
	cf.nFontType      = 0;
	cf.nSizeMin       = 0;
	cf.nSizeMax       = 0;
	
	if (!ChooseFont(&cf))
	{
		return;
	}

	m_pEdit->SetTextColor(cf.rgbColors);

	HFONT hNewFont = CreateFontIndirect(&m_lf);
	ASSERT(hNewFont);
	::SendMessage(m_pEdit->GetSafeHwnd(), WM_SETFONT, (WPARAM)hNewFont, TRUE);

	DeleteObject(m_hFont);
	m_hFont = hNewFont;
}

void CEditFrame::OnUpdateCmdUI(CCmdUI* pCmdUI) 
{
	switch(pCmdUI->m_nID)
	{
	case ID_FILE_NEW:
	case ID_FILE_OPEN:
		pCmdUI->Enable(EDITFRAMETYPE_NEWOREDIT == m_nType);
		break;

	case ID_FILE_SAVE:
	case ID_FILE_SAVE_AS:
		pCmdUI->Enable(EDITFRAMETYPE_SHOWDESC != m_nType);
		break;

	case ID_FILE_MRU_FILE1:
		ASSERT_VALID(this);
		if (m_pRecentFileList == NULL)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			m_pRecentFileList->UpdateMenu(pCmdUI);
		}
		break;

	case ID_FILE_CLEARMRU:
		pCmdUI->Enable(IsAnyRecentFile());
		break;

	case ID_EDIT_UNDO:
		pCmdUI->Enable(m_pEdit->CanUndo());
		break;

	case ID_EDIT_CUT:
	case ID_EDIT_COPY:
	case ID_EDIT_CLEAR:
		{
			int nStart, nEnd;
			m_pEdit->GetSel(nStart, nEnd);
			pCmdUI->Enable(nStart < nEnd);
		}
		break;

	case ID_EDIT_PASTE:
		pCmdUI->Enable(IsClipboardFormatAvailable(CF_TEXT));
		break;

	case ID_EDIT_FIND:
	case ID_EDIT_REPLACE:
		pCmdUI->Enable(!m_pFindReplaceDlg && m_pEdit->GetWindowTextLength());
		break;

	case ID_EDIT_FINDNEXT:
		pCmdUI->Enable(!m_pFindReplaceDlg && m_pEdit->GetWindowTextLength() &&
			lstrlen(m_strFind));
		break;

	case ID_EDIT_SELECT_ALL:
		pCmdUI->Enable(m_pEdit->GetWindowTextLength());
		break;

	case ID_EX_AUTOSEP:
		pCmdUI->SetCheck(m_bAutoSep);
		break;

	default:
		ASSERT(FALSE);
	}
}

void CEditFrame::SetContent(LPCTSTR lpszContent)
{
	if (!lpszContent)
	{
		return;
	}

	m_pEdit->SetWindowText(lpszContent);

	if (EDITFRAMETYPE_SHOWDESC == m_nType)
	{
		m_pEdit->SetModify(FALSE);
	}
}

LPTSTR CEditFrame::GetSelText()
{
	ASSERT(m_pEdit);

	LPTSTR lpBuf = NULL;
	long lLen = m_pEdit->GetSelText(NULL);
	if (lLen)
	{
		lpBuf = (LPTSTR)malloc((lLen + 1) * sizeof(TCHAR));
		if (lpBuf)
		{
			m_pEdit->GetSelText(lpBuf);
			return lpBuf;
		}
	}

	return NULL;
}

void CEditFrame::GetSel(int& nStartChar, int& nEndChar)
{
	m_pEdit->GetSel(nStartChar, nEndChar);
}

void CEditFrame::SetSel(int nStartChar, int nEndChar, BOOL bNoScroll /* = FALSE */)
{
	m_pEdit->SetSel(nStartChar, nEndChar, bNoScroll);
}

void CEditFrame::OnDropFiles(HDROP hDropInfo) 
{
	if (!CheckSaveable(TRUE))
	{
		return;
	}

	UINT uCount;
	TCHAR szPath[MAX_PATH];
	
	uCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	if (uCount)
	{
		if (uCount > 1)
		{
			CTMessageBox Box;
			Box.SuperMessageBox(m_hWnd, IDS_EDITFRM_DROPMULTIFILES, IDS_MSGCAPTION_INFOR,
				MB_OK | MB_ICONINFORMATION, 0, 0);
		}

		// 只打开第一个
		DragQueryFile(hDropInfo, 0, szPath, _countof(szPath));
		ReadTxtFile(szPath);
	}

	DragFinish(hDropInfo);
}
