#pragma once
#define	COLS	(26)//列.
#define	ROWS	(26)//行.
#define FIELD_SIZE (32)//フィールド画像一枚の解析度32*32

enum E_TEXTURE : int;

enum ENUM_FIELD_OBJECT1{

	FIELD_OBJECT1_NOTHING,
	FIELD_OBJECT1_ROAD,  //道
	FIELD_OBJECT1_LAKE,	 //川
	FIELD_OBJECT1_GRASS, //草
	FIELD_OBJECT1_SWAMP, //沼
};
enum ENUM_FIELD_OBJECT2 {

	FIELD_OBJECT2_NOTHING,
	FIELD_OBJECT2_TREE,       //樹
	FIELD_OBJECT2_STONE,      //石
	FIELD_OBJECT2_BOX,	      //箱
	FIELD_OBJECT2_BREAKTREE,  //枯れ樹

	FIELD_OBJECT2_AAA,        //1p
	FIELD_OBJECT2_BBB,		  //2p
	FIELD_OBJECT2_CCC,		  //3p
	FIELD_OBJECT2_DDD		  //4p

};

enum E_BOX_ITEM {

	BOX_ITEM_NONE, 
	BOX_ITEM_MOVE,     //移動速度
	BOX_ITEM_DISTANCE, //射程距離
	BOX_ITEM_HP		   //HP回復
};


class Field {

private:

	//layout1要素格納用
	ENUM_FIELD_OBJECT1 m_object1[ROWS][COLS];

	//layou2t要素格納用
	ENUM_FIELD_OBJECT2 m_object2[ROWS][COLS];

	//破壊可能のオブジェクトに　HPを与える用の格納配列
	int m_object2HP[ROWS][COLS];

	E_BOX_ITEM m_boxItem[ROWS][COLS];
public:
	//csvから読み込み
	void ReadCsv1(const char* filename);
	void ReadCsv2(const char* filename);
	//フィールド初期化
	//***********************************************
	void FieldInit();

	void Field2HPInit();
	//***********************************************
	//フィールド描画
	//***********************************************
	
	void DrawObject1(E_TEXTURE TEX_NAME);
	void DrawObject2(E_TEXTURE TEX_NAME);
	//***********************************************
	//フィールド解放
	void FieldRelease();

	ENUM_FIELD_OBJECT1 GetField1(int mx, int my) { return m_object1[my][mx]; }
	ENUM_FIELD_OBJECT2 GetField2(int mx, int my) { return m_object2[my][mx]; }
	void SetField2(int x, int y, ENUM_FIELD_OBJECT2 obj) {m_object2[y][x] = obj;}
	int GetField2HP(int x, int y) const {return m_object2HP[y][x];}
	void SetField2HP(int x, int y, int hp) {m_object2HP[y][x] = hp;}

	//debug
	void DebugPrintAll();

	//３期
	void DropUpdate();
	void DrawItems();
	E_BOX_ITEM RandomDrop();
	void DropItem(int row, int col);
	E_BOX_ITEM& GetBoxItem(int row, int col){return m_boxItem[row][col];}

};