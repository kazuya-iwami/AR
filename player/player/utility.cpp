#include "utility.h"

bool CSystem_timer::draw(){


	return true;//常に描画
}

CSystem_timer::CSystem_timer(int x_,int y_){
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x;
	y=ip_y;

	if(visible){//視界に入っているなら
		DrawGraph(x*1000/320 - 100,y*750/240 - 130,figure_id["F_DETECT"],true);//画面引き延ばしてる分の補正
	}


	return true;
}