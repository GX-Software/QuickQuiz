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
	// �����б������ٵ�ʱ��Ҫ������������
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
���ܣ�	���б���ȫ����Ŀ�����Զ������
����ֵ����
��ע��	���ܽ��Զ����б��е���Ŀ�����Զ������
		�����ὫĬ���б��е���Ŀ���з���
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
���ܣ�	�����б������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	���б�������ʱ����Ȼ�����������б�
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
���ܣ�	����б������
����ֵ�����������ִ�ָ��
��ע��	���б�������ʱ����Ȼ�����������б�
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
���ܣ�	����б����׸���Ч��Ŀ
����ֵ��������Ч��Ŀ��ţ�û�ҵ�����-1
��ע��	��Ч��Ŀ�Ķ�����û�����ô𰸡�ѡ����û��ѡ������û�п�
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
���ܣ�	���б��е�ȫ����Ч��Ŀ���
����ֵ����
��ע��	���������½���Ŀ��༭��Ŀʱʹ��
		�����Ŀ�󣬻���ȥ������ж�Ӧ��Ŀ�ı༭��־
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
			// ȥ���༭��־���������޸�
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
���ܣ�	����ӡ�һ����Ŀ�����б�
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	����������Ŀ������һ���б����Զ����Ǹ��б����Ƴ�
		�����б�Ϊ�Զ�����������¼���Ŀ��ԭʼ�����������Ϊ�¼���Ŀ�����Զ������
		�����ܲ��漰����
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

	// ����ö����Ѿ�����һ���б���
	// ����Ǹ��б�������
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

	// ���Ԫ�غ󣬲��Ҽ�¼һ��Ҫ����
	m_nLastItemGet = 0;
	m_pLastItemGet = NULL;

	return TRUE;
}

/*
���ܣ�	�������б��С����ơ���Ŀ�����б�
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	�����������µ���Ŀ�����룬��Ӱ��ԭ�б�
		�����ܲ��漰����
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
���ܣ�	�������б��С��ƶ�����Ŀ�����б�
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	���������������µ���Ŀ�������ж�������٣�������ָ�봫��
		�����ܲ��漰����
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
���ܣ�	���б��С�ɾ������Ŀ
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	bFree����ָ��ɾ�������Ŀ�Ƿ�����
		�����ܲ��漰����
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

	// ɾ��Ԫ�غ󣬲��Ҽ�¼һ��Ҫ����
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
���ܣ�	��һ����Ŀ�����桿�б��е�һ����Ŀ
����ֵ���ɹ����ر��滻������Ŀ�����򷵻�NULL
��ע��	��Դ��Ŀ����һ���б��򽫴��Ǹ��б���ɾ��
		���滻������Ŀ���ᱻ���٣�Ҳ���ٴ����ڱ��б��У�������ִ�б��������ֶ�����
		�����ܲ��漰����
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

	// �ҵ��˵�ǰ��ָ��
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
���ܣ�	���������б��е�������Ŀ��λ��
����ֵ����
��ע��	���滻������Ŀ���ᱻ���٣�Ҳ���ٴ����ڱ��б��У�������ִ�б��������ֶ�����
		�����ܲ��漰����
*/
void CList::SwapItems(int nItemIdx1, int nItemIdx2)
{
	if (nItemIdx1 == nItemIdx2)
	{
		return;
	}

	CItem *pItem1 = GetItem(nItemIdx1);
	CItem *pItem2 = GetItem(nItemIdx2);

	// ��һ��ΪNULL������
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
���ܣ�	��һ����Ŀ�����롿�����б��е�ָ��λ��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	����б���û����Ŀ���򱾺�����ͬ��AddItem()
		�����ܲ��漰����
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

	// ע�⣬��Ų��ܱ�����
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
���ܣ�	ɾ���б��о����ض���ǵ���Ŀ�������䳷�����
����ֵ��ɾ����Ŀ������
��ע��
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

	// ɾ��Ԫ�غ󣬲��Ҽ�¼Ҫ����
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
���ܣ�	���б��е�ȫ��Ԫ����������
����ֵ����
��ע��	������ӿ��Բ�����
		�ڽ������ʱ�����Գ�����������ж�
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
���ܣ�	������Ч�������޸���Ϣ
����ֵ����
��ע��	����CPaper����
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

	// �������һ�εĳ�����Ϣ
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
	// �������ֽ����µĲ�������������µ�����
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
���ܣ�	���б���������Ŀ�����䳷�����
����ֵ����
��ע��
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
���ܣ�	�����Ŀ�Ƿ���Լ����б�
����ֵ���������򷵻�TRUE�����򷵻�FALSE
��ע��
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
