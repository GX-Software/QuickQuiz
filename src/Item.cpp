// Item.cpp : implementation file
//

#include "stdafx.h"
#include "Item.h"

#include "Base64.h"
#include "ImageManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PAPER_IMAGE_MAXX		800

/////////////////////////////////////////////////////////////////////////////
// CItem

LPCTSTR CItem::s_strNull = _T("");
int CItem::s_nImageListFlag[] = {ITEMIMAGEFLAG_DESC, ITEMIMAGEFLAG_RESOLVE};

CItem::CItem() :
	m_nType(TYPE_DEFAULT),
	m_strDescription(NULL), m_strResolve(NULL),
	m_wCRC(0),
	m_pFrom(NULL),
	m_pParent(NULL), m_pPrev(NULL), m_pNext(NULL),
	m_uState(ITEMMARK_NULL),
	m_DescImageList(IMAGE_MAXCOUNT), m_ResvImageList(IMAGE_MAXCOUNT),
	m_nAnswerCheck(WRONG)
{
	m_nUndoType[UNDO_ADDINDEX] = m_nUndoType[UNDO_DELINDEX] = -1;

	ZeroMemory(m_szTypeDesc, sizeof(m_szTypeDesc));
}

CItem::~CItem()
{
	ClearAllInfo();
}

/*
���ܣ�	��ȡ������������
����ֵ����������ַ���
��ע��	������Ӧ���������麯�����ã�����ֱ�ӱ�����
*/
LPCTSTR CItem::GetTypeDesc()
{
	// ��ȡ�������ʱ����׷�����������
	// �����Ϊ��⣬��Ѱ�Ҷ�Ӧ�б����ƣ��������б�����
	// �������������NULL����ʹ��Ĭ������
	CItem *pItem = GetItemFrom();
	CList *pParent = pItem->GetParent();
	while(pParent)
	{
		if (pParent->IsPaper())
		{
			CTempList *p = ((CPaper*)pParent)->GetTypeList(pItem->GetRawType());
			ASSERT(p);
			return p->GetDescription();
		}
		else if (pParent->IsList())
		{
			LPCTSTR strTitle = pParent->GetDescription();
			if (_tcslen(strTitle))
			{
				return strTitle;
			}
		}

		pParent = pParent->GetParent();
	}

	return NULL;
}

void CItem::SetCustomIndex(int nIndex)
{
	if (nIndex < 0 || nIndex > TYPE_MAXCUSTOMCOUNT)
	{
		return;
	}

	m_nType = (m_nType % TYPE_CUSTOMRIGHT) + (nIndex * TYPE_CUSTOMRIGHT);
}

BOOL CItem::CheckTypeMask(DWORD dwMask)
{
	switch(m_nType)
	{
	case TYPE_SCHOISE:
		return ((dwMask & TYPEMASK_SCHOISE) ? TRUE : FALSE);

	case TYPE_MCHOISE:
		return ((dwMask & TYPEMASK_MCHOISE) ? TRUE : FALSE);

	case TYPE_JUDGE:
		return ((dwMask & TYPEMASK_JUDGE) ? TRUE : FALSE);

	case TYPE_BLANK:
		return ((dwMask & TYPEMASK_BLANK) ? TRUE : FALSE);

	case TYPE_TEXT:
		return ((dwMask & TYPEMASK_TEXT) ? TRUE : FALSE);

	case TYPE_GROUP:
		return ((dwMask & TYPEMASK_GROUP) ? TRUE : FALSE);

	default:
		{
			DWORD d = TYPEMASK_1STCUSTOM << (CItem::GetCustomIndex(m_nType) - 1);
			return ((dwMask & d) ? TRUE : FALSE);
		}
	}

	return FALSE;
}

void CItem::SetItemState(UINT nState, UINT nMark)
{
	nState &= nMark;
	
	nMark = ~nMark;
	m_uState &= nMark;
	
	m_uState |= nState;
}

BOOL CItem::SaveStore(FILE *fp)
{
	if (!fp)
	{
		return FALSE;
	}

	BOOL bStore = IsStored();
	if (EOF == fputc(bStore, fp))
	{
		return FALSE;
	}

	if (bStore)
	{
		if (EOF == fputwc(m_wCRC, fp))
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CItem::LoadStore(FILE *fp)
{
	if (!fp)
	{
		return FALSE;
	}
	
	BOOL bStore = fgetc(fp);
	if (EOF == bStore)
	{
		return FALSE;
	}

	if (bStore)
	{
		WORD wCRC = fgetwc(fp);
		if (EOF == wCRC)
		{
			return FALSE;
		}

		SetStore(wCRC == m_wCRC);
	}

	return TRUE;
}

// ͨ����Ŀ�洢��ʽ
// 2Bytes����ɳ��ȣ��ֽڼƣ�������ĩβ0
// ?Bytes�����
// 2Bytes����Ŀ�������ȣ��ֽڼƣ�������ĩβ0
// ?Bytes����Ŀ����
EBOOL CItem::LoadFile(FILE *fp, DWORD dwVersion)
{
	LPTSTR ptr = CTextManager::LoadFileString(fp, sizeof(short));
	if (!ptr)
	{
		return Q_WARNING(ERROR_INNER);
	}
	if (!SetDescription(ptr))
	{
		free(ptr);
		return Q_WARNING(ERROR_INNER);
	}
	free(ptr);

	// �汾3�������ܣ���Ŀ����
	if (dwVersion >= 3)
	{
		ptr = CTextManager::LoadFileString(fp, sizeof(short));
		if (!ptr)
		{
			return Q_WARNING(ERROR_INNER);
		}

		SetResolve(ptr);
		free(ptr);
	}
	else
	{
		SetResolve(_T(""));
	}
	return Q_RET(TRUE);
}

// ͨ����Ŀ�洢��ʽ
// 2Bytes����ɳ��ȣ��ֽڼƣ�������ĩβ0
// ?Bytes�����
// 2Bytes����Ŀ�������ȣ��ֽڼƣ�������ĩβ0
// ?Bytes����Ŀ����
EBOOL CItem::SaveFile(FILE *fp)
{
	if (!CTextManager::SaveFileString(fp, GetDescription(), sizeof(short)))
	{
		return Q_ERROR(ERROR_INNER);
	}

	// ��ֹ��Ŀ����麯����������
	if (!CTextManager::SaveFileString(fp, CItem::GetResolve(), sizeof(short)))
	{
		return Q_ERROR(ERROR_INNER);
	}

	return Q_TRUE;
}

RETURN_E TiXmlElement* CItem::WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible)
{
	if (!pParent)
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}
	
	TiXmlElement *pQuestion = new TiXmlElement(xml_lpszNode_Question);
	if (!pQuestion)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pParent->LinkEndChild(pQuestion);
	
	pQuestion->SetAttribute(xml_lpszAttribute_Type, XMLGetTypeAttribute(GetType()));
	pQuestion->SetAttribute(xml_lpszAttribute_Custom, m_nType / TYPE_CUSTOMRIGHT);
	
	TiXmlElement *pNode = new TiXmlElement(xml_lpszNode_Description);
	if (!pNode)
	{
		return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
	}
	pQuestion->LinkEndChild(pNode);
	
	LPSTR lpString = CTextManager::SwitchSave(GetDescription(), nEncode, NULL);
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
	pNode->LinkEndChild(pText);

	if (_tcslen(GetResolve()))
	{
		pNode = new TiXmlElement(xml_lpszNode_Resolve);
		if (!pNode)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pQuestion->LinkEndChild(pNode);
		
		lpString = CTextManager::SwitchSave(GetResolve(), nEncode, NULL);
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
		pNode->LinkEndChild(pText);
	}

	// ����ͼƬ
	if (!WriteItemImage(pQuestion, nEncode, ITEMIMAGEFLAG_DESC) ||
		!WriteItemImage(pQuestion, nEncode, ITEMIMAGEFLAG_RESOLVE))
	{
		return Q_ERROR_RET(ERROR_INNER, NULL);
	}
	
	return Q_TRUE_RET(pQuestion);
}

EBOOL CItem::ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup)
{
	int nCust = 0;
	if (pQstn->Attribute(xml_lpszAttribute_Custom, &nCust))
	{
		SetCustomIndex(nCust);
	}

	LPTSTR lpString;
	// ����Ŀ���⣬������Ŀ��Ӧ�������
	TiXmlElement *pNode = pQstn->FirstChildElement(xml_lpszNode_Description);
	if (pNode && pNode->GetText())
	{
		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (lpString)
		{
			EBOOL bRet = SetDescription(lpString);
			free(lpString);
			if (!bRet)
			{
				return Q_WARNING(ERROR_INNER);
			}
		}
		else
		{
			return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
		}
	}
	else if (!bGroup)
	{
		return Q_WARNING(ERROR_FILE_INCOMPLETE);
	}

	// ��Ŀ������û�н�����
	pNode = pQstn->FirstChildElement(xml_lpszNode_Resolve);
	if (pNode && pNode->GetText())
	{
		lpString = CTextManager::SwitchLoad(pNode->GetText(), nEncode, NULL);
		if (lpString)
		{
			EBOOL bRet = SetResolve(lpString);
			free(lpString);
			if (!bRet)
			{
				return Q_WARNING(ERROR_INNER);
			}
		}
		else
		{
			return Q_WARNING(ERROR_TEXT_TRANSCODING_FAIL);
		}
	}

	// ��ȡ����е�ͼƬ
	if (!LoadItemImage(pQstn, nEncode, ITEMIMAGEFLAG_DESC) ||
		!LoadItemImage(pQstn, nEncode, ITEMIMAGEFLAG_RESOLVE))
	{
		return Q_RET(FALSE);
	}
	
	return Q_RET(TRUE);
}

LPCTSTR CItem::GetDescription()
{
	if (m_strDescription)
	{
		return m_strDescription;
	}
	else
	{
		return s_strNull;
	}
}

int CItem::GetDescriptionSaveLen()
{
	LPCTSTR strDescription = GetDescription();
#ifdef _UNICODE
	return (_tcslen(strDescription) + 1) * sizeof(TCHAR);
#else
	return MultiByteToWideChar(CP_ACP, 0, strDescription, -1, NULL, 0) * sizeof(WCHAR);
#endif
}

BOOL CItem::SetDescription(LPCTSTR szDescription)
{
	// ����Ŀ������Ŀ�⣬������Ŀ����ɶ���ӦΪ��
	if (!szDescription)
	{
		return FALSE;
	}

	LPTSTR szSet = NULL;
	if (!CTextManager::CheckValidCharactor(szDescription))
	{
		szSet = _T("");
	}
	else
	{
		szSet = (LPTSTR)szDescription;
	}
	
	if (m_strDescription)
	{
		free(m_strDescription);
		m_strDescription = NULL;
	}
	
	m_strDescription = _tcsdup(szSet);
	if (!m_strDescription)
	{
		return FALSE;
	}
	
	return TRUE;
}

#ifndef _UNICODE
BOOL CItem::SetWDescription(LPCWSTR szDescription)
{
	LPTSTR strDes = CTextManager::SwitchLoad((LPCSTR)szDescription, CS_UTF16, NULL);
	if (!strDes)
	{
		return FALSE;
	}
	BOOL bRet = this->SetDescription(strDes);
	free(strDes);
	return bRet;
}
#endif

EBOOL CItem::WriteItemImage(TiXmlElement *pQstn, int nEncode, int nFlag)
{
	CDataList *pList;
	LPCSTR pLoc;
	int i;

	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		pList = &m_DescImageList;
		pLoc = xml_lpszNode_Description;
		break;

	case ITEMIMAGEFLAG_RESOLVE:
		pList = &m_ResvImageList;
		pLoc = xml_lpszNode_Resolve;
		break;

	default:
		ASSERT(FALSE);
		return Q_ERROR(ERROR_INNER);
	}

	for (i = 0; i < GetImageCount(nFlag); ++i)
	{
		std::string string;
		PITEMIMAGE p;
		TiXmlElement *pNode;
		TiXmlText *pText;

		string = xml_lpszText_ImageHead;
		p = GetImage(nFlag, i);
		if (!p)
		{
			break;
		}
		
		switch(p->nImageType)
		{
		case ITEMIMAGETYPE_DIB:
			string += xml_lpszText_ImageBmp;
			break;
			
		case ITEMIMAGETYPE_JPEG:
			string += xml_lpszText_ImageJpeg;
			break;
			
		case ITEMIMAGETYPE_PNG:
			string += xml_lpszText_ImagePng;
			break;
		}
		
		if (!MakeImageBase64(p, string))
		{
			return Q_ERROR_RET(ERROR_BASE64_ENCODE_FAIL, NULL);
		}
		
		pNode = new TiXmlElement(xml_lpszNode_Image);
		if (!pNode)
		{
			Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pQstn->LinkEndChild(pNode);
		
		pNode->SetAttribute(xml_lpszAttribute_Index, i + 1);
		pNode->SetAttribute(xml_lpszAttribute_Loc, pLoc);
		
		if (p->szImageTitle)
		{
			LPSTR lpString = CTextManager::SwitchSave(p->szImageTitle, nEncode, NULL);
			if (!lpString)
			{
				return Q_ERROR_RET(ERROR_TEXT_TRANSCODING_FAIL, NULL);
			}
			pNode->SetAttribute(xml_lpszAttribute_Title, lpString);
			free(lpString);
		}
		
		pText = new TiXmlText(string.c_str());
		if (!pText)
		{
			return Q_ERROR_RET(ERROR_ALLOC_FAIL, NULL);
		}
		pNode->LinkEndChild(pText);
	}

	return Q_RET(TRUE);
}

EBOOL CItem::LoadItemImage(TiXmlElement *pQstn, int nEncode, int nFlag)
{
	CDataList *pList;
	LPCSTR pLoc;

	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		pList = &m_DescImageList;
		pLoc = xml_lpszNode_Description;
		break;

	case ITEMIMAGEFLAG_RESOLVE:
		pList = &m_ResvImageList;
		pLoc = xml_lpszNode_Resolve;
		break;

	default:
		ASSERT(FALSE);
		return Q_WARNING(ERROR_INNER);
	}

	_local
	{
		TiXmlElement *pNode;
		LPTSTR lpString;
		PBYTE pImage = NULL;
		size_t ulImageSize = 0;
		int nImageType;
		PITEMIMAGE p = NULL;
		int nIndex = 1, nLoad;

		while(1)
		{
			pNode = pQstn->FirstChildElement(xml_lpszNode_Image);
			while(pNode)
			{
				if (pNode->Attribute(xml_lpszAttribute_Loc) &&
					!stricmp(pNode->Attribute(xml_lpszAttribute_Loc), pLoc))
				{
					if (pNode->Attribute(xml_lpszAttribute_Index, &nLoad))
					{
						if (nLoad == nIndex)
						{
							break;
						}
						else if (nLoad > nIndex)
						{
							pNode = NULL;
							break;
						}
					}
				}
				
				pNode = pNode->NextSiblingElement(xml_lpszNode_Image);
			}
			if (!pNode)
			{
				break;
			}
			
			if (!MakeImageBin(&pImage, &ulImageSize, &nImageType, pNode->GetText()))
			{
				return Q_RET(FALSE);
			}
			if (!SetItemImage(pList, -1, nImageType, pImage, ulImageSize))
			{
				return Q_RET(FALSE);
			}
			
			lpString = CTextManager::SwitchLoad(pNode->Attribute(xml_lpszAttribute_Title), nEncode, NULL);
			if (lpString)
			{
				SetItemImageTitle(pList, GetImageCount(nFlag) - 1, lpString);
				free(lpString);
			}
			
			++nIndex;
		}
	}

	return Q_RET(TRUE);
}

int CItem::GetDescriptionSummary(int nChs)
{
#ifdef _UNICODE
	return nChs;
#else
	int i = 0;
	_stdstring str;
	MakeText(str, MKTXT_OPTIONSINALINE, 0);
	LPCTSTR ptr = str.c_str();
	while(*ptr)
	{
		if ((UCHAR)(*ptr) > 0x7F && (UCHAR)(*(ptr + 1)) > 0x7F)
		{
			ptr++;
		}

		ptr++;
		i++;
		if (i == nChs)
		{
			break;
		}
	}

	return (ptr - str.c_str());
#endif
}

void CItem::ClearAllInfo()
{
	if (m_strDescription)
	{
		free(m_strDescription);
		m_strDescription = NULL;
	}

	if (m_strResolve)
	{
		free(m_strResolve);
		m_strResolve = NULL;
	}
}

void CItem::ClearUserInfo()
{
	SetItemState(ITEMMARK_NULL, ITEMMARK_UNCERTAIN);
}

BOOL CItem::CopyItem(CItem *pItem)
{
	ASSERT(!pItem->IsList());

	CItem *pFrom = pItem;
	CItem *pNext = pFrom->m_pFrom;
	while(pNext)
	{
		pFrom = pNext;
		pNext = pNext->m_pFrom;
	}
	ASSERT(pFrom);
	m_pFrom = pFrom;

	memcpy(m_szTypeDesc, pItem->m_szTypeDesc, sizeof(m_szTypeDesc));
	SetCustomIndex(pItem->GetCustomIndex());

	if (!SetDescription(pItem->GetDescription()))
	{
		return FALSE;
	}
	if (!SetResolve(pItem->CItem::GetResolve()))
	{
		return FALSE;
	}

	m_DescImageList.ClearData();
	m_ResvImageList.ClearData();

	int i, j;
	PITEMIMAGE p = NULL;
	int nImageType;
	PBYTE pImageData;
	size_t ulImageSize;
	CDataList *pList;
	for (j = 0; j < _countof(s_nImageListFlag); ++j)
	{
		switch(s_nImageListFlag[j])
		{
		case ITEMIMAGEFLAG_DESC:
			pList = &m_DescImageList;
			break;

		case ITEMIMAGEFLAG_RESOLVE:
			pList = &m_ResvImageList;
			break;

		default:
			return FALSE;
		}

		for (i = 0; i < pItem->GetImageCount(s_nImageListFlag[j]); ++i)
		{
			p = pItem->GetImage(s_nImageListFlag[j], i);
			
			nImageType = p->nImageType;
			ulImageSize = p->ulImageSize;
			pImageData = (PBYTE)malloc(ulImageSize);
			if (!pImageData)
			{
				return FALSE;
			}
			memcpy(pImageData, p->pImageData, ulImageSize);
			
			if (!SetItemImage(pList, i, nImageType, pImageData, ulImageSize))
			{
				free(pImageData);
				return FALSE;
			}
			
			if (p->szImageTitle)
			{
				SetItemImageTitle(pList, i, p->szImageTitle);
			}
		}
	}

	return TRUE;
}

void CItem::ClipCopy(CFixedSharedFile &sf)
{
	sf.Write(&m_nType, sizeof(m_nType));

	int nLen = _tcslen(GetDescription());
	sf.Write(&nLen, sizeof(nLen));
	sf.Write(GetDescription(), nLen * sizeof(TCHAR));

	nLen = _tcslen(CItem::GetResolve());
	sf.Write(&nLen, sizeof(nLen));
	sf.Write(CItem::GetResolve(), nLen * sizeof(TCHAR));

	int i, j;
	PITEMIMAGE p;
	for (j = 0; j < _countof(s_nImageListFlag); ++j)
	{
		nLen = GetImageCount(s_nImageListFlag[j]);
		sf.Write(&nLen, sizeof(nLen));
		if (!nLen)
		{
			continue;
		}

		for (i = 0; i < GetImageCount(s_nImageListFlag[j]); ++i)
		{
			p = GetImage(s_nImageListFlag[j], i);
			sf.Write(&(p->nImageType), sizeof(p->nImageType));
			
			sf.Write(&(p->ulImageSize), sizeof(p->ulImageSize));
			sf.Write(p->pImageData, p->ulImageSize);
			
			nLen = _tcslen(p->szImageTitle);
			sf.Write(&nLen, sizeof(nLen));
			if (nLen)
			{
				sf.Write(p->szImageTitle, nLen * sizeof(TCHAR));
			}
		}
	}
}

BOOL CItem::ClipPaste(CFixedSharedFile &sf)
{
	int nLen = 0;
	CString str;

	sf.Read(&nLen, sizeof(nLen));
	sf.Read(str.GetBufferSetLength(nLen), nLen * sizeof(TCHAR));
	if (!SetDescription(str))
	{
		return FALSE;
	}

	sf.Read(&nLen, sizeof(nLen));
	sf.Read(str.GetBufferSetLength(nLen), nLen * sizeof(TCHAR));
	if (!SetResolve(str))
	{
		return FALSE;
	}

	int i, j;
	CDataList *pList;
	for (j = 0; j < _countof(s_nImageListFlag); ++j)
	{
		int nCount;

		sf.Read(&nCount, sizeof(nCount));
		if (nCount > IMAGE_MAXCOUNT)
		{
			return FALSE;
		}

		switch(s_nImageListFlag[j])
		{
		case ITEMIMAGEFLAG_DESC:
			pList = &m_DescImageList;
			break;
			
		case ITEMIMAGEFLAG_RESOLVE:
			pList = &m_ResvImageList;
			break;
			
		default:
			return FALSE;
		}
		for (i = 0; i < nCount; ++i)
		{
			int nImageType;
			PBYTE pImageData;
			size_t ulImageSize;

			sf.Read(&nImageType, sizeof(nImageType));
			
			sf.Read(&ulImageSize, sizeof(ulImageSize));
			pImageData = (PBYTE)malloc(ulImageSize);
			if (!pImageData)
			{
				return FALSE;
			}
			sf.Read(pImageData, ulImageSize);
			if (!SetItemImage(pList, i, nImageType, pImageData, ulImageSize))
			{
				free(pImageData);
				return FALSE;
			}
			
			sf.Read(&nLen, sizeof(nLen));
			if (nLen)
			{
				sf.Read(str.GetBufferSetLength(nLen), nLen * sizeof(TCHAR));
				SetItemImageTitle(pList, i, str);
			}
		}
	}

	return TRUE;
}

void CItem::MakeTextOut(_stdstring &str,
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
	str += CTextManager::s_szWinReturn;

	// �������ʱ��������ͼƬ�ļ�
	MakeTextImage(str, nItemIndex, pImagePara, !bWithAnswer);
}

void CItem::MakeTextImage(_stdstring &str, int nIndex, PTPARAS pImagePara, BOOL bSaveFile)
{
	int i;
	PITEMIMAGE p;
	TCHAR title[64] = {0};

	for (i = 0; i < m_DescImageList.GetDataCount(); ++i)
	{
		p = (PITEMIMAGE)m_DescImageList.GetData(i);
		MakeImageTitle(p, title, 64, nIndex, i + 1);

		str += _T("[");
		str += title;
		str += _T(" - ");
		str += p->szImageTitle;
		str += _T("]\r\n");

		if (bSaveFile)
		{
			SaveImageFile(p, pImagePara, title);
		}
	}
}

void CItem::MakeHTML(FILE *fp,
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
	str += _T("\">\n<li class=\"desc\"><div>");

	_stdstring ss;
	ss = GetDescription();
	ReplaceHTMLEndLine(ss);
	str += ss;
	str += _T("</div>");

	if (m_DescImageList.GetDataCount())
	{
		str += _T("\n");
		MakeHTMLImage(fp, str, nItemIndex, pImagePara, !bWithAnswer);
	}

	str += _T("</li></ol>");
	str += _T("\n");
}

void CItem::MakeHTMLImage(FILE *fp, _stdstring &str, int nIndex, PTPARAS pImagePara, BOOL bSaveFile)
{
	int i;
	LPSTR pData = NULL;
	size_t ulSize;
	PITEMIMAGE p;
	TCHAR format[64] = {0};

	int nImageWidth = 0; // ����ͼƬ���ܿ��
	int nImageCount = 0; // ����ͼƬ������

	LoadString(AfxGetInstanceHandle(), IDS_ITEM_IMAGETITLE, format, 64);

	for (i = 0; i < m_DescImageList.GetDataCount(); ++i)
	{
		SIZE imgSize = {0};

		if (!nImageCount)
		{
			str += _T("<table class=\"img\"><tr>");
		}
		p = (PITEMIMAGE)m_DescImageList.GetData(i);
		if (!ReadImageSize(p->nImageType, p->pImageData, p->ulImageSize, &imgSize))
		{
			return;
		}

		// ����ʾ��һ�к�д��ȫ������
		if (nImageWidth + imgSize.cx > PAPER_IMAGE_MAXX && nImageCount)
		{
			WriteHTMLImageTitle(str, format, i - nImageCount, nImageCount, &m_DescImageList);
			
			nImageWidth = 0;
			nImageCount = 0;

			--i;
			continue;
		}
		nImageWidth += imgSize.cx;

		str += _T("<td class=\"img\" style=\"width:0px;\"><img src=\"");
		if (pImagePara->dwPara1)
		{
			TCHAR title[64] = {0};
			MakeImageTitle(p, title, 64, nIndex, i + 1);

			// ·�����������·��
			str += (LPCTSTR)pImagePara->dwPara3;
			str += title;

			if (bSaveFile)
			{
				SaveImageFile(p, pImagePara, title);
			}
		}
		else
		{
			int nLen;
			LPSTR pString = CTextManager::SwitchSave(str.c_str(), CS_UTF8, &nLen);
			if (!pString)
			{
				return;
			}
			if (!fwrite(pString, nLen - 1, 1, fp))
			{
				free(pString);
				return;
			}
			free(pString);
			str.erase();
			
			if (!fwrite(xml_lpszText_ImageHead, strlen(xml_lpszText_ImageHead), 1, fp))
			{
				return;
			}
			switch(p->nImageType)
			{
			case ITEMIMAGETYPE_DIB:
				if (!fwrite(xml_lpszText_ImageBmp, strlen(xml_lpszText_ImageBmp), 1, fp))
				{
					return;
				}
				break;
				
			case ITEMIMAGETYPE_JPEG:
				if (!fwrite(xml_lpszText_ImageJpeg, strlen(xml_lpszText_ImageJpeg), 1, fp))
				{
					return;
				}
				break;
				
			case ITEMIMAGETYPE_PNG:
				if (!fwrite(xml_lpszText_ImagePng, strlen(xml_lpszText_ImagePng), 1, fp))
				{
					return;
				}
				break;
			}
			
			p = (PITEMIMAGE)m_DescImageList.GetData(i);
			BinToBase64(NULL, p->ulImageSize, NULL, &ulSize);
			pData = (LPSTR)malloc(ulSize);
			if (!pData)
			{
				return;
			}
			BinToBase64(p->pImageData, p->ulImageSize, pData, &ulSize);
			if (!fwrite(pData, ulSize, 1, fp))
			{
				free(pData);
				return;
			}
			free(pData);
		}

		str += _T("\" alt=\"");
		str += p->szImageTitle ? p->szImageTitle : s_strNull;
		str += _T("\"></img></td>\n");

		++nImageCount;
	}

	if (m_DescImageList.GetDataCount())
	{
		WriteHTMLImageTitle(str, format, i - nImageCount, nImageCount, &m_DescImageList);
	}
}

void CItem::WriteHTMLImageTitle(_stdstring &str, LPCTSTR format, int nStart, int nCount, CDataList *pList)
{
	int i;
	TCHAR szTitle[64] = {0};
	PITEMIMAGE p;
	
	str += _T("</td></tr>\n<tr>");
	for (i = nStart; i < nStart + nCount; ++i)
	{
		p = (PITEMIMAGE)pList->GetData(i);
		str += _T("<td class=\"imgtitle\">");
		
		_sntprintf(szTitle, 64, format, i + 1);
		str += szTitle;
		if (p->szImageTitle && _tcslen(p->szImageTitle))
		{
			str += _T(" - ");
			str += p->szImageTitle;
		}
		str += _T("</td>");
	}
	
	str += _T("</tr></table>\n");
}

void CItem::MakeImageTitle(PITEMIMAGE p, LPTSTR pBuf, int nBufLen, int nIndex, int nFigIndex)
{
	if (TYPE_GROUP == GetType())
	{
		_sntprintf(pBuf, nBufLen, _T("gfig%d-%d"), nIndex, nFigIndex);
	}
	else
	{
		_sntprintf(pBuf, nBufLen, _T("fig%d-%d"), nIndex, nFigIndex);
	}
	
	switch(p->nImageType)
	{
	case ITEMIMAGETYPE_DIB:
		_tcsncat(pBuf, _T(".bmp"), nBufLen);
		break;
		
	case ITEMIMAGETYPE_JPEG:
		_tcsncat(pBuf, _T(".jpeg"), nBufLen);
		break;
		
	case ITEMIMAGETYPE_PNG:
		_tcsncat(pBuf, _T(".png"), nBufLen);
		break;
	}
}

void CItem::SaveImageFile(PITEMIMAGE p, PTPARAS pImagePara, LPTSTR title)
{
	_stdstring szPath;

	szPath = (LPCTSTR)pImagePara->dwPara2;
	szPath += title;
	
	FILE *fp = _tfopen(szPath.c_str(), _T("wb"));
	if (fp)
	{
		fwrite(p->pImageData, p->ulImageSize, 1, fp);
		fclose(fp);
	}
}

void CItem::MakeResolve(_stdstring &str, int nPaperType)
{
	TCHAR sz[64] = {0};
	_stdstring temp;

	if (!GetResolve() || !_tcslen(GetResolve()))
	{
		return;
	}

	switch(nPaperType)
	{
	case PAPERTYPE_TEXTONLY:
		LoadString(AfxGetInstanceHandle(), IDS_QUIZMKR_RESTXT, sz, 64);
		str += sz;
		str += GetResolve();
		str += CTextManager::s_szWinReturn;
		break;

	case PAPERTYPE_HTMLPLUS:
	case PAPERTYPE_HTMLONLY:
		LoadString(AfxGetInstanceHandle(), IDS_ITEM_RESOLVE, sz, 64);
		temp += _T("<div class=\"p\"><span style=\"font-weight:bold;\">");
		temp += sz;
		temp += _T("</span>");
		temp += GetResolve();
		temp += _T("</div>\n");
		ReplaceHTMLEndLine(temp);
		str += temp;
		break;
	}
}

void CItem::ReplaceHTMLEndLine(_stdstring &str)
{
	while(1)
	{
		int i = str.find(_T('\n'));
		if (i < 0)
		{
			break;
		}
		str.replace(i, 1, _T("<br/>"));
	}
	while(1)
	{
		int i = str.find(_T('\r'));
		if (i < 0)
		{
			break;
		}
		str.erase(i, 1);
	}
}

BOOL CItem::Match(LPCTSTR strMatch, BOOL bMatchCase)
{
	int bRet;
	LPCTSTR strDescription = GetDescription();

	if (bMatchCase)
	{
		bRet = (int)_tcsstr(strDescription, strMatch);
	}
	else
	{
		bRet = (int)_tcsstri(strDescription, strMatch);
	}

	return bRet ? TRUE : FALSE;
}

void CItem::CalcCRC()
{
	LPCTSTR strDescription = GetDescription();
	m_wCRC = m_nType;

#ifdef _UNICODE
	PBYTE ptr = (PBYTE)strDescription;
	int nLen = _tcslen(strDescription) * sizeof(TCHAR);
#else
	PBYTE ptr = (PBYTE)CTextManager::SwitchSave(strDescription, CS_UTF16, NULL);
	int nLen = wcslen((LPWSTR)ptr) * sizeof(WCHAR);
#endif

	int i;
	for (i = 0; i < nLen; i++)
	{
		m_wCRC += ptr[i];
	}

#ifndef _UNICODE
	free(ptr);
#endif

	// ��Ŀ����������У�����㣬��Ϊ���ܻᾭ���޸�
}

void CItem::SetUndoStep(int nUndoStep, int nUndoType)
{
	switch(nUndoType)
	{
	case UNDO_ADD:
	case UNDO_EDIT_ADD:
	case UNDO_ADD_CUSTOM:
		m_nUndoType[UNDO_ADDINDEX] = nUndoStep;
		break;

	case UNDO_DEL:
	case UNDO_EDIT_DEL:
	case UNDO_DEL_CUSTOM:
		m_nUndoType[UNDO_DELINDEX] = nUndoStep;
		break;

	default:
		break;
	}
}

BOOL CItem::NeedUndoClear(const UNDOSTEP *pStep)
{
	BOOL bRet = FALSE;
	BOOL bIsClearNewest;
	int nStart, nEnd;

	// �����ǰ���費�������²���
	// ����Ҫ����ǰ����֮��Ĳ���
	if (pStep->nNewestStep != pStep->nCurStep)
	{
		nStart = (pStep->nCurStep + 1) % UNDO_SIZE;
		nEnd = pStep->nNewestStep;
		bIsClearNewest = TRUE;
	}
	// �����ǰ������Ҫ������ɵĲ���
	// ����Ҫ������ɵ��Ǹ�����
	else if (((pStep->nCurStep + 1) % UNDO_SIZE) == pStep->nOldestStep)
	{
		nStart = pStep->nOldestStep;
		nEnd = pStep->nOldestStep;
		bIsClearNewest = FALSE;
	}
	// �����������������
	else
	{
		return FALSE;
	}

	while(1)
	{
		if (m_nUndoType[UNDO_DELINDEX] == nStart)
		{
			// ������������Ȼ�ڱ�ɾ����֮ǰ
			// ˵��������ǵ�ǰ����֮��Ĳ���
			// �����

			// ���û����������������
			if (m_nUndoType[UNDO_ADDINDEX] < 0)
			{
				// ������ǰ����֮��Ĳ���ʱ�������
				// ��������ɲ���ʱ�������������
				bRet = !bIsClearNewest;
			}

			m_nUndoType[UNDO_DELINDEX] = -1;
		}
		else if (m_nUndoType[UNDO_ADDINDEX] == nStart)
		{
			// �����ɾ������Ȼ�ڱ������֮��
			// ˵�����������ɵĲ���
			// �����

			// ���û��ɾ�������������
			if (m_nUndoType[UNDO_DELINDEX] < 0)
			{
				// ������ǰ����֮��Ĳ���ʱ�������������
				// ��������ɵĲ���ʱ�������
				bRet = bIsClearNewest;
			}

			m_nUndoType[UNDO_ADDINDEX] = -1;
		}

		if (nStart == nEnd)
		{
			break;
		}
		nStart = (nStart + 1) % UNDO_SIZE;
	}

	return bRet;
}

BOOL CItem::IsUndoValid(const UNDOSTEP *pStep)
{
	BOOL bRet = FALSE;

	// �ӵ�ǰ���迪ʼ��ǰ��
	int nCurStep = pStep->nCurStep;

	while(1)
	{
		// �����ĳһ����ö���ɾ������ö�����Ч
		if (m_nUndoType[UNDO_DELINDEX] == nCurStep)
		{
			return FALSE;
		}
		// �����ĳһ����ö��󱻼��룬��ö�����Ч
		else if (m_nUndoType[UNDO_ADDINDEX] == nCurStep)
		{
			return TRUE;
		}

		if (nCurStep == pStep->nOldestStep)
		{
			break;
		}
		nCurStep = (nCurStep + UNDO_SIZE - 1) % UNDO_SIZE;
	}

	// ����ڵ�ǰ����ɵĲ����У�û����ӻ�ɾ���Ĳ���
	// ����������ʵ�����Ǽ�鵱ǰ����²������Ƿ��в���
	if (m_nUndoType[UNDO_ADDINDEX] >= 0)
	{
		return FALSE;
	}
	if (m_nUndoType[UNDO_DELINDEX] >= 0)
	{
		return TRUE;
	}

	return TRUE;
}

BOOL CItem::SetResolve(LPCTSTR szResolve)
{
	if (!szResolve)
	{
		return TRUE;
	}

	LPCTSTR szSet = NULL;
	if (!CTextManager::CheckValidCharactor(szResolve))
	{
		szSet = _T("");
	}
	else
	{
		szSet = (LPCTSTR)szResolve;
	}
	
	if (m_strResolve)
	{
		free(m_strResolve);
		m_strResolve = NULL;
	}
	
	m_strResolve = _tcsdup(szSet);
	if (!m_strResolve)
	{
		return FALSE;
	}
	
	return TRUE;
}

LPCTSTR CItem::GetResolve()
{
	if (m_strResolve)
	{
		return m_strResolve;
	}
	else
	{
		return s_strNull;
	}
}

BOOL CItem::IsSameItem(CItem *pItem, int nSimilar)
{
	if (m_nType != pItem->m_nType)
	{
		return FALSE;
	}

	if (nSimilar > 99)
	{
		if (_tcscmp(m_strDescription, pItem->m_strDescription))
		{
			return FALSE;
		}
	}
	else
	{
		CTextSimilarity t;
		t.Compare(m_strDescription, pItem->m_strDescription);
		if ((int)(t.GetInfo() * 100.f) < nSimilar)
		{
			return FALSE;
		}
	}

	return TRUE;
}

PITEMIMAGE CItem::GetImage(int nFlag, int nIndex)
{
	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		return (PITEMIMAGE)m_DescImageList.GetData(nIndex);

	case ITEMIMAGEFLAG_RESOLVE:
		return (PITEMIMAGE)m_ResvImageList.GetData(nIndex);

	default:
		ASSERT(FALSE);
		return NULL;
	}
}

CDataList* CItem::GetImageList(int nFlag)
{
	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		return &m_DescImageList;
		
	case ITEMIMAGEFLAG_RESOLVE:
		return &m_ResvImageList;
		
	default:
		ASSERT(FALSE);
		return NULL;
	}
}

int CItem::GetImageCount(int nFlag)
{
	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		return m_DescImageList.GetDataCount();

	case ITEMIMAGEFLAG_RESOLVE:
		return m_ResvImageList.GetDataCount();

	default:
		ASSERT(FALSE);
		return -1;
	}
}

void CItem::SaveImage(int nFlag, CDataList *pList)
{
	CDataList *pItemList;
	switch(nFlag)
	{
	case ITEMIMAGEFLAG_DESC:
		pItemList = &m_DescImageList;
		break;
		
	case ITEMIMAGEFLAG_RESOLVE:
		pItemList = &m_ResvImageList;
		break;
		
	default:
		ASSERT(FALSE);
		return;
	}

	ASSERT(!pItemList->GetDataCount());
	if (pList == pItemList)
	{
		return;
	}

	while(pList->GetDataCount())
	{
		pItemList->MoveData(pList, 0, -1);
	}
}

PITEMIMAGE FUNCCALL CItem::CreateItemImage()
{
	PITEMIMAGE p = (PITEMIMAGE)malloc(sizeof(ITEMIMAGE));
	if (!p)
	{
		return NULL;
	}
	ZeroMemory(p, sizeof(ITEMIMAGE));

	return p;
}

BOOL CItem::SetItemImage(CDataList *pList, int nIndex, int nImageType, PBYTE pImageData, size_t ulImageSize)
{
	SIZE sz = {0};
	PITEMIMAGE p = CreateItemImage();
	if (!p)
	{
		return FALSE;
	}

	p->nImageType = nImageType;
	p->pImageData = pImageData;
	p->ulImageSize = ulImageSize;
	p->szImageTitle = NULL;
	
	if (!pList->AddData(nIndex, p, CItem::ClearItemImage))
	{
		free(p);
		return FALSE;
	}
	return TRUE;
}

BOOL CItem::SetItemImageTitle(CDataList *pList, int nIndex, LPCTSTR lpszTitle)
{
	PITEMIMAGE p = (PITEMIMAGE)pList->GetData(nIndex);
	if (!p)
	{
		return FALSE;
	}

	if (p->szImageTitle)
	{
		free(p->szImageTitle);
		p->szImageTitle = NULL;
	}
	if (lpszTitle)
	{
		p->szImageTitle = _tcsdup(lpszTitle);
	}

	return (NULL != p->szImageTitle);
}

int CItem::ClearItemImage(PVOID pData)
{
	PITEMIMAGE p = (PITEMIMAGE)pData;
	if (p->pImageData)
	{
		free(p->pImageData);
		p->pImageData = NULL;
		p->ulImageSize = 0;
	}
	if (p->szImageTitle)
	{
		free(p->szImageTitle);
		p->szImageTitle = NULL;
	}

	free(pData);
	return TRUE;
}

EBOOL CItem::MakeImageBase64(PITEMIMAGE p, std::string &string)
{
	LPSTR pData = NULL;
	size_t ulSize;
	
	BinToBase64(NULL, p->ulImageSize, NULL, &ulSize);
	pData = (LPSTR)malloc(ulSize + 1);
	if (!pData)
	{
		return Q_ERROR(ERROR_ALLOC_FAIL);
	}

	BinToBase64(p->pImageData, p->ulImageSize, pData, &ulSize);
	pData[ulSize] = 0;
	string += (LPSTR)pData;

	free(pData);
	return Q_TRUE;
}

EBOOL CItem::MakeImageBin(PBYTE *pImageData, size_t *pImageSize, int *pType, LPCSTR pBase64)
{
	int nType;
	LPCSTR ptr = pBase64;
	if (strnicmp(ptr, xml_lpszText_ImageHead, strlen(xml_lpszText_ImageHead)))
	{
		return Q_WARNING(ERROR_IMAGE_BASE64_INCORRECT);
	}

	ptr += strlen(xml_lpszText_ImageHead);
	if (!strnicmp(ptr, xml_lpszText_ImageJpeg, strlen(xml_lpszText_ImageJpeg)))
	{
		nType = ITEMIMAGETYPE_JPEG;
		ptr += strlen(xml_lpszText_ImageJpeg);
	}
	else if (!strnicmp(ptr, xml_lpszText_ImageJpg, strlen(xml_lpszText_ImageJpg)))
	{
		nType = ITEMIMAGETYPE_JPEG;
		ptr += strlen(xml_lpszText_ImageJpg);
	}
	else if (!strnicmp(ptr, xml_lpszText_ImageBmp, strlen(xml_lpszText_ImageBmp)))
	{
		nType = ITEMIMAGETYPE_DIB;
		ptr += strlen(xml_lpszText_ImageBmp);
	}
	else if (!strnicmp(ptr, xml_lpszText_ImagePng, strlen(xml_lpszText_ImagePng)))
	{
		nType = ITEMIMAGETYPE_PNG;
		ptr += strlen(xml_lpszText_ImagePng);
	}
	else
	{
		// �в�֧�ֵ�ͼ���ʽҲ��Ӱ���ȡ
		return Q_WARNING(ERROR_INVALID_IMAGE_FORMAT);
	}

	PBYTE pData = NULL;
	size_t ulDataLen = 0;
	Base64ToBin(NULL, strlen(ptr), NULL, &ulDataLen);

	pData = (PBYTE)malloc(ulDataLen);
	if (!pData)
	{
		return Q_WARNING(ERROR_ALLOC_FAIL);
	}

	Base64ToBin((LPSTR)ptr, strlen(ptr), pData, &ulDataLen);
	if (!ulDataLen)
	{
		free(pData);
		return Q_WARNING(ERROR_BASE64_DECODE_FAIL);
	}

	SIZE sz = {0};
	if (!ReadImageSize(nType, pData, ulDataLen, &sz))
	{
		free(pData);
		return Q_WARNING(ERROR_BASE64_DECODE_FAIL);
	}

	*pType = nType;
	*pImageData = pData;
	*pImageSize = ulDataLen;
	return Q_RET(TRUE);
}
