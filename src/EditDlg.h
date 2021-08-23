#if !defined(AFX_EDITDLG_H__93953B63_5F7A_4133_8304_0AC5DEEB3F70__INCLUDED_)
#define AFX_EDITDLG_H__93953B63_5F7A_4133_8304_0AC5DEEB3F70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditDlg.h : header file
//
#include "TabPage.h"
#include "ClrDialog.h"
#include "DataList.h"

class CEditFrame;
class CListFrame;
class CImageFrame;

class CChoise;
class CJudge;
class CBlank;
class CText;

class CEditDlg;

/////////////////////////////////////////////////////////////////////////////
// CChildSChoiseDlg dialog

class CChildSChoiseDlg : public CTabPage
{
	// Construction
public:
	CChildSChoiseDlg(CWnd* pParent = NULL);   // standard constructor
	
	virtual void SetShow(const CItem *pChoise);
	virtual BOOL OnKillActive(BOOL bIsDestroy);

	void SetMChoisePtr(CTabPage *pPage) { m_pMChoisePage = pPage; }
	void FillChoiseText(CTabPage *pPage);
	
	virtual int CheckSaveable();
	void GetInfo(CChoise *pChoise);
	
	// Dialog Data
	//{{AFX_DATA(CChildSChoiseDlg)
	enum { IDD = IDD_CHILD_SCHOISE_DLG };
	CQButton	m_cBtnAuto;
	CQButton	m_cRadioAnswer1;
	CQButton	m_cRadioAnswer2;
	CQButton	m_cRadioAnswer3;
	CQButton	m_cRadioAnswer4;
	CQButton	m_cRadioAnswer5;
	CQButton	m_cRadioAnswer6;
	CQButton	m_cRadioAnswer7;
	CQButton	m_cRadioAnswer8;
	CQButton	m_cBtnSet1;
	CQButton	m_cBtnSet2;
	CQButton	m_cBtnSet3;
	CQButton	m_cBtnSet4;
	CQButton	m_cBtnSet5;
	CQButton	m_cBtnSet6;
	CQButton	m_cBtnSet7;
	CQButton	m_cBtnSet8;
	CQEdit	m_cEditAnswer1;
	CQEdit	m_cEditAnswer2;
	CQEdit	m_cEditAnswer3;
	CQEdit	m_cEditAnswer4;
	CQEdit	m_cEditAnswer5;
	CQEdit	m_cEditAnswer6;
	CQEdit	m_cEditAnswer7;
	CQEdit	m_cEditAnswer8;
	int		m_nAnswer;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildSChoiseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChildSChoiseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnRadioChange();
	afx_msg void OnButtonAuto();
	//}}AFX_MSG
	afx_msg void OnSet(UINT uID);
	DECLARE_MESSAGE_MAP()

	CTabPage *m_pMChoisePage;
};

/////////////////////////////////////////////////////////////////////////////
// CChildMChoiseDlg dialog

class CChildMChoiseDlg : public CTabPage
{
	// Construction
public:
	CChildMChoiseDlg(CWnd* pParent = NULL);   // standard constructor
	
	virtual void SetShow(const CItem *pChoise);
	virtual BOOL OnKillActive(BOOL bIsDestroy);

	void SetSChoisePtr(CTabPage *pPage) { m_pSChoisePage = pPage; }
	void FillChoiseText(CTabPage *pPage);
	
	virtual int CheckSaveable();
	void GetInfo(CChoise *pChoise);
	
	// Dialog Data
	//{{AFX_DATA(CChildMChoiseDlg)
	enum { IDD = IDD_CHILD_MCHOISE_DLG };
	CQButton	m_cBtnAuto;
	CQButton	m_cCheckAnswer1;
	CQButton	m_cCheckAnswer2;
	CQButton	m_cCheckAnswer3;
	CQButton	m_cCheckAnswer4;
	CQButton	m_cCheckAnswer5;
	CQButton	m_cCheckAnswer6;
	CQButton	m_cCheckAnswer7;
	CQButton	m_cCheckAnswer8;
	CQButton	m_cBtnSet1;
	CQButton	m_cBtnSet2;
	CQButton	m_cBtnSet3;
	CQButton	m_cBtnSet4;
	CQButton	m_cBtnSet5;
	CQButton	m_cBtnSet6;
	CQButton	m_cBtnSet7;
	CQButton	m_cBtnSet8;
	CQEdit	m_cEditAnswer1;
	CQEdit	m_cEditAnswer2;
	CQEdit	m_cEditAnswer3;
	CQEdit	m_cEditAnswer4;
	CQEdit	m_cEditAnswer5;
	CQEdit	m_cEditAnswer6;
	CQEdit	m_cEditAnswer7;
	CQEdit	m_cEditAnswer8;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildMChoiseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CChildMChoiseDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnButtonAuto();
	//}}AFX_MSG
	afx_msg void OnSet(UINT uID);
	DECLARE_MESSAGE_MAP()

	CTabPage *m_pSChoisePage;
};

/////////////////////////////////////////////////////////////////////////////
// CChildJudgeDlg dialog

class CChildJudgeDlg : public CTabPage
{
	// Construction
public:
	CChildJudgeDlg(CWnd* pParent = NULL);   // standard constructor
	
	virtual void SetShow(const CItem *pJudge);
	
	virtual int CheckSaveable() { return SAVEABLE_OK; }
	void GetInfo(CJudge *pJudge);
	
	// Dialog Data
	//{{AFX_DATA(CChildJudgeDlg)
	enum { IDD = IDD_CHILD_JUDGE_DLG };
	CQButton	m_cRadioTrue;
	CQButton	m_cRadioFalse;
	int		m_nAnswer;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildJudgeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CChildJudgeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CChildBlankDlg dialog

class CChildBlankDlg : public CTabPage
{
	// Construction
public:
	CChildBlankDlg(CWnd* pParent = NULL);   // standard constructor
	~CChildBlankDlg();

	inline void SetDescEdit(CQEdit* pEdit, CQButton* pBtn) { m_pDescEdit = pEdit; m_pSetBtn = pBtn; }
	virtual BOOL OnKillActive(BOOL bIsDestroy);

	virtual void SetShow(const CItem *pItem);

	virtual int CheckSaveable();
	void GetInfo(CBlank *pBlank);
	
	// Dialog Data
	//{{AFX_DATA(CChildBlankDlg)
	enum { IDD = IDD_CHILD_BLANK_DLG };
	CQEdit		m_cEditAnswer;
	CListCtrl	m_cList;
	CQButton	m_cBtnDelWithAsw;
	CQButton	m_cBtnDelNoAsw;
	CQButton	m_cBtnEdit;
	CQButton	m_cBtnAdd;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildBlankDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	void UpdatePage();
	void ShowErrorInfo();
	
	// Generated message map functions
	//{{AFX_MSG(CChildBlankDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonDelWithAsw();
	afx_msg void OnButtonDelNoAsw();
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblClkListBlanks(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CQEdit*	m_pDescEdit;
	CQButton* m_pSetBtn;
	CBlank*	m_pBlank;
};

/////////////////////////////////////////////////////////////////////////////
// CChildTextDlg dialog

class CChildTextDlg : public CTabPage
{
	// Construction
public:
	CChildTextDlg(CWnd* pParent = NULL);   // standard constructor

	virtual void SetShow(const CItem *pItem);

	virtual int CheckSaveable();
	void GetInfo(CText *pText);
	
	// Dialog Data
	//{{AFX_DATA(CChildTextDlg)
	enum { IDD = IDD_CHILD_TEXT_DLG };
	CQButton	m_cBtnAsw;
	CQEdit	m_cEditAnswer;
	CQGroupBox	m_cGroup1;
	//}}AFX_DATA
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildTextDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	
	// Implementation
protected:
	
	// Generated message map functions
	//{{AFX_MSG(CChildTextDlg)
	afx_msg void OnChange();
	afx_msg void OnButtonAnswer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEditDlg dialog

#define DELTYPE_BUTTON		0
#define DELTYPE_MARK		1
#define DELTYPE_DO			2

class CEditDlg : public CClrDialog
{
// Construction
public:
	CEditDlg(int nInitShow = 0, CWnd* pParent = NULL);
	virtual ~CEditDlg();

	void SetEditPara(CList *pList, CList *pAddList, int nType);
	BOOL IsEdited() { return m_bEdited; }

	int GetCurSel() { return (m_nCurShow); }
	BOOL SetCurSel(int nIndex);
	void Delete(int nDelType, int nIndex = -1);

	CList* GetList() { return (m_pList); }
	CEditFrame* GetEditFrame() { return m_pEditWnd; }
	LPTSTR GetEditFrmSel();

// Dialog Data
	//{{AFX_DATA(CEditDlg)
	enum { IDD = IDD_EDIT_DLG };
	CQButton	m_cBtnResImage;
	CQButton	m_cBtnDescImage;
	CQButton	m_cBtnOK;
	CQButton	m_cCheckTopMost;
	CQButton	m_cCheckQList;
	CQButton	m_cBtnText;
	CQButton	m_cBtnPre;
	CQButton	m_cBtnNext;
	CQButton	m_cBtnNew;
	CQButton	m_cBtnDesc;
	CQButton	m_cBtnResolve;
	CQButton	m_cBtnDel;
	CQButton	m_cBtnAllBuild;
	CQEdit	m_cEditDesc;
	CQEdit	m_cEditResolve;
	CQGroupBox	m_cGroup1;
	CQGroupBox	m_cGroup2;
	CQTabCtrl	m_cTab;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CList				*m_pList;			// 新建题目的列表
	int					m_nCurShow;			// 当前显示的题目
	int					m_nLastShow;		// 上一个显示的题目
	int					m_nInitShow;		// 编辑器启动时默认的位置
	CList				*m_pAddList;		// 要加入题目的列表（可能是题库）

	CChildSChoiseDlg	*m_pSChoiseDlg;		// 单选题页面
	CChildMChoiseDlg	*m_pMChoiseDlg;		// 多选题页面
	CChildJudgeDlg		*m_pJudgeDlg;		// 判断题页面
	CChildBlankDlg		*m_pBlankDlg;		// 填空题页面
	CChildTextDlg		*m_pTextDlg;		// 简答题页面
	int					m_nType;			// 固定显示的题目类型

	BOOL				m_bIsNew;			// 对话框模式，是新建还是编辑现有

	BOOL				m_bEdited;			// 是否已经经过编辑
	BOOL				m_bModify;			// 题干部分是否已经修改过

	CEditFrame			*m_pEditWnd;		// 编辑器
	CListFrame			*m_pListWnd;		// 用来显示题目情况

	CImageFrame			*m_pImageWnd;		// 用来显示题目图片
	CDataList			m_DescImageList;	// 临时保存题干图片
	CDataList			m_ResvImageList;	// 临时保存解析图片

	// Generated message map functions
	//{{AFX_MSG(CEditDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChange();
	afx_msg void OnButtonPre();
	afx_msg void OnButtonNext();
	afx_msg void OnButtonNew();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonAllBuild();
	afx_msg void OnButtonText();
	afx_msg void OnButtonDesc();
	afx_msg void OnButtonResolve();
	afx_msg void OnCheckTopMost();
	afx_msg void OnCheckQList();
	afx_msg void OnSetDescImage();
	afx_msg void OnSetResImage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetShow();
	void ShowPage();
	BOOL CheckModify();

	int CheckSaveable();
	BOOL SaveCurrent();
	
	CChoise* MakeChoise(BOOL bIsSingle);
	CJudge* MakeJudge();
	CBlank* MakeBlank();
	CText* MakeText();

	void SaveCommon(CItem *pItem);
	void SaveChoise(CItem *pChoise, int nType);
	void SaveJudge(CItem *pJudge);
	void SaveBlank(CItem *pBlank);
	void SaveText(CItem *pText);

	int Index2Type(int nIndex);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITDLG_H__93953B63_5F7A_4133_8304_0AC5DEEB3F70__INCLUDED_)
