﻿#include "utility.h"


//ゲームの制限時間。適当に調整
int timer=3359;

bool CSystem_timer::draw(){
	//残り時間表示
	SetDrawBlendMode(DX_BLENDMODE_ALPHA,rand()%255);
	DrawFormatString(650,20,GetColor(255,122,0),"Time:%d",timer/10);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	timer--;

	return true;//常に描画
}

bool CEffect::draw(){
	DrawString(100,100,"red",0);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,shaketiemr*10);
	DrawGraph(0,0,figure_id["T_REDBACK"],1);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	if(shaketiemr==0){
		return false;
	}
	return true;
}

CSystem_timer::CSystem_timer(int x_,int y_){
	//ChangeFont("07ロゴたいぷゴシック7");
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x*1000/320;//画面引き延ばしてる分の補正
	y=ip_y*750/240;

	if(visible){//視界に入っているなら
		if(!lockon){
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		}else
		DrawGraph(x - 100,y - 130,figure_id["F_DETECT"],true);
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
    if(visible){
        auto explosion = make_shared<CExplosion>(x , y, EXPLOSION_KIND::EXPLOSION_NOMAL);
        CObject::register_object(explosion,DRAW_LAYER::EXPLOSION_LAYER);
    }
}

bool CBullet_image :: draw(){
	int i;
	for(i=0;i<num_bullet;i++){
		DrawGraph(5,150+(max_bullet_num - 1)*25-25*i,figure_id["F_BULLETNOKORI"],true);	
	}
	for(i=0;i<max_bullet_num - num_bullet;i++){
		DrawGraph(5,150+25*i,figure_id["F_BULLETUSED"],true);
	}
	return true;
}

CBullet_image :: CBullet_image(int x_, int y_, int max_bullet_num_): max_bullet_num(max_bullet_num_){
	x=x_;
	y=y_;
	num_bullet = max_bullet_num_; //残弾補充数は一定
}

//num_bulletを更新する関数
void CBullet_image :: update_num_bullet(int num_bullet_){

	if(num_bullet_ >= 0 && num_bullet_ <= max_bullet_num ){

		num_bullet = num_bullet_;

	}else num_bullet = 0;
}



