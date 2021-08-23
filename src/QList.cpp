// QList.cpp : implementation file
//

#include "stdafx.h"
#include "quickquiz.h"
#include "QList.h"

#include "TestMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SUMMARYLENGTH		60

/////////////////////////////////////////////////////////////////////////////
// CQList

CQList::CQList() :
	m_nListType(-1), m_nCurShowIndex(-1)
{
}

CQList::~CQList()
{
}


BEGIN_MESSAGE_MAP(CQList, CListCtrl)
	//{{AFX_MSG_MAP(CQList)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_WM_CREATE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CQList message handlers

int CQList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DWORD ExStyle = LVS_EX_FULLROWSELECT;
	// 设定色彩
	switch(g_nColorType)
	{
	case CLRTYPE_GREEN:
		SetBkColor(AfxGetQColor(QCOLOR_FACE));
		SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
	case CLRTYPE_DEFAULT:
		ExStyle |= LVS_EX_GRIDLINES;
		break;
		
	case CLRTYPE_DARK:
		SetBkColor(AfxGetQColor(QCOLOR_FACE));
		SetTextColor(AfxGetQColor(QCOLOR_TEXT));
		SetTextBkColor(AfxGetQColor(QCOLOR_FACE));
		break;
		
	default:
		ASSERT(FALSE);
	}
	SetExtendedStyle(ExStyle);
	Invalidate(TRUE);
	
	CString string;
	string.LoadString(IDS_QLIST_INDEX);
	InsertColumn(0, string, LVCFMT_LEFT, 50);
	
	string.LoadString(IDS_QLIST_STATE);
	InsertColumn(1, string, LVCFMT_LEFT, 50);
	
	string.LoadString(IDS_QLIST_TYPE);
	InsertColumn(2, string, LVCFMT_LEFT, 50);
	
	string.LoadString(IDS_QLIST_ABSTRACT);
	InsertColumn(3, string, LVCFMT_LEFT, 200);
	
	SendMessage(WM_SETFONT, (WPARAM)g_hNormalFont, TRUE);
	SetItemCount(0);
	
	return 0;
}

void CQList::SetList(CList *pList, int nListType, int nSel)
{
	m_nListType = nListType;

	m_List.ClearAllItems(TRUE);
	if (m_nListType == LISTFRM_TYPE_EDIT)
	{
		ModifyStyle(LVS_SINGLESEL, NULL);
		m_List.AddFromList(pList);
	}
	else
	{
		ModifyStyle(NULL, LVS_SINGLESEL);
		CTestMaker::MakeAtomList(pList, &m_List);
	}

	UpdateList(nSel);
}

void CQList::SetCurSel(int nIndex)
{
	int nSelIndex = GetNextItem(-1, LVIS_FOCUSED | LVNI_SELECTED);
	while (nSelIndex >= 0)
	{
		SetItemState(nSelIndex, NULL, LVIS_SELECTED);
		nSelIndex = GetNextItem(nSelIndex, LVIS_FOCUSED | LVNI_SELECTED);
	}
	
	if (nIndex < 0)
	{
		SetItemState(-1, NULL, LVIS_FOCUSED | LVIS_SELECTED);
	}
	else
	{
		SetItemState(nIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		EnsureVisible(nIndex, FALSE);
	}
	
	Invalidate();
}

void CQList::SetCurSel(CItem *pItem)
{
	int i = 0;
	CItem *pPtr = m_List.GetItem(i);

	while(pPtr)
	{
		if (pPtr == pItem)
		{
			SetCurSel(i);
			break;
		}

		++i;
		pPtr = m_List.GetItem(i);
	}
}

void CQList::UpdateList(int nSel)
{
	int nCount = m_List.GetItemCount();
	SetItemCount(nCount);
	
	if (nSel >= nCount)
	{
		nSel = 0;
	}
	
	SetCurSel(nSel);
	m_nCurShowIndex = nSel;
}

void CQList::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pTVDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pTVDispInfo->item;
	
	CItem *pItem = m_List.GetItem(plvItem->iItem);
	ASSERT(pItem);
	
	if (plvItem->mask & TVIF_TEXT)
	{
		switch(plvItem->iSubItem)
		{
		case 0: // 序号
			_sntprintf(plvItem->pszText, plvItem->cchTextMax, _T("%d"),
				plvItem->iItem + 1);
			break;
			
		case 1: // 状态
			switch (m_nListType)
			{
			case LISTFRM_TYPE_VIEW:
				if (pItem->IsStored())
				{
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_STORED, plvItem->pszText, plvItem->cchTextMax);
				}
				break;
				
			case LISTFRM_TYPE_TESTING:
				if (!pItem->IsAnswered())
				{
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_UNASW, plvItem->pszText, plvItem->cchTextMax);
				}
				else if (pItem->GetItemState(ITEMMARK_UNCERTAIN))
				{
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_MARK, plvItem->pszText, plvItem->cchTextMax);
				}
				break;
				
			case LISTFRM_TYPE_REVIEW:
				switch(pItem->CheckAnswer())
				{
				case WRONG:
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_WRONG, plvItem->pszText, plvItem->cchTextMax);
					break;
					
				case HALFRIGHT:
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_HALFRIGHT, plvItem->pszText, plvItem->cchTextMax);
					break;
					
				case ALLRIGHT:
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_ALLRIGHT, plvItem->pszText, plvItem->cchTextMax);
					break;
					
				default:
					ASSERT(FALSE);
				}
				break;
				
			case LISTFRM_TYPE_EDIT:
				if (!pItem->CheckItemValid())
				{
					LoadString(AfxGetInstanceHandle(), IDS_QLIST_NOSET, plvItem->pszText, plvItem->cchTextMax);
				}
				break;
				
			default:
				ASSERT(FALSE);
			}
			break;
			
		case 2: // 题型
			switch(m_nListType)
			{
			case LISTFRM_TYPE_TESTING:
			case LISTFRM_TYPE_REVIEW:
				{
					CItem *pParent = pItem->GetParent();
					if (pParent && pParent->GetType() == TYPE_GROUP)
					{
						_tcscpyn(plvItem->pszText, pParent->GetTypeDesc(), plvItem->cchTextMax);
					}
					else
					{
						_tcscpyn(plvItem->pszText, pItem->GetTypeDesc(), plvItem->cchTextMax);
					}
				}
				break;

			default:
				_tcscpyn(plvItem->pszText, pItem->GetTypeDesc(), plvItem->cchTextMax);
				break;
			}
			break;
			
		case 3: // 摘要
			{
				int nLen = pItem->GetDescriptionSummary(SUMMARYLENGTH);
				
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

	*pResult = 0;
}

void CQList::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UINT uHitFlags;
	int nItem = HitTest(point, &uHitFlags);
	
	if (nItem >= 0 && (uHitFlags & LVHT_ONITEM))
	{
		GetParent()->SendMessage(WM_QLISTNOTIFY, QLN_LBUTTONDBLCLK, nItem);
	}
	
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CQList::OnRButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->SendMessage(WM_QLISTNOTIFY, QLN_RBUTTONUP, MAKELPARAM(point.x, point.y));
	
	CListCtrl::OnRButtonUp(nFlags, point);
}
