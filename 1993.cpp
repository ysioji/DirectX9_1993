
#include "stdafx.h"
#include "myApp.h"
//#include "filed.h"
//KJJ V
#define FULLSCREEN

//#if !defined(_DEBUG)
//#define FULLSCREEN
//#endif

// プロトタイプ宣言.
const TCHAR* D3DErrStr(HRESULT res);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ウィンドウタイトルバーに表示されるバージョン名.
static const TCHAR* APP_NAME = _T("1993");
static const TCHAR* APP_VERSION = _T(__DATE__) " " _T(__TIME__);

// アプリケーションデータ実体の定義.
MyApp MyApp::sm_appData;



////////////////////////////////////////////////////////////////////////////////
// MyAppクラスのコンストラクタ.
MyApp::MyApp()
	: m_hInstance(NULL)
	, m_hWnd(NULL)
	, m_pD3D(NULL)
	, m_pDev(NULL)
	, m_pSpr(NULL)
	, m_gameScene(GAME_SCENE_INVALID)
{
	
}

// Windowを作成するなどのアプリケーション初期化.
BOOL MyApp::InitApp()
{
	srand((UINT)time(NULL));// 乱数系列を現在時刻で初期化.

	// このプログラムが実行されるときのインスタンスハンドルを取得.
	m_hInstance = GetModuleHandle(NULL);

	// まずウィンドウクラスを登録する.
	// これはウィンドウ生成後の処理の仕方をWindowsに教えるためである.

	// ウィンドウクラスの定義.
	WNDCLASSEX wc;								// wc: ウィンドウクラス（WindowClass).
	ZeroMemory(&wc, sizeof(WNDCLASSEX));		// 変数wcをゼロクリアする.
	wc.cbSize = sizeof(WNDCLASSEX);				// この構造体の大きさを与える.
	wc.lpfnWndProc = (WNDPROC)WindowProc;		// ウィンドウプロシージャ登録（関数のポインタをセット）.
	wc.hInstance = m_hInstance;					// インスタンスハンドルを設定.
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);	// マウスカーソルの登録（Windowsに組み込まれているカーソルの絵を使う）.
	wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);	// 背景をGRAYに.
	wc.lpszClassName = APP_NAME;				// クラス名、CreateWindowと一致させる.

	// ウィンドウクラスの登録.
	RegisterClassEx(&wc);

	// タイトルバーのサイズを考慮したウィンドウサイズを取得.
	RECT rc = { 0, 0, WIDTH, HEIGHT }; //ゲーム画面のサイズ（解析度）をrcに入れて
	/*
	left：矩形の左端のX1座標
	top：矩形の左端のY1座標
	right：矩形の右端のX2座標
	bottom：矩形の右端のY2座標
	*/
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);//ウィンドウを調整
	const int width = rc.right - rc.left; //ウィンドウ幅　　0の場合右端のX2座標も同じ値
	const int height = rc.bottom - rc.top;//ウィンドウ高さ　0の場合右端のY2座標も同じ値
	// 起動時の画面サイズを取得.
	const int screen_x = GetSystemMetrics(SM_CXSCREEN);//幅スクリーン  が ウィンドウのサイズを取得
	const int screen_y = GetSystemMetrics(SM_CYSCREEN);//高さスクリーンが ウィンドウのサイズを取得
	// 起動時の画面サイズに対してセンタリング.
	const int x = (screen_x - width) / 2;	//	居中
	const int y = (screen_y - height) / 2;  //  居中

	// ウィンドウの生成.
	m_hWnd = CreateWindow(
		APP_NAME, APP_VERSION, WS_OVERLAPPEDWINDOW, x, y, width, height,
		NULL, NULL, m_hInstance, NULL
	);
	if (m_hWnd == NULL) {
		return FALSE;
	}

	HRESULT hr = E_FAIL;

	// DirectXを初期化する.
	hr = InitDirectX();
	if (FAILED(hr)) {
		MessageBox(NULL, D3DErrStr(hr), _T("Direct3D初期化失敗"), MB_OK);
		return FALSE;
	}

	// リソース読み込み.
	if (!LoadResource()) {
		return FALSE;
	}

	ShowWindow(m_hWnd, SW_SHOWNORMAL);	// 作成したウィンドウを表示する.

	InitGame();

	m_syncTimer.StartTimer();
#if defined(_DEBUG)
	m_fpsTimer.StartTimer();
#endif

	return TRUE;
}

// DirectXを初期化する.
HRESULT MyApp::InitDirectX()
{
	HRESULT hr = D3D_OK;

	// Direct3Dインスタンスオブジェクトを生成する.
	// D3D_SDK_VERSIONと、ランタイムバージョン番号が適切でないと、NULLが返る.
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	// NULLならランタイムが不適切.
	if (m_pD3D == NULL) return 0;

	D3DPRESENT_PARAMETERS d3dpp;// デバイス作成時のパラメータ.
	// PRESENTパラメータをゼロクリアし、適切に初期化.
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	// ウィンドウモードに.
#ifdef FULLSCREEN
	// FULLSCREEN

	d3dpp.Windowed = FALSE;//falseがfullscreen tureがwindows mode
	int fx = WIDTH;
	int fy = HEIGHT;
	d3dpp.BackBufferWidth = fx;
	d3dpp.BackBufferHeight = fy;
	
#else
	//windows mode
	d3dpp.Windowed = TRUE;
	d3dpp.BackBufferHeight = WIDTH;
	d3dpp.BackBufferWidth = HEIGHT;
#endif 
	
	d3dpp.BackBufferCount = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	// バックバッファはRGBそれぞれ８ビットで.
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	// Present時に垂直同期に合わせる.
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	// D3Dデバイスオブジェクトの作成.
	//hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
	//	D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_pDev);
	hr = m_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &(d3dpp), &(m_pDev));
	if (FAILED(hr)) return hr;

	// スプライトの作成.
	hr = D3DXCreateSprite(m_pDev, &m_pSpr);
	if (FAILED(hr)) return hr;

	// フォントの作成.
	hr = InitFont();
	if (FAILED(hr)) return hr;
	//input初期化
	MyInput* pInput = GetInputInst();
	pInput->InitInput(m_hInstance, m_hWnd);


	return D3D_OK;
}

// リソース読み込み.
BOOL MyApp::LoadResource()
{
	try {
		// フォント用画像読み込み.
		m_bmpFont[BMP_FONT_16].LoadFontTexture(m_pDev, _T("img/fontBitmap.png"), 16);
		m_bmpFont[BMP_FONT_64].LoadFontTexture(m_pDev, _T("img/fontBitmap_64r.png"), 64);
	
	//テクスチャ読み込み
		const TCHAR* pszTexName[]= {
			//p:ポインタ型  s:string文字列   z:端末がnullで終了
			  _T("img/tile/tsuchi_32.png")
			 ,_T("img/tile/umi_32.png")
			 ,_T("img/tile/kusa_32.png")
			 ,_T("img/tile/numa_32.png")

			 ,_T("img/tile/ki_32.png")
			 ,_T("img/tile/kareki_32.png")
			 ,_T("img/tile/ishi_32.png")
			 ,_T("img/tile/ishi3_32.png")
			 ,_T("img/tile/ishi2_32.png")
			 ,_T("img/tile/ishi1_32.png")
			 ,_T("img/tile/hako_32.png")

			
			 ,_T("img/playerAAA_32.png")
			 ,_T("img/playerBBB_32.png")
			 ,_T("img/playerCCC_32.png")
			 ,_T("img/playerDDD_32.png")
			 ,_T("img/tile/playerBullets_32.png")
		     ,_T("img/AAAPOTO_128.png")
		     ,_T("img/AAANATION_224CCC.png")
		     ,_T("img/TANK_HPMARK_24.png")
		     ,_T("img/TANK_RINGMARK_24.png")
		     ,_T("img/TANK_SPEEDMARK_24.png")
		     ,_T("img/PILOT_HPMARK_24.png")
		     ,_T("img/BBBPOTO_128.png")
		     ,_T("img/BBBNATION_224C.png")
		     ,_T("img/CCCPOTO_128.png")
			 ,_T("img/CCCNATION_224C.png")
			 ,_T("img/DDDPOTO_128C.png")
			 ,_T("img/DDDNATION_224C.png")

			 ,_T("img/tile/speed_32.png")
			 ,_T("img/tile/distance_32.png")
			 ,_T("img/tile/spanner_32.png")
			/* ,_T("img/post.png")*/
			,_T("img/TEST.png")		// タイトル画像
			,_T("img/logo_1993.png")		// ロゴ画像
			/*,_T("img/image.png")*/
		};
		//static_assert(_countof(pszTexName)==TEX_COUNT,"" ):
		for (int i = 0; i < TEX_COUNT; i++) {
			LoadTexture(m_pDev, pszTexName[i], &m_pTex[i]);
		}
	}
	catch (...) {
		return FALSE;
	}

	return TRUE;
}


//ゲーム開始時の初期化
void MyApp::InitGame()
{

	//タイトル部
	{
		TitleInit();
	}
	//ゲーム部
	{
		m_field.FieldInit(); //フィールドの初期化
		m_field.ReadCsv1("field_1.csv");//フィールド1を読み取る
		m_field.ReadCsv2("field_2.csv");//フィールド2を読み取る
		m_field.Field2HPInit();

		//Players動的生成の初期化
		m_players.clear();
		m_players.push_back(&m_pAAA);
		m_players.push_back(&m_pBBB);
		m_players.push_back(&m_pCCC);
		m_players.push_back(&m_pDDD);

		//Players各ステータスの初期化
		m_pAAA.Init(&m_field);
		m_pBBB.Init(&m_field);
		m_pCCC.Init(&m_field);
		m_pDDD.Init(&m_field);
	}

}


// リソースを解放する.
void MyApp::Release()
{
	//テクスチャの解放を追加
	for (int i = TEX_COUNT - 1; i >= 0; i--) {
		if (m_pTex[i]) m_pTex[i]->Release();
	}
	if (m_pSpr)	m_pSpr->Release();
	if (m_pDev)	m_pDev->Release();
	if (m_pD3D)	m_pD3D->Release();
}

// Title
void MyApp::TitleInit() {
	m_gameScene = GAME_SCENE_BEGIN;
}

void MyApp::UpdateTitle()
{
	MyInput* pInput = GetInputInst();
	// デバッグ用 : Enterキーでゲームシーンへ
	if (pInput->IsPushKeyOne(DIK_RETURN)) {
		/*m_gameScene = GAME_SCENE_STORY;*/
		ChangeScene(GAME_SCENE_SELECT);
		printf("Enter");
	}

	//MyInput* pInput = GetInputInst();
	//// 何かキーが押されたらゲームシーンへ
	//bool isAnyKeyPressed = false;
	//for (int i = 0; i < 256; ++i) {
	//	if (pInput->IsPushKeyOne(i)) {
	//		isAnyKeyPressed = true;
	//		break;
	//	}
	//}
	// コントローラーは2つ引数必要
	/*for (int i = 0; i < 30; ++i) {
		if (pInput->IsPushBtnOne(i)) {
			isAnyKeyPressed = true;
			break;
		}
	}*/
	//if (isAnyKeyPressed = true) {
	//	/*m_gameScene = GAME_SCENE_STORY;*/
	//	ChangeScene(GAME_SCENE_STORY);
	//	printf("キー入力 o");
	//}
}
//タイトル画面を描画
void MyApp::DrawTitle()
{
	//Title messsage
	//背景
	// 背景色を決める。RGB=(0,0,255)とする.-
	D3DCOLOR rgb = D3DCOLOR_XRGB(0, 0, 255);
	// 画面全体を消去.
	m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, rgb, 1.0f, 0);

	// 描画を開始（シーン描画の開始）.
	m_pDev->BeginScene();
	m_pSpr->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_pSpr->SetTransform(&identity);

	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();
	ID3DXSprite* pSpr = GetAppInst()->GetDxSpr();
	IDirect3DTexture9* pTex = GetAppInst()->GetDxTex(TEX_TITLE);
	IDirect3DTexture9* pLogo = GetAppInst()->GetDxTex(TEX_LOGO);

	//D3DXMATRIX identity;
	//D3DXMatrixIdentity(&identity);
	//pSpr->SetTransform(&identity);
	// テクスチャ情報を取得（幅、高さ）
	D3DSURFACE_DESC desc;
	pTex->GetLevelDesc(0, &desc);

	float texW = (float)desc.Width;   // 512
	float texH = (float)desc.Height;  // 512
		
	// 画面いっぱいにするスケールを計算
	float scaleX = 1280.0f / texW;
	float scaleY = 1024.0f / texH;

	D3DXMATRIX mat;
	D3DXMatrixScaling(&mat, scaleX, scaleY, 1.0f);
	pSpr->SetTransform(&mat);

	// 背景画像の描画
	m_pSpr->Draw(pTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);//888899
	//m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xFFFFFFFF);
	//pSpr->Draw(pTex, nullptr, nullptr, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pSpr->End();

	////text処理
	static int frameBlink = 0;

	// 点滅の間隔
	int blinkInterval = 30;
	frameBlink++;

	// 点滅 ON/OFF
	bool visible = (frameBlink / blinkInterval) % 2 == 0;

	// TEXTアニメーション描画 機体の4色で回す
	D3DCOLOR colors[4] =
	{
		D3DCOLOR_XRGB(230,  20,  20),   // 赤
		D3DCOLOR_XRGB(250, 200,  50),   // 黄色
		D3DCOLOR_XRGB(250, 100, 160),	// ピンク
		D3DCOLOR_XRGB(148, 20, 210),    // 紫 6216E5
	};
	static int colorTimer = 0;
	static int colorIndex = 0;
	static bool oldVisible = true;
	// 4色のインデックス
	if (visible == true && oldVisible == false)
	{
		colorIndex = rand() % 4;
	}
	oldVisible = visible;
	ID3DXFont* font = GetAppInst()->GetFont();
	// テキスト
	if (visible == true)   // 点滅 : OFFの時は描画しない
	{
		wchar_t Text[64];
		ZeroMemory(Text, sizeof(Text));
		swprintf_s(Text, 64, L"Hello world\n");
		// 中央に表示
		RECT rcTitle{ -30, (HEIGHT / 2) + 200, WIDTH, HEIGHT };
		// RECT rcTitle{ 0, 0, WIDTH, HEIGHT};
		 /*font->DrawText(nullptr, Text, -1, &rcTitle,
			 DT_CENTER | DT_VCENTER | DT_SINGLELINE,
			 colors[colorIndex]);*/
			 // コントローラーABXY
			 //font->DrawText(nullptr, L"任意のボタンを押してゲームを開始", -1, &rcTitle, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 255, 0));
		font->DrawText(nullptr, L"任意のボタンを押してゲームを開始", -1, &rcTitle, DT_CENTER | DT_VCENTER, colors[colorIndex]);
	}
	// シーンの描画を終了.
	m_pDev->EndScene();
}
void MyApp::SelectInit()
{
	m_gameScene = GAME_SCENE_SELECT;
}
void MyApp::UpdateSelect() 
{
	MyInput* pInput = GetInputInst();
	// デバッグ用 : Enterキーでゲームシーンへ
	if (pInput->IsPushKeyOne(DIK_RETURN)) {
		/*m_gameScene = GAME_SCENE_STORY;*/
		ChangeScene(GAME_SCENE_STORY);
		printf("Enter");
	}

}
void MyApp::DrawSelect()
{	
	//Select messsage
	//背景
	// 背景色を決める。RGB=(0,0,255)とする.-
	D3DCOLOR rgb = D3DCOLOR_XRGB(0, 0, 0);
	// 画面全体を消去.
	m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, rgb, 1.0f, 0);
	// 描画を開始（シーン描画の開始）.
	m_pDev->BeginScene();
	m_pSpr->Begin(D3DXSPRITE_ALPHABLEND);
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_pSpr->SetTransform(&identity);
	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();
	ID3DXSprite* pSpr = GetAppInst()->GetDxSpr();
	// 動画を取得してループ再生・カーソルが動いたら初期化

	//シーンの描画を終了.
	m_pSpr->End();
	m_pDev->EndScene();
}

void MyApp::ChangeScene(E_GAME_SCENE nextScene)
{
	// 現在のシーンから次のシーンへの移行処理をここに書く
	switch (nextScene)
	{
	case GAME_SCENE_TITLE:
		this->TitleInit(); // タイトル初期化関数を呼ぶ
		break;
	case GAME_SCENE_SELECT:
		this->SelectInit(); // タイトル初期化関数を呼ぶ
		break;
	case GAME_SCENE_STORY:
		/*case GAME_SCENE_BUTTLE:*/
		this->InitGame(); // ゲーム開始初期化関数を呼ぶ
		break;
		//case GAME_SCENE_OVER:
		//	this->GameOverInit(); // ゲームオーバー初期化関数を呼ぶ
		//	break;
	case GAME_SCENE_RESULT:
		// リザルト画面の初期化処理
		break;
	default:
		break;
	}
	m_gameScene = nextScene;
}
void MyApp::UpdateScene()
{
	switch (m_gameScene)
	{
	case GAME_SCENE_TITLE:
		this->UpdateTitle();
		break;
	case GAME_SCENE_SELECT:
		this->UpdateSelect();
		break;
	case GAME_SCENE_STORY:
		/*case GAME_SCENE_BUTTLE:*/
		this->UpdateStory(); // プレイ中の更新処理
		break;
		//case GAME_SCENE_OVER:
		//	this->Update();
		//	break;
	case GAME_SCENE_RESULT:
		this->UpdateResult();
		break;

	default:
		break;
	}
}
void MyApp::DrawScene()
{
	switch (m_gameScene)
	{
	case GAME_SCENE_TITLE:
		this->DrawTitle();
		break;
	case GAME_SCENE_SELECT:
		this->DrawSelect();
		break;

	case GAME_SCENE_STORY:
		this->DrawStory();
		break;
		//case GAME_SCENE_OVER:
		//	this->DrawGameOver();
		//	break;
	case GAME_SCENE_RESULT:
		this->DrawResult();
		break;
	default:
		break;
	}
}

void MyApp::UpdateStory()
{
	MyInput* pInput = GetInputInst();
	// 何かキーが押されたらゲームシーンへ
	if (pInput->IsPushKeyOne(DIK_TAB)) {
		/*	m_gameScene = GAME_SCENE_RESULT;*/
		this->ChangeScene(GAME_SCENE_RESULT);
		printf("Key");
	}
}
void MyApp::DrawStory() {
	D3DCOLOR rgb = D3DCOLOR_XRGB(64, 64, 64);
	// 画面全体を消去.
	m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, rgb, 1.0f, 0);

	// 描画を開始（シーン描画の開始）.
	m_pDev->BeginScene();

	// スプライトの描画を開始. 追加部分
	m_pSpr->Begin(D3DXSPRITE_ALPHABLEND);

	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	m_pSpr->SetTransform(&identity);


	// ↓ここでスプライトを描画.
	{
		//フィールド(layout1)の描画
		m_pSpr->SetTransform(&identity);
		m_field.DrawObject1(TEX_TSUCHI);
		m_field.DrawObject1(TEX_UMI);
		m_field.DrawObject1(TEX_KUSA);
		m_field.DrawObject1(TEX_NUMA);
		//フィールド(layout2)の描画
		m_pSpr->SetTransform(&identity);
		m_field.DrawObject2(TEX_KI);
		m_field.DrawObject2(TEX_ISHI);
		m_field.DrawObject2(TEX_HAKO);
		//アイテムの描画
		m_field.DrawItems();
		//p1
		m_pSpr->SetTransform(&identity);
		m_pAAA.Draw(TEX_PAAA);
		m_pAAA.DrawBullets();
		m_pAAA.DrawPoto();
		m_pAAA.DrawNation();
		m_pAAA.DrawHpBar();
		m_pAAA.DrawTankMark();
		m_pAAA.DrawTankRing();
		m_pAAA.DrawTankSpeed();
		m_pAAA.DrawPilotMark();
		//P2
		m_pSpr->SetTransform(&identity);
		m_pBBB.Draw(TEX_PBBB);
		m_pBBB.DrawBullets();
		m_pBBB.DrawPoto();
		m_pBBB.DrawNation();
		m_pBBB.DrawHpBar();
		m_pBBB.DrawTankMark();
		m_pBBB.DrawTankRing();
		m_pBBB.DrawTankSpeed();
		m_pBBB.DrawPilotMark();
		//p3
		m_pSpr->SetTransform(&identity);
		m_pCCC.Draw(TEX_PCCC);
		m_pCCC.DrawBullets();
		m_pCCC.DrawPoto();
		m_pCCC.DrawNation();
		m_pCCC.DrawHpBar();
		m_pCCC.DrawTankMark();
		m_pCCC.DrawTankRing();
		m_pCCC.DrawTankSpeed();
		m_pCCC.DrawPilotMark();
		//p4
		m_pSpr->SetTransform(&identity);
		m_pDDD.Draw(TEX_PDDD);
		m_pDDD.DrawBullets();
		m_pDDD.DrawPoto();
		m_pDDD.DrawNation();
		m_pDDD.DrawHpBar();
		m_pDDD.DrawTankMark();
		m_pDDD.DrawTankRing();
		m_pDDD.DrawTankSpeed();
		m_pDDD.DrawPilotMark();

#if defined(_DEBUG)
		DrawDebugInfo();// デバッグ情報の表示.
#endif
	}
	// ↑ここでスプライトを描画.

	// スプライトの描画を終了.
	m_pSpr->End();

	// シーンの描画を終了.
	m_pDev->EndScene();

}
void MyApp::UpdateResult()
{
	MyInput* pInput = GetInputInst();
	if (pInput->IsPushKeyOne(DIK_RETURN)) {
		this->ChangeScene(GAME_SCENE_RESULT);
		printf("push Enterkey");
	}
}

void MyApp::DrawResult() {
	D3DCOLOR rgb = D3DCOLOR_XRGB(64, 64, 64);
	// 画面全体を消去.
	m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, rgb, 1.0f, 0);
	// 描画を開始（シーン描画の開始）.
	m_pDev->BeginScene();
	m_pSpr->Begin(D3DXSPRITE_ALPHABLEND);
	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();
	ID3DXSprite* pSpr = GetAppInst()->GetDxSpr();
	//IDirect3DTexture9* pTex = GetAppInst()->GetDxTex(TEX_TITLE);
	//// 背景画像の描画
	//m_pSpr->Draw(pTex, nullptr, nullptr, nullptr, 0xFFFFFFFF);//888899
	//m_pSpr->End();
	
	ID3DXFont* font = GetAppInst()->GetFont();
	// テキスト表示
	RECT rcWIN = { -30, -300, WIDTH, HEIGHT };
	// 誰が勝利したか表示する(gamesceneで勝った人)
	//font->DrawText(nullptr, L" PL :勝利", /*&,*/-1,&rcWIN, DT_CENTER | DT_TOP, D3DCOLOR_XRGB(255, 255, 0));
	//RECT rcSelect = { -30, 0, WIDTH, HEIGHT };
	//font->DrawText(nullptr, L"もう一度戦う", /*&,*/-1,&rcWIN, DT_CENTER | DT_TOP, D3DCOLOR_XRGB(255, 255, 0));
	// 次の画面遷移を提示する (操作はどのPLができるか)
	m_pDev->EndScene();
}

// 毎フレームの更新処理.
BOOL MyApp::UpdateMain()
{
	LONGLONG us = 0; UINT fps = 0;

	// 60FPSで画面更新.
	{
		m_syncTimer.GetPassTime(&us, &fps);
		if (us < 1000000 / 60) {
			return FALSE;// 60FPSで画面更新.
		}
		m_syncTimer.StartTimer();
	}

	
	MyInput* pInput = GetInputInst();
	pInput->UpdateInput(m_hWnd);
	UpdateScene(); // シーンごとの更新処理(ここに必須)
	//1p
	m_pAAA.Updata();
	CheckBulletHitPlayer(m_pAAA, m_pBBB);
	CheckBulletHitPlayer(m_pAAA, m_pCCC);
	CheckBulletHitPlayer(m_pAAA, m_pDDD);
	//2p
	m_pBBB.Updata();
	CheckBulletHitPlayer(m_pBBB, m_pAAA);
	CheckBulletHitPlayer(m_pBBB, m_pCCC);
	CheckBulletHitPlayer(m_pBBB, m_pDDD);
	//3p
	m_pCCC.Updata();
	CheckBulletHitPlayer(m_pCCC, m_pAAA);
	CheckBulletHitPlayer(m_pCCC, m_pBBB);
	CheckBulletHitPlayer(m_pCCC, m_pDDD);
	//4p
	m_pDDD.Updata();
	CheckBulletHitPlayer(m_pDDD, m_pAAA);
	CheckBulletHitPlayer(m_pDDD, m_pBBB);
	CheckBulletHitPlayer(m_pDDD, m_pCCC);
	

	m_field.DebugPrintAll();
	//ドロップ処理
	m_field.DropUpdate();
	return TRUE;
}

// 描画メイン.
void MyApp::DrawMain()
{}
	//デバイスロスト対策
	void MyApp::ScreenFlip()
	{
		if (m_pDev->Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) {

			printf("デバイスロスト発生!\n");

			//復帰可能かをチェック
			if (m_pDev->TestCooperativeLevel() == D3DERR_DEVICELOST) {

				printf("描画を再開する準備が整っていない!\n");

				return;
			}

			if (m_pDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {

				printf("描画デバイスを復帰できる!\n");

				//ビデオメモリリソースをすべて解放
				m_pSpr->Release();
				m_pDev->Release();
				m_pD3D->Release();

				//pPresentationParametersには復活させる時の描画デバイスの能力を設定する
				HRESULT Reset(
					D3DPRESENT_PARAMETERS * pPresentationParameters
				);

			}

		}

	}



// メインループ処理.
void MyApp::MainLoop()
{
	// メッセージループ
	bool flag = 1;
	while (flag) {
		// メッセージがあるかどうか確認する.
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			// メッセージがあるので処理する.
			if (GetMessage(&msg, NULL, 0, 0) == 1) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				flag = 0;
			}
		}
		else {
			if (UpdateMain()) {	// 更新.
				DrawScene();		// 描画.
				ScreenFlip();
			}
		}
		// Sleepなど行っていないが、DrawMain関数内のpDevice->Presentが.
		// 画面更新待ちを行うので、結果的にVSyncごとにイベントループが回る.
	}
}
//3
template <class S, class T>
void MyApp::CheckBulletHitPlayer(S& shooter, T& target)
{
	// 死亡しているプレイヤーには当たらない
	if (target.GetHp() <= 0) return;

	float pr = 16.0f;  // プレイヤー半径（調整可能）
	float br = 8.0f;   // 弾半径（BulletBase::radius）

	float hitR2 = (pr + br) * (pr + br);

	// shooter の弾を全部確認
	for (auto& bullet : shooter.GetBullets())
	{
		if (!bullet.GetAlive()) continue;

		Vector2 bp = bullet.GetPos();
		Vector2 tp = target.GetPos();

		float dx = bp.x - tp.x;
		float dy = bp.y - tp.y;
		float dist2 = dx * dx + dy * dy;

		if (dist2 <= hitR2)
		{
			// 当たり！
			//target.m_tstatus.hp--;
			bullet.SetAlive(false);
			target.Damage(1);
			target.IsLive();
		}
	}
}


#if defined(_DEBUG)
// デバッグ情報の表示.
void MyApp::DrawDebugInfo()
{
	// FPS(Frame/Seconds)を画面左下に表示.
	LONGLONG us = 0; UINT fps = 0;
	m_fpsTimer.GetPassTime(&us, &fps);

	char txt[256] = { 0 };
	sprintf_s(txt, "%lluus, %dFPS\n", us, fps);
	m_bmpFont[BMP_FONT_16].DrawBmpFont(txt, 4, HEIGHT - 20, 16, 8);

	//char txt[256] = { 0 };
	float AAAx = m_pAAA.GetPos().x +16;
	float AAAy = m_pAAA.GetPos().y +16;
	float AAAangle = m_pAAA.GetAngle();
	sprintf_s(txt, "AAA.x%.1f,AAA.y%.1f,pos[%d][%d],AAA.angle %.1f\n", AAAx, AAAy, int((AAAx-256)/ FIELD_SIZE), int((AAAy-128)/ FIELD_SIZE),AAAangle);
	m_bmpFont[BMP_FONT_16].DrawBmpFont(txt, 4, HEIGHT - 40, 16, 8);

	m_fpsTimer.StartTimer();
}
#endif

//フォントの初期化
HRESULT MyApp::InitFont()
{
	HRESULT hr;
	//フォントの生成
	//デバイス,文字高さ,文字幅,フォントスタイル,ミップマップのレベル,斜体にするかどうか,文字セット,出力精度,出力品質,フォントピッチとファミリ,フォント名,フォントポインタ
	/*hr = D3DXCreateFont(
		m_pDev, 32, 0, FW_HEAVY, 1, FALSE,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, L"DragonQuestFC", &
		);
	if FAILED(hr) { return(E_FAIL); }*/
	hr = D3DXCreateFont(
		m_pDev, 36, 0, FW_HEAVY, 1, FALSE,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, L"DotGothic16-Regular", &m_pFont);
	if FAILED(hr) { return(E_FAIL); }
	//return S_OK;
	//小さく使うためのフォント
	hr = D3DXCreateFont(
		m_pDev, 16, 0, FW_HEAVY, 1, FALSE,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS,
		ANTIALIASED_QUALITY, FF_DONTCARE, L"DotGothic16-Regular", &m_pFontS);
	if (FAILED(hr)) return E_FAIL;
	return S_OK;
}










