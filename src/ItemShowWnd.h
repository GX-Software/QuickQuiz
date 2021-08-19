#if !defined(AFX_ITEMSHOWWND_H__E24CC868_6F5C_45A1_B920_71D7468C44F9__INCLUDED_)
#define AFX_ITEMSHOWWND_H__E24CC868_6F5C_45A1_B920_71D7468C44F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemShowWnd.h : header file
//

#include "MemBmp.h"
#include "ShowManager.h"

// ��¼ѡ�����ֿ�ʼ������Ŀ���
typedef struct _tagTextSelInfo
{
	int nStartMgr;
	int nStartBlock;
	int nStartPos;
	int nEndMgr;
	int nEndBlock;
	int nEndPos;
}TEXTSELINFO, *PTEXTSELINFO;

enum ShowWndType {
	SHOWWNDTYPE_PROPERTY = 0,		// չʾ����
	SHOWWNDTYPE_VIEW,				// ѧϰģʽ
	SHOWWNDTYPE_TEST,				// ����ģʽ
	SHOWWNDTYPE_TESTVIEW,			// ���Ժ�鿴��
	SHOWWNDTYPE_SUBJECTIVEVIEW		// �������ľ�
};

#define WM_ITEMSHOWWNDNOTIFY		(WM_USER + 2)
#define ISN_USERSET					0
#define ISN_STOREITEM				1				// lParam�����ղ���Ŀ��ָ��
#define ISN_SELECTITEM				2				// lParam����ѡ����Ŀ��ָ��
#define ISN_BLANKCHECK				3				// lParam����������Ŀ��ָ��
#define ISN_PRE						4
#define ISN_NEXT					5

#define ISP_SMALLFONT				12
#define ISP_NORMALFONT				15
#define ISP_LARGEFONT				20
#define ISP_GIANTFONT				25

/////////////////////////////////////////////////////////////////////////////
// CItemShowWnd window

class CItemShowWnd : public CWnd
{
// Construction
public:
	CItemShowWnd();

	BOOL RegisterClass(HINSTANCE hInstance = NULL);
	LPCTSTR GetClassName();

	void SetItemSelect(CItem *pItem, BOOL bRedraw);
	void InitSelection();
	CItem* GetSelection();

	void ScrollToItem(CItem *pItem, int nSub);
	void InitScroll();

	void CheckImmInput(MSG *pMsg);

// Attributes
public:
	void SetList(CList *pList) { m_pList = pList; } // ����Ҫ��ʾ���б�
	inline CList* GetList() { return m_pList; }
	
	void SetShowType(int nType) { m_nShowType = nType; }
	void ShowItem(int nIndex, DWORD dwShowStyle, int nStep = -1); // ��ʾ��ǰ�б������

	void SetFontSize(long lFontSize, BOOL bRebuild);
	long GetFontSize() { return m_lFontHeight; }

// Operations
public:
	void RefreshShowInfo();	// �������½����߷����ı�ʱ���²���
	void UpdateShowInfo();	// ���û�������Ŀʱ�����²��֣��󲿷ֽ�Ϊƫ�����ı仯��

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemShowWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CItemShowWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CItemShowWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return TRUE; }
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCopy();
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnSaveImg();
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnQEditNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
		
	void SetShowInfo(HDC hDC);
	CShowManager* GetShowManager(int nIndex, CItem *pItem); // ����ָ��λ�õĹ���������������ڣ��򴴽�
	void UpdateShowInfo(HDC hDC, int nStart); // ���û�������Ŀʱ�����²��֣����ı䲿����Ŀ������ƫ�ƣ�

	void DrawItem(HDC hDC);
	int HitTest(int x, int y, PISWHITTEST pHit);

	void SetItemSelect(int nSel, BOOL bRedraw);
	void ScrollToItem(int nIndex, int nSub);

	void ShowEdit(int nMgrIndex, int nBlkIndex);
	void ShowEdit(PTPARAS pEditInfo) { ShowEdit(pEditInfo->dwPara1, pEditInfo->dwPara2); }
	void CloseEdit();

	void SelectNextItem();
	void SelectPrevItem();

	static int FUNCCALL ClearShowManager(PVOID p);

	BOOL ISWSetCursor(CPoint point);
	void SetTextSelect(PISWHITTEST pHit, BOOL bIsStart);
	void Copy(CString &str);

protected:
	int m_nShowType;
	CRect m_rcClient;
	HACCEL m_hAccelTable;

	int m_nShowIndex;		// ������ʾ����ţ�-1Ϊȫ����ʾ��
	int m_nShowStep;		// ��������ʾ����
	DWORD m_dwShowStyle;	// ������ʾ����

	int m_nSel;				// ��ǰ����ѡ�е���Ŀ
	UINT m_uHitTest;		// ����������
	ISWHITTEST m_HitInfo;	// ��������Ϣ

	BOOL m_bEditing;		// �Ƿ����ڱ༭
	TPARAS m_EditInfo;		// ����༭����Ϣ

	long m_lTotalHeight;
	long m_lYScroll;
	long m_lXScroll;
	BOOL m_bYScroll;
	BOOL m_bXScroll;
	BOOL m_bSizing;

	CPoint m_point;			// ���϶�ʱ����¼ÿ�ε����λ��
	int m_nAutoScroll;
	BOOL m_bPointingImg;	// �Ҽ�����ʱָ����ͼƬ
	
	CList *m_pList;
	CDataList *m_pInfo;
	int m_nInfoCount;

	CMemBmp m_Bmp;
	HFONT m_hFont[3];
	long m_lFontHeight;

	CQEdit m_cBlankEdit;
	CQEdit m_cTextEdit;

	BOOL m_bClick;
	TEXTSELINFO m_SelInfo;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMSHOWWND_H__E24CC868_6F5C_45A1_B920_71D7468C44F9__INCLUDED_)
