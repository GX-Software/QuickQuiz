// Blank.h: interface for the CBlank class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLANK_H__761BE267_1F78_40B3_BA4E_A3FEFF1A8EF6__INCLUDED_)
#define AFX_BLANK_H__761BE267_1F78_40B3_BA4E_A3FEFF1A8EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define ERROR_OK				0
#define ERROR_REACHMAX			1
#define ERROR_NODESCRIPTION		2
#define ERROR_RANGECONFLICT		3
#define ERROR_ALLOCFAIL			4
#define ERROR_NOANSWER			5
#define ERROR_INDEXOVERFLOW		6
#define ERROR_TRANSCODEFAIL		7

#define BLANKPARA_NORMAL		0
#define BLANKPARA_ONLYBLANK		1
#define BLANKPARA_RANDOMBLANK	2

#define MAXBLANKS				32

typedef struct _tagBlank
{
	int nStart;			// 空白起始位置
	int nEnd;			// 空白结束位置
	LPTSTR pBlank;		// 空白内容
	BOOL bCheck;		// 用户答案是否正确
}BLANK, *PBLANK;

class CBlank : public CItem  
{
public:
	CBlank();
	virtual ~CBlank();

	virtual LPCTSTR GetTypeDesc();
	inline DWORD GetLastError() { return m_dwErrorCode; }

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara = 0);

	int AddBlank(int nStart, int nEnd, LPCTSTR strBlank = NULL);
	BOOL EditBlank(int nIndex, LPCTSTR strBlank, int nLen);
#ifndef _UNICODE
	BOOL EditBlankW(int nIndex, LPCWSTR strBlank, int nLen);
#endif
	BOOL DelBlank(int nIndex, BOOL bFillAnswer);

	void SetBlankPara(int nPara = BLANKPARA_NORMAL);
	LPCTSTR GetBlank(int nIndex);
	inline int GetBlankCount() { return (m_pValidBlanks ? m_nValidBlanksCount : m_nBlanksCount); }
	void GetBlankRange(int nIndex, int *nStart, int *nEnd);
	int IsBlank(int nStart, int nEnd);

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
	BOOL GetInfo(CBlank *pBlank, BOOL bWithDescription);

	virtual void MakeText(_stdstring &str, UINT nStyle, int nPara);
	virtual void MakeAnswer(_stdstring &str, UINT nStyle, int nPara = 0);
	void CheckSimilarity(CTextSimilarity *pSim);
	virtual BOOL CheckItemValid();

	virtual void MakeTextOut(_stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	virtual void MakeHTML(FILE *fp, _stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);

	BOOL SetUserAnswer(int nIndex, LPCTSTR strAnswer);
	LPCTSTR GetUserAnswer(int nIndex);

	virtual int Score(CTestPara *pCPara);
	int AutoScore(CTestPara *pCPara);
	virtual int CheckAnswer(int *pnRightCount);
	virtual BOOL IsAnswered() { return (m_nUserAnswerCount == GetBlankCount()); }
	int GetUserAnswerCount() { return m_nUserAnswerCount; }

	void SetUserAnswerCheck(int nIndex, BOOL bCorrect);
	BOOL GetUserAnswerCheck(int nIndex);

	virtual BOOL Match(LPCTSTR strMatch, BOOL bMatchCase);

	virtual void CalcCRC();

	virtual BOOL IsSameItem(CItem *pItem, int nSimilar);

	static int FUNCCALL CheckBlksCount(CItem *pItem, BOOL bInterupt);

protected:
	PBLANK GetRealBlanks() { return (m_pValidBlanks ? m_pValidBlanks : m_pBlanks); }
	BOOL CheckRanges(int nStart, int nEnd);

	LPWSTR GetWDescription(int &nBuffSize);
	LPTSTR GetTDescription(LPWSTR wstrDescription);
	LPTSTR GetBlankText(LPWSTR wstrDescription, int nStart, int nEnd);
	LPTSTR ClearSpaces(LPTSTR strText);
	int MakeQuote(LPWSTR wstrDescription, int &nStart, int &nEnd);
	int InsertPos(int nStart, int nEnd, int nOffset);

	void FillAnswer(_stdstring &str, BOOL bIsRealAsw, int nCount);
	void ClearAllBlanks();

	BOOL SetBlankListSize(int nSize);
	BOOL SetUserAnswerListSize();

protected:
	int m_nBlanksCount;

	PBLANK m_pValidBlanks;
	int m_nValidBlanksCount;

	PBLANK m_pBlanks;
	int m_nBlankListSize;

	int m_nUserAnswerListSize;
	int m_nUserAnswerCount;
	LPTSTR *m_pUserAnswers;

	DWORD m_dwErrorCode;
};

#endif // !defined(AFX_BLANK_H__761BE267_1F78_40B3_BA4E_A3FEFF1A8EF6__INCLUDED_)
