#include "myJanFont.h"
#include <iostream>
#pragma comment(lib, "Gdi32.lib")
CMyJanFont CMyJanFont::sm_mjfData;

/// 頂点関係 ///
#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 ) // 座標変換済み頂点

DWORD CMyJanFont::GetGlyphOutline(
	HDC hdc,
	UINT uChar,
	UINT uFormat,
	LPGLYPHMETRICS lpgm,
	DWORD cbBuffer,
	LPVOID lpvBuffer,
	const MAT2* lpmat2)
{
	// WinAPI をそのまま使うだけならこう書けばOK
	return ::GetGlyphOutline(hdc, uChar, uFormat, lpgm, cbBuffer, lpvBuffer, lpmat2);
}

CMyJanFont::CMyJanFont()
	: m_fontsize(0)
	, m_pDev(nullptr)
	, m_pD3D(nullptr)
	, m_hFont(nullptr)
	, m_hdc(nullptr)
	, m_oldFont(nullptr)
	, m_pTex(nullptr)
	, pVertex(nullptr)
{
	ZeroMemory(&m_lf, sizeof(m_lf)); // LOGFONT を 0 で初期化
}
//CMyJanFont::~CMyJanFont() {
//	
//	m_pDev->Release();
//	m_pD3D->Release();
//  pVertex->Release();
//}

//初期化
//引数.デバイス インターフェース
int CMyJanFont::Init(IDirect3DDevice9* pDev, IDirect3D9* pD3D) {
	m_pDev = pDev;
	m_pD3D = pD3D;

	m_fontsize = 15;//フォントの高さ

	// フォントの高さ、太さ、種類、文字セット 
	// SHIFTJIS_CHARSET：日本語（Shift-JIS）に対応
	//FIXED_PITCH | FF_MODERN：等幅寄りの指定です
	m_lf = { m_fontsize, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
	CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, _T("ＭＳ 明朝") };


	//フォントを作成
	m_hFont = CreateFontIndirect(&m_lf);
	if (!m_hFont)
	{
		m_pDev->Release();
		m_pD3D->Release();
		return 0;
	}

	// デバイスコンテキスト取得
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	m_hdc = GetDC(NULL);//デスクトップ全体のDCを取得
	m_oldFont = (HFONT)SelectObject(m_hdc, m_hFont);//上書き防止のためoldFontでリセット

	return 1;
}
void CMyJanFont::PreloadString(const TCHAR* text) {
	TEXTMETRIC TM;//フォントに関する基本情報を保持する構造体。
	GetTextMetrics(m_hdc, &TM); //現在 HDC に設定されているフォントの情報をTMに入れる
	const MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} }; //フォントの変形指定行列

	//各文字ループ開始
	//テキストを1文字ずつ処理
	for (int i = 0; text[i] != _T('\0'); ++i) {
		TCHAR ch = text[i];
		if (m_glyphMap.count(ch)) continue; //すでに作った文字ならスキップ

		//グリフ情報取得
		GLYPHMETRICS GM; //	描画領域の幅,高さ,原点からのオフセットXY,次の文字に進むX距離
		DWORD size = GetGlyphOutline(m_hdc, ch, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
		if (size == GDI_ERROR) continue;

		BYTE* ptr = new BYTE[size];
		GetGlyphOutline(m_hdc, ch, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

		// テクスチャサイズ計算
		 //GGO_GRAY4_BITMAP では1行のサイズが4バイト単位に揃えられる
		 //そのため、描画領域の横幅を4でパディング
		int bmpW = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
		int bmpH = GM.gmBlackBoxY; //グリフの高さ
		int ofsX = GM.gmptGlyphOrigin.x;//原点位置 → 描画左端から文字までのオフセット
		int ofsY = TM.tmAscent - GM.gmptGlyphOrigin.y;
		int level = 17;
		int texW = max(bmpW + ofsX, GM.gmCellIncX);//テクスチャ全体の幅（グリフサイズ + 左オフセット or セル幅）
		int texH = TM.tmHeight;
		
		// テクスチャ作成と転送
		IDirect3DTexture9* tex = nullptr;
		if (FAILED(m_pDev->CreateTexture(texW, texH, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, NULL))) {
			delete[] ptr;
			continue;
		}

		D3DLOCKED_RECT LockedRect;
		if (FAILED(tex->LockRect(0, &LockedRect, NULL, D3DLOCK_DISCARD))) {
			tex->Release();
			delete[] ptr;
			continue;
		}
		//メモリクリア
		FillMemory(LockedRect.pBits, LockedRect.Pitch * texH, 0);

		//各ピクセルに alpha を設定してFFFFFF色 + 透明度で描画
		for (int y = 0; y < bmpH; ++y) {
			for (int x = 0; x < GM.gmBlackBoxX; ++x) {
				int idx = x + bmpW * y;
				DWORD alpha = (255 * ptr[idx]) / (level - 1);
				DWORD color = 0x00FFFFFF | (alpha << 24);
				memcpy((BYTE*)LockedRect.pBits + (ofsY + y) * LockedRect.Pitch + (ofsX + x) * 4, &color, sizeof(DWORD));
			}
		}
		tex->UnlockRect(0);
		delete[] ptr;
		texW *= 2;
		texH *= 2;
		m_glyphMap[ch] = { tex,texW, texH, GM.gmCellIncX };

	}
	SelectObject(m_hdc, m_oldFont);
	DeleteObject(m_hFont);
	ReleaseDC(NULL, m_hdc);
}
/**
 * @brief 指定された位置に文字列を描画する関数（倍率・間隔付き）
 *
 * @param x         表示開始位置のX座標（左上基準）
 * @param y         表示開始位置のY座標（左上基準）
 * @param text      表示する文字列（TCHAR形式）
 * @param scale     描画倍率（1.0fで等倍、2.0fで2倍サイズ）
 * @param span      文字の左右間隔（ピクセル単位）
 * @param lineHeight 行間  （改行時の上下間隔）
 * @param maxWidth   行変え（値を超えると改行）
 * @param color      色指定（Alpha, Red, Green, Blue）
 */
void CMyJanFont::Draw(int x, int y, const TCHAR* text,
	                  float scale,int span, int lineHeight, int maxWidth, DWORD color)
{
	int posX = x;//現在描画する文字の左上座標
	int posY = y;//現在描画する文字の左上座標

	//テキストの1文字ずつ処理
	for (int i = 0; text[i] != _T('\0'); ++i) {
		TCHAR ch = text[i];
		//改行処理
		if (ch == _T('\n')) {
			posX = x; //posX をリセット
			posY += lineHeight;//行送り
			continue;
		}

		if (!m_glyphMap.count(ch)) continue;
		
		//グリフ情報取得
		GLYPHINFO& g = m_glyphMap[ch];

		//xが一定値になると改行
		//if (maxWidth >NULL && (posX + (g.width * scale) > x + maxWidth)) {
		if (maxWidth >NULL && (posX+ (m_fontsize*scale)/2 > maxWidth)) {
			posX = x;//先頭に戻る
			posY += lineHeight;//行変え
		}

		//テクスチャ設置
		m_pDev->SetTexture(0, g.texture);
		//文字の画像をセット
		m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//アルファブレンド有効
		m_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//テクスチャと頂点色を乗算
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE);	// ライト有効.
		m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);	// Zバッファリング有効.

		//頂点バッファ作成
		CUSTOMVERTEX vertices[4];
		if (color == NULL) {
			color = 0xFFFFFFFF;//白0%透明
		}
		//DWORD fontColor = 0xFFFFFFFF;
		//四角形（2D）で1枚の板ポリ（4頂点）
		vertices[0] = { posX + (float)g.width * scale, posY + 0.0f, 0, 1.0f, color, 1.0f, 0.0f};// 右上.
		vertices[1] = { posX + (float)g.width * scale, posY + (float)g.height * scale, 0, 1.0f, color, 1.0f, 1.0f };// 右下.
		vertices[2] = { posX + 0.0f, posY + 0.0f, 0, 1.0f, color, 0.0f, 0.0f };// 左上.
		vertices[3] = { posX + 0.0f, posY + (float)g.height * scale, 0, 1.0f, color, 0.0f, 1.0f };// 左下.

		//頂点バッファを描画設定して描く
		m_pDev->SetFVF(FVF_CUSTOM);

		m_pDev->DrawPrimitiveUP(
			D3DPT_TRIANGLESTRIP, // 四角形を2三角形として描画
			2,                   // 三角形数
			vertices,            // 頂点配列
			sizeof(CUSTOMVERTEX) // 1頂点のサイズ(byte)
		);
		//次の文字の位置をずらす
		int offset = g.advanceX*scale + span;
		posX += offset;
	}
}


/**
 * @brief 指定された位置に文字列を描画する関数（倍率・間隔付き）
 *
 * @param x         表示開始位置のX座標（左上基準）
 * @param y         表示開始位置のY座標（左上基準）
 * @param text      表示する文字列（TCHAR形式）
 * @param scale     描画倍率（1.0fで等倍、2.0fで2倍サイズ）
 * @param span      文字の左右間隔（ピクセル単位）
 * @param lineHeight 行間  （改行時の上下間隔）
 * @param maxWidth   行変え（値を超えると改行）
 * @param color      色指定（Alpha, Red, Green, Blue）
 * @param alignRight false左から右draw　true右から左drawか
 */
void CMyJanFont::Draw(int x, int y, const TCHAR* text,
	float scale, int span, int lineHeight, int maxWidth, DWORD color, bool alignRight)
{
	int posX = x;//現在描画する文字の左上座標
	int posY = y;//現在描画する文字の左上座標

	//テキストの1文字ずつ処理
	for (int i = 0; text[i] != _T('\0'); ++i) {
		TCHAR ch = text[i];
		//改行処理
		if (ch == _T('\n')) {
			posX = x; //posX をリセット
			posY += lineHeight;//行送り
			continue;
		}

		if (!m_glyphMap.count(ch)) continue;

		//グリフ情報取得
		GLYPHINFO& g = m_glyphMap[ch];

		//xが一定値になると改行
		//if (maxWidth >NULL && (posX + (g.width * scale) > x + maxWidth)) {
		if (maxWidth > NULL && (posX + (m_fontsize * scale) / 2 < maxWidth)) {
			posX = x;//先頭に戻る
			posY += lineHeight;//行変え
		}

		//テクスチャ設置
		m_pDev->SetTexture(0, g.texture);
		//文字の画像をセット
		m_pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		//アルファブレンド有効
		m_pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		m_pDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		m_pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		//テクスチャと頂点色を乗算
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		m_pDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		m_pDev->SetRenderState(D3DRS_LIGHTING, TRUE);	// ライト有効.
		m_pDev->SetRenderState(D3DRS_ZENABLE, TRUE);	// Zバッファリング有効.

		//頂点バッファ作成
		CUSTOMVERTEX vertices[4];
		if (color == NULL) {
			color = 0xFFFFFFFF;//白0%透明
		}
		//DWORD fontColor = 0xFFFFFFFF;
		//四角形（2D）で1枚の板ポリ（4頂点）
		vertices[0] = { posX + (float)g.width * scale, posY + 0.0f, 0, 1.0f, color, 1.0f, 0.0f };// 右上.
		vertices[1] = { posX + (float)g.width * scale, posY + (float)g.height * scale, 0, 1.0f, color, 1.0f, 1.0f };// 右下.
		vertices[2] = { posX + 0.0f, posY + 0.0f, 0, 1.0f, color, 0.0f, 0.0f };// 左上.
		vertices[3] = { posX + 0.0f, posY + (float)g.height * scale, 0, 1.0f, color, 0.0f, 1.0f };// 左下.

		//頂点バッファを描画設定して描く
		m_pDev->SetFVF(FVF_CUSTOM);

		m_pDev->DrawPrimitiveUP(
			D3DPT_TRIANGLESTRIP, // 四角形を2三角形として描画
			2,                   // 三角形数
			vertices,            // 頂点配列
			sizeof(CUSTOMVERTEX) // 1頂点のサイズ(byte)
		);
		//次の文字の位置をずらす
		int offset = g.advanceX * scale + span;
		if(alignRight)posX -= offset;
		else if(!alignRight)posX += offset;
	}
}


std::wstring  CMyJanFont::ANSIToWCHAR(const std::string& src){

   int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
   std::wstring result(len, 0);
   MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, &result[0], len);
   result.pop_back();

   return result;
	
}

void CMyJanFont::LoadPreloadTextCSV(const std::string& filename) {


}