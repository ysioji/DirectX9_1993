#include <stdio.h>
#include <tchar.h>
#include "stdafx.h"
#include "myApp.h"
#include "myInput.h"

//AAA BBB CCC DDDの初期描画場所それぞれcenterPosX[4]とcenterPosY[4]に格納
 // int cpx[4] = { (WIDTH - ROWS * FIELD_SIZE) / 2 + FIELD_SIZE / 2,                        ///AAA.cx 左上
 //                (ROWS * FIELD_SIZE) + (WIDTH - ROWS * FIELD_SIZE) / 2 - FIELD_SIZE / 2 , ///BBB.cx 右下
 //                (WIDTH - ROWS * FIELD_SIZE) / 2 + FIELD_SIZE / 2,                        ///CCC.cx 左下
 //                (ROWS * FIELD_SIZE) + (WIDTH - ROWS * FIELD_SIZE) / 2 - FIELD_SIZE / 2,  ///DDD.cx 右上 
 // };
 // int cpy[4] = { (HEIGHT - COLS * FIELD_SIZE) / 2 + FIELD_SIZE / 2,                       ///AAA.cy 左上
 //                (COLS * FIELD_SIZE) + (HEIGHT - COLS * FIELD_SIZE) / 2 - FIELD_SIZE / 2, ///BBB.cy 右下
 //                (COLS * FIELD_SIZE) + (HEIGHT - COLS * FIELD_SIZE) / 2 - FIELD_SIZE / 2, ///CCC.cy 左下
 //                (HEIGHT - COLS * FIELD_SIZE) / 2 + FIELD_SIZE / 2,                       ///DDD.cy 右上
 // };
void TankBase::Init(float tx, float ty, float tvx, float tvy,int tspeed, int thp,bool isDead)             
 {

    m_tstatus.pos = { tx, ty };
    m_tstatus.vel = { tvx, tvy };
    m_tstatus.moveSpeed = tspeed;
    m_tstatus.hp = thp;
	m_tstatus.isDead = isDead;

}
void TankBase::Updata() {

    Move();
    m_tstatus.pos.x += m_tstatus.vel.x ;
    m_tstatus.pos.y += m_tstatus.vel.y ;

}
void TankBase::Draw(E_TEXTURE TEX_NAME) {

	if (m_tstatus.isDead) return;
    IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
    assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

    ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
    assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

    IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_NAME);// テクスチャの確認
    assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

    RECT rc= { 0,  0, 32, 32 };//tankを選択

    D3DXVECTOR3 cnt((rc.right - rc.left) * 0.5f, (rc.bottom - rc.top) * 0.5f, 0);//中心点指定
   
    D3DXMATRIX trans;
    D3DXMatrixTranslation(&trans, m_tstatus.pos.x, m_tstatus.pos.y, 0.0f);

    D3DXMATRIX rot;
    D3DXMatrixRotationZ(&rot, m_tstatus.angle);
   

    D3DXMATRIX mat;
    mat = rot*trans ;
    //mat = trans*rot ;
    m_pSpr->SetTransform(&mat);

    DWORD color = D3DCOLOR_ARGB(m_tstatus.alpha, 255, 255, 255);

    
    m_pSpr->Draw(m_pTex, &rc, &cnt, NULL, color);

  
    D3DXMatrixIdentity(&mat);
    m_pSpr->SetTransform(&mat);

}

void TankBase::IsLive() {


		if (m_tstatus.hp <= 0) {
			m_tstatus.isDead = true;   // ← これで死亡
		}
	
}
