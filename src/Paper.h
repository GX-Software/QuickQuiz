// Paper.h: interface for the CPaper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAPER_H__CB15E1DB_3F8C_4536_B55F_9522FD640FF6__INCLUDED_)
#define AFX_PAPER_H__CB15E1DB_3F8C_4536_B55F_9522FD640FF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum FileType {
	PAPER_FILETYPE_NONE = 0,
	PAPER_FILETYPE_BIN,
	PAPER_FILETYPE_OLDBIN,
	PAPER_FILETYPE_XML,
	PAPER_FILETYPE_JPEGBIN,
	PAPER_FILETYPE_JPEGXML,
	PAPER_FILETYPE_JPEG
};
#define PAPER_FILETYPE_WIN95		0x100 // Win95兼容性
#define PAPER_FILETYPE_MAXCOMP		0x200 // 最大兼容

enum CoverType {
	PAPER_COVERTYPE_NONE = 0,
	PAPER_COVERTYPE_DEFAULT,
	PAPER_COVERTYPE_CUSTOM
};

enum Encryption {
	ENCRYPT_NO = 0,
	ENCRYPT_CBC128AES
};

enum Compress {
	PAPER_COMPRESS_NO = 0,
	PAPER_COMPRESS_ZLIB
};

extern LPCTSTR paper_lpszBinExt;
extern LPCTSTR paper_lpszOldBinExt;
extern LPCTSTR paper_lpszSavExt;

typedef struct _tagPaperCoverInfo
{
	int nType;
	int nWidth;
	int nHeight;
	int nQuality;
	COLORREF clrBk;
	COLORREF clrText;
	LPTSTR strTitle;
	LPTSTR strJpegPath;
	size_t ulDataSize;
	LPBYTE lpData;
}PAPERCOVERINFO, *LPPAPERCOVERINFO;
typedef const LPPAPERCOVERINFO	LPCPAPERCOVERINFO;

typedef BOOL (FUNCCALL *password)(LPSTR pass, LPBYTE md5);

enum PaperType {
	PAPERTYPE_TEXTONLY = 0,
	PAPERTYPE_HTMLPLUS,
	PAPERTYPE_HTMLONLY
};
enum AnswerType {
	ANSWERTYPE_ONLYANSWER = 0,
	ANSWERTYPE_WITHQUESTION
};

class CPaper : public CList
{
public:
	friend CItem* CList::GetHead();

	CPaper();
	virtual ~CPaper();

	virtual inline BOOL IsList() { return FALSE; }
	virtual inline BOOL IsPaper() { return TRUE; }
	inline int GetDefSaveStyle() { return (m_nDefSaveStyle); }

	CTempList* GetTypeList(int nType);
	inline CTempList* GetSaveList() { return m_pSaveList; }
	int GetItemCount(int nType);
	int GetCustomTypeCount();
	CList* CheckQNumberValid(int n, CList *pThis);

	BOOL AllocSingleItems(CList *pList, int nType, BOOL bStore);
	BOOL AllocCustomItems(CList *pList, int nCustIndex, BOOL bStore);

	EBOOL InitPaper(BOOL bIsNewPaper);
	virtual BOOL SetDescription(LPCTSTR szDescription);
	virtual LPCTSTR GetDescription();

	inline void SetEdit(BOOL bCanEdit) { m_bCanEdit = bCanEdit; }
	inline BOOL CanEdit() { return m_bCanEdit; }
	inline BOOL IsModify() { return m_bModify; }

	inline BOOL IsLocked() { return m_nEncryption ? TRUE : FALSE; }
	LPBYTE GetPassword();
	BOOL CheckPassword(const LPBYTE pPswd);
	void SetPassword(LPBYTE pPswd);
	void SetPasswordText(LPCSTR strPass);

	inline BOOL CanAddItems() { return (CList::GetItemCount() < MAX_QUESTION_NUM); }
	BOOL AddList(CList *pList, int nCheckType = TYPE_DEFAULT);
	void ResetTypeList(int nType = TYPE_DEFAULT);

	virtual CItem* GetItem(int nIndex);
	CItem* GetItem(int nType, int nIndex);
	virtual int GetIndex(const CItem *pItem) { ASSERT(FALSE); return -1; }
	virtual int GetItemCount();

	int GetSingleChoiseCount();
	int GetMultiChoiseCount();
	void SingleChoiseToMultiChoise();
	void MultiChoiseToSingleChoise();
	void ChoiseToJudge();

	UINT RemoveAllMarkedItems(UINT uMark);
	virtual void SetItemState(UINT uState, UINT uMark);

	LPCTSTR GetFilePath();
	void GetShortFilePath(LPTSTR pBuff, int nBuffSize);
	void GetPaperInfo(CString &str);

	static int GetFileExt(LPCTSTR strFilePath);
	EBOOL SavePaper(LPCTSTR strFilePath, int nSaveStyle);
	EBOOL LoadPaper(LPCTSTR strFilePath, password funPass);
	void LoadSave(LPCTSTR strFilePath);

	BOOL CreateTestList(CTestPara *pPara, CList *pList);

	void FillTempList(PTYPEPARA pPara, CTempList *pList);

	void SaveStoreQuestions(CList *pList);
	void UnStoreQuestions(CList *pList);

	BOOL Undo(TPARAS *pListInfo);
	BOOL Redo(TPARAS *pListInfo);
	inline BOOL CanUndo() { return (m_sUndoStep.nCurStep != m_sUndoStep.nOldestStep); }
	inline BOOL CanRedo() { return (m_sUndoStep.nCurStep != m_sUndoStep.nNewestStep); }
	inline LPCUNDOSTEP GetUndoStepInfo() { return (&m_sUndoStep); }

	RETURN_E CList* AddCustomType(int nType, LPCTSTR strDefDesc);
	BOOL DelCustomType(int nType);
	void EditCustomType(int nUndoType);
	BOOL CanAddCustomType() { return (GetCustomTypeCount() < TYPE_MAXCUSTOMCOUNT); }
	CTempList* GetCustomTypeAddAfter(CTempList* pList);

	BOOL Paste(CList *pList, int nCheckType = TYPE_DEFAULT);

	static void FUNCCALL ClearCoverInfo(LPPAPERCOVERINFO ppc);
	EBOOL SetCoverInfo(LPPAPERCOVERINFO ppc);
	LPCPAPERCOVERINFO GetCoverInfo();

protected:
	DWORD m_dwVersion;
	int m_nDefSaveStyle;
	int m_nCompressType;

	BOOL m_bCanEdit;
	BOOL m_bModify;

	TCHAR m_strFilePath[MAX_PATH];

	// 保存全部题目的指针
	CList *m_pItemList;
	CTempList *m_pSaveList;

	// 加密相关内容
	BOOL m_nEncryption;
	char m_cPassword[MAX_PASSWORD_LEN + 1];
	UCHAR m_uMD5[16];

	UNDOSTEP m_sUndoStep;

	// 题库类独有，加快查找速度
	int m_nTypeCache;
	CItem *m_pItemCache;
	int m_nItemCache;

	PAPERCOVERINFO m_sCover;
	LPPAPERCOVERINFO m_sUndoCover[UNDO_SIZE];

	BOOL m_bEncodeMBCS;
	BOOL m_bMaxCompatible;

	inline void ClearCache() { m_nTypeCache = TYPE_DEFAULT; m_nItemCache = -1; m_pItemCache = NULL; }
	BOOL AddItem(CItem *pItem, int nCheckType);

	EBOOL LoadPaperM(LPBYTE p, DWORD dwSize, password funcPass);
	EBOOL LoadPaperF(FILE *fp, password funcPass);
	EBOOL LoadXML(PCHAR b);
	EBOOL SavePaperM(FILE *fp);
	EBOOL SavePaperF(FILE *fp);
	EBOOL SaveXML(FILE *fp, BOOL bEncodeMBCS);
	EBOOL PaperToXML(TiXmlDocument *pDoc, BOOL bEncodeMBCS);

	void GetSaveFilePath(LPTSTR strPath);
	void SaveSave();

	void SetUndoChain(PVOID pPara, int nType);
	void ClearUndoChain();
	void ClearUndoCoverInfo();
	BOOL UndoCustomLists(TPARAS *pListInfo);
};

#endif // !defined(AFX_PAPER_H__CB15E1DB_3F8C_4536_B55F_9522FD640FF6__INCLUDED_)
