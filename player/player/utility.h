
#pragma once
#include "object.h"
#include "image_processing.h"

//�ׂ����N���X�͂����ɂ����W�߂Ă���

//�c�莞�ԕ`��N���X
class CSystem_timer : public CObject {

public:
	CSystem_timer(int x_,int y_);
	bool draw();
};

//�G�̈ʒu���Ǘ��N���X
class CEnemy : public CImage_processer,public CObject {

public:
	int score;

	CEnemy();
	int get_x(){return x;}
	int get_y(){return y;}
	bool draw();
};

//�J�[�\���N���X
class CCursur:public CObject{
public:
	bool draw();
};

