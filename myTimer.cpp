#include "stdafx.h"
#include "myTimer.h"

// ŠJŽn‚©‚ç‚ÌŒo‰ßŽžŠÔ‚ð“¾‚é.
void MyTimer::GetPassTime(LONGLONG* pMicroSec, UINT* pFps)
{
	LARGE_INTEGER endTime;
	LARGE_INTEGER elapsedMS;

	QueryPerformanceCounter(&endTime);
	elapsedMS.QuadPart = endTime.QuadPart - m_startTime.QuadPart;

	elapsedMS.QuadPart *= 1000000;
	elapsedMS.QuadPart /= m_frequency.QuadPart;

	*pMicroSec = elapsedMS.QuadPart;
	*pFps = (UINT)(1000000 / elapsedMS.QuadPart);
}
