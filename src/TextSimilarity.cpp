// TextSimilarity.cpp: implementation of the CTextSimilarity class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TextSimilarity.h"

#define SIZEGROWSTEP		64

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTextSimilarity::CTextSimilarity() :
	m_pRight(NULL), m_nSize(0),
	m_nNum(0), m_nDenom(0)
{

}

CTextSimilarity::~CTextSimilarity()
{
	if (m_pRight)
	{
		free(m_pRight);
	}
}

BOOL CTextSimilarity::Compare(LPCTSTR szStr1, LPCTSTR szStr2)
{
	int nLen1 = lstrlen(szStr1 ? szStr1 : _T(""));
	int nLen2 = lstrlen(szStr2 ? szStr2 : _T(""));

	m_nDenom += max(nLen1, nLen2);
	
	if (nLen1 && nLen2)
	{
		if (!CheckBuffSize(max(nLen1 + 1, nLen2 + 1)))
		{
			return FALSE;
		}
		
		int i, j, nTemp;
		for (i = 1; i <= nLen1; i++)
		{
			for (j = 1; j <= nLen2; j++)
			{
				if (szStr1[i - 1] == szStr2[j - 1])
				{
					nTemp = 0;
				}
				else
				{
					nTemp = 1;
				}
				
				m_pRight[i + j * m_nSize] = Min3(m_pRight[(i - 1) + m_nSize * (j - 1)] + nTemp,
												 m_pRight[i + m_nSize * (j - 1)] + 1,
												 m_pRight[(i - 1) + m_nSize * j] + 1);
			}
		}

		m_nNum += m_pRight[nLen1 + m_nSize * nLen2];
	}
	else
	{
		m_nNum += max(nLen1, nLen2);
	}
	
	return TRUE;
}

BOOL CTextSimilarity::CheckBuffSize(int nNewSize)
{
	BOOL bNew = FALSE;

	while(m_nSize < nNewSize)
	{
		m_nSize += SIZEGROWSTEP;
		bNew = TRUE;
	}
	if (!bNew)
	{
		return TRUE;
	}

	int *pNew = (int*)realloc(m_pRight, sizeof(int) * m_nSize * m_nSize);
	if (pNew)
	{
		m_pRight = pNew;
	}
	else
	{
		return FALSE;
	}

	// ¸³³õÖµ
	int i;
	for (i = 0; i < m_nSize; i++)
	{
		m_pRight[i] = i;
		m_pRight[m_nSize * i] = i;
	}

	return TRUE;
}