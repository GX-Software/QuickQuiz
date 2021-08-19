// StopWatch.cpp: implementation of the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StopWatch.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStopWatch::CStopWatch() :
	m_lfDur(0.f), m_bHighResolution(TRUE)
{

}

CStopWatch::~CStopWatch()
{

}

void CStopWatch::Start()
{
	// 如果支持高精度计时器
	if (QueryPerformanceFrequency(&m_lnFreq))
	{
		m_bHighResolution = TRUE;
		QueryPerformanceCounter(&m_lnStart);
	}
	// 如果不支持，就只能采用低精度计时了
	else
	{
		m_bHighResolution = FALSE;
		m_clkStart = clock();
	}
}

void CStopWatch::End()
{
	if (m_bHighResolution)
	{
		QueryPerformanceCounter(&m_lnEnd);

		double lfDur = (double)(m_lnEnd.QuadPart - m_lnStart.QuadPart);
		m_lfDur = lfDur / m_lnFreq.QuadPart;
	}
	else
	{
		m_clkEnd = clock();

		m_lfDur = (m_clkEnd - m_clkStart) / CLOCKS_PER_SEC;
	}
}

long CStopWatch::GetTimeDur(PSYSTEMTIME pTime1, PSYSTEMTIME pTime2)
{
	_FILETIME fTime1 = {0}, fTime2 = {0};

	SystemTimeToFileTime(pTime1, &fTime1);
	SystemTimeToFileTime(pTime2, &fTime2);

	ULARGE_INTEGER ulTime1 = {fTime1.dwLowDateTime, fTime1.dwHighDateTime};
	ULARGE_INTEGER ulTime2 = {fTime2.dwLowDateTime, fTime2.dwHighDateTime};

	return (long)(ulTime2.QuadPart - ulTime1.QuadPart);
}
