// Doc.h : interface of the CDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOC_H__04FBE8F2_CAA9_4ACF_8C9E_957F6944609B__INCLUDED_)
#define AFX_DOC_H__04FBE8F2_CAA9_4ACF_8C9E_957F6944609B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum CopyEx {
	COPYEX_ONLYQ = 0,
	COPYEX_WITHASW,
	COPYEX_ALL
};

class CDoc : public CDocument
{
protected: // create from serialization only
	CDoc();
	DECLARE_DYNCREATE(CDoc)

public:
	// 使所有视图都能响应消息
	BOOL RouteCmdMsg (CView *pActiveView, UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

	inline CList* GetAllPapersList() { return m_pAllPapers; }
	void SetSelectedPaper(CPaper *pPaper, CTempList *pType);
	inline CPaper* GetSelectedPaper() { return (m_pCurSelPaper); }
	inline CList* GetSelectedType() { return (m_pCurSelType); }
	CList* GetSelectedList();
	inline BOOL IsSelectPaper() { return (m_pCurSelPaper && !m_pCurSelType); }
	inline BOOL IsSelectStore() { return (m_pCurSelPaper && m_pCurSelPaper->GetSaveList() == m_pCurSelType); }
	int GetPaperCount();

	void OpenPapers() { OnFileOpen(); }
	BOOL SaveAllPapers(BOOL bAsk);
	EBOOL SetCoverInfo(LPPAPERCOVERINFO ppc);

	void AddNewQuestion(CList *pList);
	void EditQuestion(CList *pList);
	void BatchSet(int nIndex);

	void AddNewGroup(CList *pList);
	void EditGroup(CList *pList);

	BOOL AddCustomType(int nType, LPCTSTR strDefDesc);
	BOOL DelCustomType();

	void Store(CList *pList, CPaper *pPaper = NULL);
	void UnStore(CList *pList);
	void Copy(CTempList *pList);
	void CopyEx(CTempList *pList, int nCopyStyle);
	void Paste();
	void Delete();

	void Undo();
	void Redo();

	void SetEditable();
	void SetLock(LPBYTE btPass, LPCSTR strPass);
	void LoadStore();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bFirst;

	CList *m_pAllPapers;

	CPaper *m_pCurSelPaper;
	CList *m_pCurSelType;

	static BOOL s_bMaxCompatible;
	static BOOL s_bSaveAsWin95;

// Generated message map functions
protected:
	//{{AFX_MSG(CDoc)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAll();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileClose();
	//}}AFX_MSG
	afx_msg BOOL OnFileOpenMRU(UINT nID);
	DECLARE_MESSAGE_MAP()

protected:
	RETURN_E CPaper* CreatePaper(BOOL bIsNewPaper);
	CPaper* IsPaperAlreadyOpen(LPCTSTR strFilePath);

	static BOOL FUNCCALL ShowPassword(LPSTR strPassword, LPBYTE pMD5);
	static UINT_PTR CALLBACK OFNHookProc(HWND hDlg, UINT uiMsg, WPARAM wParam, LPARAM lParam);

	BOOL SavePaper(BOOL bAskIfFail);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOC_H__04FBE8F2_CAA9_4ACF_8C9E_957F6944609B__INCLUDED_)
