/*
	myTimer.h
*/
#pragma once

/////////////////////////////////////////////////////////////
// ↓タイマークラス.

class MyTimer
{
private:
	LARGE_INTEGER m_startTime;
	LARGE_INTEGER m_frequency;

public:
	MyTimer()
	{
		m_startTime.QuadPart = 0;
		m_frequency.QuadPart = 0;
		InitTimer();
	}
	~MyTimer()
	{
	};

	// タイマー初期化.
	void InitTimer()
	{
		QueryPerformanceFrequency(&m_frequency);
	}
	// タイマー開始.
	void StartTimer()
	{
		QueryPerformanceCounter(&m_startTime);
	}
	// 開始からの経過時間を得る.
	void GetPassTime(LONGLONG*, UINT*);
};

// ↑タイマークラス.
/////////////////////////////////////////////////////////////
