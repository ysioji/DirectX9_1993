/*
	myBmpFont.h

	ビットマップフォント表示用.
*/
#pragma once

// ビットマップフォントの種類.
enum E_BMP_FONT
{
	BMP_FONT_16,		// 16ドット画像.
	BMP_FONT_64,		// 64ドット画像.

	BMP_FONT_COUNT,
};

// 文字列画像を表示するためのクラス.
// ビットマップフォント画像ごとにオブジェクトを生成すること.
class MyBmpFont
{
private:
	IDirect3DTexture9* m_pTex;	// 文字フォント用のテクスチャ.
	int m_size;					// １文字分のテクスチャサイズ.

public:
	MyBmpFont() : m_pTex(NULL), m_size(16) {}
	virtual ~MyBmpFont();

	void LoadFontTexture(IDirect3DDevice9*, const TCHAR*, int size);	// フォント用テクスチャの読み込み.
	int GetDrawWidth(char str[], int size, int stride);					// 描画したい文字列の横幅を得る.
	int DrawBmpFont(char str[], int xx, int yy, int size, int stride);	// 文字列を描画する.
};
