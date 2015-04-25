#include "utility.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

int CEnemy::just_before_shooted;
#define SORT_SPEED 5
#define RANK_HEIGHT 70

bool CSystem_timer::draw(){
	if(system_timer > 0){
		if(countdown_finish_flag){
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
		}
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
		DrawRotaGraph(LEFT_WINDOW_WIDTH+500,375,1,-3.14/4*((countdown_timer/2)%8),figure_id["F_COUNTSIDE"],true);
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
	system_timer = game_time * 30-1;
	finish_flag = false;
	countdown_timer= 5 * 30;
	countdown_finish_flag = false;
}

bool CEnemy::draw(){
	x=ip_x*1000/320;//画面引き延ばしてる分の補正
	y=ip_y*750/240;

	if(countdown_finish_flag && visible){//視界に入っているなら
		if(exist){
			// 敵アイコンの表示
			DrawGraph(x-60 + LEFT_WINDOW_WIDTH, y-40, figure_id["F_ICON"+to_string(enemy_id+1)],true);
			// 前回打った敵は攻撃できない
			if(enemy_id == CEnemy::just_before_shooted){
				DrawGraph(x - 60 + LEFT_WINDOW_WIDTH,y - 40,figure_id["F_X"],true);
			}

			if (enemy_id == CEnemy::just_before_shooted){
				DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "もう撃たないで(´・ω・`)");
			}
			else if (HP == 0) {//死んでるときはlock-onできない
				DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "こいつ死んでるよ(´・ω・`)");	
			}
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
	just_before_shooted = -1; // 直前には誰も撃たれていない
	countdown_finish_flag = false;
}


void CEnemy::disconnect(){
	exist = false;
}

void CEnemy::attacked(int score_){
	//score -= score_;//撃たれてもsvoreは減らない
	if (HP > 0) {//念のため
	HP -= score_; //HPも撃たれたら減ります
	}
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
DrawOriginalString(300,85,2.0,48," player "+to_string(result[0].second)+"\t\t\t\t\t"+to_string(result[0].first));
for(i=1;i<4;i++){
		DrawOriginalString(560,170+100*i,1.0,24," player "+to_string(result[i].second)+"\t\t\t\t\t\t\t"+to_string(result[i].first));
	}
SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

return true;
}


CThunder :: CThunder(){
	draw_timer = 0;
}

bool CThunder :: draw(){
	DrawExtendGraph( LEFT_WINDOW_WIDTH , 0,
				1000 + LEFT_WINDOW_WIDTH  , 750 , thunder[draw_timer % 10], true ) ;

	draw_timer++;

	if(draw_timer <90) return true;
	else return false;
}



bool CMap::draw(){

	DrawGraph(10+LEFT_WINDOW_WIDTH,520,figure_id["F_MAPFRAME"],true);
	DrawGraph(10+LEFT_WINDOW_WIDTH,520,figure_id["F_ICONRED"],true);
	DrawGraph(10+LEFT_WINDOW_WIDTH,720-15,figure_id["F_ICONGREEN"],true);
	DrawGraph(210-15+LEFT_WINDOW_WIDTH,520,figure_id["F_ICONBLUE"],true);
	DrawGraph(210-15+LEFT_WINDOW_WIDTH,720-15,figure_id["F_ICONYELLOW"],true);
	DrawGraph(110-15+LEFT_WINDOW_WIDTH,620-15,figure_id["F_ICONDEAD"],true);
	return true;
}


bool CWait::draw(){
	DrawGraph(0,0,figure_id["F_WAIT06"],false);

	DrawGraph(300,630,figure_id["F_WAIT05"],true);
	DrawGraph(620,660,figure_id["F_WAIT05"],true);

	DrawGraph(draw_timer%1800-350,470,figure_id["F_WAIT02"],true);
	DrawRotaGraph(draw_timer%1800-230,640,1.0,draw_timer/12,figure_id["F_WAIT03"],true);
	DrawRotaGraph(draw_timer%1800-130,640,1.0,draw_timer/12+2,figure_id["F_WAIT03"],true);
	
	int i;
	for(i=1;i<mode;i++){
		DrawGraph((draw_timer-i*339)%1800-350,470,figure_id["F_WAIT02"],true);
		DrawRotaGraph((draw_timer-i*339)%1800-230,640,1.0,draw_timer/12,figure_id["F_WAIT03"],true);
		DrawRotaGraph((draw_timer-i*339)%1800-130,640,1.0,draw_timer/12+2,figure_id["F_WAIT03"],true);
	}
	

	DrawGraph(420,470,figure_id["F_WAIT05"],true);
	DrawGraph(880,450,figure_id["F_WAIT05"],true);

	//以下でライトの点滅パターンを制御しましょう
	switch ((draw_timer/40)%10){
	case 0:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 1:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 2:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 3:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_ILIGHT1"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 4:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_ILIGHT1"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_CLIGHT1"],true);
		break;
	case 5:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 6:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_ILIGHT1"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_CLIGHT1"],true);
		break;
	case 7:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	case 8:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_ELIGHT1"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_ILIGHT1"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_CLIGHT1"],true);
		break;
	case 9:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	default:
		DrawRotaGraph(300,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(500,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(700,100,0.3,0,figure_id["F_LIGHT0"],true);
		DrawRotaGraph(900,100,0.3,0,figure_id["F_LIGHT0"],true);
		break;
	}
	draw_timer++;
	return true;
}

CWait::CWait(){
	draw_timer=0;
	mode=1;
}

bool CKamifubuki::draw(){
	DrawExtendGraph(0,0,1500,750,kamifubuki[(draw_timer%120)/5],true);
	draw_timer++;
	return  true;
}

CKamifubuki::CKamifubuki(){
	draw_timer=0;
}

CScore_info_enemy::CScore_info_enemy(){
	score = 0;


}

CScore_Info::CScore_Info(int mytank_id_){
	mytank_id = mytank_id_;
	for(int i=0;i<4;i++){
		score_info_enemy[i].rank = i;
		score_info_enemy[i].info_y = i*RANK_HEIGHT;
	}
}

void CScore_Info::update_score(int score0,int score1, int score2,int score3){

	score_info_enemy[0].score = score0;
	score_info_enemy[1].score = score1;
	score_info_enemy[2].score = score2;
	score_info_enemy[3].score = score3;

}
bool CScore_Info::draw(){
	//点数ソートしてrankに反映
	vector<pair<int,int>> tmp_rank;
	for(int i=0;i<4;i++){
		tmp_rank.push_back(pair<int,int>(score_info_enemy[i].score,i));
	}
	sort(tmp_rank.begin(), tmp_rank.end(), greater<pair<int, int> >() );
	for(int i=0;i<4;i++){
		score_info_enemy[tmp_rank[i].second].rank = i;
	}

	//毎回rankとinfo_yがずれていると動かす
	for(int i=0;i<4;i++){
		if(abs(score_info_enemy[i].info_y - score_info_enemy[i].rank*RANK_HEIGHT) > SORT_SPEED+1){
			int flag = (score_info_enemy[i].info_y - score_info_enemy[i].rank*RANK_HEIGHT < 0)? 1:-1;
			score_info_enemy[i].info_y += SORT_SPEED * flag;
		}else{
			score_info_enemy[i].info_y = score_info_enemy[i].rank*RANK_HEIGHT;
		}
	}

	SetDrawBlendMode(DX_BLENDMODE_ADD,255);
	//描画
	for(int i=0;i<4;i++){
		DrawOriginalString(900,score_info_enemy[i].info_y + 100,1.0,24,to_string(score_info_enemy[i].score)+"  "+to_string(i)+"P");

	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	return true;


}
