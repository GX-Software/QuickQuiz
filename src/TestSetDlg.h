#if !defined(AFX_TESTSETDLG_H__F8F3C089_F41B_4B1B_A8F9_F431F80EB5F4__INCLUDED_)
#define AFX_TESTSETDLG_H__F8F3C089_F41B_4B1B_A8F9_F431F80EB5F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestSetDlg.h : header file
//
#include "TabPage.h"

/////////////////////////////////////////////////////////////////////////////
// CTestSetChsDlg dialog

class CTestSetChsDlg : public CTabPage
{
	// Construction
public:
	CTestSetChsDlg(CWnd* pParent = NULL);   // standard constructor

	virtual BOOL OnKillActive(BOOL bIsDestroy);

	void InitTestPara(CTestPara *pCPara);
	void FillTestPara(CTestPara *pCPara);
	
	// Dialog Data
	//{{AFX_DATA(CTestSetChsDlg)
	enum { IDD = IDD_TESTSET_CHSDLG };
	CQButton	m_cCheckSAsM;
	CQButton	m_cCheckRandom;
	CQButton	m_cCheckMNoSingle;
	CQButton	m_cCheckHalf;
	CQButton	m_cCheckMAsS;
	BOOL	m_bMChAsSCh;
	BOOL	m_bMChHalf;
	BOOL	m_bMChNoSingle;
	BOOL	m_bRandom;
	BOOL	m_bSChAsMCh;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSetChsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CTestSetChsDlg)
	afx_msg void OnCheckMChNoSingle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTestSetBlkTxtDlg dialog

class CTestSetBlkTxtDlg : public CTabPage
{
	// Construction
public:
	CTestSetBlkTxtDlg(CWnd* pParent = NULL);   // standard constructor

	virtual BOOL OnKillActive(BOOL bIsDestroy);

	void InitTestPara(CTestPara *pCPara);
	void FillTestPara(CTestPara *pCPara);
	
	// Dialog Data
	//{{AFX_DATA(CTestSetBlkTxtDlg)
	enum { IDD = IDD_TESTSET_BLKTXTDLG };
	CQButton	m_cCheckTxtAllRight;
	CQButton	m_cCheckRandomBlk;
	CQButton	m_cCheckOnlyBlk;
	CQButton	m_cCheckBlkAllRight;
	BOOL	m_bBlkAllRight;
	BOOL	m_bOnlyBlk;
	BOOL	m_bRandomBlk;
	BOOL	m_bTxtAllRight;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSetBlkTxtDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CTestSetBlkTxtDlg)
	afx_msg void OnCheckOnlyBlk();
	afx_msg void OnCheckRandomBlk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTestSetCommDlg dialog

class CTestSetCommDlg : public CTabPage
{
	// Construction
public:
	CTestSetCommDlg(CList *pPaperList, CPaper *pPaper, CWnd* pParent = NULL);   // standard constructor

	BOOL m_bCanExit;
	
	// Dialog Data
	//{{AFX_DATA(CTestSetCommDlg)
	enum { IDD = IDD_TESTSET_COMMDLG };
	CQButton	m_cCheckQNumOrder;
	CQButton	m_cCheckWholeGroup;
	CQEdit	m_cEditGrpCount;
	CQEdit	m_cEditCustPoint;
	CQEdit	m_cEditCustCount;
	CQComboBox	m_cComboType2;
	CQComboBox	m_cComboType1;
	CQButton	m_cRadioEQU;
	CQButton	m_cRadioDIF;
	CQEdit	m_cEditMinute;
	CQEdit	m_cEditHour;
	CQEdit	m_cEditFullMark;
	CQEdit	m_cEditSChCount;
	CQEdit	m_cEditMChCount;
	CQEdit	m_cEditJdgCount;
	CQEdit	m_cEditBlkCount;
	CQEdit	m_cEditTxtCount;
	CQEdit	m_cEditSChPoint;
	CQEdit	m_cEditMChPoint;
	CQEdit	m_cEditJdgPoint;
	CQEdit	m_cEditBlkPoint;
	CQEdit	m_cEditTxtPoint;
	CQButton	m_cCheckUserStore;
	CQComboBox	m_PaperCombo;
	int		m_nSChCount;
	int		m_nSChPoint;
	int		m_nMChCount;
	int		m_nMChPoint;
	int		m_nJdgCount;
	int		m_nJdgPoint;
	int		m_nBlkCount;
	int		m_nBlkPoint;
	int		m_nTxtCount;
	int		m_nTxtPoint;
	int		m_nTimeHour;
	int		m_nTimeMinute;
	int		m_nFullMark;
	int		m_nMarkType;
	int		m_nCustCount;
	int		m_nCustPoint;
	int		m_nGrpCount;
	BOOL	m_bUseStore;
	BOOL	m_bQNumOrder;
	//}}AFX_DATA
	
public:
	void SetChsDlg(CTestSetChsDlg *pDlg) { m_pChsDlg = pDlg; }

	BOOL CheckCanExit();

	void InitTestPara(CTestPara *pCPara);
	void FillTestPara(CTestPara *pCPara);

	virtual void OnSetActive();
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSetCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CTestSetCommDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnComboSelChange();
	afx_msg void OnCheckUseStore();
	afx_msg void OnRadioEqu();
	afx_msg void OnRadioDif();
	afx_msg void OnSelChangeCountCombo();
	afx_msg void OnSelChangePointCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void FillPaperList();
	void FillTypeList();
	void ShowItemCount();

	void ShowCustomCount();
	BOOL CheckInput(UINT nID = 0);

protected:
	CList *m_pPaperList;
	CPaper *m_pCurSelPaper;

	int m_nItemCount[TYPE_DEFAULTCOUNT];
	TPARAS m_sCustInfo[TYPE_MAXCUSTOMCOUNT];
	CTestSetChsDlg *m_pChsDlg;
};

/////////////////////////////////////////////////////////////////////////////
// CTestSetDlg

class CTestSetDlg : public CClrDialog
{
// Construction
public:
	CTestSetDlg(UINT nIDCaption, CList *pPaperList, CPaper *pCurSelPaper, CWnd* pParent = NULL);
	virtual ~CTestSetDlg();

// Attributes
public:

// Operations
public:
	CTestPara* GetTestPara() { return &m_TestPara; }

// Dialog Data
	//{{AFX_DATA(CTestSetDlg)
	enum { IDD = IDD_TESTSET_DLG };
	CQButton	m_cBtnOK;
	CQButton	m_cBtnCancel;
	CQTabCtrl	m_cTab;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestSetDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	// Generated message map functions
	//{{AFX_MSG(CTestSetDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	void LoadSetPara();
	void SaveSetPara();

protected:
	CList *m_pList;
	CPaper *m_pCurSelPaper;

	UINT m_nIDCaption;

	CTestSetCommDlg m_CommDlg;
	CTestSetChsDlg m_ChsDlg;
	CTestSetBlkTxtDlg m_BlkTxtDlg;

	CTestPara m_TestPara;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTSETDLG_H__F8F3C089_F41B_4B1B_A8F9_F431F80EB5F4__INCLUDED_)
