// LeftView.cpp : implementation of the CLView class
//

#include "stdafx.h"
#include "QuickQuiz.h"

#include "Doc.h"
#include "LeftView.h"

#include "AddTypeDlg.h"
#include "PasswordDlg.h"
#include "PropertiesDlg.h"
#include "JpegCoverDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static BOOL bFirstSetCustomType = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CLView

IMPLEMENT_DYNCREATE(CLView, CTreeView)

BEGIN_MESSAGE_MAP(CLView, CTreeView)
	//{{AFX_MSG_MAP(CLView)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_DROPFILES()
	ON_NOTIFY_REFLECT(TVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnProperties)
	ON_COMMAND(ID_PAPER_EDITABLE, OnEditable)
	ON_COMMAND(ID_PAPER_LOCK, OnLock)
	ON_COMMAND(ID_PAPER_NEWTYPE, OnNewType)
	ON_COMMAND(ID_PAPER_LOADSTORE, OnEditLoadStore)
	ON_COMMAND(ID_PAPER_EDITOBJECT, OnEditLable)
	ON_COMMAND(ID_PAPER_DELTYPE, OnDelType)
	ON_COMMAND(ID_PAPER_OPENINEXPLORER, OnOpenInExplorer)
	ON_COMMAND(ID_PAPER_COVERSET, OnCoverSet)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_EDITOBJECT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_LOADSTORE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_LOCK, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_NEWTYPE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_DELTYPE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_OPENINEXPLORER, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_COVERSET, OnUpdateCmdUI)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLView construction/destruction

CLView::CLView() :
	m_clrDefaultBK(RGB(255, 255, 255)), m_clrDefaultTxt(RGB(0, 0, 0))
{
	// TODO: add construction code here

}

CLView::~CLView()
{
}

BOOL CLView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= (TVS_SHOWSELALWAYS | TVS_EDITLABELS | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT);

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLView drawing

void CLView::OnDraw(CDC* pDC)
{
	CDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}


void CLView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	// TODO: You may populate your TreeView with items by directly accessing
	//  its tree control through a call to GetTreeCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CLView diagnostics

#ifdef _DEBUG
void CLView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CDoc* CLView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLView message handlers

BOOL CLView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::CreateEx(WS_EX_ACCEPTFILES, lpszClassName, lpszWindowName, dwStyle | WS_CHILD,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU)nID, (LPVOID)pContext))
	{
		return FALSE;
	}

	m_cImageList.Create(IDB_LVIEWICON, 16, 9, RGB(255, 0, 255));
	m_cImageList.SetBkColor(CLR_NONE);
	GetTreeCtrl().SetImageList(&m_cImageList, TVSIL_NORMAL);

	CTreeCtrl &cTree = GetTreeCtrl();
	m_clrDefaultBK = cTree.GetBkColor();
	m_clrDefaultTxt = cTree.GetTextColor();

	return TRUE;
}

void CLView::OnSize(UINT nType, int cx, int cy) 
{
	CTreeView::OnSize(nType, cx, cy);
	
	if (!GetDocument()->GetPaperCount())
	{
		Invalidate(TRUE);
	}
}

void CLView::OnPaint() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc->GetPaperCount())
	{
		CPaintDC paintDC(this); // device context for painting
		HDC dc = paintDC.m_hDC;
		
		CRect rc;
		GetClientRect(rc);

		HFONT hOldFont = (HFONT)SelectObject(dc, (HFONT)SendMessage(WM_GETFONT, 0, 0));
		COLORREF clrOld = SetTextColor(dc, AfxGetQColor(QCOLOR_TEXT));
		int nMode = SetBkMode(dc, TRANSPARENT);

		CString string;
		string.LoadString(IDS_LVIEW_INFO);
		DrawText(dc, string, -1, rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE | DT_WORD_ELLIPSIS);

		SelectObject(dc, hOldFont);
		SetTextColor(dc, clrOld);
		SetBkMode(dc, nMode);
	}
	else
	{
		CTreeView::OnPaint();
	}
}

void CLView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc->GetPaperCount())
	{
		pDoc->OpenPapers();
	}
	if (pDoc->GetPaperCount())
	{
		Invalidate(TRUE);
	}
	
	CTreeView::OnLButtonDblClk(nFlags, point);
}

void CLView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CTreeCtrl &cTree = GetTreeCtrl();

	CMenu *pMainMenu = AfxGetMainWnd()->GetMenu();

	CMenu cMenu;
	UINT uHitFlags;
	HTREEITEM hItem;
	CString strMenuText;

	hItem = cTree.HitTest(point, &uHitFlags);

	if (hItem && (uHitFlags & TVHT_ONITEM)) 
	{
		cTree.Select(hItem, TVGN_CARET);
	}
	
	ClientToScreen(&point);	
	
	if (cMenu.CreatePopupMenu())
	{
		if (uHitFlags & TVHT_ONITEM)
		{
			// 指向了题库
			if (!cTree.GetParentItem(hItem))
			{
				pMainMenu->GetMenuString(ID_PAPER_EDITABLE, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_EDITABLE, strMenuText);

				pMainMenu->GetMenuString(ID_PAPER_LOCK, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_LOCK, strMenuText);

				cMenu.AppendMenu(MF_SEPARATOR);

				pMainMenu->GetMenuString(ID_PAPER_NEWQUESTION, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_NEWQUESTION, strMenuText);

				pMainMenu->GetMenuString(ID_PAPER_EDITOBJECT, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_EDITOBJECT, strMenuText);

				pMainMenu->GetMenuString(ID_PAPER_COVERSET, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_COVERSET, strMenuText);

				pMainMenu->GetMenuString(ID_PAPER_LOADSTORE, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_LOADSTORE, strMenuText);

				cMenu.AppendMenu(MF_SEPARATOR);

				pMainMenu->GetMenuString(ID_PAPER_NEWGROUP, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_NEWGROUP, strMenuText);

				pMainMenu->GetMenuString(ID_PAPER_NEWTYPE, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_NEWTYPE, strMenuText);

				cMenu.AppendMenu(MF_SEPARATOR);

				pMainMenu->GetMenuString(ID_PAPER_OPENINEXPLORER, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_PAPER_OPENINEXPLORER, strMenuText);

				pMainMenu->GetMenuString(ID_EDIT_PROPERTIES, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_EDIT_PROPERTIES, strMenuText);

				pMainMenu->GetMenuString(ID_FILE_CLOSE, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_FILE_CLOSE, strMenuText);
			}
			// 指向了题目类型
			else
			{
				CDoc *pDoc = GetDocument();
				ASSERT(pDoc);
				CList *pList = pDoc->GetSelectedList();

				if (pDoc->IsSelectStore())
				{
					pMainMenu->GetMenuString(ID_PAPER_LOADSTORE, strMenuText, MF_BYCOMMAND);
					cMenu.AppendMenu(nFlags, ID_PAPER_LOADSTORE, strMenuText);
				}
				else if (pList->GetType() == TYPE_GROUP)
				{
					pMainMenu->GetMenuString(ID_PAPER_NEWGROUP, strMenuText, MF_BYCOMMAND);
					cMenu.AppendMenu(nFlags, ID_PAPER_NEWGROUP, strMenuText);
				}
				else
				{
					pMainMenu->GetMenuString(ID_PAPER_NEWQUESTION, strMenuText, MF_BYCOMMAND);
					cMenu.AppendMenu(nFlags, ID_PAPER_NEWQUESTION, strMenuText);
				}

				if (!pDoc->GetSelectedList()->IsDefaultType())
				{
					pMainMenu->GetMenuString(ID_PAPER_DELTYPE, strMenuText, MF_BYCOMMAND);
					cMenu.AppendMenu(nFlags, ID_PAPER_DELTYPE, strMenuText);
				}
				
				cMenu.AppendMenu(MF_SEPARATOR);
				
				pMainMenu->GetMenuString(ID_EDIT_PROPERTIES, strMenuText, MF_BYCOMMAND);
				cMenu.AppendMenu(nFlags, ID_EDIT_PROPERTIES, strMenuText);
			}
		}
		else
		{
			pMainMenu->GetMenuString(ID_FILE_NEW, strMenuText, MF_BYCOMMAND);
			cMenu.AppendMenu(nFlags, ID_FILE_NEW, strMenuText);
		}
	}
	
	if (cMenu.GetMenuItemCount() > 0)
	{
		cMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
	}
}

void CLView::OnDropFiles(HDROP hDropInfo) 
{
	UINT uCount;
	TCHAR szPath[MAX_PATH];
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	
	uCount = DragQueryFile(hDropInfo, -1, NULL, 0);
	if (uCount)
	{
		UINT uIndex;
		for (uIndex = 0; uIndex < uCount; uIndex++)
		{
			DragQueryFile(hDropInfo, uIndex, szPath, _countof(szPath));
			pDoc->OnOpenDocument(szPath);
		}
		
		Invalidate(TRUE);
	}
	
	DragFinish(hDropInfo);
}

void CLView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	SetRedraw(FALSE);

	switch(lHint)
	{
	case HINT_ADD_PAPER:
		{
			ASSERT(pHint);
			AddPaper((CPaper*)pHint);
			if (GetDocument()->GetPaperCount() < 2)
			{
				Invalidate(TRUE);
			}
		}
		break;

	case HINT_SELECT_PAPER:
		GetTreeCtrl().SelectItem(((CPaper*)pHint)->GetGUIHandle());
		break;

	case HINT_CLOSE_PAPER:
		GetTreeCtrl().DeleteItem(((CPaper*)pHint)->GetGUIHandle());
		break;

	case HINT_ADD_QUESTION:
	case HINT_SAVE_PAPER:
	case HINT_EDIT_ITEM:
	case HINT_READONLY_PAPER:
	case HINT_COVER_PAPER:
	case HINT_EDIT_CUSTOMTYPE:
		Invalidate(TRUE);
		break;

	case HINT_RESET_PAPER:
		ResetPaper((TPARAS*)pHint);
		break;

	case HINT_UNDO:
	case HINT_REDO:
		if (pHint)
		{
			ChangeCustomType((TPARAS*)pHint);
		}
		Invalidate(TRUE);
		break;
		
	case HINT_ADD_CUSTOMTYPE:
		AddNewType((CTempList*)pHint, TRUE);
		break;

	case HINT_DEL_CUSTOMTYPE_L:
		DelNewType((CTempList*)pHint);
		break;

	default:
		break;
	}

	SetRedraw(TRUE);
}

void CLView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	TV_ITEM* pItem = &pTVDispInfo->item;
	
	BOOL bIsPaper = (GetTreeCtrl().GetParentItem(pItem->hItem) == NULL);
	CList *pList = (CList*)pItem->lParam;

	if (pItem->mask & TVIF_TEXT)
	{
		if (pList->GetQNumber())
		{
			_sntprintf(pItem->pszText, pItem->cchTextMax, _T("%d:%s"),
				pList->GetQNumber(), pList->GetDescription());
		}
		else
		{
			lstrcpyn(pItem->pszText, pList->GetDescription(), pItem->cchTextMax);
		}
		if (bIsPaper)
		{
			CPaper *pPaper = (CPaper*)pList;
			if(pPaper->IsModify())
			{
				_tcsncat(pItem->pszText, _T("*"), 1);
			}
			if(!pPaper->CanEdit())
			{
				CString str;
				str.LoadString(IDS_LVIEW_READONLY);
				_tcsncat(pItem->pszText, str, str.GetLength());
			}
		}
	}
	if (pItem->mask & TVIF_IMAGE)
	{
		if (bIsPaper)
		{
			CPaper *pPaper = (CPaper*)pList;
			if(pItem->state & TVIS_EXPANDED)
			{
				pItem->iImage = pPaper->IsLocked() ? 3 : 1;
			}
			else
			{
				pItem->iImage = pPaper->IsLocked() ? 2 : 0;
			}
		}
		else if(pList == ((CPaper*)pList->GetParent())->GetSaveList())
		{
			pItem->iImage = 7;
		}
		else if(pList->IsDefaultType())
		{
			if (pList->GetType() == TYPE_GROUP)
			{
				pItem->iImage = 5;
			}
			else
			{
				pItem->iImage = 4;
			}
		}
		else
		{
			pItem->iImage = 6;
		}
	}
	if (pItem->mask & TVIF_SELECTEDIMAGE)
	{
		if (bIsPaper)
		{
			CPaper *pPaper = (CPaper*)pList;
			if(pItem->state & TVIS_EXPANDED)
			{
				pItem->iSelectedImage = pPaper->IsLocked() ? 3 : 1;
			}
			else
			{
				pItem->iSelectedImage = pPaper->IsLocked() ? 2 : 0;
			}
		}
		else if(pList == ((CPaper*)pList->GetParent())->GetSaveList())
		{
			pItem->iSelectedImage = 8;
		}
		else if (pList->IsDefaultType())
		{
			if (pList->GetType() == TYPE_GROUP)
			{
				pItem->iSelectedImage = 5;
			}
			else
			{
				pItem->iSelectedImage = 4;
			}
		}
		else
		{
			pItem->iSelectedImage = 6;
		}
	}
	
	*pResult = 0;
}

void CLView::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* ptv = (NM_TREEVIEW*)pNMHDR;
	
	CTreeCtrl &cTree = GetTreeCtrl ();
	
	ASSERT (ptv->itemNew.mask & TVIF_HANDLE);
	ASSERT (ptv->itemOld.mask & TVIF_HANDLE);
	
	if (ptv->itemOld.hItem)
	{
		cTree.SetItemState (ptv->itemOld.hItem, 0, TVIS_BOLD);
	}
	if (ptv->itemNew.hItem)
	{
		cTree.SetItemState (ptv->itemNew.hItem, TVIS_BOLD, TVIS_BOLD);
	}
	
	CDoc *pDoc = (CDoc*)GetDocument();
	if (pDoc)
	{
		HTREEITEM hParent = cTree.GetParentItem (ptv->itemNew.hItem);
		
		// 选中了题库
		if (hParent == NULL)
		{
			pDoc->SetSelectedPaper((CPaper*)cTree.GetItemData(ptv->itemNew.hItem), NULL);
		}
		// 选中了题库下的题目类型
		else
		{
			pDoc->SetSelectedPaper((CPaper*)cTree.GetItemData(hParent), (CTempList*)cTree.GetItemData(ptv->itemNew.hItem));
		}
	}
	
	*pResult = 0;
}

void CLView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		*pResult = 1;
		return;
	}
	
	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);
	
	if (!pPaper->CanEdit())
	{
		*pResult = 1;
		return;
	}
	
	if (IsSelectPaper())
	{
		CEdit *pEdit = GetTreeCtrl().GetEditControl();
		if (pEdit)
		{
			pEdit->SetWindowText(pPaper->GetDescription());
			pEdit->SetLimitText(MAX_PAPERNAME_LEN);
			*pResult = 0;
			
			return;
		}
	}
	else
	{
		CList *pList = pDoc->GetSelectedType();
		if (!pList->IsDefaultType())
		{
			CEdit *pEdit = GetTreeCtrl().GetEditControl();
			if (pEdit)
			{
				pEdit->SetWindowText(pList->GetDescription());
				pEdit->SetLimitText(MAX_CUSTOMTYPENAME_LEN);
				*pResult = 0;
				
				return;
			}
		}
	}
	
	*pResult = 1;
}

void CLView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
	CList *pItem = (CList*)pTVDispInfo->item.lParam;
	
	if (!pTVDispInfo->item.pszText ||
		!lstrlen(pTVDispInfo->item.pszText) ||
		!CTextManager::CheckValidCharactor(pTVDispInfo->item.pszText))
	{
		*pResult = 0;
		return;
	}

	if (_tcscmp(pItem->GetDescription(), pTVDispInfo->item.pszText))
	{
		if (bFirstSetCustomType)
		{
			pItem->CItem::SetDescription(pTVDispInfo->item.pszText);
		}
		else
		{
			pItem->SetDescription(pTVDispInfo->item.pszText);
		}

		GetDocument()->UpdateAllViews(NULL, HINT_EDIT_CUSTOMTYPE, (CObject*)pItem);
	}
	
	bFirstSetCustomType = FALSE;
	*pResult = 0;
}

void CLView::OnProperties() 
{
	CPaper *pPaper = GetDocument()->GetSelectedPaper();
	ASSERT(pPaper);
	
	if (IsSelectPaper())
	{
		CPaperPropertiesDlg Dlg;
		Dlg.SetPaper(pPaper, FALSE);
		Dlg.DoModal();
	}
	else
	{
		CList *pList = GetDocument()->GetSelectedType();
		ASSERT(pList);

		if (pList->GetType() != TYPE_DEFAULT)
		{
			CTypePropertiesDlg Dlg(pPaper);
			Dlg.SetType(pList);
			Dlg.DoModal();

			GetDocument()->UpdateAllViews(NULL, HINT_EDIT_CUSTOMTYPE);
		}
		else
		{
			// 当点击收藏夹时来到这里
			CPaperPropertiesDlg Dlg;
			Dlg.SetPaper(pPaper, TRUE);
			Dlg.DoModal();
		}
	}
}

void CLView::OnEditable() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	
	pDoc->SetEditable();
}

void CLView::OnLock() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);

	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);
	
	if (pPaper->GetDefSaveStyle() == PAPER_FILETYPE_XML)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(AfxGetMainWnd()->GetSafeHwnd(), IDS_LVIEW_LOCKXMLCAUTION, IDS_MSGCAPTION_INFOR,
			MB_OK | MB_ICONINFORMATION, 0, 0);
		return;
	}
	else
	{
		CPasswordDlg Dlg;
		Dlg.SetPara(pPaper->IsLocked(), pPaper->GetPassword());
		if (IDOK == Dlg.DoModal())
		{
			pDoc->SetLock(Dlg.GetPassword(), Dlg.GetPasswordText());
		}
	}
}

void CLView::OnNewType() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	
	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);

	CAddTypeDlg Dlg;
	int nType = Dlg.DoModal();
	if (!nType)
	{
		return;
	}

	// 默认名称
	CString str1, str2;
	str1.LoadString(IDS_ITEMTYPE_CUSTOM);
	str2.Format(_T("%s%d"), str1, pPaper->GetCustomTypeCount() + 1);

	pDoc->AddCustomType(nType, str2);
}

void CLView::OnEditLoadStore() 
{
	CDoc *pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->LoadStore();
	}
}

void CLView::OnEditLable() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	
	CList *pList = pDoc->GetSelectedType();
	if (pList)
	{
		if (pList->IsDefaultType())
		{
			return;
		}
	}
	else
	{
		pList = (CList*)pDoc->GetSelectedPaper();
	}
	
	TreeView_EditLabel(GetTreeCtrl().GetSafeHwnd(), pList->GetGUIHandle());
}

void CLView::OnDelType() 
{
	CDoc *pDoc = GetDocument();

	if (pDoc)
	{
		CTMessageBox Box;
		if (IDNO == Box.SuperMessageBox(m_hWnd, IDS_LVIEW_ASKDELTYPE, IDS_MSGCAPTION_ASK,
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0,
			pDoc->GetSelectedList()->GetDescription()))
		{
			return;
		}
		
		pDoc->DelCustomType();
	}
}

void CLView::OnOpenInExplorer() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}
	
	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);
	
	CString string;
	string.Format(_T("/select,%s"), pPaper->GetFilePath());
	
	if (ShellExecute(NULL, _T("open"), _T("explorer.exe"), string, NULL, SW_SHOWNORMAL) < (HINSTANCE)32)
	{
		CTMessageBox Box;
		Box.SuperMessageBox(m_hWnd, IDS_LVIEW_OPENEXPLRFAIL, IDS_MSGCAPTION_ERROR,
			MB_OK | MB_ICONERROR, 0, 0);
	}
}

void CLView::OnCoverSet() 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc->GetSelectedPaper());
	
	CJpegCoverDlg Dlg(pDoc->GetSelectedPaper());
	if (IDOK != Dlg.DoModal())
	{
		return;
	}
	if (!Dlg.IsModify())
	{
		return;
	}
	
	if (!pDoc->SetCoverInfo(Dlg.GetCoverInfo()))
	{
		Q_SHOWERRORMSG_CODE(GetSafeHwnd(), IDS_LVIEW_SETCOVERFAIL);
	}
}

void CLView::OnUpdateCmdUI(CCmdUI* pCmdUI) 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);
	
	switch(pCmdUI->m_nID)
	{
	case ID_PAPER_EDITOBJECT:
		{
			CPaper *pPaper = pDoc->GetSelectedPaper();
			CList *pList = pDoc->GetSelectedType();
			if (!pPaper || !pPaper->CanEdit() ||
				(pList && pList->IsDefaultType()))
			{
				pCmdUI->Enable(FALSE);
			}
			else
			{
				pCmdUI->Enable(TRUE);
			}
		}
		break;
		
	case ID_PAPER_LOCK:
		if (!IsSelectPaper())
		{
			pCmdUI->Enable(FALSE);
			break;
		}
		else
		{
			CPaper *pPaper = pDoc->GetSelectedPaper();
			ASSERT(pPaper);
			
			pCmdUI->SetCheck(pPaper->IsLocked());
		}
		break;
		
	case ID_PAPER_LOADSTORE:
		if (IsSelectPaper())
		{
			pCmdUI->Enable(TRUE);
		}
		else
		{
			pCmdUI->Enable(pDoc->IsSelectStore());
		}
		break;
		
	case ID_PAPER_NEWTYPE:
		if (!IsSelectPaper())
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CPaper *pPaper = pDoc->GetSelectedPaper();
			pCmdUI->Enable(pPaper->CanEdit() && pPaper->CanAddCustomType());
		}
		break;
		
	case ID_PAPER_DELTYPE:
		if (IsSelectPaper() || !GetDocument()->GetSelectedList())
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			CList *pList = GetDocument()->GetSelectedType();
			CPaper *pPaper = GetDocument()->GetSelectedPaper();
			pCmdUI->Enable(pPaper->CanEdit() && pList && !pList->IsDefaultType());
		}
		break;
		
	case ID_EDIT_PROPERTIES:
		pCmdUI->Enable((BOOL)GetTreeCtrl().GetSelectedItem());
		break;
		
	case ID_PAPER_OPENINEXPLORER:
		pCmdUI->Enable(IsSelectPaper() && pDoc->GetSelectedPaper()->GetFilePath());
		break;
		
	case ID_PAPER_COVERSET:
		pCmdUI->Enable(IsSelectPaper());
		break;
	}
}

void CLView::UpdateColor()
{
	CTreeCtrl &cTree = GetTreeCtrl();
	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		cTree.SetBkColor(m_clrDefaultBK);
		cTree.SetTextColor(m_clrDefaultTxt);
		break;
		
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		cTree.SetBkColor(AfxGetQColor(QCOLOR_FACE));
		cTree.SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		break;
		
	default:
		ASSERT(FALSE);
	}
}

void CLView::AddPaper(CPaper *pPaper,
					  HTREEITEM hAfter /* = TVI_LAST */,
					  BOOL bSelect /* = TRUE */)
{
	ASSERT (pPaper != NULL);
	
	TV_INSERTSTRUCT tvis;
	HTREEITEM hItem;
	
	CTreeCtrl &cTree = GetTreeCtrl();
	
	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = hAfter;
	tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT;
	
	tvis.item.lParam = (LPARAM)pPaper;
	tvis.item.pszText = LPSTR_TEXTCALLBACK;
	tvis.item.iImage = I_IMAGECALLBACK;
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
	
	hItem = cTree.InsertItem(&tvis);
	pPaper->SetGUIHandle(hItem);

	tvis.hParent = hItem;
	tvis.hInsertAfter = TVI_LAST;

	CTempList *pList = pPaper->GetTypeList(TYPE_SCHOISE);
	while(pList)
	{
		tvis.item.lParam = (LPARAM)pList;

		hItem = cTree.InsertItem(&tvis);
		pList->SetGUIHandle(hItem);

		pList = (CTempList*)pList->GetNext();
	}

	tvis.item.lParam = (LPARAM)pPaper->GetSaveList();
	cTree.InsertItem(&tvis);
	
	if (bSelect)
	{
		cTree.SelectItem(pPaper->GetGUIHandle());
	}
}

void CLView::ResetPaper(TPARAS *pParas)
{
	CPaper *pPaper = (CPaper*)pParas->dwPara1;
	HTREEITEM hDel = (HTREEITEM)pParas->dwPara2;
	HTREEITEM hAfter = (HTREEITEM)pParas->dwPara3;
	CTreeCtrl &cTree = GetTreeCtrl();

	UINT dwState = cTree.GetItemState(hDel, TVIS_EXPANDED);

	cTree.DeleteItem(hDel);
	AddPaper(pPaper, hAfter);

	if(dwState)
	{
		cTree.Expand(pPaper->GetGUIHandle(), TVE_EXPAND);
	}
}

void CLView::AddNewType(CTempList *pList, BOOL bInitEdit)
{
	ASSERT(pList != NULL);
	
	TV_INSERTSTRUCT tvis;
	HTREEITEM hItem;
	
	CTreeCtrl &cTree = GetTreeCtrl();
	CPaper *pPaper = (CPaper*)pList->GetParent();
	ASSERT(pPaper);
	
	tvis.hParent = pPaper->GetGUIHandle();
	tvis.hInsertAfter = pPaper->GetCustomTypeAddAfter(pList)->GetGUIHandle();
	tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM | TVIF_TEXT;
	
	tvis.item.lParam = (LPARAM)pList;
	tvis.item.pszText = LPSTR_TEXTCALLBACK;
	tvis.item.iImage = I_IMAGECALLBACK;
	tvis.item.iSelectedImage = I_IMAGECALLBACK;
	
	hItem = cTree.InsertItem(&tvis);
	pList->SetGUIHandle(hItem);

	if (bInitEdit)
	{
		bFirstSetCustomType = TRUE;

		cTree.Expand(pPaper->GetGUIHandle(), TVE_EXPAND);
		cTree.SelectItem(pList->GetGUIHandle());
		
		TreeView_EditLabel(cTree.GetSafeHwnd(), pList->GetGUIHandle());
	}
}

void CLView::DelNewType(CTempList *pList)
{
	CTreeCtrl &cTree = GetTreeCtrl();
	
	cTree.SelectItem(GetDocument()->GetSelectedPaper()->GetGUIHandle());
	cTree.DeleteItem(pList->GetGUIHandle());

	pList->SetGUIHandle(NULL);
}

BOOL CLView::IsSelectPaper()
{
	CTreeCtrl &cTree = GetTreeCtrl();
	HTREEITEM hItem = cTree.GetSelectedItem();
	if (!hItem)
	{
		return FALSE;
	}
	
	return !cTree.GetParentItem(hItem);
}

void CLView::ChangeCustomType(TPARAS *pListInfo)
{
	ASSERT(pListInfo);

	if (pListInfo->dwPara2 == UNDO_ADDITEM)
	{
		AddNewType((CTempList*)pListInfo->dwPara1, FALSE);
	}
	else
	{
		DelNewType((CTempList*)pListInfo->dwPara1);
	}
}
