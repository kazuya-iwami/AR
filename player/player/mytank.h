
#pragma once

#include "object.h"
#include "item.h"
#include "HTTPConnection.h"
#include "network.h"
#include "bullet.h"
#include "utility.h"
#include <memory>

enum OPERATION_STATUS {
	REGULAR,
	STOP
};

enum GAME_STATUS{
    GAME_PLAY,
    GAME_PAUSE
};


//���@�N���X

class CMytank : public CObject,public CNetwork {
	ITEM_KIND item_kind;
	int score;//���Đ�-�팂�Đ�
	int num_bullet; //�c�e
	OPERATION_STATUS ope_status; //����p�X�e�[�^�X
	int ope_timer; //����p�^�C�}�[
	int vel_R,vel_L; //���E�̃^�C���̑��x
	int focus_x,focus_y;//�Ə��̈ʒu
	GAME_STATUS game_status;

	
public:
	shared_ptr<CEnemy> enemy0;
	shared_ptr<CEnemy> enemy1;
	shared_ptr<CEnemy> enemy2;
	shared_ptr<CEnemy> enemy3;

	CMytank();
	void move(tstring direction);//�ړ�
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr,int vl);
	void get_msg();//�T�[�o�[����̃��b�Z�[�W�p
	bool draw();
	void gen_bullet(BULLET_KIND kind);//�e�����֐�
	void detect_enemy(Mat image);
};
