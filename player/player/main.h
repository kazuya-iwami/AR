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