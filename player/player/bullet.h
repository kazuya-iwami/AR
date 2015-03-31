
#pragma once

#include "object.h"

//�e�N���X

enum BULLET_KIND{
	BULLET_NOMAL,
	BULLET_3D
};

class CBullet : public CObject{
	BULLET_KIND kind;
	int z; //bullet�݂̂������s���̍��W
	

public:
	/*
		�R���X�g���N�^�i�������j
		�����F
			int x_	�e�G�t�F�N�g��x���W
			int y_	�e�G�t�F�N�g��y���W
			int z_	�e�G�t�F�N�g��z���W
			BULLET_KIND kind_	�e�G�t�F�N�g�̎�� 
		�߂�l�F
			�Ȃ�
	*/
	CBullet(int x_, int y_, int z_, BULLET_KIND kind_);

	/*
		�e�G�t�F�N�g�̕`����s���֐�
		�����F
			�Ȃ�
		�߂�l�F
			true �G�t�F�N�g�`�撆
			false �G�t�F�N�g�I����
	*/
	bool draw();
};