#pragma once
#include "stdafx.h"
#include "map"
#include <string>

struct CUSTOMVERTEX {
	float x, y, z; // 頂点座標 
	float rhw;     // 除算数
	DWORD dwColor; // 頂点の色
	float u, v;    //テクスチャ座標 
};

struct GLYPHINFO {
	IDirect3DTexture9* texture;     //文字1文字ぶんのテクスチャ
	//IDirect3DVertexBuffer9* vertex; //その文字の四角ポリゴンの頂点バッファ
	int width;                      //テクスチャのサイズ（表示用）
	int height;
	int advanceX;                   //この文字の「横方向の移動幅」
};


class CMyJanFont {
private:
	static CMyJanFont sm_mjfData;// このクラス自身のオブジェクト（実体）の宣言.
	CMyJanFont();
	IDirect3D9* m_pD3D;			 // Direct3Dインスタンスオブジェクト.
	IDirect3DDevice9* m_pDev;	 // Direct3Dデバイスオブジェクト.

	IDirect3DVertexBuffer9* pVertex;
	LPDIRECT3DTEXTURE9 m_pTex;

	int m_fontsize;              //フォントの大きさ
	LOGFONT m_lf;                //フォントの高さ、太さ、種類、文字セット 
	HFONT m_hFont;               //フォント受け取る用
	HFONT m_oldFont;             //
	HDC m_hdc;                   //デバイスにフォントを持たせる
	
	std::map<TCHAR, GLYPHINFO> m_glyphMap;

public:
	static CMyJanFont* GetInstance() { return &sm_mjfData; }
	
	//~CMyJanFont();
	//フォント領域と同じ縦横の比率を持つ板ポリゴンを新規に作成
	HRESULT D3DXCreateTexure(
		LPDIRECT3DDEVICE9	pDevice,  //pDeviceはIDirect3DDevice9オブジェクトへのポインタを渡す
		UINT	Width,                //WidthとHrightは作成するテクスチャのサイズを指定
		UINT	Height,				  //WidthとHrightは作成するテクスチャのサイズを指定
		UINT	Levels,               //Levelsは「ミップマップレベル」を設定
		DWORD	Usage,                //Usageはテクスチャの性質や確保するメモリの場所を指定するオプション
		D3DFORMAT	Format,           //Formatはテクスチャのピクセルフォーマットを指定
		D3DPOOL	Pool,                 //Poolはテクスチャを格納するメモリの位置と管理に関するフラグを設定
		IDirect3DTexture9** ppTexture //ppTextureに作成されたテクスチャオブジェクトが返され
	);

	// テクスチャのサーフェイスをIDirect3DTexture9::LockRect関数でロック
// テクスチャ内の特定範囲をロック
	HRESULT LockRect(
		UINT	Level,                //Levelはミップマップのレベルの事で、今は1つしかありませんから、ここには0が入ります
		D3DLOCKED_RECT* pLockedRect,  //D3DLOCKED構造体へのポインタを渡します
		CONST RECT* pRect,            //ロックする矩形領域を指定します。今はテクスチャ全部を指定したいのでNULLとします。
		DWORD	Flags                 //ロック時のオプションフラグです
	);

	//テクスチャのサーフェイスにフォントの点を直接書き込む
	//GetGlyphOutline関数で得たフォントのピクセル情報を書き込む
	DWORD GetGlyphOutline(
		HDC	hdc,                      //hdcはデバイスコンテキストハンドルです
		UINT	uChar,				  //文字に割り当てられているコード番号です
		UINT	uFormat,			  //関数が返すデータフォーマットを指定します
		// GGO_GRAY2_BITMAP　　5段階のアンチエイリアス付きビットマップ                              
		// GGO_GRAY4_BITMAP　　17段階のアンチエイリアス付きビットマップ
		// GGO_GRAY7_BITMAP　　65階のアンチエイリアス付きビットマップ
		LPGLYPHMETRICS	lpgm,		  //GLYPHMETRICS構造体へのポインタを渡す
		DWORD	cbBuffer,			  //次のlpvBufferのサイズを指定します
		LPVOID	lpvBuffer,			  //ビットマップ情報を受け取るバッファへのポインタを渡す
		CONST MAT2* lpmat2			  //文字の変換行列で、MAT2という行列構造体へのポインタを渡す
	);
	
	int  Init(IDirect3DDevice9* pDev, IDirect3D9* pD3D);
	void Draw(int x, int y,const TCHAR* text, float scale,int span,int lineHeight,int maxWidth, DWORD color);
	void Draw(int x, int y,const TCHAR* text, float scale,int span,int lineHeight,int maxWidth, DWORD color , bool alignRight);
	void PreloadString(const TCHAR* text);
	void LoadPreloadTextCSV(const std::string&filename);
	// ANSI → WCHAR（TCHAR*）
	std::wstring  ANSIToWCHAR(const std::string& src);

};


// アプリケーションオブジェクトにアクセスするためのラッパ関数をインラインで.
inline CMyJanFont* GetMJFInst() { return CMyJanFont::GetInstance(); }
