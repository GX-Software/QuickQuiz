// View.cpp : implementation of the CQView class
//

#include "stdafx.h"
#include "QuickQuiz.h"

#include "MainFrm.h"
#include "Doc.h"
#include "View.h"

#include "EditDlg.h"
#include "ItemGroupDlg.h"
#include "PropertiesDlg.h"

#include "BatchSetDlg.h"
#include "SummaryLenDlg.h"
#include "CopyExDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define DEFAULTCOLUMNWIDTH	120
#define ASCENDING			0
#define DESCENDING			1
#define GROWLIST			1024
#define SUMMARYLENGTH		80

static LPCTSTR lpszRegSection = _T("Question View");
static LPCTSTR lpszSummaryLen = _T("Summary Len");

WORD CQView::sm_wSortOrder = ASCENDING;
WORD CQView::sm_wSortColumn = 1;
CItem** CQView::sm_pSortedItemList = NULL;
LPTSTR CQView::sm_szTitle[NUMCOLUMNS];

/////////////////////////////////////////////////////////////////////////////
// CQView

IMPLEMENT_DYNCREATE(CQView, CListView)

BEGIN_MESSAGE_MAP(CQView, CListView)
	//{{AFX_MSG_MAP(CQView)
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_COMMAND(ID_EDIT_UNDO, OnUndo)
	ON_COMMAND(ID_EDIT_REDO, OnRedo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
	ON_COMMAND(ID_EDIT_CUT, OnCut)
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_EDIT_COPYEX, OnCopyEx)
	ON_COMMAND(ID_EDIT_PASTE, OnPaste)
	ON_COMMAND(ID_EDIT_CLEAR, OnClear)
	ON_COMMAND(ID_EDIT_FIND, OnFind)
	ON_COMMAND(ID_EDIT_FINDNEXT, OnFindNext)
	ON_COMMAND(ID_PAPER_NEWQUESTION, OnNewQuestion)
	ON_COMMAND(ID_PAPER_EDITOBJECT, OnEditQuestion)
	ON_COMMAND(ID_PAPER_BATCHSET, OnPaperBatchSet)
	ON_COMMAND(ID_PAPER_NEWGROUP, OnNewGroup)
	ON_COMMAND(ID_PAPER_EDITGROUP, OnEditGroup)
	ON_COMMAND(ID_EDIT_PROPERTIES, OnProperties)
	ON_COMMAND(ID_PAPER_STORE, OnEditStore)
	ON_COMMAND(ID_TEST_BEGIN, OnTestBegin)
	ON_COMMAND(ID_TEST_VIEW, OnTestView)
	ON_COMMAND(ID_TEST_MAKE, OnTestMake)
	ON_COMMAND(ID_TEST_COMPETITIVE, OnTestCompetitive)
	ON_COMMAND(ID_VIEW_SAMMARYLEN, OnSammaryLen)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYEX, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FINDNEXT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_EDITOBJECT, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_STORE, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_EDITGROUP, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PROPERTIES, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_PAPER_BATCHSET, OnUpdateCmdUI)
	ON_UPDATE_COMMAND_UI(ID_TEST_BEGIN, OnUpdateCmdUI)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplace)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQView construction/destruction

CQView::CQView() :
	m_nItemCount(0), m_nSortListSize(0),
	m_pFindDlg(NULL), m_dwFindFlags(0),
	m_clrDefaultBK(RGB(255, 255, 255)), m_clrDefaultTxt(RGB(0, 0, 0)), m_clrDefaultTxtBK(RGB(255, 255, 255))
{
	sm_pSortedItemList = NULL;

	ZeroMemory(m_strFind, sizeof(m_strFind));
	ZeroMemory(sm_szTitle, sizeof(sm_szTitle));
}

CQView::~CQView()
{
	if (sm_pSortedItemList)
	{
		free(sm_pSortedItemList);
	}

	int i;
	for (i = 0; i < NUMCOLUMNS; i++)
	{
		if (sm_szTitle[i])
		{
			free(sm_szTitle[i]);
			sm_szTitle[i] = NULL;
		}
	}
}

BOOL CQView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~(LVS_ICON | LVS_SMALLICON | LVS_LIST);
	cs.style |= (LVS_REPORT | LVS_OWNERDATA | LVS_SHOWSELALWAYS);

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CQView drawing

void CQView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}

/////////////////////////////////////////////////////////////////////////////
// CQView diagnostics

#ifdef _DEBUG
void CQView::AssertValid() const
{
	CListView::AssertValid();
}

void CQView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CDoc* CQView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CQView message handlers

BOOL CQView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	if (!CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
	{
		return FALSE;
	}

	m_cImageList.Create(IDB_QVIEWICON, 16, 1, RGB (255, 0, 255));
	m_cImageList.SetBkColor(CLR_NONE);
	GetListCtrl().SetImageList(&m_cImageList, LVSIL_SMALL);

	CWinApp *pApp = AfxGetApp();
	CString strColumnTitle;
	
	int i;
	for (i = 0; i < NUMCOLUMNS; i++)
	{
		TCHAR szNum[8];
		_sntprintf(szNum, 8, _T("%d"), i);
		
		int nWidth = pApp->GetProfileInt(lpszRegSection, szNum, (-1));
		
		switch (i)
		{
		case 0: // 序号
			strColumnTitle.LoadString(IDS_LIST_INDEX);
			if (nWidth < 0)
			{
				nWidth = DEFAULTCOLUMNWIDTH;
			}
			break;
			
		case 1:	// 题目类型
			strColumnTitle.LoadString(IDS_LIST_TYPE);
			if (nWidth < 0)
			{
				nWidth = DEFAULTCOLUMNWIDTH;
			}
			break;
			
		case 2: // 摘要
			strColumnTitle.LoadString(IDS_LIST_SUMMARY);
			if (nWidth < 0)
			{
				nWidth = DEFAULTCOLUMNWIDTH * 2;
			}
			break;
			
		default: // 崩掉
			ASSERT (FALSE);
			break;
		}
		
		sm_szTitle[i] = _tcsdup(strColumnTitle);
		if (sm_wSortColumn == i)
		{
			if (sm_wSortOrder == ASCENDING)
			{
				strColumnTitle += _T("▲");
			}
			else
			{
				strColumnTitle += _T("▼");
			}
		}
		GetListCtrl().InsertColumn(i, strColumnTitle, LVCFMT_LEFT, nWidth);
	}
	m_nSummaryLen = pApp->GetProfileInt(lpszRegSection, lpszSummaryLen, SUMMARYLENGTH);
	
	SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);

	CListCtrl &cList = GetListCtrl();
	m_clrDefaultBK = cList.GetBkColor();
	m_clrDefaultTxt = cList.GetTextColor();
	m_clrDefaultTxtBK = cList.GetTextBkColor();
	
	return TRUE;
}

void CQView::OnDestroy() 
{
	CListView::OnDestroy();
	
	CWinApp *pApp = AfxGetApp();
	if (!g_bReset)
	{
		int i;
		for (i = 0; i < NUMCOLUMNS; i++)
		{
			TCHAR szNum[8];
			_sntprintf(szNum, 8, _T("%d"), i);
			
			pApp->WriteProfileInt(lpszRegSection, szNum, GetListCtrl().GetColumnWidth(i));
		}
	}

	pApp->WriteProfileInt(lpszRegSection, lpszSummaryLen, m_nSummaryLen);
}

void CQView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;

	if (!sm_pSortedItemList || !m_nSortListSize)
	{
		return;
	}
	if (plvItem->iItem >= m_nItemCount)
	{
		return;
	}

	CItem *pItem = sm_pSortedItemList[plvItem->iItem];
	ASSERT(pItem);

	if (plvItem->mask & LVIF_TEXT)
	{
		switch(plvItem->iSubItem)
		{
		case 0: // 序号
			if (pItem->GetType() == TYPE_GROUP)
			{
				CItemGroup *pGroup = (CItemGroup*)pItem;
				if (pGroup->GetQNumber())
				{
					_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d(%d)"),
						plvItem->iItem + 1, pGroup->GetQNumber());
				}
				else
				{
					_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d"),
						plvItem->iItem + 1);
				}
			}
			else
			{
				CDoc *pDoc = GetDocument();
				CPaper *pPaper = pDoc->GetSelectedPaper();
				ASSERT(pPaper);
				CTempList *pList = pPaper->GetTypeList(pItem->GetRawType());
				if (pList->GetQNumber())
				{
					_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d(%d)"),
						plvItem->iItem + 1, pList->GetQNumber());
				}
				else
				{
					_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d"),
						plvItem->iItem + 1);
				}
			}
			break;

		case 1: // 题目类型
			_tcscpyn(plvItem->pszText, pItem->GetTypeDesc(), plvItem->cchTextMax);
			break;

		case 2: // 摘要
			{
				int nLen = pItem->GetDescriptionSummary(m_nSummaryLen);

				_stdstring str;
				pItem->MakeText(str, MKTXT_OPTIONSINALINE, 0);

				lstrcpyn(plvItem->pszText, str.c_str(), min(nLen + 1, plvItem->cchTextMax));
			}
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
	if (plvItem->mask & LVIF_IMAGE)
	{
		if (pItem->IsStored())
		{
			plvItem->iImage = 0;
		}
	}
	
	*pResult = 0;
}

void CQView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// 防止闪烁
	SetRedraw(FALSE);

	switch(lHint)
	{
	case HINT_STORE_QUESTION:
		if (!pHint)
		{
			RedrawIcons();
			break;
		}

	case HINT_ADD_PAPER:
	case HINT_SELECT_PAPER:
	case HINT_EDIT_ITEM:
	case HINT_SELECT_TYPE:
	case HINT_ADD_QUESTION:
		ASSERT(pHint);
		ResetItems((CList*)pHint, TRUE);
		break;

	case HINT_RESET_PAPER:
		ASSERT(pHint);
		ResetItems((CList*)((TPARAS*)pHint)->dwPara1, TRUE);
		break;

	case HINT_UNDO:
	case HINT_REDO:
	case HINT_ADD_CUSTOMTYPE:
		ResetItems(GetDocument()->GetSelectedList(), TRUE);
		break;

	case HINT_CLOSE_PAPER:
		if (GetDocument()->GetPaperCount() <= 1)
		{
			DeleteAllItems();
			((CMain*)AfxGetMainWnd())->SetItemCount(m_nItemCount);
		}
		break;

	case HINT_DEL_CUSTOMTYPE_R:
		ResetItems(GetDocument()->GetSelectedList(), TRUE);
		break;

	case HINT_EDIT_CUSTOMTYPE:
		Invalidate();
		break;
	}

	SetRedraw(TRUE);
}

BOOL CQView::SetColumnText(int nIndex, LPCTSTR szText)
{
	LVCOLUMN col;
	col.mask = LVCF_TEXT;
	col.pszText = (LPTSTR)szText;

	return GetListCtrl().SetColumn(nIndex, &col);
}

/*
功能：	重置题目列表中的题目
返回值：无
备注：	传入的pList为题库或题库下某一类别列表
*/
void CQView::ResetItems(CList *pList, BOOL bSort)
{
	CListCtrl &cList = GetListCtrl();
	if (cList.GetItemCount())
	{
		DeleteAllItems();
	}
	
	CPaper *pPaper;
	int nItemCount;
	if (pList->IsPaper())
	{
		pPaper = (CPaper*)pList;
		nItemCount = pPaper->GetItemCount();
	}
	else if (pList->IsTempList())
	{
		nItemCount = pList->GetItemCount();
	}
	else
	{
		pPaper = (CPaper*)pList->GetParent();
		nItemCount = pPaper->GetItemCount(pList->GetRawType());
	}
	if (m_nItemCount + nItemCount > m_nSortListSize)
	{
		while(m_nItemCount + nItemCount > m_nSortListSize)
		{
			m_nSortListSize += GROWLIST;
		}
		
		sm_pSortedItemList = (CItem**)realloc(sm_pSortedItemList, sizeof(CItem*) * m_nSortListSize);
	}

	int i;
	if (pList->IsPaper())
	{
		for (i = 0; i < nItemCount; i++)
		{
			sm_pSortedItemList[m_nItemCount++] = pPaper->GetItem(i);
		}
	}
	else if (pList->IsTempList())
	{
		for (i = 0; i < nItemCount; i++)
		{
			sm_pSortedItemList[m_nItemCount++] = pList->GetItem(i);
		}
	}
	else
	{
		for (i = 0; i < nItemCount; i++)
		{
			sm_pSortedItemList[m_nItemCount++] = pPaper->GetItem(pList->GetRawType(), i);
		}
	}

	cList.SetItemCountEx(m_nItemCount, LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
	
	// 当按照序号列进行排序时，就不排序了
	if (bSort && 0 != sm_wSortColumn)
	{
		SortList();
	}
	
	((CMain*)AfxGetMainWnd())->SetItemCount(m_nItemCount);
}

BOOL CQView::CheckEditable()
{
	CDoc *pDoc = GetDocument();
	CPaper *pPaper = pDoc->GetSelectedPaper();

	if (pDoc->IsSelectStore() || !pPaper || !pPaper->CanEdit() ||
		!GetListCtrl().GetSelectedCount())
	{
		return FALSE;
	}

	CTempList tmpList;
	if (!GetSelectedItems(&tmpList, ITEMMARK_NULL))
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < tmpList.GetItemCount(); ++i)
	{
		CItem *pItem = tmpList.GetItem(i);
		if (pItem->GetType() != TYPE_GROUP)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CQView::CheckGroupEditable()
{
	CDoc *pDoc = GetDocument();
	CPaper *pPaper = pDoc->GetSelectedPaper();
	
	if (pDoc->IsSelectStore() || !pPaper || !pPaper->CanEdit() ||
		!GetListCtrl().GetSelectedCount())
	{
		return FALSE;
	}
	
	CTempList tmpList;
	if (!GetSelectedItems(&tmpList, ITEMMARK_NULL))
	{
		return FALSE;
	}
	
	int i;
	for (i = 0; i < tmpList.GetItemCount(); ++i)
	{
		CItem *pItem = tmpList.GetItem(i);
		if (pItem->GetType() == TYPE_GROUP)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CQView::DeleteAllItems()
{
	CListCtrl &cList = GetListCtrl();
	
	ZeroMemory(sm_pSortedItemList, sizeof(CItem*) * m_nSortListSize);
	m_nItemCount = 0;
	cList.SetItemCountEx(0, LVSICF_NOSCROLL);
}

void CQView::ClearAllStates(BOOL bClearSelected, BOOL bClearFocused)
{
	int nSelIndex;
	CListCtrl &ctList = GetListCtrl();
	
	if (bClearSelected)
	{
		nSelIndex = ctList.GetNextItem (-1, LVNI_ALL | LVNI_SELECTED);
		while (nSelIndex >= 0)
		{
			ctList.SetItemState (nSelIndex, NULL, LVIS_SELECTED);
			nSelIndex = ctList.GetNextItem (nSelIndex, LVNI_ALL | LVNI_SELECTED);
		}
	}
	if (bClearFocused)
	{
		nSelIndex = ctList.GetNextItem (-1, LVNI_ALL | LVNI_FOCUSED);
		while (nSelIndex >= 0)
		{
			ctList.SetItemState (nSelIndex, NULL, LVNI_FOCUSED);
			nSelIndex = ctList.GetNextItem (nSelIndex, LVNI_ALL | LVNI_FOCUSED);
		}
	}
}

void CQView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CMenu cMenu;
	CListCtrl &cList = GetListCtrl();

	// 在选中两个以下的前提下才有效
	if (cList.GetSelectedCount() < 2)
	{
		UINT uHitFlags;
		int nItem = cList.HitTest(point, &uHitFlags);
		
		if (nItem >= 0 && (uHitFlags & LVHT_ONITEM)) // 选中某项
		{
			ClearAllStates(TRUE, TRUE);

			cList.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		}
	}
	
	ClientToScreen(&point);	
	if (cMenu.CreatePopupMenu())
	{
		CMenu *pMainMenu = AfxGetMainWnd()->GetMenu();
		CString strMenuText;
		
		int nFlags = MF_ENABLED | MF_STRING;
		int nSelectedCount = cList.GetSelectedCount();
		
		CList *pSelPaper = GetDocument()->GetSelectedPaper();
		if (!pSelPaper)
		{
			return;
		}

		pMainMenu->GetMenuString(ID_EDIT_SELECT_ALL, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_SELECT_ALL, strMenuText);
		
		cMenu.AppendMenu(MF_SEPARATOR);

		pMainMenu->GetMenuString(ID_EDIT_CUT, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_CUT, strMenuText);

		pMainMenu->GetMenuString(ID_EDIT_COPY, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_COPY, strMenuText);

		pMainMenu->GetMenuString(ID_EDIT_COPYEX, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_COPYEX, strMenuText);

		pMainMenu->GetMenuString(ID_EDIT_PASTE, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_PASTE, strMenuText);

		pMainMenu->GetMenuString(ID_EDIT_CLEAR, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_CLEAR, strMenuText);
			
		cMenu.AppendMenu(MF_SEPARATOR);

		pMainMenu->GetMenuString(ID_PAPER_NEWQUESTION, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_PAPER_NEWQUESTION, strMenuText);

		pMainMenu->GetMenuString(ID_PAPER_STORE, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_PAPER_STORE, strMenuText);
		
		pMainMenu->GetMenuString(ID_PAPER_EDITOBJECT, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_PAPER_EDITOBJECT, strMenuText);

		cMenu.AppendMenu(MF_SEPARATOR);

		pMainMenu->GetMenuString(ID_PAPER_NEWGROUP, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_PAPER_NEWGROUP, strMenuText);

		pMainMenu->GetMenuString(ID_PAPER_EDITGROUP, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_PAPER_EDITGROUP, strMenuText);

		cMenu.AppendMenu(MF_SEPARATOR);

		pMainMenu->GetMenuString(ID_EDIT_PROPERTIES, strMenuText, MF_BYCOMMAND);
		cMenu.AppendMenu(nFlags, ID_EDIT_PROPERTIES, strMenuText);
		
		if (cMenu.GetMenuItemCount())
		{
			cMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd ());
		}
	}
}

void CQView::OnNewQuestion() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	// 禁止采用编辑框的方式新增题目组
	ASSERT(pDoc->GetSelectedPaper());
	if (pDoc->GetSelectedType())
	{
		if (pDoc->GetSelectedType()->GetRawType() == TYPE_GROUP)
		{
			return;
		}
	}
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CEditDlg Dlg;
	Dlg.SetEditPara(NULL, pDoc->GetSelectedList(), pDoc->GetSelectedList()->GetRawType());
	if (Dlg.DoModal() != IDOK)
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	CList *pList = Dlg.GetList();
	if (!pList->GetItemCount())
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	pDoc->AddNewQuestion(pList);
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void CQView::OnEditStore() 
{
	CTempList List;
	GetSelectedItems(&List, ITEMMARK_NULL);

	// 全都保存了的话，就取消所有保存的
	// 否则保存全部
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);

	if (List.IsAllStored())
	{
		pDoc->UnStore(&List);
	}
	else
	{
		pDoc->Store(&List);
	}
}

void CQView::OnEditQuestion() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	// 策略是将所有选中的题目复制其副本，并在原列表中进行标记
	// 如果编辑过了，就将编辑的各题目重新加入列表中
	// 如果没有编辑过，就清除所有标记
	CList List;
	if (!GetSelectedItems(&List, ITEMMARK_EDIT))
	{
		List.SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
		return;
	}

	// 删除被选中的题目组
	int i;
	for (i = 0; i < List.GetItemCount();)
	{
		CItem *pItem = List.GetItem(i);
		if (pItem->GetType() == TYPE_GROUP)
		{
			ASSERT(pItem->GetItemFrom()->GetItemState(ITEMMARK_EDIT));
			pItem->GetItemFrom()->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			List.RemoveItem(pItem, TRUE);
		}
		else
		{
			++i;
		}
	}
	if (!List.GetItemCount())
	{
		return;
	}

	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CEditDlg Dlg;
	Dlg.SetEditPara(&List, pDoc->GetSelectedList(), pDoc->GetSelectedList()->GetRawType());
	if (Dlg.DoModal() != IDOK || !Dlg.IsEdited())
	{
		pDoc->GetSelectedPaper()->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	if (List.GetItemCount())
	{
		pDoc->EditQuestion(&List);
	}
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void CQView::OnPaperBatchSet() 
{
	CDoc *pDoc = GetDocument();
	CBatchSetDlg Dlg(pDoc->GetSelectedPaper()->CanEdit());

	if (IDOK != Dlg.DoModal())
	{
		return;
	}

	CWaitCursor ws;

	switch(Dlg.m_nIndex)
	{
	case BATCHSET_STOM:
	case BATCHSET_MTOS:
	case BATCHSET_CTOJ:
		if (pDoc)
		{
			GetSelectedItems(NULL, ITEMMARK_EDIT);
			pDoc->BatchSet(Dlg.m_nIndex);
		}
		break;

	case BATCHSET_SELDUPLICATE:
		CheckDuplicate(Dlg.m_nSimilar);
		break;

	default:
		break;
	}
}

void CQView::OnNewGroup() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}

	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);
	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CItemGroupDlg Dlg(NULL, pPaper);
	if (IDOK != Dlg.DoModal())
	{
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}
	
	pDoc->AddNewGroup(Dlg.GetItemGroup());
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void CQView::OnEditGroup() 
{
	CDoc *pDoc = GetDocument();
	if (!pDoc)
	{
		return;
	}
	CPaper *pPaper = pDoc->GetSelectedPaper();
	ASSERT(pPaper);

	CList List;
	if (!GetSelectedItems(&List, ITEMMARK_EDIT))
	{
		List.SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
		return;
	}

	// 删除被选中的非题目组题目
	int i = 0;
	CItem *pItem;
	while(1)
	{
		pItem = List.GetItem(i);
		if (!pItem)
		{
			break;
		}

		if (pItem->GetType() != TYPE_GROUP)
		{
			ASSERT(pItem->GetItemFrom()->GetItemState(ITEMMARK_EDIT));
			pItem->GetItemFrom()->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			List.RemoveItem(pItem, TRUE);
		}
		else
		{
			++i;
		}
	}
	if (!List.GetItemCount())
	{
		return;
	}

	AfxGetMainWnd()->ShowWindow(SW_HIDE);

	CItemGroupDlg Dlg(&List, pPaper);
	if (IDOK != Dlg.DoModal())
	{
		pDoc->Store(NULL);
		pDoc->GetSelectedPaper()->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
		AfxGetMainWnd()->ShowWindow(SW_SHOW);
		return;
	}

	if (Dlg.IsEdited())
	{
		pDoc->EditGroup(&List);
	}
	AfxGetMainWnd()->ShowWindow(SW_SHOW);
}

void CQView::OnProperties() 
{
	ShowQuestions();
}

void CQView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl &cList = GetListCtrl();

	UINT uHitFlags;
	int nItem = cList.HitTest(point, &uHitFlags);
	
	if (nItem >= 0 && (uHitFlags & LVHT_ONITEM)) // 选中某项
	{
		ShowQuestions();
	}
	else
	{
		CListView::OnLButtonDblClk(nFlags, point);
	}
}

void CQView::ShowQuestions()
{
	CTempList List;
	CListCtrl &cListCtrl = GetListCtrl();
	UINT ulCount = cListCtrl.GetSelectedCount();

	// 当仅选中一个时，等效于选中全部，可以来回翻动
	if (ulCount == 1)
	{
		int i;
		int nSel = cListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		for (i = 0; i < m_nItemCount; i++)
		{
			List.AddItem(sm_pSortedItemList[i]);
		}
		
		CItemPropertiesDlg Dlg(&List, cListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED));
		Dlg.DoModal();
	}
	else
	{
		GetSelectedItems(&List, ITEMMARK_NULL);

		CItemPropertiesDlg Dlg(&List);
		Dlg.DoModal();
	}

	Invalidate(TRUE);
}

UINT CQView::GetSelectedItems(CList *pList, UINT nMark)
{
	CListCtrl &cListCtrl = GetListCtrl();
	UINT ulSelectedCount = cListCtrl.GetSelectedCount();
	BOOL bGetCustom = FALSE;
	
	if (ulSelectedCount)
	{
		CDoc *pDoc = GetDocument();
		ASSERT(pDoc->GetSelectedList());
		int nSelIndex = cListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
		while (nSelIndex >= 0)
		{
			if (ITEMMARK_EDIT & nMark)
			{
				if (!sm_pSortedItemList[nSelIndex]->IsDefaultType() &&
					pDoc->GetSelectedList()->IsDefaultType())
				{
					if (!bGetCustom)
					{
						CTMessageBox Box;
						Box.SuperMessageBox(m_hWnd, IDS_QVIEW_FINDCUSTEOM, IDS_MSGCAPTION_INFOR,
							MB_OK | MB_ICONINFORMATION, 0, 0);
					}
					bGetCustom = TRUE;
				}
				else
				{
					sm_pSortedItemList[nSelIndex]->SetItemState(ITEMMARK_EDIT, ITEMMARK_EDIT);
					if (pList)
					{
						CItem *pItem = sm_pSortedItemList[nSelIndex]->Alloc();
						if (!pItem)
						{
							return pList->GetItemCount();
						}
						pItem->SetStore(sm_pSortedItemList[nSelIndex]->IsStored());
						pList->AddItem(pItem);
					}
				}
			}
			else if (ITEMMARK_DELETE & nMark)
			{
				sm_pSortedItemList[nSelIndex]->SetItemState(ITEMMARK_DELETE, ITEMMARK_DELETE);
			}
			else if (pList)
			{
				ASSERT(pList->IsTempList());
				pList->AddItem(sm_pSortedItemList[nSelIndex]);
			}
			
			nSelIndex = cListCtrl.GetNextItem(nSelIndex, LVNI_ALL | LVNI_SELECTED);
		}

		if (pList && !bGetCustom)
		{
			ASSERT(ulSelectedCount == (UINT)pList->GetItemCount());
		}
	}
	
	return ulSelectedCount;
}

void CQView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CWaitCursor ws;

	WORD wColumn = ((NM_LISTVIEW *)pNMHDR)->iSubItem;
	if (wColumn == sm_wSortColumn)
	{
		sm_wSortOrder = !sm_wSortOrder;
	}
	else
	{
		SetColumnText(sm_wSortColumn, sm_szTitle[sm_wSortColumn]);
		sm_wSortColumn = wColumn;
	}
	
	CString str;
	str = sm_szTitle[wColumn];
	if (sm_wSortOrder == ASCENDING)
	{
		str += _T("▲");
	}
	else
	{
		str += _T("▼");
	}
	SetColumnText(sm_wSortColumn, str);

	SortList();
	*pResult = 0;
}

void CQView::SortList()
{
	if (!m_nItemCount)
	{
		return;
	}

	CItem *pFocus = NULL;
	CListCtrl &cList = GetListCtrl();
	int nSelectCount = cList.GetSelectedCount();

	// 需要保存之前选中的情况
	CItem **pList = (CItem**)malloc(sizeof(CItem*) * nSelectCount);
	if (!pList)
	{
		return;
	}

	int i = 0;
	// 保存所有选中的内容
	if (nSelectCount)
	{
		int nSel = cList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

		if (cList.GetItemState (nSel, LVIS_FOCUSED) == LVIS_FOCUSED)
		{
			pFocus = sm_pSortedItemList[nSel];
		}

		while(nSel >= 0)
		{
			if (cList.GetItemState (nSel, LVIS_FOCUSED) == LVIS_FOCUSED)
			{
				pFocus = sm_pSortedItemList[nSel];
			}
			cList.SetItemState(nSel, 0, LVIS_SELECTED | LVIS_FOCUSED);
			pList[i++] = sm_pSortedItemList[nSel];
			ASSERT(i <= nSelectCount);

			nSel = cList.GetNextItem(nSel, LVNI_ALL | LVNI_SELECTED);
		}
	}

	switch(sm_wSortColumn)
	{
	case 0:
		ResetItems(GetDocument()->GetSelectedList(), FALSE);
		if (DESCENDING == sm_wSortOrder)
		{
			int i;
			CItem *pItem;
			for (i = 0; i < m_nItemCount >> 1; i++)
			{
				pItem = sm_pSortedItemList[i];
				sm_pSortedItemList[i] = sm_pSortedItemList[m_nItemCount - i - 1];
				sm_pSortedItemList[m_nItemCount - i - 1] = pItem;
			}
		}
		break;

	default:
		qsort((void*)sm_pSortedItemList, m_nItemCount, sizeof(sm_pSortedItemList[0]), CompareItems);
	}

	// 恢复所有选中的内容
	if (nSelectCount)
	{
		int j;
		for (i = 0; i < nSelectCount; i++)
		{
			for (j = 0; j < m_nItemCount; j++)
			{
				if (sm_pSortedItemList[j] == pList[i])
				{
					if (pList[i] == pFocus)
					{
						cList.EnsureVisible(j, FALSE);
						cList.SetItemState(j, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
					}
					else
					{
						cList.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
					}

					break;
				}
			}
		}
	}

	free(pList);
	cList.Invalidate(FALSE);
}

int CQView::CompareItems(const void *arg1, const void *arg2)
{
	int ret = 0;

	CItem* pItem1 = (CItem*)(*(DWORD*)arg1);
	CItem* pItem2 = (CItem*)(*(DWORD*)arg2);

	switch(sm_wSortColumn)
	{
		// 序号按加入顺序排序
	case 0:
		ASSERT(FALSE);
		break;

		// 题目类型
	case 1:
		{
			int nType1 = pItem1->GetRawType();
			int nType2 = pItem2->GetRawType();
			if (nType1 > nType2)
			{
				ret = 1;
			}
			else if (nType1 < nType2)
			{
				ret = -1;
			}
			else
			{
				// 当类型相同时，按题目名称排序
				ret = lstrcmp(pItem1->GetDescription(), pItem2->GetDescription());
				// 当题目名称相同时，按题目解析排序（有解析的排前面）
				if (!ret)
				{
					ret = -lstrcmp(pItem1->GetResolve(), pItem2->GetResolve());
				}
				// 当名称和解析完全相同时，按加入的先后排序
				if (!ret)
				{
					nType1 = ((CList*)pItem1->GetParent())->GetIndex(pItem1);
					nType2 = ((CList*)pItem2->GetParent())->GetIndex(pItem2);
					if (nType1 > nType2)
					{
						ret = 1;
					}
					else if (nType1 < nType2)
					{
						ret = -1;
					}
					else
					{
						ret = 0;
					}
				}
			}
		}
		break;

		// 题目名称
	case 2:
		ret = lstrcmp(pItem1->GetDescription(), pItem2->GetDescription());
		// 题目名称相同时，按题目类型排序
		if (!ret)
		{
			int nType1 = pItem1->GetRawType();
			int nType2 = pItem2->GetRawType();
			if (nType1 > nType2)
			{
				ret = 1;
			}
			else if (nType1 < nType2)
			{
				ret = -1;
			}
			else
			{
				// 类型相同时，按题目解析排序（有解析的排前面）
				ret = -lstrcmp(pItem1->GetResolve(), pItem2->GetResolve());
				// 当名称和解析完全相同时，按加入的先后排序
				if (!ret)
				{
					nType1 = ((CList*)pItem1->GetParent())->GetIndex(pItem1);
					nType2 = ((CList*)pItem2->GetParent())->GetIndex(pItem2);
					if (nType1 > nType2)
					{
						ret = 1;
					}
					else if (nType1 < nType2)
					{
						ret = -1;
					}
					else
					{
						ret = 0;
					}
				}
			}
		}
		break;

	default:
		ASSERT(FALSE);
	}

	// 逆序
	if (DESCENDING == sm_wSortOrder)
	{
		ret = -ret;
	}

	return ret;
}

void CQView::OnUndo() 
{
	GetDocument()->Undo();
}

void CQView::OnRedo() 
{
	GetDocument()->Redo();
}

void CQView::OnSelectAll() 
{
	int i;
	CListCtrl &cList = GetListCtrl();

	for (i = 0; i < m_nItemCount; i++)
	{
		cList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}
}

void CQView::OnCut() 
{
	OnCopy();
	
	OnClear();
}

void CQView::OnCopy() 
{
	CTempList List;
	if (!GetSelectedItems(&List, ITEMMARK_NULL))
	{
		return;
	}

	// 复制内容
	GetDocument()->Copy(&List);
}

void CQView::OnCopyEx() 
{
	CCopyExDlg Dlg;
	if (IDOK != Dlg.DoModal())
	{
		return;
	}

	CTempList List;
	if (!GetSelectedItems(&List, ITEMMARK_NULL))
	{
		return;
	}
	
	// 复制内容
	GetDocument()->CopyEx(&List, Dlg.m_nSel);
}

void CQView::OnPaste() 
{
	GetDocument()->Paste();
}

void CQView::OnClear() 
{
	CTMessageBox Box;
	if (IDYES != Box.SuperMessageBox(m_hWnd, IDS_QVIEW_DELASK, IDS_MSGCAPTION_ASK,
		MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2, 0, 0))
	{
		return;
	}

	// 为要清除的题目做标记
	UINT nItemCount = GetSelectedItems(NULL, ITEMMARK_DELETE);

	// 清除
	GetDocument()->Delete();
}

void CQView::OnFind() 
{
	if(m_pFindDlg) // 判断是否已存在一个对话框
	{
		m_pFindDlg->SetActiveWindow(); // 若替换对话框已打开，则使之成为活动窗口
		return;
	}

	m_pFindDlg = new CFindReplaceDialog;
	if (m_pFindDlg)
	{
		m_pFindDlg->Create(TRUE, m_strFind, NULL, FR_HIDEWHOLEWORD | FR_DOWN, this);
	}
}

void CQView::OnFindNext()
{
	FindItem();
}

LRESULT CQView::OnFindReplace(WPARAM wParam, LPARAM lParam)
{
	LPFINDREPLACE ptr = (LPFINDREPLACE)lParam;

	if (ptr->Flags & FR_DIALOGTERM)
	{
		m_pFindDlg = NULL;
	}
	else if (ptr->Flags & FR_FINDNEXT)
	{
		ASSERT(m_pFindDlg);
		// 保存查找信息
		_tcscpyn(m_strFind, m_pFindDlg->m_fr.lpstrFindWhat, MAX_STRING_LEN);
		m_dwFindFlags = m_pFindDlg->m_fr.Flags;
		FindItem();
	}

	return 0;
}

void CQView::OnUpdateCmdUI(CCmdUI* pCmdUI) 
{
	CDoc *pDoc = GetDocument();
	ASSERT(pDoc);

	CPaper *pPaper = pDoc->GetSelectedPaper();

	switch(pCmdUI->m_nID)
	{
	case ID_EDIT_UNDO:
		if (!pPaper)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(pPaper->CanUndo());
		}
		break;

	case ID_EDIT_REDO:
		if (!pPaper)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(pPaper->CanRedo());
		}
		break;

	case ID_EDIT_CUT:
	case ID_EDIT_CLEAR:
		if (!pPaper)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(!pDoc->IsSelectStore() && pPaper->CanEdit() && GetListCtrl().GetSelectedCount());
		}
		break;

	case ID_PAPER_EDITOBJECT:
		pCmdUI->Enable(CheckEditable());
		break;

	case ID_PAPER_BATCHSET:
		if (!pPaper)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(pPaper->GetItemCount() > 0);
		}
		break;

	case ID_PAPER_EDITGROUP:
		pCmdUI->Enable(CheckGroupEditable());
		break;

	case ID_PAPER_STORE:
		{
			// 没有选中则无效
			if (!GetListCtrl().GetSelectedCount())
			{
				pCmdUI->Enable(FALSE);
				break;
			}

			CTempList List;
			GetSelectedItems(&List, ITEMMARK_NULL);
			ASSERT(List.GetItemCount());

			CString string;
			if (List.IsAllStored())
			{
				string.LoadString(IDS_QVIEW_MENUUNSTORE);
			}
			else
			{
				string.LoadString(IDS_QVIEW_MENUSTORE);
			}
			pCmdUI->SetText(string);
		}
		break;

	case ID_EDIT_SELECT_ALL:
		pCmdUI->Enable(m_nItemCount);
		break;

	case ID_EDIT_COPY:
	case ID_EDIT_COPYEX:
	case ID_EDIT_PROPERTIES:
		pCmdUI->Enable(GetListCtrl().GetSelectedCount());
		break;
		
	case ID_EDIT_PASTE:
		if (!pPaper)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(!pDoc->IsSelectStore() && pPaper->CanEdit() && IsClipboardFormatAvailable(CF_ITEM));
		}
		break;

	case ID_EDIT_FIND:
		pCmdUI->Enable(m_nItemCount);
		break;

	case ID_EDIT_FINDNEXT:
		pCmdUI->Enable(m_nItemCount && !m_pFindDlg && m_strFind[0]);
		break;

	case ID_TEST_BEGIN:
		pCmdUI->Enable(pDoc->GetPaperCount() && pDoc->GetSelectedPaper()->GetItemCount());
		break;
	}
}

void CQView::FindItem()
{
	CListCtrl &cListCtrl = GetListCtrl();
	int iOffset = 0, iOffsetSave;
	
	if (!m_nItemCount)
	{
		return;
	}

	// 找到查找的起始点
	iOffset = cListCtrl.GetNextItem(-1, LVNI_SELECTED);
	if (iOffset < 0)
	{
		iOffset = 0;
	}
	iOffsetSave = iOffset;

	while(1)
	{
		if (m_dwFindFlags & FR_DOWN)
		{
			iOffset++;
			if (iOffset >= m_nItemCount)
			{
				iOffset = 0;
			}
		}
		else
		{
			iOffset--;
			if (iOffset < 0)
			{
				iOffset = (m_nItemCount - 1);
			}
		}

		ASSERT(sm_pSortedItemList[iOffset]);

		if (sm_pSortedItemList[iOffset]->Match(m_strFind, m_dwFindFlags & FR_MATCHCASE))
		{
			break;
		}

		if (iOffset == iOffsetSave)
		{
			CTMessageBox Box;
			Box.SuperMessageBox(m_hWnd, IDS_QVIEW_FINDFAIL, IDS_MSGCAPTION_INFOR,
				MB_OK | MB_ICONINFORMATION, 0, 0);

			if (m_pFindDlg)
			{
				m_pFindDlg->SetActiveWindow();
			}
			return;
		}
	}

	// 找到了符合条件的题目
	ClearAllStates(TRUE, FALSE);
	cListCtrl.SetItemState(iOffset, LVIS_SELECTED, LVIS_SELECTED);
	cListCtrl.EnsureVisible(iOffset, FALSE);
}

void CQView::UpdateColor()
{
	CListCtrl &cList = GetListCtrl();
	switch(g_nColorType)
	{
	case CLRTYPE_DEFAULT:
		cList.SetBkColor(m_clrDefaultBK);
		cList.SetTextColor(m_clrDefaultTxt);
		cList.SetTextBkColor(m_clrDefaultTxtBK);
		break;
		
	case CLRTYPE_GREEN:
	case CLRTYPE_DARK:
		cList.SetBkColor(AfxGetQColor(QCOLOR_FACE));
		cList.SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		cList.SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
		break;
		
	default:
		ASSERT(FALSE);
	}

	// 可能是自绘的原因吧
	cList.Invalidate(TRUE);
}

void CQView::RedrawIcons()
{
	if (!m_nItemCount)
	{
		Invalidate(TRUE);
	}

	CListCtrl &cList = GetListCtrl();
	RECT rcIcon;
	RECT rcClient;
	
	cList.GetClientRect(&rcClient);
	cList.GetItemRect(cList.GetTopIndex(), &rcIcon, LVIR_ICON);

	rcIcon.bottom = rcClient.bottom;

	InvalidateRect(&rcIcon, TRUE);
}

void CQView::CheckDuplicate(int nSimilar)
{
	if (m_nItemCount < 2)
	{
		return;
	}

	if (sm_wSortColumn != 1 || sm_wSortOrder != ASCENDING)
	{
		SetColumnText(sm_wSortColumn, sm_szTitle[sm_wSortColumn]);

		sm_wSortOrder = ASCENDING;
		sm_wSortColumn = 1;
		SortList();
		
		CString str;
		str = sm_szTitle[sm_wSortColumn];
		str += _T("▲");
		SetColumnText(sm_wSortColumn, str);
	}

	ClearAllStates(TRUE, TRUE);

	CListCtrl &ctList = GetListCtrl();
	int i = 0, j = 1, nCount = 0;
	while(1)
	{
		if (sm_pSortedItemList[i]->IsSameItem(sm_pSortedItemList[j], nSimilar))
		{
			ctList.SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
			nCount++;
		}
		else
		{
			i = j;
		}

		j++;
		if (j >= m_nItemCount)
		{
			break;
		}
	}

	CTMessageBox Box;
	if (nCount > 0)
	{
		Box.SuperMessageBox(m_hWnd, IDS_BATCHSETDLG_SELDUPY, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0, nCount);
		ctList.EnsureVisible((int)ctList.GetFirstSelectedItemPosition(), FALSE);
	}
	else
	{
		Box.SuperMessageBox(m_hWnd, IDS_BATCHSETDLG_SELDUPN, IDS_MSGCAPTION_INFOR, MB_OK | MB_ICONINFORMATION, 0, 0);
	}

	SetFocus();
}

void CQView::OnSammaryLen() 
{
	CSummaryLenDlg Dlg(m_nSummaryLen);
	if (IDOK != Dlg.DoModal())
	{
		return;
	}

	m_nSummaryLen = Dlg.m_nLen;
	Invalidate(TRUE);
}
