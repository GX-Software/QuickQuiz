#if !defined(AFX_COPYEXDLG_H__7D594A0F_EB6A_40B6_B9A8_DA9869D769B4__INCLUDED_)
#define AFX_COPYEXDLG_H__7D594A0F_EB6A_40B6_B9A8_DA9869D769B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CopyExDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCopyExDlg dialog

class CCopyExDlg : public CClrDialog
{
// Construction
public:
	CCopyExDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCopyExDlg)
	enum { IDD = IDD_COPYEX_DLG };
	CQButton	m_cRadioAsw;
	CQButton	m_cRadioQ;
	CQButton	m_cRadioAll;
	CQButton	m_cBtnOK;
	int		m_nSel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCopyExDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCopyExDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COPYEXDLG_H__7D594A0F_EB6A_40B6_B9A8_DA9869D769B4__INCLUDED_)
