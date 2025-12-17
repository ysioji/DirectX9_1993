#pragma once

struct PilotStatus
{
	Vector2 pos;   //座標
	Vector2 vel;   //速度成分
	int moveSpeed; //移動速度	
	int hp;		   //体力値
	int alpha;     //255不透明　0完全透明
	bool isDead;   //生存
};



class PilotBase {

protected:
	PilotStatus m_pstatus;
	E_TEXTURE   m_texName;// 表示するテクスチャ.

	PilotBase()
		:m_texName(TEX_INVALID) {
		m_pstatus.pos = { 0.0f, 0.0f };
		m_pstatus.vel = { 0.0f, 0.0f };
		m_pstatus.moveSpeed = 0;
		m_pstatus.hp = 0;
		m_pstatus.alpha = 255;
		m_pstatus.isDead = false;
	}

public:
	virtual ~PilotBase() = default;
	virtual void Init(float px, float py, float pvx, float pvy, int pspeed, int php, bool isDead);//初期化
	virtual void Move() = 0;
	virtual void Fire() = 0;
	void Updata();//毎フレーム呼び出し
	virtual void Draw(E_TEXTURE texName);//描画タンク/パイロット本体
	Vector2 GetPos() { return m_pstatus.pos; }
	void IsLive();
	//virtual void DrawExplosion();//描画爆発
	bool IsDead() const { return  m_pstatus.isDead; }
	void Kill() { m_pstatus.isDead = true; }


};