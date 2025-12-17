/*
	myBmpFont.cpp

	ビットマップフォント表示用.
*/
#include "stdafx.h"
#include "myBmpFont.h"
#include "myTexture.h"
#include "myApp.h"


MyBmpFont::~MyBmpFont()
{
	if (m_pTex) {
		m_pTex->Release();
	}
}

// オブジェクトを生成し、フォントを読み込む.
void MyBmpFont::LoadFontTexture(IDirect3DDevice9* pDev, const TCHAR* pFname, int size)
{
	// スプライトの作成.
	LoadTexture(pDev, pFname, &m_pTex);
	m_size = size;
}

// 描画したい文字列の横幅を得る.
int MyBmpFont::GetDrawWidth(char str[], int size, int stride)
{
	int width = 0;

	if (str[0]) {
		width = size;
	}
	for (unsigned int i = 1; str[i]; i++) {
		width += stride;
	}
	return width;
}

// 文字列を描画する。英文字しか表示できないので、UNICODE非対応.
// 位置(xx, yy)に、文字列strを表示する。文字間はstrideで指定.
int MyBmpFont::DrawBmpFont(char str[], int xx, int yy, int size, int stride)
{
	ID3DXSprite* pSpr = GetAppInst()->GetDxSpr();

	// 拡縮用の行列設定.
	D3DXMATRIX matrix;
	D3DXMATRIX matScale;
	float fScale = (float)size / m_size;
	D3DXMatrixScaling(&matScale, fScale, fScale, 1.0f);

	for (unsigned int ii = 0; str[ii]; ii++) {
		const D3DXVECTOR3 pos((float)xx, (float)yy, 0);
		int cc = (int)str[ii];
		int uu = (cc % 16) * m_size;
		int vv = (cc / 16) * m_size;
		int ee = m_size;

		// 表示範囲を設定する.
		// { left, top, right, bottom }
		const RECT rc = {uu, vv, uu + ee, vv + ee};

		// 移動用の行列設定.
		D3DXMATRIX matTrans;
		D3DXMatrixTranslation(&matTrans, (float)xx, (float)yy, 0);

		// 行列の合成と描画前設定.
		matrix = matScale * matTrans;
		pSpr->SetTransform(&matrix);

		pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xFFFFFFFF);

		xx += stride;
	}

	// スプライトの描画設定を初期化する.
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	pSpr->SetTransform(&identity);

	return xx;
}
