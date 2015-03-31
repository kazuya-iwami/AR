
#pragma once
#include <map>
#include <list>
#include <string>
#include "DxLib.h"

using namespace std;

// �����G�t�F�N�g�p
extern int explosion[45];
extern int explosion1[140];
extern int bullet[30];

//�`��A���������܂Ƃ߂�N���X
class CObject {
protected:
	int x,y; //�`��ʒu
	int draw_timer; //�A�j���[�V�����p
	static map<string,int> figure_id;//������load�����C���[�W�ɑΉ�����id��A�z�z��Ƃ��ĕۑ�   F_���珑���n�߂�
	static map<string,int> sound_id;//������load�����C���[�W�ɑΉ�����id��A�z�z��Ƃ��ĕۑ�    S_���珑���n�߂�

public:
	/*
		�摜�̕`����s���֐�
		false���Ԃ��Ă�����`��Ώۂ���O��
		���z�֐��Ȃ̂ŁA�q�N���X���Ŋe�X�̎��ۂ̏���������
	*/
	virtual bool draw();

	/*
		�摜�̓o�^���s���֐�
		�����A�߂�l�@���ɂȂ�
		�C�ӂ̕`��摜�͂����ɓo�^������K�v������
	*/
	static void load();//���[�h
};

/* �`����@

	�܂�object.cpp�ŉ摜�A����load�i�������j

	�`���o�^����ۂ͉��̂Q�s�g��
	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//�X�}�[�g�|�C���^�����@�R���X�g���N�^�͊e�w�b�_�t�@�C������
	drawlist.push_back(bullet);//�`�惊�X�g�ɓo�^

	draw()�̒��ł�
	PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;���Đ��@drawtimer==0�̎��̂ݖ炷���̂ɒ��� �Q�߂̈����͂���
	DrawGraph(0,0,figure_id["F_TEST"],false);�@�`��@���߂̂Q������x,y

	//DX���C�u�����ɂ��Ă͉�����
	DX���C�u�����ڍ�
	http://homepage2.nifty.com/natupaji/DxLib/dxlecture_main.html
	DX���C�u�����@�G�t�F�N�g�ɂ��āipdf�j
	http://sherbet.transjiggen.com/ccs/ccs_wiki2/index.php?plugin=attach&pcmd=open&file=DXLib_Effect_h002.pdf&refer=%A5%CF%A5%EB%A5%C8



*/

