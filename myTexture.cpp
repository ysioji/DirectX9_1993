/*
	myTexture.cpp
*/
#include "stdafx.h"

// 画像を読み込む。カレントで失敗したら..から読み込む.
// pFname	ファイル名。const修飾子をつけ関数内で値を変更しないことを宣言する.
// ppTex	作成するテクスチャへのポインタを入れるためのポインタ.
HRESULT LoadTexture(IDirect3DDevice9* pDev, LPCTSTR pFname, IDirect3DTexture9** ppTex)
{
	HRESULT hr = D3DXCreateTextureFromFile(pDev, pFname, ppTex);
	if (FAILED(hr)) {
		// 失敗したので一つ上のフォルダからテクスチャを探してみる.
		int flen = (int)_tcslen(pFname) + 4;
		_tprintf(_T("%s: 親ディレクトリから探索...\n"), pFname);
		TCHAR* fn = new TCHAR[flen];
		_stprintf_s(fn, flen, _T("..\\%s"), pFname);
		hr = D3DXCreateTextureFromFile(pDev, fn, ppTex);
		delete[] fn;
		if (FAILED(hr)) {
			// 失敗したので例外を発生させて終わる.
			MessageBox(NULL, _T("テクスチャ読み込み失敗"), pFname, MB_OK);
			throw hr;		// エラーが発生したので例外を送る.
		}
	}
	return hr;
}
