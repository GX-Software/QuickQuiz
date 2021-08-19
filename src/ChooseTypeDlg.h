#if !defined(AFX_CHOOSETYPEDLG_H__323A5A94_9E0A_48AC_8AF7_8FC324AEB100__INCLUDED_)
#define AFX_CHOOSETYPEDLG_H__323A5A94_9E0A_48AC_8AF7_8FC324AEB100__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseTypeDlg.h : header file
//
#include "ClrDialog.h"

#define LASTVIEWLISTSIZE	10

/////////////////////////////////////////////////////////////////////////////
// CChooseTypeDlg dialog

class CChooseTypeDlg : public CClrDialog
{
// Construction
public:
	CChooseTypeDlg(PTYPEPARA pPara, CPaper *pPaper,
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseTypeDlg)
	enum { IDD = IDD_CHOOSETYPE_DLG };
	CListCtrl	m_cList;
	CQButton	m_cBtnOK;
	CQGroupBox	m_cGroup1;
	CQButton	m_cCheckRandom;
	BOOL	m_bRandom;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseTypeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickType(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void InitShow();

protected:
	PTYPEPARA m_pPara;
	CPaper *m_pPaper;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSETYPEDLG_H__323A5A94_9E0A_48AC_8AF7_8FC324AEB100__INCLUDED_)
