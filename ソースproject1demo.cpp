#include <algorithm> // std::max���g�p���邽��
#define NOMINMAX
#include "DxLib.h"

//
// �����蔻����s���֐�
// 
// �΂̃L���� ��   x1
// �@�@�@�@�@ ��   y1
// �@�@�@�@�@ ���� w1
// �@�@�@�@�@ �c�� h1
// ���̃L���� ��   x2
// �@�@�@�@�@ ��   y2
// �@�@�@�@�@ ���� w2
// �@�@�@�@�@ �c�� h2
//
int CheckHit(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int L1 = x1;      // ��
	int R1 = x1 + w1; // �E(��+����)
	int L2 = x2;      // ��
	int R2 = x2 + w2; // �E(��+����)

	if (R1 <= L2) return 0; //< �����������Ȃ����Ƃɂ���
	if (R2 <= L1) return 0; //< �����������Ȃ����Ƃɂ���

	int U1 = y1;      // ��
	int D1 = y1 + h1; // ��(��+�c��)
	int U2 = y2;      // ��
	int D2 = y2 + h2; // ��(��+�c��)

	if (D1 <= U2) return 0; //< �����������Ȃ����Ƃɂ���
	if (D2 <= U1) return 0; //< �����������Ȃ����Ƃɂ���

	// ����ȊO�̏ꍇ�͓������Ă���
	return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//======================================================
	// �E�C���h�E�ݒ��DX���C�u�����̏�����
	//======================================================
	const int SCREEN_WIDTH = 640;		//< �X�N���[�����̑傫��
	const int SCREEN_HEIGHT = 480;		//< �X�N���[���c�̑傫��

	// Windows���[�h�ɂ���
	ChangeWindowMode(TRUE);
	// ��ʃT�C�Y�̐ݒ�
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
	// DX���C�u����������
	if (DxLib_Init() == -1)
	{
		//�������Ɏ��s
		return -1;
	}
	// �_�u���o�b�t�@���[�h
	SetDrawScreen(DX_SCREEN_BACK);
	//======================================================
	// �Q�[������
	//======================================================
	//------------------------------------------------------
	// �Q�[������������
	//------------------------------------------------------
	const int CHARA_WIDTH = 32;
	const int CHARA_HEIGHT = 32;

	//< ���C���L�����̈ʒu�A��ʂ̒��S
	float x = (SCREEN_WIDTH - CHARA_WIDTH) / 2;	
	float y = (SCREEN_HEIGHT - CHARA_HEIGHT) / 2;
	int is_chara = 1; // �t���O��p��(�����͕\�����邽�߂�1)

	float ex = (SCREEN_WIDTH - CHARA_WIDTH) / 2;		//�G�L�����̈ʒu
	float ey = 300;		//�G�L�����̈ʒu
	int is_enemy = 1; // �t���O��p��(�����͕\�����邽�߂�1)

	// �e�e(shot)��ł�
	int shot_x = 470;
	int shot_y = 370;
	int is_shot = 0;  // �t���O��p��

	float yadd = 0.0f;
	float eyadd = 0.0f;

	int chara_act[12];		//�v���C���[�L�������[�h
	LoadDivGraph("demo/sample007.png", 12, 3, 4, 32, 32, chara_act);
	int enemy_act[12];		//�G�L�������[�h
	LoadDivGraph("demo/sample008.png", 12, 3, 4, 32, 32, enemy_act);

	int act_motion[] = { 0, 1, 2, 1, };
	int act_index = 0;

	int MAX_MOTION_INDEX = 4; //< 0,1,2,1 ��4��� [0�`3]�܂łɂ��邽�߂ɗp��

	const int ACT_SPEED = 20;
	int act_wait = ACT_SPEED;

	const int ACT_DIR_SLIDE = 3; //< ���̃L�������X���C�h������Ύ��̗��I������
	int act_dir = 0;

	// �}�b�v�`�b�v���[�h
	const int SIZE_MAP_X = 32;  //< �}�b�v�`�b�vX�T�C�Y
	const int SIZE_MAP_Y = 32;	//< �}�b�v�`�b�vY�T�C�Y
	const int NUM_CHIP_X = 10;  //< ��10���
	const int NUM_CHIP_Y = 4;   //< �c4���
	const int NUM_CHIP_ALL = NUM_CHIP_X * NUM_CHIP_Y; //< 40��
	int chip[NUM_CHIP_ALL];
	LoadDivGraph("demo/mapchip.png", NUM_CHIP_ALL,
		NUM_CHIP_X, NUM_CHIP_Y,
		SIZE_MAP_X, SIZE_MAP_Y,
		chip);

	// �}�b�v�`�b�v���g�����w�i
	const int NUM_MAP_X = 20; // 32x20=640
	const int NUM_MAP_Y = 15; // 32x15=480
	int data[] = {
		// ��20x15
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
	// ���C�����[�v����
	//------------------------------------------------------
	while (ProcessMessage() == 0)
	{
		//����ʏ���
		ClearDrawScreen();

		// �w�i
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

		// �O��̈ʒu���o���Ă���
		float old_x = x;
		float old_y = y;
		float speed = 3.0f;
		// �G�L�����̑O��̈ʒu
		float old_ex = ex;
		float old_ey = ey;
		float enemy_speed = 0.0f;

		// �L�[�����擾����
		int input = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		// �L�[�ňړ�������
		if (input & PAD_INPUT_RIGHT)
		{
			x += speed;
			act_dir = 2; //< 2��ڂ�I��(0�X�^�[�g)
		}
		if (input & PAD_INPUT_LEFT)
		{
			x -= speed;
			act_dir = 1; //< 1��ڂ�I��(0�X�^�[�g)
		}
		if (input & PAD_INPUT_UP) //< �L�[�{�[�h�Ȃ�Ώ�{�^�� 
		{
			yadd = -5.0f;
		}
		if (is_shot == 0)
		{
			if (input & PAD_INPUT_A)	//< �L�[�{�[�h�Ȃ��Z�{�^��
			{
				is_shot = 1;
				shot_x = x + 15;
				shot_y = y + 15;
			}
		}
		if (is_shot == 1)
		{
			// �E�������Ă���Ƃ��ɏe�e�𔭎˂����ꍇ
			if (act_dir == 2) {
				shot_x += 8;

				// �e�e�̓����蔻��
				if (is_enemy == 1)
				{
					int hit = CheckHit(ex, ey, 30, 30, shot_x - 5, shot_y - 5, 20, 20);
					if (hit == 1)
					{
						// �������Ă���̂ŉ�ʂɏo���̂Ɠ������Ƃ�����
						is_shot = 0;
						is_enemy = 0;
					}
				}
				DrawCircle(shot_x, shot_y, 10, GetColor(255, 255, 255), TRUE);
			}
			// ���������Ă���Ƃ��ɏe�e�𔭎˂����ꍇ
			if (act_dir == 1) {
				shot_x -= 8;

				// �e�e�̓����蔻��
				if (is_enemy == 1)
				{
					int hit_shottoenemy = CheckHit(ex, ey, 30, 30, shot_x - 5, shot_y - 5, 20, 20);
					if (hit_shottoenemy == 1)
					{
						// �������Ă���̂ŉ�ʂɏo���̂Ɠ������Ƃ�����
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
		// �v���C���[�L�����ƓG�L�����̓����蔻��
		if (is_enemy == 1)
		{
			int hit_charatoenemy = CheckHit(ex, ey, 30, 30, x - 5, y - 5, 32, 32);
			if (hit_charatoenemy == 1)
			{
				// �������Ă���̂ŉ�ʂɏo���̂Ɠ������Ƃ�����
				is_chara = 0;
				is_enemy = 0;
			}
		}

		y += yadd;
		ey += eyadd;
		yadd += 0.2f;
		eyadd += 0.2f;

		//if (yadd > 30) yadd = 30; //< �}�X��΂��Ȃ��悤�ɐ���

		// �u���b�N�̓����蔻��
		for (int j = 0; j < NUM_MAP_Y; j++)
		{
			for (int i = 0; i < NUM_MAP_X; i++)
			{
				int no = data[i + j * NUM_MAP_X];
				if (no >= 10 && no < 20)
				{
					// �v���C���[�L�����̕Ǔ����蔻��
					if (CheckHit(i * SIZE_MAP_X, j * SIZE_MAP_Y, SIZE_MAP_X, SIZE_MAP_Y,
						x, y, CHARA_WIDTH, CHARA_HEIGHT))
					{
						// �㉺���瓖�����Ă���
						if (y != old_y)
						{
							int _y = std::max(y, old_y);
							_y /= SIZE_MAP_Y;
							_y *= SIZE_MAP_Y;
							y = _y;
							yadd = 0.0f;
						}
						else
							// ���E���瓖�����Ă���
							if (x != old_x)
							{
								int _x = std::max(x, old_x);
								_x /= SIZE_MAP_X;
								_x *= SIZE_MAP_X;
								x = _x;
							}
					}
					// �G�L�����̕Ǔ����蔻��
					if (CheckHit(i * SIZE_MAP_X, j * SIZE_MAP_Y, SIZE_MAP_X, SIZE_MAP_Y,
						ex, ey, CHARA_WIDTH, CHARA_HEIGHT))
					{
						// �㉺���瓖�����Ă���
						if (ey != old_ey)
						{
							int _ey = std::max(ey, old_ey);
							_ey /= SIZE_MAP_Y;
							_ey *= SIZE_MAP_Y;
							ey = _ey;
							eyadd = 0.0f;
						}
						else
							// ���E���瓖�����Ă���
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

		// �G�L������\������
		if (is_enemy)
		{
			// �G�L�����������Ɉړ�����@���ǂɓ��������ꍇ���łɂ��������ŏI�I�ɂ͔��]��������
			if (enemy_speed == 0) {
				enemy_speed = 1.0f;
				ex += enemy_speed;
			}
			DrawGraph(ex, ey, enemy_act[motion_index + act_dir * ACT_DIR_SLIDE], TRUE);
		}

		// �v���C���[�L������\������
		if (is_chara)
		{
			DrawGraph(x, y, chara_act[motion_index + act_dir * ACT_DIR_SLIDE], TRUE);
		}

		//����ʂ�\��ʂ����ւ���
		ScreenFlip();
	}

	//======================================================
	// DX���C�u�����I���ƃv���O�����I��
	//======================================================
	DxLib_End();
	return 0;
}