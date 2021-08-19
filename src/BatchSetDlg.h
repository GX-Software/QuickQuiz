#if !defined(AFX_BATCHSETDLG_H__BE06B25B_B3D3_47B8_8C64_068B17793538__INCLUDED_)
#define AFX_BATCHSETDLG_H__BE06B25B_B3D3_47B8_8C64_068B17793538__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BatchSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBatchSetDlg dialog

class CBatchSetDlg : public CClrDialog
{
// Construction
public:
	CBatchSetDlg(BOOL bEditFlag, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBatchSetDlg)
	enum { IDD = IDD_BATCHSET_DLG };
	CQButton	m_cBtnOK;
	CQButton	m_cRadioMToS;
	CQButton	m_cRadioSToM;
	CQButton	m_cRadioSelDuplicate;
	CQEdit	m_cEditSimilar;
	CQButton	m_cRadioCToJ;
	int		m_nIndex;
	int		m_nSimilar;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bEditFlag;

	BOOL CheckInput(UINT nID = 0);

	// Generated message map functions
	//{{AFX_MSG(CBatchSetDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHSETDLG_H__BE06B25B_B3D3_47B8_8C64_068B17793538__INCLUDED_)
