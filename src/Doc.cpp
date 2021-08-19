// Doc.cpp : implementation of the CDoc class
//

#include "stdafx.h"
#include "QuickQuiz.h"
#include "Doc.h"
#include <dlgs.h>

#include "PasswordDlg.h"
#include "ImageManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDoc

IMPLEMENT_DYNCREATE(CDoc, CDocument)

BEGIN_MESSAGE_MAP(CDoc, CDocument)
	//{{AFX_MSG_MAP(CDoc)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_ALL, OnFileSaveAll)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	//}}AFX_MSG_MAP
	ON_COMMAND_EX_RANGE (ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnFileOpenMRU)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDoc construction/destruction

BOOL CDoc::s_bSaveAsWin95 = FALSE;
BOOL CDoc::s_bMaxCompatible = FALSE;

CDoc::CDoc() :
	m_bFirst(TRUE),
	m_pAllPapers(NULL),
	m_pCurSelPaper(NULL),
	m_pCurSelType(TYPE_DEFAULT)
{
	// 不精简空格
	TiXmlBase::SetCondenseWhiteSpace(false);
}

CDoc::~CDoc()
{
	if (m_pAllPapers)
	{
		delete m_pAllPapers;
	}
}

BOOL CDoc::OnNewDocument()
{
	if (!m_pAllPapers)
	{
		m_pAllPapers = new CList;
		if (!m_pAllPapers)
		{
			return FALSE;
		}
	}

	if (m_bFirst)
	{
		m_bFirst = FALSE;
		return TRUE;
	}
	else
	{
		CPaper *pPaper = CreatePaper(TRUE);
		if (!pPaper)
		{
			Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_INITPAPERFAIL);
			return FALSE;
		}
		m_pAllPapers->AddItem(pPaper);
		
		// 避免了一次调用
		m_pCurSelPaper = pPaper;
		m_pCurSelType = NULL;

		UpdateAllViews(NULL, HINT_ADD_PAPER, (CObject*)pPaper);
		return TRUE;
	}
}

BOOL CDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	CWaitCursor wc;

	// 检查这个文件是不是已经打开过了
	CPaper* pPaper = IsPaperAlreadyOpen(lpszPathName);
	if (pPaper)
	{
		m_pCurSelPaper = pPaper;
		m_pCurSelType = NULL;

		UpdateAllViews(NULL, HINT_SELECT_PAPER, (CObject*)m_pCurSelPaper);
		return TRUE;
	}

	// 创建一个题库
	pPaper = CreatePaper(FALSE);
	if (!pPaper)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_INITPAPERFAIL);
		return FALSE;
	}
	
	if (!pPaper->LoadPaper(lpszPathName, ShowPassword))
	{
		delete pPaper;
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_OPENFAIL);
		return FALSE;
	}
	// 产生此情况的原因是没有读取完整，将弹窗提示
	else if (Q_ERRORCODE)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_OPENINCOMPLETE);
	}
	Q_CLEARERROR();
	m_pAllPapers->AddItem(pPaper);
	
	// 避免了一次调用
	m_pCurSelPaper = pPaper;
	m_pCurSelType = NULL;

	UpdateAllViews(NULL, HINT_ADD_PAPER, (CObject*)pPaper);

	// 加入最近文件列表中
	AfxGetApp()->AddToRecentFileList(lpszPathName);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDoc serialization

void CDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDoc diagnostics

#ifdef _DEBUG
void CDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDoc commands

BOOL CDoc::RouteCmdMsg(CView *pActiveView, UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	POSITION pos = GetFirstViewPosition ();
	
	while (pos != NULL)
	{
		CView *pNextView = GetNextView (pos);
		if (pNextView != pActiveView)
		{
			if (((CCmdTarget *) pNextView)->OnCmdMsg (nID, nCode, pExtra, pHandlerInfo))
			{
				return (TRUE);
			}
		}
	}
	
	return (FALSE);
}

typedef struct _tagOFNNT5 { 
	DWORD         lStructSize;
	HWND          hwndOwner;
	HINSTANCE     hInstance;
	LPCTSTR       lpstrFilter;
	LPTSTR        lpstrCustomFilter;
	DWORD         nMaxCustFilter;
	DWORD         nFilterIndex;
	LPTSTR        lpstrFile;
	DWORD         nMaxFile;
	LPTSTR        lpstrFileTitle;
	DWORD         nMaxFileTitle;
	LPCTSTR       lpstrInitialDir;
	LPCTSTR       lpstrTitle;
	DWORD         Flags;
	WORD          nFileOffset;
	WORD          nFileExtension;
	LPCTSTR       lpstrDefExt;
	LPARAM        lCustData;
	LPOFNHOOKPROC lpfnHook;
	LPCTSTR       lpTemplateName;
	void *        pvReserved;
	DWORD         dwReserved;
	DWORD         FlagsEx;
} OPENFILENAME_NT5;

void CDoc::OnFileOpen() 
{
	OPENFILENAME ofn = {0};
	TCHAR strFilePath[MAX_PATH] = {0};
	DWORD lStructSize;
	
	CString strTitle;
	strTitle.LoadString(IDS_DOC_OPENFILECAPTION);

	LPTSTR strFilter = NULL;
	if (OS_WIN95 == g_nOSVersion)
	{
		strFilter = CTextManager::LoadFileFilter(IDS_DOC_95LOADFILEFILTER);
	}
	else
	{
		strFilter = CTextManager::LoadFileFilter(IDS_DOC_LOADFILEFILTER);
	}
	if (!strFilter)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_OPENFAIL);
		return;
	}
	
	// 适应当前系统的对话框风格
	switch(g_nOSVersion)
	{
	case OS_WIN95:
	case OS_WIN98:
	case OS_WINNT4:
		lStructSize = sizeof(OPENFILENAME);
		break;
		
	default:
		lStructSize = sizeof(OPENFILENAME_NT5);
		break;
	}
	
	ofn.lStructSize       = lStructSize;
	ofn.hwndOwner         = AfxGetMainWnd()->GetSafeHwnd();
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
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = paper_lpszBinExt;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	
	// 获取了文件路径
	if (GetOpenFileName(&ofn))
	{
		// 进入文档类开始打开题库
		OnOpenDocument(strFilePath);
	}

	free(strFilter);
}

void CDoc::OnFileSave() 
{
	ASSERT(m_pCurSelPaper);

	LPCTSTR strFilePath = m_pCurSelPaper->GetFilePath();
	if (!m_pCurSelPaper->IsModify() && strFilePath)
	{
		return;
	}
	
	if (!strFilePath)
	{
		OnFileSaveAs();
	}
	else
	{
		// 如果是按XML读取的，则加入选择
		if (m_pCurSelPaper->GetDefSaveStyle() == PAPER_FILETYPE_XML)
		{
			OnFileSaveAs();
		}
		else
		{
			if (!m_pCurSelPaper->SavePaper(NULL, PAPER_FILETYPE_NONE))
			{
				Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_SAVEFAIL);
				return;
			}
		}

		UpdateAllViews(NULL, HINT_SAVE_PAPER, (CObject*)m_pCurSelPaper);
		
		// 加入最近文件列表中
		AfxGetApp()->AddToRecentFileList(m_pCurSelPaper->GetFilePath());
	}
}

void CDoc::OnFileSaveAll() 
{
	if (!SaveAllPapers(FALSE))
	{
		return;
	}

	UpdateAllViews(NULL, HINT_SAVE_PAPER, (CObject*)m_pCurSelPaper);
}

void CDoc::OnFileSaveAs() 
{
	if (!SavePaper(TRUE))
	{
		Q_CLEARERROR();
		return;
	}

	UpdateAllViews(NULL, HINT_SAVE_PAPER, (CObject*)m_pCurSelPaper);

	// 加入最近文件列表中
	AfxGetApp()->AddToRecentFileList(m_pCurSelPaper->GetFilePath());
}

void CDoc::OnFileClose() 
{
	if (!m_pCurSelPaper)
	{
		return;
	}
	
	CTMessageBox Box;
	if (m_pCurSelPaper->IsModify())
	{
		switch(Box.SuperMessageBox(NULL, IDS_DOC_CLOSEASKMODIFY, IDS_MSGCAPTION_ASK,
			MB_YESNOCANCEL | MB_ICONQUESTION, 0, 0, m_pCurSelPaper->GetDescription()))
		{
		case IDYES:
			OnFileSave();
			break;

		case IDNO:
			break;

		case IDCANCEL:
			return;
		}
	}
	else
	{
		if (IDYES != Box.SuperMessageBox(NULL, IDS_DOC_CLOSEASKNOSAVE, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION, 0, 0, m_pCurSelPaper->GetDescription()))
		{
			return;
		}
	}
	
	CPaper *pPaper = m_pCurSelPaper;
	
	// 通知视图关闭题库
	UpdateAllViews(NULL, HINT_CLOSE_PAPER, (CObject*)m_pCurSelPaper);
	
	m_pAllPapers->RemoveItem(pPaper, TRUE);
	
	// 在删除最后一个节点时不会触发消息
	// 需要在这里做额外工作
	if (!m_pAllPapers->GetItemCount())
	{
		m_pCurSelPaper = NULL;
		m_pCurSelType = NULL;
	}
}

BOOL CDoc::SavePaper(BOOL bAskIfFail)
{
	OPENFILENAME ofn = {0};
	TCHAR tmpFilePath[MAX_PATH] = {0};
	int nType = 0;

	DWORD lStructSize = 0;
	DWORD Flags = 0;
	LPOFNHOOKPROC lpfnHook = (LPOFNHOOKPROC)OFNHookProc;

	CString strTitle;
	strTitle.LoadString(IDS_DOC_SAVEFILECAPTION);

	LPTSTR strFilter = NULL;
	if (OS_WIN95 == g_nOSVersion)
	{
		CDoc::s_bSaveAsWin95 = TRUE;
		Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
		strFilter = CTextManager::LoadFileFilter(IDS_DOC_95SAVEFILEFILTER);
	}
	else
	{
		Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLESIZING;
		if (PAPER_COVERTYPE_NONE == m_pCurSelPaper->GetCoverInfo()->nType)
		{
			strFilter = CTextManager::LoadFileFilter(IDS_DOC_SAVEFILEFILTERNC);
		}
		else
		{
			strFilter = CTextManager::LoadFileFilter(IDS_DOC_SAVEFILEFILTER);
		}
	}
	if (!strFilter && bAskIfFail)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_SAVEFAIL);
		return FALSE;
	}
	_tcscpyn(tmpFilePath, m_pCurSelPaper->GetDescription(), MAX_PAPERNAME_LEN);

	// 保证在加上钩子以后，仍然可以适应当前系统的对话框风格
	switch(g_nOSVersion)
	{
	case OS_WIN95:
	case OS_WIN98:
	case OS_WINNT4:
		lStructSize = sizeof(OPENFILENAME);
		break;
		
	default:
		lStructSize = sizeof(OPENFILENAME_NT5);
		break;
	}

	ofn.lStructSize       = lStructSize;
	ofn.hwndOwner         = AfxGetMainWnd()->GetSafeHwnd();
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
	ofn.lpstrTitle        = strTitle;
	ofn.Flags             = Flags;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = paper_lpszBinExt;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = lpfnHook;
	ofn.lpTemplateName    = NULL;
	
	if (!GetSaveFileName(&ofn))
	{
		free(strFilter);
		return FALSE;
	}

	// Windows95系统只能添加3字符的扩展名，需要额外处理
	if (OS_WIN95 == g_nOSVersion)
	{
		if (!_tcsnicmp(tmpFilePath + ofn.nFileExtension, paper_lpszOldBinExt, 3))
		{
			_tcscat(tmpFilePath, _T("z"));
		}
	}

	// 存储类型
	nType = CPaper::GetFileExt(tmpFilePath);
	switch(ofn.nFilterIndex)
	{
	case 4:
		if (PAPER_FILETYPE_JPEG == nType)
		{
			nType = PAPER_FILETYPE_JPEGBIN;
		}
		break;

	case 5:
		if (PAPER_FILETYPE_JPEG == nType)
		{
			nType = PAPER_FILETYPE_JPEGXML;
		}
		break;
	}
	ASSERT(PAPER_FILETYPE_JPEG != nType);

	CWaitCursor wc;
	free(strFilter);

	switch(nType)
	{
		// 标准格式
	case PAPER_FILETYPE_BIN:
	case PAPER_FILETYPE_OLDBIN:
	case PAPER_FILETYPE_JPEGBIN:
		break;

		// 兼容格式
	case PAPER_FILETYPE_XML:
	case PAPER_FILETYPE_JPEGXML:
		if (m_pCurSelPaper->IsLocked())
		{
			CUnlockDlg Dlg(m_pCurSelPaper->GetPassword());
			if (IDOK != Dlg.DoModal())
			{
				return FALSE;
			}
		}
		break;
	}

	if (CDoc::s_bSaveAsWin95)
	{
		nType |= PAPER_FILETYPE_WIN95;
	}
	if (CDoc::s_bMaxCompatible)
	{
		nType |= PAPER_FILETYPE_MAXCOMP;
	}
	if (!m_pCurSelPaper->SavePaper(tmpFilePath, nType) && bAskIfFail)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_SAVEFAIL);
		return FALSE;
	}

	return TRUE;
}

UINT_PTR CALLBACK CDoc::OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	static HWND hParent = NULL;
	static HWND hButton95 = NULL; // 兼容win95按钮
	static HWND hButtonComp = NULL; // 最大兼容按钮
	
	if (WM_INITDIALOG == uiMsg)
	{
		HWND hCtrl;
		HFONT hFont;
		RECT rcCombo = {0}, rcParent = {0};
		TCHAR szText[64] = {0};
		int nHeight;
		
		hParent = GetParent(hDlg);
		GetWindowRect(hParent, &rcParent);
		
		hCtrl = GetDlgItem(hParent, cmb1);
		GetWindowRect(hCtrl, &rcCombo);

		nHeight = rcParent.bottom - rcParent.top + rcCombo.bottom - rcCombo.top;
		
		SetWindowPos(hParent, NULL, 0, 0,
			rcParent.right - rcParent.left, nHeight,
			SWP_NOMOVE | SWP_NOZORDER);
		
		hFont = (HFONT)SendMessage(hParent, WM_GETFONT, 0, 0);
		LoadString(AfxGetInstanceHandle(), IDS_DOC_MAXCOMPATIBLE, szText, _countof(szText));
		hButtonComp = CreateWindow(_T("BUTTON"), szText, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			0, 0, 0, 0, hParent, NULL, AfxGetInstanceHandle(), NULL);
		SendMessage(hButtonComp, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
		SendMessage(hButtonComp, BM_SETCHECK, (WPARAM)(CDoc::s_bMaxCompatible ? BST_CHECKED : BST_UNCHECKED), 0);
		EnableWindow(hButtonComp, FALSE);
		
		if (OS_WIN95 != g_nOSVersion)
		{
			LoadString(AfxGetInstanceHandle(), IDS_DOC_WIN95XML, szText, _countof(szText));
			hButton95 = CreateWindow(_T("BUTTON"), szText, WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
				0, 0, 0, 0, hParent, NULL, AfxGetInstanceHandle(), NULL);
			SendMessage(hButton95, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
			SendMessage(hButton95, BM_SETCHECK, (WPARAM)(CDoc::s_bSaveAsWin95 ? BST_CHECKED : BST_UNCHECKED), 0);
			EnableWindow(hButton95, FALSE);
		}
		else
		{
			hButton95 = NULL;
		}
		
	}
	else if (WM_SIZE == uiMsg)
	{
		HWND hCombo = GetDlgItem(hParent, cmb1);
		HWND hEdit = GetDlgItem(hParent, cmb13);
		RECT rcCombo = {0}, rcDlg = {0};
		int x, y, i;
		
		GetWindowRect(hCombo, &rcCombo);
		GetWindowRect(hDlg, &rcDlg);
		if (hEdit)
		{
			RECT rcEdit = {0};
			GetWindowRect(hEdit, &rcEdit);
			i = rcCombo.top - rcEdit.bottom;
			if (i < 5)
			{
				i = 5;
			}
		}
		else
		{
			i = 5;
		}

		x = rcCombo.left - rcDlg.left;
		y = rcCombo.bottom - rcDlg.top + i;
		SetWindowPos(hButtonComp, NULL, x, y, (rcCombo.right - rcCombo.left) / 2,
			rcCombo.bottom - rcCombo.top, SWP_NOZORDER);

		if (hButton95)
		{
			SetWindowPos(hButton95, NULL, x + (rcCombo.right - rcCombo.left) / 2, y,
				(rcCombo.right - rcCombo.left) / 2, rcCombo.bottom - rcCombo.top, SWP_NOZORDER);
		}
	}
	else if (WM_NOTIFY == uiMsg)
	{
		LPOFNOTIFY pNotify = (LPOFNOTIFY)lParam;
		switch(pNotify->hdr.code)
		{
		case CDN_TYPECHANGE:
			switch(pNotify->lpOFN->nFilterIndex)
			{
				// 仅在保存为兼容模式时允许选中
			case 3:
			case 5:
				EnableWindow(hButtonComp, TRUE);
				if (hButton95)
				{
					EnableWindow(hButton95, TRUE);
				}
				break;
				
			default:
				EnableWindow(hButtonComp, FALSE);
				if (hButton95)
				{
					EnableWindow(hButton95, FALSE);
				}
				break;
			}
			break;
			
		case CDN_FILEOK:
			switch(pNotify->lpOFN->nFilterIndex)
			{
			case 3:
			case 5:
				CDoc::s_bMaxCompatible = SendMessage(hButtonComp, BM_GETCHECK, 0, 0);
				if (hButton95)
				{
					CDoc::s_bSaveAsWin95 = SendMessage(hButton95, BM_GETCHECK, 0, 0);
				}
				break;
				
			default:
				CDoc::s_bMaxCompatible = FALSE;
				CDoc::s_bSaveAsWin95 = FALSE;
				break;
			}
			break;
		}
	}
	
	return 0;
}

RETURN_E CPaper* CDoc::CreatePaper(BOOL bIsNewPaper)
{
	CPaper *pPaper = NULL;

	// 创建新的题库
	// 打开的题库默认不可编辑
	pPaper = new CPaper();
	if (!pPaper)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	if (!pPaper->InitPaper(bIsNewPaper))
	{
		delete pPaper;
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}

	return Q_TRUE_RET(pPaper);
}

CPaper* CDoc::IsPaperAlreadyOpen(LPCTSTR strFilePath)
{
	if (!m_pAllPapers)
	{
		return NULL;
	}

	int i;
	CPaper *pPaper;
	for (i = 0; i < m_pAllPapers->GetItemCount(); ++i)
	{
		pPaper = (CPaper*)m_pAllPapers->GetItem(i);
		if (!lstrcmp(strFilePath, pPaper->GetFilePath()))
		{
			return pPaper;
		}
	}

	return NULL;
}

BOOL FUNCCALL CDoc::ShowPassword(LPSTR strPassword, LPBYTE pMD5)
{
	CUnlockDlg Dlg(pMD5);
	if (IDOK != Dlg.DoModal())
	{
		return FALSE;
	}
	strcpy(strPassword, Dlg.GetPasswordText());
	return TRUE;
}

BOOL CDoc::OnFileOpenMRU(UINT nID)
{
	return ((CApp*)AfxGetApp())->OpenRecentFile(this, nID);
}

void CDoc::SetSelectedPaper(CPaper *pPaper, CTempList *pType)
{
	ASSERT(pPaper);
	m_pCurSelPaper = pPaper;
	m_pCurSelType = pType;

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_SELECT_TYPE, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_SELECT_PAPER, (CObject*)m_pCurSelPaper);
	}
}

CList* CDoc::GetSelectedList()
{
	if (m_pCurSelType)
	{
		return m_pCurSelType;
	}
	else if (m_pCurSelPaper)
	{
		return m_pCurSelPaper;
	}
	else
	{
		return NULL;
	}
}

int CDoc::GetPaperCount()
{
	if (!m_pAllPapers)
	{
		return 0;
	}

	return m_pAllPapers->GetItemCount();
}

/*
功能：	保存全部已打开的题库
返回值：当bAsk为TRUE时，返回询问的结果；否则永远返回TRUE
说明：	bAsk的作用是，当题库为新建时，是否提示保存
		当用户要求新建的题库不做保存时，返回FALSE，否则永远返回TRUE
*/
BOOL CDoc::SaveAllPapers(BOOL bAsk)
{
	BOOL bSave = FALSE;
	
	int i;
	CPaper *pPaper;

	CWaitCursor wc;
	for (i = 0; i < m_pAllPapers->GetItemCount(); ++i)
	{
		pPaper = (CPaper*)m_pAllPapers->GetItem(i);
		if (pPaper->IsModify() || !pPaper->GetFilePath())
		{
			if (bAsk)
			{
				CTMessageBox Box;
				switch(Box.SuperMessageBox(NULL, IDS_DOC_CLOSEASKNEW, IDS_MSGCAPTION_ASK,
					MB_YESNOCANCEL | MB_ICONQUESTION, 0, 0, pPaper->GetDescription()))
				{
				case IDCANCEL:
					return FALSE;
					
				case IDYES:
					bSave = TRUE;
					
				case IDNO:
					bSave = FALSE;
				}
			}
			else
			{
				bSave = TRUE;
			}
			
			if (bSave)
			{
				// 在保存全部题库的功能中，不判断保存结果是否是xml
				// 直接按照原格式保存
				if (pPaper->GetFilePath())
				{
					if (!pPaper->SavePaper(NULL, PAPER_FILETYPE_NONE))
					{
						Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_SAVEFAIL);
						Q_CLEARERROR();
					}
				}
				else
				{
					OnFileSaveAs();
				}
			}
		}
	}
	
	return TRUE;
}

EBOOL CDoc::SetCoverInfo(LPPAPERCOVERINFO ppc)
{
	ASSERT(m_pCurSelPaper);

	// 此处申请的堆内存将直接通过paper->SetCoverInfo赋给题库类
	// 故在这里不需要释放
	PAPERCOVERINFO i;
	i.nType = ppc->nType;
	i.nWidth = ppc->nWidth;
	i.nHeight = ppc->nHeight;
	i.clrBk = ppc->clrBk;
	i.clrText = ppc->clrText;
	i.nQuality = ppc->nQuality;
	i.strTitle = _tcsdup(ppc->strTitle);
	i.strJpegPath = _tcsdup(ppc->strJpegPath);
	i.lpData = NULL;
	i.ulDataSize = 0;

	LPBYTE buf = NULL;
	if (PAPER_COVERTYPE_CUSTOM == ppc->nType)
	{
		int nCount = 0, nSize;
		FILE *fp = _tfopen(ppc->strJpegPath, _T("rb"));
		if (!fp)
		{
			CPaper::ClearCoverInfo(&i);
			return Q_ERROR(ERROR_FILE_NOT_EXIST);
		}
		
		fseek(fp, 0, SEEK_END);
		nSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		
		buf = (LPBYTE)malloc(nSize);
		if (!buf)
		{
			CPaper::ClearCoverInfo(&i);
			return Q_ERROR(ERROR_ALLOC_FAIL);
		}
		if (!fread(buf, nSize, 1, fp))
		{
			CPaper::ClearCoverInfo(&i);
			free(buf);
			return Q_ERROR(ERROR_READ_FILE_FAIL);
		}
		fclose(fp);
		
		i.ulDataSize = nSize;
		i.lpData = buf;
		
		if (!m_pCurSelPaper->SetCoverInfo(&i))
		{
			CPaper::ClearCoverInfo(&i);
			return Q_ERROR(ERROR_INNER);
		}
	}
	else if (PAPER_COVERTYPE_DEFAULT == ppc->nType)
	{
		HDC hDC = ::GetDC(NULL);
		EBOOL bRet = MakeJpegCover(hDC, &i);
		::ReleaseDC(NULL, hDC);
		
		if (!bRet)
		{
			return Q_ERROR(ERROR_INNER);
		}
		if (!m_pCurSelPaper->SetCoverInfo(&i))
		{
			CPaper::ClearCoverInfo(&i);
			return Q_ERROR(ERROR_INNER);
		}
	}
	else
	{
		if (!m_pCurSelPaper->SetCoverInfo(&i))
		{
			CPaper::ClearCoverInfo(&i);
			return Q_ERROR(ERROR_INNER);
		}
	}

	UpdateAllViews(NULL, HINT_COVER_PAPER, NULL);
	return Q_TRUE;
}

void CDoc::AddNewQuestion(CList *pList)
{
	ASSERT(m_pCurSelPaper);
	m_pCurSelPaper->AddList(pList, GetSelectedList()->GetRawType());

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::EditQuestion(CList *pList)
{
	ASSERT(m_pCurSelPaper);

	// 如果一个题目都没有修改，直接返回
	if (!m_pCurSelPaper->RemoveAllMarkedItems(ITEMMARK_EDIT))
	{
		return;
	}

	if (m_pCurSelType)
	{
		m_pCurSelPaper->AddList(pList, m_pCurSelType->GetRawType());
	}
	else
	{
		m_pCurSelPaper->AddList(pList);
	}
	m_pCurSelPaper->SaveStoreQuestions(NULL);

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::BatchSet(int nIndex)
{
	switch(nIndex)
	{
	case BATCHSET_STOM:
		// 将选中的单选题转换为多选题
		m_pCurSelPaper->SingleChoiseToMultiChoise();
		break;

	case BATCHSET_MTOS:
		// 将选中的单选项多选题转换为单选题
		m_pCurSelPaper->MultiChoiseToSingleChoise();
		break;

	case BATCHSET_CTOJ:
		// 将选中的选择题转换为判断题
		m_pCurSelPaper->ChoiseToJudge();
		break;

	default:
		return;
	}

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::AddNewGroup(CList *pList)
{
	ASSERT(m_pCurSelPaper);

	m_pCurSelPaper->AddList(pList, TYPE_GROUP);
	m_pCurSelPaper->SaveStoreQuestions(NULL);

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::EditGroup(CList *pList)
{
	ASSERT(m_pCurSelPaper);
	m_pCurSelPaper->RemoveAllMarkedItems(ITEMMARK_EDIT);
	m_pCurSelPaper->AddList(pList, TYPE_GROUP);
	m_pCurSelPaper->SaveStoreQuestions(NULL);
	
	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
	}
}

BOOL CDoc::AddCustomType(int nType, LPCTSTR strDefDesc)
{
	ASSERT(m_pCurSelPaper);
	
	CList *pList = m_pCurSelPaper->AddCustomType(nType, strDefDesc);
	if (!pList)
	{
		Q_SHOWERRORMSG_CODE(NULL, IDS_DOC_ADDTYPEFAIL);
		return FALSE;
	}

	m_pCurSelType = pList;
	UpdateAllViews(NULL, HINT_ADD_CUSTOMTYPE, (CObject*)pList);
	return TRUE;
}

BOOL CDoc::DelCustomType()
{
	if (!m_pCurSelType || m_pCurSelType->IsDefaultType())
	{
		return TRUE;
	}

	CList *pDelList = m_pCurSelType;

	// 通知左视图删除列表
	UpdateAllViews(NULL, HINT_DEL_CUSTOMTYPE_L, (CObject*)m_pCurSelType);
	
	if (!m_pCurSelPaper->DelCustomType(pDelList->GetRawType()))
	{
		CTMessageBox Box;
		Box.SuperMessageBox(NULL, IDS_DOC_DELTYPEFAIL, IDS_MSGCAPTION_ERROR,
			MB_OK | MB_ICONERROR, 0, 0);
		return FALSE;
	}

	// 通知右视图重新显示列表
	UpdateAllViews(NULL, HINT_DEL_CUSTOMTYPE_R, (CObject*)m_pCurSelType);

	return TRUE;
}

void CDoc::Store(CList *pList, CPaper *pPaper /* = NULL */)
{
	if (!pPaper)
	{
		ASSERT(m_pCurSelPaper);
		m_pCurSelPaper->SaveStoreQuestions(pList);
		
		UpdateAllViews(NULL, HINT_STORE_QUESTION,
			(CObject*)(m_pCurSelType == m_pCurSelPaper->GetSaveList() ? m_pCurSelType : NULL));
	}
	else
	{
		pPaper->SaveStoreQuestions(pList);
		if (pPaper == m_pCurSelPaper)
		{
			UpdateAllViews(NULL, HINT_STORE_QUESTION,
				(CObject*)(m_pCurSelType == m_pCurSelPaper->GetSaveList() ? m_pCurSelType : NULL));
		}
	}
}

void CDoc::UnStore(CList *pList)
{
	if (!pList)
	{
		return;
	}

	CTMessageBox Box;
	if (IDOK != Box.SuperMessageBox(NULL, IDS_DOC_UNSTOREASK, IDS_MSGCAPTION_ASK,
		MB_OKCANCEL | MB_ICONQUESTION, 0, 0))
	{
		return;
	}

	ASSERT(m_pCurSelPaper);
	m_pCurSelPaper->UnStoreQuestions(pList);

	UpdateAllViews(NULL, HINT_STORE_QUESTION,
		(CObject*)(m_pCurSelType == m_pCurSelPaper->GetSaveList() ? m_pCurSelType : NULL));
}

void CDoc::Copy(CTempList *pList)
{
	ASSERT(pList->GetItemCount());

	// 将题目信息写入剪贴板中
	CFixedSharedFile sf;
	pList->ClipCopy(sf);
	
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_ITEM, sf.Detach());
		CloseClipboard();
	}
}

void CDoc::CopyEx(CTempList *pList, int nCopyStyle)
{
	ASSERT(pList->GetItemCount());
	
	// 将题目文字写入剪贴板中
	CString strMain;
	_stdstring strTemp;
	int i;
	CItem *pItem;
	
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		pItem = pList->GetItem(i);
		switch(nCopyStyle)
		{
		case COPYEX_WITHASW:
			pItem->MakeAnswer(strTemp,
				MKASW_ASWINQUESTION | MKTXT_OPTIONNEWLINE | MKASW_ENDLINE, -1);
			break;

		case COPYEX_ALL:
			pItem->MakeAnswer(strTemp,
				MKASW_ASWINQUESTION | MKTXT_OPTIONNEWLINE | MKASW_ENDLINE | MKASW_WITHRESOLVE, -1);
			break;

		default:
			pItem->MakeText(strTemp,
				MKTXT_OPTIONNEWLINE | MKTXT_ENDLINE | MKTXT_JUDGEBRACKET, 0);
			break;
		}
		strMain += strTemp.c_str();
	}
	if (!strMain.GetLength())
	{
		return;
	}
	
	HGLOBAL hGlobal;
	PTSTR pGlobal;
	
	hGlobal = GlobalAlloc(GHND | GMEM_SHARE, (strMain.GetLength() + 1) * sizeof (TCHAR));
	pGlobal = (PTSTR)GlobalLock(hGlobal);
	
	lstrcpy(pGlobal, strMain);
	GlobalUnlock(hGlobal);
	
	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		SetClipboardData(CF_TCHAR, hGlobal);
		CloseClipboard();
	}
}

void CDoc::Paste()
{
	CList List;
	if (!AfxPasteItem(&List, NULL))
	{
		CTMessageBox Box;
		Box.SuperMessageBox(NULL, IDS_DOC_PASTEFAIL, IDS_MSGCAPTION_ERROR, MB_OK | MB_ICONERROR, 0, 0);

		return;
	}
	if (!List.GetItemCount())
	{
		return;
	}

	ASSERT(m_pCurSelPaper);
	if (m_pCurSelType)
	{
		m_pCurSelPaper->Paste(&List, m_pCurSelType->GetRawType());
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelType);
	}
	else
	{
		m_pCurSelPaper->Paste(&List);
		UpdateAllViews(NULL, HINT_ADD_QUESTION, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::Delete()
{
	ASSERT(m_pCurSelPaper);
	m_pCurSelPaper->RemoveAllMarkedItems(ITEMMARK_DELETE);

	if (m_pCurSelType)
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelType);
	}
	else
	{
		UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
	}
}

void CDoc::Undo()
{
	ASSERT(m_pCurSelPaper);

	TPARAS ListInfo = {0};
	if (m_pCurSelPaper->Undo(&ListInfo))
	{
		if (m_pCurSelType == (CList*)ListInfo.dwPara1 &&
			ListInfo.dwPara2 == UNDO_DELITEM)
		{
			m_pCurSelType = NULL;
		}
		UpdateAllViews(NULL, HINT_UNDO, (CObject*)&ListInfo);
	}
	else
	{
		UpdateAllViews(NULL, HINT_UNDO, NULL);
	}
}

void CDoc::Redo()
{
	ASSERT(m_pCurSelPaper);
	
	TPARAS ListInfo = {0};
	if (m_pCurSelPaper->Redo(&ListInfo))
	{
		if (m_pCurSelType == (CList*)ListInfo.dwPara1 &&
			ListInfo.dwPara2 == UNDO_DELITEM)
		{
			m_pCurSelType = NULL;
		}
		UpdateAllViews(NULL, HINT_REDO, (CObject*)&ListInfo);
	}
	else
	{
		UpdateAllViews(NULL, HINT_REDO, NULL);
	}
}

void CDoc::SetEditable()
{
	ASSERT(m_pCurSelPaper);
	
	BOOL b = m_pCurSelPaper->CanEdit();
	m_pCurSelPaper->SetEdit(!b);

	UpdateAllViews(NULL, HINT_READONLY_PAPER, NULL);
}

void CDoc::SetLock(LPBYTE btPass, LPCSTR strPass)
{
	ASSERT(m_pCurSelPaper);

	m_pCurSelPaper->SetPassword(btPass);
	m_pCurSelPaper->SetPasswordText(strPass);
	UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
}

void CDoc::LoadStore()
{
	OPENFILENAME ofn;
	TCHAR strFilePath[MAX_PATH] = {0};
	LPTSTR strFilter = CTextManager::LoadFileFilter(IDS_DOC_LOADSTOREFILTER);
	if (!strFilter)
	{
		return;
	}
	
	CString strTitle;
	strTitle.LoadString(IDS_DOC_LOADSTORECAPTION);
	
	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = AfxGetMainWnd()->GetSafeHwnd();
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
	ofn.Flags             = OFN_HIDEREADONLY;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = paper_lpszSavExt;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	
	// 获取了文件路径
	if (GetOpenFileName(&ofn))
	{
		ASSERT(m_pCurSelPaper);
		m_pCurSelPaper->LoadSave(strFilePath);
		
		if (m_pCurSelType)
		{
			UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelType);
		}
		else
		{
			UpdateAllViews(NULL, HINT_EDIT_ITEM, (CObject*)m_pCurSelPaper);
		}
	}

	free(strFilter);
}
