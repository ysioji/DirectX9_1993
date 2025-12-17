#include "stdafx.h"
#include "myApp.h"
void PilotBase::Init(float px, float py, float pvx, float pvy, int pspeed, int php, bool isDead)
{

	m_pstatus.pos = { px, py };
	m_pstatus.vel = { pvx, pvy };
	m_pstatus.moveSpeed = pspeed;
	m_pstatus.hp = php;
	m_pstatus.isDead = isDead;

}

void PilotBase::Updata() {

	Move();
	m_pstatus.pos.x += m_pstatus.vel.x;
	m_pstatus.pos.y += m_pstatus.vel.y;

}

void PilotBase::Draw(E_TEXTURE TEX_NAME) {

	if (m_pstatus.isDead) return;
	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
	assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
	assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

	IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_NAME);// テクスチャの確認
	assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

	RECT rc = { 32,  0, 64, 64 };//pilotを選択

	D3DXVECTOR3 cnt((rc.right - rc.left) * 0.5f, (rc.bottom - rc.top) * 0.5f, 0);//中心点指定

	D3DXMATRIX trans;
	D3DXMatrixTranslation(&trans, m_pstatus.pos.x, m_pstatus.pos.y, 0.0f);

	DWORD color = D3DCOLOR_ARGB(m_pstatus.alpha, 255, 255, 255);

	m_pSpr->Draw(m_pTex, &rc, &cnt, NULL, color);

}

void PilotBase::IsLive() {


	if (m_pstatus.hp <= 0) {
		m_pstatus.isDead = true;   // ← これで死亡
	}

}