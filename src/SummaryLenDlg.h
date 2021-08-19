#if !defined(AFX_SUMMARYLENDLG_H__2EC0764D_AE48_48C9_8F05_F2990C3E9B4C__INCLUDED_)
#define AFX_SUMMARYLENDLG_H__2EC0764D_AE48_48C9_8F05_F2990C3E9B4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SummaryLenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSummaryLenDlg dialog

class CSummaryLenDlg : public CClrDialog
{
// Construction
public:
	CSummaryLenDlg(int nLen, CWnd* pParent = NULL);   // standard constructor

	int m_nLen;

// Dialog Data
	//{{AFX_DATA(CSummaryLenDlg)
	enum { IDD = IDD_SUMMARYLEN_DLG };
	CQButton	m_cBtnOK;
	CQEdit	m_cEditLen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSummaryLenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSummaryLenDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUMMARYLENDLG_H__2EC0764D_AE48_48C9_8F05_F2990C3E9B4C__INCLUDED_)
