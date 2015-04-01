#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//�O���[�o���ϐ�

std::list<std::shared_ptr<CObject>> drawlist;

auto enemy1 = make_shared<CEnemy>(); //�X�}�[�g�|�C���^�ɔz�񂪎�������Ă��Ȃ�����
auto enemy2 = make_shared<CEnemy>();
auto enemy3 = make_shared<CEnemy>();

auto network = make_shared<CNetwork>();

int score=0;
int cursur_x=500;
int cursur_y=350;

enum COMMAND_NAME{
	USE_ITEM,
	SHOOT_BULLET,
	FINISH_ITEM,
};

string encode(COMMAND_NAME, int, int, int);