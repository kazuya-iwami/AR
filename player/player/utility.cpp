#include "utility.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

int CEnemy::just_before_shooted;
#define SORT_SPEED 5
#define RANK_HEIGHT 40

#define DENKYU_HEIGHT 120
#define MAP_PARAM 2.0

bool CSystem_timer::draw(){
	if(system_timer > 0){
		//背景出力
		//DrawGraph(LEFT_WINDOW_WIDTH+500-75, 20, figure_id["F_TIMER_FRAME"], true);


		//文字出力
		std::ostringstream sout0, sout1, sout2;
		sout0 << std::setfill('0') << std::setw(2) << (((system_timer)%30)*100)/30;
		std::string under_sec = sout0.str();
		sout1 << std::setfill('0') << std::setw(2) << (system_timer/30 + 1)%60;
		std::string sec = sout1.str();
		sout2 << std::setfill('0') << std::setw(1) << (system_timer/30 + 1)/60;
		std::string min = sout2.str();
		DrawDigitNum(438+LEFT_WINDOW_WIDTH, 20, 0.3, 18, min+":"+sec+"."+under_sec);
		//timerカウント

		if(countdown_finish_flag)system_timer--;
		
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
	system_timer = (game_time -1 ) * 30;
	finish_flag = false;
	countdown_timer= 3 * 30;
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

			if (VIABILITY_STATUS::DEAD == viability_status) {//死んでるときはlock-onできない
				DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "こいつ死んでるよ(´・ω・`)");	
			}
			else if (enemy_id == CEnemy::just_before_shooted){
				DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "もう撃たないで(´・ω・`)");
			}

		}else{ //切断されていたら
			DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "こいつ死んでるよ(´・ω・`)");
		}
	}
	// 視界に入っていないなら敵の位置情報をリセット
	else if(countdown_finish_flag && !visible) x = y = -100;

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
	//score -= score_;//撃たれてもscoreは減らない
	if (HP > 0) {//念のため
	HP -= score_; //HPは撃たれたら減ります
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


CFinish :: CFinish(vector<pair<int,int> > result_){
	result = result_;
}


bool CFinish::draw(){
DrawGraph(0,0,figure_id["F_FINISH"],false);
int i;

SetDrawBlendMode(DX_BLENDMODE_SUB,200);
DrawOriginalString(300,85,2.0,48," player "+to_string(result[0].second+1)+"\t\t\t\t\t"+to_string(result[0].first));
for(i=1;i<4;i++){
		DrawOriginalString(560,170+100*i,1.0,24," player "+to_string(result[i].second+1)+"\t\t\t\t\t\t\t"+to_string(result[i].first));
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

bool CEeic::draw(){
	/*
	if(denkyu[0].hit){ 
		DrawGraph(LEFT_WINDOW_WIDTH+600,600,figure_id["F_MINILIGHT_E"],true); 
		DrawGraph(LEFT_WINDOW_WIDTH+600,590,figure_id["F_MINILIGHT_TOP"],true);
	}else{
		DrawGraph(LEFT_WINDOW_WIDTH+600,600,figure_id["F_MINILIGHT_0"],true);
	}
	if(denkyu[1].hit){
		DrawGraph(LEFT_WINDOW_WIDTH+700,600,figure_id["F_MINILIGHT_E"],true);	
		DrawGraph(LEFT_WINDOW_WIDTH+700,590,figure_id["F_MINILIGHT_TOP"],true);
	}else{
		DrawGraph(LEFT_WINDOW_WIDTH+700,600,figure_id["F_MINILIGHT_0"],true);
	}
	if(denkyu[2].hit){
		DrawGraph(LEFT_WINDOW_WIDTH+800,600,figure_id["F_MINILIGHT_I"],true);		
		DrawGraph(LEFT_WINDOW_WIDTH+800,590,figure_id["F_MINILIGHT_TOP"],true);
	}else{
		DrawGraph(LEFT_WINDOW_WIDTH+800,600,figure_id["F_MINILIGHT_0"],true);
	}
	*/
	return true;
}

void CEeic::detect(Mat image){
		for(int i = 0;i<3;i++){
		denkyu[i].detect(image);
	}
}

CEeic::CEeic(){
	for(int i=0;i<3;i++){
		denkyu[i].hit=false;
		denkyu[i].denkyu_id = i;
		denkyu[i].lockon=false;
	}

	denkyu[0].init(0,30,100,200,100,200);
	denkyu[1].init(100,150,100,200,100,200);
	denkyu[2].init(150,200,100,200,100,200);
}

CMap::CMap(){
	for(int i=0;i<4;i++){
		player_x[i]=0;
		player_y[i]=0;
	}
}

void CMap::update_pos(int p0_x,int p0_y,int p1_x, int p1_y,int p2_x,int p2_y,int p3_x,int p3_y){
	player_x[0]=(int)(p0_x*MAP_PARAM);
	player_y[0]=(int)(p0_y*MAP_PARAM);
	player_x[1]=(int)(p1_x*MAP_PARAM);
	player_y[1]=(int)(p1_y*MAP_PARAM);
	player_x[2]=(int)(p2_x*MAP_PARAM);
	player_y[2]=(int)(p2_y*MAP_PARAM);
	player_x[3]=(int)(p3_x*MAP_PARAM);
	player_y[4]=(int)(p3_y*MAP_PARAM);

}
bool CMap::draw(){
	/*
	DrawGraph(10+LEFT_WINDOW_WIDTH,520,figure_id["F_MAPFRAME"],true);
	DrawGraph(15+player_x[0]+LEFT_WINDOW_WIDTH,520+player_y[0],figure_id["F_ICONRED"],true);
	DrawGraph(15+player_x[1]+LEFT_WINDOW_WIDTH,520+player_y[1],figure_id["F_ICONGREEN"],true);
	DrawGraph(15+player_x[2]+LEFT_WINDOW_WIDTH,520+player_y[2],figure_id["F_ICONBLUE"],true);
	DrawGraph(15+player_x[3]+LEFT_WINDOW_WIDTH,520+player_y[3],figure_id["F_ICONYELLOW"],true);
	DrawGraph(110-15+LEFT_WINDOW_WIDTH,620-15,figure_id["F_ICONDEAD"],true);
	*/
	return true;
}

float setposition(int n,int i){
	if(n>i*5){
			return 400-(n-i*5)*(n-i*5)*(n-i*5)/20.0;
		}else{
			return 400;
		}
}

//消えゆくタイトルを表示,流れるタイトル
void Drawtitle(int n){
	int wordstart=400;
	for(int i =0;i<14;i++){
		DrawGraph(setposition(n,i)	
		,310,title[i],true);
	}
	/*	DrawGraph(wordstart-n,360,title[1],true);
		DrawGraph(wordstart-n,360,title[2],true);
		DrawGraph(wordstart-n,360,title[3],true);
		DrawGraph(wordstart-n,360,title[4],true);
		DrawGraph(wordstart-n,360,title[5],true);
		DrawGraph(wordstart-n,360,title[6],true);
		DrawGraph(wordstart-n,360,title[7],true);
		DrawGraph(wordstart-n,360,title[8],true);
		DrawGraph(wordstart-n,360,title[9],true);
		DrawGraph(wordstart-n,360,title[10],true);
		DrawGraph(wordstart-n,360,title[11],true);
		DrawGraph(wordstart-n,360,title[12],true);
		DrawGraph(wordstart-n,360,title[13],true);*/
	
}


bool CWait::draw(){
	DrawGraph(0,0,figure_id["F_BACKGROUND_WAIT"],false);
	//DrawBox(0,0,1350,730,GetColor(70,70,70),TRUE);
	
	int wordwidth=48;
	int wordstart=435;
	int wordy=300;
//	タイトル表示
	if(mode>0){
		SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,((draw_timer%80-40)*(draw_timer%80-40))/5);
		DrawGraph(wordstart,wordy,figure_id["F_TITLE"],true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		DrawGraph(wordstart+500,wordy+125,figure_id["F_CONNECT"],true);
		if(draw_timer%160>40){
			DrawGraph(wordstart+740,wordy+135,figure_id["F_DOTGRAY"],true);
			if(draw_timer%160>80){
				DrawGraph(wordstart+760,wordy+135,figure_id["F_DOTGRAY"],true);
				if(draw_timer%160>120){
					DrawGraph(wordstart+780,wordy+135,figure_id["F_DOTGRAY"],true);
				}
			}
		}
		MV1SetWireFrameDrawFlag(figure_id["X_TANK"],true);
		MV1SetScale(figure_id["X_TANK"],VGet(5.0f,5.0f,5.0f));
		MV1SetPosition(figure_id["X_TANK"],VGet(180.0f,50.0f,150.0f));
		MV1SetRotationXYZ(figure_id["X_TANK"],VGet(0.0f,spin*draw_timer/10.0f,0.0f));
		MV1DrawModel(figure_id["X_TANK"]);
	}else{//Pでモード変更
		if(flag<250){
			flag++;
			Drawtitle(flag);
			//ムービー前に戦車にはきえてもらう
			MV1SetWireFrameDrawFlag(figure_id["X_TANK"],true);
			MV1SetScale(figure_id["X_TANK"],VGet(5.0f,5.0f,5.0f));
			if(flag>100){
				MV1SetPosition(figure_id["X_TANK"],VGet(180.0f,(50+((flag-100)*(flag-100))/10)*1.0f,150.0f));
				MV1SetRotationXYZ(figure_id["X_TANK"],VGet(0.0f,spin*(draw_timer+((flag-100)*(flag-100))/4)/10.0*1.0f,0.0f));
			}else{
				MV1SetPosition(figure_id["X_TANK"],VGet(180.0f,(50)*1.0f,150.0f));
				MV1SetRotationXYZ(figure_id["X_TANK"],VGet(0.0f,spin*draw_timer/10.0f,0.0f));
			}
			MV1DrawModel(figure_id["X_TANK"]);
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,255-flag);
			DrawGraph(wordstart+500,wordy+115,figure_id["F_CONNECTED"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);	
		}else{//画面から物体がすべて消えた状態
			DrawFormatString(0,0,1,"%d %d",mode,flag);
		}
	}
	if(spin>1 && draw_timer%60==0){spin--;}
	//DrawFormatString(0,0,1,"%d %d",mode,flag);
	draw_timer++;
	return true;
}
//以下のコメントアウト部分はdraw()で使わなくなった部分。邪魔なので関数外に出しました。必要に応じて戻してください
/*	DrawGraph(wordstart,wordy,figure_id["F_R"],true);
	DrawGraph(wordstart+1*wordwidth,wordy,figure_id["F_E"],true);
	DrawGraph(wordstart+2*wordwidth,wordy,figure_id["F_A"],true);
	DrawGraph(wordstart+3*wordwidth,wordy,figure_id["F_L"],true);
	DrawGraph(wordstart+5*wordwidth,wordy,figure_id["F_T"],true);
	DrawGraph(wordstart+6*wordwidth,wordy,figure_id["F_A"],true);
	DrawGraph(wordstart+7*wordwidth,wordy,figure_id["F_N"],true);
	DrawGraph(wordstart+8*wordwidth,wordy,figure_id["F_K"],true);
	DrawGraph(wordstart+10*wordwidth,wordy,figure_id["F_B"],true);
	DrawGraph(wordstart+11*wordwidth,wordy,figure_id["F_A"],true);
	DrawGraph(wordstart+12*wordwidth,wordy,figure_id["F_T"],true);
	DrawGraph(wordstart+13*wordwidth,wordy,figure_id["F_T"],true);
	DrawGraph(wordstart+14*wordwidth,wordy,figure_id["F_L"],true);
	DrawGraph(wordstart+15*wordwidth,wordy,figure_id["F_E"],true);
	*/
	//DrawGraph(300,630,figure_id["F_WAIT05"],true);
	//DrawGraph(620,660,figure_id["F_WAIT05"],true);

	/*
	for(int i=0;i<mode;i++){
		DrawGraph((draw_timer-i*360)%1800-350,470,figure_id["F_WAIT02"],true);
		DrawRotaGraph((draw_timer-i*360)%1800-230,640,1.0,draw_timer/12,figure_id["F_WAIT03"],true);
		DrawRotaGraph((draw_timer-i*360)%1800-130,640,1.0,draw_timer/12+2,figure_id["F_WAIT03"],true);
	}
	*/
	//DrawGraph(420,470,figure_id["F_WAIT05"],true);
	//DrawGraph(880,450,figure_id["F_WAIT05"],true);

	//以下でライトの点滅パターンを制御しましょう
	/*
	switch ((draw_timer/40)%10){
	case 0:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		break;
	case 1:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		break;
	case 2:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		break;
	case 3:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_0"],true);
		break;
	case 4:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		break;
	case 5:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		DrawGraph(300,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(500,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(700,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(900,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		break;
	case 6:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		break;
	case 7:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		DrawGraph(300,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(500,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(700,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(900,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		break;
	case 8:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);

		break;
	case 9:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		DrawGraph(300,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(500,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(700,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		DrawGraph(900,DENKYU_HEIGHT-50,figure_id["F_LIGHT_TOP"],true);
		break;
	default:
		DrawGraph(300,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(500,DENKYU_HEIGHT,figure_id["F_LIGHT_E"],true);
		DrawGraph(700,DENKYU_HEIGHT,figure_id["F_LIGHT_I"],true);
		DrawGraph(900,DENKYU_HEIGHT,figure_id["F_LIGHT_C"],true);
		
		break;
	}
	*/

	/*if(mode<=0){
		if(speed == 4){
			DrawGraph(x-220,y,figure_id["F_WAIT02"],true);
			DrawRotaGraph(x-100,y+170,1.0,draw_timer/12,figure_id["F_WAIT03"],true);
			DrawRotaGraph(x,y+170,1.0,draw_timer/12+2,figure_id["F_WAIT03"],true);
		}else DrawGraph(x-220,y+5,figure_id["F_WAIT01"],true);
		if(bullet) DrawRotaGraph(bullet_x,bullet_y,0.7,draw_timer/6+1,figure_id["F_WAIT03"],true);
		if(mode<0 && mode > -60){
			DrawExtendGraph(0,0,1349,729,kamifubuki[((-mode)%120)/5],true);
			DrawOriginalString(200,500,3,100,"Perfect!!");
		}
		if(mode < -60){
			DrawExtendGraph(0,0,1349,729,fire[(-mode)%5],true);
			DrawOriginalString(170,500,3,100,"Game Over...");
		}
		draw_timer++;
	}else draw_timer += mode;
	*/
	//DrawFormatString(0,0,1,"%d %d",mode,draw_timer);
	

CWait::CWait(){
	draw_timer=0;
	mode=1;
	spin=1;
	flag=1;
}

void CWait::play_init(){
	x=600;y=420;
	speed=4;
	bullet=false;
	ojisan_num=0;
	ojisan_pop_num=0;
}

void CWait::update(const char key_buf[256]){
	if(key_buf[D_DIK_W] || key_buf[D_DIK_UP]) y -=speed;
	if(key_buf[D_DIK_S] || key_buf[D_DIK_DOWN]) y +=speed;
	if(key_buf[D_DIK_D] || key_buf[D_DIK_RIGHT]) x +=speed;
	if(key_buf[D_DIK_A] || key_buf[D_DIK_LEFT]) x -=speed;

	if(key_buf[D_DIK_SPACE] && bullet == false){
		bullet_x=x+140;
		bullet_y=y+70;
		bullet=true;
	}
	if(key_buf[D_DIK_Q]) mode=1;
	if(key_buf[D_DIK_4]) speed =4;
	if(key_buf[D_DIK_8]) speed =8;
	if(( draw_timer%60 == 0 )&& ojisan_pop_num < 20){
		auto ojisan=make_shared<COjisan>(&bullet_x,&bullet_y,&bullet,&ojisan_num,&mode);
		CObject::register_object(ojisan,DRAW_LAYER::IMFOMATION_LAYER);
		ojisan_pop_num++;
	}

	if(bullet){
		bullet_x +=4+speed;
		if(bullet_x > 1400) bullet=false;
	}

	if(mode<0 && mode> -60){
		mode--;
		if(mode == -60){mode=1;}
	}
	if(mode < -60 && mode > -120){
		mode--;
		if(mode == -120){mode=1;}
	}
}

bool CKamifubuki::draw(){
	DrawExtendGraph(0,0,1500,750,kamifubuki[(draw_timer%120)/5],true);
	draw_timer++;
	return  true;
}

CKamifubuki::CKamifubuki(){
	draw_timer=0;
}

bool CMovie::draw(){
	DrawExtendGraph(0,0,1349,729,figure_id[name],false);
	if( GetMovieStateToGraph(figure_id[name]) ==1 ) return true;
	else return false;
}

CMovie::CMovie(std::string name_){
	name=name_;
}

void CMovie::init(){
	SeekMovieToGraph(figure_id[name],0);

	if(GetMovieStateToGraph(figure_id[name]) == 0){
		PlayMovieToGraph(figure_id[name]);
	}
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
	//点数をセット
	vector<pair<int,int>> tmp_rank;
	for(int i=0;i<4;i++){
		tmp_rank.push_back(pair<int,int>(score_info_enemy[i].score,i));
	}
	//点数ソート
	sort(tmp_rank.begin(), tmp_rank.end(), [](const pair<int, int>& left, const pair<int, int>& right){
		if (left.first != right.first) //同じでないなら点数の高い順に
			return left.first > right.first;
		else //同じならid(1P2P3P4P)の若い順に
			return left.second < right.second;
	});

	//rankに反映
	for(int i=0;i<4;i++){
		score_info_enemy[tmp_rank[i].second].rank = i;
	}
	rank_info = tmp_rank;

	//毎回rankとinfo_yがずれていると動かす
	for(int i=0;i<4;i++){
		if(abs(score_info_enemy[i].info_y - score_info_enemy[i].rank*RANK_HEIGHT) > SORT_SPEED+1){
			int flag = (score_info_enemy[i].info_y - score_info_enemy[i].rank*RANK_HEIGHT < 0)? 1:-1;
			score_info_enemy[i].info_y += SORT_SPEED * flag;
		}else{
			score_info_enemy[i].info_y = score_info_enemy[i].rank*RANK_HEIGHT;
		}
	}

	//描画
	for(int i=0;i<4;i++){
		DrawExtendGraph(1050, score_info_enemy[i].info_y+10,1170,score_info_enemy[i].info_y+RANK_HEIGHT+5, figure_id["F_SCORE"], true);

		SetDrawBlendMode(DX_BLENDMODE_ADD,255);
		DrawOriginalString(1060,score_info_enemy[i].info_y + 12,0.625,16,"+"+to_string(score_info_enemy[i].score)+"  "+to_string(i+1)+"P");
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	}

	return true;


}

bool COjisan::draw(){
	DrawRotaGraph(x,y,0.7,hit/18.0,figure_id["F_MAN"],true,true);
	if(hit==0){
		x -= speed;
		if(*bullet && *bullet_x < x +10 && *bullet_x > x -10 && *bullet_y < y +60 && *bullet_y > y -60){
			hit=1;
			*bullet = false;
		}
	}else{
		DrawExtendGraph(x-50,y-30,x+40,y+70,fire[hit%5],true);
		hit++;
	}
	
	draw_timer++;

	if(hit < 20){
		if((*mode) > 0) return false;
		if(x<0){
			(*mode)= -61;
			return false;
		}
		else return true;
	}
	else{
		(*ojisan_num)++;
		if((*ojisan_num) ==20){
			(*mode)=-1;
		}
		return false;
	}
}

COjisan::COjisan(int *bullet_x_,int *bullet_y_,bool *bullet_,int* ojisan_num_,int *mode_){
	draw_timer=0;
	bullet_x =bullet_x_;
	bullet_y =bullet_y_;
	bullet =bullet_;
	ojisan_num =ojisan_num_;
	mode=mode_;
	x = 1400;;
	y = rand()%600 + 65;
	hit=0;
	speed = rand()%3 + 3;
}

void CDenkyu::attaacked(){
	hit = true;
	send_msg(encode(COMMAND_NAME::UPDATE_DENKYU,denkyu_id,1,0)); //denkyu_idをONに;

}

bool CDenkyu::draw(){

	x=ip_x*1000/320;//画面引き延ばしてる分の補正
	y=ip_y*750/240;

	if(denkyu_id = 0)DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "電");
	if(denkyu_id = 0)DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "気");
	if(denkyu_id = 0)DrawFormatString(x - 50 + LEFT_WINDOW_WIDTH ,y-50 , GetColor(255,255,255), "系");

	return true;
}