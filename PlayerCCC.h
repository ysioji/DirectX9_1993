#pragma once
#include <vector>

class PlayerCCC : public TankBase {

private:
	int m_facing;   //顔の向き　時計の時方向
	ENUM_FIELD_OBJECT2 m_field2Pos;//MASU目上の自座標
	ENUM_FIELD_OBJECT2 m_buff;//フィールド格納用
	Field* m_pfield;
	std::vector<BulletBase> m_bullets;
	int m_maxBullets;
	BulletBase CreateBullet();
public:

	PlayerCCC() :
		m_facing(0)
		//m_fieldPos(FIELD_OBJECT2_NOTHING)
		, m_maxBullets(0)
	{
		m_texName = TEX_PCCC;
		m_pfield = nullptr;


	};
	void Init(Field* f);
	void Move()override;
	void Fire()override;
	void DrawBullets();
	//キーを押した方向でそのマスが通れるかチェック
	//戻り値がtrue  通れる
	//        false 通れない
	bool CheckLayOut1();
	bool CheckLayOut2();
	Vector2 GetBulletStartPos();
	//3
	std::vector<BulletBase>& GetBullets() { return m_bullets; }
	const Vector2& GetPos() const { return m_tstatus.pos; }
	int GetHp() const { return m_tstatus.hp; }
	void Damage(int d) { m_tstatus.hp -= d; }

	//3期
	void DrawPoto();
	void DrawNation();
	void DrawHpBar();
	void DrawTankMark();
	void DrawTankRing();
	void DrawTankSpeed();
	void DrawPilotMark();
};