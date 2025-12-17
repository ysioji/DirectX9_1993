#include "stdafx.h"
#include "field.h"
#include "myApp.h"

using namespace std;

void Field::FieldInit() {

    //layout 1初期化
    for (int i = 0; i < ROWS; i++) {
        for (int  j = 0; j < COLS; j++)
        {
            m_object1[i][j] = FIELD_OBJECT1_NOTHING;
        }
    }
    
    //layout 2初期化
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
        {
            m_object2[i][j] = FIELD_OBJECT2_NOTHING;
        }
    }
}
//障害物のHPを設置
void Field::Field2HPInit() {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            //tree 2
            if (m_object2[i][j] == FIELD_OBJECT2_TREE) {
                m_object2HP[i][j] = 2;
            }
			//else if (m_object2[i][j] == FIELD_OBJECT2_BREAKTREE) {
			//	m_object2HP[i][j] = 1;     
			//}
            //stone 4
			else if (m_object2[i][j] == FIELD_OBJECT2_STONE) {
                m_object2HP[i][j] = 4;
            }
            //box 1
            else if (m_object2[i][j] == FIELD_OBJECT2_BOX) {
                m_object2HP[i][j] = 1;
            }
            else {
                m_object2HP[i][j] = 0;
            }
        }
    }
}

void Field::ReadCsv1(const char* filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        
        std::cout << "開けませんでした " << filename << std::endl;
        
        return;
    }
    //考えて方
    //一行内容26 「,」捨て,「space」行切り替え列+1
    std::string line;//一行の内容を格納用
    int r = 0;  


    while (r < ROWS && std::getline(file, line))
    //target < COLS以上
    //対象fileに一行文ずつ読み取る
    {
        std::istringstream ss(line);//lineの中身をssにコピー
        for (int c = 0; c < COLS; c++) {
        
            int v;
            if (!(ss >> v)) break;
            // enum が 1..4（Road..Swamp）想定、範囲外は NOTHING
            m_object1[r][c] = (v >= FIELD_OBJECT1_ROAD && v <= FIELD_OBJECT1_SWAMP)
                ? static_cast<ENUM_FIELD_OBJECT1>(v)
                : FIELD_OBJECT1_NOTHING;
           
            //debug
            std::cout << "m_object1[" << r << "][" << c << "] = "
                << static_cast<int>(m_object1[r][c]) << '\n';

            // 直後のカンマ1個を食べる（無ければ戻す）
            if (ss.peek() == ',') ss.get();
        }
        r++; // 次の行へ
    }
}

void Field::ReadCsv2(const char* filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {

        std::cout << "開けませんでした " << filename << std::endl;

        return;
    }
    //考えて方
    //一行内容26 「,」捨て,「space」行切り替え列+1
    std::string line;//一行の内容を格納用
    int r = 0;


    while (r < ROWS && std::getline(file, line))
        //target < COLS以上
        //対象fileに一行文ずつ読み取る
    {
        std::istringstream ss(line);//lineの中身をssにコピー
        for (int c = 0; c < COLS; c++) {

            int v;
            if (!(ss >> v)) break;
            // enum が 1..4（Road..Swamp）想定、範囲外は NOTHING
            m_object2[r][c] = (v >= FIELD_OBJECT2_TREE && v <= FIELD_OBJECT2_DDD)
                ? static_cast<ENUM_FIELD_OBJECT2>(v)
                : FIELD_OBJECT2_NOTHING;
            //debug  
                std::cout << "m_object2[" << r << "][" << c << "] = "
                    << static_cast<int>(m_object2[r][c]) << '\n';
        
            // 直後のカンマ1個を食べる（無ければ戻す）
            if (ss.peek() == ',') ss.get();
        }
        r++; // 次の行へ
    }
}



void Field::DrawObject1(E_TEXTURE TEX_NAME){ //TEX_OBJECTS

   // HRESULT hr;
   // hr = D3D_OK;

    IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();// Direct3D デバイスの確認
    assert(m_pDev != NULL && "Direct3D デバイスが初期化されていません");

    ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();// スプライトオブジェクトの確認
    assert(m_pSpr != NULL && "ID3DXSprite オブジェクトが初期化されていません");

    IDirect3DTexture9* m_pTex = GetAppInst()->GetDxTex(TEX_NAME);// テクスチャの確認
    assert(m_pTex != NULL && "ID3DTex オブジェクトが初期化されていません");

    //D3DXVECTOR3 cnt(float(FIELD_SIZE/2), float(FIELD_SIZE/2), 0.0f);//中心点
    //D3DXVECTOR3 cnt(float(0.0f), float(0.0f), 0.0f);//左上
    D3DXMATRIX trans;
  
    const int startX = (WIDTH- 832) /2; //始点x
    const int startY = (HEIGHT- 832)/2 ;//始点y
    const int size = FIELD_SIZE; //one pixiv 32*32
    RECT rc;
    rc = { 0, 0,  32, 32 };

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            //描画の内容 m_object 1内の要素数で決める
                //m_object1[i][j]= ? (1-4)
                //1 道 0  0  32  32 |0   0   32   32
                //2 湖 32 0  64  32 |37  0   69   32
                //3 草 64 0  96  32 |74  0   106  32          
                //4 沼 96 0  128 32 |106 0   138  32
            int tile = m_object1[i][j];
            assert(1 <= tile && tile <= 4);
            //rc = { size * (tile - 1), 0,
            //       size * tile,   size };
            
            if (tile == int(TEX_NAME) + 1) { //
                //描画の場所を決める
                //0,0 から→32,0 →64, 0.....
                //0,32から→32,32→64,32.....
                D3DXMatrixTranslation(&trans, float(startX + j * size),
                    float(startY + i * size), 0.0f);
                m_pSpr->SetTransform(&trans);
                m_pSpr->Draw(m_pTex, &rc, NULL, NULL, 0xFFFFFFFF);
                //if (FAILED(hr)) { m_pSpr->End();  return hr; }
                //return hr;
            }
            else {
                continue;
            }
        }
    }
}

void Field::DrawObject2(E_TEXTURE TEX_NAME)
{
	ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();
	IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();

	const int startX = (WIDTH - 832) / 2;
	const int startY = (HEIGHT - 832) / 2;
	const int size = FIELD_SIZE;
	RECT rc = { 0,0,32,32 };

	D3DXMATRIX trans;

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			int tile = m_object2[i][j];
			int hp = m_object2HP[i][j];

			// ---- TREE（樹）----
			if (tile == FIELD_OBJECT2_TREE)
			{
				IDirect3DTexture9* tex = GetAppInst()->GetDxTex(TEX_KI);
				if (hp <= 0) continue;
				if (hp == 1) tex = GetAppInst()->GetDxTex(TEX_KAREKI);

				D3DXMatrixTranslation(&trans, startX + j * size, startY + i * size, 0);
				m_pSpr->SetTransform(&trans);
				m_pSpr->Draw(tex, &rc, NULL, NULL, 0xffffffff);
			}

			// ▼追加：枯れ樹（BREAKTREE）
			else if (tile == FIELD_OBJECT2_BREAKTREE)
			{
				IDirect3DTexture9* tex = GetAppInst()->GetDxTex(TEX_KAREKI);

				D3DXMatrixTranslation(&trans, startX + j * size, startY + i * size, 0);
				m_pSpr->SetTransform(&trans);
				m_pSpr->Draw(tex, &rc, NULL, NULL, 0xffffffff);
			}

			// === 石：HPごとにテクスチャ切り替え ===
			else if (tile == FIELD_OBJECT2_STONE)
			{
				if (hp <= 0) continue;   // 壊れたら描画しない

				E_TEXTURE tex;

				if (hp >= 4)      tex = TEX_ISHI;    // 4段階目(無傷)
				else if (hp == 3) tex = TEX_ISHI_3;  // 3
				else if (hp == 2) tex = TEX_ISHI_2;  // 2
				else              tex = TEX_ISHI_1;  // 1

				IDirect3DTexture9* pTex = GetAppInst()->GetDxTex(tex);

				D3DXMatrixTranslation(&trans,
					float(startX + j * size),
					float(startY + i * size),
					0.0f);

				m_pSpr->SetTransform(&trans);
				m_pSpr->Draw(pTex, &rc, NULL, NULL, 0xFFFFFFFF);
			}

			// ---- BOX（箱）----
			else if (tile == FIELD_OBJECT2_BOX && TEX_NAME == TEX_HAKO)
			{
				if (hp > 0)
				{
					D3DXMatrixTranslation(&trans,
						float(startX + j * size),
						float(startY + i * size),
						0);

					m_pSpr->SetTransform(&trans);
					m_pSpr->Draw(GetAppInst()->GetDxTex(TEX_HAKO),
						&rc, NULL, NULL, 0xFFFFFFFF);
				}
			}
		}
	}
}

void Field::DebugPrintAll()
{

    MyInput* pInput = GetInputInst(); //D3DInputインスタンスを取得
    if (pInput->IsPushKeyOne(DIK_1)) {
        std::cout << "===== FIELD OBJECT 2 全内容 =====\n";

        for (int r = 0; r < ROWS; r++)
        {
            for (int c = 0; c < COLS; c++)
            {
                std::cout << static_cast<int>(m_object2[r][c]);

                if (c < COLS - 1) std::cout << ", ";
            }
            std::cout << "\n";
        }

        std::cout << "=================================\n";
    }
   }

//3期
void Field::DropUpdate() {
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            int& hp = m_object2HP[i][j];
            ENUM_FIELD_OBJECT2& tile = m_object2[i][j];
            printf("hp=%d tile=%d\n", hp, tile);
            if (hp <= 0 && tile == FIELD_OBJECT2_BOX)
            {
                DropItem(i, j);  

                tile = FIELD_OBJECT2_NOTHING; 
                hp = 0;                     
            }
        }
    }
}
void Field::DrawItems() {

    ID3DXSprite* m_pSpr = GetAppInst()->GetDxSpr();
    IDirect3DDevice9* m_pDev = GetAppInst()->GetDxDev();

    const int startX = (WIDTH - 832) / 2;
    const int startY = (HEIGHT - 832) / 2;
    const int size = FIELD_SIZE;

    D3DXMATRIX trans;
    RECT rc = { 0, 0, 32, 32 };
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            E_BOX_ITEM item = m_boxItem[i][j];
           
            if (item == 0)
              //  printf("no item at %d,%d\n", i, j);
                continue; //skip

            int texID = 0;
            switch (item)
            {
            case BOX_ITEM_MOVE:     texID = TEX_MOVE_UP;break;
            case BOX_ITEM_DISTANCE: texID = TEX_DIS_UP ;break;
            case BOX_ITEM_HP:       texID = TEX_HP_UP  ;break;
            }

            IDirect3DTexture9* pTex = GetAppInst()->GetDxTex(texID);

            D3DXMatrixTranslation(&trans,
                float(startX + j * size),
                float(startY + i * size),
                0);

            m_pSpr->SetTransform(&trans);
            m_pSpr->Draw(pTex, &rc, NULL, NULL, 0xFFFFFFFF);
        }
    }
}


E_BOX_ITEM Field::RandomDrop() {


    int weights[] = {
       25, // MOVE
       25, // DISTANCE
       50  // HP
    };

    int total = 0;
    for (int i = 0; i < 3; i++)
    {
        total += weights[i];
    }


    int r = rand() % total;//1から100

 
    if (r < weights[0]) 
        return BOX_ITEM_MOVE;
    r -= weights[0];

    if (r < weights[1]) return BOX_ITEM_DISTANCE;
    r -= weights[1];
    
    return BOX_ITEM_HP;

}

void Field::DropItem(int row, int col)
{
    E_BOX_ITEM item = RandomDrop(); //ジャージ
    printf("DropItem called at (%d, %d)\n", row, col);
    m_boxItem[row][col] = item; 

  
   // m_object2[row][col] = FIELD_OBJECT2_NOTHING; 描画部,既に実装

}