#pragma once

#include "field.h"

class DeadZone
{
private:
	int m_left;     // ゾーン左
	int m_top;      // ゾーン上
	int m_right;    // ゾーン右
	int m_bottom;   // ゾーン下

	int m_round;    // 現在ラウンド (1〜4)
	float m_timer;  // 時間計測用

	Field* m_pField;

public:
	DeadZone() :
		m_left(0), m_top(0), m_right(25), m_bottom(25),
		m_round(0), m_timer(0), m_pField(nullptr)
	{}

	void Init(Field* f);

	void Update(float delta);
	void ShrinkRound();
	bool IsInside(int mx, int my); // そのマスがゾーン内か？
};
