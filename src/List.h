// List.h: interface for the CList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIST_H__2963065A_B679_433A_AF18_93EE336FD57C__INCLUDED_)
#define AFX_LIST_H__2963065A_B679_433A_AF18_93EE336FD57C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"
class CTempList;

enum UndoType
{
	LISTUNDO_NONE = 0,
	LISTUNDO_SETDESC,		// 设置类型描述
	LISTUNDO_SETQNUMBER		// 设置题号
};

typedef struct _tagListUndoInfo
{
	int nType;
	DWORD dwData;
}LISTUNDOINFO, *PLISTUNDOINFO;

class CList : public CItem
{
public:
	friend class CPaper;

	CList(int nQuestionType = TYPE_DEFAULT);
	virtual ~CList();

	void SetItemLimit(size_t uLimit) { m_uItemLimit = uLimit; }
	virtual void SetCustomIndex(int nIndex);
	virtual BOOL CheckTypeMask(DWORD dwMask) { ASSERT(FALSE); return FALSE; }

	virtual inline BOOL IsList() { return TRUE; }
	virtual inline BOOL IsPaper() { return FALSE; }
	virtual inline BOOL IsTempList() { return FALSE; }

	int GetQNumber();
	void SetQNumber(int nQNumber);

	virtual BOOL SetDescription(LPCTSTR szDescription);
	virtual LPCTSTR GetDescription();
	
	int GetFirstInvalidItem();
	void ClearInvalidItems();
	CItem* GetFirstUnAnswerd();
	
	virtual BOOL AddItem(CItem *pItem);
	virtual BOOL AddFromList(CList *pList);
	virtual int GetItemCount() { return m_nItemCount; }
	BOOL MoveFromList(CList *pList);

	BOOL RemoveItem(CItem *pItem, BOOL bFree);
	CItem* ReplaceItem(CItem *pItemDst, CItem *pItemSrc);
	virtual void SwapItems(int nItemIdx1, int nItemIdx2);
	BOOL InsertItem(CItem *pIns, CItem *pDes, BOOL bAfter);
	virtual void ClearAllItems(BOOL bFree);
	virtual void ClearUserInfo();

	virtual CItem* GetItem(int nIndex);
	virtual int GetIndex(const CItem *pItem);

	inline void SetGUIHandle(HTREEITEM hTreeItem) { m_hTreeItem = hTreeItem; }
	inline HTREEITEM GetGUIHandle() { return m_hTreeItem; }

	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf);

	UINT RemoveAllMarkedItems(UINT uMark, int nUndoStep);
	virtual void SetItemState(UINT nState, UINT nMark);
	virtual void SetStore(BOOL bStore) { CList::SetItemState((bStore ? ITEMMARK_STORE : ITEMMARK_NULL), ITEMMARK_STORE); }

	virtual void Random(int nStart, int nEnd, int nSeed);

	// 仅应对题库类的调用
	void UndoClear(LPCUNDOSTEP pStep);
	void SetUndoStep(int nUndoStep, int nUndoType);

protected:
	inline CItem* GetHead() { return (m_pHead); }
	inline CItem* GetTail() { return (m_pTail); }

	BOOL CanAddIn(CItem *pItem);

	CItem *m_pHead;
	CItem *m_pTail;
	int m_nItemCount;
	size_t m_uItemLimit; // 当此值为0时，不限制数量
	
	// 加快寻找速度
	int m_nLastItemGet;
	CItem *m_pLastItemGet;
	
	HTREEITEM m_hTreeItem;
	
	// 和撤销有关的内容
	// 仅题库的子列表才能改名，故以后均只判断父列表
	LISTUNDOINFO m_UndoInfo[UNDO_SIZE];

	int m_nQNumber; // 题号
};

#endif // !defined(AFX_LIST_H__2963065A_B679_433A_AF18_93EE336FD57C__INCLUDED_)
