#include "utility.h"
#include <string>
#include <sstream>
#include <iomanip>


bool CSystem_timer::draw(){
	//残り時間表示
	if(system_timer > 0){
		SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,230);

		DrawGraph(LEFT_WINDOW_WIDTH+500-87, 20, figure_id["F_TIMER_BASE2"], true); //背景出力
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		std::ostringstream sout;
		sout << std::setfill('0') << std::setw(2) << (system_timer/30 + 1)%60;
		std::string s = sout.str();
		DrawOriginalString(440+LEFT_WINDOW_WIDTH, 35, 1.0, 24, to_string((system_timer/30 + 1)/60)+":"+s); //文字出力
		system_timer--;
	}else finish_flag = true;


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
}

bool CEnemy::draw(){
	x=ip_x*1000/320;//画面引き延ばしてる分の補正
	y=ip_y*750/240;

	if(visible){//視界に入っているなら
		if(exist){
			if(!lockon){
				SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
			}
			DrawGraph(x - 100 + LEFT_WINDOW_WIDTH,y - 130,figure_id["F_DETECT"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		}else{ //切断されていたら
			DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "こいつ死んでるよ(´・ω・`)");
		}
		DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y , GetColor(255,255,255), "%dP", enemy_id);
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


bool CFinish::draw(){
DrawGraph(0,0,figure_id["F_FINISH"],false);

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