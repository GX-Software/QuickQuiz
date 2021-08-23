// ShowManager.cpp: implementation of the CShowManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ShowManager.h"
#include "ItemShowWnd.h"
#include "ImageManager.h"
#include "MemBmp.h"

#include "Choise.h"
#include "Judge.h"
#include "Blank.h"
#include "Text.h"
#include "ItemGroup.h"

#pragma comment(lib, "Msimg32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define TESTVIEW_RED		RGB(255, 0, 0)
#define TESTVIEW_GREEN		RGB(0, 128, 0)
#define TESTVIEW_ORANGE		RGB(255, 140, 0)

#define DRAWSTYLE_FOCUS			0x00000001L
#define DRAWSTYLE_UNDERLINE		0x00000002L
#define DRAWSTYLE_DELETELINE	0x00000004L

enum ShowType {
	SHOWTYPE_TEXTDIVISION = 0,	// 末尾需换行的普通文字
	SHOWTYPE_TEXTSPAN,			// 末尾不换行的普通文字
	SHOWTYPE_TEXTPARAGRAPH,		// 自然段文字（用于题目组描述）
	SHOWTYPE_IMAGE,				// 普通图片
	SHOWTYPE_OPTIONINDEX,		// 选择题选项号（后面不换行）
	SHOWTYPE_OPTION,			// 选择题/判断题选项内容（后面换行）
	SHOWTYPE_OPTIONANS,			// 选择题答案（带颜色）
	SHOWTYPE_JUDGEANS,			// 判断题答案（带颜色）
	SHOWTYPE_BLANK,				// 填空题中的空白内容
	SHOWTYPE_BLANKDIVISION,		// 填空题后面换行的空白
	SHOWTYPE_BLANKANS,			// 填空题后的正确答案
	SHOWTYPE_TEXT,				// 简答题中的答案
	SHOWTYPE_TEXTANS,			// 用户的答案
};

LPCTSTR CShowManager::s_optionIndex[] = {
	_T("A. "), _T("B. "), _T("C. "), _T("D. "), _T("E. "), _T("F. "), _T("G. "), _T("H. ")
};

long CShowManager::s_nMetrix[SHOWMETRIX_LAST] = {
	25,				// 题目之间的纵向间隙
	3,				// 标记或收藏图标与后面内容的间隙
	1,				// 文字的行距
	5,				// 图片横向间隙
	3,				// 题干部分之间的纵向间隙
	3,				// 图片与标题的间隙
	7,				// 题干与后续内容的纵向间隙
	10,				// 填空题的行距
	15,				// 仅显示填空题答案时，答案之间的间隙
	7,				// 解析之前的间隙
	1,				// 下划线宽度
	2,				// 删除线宽度
	30				// 首行缩进量
};
static int s_nDefaultFontSize = 15;

TCHAR CShowManager::s_Answer[32] = {0};
TCHAR CShowManager::s_Resolve[32] = {0};
TCHAR CShowManager::s_UserAnswer[32] = {0};
TCHAR CShowManager::s_NoAnswer[32] = {0};
TCHAR CShowManager::s_Judge[2][32] = {0};
TCHAR CShowManager::s_Score[32] = {0};

long CShowManager::s_lBlankSampleSize = 0;
long CShowManager::s_lTextSampleSize = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShowManager::CShowManager(CItem *pItem, int nShowType, HFONT *pFont) :
	m_nShowType(nShowType), m_pItem(pItem), m_pInfoList(NULL), m_nInfoIndex(0),
	m_pFont(pFont), m_nFontSize(s_nDefaultFontSize), m_lWidth(0), m_lHeight(0),
	m_nShowIndex(0), m_nShowStep(-1), m_dwShowStyle(0)
{
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_TEXTASW, s_Answer, 32);
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_RESOLVE, s_Resolve, 32);
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_USERASW, s_UserAnswer, 32);
	LoadString(AfxGetInstanceHandle(), IDS_QLIST_UNASW, s_NoAnswer, 32);
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_CORRECT, s_Judge[0], 32);
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_INCORRECT, s_Judge[1], 32);
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_SCORE, s_Score, 32);

	ZeroMemory(&m_SelInfo, sizeof(m_SelInfo));
}

CShowManager::~CShowManager()
{
	if (m_pInfoList)
	{
		int i;
		for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
		{
			ClearShowInfo(m_pInfoList->GetData(i));
		}
		delete m_pInfoList;
	}
}

long CShowManager::GetMetrix(int nID)
{
	long lRet = s_nMetrix[nID] * m_nFontSize / s_nDefaultFontSize;
	if (lRet <= 0)
	{
		lRet = 1;
	}

	return lRet;
}

int CShowManager::GetBlockLength(int nIndex)
{
	PSHOWINFO pShow = (PSHOWINFO)m_pInfoList->GetData(nIndex);
	if (!pShow || !pShow->pRetList->GetDataCount())
	{
		return 0;
	}

	return pShow->ulDataSize;
}

long CShowManager::GetYOffset()
{
	PSHOWINFO pInfo = (PSHOWINFO)m_pInfoList->GetData(0);
	if (!pInfo)
	{
		return 0;
	}

	return pInfo->lFirstYOff;
}

BOOL CShowManager::GetEditRect(int nIndex, LPRECT pRC)
{
	int i = 0;
	int nDescHeight = 0;
	PSHOWINFO pInfo = (PSHOWINFO)m_pInfoList->GetData(i);

	while(pInfo)
	{
		if (SHOWTYPE_TEXTDIVISION == pInfo->nType ||
			SHOWTYPE_IMAGE == pInfo->nType)
		{
			nDescHeight += pInfo->lHeight;
		}
		else if (SHOWTYPE_BLANK == pInfo->nType ||
				 SHOWTYPE_BLANKDIVISION == pInfo->nType)
		{
			if (!nIndex)
			{
				break;
			}
			--nIndex;
		}
		else if (SHOWTYPE_TEXT == pInfo->nType ||
				 SHOWTYPE_TEXTANS == pInfo->nType)
		{
			break;
		}

		++i;
		pInfo = (PSHOWINFO)m_pInfoList->GetData(i);
	}
	if (!pInfo)
	{
		return FALSE;
	}

	switch(pInfo->nType)
	{
	case SHOWTYPE_BLANK:
	case SHOWTYPE_BLANKDIVISION:
		pRC->left = pInfo->lFirstXOff;
		if (pInfo->pRetList->GetDataCount() <= 1)
		{
			pRC->right = pInfo->lLastXOff + pInfo->lFirstTxtXOff * 2;
		}
		else
		{
			pRC->right = m_lWidth;
		}
		pRC->top = pInfo->lFirstYOff + pInfo->lLineGap / 2;
		pRC->bottom = pInfo->lFirstYOff + (pInfo->lHeight / pInfo->pRetList->GetDataCount());
		break;

	case SHOWTYPE_TEXT:
	case SHOWTYPE_TEXTANS:
		pRC->left = 0;
		pRC->right = m_lWidth;
		pRC->top = pInfo->lFirstYOff;
		pRC->bottom = pInfo->lFirstYOff + m_lHeight - nDescHeight;
		break;

	default:
		break;
	}
	return TRUE;
}

void CShowManager::ShowBlankText(int nIndex, BOOL bShow)
{
	int i;
	PSHOWINFO pShow;

	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		pShow = (PSHOWINFO)m_pInfoList->GetData(i);
		if (!pShow)
		{
			return;
		}

		if (SHOWTYPE_BLANK == pShow->nType ||
			SHOWTYPE_BLANKDIVISION == pShow->nType)
		{
			if (!nIndex)
			{
				pShow->bEditing = !bShow;
				return;
			}
			--nIndex;
		}
	}
}

PITEMIMAGE CShowManager::GetHitImage(PISWHITTEST pHit)
{
	if (pHit->nHitType != SHOWHITTEST_IMAGE)
	{
		return NULL;
	}

	PSHOWINFO pInfo = (PSHOWINFO)m_pInfoList->GetData(pHit->nBlockIndex);
	return (PITEMIMAGE)pInfo->pData;
}

PSHOWINFO FUNCCALL CShowManager::CreateShowInfo(int nType)
{
	PSHOWINFO pInfo = (PSHOWINFO)malloc(sizeof(SHOWINFO));
	if (!pInfo)
	{
		return NULL;
	}
	ZeroMemory(pInfo, sizeof(SHOWINFO));
	
	pInfo->pRetList = new CDataList;
	if (!pInfo->pRetList)
	{
		ClearShowInfo(pInfo);
		return NULL;
	}
	
	pInfo->nType = nType;
	return pInfo;
}

int FUNCCALL CShowManager::ClearShowInfo(PVOID p)
{
	PSHOWINFO pInfo = (PSHOWINFO)p;

	if (!pInfo)
	{
		return 0;
	}

	if (pInfo->pRetList)
	{
		delete pInfo->pRetList;
	}
	if (pInfo->bAlloc)
	{
		free(pInfo->pData);
		pInfo->pData = NULL;
		pInfo->ulDataSize = 0;
	}
	
	free(p);
	return 0;
}

/*
功能：	将显示信息的内容清空
返回值：无
备注：	不影响换行列表
*/
void CShowManager::InitShowInfo(PSHOWINFO p)
{
	if (!p)
	{
		return;
	}

	if (p->bAlloc)
	{
		free(p->pData);
	}

	CDataList *pList = p->pRetList;
	ZeroMemory(p, sizeof(SHOWINFO));
	p->pRetList = pList;
}

/*
功能：	取得一个显示信息指针
返回值：取得的指针
备注：	pMalloc参数需要传入一个指针
		如果取得的显示信息指针是malloc得出的，保存于pMalloc
		如果取得的显示信息已经在列表中，pMalloc为NULL
*/
PSHOWINFO CShowManager::GetShowInfo(int nIndex, int nShowInfoType, PSHOWINFO *pMalloc)
{
	PSHOWINFO p = (PSHOWINFO)m_pInfoList->GetData(nIndex);
	if (!p)
	{
		p = CreateShowInfo(nShowInfoType);
		*pMalloc = p;
	}
	else
	{
		InitShowInfo(p);
		p->nType = nShowInfoType;
		*pMalloc = NULL;
	}

	return p;
}

PSHOWINFO CShowManager::MakeShowInfo(int nShowInfoType,
									 HDC hDC,
									 HFONT hFont,
									 LPCTSTR str,
									 int nLength,
									 long lWidth,
									 long lxOffset,
									 long lyOffset,
									 long *lHeight)
{
	SelectObject(hDC, hFont);

	PSHOWINFO pMalloc = NULL; // 是否是malloc的内容
	PSHOWINFO p = GetShowInfo(m_nInfoIndex, nShowInfoType, &pMalloc);
	if (!p)
	{
		ClearShowInfo(pMalloc);
		return NULL;
	}

	if (!SetStringShow(p, hDC, str, nLength, lWidth, lxOffset))
	{
		ClearShowInfo(pMalloc);
		return NULL;
	}
	
	p->dwShowStyle = m_dwShowStyle;
	if (p->lFirstYOff)
	{
		*lHeight += p->lFirstYOff;
	}
	p->lFirstYOff = lyOffset + *lHeight;
	p->hFont = hFont;

	return p;
}

BOOL CShowManager::MakeImageInfo(int nShowInfoType,
								 HDC hDC,
								 CDataList *pImageList,
								 long lWidth,
								 int nLeft,
								 long lyOffset,
								 long *lHeight)
{
	PSHOWINFO p1, p2;
	int i;
	int nImageWidth = 0; // 图片的总体宽度（包括标题宽度）
	int nImageHeight = 0; // 图片的总体高度（包括标题高度）
	int nTitleLeft = 0; // 标题距图片左侧的距离

	// 当一行中有多张图片时，需要控制插入的位置
	PSHOWINFO pList[IMAGE_MAXCOUNT * 3] = {0};
	int nImageInsert = 0; // 图片插入位置
	int nTitleInsert = 10; // 标题插入位置
	int nImageInLine = 0; // 单行的图片数量

	for (i = 0; i < pImageList->GetDataCount(); ++i)
	{
		_stdstring str;
		TCHAR sz[64] = {0}, format[64] = {0};
		PITEMIMAGE pImage = (PITEMIMAGE)pImageList->GetData(i);
		SIZE imgSize = {0}, titleSize = {0};

		// 计算图片大小
		if (!ReadImageSize(pImage->nImageType, pImage->pImageData, pImage->ulImageSize, &imgSize))
		{
			return FALSE;
		}

		// 计算图片标题的宽度
		LoadString(AfxGetInstanceHandle(), IDS_ITEM_IMAGETITLE, format, 64);
		_sntprintf(sz, 64, format, i + 1);
		str = sz;
		if (pImage->szImageTitle && lstrlen(pImage->szImageTitle))
		{
			LPCTSTR and = _T(" - ");
			_tcsncat(sz, and, 64);
			str += and;
			str += pImage->szImageTitle;
		}
		if (!GetTextExtentPoint(hDC, str.c_str(), str.length(), &titleSize))
		{
			return FALSE;
		}

		// 计算图片带标题的总高度和总宽度
		if (imgSize.cx >= titleSize.cx)
		{
			nImageWidth = imgSize.cx;
			nTitleLeft = (imgSize.cx - titleSize.cx) / 2;
		}
		else
		{
			nImageWidth = titleSize.cx;
			nTitleLeft = 0;
		}

		// 如果单行放不下了，改到下一行
		if (nLeft && nLeft + nImageWidth + GetMetrix(SHOWMETRIX_IMAGEGAP) > lWidth)
		{
			int j;
			for (j = 0; j < nImageInLine * 2; ++j)
			{
				pList[nImageInsert + nImageInLine + j] = pList[nTitleInsert + j];
				pList[nTitleInsert + j] = NULL;
			}
			nImageInLine = 0;
			nImageInsert = i * 3;
			nTitleInsert = (IMAGE_MAXCOUNT * 3 - nImageInsert) / 3 + nImageInsert;

			nLeft = 0;
			(*lHeight) += (nImageHeight + GetMetrix(SHOWMETRIX_DESCGAP));
			nImageHeight = 0;
		}

		// 计算新图片的高度
		if (imgSize.cy + titleSize.cy + GetMetrix(SHOWMETRIX_IMAGETITLEGAP) > nImageHeight)
		{
			nImageHeight = imgSize.cy + titleSize.cy + GetMetrix(SHOWMETRIX_IMAGETITLEGAP);
		}
		
		// 图片
		p1 = CreateShowInfo(nShowInfoType);
		if (!p1)
		{
			return FALSE;
		}
		p1->nIndex = i;
		p1->dwShowStyle = m_dwShowStyle;
		p1->pData = pImage;
		p1->lWidth = imgSize.cx;
		p1->lHeight = imgSize.cy;
		p1->lFirstXOff = nLeft;
		if (imgSize.cx < titleSize.cx)
		{
			p1->lFirstXOff += (titleSize.cx - imgSize.cx) / 2;
		}
		p1->lXOff = p1->lFirstXOff;
		p1->lFirstYOff = lyOffset + (*lHeight);
		pList[nImageInsert + nImageInLine] = p1;

		// 图片标号
		p2 = CreateShowInfo(SHOWTYPE_TEXTSPAN);
		if (!p2)
		{
			return FALSE;
		}
		if (!SetStringShow(p2, hDC, sz, lstrlen(sz), nImageWidth + nLeft, nLeft + nTitleLeft))
		{
			ClearShowInfo(p2);
			goto _MakeImageInfo_Fail;
		}
		p2->lFirstYOff = p1->lFirstYOff + p1->lHeight + GetMetrix(SHOWMETRIX_IMAGETITLEGAP);
		p2->bAlloc = TRUE;
		p2->pData = _tcsdup(sz);
		p2->hFont = m_pFont[0];
		pList[nTitleInsert + nImageInLine * 2] = p2;
		nTitleLeft += p2->lWidth;

		// 图片标题
		if (pImage->szImageTitle && lstrlen(pImage->szImageTitle))
		{
			p2 = CreateShowInfo(SHOWTYPE_TEXTDIVISION);
			if (!p2)
			{
				return FALSE;
			}
			if (!SetStringShow(p2, hDC, pImage->szImageTitle, lstrlen(pImage->szImageTitle),
				nImageWidth + nLeft, nLeft + nTitleLeft))
			{
				ClearShowInfo(p2);
				goto _MakeImageInfo_Fail;
			}
			p2->lFirstYOff = p1->lFirstYOff + p1->lHeight + GetMetrix(SHOWMETRIX_IMAGETITLEGAP);
			p2->hFont = m_pFont[0];
			pList[nTitleInsert + nImageInLine * 2 + 1] = p2;
		}

		++nImageInLine;
		nLeft += nImageWidth + GetMetrix(SHOWMETRIX_IMAGEGAP);
	}

	for (i = 0; i < IMAGE_MAXCOUNT * 3; ++i)
	{
		if (pList[i])
		{
			ClearShowInfo(m_pInfoList->GetData(m_nInfoIndex));
			m_pInfoList->SetData(m_nInfoIndex++, pList[i]);
		}
	}

	(*lHeight) += nImageHeight;
	return TRUE;

_MakeImageInfo_Fail:
	for (i = 0; i < IMAGE_MAXCOUNT * 3; ++i)
	{
		if (pList[i])
		{
			ClearShowInfo(pList[i]);
		}
	}
	return FALSE;
}

/*
功能：	设置字符串的显示信息
返回值：成功返回TRUE，否则返回FALSE
备注：	pRetList包含一个DWORD
		低16位表示行内文字数量（WORD）
		16-24位表示每段行首缩进的量（BYTE）
		24-32位表示行尾是否是换行符（BOOL）
		当pRetList为0时，代表用换行符产生的空行
		多个回车换行将被合并为一个换行
*/
BOOL CShowManager::SetStringShow(PSHOWINFO pInfo,
								 HDC hDC,
								 LPCTSTR lpStr,
								 int nStrLen,
								 long lWidth,
								 long lxOffset,
								 int nPCount /* = -1 */)
{
	int nIndex = 0, i, nFit = 0;
	long lHeight = 0;
	SIZE size = {0};
	long x = lxOffset, lyOffset = 0;
	DWORD dwRet = 0;
	long lLineGap; // 行间距
	int nIndent; // 行缩进
	int nRCount, nNCount; // 换行符的数量
	BOOL bNewParagraph = TRUE;
	int nParagraphCount = 0;
	int nRetIndex = 0; // 回车列表的位置
	
	// 行距
	if (m_pItem->GetType() == TYPE_BLANK && SHOWWNDTYPE_PROPERTY != m_nShowType)
	{
		lLineGap = GetMetrix(SHOWMETRIX_BLANKGAP);
		pInfo->lLineGap = lLineGap;
	}
	else
	{
		lLineGap = GetMetrix(SHOWMETRIX_LINEGAP);
		pInfo->lLineGap = 0;
	}

	// 处理字符串最开头的换行符
	for (nRCount = 0, nNCount = 0; nIndex < nStrLen; ++nIndex)
	{
		if (_T('\r') == lpStr[nIndex])
		{
			++nRCount;
		}
		else if (_T('\n') == lpStr[nIndex])
		{
			++nNCount;
		}
		else
		{
			break;
		}
	}
	if (nRCount + nNCount)
	{
		// 这里宽度是多少都无所谓，因为当行内全部都是非打印字符时，
		// size.cx必为0，而size.cy是有值的
		if (!GetTextExtentExPoint(hDC, lpStr, nRCount + nNCount, lWidth, &nFit, NULL, &size))
		{
			pInfo->pRetList->ClearData();
			return FALSE;
		}

		// 不论行开头有多少回车，只按一次换行处理
		pInfo->pRetList->SetData(nRetIndex++, 0);
		lHeight += size.cy + lLineGap;
	}

	while(1)
	{
		if (nIndex >= nStrLen ||
			(nPCount >= 0 && nParagraphCount >= nPCount))
		{
			int nCount = nRetIndex;

			// 当逐步显示时，一行也没有是可能的
			if (!nCount)
			{
				ASSERT(nPCount >= 0);
				pInfo->lLastXOff = lxOffset;
				break;
			}
			else if (1 == nCount)
			{
				pInfo->lWidth = size.cx;
				pInfo->lXOff = lxOffset;
				pInfo->lLastXOff = lxOffset + size.cx;
			}
			else
			{
				pInfo->lWidth = lWidth;
				pInfo->lXOff = 0;
				pInfo->lLastXOff = size.cx;
			}

			pInfo->pData = (PVOID)lpStr;
			pInfo->ulDataSize = nStrLen;
			pInfo->lHeight = lHeight;
			pInfo->lFirstXOff = lxOffset;
			pInfo->lFirstYOff = lyOffset;
			pInfo->lLastYOff = lHeight;
			if (!bNewParagraph)
			{
				pInfo->lLastYOff -= (size.cy + lLineGap);
			}
			break;
		}

		if (pInfo->nType == SHOWTYPE_TEXTPARAGRAPH)
		{
			if (bNewParagraph)
			{
				nIndent = GetMetrix(SHOWMETRIX_TEXTINDENT);
			}
			else
			{
				nIndent = 0;
			}
		}
		else
		{
			nIndent = 0;
		}

		if (!GetTextExtentExPoint(hDC, lpStr + nIndex, nStrLen - nIndex, lWidth - x - nIndent, &nFit, NULL, &size))
		{
			pInfo->pRetList->ClearData();
			return FALSE;
		}
		
		// 只有匹配了一定数量的字符，才能记录本行内容
		if (nFit)
		{
			BOOL bFindNewLine = FALSE;

			// 检查匹配行的开头
			// 如果有换行，则进行处理
			for (i = nIndex, nRCount = 0, nNCount = 0; i < nIndex + nFit; ++i)
			{
				// 遇到\n视为一个换行
				if (_T('\n') == lpStr[i])
				{
					bFindNewLine = TRUE;
					++nNCount;
				}
				else if (_T('\r') == lpStr[i])
				{
					bFindNewLine = TRUE;
					++nRCount;
				}
				else
				{
					// 截止到每行末尾的回车符
					if (bFindNewLine)
					{
						break;
					}
				}
			}
			// 到这里，字符串被截断
			// 如果行末尾有回车，则统计回车的数量
			
			// 由于换行不会被实际输出，所以为了适配文字复制
			// 所有换行都包含在文字行内
			dwRet = (i - nIndex) & 0xFFFFul;
			dwRet |= ((nIndent << 16) & 0xFF0000ul);

			// 括号内代表行的末尾有换行符
			bNewParagraph = (nNCount + nRCount > 0);
			if (bNewParagraph)
			{
				dwRet |= 0x01000000ul;
				++nParagraphCount;
			}
			
			// 记录当前行的信息
			pInfo->pRetList->SetData(nRetIndex++, (PVOID)dwRet);
			nIndex = i;

			lHeight += size.cy + lLineGap;
		}
		else
		{
			//ASSERT(!pInfo->pRetList->GetDataCount());
			lxOffset = 0;
			lyOffset += size.cy + lLineGap;
		}

		// 从下一行开始了
		x = 0;
	}

	for (i = pInfo->pRetList->GetDataCount() - nRetIndex; i > 0; --i)
	{
		pInfo->pRetList->DeleteData(nRetIndex);
	}
	return TRUE;
}

long CShowManager::SetItemShow(HDC hDC,
							   long lyOffset,
							   long lWidth,
							   DWORD dwShowStyle,
							   int nIndex,
							   int nStep) // 当为题目组时，nStep用于存储是否为单题显示
{
	LOGFONT lf = {0};
	GetObject(m_pFont[0], sizeof(LOGFONT), &lf);
	m_nFontSize = lf.lfHeight;
	if (m_nFontSize < 0)
	{
		m_nFontSize *= -1;
	}

	if (!m_pInfoList)
	{
		m_pInfoList = new CDataList;
		if (!m_pInfoList)
		{
			return 0;
		}
	}

	if (m_pItem->GetType() == TYPE_GROUP)
	{
		dwShowStyle &= ~(SHOWSTYLE_WITHINDEX);
	}
	m_nShowIndex = nIndex;
	m_nShowStep = nStep;
	m_dwShowStyle = dwShowStyle;

	s_lBlankSampleSize = GetBlankSampleSize(hDC);
	s_lTextSampleSize = GetTextSampleSize(hDC);

	return RecalcItemShow(hDC, lyOffset, lWidth);
}

/*
功能：	重新计算显示尺寸
返回值：当前题目的显示高度
备注：	仅重新计算改动后的新高度，内容不做修改
*/
long CShowManager::RecalcItemShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0;
	HFONT hOldFont = (HFONT)SelectObject(hDC, m_pFont[0]);
	int i;
	
	m_nInfoIndex = 0;
	switch(m_pItem->GetType())
	{
	case TYPE_GROUP:
		// 当为题目组时，m_nShowStep保存的是是否为单题显示
		if (m_nShowType == SHOWWNDTYPE_TEST ||
			m_nShowType == SHOWWNDTYPE_TESTVIEW)
		{
			lRet += SetGroupShow(hDC, lyOffset + lRet, lWidth);
			lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		}
		lRet += SetDescriptionShow(hDC, lyOffset + lRet, lWidth);
		break;

	case TYPE_BLANK:
		break;

	default:
		lRet += SetDescriptionShow(hDC, lyOffset + lRet, lWidth);
		break;
	}
	
	switch(m_pItem->GetType())
	{
	case TYPE_SCHOISE:
	case TYPE_MCHOISE:
		lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		lRet += SetOptionsShow(hDC, lyOffset + lRet, lWidth);
		break;
		
	case TYPE_JUDGE:
		lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		lRet += SetJudgeShow(hDC, lyOffset + lRet, lWidth);
		break;
		
	case TYPE_BLANK:
		lRet = SetBlanksShow(hDC, lyOffset, lWidth);
		break;
		
	case TYPE_TEXT:
		lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		lRet += SetTextShow(hDC, lyOffset + lRet, lWidth);
		break;
	}
	
	if ((m_dwShowStyle & SHOWSTYLE_WITHRESOLVE) &&
		(lstrlen(m_pItem->CItem::GetResolve()) || m_pItem->GetImageCount(ITEMIMAGEFLAG_RESOLVE)))
	{
		lRet += GetMetrix(SHOWMETRIX_RESOLVEGAP);
		lRet += SetResolveShow(hDC, lyOffset + lRet, lWidth);
	}
	
	SelectObject(hDC, hOldFont);
	
	m_lHeight = lRet;

	// 清理多余的显示信息
	for (i = m_pInfoList->GetDataCount() - m_nInfoIndex; i > 0; --i)
	{
		ClearShowInfo(m_pInfoList->GetData(m_nInfoIndex));
		m_pInfoList->DeleteData(m_nInfoIndex);
	}

	m_lWidth = lWidth;
	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		PSHOWINFO p = (PSHOWINFO)m_pInfoList->GetData(i);
		m_lWidth = max(m_lWidth, p->lWidth);
	}
	
	return lRet;
}

long CShowManager::AddOffset(long lyOffset)
{
	int i;
	PSHOWINFO pInfo;
	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		pInfo = (PSHOWINFO)m_pInfoList->GetData(i);
		if (!pInfo)
		{
			return 0;
		}

		pInfo->lFirstYOff += lyOffset;
	}

	return m_lHeight;
}

long CShowManager::SetDescriptionShow(HDC hDC,
									  long lyOffset,
									  long lWidth)
{
	long lRet = 0, lImageHeight = 0;
	TCHAR sz[64] = {0};
	int nIcon = 0; // 收藏标记大小（为0则没有）
	int nLeft = 0;
	PSHOWINFO p = NULL;
	LPCTSTR pDesc = NULL;
	SIZE size = {0};
	UINT uTextType;

	SelectObject(hDC, m_pFont[2]);
	_sntprintf(sz, 64, _T("%d. "), m_nShowIndex);
	if (!GetTextExtentPoint(hDC, sz, lstrlen(sz), &size))
	{
		return 0;
	}

	// 题干序号
	if (m_dwShowStyle & SHOWSTYLE_WITHINDEX)
	{
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], sz, lstrlen(sz),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nIndex = m_nShowIndex;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(sz);
		m_pInfoList->SetData(m_nInfoIndex++, p);

		nLeft += p->lWidth;
	}

	// 收藏标记
	if (m_pItem->GetType() != TYPE_GROUP)
	{
		uTextType = SHOWTYPE_TEXTDIVISION;
		if (m_dwShowStyle & SHOWSTYLE_WITHSTORE && m_pItem->GetItemFrom()->IsStored())
		{
			nLeft += size.cy + GetMetrix(SHOWMETRIX_ICONGAP);
			nIcon = size.cy;
		}
		else if (m_pItem->GetItemState(ITEMMARK_UNCERTAIN))
		{
			nLeft += size.cy + GetMetrix(SHOWMETRIX_ICONGAP);
			nIcon = size.cy;
		}
	}
	else
	{
		uTextType = SHOWTYPE_TEXTPARAGRAPH;
	}
	
	pDesc = m_pItem->GetDescription();
	if (lstrlen(pDesc))
	{
		p = MakeShowInfo(uTextType, hDC, m_pFont[0], pDesc, lstrlen(pDesc),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nIndex = m_nShowIndex;
		m_pInfoList->SetData(m_nInfoIndex++, p);
	}
	// 可能没有题干的情况
	else
	{
		p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[0], sz, lstrlen(sz),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->pData = NULL;
		p->ulDataSize = 0;
		p->nIndex = m_nShowIndex;
		m_pInfoList->SetData(m_nInfoIndex++, p);
	}
	lRet += p->lHeight;
	p->nIcon = nIcon;

	nLeft = 0;
	if (m_pItem->GetImageCount(ITEMIMAGEFLAG_DESC))
	{
		lRet += GetMetrix(SHOWMETRIX_DESCGAP);
	}

	// 图片
	if (!MakeImageInfo(SHOWTYPE_IMAGE, hDC, m_pItem->GetImageList(ITEMIMAGEFLAG_DESC),
		lWidth, nLeft, lyOffset, &lRet))
	{
		return 0;
	}
	
	return lRet + lImageHeight;
}

long CShowManager::SetOptionsShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0;
	int i;
	PSHOWINFO p;
	CChoise *pChoise = (CChoise*)m_pItem;
	int nLeft;
	BOOL bShowAns = FALSE;

	if (m_dwShowStyle & SHOWSTYLE_WITHANSWER && m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
	{
		bShowAns = TRUE;
	}

	for (i = 0; i < pChoise->GetOptionCount(); ++i)
	{
		HFONT hFont;
		if (bShowAns)
		{
			hFont = m_pFont[0];
		}
		else if (((m_dwShowStyle & SHOWSTYLE_WITHANSWER) && pChoise->GetAnswer(i)) ||
				 ((m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER) && pChoise->GetUserAnswer(i)))
		{
			hFont = m_pFont[2];
		}
		else
		{
			hFont = m_pFont[0];
		}

		// 选项号
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_OPTIONINDEX, hDC, hFont, s_optionIndex[i], lstrlen(s_optionIndex[i]),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = i;
		m_pInfoList->SetData(m_nInfoIndex++, p);

		nLeft = p->lLastXOff;
		p = MakeShowInfo(SHOWTYPE_OPTION, hDC, hFont, pChoise->GetOption(i), lstrlen(pChoise->GetOption(i)),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = i;
		m_pInfoList->SetData(m_nInfoIndex++, p);

		lRet += p->lHeight + GetMetrix(SHOWMETRIX_DESCGAP);
	}
	lRet -= GetMetrix(SHOWMETRIX_DESCGAP);

	// 显示答案
	if (bShowAns)
	{
		_stdstring str;

		lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_UserAnswer, lstrlen(s_UserAnswer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		m_pInfoList->SetData(m_nInfoIndex++, p);

		pChoise->MakeUserAnswer(str);
		nLeft = p->lLastXOff;
		p = MakeShowInfo(SHOWTYPE_OPTIONANS, hDC, m_pFont[0], str.c_str(), str.length(),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 1;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(str.c_str());
		m_pInfoList->SetData(m_nInfoIndex++, p);

		lRet += p->lHeight + GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Answer, lstrlen(s_Answer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		m_pInfoList->SetData(m_nInfoIndex++, p);

		pChoise->MakeAnswer(str, 0);
		nLeft = p->lLastXOff;
		p = MakeShowInfo(SHOWTYPE_OPTIONANS, hDC, m_pFont[0], str.c_str(), str.length(),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 2;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(str.c_str());
		m_pInfoList->SetData(m_nInfoIndex++, p);

		lRet += p->lHeight;
	}

	return lRet;
}

long CShowManager::SetJudgeShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0;
	int i, nLeft = 0;
	PSHOWINFO p;
	CJudge *pJudge = (CJudge*)m_pItem;
	BOOL bShowAns = FALSE;

	if (m_dwShowStyle & SHOWSTYLE_WITHANSWER && m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
	{
		bShowAns = TRUE;
	}

	for (i = 0; i < 2; ++i)
	{
		HFONT hFont;
		if (bShowAns)
		{
			hFont = m_pFont[0];
		}
		else if ((pJudge->GetAnswer() == JUDGEANS_TRUE && m_dwShowStyle & SHOWSTYLE_WITHANSWER) ||
				 (pJudge->GetUserAnswer() == JUDGEANS_TRUE && m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER))
		{
			hFont = (0 == i) ? m_pFont[2] : m_pFont[0];
		}
		else if ((pJudge->GetAnswer() == JUDGEANS_FALSE && m_dwShowStyle & SHOWSTYLE_WITHANSWER) ||
				 (pJudge->GetUserAnswer() == JUDGEANS_FALSE && m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER))
		{
			hFont = (1 == i) ? m_pFont[2] : m_pFont[0];
		}
		else
		{
			hFont = m_pFont[0];
		}
		
		p = MakeShowInfo(SHOWTYPE_OPTION, hDC, hFont, s_Judge[i], lstrlen(s_Judge[i]),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		if (0 == i)
		{
			p->nInfo = TRUE;
		}
		else
		{
			p->nInfo = FALSE;
		}
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		lRet += p->lHeight + GetMetrix(SHOWMETRIX_DESCGAP);
	}
	lRet -= GetMetrix(SHOWMETRIX_DESCGAP);

	// 显示答案
	if (bShowAns)
	{
		_stdstring str;
		
		lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_UserAnswer, lstrlen(s_UserAnswer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 0;
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		pJudge->MakeUserAnswer(str);
		nLeft = p->lLastXOff;
		p = MakeShowInfo(SHOWTYPE_JUDGEANS, hDC, m_pFont[0], str.c_str(), str.length(),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 1;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(str.c_str());
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		lRet += p->lHeight + GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Answer, lstrlen(s_Answer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 0;
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		pJudge->MakeAnswer(str, 0);
		nLeft = p->lLastXOff;
		p = MakeShowInfo(SHOWTYPE_JUDGEANS, hDC, m_pFont[0], str.c_str(), str.length(),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 2;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(str.c_str());
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		lRet += p->lHeight;
	}
	
	return lRet;
}

long CShowManager::GetBlankSampleSize(HDC hDC)
{
	TCHAR sz[64] = {0};
	SIZE size = {0};
	
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_BLANKSAMPLE, sz, 64);
	if (!GetTextExtentPoint(hDC, sz, lstrlen(sz), &size))
	{
		return 0;
	}

	return size.cx;
}

long CShowManager::SetBlanksShow(HDC hDC,
								 long lyOffset,
								 long lWidth)
{
	long lRet = 0;
	CBlank *pBlank = (CBlank*)m_pItem;
	PSHOWINFO p;

	int i = 0, j = 0; // 变量
	int nStrLen, nPtr = 0; // 每段题干的长度和起始位置（注意，nPtr只能用于题目描述）
	int nLeft = 0; // 字符串从左侧何处开始
	int nIconSize = 0; // 标记或收藏图表的大小
	LPCTSTR pDesc, pBlkText;
	SIZE size = {0};
	TCHAR sz[64] = {0};
	int nBlankLen;
	BOOL bShowAns = FALSE; // 同时显示用户答案与正确答案
	
	if (m_dwShowStyle & SHOWSTYLE_WITHANSWER && m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
	{
		bShowAns = TRUE;
	}
	
	SelectObject(hDC, m_pFont[2]);
	_sntprintf(sz, 64, _T("%d. "), m_nShowIndex);
	if (!GetTextExtentPoint(hDC, sz, lstrlen(sz), &size))
	{
		return 0;
	}
	nIconSize = size.cy;

	if (m_dwShowStyle & SHOWSTYLE_WITHINDEX)
	{
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], sz, lstrlen(sz),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nIndex = m_nShowIndex;
		p->bAlloc = TRUE;
		p->pData = _tcsdup(sz);
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		nLeft += p->lWidth;
	}

	// 收藏标记
	if (m_dwShowStyle & SHOWSTYLE_WITHSTORE && m_pItem->GetItemFrom()->IsStored())
	{
		nLeft += nIconSize + GetMetrix(SHOWMETRIX_ICONGAP);
	}
	else if (m_pItem->GetItemState(ITEMMARK_UNCERTAIN))
	{
		nLeft += nIconSize + GetMetrix(SHOWMETRIX_ICONGAP);
	}
	else
	{
		nIconSize = 0;
	}

	// 只有在显示属性时，会按照原本内容显示
	if (SHOWWNDTYPE_PROPERTY == m_nShowType)
	{
		_stdstring ss;
		LPTSTR pStr;

		pBlank->MakeAnswer(ss, MKASW_ASWINQUESTION, -1);
		pStr = _tcsdup(ss.c_str());
		if (!pStr)
		{
			return 0;
		}

		p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[0], pStr, lstrlen(pStr),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			free(pStr);
			return 0;
		}
		p->nIcon = nIconSize;
		p->bAlloc = TRUE;
		m_pInfoList->SetData(m_nInfoIndex++, p);

		lRet += p->lHeight;
	}
	else
	{
		pDesc = pBlank->GetDescription();
		// 每次循环均添加一个空白的前部分内容和空白本身
		for (i = 0; i < pBlank->GetBlankCount(); ++i)
		{
			// 每个空白的起始和结束位置
			int nStart, nEnd;
			HFONT hFont;
			BOOL bHide = FALSE; // 是否隐藏文字

			// 获取空白的起始
			pBlank->GetBlankRange(i, &nStart, &nEnd);

			// 每个空白至前一个空白之间的字符串长度
			// 注意，这里取得的长度不包含括号
			nStrLen = CTextManager::GetRealIndex(pDesc, nStart - 1) - nPtr;

			// 空白和空白之间可能没有内容
			if (nStrLen)
			{
				// 将空白前面的部分添加进显示区域中
				p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[0], pDesc + nPtr, nStrLen,
					lWidth, nLeft, lyOffset, &lRet);
				if (!p)
				{
					return 0;
				}
				p->nIcon = nIconSize;
				nIconSize = 0;

				p->nInfo = 0;
				m_pInfoList->SetData(m_nInfoIndex++, (PVOID)p);

				if ((int)(p->pRetList->GetData(p->pRetList->GetDataCount() - 1)) & 0xFF000000ul)
				{
					nLeft = 0;
				}
				else
				{
					nLeft = p->lLastXOff;
				}
				// 指向最后一行的顶部
				lRet += p->lLastYOff;
			}
			
			// 检查空白的长度，不可短于最小值
			if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
			{
				pBlkText = pBlank->GetUserAnswer(i);
				if (!pBlkText || !lstrlen(pBlkText))
				{
					hFont = m_pFont[0];
					if (bShowAns ||
						SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
					{
						pBlkText = s_NoAnswer;
					}
					else
					{
						pBlkText = pBlank->GetBlank(i);
						bHide = TRUE;
					}
				}
				else if (bShowAns)
				{
					if (!_tcscmp(pBlank->GetBlank(i), pBlkText))
					{
						hFont = m_pFont[2];
					}
					else
					{
						hFont = m_pFont[0];
					}
				}
				else
				{
					hFont = m_pFont[0];
				}
			}
			else
			{
				pBlkText = pBlank->GetBlank(i);
				hFont = m_pFont[0];

				// 当分步显示时，不显示所有空白
				if (m_nShowStep >= 0 && i + 1 > m_nShowStep)
				{
					bHide = TRUE;
				}
			}
			SelectObject(hDC, hFont);

			if (!GetTextExtentPoint(hDC, pBlkText, lstrlen(pBlkText), &size))
			{
				return 0;
			}
			nBlankLen = size.cx;

			// 当一行中用于显示空白的区域不够长时，将自动换到下一行显示
			// 注意，这里仅计算最小空白长度，即足够显示编辑框即可
			if (nLeft + s_lBlankSampleSize > lWidth)
			{
				nLeft = 0;
				lRet += (size.cy + GetMetrix(SHOWMETRIX_BLANKGAP));
			}
			// 当答案长度短于最小长度时
			// 答案前后会有一段空白

			// 填空题答案
			p = MakeShowInfo(SHOWTYPE_BLANK, hDC, hFont, pBlkText, lstrlen(pBlkText),
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			p->nIcon = nIconSize;
			nLeft = p->lLastXOff;
			// 当空白长度过短
			if (nBlankLen < s_lBlankSampleSize)
			{
				p->lFirstTxtXOff = (s_lBlankSampleSize - nBlankLen) / 2;
				p->lWidth = s_lBlankSampleSize;
				nLeft += (s_lBlankSampleSize - nBlankLen);
			}
			else
			{
				p->lFirstTxtXOff = 0;
			}
			p->nInfo = i;
			p->bHide = bHide;
			m_pInfoList->SetData(m_nInfoIndex++, p);

			nPtr = CTextManager::GetRealIndex(pDesc, nEnd + 1);

			// 空白后面有换行，则从头开始
			if (pDesc[nPtr] == _T('\r') || pDesc[nPtr] == _T('\n'))
			{
				lRet += p->lHeight;
				nLeft = 0;
				while(pDesc[nPtr] == _T('\r') || pDesc[nPtr] == _T('\n'))
				{
					++nPtr;
				}
			}
			else
			{
				lRet += p->lLastYOff;
			}

			nIconSize = 0;
		}

		// 最后一个空白后面还有内容
		if (nPtr < lstrlen(pDesc))
		{
			// 将最后一段内容加入
			p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[0], pDesc + nPtr, lstrlen(pDesc) - nPtr,
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			m_pInfoList->SetData(m_nInfoIndex++, p);
			lRet += p->lHeight;
		}
		else
		{
			p->nType = SHOWTYPE_BLANKDIVISION;
			lRet += (p->lHeight / p->pRetList->GetDataCount());
		}

		// 同时显示用户答案与正确答案：
		if (bShowAns)
		{
			nLeft = 0;
			p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Answer, lstrlen(s_Answer),
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			p->nInfo = 1;
			m_pInfoList->SetData(m_nInfoIndex++, p);
			lRet += p->lLastYOff;
			nLeft = p->lLastXOff;

			for (i = 0; i < pBlank->GetBlankCount(); ++i)
			{
				pBlkText = pBlank->GetBlank(i);
				if (!GetTextExtentPoint(hDC, pBlkText, lstrlen(pBlkText), &size))
				{
					return 0;
				}
				nBlankLen = size.cx;
				if (nLeft + s_lBlankSampleSize > lWidth)
				{
					nLeft = 0;
					lRet += (size.cy + GetMetrix(SHOWMETRIX_BLANKGAP));
				}

				p = MakeShowInfo(SHOWTYPE_BLANKANS, hDC, m_pFont[0], pBlkText, lstrlen(pBlkText),
					lWidth, nLeft, lyOffset, &lRet);
				if (!p)
				{
					return 0;
				}
				p->nInfo = i;
				m_pInfoList->SetData(m_nInfoIndex++, p);

				nLeft = p->lLastXOff;
				if (nBlankLen < s_lBlankSampleSize)
				{
					p->lFirstTxtXOff = (s_lBlankSampleSize - nBlankLen) / 2;
					nLeft += (s_lBlankSampleSize - nBlankLen);
				}
				lRet += p->lLastYOff;
				nLeft += GetMetrix(SHOWMETRIX_BLANKANSGAP);
			}

			lRet += p->lHeight;
		}
	}

	return lRet + GetMetrix(SHOWMETRIX_LINEGAP);
}

long CShowManager::GetTextSampleSize(HDC hDC)
{
	TCHAR sz[64] = {0};
	SIZE size = {0};
	
	LoadString(AfxGetInstanceHandle(), IDS_ITEM_BLANKSAMPLE, sz, 64);
	if (!GetTextExtentPoint(hDC, sz, lstrlen(sz), &size))
	{
		return 0;
	}
	
	return size.cy * 6;
}

long CShowManager::SetTextShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0;
	CText *pText = (CText*)m_pItem;
	SIZE size = {0};
	int nLeft = 0;
	LPCTSTR pAnswer;
	PSHOWINFO p;
	BOOL bHide = FALSE;

	// 当正确答案和用户答案同时显示、或在评分界面时，显示前面的“答案”字样
	// 否则不显示
	if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
	{
		if (m_dwShowStyle & SHOWSTYLE_WITHANSWER ||
			SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
		{
			p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_UserAnswer, lstrlen(s_UserAnswer),
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			p->nInfo = 0;
			m_pInfoList->SetData(m_nInfoIndex++, p);
			
			nLeft = p->lLastXOff;

			pAnswer = pText->GetUserAnswer();
			if (!pAnswer || !lstrlen(pAnswer))
			{
				pAnswer = s_NoAnswer;
			}
		}
		else
		{
			pAnswer = pText->GetUserAnswer();
			if (!pAnswer || !lstrlen(pAnswer))
			{
				pAnswer = s_NoAnswer;
				bHide = TRUE;
			}
		}

		p = MakeShowInfo(SHOWTYPE_TEXTANS, hDC, m_pFont[0], pAnswer, lstrlen(pAnswer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 1;
		p->bHide = bHide;
		m_pInfoList->SetData(m_nInfoIndex++, p);
		lRet += p->lHeight;
		
		if (m_dwShowStyle & SHOWSTYLE_WITHANSWER)
		{
			TCHAR score[32] = {0};
			int nHeight;

			lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);

			// 显示得分
			nLeft = 0;
			p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Score, lstrlen(s_Score),
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			p->nInfo = 0;
			m_pInfoList->SetData(m_nInfoIndex++, p);
			nHeight = p->lHeight;

			nLeft = p->lLastXOff;
			_sntprintf(score, 32, _T("%d"), pText->GetPoint());
			p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[0], score, lstrlen(score),
				lWidth, nLeft, lyOffset, &lRet);
			p->nInfo = 0;
			p->pData = _tcsdup(score);
			p->bAlloc = TRUE;
			m_pInfoList->SetData(m_nInfoIndex++, p);

			lRet += max(nHeight, p->lHeight);
		}
		else
		{
			// 仅显示用户答案时，才会有最小高度
			if (lRet < s_lTextSampleSize)
			{
				lRet = s_lTextSampleSize;
				p->lHeight = s_lTextSampleSize;
			}
		}
	}
	
	if (m_dwShowStyle & SHOWSTYLE_WITHANSWER)
	{
		int lHeight = 0; // 用来保存“答案：”字样的高度

		if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER)
		{
			lRet += GetMetrix(SHOWMETRIX_AFTERDESCGAP);
		}
		
		nLeft = 0;
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Answer, lstrlen(s_Answer),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		p->nInfo = 0;
		m_pInfoList->SetData(m_nInfoIndex++, p);
		lHeight = p->lHeight;

		nLeft = p->lLastXOff;
		pAnswer = pText->GetAnswer();

		// 如果是逐步显示
		if (m_nShowStep >= 0)
		{
			PSHOWINFO pMalloc = NULL;

			SelectObject(hDC, m_pFont[0]);
			
			p = GetShowInfo(m_nInfoIndex, SHOWTYPE_TEXT, &pMalloc);
			if (!p)
			{
				return NULL;
			}
			if (!SetStringShow(p, hDC, pAnswer, lstrlen(pAnswer), lWidth, nLeft, m_nShowStep))
			{
				ClearShowInfo(pMalloc);
				return NULL;
			}

			if (p->pRetList->GetDataCount())
			{
				p->dwShowStyle = m_dwShowStyle;
				if (p->lFirstYOff)
				{
					lRet += p->lFirstYOff;
				}
				p->lFirstYOff = lyOffset + lRet;
				p->hFont = m_pFont[0];
			}
			// 如果一个段落也没有
			// 为了防止被误认为是图片，应该删除
			else
			{
				ClearShowInfo(pMalloc);
				p = NULL;
			}
		}
		else
		{
			p = MakeShowInfo(SHOWTYPE_TEXT, hDC, m_pFont[0], pAnswer, lstrlen(pAnswer),
				lWidth, nLeft, lyOffset, &lRet);
			if (!p)
			{
				return 0;
			}
			p->nInfo = 2;
		}

		if (p)
		{
			m_pInfoList->SetData(m_nInfoIndex++, p);
			lRet += p->lHeight;
		}
		else
		{
			// 在逐步显示时，如果一段都没有显示
			// 则高度为“答案：”字样的高度
			lRet += lHeight;
		}
	}
	
	return lRet;
}

long CShowManager::SetGroupShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0;
	PSHOWINFO p;
	CString str;
	CItemGroup *pGroup = (CItemGroup*)m_pItem;
	int nLeft = 0;

	// 当为单题显示时，不显示顶部内容
	if (m_nShowStep >= 0 || !lstrlen(pGroup->GetDescription()))
	{
		return lRet;
	}
	else if (pGroup->GetItemCount() > 1)
	{
		int nEnd = m_nShowIndex + pGroup->GetItemCount() - 1;
		str.Format(IDS_TEST_GROUPCAUTIONM, m_nShowIndex, nEnd);
	}
	else
	{
		str.Format(IDS_TEST_GROUPCAUTIONS, m_nShowIndex);
	}

	p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[2], str, str.GetLength(),
		lWidth, nLeft, lyOffset, &lRet);
	if (!p)
	{
		return 0;
	}
	p->pData = _tcsdup(str);
	p->bAlloc = TRUE;
	m_pInfoList->SetData(m_nInfoIndex++, p);

	return lRet + p->lHeight;
}

long CShowManager::SetResolveShow(HDC hDC, long lyOffset, long lWidth)
{
	long lRet = 0, lImageHeight = 0;
	PSHOWINFO p;
	LPCTSTR pRes;
	int nLeft = 0;

	pRes = m_pItem->CItem::GetResolve();
	if (pRes && lstrlen(pRes))
	{
		p = MakeShowInfo(SHOWTYPE_TEXTSPAN, hDC, m_pFont[2], s_Resolve, lstrlen(s_Resolve),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		m_pInfoList->SetData(m_nInfoIndex++, p);
		nLeft = p->lWidth;

		p = MakeShowInfo(SHOWTYPE_TEXTDIVISION, hDC, m_pFont[0], pRes, lstrlen(pRes),
			lWidth, nLeft, lyOffset, &lRet);
		if (!p)
		{
			return 0;
		}
		m_pInfoList->SetData(m_nInfoIndex++, p);
		
		if (p->lHeight > lRet)
		{
			lRet = p->lHeight;
		}
	}
	else
	{
		lRet = 0;
	}

	// 图片
	if (!MakeImageInfo(SHOWTYPE_IMAGE, hDC, m_pItem->GetImageList(ITEMIMAGEFLAG_RESOLVE),
		lWidth, nLeft, lyOffset, &lRet))
	{
		return 0;
	}
	
	return lRet;
}

void CShowManager::DrawItem(HDC hDC, int nXScroll, int nYScroll, BOOL bFocus)
{
	int nCurShow = 0;
	PSHOWINFO pShow = NULL;
	HFONT hOldFont = (HFONT)SelectObject(hDC, m_pFont[0]);
	COLORREF clrOld = SetTextColor(hDC,
		AfxGetQColor(bFocus ? QCOLOR_TEXTFOCUS : QCOLOR_TEXT));
	DWORD dwDrawStyle = bFocus ? DRAWSTYLE_FOCUS : 0;

	if (bFocus)
	{
		HBRUSH hBrush = CreateSolidBrush(AfxGetQColor(QCOLOR_HIGHLIGHT));
		PSHOWINFO pShow = (PSHOWINFO)m_pInfoList->GetData(nCurShow);
		RECT rc = {0};

		if (!pShow)
		{
			return;
		}
		rc.top = pShow->lFirstYOff - nYScroll;
		rc.bottom = rc.top + m_lHeight;
		rc.left = 0;
		rc.right = rc.right + m_lWidth;

		FillRect(hDC, &rc, hBrush);
		DeleteObject(hBrush);
	}

	while(1)
	{
		pShow = (PSHOWINFO)m_pInfoList->GetData(nCurShow);
		if (!pShow)
		{
			break;
		}

		switch(pShow->nType)
		{
		case SHOWTYPE_TEXTSPAN:
		case SHOWTYPE_TEXTDIVISION:
			if (pShow->nIcon)
			{
				DrawStore(hDC, pShow->lFirstXOff - pShow->nIcon - GetMetrix(SHOWMETRIX_ICONGAP) - nXScroll,
					pShow->lFirstYOff - nYScroll, pShow->nIcon);
			}
			DrawString(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		case SHOWTYPE_IMAGE:
			DrawImage(hDC, pShow, nXScroll, nYScroll);
			break;

		case SHOWTYPE_TEXTPARAGRAPH:
		case SHOWTYPE_OPTIONINDEX:
		case SHOWTYPE_OPTION:
		case SHOWTYPE_TEXT:
			DrawString(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		case SHOWTYPE_OPTIONANS:
			DrawOptionAns(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		case SHOWTYPE_JUDGEANS:
			DrawJudgeAns(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		case SHOWTYPE_BLANK:
		case SHOWTYPE_BLANKANS:
		case SHOWTYPE_BLANKDIVISION:
			DrawBlank(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		case SHOWTYPE_TEXTANS:
			DrawText(hDC, nCurShow, nXScroll, nYScroll, dwDrawStyle);
			break;

		default:
			ASSERT(FALSE);
		}

		++nCurShow;
	}

	SelectObject(hDC, hOldFont);
	SetTextColor(hDC, clrOld);
}

void CShowManager::DrawOptionAns(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle)
{
	PSHOWINFO pShow;
	CChoise *pChoise = (CChoise*)m_pItem;
	
	pShow = (PSHOWINFO)(m_pInfoList->GetData(nCurShow));
	if (!pShow)
	{
		return;
	}

	if (0 == pShow->nInfo)
	{
		DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
	}
	else
	{
		COLORREF clrOld = GetTextColor(hDC);

		if (1 == pShow->nInfo)
		{
			switch(pChoise->CheckAnswer())
			{
			case ALLRIGHT:
				SetTextColor(hDC, TESTVIEW_GREEN);
				break;
				
			case HALFRIGHT:
				SetTextColor(hDC, TESTVIEW_ORANGE);
				break;
				
			case WRONG:
				SetTextColor(hDC, TESTVIEW_RED);
				break;
			}
		}
		
		DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
		SetTextColor(hDC, clrOld);
	}
}

void CShowManager::DrawJudgeAns(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle)
{
	PSHOWINFO pShow;
	CJudge *pJudge = (CJudge*)m_pItem;

	pShow = (PSHOWINFO)(m_pInfoList->GetData(nCurShow));
	if (!pShow)
	{
		return;
	}

	if (0 == pShow->nInfo)
	{
		DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
	}
	else
	{
		COLORREF clrOld = GetTextColor(hDC);

		if (1 == pShow->nInfo)
		{
			switch(pJudge->CheckAnswer())
			{
			case ALLRIGHT:
				SetTextColor(hDC, TESTVIEW_GREEN);
				break;
				
			case HALFRIGHT:
				SetTextColor(hDC, TESTVIEW_ORANGE);
				break;
				
			case WRONG:
				SetTextColor(hDC, TESTVIEW_RED);
				break;
			}
		}
		
		DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
		SetTextColor(hDC, clrOld);
	}
}

void CShowManager::DrawBlank(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle)
{
	PSHOWINFO pShow;
	CBlank *pBlank = (CBlank*)m_pItem;
	COLORREF clrOld = GetTextColor(hDC);

	pShow = (PSHOWINFO)(m_pInfoList->GetData(nCurShow));
	if (!pShow)
	{
		return;
	}

	if (pShow->nIcon)
	{
		DrawStore(hDC, pShow->lFirstXOff - pShow->nIcon - GetMetrix(SHOWMETRIX_ICONGAP) - nXScroll,
			pShow->lFirstYOff - nYScroll + GetMetrix(SHOWMETRIX_BLANKGAP), pShow->nIcon);
	}

	dwStyle |= DRAWSTYLE_UNDERLINE;
	if (SHOWWNDTYPE_SUBJECTIVEVIEW == m_nShowType)
	{
		if (!pBlank->GetUserAnswerCheck(pShow->nInfo))
		{
			dwStyle |= DRAWSTYLE_DELETELINE;
		}
	}
	else if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER && m_dwShowStyle & SHOWSTYLE_WITHANSWER)
	{
		if (pShow->nType == SHOWTYPE_BLANKANS)
		{
			SetTextColor(hDC, clrOld);
		}
		else
		{
			if (pBlank->GetUserAnswerCheck(pShow->nInfo))
			{
				SetTextColor(hDC, TESTVIEW_GREEN);
			}
			else
			{
				SetTextColor(hDC, TESTVIEW_RED);
			}
		}
	}

	DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
	SetTextColor(hDC, clrOld);
}

void CShowManager::DrawText(HDC hDC, int nCurShow, int nXScroll, int nYScroll, DWORD dwStyle)
{
	PSHOWINFO pShow;
	CText *pText = (CText*)m_pItem;
	COLORREF clrOld = GetTextColor(hDC);

	pShow = (PSHOWINFO)(m_pInfoList->GetData(nCurShow));
	if (!pShow)
	{
		return;
	}

	if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER && m_dwShowStyle & SHOWSTYLE_WITHANSWER)
	{
		switch(pText->CheckAnswer())
		{
		case ALLRIGHT:
			SetTextColor(hDC, TESTVIEW_GREEN);
			break;

		case HALFRIGHT:
			SetTextColor(hDC, TESTVIEW_ORANGE);
			break;

		case WRONG:
			SetTextColor(hDC, TESTVIEW_RED);
			break;

		default:
			ASSERT(FALSE);
		}
	}

	DrawString(hDC, nCurShow, nXScroll, nYScroll, dwStyle);
	SetTextColor(hDC, clrOld);
}

void CShowManager::DrawStore(HDC hDC, int x, int y, int nHeight)
{
	CMemBmp Bmp;
	DWORD dwIcon;
	if (m_dwShowStyle & SHOWSTYLE_WITHUSERANSWER && (!(m_dwShowStyle & SHOWSTYLE_WITHANSWER)))
	{
		dwIcon = IDB_STOREICON;
	}
	else
	{
		dwIcon = IDB_QVIEWICON;
	}

	HBITMAP hBmp = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(dwIcon), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	Bmp.CreateBmp(hDC, hBmp);
	if (hBmp)
	{
		TransparentBlt2(hDC, x, y, nHeight, nHeight,
			Bmp.GetDC(), 0, 0, Bmp.GetWidth(), Bmp.GetHeight(), RGB(255, 0, 255));
		DeleteObject(hBmp);
	}
}

void CShowManager::DrawString(HDC hDC,
							  int nCurShow,
							  int nXScroll,
							  int nYScroll,
							  DWORD dwStyle)
{
	COLORREF clrText = GetTextColor(hDC);
	HPEN hOldPen = (HPEN)GetCurrentObject(hDC, OBJ_PEN);
	HPEN hUnderPen = CreatePen(PS_SOLID, GetMetrix(SHOWMETRIX_UNDERLINEWIDTH),
		AfxGetQColor((dwStyle & DRAWSTYLE_FOCUS) ? QCOLOR_TEXTFOCUS : QCOLOR_TEXT));
	HPEN hDeletePen = CreatePen(PS_SOLID, GetMetrix(SHOWMETRIX_DELETELINEWIDTH),
		AfxGetQColor((dwStyle & DRAWSTYLE_FOCUS) ? QCOLOR_TEXTFOCUS : QCOLOR_TEXT));
	PSHOWINFO pShow = (PSHOWINFO)m_pInfoList->GetData(nCurShow);

	if (!pShow)
	{
		return;
	}
	SelectObject(hDC, pShow->hFont);
	if (pShow->pData)
	{
		long lxOffset = pShow->lFirstXOff + pShow->lFirstTxtXOff;
		long lyOffset = pShow->lFirstYOff;
		long lLineHeight = pShow->lHeight / pShow->pRetList->GetDataCount();
		int i, nIndex;

		for (i = 0, nIndex = 0; i < pShow->pRetList->GetDataCount(); ++i)
		{
			DWORD dwRet = (DWORD)pShow->pRetList->GetData(i);
			SIZE sizeSingleLine = {0}; // 每行文字的宽度
			int nCharsCount = dwRet & 0xFFFFul;
			int nIndent = (dwRet >> 16) & 0xFFul;

			if (!nCharsCount)
			{
				lyOffset += lLineHeight;
				continue;
			}

			if (!GetTextExtentPoint(hDC, (LPCTSTR)(pShow->pData) + nIndex, nCharsCount, &sizeSingleLine))
			{
				return;
			}

			if (!pShow->bHide)
			{
				// 未选中内容
				if (nCurShow < m_SelInfo.nStartBlock || nCurShow > m_SelInfo.nEndBlock)
				{
					SetBkMode(hDC, TRANSPARENT);
					SetTextColor(hDC, clrText);
					TextOut(hDC, lxOffset + nIndent - nXScroll, lyOffset + pShow->lLineGap - nYScroll,
						(LPCTSTR)(pShow->pData) + nIndex, nCharsCount);
				}
				// 选中了全部内容
				else if (nCurShow > m_SelInfo.nStartBlock && nCurShow < m_SelInfo.nEndBlock)
				{
					SetBkMode(hDC, OPAQUE);
					SetTextColor(hDC, AfxGetQColor(QCOLOR_TEXTHIGHLIGHT));
					TextOut(hDC, lxOffset + nIndent - nXScroll, lyOffset + pShow->lLineGap - nYScroll,
						(LPCTSTR)(pShow->pData) + nIndex, nCharsCount);
				}
				else
				{
					int a, b;
					long lXSag = 0;
					SIZE size = {0};

					if (nCurShow == m_SelInfo.nStartBlock)
					{
						a = m_SelInfo.lStartPos - nIndex;
						if (a < 0) // 本行开头就被选中
						{
							a = 0;
						}
						else if (a > nCharsCount) // 本行末尾在选区之前
						{
							a = nCharsCount;
						}

						if (nCurShow == m_SelInfo.nEndBlock)
						{
							b = m_SelInfo.lEndPos - nIndex;
							if (b < 0) // 本行开头在选区之后
							{
								b = a;
							}
							else if (b > nCharsCount) // 本行直到末尾都被选中
							{
								b = nCharsCount;
							}
						}
						else
						{
							b = nCharsCount;
						}
					}
					else if (nCurShow == m_SelInfo.nEndBlock)
					{
						a = 0;
						b = m_SelInfo.lEndPos - nIndex;
						if (b < 0) // 本行开头在选区之后
						{
							b = a;
						}
						else if (b > nCharsCount) // 本行直到末尾都被选中
						{
							b = nCharsCount;
						}
					}

					if (a > 0)
					{
						// 起始位置之前
						SetBkMode(hDC, TRANSPARENT);
						SetTextColor(hDC, clrText);
						TextOut(hDC, lxOffset + lXSag + nIndent - nXScroll, lyOffset + pShow->lLineGap - nYScroll,
							(LPCTSTR)(pShow->pData) + nIndex, a);
						GetTextExtentPoint(hDC, (LPCTSTR)(pShow->pData) + nIndex, a, &size);
						lXSag += size.cx;
					}
					if (b > a)
					{
						// 选区中
						SetBkMode(hDC, OPAQUE);
						SetTextColor(hDC, AfxGetQColor(QCOLOR_TEXTHIGHLIGHT));
						TextOut(hDC, lxOffset + lXSag + nIndent - nXScroll, lyOffset + pShow->lLineGap - nYScroll,
							(LPCTSTR)(pShow->pData) + nIndex + a, b - a);
						GetTextExtentPoint(hDC, (LPCTSTR)(pShow->pData) + nIndex + a, b - a, &size);
						lXSag += size.cx;
					}
					// 选区后
					SetBkMode(hDC, TRANSPARENT);
					SetTextColor(hDC, clrText);
					TextOut(hDC, lxOffset + lXSag + nIndent - nXScroll, lyOffset + pShow->lLineGap - nYScroll,
						(LPCTSTR)(pShow->pData) + nIndex + b, nCharsCount - b);
				}

				if (dwStyle & DRAWSTYLE_DELETELINE)
				{
					SelectObject(hDC, hDeletePen);
					MoveToEx(hDC, lxOffset + nIndent - nXScroll, lyOffset + nYScroll + lLineHeight / 2 + pShow->lLineGap, NULL);
					LineTo(hDC, lxOffset + nIndent + sizeSingleLine.cx, lyOffset + nYScroll + lLineHeight / 2 + pShow->lLineGap);
				}
			}

			if (dwStyle & DRAWSTYLE_UNDERLINE)
			{
				long lBlankWidth = sizeSingleLine.cx;
				if (!i)
				{
					lxOffset -= pShow->lFirstTxtXOff;
					lBlankWidth += pShow->lFirstTxtXOff * 2;
				}

				SelectObject(hDC, hUnderPen);
				MoveToEx(hDC, lxOffset + nIndent - nXScroll, lyOffset + lLineHeight - nYScroll, NULL);
				LineTo(hDC, lxOffset + nIndent + lBlankWidth - nXScroll, lyOffset + lLineHeight - nYScroll);
			}
			
			lxOffset = 0;
			lyOffset += lLineHeight;
			nIndex += nCharsCount;
		}
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hUnderPen);
	DeleteObject(hDeletePen);
	SetTextColor(hDC, clrText);
}

void CShowManager::DrawImage(HDC hDC, PSHOWINFO pShow, int nXScroll, int nYScroll)
{
	PITEMIMAGE pImage;
	CMemBmp Bmp;
	
	pImage = (PITEMIMAGE)(pShow->pData);
	if (!ShowImage(hDC, &Bmp, pImage->nImageType, pImage->pImageData, pImage->ulImageSize))
	{
		return;
	}
	Bmp.Draw(hDC, pShow->lFirstXOff - nXScroll, pShow->lFirstYOff - nYScroll);
}

/*
功能：	对鼠标指向的位置进行测试
返回值：当鼠标位于当前Mgr前时，返回FALSE，否则返回TRUE
备注：	当pHit不为NULL时，将影响以下参数：
		nBlockIndex, nInfo, nTextPos
*/
BOOL CShowManager::HitTest(HDC hDC, long x, long y, PISWHITTEST pHit)
{
	int i = 0, j = 0;
	PSHOWINFO pShow;

	ASSERT(pHit);
	pHit->nHitType = SHOWHITTEST_NONE;

	// 当点击的位置在本题目之上时
	// 不再做后续的判断，直接返回FALSE，但将在pHit中返回测试结果
	pShow = (PSHOWINFO)(m_pInfoList->GetData(0));
	if (y < pShow->lFirstYOff)
	{
		return FALSE;
	}

	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		pShow = (PSHOWINFO)(m_pInfoList->GetData(i));
		if (!pShow)
		{
			return FALSE;
		}

		if (m_nShowType == SHOWWNDTYPE_TEST &&
			(pShow->nType == SHOWTYPE_TEXT || pShow->nType == SHOWTYPE_TEXTANS))
		{
			if (y >= pShow->lFirstYOff && y <= pShow->lFirstYOff + pShow->lHeight)
			{
				pHit->nBlockIndex = i;
				pHit->nHitType = SHOWHITTEST_TEXTANS;
				return TRUE;
			}
		}
		else if (y >= pShow->lFirstYOff + pShow->lLineGap &&
				 y <= pShow->lFirstYOff + pShow->lLineGap + pShow->lHeight)
		{
			if (!pShow->pRetList->GetDataCount())
			{
				if (x >= pShow->lFirstXOff && x <= pShow->lFirstXOff + pShow->lWidth)
				{
					pHit->nBlockIndex = i;
					pHit->nHitType = SHOWHITTEST_IMAGE;
					return TRUE;
				}
			}
			else
			{
				int nLineHeight = pShow->lHeight / pShow->pRetList->GetDataCount();
				int nLineIndex = (y - pShow->lFirstYOff) / nLineHeight;
				BOOL bIn = FALSE;
				
				if (0 == nLineIndex)
				{
					DWORD dwRet = (DWORD)pShow->pRetList->GetData(nLineIndex);
					int nIndent = (dwRet >> 16) & 0xFFul;
					if (pShow->pRetList->GetDataCount() <= 1)
					{
						int nLeft = pShow->lFirstXOff + nIndent;
						int nRight = pShow->lLastXOff + nIndent + pShow->lFirstTxtXOff * 2;
						if (x >= nLeft && x <= nRight)
						{
							bIn = TRUE;
						}
					}
					else if (x >= pShow->lFirstXOff + nIndent && x < m_lWidth)
					{
						bIn = TRUE;
					}
				}
				else if (nLineIndex >= pShow->pRetList->GetDataCount() - 1)
				{
					// 这里必然是多行了
					if (x <= pShow->lLastXOff)
					{
						bIn = TRUE;
					}
				}
				else
				{
					bIn = (x <= m_lWidth);
				}
				
				if (bIn)
				{
					switch(pShow->nType)
					{
					case SHOWTYPE_TEXTSPAN:
					case SHOWTYPE_TEXTDIVISION:
					case SHOWTYPE_TEXTPARAGRAPH:
					case SHOWTYPE_BLANKANS:
						pHit->nHitType = SHOWHITTEST_TEXT;
						break;
						
					case SHOWTYPE_OPTION:
					case SHOWTYPE_OPTIONINDEX:
						pHit->nHitType = SHOWHITTEST_OPTION;
						pHit->nInfo = pShow->nInfo;
						break;
						
					case SHOWTYPE_BLANK:
					case SHOWTYPE_BLANKDIVISION:
						pHit->nInfo = pShow->nInfo;
						pHit->nHitType = SHOWHITTEST_BLANK;
						break;

					case SHOWTYPE_TEXT:
					case SHOWTYPE_TEXTANS:
						pHit->nHitType = SHOWHITTEST_TEXTANS;
						break;
						
					default:
						break;
					}
				}
			}
		}
		if (pHit->nHitType != SHOWHITTEST_NONE)
		{
			break;
		}
	}

	if (SHOWHITTEST_NONE == pHit->nHitType)
	{
		pShow = (PSHOWINFO)(m_pInfoList->GetData(0));
		if (y >= pShow->lFirstYOff && y <= pShow->lFirstYOff + m_lHeight &&
			x >= 0 && x <= m_lWidth)
		{
			pHit->nHitType = SHOWHITTEST_INITEM;
		}
	}

	CheckTextPos(hDC, x, y, pHit);
	return TRUE;
}

BOOL CShowManager::IsVisible(long lYPos, long lHeight)
{
	int i;
	PSHOWINFO p;
	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		p = (PSHOWINFO)m_pInfoList->GetData(i);
		if (lYPos < p->lFirstYOff + p->lHeight &&
			lYPos + lHeight > p->lFirstYOff)
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CShowManager::SetTextSelect(int nStartBlock, int nStartPos, int nEndBlock, int nEndPos)
{
	switch(m_nShowType)
	{
	case SHOWWNDTYPE_PROPERTY:
	case SHOWWNDTYPE_TESTVIEW:
	case SHOWWNDTYPE_SUBJECTIVEVIEW:
		break;

	default:
		return;
	}

	m_SelInfo.nStartBlock = nStartBlock;
	m_SelInfo.lStartPos = nStartPos;
	
	m_SelInfo.nEndBlock = nEndBlock;
	if (m_SelInfo.nEndBlock < 0)
	{
		m_SelInfo.nEndBlock = m_pInfoList->GetDataCount() - 1;
	}
	
	m_SelInfo.lEndPos = nEndPos;
	if (m_SelInfo.lEndPos < 0)
	{
		PSHOWINFO pShow = (PSHOWINFO)(m_pInfoList->GetData(m_SelInfo.nEndBlock));
		if (!pShow)
		{
			return;
		}
		if (!pShow->pRetList->GetDataCount())
		{
			return;
		}
		m_SelInfo.lEndPos = pShow->ulDataSize;
	}
}

void CShowManager::CheckTextPos(HDC hDC, int x, int y, PISWHITTEST pHit)
{
	int i;
	long lYStore; // 当y偏移量超出了块总高度时计算用
	HFONT hOldFont = (HFONT)GetCurrentObject(hDC, OBJ_FONT);
	
	if (!pHit)
	{
		return;
	}
	for (i = 0; i < m_pInfoList->GetDataCount(); ++i)
	{
		int j;
		int nSingleHeight; // 单行的高度
		int nLineIndex; // 选中的行序号
		int nTextIndex; // 行开头的字符序号
		int nChsCount; // 行内的字符序号
		int nIndent; // 行缩进
		DWORD dwRet;
		SIZE size = {0};

		PSHOWINFO pShow = (PSHOWINFO)(m_pInfoList->GetData(i));
		if (!pShow)
		{
			break;
		}

		if (y < pShow->lFirstYOff)
		{
			break;
		}
		
		// 检测到了图片
		if (!pShow->pRetList->GetDataCount())
		{
			pHit->nBlockIndex = i;
			continue;
		}
		
		SelectObject(hDC, pShow->hFont);
		nSingleHeight = pShow->lHeight / pShow->pRetList->GetDataCount();

		lYStore = y - pShow->lFirstYOff;
		if (lYStore > pShow->lHeight)
		{
			lYStore = pShow->lHeight;
		}
		nLineIndex = lYStore / nSingleHeight;
		if (nLineIndex >= pShow->pRetList->GetDataCount())
		{
			nLineIndex = pShow->pRetList->GetDataCount() - 1;
		}
		
		// 获得了行开始的位置
		for (j = 0, nTextIndex = 0; ; ++j)
		{
			dwRet = (DWORD)(pShow->pRetList->GetData(j));
			if (j >= nLineIndex)
			{
				break;
			}
			nTextIndex += dwRet & 0xFFFFul;
		}

		// 当鼠标在行缩进左边时，认为在最左侧
		nIndent = (dwRet >> 16) & 0xFFul;
		
		if (x < nIndent)
		{
			pHit->nBlockIndex = i;
			pHit->nTextPos = nTextIndex;
		}
		// 点击第一行
		else if (0 == nLineIndex)
		{
			if (GetTextExtentExPoint(hDC, (LPTSTR)(pShow->pData) + nTextIndex, dwRet & 0xFFFFul,
				x - pShow->lFirstXOff - pShow->lFirstTxtXOff - nIndent, &nChsCount, NULL, &size))
			{
				pHit->nBlockIndex = i;
				pHit->nTextPos = nChsCount + nTextIndex;
			}
		}
		else
		{
			if (GetTextExtentExPoint(hDC, (LPTSTR)(pShow->pData) + nTextIndex, dwRet & 0xFFFFul,
				x - nIndent, &nChsCount, NULL, &size))
			{
				pHit->nBlockIndex = i;
				pHit->nTextPos = nChsCount + nTextIndex;
			}
		}
	}
	
	SelectObject(hDC, hOldFont);
}

void CShowManager::Copy(CString &str)
{
	_stdstring ss;
	PSHOWINFO pShow;
	long a, b;
	int i;

	for (i = m_SelInfo.nStartBlock; i <= m_SelInfo.nEndBlock; ++i)
	{
		pShow = (PSHOWINFO)m_pInfoList->GetData(i);
		if (!pShow)
		{
			return;
		}

		if (i == m_SelInfo.nStartBlock)
		{
			a = m_SelInfo.lStartPos;
		}
		else
		{
			a = 0;
		}

		if (i == m_SelInfo.nEndBlock)
		{
			b = m_SelInfo.lEndPos;
		}
		else
		{
			b = pShow->ulDataSize;
		}

		ss.append((LPCTSTR)pShow->pData + a, b - a);
		if (b >= pShow->ulDataSize)
		{
			switch(pShow->nType)
			{
			case SHOWTYPE_TEXTDIVISION:
			case SHOWTYPE_TEXTPARAGRAPH:
			case SHOWTYPE_OPTION:
			case SHOWTYPE_OPTIONANS:
			case SHOWTYPE_JUDGEANS:
			case SHOWTYPE_BLANKDIVISION:
			case SHOWTYPE_TEXT:
			case SHOWTYPE_TEXTANS:
				ss += CTextManager::s_szWinReturn;
				break;

			default:
				break;
			}
		}
	}

	ASSERT(pShow);
	switch(pShow->nType)
	{
	case SHOWTYPE_BLANK:
	case SHOWTYPE_BLANKANS:
		ss += CTextManager::s_szWinReturn;
		break;

	default:
		break;
	}

	str += ss.c_str();
}
