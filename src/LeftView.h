// LeftView.h : interface of the CLView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__F4B11A7B_50A4_4FD8_8A70_E047F550CEF1__INCLUDED_)
#define AFX_LEFTVIEW_H__F4B11A7B_50A4_4FD8_8A70_E047F550CEF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDoc;

class CLView : public CTreeView
{
protected: // create from serialization only
	CLView();
	DECLARE_DYNCREATE(CLView)

public:
	CDoc* GetDocument();

	void UpdateColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList m_cImageList;

private:
	COLORREF m_clrDefaultBK;
	COLORREF m_clrDefaultTxt;

// Generated message map functions
protected:
	//{{AFX_MSG(CLView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnProperties();
	afx_msg void OnEditable();
	afx_msg void OnLock();
	afx_msg void OnNewType();
	afx_msg void OnEditLoadStore();
	afx_msg void OnEditLable();
	afx_msg void OnDelType();
	afx_msg void OnOpenInExplorer();
	afx_msg void OnCoverSet();
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void AddPaper(CPaper *pPaper, HTREEITEM hAfter = TVI_LAST, BOOL bSelect = TRUE);
	void ResetPaper(TPARAS *TParas);
	void AddNewType(CTempList *pList, BOOL bInitEdit);
	void DelNewType(CTempList *pList);
	BOOL IsSelectPaper();

	void ChangeCustomType(TPARAS *pListInfo);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CDoc* CLView::GetDocument()
   { return (CDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEFTVIEW_H__F4B11A7B_50A4_4FD8_8A70_E047F550CEF1__INCLUDED_)
