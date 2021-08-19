#if !defined(AFX_QBUTTON_H__2A1AD1FD_A49D_4A91_AC40_E31C8FC5FAA9__INCLUDED_)
#define AFX_QBUTTON_H__2A1AD1FD_A49D_4A91_AC40_E31C8FC5FAA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQButton window

class CQButton : public CButton
{
// Construction
public:
	CQButton();

// Attributes
public:

// Operations
public:
	static void FUNCCALL DrawButton(HWND hWnd, HDC hWndDC, BOOL bDown);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQButton)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQButton)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	BOOL m_bDown;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QBUTTON_H__2A1AD1FD_A49D_4A91_AC40_E31C8FC5FAA9__INCLUDED_)
