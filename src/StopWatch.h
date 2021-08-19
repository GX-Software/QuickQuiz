// StopWatch.h: interface for the CStopWatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOPWATCH_H__1F8FCC79_1865_4750_BC33_DD77207E56CF__INCLUDED_)
#define AFX_STOPWATCH_H__1F8FCC79_1865_4750_BC33_DD77207E56CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStopWatch  
{
public:
	CStopWatch();
	virtual ~CStopWatch();

	void Start();
	void End();
	
	double GetDur() { return m_lfDur; }

protected:
	double m_lfDur;

	BOOL m_bHighResolution;

	LARGE_INTEGER m_lnFreq;
	LARGE_INTEGER m_lnStart;
	LARGE_INTEGER m_lnEnd;

	clock_t m_clkStart;
	clock_t m_clkEnd;

	long GetTimeDur(PSYSTEMTIME pTime1, PSYSTEMTIME pTime2);
};

#endif // !defined(AFX_STOPWATCH_H__1F8FCC79_1865_4750_BC33_DD77207E56CF__INCLUDED_)
