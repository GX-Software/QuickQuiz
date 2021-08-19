#if !defined(AFX_CHOISE_H__D4BE52F7_2C6D_4F42_BA37_8E3862F61827__INCLUDED_)
#define AFX_CHOISE_H__D4BE52F7_2C6D_4F42_BA37_8E3862F61827__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Choise.h : header file
//

#define MAXOPTIONNUM	8

typedef struct _tagChoiseOption
{
	LPTSTR option;
	BOOL answer;
}CHOISEOPTION;

/////////////////////////////////////////////////////////////////////////////
// CChoise window

class CChoise : public CItem
{
// Construction
public:
	CChoise(BOOL bIsSingle);
	virtual ~CChoise();

public:
	virtual void SetType(int nType);
	virtual LPCTSTR GetTypeDesc();
	CItem* MakeJudge();
	
	BOOL AddOption(LPCTSTR szOption, BOOL bIsTrue);
	BOOL DeleteOption(int nIndex);
	BOOL AutoOptions(LPCWSTR pText, int nStrLen);

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara);

	BOOL GetAnswer(int nIndex);
	inline int GetAnswerCount() { return (m_nAnswerCount); }
	LPCTSTR GetOption(int nIndex);
	inline int GetOptionCount() { return (m_nOptionCount); }

	virtual EBOOL LoadFile(FILE *fp, DWORD dwVersion); // 所有LoadFile函数，返回的都是非致命错误
	virtual EBOOL SaveFile(FILE *fp);
	virtual RETURN_E TiXmlElement* WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible);
	virtual EBOOL ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup); // 所有ReadXML函数，返回的都是非致命错误

	void ClearAllOptions();
	virtual void ClearAllInfo();
	virtual void ClearUserInfo();

	virtual CItem* Alloc();
	virtual BOOL CopyItem(CItem *pItem);
	BOOL CopyChoiseIgnoreType(CItem *pItem);
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

	void SetUserAnswer(int nIndex);
	BOOL GetUserAnswer(int nIndex);

	virtual int Score(CTestPara *pCPara);
	virtual BOOL IsAnswered() { return m_btUserAnswer; }

	virtual BOOL Match(LPCTSTR strMatch, BOOL bMatchCase);

	virtual void SetU32Answer(DWORD dwAnswer);

	virtual void CalcCRC();

	virtual BOOL IsSameItem(CItem *pItem, int nSimilar);

private:
	CHOISEOPTION m_Option[MAXOPTIONNUM];
	int m_nOptionCount;
	int m_nAnswerCount;

	BYTE m_btUserAnswer;

private:
#ifndef _UNICODE
	BOOL AddWOption(LPCWSTR szOption, BOOL bIsTrue);
#endif
	void RandomOptions(DWORD seed = NULL);
	BOOL ModifyOption(int nIndex, LPCTSTR szOption, BOOL bIsTrue);
	BYTE MakeByteAnswer();
	int CheckOptionWidth(int nWidth, int *ColWidth);
	void MakeOptionText(_stdstring &str, UINT nStyle, int nPara);
	BOOL FindQuotePos(int &nStart, int &nEnd);
};

#endif // !defined(AFX_CHOISE_H__D4BE52F7_2C6D_4F42_BA37_8E3862F61827__INCLUDED_)
