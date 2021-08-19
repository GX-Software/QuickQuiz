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
	CItem** m_GrpCustList;		// 保存全部题目组和自定义类型列表的指针
								// 用于按题号对题目进行排序

	BOOL FillGrpCustList();
	BOOL MakeChoiseList(CList *pList);
	BOOL MakeSingleList(CList *pList, int nType);
	BOOL MakeQNumList(CList *pList, BOOL bQNumOrder);
};

#endif // !defined(AFX_TESTMAKER_H__B69B750C_CFA9_4213_B2AF_293E13C7922E__INCLUDED_)
