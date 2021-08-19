// ItemGroup.h: interface for the CItemGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMGROUP_H__6F58A2A0_0B86_4C37_8537_502B54D41425__INCLUDED_)
#define AFX_ITEMGROUP_H__6F58A2A0_0B86_4C37_8537_502B54D41425__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_ITEMGROUP_NUM		200

class CItemGroup : public CList  
{
public:
	CItemGroup();
	virtual ~CItemGroup();

	virtual inline BOOL IsList() { return FALSE; }

	virtual LPCTSTR GetTypeDesc();
	void SetTypeDesc(LPCTSTR szTypeDesc);
	int GetTypeCount(int nType, BOOL bStore);

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara = 0);

	virtual void SetItemState(UINT nState, UINT nMark) { CItem::SetItemState(nState, nMark); }
	virtual UINT GetItemState(UINT nMark) { return CItem::GetItemState(nMark); }
	virtual void SetStore(BOOL bStore);
	virtual BOOL IsStored();
	virtual BOOL LoadStore(FILE *fp);
	virtual BOOL SaveStore(FILE *fp);

	virtual EBOOL LoadFile(FILE *fp, DWORD dwVersion); // 所有LoadFile函数，返回的都是非致命错误
	virtual EBOOL SaveFile(FILE *fp);
	virtual RETURN_E TiXmlElement* WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible);
	virtual EBOOL ReadXML(TiXmlElement *pGrp, int nEncode, BOOL bGroup); // 所有ReadXML函数，返回的都是非致命错误

	virtual EBOOL AddItem(CItem *pItem);
	void ClearSubItemFrom();

	virtual void ClearAllInfo();
	virtual void ClearUserInfo();

	virtual CItem* Alloc();
	virtual BOOL CopyItem(CItem *pItem);
	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf);

	virtual void MakeText(_stdstring &str, UINT nStyle, int nPara);
	virtual void MakeAnswer(_stdstring &str, UINT nStyle, int nPara = 0);
	inline void SetStartIndex(int nIndex) { m_nStartIndex = nIndex; }
	virtual BOOL CheckItemValid();

	int GetFullMark(CTestPara *pCPara);
	int GetAtomCount();
	virtual int Score(CTestPara *pCPara);

	virtual void MakeTextOut(_stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	virtual void MakeHTML(FILE *fp, _stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);

	BOOL PointSubItem(CTempList *pList);

	PVOID GetUserAnswer(int nItem, int nIndex);
	int CheckAnswer(int nItem, int *nRightCount = NULL);
	BOOL IsAnswered(int nItem);

	virtual LPCTSTR GetResolve();

	virtual BOOL Match(LPCTSTR strMatch, BOOL bMatchCase);
	
	virtual void CalcCRC();

private:
	static void ReplaceHTMLEndLine(_stdstring &str);

	int m_nStartIndex;

	_stdstring m_strGResolve;
};

#endif // !defined(AFX_ITEMGROUP_H__6F58A2A0_0B86_4C37_8537_502B54D41425__INCLUDED_)
