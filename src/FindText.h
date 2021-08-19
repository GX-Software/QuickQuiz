// Find.h: interface for the FindWhat class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FIND_H__41C554B8_0F00_4735_A3C3_BCF419AFCE77__INCLUDED_)
#define AFX_FIND_H__41C554B8_0F00_4735_A3C3_BCF419AFCE77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFindText
{
public:
	CFindText();
	virtual ~CFindText();

	BOOL SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase);
	BOOL FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd);

protected:
	BOOL Default_SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase);
	BOOL Default_FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd);
	LPTSTR StrStrIR(LPCTSTR lpFirst, int nStart, LPCTSTR lpSrch);

	BOOL Sunday_SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase);
	BOOL Sunday_FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd);
	
protected:
	BOOL m_bDown;
	BOOL m_bMachCase;

	LPTSTR m_pPatt;
	int m_nPattLen;
	int m_nAlphabet[256];
};

#endif // !defined(AFX_FIND_H__41C554B8_0F00_4735_A3C3_BCF419AFCE77__INCLUDED_)