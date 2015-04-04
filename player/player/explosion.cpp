#include "explosion.h"

CExplosion::CExplosion(int x_,int y_,EXPLOSION_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer=0;
}

bool CExplosion::draw(){
	
	// ����m�F�p �����o��
	//DrawFormatString(0, 10, GetColor(255,255,255), "explosion:%d", draw_timer);

	switch(kind){
	case EXPLOSION_KIND::EXPLOSION_NOMAL :
		if(draw_timer != 44){
			// �����G�t�F�N�g�̕`��
			DrawGraph(x, y, explosion[draw_timer], true);
			draw_timer++;
			return true;
		} else {
			return false;
		}
	case EXPLOSION_KIND::EXPLOSION_1 :
		if(draw_timer != 140){
			// �����G�t�F�N�g�̕`��
			DrawGraph(x, y, explosion1[draw_timer], true);
			draw_timer++;
			return true;
		} else {
			return false;
		}
	}
	return false;
}
