
#pragma once
#include "object.h"

//�A�C�e���N���X
enum ITEM_KIND{
	ITEM_NONE,
	STAR,
	THUNDER
};

class CItem : public CObject{
	ITEM_KIND kind;
	
public:
	/*
		�R���X�g���N�^�i�������j
		�����F
			int x_	�A�C�e���\����ʂ̉������W
			int y_	�A�C�e���\����ʂ̏c�����W
			ITEM_KIND kind_	�A�C�e���̎�� 
		�߂�l�F
			�Ȃ�
	*/
	CItem(int x_,int y_,ITEM_KIND kind_);

	/*
		�A�C�e���\����ʂ̕`����s���֐�
		�����F
			�Ȃ�
		�߂�l�F
			true �G�t�F�N�g�`�撆
			false �G�t�F�N�g�I����
	*/
	bool draw();
};