
#pragma once
#include <map>
#include <list>
#include "DxLib.h"
#include <string>

using namespace std;

class CObject {
protected:
	int x,y;
	int draw_timer;//-1�ŏ펞�`��
	static map<string,int> figure_id;//������load�����C���[�W�ɑΉ�����id��A�z�z��Ƃ��ĕۑ�   F_���珑���n�߂�
	static map<string,int> sound_id;//������load�����C���[�W�ɑΉ�����id��A�z�z��Ƃ��ĕۑ�    S_���珑���n�߂�

public:
	virtual bool draw();//false���Ԃ��Ă�����`��Ώۂ���O��
	static void load();
};

/* �`����@

	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//�X�}�[�g�|�C���^����
	drawlist.push_back(bullet);//�`�惊�X�g�ɓo�^

*/

