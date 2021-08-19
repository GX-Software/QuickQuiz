// Text.cpp: implementation of the CText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "quickquiz.h"
#include "Text.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CText::CText() :
	m_strAnswer(NULL), m_strUserAnswer(NULL),
	m_nUserScore(0), m_nMaxScore(100)
{
	m_nType = TYPE_TEXT;
}

CText::~CText()
{
	ClearAnswer();
	ClearUserInfo();
}

LPCTSTR CText::GetTypeDesc()
{
	LPCTSTR pReturn = CItem::GetTypeDesc();
	if (pReturn)
	{
		return pReturn;
	}
	
	LoadString(AfxGetInstanceHandle(), IDS_ITEMTYPE_TXT, m_szTypeDesc, TYPE_BUFFSIZE);	
	return m_szTypeDesc;
}

BOOL CText::SetAnswer(LPCTSTR strAnswer, int nLen)
{
	ClearAnswer();

	if (!strAnswer)
	{
		return TRUE;
	}
	if (nLen < 0)
	{
		nLen = _tcslen(strAnswer);
	}

	m_strAnswer = (LPTSTR)malloc((nLen + 1) * sizeof(TCHAR));
	if (!m_strAnswer)
	{
		return FALSE;
	}

	return _tcscpyn(m_strAnswer, strAnswer, nLen + 1) ? TRUE : FALSE;
}

#ifndef _UNICODE
BOOL CText::SetAnswerW(LPCWSTR strAnswer, int nLen)
{
	int nSize = WideCharToMultiByte(CP_ACP, 0, strAnswer, nLen, NULL, 0, NULL, NULL);
	LPTSTR pStr = (LPTSTR)malloc((nSize + 1) * sizeof(TCHAR));
	if (!WideCharToMultiByte(CP_ACP, 0, strAnswer, nLen, pStr, nSize, NULL, NULL))
	{
		free(pStr);
		return FALSE;
	}
	pStr[nSize] = 0;
	
	BOOL bRet = SetAnswer(pStr, -1);
	free(pStr);
	return bRet;
}
#endif

int CText::GetAnswerParagraphs()
{
	int nRet = 0, i = 0;
	LPTSTR ptr = m_strAnswer;
	BOOL bFindReturn = TRUE;

	while(ptr[i])
	{
		switch(ptr[i])
		{
		case _T('\r'):
		case _T('\n'):
			bFindReturn = TRUE;
			break;

		default:
			if (bFindReturn)
			{
				++nRet;
				bFindReturn = FALSE;
			}
		}
		++i;
	}
	
	return nRet;
}

// 简答题存储格式
// ?Bytes：通用存储块
// 2Bytes：答案长度（字节计），包括末尾0
// ?Bytes：答案内容
EBOOL CText::LoadFile(FILE *fp, DWORD dwVersion)
{
	int nTemp = 0;
	LPWSTR ptr = NULL;

	if (!CItem::LoadFile(fp, dwVersion))
	{
		return Q_WARNING(ERROR_INNER);
	}

	// 答案长度
	if (!fread(&nTemp, sizeof(short), 1, fp))
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	ptr = (LPWSTR)malloc(nTemp);
	if (!ptr)
	{
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}

	if (!fread(ptr, nTemp, 1, fp))
	{
		free(ptr);
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

#ifdef _UNICODE
	SetAnswer(ptr, -1);
#else
	LPTSTR ptrC = CTextManager::SwitchLoad((LPCSTR)ptr, CS_UTF16, NULL);
	if (!ptrC)
	{
		free(ptr);
		return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
	}
	
	if (!SetAnswer(ptrC, -1))
	{
		free(ptrC);
		free(ptr);
		return Q_WARNING(ERROR_INNER);
	}
	free(ptrC);
#endif

	free(ptr);	
	return Q_RET(TRUE);
}

// 简答题存储格式
// ?Bytes：通用存储块
// 2Bytes：答案长度（字节计），包括末尾0
// ?Bytes：答案内容
EBOOL CText::SaveFile(FILE *fp)
{
	int nTemp;

	if (!CItem::SaveFile(fp))
	{
		return Q_ERROR(ERROR_INNER);
	}

#ifdef _UNICODE
	nTemp = (_tcslen(m_strAnswer) + 1) * sizeof(TCHAR);
	if (!fwrite(&nTemp, sizeof(short),1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	if (!fwrite(m_strAnswer, nTemp, 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
#else
	int nCount = MultiByteToWideChar(CP_ACP, 0, m_strAnswer, -1, NULL, 0);
	if (!nCount)
	{
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	nTemp = nCount * sizeof(WCHAR);
	if (!fwrite(&nTemp, sizeof(short), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	LPWSTR ptrW = (LPWSTR)malloc(nTemp);
	if (!ptrW)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	if (!MultiByteToWideChar(CP_ACP, 0, m_strAnswer, -1, ptrW, nCount))
	{
		free(ptrW);
		return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
	}
	if (!fwrite(ptrW, nTemp, 1, fp))
	{
		free(ptrW);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	free(ptrW);
#endif
	return Q_TRUE;
}

RETURN_E TiXmlElement* CText::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
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
	
	LPSTR lpString = CTextManager::SwitchSave(m_strAnswer, nEncode, NULL);
	if (!lpString)
	{
		return Q_ERROR_RET(ERROR_TEXT_TRANSCODING_FAIL, NULL);
	}
	TiXmlText *pText = new TiXmlText(lpString);
	free(lpString);
	if (!pText)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pAnswer->LinkEndChild(pText);
	
	return Q_TRUE_RET(pParent);
}

EBOOL CText::ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup)
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

	LPTSTR lpString = CTextManager::SwitchLoad(pAsw->GetText(), nEncode, NULL);
	if (!lpString)
	{
		return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
	}

	ClearAnswer();
	m_strAnswer = lpString;

	return Q_RET(TRUE);
}

void CText::ClearAllInfo()
{
	CItem::ClearAllInfo();
	
	ClearAnswer();
}

void CText::ClearUserInfo()
{
	CItem::ClearUserInfo();

	if (m_strUserAnswer)
	{
		free(m_strUserAnswer);
		m_strUserAnswer = NULL;
	}
}

CItem* CText::Alloc()
{
	CText *pText = new CText;
	if (!pText)
	{
		return NULL;
	}
	
	if (!pText->CopyItem(this))
	{
		delete pText;
		return NULL;
	}
	
	return (CItem*)pText;
}

BOOL CText::CopyItem(CItem *pItem)
{
	ClearAllInfo();
	
	if (!CItem::CopyItem(pItem))
	{
		return FALSE;
	}
	
	if (TYPE_TEXT == pItem->GetType())
	{
		return SetAnswer(((CText*)pItem)->GetAnswer(), -1);
	}
	
	return TRUE;
}

void CText::ClipCopy(CFixedSharedFile &sf)
{
	CItem::ClipCopy(sf);
	
	int nLen;
	
	nLen = _tcslen(m_strAnswer);
	sf.Write(&nLen, sizeof(nLen));
	
	sf.Write(m_strAnswer, nLen * sizeof(TCHAR));
}

BOOL CText::ClipPaste(CFixedSharedFile &sf)
{
	if (!CItem::ClipPaste(sf))
	{
		return FALSE;
	}
	
	int nLen;
	CString str;
	
	sf.Read(&nLen, sizeof(nLen));
	
	sf.Read(str.GetBufferSetLength(nLen), nLen * sizeof(TCHAR));
	
	ClearAnswer();
	m_strAnswer = _tcsdup(str);
	if (!m_strAnswer)
	{
		return FALSE;
	}
	
	return TRUE;
}

void CText::MakeText(_stdstring &str, UINT nStyle, int nPara)
{
	str = m_strDescription;

	if (nStyle & MKTXT_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CText::MakeAnswer(_stdstring &str, UINT nStyle, int nPara)
{
	ASSERT(m_strAnswer);

	if (nStyle & MKASW_ASWINQUESTION)
	{
		str = m_strDescription;
		str += CTextManager::s_szWinReturn;
	}
	else
	{
		str.erase();
	}

	str += m_strAnswer;

	if (MKASW_WITHRESOLVE & nStyle)
	{
		str += CTextManager::s_szWinReturn;
		str += GetResolve();
	}
	if (nStyle & MKASW_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CText::CheckSimilarity(CTextSimilarity *pSim)
{
	pSim->ClearInfo();

	pSim->Compare(m_strAnswer, m_strUserAnswer);
}

void CText::MakeTextOut(_stdstring &str,
						int nListIndex,
						int nItemIndex,
						PTPARAS pImagePara,
						BOOL bWithAnswer,
						BOOL bWithRes)
{
	int nCount = GetAnswerLineCount();

	CItem::MakeTextOut(str, nListIndex, nItemIndex, pImagePara,
		bWithAnswer, bWithRes);

	if (bWithAnswer)
	{
		TCHAR szAsw[64] = {0};
		
		LoadString(AfxGetInstanceHandle(), IDS_QUIZMKR_ASWTXT, szAsw, 64);
		str += szAsw;
		str += GetAnswer();
		str += CTextManager::s_szWinReturn;

		if (bWithRes)
		{
			MakeResolve(str, PAPERTYPE_TEXTONLY);
		}
	}
	else
	{
		while(nCount--)
		{
			str += CTextManager::s_szWinReturn;
		}
	}
}

void CText::MakeHTML(FILE *fp,
					 _stdstring &str,
					 int nListIndex,
					 int nItemIndex,
					 PTPARAS pImagePara,
					 BOOL bWithAnswer,
					 BOOL bWithRes)
{
	TCHAR sz[64] = {0};
	int nCount = GetAnswerLineCount();
	int i;

	CItem::MakeHTML(fp, str, nListIndex, nItemIndex, pImagePara,
		bWithAnswer, bWithRes);
	str += _T("<div class=\"p\">");
	
	if (bWithAnswer)
	{
		TCHAR szAsw[64] = {0};
		_stdstring temp;
		
		LoadString(AfxGetInstanceHandle(), IDS_ITEM_TEXTASW, szAsw, 64);
		temp += _T("<span style=\"font-weight:bold;\">");
		temp += szAsw;
		temp += _T("</span>");
		temp += GetAnswer();

		ReplaceHTMLEndLine(temp);
		str += temp;
	}
	else
	{
		for (i = 0; i < nCount; ++i)
		{
			str += _T("<br/>");
		}
	}
	str += _T("</div>\n");

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_HTMLONLY);
	}
}

void CText::SetUserAnswer(LPCTSTR strUserAnswer)
{
	ClearUserInfo();

	m_strUserAnswer = _tcsdup(strUserAnswer);
}

LPTSTR CText::GetUserAnswer()
{
	if (m_strUserAnswer)
	{
		return m_strUserAnswer;
	}
	else
	{
		return (LPTSTR)CItem::s_strNull;
	}
}

int CText::Score(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();
	
	if (pPara->dwTxtOptions & TXTOPTION_ALLRIGHT)
	{
		AutoScore(pCPara);
	}
	else
	{
		if (pPara->nMarkType & MARKTYPE_AVERAGE)
		{
			m_nMaxScore = 100;
		}
		else
		{
			m_nMaxScore = pPara->nTxtPoints;
		}

		if (!m_nUserScore)
		{
			m_nAnswerCheck = WRONG;
		}
		else if (m_nUserScore == m_nMaxScore)
		{
			m_nAnswerCheck = ALLRIGHT;
		}
		else
		{
			m_nAnswerCheck = HALFRIGHT;
		}
	}
	
	return m_nUserScore;
}

int CText::AutoScore(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();
	
	if (!lstrcmpi(m_strAnswer, m_strUserAnswer))
	{
		m_nAnswerCheck = ALLRIGHT;
		m_nUserScore = (pPara->nMarkType & MARKTYPE_AVERAGE) ?
			100 : pPara->nTxtPoints;
	}
	else
	{
		m_nAnswerCheck = WRONG;
		m_nUserScore = 0;
	}

	return m_nUserScore;
}

BOOL CText::IsAnswered()
{
	if (m_strUserAnswer && _tcslen(m_strUserAnswer))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CText::Match(LPCTSTR strMatch, BOOL bMatchCase)
{
	if (CItem::Match(strMatch, bMatchCase))
	{
		return TRUE;
	}

	int bRet;
	
	if (bMatchCase)
	{
		bRet = (int)_tcsstr(m_strAnswer, strMatch);
	}
	else
	{
		bRet = (int)_tcsstri(m_strAnswer, strMatch);
	}
	
	return bRet ? TRUE : FALSE;
}

void CText::CalcCRC()
{
	CItem::CalcCRC();

#ifdef _UNICODE
	PBYTE ptr = (PBYTE)m_strAnswer;
	int nLen = lstrlen(m_strAnswer) * sizeof(TCHAR);
#else
	int nLen = (lstrlen(m_strAnswer) + 1) * sizeof(WCHAR);
	LPWSTR str = (LPWSTR)malloc(nLen);
	if (!str)
	{
		return;
	}
	if (!MultiByteToWideChar(CP_ACP, 0, m_strAnswer, -1, str, nLen))
	{
		free(str);
		return;
	}
	PBYTE ptr = (PBYTE)str;
	nLen = wcslen(str) * sizeof(WCHAR);
#endif
	
	int i;
	for (i = 0; i < nLen; i++)
	{
		m_wCRC += ptr[i];
	}

#ifndef _UNICODE
	free(str);
#endif
}

BOOL CText::IsSameItem(CItem *pItem, int nSimilar)
{
	if (!CItem::IsSameItem(pItem, nSimilar))
	{
		return FALSE;
	}

	if (nSimilar > 99)
	{
		if (_tcscmp(m_strAnswer, ((CText*)pItem)->m_strAnswer))
		{
			return FALSE;
		}
	}
	else
	{
		CTextSimilarity t;
		t.Compare(m_strAnswer, ((CText*)pItem)->m_strAnswer);
		if ((int)(t.GetInfo() * 100.f) < nSimilar)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void CText::ClearAnswer()
{
	if (m_strAnswer)
	{
		free(m_strAnswer);
		m_strAnswer = NULL;
	}
}

int CText::GetAnswerLineCount()
{
	int nCount = CTextManager::GetCharactersCount(m_strAnswer);
	int i = 0;

	// 每行可以写20个字符
	nCount = (nCount + 19) / 20;

	while(m_strAnswer[i])
	{
		if (m_strAnswer[i] == _T('\n'))
		{
			++nCount;
		}
		++i;
	}

	nCount = nCount * 3 / 2;
	if (nCount < 10)
	{
		nCount = 10;
	}

	return nCount;
}