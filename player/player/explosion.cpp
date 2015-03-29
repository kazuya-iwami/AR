#include "explosion.h"

CExplosion::CExplosion(int x_,int y_,EXPLOSION_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer=0;
}

bool CExplosion::draw(){
	


	switch(kind){
	case EXPLOSION_KIND::EXPLOSION_NOMAL :
		if(draw_timer != 44){
			// ����m�F�p �����o��
			DrawFormatString(0, 0, GetColor(255,255,255), "explosion:%d", draw_timer);
			// �����G�t�F�N�g�̕`��
			DrawGraph(530, 50, explosion[draw_timer], true);
			draw_timer++;
			return true;
		} else {
			return false;
		}
	case EXPLOSION_KIND::EXPLOSION_1 :
		if(draw_timer != 140){
			// ����m�F�p �����o��
			DrawFormatString(0, 0, GetColor(255,255,255), "explosion:%d", draw_timer);
			// �����G�t�F�N�g�̕`��
			DrawGraph(530, 50, explosion1[draw_timer], true);
			draw_timer++;
			return true;
		} else {
			return false;
		}
	}
	return false;
}
