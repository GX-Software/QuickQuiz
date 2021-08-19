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
	// 退出的时候保存错题本
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
功能：	获取特定题目类型的指针列表
返回值：特定题目类型的指针列表
备注：	返回的是指针列表
		指定题目类型时必须包含自定义序号
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
功能：	获取题库中某种类别的题目数
返回值：某种类别的题目数
备注：	返回值不包含已被删除但未被销毁的题目
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
功能：	获取题库中自定义类型数量
返回值：自定义类型数量
备注：	返回值不包含已被删除但未被销毁的题目
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
功能：	检查当前题目号是否有冲突
返回值：有冲突返回冲突列表，否则返回NULL
备注：	当设置为0时，为缺省值，不进行判断
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
功能：	复制特定类型的题目，并添加至pList中
返回值：成功返回TRUE，否则返回FALSE
备注：	pList中添加的都是新建后的题目，不影响本题库中原有的题目
		若bStore为TRUE，则仅复制被收藏的题目
		不论何种情况，在添加前，都会检查撤销信息
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
功能：	复制特定的自定义类型题目，并添加至pList中
返回值：成功返回TRUE，否则返回FALSE
备注：	pList中添加的都是新建后的题目，不影响本题库中原有的题目
		若bStore为TRUE，则仅复制被收藏的题目
		不论何种情况，在添加前，都会检查撤销信息
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
功能：	初始化一个题库对象
返回值：成功返回TRUE，否则返回FALSE
备注：	初始化动作如下：
		创建题目列表
		创建各类别列表（指针列表）（此时尚未创建自定义类别列表）
		创建收藏列表
		初始化题库名称、封面、撤销链
*/
EBOOL CPaper::InitPaper(BOOL bIsNewPaper)
{
	BOOL ret = TRUE;
	CTempList *pList = NULL;
	CString string;

	ASSERT(!m_nItemCount);

	// 全部题目列表
	m_pItemList = new CList();
	if (!m_pItemList)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	m_pItemList->SetParent(this);

	// 单选列表
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

	// 多选列表
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

	// 判断题列表
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

	// 填空题列表
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

	// 简答题列表
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

	// 题目组列表
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

	// 收藏题目
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
功能：	设置题库的名称
返回值：成功返回TRUE，否则返回FALSE
备注：	题库名称不可为空
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
功能：	获得题库的名称
返回值：返回题库名称字串指针
备注：
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

	// 任何一次修改密码，均需要重新保存
	m_sUndoStep.nSaveIndex = -1;
}

void CPaper::SetPasswordText(LPCSTR strPass)
{
	strcpy(m_cPassword, strPass);
}

/*
功能：	向题库中添加一个题目
返回值：成功则返回TRUE，否则返回FALSE
备注：	本函数仅限类内调用
		本函数不涉及撤销动作，不对pItem的原父列表做操作
		当pItem的来源和pItem本身不同时，可判定为编辑新增
		加入题库的题目没有来源
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

	// 在编辑时，修改后新增的题目被加在修改前删除题目的前面
	// 保证了修改后题目的顺序不发生大的变化
	if (pItem->GetItemFrom() != pItem)
	{
		m_pItemList->InsertItem(pItem, pItem->GetItemFrom(), FALSE);
	}
	else
	{
		m_pItemList->AddItem(pItem);
	}
	
	// 加入题库的题目，都没有来源
	pItem->ClearFromPtr();

	// 计算一下CRC码
	pItem->CalcCRC();

	m_bModify = TRUE;
	return TRUE;
}

/*
功能：	向题库中添加一列题目
返回值：加入至少一题则返回TRUE，否则返回FALSE
备注：	其它对象向题库新增题目，只能通过这一种方式
		向题库新增题目，均为移动方式（即不再构造题目），且不支持通过指针列表新增
		若需要判定题目类别，则仅将符合要求的题目加入，不符合者不作处理，由原列表负责销毁
*/
BOOL CPaper::AddList(CList *pList, int nCheckType /* = TYPE_DEFAULT */)
{
	BOOL bRet = FALSE;
	if (!m_bCanEdit || !pList)
	{
		return FALSE;
	}
	ASSERT(!pList->IsTempList());

	// 当添加自定义类别题目时
	// 如果当前题库中没有相同自定义序号列表，则无法加入
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

	// 在这里对全部列表项添加撤销链
	// 如果实际未加入，也无影响
	SetUndoChain(pList, UNDO_ADD);
	
	CItem *pNext = NULL;
	pItem = pList->GetHead();
	BOOL bAdd;
	while(pItem)
	{
		pNext = pItem->GetNext();
		bAdd = FALSE;

		// 当不判断类型时，不允许自定义类别加入
		if (TYPE_DEFAULT == nCheckType)
		{
			bAdd = pItem->IsDefaultType();
		}
		// 当判断类型时，基本类型相同即可以加入
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
功能：	重新生成类别列表
返回值：无
备注：	当nType为TYPE_DEFAULT时，将重新生成全部类别列表
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
功能：	获取按加入顺序的序号的题目
返回值：获取到的题目
备注：
*/
CItem* CPaper::GetItem(int nIndex)
{
	CItem *pItem;

	// 如果之前是按照题目类别来查找的
	// 则需要复原
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
功能：	获取按加入顺序序号的特定类型的题目
返回值：获取到的题目
备注：	按类型取得的题目，快取中存的实际是题目指针，需要类型转换
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
功能：	获取题库中题目总数
返回值：题库中题目的总数
备注：	获得的总数不包括被撤销的题目
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
功能：	获取题库中单个答案的选择题数量（也包括单个答案的多选题）
返回值：单个答案的选择题题目数量
备注：	获得的总数不包括被撤销的题目
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
功能：	获取题库中多个答案的选择题数量
返回值：多个答案的选择题题目数量
备注：	获得的总数不包括被撤销的题目
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
功能：	将题库中被选中的单选题转换为多选题
返回值：无
备注：
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
功能：	将题库中被选中的单个答案多选题转换为单选题
返回值：无
备注：
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
功能：	将题库中被选中的选择题转换为判断题
返回值：无
备注：	转换时会判断选项内容，且答案多于一个时不能转换
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
功能：	将题库中所有带特定标记的题目删除
返回值：被删除的题目数
备注：	这里的删除添加删除标记
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
功能：	将题库中所有题目设定标记
返回值：无
备注：
*/
void CPaper::SetItemState(UINT uState, UINT uMark)
{
	m_pItemList->SetItemState(uState, uMark);
}

/*
功能：	按照参数生成试卷并放入pList中
返回值：成功则返回TRUE，否则返回FALSE
备注：
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
功能：	将题库中符合pPara的题目放入指针列表中
返回值：无
备注：	在放入时，不判断pPara中的数量
		当放入题目组时，将把所有子题目打散，但不会改变顺序
*/
void CPaper::FillTempList(PTYPEPARA pPara, CTempList *pList)
{
	// 检查是否有符合类型的题目
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
功能：	收藏题目，并更新收藏列表
返回值：无
备注：	若pList不为空，则仅新增pList中的题目收藏
		若pList为空，则彻底重建收藏列表 
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
功能：	取消收藏题目，并更新收藏列表
返回值：无
备注：	pList必不为空
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
功能：	新增一个自定义类型
返回值：返回新增的自定义题目列表
备注：
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

	// 这里歪打正着了，应该先设置默认名称再加入题库
	// 否则设置默认名称会产生一个多余的撤销链
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
功能：	删除一个自定义类型
返回值：无
备注：
*/
BOOL CPaper::DelCustomType(int nType)
{
	CTempList *pList = GetTypeList(nType);
	ASSERT(!pList->IsDefaultType());

	SetUndoChain(pList, UNDO_DEL_CUSTOM);

	// 修改此列表之后所有列表的自定义序号
	pList = (CTempList*)pList->GetNext();
	while(pList)
	{
		pList->SetCustomIndex(pList->GetCustomIndex() - 1);
		pList = (CTempList*)pList->GetNext();
	}

	// 删除题目类型后，应当重新写入错题本内容
	SaveStoreQuestions(NULL);

	// 清理查找缓冲
	m_pItemCache = NULL;
	m_nItemCache = -1;

	m_bModify = TRUE;
	return TRUE;
}

/*
功能：	编辑自定义类型信息
返回值：无
备注：
*/
void CPaper::EditCustomType(int nUndoType)
{
	m_bModify = TRUE;
	SetUndoChain(NULL, nUndoType);
}

/*
功能：	当添加自定义题目时，需要确定在视图列表中加在哪个类别之后
返回值：新增列表的前一个有效列表
备注：	本函数适用于对自定义类别进行新增、删除、撤销
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
功能：	向题库中粘贴入题目
返回值：粘贴入至少一题则返回TRUE，否则返回FALSE
备注：	粘贴入的规律和新建题目完全相同
*/
BOOL CPaper::Paste(CList *pList, int nCheckType /* = TYPE_DEFAULT */)
{
	ASSERT(!pList->IsTempList());

	BOOL bRet = AddList(pList, nCheckType);
	SaveStoreQuestions(NULL);

	return bRet;
}

/*
功能：	清理封面信息
返回值：无
备注：	也可用于撤销列表的清理
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
功能：	给题库设置封面
返回值：成功则返回TRUE，否则返回FALSE
备注：	本函数会把ppc中的堆指针直接保存
		执行完本函数后，ppc中的所有内存都不能再使用了
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