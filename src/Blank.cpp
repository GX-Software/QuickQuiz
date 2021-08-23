// Blank.cpp: implementation of the CBlank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Blank.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GROWSTEP		4

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlank::CBlank() :
	m_nBlanksCount(0), m_pValidBlanks(NULL), m_nValidBlanksCount(0),
	m_pBlanks(NULL), m_nBlankListSize(0),
	m_nUserAnswerListSize(0), m_nUserAnswerCount(0), m_pUserAnswers(NULL),
	m_dwErrorCode(ERROR_OK)
{
	m_nType = TYPE_BLANK;
}

CBlank::~CBlank()
{
	if (m_pBlanks)
	{
		ClearAllInfo();
		free(m_pBlanks);
	}

	if (m_pUserAnswers)
	{
		ClearUserInfo();
		free(m_pUserAnswers);
	}

	if (m_pValidBlanks)
	{
		free(m_pValidBlanks);
	}
}

LPCTSTR CBlank::GetTypeDesc()
{
	LPCTSTR pReturn = CItem::GetTypeDesc();
	if (pReturn)
	{
		return pReturn;
	}
	
	LoadString(AfxGetInstanceHandle(), IDS_ITEMTYPE_BLK, m_szTypeDesc, TYPE_BUFFSIZE);	
	return m_szTypeDesc;
}

void CBlank::ChangeAsPara(CTestPara *pCPara, int nExPara /* = 0 */)
{
	PTESTPARA pPara = pCPara->GetTestPara();

	if (pPara->dwBlkOptions & BLKOPTION_ONLYBLK)
	{
		SetBlankPara(BLANKPARA_ONLYBLANK);
	}
	else if (pPara->dwBlkOptions & BLKOPTION_RANDOMBLK)
	{
		SetBlankPara(BLKOPTION_RANDOMBLK);
	}
}

int CBlank::AddBlank(int nStart, int nEnd, LPCTSTR strBlank /* = NULL */)
{
	if (m_nBlanksCount >= MAXBLANKS)
	{
		m_dwErrorCode = ERROR_REACHMAX;
		return (-1);
	}

	// 要求必须已经设置好了题干才能添加空
	if (!m_strDescription)
	{
		m_dwErrorCode = ERROR_NODESCRIPTION;
		return (-1);
	}

	// 检查选区是否有冲突
	if (!CheckRanges(nStart, nEnd))
	{
		m_dwErrorCode = ERROR_RANGECONFLICT;
		return (-1);
	}

	// 扩大答案列表
	if (!SetBlankListSize(m_nBlanksCount + 1))
	{
		return (-1);
	}

	// 将题干转化为UNICODE类型
	int nBuffLen = 0;
	LPWSTR wstrDescription = GetWDescription(nBuffLen);
	if (!wstrDescription)
	{
		return (-1);
	}

	LPTSTR strTemp = _tcsdup(strBlank);
	LPTSTR strAsw = ClearSpaces(strTemp);
	// 如果没有答案，则需要从选区中获取
	if(!strAsw)
	{
		free(strTemp);
		strTemp = GetBlankText(wstrDescription, nStart, nEnd);
		//  错误代码在函数中完成
		if (!strTemp)
		{
			m_dwErrorCode = ERROR_NOANSWER;
			free(wstrDescription);
			return (-1);
		}
		strAsw = ClearSpaces(strTemp);
		if (!strAsw)
		{
			m_dwErrorCode = ERROR_NOANSWER;
			free(wstrDescription);
			return (-1);
		}
	}

	// 添加选区
	int nOffset = MakeQuote(wstrDescription, nStart, nEnd);
	// 在这里添加了空的数量
	int nInsertPos = InsertPos(nStart, nEnd, nOffset);

	// 保存这个选项
	m_pBlanks[nInsertPos].nStart = nStart;
	m_pBlanks[nInsertPos].nEnd = nEnd;
	m_pBlanks[nInsertPos].pBlank = _tcsdup(strAsw);
	m_pBlanks[nInsertPos].bCheck = FALSE;
	if (!m_pBlanks[nInsertPos].pBlank)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		free(strTemp);
		free(wstrDescription);
		DelBlank(nInsertPos, FALSE);
		return (-1);
	}
	else
	{
		CTextManager::RemoveReturn(m_pBlanks[nInsertPos].pBlank, 0);
	}
	free(strTemp);

	// 重新设定题目（因为添加了空格）
	// 到这里，wstrDescription就不能再使用了，因为在这个函数里可能已经被释放掉了
	LPTSTR strDescription = GetTDescription(wstrDescription);
	if (!strDescription)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		DelBlank(nInsertPos, FALSE);
		return (-1);
	}
	if (!CItem::SetDescription(strDescription))
	{
		m_dwErrorCode = ERROR_NODESCRIPTION;
		DelBlank(nInsertPos, FALSE);
		free(strDescription);
		return (-1);
	}
	free(strDescription);

	// 在修改之后，所有的空都是有效空
	SetBlankPara();

	m_dwErrorCode = ERROR_OK;
	return nInsertPos;
}

BOOL CBlank::EditBlank(int nIndex, LPCTSTR strBlank, int nLen)
{
	if (nIndex >= m_nBlanksCount || nIndex < 0)
	{
		m_dwErrorCode = ERROR_INDEXOVERFLOW;
		return FALSE;
	}
	
	if (nLen < 0)
	{
		nLen = lstrlen(strBlank);
	}
	LPTSTR strTemp = (LPTSTR)malloc((nLen + 1) * sizeof(TCHAR));
	if (!strTemp)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}
	_tcscpyn(strTemp, strBlank, nLen + 1);
	LPTSTR str = ClearSpaces(strTemp);
	if (!str)
	{
		free(strTemp);
		m_dwErrorCode = ERROR_NOANSWER;
		return FALSE;
	}

	free(m_pBlanks[nIndex].pBlank);
	m_pBlanks[nIndex].pBlank = _tcsdup(str);
	free(strTemp);

	if (!m_pBlanks[nIndex].pBlank)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}

	// 在修改之后，所有的空都是有效空
	SetBlankPara();

	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

#ifndef _UNICODE
BOOL CBlank::EditBlankW(int nIndex, LPCWSTR strBlank, int nLen)
{
	int nSize = WideCharToMultiByte(CP_ACP, 0, strBlank, nLen, NULL, 0, NULL, NULL);
	LPTSTR pStr = (LPTSTR)malloc((nSize + 1) * sizeof(TCHAR));
	if (!WideCharToMultiByte(CP_ACP, 0, strBlank, nLen, pStr, nSize, NULL, NULL))
	{
		free(pStr);
		return FALSE;
	}
	pStr[nSize] = 0;

	BOOL bRet = EditBlank(nIndex, pStr, -1);
	free(pStr);
	return bRet;
}
#endif

BOOL CBlank::DelBlank(int nIndex, BOOL bFillAnswer)
{
	if (nIndex >= m_nBlanksCount || nIndex < 0)
	{
		m_dwErrorCode = ERROR_INDEXOVERFLOW;
		return FALSE;
	}

	LPTSTR strTemp = (LPTSTR)malloc((lstrlen(m_strDescription) + lstrlen(m_pBlanks[nIndex].pBlank) + 1) * sizeof(TCHAR));
	if (!strTemp)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}

	_tcscpyn(strTemp, m_strDescription, CTextManager::GetRealIndex(m_strDescription, m_pBlanks[nIndex].nStart) + 1);
	if (bFillAnswer)
	{
		_tcscat(strTemp, m_pBlanks[nIndex].pBlank);
	}
	_tcscat(strTemp, m_strDescription + CTextManager::GetRealIndex(m_strDescription, m_pBlanks[nIndex].nEnd));

	if (!CItem::SetDescription(strTemp))
	{
		free(strTemp);
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}
	free(strTemp);

	// 调整空的位置
	int i;
#ifdef _UNICODE
	int nOffset = (bFillAnswer ? lstrlen(m_pBlanks[nIndex].pBlank) : 0) - 4;
#else
	int nOffset = MultiByteToWideChar(CP_ACP, 0, m_pBlanks[nIndex].pBlank, -1, NULL, 0);
	if (!nOffset)
	{
		m_dwErrorCode = ERROR_TRANSCODEFAIL;
		return FALSE;
	}
	nOffset = (bFillAnswer ? nOffset : 1) - 5;
#endif
	
	free(m_pBlanks[nIndex].pBlank);
	for (i = nIndex; i < m_nBlanksCount - 1; i++)
	{
		m_pBlanks[i].nStart = m_pBlanks[i + 1].nStart + nOffset;
		m_pBlanks[i].nEnd = m_pBlanks[i + 1].nEnd + nOffset;
		m_pBlanks[i].pBlank = m_pBlanks[i + 1].pBlank;
		m_pBlanks[i].bCheck = m_pBlanks[i + 1].bCheck;
	}
	m_pBlanks[i].pBlank = NULL;

	m_nBlanksCount--;

	// 在修改之后，所有的空都是有效空
	SetBlankPara();

	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

void CBlank::SetBlankPara(int nPara /* = BLANKPARA_NORMAL */)
{
	int i, j;
	int nOffset = 0;

	if (m_pValidBlanks)
	{
		free(m_pValidBlanks);
		m_pValidBlanks = NULL;
	}
	m_nValidBlanksCount = 0;

	switch(nPara)
	{
	case BLANKPARA_NORMAL:
		break;

	case BLANKPARA_RANDOMBLANK:
		m_pValidBlanks = (PBLANK)malloc(m_nBlanksCount * sizeof(BLANK));
		if (!m_pValidBlanks)
		{
			return;
		}
		srand(time(NULL) + _getpid());
		for (i = 0; i < m_nBlanksCount; i++)
		{
			j = rand() % 2;
			if (j)
			{
				m_pValidBlanks[m_nValidBlanksCount].nStart = m_pBlanks[i].nStart + nOffset;
				m_pValidBlanks[m_nValidBlanksCount].nEnd = m_pBlanks[i].nEnd + nOffset;
				m_pValidBlanks[m_nValidBlanksCount].pBlank = m_pBlanks[i].pBlank;
				m_pValidBlanks[m_nValidBlanksCount].bCheck = FALSE;
				m_nValidBlanksCount++;
			}
			else
			{
#ifdef _UNICODE
				nOffset += (lstrlen(m_pBlanks[i].pBlank) - 6);
#else
				nOffset += (MultiByteToWideChar(CP_ACP, 0, m_pBlanks[i].pBlank, -1, NULL, 0) - 7);
#endif
			}
		}

		if (m_nValidBlanksCount)
		{
			break;
		}
		else
		{
			// 不跳出，直接开始下一个case
			m_nValidBlanksCount = 1;
			nOffset = 0;
		}

	case BLANKPARA_ONLYBLANK:
		if (!m_nValidBlanksCount)
		{
			m_pValidBlanks = (BLANK*)malloc(sizeof(BLANK));
			if (!m_pValidBlanks)
			{
				return;
			}
		}
		srand(time(NULL) + _getpid());
		j = rand() % m_nBlanksCount;
		for (i = 0; i < j; i++)
		{
#ifdef _UNICODE
			nOffset += (lstrlen(m_pBlanks[i].pBlank) - 6);
#else
			nOffset += (MultiByteToWideChar(CP_ACP, 0, m_pBlanks[i].pBlank, -1, NULL, 0) - 7);
#endif
		}
		m_pValidBlanks[0].nStart = m_pBlanks[i].nStart + nOffset;
		m_pValidBlanks[0].nEnd = m_pBlanks[i].nEnd + nOffset;
		m_pValidBlanks[0].pBlank = m_pBlanks[i].pBlank;
		m_pValidBlanks[0].bCheck = FALSE;
		m_nValidBlanksCount = 1;
		break;

	default:
		ASSERT(FALSE);
	}
}

LPCTSTR CBlank::GetBlank(int nIndex)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	
	if (!pBlank || nIndex >= nCount)
	{
		return NULL;
	}
	
	return pBlank[nIndex].pBlank ? pBlank[nIndex].pBlank : s_strNull;
}

void CBlank::GetBlankRange(int nIndex, int *nStart, int *nEnd)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	
	if (!pBlank || nIndex >= nCount)
	{
		return;
	}
	
	if (nStart)
	{
		*nStart = pBlank[nIndex].nStart;
	}
	if (nEnd)
	{
		*nEnd = pBlank[nIndex].nEnd;
	}
}

int CBlank::IsBlank(int nStart, int nEnd)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	
	int i;
	for (i = 0; i < nCount; i++)
	{
		if (nStart >= pBlank[i].nStart && nEnd <= pBlank[i].nEnd)
		{
			return i;
		}
	}
	
	return (-1);
}

// 填空题存储格式
// ?Bytes：通用存储块
// 4Bytes：读取需要的最大缓冲区字节数
// 1Bytes：空白数
// 2Bytes：某答案空白开始
// 2Bytes：某答案空白结束
// 2Bytes：某答案长度（字节计），包括末尾0
// ?Bytes：答案
EBOOL CBlank::LoadFile(FILE *fp, DWORD dwVersion)
{
	int nMax = 0, nTemp = 0, i;
	LPWSTR ptr = NULL;

	if (!CItem::LoadFile(fp, dwVersion))
	{
		return Q_WARNING(ERROR_INNER);
	}
	
	// 最大缓冲字节数
	if (!fread(&nMax, sizeof(int), 1, fp))
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}
	ptr = (LPWSTR)malloc(nMax);
	if (!ptr)
	{
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}

#ifndef _UNICODE
	// 全部题库按UNICODE存储
	LPTSTR ptrC = (LPTSTR)malloc(nMax);
	if (!ptrC)
	{
		free(ptr);
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}
#endif

	ClearAllBlanks();
	// 空白数
	m_nBlanksCount = 0;
	if (!fread(&m_nBlanksCount, sizeof(BYTE), 1, fp))
	{
		free(ptr);
#ifndef _UNICODE
		free(ptrC);
#endif
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	if (!SetBlankListSize(m_nBlanksCount))
	{
		return Q_WARNING(ERROR_INNER);
	}

	// 读各答案
	for (i = 0; i < m_nBlanksCount; i++)
	{
		m_pBlanks[i].nStart = m_pBlanks[i].nEnd = 0;
		// 空白的起始和结束
		if (!fread(&m_pBlanks[i].nStart, sizeof(SHORT), 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		if (!fread(&m_pBlanks[i].nEnd, sizeof(SHORT), 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}

		// 答案长度
		nTemp = 0;
		if (!fread(&nTemp, sizeof(short), 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		if (nTemp > nMax)
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCORRECT);
		}

		// 答案内容
		if (!fread(ptr, nTemp, 1, fp))
		{
			free(ptr);
#ifndef _UNICODE
			free(ptrC);
#endif
			return Q_WARNING(ERROR_FILE_INCOMPLETE);
		}
		
		// 添加选项
#ifdef _UNICODE
		m_pBlanks[i].pBlank = _tcsdup(ptr);
#else
		if (!WideCharToMultiByte(CP_ACP, 0, ptr, -1, ptrC, nTemp, NULL, FALSE))
		{
			free(ptr);
			free(ptrC);
			return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
		}
		m_pBlanks[i].pBlank = _tcsdup(ptrC);
#endif
	}

	free(ptr);
#ifndef _UNICODE
	free(ptrC);
#endif

	return Q_RET(TRUE);
}

// 填空题存储格式
// ?Bytes：通用存储块
// 4Bytes：读取需要的最大缓冲区字节数
// 1Bytes：空白数
// 2Bytes：某答案空白开始
// 2Bytes：某答案空白结束
// 2Bytes：某答案长度（字节计），包括末尾0
// ?Bytes：答案
EBOOL CBlank::SaveFile(FILE *fp)
{
	int i, nMaxLen, nTemp;

	if (!CItem::SaveFile(fp))
	{
		return Q_ERROR(ERROR_INNER);
	}

#ifdef _UNICODE
	nMaxLen = 0;
	// 找出读取需要的最大缓冲区大小
	for(i = 0; i < m_nBlanksCount; i++)
	{
		nTemp = (lstrlen(m_pBlanks[i].pBlank) + 1) * sizeof(TCHAR);
		nMaxLen = max(nMaxLen, nTemp);
	}
	if (!fwrite(&nMaxLen, sizeof(int), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	// 空白数
	if (!fwrite(&m_nBlanksCount, sizeof(BYTE), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}
	
	for (i = 0; i < m_nBlanksCount; i++)
	{
		// 空白的起始和结束
		if (!fwrite(&m_pBlanks[i].nStart, sizeof(SHORT), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		if (!fwrite(&m_pBlanks[i].nEnd, sizeof(SHORT), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}

		// 答案长度
		nTemp = (lstrlen(m_pBlanks[i].pBlank) + 1) * sizeof(TCHAR);
		if (!fwrite(&nTemp, sizeof(short), 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		
		// 选项内容
		if (!fwrite(m_pBlanks[i].pBlank, nTemp, 1, fp))
		{
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}
#else
	nMaxLen = 0;
	for(i = 0; i < m_nBlanksCount; i++)
	{
		nTemp = MultiByteToWideChar(CP_ACP, 0, m_pBlanks[i].pBlank, -1, NULL, 0);
		nMaxLen = max(nMaxLen, nTemp);
	}
	nTemp = nMaxLen * sizeof(WCHAR);
	if (!fwrite(&nTemp, sizeof(int), 1, fp))
	{
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	LPWSTR ptrW = (LPWSTR)malloc(nTemp);
	if (!ptrW)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}
	
	// 空白数
	if (!fwrite(&m_nBlanksCount, sizeof(BYTE), 1, fp))
	{
		free(ptrW);
		return Q_ERROR(ERROR_WRITE_FILE_FAIL);
	}

	for (i = 0; i < m_nBlanksCount; i++)
	{
		// 空白的起始和结束
		if (!fwrite(&m_pBlanks[i].nStart, sizeof(SHORT), 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
		if (!fwrite(&m_pBlanks[i].nEnd, sizeof(SHORT), 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}

		// 答案长度
		nTemp = MultiByteToWideChar(CP_ACP, 0, m_pBlanks[i].pBlank, -1, ptrW, nMaxLen) * sizeof(WCHAR);
		if (!fwrite(&nTemp, sizeof(short), 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_TEXT_TRANSCODING_FAIL);
		}
		
		// 答案内容
		if (!fwrite(ptrW, nTemp, 1, fp))
		{
			free(ptrW);
			return Q_ERROR(ERROR_WRITE_FILE_FAIL);
		}
	}

	free(ptrW);
#endif

	return Q_TRUE;
}

RETURN_E TiXmlElement* CBlank::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
{
	pParent = CItem::WriteXML(pParent, nEncode, bMaxCompatible);
	if (!pParent)
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}
	
	TiXmlText *pText = NULL;
	TiXmlElement *pBlank = NULL;
	LPSTR lpString = NULL;
	
	int i;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		pBlank = new TiXmlElement(xml_lpszNode_Blank);
		if (!pBlank)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pParent->LinkEndChild(pBlank);
		pBlank->SetAttribute(xml_lpszAttribute_Begin, m_pBlanks[i].nStart);
		pBlank->SetAttribute(xml_lpszAttribute_End, m_pBlanks[i].nEnd);
		
		lpString = CTextManager::SwitchSave(m_pBlanks[i].pBlank, nEncode, NULL);
		if (!lpString)
		{
			return Q_ERROR_RET(ERROR_TEXT_TRANSCODING_FAIL, NULL);
		}
		pText = new TiXmlText(lpString);
		free(lpString);
		if (!pText)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pBlank->LinkEndChild(pText);
	}
	
	return Q_TRUE_RET(pParent);
}

EBOOL CBlank::ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup)
{
	// 对于填空题，无论如何都必须包含题干
	if (!CItem::ReadXML(pQstn, nEncode, FALSE))
	{
		return Q_WARNING(ERROR_INNER);
	}

	LPTSTR lpString = NULL;

	// 检查读入的空位置
	LPWSTR lpDescCheck = (LPWSTR)CTextManager::SwitchSave(m_strDescription, nEncode, NULL);
	if (!lpDescCheck)
	{
		return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
	}
	int nLen = wcslen(lpDescCheck);
	free(lpDescCheck);

	ClearAllBlanks();
	int nStart, nEnd, nIndex;
	TiXmlElement *pNode;

	pNode = pQstn->FirstChildElement(xml_lpszNode_Blank);
	while(pNode)
	{
		if (!pNode->Attribute(xml_lpszAttribute_Begin, &nStart) ||
			!pNode->Attribute(xml_lpszAttribute_End, &nEnd))
		{
			break;
		}

		// 检查范围是否有效
		if (!CheckRanges(nStart, nEnd) || nStart >= nLen || nEnd >= nLen)
		{
			break;
		}

		// 扩展空列表
		if (!SetBlankListSize(m_nBlanksCount + 1))
		{
			break;
		}
		nIndex = InsertPos(nStart, nEnd, 0);
		
		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (!lpString)
		{
			break;
		}
		CTextManager::RemoveReturn(lpString, 0);
		m_pBlanks[nIndex].nStart = nStart;
		m_pBlanks[nIndex].nEnd = nEnd;
		m_pBlanks[nIndex].pBlank = lpString;

		pNode = pNode->NextSiblingElement(xml_lpszNode_Blank);
	}
	
	return Q_RET(m_nBlanksCount > 0);
}

BOOL CBlank::SetDescription(LPCTSTR szDescription)
{
	if (m_nBlanksCount)
	{
		return TRUE;
	}

	return CItem::SetDescription(szDescription);
}

void CBlank::ClearAllInfo()
{
	CItem::ClearAllInfo();

	ClearAllBlanks();
}

void CBlank::ClearUserInfo()
{
	CItem::ClearUserInfo();
	
	int i;
	for (i = 0; i < m_nUserAnswerListSize; i++)
	{
		if (m_pUserAnswers[i])
		{
			free(m_pUserAnswers[i]);
			m_pUserAnswers[i] = NULL;
		}
	}
	m_nUserAnswerCount = 0;
}

CItem* CBlank::Alloc()
{
	CBlank *pBlank = new CBlank;
	if (!pBlank)
	{
		return NULL;
	}
	
	if (!pBlank->CopyItem(this))
	{
		delete pBlank;
		return NULL;
	}
	return (CItem*)pBlank;
}

BOOL CBlank::CopyItem(CItem *pItem)
{
	ClearAllInfo();
	
	if (!CItem::CopyItem(pItem))
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}
	
	// 只有在题目类型相符的时候才会复制选项
	if (TYPE_BLANK == pItem->GetType())
	{
		return GetInfo((CBlank*)pItem, FALSE);
	}
	
	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

void CBlank::ClipCopy(CFixedSharedFile &sf)
{
	CItem::ClipCopy(sf);
	
	int nLen, i;
	
	sf.Write(&m_nBlanksCount, sizeof(m_nBlanksCount));
	
	for (i = 0; i < m_nBlanksCount; i++)
	{
		sf.Write(&m_pBlanks[i].nStart, sizeof(m_pBlanks[i].nStart));
		sf.Write(&m_pBlanks[i].nEnd, sizeof(m_pBlanks[i].nEnd));

		nLen = lstrlen(m_pBlanks[i].pBlank);
		
		sf.Write(&nLen, sizeof(nLen));
		sf.Write(m_pBlanks[i].pBlank, nLen * sizeof(TCHAR));
	}
}

BOOL CBlank::ClipPaste(CFixedSharedFile &sf)
{
	if (!CItem::ClipPaste(sf))
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}
	
	int i, j;
	CString str;

	ClearAllBlanks();
	
	sf.Read(&m_nBlanksCount, sizeof(m_nBlanksCount));

	if (!SetBlankListSize(m_nBlanksCount))
	{
		return FALSE;
	}

	for(i = 0; i < m_nBlanksCount; i++)
	{
		sf.Read(&m_pBlanks[i].nStart, sizeof(m_pBlanks[i].nStart));
		sf.Read(&m_pBlanks[i].nEnd, sizeof(m_pBlanks[i].nEnd));

		sf.Read(&j, sizeof(j));
		sf.Read(str.GetBufferSetLength(j), j * sizeof(TCHAR));
		m_pBlanks[i].pBlank = _tcsdup(str);
		if (!m_pBlanks[i].pBlank)
		{
			m_dwErrorCode = ERROR_ALLOCFAIL;
			return FALSE;
		}
	}

	SetBlankPara();
	return TRUE;
}

BOOL CBlank::GetInfo(CBlank *pBlank, BOOL bWithDescription)
{
	ASSERT(TYPE_BLANK == ((CItem*)pBlank)->GetType());

	if (bWithDescription)
	{
		ClearAllInfo();
		SetDescription(pBlank->m_strDescription);
	}

	if (!SetBlankListSize(pBlank->m_nBlanksCount))
	{
		return FALSE;
	}
	
	int i;
	for (i = 0; i < pBlank->m_nBlanksCount; i++)
	{
		m_pBlanks[i].nStart = pBlank->m_pBlanks[i].nStart;
		m_pBlanks[i].nEnd = pBlank->m_pBlanks[i].nEnd;
		m_pBlanks[i].pBlank = _tcsdup(pBlank->m_pBlanks[i].pBlank);
		m_pBlanks[i].bCheck = FALSE;
		if (!m_pBlanks[i].pBlank)
		{
			ClearAllInfo();
			m_dwErrorCode = ERROR_ALLOCFAIL;
			return FALSE;
		}
	}
	
	m_nBlanksCount = pBlank->m_nBlanksCount;
	SetBlankPara();

	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

void CBlank::MakeText(_stdstring &str, UINT nStyle, int nPara)
{
	str = m_strDescription;
	if (m_pValidBlanks)
	{
		int i, j;
		int nOffset = 0;
#ifndef _UNICODE
		int idxInsert = 0;
#endif
		for (i = 0; i < m_nBlanksCount; i++)
		{
			for (j = 0; j < m_nValidBlanksCount; j++)
			{
				if (m_pValidBlanks[j].pBlank == m_pBlanks[i].pBlank)
				{
					break;
				}
			}
			
			// 这个空不是有效的
			if (j == m_nValidBlanksCount)
			{
#ifdef _UNICODE
				str.erase(m_pBlanks[i].nStart + nOffset - 1, 6);
				str.insert(m_pBlanks[i].nStart + nOffset - 1, m_pBlanks[i].pBlank);
				nOffset += (lstrlen(m_pBlanks[i].pBlank) - 6);
#else
				idxInsert = CTextManager::GetRealIndex(m_strDescription, m_pBlanks[i].nStart);
				str.erase(idxInsert + nOffset - 1, 6);
				str.insert(idxInsert + nOffset - 1, m_pBlanks[i].pBlank);
				nOffset += (lstrlen(m_pBlanks[i].pBlank) - 6);
#endif
			}
		}
	}

	if (nStyle & MKTXT_ENDLINE)
	{
		str += CTextManager::s_szWinReturn;
	}
}

void CBlank::MakeAnswer(_stdstring &str, UINT nStyle, int nPara /* = 0 */)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	
	if (nPara < 0 || nPara > nCount)
	{
		nPara = nCount;
	}

	if (nStyle & MKASW_ASWINQUESTION)
	{
		FillAnswer(str, TRUE, nPara);

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
	else
	{
		int i;
		TCHAR Index[8] = {0};
		str.erase();
		for (i = 0; i < nPara; i++)
		{
			if (nStyle & MKASW_WITHINDEX)
			{
				_stprintf(Index, _T("(%d) "), i + 1);
				str += Index;
			}

			if (nStyle & MKASW_INALINE)
			{
				str += _T("( ");
			}

			str += pBlank[i].pBlank;

			if (nStyle & MKASW_INALINE)
			{
				str += _T(" )");
			}

			if (i < nPara - 1)
			{
				if (nStyle & MKASW_INALINE)
				{
					str += _T(" ");
				}
				else
				{
					str += CTextManager::s_szWinReturn;
				}
			}
		}
	}
}

void CBlank::CheckSimilarity(CTextSimilarity *pSim)
{
	pSim->ClearInfo();

	int nCount = m_pValidBlanks ? m_nValidBlanksCount : m_nBlanksCount;
	BLANK* pBlank = m_pValidBlanks ? m_pValidBlanks : m_pBlanks;
	
	int i;
	for (i = 0; i < nCount; i++)
	{
		pSim->Compare(m_pUserAnswers[i], pBlank[i].pBlank);
	}
}

BOOL CBlank::CheckItemValid()
{
	return m_nBlanksCount ? TRUE : FALSE;
}

void CBlank::MakeTextOut(_stdstring &str,
						 int nListIndex,
						 int nItemIndex,
						 PTPARAS pImagePara,
						 BOOL bWithAnswer,
						 BOOL bWithRes)
{
	_stdstring tmp;
	TCHAR sz[64] = {0};

	_sntprintf(sz, 64, _T("%d. "), nItemIndex);
	str += sz;

	if (bWithAnswer)
	{
		MakeAnswer(tmp, MKASW_ASWINQUESTION | MKASW_ENDLINE, -1);
	}
	else
	{
		MakeText(tmp, MKTXT_DEFAULT, 0);
	}
	str += tmp;

	MakeTextImage(str, nItemIndex, pImagePara, !bWithAnswer);
	str += CTextManager::s_szWinReturn;

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_TEXTONLY);
	}
}

void CBlank::MakeHTML(FILE *fp,
					  _stdstring &str,
					  int nListIndex,
					  int nItemIndex,
					  PTPARAS pImagePara,
					  BOOL bWithAnswer,
					  BOOL bWithRes)
{
	int i;
	TCHAR sz[64] = {0};
	_stdstring temp;
	LPCTSTR lpDesc = GetDescription();
	int nPtr = 0;
	int nStrLen;

	str += _T("<ol class=\"desc\" start=\"");
	
	_sntprintf(sz, 64, _T("%d"), nItemIndex);
	str += sz;
	str += _T("\">\n<li class=\"desc\">");

	for (i = 0; i < GetBlankCount(); ++i)
	{
		int nStart, nEnd;
		int j;

		GetBlankRange(i, &nStart, &nEnd);
		nStrLen = CTextManager::GetRealIndex(lpDesc, nStart - 1) - nPtr;

		if (nStrLen)
		{
			temp.append(lpDesc, nPtr, nStrLen);
		}

		if (bWithAnswer)
		{
			temp += _T("<span style=\"text-decoration:underline;font-weight:bold;\">");
			temp += _T("&nbsp;&nbsp;&nbsp;");
			temp += GetBlank(i);
			temp += _T("&nbsp;&nbsp;&nbsp;");
			temp += _T("</span>");
		}
		else
		{
			nStrLen = CTextManager::GetCharactersCount(m_pBlanks[i].pBlank);
			temp += _T("<span class=\"under\">");
			for (j = 0; j < nStrLen; ++j)
			{
				temp += _T("_____");
			}
			temp += _T("</span>");
		}

		nPtr = CTextManager::GetRealIndex(lpDesc, nEnd + 1);
	}

	nStrLen = lstrlen(lpDesc) - nPtr;
	if (nStrLen)
	{
		temp.append(lpDesc, nPtr, nStrLen);
	}

	if (m_DescImageList.GetDataCount())
	{
		MakeHTMLImage(fp, str, nItemIndex, pImagePara, !bWithAnswer);
	}

	CItem::ReplaceHTMLEndLine(temp);
	str += temp;
	str += _T("</li></ol>\n");

	if (bWithAnswer && bWithRes)
	{
		MakeResolve(str, PAPERTYPE_HTMLONLY);
	}
}

BOOL CBlank::SetUserAnswer(int nIndex, LPCTSTR strAnswer)
{
	if (nIndex >= m_nBlanksCount || !strAnswer)
	{
		m_dwErrorCode = ERROR_NOANSWER;
		return FALSE;
	}

	if (m_pUserAnswers[nIndex])
	{
		if (lstrlen(m_pUserAnswers[nIndex]))
		{
			m_nUserAnswerCount--;
		}
		free(m_pUserAnswers[nIndex]);
		m_pUserAnswers[nIndex] = NULL;
	}

	m_pUserAnswers[nIndex] = _tcsdup(strAnswer);
	if (!m_pUserAnswers[nIndex])
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}

	if (lstrlen(strAnswer))
	{
		m_nUserAnswerCount++;
	}
	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

LPCTSTR CBlank::GetUserAnswer(int nIndex)
{
	if (nIndex >= m_nBlanksCount)
	{
		return NULL;
	}
	
	if (m_pUserAnswers[nIndex])
	{
		return m_pUserAnswers[nIndex];
	}
	else
	{
		return s_strNull;
	}
}

int CBlank::Score(CTestPara *pCPara)
{
	PTESTPARA pPara = pCPara->GetTestPara();
	int nCount = 0;
	int n = GetBlankCount();

	if (pPara->dwBlkOptions & BLKOPTION_ALLRIGHT)
	{
		nCount = AutoScore(pCPara);
	}
	else
	{
		CheckAnswer(&nCount);
	}

	if (!nCount)
	{
		m_nAnswerCheck = WRONG;
	}
	else if (nCount == n)
	{
		m_nAnswerCheck = ALLRIGHT;
	}
	else
	{
		m_nAnswerCheck = HALFRIGHT;
	}
	
	if (MARKTYPE_AVERAGE == pPara->nMarkType)
	{
		return (nCount * 100);
	}
	else
	{
		return (pPara->nBlkPoints * nCount * 2);
	}
}

int CBlank::AutoScore(CTestPara *pCPara)
{
	int n = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	int i;
	int nCount = 0;

	for (i = 0; i < n; ++i)
	{
		if (m_pUserAnswers[i] && !_tcsicmp(pBlank[i].pBlank, m_pUserAnswers[i]))
		{
			pBlank[i].bCheck = TRUE;
			++nCount;
		}
		else
		{
			pBlank[i].bCheck = FALSE;
		}
	}

	return nCount;
}

int CBlank::CheckAnswer(int *pnRightCount)
{
	if (pnRightCount)
	{
		int nCount = GetBlankCount();
		PBLANK pBlank = GetRealBlanks();
		int i, nRet;
		
		for (i = 0, nRet = 0; i < nCount; ++i)
		{
			if (pBlank[i].bCheck)
			{
				++nRet;
			}
		}

		*pnRightCount = nRet;
	}

	return m_nAnswerCheck;
}

void CBlank::SetUserAnswerCheck(int nIndex, BOOL bCorrect)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();

	if (nIndex < 0 || nIndex > nCount)
	{
		return;
	}
	pBlank[nIndex].bCheck = bCorrect;
}

BOOL CBlank::GetUserAnswerCheck(int nIndex)
{
	int nCount = GetBlankCount();
	PBLANK pBlank = GetRealBlanks();
	
	if (nIndex < 0 || nIndex > nCount)
	{
		return FALSE;
	}
	return pBlank[nIndex].bCheck;
}

BOOL CBlank::Match(LPCTSTR strMatch, BOOL bMatchCase)
{
	if (CItem::Match(strMatch, bMatchCase))
	{
		return TRUE;
	}
	
	int i;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		if (bMatchCase)
		{
			if (_tcsstr(m_pBlanks[i].pBlank, strMatch))
			{
				return TRUE;
			}
		}
		else
		{
			if (_tcsstri(m_pBlanks[i].pBlank, strMatch))
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}

void CBlank::CalcCRC()
{
	CItem::CalcCRC();

	int nLen, i, j;
	PBYTE ptr;

#ifndef _UNICODE
	int nBuffSize = 0;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		j = lstrlen(m_pBlanks[i].pBlank) + 1;
		nBuffSize = max(nBuffSize, j);
	}
	nBuffSize *= sizeof(WCHAR);
	LPWSTR str = (LPWSTR)malloc(nBuffSize);
	if (!str)
	{
		return;
	}
#endif
	
	for (i = 0; i < m_nBlanksCount; i++)
	{
		m_wCRC += m_pBlanks[i].nStart;
		m_wCRC += m_pBlanks[i].nEnd;

#ifdef _UNICODE
		ptr = (PBYTE)m_pBlanks[i].pBlank;
		nLen = lstrlen(m_pBlanks[i].pBlank) * sizeof(TCHAR);
#else
		if (!MultiByteToWideChar(CP_ACP, 0, m_pBlanks[i].pBlank, -1, str, nBuffSize))
		{
			free(str);
			return;
		}
		ptr = (PBYTE)str;
		nLen = wcslen(str) * sizeof(WCHAR);
#endif
		for (j = 0; j < nLen; j++)
		{
			m_wCRC += ptr[j];
		}
	}

#ifndef _UNICODE
	free(str);
#endif
}

BOOL CBlank::IsSameItem(CItem *pItem, int nSimilar)
{
	if (!CItem::IsSameItem(pItem, nSimilar))
	{
		return FALSE;
	}

	CBlank *pBlk = (CBlank*)pItem;
	if (m_nBlanksCount != pBlk->m_nBlanksCount)
	{
		return FALSE;
	}

	int i;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		// 不判断空格的起始和结束位置完全相同
		// 因为如果要求完全相同，很可能一个都选不上
		if (nSimilar > 99)
		{
			if (_tcscmp(m_pBlanks[i].pBlank, pBlk->m_pBlanks[i].pBlank))
			{
				return FALSE;
			}
		}
		else
		{
			CTextSimilarity t;
			t.Compare(m_pBlanks[i].pBlank, pBlk->m_pBlanks[i].pBlank);
			if ((int)(t.GetInfo() * 100.f) < nSimilar)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

int FUNCCALL CBlank::CheckBlksCount(CItem *pItem, BOOL bInterupt)
{
	int ret = 0;
	
	while(pItem)
	{
		if (TYPE_BLANK == pItem->GetType())
		{
			ret += ((CBlank*)pItem)->GetBlankCount();
		}
		else
		{
			if (bInterupt)
			{
				break;
			}
		}

		pItem = pItem->GetNext();
	}
	
	return ret;
}

BOOL CBlank::CheckRanges(int nStart, int nEnd)
{
	if (nStart > nEnd)
	{
		return FALSE;
	}
	
	int i;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		// 需要空出边上括号
		if (!(nEnd < m_pBlanks[i].nStart || nStart > m_pBlanks[i].nEnd))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

LPWSTR CBlank::GetWDescription(int &nBuffSize)
{
	LPWSTR pText = NULL;
	nBuffSize = 0;
	// 需要预留一些空间（用于可能会添加的括号）
#ifdef _UNICODE
	int nLen = lstrlen(m_strDescription);
	
	// 这里还要加上末尾的0
	nBuffSize = nLen + 6 + 1;
	pText = (LPWSTR)malloc(nBuffSize * sizeof(WCHAR));
	if (!pText)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		nBuffSize = 0;
		return NULL;
	}
	
	_tcscpyn(pText, m_strDescription, nLen + 1);
#else
	int nLen = MultiByteToWideChar(CP_ACP, 0, m_strDescription, -1, NULL, 0);
	if (!nLen)
	{
		m_dwErrorCode = ERROR_TRANSCODEFAIL;
		return NULL;
	}
	
	// 这里的nLen已经预先包含了末尾的0
	nBuffSize = nLen + 6;
	pText = (LPWSTR)malloc(nBuffSize * sizeof(WCHAR));
	if (!pText)
	{
		nBuffSize = 0;
		return NULL;
	}
	
	if (!MultiByteToWideChar(CP_ACP, 0, m_strDescription, -1, pText, nLen))
	{
		free(pText);
		nBuffSize = 0;
		return NULL;
	}
#endif

	return pText;
}

LPTSTR CBlank::GetTDescription(LPWSTR wstrDescription)
{
#ifdef _UNICODE
	return wstrDescription;
#else
	LPTSTR strDescription = NULL;
	int nLen = WideCharToMultiByte(CP_ACP, 0, wstrDescription, -1, NULL, 0, 0, FALSE);
	
	strDescription = (LPTSTR)malloc(nLen * sizeof(TCHAR));
	if (!strDescription)
	{
		free(wstrDescription);
		return NULL;
	}

	if (!WideCharToMultiByte(CP_ACP, 0, wstrDescription, -1, strDescription, nLen, 0, FALSE))
	{
		free(wstrDescription);
		free(strDescription);
		return NULL;
	}

	free(wstrDescription);
	return strDescription;
#endif
}

LPTSTR CBlank::GetBlankText(LPWSTR wstrDescription, int nStart, int nEnd)
{
	LPTSTR pReturn = NULL;

#ifdef _UNICODE
	pReturn = (LPTSTR)malloc((nEnd - nStart + 1) * sizeof(TCHAR));
	if (!pReturn)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return NULL;
	}

	_tcscpyn(pReturn, wstrDescription + nStart, nEnd - nStart + 1);
#else
	int nLen = WideCharToMultiByte(CP_ACP, 0, wstrDescription + nStart, nEnd - nStart, NULL, 0, NULL, NULL);
	pReturn = (LPTSTR)malloc((nLen + 1) * sizeof(TCHAR));
	if (!pReturn)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return NULL;
	}
	if (!WideCharToMultiByte(CP_ACP, 0, wstrDescription + nStart, nEnd - nStart, pReturn, nLen, NULL, NULL))
	{
		free(pReturn);
		m_dwErrorCode = ERROR_NOANSWER;
		return NULL;
	}
	pReturn[nLen] = 0;
#endif

	return pReturn;
}

LPTSTR CBlank::ClearSpaces(LPTSTR strText)
{
	if (!strText)
	{
		return NULL;
	}

	int i = 0;
	int nBegin = -1, nEnd = -1;
	int nLen = lstrlen(strText);

	while(strText[i])
	{
		switch(strText[i])
		{
		case _T('\r'):
		case _T('\n'):
		case _T(' '):
		case _T('\t'):
#ifdef _UNICODE
		case _T('　'):
#endif
			break;

#ifndef _UNICODE
		case (TCHAR)0xA1:
			if (strText[i + 1] != (TCHAR)0xA1)
			{
				if (nBegin < 0)
				{
					nBegin = i;
				}
				else
				{
					nEnd = i + 1;
				}
			}
			i++;
			break;
#endif

		default:
			{
				int nOffset = 0;
#ifndef _UNICODE
				if (strText[i] & 0x80)
				{
					nOffset++;
				}
#endif
				if (nBegin < 0)
				{
					nBegin = i;
					nEnd = i + nOffset + 1;
				}
				else
				{
					nEnd = i + nOffset;
				}

				i += nOffset;
			}

		}

		i++;
	}

	// 始终没有找到有效字符
	if (nBegin < 0)
	{
		return NULL;
	}

	// 如果末尾是连续空格
	if (nEnd != nLen)
	{
		strText[nEnd + 1] = 0;
	}

	return strText + nBegin;
}

int CBlank::MakeQuote(LPWSTR wstrDescription, int &nStart, int &nEnd)
{
	ASSERT(m_strDescription);
	ASSERT(nEnd >= nStart);

	// 检查选区周围是否有括号
	int i = nStart - 1, j;
	BOOL bOut = FALSE;
	int nQuoteStart = -1, nQuoteEnd = -1;
	int nRet = 0;
	while(i >= 0 && !bOut)
	{
		switch(wstrDescription[i])
		{
		CASE_LEFTBRACE:
			nQuoteStart = i;
			bOut = TRUE;
			continue;
			
		CASE_SPACE:
		CASE_RETURN:
			i--;
			break;
				
		default:
			if (i == nStart)
			{
				i--;
				continue;
			}
			bOut = TRUE;
			break;
		}
	}
	if (nQuoteStart >= 0)
	{
		i = nEnd;
		bOut = FALSE;
		
		while(wstrDescription[i] && !bOut)
		{
			switch(wstrDescription[i])
			{
			CASE_RIGHTBRACE:
				nQuoteEnd = i;
				bOut = TRUE;
				continue;

			CASE_SPACE:
			CASE_RETURN:
				i++;
				break;
				
			default:
				if (i == nEnd)
				{
					i++;
					continue;
				}
				bOut = TRUE;
				break;
			}
		}
	}

	// 如果发现了括号，则直接在括号基础上进行改动
	if (nQuoteStart >= 0 && nQuoteEnd >= 0)
	{
		nRet = 4 - (nQuoteEnd - nQuoteStart - 1);
		// 如果空格长度小于4，则需要补全4个空位
		if (nRet > 0)
		{
			j = wcslen(wstrDescription);
			i = j + nRet;
			for(; j >= nQuoteEnd; j--, i--)
			{
				wstrDescription[i] = wstrDescription[j];
			}
			for (i = nQuoteStart + 1; i < nQuoteEnd + nRet; i++)
			{
				wstrDescription[i] = L' ';
			}
		}
		// 如果超出了4个位置，则需要将结尾提前
		else
		{
			for (i = 1; i <= 4; i++)
			{
				wstrDescription[nQuoteStart + i] = L' ';
			}

			if (nRet < 0)
			{
				for (j = i - nRet; wstrDescription[nQuoteStart + j]; i++, j++)
				{
					wstrDescription[nQuoteStart + i] = wstrDescription[nQuoteStart + j];
				}
				wstrDescription[nQuoteStart + i] = 0;
			}
		}

		nStart = nQuoteStart + 1;
		nEnd = nStart + 4;
	}
	// 如果没有括号，则需要添加括号
	else
	{
		nRet = 6 - (nEnd - nStart);
		if (nRet > 0)
		{
			j = wcslen(wstrDescription);
			i = j + nRet;
			for(; j >= nEnd; j--, i--)
			{
				wstrDescription[i] = wstrDescription[j];
			}
		}
		else
		{
			for (i = nStart + 6, j = nEnd; wstrDescription[j]; i++, j++)
			{
				wstrDescription[i] = wstrDescription[j];
			}
			wstrDescription[i] = 0;
		}
		
		wcsncpy(wstrDescription + nStart, L"(    )", 6);

		nStart++;
		nEnd = nStart + 4;
	}

	return nRet;
}

int CBlank::InsertPos(int nStart, int nEnd, int nOffset)
{
	ASSERT(nStart < nEnd);
	
	int i;
	for (i = m_nBlanksCount; i > 0; i--)
	{
		if (nEnd < m_pBlanks[i - 1].nStart + nOffset)
		{
			m_pBlanks[i].nStart = m_pBlanks[i - 1].nStart + nOffset;
			m_pBlanks[i].nEnd = m_pBlanks[i - 1].nEnd + nOffset;
			m_pBlanks[i].pBlank = m_pBlanks[i - 1].pBlank;
			m_pBlanks[i].bCheck = FALSE;
		}
		else
		{
			break;
		}
	}
	
	m_nBlanksCount++;
	return i;
}

void CBlank::FillAnswer(_stdstring &str, BOOL bIsRealAsw, int nCount)
{
	int nBlkCount = GetBlankCount();
	BLANK* pBlank = GetRealBlanks();
	
	int i, nOffset = 0;
#ifndef _UNICODE
	int idxInsert = 0;
#endif
	
	MakeText(str, MKTXT_DEFAULT, -1);
#ifndef _UNICODE
	_stdstring strTmpDesc;
	strTmpDesc = str;
#endif
	
	if (bIsRealAsw)
	{
		if (nCount < 0)
		{
			return;
		}
		
		if (nCount > nBlkCount)
		{
			nCount = nBlkCount;
		}
		
		for (i = 0; i < nCount; i++)
		{
#ifdef _UNICODE
			str.erase(pBlank[i].nStart + nOffset, 4);
			str.insert(pBlank[i].nStart + nOffset, pBlank[i].pBlank);
			nOffset += (lstrlen(pBlank[i].pBlank) - 4);
#else
			idxInsert = CTextManager::GetRealIndex(strTmpDesc.c_str(), pBlank[i].nStart);
			str.erase(idxInsert + nOffset, 4);
			str.insert(idxInsert + nOffset, pBlank[i].pBlank);
			nOffset += (lstrlen(pBlank[i].pBlank) - 4);
#endif
		}
	}
	else
	{
		for (i = 0; i < nBlkCount; i++)
		{
			if (!m_pUserAnswers[i])
			{
				continue;
			}
#ifdef _UNICODE
			str.erase(pBlank[i].nStart + nOffset, 4);
			str.insert(pBlank[i].nStart + nOffset, m_pUserAnswers[i]);
			nOffset += (lstrlen(m_pUserAnswers[i]) - 4);
#else
			idxInsert = CTextManager::GetRealIndex(strTmpDesc.c_str(), pBlank[i].nStart);
			str.erase(idxInsert + nOffset, 4);
			str.insert(idxInsert + nOffset, m_pUserAnswers[i]);
			nOffset += (lstrlen(m_pUserAnswers[i]) - 4);
#endif
		}
	}
}

void CBlank::ClearAllBlanks()
{
	int i;
	for (i = 0; i < m_nBlanksCount; i++)
	{
		m_pBlanks[i].nStart = 0;
		m_pBlanks[i].nEnd = 0;
		free(m_pBlanks[i].pBlank);
		m_pBlanks[i].pBlank = NULL;
		m_pBlanks[i].bCheck = FALSE;
	}
	
	m_nBlanksCount = 0;
	SetBlankPara();
}

BOOL CBlank::SetBlankListSize(int nSize)
{
	BOOL b = FALSE;

	while(m_nBlankListSize < nSize)
	{
		m_nBlankListSize += GROWSTEP;
		b = TRUE;
	}
	if (b)
	{
		m_pBlanks = (BLANK*)realloc(m_pBlanks, m_nBlankListSize * sizeof(BLANK));
		if (!m_pBlanks)
		{
			m_dwErrorCode = ERROR_ALLOCFAIL;
			return FALSE;
		}
		
		if (!SetUserAnswerListSize())
		{
			return FALSE;
		}
	}

	m_dwErrorCode = ERROR_OK;
	return TRUE;
}

BOOL CBlank::SetUserAnswerListSize()
{
	m_dwErrorCode = ERROR_OK;
	if (m_nUserAnswerListSize >= m_nBlankListSize)
	{
		return TRUE;
	}
	
	m_pUserAnswers = (LPTSTR*)realloc(m_pUserAnswers, m_nBlankListSize * sizeof(LPTSTR));
	if (!m_pUserAnswers)
	{
		m_dwErrorCode = ERROR_ALLOCFAIL;
		return FALSE;
	}
	
	int i;
	for (i = m_nUserAnswerListSize; i < m_nBlankListSize; i++)
	{
		m_pUserAnswers[i] = NULL;
	}
	
	m_nUserAnswerListSize = m_nBlankListSize;
	return TRUE;
}
