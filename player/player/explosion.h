#pragma once

#include "object.h"

//�����N���X

enum EXPLOSION_KIND {
	EXPLOSION_NOMAL
};

class CExplosion : public CObject {
	EXPLOSION_KIND kind;
	
public:
	/*
		�R���X�g���N�^�i�������j
		�����F
			int x_	�����G�t�F�N�g�̉������W
			int y_	�����G�t�F�N�g�̏c�����W
			EXPLOSION_KIND kind_	�����̎�� 
		�߂�l�F
			�Ȃ�
	*/
	CExplosion(int x_,int y_,EXPLOSION_KIND kind_);

	/*
		�����G�t�F�N�g�̕`����s���֐�
		�����F
			�Ȃ�
		�߂�l�F
			true �G�t�F�N�g�`�撆
			false �G�t�F�N�g�I����
	*/
	bool draw();
};