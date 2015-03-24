
#pragma once
#include <map>
#include <list>
#include "DxLib.h"
#include <string>

using namespace std;

class CDrawable {
protected:
	int x,y;
	int draw_timer;//-1�ŏ펞�`��
	static map<string,int> figure_id;//������load�����C���[�W�ɑΉ�����id��A�z�z��Ƃ��ĕۑ�

public:
	virtual bool draw();//false���Ԃ��Ă�����`��Ώۂ���O��
	static void load();
};
