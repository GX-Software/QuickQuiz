// Judge.cpp: implementation of the CJudge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Judge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CJudge::CJudge() :
	m_bAnswer(JUDGEANS_UNKNOWN), m_bUserAnswer(JUDGEANS_UNKNOWN)
{
	m_nType = TYPE_JUDGE;
}

CJudge::~CJudge()
{
}

LPCTSTR CJudge::GetTypeDesc()
{
	LPCTSTR pReturn = CItem::GetTypeDesc();
	if (pReturn)
	{
		return pReturn;
	}
	
	LoadString(AfxGetInstanceHandle(), IDS_ITEMTYPE_JDG, m_szTypeDesc, TYPE_BUFFSIZE);	
	return m_szTypeDesc;
}

// 判断题存储格式
// ?Bytes：通用存储块
// 1Byte：答案
EBOOL CJudge::LoadFile(FILE *fp, DWORD dwVersion)
{
	if (!CItem::LoadFile(fp, dwVersion))
	{
		return Q_WARNING(ERROR_INNER);
	}
	
	// 答案
	m_bAnswer = fgetc(fp);
	switch(m_bAnswer)
	{
	case EOF:
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	
	return Q_RET(TRUE);
}

// 判断题存储格式
// ?Bytes：通用存储块
// 1Byte：答案
EBOOL CJudge::SaveFile(FILE *fp)
{
	if (!CItem::SaveFile(fp))
	{
		return Q_ERROR(ERROR_INNER);
	}
	
	// 答案
	if (!fwrite(&m_bAnswer, sizeof(BYTE), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	return Q_TRUE;
}

RETURN_E TiXmlElement* CJudge::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
{
	pParent = CItem::WriteXML(pParent, nEncode, bMaxCompatible);
	if (!pParent)
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}
	
	TiXmlElement *pAnswer = new TiXmlElement(xml_lpszNode_Answer);
	if (!pAnswer)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pParent->LinkEndChild(pAnswer);
	
	TiXmlText *pText = new TiXmlText(m_bAnswer ? xml_lpszCommon_True : xml_lpszCommon_False);
	if (!pText)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pAnswer->LinkEndChild(pText);
	
	return Q_TRUE_RET(pParent);
}

EBOOL CJudge::ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup)
{
	if (!CItem::ReadXML(pQstn, nEncode, bGroup))
	{
		return Q_WARNING(ERROR_INNER);
	}

	TiXmlElement *pAsw = pQstn->FirstChildElement(xml_lpszNode_Answer);
	if (!pAsw)
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	LPCSTR lpAsw = pAsw->GetText();
	if (!stricmp(lpAsw, xml_lpszCommon_True))
	{
		m_bAnswer = TRUE;
	}
	else if (!stricmp(lpAsw, xml_lpszCommon_False))
	{
		m_bAnswer = FALSE;
	}
	else
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	return Q_RET(TRUE);
}

BOOL CJudge::SetDescription(LPCTSTR szDescription)
{
	LPTSTR s = ClearEndQuote(szDescription);
	if (!s)
	{
		return FALSE;
	}

	BOOL bRet = CItem::SetDescription(s);
	free(s);

	return bRet;
}

void CJudge::ClearAllInfo()
{
	CItem::ClearAllInfo();
	
	m_bAnswer = -1;
}

void CJudge::ClearUserInfo()
{
	CItem::ClearUserInfo();
	m_bUserAnswer = -1;
}

CItem* CJudge::Alloc()
{
	CJudge *pJudge = new CJudge;
	if (!pJudge)
	{
		return NULL;
	}
	
	pJudge->CopyItem(this);
	
	return (CItem*)pJudge;
}

BOOL CJudge::CopyItem(CItem *pItem)
{
	if (!CItem::CopyItem(pItem))
	{
		return FALSE;
	}
	
	if (TYPE_JUDGE == pItem->GetType())
	{
		SetAnswer(((CJudge*)pItem)->m_bAnswer);
	}
	
	return TRUE;
}

void CJudge::ClipCopy(CFixedSharedFile &sf)
{
	CItem::ClipCopy(sf);
	
	sf.Write(&m_bAnswer, sizeof(m_bAnswer));
}

BOOL CJudge::ClipPaste(CFixedSharedFile &sf)
{
	if (!CItem::ClipPaste(sf))
	{
		return FALSE;
	}
	
	sf.Read(&m_bAnswer, sizeof(m_bAnswer));
	
	return TRUE;
}

void CJudge::MakeText(_stdstring &str, UINT nStyle, int nPara)
{
	str = m_strDescription;
	
	if (nStyle & MKTXT_JUDGEBRACKET)
	{
		str += _T("  (  )");
	}
	if (nStyle & MKTXT_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CJudge::MakeAnswer(_stdstring &str, UINT nStyle, int nPara /* = 0 */)
{
	TCHAR szBuf[16] = {0};

	if (nStyle & MKASW_ASWINQUESTION)
	{
		str = m_strDescription;

		switch(m_bAnswer)
		{
		case FALSE:
			_tcscpy(szBuf, _T("( × )"));
			break;
			
		case TRUE:
			_tcscpy(szBuf, _T("( √ )"));
			break;
			
		default:
			str.erase();
			return;
		}

		str += szBuf;

		if (MKASW_WITHRESOLVE & nStyle)
		{
			str += CTextManager::s_szWinReturn;
			str += GetResolve();
		}
	}
	else
	{
		switch(m_bAnswer)
		{
		case FALSE:
			LoadString(AfxGetInstanceHandle(), IDS_ITEM_INCORRECT, szBuf, _countof(szBuf));
			break;
			
		case TRUE:
			LoadString(AfxGetInstanceHandle(), IDS_ITEM_CORRECT, szBuf, _countof(szBuf));
			break;
			
		default:
			str.erase();
			return;
		}
		
		str = szBuf;
	}

	if (nStyle & MKASW_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CJudge::MakeUserAnswer(_stdstring &str)
{
	TCHAR szBuf[16] = {0};

	switch(m_bUserAnswer)
	{
	case FALSE:
		LoadString(AfxGetInstanceHandle(), IDS_ITEM_INCORRECT, szBuf, _countof(szBuf));
		break;
		
	case TRUE:
		LoadString(AfxGetInstanceHandle(), IDS_ITEM_CORRECT, szBuf, _countof(szBuf));
		break;
		
	default:
		LoadString(AfxGetInstanceHandle(), IDS_QLIST_UNASW, szBuf, _countof(szBuf));
	}

	str = szBuf;
}

BOOL CJudge::CheckItemValid()
{
	return (m_bAnswer < 0) ? FALSE : TRUE;
}

void CJudge::MakeTextOut(_stdstring &str,
						 int nListIndex,
						 int nItemIndex,
						 PTPARAS pImagePara,
						 BOOL bWithAnswer,
						 BOOL bWithRes)
{
	TCHAR sz[64] = {0};

	_sntprintf(sz, 64, _T("%d. "), nItemIndex);
	str += sz;
	str += GetDescription();

	if (bWithAnswer)
	{
		_stdstring asw;
		MakeAnswer(asw, MKTXT_DEFAULT, 0);
		str += _T("  (");
		str += asw;
		str += _T(")\r\n");
	}
	else
	{
		str += _T("  (  )\r\n");
	}
	
	MakeTextImage(str, nItemIndex, pImagePara, !bWithAnswer);
	str += CTextManager::s_szWinReturn;

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_TEXTONLY);
	}
}

void CJudge::MakeHTML(FILE *fp,
					  _stdstring &str,
					  int nListIndex,
					  int nItemIndex,
					  PTPARAS pImagePara,
					  BOOL bWithAnswer,
					  BOOL bWithRes)
{
	TCHAR sz[64] = {0};
	str += _T("<ol class=\"desc\" start=\"");

	_sntprintf(sz, 64, _T("%d"), nItemIndex);
	str += sz;
	str += _T("\">\n<li class=\"desc\">");

	_stdstring ss;
	ss = GetDescription();
	ReplaceHTMLEndLine(ss);
	str += ss;

	if (bWithAnswer)
	{
		MakeAnswer(ss, MKASW_DEFAULT, 0);
		str += _T("&nbsp;&nbsp;&nbsp;&nbsp;(");
		str += ss;
		str += _T(")");
	}
	else
	{
		str += _T("&nbsp;&nbsp;&nbsp;&nbsp;(&nbsp;&nbsp&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;)");
	}

	if (m_DescImageList.GetDataCount())
	{
		MakeHTMLImage(fp, str, nItemIndex, pImagePara, !bWithAnswer);
	}

	str += _T("</li></ol>\n");

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_HTMLONLY);
	}
}

void CJudge::SetUserAnswer(BOOL bAnswer)
{
	m_bUserAnswer = bAnswer;
}

int CJudge::Score(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();

	if (m_bUserAnswer >= 0)
	{
		m_nAnswerCheck = (m_bUserAnswer == m_bAnswer) ? ALLRIGHT : WRONG;
	}
	else
	{
		m_nAnswerCheck = WRONG;
	}
	
	if (MARKTYPE_AVERAGE == pPara->nMarkType)
	{
		switch(m_nAnswerCheck)
		{
		case ALLRIGHT:
			return 100;
			
		default:
			return 0;
		}
	}
	else
	{
		switch(m_nAnswerCheck)
		{
		case ALLRIGHT:
			return pPara->nJdgPoints * 2;
			
		default:
			return 0;
		}
	}
}

void CJudge::SetU32Answer(DWORD dwAnswer)
{
	if (1 == dwAnswer || 0 == dwAnswer)
	{
		m_bAnswer = dwAnswer ? TRUE : FALSE;
	}
}

BOOL CJudge::IsSameItem(CItem *pItem, int nSimilar)
{
	if (!CItem::IsSameItem(pItem, nSimilar))
	{
		return FALSE;
	}

	return (m_bAnswer == ((CJudge*)pItem)->m_bAnswer);
}

LPTSTR CJudge::ClearEndQuote(LPCTSTR strDesc)
{
	ASSERT(strDesc);
#ifdef _UNICODE
	LPWSTR ptr = wcsdup(strDesc);
#else
	LPWSTR ptr = (LPWSTR)CTextManager::SwitchSave(strDesc, CS_UTF16, NULL);
#endif
	if (!ptr)
	{
		return NULL;
	}

	// 去除开头的空白
	int i = 0, nStart = 0, nEnd = 0;
	BOOL bOut = FALSE;
	while(ptr[i] && !bOut)
	{
		switch(ptr[i])
		{
		CASE_SPACE:
		CASE_RETURN:
			break;

		default:
			bOut = TRUE;
			nStart = i;
			continue;
		}
		++i;
	}

	// 去除末尾的空白与括号
	i = wcslen(ptr) - 1;
	bOut = FALSE;
	int nValid = -1;
	BOOL bFindLeft = FALSE;
	while(ptr[i] && !bOut)
	{
		switch(ptr[i])
		{
		CASE_SPACE:
		CASE_RETURN:
			break;

		CASE_LEFTBRACE:
			bFindLeft = TRUE;
			break;

		CASE_RIGHTBRACE:
			nValid = (nValid < 0) ? i : nValid;
			break;

		default:
			nValid = (nValid < 0) ? i : nValid;
			if (bFindLeft)
			{
				nEnd = i;
			}
			else
			{
				nEnd = nValid;
			}
			bOut = TRUE;
			continue;
		}
		--i;
	}

	// 除括号外没有其他字符
	if (nEnd <= nStart)
	{
		nEnd = nValid;
	}

	for (i = nStart; i <= nEnd; ++i)
	{
		ptr[i - nStart] = ptr[i];
	}
	ptr[i] = 0;
	
#ifdef _UNICODE
	return ptr;
#else
	LPTSTR ptrC = CTextManager::SwitchLoad((LPCSTR)ptr, CS_UTF16, NULL);
	free(ptr);
	if (!ptrC)
	{
		return NULL;
	}
	return ptrC;
#endif
}