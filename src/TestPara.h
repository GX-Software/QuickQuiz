// TestPara.h: interface for the CTestPara class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTPARA_H__76A356DE_FD99_499D_A1DF_7BCDCD3E809A__INCLUDED_)
#define AFX_TESTPARA_H__76A356DE_FD99_499D_A1DF_7BCDCD3E809A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CList;
class CPaper;

// 考试参数
typedef struct _tagTestPara
{
	int nSize;
	CPaper *pPaper;
	BOOL bUseStore;
	BOOL bQNumOrder;
	
	int nSChCount;
	int nMChCount;
	int nJdgCount;
	int nBlkCount;
	int nTxtCount;
	int nGrpCount;
	int nCustCount[TYPE_MAXCUSTOMCOUNT];
	
	int nTime;			// 单位为分钟
	int nFullMark;
	int nMarkType;
	int nSChPoints;
	int nMChPoints;
	int nJdgPoints;
	int nBlkPoints;
	int nTxtPoints;
	int nCustPoints[TYPE_MAXCUSTOMCOUNT];

	float fScoreH;		// 百分制的总分
	float fScoreR;		// 实际的总分
	
	DWORD dwChsOptions;
	DWORD dwJdgOptions;
	DWORD dwBlkOptions;
	DWORD dwTxtOptions;
}TESTPARA, *PTESTPARA;

class CTestPara  
{
public:
	CTestPara();
	CTestPara(PTESTPARA pPara);
	virtual ~CTestPara();

	inline PTESTPARA GetTestPara() { return &m_sTestPara; }

	int GetTotalCount();
	int GetTotalAtomCount(CList *pList);
	int GetCustomCount(int nType = TYPE_DEFAULT);
	int GetTotalPoints(CList *pList);

	BOOL LoadTestPara();
	void SaveTestPara();

protected:
	TESTPARA m_sTestPara;
};

#endif // !defined(AFX_TESTPARA_H__76A356DE_FD99_499D_A1DF_7BCDCD3E809A__INCLUDED_)
