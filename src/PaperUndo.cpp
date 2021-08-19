// PaperUndo.cpp: implementation of the CPaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Paper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/*
功能：	题库撤销操作
返回值：是否对pListInfo进行了修改
备注：	当对自定义类型进行了改动后，将填充pListInfo内容
*/
int CPaper::Undo(TPARAS *pListInfo)
{
	ASSERT(m_sUndoStep.nCurStep != m_sUndoStep.nOldestStep);
	
	m_sUndoStep.nCurStep = (m_sUndoStep.nCurStep + UNDO_SIZE - 1) % UNDO_SIZE;
	if (m_sUndoStep.nSaveIndex == m_sUndoStep.nCurStep)
	{
		m_bModify = FALSE;
	}
	else
	{
		m_bModify = TRUE;
	}
	
	BOOL bRet = UndoCustomLists(pListInfo);
	
	ClearCache();
	SaveStoreQuestions(NULL);
	return bRet;
}

/*
功能：	题库重做操作
返回值：是否对pListInfo进行了修改
备注：	当对自定义类型进行了改动后，将填充pListInfo内容
*/
int CPaper::Redo(TPARAS *pListInfo)
{
	ASSERT(m_sUndoStep.nCurStep != m_sUndoStep.nNewestStep);
	
	m_sUndoStep.nCurStep = (m_sUndoStep.nCurStep + 1) % UNDO_SIZE;
	if (m_sUndoStep.nSaveIndex == m_sUndoStep.nCurStep)
	{
		m_bModify = FALSE;
	}
	else
	{
		m_bModify = TRUE;
	}
	
	BOOL bRet = UndoCustomLists(pListInfo);
	
	ClearCache();
	SaveStoreQuestions(NULL);
	return bRet;
}

/*
功能：	每次对题库进行改动后，均调用此函数用来保存撤销信息
返回值：无
备注：	对撤销指针的前进操作和对撤销信息的清理是在ClearUndoChain()中完成的
*/
void CPaper::SetUndoChain(PVOID pPara, int nType)
{
	// 首先处理撤销步骤不增加的情况
	switch(nType)
	{
	case UNDO_ADD:
		if (UNDO_EDIT_DEL == m_sUndoStep.nLastUndoType)
		{
			nType = UNDO_EDIT_ADD;
		}
		else
		{
			ClearUndoChain();
		}
		break;
		
	default:
		ClearUndoChain();
	}
	ASSERT(m_sUndoStep.nCurStep == m_sUndoStep.nNewestStep);
	ASSERT(m_sUndoStep.nCurStep != m_sUndoStep.nOldestStep);
	
	switch(nType)
	{
	case UNDO_ADD_CUSTOM:
		ASSERT(pPara);
		((CItem*)pPara)->SetUndoStep(m_sUndoStep.nNewestStep, nType);
		break;

	case UNDO_DEL_CUSTOM:
		ASSERT(pPara);
		((CTempList*)pPara)->SetUndoStep(m_sUndoStep.nNewestStep, UNDO_DEL);
		((CItem*)pPara)->SetUndoStep(m_sUndoStep.nNewestStep, nType);
		break;
		
	default:
		if (pPara)
		{
			((CList*)pPara)->SetUndoStep(m_sUndoStep.nNewestStep, nType);
		}
		break;
	}
	
	m_sUndoStep.nLastUndoType = nType;
	ClearCache();
}

/*
功能：	将无效的撤销信息进行清理
返回值：无
备注：	清理后，移动撤销指针
*/
void CPaper::ClearUndoChain()
{
	CItem *pItem = NULL;
	CItem *pNext = NULL;
	
	// 删除已经失效的类型列表（仅包括自定义列表）
	// 并通知各列表清理无效的自保留撤销信息
	pItem = m_pHead;
	while(pItem)
	{
		pNext = pItem->GetNext();
		if (pItem->NeedUndoClear(&m_sUndoStep))
		{
			CList::RemoveItem(pItem, TRUE);
		}
		else
		{
			((CList*)pItem)->UndoClear(&m_sUndoStep);
		}
		
		pItem = pNext;
	}

	// 清理题库自身的自保留撤销信息
	CList::UndoClear(&m_sUndoStep);
	ClearUndoCoverInfo();
	
	// 删除失效的题目
	// 当标记删除自定义列表时，应将列表下的全部题目均标记删除
	pItem = m_pItemList->GetHead();
	while(pItem)
	{
		pNext = pItem->GetNext();
		if (pItem->NeedUndoClear(&m_sUndoStep))
		{
			m_pItemList->RemoveItem(pItem, TRUE);
		}
		
		pItem = pNext;
	}
	
	m_sUndoStep.nCurStep = (m_sUndoStep.nCurStep + 1) % UNDO_SIZE;
	if (m_sUndoStep.nCurStep == m_sUndoStep.nOldestStep)
	{
		m_sUndoStep.nOldestStep = (m_sUndoStep.nOldestStep + 1) % UNDO_SIZE;
	}
	if (m_sUndoStep.nSaveIndex == m_sUndoStep.nCurStep)
	{
		m_sUndoStep.nSaveIndex = -1;
	}
	m_sUndoStep.nNewestStep = m_sUndoStep.nCurStep;
}

/*
功能：	清理题库封面的撤销信息
返回值：无
备注：
*/
void CPaper::ClearUndoCoverInfo()
{
	// 清理最旧一次的撤销信息
	if (((m_sUndoStep.nCurStep + 1) % UNDO_SIZE) == m_sUndoStep.nOldestStep)
	{
		if (m_sUndoCover[m_sUndoStep.nOldestStep])
		{
			memcpy(&m_sCover, m_sUndoCover[m_sUndoStep.nOldestStep], sizeof(m_sCover));
			free(m_sUndoCover[m_sUndoStep.nOldestStep]);
			m_sUndoCover[m_sUndoStep.nOldestStep] = NULL;
		}
	}
	// 撤销后又进行新的操作，则清理更新的内容
	else if (m_sUndoStep.nCurStep != m_sUndoStep.nNewestStep)
	{
		int nStart = (m_sUndoStep.nCurStep + 1) % UNDO_SIZE;
		while(1)
		{
			if (m_sUndoCover[nStart])
			{
				ClearCoverInfo(m_sUndoCover[nStart]);
				free(m_sUndoCover[nStart]);
				m_sUndoCover[nStart] = NULL;
			}
			if (nStart == m_sUndoStep.nNewestStep)
			{
				break;
			}
			
			nStart = (nStart + 1) % UNDO_SIZE;
		}
	}
}

/*
功能：	对自定义题目类型列表进行动作检查，以检查是否有新增或删除动作
返回值：在更新视图时是否需要传递参数
备注：	传递的参数为需要更改的自定义题目列表
*/
BOOL CPaper::UndoCustomLists(TPARAS *pListInfo)
{
	ZeroMemory(pListInfo, sizeof(TPARAS));
	CTempList *pList = GetTypeList(TYPE_CUSTOMRIGHT);
	BOOL bRet = FALSE;
	
	while(pList)
	{
		// 如果撤销后没有此列表了，说明删掉了
		if(!pList->IsUndoValid(&m_sUndoStep) && pList->GetGUIHandle())
		{
			pListInfo->dwPara1 = (DWORD)pList;
			pListInfo->dwPara2 = UNDO_DELITEM;
			
			break;
		}
		else if (pList->IsUndoValid(&m_sUndoStep) && !pList->GetGUIHandle())
		{
			pListInfo->dwPara1 = (DWORD)pList;
			pListInfo->dwPara2 = UNDO_ADDITEM;
			
			break;
		}
		
		// 这里应检查所有的列表内容
		pList = (CTempList*)pList->GetNext();
	}
	
	// 修改自定义类型的类型号
	if (pList)
	{
		bRet = TRUE;
		
		pList = (CTempList*)pList->GetNext();
		while(pList)
		{
			if (pListInfo->dwPara2 == UNDO_ADDITEM)
			{
				pList->SetCustomIndex(pList->GetCustomIndex() + 1);
			}
			else
			{
				pList->SetCustomIndex(pList->GetCustomIndex() - 1);
			}
			
			pList = (CTempList*)pList->GetNext();
		}
	}
	
	return bRet;
}
