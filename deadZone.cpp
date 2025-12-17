#include "stdafx.h"
#include "deadZone.h"

void DeadZone::Init(Field* f)
{
	m_pField = f;

	// 20x20 のランダム開始位置
	int maxStart = 26 - 20; // = 6
	int sx = GetRand(maxStart);
	int sy = GetRand(maxStart);

	m_left = sx;
	m_top = sy;
	m_right = sx + 20 - 1;
	m_bottom = sy + 20 - 1;

	m_round = 1;
	m_timer = 0;
}
void DeadZone::Update(float delta)
{
	m_timer += delta;

	// 30秒ごとに縮小
	if (m_timer >= 30.0f)
	{
		m_timer = 0;
		ShrinkRound();
	}
}
void DeadZone::ShrinkRound()
{
	switch (m_round)
	{
	case 1: // Round1 → Round2
		m_left += 5;
		m_top += 5;
		m_right -= 5;
		m_bottom -= 5;
		break;

	case 2: // Round2 → Round3
		m_left += 9;
		m_top += 9;
		m_right -= 9;
		m_bottom -= 9;
		break;

	case 3: // Round3 → Round4
		m_left += 6;
		m_top += 6;
		m_right -= 6;
		m_bottom -= 6;
		break;

	case 4: // Round4 → 完了
		m_left += 3;
		m_top += 3;
		m_right -= 3;
		m_bottom -= 3;
		break;
	}

	m_round++;
}
bool DeadZone::IsInside(int mx, int my)
{
	return (mx >= m_left && mx <= m_right &&
		my >= m_top && my <= m_bottom);
}


//void Field::DrawField1()
//{
//	DeadZone* dz = GetAppInst()->GetDeadZone();
//
//	for (int i = 0; i < ROWS; i++)
//	{
//		for (int j = 0; j < COLS; j++)
//		{
//			bool inside = dz->IsInside(j, i);
//
//			int alpha = inside ? 255 : 120; // ゾーン外は半透明赤
//			DWORD color = inside ? 0xFFFFFFFF : 0x80FF0000; // 赤
//
//			DrawTile(j, i, color);
//		}
//	}
//}
