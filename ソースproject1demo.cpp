#include <algorithm> // std::maxを使用するため
#define NOMINMAX
#include "DxLib.h"

//
// 当たり判定を行う関数
// 
// 緑のキャラ 左   x1
// 　　　　　 上   y1
// 　　　　　 横幅 w1
// 　　　　　 縦幅 h1
// 黄のキャラ 左   x2
// 　　　　　 上   y2
// 　　　　　 横幅 w2
// 　　　　　 縦幅 h2
//
int CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int L1 = x1;      // 左
	int R1 = x1 + w1; // 右(左+横幅)
	int L2 = x2;      // 左
	int R2 = x2 + w2; // 右(左+横幅)

	if (R1 <= L2) return 0; //< 線上も当たらないことにする
	if (R2 <= L1) return 0; //< 線上も当たらないことにする

	int U1 = y1;      // 上
	int D1 = y1 + h1; // 下(上+縦幅)
	int U2 = y2;      // 上
	int D2 = y2 + h2; // 下(上+縦幅)

	if (D1 <= U2) return 0; //< 線上も当たらないことにする
	if (D2 <= U1) return 0; //< 線上も当たらないことにする

	// それ以外の場合は当たっている
	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//======================================================
	// ウインドウ設定とDXライブラリの初期化
	//======================================================
	const int SCREEN_WIDTH = 640;		//< スクリーン横の大きさ
	const int SCREEN_HEIGHT = 480;		//< スクリーン縦の大きさ

	// Windowsモードにする
	ChangeWindowMode(TRUE);
	// 画面サイズの設定
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	// DXライブラリ初期化
	if (DxLib_Init() == -1)
	{
		//初期化に失敗
		return -1;
	}
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);
	//======================================================
	// ゲーム部分
	//======================================================
	//------------------------------------------------------
	// ゲーム初期化部分
	//------------------------------------------------------
	const int CHARA_WIDTH = 32;
	const int CHARA_HEIGHT = 32;

	//< メインキャラの位置、画面の中心
	float x = (SCREEN_WIDTH - CHARA_WIDTH) / 2;	
	float y = (SCREEN_HEIGHT - CHARA_HEIGHT) / 2;
	int is_chara = 1; // フラグを用意(初期は表示するために1)

	float ex = (SCREEN_WIDTH - CHARA_WIDTH) / 2;		//敵キャラの位置
	float ey = 300;		//敵キャラの位置
	int is_enemy = 1; // フラグを用意(初期は表示するために1)

	// 銃弾(shot)を打つ
	int shot_x = 470;
	int shot_y = 370;
	int is_shot = 0;  // フラグを用意

	float yadd = 0.0f;
	float eyadd = 0.0f;

	int chara_act[12];		//プレイヤーキャラロード
	LoadDivGraph("demo/sample007.png", 12, 3, 4, 32, 32, chara_act);
	int enemy_act[12];		//敵キャラロード
	LoadDivGraph("demo/sample008.png", 12, 3, 4, 32, 32, enemy_act);

	int act_motion[] = { 0, 1, 2, 1, };
	int act_index = 0;

	int MAX_MOTION_INDEX = 4; //< 0,1,2,1 の4種類 [0～3]までにするために用意

	const int ACT_SPEED = 20;
	int act_wait = ACT_SPEED;

	const int ACT_DIR_SLIDE = 3; //< 横のキャラ分スライドさせれば次の列を選択する
	int act_dir = 0;

	// マップチップロード
	const int SIZE_MAP_X = 32;  //< マップチップXサイズ
	const int SIZE_MAP_Y = 32;	//< マップチップYサイズ
	const int NUM_CHIP_X = 10;  //< 横10種類
	const int NUM_CHIP_Y = 4;   //< 縦4種類
	const int NUM_CHIP_ALL = NUM_CHIP_X * NUM_CHIP_Y; //< 40個
	int chip[NUM_CHIP_ALL];
	LoadDivGraph("demo/mapchip.png", NUM_CHIP_ALL,
		NUM_CHIP_X, NUM_CHIP_Y,
		SIZE_MAP_X, SIZE_MAP_Y,
		chip);

	// マップチップを使った背景
	const int NUM_MAP_X = 20; // 32x20=640
	const int NUM_MAP_Y = 15; // 32x15=480
	int data[] = {
		// 横20x15
		10,  10,  10,  10,  10,   10,  10,  10,  10,  10,   10,  10,  10,  10,  10,   10,  10,  10,  10, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,

		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   10,  10,  10,  10, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,

		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  10, 10,  10,  10,   10,  10,  10,  10,  10,   10,  10,  10,  10,  10,   10,  10,  10,  10, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0,  0,   0,  0,  0,  0, 10,
		10, 10, 10, 10, 10,  10, 10, 10, 10, 10,  10, 10, 10, 10, 10,  10, 10, 10, 10, 10,
	};

	//------------------------------------------------------
	// メインループ部分
	//------------------------------------------------------
	while (ProcessMessage() == 0)
	{
		//裏画面消す
		ClearDrawScreen();

		// 背景
		for (int j = 0; j < NUM_MAP_Y; j++)
		{
			for (int i = 0; i < NUM_MAP_X; i++)
			{
				int no = data[i + j * NUM_MAP_X];
				if (no != 0)
				{
					DrawGraph(i * SIZE_MAP_X, j * SIZE_MAP_Y, chip[no], FALSE);
				}
			}
		}

		// 前回の位置を覚えておく
		float old_x = x;
		float old_y = y;
		float speed = 3.0f;
		// 敵キャラの前回の位置
		float old_ex = ex;
		float old_ey = ey;
		float enemy_speed = 0.0f;

		// キー情報を取得する
		int input = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		// キーで移動させる
		if (input & PAD_INPUT_RIGHT)
		{
			x += speed;
			act_dir = 2; //< 2列目を選ぶ(0スタート)
		}
		if (input & PAD_INPUT_LEFT)
		{
			x -= speed;
			act_dir = 1; //< 1列目を選ぶ(0スタート)
		}
		if (input & PAD_INPUT_UP) //< キーボードならば上ボタン 
		{
			yadd = -5.0f;
		}
		if (is_shot == 0)
		{
			if (input & PAD_INPUT_A)	//< キーボードならばZボタン
			{
				is_shot = 1;
				shot_x = x + 15;
				shot_y = y + 15;
			}
		}
		if (is_shot == 1)
		{
			// 右を向いているときに銃弾を発射した場合
			if (act_dir == 2) {
				shot_x += 8;

				// 銃弾の当たり判定
				if (is_enemy == 1)
				{
					int hit = CheckHit(ex, ey, 30, 30, shot_x - 5, shot_y - 5, 20, 20);
					if (hit == 1)
					{
						// 当たっているので画面に出たのと同じことをする
						is_shot = 0;
						is_enemy = 0;
					}
				}
				DrawCircle(shot_x, shot_y, 10, GetColor(255, 255, 255), TRUE);
			}
			// 左を向いているときに銃弾を発射した場合
			if (act_dir == 1) {
				shot_x -= 8;

				// 銃弾の当たり判定
				if (is_enemy == 1)
				{
					int hit_shottoenemy = CheckHit(ex, ey, 30, 30, shot_x - 5, shot_y - 5, 20, 20);
					if (hit_shottoenemy == 1)
					{
						// 当たっているので画面に出たのと同じことをする
						is_shot = 0;
						is_enemy = 0;
					}
				}
				DrawCircle(shot_x, shot_y, 10, GetColor(255, 255, 255), TRUE);
			}
		}
		if (shot_x > 640)
		{
			is_shot = 0;
		}
		// プレイヤーキャラと敵キャラの当たり判定
		if (is_enemy == 1)
		{
			int hit_charatoenemy = CheckHit(ex, ey, 30, 30, x - 5, y - 5, 32, 32);
			if (hit_charatoenemy == 1)
			{
				// 当たっているので画面に出たのと同じことをする
				is_chara = 0;
				is_enemy = 0;
			}
		}

		y += yadd;
		ey += eyadd;
		yadd += 0.2f;
		eyadd += 0.2f;

		//if (yadd > 30) yadd = 30; //< マス飛ばさないように制御

		// ブロックの当たり判定
		for (int j = 0; j < NUM_MAP_Y; j++)
		{
			for (int i = 0; i < NUM_MAP_X; i++)
			{
				int no = data[i + j * NUM_MAP_X];
				if (no >= 10 && no < 20)
				{
					// プレイヤーキャラの壁当たり判定
					if (CheckHit(i * SIZE_MAP_X, j * SIZE_MAP_Y, SIZE_MAP_X, SIZE_MAP_Y,
						x, y, CHARA_WIDTH, CHARA_HEIGHT))
					{
						// 上下から当たっている
						if (y != old_y)
						{
							int _y = std::max(y, old_y);
							_y /= SIZE_MAP_Y;
							_y *= SIZE_MAP_Y;
							y = _y;
							yadd = 0.0f;
						}
						else
							// 左右から当たっている
							if (x != old_x)
							{
								int _x = std::max(x, old_x);
								_x /= SIZE_MAP_X;
								_x *= SIZE_MAP_X;
								x = _x;
							}
					}
					// 敵キャラの壁当たり判定
					if (CheckHit(i * SIZE_MAP_X, j * SIZE_MAP_Y, SIZE_MAP_X, SIZE_MAP_Y,
						ex, ey, CHARA_WIDTH, CHARA_HEIGHT))
					{
						// 上下から当たっている
						if (ey != old_ey)
						{
							int _ey = std::max(ey, old_ey);
							_ey /= SIZE_MAP_Y;
							_ey *= SIZE_MAP_Y;
							ey = _ey;
							eyadd = 0.0f;
						}
						else
							// 左右から当たっている
							if (ex != old_ex)
							{
								int _ex = std::max(ex, old_ex);
								_ex /= SIZE_MAP_X;
								_ex *= SIZE_MAP_X;
								ex = _ex;
							}
					}
				}
			}
		}

		if (--act_wait <= 0)
		{
			act_index++;
			act_wait = ACT_SPEED;
			act_index %= MAX_MOTION_INDEX;
		}

		int motion_index = act_motion[act_index];

		// 敵キャラを表示する
		if (is_enemy)
		{
			// 敵キャラが自動に移動する　※壁に当たった場合消滅にしたい→最終的には反転させたい
			if (enemy_speed == 0) {
				enemy_speed = 1.0f;
				ex += enemy_speed;
			}
			DrawGraph(ex, ey, enemy_act[motion_index + act_dir * ACT_DIR_SLIDE], TRUE);
		}

		// プレイヤーキャラを表示する
		if (is_chara)
		{
			DrawGraph(x, y, chara_act[motion_index + act_dir * ACT_DIR_SLIDE], TRUE);
		}

		//裏画面を表画面を入れ替える
		ScreenFlip();
	}

	//======================================================
	// DXライブラリ終了とプログラム終了
	//======================================================
	DxLib_End();
	return 0;
}