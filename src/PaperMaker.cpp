// PaperMaker.cpp: implementation of the CPaperMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PaperMaker.h"

#include <direct.h>
#include "Blank.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static LPCTSTR szHead1 =
	_T("<!DOCTYPE html>\n<html>\n<head>\n<meta http-equiv=\"Content-Type\" content=\"text/html;charset=utf-8;\"/>\n<title>");
static LPCTSTR szHead2 =
	_T("</title>\n<style>\n") \
	_T("body{font-family:");
static LPCTSTR szHead3 =
	_T("}\n.txt{font-family:");
static LPCTSTR szHead4 =
	_T(";line-height:1.5em;padding-left:0.5em;margin-bottom:0.5em;margin-top:0;text-indent:2em;clear:both;}\n") \
	_T(".p{padding-left:2.5em;line-height:2em;margin-bottom:0.5em;}\n") \
	_T(".title{margin-bottom:0.5em;clear:both;}\n") \
	_T(".clear{clear:both;}\n") \
	_T("ol.desc,li.desc{list-style-type:decimal;padding-left:2.5em;line-height:2em;}\n") \
	_T("li.desc{padding:0;}\n") \
	_T("ol.desc{margin-top:0em;margin-bottom:0.5em;clear:both;}\n") \
	_T("ol.choise,li.choise{list-style-type:upper-alpha;line-height:2em;}\n") \
	_T("ol.choise{margin-top:0em;margin-bottom:0.5em;padding-left:4em;clear:both;}\n") \
	_T("li.choise{padding:0 5em 0 0;}\n") \
	_T("ol.choisein,li.choisein{list-style-type:upper-alpha;line-height:2em;}\n") \
	_T("ol.choisein{margin-top:0em;margin-bottom:0.5em;padding-left:1.5em;}\n") \
	_T("li.choisein{padding:0 4em 0 0;}\n") \
	_T("span.under{word-break:normal;width:auto;white-space:pre-wrap;word-wrap:break-word;overflow:hidden;}\n") \
	_T("td.imgtitle{margin:0;padding:0;}\n") \
	_T("table.img,td.img{margin:auto;padding:0;border:none;line-height:normal;text-indent:0;text-align:center;}\n") \
	_T("</style>\n</head>\n<body>\n<h1 style=\"text-align:center;\">");
static LPCTSTR szHead5 =
	_T("</h1>\n");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaperMaker::CPaperMaker() :
	m_fp(NULL),
	m_pList(NULL), m_pPara(NULL), m_nCurItem(0),
	m_nPaperType(PAPERTYPE_TEXTONLY), m_nAnswerType(ANSWERTYPE_ONLYANSWER), m_bWithRes(FALSE),
	m_szAnsiFont(NULL), m_szMainFont(NULL), m_szPreFont(NULL),
	m_nListIndex(0), m_nItemIndex(0)
{
}

BOOL CPaperMaker::PaperOut(CList *pList,
						   CTestPara *pCPara,
						   LPCTSTR szPaperPath,
						   int nPaperType,
						   int nAnswerType,
						   BOOL bWithRes,
						   LPCTSTR szAnsiFont,
						   LPCTSTR szMainFont,
						   LPCTSTR szPreFont)
{
	if (!pList)
	{
		return FALSE;
	}

	m_pList = pList;
	m_pPara = pCPara;
	m_nPaperType = nPaperType;
	m_nAnswerType = nAnswerType;
	m_bWithRes = bWithRes;

	m_szAnsiFont = szAnsiFont;
	m_szMainFont = szMainFont;
	m_szPreFont = szPreFont;

	return (MakePaper(szPaperPath) && MakeAnswer(szPaperPath));
}

void CPaperMaker::MakePaperHead()
{
	PTESTPARA pPara = m_pPara->GetTestPara();
	CString strMainFont, strPreFont;

	strMainFont.Format(_T("%s,%s"), m_szAnsiFont, m_szMainFont);
	strPreFont.Format(_T("%s,%s"), m_szAnsiFont, m_szPreFont);

	switch(m_nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		m_string += pPara->pPaper->GetDescription();
		m_string += _T("\r\n\r\n");
		break;

	case PAPERTYPE_HTMLPLUS:
	case PAPERTYPE_HTMLONLY:
		m_string += szHead1;
		m_string += pPara->pPaper->GetDescription();
		m_string += szHead2;
		m_string += strMainFont;
		m_string += szHead3;
		m_string += strPreFont;
		m_string += szHead4;
		m_string += pPara->pPaper->GetDescription();
		m_string += szHead5;
		break;
	}
}

void CPaperMaker::MakeHTMLTail()
{
	m_string += _T("</body></html>");
}

void CPaperMaker::MakeMarkTime()
{
	CString temp;
	PTESTPARA pPara = m_pPara->GetTestPara();

	temp.Format(IDS_QUIZMKR_TIMEPOINT, pPara->nTime, m_pPara->GetTotalPoints(m_pList));

	switch(m_nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		m_string += temp;
		m_string += _T("\r\n\r\n");
		break;

	case PAPERTYPE_HTMLONLY:
	case PAPERTYPE_HTMLPLUS:
		m_string += _T("<div style=\"text-align:center;\">");
		m_string += temp;
		m_string += _T("</div>\n");
		break;
	}
}

void CPaperMaker::MakeQuestionList(int nPaperType, int nListIndex, CItem *pItem, PTESTPARA pPara)
{
	CString string;
	int nCount, nPoint;
	UINT uFormat;

	switch(nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		string.Format(_T("[%d] "), nListIndex);
		break;

	case PAPERTYPE_HTMLPLUS:
	case PAPERTYPE_HTMLONLY:
		string.Format(_T("<h3 class=\"title\">%d. "), nListIndex);
		break;
	}
	m_string += string;

	switch(pItem->GetType())
	{
	case TYPE_SCHOISE:
		nCount = pPara->nSChCount;
		nPoint = pPara->nSChPoints;
		break;
		
	case TYPE_MCHOISE:
		nCount = pPara->nMChCount;
		nPoint = pPara->nMChPoints;
		break;
		
	case TYPE_JUDGE:
		nCount = pPara->nJdgCount;
		nPoint = pPara->nJdgPoints;
		break;
		
	case TYPE_BLANK:
		nCount = pPara->nBlkCount;
		nPoint = pPara->nBlkPoints;
		break;
		
	case TYPE_TEXT:
		nCount = pPara->nTxtCount;
		nPoint = pPara->nTxtPoints;
		break;
		
	case TYPE_GROUP:
		nCount = ((CItemGroup*)pItem)->GetItemCount();
		nPoint = ((CItemGroup*)pItem)->GetFullMark(m_pPara);
		break;
	}
	if (!pItem->IsDefaultType())
	{
		nCount = pPara->nCustCount[pItem->GetCustomIndex() - 1];
	}

	switch(pItem->GetType())
	{
	case TYPE_BLANK:
		uFormat = (MARKTYPE_AVERAGE == pPara->nMarkType) ?
			IDS_QUIZMKR_NOSCOREINFO : IDS_QUIZMKR_BLKINFO;
		break;

	case TYPE_GROUP:
		uFormat = IDS_QUIZMKR_NOSCOREINFO;
		break;

	default:
		uFormat = (MARKTYPE_AVERAGE == pPara->nMarkType) ?
			IDS_QUIZMKR_NOSCOREINFO : IDS_QUIZMKR_TYPEINFO;
		break;
	}
	string.Format(uFormat, pItem->GetTypeDesc(), nCount);
	m_string += string;

	if (MARKTYPE_USERSET == pPara->nMarkType)
	{
		switch(pItem->GetType())
		{
		case TYPE_GROUP:
			string.Empty();
			break;

		default:
			string.Format(IDS_QUIZMKR_POINT, nPoint);
			break;
		}
		m_string += string;
	}

	switch(nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		m_string += CTextManager::s_szWinReturn;
		break;

	case PAPERTYPE_HTMLPLUS:
	case PAPERTYPE_HTMLONLY:
		m_string += _T("</h3>");
		m_string += _T("\n");
		break;
	}
}

void CPaperMaker::MakeSingle(BOOL bWithAnswer, BOOL bWithRes, int nType, PTPARAS pImagePara)
{
	CItem *pItem = m_pList->GetItem(m_nCurItem);
	PTESTPARA pPara = m_pPara->GetTestPara();
	CString temp;

	if (!pItem || pItem->GetRawType() != nType)
	{
		return;
	}
	MakeQuestionList(m_nPaperType, m_nListIndex, pItem, pPara);

	_stdstring temp1;
	while(pItem && pItem->GetRawType() == nType)
	{
		switch(m_nPaperType)
		{
		case PAPERTYPE_TEXTONLY:
			pItem->MakeTextOut(m_string, m_nListIndex, m_nItemIndex, pImagePara,
				bWithAnswer, bWithRes);
			m_string += CTextManager::s_szWinReturn;
			break;

		case PAPERTYPE_HTMLONLY:
		case PAPERTYPE_HTMLPLUS:
			pItem->MakeHTML(m_fp, m_string, m_nListIndex, m_nItemIndex, pImagePara,
				bWithAnswer, bWithRes);
			break;
		}

		++m_nItemIndex;
		++m_nCurItem;
		pItem = m_pList->GetItem(m_nCurItem);
	}

	++m_nListIndex;
}

BOOL CPaperMaker::MakeGroup(BOOL bWithAnswer, BOOL bWithRes, PTPARAS pImagePara)
{
	CItem *pItem = m_pList->GetItem(m_nCurItem);
	PTESTPARA pPara = m_pPara->GetTestPara();
	CString temp;

	if (!pItem || pItem->GetType() != TYPE_GROUP)
	{
		return FALSE;
	}
	CItemGroup *pGroup = (CItemGroup*)pItem;
	MakeQuestionList(m_nPaperType, m_nListIndex, pItem, pPara);

	switch(m_nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		pGroup->MakeTextOut(m_string, m_nListIndex, m_nItemIndex, pImagePara,
			bWithAnswer, bWithRes);
		m_string += CTextManager::s_szWinReturn;
		break;

	case PAPERTYPE_HTMLONLY:
	case PAPERTYPE_HTMLPLUS:
		pGroup->MakeHTML(m_fp, m_string, m_nListIndex, m_nItemIndex, pImagePara,
			bWithAnswer, bWithRes);
		break;
	}

	++m_nListIndex;
	++m_nCurItem;
	m_nItemIndex += pGroup->GetItemCount();
	return TRUE;
}

void CPaperMaker::MakeAnswerPath(CString &strPath)
{
	int n = strPath.ReverseFind(_T('.'));
	CString tmp = strPath.Left(n);

	tmp += _T(".asw");
	strPath = tmp;
}

BOOL CPaperMaker::SaveText(LPCTSTR pText, FILE *fp)
{
	LPSTR ptr;
	int nLen;

	if (PAPERTYPE_TEXTONLY == m_nPaperType)
	{
		BYTE bom[3] = {0xEF, 0xBB, 0xBF};
		int nBomLen = 3;
		int nCS = CS_UTF8;

		if (OS_WIN95 == g_nOSVersion)
		{
			bom[0] = 0xFF;
			bom[1] = 0xFE;
			nBomLen = 2;
			nCS = CS_UTF16;
		}

		ptr = CTextManager::SwitchSave(m_string.c_str(), nCS, &nLen);
		if (!ptr)
		{
			return FALSE;
		}
		if (!fwrite(bom, nBomLen, 1, fp))
		{
			free(ptr);
			return FALSE;
		}
	}
	else
	{
		ptr = CTextManager::SwitchSave(m_string.c_str(), CS_UTF8, &nLen);
		if (!ptr)
		{
			return FALSE;
		}
	}

	if (!fwrite(ptr, nLen - 1, 1, fp))
	{
		free(ptr);
		return FALSE;
	}
	free(ptr);
	return TRUE;
}

void CPaperMaker::MakeContent(BOOL bWithAnswer, BOOL bWithRes)
{
	TPARAS sImagePara = {0};

	MakePaperHead();
	MakeMarkTime();

	switch(m_nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
	case PAPERTYPE_HTMLPLUS:
		sImagePara.dwPara1 = TRUE;
		sImagePara.dwPara2 = (DWORD)(LPCTSTR)m_strImagePath;
		sImagePara.dwPara3 = (DWORD)m_szRelativeImgPath;
		break;
	}

	m_nCurItem = 0;
	m_nListIndex = 1;
	m_nItemIndex = 1;
	while(1)
	{
		CItem *pItem = m_pList->GetItem(m_nCurItem);
		int nItemType;
		
		if (!pItem)
		{
			break;
		}
		nItemType = pItem->GetRawType();
		
		switch(nItemType)
		{
		case TYPE_GROUP:
			MakeGroup(bWithAnswer, bWithRes, &sImagePara);
			break;
			
		default:
			MakeSingle(bWithAnswer, bWithRes, nItemType, &sImagePara);
			break;
		}
	}
	
	switch(m_nPaperType)
	{
	case PAPERTYPE_HTMLPLUS:
	case PAPERTYPE_HTMLONLY:
		MakeHTMLTail();
		break;
	}
}

BOOL CPaperMaker::MakePaper(LPCTSTR strPath)
{
	TPARAS sImagePara = {0};
	int t = time(NULL);

	if (!CTextManager::GetFilePath(m_strImagePath, strPath))
	{
		return FALSE;
	}
	_sntprintf(m_szRelativeImgPath, 64, _T("%d"), t);
	m_strImagePath += m_szRelativeImgPath;
	m_strImagePath += _T("\\");
	_tmkdir(m_strImagePath);
	_sntprintf(m_szRelativeImgPath, 64, _T("./%d/"), t);

	m_fp = _tfopen(strPath, _T("wb"));
	if (!m_fp)
	{
		return FALSE;
	}

	m_string.erase();
	MakeContent(FALSE, FALSE);

	SaveText(m_string.c_str(), m_fp);
	fclose(m_fp);

	// 若没有生成图片，则删除路径
	_trmdir(m_strImagePath);
	return TRUE;
}

BOOL CPaperMaker::MakeAnswer(LPCTSTR strPath)
{
	CString strFilePath;

	strFilePath = strPath;
	MakeAnswerPath(strFilePath);

	m_string.erase();

	if (ANSWERTYPE_ONLYANSWER == m_nAnswerType)
	{
		CString temp;
		_stdstring temp1;
		TCHAR sz[64] = {0};
		
		strFilePath += _T(".txt");
		LoadString(AfxGetInstanceHandle(), IDS_QUIZMKR_RESTXT, sz, 64);

		int nQuestionList = 1;
		int nIndex = 1;
		int nCurItem = 0;
		int nType = TYPE_DEFAULT;
		CItem *pItem;
		
		while(1)
		{
			pItem = m_pList->GetItem(nCurItem);
			if (!pItem)
			{
				break;
			}

			temp.Format(_T("[%d]\r\n"), nQuestionList++);
			m_string += temp;
			nIndex = 1;
			nType = pItem->GetRawType();

			if (TYPE_GROUP == nType)
			{
				CItemGroup *pGroup = (CItemGroup*)pItem;
				CItem *pSub;
				int nSub = 0;
				BOOL bRet = TRUE;

				while(1)
				{
					pSub = pGroup->GetItem(nSub);
					if (!pSub)
					{
						break;
					}

					if (pSub->GetType() == TYPE_TEXT && !bRet)
					{
						m_string += CTextManager::s_szWinReturn;
					}

					pSub->MakeAnswer(temp1, MKASW_INALINE, -1);
					temp.Format(_T("%d. %s"), nIndex, temp1.c_str());
					m_string += temp;

					if (m_bWithRes && pSub->GetResolve() && _tcslen(pSub->GetResolve()))
					{
						m_string += CTextManager::s_szWinReturn;
						m_string += sz;
						m_string += pSub->GetResolve();
						m_string += CTextManager::s_szWinReturn;
						bRet = TRUE;
					}
					else if (!(nIndex % 5) || pSub->GetType() == TYPE_TEXT)
					{
						m_string += CTextManager::s_szWinReturn;
						bRet = TRUE;
					}
					else
					{
						m_string += _T("   ");
						bRet = FALSE;
					}

					++nIndex;
					++nSub;
				}

				++nCurItem;
				if (!bRet)
				{
					m_string += CTextManager::s_szWinReturn;
				}
			}
			else
			{
				BOOL bRet = TRUE;

				while(1)
				{
					if (pItem->GetType() == TYPE_TEXT && !bRet)
					{
						m_string += CTextManager::s_szWinReturn;
					}

					pItem->MakeAnswer(temp1, MKASW_INALINE, -1);
					temp.Format(_T("%d. %s"), nIndex, temp1.c_str());
					m_string += temp;
					
					if (m_bWithRes && pItem->GetResolve() && _tcslen(pItem->GetResolve()))
					{
						m_string += CTextManager::s_szWinReturn;
						m_string += sz;
						m_string += pItem->GetResolve();
						m_string += CTextManager::s_szWinReturn;
						bRet = TRUE;
					}
					else if (!(nIndex % 5) || pItem->GetType() == TYPE_TEXT)
					{
						m_string += CTextManager::s_szWinReturn;
						bRet = TRUE;
					}
					else
					{
						m_string += _T("   ");
						bRet = FALSE;
					}

					++nIndex;
					++nCurItem;
					pItem = m_pList->GetItem(nCurItem);
					if (!pItem || pItem->GetRawType() != nType)
					{
						break;
					}
				}

				if (!bRet)
				{
					m_string += CTextManager::s_szWinReturn;
				}
			}
		}
	}
	else
	{
		// 答案文件名
		switch(m_nPaperType)
		{
		case PAPERTYPE_TEXTONLY:
			strFilePath += _T(".txt");
			break;

		case PAPERTYPE_HTMLONLY:
		case PAPERTYPE_HTMLPLUS:
			strFilePath += _T(".html");
			break;

		default:
			ASSERT(FALSE);
		}

		// 此时已经产生了图片路径和相对图片路径
		MakeContent(TRUE, m_bWithRes);
	}

	FILE *fp = _tfopen(strFilePath, _T("wb"));
	if (!fp)
	{
		return FALSE;
	}
	SaveText(m_string.c_str(), fp);
	fclose(fp);

	return TRUE;
}
