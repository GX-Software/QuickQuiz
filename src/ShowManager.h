// ShowManager.h: interface for the CShowManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_)
#define AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum ShowMetrix {
	SHOWMETRIX_QUESTIONGAP = 0,		// 题目之间的纵向间隙
	SHOWMETRIX_ICONGAP,				// 标记或收藏图标与后面内容的间隙
	SHOWMETRIX_LINEGAP,				// 文字的行距
	SHOWMETRIX_IMAGEGAP,			// 图片横向间隙
	SHOWMETRIX_DESCGAP,				// 题干部分之间的纵向间隙
	SHOWMETRIX_IMAGETITLEGAP,		// 图片与标题的间隙
	SHOWMETRIX_AFTERDESCGAP,		// 题干与后续内容的纵向间隙
	SHOWMETRIX_BLANKGAP,			// 填空题的行距
	SHOWMETRIX_BLANKANSGAP,			// 仅显示填空题答案时，答案之间的间隙
	SHOWMETRIX_RESOLVEGAP,			// 解析之前的间隙
	SHOWMETRIX_UNDERLINEWIDTH,		// 下划线宽度
	SHOWMETRIX_DELETELINEWIDTH,		// 删除线宽度
	SHOWMETRIX_TEXTINDENT,			// 首行缩进量
	SHOWMETRIX_LAST
};

enum ShowMgrHitTest {
	SHOWHITTEST_NONE = 0,			// 未点击任何题目
	SHOWHITTEST_INITEM,				// 在题目区域内点击，但未命中任何内容
	SHOWHITTEST_TEXT,				// 点击了普通文字
	SHOWHITTEST_IMAGE,				// 点击了图片
	SHOWHITTEST_OPTION,				// 点击选择题选项
	SHOWHITTEST_BLANK,				// 点击填空题空白
	SHOWHITTEST_TEXTANS				// 点击简答题编辑区
};

#define SHOWSTYLE_WITHINDEX			0x00000001L	// 包含了题目编号
#define SHOWSTYLE_WITHANSWER		0x00000002L	// 显示题目答案
#define SHOWSTYLE_WITHUSERANSWER	0x00000004L	// 显示用户答案
#define SHOWSTYLE_WITHRESOLVE		0x00000008L	// 显示题目解析
#define SHOWSTYLE_WITHSTORE			0x00000010L // 显示题目是否被收藏

typedef struct _tagShowInfo
{
	int nType;				// 信息类别
	DWORD dwShowStyle;		// 显示方式
	int nIndex;				// 题目编号
	int nInfo;				// 选项及填空信息
	int nIcon;				// 收藏或标记的图标大小

	BOOL bAlloc;			// 是否需要释放内存
	PVOID pData;			// 文字或图片信息
	size_t ulDataSize;		// 文字或图片的大小
	HFONT hFont;			// 所用字体指针

	BOOL bHide;				// 隐藏文字内容
	BOOL bEditing;			// 当编辑时临时隐藏文字内容

	long lWidth;			// 块的总宽度
	long lHeight;			// 块的总高度（用于显示多行文字）
	long lXOff;				// 块的x偏移量
	long lFirstXOff;		// 块首行的x偏移量
	long lFirstTxtXOff;		// 块内首行文字相对块的x向偏移量
	long lLastXOff;			// 最后一行文字结束的x偏移量
	long lFirstYOff;		// 首行文字的y向偏移量
	long lLastYOff;			// 最后一行文字的y向偏移量
	long lLineGap;			// 行间距（为正时向上对齐，为负时向下对齐）
	CDataList *pRetList;	// 记录文字每行的长度（按TCHAR计）
}SHOWINFO, *PSHOWINFO;

// 点击测试
typedef struct _tagISWHitTest
{
	int nHitType;			// 点击类型
	int nMgrIndex;			// 题目序号
	int nBlockIndex;		// 题目内块序号
	int nInfo;				// 块额外信息
	int nTextPos;			// 块内文字序号
}ISWHITTEST, *PISWHITTEST;

typedef struct _tagSelInfo
{
	int nStartBlock;		// 起始块号
	long lStartPos;			// 起始位置在块内的序号
	int nEndBlock;			// 结束块号
	long lEndPos;			// 结束位置在块内的序号
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

	CDataList *m_pInfoList; // 根据不同的题目类型创建不同的显示信息
	int m_nInfoIndex; // 当前正在设置的信息序号

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

	SELINFO m_SelInfo; // 记录选中信息
};

#endif // !defined(AFX_SHOWMANAGER_H__556960E7_5560_4F72_88A9_522F0B97B2C2__INCLUDED_)
