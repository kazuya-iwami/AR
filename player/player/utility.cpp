#include "utility.h"

//ゲームの制限時間。適当に調整
int timer=3359;

bool CSystem_timer::draw(){
	//残り時間表示
	DrawGraph(760,20,number[timer/30/100],true);
	DrawGraph(820,20,number[(timer/30%100)/10],true);
	DrawGraph(880,20,number[timer/30%10],true);

	timer--;

	return true;//常に描画
}

CSystem_timer::CSystem_timer(int x_,int y_){
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x*1000/320;
	y=ip_y*750/240;

	if(visible){//視界に入っているなら
		if(!lockon){
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		}else
		DrawGraph(x - 100,y - 130,figure_id["F_DETECT"],true);//画面引き延ばしてる分の補正
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		DrawFormatString(x - 50 ,y , GetColor(255,255,255), "%dP", enemy_id);
	}


	return true;
}


CEnemy::CEnemy(int enemy_id_){
	score = 0;
	exist=true;
	enemy_id = enemy_id_;
	lockon = false;
}


void CEnemy::disconnect(){
	exist = false;
}

void CEnemy::attacked(int score_){
	score += score_;
	auto explosion = make_shared<CExplosion>(x , y, EXPLOSION_KIND::EXPLOSION_NOMAL);
    CObject::register_object(explosion);
}