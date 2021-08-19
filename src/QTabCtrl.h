#if !defined(AFX_QTABCTRL_H__67849A40_66AF_4A80_9932_B24E59CC1F33__INCLUDED_)
#define AFX_QTABCTRL_H__67849A40_66AF_4A80_9932_B24E59CC1F33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QTabCtrl.h : header file
//

class CTabPage;

#define MAXPAGENUM		6

/////////////////////////////////////////////////////////////////////////////
// CQTabCtrl window

class CQTabCtrl : public CTabCtrl
{
// Construction
public:
	CQTabCtrl();

// Attributes
public:

// Operations
public:
	BOOL AddPage(UINT nIDTemplate, CTabPage *pPage, LPCTSTR szTitle);

	CTabPage* GetPage(int nIndex);
	int GetPageCount() { return m_nPageCount; }

	void SwitchPage(int nIndex);
	void SetChangeable(BOOL bIsChangeable) { m_bCanChangeType = bIsChangeable; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQTabCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQTabCtrl)
	afx_msg void OnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetChildPos(CTabPage *pPage);
	BOOL SwitchShow(int nIndex);

	void Paint(HDC hDC);

protected:
	int m_nPageCount;
	int m_nShowingPage;

	CTabPage *m_pPageList[MAXPAGENUM];

	BOOL m_bCanChangeType;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QTABCTRL_H__67849A40_66AF_4A80_9932_B24E59CC1F33__INCLUDED_)
