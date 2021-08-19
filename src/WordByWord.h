#if !defined(AFX_WORDBYWORD_H__71C23541_FA74_47BF_96E1_98DE16A1CBD6__INCLUDED_)
#define AFX_WORDBYWORD_H__71C23541_FA74_47BF_96E1_98DE16A1CBD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WordByWord.h : header file
//

#define WWN_SHOWEND		0

/////////////////////////////////////////////////////////////////////////////
// CWordByWord window

class CWordByWord : public CWnd
{
// Construction
public:
	CWordByWord();

// Attributes
public:

// Operations
public:
	void SetText(LPCTSTR string);
	void Show(int nInter, int nMaxLineCount = 0);
	void Stop();
	inline BOOL IsEnd() { return m_bIsEnd; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWordByWord)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWordByWord();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWordByWord)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void DisplayText(HDC dc);
	void InCreateReturnList(int nNeedLen);
	BOOL SetNextWord();

protected:
	LPTSTR m_strText;

	CRect m_rcClient;
	int m_nHeight;
	int *m_ReturnIndex;
	int m_nReturnListCount;
	int m_nLineCount;

	int m_nMaxLineCount;
	int m_nFirstShowIndex;
	int m_nCurLine;
	int m_nCurIndex;
	int m_nCurLineFirst;

	BOOL m_bIsShowing;
	BOOL m_bIsEnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORDBYWORD_H__71C23541_FA74_47BF_96E1_98DE16A1CBD6__INCLUDED_)
