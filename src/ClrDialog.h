#if !defined(AFX_CLRDIALOG_H__5C025795_F6AB_417F_BEF7_579AF050D5D0__INCLUDED_)
#define AFX_CLRDIALOG_H__5C025795_F6AB_417F_BEF7_579AF050D5D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClrDialog.h : header file
//

#include "QButton.h"
#include "QEdit.h"
#include "QGroupBox.h"
#include "QTabCtrl.h"
#include "QComboBox.h"
#include "QColorSetter.h"

/////////////////////////////////////////////////////////////////////////////
// CClrDialog dialog

class CClrDialog : public CDialog
{
// Construction
public:
	CClrDialog(UINT nIDTemplate, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CClrDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClrDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CClrDialog)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLRDIALOG_H__5C025795_F6AB_417F_BEF7_579AF050D5D0__INCLUDED_)
