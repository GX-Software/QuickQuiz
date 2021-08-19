// TempList.cpp: implementation of the CTempList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TempList.h"

#include "Choise.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTempList::CTempList(int nType /* = TYPE_DEFAULT */, BOOL bCanSame /* = TRUE */) :
	CList(nType), m_bCanSame(bCanSame)
{
	ZeroMemory(m_nTypeCount, sizeof(m_nTypeCount));
	ZeroMemory(m_stCustomCount, sizeof(m_stCustomCount));
}

CTempList::~CTempList()
{
	// 这里调用父类的ClearAllItems已经足够了
	// 如果有新需求，可以改成调用子类虚函数
}

/*
功能：	将列表中全部指针指向的题目分配自定义序号
返回值：无
备注：	仅能将自定义列表中的题目分配自定义序号
		即不会将默认列表中的题目进行分配
*/
void CTempList::SetCustomIndex(int nIndex)
{
	if (nIndex < 1 || nIndex > TYPE_MAXCUSTOMCOUNT)
	{
		return;
	}
	
	CItemPtr *pItem = (CItemPtr*)m_pHead;
	while(pItem)
	{
		pItem->m_pItem->SetCustomIndex(nIndex);
		pItem = (CItemPtr*)pItem->GetNext();
	}
	
	CItem::SetCustomIndex(nIndex);
}

BOOL CTempList::AddItem(CItem *pItem)
{
	if (!pItem)
	{
		return FALSE;
	}

	// 检查题目类型
	if (m_nType != TYPE_DEFAULT && pItem->GetRawType() != m_nType)
	{
		return FALSE;
	}

	CItemPtr *pPtr;
	// 检查重复
	if (!m_bCanSame)
	{
		pPtr = (CItemPtr*)m_pHead;
		while(pPtr)
		{
			if (pPtr->m_pItem == pItem)
			{
				return FALSE;
			}

			pPtr = (CItemPtr*)pPtr->GetNext();
		}
	}
	
	pPtr = new CItemPtr;
	if (!pPtr)
	{
		return FALSE;
	}

	pPtr->m_pItem = pItem;
	pPtr->SetType(pItem->GetRawType());

	m_pLastItemGet = NULL;
	m_nLastItemGet = 0;

	// 记录所有题目的数量
	if (pItem->IsDefaultType())
	{
		m_nTypeCount[AfxTypeToIndex(pItem->GetType())]++;
	}
	else
	{
		int i = pItem->GetCustomIndex() - 1;
		m_stCustomCount[i].nTypeCount++;
		m_stCustomCount[i].szCustomDesc = pItem->GetTypeDesc();
	}

	return CList::AddItem(pPtr);
}

BOOL CTempList::AddFromList(CList *pList)
{
	if (!pList)
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		AddItem(pItem);
	}

	return TRUE;
}

void CTempList::AddStoreList(CList *pList)
{
	if (!pList)
	{
		return;
	}

	int i;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		if (pItem->IsStored())
		{
			AddItem(pItem);
		}
	}
}

BOOL CTempList::IsAllStored()
{
	CItemPtr *pPtr = (CItemPtr*)m_pHead;

	while(pPtr)
	{
		if (!pPtr->m_pItem->IsStored())
		{
			return FALSE;
		}

		pPtr = (CItemPtr*)pPtr->GetNext();
	}

	return TRUE;
}

BOOL CTempList::RemoveList(CList *pList, BOOL bFree)
{
	int i;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		CItemPtr *pPtr = GetItemPtr(pItem);

		if (pPtr)
		{
			if (pItem->IsDefaultType())
			{
				--m_nTypeCount[AfxTypeToIndex(pItem->GetType())];
			}
			else
			{
				--m_stCustomCount[pItem->GetCustomIndex() - 1].nTypeCount;
			}
			RemoveItem(pPtr, bFree);
		}
	}

	return TRUE;
}

void CTempList::ClearAllItems(BOOL bFree)
{
	CList::ClearAllItems(bFree);

	ZeroMemory(m_nTypeCount, sizeof(m_nTypeCount));
	ZeroMemory(m_stCustomCount, sizeof(m_stCustomCount));
}

void CTempList::SetItemState(UINT nState, UINT nMark)
{
	if (!m_pHead)
	{
		return;
	}
	
	CItemPtr *pPtr = (CItemPtr*)m_pHead;
	CItem *pSet = NULL;
	while(pPtr)
	{
		pPtr->m_pItem->GetItemFrom()->SetItemState(nState, nMark);
		pPtr = (CItemPtr*)pPtr->GetNext();
	}
}

void CTempList::SetStore(BOOL bStore)
{
	if (!m_pHead)
	{
		return;
	}
	
	CItemPtr *pPtr = (CItemPtr*)m_pHead;
	CItem *pSet = NULL;
	while(pPtr)
	{
		pPtr->m_pItem->GetItemFrom()->SetStore(bStore);
		pPtr = (CItemPtr*)pPtr->GetNext();
	}
}

CItem* CTempList::GetItem(int nIndex)
{
	if (nIndex >= m_nItemCount || nIndex < 0)
	{
		return NULL;
	}

	CItemPtr *pPtr = (CItemPtr*)CList::GetItem(nIndex);
	ASSERT(pPtr);

	return pPtr->m_pItem;
}

int CTempList::GetIndex(const CItem *pItem)
{
	if (!m_pHead)
	{
		return -1;
	}
	
	int ret = 0;
	CItemPtr *pCurItem = (CItemPtr*)m_pHead;
	while(pCurItem)
	{
		if (pCurItem->m_pItem == pItem)
		{
			return ret;
		}
		
		pCurItem = (CItemPtr*)pCurItem->GetNext();
		ret++;
	}
	
	return -1;
}

CItemPtr* CTempList::GetItemPtr(CItem* pItem)
{
	if (!pItem)
	{
		return NULL;
	}

	CItemPtr *pPtr = (CItemPtr*)m_pHead;
	while(pPtr)
	{
		if (pPtr->m_pItem == pItem)
		{
			break;
		}

		pPtr = (CItemPtr*)pPtr->GetNext();
	}
	
	return pPtr;
}

void CTempList::ClipCopy(CFixedSharedFile &sf)
{
	CItemPtr *pPtr = (CItemPtr*)m_pHead;
	
#ifdef _UNICODE
	int nCode = 0;
#else
	int nCode = 1;
#endif
	sf.Write(&nCode, sizeof(nCode));

	sf.Write(&m_nItemCount, sizeof(m_nItemCount));
	
	while(pPtr)
	{
		pPtr->m_pItem->ClipCopy(sf);
		
		pPtr = (CItemPtr*)pPtr->GetNext();
	}
}

int CTempList::GetSingleChoiseCount()
{
	int nCount = m_nTypeCount[ITEMINDEX_SCHOISE];
	CItemPtr *pPtr = (CItemPtr*)m_pHead;

	while(pPtr)
	{
		if (pPtr->m_pItem->GetType() == TYPE_MCHOISE)
		{
			if (((CChoise*)(pPtr->m_pItem))->GetAnswerCount() < 2)
			{
				nCount++;
			}
		}

		pPtr = (CItemPtr*)pPtr->GetNext();
	}

	return nCount;
}

int CTempList::GetMultiChoiseCount()
{
	int nCount = 0;
	CItemPtr *pPtr = (CItemPtr*)m_pHead;
	
	while(pPtr)
	{
		if (pPtr->m_pItem->GetType() == TYPE_MCHOISE)
		{
			if (((CChoise*)(pPtr->m_pItem))->GetAnswerCount() > 1)
			{
				nCount++;
			}
		}

		pPtr = (CItemPtr*)pPtr->GetNext();
	}
	
	return nCount;
}

int CTempList::GetCustomCount(int nType)
{
	nType /= TYPE_CUSTOMRIGHT;

	return m_stCustomCount[nType].nTypeCount;
}

void CTempList::GetListInfo(CString &str)
{
	CString temp;
	
	str.Format(IDS_PAPER_ITEMCOUNT, m_nItemCount);
	
	temp.Format(IDS_TMPLIST_SCHCOUNT, m_nTypeCount[ITEMINDEX_SCHOISE]);
	str += temp;

	temp.Format(IDS_TMPLIST_MCHCOUNT, m_nTypeCount[ITEMINDEX_MCHOISE]);
	str += temp;

	temp.Format(IDS_TMPLIST_JDGCOUNT, m_nTypeCount[ITEMINDEX_JUDGE]);
	str += temp;

	temp.Format(IDS_TMPLIST_BLKCOUNT, m_nTypeCount[ITEMINDEX_BLANK]);
	str += temp;

	temp.Format(IDS_TMPLIST_TXTCOUNT, m_nTypeCount[ITEMINDEX_TEXT]);
	str += temp;

	int i;
	for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
	{
		if (!m_stCustomCount[i].szCustomDesc)
		{
			break;
		}

		temp.Format(IDS_PAPER_TYPEINFO, m_stCustomCount[i].szCustomDesc,
			m_stCustomCount[i].nTypeCount);
		str += temp;
	}
}

void CTempList::SwapItems(int nItemIdx1, int nItemIdx2)
{
	if (nItemIdx1 == nItemIdx2)
	{
		return;
	}
	
	CItemPtr *pItem1 = (CItemPtr*)CList::GetItem(nItemIdx1);
	CItemPtr *pItem2 = (CItemPtr*)CList::GetItem(nItemIdx2);
	
	// 有一个为NULL即返回
	if (!(pItem1 && pItem2))
	{
		return;
	}
	
	CItem *pSwap = NULL;
	
	pSwap = pItem1->m_pItem;
	pItem1->m_pItem = pItem2->m_pItem;
	pItem2->m_pItem = pSwap;
}

void CTempList::Random(int nStart, int nEnd, int nSeed)
{
	int nCount;
	
	if (nEnd <= nStart)
	{
		nStart = 0;
		nEnd = m_nItemCount - 1;
		nCount = m_nItemCount;
	}
	else
	{
		nCount = nEnd - nStart + 1;
	}
	
	int i, j;
	
	if (!nSeed)
	{
		nSeed = time(NULL) + _getpid();
	}
	
	srand(nSeed);
	for (i = nStart; i <= nEnd; i++)
	{
		j = rand() % nCount + nStart;
		
		SwapItems(i, j);
	}
	
	srand(nSeed + j);
	for (i--; i >= nStart; i--)
	{
		j = rand() % nCount + nStart;
		
		SwapItems(i, j);
	}
}
