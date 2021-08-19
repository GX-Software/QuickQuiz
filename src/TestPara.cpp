// TestPara.cpp: implementation of the CTestPara class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "quickquiz.h"
#include "TestPara.h"

#include "Blank.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static LPCTSTR lpszTestDlg = _T("Test Dialog");
static LPCTSTR lpszTestPara = _T("Test Parameter");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestPara::CTestPara()
{
	ZeroMemory(&m_sTestPara, sizeof(m_sTestPara));
	m_sTestPara.nSize = sizeof(_tagTestPara);
}

CTestPara::CTestPara(PTESTPARA pPara)
{
	memcpy(&m_sTestPara, pPara, sizeof(m_sTestPara));
	m_sTestPara.nSize = sizeof(_tagTestPara);
}

CTestPara::~CTestPara()
{

}

int CTestPara::GetTotalCount()
{
	int nCount = m_sTestPara.nSChCount +
				 m_sTestPara.nMChCount +
				 m_sTestPara.nJdgCount +
				 m_sTestPara.nBlkCount +
				 m_sTestPara.nTxtCount +
				 m_sTestPara.nGrpCount;

	int i;
	for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
	{
		nCount += m_sTestPara.nCustCount[i];
	}

	return nCount;
}

int CTestPara::GetTotalAtomCount(CList *pList)
{
	int i, nCount = 0;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
		case TYPE_MCHOISE:
		case TYPE_JUDGE:
		case TYPE_TEXT:
			++nCount;
			break;
			
		case TYPE_BLANK:
			nCount += ((CBlank*)pItem)->GetBlankCount();
			break;

		case TYPE_GROUP:
			nCount += ((CItemGroup*)pItem)->GetAtomCount();
			break;
			
		default:
			break;
		}
	}

	return nCount;
}

int CTestPara::GetCustomCount(int nType /* = TYPE_DEFAULT */)
{
	int i;
	int nCount = 0;
	if (nType == TYPE_DEFAULT)
	{
		for (i = 0; i < TYPE_MAXCUSTOMCOUNT; i++)
		{
			nCount += m_sTestPara.nCustCount[i];
		}
	}
	else
	{
		nType /= TYPE_CUSTOMRIGHT;
		nType = max(1, min(nType, TYPE_MAXCUSTOMCOUNT));
		nCount = m_sTestPara.nCustCount[nType - 1];
	}

	return nCount;
}

int CTestPara::GetTotalPoints(CList *pList)
{
	if (MARKTYPE_AVERAGE == m_sTestPara.nMarkType)
	{
		return m_sTestPara.nFullMark;
	}

	int nPoints = 0;
	int i;
	for (i = 0; i < pList->GetItemCount(); ++i)
	{
		CItem *pItem = pList->GetItem(i);
		switch(pItem->GetType())
		{
		case TYPE_SCHOISE:
			nPoints += m_sTestPara.nSChPoints;
			break;
			
		case TYPE_MCHOISE:
			nPoints += m_sTestPara.nMChPoints;
			break;
			
		case TYPE_JUDGE:
			nPoints += m_sTestPara.nJdgPoints;
			break;
			
		case TYPE_BLANK:
			nPoints += ((CBlank*)pItem)->GetBlankCount() * m_sTestPara.nBlkPoints;
			break;
			
		case TYPE_TEXT:
			nPoints += m_sTestPara.nTxtPoints;
			break;

		case TYPE_GROUP:
			nPoints += ((CItemGroup*)pItem)->GetFullMark(this);
			break;
		}
	}

	return nPoints;
}

BOOL CTestPara::LoadTestPara()
{
	LPBYTE ptr = NULL;
	UINT n = 0;
	if (AfxGetApp()->GetProfileBinary(lpszTestDlg, lpszTestPara,
		&ptr, &n))
	{
		PTESTPARA pPara = (PTESTPARA)ptr;
		if (pPara->nSize == sizeof(_tagTestPara))
		{
			memcpy(&m_sTestPara, ptr, sizeof(_tagTestPara));
			delete[] ptr;

			return TRUE;
		}
		else
		{
			delete[] ptr;
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

void CTestPara::SaveTestPara()
{
	CPaper *pPaper = m_sTestPara.pPaper;
	m_sTestPara.pPaper = NULL;
	
	AfxGetApp()->WriteProfileBinary(lpszTestDlg, lpszTestPara,
		(LPBYTE)&m_sTestPara, sizeof(_tagTestPara));
	
	m_sTestPara.pPaper = pPaper;
}