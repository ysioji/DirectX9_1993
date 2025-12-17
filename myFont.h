#pragma once

#include <string>
#include <winnt.h>

//#define FontNameBIZUDPMincho   "BIZ UDP–¾’©"
//#define FontDefName            FontNameBIZUDPMincho			
//#define FontDefSize            20
//#define FontDefThink           1
//#define FontDefType            DX_FONTTYPE_ANTIALIASING_8X8

//#define FontNameDCFC           "DragonQuestFC"
//#define FontDefName            FontNameDCFC			
//#define FontDefSize            20
//#define FontDefThink           1
//#define FontDefType            DX_FONTTYPE_ANTIALIASING_8X8
//
//class Font
//{
//public:
//    Font();
//    ~Font();
//
//    bool Load(const TCHAR* ttfPath, int size = FontDefSize);
//    void Release();
//    int GetHandle() const { return fontHandle; }
//
//private:
//    int fontHandle = -1;
//    int m_Font;
//    std::wstring fontName = L"DragonQuestFC";
// //   std::wstring fontName = L"BIZ UDP–¾’©";
//   
//};
#define FontNameDCFC           "DotGothic16-Regular"
#define FontDefName            FontNameDotGothic16			
#define FontDefSize            20
#define FontDefThink           1
#define FontDefType            DX_FONTTYPE_ANTIALIASING_8X8

class Font
{
public:
    Font();
    ~Font();

    bool Load(const TCHAR* ttfPath, int size = FontDefSize);
    void Release();
    int GetHandle() const { return fontHandle; }

private:
    int fontHandle = -1;
    int m_Font;
    std::wstring fontName = L"DotGothic16-Regular";
 //   std::wstring fontName = L"BIZ UDP–¾’©";
   
};


