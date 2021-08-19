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
���ܣ�	��⳷������
����ֵ���Ƿ��pListInfo�������޸�
��ע��	�����Զ������ͽ����˸Ķ��󣬽����pListInfo����
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
���ܣ�	�����������
����ֵ���Ƿ��pListInfo�������޸�
��ע��	�����Զ������ͽ����˸Ķ��󣬽����pListInfo����
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
���ܣ�	ÿ�ζ������иĶ��󣬾����ô˺����������泷����Ϣ
����ֵ����
��ע��	�Գ���ָ���ǰ�������ͶԳ�����Ϣ����������ClearUndoChain()����ɵ�
*/
void CPaper::SetUndoChain(PVOID pPara, int nType)
{
	// ���ȴ��������費���ӵ����
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
���ܣ�	����Ч�ĳ�����Ϣ��������
����ֵ����
��ע��	������ƶ�����ָ��
*/
void CPaper::ClearUndoChain()
{
	CItem *pItem = NULL;
	CItem *pNext = NULL;
	
	// ɾ���Ѿ�ʧЧ�������б��������Զ����б�
	// ��֪ͨ���б�������Ч���Ա���������Ϣ
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

	// �������������Ա���������Ϣ
	CList::UndoClear(&m_sUndoStep);
	ClearUndoCoverInfo();
	
	// ɾ��ʧЧ����Ŀ
	// �����ɾ���Զ����б�ʱ��Ӧ���б��µ�ȫ����Ŀ�����ɾ��
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
���ܣ�	����������ĳ�����Ϣ
����ֵ����
��ע��
*/
void CPaper::ClearUndoCoverInfo()
{
	// �������һ�εĳ�����Ϣ
	if (((m_sUndoStep.nCurStep + 1) % UNDO_SIZE) == m_sUndoStep.nOldestStep)
	{
		if (m_sUndoCover[m_sUndoStep.nOldestStep])
		{
			memcpy(&m_sCover, m_sUndoCover[m_sUndoStep.nOldestStep], sizeof(m_sCover));
			free(m_sUndoCover[m_sUndoStep.nOldestStep]);
			m_sUndoCover[m_sUndoStep.nOldestStep] = NULL;
		}
	}
	// �������ֽ����µĲ�������������µ�����
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
���ܣ�	���Զ�����Ŀ�����б���ж�����飬�Լ���Ƿ���������ɾ������
����ֵ���ڸ�����ͼʱ�Ƿ���Ҫ���ݲ���
��ע��	���ݵĲ���Ϊ��Ҫ���ĵ��Զ�����Ŀ�б�
*/
BOOL CPaper::UndoCustomLists(TPARAS *pListInfo)
{
	ZeroMemory(pListInfo, sizeof(TPARAS));
	CTempList *pList = GetTypeList(TYPE_CUSTOMRIGHT);
	BOOL bRet = FALSE;
	
	while(pList)
	{
		// ���������û�д��б��ˣ�˵��ɾ����
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
		
		// ����Ӧ������е��б�����
		pList = (CTempList*)pList->GetNext();
	}
	
	// �޸��Զ������͵����ͺ�
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
