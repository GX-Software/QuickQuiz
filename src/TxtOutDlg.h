#if !defined(AFX_TXTOUTDLG_H__9F181BA5_0FBE_49DF_9C50_0D861870C716__INCLUDED_)
#define AFX_TXTOUTDLG_H__9F181BA5_0FBE_49DF_9C50_0D861870C716__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TxtOutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTxtOutDlg dialog

class CTxtOutDlg : public CClrDialog
{
// Construction
public:
	CTxtOutDlg(CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor

	CString m_sAnsiFont;
	CString m_sMainFont;
	CString m_sPreFont;

// Dialog Data
	//{{AFX_DATA(CTxtOutDlg)
	enum { IDD = IDD_TXTOUT_DLG };
	CQButton	m_cCheckRes;
	CQButton	m_cRadioWithQ;
	CQButton	m_cRadioOnly;
	CQComboBox	m_cComboPre;
	CQComboBox	m_cComboMain;
	CQComboBox	m_cComboAnsi;
	CQEdit	m_cEditPath;
	CQButton	m_cBtnPath;
	CListBox	m_cList;
	CQButton	m_cBtnOK;
	int		m_nSel;
	CString	m_sFileFullPath;
	int		m_nAswStyle;
	BOOL	m_bWithRes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTxtOutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPaper *m_pPaper;
	CString m_sFilePath;
	CString m_sFileTitle;

	void SaveSettings();

	void FillFileInfo(LPCTSTR pPath);
	void FillFontList();

	static int CALLBACK EnumFontFamExProc(
		ENUMLOGFONTEX *lpelfe,
		NEWTEXTMETRICEX *lpntme,
		DWORD FontType,
		LPARAM lParam
		);


	// Generated message map functions
	//{{AFX_MSG(CTxtOutDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblClkList();
	afx_msg void OnFilePath();
	afx_msg void OnSelChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TXTOUTDLG_H__9F181BA5_0FBE_49DF_9C50_0D861870C716__INCLUDED_)
