// ShowManager.h: interface for the CShowManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_)
#define AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ShowMetrix {
	SHOWMETRIX_QUESTIONGAP = 0,		// ��Ŀ֮��������϶
	SHOWMETRIX_ICONGAP,				// ��ǻ��ղ�ͼ����������ݵļ�϶
	SHOWMETRIX_LINEGAP,				// ���ֵ��о�
	SHOWMETRIX_IMAGEGAP,			// ͼƬ�����϶
	SHOWMETRIX_DESCGAP,				// ��ɲ���֮��������϶
	SHOWMETRIX_IMAGETITLEGAP,		// ͼƬ�����ļ�϶
	SHOWMETRIX_AFTERDESCGAP,		// �����������ݵ������϶
	SHOWMETRIX_BLANKGAP,			// �������о�
	SHOWMETRIX_BLANKANSGAP,			// ����ʾ������ʱ����֮��ļ�϶
	SHOWMETRIX_RESOLVEGAP,			// ����֮ǰ�ļ�϶
	SHOWMETRIX_UNDERLINEWIDTH,		// �»��߿��
	SHOWMETRIX_DELETELINEWIDTH,		// ɾ���߿��
	SHOWMETRIX_TEXTINDENT,			// ����������
	SHOWMETRIX_LAST
};

enum ShowMgrHitTest {
	SHOWHITTEST_NONE = 0,			// δ����κ���Ŀ
	SHOWHITTEST_INITEM,				// ����Ŀ�����ڵ������δ�����κ�����
	SHOWHITTEST_TEXT,				// �������ͨ����
	SHOWHITTEST_IMAGE,				// �����ͼƬ
	SHOWHITTEST_OPTION,				// ���ѡ����ѡ��
	SHOWHITTEST_BLANK,				// ��������հ�
	SHOWHITTEST_TEXTANS				// ��������༭��
};

#define SHOWSTYLE_WITHINDEX			0x00000001L	// ��������Ŀ���
#define SHOWSTYLE_WITHANSWER		0x00000002L	// ��ʾ��Ŀ��
#define SHOWSTYLE_WITHUSERANSWER	0x00000004L	// ��ʾ�û���
#define SHOWSTYLE_WITHRESOLVE		0x00000008L	// ��ʾ��Ŀ����
#define SHOWSTYLE_WITHSTORE			0x00000010L // ��ʾ��Ŀ�Ƿ��ղ�

typedef struct _tagShowInfo
{
	int nType;				// ��Ϣ���
	DWORD dwShowStyle;		// ��ʾ��ʽ
	int nIndex;				// ��Ŀ���
	int nInfo;				// ѡ������Ϣ
	int nIcon;				// �ղػ��ǵ�ͼ���С

	BOOL bAlloc;			// �Ƿ���Ҫ�ͷ��ڴ�
	PVOID pData;			// ���ֻ�ͼƬ��Ϣ
	size_t ulDataSize;		// ���ֻ�ͼƬ�Ĵ�С
	HFONT hFont;			// ��������ָ��

	BOOL bHide;				// ������������
	BOOL bEditing;			// ���༭ʱ��ʱ������������

	long lWidth;			// ����ܿ��
	long lHeight;			// ����ܸ߶ȣ�������ʾ�������֣�
	long lXOff;				// ���xƫ����
	long lFirstXOff;		// �����е�xƫ����
	long lFirstTxtXOff;		// ��������������Կ��x��ƫ����
	long lLastXOff;			// ���һ�����ֽ�����xƫ����
	long lFirstYOff;		// �������ֵ�y��ƫ����
	long lLastYOff;			// ���һ�����ֵ�y��ƫ����
	long lLineGap;			// �м�ࣨΪ��ʱ���϶��룬Ϊ��ʱ���¶��룩
	CDataList *pRetList;	// ��¼����ÿ�еĳ��ȣ���TCHAR�ƣ�
}SHOWINFO, *PSHOWINFO;

// �������
typedef struct _tagISWHitTest
{
	int nHitType;			// �������
	int nMgrIndex;			// ��Ŀ���
	int nBlockIndex;		// ��Ŀ�ڿ����
	int nInfo;				// �������Ϣ
	int nTextPos;			// �����������
}ISWHITTEST, *PISWHITTEST;

typedef struct _tagSelInfo
{
	int nStartBlock;		// ��ʼ���
	long lStartPos;			// ��ʼλ���ڿ��ڵ����
	int nEndBlock;			// �������
	long lEndPos;			// ����λ���ڿ��ڵ����
}SELINFO, *PSELINFO;

struct _tagISWHitTest;
class CShowManager  
{
public:
	CShowManager(CItem *pItem, int nShowType, HFONT *pFont);
	virtual ~CShowManager();

	long GetMetrix(int nID);

	int GetBlockCount() { return m_pInfoList->GetDataCount(); }
	int GetBlockLength(int nIndex);

	inline CItem* GetItem() { return m_pItem; }
	long SetItemShow(HDC hDC, long lyOffset, long lWidth, DWORD dwShowStyle, int nIndex, int nStep);
	long RecalcItemShow(HDC hDC, long lyOffset, long lWidth);
	long AddOffset(long lyOffset);
	void DrawItem(HDC hDC, int nXScroll, int nYScroll, BOOL bFocus);

	BOOL IsVisible(long lYPos, long lHeight);
	BOOL HitTest(HDC hDC, long x, long y, PISWHITTEST pHit);
	void SetTextSelect(int nStartBlock, int nStartPos, int nEndBlock, int nEndPos);
	void Copy(CString &str);

	inline long GetShowWidth() { return m_lWidth; }
	inline long GetShowHeight() { return m_lHeight; }
	long GetYOffset();

	BOOL GetEditRect(int nIndex, LPRECT pRC);
	void ShowBlankText(int nIndex, BOOL bShow);

	PITEMIMAGE GetHitImage(PISWHITTEST pHit);

protected:
	static PSHOWINFO FUNCCALL CreateShowInfo(int nType);
	static int FUNCCALL ClearShowInfo(PVOID p);
	PSHOWINFO GetShowInfo(int nIndex, int nShowInfoType, PSHOWINFO *pMalloc);
	void InitShowInfo(PSHOWINFO p);

	PSHOWINFO MakeShowInfo(int nShowInfoType, HDC hDC, HFONT hFont, LPCTSTR str, int nLength,
		long lWidth, long lxOffset, long lyOffset, long *lHeight);
	BOOL MakeImageInfo(int nShowInfoType, HDC hDC, CDataList *pImageList,
		long lWidth, int nLeft, long lyOffset, long *lHeight);

	long GetBlankSampleSize(HDC hDC);
	long GetTextSampleSize(HDC hDC);

	BOOL SetStringShow(PSHOWINFO pInfo, HDC hDC, LPCTSTR lpStr, int nStrLen, long lWidth, long lxOffset, int nPCount = -1);
	long SetDescriptionShow(HDC hDC, long lyOffset, long lWidth);
	long SetOptionsShow(HDC hDC, long lyOffset, long lWidth);
	long SetJudgeShow(HDC hDC, long lyOffset, long lWidth);
	long SetBlanksShow(HDC hDC, long lyOffset, long lWidth);
	long SetTextShow(HDC hDC, long lyOffset, long lWidth);
	long SetGroupShow(HDC hDC, long lyOffset, long lWidth);
	long SetResolveShow(HDC hDC, long lyOffset, long lWidth);

	void DrawString(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle);
	void DrawStore(HDC hDC, int x, int y, int nHeight);
	void DrawImage(HDC hDC, PSHOWINFO pShow, int nXScroll, int nYScroll);
	void DrawOptionAns(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle);
	void DrawJudgeAns(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle);
	void DrawBlank(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle);
	void DrawText(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle);

	void CheckTextPos(HDC hDC, int x, int y, PISWHITTEST pHit);

protected:
	int m_nShowType;
	CItem *m_pItem;

	CDataList *m_pInfoList; // ���ݲ�ͬ����Ŀ���ʹ�����ͬ����ʾ��Ϣ
	int m_nInfoIndex; // ��ǰ�������õ���Ϣ���

	int m_nShowIndex;
	int m_nShowStep;
	DWORD m_dwShowStyle;

	static LPCTSTR s_optionIndex[];
	static TCHAR s_Judge[2][32];
	static TCHAR s_Answer[32];
	static TCHAR s_Resolve[32];
	static TCHAR s_UserAnswer[32];
	static TCHAR s_NoAnswer[32];
	static TCHAR s_Score[32];
	static long s_nMetrix[SHOWMETRIX_LAST];

	static long s_lBlankSampleSize;
	static long s_lTextSampleSize;

	HFONT *m_pFont;
	int m_nFontSize;

	long m_lWidth;
	long m_lHeight;

	SELINFO m_SelInfo; // ��¼ѡ����Ϣ
};

#endif // !defined(AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_)
