#include "stdafx.h"
#include "myApp.h"
#include "PlayerBBB.h"

//BBB(平均)
//TankHp   3   + (1)  =4
//TankMove 10  + (1)  =10 X 1.2 = 12
//PoHP     3   + (1)  =4 
//PoMove   5   + (1)  = 5 X 1.2 = 6



//初期化
void PlayerBBB::Init(Field* f) {
	m_pfield = f;
	//初期リスポーン場所決める
	float fieldLeft = (WIDTH - COLS * FIELD_SIZE) / 2;
	float fieldTop = (HEIGHT - ROWS * FIELD_SIZE) / 2;

	// ★右上マスの中心に配置（AAA は左上、BBB は右上）
	m_tstatus.pos.x = fieldLeft + (COLS - 1) * FIELD_SIZE + FIELD_SIZE / 2;
	m_tstatus.pos.y = fieldTop + FIELD_SIZE / 2;
	m_tstatus.vel = { 0.0f, 0.0f };
	m_tstatus.moveSpeed = 1.0f;
	m_tstatus.hp = 15;
	m_facing = DIR_DOWN;
	m_maxBullets = 3;

	m_bullets.reserve(3);
}
void PlayerBBB::Move() {

	if (IsDead()) return;
	MyInput* pInput = GetInputInst(); //D3DInputインスタンスを取得


	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i].GetAlive())
		{
			m_bullets[i].Move();
			m_bullets[i].BulletJudge();
			m_bullets[i].BulletArea();
		}
	}

	//Player移動処理
	Vector2 v{ 0,0 };
	const int top = (HEIGHT - ROWS * FIELD_SIZE) / 2 + FIELD_SIZE / 2;
	const int down = (HEIGHT + ROWS * FIELD_SIZE) / 2 - FIELD_SIZE / 2;
	const int left = (WIDTH - COLS * FIELD_SIZE) / 2 + FIELD_SIZE / 2;
	const int right = (WIDTH + COLS * FIELD_SIZE) / 2 - FIELD_SIZE / 2;
	//ジョイスティック
	if (m_tstatus.hp > 0) {
		int ix = 0, iy = 0;
		// ↑
		//if (pInput->IsPushKey(DIK_UP) && m_tstatus.pos.y >= top)
			if (pInput->IsPushBtn(JOY_CON_0, JOY_BTN_BIT_UP)&& m_tstatus.pos.y >= top)
		{

			iy -= 1;
			m_tstatus.angle = D3DXToRadian(180);//ラジアン必須
			m_facing = DIR_UP;
		}
		// ↓
		//if (pInput->IsPushKey(DIK_DOWN) && m_tstatus.pos.y <= down)
			if (pInput->IsPushBtn(JOY_CON_0, JOY_BTN_BIT_DN) && m_tstatus.pos.y <= down)
		{
			iy += 1;
			m_tstatus.angle = D3DXToRadian(0);//ラジアン必須
			m_facing = DIR_DOWN;
		}
		// ←

		//if (pInput->IsPushKey(DIK_LEFT) && m_tstatus.pos.x >= left)
			if (pInput->IsPushBtn(JOY_CON_0, JOY_BTN_BIT_LT) && m_tstatus.pos.x >= left)
		{
			ix -= 1;
			m_tstatus.angle = D3DXToRadian(90);//ラジアン必須
			m_facing = DIR_LEFT;
		}
		// →
		//if (pInput->IsPushKey(DIK_RIGHT) && m_tstatus.pos.x <= right)
			if (pInput->IsPushBtn(JOY_CON_0, JOY_BTN_BIT_RT)&& m_tstatus.pos.x <= right) 
		{
			ix += 1;
			m_tstatus.angle = D3DXToRadian(270);//ラジアン必須
			m_facing = DIR_RIGHT;
		}
		//斜め防止
		//両方速度がある時一歩を0にする
		if (ix != 0 && iy != 0) {

			iy = 0;
			ix = 0;
		}
		//true  動ける false 動かない
		if (CheckLayOut1() && CheckLayOut2()) {
			v.x = ix * m_tstatus.moveSpeed;
			v.y = iy * m_tstatus.moveSpeed;
		}
	}
	m_tstatus.vel = v;

	//if (pInput->IsPushKeyOne(DIK_SPACE))
	if (pInput->IsPushBtnOne(JOY_CON_0, JOY_BTN_BIT_A | 
			                             JOY_BTN_BIT_B    | 
			                             JOY_BTN_BIT_X    | 
			                             JOY_BTN_BIT_Y))

	{
		Fire();
	}
}

//spaceを外に
void PlayerBBB::Fire()
{
	// 1) 数える：生きている弾の数
	int activeCount = 0;
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i].GetAlive())
			activeCount++;
	}

	// 最大弾数チェック
	if (activeCount >= m_maxBullets) {

		return;

	}

	// 2) 既存の弾を再利用
	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (!m_bullets[i].GetAlive())
		{
			m_bullets[i] = CreateBullet();

			return;
		}
	}

	// 3) 新規作成
	m_bullets.push_back(CreateBullet());
}

void PlayerBBB::DrawBullets() {

	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i].GetAlive())
		{
			m_bullets[i].Draw(1); // playerNo = 0
		}
	}


}
bool PlayerBBB::CheckLayOut1() {

	//上下左右
	//先ずは自分(タンク)の現在MASU目を確認
	const int ox = (WIDTH - COLS * FIELD_SIZE) / 2;//field左上ｘ
	const int oy = (HEIGHT - ROWS * FIELD_SIZE) / 2;//field左上ｙ

	// int mx = int((m_tstatus.pos.x - ox) / FIELD_SIZE);
	// int my = int((m_tstatus.pos.y - oy) / FIELD_SIZE);

	float offsetHalf = FIELD_SIZE / 2.0f;

	//player.中心座標
	float cx = m_tstatus.pos.x;
	float cy = m_tstatus.pos.y;
	//ENUM_FIELD_OBJECT1 obj = m_pfield->GetField1(mx, my);
	// layout1には 
	// タンクは　1,3,4　通過できる　
	// パイロットは　1,2,3　通過できる

	switch (m_facing)
	{
		//yの一個分上をチェック
		//***タンクだけ
		//河ではない場合通過可能
	case DIR_UP:    cy -= offsetHalf; break;
	case DIR_DOWN:  cy += offsetHalf; break;
	case DIR_LEFT:  cx -= offsetHalf; break;
	case DIR_RIGHT: cx += offsetHalf; break;

	default:
		assert("フィールド配列に予期されてない値です");
		return false;
		break;
	}
	//ずっれたマス
	int tx = int((cx - ox) / FIELD_SIZE);
	int ty = int((cy - oy) / FIELD_SIZE);

	ENUM_FIELD_OBJECT1 tile = m_pfield->GetField1(tx, ty);

	if (tile == FIELD_OBJECT1_GRASS) {

		m_tstatus.alpha = 30;
	}
	else
	{

		m_tstatus.alpha = 255;

	}
	//海ならfalse
	//ほか true
	return (tile != FIELD_OBJECT1_LAKE);
}

bool PlayerBBB::CheckLayOut2() {


	//上下左右
	//先ずは自分(タンク)の現在MASU目を確認
	const int ox = (WIDTH - COLS * FIELD_SIZE) / 2;//field左上ｘ
	const int oy = (HEIGHT - ROWS * FIELD_SIZE) / 2;//field左上ｙ

	// int mx = int((m_tstatus.pos.x - ox) / FIELD_SIZE);
	// int my = int((m_tstatus.pos.y - oy) / FIELD_SIZE);

	float offsetHalf = FIELD_SIZE / 2.0f;

	//player.中心座標
	float cx = m_tstatus.pos.x;
	float cy = m_tstatus.pos.y;
	//ENUM_FIELD_OBJECT1 obj = m_pfield->GetField1(mx, my);
	// layout1には 
	// タンクは　1,3,4　通過できる　
	// パイロットは　1,2,3　通過できる

	switch (m_facing)
	{
		//yの一個分上をチェック
		//***タンクだけ
		//河ではない場合通過可能
	case DIR_UP:    cy -= offsetHalf; break;
	case DIR_DOWN:  cy += offsetHalf; break;
	case DIR_LEFT:  cx -= offsetHalf; break;
	case DIR_RIGHT: cx += offsetHalf; break;

	default:
		assert("フィールド配列に予期されてない値です");
		return false;
		break;
	}
	//ずっれたマス
	int tx = int((cx - ox) / FIELD_SIZE);
	int ty = int((cy - oy) / FIELD_SIZE);

	//ここが違う
	ENUM_FIELD_OBJECT2 tile = m_pfield->GetField2(tx, ty);


	if (tile == FIELD_OBJECT2_NOTHING) {

		return true;

	}
	else {

		return false;

	}

}


BulletBase PlayerBBB::CreateBullet() {

	BulletBase bullet;
	Vector2 startPos = GetBulletStartPos();

	bullet.Init(
		startPos, // 位置
		8,                   // 半径
		3,                   // 最大弾数（以后可用 m_maxBullets）
		10,                  // 速度
		1,                   // ダメージ
		0,                   // 角度（以后可用 m_facing）
		true,                // alive
		m_facing,
		m_pfield             // field
	);

	return bullet;


}

Vector2 PlayerBBB::GetBulletStartPos()
{
	float cx = m_tstatus.pos.x;  // 中心X
	float cy = m_tstatus.pos.y;  // 中心Y

	switch (m_facing)
	{
	case DIR_UP:
		return { cx, cy - 16 };  // 上
	case DIR_DOWN:
		return { cx, cy + 16 };  // 下
	case DIR_LEFT:
		return { cx - 16 , cy };  // 左
	case DIR_RIGHT:
		return { cx + 16 , cy };  // 右
	}
	return { cx, cy };
}

void PlayerBBB::DrawPoto() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_BBBPHTO);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

	RECT rc = { 0,  0, 96, 96 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	D3DXMATRIX trans;
	const float startX = 320;
	const float startY = 0;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);
	m_pSpr->SetTransform(&trans);
	m_pSpr->Draw(m_pTex, &rc, &cnt, NULL, 0xffffffff);

}


void PlayerBBB::DrawNation() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_BBBNATION);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");


	RECT rc = { 0,  0, 224, 96 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	const float startX = 416.0f;
	const float startY = 0.0f;

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);

	m_pSpr->SetTransform(&trans);

	const int alpha = 32;
	DWORD color = D3DCOLOR_ARGB(alpha, 255, 255, 255);
	m_pSpr->Draw(m_pTex, &rc, NULL, NULL, color);
}

void PlayerBBB::DrawHpBar() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_BBBPHTO);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

	RECT rc = { 0,  0, 6, 6 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定
	const float offsetX = 12.0f;
	const float startX = 416.0f + 24.0f + offsetX;
	const float startY = 12.0f;
	for (int i = 0; i < m_tstatus.hp; i++)
	{
		D3DXMATRIX trans;
		D3DXMatrixTranslation(&trans, startX + i * 12, startY, 0.0f);
		m_pSpr->SetTransform(&trans);

		m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xFFFFFFFF);
	}
}


void PlayerBBB::DrawTankMark() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_TANKHPMARK);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");


	RECT rc = { 0,  0, 24, 24 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	const float offsetX = 4.0f;
	const float startX = 416.0f + offsetX;
	const float startY = 0.0f;

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);

	m_pSpr->SetTransform(&trans);

	m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xffffffff);
}

void PlayerBBB::DrawTankRing() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_TANKRINGMARK);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");


	RECT rc = { 0,  0, 24, 24 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	const float offsetX = 4.0f;
	const float startX = 416.0f + offsetX;
	const float startY = 24.0f;

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);

	m_pSpr->SetTransform(&trans);

	m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xffffffff);
}

void PlayerBBB::DrawTankSpeed() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_TANKSPEEDMARK);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

	RECT rc = { 0,  0, 24, 24 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	const float offsetX = 4.0f;
	const float startX = 416.0f + offsetX;
	const float startY = 48.0f;

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);

	m_pSpr->SetTransform(&trans);

	m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xffffffff);

}

void PlayerBBB::DrawPilotMark() {

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_TANKPILOTMARK);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

	RECT rc = { 0,  0, 24, 24 };

	D3DXVECTOR3 cnt(0, 0, 0);//左上指定

	const float offsetX = 4.0f;
	const float startX = 416.0f + offsetX;
	const float startY = 72.0f;

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, startX, startY, 0.0f);

	m_pSpr->SetTransform(&trans);

	m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xffffffff);
}