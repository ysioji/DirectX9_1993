#include "stdafx.h"
#include "myFont.h"

Font::Font() {}

Font::~Font() {}

//bool Font::Load(const TCHAR* ttfPath, int size)
//{
//	return false;
//}
bool Font::Load(const TCHAR* ttfPath, int size)
{
    LOGFONT lf = {};
    lf.lfHeight = size;
    lf.lfWeight = FW_NORMAL;

    // インストールしたフォント名をここに書く
    wcscpy_s(lf.lfFaceName, L"DotGothic16-Regular");
    return 0;
}

void Font::Release() {}
