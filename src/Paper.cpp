// Paper.cpp: implementation of the CPaper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Paper.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"

#include "TestMaker.h"
#include "ImageManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaper::CPaper() :
	CList(TYPE_DEFAULT),
	m_dwVersion(FILEVERSION), m_nDefSaveStyle(PAPER_FILETYPE_BIN), m_nCompressType(PAPER_COMPRESS_ZLIB),
	m_bCanEdit(TRUE), m_bModify(FALSE),
	m_pSaveList(NULL),
	m_nEncryption(ENCRYPT_NO),
	m_nTypeCache(TYPE_DEFAULT), m_pItemCache(NULL), m_nItemCache(-1)
{
	ZeroMemory(m_cPassword, sizeof(m_cPassword));
	ZeroMemory(m_uMD5, sizeof(m_uMD5));

	ZeroMemory(&m_sUndoStep, sizeof(m_sUndoStep));
	m_sUndoStep.nSaveIndex = -1;

	ZeroMemory(&m_sCover, sizeof(m_sCover));
	m_sCover.nWidth = JPEGCOVER_DEFAULTWIDTH;
	m_sCover.nHeight = JPEGCOVER_DEFAULTHEIGHT;
	m_sCover.clrBk = JPEGCOVER_DEFAULTBKCOLOR;
	m_sCover.clrText = JPEGCOVER_DEFAULTTXTCOLOR;
	m_sCover.nQuality = JPEGCOVER_DEFAULTQUALITY;
	ZeroMemory(m_sUndoCover, sizeof(m_sUndoCover));
}

CPaper::~CPaper()
{
	// �˳���ʱ�򱣴���Ȿ
	SaveSave();

	ClearAllItems(TRUE);
	
	if (m_pItemList)
	{
		delete m_pItemList;
		m_pItemList = NULL;
	}
	if (m_pSaveList)
	{
		delete m_pSaveList;
		m_pSaveList = NULL;
	}
	
	ClearCoverInfo(&m_sCover);
	int i;
	for (i = 0; i < UNDO_SIZE; i++)
	{
		if (m_sUndoCover[i])
		{
			ClearCoverInfo(m_sUndoCover[i]);
			free(m_sUndoCover[i]);
			m_sUndoCover[i] = NULL;
		}
	}
}

/*
���ܣ�	��ȡ�ض���Ŀ���͵�ָ���б�
����ֵ���ض���Ŀ���͵�ָ���б�
��ע��	���ص���ָ���б�
		ָ����Ŀ����ʱ��������Զ������
*/
CTempList* CPaper::GetTypeList(int nType)
{
	ASSERT(TYPE_DEFAULT != nType);
	if (nType < TYPE_CUSTOMRIGHT)
	{
		nType = AfxTypeToIndex(nType);
	}
	else
	{
		nType = (nType / TYPE_CUSTOMRIGHT) + TYPE_LASTDEFAULT - 1;
	}

	return (CTempList*)CList::GetItem(nType);
}

/*
���ܣ�	��ȡ�����ĳ��������Ŀ��
����ֵ��ĳ��������Ŀ��
��ע��	����ֵ�������ѱ�ɾ����δ�����ٵ���Ŀ
*/
int CPaper::GetItemCount(int nType)
{
	CTempList *pList = GetTypeList(nType);
	if (!pList)
	{
		return 0;
	}

	int n = 0, i = 0;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		if (pItem->GetItemFrom()->IsUndoValid(&m_sUndoStep))
		{
			++n;
		}
	}

	return n;
}

/*
���ܣ�	��ȡ������Զ�����������
����ֵ���Զ�����������
��ע��	����ֵ�������ѱ�ɾ����δ�����ٵ���Ŀ
*/
int CPaper::GetCustomTypeCount()
{
	int n = 0;
	CItem *p = GetHead();
	while(p)
	{
		if (p->IsUndoValid(&m_sUndoStep))
		{
			n++;
		}
		p = p->GetNext();
	}

	return n - TYPE_DEFAULTCOUNT;
}

/*
���ܣ�	��鵱ǰ��Ŀ���Ƿ��г�ͻ
����ֵ���г�ͻ���س�ͻ�б����򷵻�NULL
��ע��	������Ϊ0ʱ��Ϊȱʡֵ���������ж�
*/
CList* CPaper::CheckQNumberValid(int n, CList *pThis)
{
	CTempList *pList;
	CItemGroup *pGroup;
	int i;
	
	if (!n)
	{
		return NULL;
	}

	pList = GetTypeList(TYPE_GROUP);
	if (!pList)
	{
		goto _CheckQNumber_List;
	}
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		pGroup = (CItemGroup*)pList->GetItem(i);
		if (pGroup->GetQNumber() == n &&
			pThis->GetItemFrom() != pGroup)
		{
			return pGroup;
		}
	}
	
_CheckQNumber_List:
	for (i = 1; i <= TYPE_MAXCUSTOMCOUNT; ++i)
	{
		pList = GetTypeList(TYPE_CUSTOMRIGHT * i);
		if (!pList)
		{
			break;
		}
		
		if (pList->GetQNumber() == n &&
			pList != pThis->GetItemFrom())
		{
			return pList;
		}
	}

	return NULL;
}

/*
���ܣ�	�����ض����͵���Ŀ���������pList��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	pList����ӵĶ����½������Ŀ����Ӱ�챾�����ԭ�е���Ŀ
		��bStoreΪTRUE��������Ʊ��ղص���Ŀ
		���ۺ�������������ǰ�������鳷����Ϣ
*/
BOOL CPaper::AllocSingleItems(CList *pList, int nType, BOOL bStore)
{
	ASSERT(pList);
	ASSERT(nType != TYPE_DEFAULT);

	if (bStore)
	{
		CItemPtr *pItem = (CItemPtr*)m_pSaveList->GetHead();
		CItem *pFrom = NULL, *pNew = NULL;
		while(pItem)
		{
			pFrom = pItem->GetItemFrom();
			if (pFrom->IsUndoValid(&m_sUndoStep) && pFrom->GetRawType() == nType)
			{
				pNew = pFrom->Alloc();
				if (!pNew)
				{
					return FALSE;
				}
				
				pList->AddItem(pNew);
			}
			
			pItem = (CItemPtr*)pItem->GetNext();
		}
	}
	else
	{
		CItem *pItem = m_pItemList->GetHead(), *pNew = NULL;
		while(pItem)
		{
			if (pItem->IsUndoValid(&m_sUndoStep) && pItem->GetRawType() == nType)
			{
				pNew = pItem->Alloc();
				if (!pNew)
				{
					return FALSE;
				}
				
				pList->AddItem(pNew);
			}
			
			pItem = pItem->GetNext();
		}
	}

	return TRUE;
}

/*
���ܣ�	�����ض����Զ���������Ŀ���������pList��
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	pList����ӵĶ����½������Ŀ����Ӱ�챾�����ԭ�е���Ŀ
		��bStoreΪTRUE��������Ʊ��ղص���Ŀ
		���ۺ�������������ǰ�������鳷����Ϣ
*/
BOOL CPaper::AllocCustomItems(CList *pList, int nCustIndex, BOOL bStore)
{
	ASSERT(pList);
	
	if (bStore)
	{
		CItemPtr *pItem = (CItemPtr*)m_pSaveList->GetHead();
		CItem *pFrom = NULL, *pNew = NULL;
		while(pItem)
		{
			pFrom = pItem->GetItemFrom();
			if (pFrom->IsUndoValid(&m_sUndoStep) && pFrom->GetCustomIndex() == nCustIndex)
			{
				pNew = pFrom->Alloc();
				if (!pNew)
				{
					return FALSE;
				}
				
				pList->AddItem(pNew);
			}
			
			pItem = (CItemPtr*)pItem->GetNext();
		}
	}
	else
	{
		CItem *pItem = m_pItemList->GetHead(), *pNew = NULL;
		while(pItem)
		{
			if (pItem->IsUndoValid(&m_sUndoStep) && pItem->GetCustomIndex() == nCustIndex)
			{
				pNew = pItem->Alloc();
				if (!pNew)
				{
					return FALSE;
				}

				pList->AddItem(pNew);
			}
			
			pItem = pItem->GetNext();
		}
	}
	
	return TRUE;
}

/*
���ܣ�	��ʼ��һ��������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	��ʼ���������£�
		������Ŀ�б�
		����������б�ָ���б�����ʱ��δ�����Զ�������б�
		�����ղ��б�
		��ʼ��������ơ����桢������
*/
EBOOL CPaper::InitPaper(BOOL bIsNewPaper)
{
	BOOL ret = TRUE;
	CTempList *pList = NULL;
	CString string;

	ASSERT(!m_nItemCount);

	// ȫ����Ŀ�б�
	m_pItemList = new CList();
	if (!m_pItemList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	m_pItemList->SetParent(this);

	// ��ѡ�б�
	pList = new CTempList(TYPE_SCHOISE);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_SCH);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// ��ѡ�б�
	pList = new CTempList(TYPE_MCHOISE);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_MCH);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// �ж����б�
	pList = new CTempList(TYPE_JUDGE);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_JDG);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// ������б�
	pList = new CTempList(TYPE_BLANK);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_BLK);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// ������б�
	pList = new CTempList(TYPE_TEXT);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_TXT);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// ��Ŀ���б�
	pList = new CTempList(TYPE_GROUP);
	if (!pList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_ITEMTYPE_GRP);
	if (!pList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	CList::AddItem(pList);

	// �ղ���Ŀ
	m_pSaveList = new CTempList(FALSE);
	if (!m_pSaveList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	string.LoadString(IDS_PAPER_STORELIST);
	if (!m_pSaveList->SetDescription(string))
	{
		return Q_ERROR(ERROR_INNER);
	}
	m_pSaveList->SetParent(this);

	if (bIsNewPaper)
	{
		string.LoadString(IDS_PAPER_DEFAULTDESC);
		if (!CItem::SetDescription(string))
		{
			return Q_ERROR(ERROR_INNER);
		}

		ClearCoverInfo(&m_sCover);
		m_sCover.strTitle = _tcsdup(string);

		int i;
		for (i = 0; i < UNDO_SIZE; i++)
		{
			if (m_sUndoCover[i])
			{
				ClearCoverInfo(m_sUndoCover[i]);
				free(m_sUndoCover[i]);
				m_sUndoCover[i] = NULL;
			}
		}
	}

	ZeroMemory(m_strFilePath, sizeof(m_strFilePath));
	ZeroMemory(&m_sUndoStep, sizeof(m_sUndoStep));

	ClearCache();

	m_bModify = FALSE;
	return Q_TRUE;
}

/*
���ܣ�	������������
����ֵ���ɹ�����TRUE�����򷵻�FALSE
��ע��	������Ʋ���Ϊ��
*/
BOOL CPaper::SetDescription(LPCTSTR szDescription)
{
	if (!szDescription)
	{
		return FALSE;
	}

	SetUndoChain(NULL, UNDO_DESC);

	ASSERT(LISTUNDO_NONE == m_UndoInfo[m_sUndoStep.nCurStep].nType);
	m_UndoInfo[m_sUndoStep.nCurStep].nType = LISTUNDO_SETDESC;
	m_UndoInfo[m_sUndoStep.nCurStep].dwData = (DWORD)_tcsdup(szDescription);
	if (!m_UndoInfo[m_sUndoStep.nCurStep].dwData)
	{
		return FALSE;
	}

	m_bModify = TRUE;
	return TRUE;
}

/*
���ܣ�	�����������
����ֵ��������������ִ�ָ��
��ע��
*/
LPCTSTR CPaper::GetDescription()
{
	int nStart = m_sUndoStep.nCurStep;
	while(1)
	{
		if (LISTUNDO_SETDESC == m_UndoInfo[nStart].nType)
		{
			return (LPCTSTR)m_UndoInfo[nStart].dwData;
		}
		
		if (nStart == m_sUndoStep.nOldestStep)
		{
			break;
		}
		
		nStart = (nStart + UNDO_SIZE - 1) % UNDO_SIZE;
	}
	
	return CItem::GetDescription();
}

LPBYTE CPaper::GetPassword()
{
	if (IsLocked())
	{
		return m_uMD5;
	}
	else
	{
		return NULL;
	}
}

BOOL CPaper::CheckPassword(const LPBYTE pPswd)
{
	if (!pPswd)
	{
		return FALSE;
	}
	else if (!IsLocked())
	{
		return TRUE;
	}

	return memcmp(m_uMD5, pPswd, sizeof(m_uMD5)) ? FALSE : TRUE;
}

void CPaper::SetPassword(LPBYTE pPswd)
{
	m_nEncryption = pPswd[0] ? ENCRYPT_CBC128AES : ENCRYPT_NO;
	memcpy(m_uMD5, pPswd, sizeof(m_uMD5));

	m_bModify = TRUE;

	// �κ�һ���޸����룬����Ҫ���±���
	m_sUndoStep.nSaveIndex = -1;
}

void CPaper::SetPasswordText(LPCSTR strPass)
{
	strcpy(m_cPassword, strPass);
}

/*
���ܣ�	����������һ����Ŀ
����ֵ���ɹ��򷵻�TRUE�����򷵻�FALSE
��ע��	�������������ڵ���
		���������漰��������������pItem��ԭ���б�������
		��pItem����Դ��pItem����ͬʱ�����ж�Ϊ�༭����
		����������Ŀû����Դ
*/
BOOL CPaper::AddItem(CItem *pItem, int nCheckType)
{
	if (!m_bCanEdit)
	{
		return FALSE;
	}

	if (nCheckType != TYPE_DEFAULT)
	{
		if (pItem->GetRawType() != nCheckType)
		{
			return FALSE;
		}
	}

	// �ڱ༭ʱ���޸ĺ���������Ŀ�������޸�ǰɾ����Ŀ��ǰ��
	// ��֤���޸ĺ���Ŀ��˳�򲻷�����ı仯
	if (pItem->GetItemFrom() != pItem)
	{
		m_pItemList->InsertItem(pItem, pItem->GetItemFrom(), FALSE);
	}
	else
	{
		m_pItemList->AddItem(pItem);
	}
	
	// ����������Ŀ����û����Դ
	pItem->ClearFromPtr();

	// ����һ��CRC��
	pItem->CalcCRC();

	m_bModify = TRUE;
	return TRUE;
}

/*
���ܣ�	����������һ����Ŀ
����ֵ����������һ���򷵻�TRUE�����򷵻�FALSE
��ע��	�������������������Ŀ��ֻ��ͨ����һ�ַ�ʽ
		�����������Ŀ����Ϊ�ƶ���ʽ�������ٹ�����Ŀ�����Ҳ�֧��ͨ��ָ���б�����
		����Ҫ�ж���Ŀ������������Ҫ�����Ŀ���룬�������߲���������ԭ�б�������
*/
BOOL CPaper::AddList(CList *pList, int nCheckType /* = TYPE_DEFAULT */)
{
	BOOL bRet = FALSE;
	if (!m_bCanEdit || !pList)
	{
		return FALSE;
	}
	ASSERT(!pList->IsTempList());

	// ������Զ��������Ŀʱ
	// �����ǰ�����û����ͬ�Զ�������б����޷�����
	if (CItem::GetCustomIndex(nCheckType) > GetCustomTypeCount())
	{
		return FALSE;
	}

	CItem *pItem = pList->GetHead();
	while(pItem)
	{
		if (TYPE_DEFAULT == nCheckType)
		{
			if (pItem->IsDefaultType())
			{
				break;
			}
		}
		else
		{
			if (pItem->GetType() == CItem::GetType(nCheckType))
			{
				break;
			}
		}
		pItem = pItem->GetNext();
	}
	if (!pItem)
	{
		return FALSE;
	}

	// �������ȫ���б�����ӳ�����
	// ���ʵ��δ���룬Ҳ��Ӱ��
	SetUndoChain(pList, UNDO_ADD);
	
	CItem *pNext = NULL;
	pItem = pList->GetHead();
	BOOL bAdd;
	while(pItem)
	{
		pNext = pItem->GetNext();
		bAdd = FALSE;

		// �����ж�����ʱ���������Զ���������
		if (TYPE_DEFAULT == nCheckType)
		{
			bAdd = pItem->IsDefaultType();
		}
		// ���ж�����ʱ������������ͬ�����Լ���
		else if (CItem::GetType(nCheckType) == pItem->GetType())
		{
			pItem->SetCustomIndex(CItem::GetCustomIndex(nCheckType));
			bAdd = TRUE;
		}
		
		if (bAdd)
		{
			pList->RemoveItem(pItem, FALSE);
			AddItem(pItem, nCheckType);
			bRet = TRUE;
		}
		pItem = pNext;
	}

	ClearCache();
	ResetTypeList();
	return bRet;
}

/*
���ܣ�	������������б�
����ֵ����
��ע��	��nTypeΪTYPE_DEFAULTʱ������������ȫ������б�
*/
void CPaper::ResetTypeList(int nType /* = TYPE_DEFAULT */)
{
	CTempList *pList = (CTempList*)GetHead();
	while(pList)
	{
		if (TYPE_DEFAULT == nType || nType == pList->GetRawType())
		{
			pList->ClearAllItems(TRUE);
		}
		pList = (CTempList*)pList->GetNext();
	}

	CItem *pItem = m_pItemList->GetHead(), *pNext;
	while(pItem)
	{
		pNext = pItem->GetNext();

		if (pItem->IsUndoValid(&m_sUndoStep) && (TYPE_DEFAULT == nType || pItem->GetRawType() == nType))
		{
			pList = GetTypeList(pItem->GetRawType());
			if (pList)
			{
				pList->AddItem(pItem);
			}
			else
			{
				m_pItemList->RemoveItem(pItem, TRUE);
			}
		}

		pItem = pNext;
	}
}

/*
���ܣ�	��ȡ������˳�����ŵ���Ŀ
����ֵ����ȡ������Ŀ
��ע��
*/
CItem* CPaper::GetItem(int nIndex)
{
	CItem *pItem;

	// ���֮ǰ�ǰ�����Ŀ��������ҵ�
	// ����Ҫ��ԭ
	if (TYPE_DEFAULT != m_nTypeCache)
	{
		ClearCache();
	}
	if (nIndex == m_nItemCache)
	{
		return m_pItemCache;
	}
	else if (m_nItemCache >= 0 && nIndex > m_nItemCache)
	{
		ASSERT(m_pItemCache->IsUndoValid(&m_sUndoStep));
		pItem = m_pItemCache;
		nIndex -= m_nItemCache;
	}
	else
	{
		pItem = m_pItemList->GetHead();
		m_nItemCache = 0;
	}
	
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (nIndex <= 0)
			{
				break;
			}
			nIndex--;
			m_nItemCache++;
		}

		pItem = pItem->GetNext();
	}

	if (pItem)
	{
		m_pItemCache = pItem;
		return pItem;
	}
	else
	{
		ClearCache();
		return NULL;
	}
}

/*
���ܣ�	��ȡ������˳����ŵ��ض����͵���Ŀ
����ֵ����ȡ������Ŀ
��ע��	������ȡ�õ���Ŀ����ȡ�д��ʵ������Ŀָ�룬��Ҫ����ת��
*/
CItem* CPaper::GetItem(int nType, int nIndex)
{
	if (TYPE_DEFAULT == nType)
	{
		return GetItem(nIndex);
	}

	CItemPtr *pItem;
	if (nType != m_nTypeCache)
	{
		ClearCache();
	}
	if (nIndex == m_nItemCache)
	{
		return ((CItemPtr*)m_pItemCache)->m_pItem;
	}
	else if (m_nItemCache >= 0 && nIndex > m_nItemCache)
	{
		ASSERT(m_pItemCache->IsUndoValid(&m_sUndoStep));
		pItem = (CItemPtr*)m_pItemCache;
		nIndex -= m_nItemCache;
	}
	else
	{
		CTempList *pList = GetTypeList(nType);
		if (!pList)
		{
			return NULL;
		}

		pItem = (CItemPtr*)pList->GetHead();
		m_nItemCache = 0;
	}

	while(pItem)
	{
		if (pItem->GetItemFrom()->IsUndoValid(&m_sUndoStep))
		{
			if (nIndex <= 0)
			{
				break;
			}
			nIndex--;
			m_nItemCache++;
		}

		pItem = (CItemPtr*)pItem->GetNext();
	}

	if (pItem)
	{
		m_nTypeCache = nType;
		m_pItemCache = pItem;
		return pItem->GetItemFrom();
	}
	else
	{
		ClearCache();
		return NULL;
	}
}

/*
���ܣ�	��ȡ�������Ŀ����
����ֵ���������Ŀ������
��ע��	��õ���������������������Ŀ
*/
int CPaper::GetItemCount()
{
	CItem *pItem = m_pItemList->GetHead();
	int nCount = 0;
	
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			nCount++;
		}

		pItem = pItem->GetNext();
	}
	
	return nCount;
}

/*
���ܣ�	��ȡ����е����𰸵�ѡ����������Ҳ���������𰸵Ķ�ѡ�⣩
����ֵ�������𰸵�ѡ������Ŀ����
��ע��	��õ���������������������Ŀ
*/
int CPaper::GetSingleChoiseCount()
{
	int nCount = 0;
	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->GetRawType() == TYPE_SCHOISE ||
			(pItem->GetRawType() == TYPE_MCHOISE && ((CChoise*)pItem)->GetAnswerCount() < 2))
		{
			if (pItem->IsUndoValid(&m_sUndoStep))
			{
				nCount++;
			}
		}

		pItem = pItem->GetNext();
	}

	return nCount;
}

/*
���ܣ�	��ȡ����ж���𰸵�ѡ��������
����ֵ������𰸵�ѡ������Ŀ����
��ע��	��õ���������������������Ŀ
*/
int CPaper::GetMultiChoiseCount()
{
	int nCount = 0;
	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->GetRawType() == TYPE_MCHOISE &&
			((CChoise*)pItem)->GetAnswerCount() >= 2)
		{
			if (pItem->IsUndoValid(&m_sUndoStep))
			{
				nCount++;
			}
		}
		
		pItem = pItem->GetNext();
	}
	
	return nCount;
}

/*
���ܣ�	������б�ѡ�еĵ�ѡ��ת��Ϊ��ѡ��
����ֵ����
��ע��
*/
void CPaper::SingleChoiseToMultiChoise()
{
	CList list;
	CItem *pItem = m_pItemList->GetHead(), *pNew = NULL;
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (pItem->GetRawType() != TYPE_SCHOISE)
			{
				pItem->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			}
			else if (pItem->GetItemState(ITEMMARK_EDIT))
			{
				pNew = pItem->Alloc();
				if (!pNew)
				{
					return;
				}
				pNew->SetStore(pItem->IsStored());
				pNew->SetType(TYPE_MCHOISE);
				list.AddItem(pNew);
			}
		}

		pItem = pItem->GetNext();
	}
	if (list.GetItemCount() <= 0)
	{
		return;
	}

	RemoveAllMarkedItems(ITEMMARK_EDIT);
	AddList(&list);
}

/*
���ܣ�	������б�ѡ�еĵ����𰸶�ѡ��ת��Ϊ��ѡ��
����ֵ����
��ע��
*/
void CPaper::MultiChoiseToSingleChoise()
{
	CList list;
	CItem *pItem = m_pItemList->GetHead(), *pNew = NULL;
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (pItem->GetRawType() != TYPE_MCHOISE)
			{
				pItem->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			}
			else if (pItem->GetItemState(ITEMMARK_EDIT) && ((CChoise*)pItem)->GetAnswerCount() <= 1)
			{
				pNew = pItem->Alloc();
				if (!pNew)
				{
					return;
				}
				pNew->SetStore(pItem->IsStored());
				pNew->SetType(TYPE_SCHOISE);
				list.AddItem(pNew);
			}
		}

		pItem = pItem->GetNext();
	}
	if (list.GetItemCount() <= 0)
	{
		return;
	}
	
	RemoveAllMarkedItems(ITEMMARK_EDIT);
	AddList(&list);
}

/*
���ܣ�	������б�ѡ�е�ѡ����ת��Ϊ�ж���
����ֵ����
��ע��	ת��ʱ���ж�ѡ�����ݣ��Ҵ𰸶���һ��ʱ����ת��
*/
void CPaper::ChoiseToJudge()
{
	CList list;
	CItem *pItem = m_pItemList->GetHead(), *pNew = NULL;
	while(pItem)
	{
		if (pItem->IsUndoValid(&m_sUndoStep))
		{
			if (pItem->GetRawType() != TYPE_SCHOISE &&
				pItem->GetRawType() != TYPE_MCHOISE)
			{
				pItem->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
			}
			else if (pItem->GetItemState(ITEMMARK_EDIT))
			{
				pNew = ((CChoise*)pItem)->MakeJudge();
				if (pNew)
				{
					pNew->SetStore(pItem->IsStored());
					list.AddItem(pNew);
				}
				else
				{
					pItem->SetItemState(ITEMMARK_NULL, ITEMMARK_EDIT);
				}
			}
		}

		pItem = pItem->GetNext();
	}
	if (list.GetItemCount() <= 0)
	{
		return;
	}
	
	SaveStoreQuestions(NULL);
	RemoveAllMarkedItems(ITEMMARK_EDIT);
	AddList(&list);
}

LPCTSTR CPaper::GetFilePath()
{
	if (m_strFilePath[0])
	{
		return m_strFilePath;
	}
	else
	{
		return NULL;
	}
}

void CPaper::GetShortFilePath(LPTSTR pBuff, int nBuffSize)
{
	if (!pBuff)
	{
		return;
	}

	BOOL bDirectSave = FALSE;
	LPTSTR ptrBefore, ptrEnd;
	ptrBefore = _tcschr(m_strFilePath, _T('\\'));
	if (ptrBefore)
	{
		ptrBefore = _tcschr(ptrBefore + 1, _T('\\'));
		if (!ptrBefore)
		{
			bDirectSave = TRUE;
		}
	}

	ptrEnd = _tcsrchr_e(m_strFilePath, NULL, _T('\\'));
	if (ptrEnd)
	{
		if (ptrEnd != m_strFilePath)
		{
			ptrEnd = _tcsrchr_e(m_strFilePath, ptrEnd - 1, _T('\\'));
			if (!ptrEnd)
			{
				bDirectSave = TRUE;
			}
		}
		else
		{
			bDirectSave = TRUE;
		}
	}

	if (bDirectSave || ptrEnd <= ptrBefore)
	{
		_tcscpyn(pBuff, m_strFilePath, nBuffSize);

		if (!pBuff[0])
		{
			LoadString(AfxGetInstanceHandle(), IDS_PAPER_NOPATH, pBuff, nBuffSize);
		}
		return;
	}

	_tcscpyn(pBuff, m_strFilePath, ptrBefore - m_strFilePath + 2);
	_tcscat(pBuff, _T("..."));
	_tcscat(pBuff, ptrEnd);
}

void CPaper::GetPaperInfo(CString &str)
{
	CString temp;

	str.Format(IDS_PAPER_ITEMCOUNT, GetItemCount());

	CTempList *pList = (CTempList*)GetHead();
	while(pList)
	{
		if (pList->IsUndoValid(&m_sUndoStep))
		{
			temp.Format(IDS_PAPER_TYPEINFO, pList->GetDescription(), GetItemCount(pList->GetRawType()));
			str += temp;
		}
		
		pList = (CTempList*)pList->GetNext();
		if (pList)
		{
			str += CTextManager::s_szWinReturn;
		}
	}
}

/*
���ܣ�	����������д��ض���ǵ���Ŀɾ��
����ֵ����ɾ������Ŀ��
��ע��	�����ɾ�����ɾ�����
*/
UINT CPaper::RemoveAllMarkedItems(UINT uMark)
{
	UINT uRet = 0;
	if (!m_bCanEdit)
	{
		SetItemState(ITEMMARK_NULL, ITEMMARK_DELETE);
		return uRet;
	}

	switch(uMark)
	{
	case ITEMMARK_EDIT:
		SetUndoChain(NULL, UNDO_EDIT_DEL);
		break;

	case ITEMMARK_DELETE:
		SetUndoChain(NULL, UNDO_DEL);
		break;

	default:
		ASSERT(FALSE);
	}

	uRet = m_pItemList->RemoveAllMarkedItems(uMark, m_sUndoStep.nCurStep);
	ResetTypeList(TYPE_DEFAULT);

	SaveStoreQuestions(NULL);
	m_bModify = TRUE;
	return uRet;
}

/*
���ܣ�	�������������Ŀ�趨���
����ֵ����
��ע��
*/
void CPaper::SetItemState(UINT uState, UINT uMark)
{
	m_pItemList->SetItemState(uState, uMark);
}

/*
���ܣ�	���ղ��������Ծ�����pList��
����ֵ���ɹ��򷵻�TRUE�����򷵻�FALSE
��ע��
*/
BOOL CPaper::CreateTestList(CTestPara *pPara, CList *pList)
{
	if (!pPara->GetTotalCount())
	{
		return TRUE;
	}

	CTestMaker TestMaker(this);
	return TestMaker.MakeTest(pList, pPara);
}

/*
���ܣ�	������з���pPara����Ŀ����ָ���б���
����ֵ����
��ע��	�ڷ���ʱ�����ж�pPara�е�����
		��������Ŀ��ʱ��������������Ŀ��ɢ��������ı�˳��
*/
void CPaper::FillTempList(PTYPEPARA pPara, CTempList *pList)
{
	// ����Ƿ��з������͵���Ŀ
	if (!pPara->dwTypeMask || !pList)
	{
		return;
	}

	CTempList tmpList;
	int i;
	for (i = 0; i < GetItemCount(); ++i)
	{
		CItem *pItem = GetItem(i);
		if (pItem->CheckTypeMask(pPara->dwTypeMask))
		{
			tmpList.AddItem(pItem);
		}
	}
	if (pPara->bRandom)
	{
		tmpList.Random(0, 0, time(NULL) + _getpid());
	}

	pList->ClearAllItems(TRUE);
	CTestMaker::MakeAtomList(&tmpList, pList);
}

/*
���ܣ�	�ղ���Ŀ���������ղ��б�
����ֵ����
��ע��	��pList��Ϊ�գ��������pList�е���Ŀ�ղ�
		��pListΪ�գ��򳹵��ؽ��ղ��б� 
*/
void CPaper::SaveStoreQuestions(CList *pList)
{
	if (pList)
	{
		pList->SetStore(TRUE);
	}
	m_pSaveList->ClearAllItems(TRUE);

	CItem *pItem = m_pItemList->GetHead();
	while(pItem)
	{
		if (pItem->IsStored() && pItem->IsUndoValid(&m_sUndoStep))
		{
			m_pSaveList->AddItem(pItem);
		}

		pItem = pItem->GetNext();
	}
}

/*
���ܣ�	ȡ���ղ���Ŀ���������ղ��б�
����ֵ����
��ע��	pList�ز�Ϊ��
*/
void CPaper::UnStoreQuestions(CList *pList)
{
	ASSERT(pList);
	if (!pList->GetItemCount())
	{
		return;
	}

	pList->SetStore(FALSE);
	m_pSaveList->RemoveList(pList, TRUE);
}

/*
���ܣ�	����һ���Զ�������
����ֵ�������������Զ�����Ŀ�б�
��ע��
*/
CList* CPaper::AddCustomType(int nType, LPCTSTR strDefDesc)
{
	int nTypeIndex = GetCustomTypeCount() + 1;
	if (nTypeIndex > TYPE_MAXCUSTOMCOUNT)
	{
		return Q_TRUE_RET(NULL);
	}

	nType = CItem::GetType(nType);
	CTempList *pList = new CTempList(nTypeIndex * TYPE_CUSTOMRIGHT + nType, TRUE);
	if (!pList)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}

	// ������������ˣ�Ӧ��������Ĭ�������ټ������
	// ��������Ĭ�����ƻ����һ������ĳ�����
	if (!pList->SetDescription(strDefDesc) ||
		!CList::AddItem(pList))
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}

	SetUndoChain(pList, UNDO_ADD_CUSTOM);

	m_bModify = TRUE;
	return Q_TRUE_RET(pList);
}

/*
���ܣ�	ɾ��һ���Զ�������
����ֵ����
��ע��
*/
BOOL CPaper::DelCustomType(int nType)
{
	CTempList *pList = GetTypeList(nType);
	ASSERT(!pList->IsDefaultType());

	SetUndoChain(pList, UNDO_DEL_CUSTOM);

	// �޸Ĵ��б�֮�������б���Զ������
	pList = (CTempList*)pList->GetNext();
	while(pList)
	{
		pList->SetCustomIndex(pList->GetCustomIndex() - 1);
		pList = (CTempList*)pList->GetNext();
	}

	// ɾ����Ŀ���ͺ�Ӧ������д����Ȿ����
	SaveStoreQuestions(NULL);

	// ������һ���
	m_pItemCache = NULL;
	m_nItemCache = -1;

	m_bModify = TRUE;
	return TRUE;
}

/*
���ܣ�	�༭�Զ���������Ϣ
����ֵ����
��ע��
*/
void CPaper::EditCustomType(int nUndoType)
{
	m_bModify = TRUE;
	SetUndoChain(NULL, nUndoType);
}

/*
���ܣ�	������Զ�����Ŀʱ����Ҫȷ������ͼ�б��м����ĸ����֮��
����ֵ�������б��ǰһ����Ч�б�
��ע��	�����������ڶ��Զ���������������ɾ��������
*/
CTempList* CPaper::GetCustomTypeAddAfter(CTempList* pList)
{
	CTempList *pPrev = (CTempList*)pList->GetPrev();
	while(pPrev)
	{
		if (pPrev->IsUndoValid(&m_sUndoStep))
		{
			break;
		}
		
		pPrev = (CTempList*)pPrev->GetPrev();
	}
	
	return pPrev;
}

/*
���ܣ�	�������ճ������Ŀ
����ֵ��ճ��������һ���򷵻�TRUE�����򷵻�FALSE
��ע��	ճ����Ĺ��ɺ��½���Ŀ��ȫ��ͬ
*/
BOOL CPaper::Paste(CList *pList, int nCheckType /* = TYPE_DEFAULT */)
{
	ASSERT(!pList->IsTempList());

	BOOL bRet = AddList(pList, nCheckType);
	SaveStoreQuestions(NULL);

	return bRet;
}

/*
���ܣ�	���������Ϣ
����ֵ����
��ע��	Ҳ�����ڳ����б������
*/
void FUNCCALL CPaper::ClearCoverInfo(LPPAPERCOVERINFO ppc)
{
	ASSERT(ppc);
	if (ppc->strTitle)
	{
		free(ppc->strTitle);
		ppc->strTitle = NULL;
	}
	if (ppc->strJpegPath)
	{
		free(ppc->strJpegPath);
		ppc->strJpegPath = NULL;
	}
	if (ppc->lpData)
	{
		free(ppc->lpData);
		ppc->lpData = NULL;
		ppc->ulDataSize = 0;
	}
}

/*
���ܣ�	��������÷���
����ֵ���ɹ��򷵻�TRUE�����򷵻�FALSE
��ע��	���������ppc�еĶ�ָ��ֱ�ӱ���
		ִ���걾������ppc�е������ڴ涼������ʹ����
*/
EBOOL CPaper::SetCoverInfo(LPPAPERCOVERINFO ppc)
{
	SetUndoChain(NULL, UNDO_COVER);

	ASSERT(!m_sUndoCover[m_sUndoStep.nCurStep]);
	m_sUndoCover[m_sUndoStep.nCurStep] = (LPPAPERCOVERINFO)malloc(sizeof(PAPERCOVERINFO));
	if (!m_sUndoCover[m_sUndoStep.nCurStep])
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	memcpy(m_sUndoCover[m_sUndoStep.nCurStep], ppc, sizeof(PAPERCOVERINFO));
	
	m_bModify = TRUE;
	return Q_TRUE;
}

LPCPAPERCOVERINFO CPaper::GetCoverInfo()
{
	int nStart = m_sUndoStep.nCurStep;
	while(1)
	{
		if (m_sUndoCover[nStart])
		{
			return m_sUndoCover[nStart];
		}
		
		if (nStart == m_sUndoStep.nOldestStep)
		{
			break;
		}
		
		nStart = (nStart + UNDO_SIZE - 1) % UNDO_SIZE;
	}

	return &m_sCover;
}