// Judge.h: interface for the CJudge class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_JUDGE_H__1DC7F8ED_6F8E_44E0_A9C0_766A27401821__INCLUDED_)
#define AFX_JUDGE_H__1DC7F8ED_6F8E_44E0_A9C0_766A27401821__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"

#define JUDGEANS_TRUE		TRUE
#define JUDGEANS_FALSE		FALSE
#define JUDGEANS_UNKNOWN	(-1)

class CJudge : public CItem  
{
public:
	CJudge();
	virtual ~CJudge();

	virtual LPCTSTR GetTypeDesc();

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara = 0) {}

	void SetAnswer(BOOL bAnswer) { m_bAnswer = bAnswer; }
	BOOL GetAnswer() { return (m_bAnswer); }

	virtual EBOOL LoadFile(FILE *fp, DWORD dwVersion); // 所有LoadFile函数，返回的都是非致命错误
	virtual EBOOL SaveFile(FILE *fp);
	virtual RETURN_E TiXmlElement* WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible);
	virtual EBOOL ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup); // 所有ReadXML函数，返回的都是非致命错误

	virtual BOOL SetDescription(LPCTSTR szDescription);

	virtual void ClearAllInfo();
	virtual void ClearUserInfo();

	virtual CItem* Alloc();
	virtual BOOL CopyItem(CItem *pItem);
	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf);

	virtual void MakeText(_stdstring &str, UINT nStyle, int nPara);
	virtual void MakeAnswer(_stdstring &str, UINT nStyle, int nPara = 0);
	virtual void MakeUserAnswer(_stdstring &str);
	virtual BOOL CheckItemValid();

	virtual void MakeTextOut(_stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	virtual void MakeHTML(FILE *fp, _stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);

	void SetUserAnswer(BOOL bAnswer);
	inline BOOL GetUserAnswer() { return m_bUserAnswer; }

	virtual int Score(CTestPara *pCPara);
	virtual BOOL IsAnswered() { return (m_bUserAnswer >= 0);}

	virtual void SetU32Answer(DWORD dwAnswer);

	virtual BOOL IsSameItem(CItem *pItem, int nSimilar);

private:
	BOOL m_bAnswer;
	BOOL m_bUserAnswer;

	static LPTSTR ClearEndQuote(LPCTSTR strDesc);
};

#endif // !defined(AFX_JUDGE_H__1DC7F8ED_6F8E_44E0_A9C0_766A27401821__INCLUDED_)
