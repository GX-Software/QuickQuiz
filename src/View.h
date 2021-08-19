// View.h : interface of the CQView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIEW_H__B96A8B6D_8FDF_486E_AB7C_F739446F309C__INCLUDED_)
#define AFX_VIEW_H__B96A8B6D_8FDF_486E_AB7C_F739446F309C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NUMCOLUMNS			3
#define MAX_STRING_LEN		128

class CQView : public CListView
{
protected: // create from serialization only
	CQView();
	DECLARE_DYNCREATE(CQView)

// Attributes
public:
	CDoc* GetDocument();

	void UpdateColor();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static LPTSTR sm_szTitle[NUMCOLUMNS];
	static CItem** sm_pSortedItemList;
	static WORD sm_wSortColumn;
	static WORD sm_wSortOrder;

	int m_nItemCount;
	int m_nSortListSize;

	// ≤È’“
	CFindReplaceDialog *m_pFindDlg;
	DWORD m_dwFindFlags; 
	TCHAR m_strFind[MAX_STRING_LEN];

	CImageList m_cImageList;

private:
	COLORREF m_clrDefaultBK;
	COLORREF m_clrDefaultTxt;
	COLORREF m_clrDefaultTxtBK;

	int m_nSummaryLen;

// Generated message map functions
protected:
	//{{AFX_MSG(CQView)
	afx_msg void OnDestroy();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnSelectAll();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnCopyEx();
	afx_msg void OnPaste();
	afx_msg void OnClear();
	afx_msg void OnFind();
	afx_msg void OnFindNext();
	afx_msg void OnNewQuestion();
	afx_msg void OnEditQuestion();
	afx_msg void OnPaperBatchSet();
	afx_msg void OnNewGroup();
	afx_msg void OnEditGroup();
	afx_msg void OnProperties();
	afx_msg void OnEditStore();
	afx_msg void OnTestBegin();
	afx_msg void OnTestView();
	afx_msg void OnTestMake();
	afx_msg void OnTestCompetitive();
	afx_msg void OnSammaryLen();
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	BOOL CheckEditable();
	BOOL CheckGroupEditable();

	void DeleteAllItems();
	void ClearAllStates(BOOL bClearSelected, BOOL bClearFocused);

	UINT GetSelectedItems(CList *pList, UINT nMark);
	void ShowQuestions();

	BOOL SetColumnText(int nIndex, LPCTSTR szText);
	void ResetItems(CList *pList, BOOL bSort);

	void SortList();
	static int CompareItems(const void *arg1, const void *arg2);

	void FindItem();

	void RedrawIcons();

	void CheckDuplicate(int nSimilar);
};

#ifndef _DEBUG  // debug version in View.cpp
inline CDoc* CQView::GetDocument()
   { return (CDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_H__B96A8B6D_8FDF_486E_AB7C_F739446F309C__INCLUDED_)
