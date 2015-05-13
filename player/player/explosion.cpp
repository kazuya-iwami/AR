#include "explosion.h"

CExplosion::CExplosion(int x_,int y_,EXPLOSION_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer=0;
}

bool CExplosion::draw(){
	
	// 動作確認用 文字出力
	//DrawFormatString(0, 10, GetColor(255,255,255), "explosion:%d", draw_timer);

	switch(kind){
	case EXPLOSION_KIND::EXPLOSION_NOMAL :
		if(draw_timer == 0){
			 PlaySoundMem( sound_id["S_EXPLOSION"] , DX_PLAYTYPE_BACK );
		}
		if(draw_timer != 44){
			// 爆発エフェクトの描画
			DrawGraph(x-300 + LEFT_WINDOW_WIDTH, y-300, explosion[draw_timer], true);
			draw_timer++;
			return true;
		} else {
			return false;
		}
	}
	return false;
}
