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
		break;
	}


	draw_timer++;

	if(draw_timer != 44){
		// 動作確認用 文字出力
		DrawFormatString(0, 0, GetColor(255,255,255), "explosion:%d", draw_timer);

		DrawGraph(530, 50, figure_id["F_EXPLOSION" + std::to_string(draw_timer)], true);
		return true;
	} else {
		return false;
	}
}
