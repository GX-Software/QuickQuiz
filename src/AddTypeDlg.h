#if !defined(AFX_ADDTYPEDLG_H__6E558DB1_48D3_40D7_B652_7B2746600FA2__INCLUDED_)
#define AFX_ADDTYPEDLG_H__6E558DB1_48D3_40D7_B652_7B2746600FA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddTypeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddTypeDlg dialog

class CAddTypeDlg : public CClrDialog
{
// Construction
public:
	CAddTypeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddTypeDlg)
	enum { IDD = IDD_ADDTYPE_DLG };
	CQButton	m_cBtnOK;
	CQComboBox	m_cCombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddTypeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddTypeDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDTYPEDLG_H__6E558DB1_48D3_40D7_B652_7B2746600FA2__INCLUDED_)
