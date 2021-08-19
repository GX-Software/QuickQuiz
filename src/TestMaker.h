// TestMaker.h: interface for the CTestMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTMAKER_H__B69B750C_CFA9_4213_B2AF_293E13C7922E__INCLUDED_)
#define AFX_TESTMAKER_H__B69B750C_CFA9_4213_B2AF_293E13C7922E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTestMaker  
{
public:
	CTestMaker(CPaper *pPaper);
	virtual ~CTestMaker();

	BOOL MakeTest(CList *pList, CTestPara *pCPara);

	static BOOL FUNCCALL MakeAtomList(CList *pIn, CTempList *pOut);

protected:
	CPaper *m_pPaper;
	CTestPara *m_pCPara;

	int m_nGrpCustCount;
	CItem** m_GrpCustList;		// ����ȫ����Ŀ����Զ��������б��ָ��
								// ���ڰ���Ŷ���Ŀ��������

	BOOL FillGrpCustList();
	BOOL MakeChoiseList(CList *pList);
	BOOL MakeSingleList(CList *pList, int nType);
	BOOL MakeQNumList(CList *pList, BOOL bQNumOrder);
};

#endif // !defined(AFX_TESTMAKER_H__B69B750C_CFA9_4213_B2AF_293E13C7922E__INCLUDED_)
