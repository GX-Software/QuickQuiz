#ifndef _FIXEDSHAREDFILE_H
#define _FIXEDSHAREDFILE_H

#ifdef __AFXADV_H__


// **************************************************************************
class CFixedSharedFile : public CSharedFile
{
public:
	CFixedSharedFile(int nGrowBy = 128) : CSharedFile (GMEM_DDESHARE | GMEM_MOVEABLE, nGrowBy)
	{
	}
	
	BYTE* GetBuffer()
	{
		ASSERT (m_lpBuffer);
		return (m_lpBuffer);
	}
	
	HGLOBAL Detach()
	{
		ASSERT (m_hGlobalMemory != NULL);
		HGLOBAL hMem = m_hGlobalMemory;
		m_hGlobalMemory = NULL;
		
		// 修正了MFC的固有BUG
		// 来自Kepware cop.
		::GlobalUnlock (hMem);
		
		m_lpBuffer = NULL;
		m_nBufferSize = 0;
		
		return hMem;
	}
};

#endif	// __AFXADV_H__
#endif	// _FIXEDSHAREDFILE_H
