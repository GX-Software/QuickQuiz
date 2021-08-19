#if !defined(AFX_CODEDLG_H__F192A421_7A3C_4ECC_8C53_CD15E4A0167E__INCLUDED_)
#define AFX_CODEDLG_H__F192A421_7A3C_4ECC_8C53_CD15E4A0167E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CodeDlg.h : header file
//

#include "ClrDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CCodeDlg dialog

class CCodeDlg : public CClrDialog
{
// Construction
public:
	CCodeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCodeDlg)
	enum { IDD = IDD_CODE_DLG };
	CQButton	m_cBtnOK;
	CQComboBox	m_cCombo;
	//}}AFX_DATA

	inline int GetCS() { return m_nCS; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nCS;

	// Generated message map functions
	//{{AFX_MSG(CCodeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEDLG_H__F192A421_7A3C_4ECC_8C53_CD15E4A0167E__INCLUDED_)
