#if !defined(AFX_QLIST_H__21BC6755_FF3D_4D04_8226_70B974D72A4A__INCLUDED_)
#define AFX_QLIST_H__21BC6755_FF3D_4D04_8226_70B974D72A4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QList.h : header file
//

#define WM_QLISTNOTIFY			(WM_USER + 5)
#define QLN_LBUTTONDBLCLK		0				// lParam±£¥Ê–Ú∫≈
#define QLN_RBUTTONUP			1				// lParam±£¥ÊŒª÷√

enum ListFrameType {
	LISTFRM_TYPE_VIEW = 0,
	LISTFRM_TYPE_TESTING,
	LISTFRM_TYPE_REVIEW,
	LISTFRM_TYPE_EDIT
};

/////////////////////////////////////////////////////////////////////////////
// CQList window

class CQList : public CListCtrl
{
// Construction
public:
	CQList();

// Attributes
public:

// Operations
public:
	void SetList(CList *pList, int nListType, int nSel);
	CItem* GetItem(int nIndex) { return m_List.GetItem(nIndex); }

	void SetCurSel(int nIndex);
	void SetCurSel(CItem *pItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQList)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void UpdateList(int nSel);

	CTempList m_List;
	int m_nListType;
	int m_nCurShowIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QLIST_H__21BC6755_FF3D_4D04_8226_70B974D72A4A__INCLUDED_)
