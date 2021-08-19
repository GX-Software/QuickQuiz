#if !defined(AFX_QCOMBOBOX_H__CBB3F044_76F0_4B72_BED6_9F6762B8CC88__INCLUDED_)
#define AFX_QCOMBOBOX_H__CBB3F044_76F0_4B72_BED6_9F6762B8CC88__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CQComboBox window

class CQComboBox : public CComboBox
{
// Construction
public:
	CQComboBox();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CQComboBox)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	void GetPoints(LPRECT rc, LPPOINT pt);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCOMBOBOX_H__CBB3F044_76F0_4B72_BED6_9F6762B8CC88__INCLUDED_)
