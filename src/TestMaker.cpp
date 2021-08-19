// TestMaker.cpp: implementation of the CTestMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestMaker.h"

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

CTestMaker::CTestMaker(CPaper *pPaper) :
	m_pPaper(pPaper), m_pCPara(NULL),
	m_nGrpCustCount(0), m_GrpCustList(NULL)
{

}

CTestMaker::~CTestMaker()
{
	if (m_GrpCustList)
	{
		free(m_GrpCustList);
	}
}

BOOL CTestMaker::MakeTest(CList *pList, CTestPara *pCPara)
{
	m_pCPara = pCPara;
	if (!m_pCPara)
	{
		return FALSE;
	}

	if (!FillGrpCustList())
	{
		return FALSE;
	}
	pList->ClearAllItems(TRUE);

	if (!MakeQNumList(pList, TRUE))
	{
		return FALSE;
	}

	if (!MakeChoiseList(pList) ||
		!MakeSingleList(pList, TYPE_JUDGE) ||
		!MakeSingleList(pList, TYPE_BLANK) ||
		!MakeSingleList(pList, TYPE_TEXT))
	{
		return FALSE;
	}

	if (!MakeQNumList(pList, FALSE))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CTestMaker::FillGrpCustList()
{
	PTESTPARA pPara = m_pCPara->GetTestPara();
	CPaper *pPaper = pPara->pPaper;
	
	m_nGrpCustCount = pPara->nGrpCount + pPaper->GetCustomTypeCount();

	if (m_GrpCustList)
	{
		free(m_GrpCustList);
	}
	m_GrpCustList = (CItem**)malloc(m_nGrpCustCount * sizeof(CItem*));
	if (!m_GrpCustList)
	{
		m_nGrpCustCount = 0;
		return FALSE;
	}

	// 保存全部的题目组和自定义类型指针
	int i = 0, it;
	CTempList TmpList;
	int n = pPaper->GetItemCount(TYPE_GROUP);
	
	for (i = 0; i < n; ++i)
	{
		TmpList.AddItem(pPaper->GetItem(TYPE_GROUP, i));
	}
	TmpList.Random(0, 0, time(NULL) + _getpid());
	for (i = 0; i < pPara->nGrpCount; ++i)
	{
		m_GrpCustList[i] = TmpList.GetItem(i);
	}
	
	n = pPaper->GetCustomTypeCount();
	for (it = 1; it <= n; ++it, ++i)
	{
		m_GrpCustList[i] = pPaper->GetTypeList(it * TYPE_CUSTOMRIGHT);
	}
	ASSERT(i == m_nGrpCustCount);

	// 按题号顺序进行调整
	if (pPara->bQNumOrder)
	{
		int j = 0, nQNum = 1;
		CItem** GrpCustList = (CItem**)malloc(m_nGrpCustCount * sizeof(CItem*));
		if (!GrpCustList)
		{
			return FALSE;
		}

		while(nQNum <= LIST_MAXQNUMBER)
		{
			for (i = 0; i < m_nGrpCustCount; ++i)
			{
				if (!m_GrpCustList[i])
				{
					continue;
				}
				else if (((CList*)m_GrpCustList[i])->GetQNumber() == nQNum)
				{
					GrpCustList[j] = m_GrpCustList[i];
					m_GrpCustList[i] = NULL;
					++j;
				}
			}
			++nQNum;
		}

		for (i = 0; i < m_nGrpCustCount; ++i)
		{
			if (!m_GrpCustList[i])
			{
				continue;
			}
			GrpCustList[j] = m_GrpCustList[i];
			++j;
		}

		free(m_GrpCustList);
		m_GrpCustList = GrpCustList;
	}

	return TRUE;
}

BOOL CTestMaker::MakeChoiseList(CList *pList)
{
	int i, n;
	PTESTPARA pPara = m_pCPara->GetTestPara();
	if (!pPara->nSChCount && !pPara->nMChCount)
	{
		return TRUE;
	}
	CList List;
	int nSeed = time(NULL) + _getpid();

	// 将所有选择题存入，并打乱顺序
	if (!m_pPaper->AllocSingleItems(&List, TYPE_SCHOISE, pPara->bUseStore) ||
		!m_pPaper->AllocSingleItems(&List, TYPE_MCHOISE, pPara->bUseStore))
	{
		return FALSE;
	}
	List.Random(0, 0, nSeed);

	CItem *pItem;

	// 写入单选题
	for (i = 0, n = 0; i < pPara->nSChCount;)
	{
		pItem = List.GetItem(n);
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
			pItem->ChangeAsPara(m_pCPara, nSeed);
			pList->AddItem(pItem);
			++i;
			break;

		case TYPE_MCHOISE:
			if ((pPara->dwChsOptions & CHSOPTION_MCHASSCH) &&
				(((CChoise*)pItem)->GetAnswerCount() == 1))
			{
				CChoise *pTemp = new CChoise(TRUE);
				if (!pTemp)
				{
					return FALSE;
				}
				pTemp->CopyChoiseIgnoreType(pItem);
				pTemp->ChangeAsPara(m_pCPara, nSeed);
				pList->AddItem(pTemp);
				List.RemoveItem(pItem, TRUE);
				++i;
			}
			else
			{
				++n;
			}
			break;

		default:
			ASSERT(FALSE);
		}
	}

	// 写入多选题
	for (i = 0, n = 0; i < pPara->nMChCount;)
	{
		pItem = List.GetItem(n);
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
			if (pPara->dwChsOptions & CHSOPTION_SCHASMCH)
			{
				CChoise *pTemp = new CChoise(FALSE);
				if (!pTemp)
				{
					return FALSE;
				}
				pTemp->CopyChoiseIgnoreType(pItem);
				pTemp->ChangeAsPara(m_pCPara, nSeed);
				pList->AddItem(pTemp);
				++i;
			}
			else
			{
				++n;
			}
			break;

		case TYPE_MCHOISE:
			if(!((pPara->dwChsOptions & CHSOPTION_MCHNOSINGLE) &&
				(((CChoise*)pItem)->GetAnswerCount() < 2)))
			{
				pItem->ChangeAsPara(m_pCPara, nSeed);
				pList->AddItem(pItem);
				++i;
			}
			else
			{
				++n;
			}
			break;

		default:
			ASSERT(FALSE);
		}
	}

	return TRUE;
}

BOOL CTestMaker::MakeSingleList(CList *pList, int nType)
{
	PTESTPARA pPara = m_pCPara->GetTestPara();
	
	int nListCount;
	switch(nType)
	{
	case TYPE_JUDGE:
		if (!pPara->nJdgCount)
		{
			return TRUE;
		}
		nListCount = pPara->nJdgCount;
		break;
		
	case TYPE_BLANK:
		if (!pPara->nBlkCount)
		{
			return TRUE;
		}
		nListCount = pPara->nBlkCount;
		break;
		
	case TYPE_TEXT:
		if (!pPara->nTxtCount)
		{
			return TRUE;
		}
		nListCount = pPara->nTxtCount;
		break;
		
	default:
		ASSERT(FALSE);
	}

	int i;
	CList List;
	CItem *pItem;
	if (!m_pPaper->AllocSingleItems(&List, nType, pPara->bUseStore))
	{
		return FALSE;
	}
	List.Random(0, 0, time(NULL) + _getpid());

	for (i = 0; i < nListCount; ++i)
	{
		pItem = List.GetItem(0);

		pItem->ChangeAsPara(m_pCPara);
		pList->AddItem(pItem);
	}
	
	return TRUE;
}

BOOL CTestMaker::MakeQNumList(CList *pList, BOOL bQNumOrder)
{
	PTESTPARA pPara = m_pCPara->GetTestPara();
	if (!pPara->bQNumOrder && bQNumOrder)
	{
		return TRUE;
	}

	int i;
	CList *pThis;
	CList List;
	int nSeed = time(NULL) + _getpid();
	for (i = 0; i < m_nGrpCustCount; ++i)
	{
		CItemGroup *pGroup;
		CItem *pItem;
		int n, nCount;

		pThis = (CList*)m_GrpCustList[i];
		if (!pThis)
		{
			continue;
		}
		if (!pThis->GetQNumber() && bQNumOrder)
		{
			break;
		}

		switch(pThis->GetType())
		{
		case TYPE_GROUP:
			pGroup = (CItemGroup*)((CItemGroup*)pThis)->Alloc();
			if (!pGroup)
			{
				return FALSE;
			}
			pGroup->ChangeAsPara(m_pCPara);
			pList->AddItem(pGroup);
			break;

		default:
			if (!m_pPaper->AllocCustomItems(&List, pThis->GetCustomIndex(), pPara->bUseStore))
			{
				return FALSE;
			}
			nCount = m_pCPara->GetCustomCount(pThis->GetRawType());
			List.Random(0, 0, nSeed);
			for (n = 0; n < nCount; ++n)
			{
				pItem = List.GetItem(0);
				pItem->ChangeAsPara(m_pCPara);
				pList->AddItem(pItem);
			}
			List.ClearAllItems(TRUE);
			break;
		}
		m_GrpCustList[i] = NULL;
	}

	return TRUE;
}

BOOL FUNCCALL CTestMaker::MakeAtomList(CList *pIn, CTempList *pOut)
{
	if (!pIn)
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < pIn->GetItemCount(); ++i)
	{
		CItem *pItem = pIn->GetItem(i);
		if (pItem->GetType() == TYPE_GROUP)
		{
			((CItemGroup*)pItem)->PointSubItem(pOut);
		}
		else
		{
			pOut->AddItem(pItem);
		}
	}
	return TRUE;
}