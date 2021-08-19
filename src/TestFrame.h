#if !defined(AFX_TESTFRAME_H__9E51591B_0C5B_4155_BAA5_F90CAFEECD1F__INCLUDED_)
#define AFX_TESTFRAME_H__9E51591B_0C5B_4155_BAA5_F90CAFEECD1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestFrame.h : header file
//

#include "ItemShowWnd.h"
#include "QList.h"
#include "TestBar.h"

/////////////////////////////////////////////////////////////////////////////
// CTestFrame frame

class CTestFrame : public CFrameWnd
{
public:
	CTestFrame();           // protected constructor used by dynamic creation
	virtual ~CTestFrame();

// Attributes
public:

// Operations
public:
	BOOL StartTest(CDoc *pDoc);

	void SetList(int nShowStep);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL CreateFrame(LPCTSTR lpszWindowName, const RECT& rect = CFrameWnd::rectDefault, CWnd* pParentWnd = NULL, DWORD dwExStyle = 0);
	void InvertTracker(LPRECT rc);
	void MarkItem(CItem *pItem);
	void StoreItem(CItem *pItem);
	void OnlyFalse(BOOL bOnlyFalse);
	void ShowSingle(BOOL bShowSingle);

	DWORD GetShowStyle();
	CList* GetList();

	void Score(CList *pList, PTESTPARA pPara);

	// Generated message map functions
	//{{AFX_MSG(CTestFrame)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnExitSizeMove(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemShowWndNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTestBarNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnQListNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CItemShowWnd *m_pShow;
	CQList *m_pList;
	CTestBar m_cBar;
	int m_nShowStep;

	BOOL m_bSplitterMoving;
	int m_nSplitterPos;		// ע�⣬����ָ�����Ҳര��Ŀ�ȣ������ָ����Ŀ�ȣ�

	BOOL m_bShowSingle;		// �Ƿ񰴵�����Ŀ��ʾ
	CList m_List;			// ��Ŀ�б���Ŀ�鰴������Ŀ�洢��
	CTempList m_SingleList;	// ��ʱ��Ŀ�б���Ŀ�鰴����Ŀ�洢��
	int m_nCurShow;			// ��ǰ��ʾ����Ŀ

	BOOL m_bOnlyFalse;		// ����ʾ����

	TESTPARA m_Para;
	RECT m_rcClient;
	CTempList m_WrongList;
	CTempList m_WrongSingleList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTFRAME_H__9E51591B_0C5B_4155_BAA5_F90CAFEECD1F__INCLUDED_)
