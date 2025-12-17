#include "stdafx.h"
#include "myApp.h"
#include "bulletBase.h"

void BulletBase::Init (Vector2 cpos, int r, int max, int bsp, int bdam,int angle,bool alive, int facing,Field* f) {

	m_bullet.pos = cpos;
	m_bullet.radius = r;
	m_bullet.maxBulets = max;
	m_bullet.bulletSpeed = bsp;
	m_bullet.bulletDamage = bdam;
    m_bullet.bulletAngle = angle;
    m_bullet.bulletFacing = facing;
    m_bullet.isAlive = alive;

    m_pfield = f;
}

// 0  1  2  3 
void BulletBase::Draw(int playerNo) {

    IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
    assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

    ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
    assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

    IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_BULLET);// テクスチャの確認
    assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

    RECT rc = { playerNo * 32,  0, (playerNo + 1)*32, 32};//bulletを選択

   // D3DXVECTOR3 cnt((rc.right - rc.left) * 0.5f, rc.bottom, 0);//先端の中心
    D3DXVECTOR3 cnt((rc.right - rc.left) * 0.5f, (rc.bottom - rc.top) * 0.5f, 0);//中心
    D3DXMATRIX trans;
    D3DXMatrixTranslation(&trans, m_bullet.pos.x, m_bullet.pos.y, 0.0f);

    D3DXMATRIX rot;
    D3DXMatrixRotationZ(&rot, m_bullet.bulletAngle);

    D3DXMATRIX mat;
    mat = rot * trans;
    //mat = trans * rot;

    m_pSpr->SetTransform(&mat);

    m_pSpr->Draw(m_pTex, &rc, &cnt, NULL, 0xffffffff);

    D3DXMatrixIdentity(&mat);
    m_pSpr->SetTransform(&mat);
}

//AAAのfacingを取る
void BulletBase::Move() {

    switch (m_bullet.bulletFacing)
    {
        //top
    case DIR_UP:
      
        m_bullet.pos.y -= m_bullet.bulletSpeed;

        break;
    case DIR_DOWN:

        m_bullet.pos.y += m_bullet.bulletSpeed;

        break;
    case DIR_LEFT:

        m_bullet.pos.x -= m_bullet.bulletSpeed;

        break;
    case DIR_RIGHT:

        m_bullet.pos.x += m_bullet.bulletSpeed;

        break;
    
    default:
        break;
    }
    
}

void BulletBase::BulletArea() {

    const int top = (HEIGHT - ROWS * FIELD_SIZE) / 2 + FIELD_SIZE / 2;
    const int down = (HEIGHT + ROWS * FIELD_SIZE) / 2 - FIELD_SIZE / 2;
    const int left = (WIDTH - COLS * FIELD_SIZE) / 2 + FIELD_SIZE / 2;
    const int right = (WIDTH + COLS * FIELD_SIZE) / 2 - FIELD_SIZE / 2;
	const int offset = 16;
    if (m_bullet.pos.y < top- offset)   m_bullet.isAlive = false;
    if (m_bullet.pos.y > down+ offset)  m_bullet.isAlive = false;
    if (m_bullet.pos.x < left- offset)  m_bullet.isAlive = false;
    if (m_bullet.pos.x > right+ offset) m_bullet.isAlive = false;

}

void BulletBase::BulletJudge()
{
    const int ox = (WIDTH - COLS * FIELD_SIZE) / 2;
    const int oy = (HEIGHT - ROWS * FIELD_SIZE) / 2;

    const int mx = int((m_bullet.pos.x - ox) / FIELD_SIZE);
    const int my = int((m_bullet.pos.y - oy) / FIELD_SIZE);

    // 範囲外チェック
    if (mx < 0 || mx >= COLS || my < 0 || my >= ROWS)
        return;

    ENUM_FIELD_OBJECT2 tile = m_pfield->GetField2(mx, my);


	//当たった対象が樹
    if (tile == FIELD_OBJECT2_TREE)
    {
        int treeHP = m_pfield->GetField2HP(mx, my);
        treeHP--;

        m_pfield->SetField2HP(mx, my, treeHP);

         if (treeHP <= 0)
            m_pfield->SetField2(mx, my, FIELD_OBJECT2_NOTHING);

        m_bullet.isAlive = false;
    }
	//当たった対象が石
    if (tile == FIELD_OBJECT2_STONE)
    {
        int stoneHP = m_pfield->GetField2HP(mx, my);
        stoneHP--;

        m_pfield->SetField2HP(mx, my, stoneHP);

        if (stoneHP <= 0)
            m_pfield->SetField2(mx, my, FIELD_OBJECT2_NOTHING);

        m_bullet.isAlive = false;
    }

	// === 宝箱（BOX） ===
	if (tile == FIELD_OBJECT2_BOX)
	{
		int boxHP = m_pfield->GetField2HP(mx, my);
		boxHP--;

		// HP を更新;:
		m_pfield->SetField2HP(mx, my, boxHP);

		// HP0 → 消滅
		if (boxHP <= 0)
		{
			//m_pfield->SetField2(mx, my, FIELD_OBJECT2_NOTHING);
		}

		m_bullet.isAlive = false;
	}
}
