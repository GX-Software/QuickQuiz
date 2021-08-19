// Text.h: interface for the CText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXT_H__ED1C11C1_B001_4844_ADAE_59FFC58E4D09__INCLUDED_)
#define AFX_TEXT_H__ED1C11C1_B001_4844_ADAE_59FFC58E4D09__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CText : public CItem  
{
public:
	CText();
	virtual ~CText();

	virtual LPCTSTR GetTypeDesc();

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara = 0) {}

	BOOL SetAnswer(LPCTSTR strAnswer, int nLen);
#ifndef _UNICODE
	BOOL SetAnswerW(LPCWSTR strAnswer, int nLen);
#endif

	inline LPCTSTR GetAnswer() { return m_strAnswer; }
	int GetAnswerParagraphs();

	virtual EBOOL LoadFile(FILE *fp, DWORD dwVersion); // ����LoadFile���������صĶ��Ƿ���������
	virtual EBOOL SaveFile(FILE *fp);
	virtual RETURN_E TiXmlElement* WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible);
	virtual EBOOL ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup); // ����ReadXML���������صĶ��Ƿ���������

	virtual void ClearAllInfo();
	virtual void ClearUserInfo();

	virtual CItem* Alloc();
	virtual BOOL CopyItem(CItem *pItem);
	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf);

	virtual void MakeText(_stdstring &str, UINT nStyle, int nPara);
	virtual void MakeAnswer(_stdstring &str, UINT nStyle, int nPara);
	void CheckSimilarity(CTextSimilarity *pSim);
	virtual BOOL CheckItemValid() { return (m_strAnswer ? TRUE : FALSE); }

	virtual void MakeTextOut(_stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	virtual void MakeHTML(FILE *fp, _stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);

	void SetUserAnswer(LPCTSTR strUserAnswer);
	LPTSTR GetUserAnswer();

	virtual int Score(CTestPara *pCPara);
	int AutoScore(CTestPara *pCPara);
	virtual BOOL IsAnswered();

	void SetPoint(int nPoint) { m_nUserScore = nPoint; }
	int GetPoint() { return m_nUserScore; }

	virtual BOOL Match(LPCTSTR strMatch, BOOL bMatchCase);

	virtual void CalcCRC();

	virtual BOOL IsSameItem(CItem *pItem, int nSimilar);

protected:
	void ClearAnswer();

	int GetAnswerLineCount();

protected:
	LPTSTR m_strAnswer;
	LPTSTR m_strUserAnswer;

	int m_nUserScore;			// �û��趨�ķ���
	int m_nMaxScore;			// ���������
};

#endif // !defined(AFX_TEXT_H__ED1C11C1_B001_4844_ADAE_59FFC58E4D09__INCLUDED_)
