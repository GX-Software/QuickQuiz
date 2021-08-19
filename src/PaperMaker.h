// PaperMaker.h: interface for the CPaperMaker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAPERMAKER_H__23F92902_17BE_4492_9913_61A5320A0DF4__INCLUDED_)
#define AFX_PAPERMAKER_H__23F92902_17BE_4492_9913_61A5320A0DF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPaperMaker  
{
public:
	CPaperMaker();
	BOOL PaperOut(CList *pList,
				  CTestPara *pCPara,
				  LPCTSTR szPaperPath,
				  int nPaperType,
				  int nAnswerType,
				  BOOL bWithRes,
				  LPCTSTR szAnsiFont,
				  LPCTSTR szMainFont,
				  LPCTSTR szPreFont);

protected:
	void MakePaperHead();
	void MakeHTMLTail();
	void MakeMarkTime();
	void MakeQuestionList(int nPaperType, int nListIndex, CItem *pItem, PTESTPARA pPara);
	void MakeSingle(BOOL bWithAnswer, BOOL bWithRes, int nType, PTPARAS pImagePara);
	BOOL MakeGroup(BOOL bWithAnswer, BOOL bWithRes, PTPARAS pImagePara);
	void MakeAnswerPath(CString &strPath);

	BOOL SaveText(LPCTSTR pText, FILE *fp);
	void MakeContent(BOOL bWithAnswer, BOOL bWithRes);

	BOOL MakePaper(LPCTSTR strPath);
	BOOL MakeAnswer(LPCTSTR strPath);

protected:
	FILE *m_fp;
	_stdstring m_string;

	CList *m_pList;
	CTestPara *m_pPara;
	int m_nCurItem;

	CString m_strImagePath;
	TCHAR m_szRelativeImgPath[64];

	int m_nPaperType;
	int m_nAnswerType;
	BOOL m_bWithRes;
	LPCTSTR m_szAnsiFont;
	LPCTSTR m_szMainFont;
	LPCTSTR m_szPreFont;

	int m_nListIndex;		// 大题题号
	int m_nItemIndex;		// 小题题号
};

#endif // !defined(AFX_PAPERMAKER_H__23F92902_17BE_4492_9913_61A5320A0DF4__INCLUDED_)
