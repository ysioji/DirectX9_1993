/*
	myApp.h
	

*/
#pragma once

#include "myBmpFont.h"
#include "myTimer.h"
#include "field.h"
#include "tankBase.h"
#include "PlayerAAA.h"
#include "PlayerBBB.h"
#include "PlayerCCC.h"
#include "PlayerDDD.h"
//#include "BulletAAA.h"
// クライアント領域の幅と高さを定数で設定する.
//16:9
#define WIDTH  (1280) // ウィンドウの全体の幅: 
#define HEIGHT (1024) // ウィンドウの全体の高さ


// ゲームのシーン.
enum E_GAME_SCENE
{
	GAME_SCENE_TITLE,	// タイトル画面.
	GAME_SCENE_SELECT,	// タイトル画面.
	GAME_SCENE_STORY,	// ゲーム中(協力)
	GAME_SCENE_BUTTLE,	// ゲーム中(対戦)
	GAME_SCENE_OVER,	// ゲームオーバー 
	GAME_SCENE_RESULT,

	GAME_SCENE_COUNT,

	GAME_SCENE_INVALID = -1,
	GAME_SCENE_BEGIN,
};
#define IS_VALID_GAME_SCENE(st) (GAME_SCENE_BEGIN <= (st) && (st) <= GAME_SCENE_COUNT)


// プログラムに必要なデータをMyAppクラスにまとめる.
class MyApp
{
private:
	static MyApp sm_appData;// このクラス自身のオブジェクト（実体）の宣言.
	// コンストラクタをprivate領域に配置して勝手なオブジェクト生成が出来ないようにする.
	MyApp();

public:
	static MyApp* GetInstance() { return &sm_appData; }
	ID3DXSprite* GetDxSpr()     {assert(m_pSpr != NULL);return m_pSpr;}
	IDirect3DTexture9* GetDxTex(int index) {assert(0<=index &&index<TEX_COUNT); return m_pTex[index];}
	IDirect3DDevice9* GetDxDev(){assert(m_pDev != NULL);return m_pDev;}



	BOOL InitApp();			// アプリの初期化.
	HRESULT InitDirectX();	// DirectXの初期化.
	BOOL LoadResource();	// リソース読み込み.


	

	void ChangeScene(E_GAME_SCENE nextScene); // シーン変更処理

	void InitGame();		// ゲーム状態を初期化.
	HRESULT InitFont();		// Font 初期化
	void TitleInit();		// タイトル画面初期化
	void SelectInit();		// セレクトシーンの初期化
	
	void Release();			// リソース解放.

	BOOL UpdateMain();		// 更新メイン.

	void UpdateScene();      // 全てのシーンの更新処理
	void UpdateTitle();     // タイトル画面用の更新処理
	void UpdateSelect();   // セレクト画面用の更新処理
	void UpdateStory();       // 協力用の更新処理
	void UpdateResult();      // リザルト画面用の更新処理
	
	void DrawScene();		 // 全てのシーンの描画処理
	void DrawTitle();		// タイトル画面描画
	void DrawSelect();
	void DrawResult();      // リザルト画面描画
	void DrawStory();		// 協力用画面描画
	void DrawMain();		// 描画メイン.

	void ScreenFlip();
	void MainLoop();		// メッセージループ.
	
	//追加
	
	LPD3DXFONT  GetFont()  { return m_pFont; } //通常フォント
	LPD3DXFONT  GetFontS() { return m_pFontS; }//小フォント
	//自作
	//3
	template <class S, class T>
	void CheckBulletHitPlayer(S& shooter, T& target);
private:
	HINSTANCE m_hInstance;				// インスタンスハンドル.
	HWND m_hWnd;						// 表示ウィンドウ.

	IDirect3D9* m_pD3D;					// Direct3Dインスタンスオブジェクト.
	IDirect3DDevice9* m_pDev;			// Direct3Dデバイスオブジェクト.
	ID3DXSprite* m_pSpr;				// スプライト.
	IDirect3DTexture9* m_pTex[TEX_COUNT];// テクスチャ.
	ID3DXLine* m_pLine;                  //Direct3Draw lineオブジェクト.

	MyBmpFont m_bmpFont[BMP_FONT_COUNT];// ビットマップフォント.
	
	LPD3DXFONT  m_pFont;				// フォント.
	LPD3DXFONT  m_pFontS;               // 小フォント.

	IDirect3DTexture9* GetTex(int index) {return m_pTex[index];}
	
	E_GAME_SCENE m_gameScene;			// ゲームのシーン.

	MyTimer m_syncTimer;				// 描画更新タイミングの制御用.

	Field m_field;//フィールド
	//1
	std::vector<TankBase*> m_players;
	PlayerAAA m_pAAA; 
	PlayerBBB m_pBBB; 
	PlayerCCC m_pCCC; 
	PlayerDDD m_pDDD; 
	
	void CheckPlayerItemHit();
	enum class GameMode {
		Coop,   // 協力
		Versus  // 対戦
	};

	enum class SelectState {
		ModeSelect,        // 協力 or 対戦を選ぶ
		PlayerJoin,        // 参加待機（人数受付）
		//ReadyConfirm,    // 準備完了演出
	};
	const float JOIN_TIME_LIMIT = 4.0f; // Debug
//	const float JOIN_TIME_LIMIT = 15.0f; // Debugなら3.0f
	float joinTimer = 0.0f;
	int joinedPlayers = 1;     // test
	//	int joinedPlayers = 2; // 2Pは必須

	// アニメーション 
	float m_logoY;						   // logo y座標
	float m_logoSpeed;					   // アニメーション速さ
	float m_logotime;                      // タイマー
#if defined(_DEBUG)
	// デバッグ情報の表示.
	void DrawDebugInfo();
	MyTimer m_fpsTimer;					// FPS確認用.
#endif
};

// アプリケーションオブジェクトにアクセスするためのラッパ関数をインラインで.
inline MyApp* GetAppInst() { return MyApp::GetInstance(); }
