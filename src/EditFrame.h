#if !defined(AFX_EDITFRAME_H__8403FF98_2F66_4474_AD71_988594575051__INCLUDED_)
#define AFX_EDITFRAME_H__8403FF98_2F66_4474_AD71_988594575051__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditFrame.h : header file
//

#include "FindText.h"

#define MAX_STRING_LEN   128

class CFixedRecentFileList;
class CQEdit;

/*
本编辑器可生成三种类型的窗口
1、在题目编辑界面中，用于取得文字，允许打开，允许编辑，关闭等于隐藏，退出编辑时销毁。
2、在浏览或答题界面中，用于显示题目组描述，禁止编辑，禁止关闭，退出浏览或答题时销毁
3、在主界面中，用于显示生成的试卷，禁止打开，允许编辑，关闭等于隐藏，退出程序时销毁
分别对应下述三个枚举常量
*/
enum EditFrameType {
	EDITFRAMETYPE_NEWOREDIT = 0,
	EDITFRAMETYPE_SHOWDESC,
	EDITFRAMETYPE_SHOWPAPER
};

/////////////////////////////////////////////////////////////////////////////
// CEditFrame frame

class CEditFrame : public CWnd
{
public:
	CEditFrame(int nType);

// Attributes
public:

// Operations
public:
	BOOL CreateFrame(LPCTSTR lpszWindowName, const RECT& rect = CFrameWnd::rectDefault, CWnd* pParentWnd = NULL, DWORD dwExStyle = 0);

	void SetContent(LPCTSTR lpszContent);
	LPTSTR GetSelText();
	void GetSel(int& nStartChar, int& nEndChar);
	void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	BOOL CheckSaveable(BOOL bCanCancel);

	void UpdateColor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditFrame)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEditFrame();

protected:
	// Generated message map functions
	//{{AFX_MSG(CEditFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnClearMRU();
	afx_msg void OnExit();
	afx_msg void OnUndo();
	afx_msg void OnCut();
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnClear();
	afx_msg void OnFind();
	afx_msg void OnFindNext();
	afx_msg void OnReplace();
	afx_msg void OnSelectAll();
	afx_msg void OnAutoSep();
	afx_msg void OnFont();
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	//}}AFX_MSG
	afx_msg BOOL OnFileOpenMRU(UINT nID);
	afx_msg LRESULT OnFindReplace(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	int m_nType;					// 编辑器窗口类型
	CFixedRecentFileList *m_pRecentFileList;

	HACCEL m_hAccelTable;
	CQEdit *m_pEdit;
	TCHAR m_strFilePath[MAX_PATH];	// 打开的文件名
	int m_nCS;						// 所用的字符集

	CFindReplaceDialog *m_pFindReplaceDlg;
	DWORD m_dwFindFlags; 
	TCHAR m_strFind[MAX_STRING_LEN];
	TCHAR m_strRepl[MAX_STRING_LEN];
	CFindText m_Find;
	BOOL m_bDown;
	BOOL m_bMatchCase;

	BOOL m_bAutoSep;				// 是否自动换行
	LOGFONT m_lf;
	HFONT m_hFont;

protected:
	void CheckUnicode();
	BOOL IsAnyRecentFile();

	BOOL ReadTxtFile(LPCTSTR strFilePath);
	BOOL WriteTxtFile(LPCTSTR strFilePath);

	int ReplaceAll(LPFINDREPLACE ptr);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITFRAME_H__8403FF98_2F66_4474_AD71_988594575051__INCLUDED_)
