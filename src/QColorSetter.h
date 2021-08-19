#if !defined(AFX_QCOLORSETTER_H__DBE098D6_BBC0_4715_A99C_EDF21379C0D8__INCLUDED_)
#define AFX_QCOLORSETTER_H__DBE098D6_BBC0_4715_A99C_EDF21379C0D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QColorSetter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQColorSetter window

class CQColorSetter : public CWnd
{
// Construction
public:
	CQColorSetter();

// Attributes
public:

// Operations
public:
	void SetColor(COLORREF clr);
	void SetReadOnly(BOOL bReadOnly) { m_bReadOnly = bReadOnly; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQColorSetter)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQColorSetter();

	CRect m_rcClient;
	COLORREF m_clr;
	BOOL m_bReadOnly;

	// Generated message map functions
protected:
	//{{AFX_MSG(CQColorSetter)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCOLORSETTER_H__DBE098D6_BBC0_4715_A99C_EDF21379C0D8__INCLUDED_)
