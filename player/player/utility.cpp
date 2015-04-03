#include "utility.h"

//ゲームの制限時間。適当に調整
int timer=3359;

extern int score;
extern int cursur_x;
extern int cursur_y;

bool CSystem_timer::draw(){
	//残り時間表示
	DrawGraph(760,20,number[timer/30/100],true);
	DrawGraph(820,20,number[(timer/30%100)/10],true);
	DrawGraph(880,20,number[timer/30%10],true);

	timer--;

	//スコア表示
	DrawGraph(200,650,number[score/10],true);
	DrawGraph(260,650,number[score%10],true);

	//アイテム枠表示
	DrawGraph(0,0,figure_id["F_FRAME"],true);

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

bool CCursur::draw(){
	DrawGraph(cursur_x,cursur_y,figure_id["F_CURSUR"],true);
	return true;
}

CEnemy::CEnemy(){
	score = 0;
}
