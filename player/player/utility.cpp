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
		DrawDigitNum(423+LEFT_WINDOW_WIDTH, 15, 0.4375, 26, min+":"+sec+"."+under_sec);
		//timerカウント

		if(countdown_finish_flag)system_timer--;
		
	}else {
		finish_flag = true;
		DrawDigitNum(423+LEFT_WINDOW_WIDTH, 15, 0.4375, 26, "0:00.00");
	}
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


CUp_effect :: CUp_effect(){
	draw_timer = 0;
}

bool CUp_effect::draw(){

	DrawGraph(80 + LEFT_WINDOW_WIDTH,200,up_effect[draw_timer],true);
	draw_timer++;

	if(draw_timer < 10) return true;
	else return false;
}


CFinish :: CFinish(vector<pair<int,int> > result_score_){
	result_score = result_score_;
	draw_timer = 0;
}


bool CFinish::draw(){

	//DrawFormatString(0, 0, GetColor(255,255,255), "%d", draw_timer);
	/*
		仕様
		・時間0:00.00を表示、「finish」等の文字を数秒出す
		・真っ黒フェードアウト
		・結果画面フェードイン、結果画面再生
	*/

	//config
	int fade_out_time = 60;
	int fade_in_start_time = 120;

	if(draw_timer == 0){
		//GameBGM音量を小さくする
		//serverからのみGameBGMを流すので音量変化はしない
		//ChangeVolumeSoundMem(126, sound_id["S_GAME_BGM"]);
	} else if(draw_timer < fade_out_time) {
		//カメラ入力はmain関数で描画
		//finishの文字出力
	} else if(draw_timer == fade_out_time){


	} else if(draw_timer < fade_in_start_time){
		int black_value = (draw_timer - fade_out_time) * 15;
		black_value = (255 < black_value) ? 255 : black_value;
		//真っ暗画面にフェードアウト
		SetDrawBlendMode(DX_BLENDMODE_SUB, black_value);
		DrawBox( 0, 0, 1350, 730,GetColor(255, 255, 255), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	} else if(draw_timer == fade_in_start_time){
		//BGMの変更
		StopSoundMem(sound_id["S_GAME_BGM"]);

		//描画リストの要素をすべて削除せず、リザルトレイヤーを一番上に
		auto result = make_shared<CResult>(result_score);
		CObject::register_object(result,DRAW_LAYER::RESULT_LAYER);
	} 
	draw_timer++;
	return true;
}


CResult :: CResult(vector<pair<int,int> > result_score_){
	result_score = result_score_;
	draw_timer = 0;
}

bool CResult::draw(){
	//config
	int fade_in_end_time = 15;

	//リザルト画面の描画
	if(draw_timer == 0){
		//動画スタート
		result_movie_handle = LoadGraph("movie/result.ogv");
		PlayMovieToGraph( result_movie_handle ) ;
	}

	//ずっと動画再生
	if(!(ProcessMessage() == 0 && GetMovieStateToGraph( result_movie_handle ) == 1)){
		//動画はループし続ける
		 SeekMovieToGraph( result_movie_handle , 0 ) ;
		 PlayMovieToGraph( result_movie_handle ) ;
	}
	DrawGraph( 0 , 0 , result_movie_handle , FALSE ) ;
	// ウエイトをかけます、あまり速く描画すると画面がちらつくからです
    WaitTimer( 17 ) ;

	/*
		仕様：
			・はじめは動画のみ
			・結果発表の文字登場
			・カードが流れる
			・自分のカードはずっと点滅
	*/
	int float_start_time = 60;
	int float_end_time = 120;
	if(draw_timer < 30){
		//はじめは待機
	} else if(draw_timer <  float_start_time){
		//結果発表であることを知らせる
		
	} else if(draw_timer < float_end_time){

		//スコアカードが流れてくる
		int dx = (draw_timer - float_start_time)*100;
		for(int i = 0; i < 4; i++){
			int x = 1400 + i*400 -dx;
			if(x == 1000) PlaySoundMem(sound_id["S_SHU"], DX_PLAYTYPE_BACK);
			if(x < 360) x = 360;
			DrawGraph(x, 150+140*i, figure_id["F_RESULT_CARD"], true);
			DrawOriginalString(x+40,170+140*i,1.0,24,to_string(result_score[i].second+1)+"P"+"\t\t\t\t\t\t\t"+to_string(result_score[i].first));
		}

	} else {
		//自分のスコアは点滅
		for(int i = 0; i < 4; i++){
			if(PLAYER_NM == result_score[i].second){
				int alpha_palam = 30 + draw_timer - float_end_time;
				alpha_palam = (alpha_palam%60-30)*(alpha_palam%60-30)/3;
				DrawGraph(360, 150+140*i, figure_id["F_RESULT_CARD"], true);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_palam);
				DrawGraph(360, 150+140*i, figure_id["F_RESULT_CARD_WHITE"], true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
				DrawOriginalString(400,170+140*i,1.0,24,to_string(result_score[i].second+1)+"P"+"\t\t\t\t\t\t\t"+to_string(result_score[i].first));
			} else {
				DrawGraph(360, 150+140*i, figure_id["F_RESULT_CARD"], true);
				DrawOriginalString(400,170+140*i,1.0,24,to_string(result_score[i].second+1)+"P"+"\t\t\t\t\t\t\t"+to_string(result_score[i].first));
			}
		}
	}
	draw_timer++;
	return true;
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

int setposition(int flag,int i){
	if(flag-i*3>0){
			return 435-(flag-i*3)*(flag-i*3)*(flag-i*3)/20;
		}else{
			return 435;
		}
}

//消えゆくタイトルを表示,流れるタイトル
void Drawtitle(int flag){
	int wordstart=435;
	for(int i =0;i<14;i++){
		DrawGraph(setposition(flag,i)	
		,300,title[i],true);
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
	int wordwidth=48;
	int wordstart=435;
	int wordy=300;
	int title_end_time = 80;
	int movie_end_time =500;
	
	//回る戦車
		MV1SetWireFrameDrawFlag(figure_id["X_TANK"],true);
		MV1SetScale(figure_id["X_TANK"],VGet(5.0f,5.0f,5.0f));
		MV1SetPosition(figure_id["X_TANK"],VGet(180.0f,50.0f,150.0f));
		MV1SetRotationXYZ(figure_id["X_TANK"],VGet(0.0f,spin++/10.0f,0.0f));
		MV1DrawModel(figure_id["X_TANK"]);

//	タイトル表示
	if(mode>0){
		//タイトルロゴ表示
		SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,((draw_timer%80-40)*(draw_timer%80-40))/5);
		DrawGraph(wordstart,wordy,figure_id["F_TITLE"],true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		//接続中表示
		DrawGraph(wordstart+500,wordy+125,figure_id["F_CONNECT"],true);
		if(draw_timer%160>40){
			DrawGraph(wordstart+740,wordy+135,figure_id["F_DOTGRAY"],true);
			if(draw_timer%160>80){
				DrawGraph(wordstart+760,wordy+135,figure_id["F_DOTGRAY"],true);
				if(draw_timer%160>120){
					DrawGraph(wordstart+780,wordy+135,figure_id["F_DOTGRAY"],true);}}}
	}else{//Pでモード変更
		flag++;
		if(waitflag==0){//P押してからタイトルがはけるまで
			//タイトル移動開始
			Drawtitle(flag);
			//conected表示
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,255-flag);
			DrawGraph(wordstart+500,wordy+125,figure_id["F_CONNECTED"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
			if(flag>=title_end_time){
				beforeflag=flag;
				waitflag=1;
			}
		}else if (waitflag==1){//タイトルはけてからホワイトアウトまで
			//if(flag == title_end_time) draw_timer= 60;
			//「接続開始」の文字出力
			if(draw_timer % 60 == 50) PlaySoundMem( sound_id["S_PI"], DX_PLAYTYPE_BACK );
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,((draw_timer%60-30)*(draw_timer%60-30))/5);
			DrawGraph(400,wordy,figure_id["F_CONNECTED_JA"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
			//接続開始と同時にジョジョに画面をホワイトアウト
			//SetDrawBlendMode(DX_BLENDMODE_MULA,2*(flag-title_end_time-10));//黒使うときはこっち
			//DrawGraph(0,0,figure_id["F_BLACKBACK"],false);
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,2*(flag-beforeflag-40));
			DrawGraph(0,0,figure_id["F_WHITEBACK"],true);
			//SetDrawBlendMode(DX_BLENDMODE_ADD,255);//2*(flag-title_end_time-10));
			//DrawGraph(0,0,figure_id["F_GRAYBACK"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);	
			SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,255-flag-(beforeflag));
			DrawGraph(wordstart+500,wordy+125,figure_id["F_CONNECTED"],true);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
			if(2*(flag-beforeflag-40)>360){
				waitflag=2;
				beforeflag=flag;
			}
		}else if(waitflag==2){//ホワイトアウトからグレーへ
			DrawGraph(0,0,gray[(flag-beforeflag)/3],true);
			if(flag-beforeflag>=30){
				DrawGraph(0,0,figure_id["F_GRAYBACK"],true);
				beforeflag=flag;
				waitflag=3;
			}			
		}else if(waitflag==3 && movieflag==-1){//ムービー再生
			movieflag=1;
			PlaySoundMem( sound_id["S_LINKSTART"], DX_PLAYTYPE_BACK );
			movie_end_time=flag+150;
			DrawGraph(0,0,figure_id["F_GRAYBACK"],true);
		}else if (waitflag==3){
			if(flag == movie_end_time){
				//GameBGMの再生
				StopSoundMem( sound_id["S_LINKSTART"] );
				PlaySoundMem( sound_id["S_GAME_BGM"] , DX_PLAYTYPE_BACK );
			}
			//ここでスタート状態の画像を表示したい→カメラから画像をあらかじめ取得しておく必要がある？
			DrawGraph(0,0,figure_id["F_BACK"],false);
			DrawExtendGraph(  LEFT_WINDOW_WIDTH ,0,1000 + LEFT_WINDOW_WIDTH  , 750, camera_image_handle, false ) ;
			if(flag<movie_end_time){
				DrawGraph(0,0,figure_id["F_GRAYBACK"],true);
			}else{
				SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,255-(flag-movie_end_time)*4);
				DrawGraph(0,0,figure_id["F_WHITEBACK"],true);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
			}
			//明るくなったらゲーム開始
			if(255-(flag-movie_end_time)*4<=-150){
				gameflag=1;
			}
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

	

CWait::CWait(){
	draw_timer=0;
	mode=1;
	spin=1;
	flag=1;
	gameflag=0;
	movieflag=-1;
	waitflag=0;
	beforeflag=0;
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
		if(score_info_enemy[i].score <10){
			DrawOriginalString(1070,score_info_enemy[i].info_y + 12,0.625,16,to_string(i+1)+"P: "+to_string(score_info_enemy[i].score));
		}else{
			DrawOriginalString(1070,score_info_enemy[i].info_y + 12,0.625,16,to_string(i+1)+"P:"+to_string(score_info_enemy[i].score));
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
	}

	return true;


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