
#pragma once

class CDrawable {
	int x,y;
	int draw_timer;//-1�ŏ펞�`��

public:
	virtual bool draw();//false���Ԃ��Ă�����`��Ώۂ���O��
};