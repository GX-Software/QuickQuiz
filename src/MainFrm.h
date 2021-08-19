// MainFrm.h : interface of the CMain class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__3631607F_09CE_49E2_8DBF_AD034D07DA63__INCLUDED_)
#define AFX_MAINFRM_H__3631607F_09CE_49E2_8DBF_AD034D07DA63__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EditFrame.h"

class CLView;
class CQView;

class CMain : public CFrameWnd
{
	
protected: // create from serialization only
	CMain();
	DECLARE_DYNCREATE(CMain)

// Attributes
protected:
	CSplitterWnd m_wndSplitter;
public:

// Operations
public:
	void SetItemCount(int nCount);

	void SetShow(LPCTSTR strShow, UINT uTitle);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMain)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMain();
	CQView* GetRightPane();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	CEditFrame	*m_pEditWnd;

// Generated message map functions
protected:
	//{{AFX_MSG(CMain)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	afx_msg void OnColorNormal();
	afx_msg void OnColorGreen();
	afx_msg void OnColorDark();
	afx_msg void OnReset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void LoadSettings();
	void SaveSettings();

	CLView* GetPaperView() { return (CLView*)m_wndSplitter.GetPane(0, 0); }
	CQView* GetItemView() { return (CQView*)m_wndSplitter.GetPane(0, 1); }

private:
	BOOL m_bLoaded;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__3631607F_09CE_49E2_8DBF_AD034D07DA63__INCLUDED_)
