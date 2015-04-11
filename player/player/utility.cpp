﻿#include "utility.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>


bool CSystem_timer::draw(){
	if(system_timer > 0){

		//残り時間表示
		//背景出力
		DrawGraph(LEFT_WINDOW_WIDTH+500-75, 20, figure_id["F_TIMER_FRAME"], true);

		//文字出力
		std::ostringstream sout1, sout2;
		sout1 << std::setfill('0') << std::setw(2) << (system_timer/30 + 1)%60;
		std::string sec = sout1.str();
		sout2 << std::setfill('0') << std::setw(2) << (system_timer/30 + 1)/60;
		std::string min = sout2.str();
		DrawOriginalString(438+LEFT_WINDOW_WIDTH, 30, 1.0, 24, min+":"+sec);
		//timerカウント
		system_timer--;
	}else finish_flag = true;

	//残り10秒になったら警告
	if(system_timer<=10*30){
		if(system_timer%30<15){
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,90-3*(system_timer%30));
		}else{
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,3*(system_timer%30));
		}
	DrawGraph(LEFT_WINDOW_WIDTH,0,figure_id["F_REDBACK"],true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	}

	//最初5秒カウントダウン
	if(countdown_timer > 0){
		SetDrawBlendMode(DX_BLENDMODE_ALPHA,200);
		DrawRotaGraph(LEFT_WINDOW_WIDTH+500,375,1,0,figure_id["F_COUNTBASE"],true);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA,140);
		DrawRotaGraph(LEFT_WINDOW_WIDTH+500,375,1,-3.14/15*countdown_timer,figure_id["F_COUNTSIDE"],true);

		DrawOriginalString(LEFT_WINDOW_WIDTH+500-30,325,2.0,0,to_string(countdown_timer/30 + 1));
		countdown_timer--;

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	}else countdown_finish_flag = true;

	return true;//常に描画
}

bool CEffect::draw(){
	DrawString(100 + LEFT_WINDOW_WIDTH,100,"red",0);
	SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,shaketiemr*10);
	DrawGraph(0 + LEFT_WINDOW_WIDTH,0,figure_id["T_REDBACK"],1);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	if(shaketiemr==0){
		return false;
	}
	return true;
}

CSystem_timer::CSystem_timer(int x_,int y_,int game_time){
	//ChangeFont("07ロゴたいぷゴシック7");
	x=x_;
	y=y_;
	system_timer = game_time * 30;
	finish_flag = false;
	countdown_timer= 5 * 30;
	countdown_finish_flag = false;
}

bool CEnemy::draw(){
	x=ip_x*1000/320;//画面引き延ばしてる分の補正
	y=ip_y*750/240;

	if(visible){//視界に入っているなら
		if(exist){
			/*
			if(!lockon){
				SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
			}
			*/
			DrawGraph(x - 100 + LEFT_WINDOW_WIDTH,y - 130,figure_id["F_ICON"+to_string(enemy_id+1)],true);
			//SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		}else{ //切断されていたら
			DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "こいつ死んでるよ(´・ω・`)");
		}
		//DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y , GetColor(255,255,255), "%dP", enemy_id);
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
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 140 );
	for(i=0;i<num_bullet;i++){
		DrawGraph(5 + LEFT_WINDOW_WIDTH,150+(max_bullet_num - 1)*25-25*i,figure_id["F_BULLETNOKORI"],true);	
	}
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 80 );
	for(i=0;i<max_bullet_num - num_bullet;i++){
		DrawGraph(5 + LEFT_WINDOW_WIDTH,150+25*i,figure_id["F_BULLETUSED"],true);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
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


CFire ::CFire(){
	draw_timer = 0;
}

bool CFire::draw(){
	
	DrawExtendGraph( LEFT_WINDOW_WIDTH , 0,
				1000 + LEFT_WINDOW_WIDTH  , 750 , fire[draw_timer % 5], true ) ;

	draw_timer++;


	if(draw_timer <120) return true;
	else return false;
}

CUp_effect :: CUp_effect(){
	draw_timer = 0;
}

bool CUp_effect::draw(){

	DrawGraph(80 + LEFT_WINDOW_WIDTH,200,up_effect[draw_timer],true);
	draw_timer++;

	if(draw_timer < 10) return true;
	else return false;
}

CFinish :: CFinish(int result_[4]){
	int i;
	for(i=0;i<4;i++){
	result.push_back(pair<int, int>(result_[i],i+1));//点数、プレイヤーの順
	}
	sort(result.begin(), result.end(), greater<pair<int, int> >() );
}

bool CFinish::draw(){
DrawGraph(0,0,figure_id["F_FINISH"],false);
int i;

SetDrawBlendMode(DX_BLENDMODE_SUB,200);
DrawOriginalString(300,85,2.0,48,"player "+to_string(result[0].second)+"\t\t"+to_string(result[0].first));
for(i=1;i<4;i++){
		DrawOriginalString(560,170+100*i,1.0,24,"player "+to_string(result[i].second)+"\t\t"+to_string(result[i].first));
	}
SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

return true;
}

/*
CRain :: CRain(){
	draw_timer = 0;
}

bool CRain :: draw(){
	DrawExtendGraph( LEFT_WINDOW_WIDTH , 0,
				1000 + LEFT_WINDOW_WIDTH  , 750 , rain[draw_timer % 24], true ) ;

	draw_timer++;

	if(draw_timer <180) return true;
	else return false;
}
*/


bool CMap::draw(){

	DrawGraph(10+LEFT_WINDOW_WIDTH,520,figure_id["F_MAPFRAME"],true);
	return true;
}

