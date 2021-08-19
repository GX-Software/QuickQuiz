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
���༭���������������͵Ĵ���
1������Ŀ�༭�����У�����ȡ�����֣�����򿪣�����༭���رյ������أ��˳��༭ʱ���١�
2����������������У�������ʾ��Ŀ����������ֹ�༭����ֹ�رգ��˳���������ʱ����
3�����������У�������ʾ���ɵ��Ծ���ֹ�򿪣�����༭���رյ������أ��˳�����ʱ����
�ֱ��Ӧ��������ö�ٳ���
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
	int m_nType;					// �༭����������
	CFixedRecentFileList *m_pRecentFileList;

	HACCEL m_hAccelTable;
	CQEdit *m_pEdit;
	TCHAR m_strFilePath[MAX_PATH];	// �򿪵��ļ���
	int m_nCS;						// ���õ��ַ���

	CFindReplaceDialog *m_pFindReplaceDlg;
	DWORD m_dwFindFlags; 
	TCHAR m_strFind[MAX_STRING_LEN];
	TCHAR m_strRepl[MAX_STRING_LEN];
	CFindText m_Find;
	BOOL m_bDown;
	BOOL m_bMatchCase;

	BOOL m_bAutoSep;				// �Ƿ��Զ�����
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
