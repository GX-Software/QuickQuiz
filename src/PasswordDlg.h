#if !defined(AFX_PASSWORDDLG_H__C8E8754A_A523_451E_B21A_6BE18A6E05B3__INCLUDED_)
#define AFX_PASSWORDDLG_H__C8E8754A_A523_451E_B21A_6BE18A6E05B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PasswordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CClrDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

	void SetPara(BOOL bIsLocked, LPBYTE pOldPswd);
	LPBYTE GetPassword() { return m_uNewPswd; }
	LPCSTR GetPasswordText() { return m_sPswd; }

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_PASSWORD_DLG };
	CQEdit	m_cEditCheckPswd;
	CQEdit	m_cEditNewPswd;
	CQEdit	m_cEditOldPswd;
	CQButton	m_cBtnOK;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	BOOL CheckInput();

// Implementation
protected:
	BOOL m_bIsLocked;

	BYTE m_uOldPswd[16];
	BYTE m_uNewPswd[16];
	char m_sPswd[MAX_PASSWORD_LEN + 1];

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CUnlockDlg dialog

class CUnlockDlg : public CClrDialog
{
	// Construction
public:
	CUnlockDlg(LPBYTE pPswd, CWnd* pParent = NULL);   // standard constructor

	LPCSTR GetPasswordText() { return m_sPswd; }
	
	// Dialog Data
	//{{AFX_DATA(CUnlockDlg)
	enum { IDD = IDD_UNLOCK_DLG };
	CQButton	m_cBtnOK;
	CQEdit	m_cEditPswd;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUnlockDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	BOOL CheckInput();
	
	// Implementation
protected:
	BYTE m_uTrue[16];
	char m_sPswd[MAX_PASSWORD_LEN + 1];
	
	// Generated message map functions
	//{{AFX_MSG(CUnlockDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PASSWORDDLG_H__C8E8754A_A523_451E_B21A_6BE18A6E05B3__INCLUDED_)
