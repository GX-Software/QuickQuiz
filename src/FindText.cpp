// Find.cpp: implementation of the CFindText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FindText.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define F_UPWORD(x, mtcs) ((x >= _T('a') && x <= _T('z') && mtcs == 0) ? (x & 0xDF) : x)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFindText::CFindText() :
	m_pPatt(NULL), m_nPattLen(0),
	m_bDown(TRUE), m_bMachCase(TRUE)
{
	int i ;
	for (i = 0 ; i < 256 ; i++)
	{
		m_nAlphabet[i] = 1;
	}
}

CFindText::~CFindText()
{
	if (m_pPatt)
	{
		free(m_pPatt);
	}
}

BOOL CFindText::SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase)
{
#ifdef _UNICODE
	if (bMachCase)
	{
		return Sunday_SetPara(pPatt, nLen, bDown, bMachCase);
	}
	else
	{
		return Default_SetPara(pPatt, nLen, bDown, bMachCase);
	}
#else
	return Sunday_SetPara(pPatt, nLen, bDown, bMachCase);
#endif
}

BOOL CFindText::FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd)
{
#ifdef _UNICODE
	if (m_bMachCase)
	{
		return Sunday_FindNext(pSource, nStart, nOutStart, nOutEnd);
	}
	else
	{
		return Default_FindNext(pSource, nStart, nOutStart, nOutEnd);
	}
#else
	return Sunday_FindNext(pSource, nStart, nOutStart, nOutEnd);
#endif
}

BOOL CFindText::Default_SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase)
{
	if (!pPatt)
	{
		return FALSE;
	}
	
	if (m_pPatt)
	{
		free(m_pPatt);
		m_pPatt = NULL;
	}
	m_pPatt = _tcsdup(pPatt);
	if (!m_pPatt)
	{
		return FALSE;
	}
	
	if (nLen <= 0)
	{
		m_nPattLen = lstrlen(m_pPatt) * sizeof(TCHAR);
	}
	else
	{
		m_nPattLen = nLen * sizeof(TCHAR);
	}
	
	if (!m_nPattLen)
	{
		return FALSE;
	}

	m_bDown = bDown;
	m_bMachCase = bMachCase;
	return TRUE;
}

BOOL CFindText::Default_FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd)
{
#ifdef _UNICODE
	ASSERT(!m_bMachCase);

	if (!m_pPatt || !m_nPattLen)
	{
		return FALSE;
	}

	if (nStart < 0 || nStart >= lstrlen(pSource))
	{
		return FALSE;
	}

	LPTSTR ptr = NULL;
	if (m_bDown)
	{
		ptr = _tcsstri(pSource + nStart, m_pPatt);
		
	}
	else
	{
		ptr = StrStrIR(pSource, nStart, m_pPatt);
	}

	if (!ptr)
	{
		return FALSE;
	}
	nOutStart = ptr - pSource;
	nOutEnd = nOutStart + m_nPattLen / sizeof(TCHAR);
#else
	ASSERT(FALSE);
#endif

	return TRUE;
}

LPTSTR CFindText::StrStrIR(LPCTSTR lpFirst, int nStart, LPCTSTR lpSrch)
{
#ifdef _UNICODE
	int nPattLenReal = m_nPattLen / sizeof(TCHAR);
	int nCheck = nStart, i = nStart, j = nPattLenReal - 1;
	while(nCheck >= 0)
	{
		if (F_UPWORD(lpFirst[i], FALSE) == F_UPWORD(lpSrch[j], FALSE))
		{
			i--;
			j--;
		}
		else
		{
			nCheck--;
			i = nCheck;
			j = nPattLenReal - 1;
		}

		if (j < 0)
		{
			return (LPTSTR)lpFirst + i + 1;
		}
	}
#else
	ASSERT(FALSE);
#endif

	return NULL;
}

BOOL CFindText::Sunday_SetPara(LPCTSTR pPatt, int nLen, BOOL bDown, BOOL bMachCase)
{
	if (!Default_SetPara(pPatt, nLen, bDown, bMachCase))
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < 256; i++)
	{
		m_nAlphabet[i] = m_nPattLen + 1;
	}

	BYTE bt = 0;
	LPBYTE ptr = (LPBYTE)m_pPatt;
	for (i = 0; i < m_nPattLen; i++)
	{
		bt = ptr[i];

		m_nAlphabet[bt] = bDown ? (m_nPattLen - i) : (i + 1);

		// 大小写不敏感
		if (!bMachCase)
		{
#ifdef _UNICODE
			// UNICODE大小写不敏感不采用Sunday算法
			ASSERT(FALSE);
#else
			if (bt >= 'A' && bt <= 'Z')
			{
				m_nAlphabet[bt + 32] = m_nAlphabet[bt];
			}
			else if (bt >= 'a' && bt <= 'z')
			{
				m_nAlphabet[bt - 32] = m_nAlphabet[bt];
			}
#endif
		}
	}

	return TRUE;
}

BOOL CFindText::Sunday_FindNext(LPCTSTR pSource, int nStart, int &nOutStart, int &nOutEnd)
{
	if (!m_pPatt || !m_nPattLen)
	{
		return FALSE;
	}

	int nSrcLen = lstrlen(pSource) * sizeof(TCHAR);
	if (!nSrcLen)
	{
		return FALSE;
	}
	nStart *= sizeof(TCHAR);

	int nCheck, nMach;
	WORD sour, pat;

	if (m_bDown)
	{
		LPBYTE ptr = (LPBYTE)pSource;
		LPBYTE patt = (LPBYTE)m_pPatt;

		int nLimit = nSrcLen - m_nPattLen + 1;
		if (nStart >= nLimit || nStart < 0)
		{
			return FALSE;
		}
		
		int i = nStart;
		while(i < nLimit)
		{
			if (F_UPWORD(ptr[i], m_bMachCase) == F_UPWORD(*patt, m_bMachCase))
			{
				nCheck = i + 1;
				nMach = 1;
				do
				{
					if (nMach == m_nPattLen)
					{
						nOutStart = i / sizeof(TCHAR);
						nOutEnd = nOutStart + (m_nPattLen / sizeof(TCHAR));
						return TRUE;
					}
					
					sour = ptr[nCheck++];
					pat = patt[nMach++];
				} while(F_UPWORD(sour, m_bMachCase) == F_UPWORD(pat, m_bMachCase));
			}
			
			i += m_nAlphabet[ptr[i + m_nPattLen]];
		}
	}
	else
	{
		LPBYTE ptr = (LPBYTE)pSource;
		LPBYTE patt = (LPBYTE)m_pPatt;

		int nLimit = m_nPattLen - 1;
		if (nStart < nLimit || nStart > nSrcLen)
		{
			return FALSE;
		}

		int i = nStart;
		while(i >= nLimit)
		{
			if (F_UPWORD(ptr[i], m_bMachCase) == F_UPWORD((BYTE)patt[m_nPattLen - 1], m_bMachCase))
			{
				nCheck = i - 1;
				nMach = m_nPattLen - 2;
				do 
				{
					if (nMach <= 0)
					{
						nOutStart = (i - m_nPattLen + 1) / sizeof(TCHAR);
						nOutEnd = nOutStart + (m_nPattLen / sizeof(TCHAR));
						return TRUE;
					}

					sour = ptr[nCheck--];
					pat = patt[nMach--];
				} while (F_UPWORD(sour, m_bMachCase) == F_UPWORD(pat, m_bMachCase));
			}

			i -= m_nAlphabet[ptr[i - m_nPattLen]];
		}
	}

	return FALSE;
}
