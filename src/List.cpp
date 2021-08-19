// List.cpp: implementation of the CList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "List.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CList::CList(int nQuestionType /* = TYPE_DEFAULT */) :
	m_pHead(NULL), m_pTail(NULL),
	m_nItemCount(0),
	m_nLastItemGet(0), m_pLastItemGet(NULL),
	m_hTreeItem(NULL),
	m_uItemLimit(0),
	m_nQNumber(0)
{
	m_nType = nQuestionType;

	ZeroMemory(m_UndoInfo, sizeof(m_UndoInfo));
}

CList::~CList()
{
	// 所有列表在销毁的时候都要清理所有内容
	ClearAllItems(TRUE);

	int i;
	for (i = 0; i < UNDO_SIZE; i++)
	{
		switch(m_UndoInfo[i].nType)
		{
		case LISTUNDO_SETDESC:
			free((LPTSTR)(m_UndoInfo[i].dwData));
			break;

		default:
			break;
		}
	}
}

/*
功能：	将列表中全部题目分配自定义序号
返回值：无
备注：	仅能将自定义列表中的题目分配自定义序号
		即不会将默认列表中的题目进行分配
*/
void CList::SetCustomIndex(int nIndex)
{
	if (nIndex < 1 || nIndex > TYPE_MAXCUSTOMCOUNT || IsDefaultType())
	{
		return;
	}

	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->SetCustomIndex(nIndex);
		pItem = pItem->GetNext();
	}
	
	CItem::SetCustomIndex(nIndex);
}

/*
功能：	设置列表的描述
返回值：成功返回TRUE，否则返回FALSE
备注：	当列表允许撤销时，必然是来自题库的列表
*/
BOOL CList::SetDescription(LPCTSTR szDescription)
{
	if (!szDescription)
	{
		return FALSE;
	}

	CList *pList = GetParent();
	if (pList && pList->IsPaper())
	{
		CPaper *pPaper = (CPaper*)pList;
		LPTSTR pDesc = _tcsdup(szDescription);
		if (!pDesc)
		{
			return FALSE;
		}

		pPaper->EditCustomType(UNDO_EDIT_CUSTOM);
		LPCUNDOSTEP pStep = pPaper->GetUndoStepInfo();
		
		ASSERT(LISTUNDO_NONE == m_UndoInfo[pStep->nCurStep].nType);
		m_UndoInfo[pStep->nCurStep].nType = LISTUNDO_SETDESC;
		m_UndoInfo[pStep->nCurStep].dwData = (DWORD)pDesc;

		return TRUE;
	}
	
	return CItem::SetDescription(szDescription);
}

/*
功能：	获得列表的描述
返回值：返回描述字串指针
备注：	当列表允许撤销时，必然是来自题库的列表
*/
LPCTSTR CList::GetDescription()
{
	CList *pList = GetParent();
	if (pList && pList->IsPaper())
	{
		CPaper *pPaper = (CPaper*)pList;
		LPCUNDOSTEP pStep = pPaper->GetUndoStepInfo();
		
		int nStart = pStep->nCurStep;
		while(1)
		{
			if (LISTUNDO_SETDESC == m_UndoInfo[nStart].nType)
			{
				return (LPCTSTR)m_UndoInfo[nStart].dwData;
			}
			
			if (nStart == pStep->nOldestStep)
			{
				break;
			}
			
			nStart = (nStart + UNDO_SIZE - 1) % UNDO_SIZE;
		}
	}
	
	return CItem::GetDescription();
}

/*
功能：	获得列表中首个无效题目
返回值：返回无效题目序号，没找到返回-1
备注：	无效题目的定义是没有设置答案、选择题没有选项、填空题没有空
*/
int CList::GetFirstInvalidItem()
{
	int nRet = 0;
	CItem *pItem = m_pHead;
	
	while(pItem)
	{
		if (!pItem->CheckItemValid())
		{
			return nRet;
		}
		
		++nRet;
		pItem = pItem->GetNext();
	}
	
	return -1;
}

/*
功能：	将列表中的全部无效题目清除
返回值：无
备注：	本函数在新建题目或编辑题目时使用
		清除题目后，还会去掉题库中对应题目的编辑标志
*/
void CList::ClearInvalidItems()
{
	CItem *pItem = m_pHead;
	CItem *pNext = NULL;
	
	while(pItem)
	{
		pNext = pItem->GetNext();
		
		if (!pItem->CheckItemValid())
		{
			// 去掉编辑标志，即不做修改
			pItem->GetItemFrom()->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			RemoveItem(pItem, TRUE);
		}
		
		pItem = pNext;
	}
}

CItem* CList::GetFirstUnAnswerd()
{
	CItem *pItem = m_pHead;
	
	while(pItem)
	{
		if (pItem->GetType() == TYPE_GROUP)
		{
			CItem *pSub = ((CItemGroup*)pItem)->m_pHead;
			while(pSub)
			{
				if (!pSub->IsAnswered())
				{
					return pSub;
				}
				pSub = pSub->GetNext();
			}
		}
		else if (!pItem->IsAnswered())
		{
			return pItem;
		}
		
		pItem = pItem->GetNext();
	}

	return NULL;
}

/*
功能：	【添加】一个题目到本列表
返回值：成功返回TRUE，否则返回FALSE
备注：	若添加入的题目属于另一个列表，将自动从那个列表中移除
		若本列表为自定义类别，且与新加题目的原始类别相符，则会为新加题目分配自定义序号
		本功能不涉及撤销
*/
BOOL CList::AddItem(CItem *pItem)
{
	if (!pItem)
	{
		return FALSE;
	}

	if (!CanAddIn(pItem))
	{
		return FALSE;
	}

	// 如果该对象已经属于一个列表了
	// 则从那个列表中清理
	if (pItem->GetParent())
	{
		pItem->GetParent()->RemoveItem(pItem, FALSE);
	}

	if (!m_pHead)
	{
		ASSERT(!m_pTail);
		m_pHead = m_pTail = pItem;

		pItem->SetNext(NULL);
		pItem->SetPrev(NULL);
	}
	else
	{
		ASSERT(m_pTail);
		m_pTail->SetNext(pItem);

		pItem->SetPrev(m_pTail);
		pItem->SetNext(NULL);
		
		m_pTail = pItem;
	}

	pItem->SetParent(this);
	++m_nItemCount;

	if (!IsDefaultType())
	{
		pItem->SetCustomIndex(m_nType / TYPE_CUSTOMRIGHT);
	}

	// 添加元素后，查找记录一定要归零
	m_nLastItemGet = 0;
	m_pLastItemGet = NULL;

	return TRUE;
}

/*
功能：	从其它列表中【复制】题目到本列表
返回值：成功返回TRUE，否则返回FALSE
备注：	本函数创建新的题目并加入，不影响原列表
		本功能不涉及撤销
*/
BOOL CList::AddFromList(CList *pList)
{
	if (!pList)
	{
		return FALSE;
	}
	
	ASSERT(!(pList->IsTempList()));
	
	CItem *pItem = pList->m_pHead;
	CItem *pNext = NULL;
	CItem *pNew = NULL;
	while(pItem)
	{
		pNext = pItem->GetNext();
		
		if (CanAddIn(pItem))
		{
			pNew = pItem->Alloc();
			if (!AddItem(pNew))
			{
				delete pNew;
			}
		}
		
		pItem = pNext;
	}
	
	return TRUE;
}

/*
功能：	从其它列表中【移动】题目到本列表
返回值：成功返回TRUE，否则返回FALSE
备注：	本函数不会生成新的题目，不进行对象的销毁，仅进行指针传递
		本功能不涉及撤销
*/
BOOL CList::MoveFromList(CList *pList)
{
	if (!pList)
	{
		return FALSE;
	}
	ASSERT(!(pList->IsTempList()));

	CItem *pItem = pList->m_pHead, *pNext = NULL;
	while(pItem)
	{
		pNext = pItem->GetNext();
		
		if (CanAddIn(pItem))
		{
			pList->RemoveItem(pItem, FALSE);
			AddItem(pItem);
		}
		
		pItem = pNext;
	}
	
	return TRUE;
}

/*
功能：	从列表中【删除】题目
返回值：成功返回TRUE，否则返回FALSE
备注：	bFree用于指定删除后的题目是否销毁
		本功能不涉及撤销
*/
BOOL CList::RemoveItem(CItem *pItem, BOOL bFree)
{
	if (!pItem || !m_pHead || pItem->GetParent() != this)
	{
		return FALSE;
	}
	
	CItem *pPrev = pItem->GetPrev();
	CItem *pNext = pItem->GetNext();
	
	if (pPrev)
	{
		pPrev->SetNext(pNext);
	}
	else
	{
		m_pHead = pNext;
	}
	
	if (pNext)
	{
		pNext->SetPrev(pPrev);
	}
	else
	{
		m_pTail = pPrev;
	}
	
	--m_nItemCount;

	// 删除元素后，查找记录一定要归零
	m_nLastItemGet = 0;
	m_pLastItemGet = NULL;
	
	if (bFree)
	{
		delete pItem;
	}
	else
	{
		pItem->SetParent(NULL);
	}

	return TRUE;
}

/*
功能：	用一个题目【代替】列表中的一个题目
返回值：成功返回被替换掉的题目，否则返回NULL
备注：	若源题目来自一个列表，则将从那个列表中删除
		被替换掉的题目不会被销毁，也不再存在于本列表中，必须在执行本函数后手动销毁
		本功能不涉及撤销
*/
CItem* CList::ReplaceItem(CItem *pItemDst, CItem *pItemSrc)
{
	if (!pItemSrc || !pItemDst)
	{
		return NULL;
	}
	if (!CanAddIn(pItemSrc))
	{
		return NULL;
	}
	if (this != pItemDst->GetParent())
	{
		return NULL;
	}
	if (pItemSrc->GetParent())
	{
		ASSERT(pItemSrc->GetParent()->IsList());
		pItemSrc->GetParent()->RemoveItem(pItemSrc, FALSE);
	}

	// 找到了当前的指针
	CItem *pPrev = pItemDst->GetPrev();
	CItem *pNext = pItemDst->GetNext();
	RemoveItem(pItemDst, FALSE);

	if (pPrev)
	{
		pPrev->SetNext(pItemSrc);
		pItemSrc->SetPrev(pPrev);
	}
	else
	{
		m_pHead = pItemSrc;
		pItemSrc->SetPrev(NULL);
	}

	if (pNext)
	{
		pNext->SetPrev(pItemSrc);
		pItemSrc->SetNext(pNext);
	}
	else
	{
		m_pTail = pItemSrc;
		pItemSrc->SetNext(NULL);
	}

	pItemSrc->SetParent(this);
	if (pItemDst == m_pLastItemGet)
	{
		m_pLastItemGet = pItemSrc;
	}
	m_nItemCount++;

	return pItemDst;
}

/*
功能：	【调换】列表中的两个题目的位置
返回值：无
备注：	被替换掉的题目不会被销毁，也不再存在于本列表中，必须在执行本函数后手动销毁
		本功能不涉及撤销
*/
void CList::SwapItems(int nItemIdx1, int nItemIdx2)
{
	if (nItemIdx1 == nItemIdx2)
	{
		return;
	}

	CItem *pItem1 = GetItem(nItemIdx1);
	CItem *pItem2 = GetItem(nItemIdx2);

	// 有一个为NULL即返回
	if (!(pItem1 && pItem2))
	{
		return;
	}

	CItem *pSwap = NULL;

	pSwap = pItem1->GetPrev();
	pItem1->SetPrev(pItem2->GetPrev());
	pItem2->SetPrev(pSwap);

	pSwap = pItem1->GetPrev();
	if (pSwap)
	{
		pSwap->SetNext(pItem1);
	}
	else
	{
		m_pHead = pItem1;
	}

	pSwap = pItem2->GetPrev();
	if (pSwap)
	{
		pSwap->SetNext(pItem2);
	}
	else
	{
		m_pHead = pItem2;
	}
	
	pSwap = pItem1->GetNext();
	pItem1->SetNext(pItem2->GetNext());
	pItem2->SetNext(pSwap);

	pSwap = pItem1->GetNext();
	if (pSwap)
	{
		pSwap->SetPrev(pItem1);
	}
	else
	{
		m_pTail = pItem1;
	}

	pSwap = pItem2->GetNext();
	if (pSwap)
	{
		pSwap->SetPrev(pItem2);
	}
	else
	{
		m_pTail = pItem2;
	}

	if (m_pLastItemGet == pItem1)
	{
		m_pLastItemGet = pItem2;
	}
	else if (m_pLastItemGet == pItem2)
	{
		m_pLastItemGet = pItem1;
	}
}

/*
功能：	将一个题目【插入】到本列表中的指定位置
返回值：成功返回TRUE，否则返回FALSE
备注：	如果列表中没有题目，则本函数等同于AddItem()
		本功能不涉及撤销
*/
BOOL CList::InsertItem(CItem *pIns, CItem *pDes, BOOL bAfter)
{
	if (!pIns)
	{
		return FALSE;
	}
	else if (!CanAddIn(pIns))
	{
		return FALSE;
	}
	else if (!pDes)
	{
		return AddItem(pIns);
	}

	if (pIns->GetParent())
	{
		pIns->GetParent()->RemoveItem(pIns, FALSE);
	}
	
	if (bAfter)
	{
		CItem *pNext = pDes->GetNext();
		
		pDes->SetNext(pIns);
		pIns->SetPrev(pDes);
		
		if (pNext)
		{
			pIns->SetNext(pNext);
			pNext->SetPrev(pIns);
		}
		else
		{
			m_pTail = pIns;
			pIns->SetNext(NULL);
		}
	}
	else
	{
		CItem *pPrev = pDes->GetPrev();
		
		pIns->SetNext(pDes);
		pDes->SetPrev(pIns);
		
		if (pPrev)
		{
			pPrev->SetNext(pIns);
			pIns->SetPrev(pPrev);
		}
		else
		{
			m_pHead = pIns;
			pIns->SetPrev(NULL);
		}
	}
	
	pIns->SetParent(this);
	m_nItemCount++;
	return TRUE;
}

void CList::ClearAllItems(BOOL bFree)
{
	if (!m_pHead)
	{
		return;
	}
	
	CItem *pItem = m_pHead;
	while(pItem)
	{
		m_pHead = m_pHead->GetNext();
		
		if (bFree)
		{
			delete pItem;
		}
		
		pItem = m_pHead;
	}
	
	ASSERT(!m_pHead);
	m_pTail = NULL;
	m_nItemCount = 0;
}

void CList::ClearUserInfo()
{
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->ClearUserInfo();
		pItem = pItem->GetNext();
	}
}

CItem* CList::GetItem(int nIndex)
{
	if (nIndex >= m_nItemCount || nIndex < 0)
	{
		return NULL;
	}
	
	CItem *pStart = m_pLastItemGet;
	int nStart = m_nLastItemGet;
	if (!pStart)
	{
		pStart = m_pHead;
		nStart = 0;
	}

	if (nStart <= nIndex)
	{
		int i;
		for (i = nStart + 1; i <= nIndex; ++i)
		{
			pStart = pStart->GetNext();
		}
	}
	else
	{
		int i;
		for (i = nStart - 1; i >= nIndex; --i)
		{
			pStart = pStart->GetPrev();
		}
	}

	m_pLastItemGet = pStart;
	m_nLastItemGet = nIndex;
	return pStart;
}

int CList::GetIndex(const CItem *pItem)
{
	if (!m_pHead)
	{
		return -1;
	}

	int ret = 0;
	CItem *pCurItem = m_pHead;
	while(pCurItem)
	{
		if (pCurItem == pItem)
		{
			return ret;
		}

		pCurItem = pCurItem->GetNext();
		ret++;
	}

	return -1;
}

void CList::ClipCopy(CFixedSharedFile &sf)
{
	int i;
#ifdef _UNICODE
	int nCode = 0;
#else
	int nCode = 1;
#endif
	sf.Write(&nCode, sizeof(nCode));

	sf.Write(&m_nItemCount, sizeof(m_nItemCount));

	CItem *pItem = m_pHead;
	for (i = 0; i < m_nItemCount; i++)
	{
		pItem->ClipCopy(sf);

		pItem = pItem->GetNext();
	}

	// 注意，题号不能被复制
}

BOOL CList::ClipPaste(CFixedSharedFile &sf)
{
	int i, nCount, nType;
	CItem *pItem = NULL;

	sf.Read(&i, sizeof(i));
#ifdef _UNICODE
	if (i != 0)
	{
		return FALSE;
	}
#else
	if (i != 1)
	{
		return FALSE;
	}
#endif

	sf.Read(&nCount, sizeof(nCount));
	for (i = 0; i < nCount; i++)
	{
		sf.Read(&nType, sizeof(nType));
		switch(nType % TYPE_CUSTOMRIGHT)
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
			pItem = new CChoise(TYPE_SCHOISE == nType % TYPE_CUSTOMRIGHT);
			break;

		case TYPE_JUDGE:
			pItem = new CJudge;
			break;

		case TYPE_BLANK:
			pItem = new CBlank;
			break;

		case TYPE_TEXT:
			pItem = new CText;
			break;

		case TYPE_GROUP:
			pItem = new CItemGroup;
			break;

		default:
			ASSERT(FALSE);
		}

		if (!pItem)
		{
			return FALSE;
		}
		pItem->SetCustomIndex(CItem::GetCustomIndex(nType));
		if (!pItem->ClipPaste(sf))
		{
			delete pItem;
			return FALSE;
		}

		if (!AddItem(pItem))
		{
			delete pItem;
			pItem = NULL;
		}
	}

	return TRUE;
}

/*
功能：	删除列表中具有特定标记的题目，并分配撤销序号
返回值：删除题目的数量
备注：
*/
UINT CList::RemoveAllMarkedItems(UINT uMark, int nUndoStep)
{
	UINT uRet = 0;
	if (!m_pHead)
	{
		return uRet;
	}

	int nUndoType = UNDO_NULL;
	switch(uMark)
	{
	case ITEMMARK_EDIT:
		nUndoType = UNDO_EDIT_DEL;
		break;

	case ITEMMARK_DELETE:
		nUndoType = UNDO_DEL;
		break;
	}

	CItem *pItem = m_pHead;
	while(pItem)
	{
		if (pItem->GetItemState(uMark))
		{
			pItem->SetUndoStep(nUndoStep, nUndoType);
			pItem->SetItemState(ITEMMARK_NULL, uMark);
			uRet++;
		}

		pItem = pItem->GetNext();
	}

	// 删除元素后，查找记录要归零
	m_pLastItemGet = NULL;
	m_nLastItemGet = 0;
	return uRet;
}

void CList::SetItemState(UINT nState, UINT nMark)
{
	if (!m_pHead)
	{
		return;
	}
	
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->SetItemState(nState, nMark);
		pItem = pItem->GetNext();
	}
}

/*
功能：	将列表中的全部元素乱序排列
返回值：无
备注：	随机种子可以不传入
		在进行随机时，不对撤销情况进行判断
*/
void CList::Random(int nStart, int nEnd, int nSeed)
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

/*
功能：	清理无效的名称修改信息
返回值：无
备注：	仅供CPaper调用
*/
void CList::UndoClear(LPCUNDOSTEP pStep)
{
	if (!IsPaper())
	{
		CList *pList = GetParent();
		if (!pList || !pList->IsPaper())
		{
			return;
		}
	}

	// 清理最旧一次的撤销信息
	if (((pStep->nCurStep + 1) % UNDO_SIZE) == pStep->nOldestStep)
	{
		switch(m_UndoInfo[pStep->nOldestStep].nType)
		{
		case LISTUNDO_SETDESC:
			ASSERT(m_strDescription);
			free(m_strDescription);
			m_strDescription = (LPTSTR)m_UndoInfo[pStep->nOldestStep].dwData;
			m_UndoInfo[pStep->nOldestStep].nType = LISTUNDO_NONE;
			m_UndoInfo[pStep->nOldestStep].dwData = 0;
			break;

		case LISTUNDO_SETQNUMBER:
			m_nQNumber = (int)m_UndoInfo[pStep->nOldestStep].dwData;
			break;

		default:
			ASSERT(FALSE);
		}
	}
	// 撤销后又进行新的操作，则清理更新的内容
	else if (pStep->nCurStep != pStep->nNewestStep)
	{
		int nStart = (pStep->nCurStep + 1) % UNDO_SIZE;
		while(1)
		{
			if (LISTUNDO_SETDESC == m_UndoInfo[nStart].nType)
			{
				free((LPTSTR)m_UndoInfo[nStart].dwData);
				m_UndoInfo[nStart].nType = LISTUNDO_NONE;
				m_UndoInfo[nStart].dwData = NULL;
			}
			if (nStart == pStep->nNewestStep)
			{
				break;
			}

			nStart = (nStart + 1) % UNDO_SIZE;
		}
	}
}

/*
功能：	将列表中所有题目均分配撤销序号
返回值：无
备注：
*/
void CList::SetUndoStep(int nUndoStep, int nUndoType)
{
	CItem *pItem = m_pHead;
	while(pItem)
	{
		pItem->SetUndoStep(nUndoStep, nUndoType);
		pItem = pItem->GetNext();
	}
}

/*
功能：	检查题目是否可以加入列表
返回值：若可以则返回TRUE，否则返回FALSE
备注：
*/
BOOL CList::CanAddIn(CItem *pItem)
{
	ASSERT(pItem);

	int nType = GetType();
	if (TYPE_DEFAULT == nType)
	{
		return TRUE;
	}
	else if (TYPE_GROUP == nType && !IsList())
	{
		return TRUE;
	}
	else
	{
		return (m_nType == pItem->GetRawType());
	}
}

void CList::SetQNumber(int nQNumber)
{
	CList *pList = GetParent();
	
	if (pList && pList->IsPaper())
	{
		CPaper *pPaper = (CPaper*)pList;
		pPaper->EditCustomType(UNDO_EDIT_CUSTOM);

		LPCUNDOSTEP pStep = pPaper->GetUndoStepInfo();
		
		ASSERT(LISTUNDO_NONE == m_UndoInfo[pStep->nCurStep].nType);
		m_UndoInfo[pStep->nCurStep].nType = LISTUNDO_SETQNUMBER;
		m_UndoInfo[pStep->nCurStep].dwData = (DWORD)nQNumber;
	}
	else
	{
		m_nQNumber = nQNumber;
	}
}

int CList::GetQNumber()
{
	CList *pList = GetParent();
	if (pList && pList->IsPaper())
	{
		CPaper *pPaper = (CPaper*)pList;
		LPCUNDOSTEP pStep = pPaper->GetUndoStepInfo();
		
		int nStart = pStep->nCurStep;
		while(1)
		{
			if (LISTUNDO_SETQNUMBER == m_UndoInfo[nStart].nType)
			{
				return (int)m_UndoInfo[nStart].dwData;
			}
			
			if (nStart == pStep->nOldestStep)
			{
				break;
			}
			
			nStart = (nStart + UNDO_SIZE - 1) % UNDO_SIZE;
		}
	}
	
	return m_nQNumber;
}
