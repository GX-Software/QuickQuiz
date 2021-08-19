#if !defined(AFX_LISTFRAME_H__D589054A_BA4A_41D2_99C1_DB5467602125__INCLUDED_)
#define AFX_LISTFRAME_H__D589054A_BA4A_41D2_99C1_DB5467602125__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListFrame.h : header file
//

#include "QList.h"

/*
本窗口必须使用父窗口进行销毁
当窗口可以关闭时，实际为隐藏
*/

class CTestDlg;

/////////////////////////////////////////////////////////////////////////////
// CListFrame frame

class CListFrame : public CWnd
{
public:
	CListFrame(CDialog *pParent);

// Attributes
public:

// Operations
public:
	BOOL CreateFrame(LPCTSTR lpszWindowName, const RECT& rect = CFrameWnd::rectDefault, CWnd* pParentWnd = NULL, DWORD dwExStyle = 0);
	void SetWndPos(HWND hWnd);

	void SetList(CList *pList, int nListType, int nSel);
	void UpdateList(int nSel = -1);
	void RefreshList();

	void SetCurSel(int nIndex);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListFrame();

	// Generated message map functions
	//{{AFX_MSG(CListFrame)
	afx_msg void OnClose() {}
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClear();
	//}}AFX_MSG
	afx_msg LRESULT OnQListNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	CQList *m_pListCtrl;

	CDialog *m_pParent;
	CList *m_pList;
	int m_nListType;

	int m_nCurShowIndex;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTFRAME_H__D589054A_BA4A_41D2_99C1_DB5467602125__INCLUDED_)
