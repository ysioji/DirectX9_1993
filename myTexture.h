#pragma once
// カーレースで使うテクスチャ.
enum E_TEXTURE
{
	TEX__BEGIN = 0,

	TEX_TSUCHI = TEX__BEGIN,// タイル  土
	TEX_UMI,	            // タイル  海
	TEX_KUSA,	            // タイル  草
	TEX_NUMA,	            // タイル  沼

	TEX_KI,                 // タイル  樹
	TEX_KAREKI,             // タイル  樹
	TEX_ISHI,               // タイル  石
	TEX_ISHI_3,             // タイル  石
	TEX_ISHI_2,             // タイル  石
	TEX_ISHI_1,             // タイル  石
	TEX_HAKO,               // タイル  箱

	TEX_PAAA,               //1p
	TEX_PBBB, 	            //2p
	TEX_PCCC,	            //3p
	TEX_PDDD,  	            //4p

	TEX_BULLET,             //弾

	TEX_AAAPHTO,
	TEX_AAANATION,

	TEX_TANKHPMARK,
	TEX_TANKRINGMARK,
	TEX_TANKSPEEDMARK,
	TEX_TANKPILOTMARK,

	TEX_BBBPHTO,
	TEX_BBBNATION,

	TEX_CCCPHTO,
	TEX_CCCNATION,

	TEX_DDDPHTO,
	TEX_DDDNATION,

	TEX_MOVE_UP,
	TEX_DIS_UP,
	TEX_HP_UP,

	TEX_TITLE,

	TEX_COUNT,
	TEX_INVALID = -1,
};

HRESULT LoadTexture(IDirect3DDevice9* pDev, LPCTSTR pFname, IDirect3DTexture9** ppTex);
