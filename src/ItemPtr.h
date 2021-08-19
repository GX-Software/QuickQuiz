// ItemPtr.h: interface for the CItemPtr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMPTR_H__D281B8A9_DAA7_4D5E_80EB_C9C2D1B290BD__INCLUDED_)
#define AFX_ITEMPTR_H__D281B8A9_DAA7_4D5E_80EB_C9C2D1B290BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CItemPtr : public CItem  
{
public:
	CItemPtr() : m_pItem(NULL) {}
	virtual ~CItemPtr() {}

	virtual BOOL IsItemPtr() { return TRUE; }

	virtual void SetType(int nType)
	{
		m_nType = nType;
	}
	virtual void SetItemState(UINT nState, UINT nMark)
	{
		if (m_pItem)
		{
			m_pItem->SetItemState(nState, nMark);
		}
	}
	virtual UINT GetItemState(UINT nMark)
	{
		if (m_pItem)
		{
			return m_pItem->GetItemState(nMark);
		}

		return ITEMMARK_NULL;
	}
	virtual CItem* GetItemFrom()
	{
		return m_pItem->GetItemFrom();
	}

	CItem *m_pItem;
};

#endif // !defined(AFX_ITEMPTR_H__D281B8A9_DAA7_4D5E_80EB_C9C2D1B290BD__INCLUDED_)
