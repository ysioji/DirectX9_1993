/*
	main.cpp

	エントリーポイントがあるソース.
*/
#include "stdafx.h"		// すべてのソースファイルで必ず一番最初にインクルードする.
#include "myApp.h"

// エラー発生時のHRESULTを文字列に変換するための補助関数.
const TCHAR* D3DErrStr(HRESULT res)
{
	switch (res) {
		case D3D_OK:					return _T("D3D_OK");
		case D3DERR_DEVICELOST:			return _T("D3DERR_DEVICELOST");
		case D3DERR_DRIVERINTERNALERROR:return _T("D3DERR_DRIVERINTERNALERROR");
		case D3DERR_INVALIDCALL:		return _T("D3DERR_INVALIDCALL");
		case D3DERR_OUTOFVIDEOMEMORY:	return _T("D3DERR_OUTOFVIDEOMEMORY");
		case D3DERR_DEVICENOTRESET:		return _T("D3DERR_DEVICENOTRESET");
		case E_OUTOFMEMORY:				return _T("E_OUTOFMEMORY");
	}
	return _T("unknown error");
}

// イベント処理コールバック（ウィンドウプロシージャ）.
// イベント発生時にDispatchMessage関数から呼ばれる.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// hWnd 	イベントの発生したウィンドウ.
	// uMsg 	イベントの種類を表すID.
	// wParam	第一メッセージパラメータ.
	// lParam 	第二メッセージパラメータ.
	// return 	DefWindowProcの戻り値に従う.

	// イベントの種類に応じて、switch文にて処理を切り分ける.
	switch (uMsg) {
		case WM_CREATE:
		{
			break;
		}
		case WM_PAINT:
		{
			//Player.DrawBrush(hWnd);
			break;
		}
		case WM_DESTROY:
		{
			// GetMessageの戻り値は0になり、イベントループを終了させる.
 			PostQuitMessage(0);
			break;
		}

		case WM_CLOSE:// 終了通知(CLOSEボタンが押された場合など)が届いた場合.
		{
			DestroyWindow(hWnd);// WM_DESTROYイベントを発生させる.
			break;
		}

		default:
		{
			break;
		}
	}

	// デフォルトのウィンドウイベント処理.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// エントリポイント（コンソールを表示する）.
int main(int argc, char* argv[])
{
	// argc コマンドラインから渡された引数の数.
	// argv 引数の実体へのポインタ配列.

	MyApp* pApp = GetAppInst();
	if (pApp->InitApp()) {
		pApp->MainLoop();
	}

	pApp->Release();	// リソース解放.
	return 0;// 正常終了.
}
