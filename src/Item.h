#if !defined(AFX_ITEM_H__A32C9DEE_2349_4E67_8620_37E94876F67D__INCLUDED_)
#define AFX_ITEM_H__A32C9DEE_2349_4E67_8620_37E94876F67D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Item.h : header file
//

// tinyxml֧��
#include "tinyxml.h"
#include "XmlTags.h"

#include "DataList.h"
#include "TestPara.h"

class CList;
class CPaper;
class CItemGroup;

// ��Ŀ����
#define TYPE_SCHOISE			1
#define TYPE_MCHOISE			2
#define TYPE_JUDGE				3
#define TYPE_BLANK				4
#define TYPE_TEXT				5
#define TYPE_GROUP				6

enum ItemIndex {
	ITEMINDEX_SCHOISE = 0,
	ITEMINDEX_CUSTOM = 0,
	ITEMINDEX_MCHOISE,
	ITEMINDEX_JUDGE,
	ITEMINDEX_BLANK,
	ITEMINDEX_TEXT,
	ITEMINDEX_GROUP
};

#define TYPEMASK_SCHOISE		0x00000001L
#define TYPEMASK_MCHOISE		0x00000002L
#define TYPEMASK_JUDGE			0x00000004L
#define TYPEMASK_BLANK			0x00000008L
#define TYPEMASK_TEXT			0x00000010L
#define TYPEMASK_GROUP			0x00000020L
#define TYPEMASK_1STCUSTOM		0x00000040L

#define ITEMMARK_ALL			0xFFFFFFFFL
#define ITEMMARK_NULL			0
#define ITEMMARK_EDIT			0x00000001L
#define ITEMMARK_DELETE			0x00000002L
#define ITEMMARK_UNCERTAIN		0x00000004L
#define ITEMMARK_STORE			0x00000008L

#define MKTXT_DEFAULT			0
#define MKTXT_ENDLINE			0x00000001L	// ��ȫ����ͨ�á���Ŀ�����һ������
#define MKTXT_OPTIONNEWLINE		0x00000002L	// ��ѡ����ר�á�ѡ�����Ŀ�ڲ�ͬ��
#define MKTXT_OPTIONSINALINE	0x00000004L // ��ѡ����ר�á�����ѡ����ͬһ����ʾ
#define MKTXT_OPTIONLEN			0x00000008L	// ��ѡ����ר�á�����ѡ�����ʾ��ȣ��԰���ֿ��Ϊ��λ��
#define MKTXT_JUDGEBRACKET		0x00000010L	// ���ж���ר�á���Ŀ���������ţ�Ĭ��û�����ţ�

#define MKASW_DEFAULT			0
#define MKASW_WITHOPTION		0x00010000L	// ��ѡ����ר�á����а���ѡ������
#define MKASW_ADDSPACE			0x00020000L // ��ѡ����ר�á��ڴ�ǰ����ӿո�
#define MKASW_ASWINQUESTION		0x00040000L	// ��ȫ����ͨ�á�����ȷ��д����Ŀ��
#define MKASW_WITHINDEX			0x00080000L	// �������ר�á�ÿ����ǰ�б����ʾ
#define MKASW_INALINE			0x00100000L	// �������ר�á���������ʾ��ÿ����������������
#define MKASW_ENDLINE			0x00200000L // ��ȫ����ͨ�á��ڴ�����һ������
#define MKASW_WITHRESOLVE		0x00400000L // ��ȫ����ͨ�á��ڴ�����ӽ���������MKASW_ASWINQUESTION��Чʱ��Ч��

#define CHSOPTION_SCHASMCH		0x00000001L
#define CHSOPTION_MCHASSCH		0x00000002L
#define CHSOPTION_MCHHALF		0x00000004L
#define CHSOPTION_MCHNOSINGLE	0x00000008L
#define CHSOPTION_RANDOM		0x00000010L
#define CHSOPTION_CUSTOMASWELL	0x00000020L
#define BLKOPTION_ALLRIGHT		0x00000001L
#define BLKOPTION_ONLYBLK		0x00000002L
#define BLKOPTION_RANDOMBLK		0x00000004L
#define TXTOPTION_ALLRIGHT		0x00000001L

#define WRONG					0
#define HALFRIGHT				1
#define ALLRIGHT				2

#define UNDO_SIZE				10
#define UNDO_NULL				(-1)
#define UNDO_DESC				0		// �޸��������
#define UNDO_ADD				1		// �½���Ŀ
#define UNDO_EDIT_DEL			2		// �༭��Ŀʱɾ������Ŀ
#define UNDO_EDIT_ADD			3		// �༭��Ŀʱ�������Ŀ
#define UNDO_DEL				4		// ɾ����Ŀ
#define UNDO_ADD_CUSTOM			5		// �½��Զ�������
#define UNDO_DEL_CUSTOM			6		// ɾ���Զ�������
#define UNDO_EDIT_CUSTOM		7		// �޸��Զ�����������
#define UNDO_COVER				8		// �޸�������

#define UNDO_ADDINDEX			0		// ��Ŀ����Ӳ��裨����m_nUndoType��
#define UNDO_DELINDEX			1		// ��Ŀ��ɾ�����裨����m_nUndoType��
#define UNDO_ADDITEM			1		// ���ڴ��ݲ���
#define UNDO_DELITEM			2		// ���ڴ��ݲ���

#define IMAGE_MAXCOUNT			10

enum BatchSet {
	BATCHSET_STOM = 0,
	BATCHSET_MTOS,
	BATCHSET_SELDUPLICATE,
	BATCHSET_CTOJ
};

typedef struct _tagUndoStep
{
	int nCurStep;		// ��ǰ����
	int nNewestStep;	// ���²���
	int nOldestStep;	// ��ɲ���
	int nSaveIndex;		// ���һ�α��沽��
	int nLastUndoType;	// ��һ�ζ�������
}UNDOSTEP, *LPUNDOSTEP;
typedef const LPUNDOSTEP	LPCUNDOSTEP;

#define ITEMIMAGESIZE_MAXX			640
#define ITEMIMAGESIZE_MAXY			640
enum ItemImageType {
	ITEMIMAGETYPE_DIB = 0,
	ITEMIMAGETYPE_JPEG,
	ITEMIMAGETYPE_PNG
};
typedef struct _tagItemImage
{
	int nImageType;
	PBYTE pImageData;
	size_t ulImageSize;
	LPTSTR szImageTitle;
}ITEMIMAGE, *PITEMIMAGE;

#define ITEMIMAGEFLAG_DESC			0
#define ITEMIMAGEFLAG_RESOLVE		1

/////////////////////////////////////////////////////////////////////////////
// CItem window

class CItem
{
public:
	CItem();
	virtual ~CItem();

public:
	inline void SetPrev(CItem *pPrev) { m_pPrev = pPrev; }
	inline CItem* GetPrev() { return (m_pPrev); }

	inline void SetNext(CItem *pNext) { m_pNext = pNext; }
	inline CItem* GetNext() { return (m_pNext); }

	inline CList* GetParent() { return (m_pParent); }
	inline void SetParent(CList *pParent) { m_pParent = pParent; }

	virtual void SetType(int nType) { ASSERT(FALSE); }
	virtual BOOL IsPaper() { return FALSE; }
	virtual BOOL IsList() { return FALSE; }
	virtual BOOL IsItemPtr() { return FALSE; }

	inline int GetType() { return CItem::GetType(m_nType); }
	static inline int GetType(int nType) { return (nType % TYPE_CUSTOMRIGHT); }
	inline int GetRawType() { return m_nType; }

	virtual LPCTSTR GetTypeDesc();
	inline BOOL IsDefaultType() { return CItem::IsDefaultType(m_nType); }
	static inline BOOL IsDefaultType(int nType) { return (nType <= TYPE_LASTDEFAULT); }
	inline BOOL IsSingleDefaultType() { return CItem::IsSingleDefaultType(); }
	static inline BOOL IsSingleDefaultType(int nType) { return (nType <= TYPE_TEXT); }
	inline BOOL IsSubjectiveType() { return (GetType() == TYPE_BLANK || GetType() == TYPE_TEXT); }

	inline int GetCustomIndex() { return CItem::GetCustomIndex(m_nType); }
	static inline int GetCustomIndex(int nType) { return (nType / TYPE_CUSTOMRIGHT); }
	virtual void SetCustomIndex(int nIndex);

	BOOL CheckTypeMask(DWORD dwMask);

	virtual void ChangeAsPara(CTestPara *pCPara, int nExPara = 0) { ASSERT(FALSE); }

	virtual void SetItemState(UINT nState, UINT nMark);
	virtual UINT GetItemState(UINT nMark) { return (m_uState & nMark); }
	virtual void SetStore(BOOL bStore) { CItem::SetItemState((bStore ? ITEMMARK_STORE : ITEMMARK_NULL), ITEMMARK_STORE); }
	virtual BOOL IsStored() { return (CItem::GetItemState(ITEMMARK_STORE) ? TRUE : FALSE); }
	virtual BOOL SaveStore(FILE *fp);
	virtual BOOL LoadStore(FILE *fp);

	virtual CItem* GetItemFrom() { return m_pFrom ? m_pFrom : this; }
	void ClearFromPtr() { m_pFrom = NULL; }

	virtual EBOOL LoadFile(FILE *fp, DWORD dwVersion); // ����LoadFile���������صĶ��Ƿ���������
	virtual EBOOL SaveFile(FILE *fp);
	virtual RETURN_E TiXmlElement* WriteXML(TiXmlElement *pParent, int nEncode, BOOL bMaxCompatible);
	virtual EBOOL ReadXML(TiXmlElement *pQstn, int nEncode, BOOL bGroup); // ����ReadXML���������صĶ��Ƿ���������

	virtual LPCTSTR GetDescription();
	virtual int GetDescriptionSaveLen();
	virtual BOOL SetDescription(LPCTSTR szDescription);
#ifndef _UNICODE
	BOOL SetWDescription(LPCWSTR szDescription);
#endif
	int GetDescriptionSummary(int nChs);

	virtual void ClearAllInfo();
	virtual void ClearUserInfo();

	virtual CItem* Alloc() { ASSERT(FALSE); return NULL; }
	virtual BOOL CopyItem(CItem *pItem);
	virtual void ClipCopy(CFixedSharedFile &sf);
	virtual BOOL ClipPaste(CFixedSharedFile &sf);

	virtual void MakeText(_stdstring &str, UINT nStyle, int nPara) { ASSERT(FALSE); }
	virtual void MakeAnswer(_stdstring &str, UINT nStyle, int nPara = 0) { ASSERT(FALSE); }
	virtual void MakeUserAnswer(_stdstring &str) { ASSERT(FALSE); }
	virtual BOOL CheckItemValid() { ASSERT(FALSE); return FALSE; }

	virtual void MakeTextOut(_stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	void MakeTextImage(_stdstring &str, int nIndex, PTPARAS pImagePara, BOOL bSaveFile);
	virtual void MakeHTML(FILE *fp, _stdstring &str, int nListIndex, int nItemIndex, PTPARAS pImagePara,
		BOOL bWithAnswer, BOOL bWithRes);
	void MakeHTMLImage(FILE *fp, _stdstring &str, int nIndex, PTPARAS pImagePara, BOOL bSaveFile);
	void MakeImageTitle(PITEMIMAGE p, LPTSTR pBuf, int nBufLen, int nIndex, int nFigIndex);
	void SaveImageFile(PITEMIMAGE p, PTPARAS pImagePara, LPTSTR title);
	void MakeResolve(_stdstring &str, int nPaperType);
	static void ReplaceHTMLEndLine(_stdstring &str);

	// CheckAnswer()��ȡ�Ѿ����ֵĴ𰸣�ֻ����ȫ�ԡ���Ժʹ�
	virtual int CheckAnswer(int *nRightCount = NULL) { return m_nAnswerCheck; }
	// Score()���տ������ý������֣������ط���
	virtual int Score(CTestPara *pCPara) { ASSERT(FALSE); return 0; }
	virtual BOOL IsAnswered() { ASSERT(FALSE); return FALSE; }

	virtual BOOL Match(LPCTSTR strMatch, BOOL bMatchCase);

	virtual void SetU32Answer(DWORD dwAnswer) { ASSERT(FALSE); }
	
	virtual void CalcCRC();
	inline WORD GetCRC() { return m_wCRC; }

	void SetUndoStep(int nUndoStep, int nUndoType);
	BOOL NeedUndoClear(const UNDOSTEP *pStep);
	BOOL IsUndoValid(const UNDOSTEP *pStep);

	BOOL SetResolve(LPCTSTR szResolve);
	virtual LPCTSTR GetResolve();

	virtual BOOL IsSameItem(CItem *pItem, int nSimilar);

	PITEMIMAGE GetImage(int nFlag, int nIndex);
	CDataList* GetImageList(int nFlag);
	int GetImageCount(int nFlag);
	void SaveImage(int nFlag, CDataList *pList);

	static PITEMIMAGE FUNCCALL CreateItemImage();
	static BOOL SetItemImage(CDataList *pList, int nIndex, int nImageType, PBYTE pImageData, size_t ulImageSize);
	static BOOL SetItemImageTitle(CDataList *pList, int nIndex, LPCTSTR lpszTitle);
	static int FUNCCALL ClearItemImage(PVOID pData);

protected:
	EBOOL MakeImageBase64(PITEMIMAGE p, std::string &string);
	EBOOL MakeImageBin(PBYTE *pImageData, size_t *pImageSize, int *pType, LPCSTR pBase64);

	EBOOL WriteItemImage(TiXmlElement *pQstn, int nEncode, int nFlag);
	EBOOL LoadItemImage(TiXmlElement *pQstn, int nEncode, int nFlag);

	static void WriteHTMLImageTitle(_stdstring &str, LPCTSTR format, int nStart, int nCount, CDataList *pList);

public:
	static LPCTSTR s_strNull;

protected:
	int m_nType;
	TCHAR m_szTypeDesc[TYPE_BUFFSIZE];
	LPTSTR m_strDescription; // ��Ŀ����
	LPTSTR m_strResolve; // ��Ŀ����

	CDataList m_DescImageList; // ���ͼƬ�б�
	CDataList m_ResvImageList; // ��Ŀ����ͼƬ�б�
	static int s_nImageListFlag[];

	WORD m_wCRC;

	int m_nAnswerCheck; // �����Ѿ������ֵĶԴ����

private:
	CList *m_pParent;
	CItem *m_pPrev;
	CItem *m_pNext;

	// �༭���
	UINT m_uState;

	// ÿ����Ŀ�������ֻ����һ��
	// ��������������ɵ���Ŀȫ�����д�ֵ
	// �������Ŀ�Ĵ�ֵΪNULL
	CItem *m_pFrom;

	// ����Ŀ�ĳ�����������Ӻ�ɾ�����ڵĲ��裩
	int m_nUndoType[2];
};


#endif // !defined(AFX_ITEM_H__A32C9DEE_2349_4E67_8620_37E94876F67D__INCLUDED_)
