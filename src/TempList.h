// TempList.h: interface for the CTempList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEMPLIST_H__8F04E51F_F9C5_4F0B_8C06_7BC78BA543F2__INCLUDED_)
#define AFX_TEMPLIST_H__8F04E51F_F9C5_4F0B_8C06_7BC78BA543F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CItemPtr;

typedef struct _tagCustomInfo
{
	int nTypeCount;
	LPCTSTR szCustomDesc;
}CUSTOMINFO;

class CTempList : public CList  
{
public:
	CTempList(int nType = TYPE_DEFAULT, BOOL bCanSame = TRUE);
	virtual ~CTempList();

	virtual void SetCustomIndex(int nIndex);

	virtual inline BOOL IsTempList() { return TRUE; }

	virtual BOOL AddItem(CItem *pItem);
	virtual BOOL AddFromList(CList *pList);

	void AddStoreList(CList *pList); // 添加具有STORE标记的题目
	BOOL IsAllStored();

	BOOL RemoveList(CList *pList, BOOL bFree);
	virtual void ClearAllItems(BOOL bFree);

	virtual void SetItemState(UINT nState, UINT nMark);
	virtual void SetStore(BOOL bStore);

	virtual CItem* GetItem(int nIndex);
	virtual int GetIndex(const CItem *pItem);
	virtual CItemPtr* GetItemPtr(CItem* pItem);

	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf) { return TRUE; } // 临时列表不可复制入

	int GetSChoiseCount() { return m_nTypeCount[ITEMINDEX_SCHOISE]; }
	int GetMChoiseCount() { return m_nTypeCount[ITEMINDEX_MCHOISE]; }
	int GetJudgeCount() { return m_nTypeCount[ITEMINDEX_JUDGE]; }
	int GetBlankCount() { return m_nTypeCount[ITEMINDEX_BLANK]; }
	int GetTextCount() { return m_nTypeCount[ITEMINDEX_TEXT]; }
	int GetSingleChoiseCount();
	int GetMultiChoiseCount();
	int GetCustomCount(int nType);

	void GetListInfo(CString &str);

	virtual void SwapItems(int nItemIdx1, int nItemIdx2);
	virtual void Random(int nStart, int nEnd, int nSeed);

protected:
	BOOL m_bCanSame;

	int m_nTypeCount[TYPE_DEFAULTCOUNT];
	CUSTOMINFO m_stCustomCount[TYPE_MAXCUSTOMCOUNT];
};

#endif // !defined(AFX_TEMPLIST_H__8F04E51F_F9C5_4F0B_8C06_7BC78BA543F2__INCLUDED_)
