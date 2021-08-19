#if !defined(AFX_QEDIT_H__ED5DB8E6_FEBF_483F_8615_4C5718DA9F68__INCLUDED_)
#define AFX_QEDIT_H__ED5DB8E6_FEBF_483F_8615_4C5718DA9F68__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QEdit.h : header file
//

#define WM_QEDITNOTIFY		(WM_USER + 4)
#define QEN_ESCAPE			0
#define QEN_NEXT			1
#define QEN_PREV			2

/////////////////////////////////////////////////////////////////////////////
// CQEdit window

class CQEdit : public CEdit
{
// Construction
public:
	CQEdit();

// Attributes
public:

// Operations
public:
	void SetTextColor(COLORREF clrTxt);
	COLORREF GetTextColor() { return m_clrTxt; }

	long GetText(int nStart, int nEnd, LPTSTR lpBuf);
	long GetSelText(LPTSTR lpBuf);

	void EnableCopy(BOOL bEnable) { m_bEnableCopy = bEnable; }

	void CheckSelChange(BOOL bCheck) { m_bCheckSelChange = bCheck; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQEdit)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnNcPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCopy(WPARAM wParam, LPARAM lParam);
	void DrawBorder();

	COLORREF GetClientEdgeClr();
	LRESULT NotifySelChange();

protected:
	COLORREF m_clrTxt;

	HBRUSH m_hbrReadOnly;
	BOOL m_bFocus;

	BOOL m_bEnableCopy;

	BOOL m_bCheckSelChange;
	int m_nStart;
	int m_nEnd;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QEDIT_H__ED5DB8E6_FEBF_483F_8615_4C5718DA9F68__INCLUDED_)
