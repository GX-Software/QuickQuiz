// TextSimilarity.h: interface for the CTextSimilarity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTSIMILARITY_H__C0473336_4885_4626_A4CD_1B37AF52DCFF__INCLUDED_)
#define AFX_TEXTSIMILARITY_H__C0473336_4885_4626_A4CD_1B37AF52DCFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTextSimilarity  
{
public:
	CTextSimilarity();
	virtual ~CTextSimilarity();

	BOOL Compare(LPCTSTR szStr1, LPCTSTR szStr2);
	void ClearInfo() { m_nNum = m_nDenom = 0; }
	float GetInfo() { return (1 - (float)m_nNum / m_nDenom); }

protected:
	BOOL CheckBuffSize(int nNewSize);
	int Min3(int a, int b, int c) { return (min(min(a, b), c)); }

	int *m_pRight;
	int m_nSize;

	int m_nNum;
	int m_nDenom;
};

#endif // !defined(AFX_TEXTSIMILARITY_H__C0473336_4885_4626_A4CD_1B37AF52DCFF__INCLUDED_)
