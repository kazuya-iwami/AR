#include "mytank.h"
#include <sstream>
#include "debug.h"
#include "item.h"
#include "main.h"
#include<stdio.h>
#include<stdlib.h>
#include <cmath>
#include <mutex>
#include <thread>

#define ENEMY_MARGIN 100
#define FOCUS_SPEED 15

void requestHttp_thread(tstring direction, tstring speed);

bool CMytank::draw() {

	//カーソル表示
	if(!is_reloading && focus_flag){
		attackable = false;
		if(id != 0 && enemy0->lockon ==true) attackable = true;
		if(id != 1 && enemy1->lockon ==true) attackable = true;
		if(id != 2 && enemy2->lockon ==true) attackable = true;
		if(id != 3 && enemy3->lockon ==true) attackable = true;
		if(attackable == true){//lockon状態
			if(!preflag){
				preflag=true;
				draw_timer = 0;
			} 
			if(preflag){
				SetDrawBlendMode(DX_BLENDMODE_ADD,255);
				if(draw_timer < 10){
					double rate = 0.7 - draw_timer * 0.02;
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,rate,0,figure_id["F_CURSUR_OUT"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,rate,-draw_timer/3.5,figure_id["F_CURSUR_IN"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,rate, draw_timer/3.5,figure_id["F_CURSUR_TRI"],true);	
				} else if(draw_timer < 40) {
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58,	    0,figure_id["F_CURSUR_ON_OUT"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58, 0.785,figure_id["F_CURSUR_ON_IN"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58, 0.785,figure_id["F_CURSUR_ON_TRI"],true);
				} else {
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58, 0,figure_id["F_CURSUR_ON_OUT"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58, -(draw_timer-40)/30.0+0.785,figure_id["F_CURSUR_ON_IN"],true);
					DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.58,  (draw_timer-40)/30.0+0.785,figure_id["F_CURSUR_ON_TRI"],true);
				}
			}
			/*　従来
			DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,1.0,draw_timer/9.0,figure_id["F_CURSUR_ON"],true);
			*/
			draw_timer++;
		}else{//lockが外れている状態
			preflag=false;
			SetDrawBlendMode(DX_BLENDMODE_ADD,255);
			DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.7, 0,figure_id["F_CURSUR_OUT"],true);
			DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.7,draw_timer/10.0,figure_id["F_CURSUR_IN"],true);
			DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,0.7, 3.14*sin(draw_timer*0.07),figure_id["F_CURSUR_TRI"],true);
			//　従来：DrawRotaGraph(focus_x+shake_x + LEFT_WINDOW_WIDTH,focus_y+shake_y,1.0,draw_timer/9.0,figure_id["F_CURSUR"],true);
			draw_timer++;
		}
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);

	//リロード画面表示
	if(is_reloading){
		DrawGraph(LEFT_WINDOW_WIDTH, 0, figure_id["F_BLACK"], true);
		DrawGraph(1350/2-456/2, 300, figure_id["F_RELOADING2"], true);
		for(int i = 0; i < draw_timer; i++){
			DrawGraph(466+i*7, 382, figure_id["F_RELOADING_GAUGE2"], true);
		}
		draw_timer++;
	}

	//HP表示
	int i;
	for(i=0;i<HP;i++){
		DrawGraph(14+96*i+LEFT_WINDOW_WIDTH,15,figure_id["F_HP"],true);
	}
	DrawGraph(0+LEFT_WINDOW_WIDTH,9,figure_id["F_HPFRAME"],true);
	

	//アイテム枠表示
	//DrawGraph(0 + LEFT_WINDOW_WIDTH, 0, figure_id["F_FRAME"], true);

	//man
	double kakudo;
	if(draw_timer%40 < 20) kakudo = -0.5+(draw_timer%40)/20.0;
	else kakudo = 0.5-(draw_timer%40-20)/20.0;
	DrawRotaGraph(80,600,1.0,kakudo,figure_id["F_MAN"],true);

	//upadte score_info
	if(id == 0)	score_info->update_score(score,enemy1->score,enemy2->score,enemy3->score);
	if(id == 1)	score_info->update_score(enemy0->score,score,enemy2->score,enemy3->score);
	if(id == 2)	score_info->update_score(enemy0->score,enemy1->score,score,enemy3->score);
	if(id == 3)	score_info->update_score(enemy0->score,enemy1->score,enemy2->score,score);
	
	return true;
};

CMytank::CMytank() {
	//初期化
	score = 0;
	HP = 3;//最初のHPは3
	viability_status = VIABILITY_STATUS::ALIVE;//最初の状態は生存
	num_bullet = 10; //残弾10こ
	ope_status = OPERATION_STATUS::REGULAR;
	ope_timer = 0;
	vel_R = 0;
	vel_L = 0;
	focus_x = 500;
	focus_y = 365;
	game_status = GAME_STATUS::GAME_PLAY;//PLAY直前に呼ばれることにした
	item_kind = ITEM_KIND::STAR; //スターを持たせる
	shaketimer=10;
	shakeflag=false;
	shake_x=0;
	shake_y=0;
	draw_timer=0;
	attackable = false;
	is_reloading = false;
	is_stunned = false;

	preflag=false;
	focus_flag = false;


	send_msg("HELLO");

	auto score_info_ = make_shared<CScore_Info>(id);
	score_info =  score_info_;
	CObject::register_object( score_info,DRAW_LAYER::IMFOMATION_LAYER);

	auto bullet_image_ = make_shared<CBullet_image>(10,10,num_bullet);
	bullet_image = bullet_image_;
	CObject::register_object(bullet_image,DRAW_LAYER::IMFOMATION_LAYER);

	auto map_ = make_shared<CMap>();
	map = map_;
	CObject::register_object(map,DRAW_LAYER::IMFOMATION_LAYER);

	if (id != 0) {
		auto enemy0_ = make_shared<CEnemy>(0); //スマートポインタに配列が実装されていないため
		enemy0 = enemy0_;
		enemy0->init(137,180,100,255,56,184);//スマホの赤
		CObject::register_object(enemy0,DRAW_LAYER::ENEMY_LAYER);
	}
	if (id != 1) {
		auto enemy1_ = make_shared<CEnemy>(1);
		enemy1 = enemy1_;
		enemy1->init(70,93,65,255,56,184);//サボテンだー
		CObject::register_object(enemy1,DRAW_LAYER::ENEMY_LAYER);
	}
	if (id != 2) {
		auto enemy2_ = make_shared<CEnemy>(2);
		enemy2 = enemy2_;
		enemy2->init(0,0,100,200,100,200);
		CObject::register_object(enemy2,DRAW_LAYER::ENEMY_LAYER);
	}
	if (id != 3) {
		auto enemy3_ = make_shared<CEnemy>(3);
		enemy3 = enemy3_;
		enemy3->init(30,30,100,200,100,200);
		CObject::register_object(enemy3,DRAW_LAYER::ENEMY_LAYER);
	}
};

void CMytank::move(tstring direction, tstring speed) {

	std::thread th(requestHttp_thread,direction,speed); //httprequestスレッド開始
	th.detach();

}


void CMytank::set_vel(int vr, int vl) {
	vel_R = vr;
	vel_L = vl;
}

void CMytank::gen_bullet(BULLET_KIND item_data) {

	//残弾処理
	if (num_bullet == 0)return;
	num_bullet--;

	bullet_image->update_num_bullet(num_bullet);//残弾数反映

	//画面振動
	shake_start(SHAKE_STATUS::SMALL_SHAKE);
	

	//弾の描画
	auto bullet = make_shared<CBullet>(focus_x , focus_y, 0, BULLET_KIND::BULLET_NOMAL);
	CObject::register_object(bullet,DRAW_LAYER::BULLET_LAYER);

	if (id != 0 && enemy0->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 0, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 1 && enemy1->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 1, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 2 && enemy2->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 2, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 3 && enemy3->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 3, (int)BULLET_KIND::BULLET_NOMAL));

}

void CMytank::check_focus(){

	if(focus_flag){
		if(id != 0){
			if(enemy0->get_x() - ENEMY_MARGIN < focus_x && enemy0->get_x() + ENEMY_MARGIN > focus_x && enemy0->get_y() -ENEMY_MARGIN < focus_y && enemy0->get_y() + ENEMY_MARGIN > focus_y){
				if(enemy0->exist){ //切断したプレーヤーへの攻撃禁止
					enemy0->lockon = true;
				}
				if(0 == CEnemy::just_before_shooted) { // 直前に撃った相手への攻撃禁止
					enemy0->lockon = false;
				}
				if(enemy0->HP==0) {
					enemy0->lockon = false;//死んだ相手への攻撃禁止
				}
			} else enemy0->lockon = false;
		}
		if(id != 1){
			if(enemy1->get_x() - ENEMY_MARGIN < focus_x && enemy1->get_x() + ENEMY_MARGIN > focus_x && enemy1->get_y() -ENEMY_MARGIN < focus_y && enemy1->get_y() + ENEMY_MARGIN > focus_y){
				if(enemy1->exist){ //切断したプレーヤーへの攻撃禁止
					enemy1->lockon = true;
				}
				if(1 == CEnemy::just_before_shooted) { // 直前に撃った相手への攻撃禁止
					enemy1->lockon = false;
				}
				if(enemy1->HP==0) {
					enemy1->lockon = false;//死んだ相手への攻撃禁止
				}
			} else enemy1->lockon = false;
		}
		if(id != 2){
			if(enemy2->get_x() - ENEMY_MARGIN < focus_x && enemy2->get_x() + ENEMY_MARGIN > focus_x && enemy2->get_y() -ENEMY_MARGIN < focus_y && enemy2->get_y() + ENEMY_MARGIN > focus_y){
				if(enemy2->exist){ //切断したプレーヤーへの攻撃禁止
					enemy2->lockon = true;
				}
				if(2 == CEnemy::just_before_shooted) { // 直前に撃った相手への攻撃禁止
					enemy2->lockon = false;
				}
				if(enemy2->HP==0) {
					enemy2->lockon = false;//死んだ相手への攻撃禁止
				}
			} else enemy2->lockon = false;
		}
		if(id != 3){
			if(enemy3->get_x() - ENEMY_MARGIN < focus_x && enemy3->get_x() + ENEMY_MARGIN > focus_x && enemy3->get_y() -ENEMY_MARGIN < focus_y && enemy3->get_y() + ENEMY_MARGIN > focus_y){
				if(enemy3->exist){ //切断したプレーヤーへの攻撃禁止
					enemy3->lockon = true;
				}
				if(3 == CEnemy::just_before_shooted) { // 直前に撃った相手への攻撃禁止
					enemy3->lockon = false;
				}
				if(enemy3->HP==0) {
					enemy3->lockon = false;//死んだ相手への攻撃禁止
				}
			} else enemy3->lockon = false;
		}
	}
}

void CMytank::use_item() {
	if (item_kind != ITEM_KIND::ITEM_NONE) {
		send_msg(encode(COMMAND_NAME::USE_ITEM, id, 4, (int)item_kind));
		
		auto item = make_shared<CItem>(200 , 200, item_kind);
		CObject::register_object(item,DRAW_LAYER::ITEM_LAYER);
		
		item_kind = ITEM_KIND::ITEM_NONE;
	}
}

//君だけのオリジナル画面振動を実装しよう！

void CMytank::shake(){
	if(shakeflag == SHAKE_STATUS::BIG_SHAKE){
		//SetDrawBlendMode(DX_BLENDGRAPHTYPE_ALPHA,shaketimer*10);
		DrawGraph(LEFT_WINDOW_WIDTH,0,figure_id["T_REDBACK"],1);
		//uSetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		shake_x=(rand()%40-20)*shaketimer;
		shake_y=(rand()%40-20)*shaketimer;
	}else if(shakeflag == SHAKE_STATUS::SMALL_SHAKE){
		shake_y=(rand()%6-3)*shaketimer;
	}

	if(shaketimer==0){
		shaketimer=11;
		shakeflag=0;
		shake_x=0;
		shake_y=0;
	}

	shaketimer--;
}

void CMytank::shake_start(SHAKE_STATUS shake_status){
	
	shakeflag = shake_status;

	shaketimer=11;
	shake_x=0;
	shake_y=0;

	/*以下shake()の残骸
	switch(n){
	case 10:
		shakeflag=true;
		PlaySoundMem( sound_id["S_BOMB"] , DX_PLAYTYPE_BACK ) ;
		shake_x=100;
		shake_y=50;
		break;
	case 9:
		shake_x=-100;
		shake_y=-50;
		break;
	case 8:
		shake_x=80;
		shake_y=40;
		break;
	case 7:
		shake_x=-80;
		shake_y=-40;
		break;
	case 6:
		shake_x=60;
		shake_y=30;
		break;
	case 5:
		shake_x=-60;
		shake_y=-30;
		break;
	case 4:
		shake_x=40;
		shake_y=20;
		break;
	case 3:
		shake_x=-40;
		shake_y=-20;
		break;
	case 2:
		shake_x=20;
		shake_y=10;
		break;
	case 1:
		shake_x=-20;
		shake_y=-10;
		break;
	case 0:
		shake_x=shake_y=0;
		shaketimer=11;
		shakeflag=false;
		break;
	default:
		break;
	}*/

}

void CMytank::get_msg(){
	string msg = check_msg();
	int bullet_score=0; //bulletによっていくつスコアが上昇するかをscoreに格納
	/* メッセージが送られてきた際の処理 */
	int data[10];
	std::string str[10];
	decode(msg.c_str(), str);
	/* commandによる処理分岐 */
	// メッセージがカンマ区切りで第四引数までもっていれば、commandとみなす
	if ("" != str[3]) {
		int command_name = std::stoi(str[0]);
		int player_from = std::stoi(str[1]);
		int player_to = std::stoi(str[2]);
		int kind = std::stoi(str[3]);

		for (int i = 0; i < 10; ++i) {
			if(str[i] == ""){
				data[i]=0;
			}else data[i] = std::stoi(str[i]);
		}
		switch (command_name) {
		case COMMAND_NAME::CHANGE_STATUS:
			
			switch (player_from) {
			case GAME_STATUS::GAME_PLAY:
				if(game_status == GAME_STATUS::GAME_WAIT){
					start();
				}else if( game_status == GAME_STATUS::GAME_PAUSE ) {
					game_status = GAME_STATUS::GAME_PLAY;
				}
				break;
			case GAME_STATUS::GAME_PAUSE:
				game_status = GAME_STATUS::GAME_PAUSE;
				break;
			default:
				break;
			}
			//game_status:player_fromに変更
			break;
		case COMMAND_NAME::RETURN_BULLET:
			{
			//player:player_fromがplayer:player_toにbullet:kindを攻撃
			//auto popup = make_shared<CPopup>(300,300,"RETURN"+msg);
			//CObject::register_object(popup);
			
			if(kind == BULLET_KIND::BULLET_NOMAL)bullet_score=1;

			switch (player_from){
			case 0:
				if(id != 0){ //他人の攻撃
					enemy0->score += bullet_score;
					switch(player_to){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					CEnemy::just_before_shooted = player_to;
					switch(player_to){
					case 1:
						enemy1->attacked(bullet_score);
						break;
					case 2:
						enemy2->attacked(bullet_score);
						break;
					case 3:
						enemy3->attacked(bullet_score);
						break;
					}
				}
				break;

			case 1:
				if(id != 1){ //他人の攻撃
					enemy1->score += bullet_score;
					switch(player_to){
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					CEnemy::just_before_shooted = player_to;
					switch(player_to){
					case 0:
						enemy0->attacked(bullet_score);
						break;
					case 2:
						enemy2->attacked(bullet_score);
						break;
					case 3:
						enemy3->attacked(bullet_score);
						break;
					}
				}
				break;

			case 2:
				if(id != 2){ //他人の攻撃
					enemy2->score += bullet_score;
					switch(player_to){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					CEnemy::just_before_shooted = player_to;
					switch(player_to){
					case 1:
						enemy1->attacked(bullet_score);
						break;
					case 0:
						enemy0->attacked(bullet_score);
						break;
					case 3:
						enemy3->attacked(bullet_score);
						break;
					}
				}
				break;
			case 3:
				if(id != 3){ //他人の攻撃
					enemy3->score += bullet_score;
					switch(player_to){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->attacked(bullet_score);
						}else{//攻撃先が自分
							attacked(bullet_score);
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					CEnemy::just_before_shooted = player_to;
					switch(player_to){
					case 1:
						enemy1->attacked(bullet_score);
						break;
					case 2:
						enemy2->attacked(bullet_score);
						break;
					case 0:
						enemy0->attacked(bullet_score);
						break;
					}
				}
				break;
			}
			break;
			}
		case COMMAND_NAME::DISCONNECT://敵が切断した場合
			switch(player_from){ //自分のidを受け取ることはない前提
			case 0:
				enemy0->disconnect();
				break;
			
			case 1:
				enemy1->disconnect();
				break;
			
			case 2:
				enemy2->disconnect();
				break;
			
			case 3:
				enemy3->disconnect();
				break;
			}
			break;
		case COMMAND_NAME::INFORM_ITEM:
			switch(kind){//アイテムの種類で場合分け
			case ITEM_KIND::ITEM_NONE:
				break;

			case ITEM_KIND::STAR:
				if(player_from != id){ //アイテム使用者が自分でなければ
					switch(player_from){//アイテム被使用者にPopUP表示
					case 0:
						{
						auto popup = make_shared<CPopup>(enemy0->get_x(),enemy0->get_y(),"スター使った☆");
						CObject::register_object(popup,DRAW_LAYER::MESSAGE_LAYER);
						break;
						}
					case 1:
						{
						auto popup = make_shared<CPopup>(enemy1->get_x(),enemy1->get_y(),"スター使った☆");
						CObject::register_object(popup,DRAW_LAYER::MESSAGE_LAYER);
						break;
						}
					case 2:
						{
						auto popup = make_shared<CPopup>(enemy2->get_x(),enemy2->get_y(),"スター使った☆");
						CObject::register_object(popup,DRAW_LAYER::MESSAGE_LAYER);
						break;
						}
					case 3:
						{
						auto popup = make_shared<CPopup>(enemy3->get_x(),enemy3->get_y(),"スター使った☆");
						CObject::register_object(popup,DRAW_LAYER::MESSAGE_LAYER);
						break;
						}

					}
				}
				break;
			case ITEM_KIND::THUNDER:
				break;

			}
			break;

		case COMMAND_NAME::UPDATE_LOCATIONS:
			if(id != 0){
				enemy0->map_x = data[1];
				enemy0->map_y = data[2];
			}else{
				map_x = data[1];
				map_y = data[2];
			}
			if(id != 1){
				enemy1->map_x = data[3];
				enemy1->map_y = data[4];
			}else{
				map_x = data[3];
				map_y = data[4];
			}
			if(id != 2){
				enemy2->map_x = data[5];
				enemy2->map_y = data[6];
			}else{
				map_x = data[5];
				map_y = data[6];
			}
			if(id != 3){
				enemy3->map_x = data[7];
				enemy3->map_y = data[8];
			}else{
				map_x = data[7];
				map_y = data[8];
			}
			break;

		case COMMAND_NAME::INFORM_DIE:
			if (id == 0) {
				break;
			} 
			break;//lockon関係は別のところで処理

		case COMMAND_NAME:: INFORM_REVIVE:
			if (id == 0) {
				viability_status = ALIVE;//生存状態の変更
				HP=3;
				score=0;
				//データの初期化
				break;//lockon関係は別のところで処理
			} 
			break;

		default:
			break;

		}//command_nameのswitch終わり
	}
	/* commandによる処理分岐ここまで */
	/* メッセージの処理ここまで */
}



void CMytank::detect_enemy(Mat image) {

	if (id != 0)enemy0->detect(image,0);
	if (id != 1)enemy1->detect(image,1);
	if (id != 2)enemy2->detect(image,2);
	if (id != 3)enemy3->detect(image,3);

}

void CMytank::attacked(int score_){
	if (HP > 0) {
		HP -= score_;
	}

	score -= score_;
	shake_start(SHAKE_STATUS::SMALL_SHAKE); // 画面振動
	is_stunned = true; // 硬直フラグ
	PlaySoundMem( sound_id["S_ATTACKED"] , DX_PLAYTYPE_BACK ); // 爆発音再生
}

void CMytank::set_game_status(GAME_STATUS game_status_){
	game_status = game_status_;
	if(game_status == GAME_STATUS::GAME_WAIT){ //FINIHSからWAITに移行する際サーバーにメッセージ送る
		send_msg(encode(COMMAND_NAME::FINISH,0,0,0));
	}


}


//my test　引数分だけ弾をチャージ
void CMytank::bullet_charge(int charge){
	//弾数がMAXまで補充されていない時
	if(num_bullet < bullet_image->max_bullet_num){
		//音声
		PlaySoundMem( sound_id["S_GET"] , DX_PLAYTYPE_BACK );
		//エフェクト
		auto up_effect = make_shared<CUp_effect>();
		CObject::register_object(up_effect,DRAW_LAYER::IMFOMATION_LAYER);
	}
	num_bullet += charge;
	
	//弾数がMAX以上の時
	if(num_bullet >= bullet_image->max_bullet_num) num_bullet = bullet_image->max_bullet_num;
	//残弾数反映
	bullet_image->update_num_bullet(num_bullet);
}


void CMytank::start(){
	//GameBGMの再生
	PlaySoundMem( sound_id["S_GAME_BGM"] , DX_PLAYTYPE_BACK );
	//Game スタート
	//set_game_status(GAME_STATUS::GAME_PLAY);//init()で無効化されるのでその後行う
	init_flag = true;

}

void CMytank::finish(){
	//GameBGMの停止
	StopSoundMem(sound_id["S_GAME_BGM"]);
	//ゲーム終了
	set_game_status(GAME_STATUS::GAME_FINISH);

	//描画リストの要素をすべて削除
	CObject::drawlist.clear();

	auto finish = make_shared<CFinish>(score_info->get_rank_info());
	CObject::register_object(finish,DRAW_LAYER::IMFOMATION_LAYER);
	auto kamifubuki=make_shared<CKamifubuki>();
	CObject::register_object(kamifubuki,DRAW_LAYER::IMFOMATION_LAYER);

}

void CMytank::show_focus(){
	focus_flag = true;
	if(id!=0)enemy0->countdown_finish();
	if(id!=1)enemy1->countdown_finish();
	if(id!=2)enemy2->countdown_finish();
	if(id!=3)enemy3->countdown_finish();

}

void CMytank::lose_HP() {
	if (HP > 0) {
		HP--;
	}
}

void CMytank::check_dead() {
	if (HP<= 0 && viability_status==VIABILITY_STATUS::ALIVE) {
		send_msg(encode(COMMAND_NAME::INFORM_DIE, id, 0, 0));
		viability_status=DEAD;
	}	
}

void CMytank::check_stun() {
	int stunned_time; // 硬直開始からの経過時間
	
	if (!is_stunned) { // 硬直していなければ
		stun_start_time = 0;
		stun_end_time = 0;
	} else {
		stun_end_time++;
		stunned_time = stun_end_time - stun_start_time;
		if (1*30 < stunned_time) { // 硬直開始から1秒以上経っていれば
			is_stunned = false;
		}
	}
	return;
}


void CMytank::focus_to_up(){
	if(focus_y < 10) return;
	focus_y -= FOCUS_SPEED;
};

void CMytank::focus_to_down(){
	if(focus_y > 720) return;
	focus_y += FOCUS_SPEED;
};


int CMytank::get_num_bullet(){
	return num_bullet;
};

void CMytank::reloading(){
	int charging_time; // 弾丸補充開始からの経過時間

	if (!is_reloading && get_num_bullet() < bullet_image->max_bullet_num) { // 弾丸補充まだ開始していなければ
		charge_start_time = 0;
		charge_end_time = 0;
		is_reloading = true;
		draw_timer = 0;
	} else {
		charge_end_time++;
		charging_time = charge_end_time - charge_start_time;
		if (2*30 < charging_time) { // 補充開始から2秒以上"2"を押し続けていれば
			bullet_charge(bullet_image->max_bullet_num);
			is_reloading = false;
		}
	}
	return;
};

void requestHttp_thread(tstring direction, tstring speed) {
	tstring ip_address = _T(RASPI_IP_ADDRESS);
	//2015/3/31時点では正常運転のみ実装
	//通信失敗の時の処理は考慮していない。
	//2015/4/4において、方向によってURLを作成したため追記。
	tstring strUrl = _T("http://") + ip_address + _T("/move/") + direction + _T("/") ;
	bool isMethodGet = true;
	tstring strResult;
	HttpRequest(strUrl, isMethodGet, speed, strResult);

}
