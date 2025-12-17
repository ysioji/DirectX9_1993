#pragma once


#define MAX_BULLET (3)

//enum E_TEXTURE;

struct Vector2
{
	float  x;
	float  y;

};
//タンク
struct TankStatus
{
	Vector2 pos;   //座標
	Vector2 vel;   //速度成分
	int moveSpeed; //移動速度	
	int hp;		   //体力値
	float angle;   // 90 180 270 360
	int alpha;     //255不透明　0完全透明
	bool isDead;   //生存
};

class TankBase {


protected:
	E_TEXTURE   m_texName;// 表示するテクスチャ.
	TankStatus  m_tstatus;//搭載物の情報
	
	TankBase()
		:m_texName(TEX_INVALID) {
		 m_tstatus.pos = { 0.0f, 0.0f };
		 m_tstatus.vel = { 0.0f, 0.0f };
		 m_tstatus.moveSpeed = 0;
		 m_tstatus.hp = 0;
		 m_tstatus.angle = 0;
		 m_tstatus.alpha = 255;
		 m_tstatus.isDead = false;
	
		// _tprintf(_T("call PlayerBase constractor\n"));
	}
public:
	virtual ~TankBase() = default;
	virtual void Init(float tx, float ty, float tvx, float tvy, int tspeed, int thp,bool isDead);//初期化
	virtual void Move() =0;
	virtual void Fire() =0;
	void Updata();//毎フレーム呼び出し
	virtual void Draw(E_TEXTURE texName);//描画タンク/パイロット本体

	Vector2 GetPos() { return m_tstatus.pos;}
	float  GetAngle() {return m_tstatus.angle;}

	void IsLive();
	

	
	bool IsDead() const { return  m_tstatus.isDead; }
	void Kill() { m_tstatus.isDead = true; }
	//int GetHp
	void Damage(int d)
	{
		m_tstatus.hp -= d;
		if (m_tstatus.hp <= 0)
			m_tstatus.isDead = true;
	}
	//3期

};