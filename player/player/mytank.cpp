
#include "mytank.h"
#include <sstream>

#define ENEMY_WIDTH 200
#define FOCUS_WIDTH 50



bool CMytank::draw(){

	//例　DrawGraph(0,0,figure_id["TEST"],false);
	//ここで 照準、所有アイテム、残機描画
	//それぞれ関数わけしても
	//DrawGraph(0,0,figure_id["F_TEST"],false);
	return true;
};

CMytank::CMytank(){
	//初期化
	score = 0;
	num_bullet = 10;
	ope_status = OPERATION_STATUS::REGULAR;
	ope_timer = 0;
	vel_R = 0;
	vel_L = 0;
	focus_x = 200;
	focus_y = 200;
	game_status=GAME_STATUS::GAME_PLAY;

	if(id != 0){
		auto enemy0_ = make_shared<CEnemy>(); //スマートポインタに配列が実装されていないため
		enemy0 = enemy0_;
		enemy0->init(80,180,100,200,100,200);
		CObject::register_object(enemy1);
	}
	if(id != 1){
		auto enemy1_ = make_shared<CEnemy>();
		enemy1 = enemy1_;
		enemy1->init(80,180,100,200,100,200);
		CObject::register_object(enemy1);
	}
	if(id != 2){
		auto enemy2_ = make_shared<CEnemy>();
		enemy2 = enemy2_;
		enemy2->init(0,30,100,200,100,200);
		CObject::register_object(enemy2);
	}
	if(id != 3){
		auto enemy3_ = make_shared<CEnemy>();
		enemy3 = enemy3_;
		enemy3->init(30,80,100,200,100,200);
		CObject::register_object(enemy3);
	}
};

void CMytank::move(tstring direction){
	//2015/3/31時点では正常運転のみ実装
	//通信失敗の時の処理やプレイヤー状態変更の場合は考慮しない。
	tstring strUrl = _T("http://") + ipAddress+_T("/move/");
	bool isMethodGet = true;
	tstring strResult;
	HttpRequest(strUrl, isMethodGet, direction, strResult);
}

void CMytank::	set_ip_address(tstring address){
	ipAddress = address;
}

void CMytank::set_vel(int vr,int vl){
	vel_R=vr;
	vel_L=vl;
}

void CMytank::gen_bullet(BULLET_KIND kind){

	if((id != 0) && (enemy0->get_x() < focus_x && enemy0->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy0->get_y() < focus_y && enemy0->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH)){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,0,(int)BULLET_KIND::BULLET_NOMAL));
	}
	if((id != 1) && (enemy1->get_x() < focus_x && enemy1->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy1->get_y() < focus_y && enemy1->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH)){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,1,(int)BULLET_KIND::BULLET_NOMAL));
	}
	if((id != 2) && (enemy2->get_x() < focus_x && enemy2->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy2->get_y() < focus_y && enemy2->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH)){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,2,(int)BULLET_KIND::BULLET_NOMAL));
	}
	if((id != 3) && (enemy3->get_x() < focus_x && enemy3->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy3->get_y() < focus_y && enemy3->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH)){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,3,(int)BULLET_KIND::BULLET_NOMAL));
	}
	
}

void CMytank::get_msg(){
	string msg = check_msg();

    /* メッセージが送られてきた際の処理 */
    std::string str[4];
    decode(msg.c_str(), str);
    /* commandによる処理分岐 */
    // メッセージがカンマ区切りで第四引数までもっていれば、commandとみなす
    if ("" != str[3]) {
        std::ostringstream stream;
        switch (std::stoi(str[0])) {
		case COMMAND_NAME::CHANGE_SCORE:
            //player:str[1]がscore:str[2]スコア上昇
			/*
			switch (std::stoi(str[1])){
			case 0:
				if(id != 0)enemy0->score += std::stoi(str[2]);
				break;
			case 1:
				if(id != 1)enemy1->score += std::stoi(str[2]);
				break;
			case 2:
				if(id != 2)enemy2->score += std::stoi(str[2]);
				break;
			case 3:
				if(id != 3)enemy3->score += std::stoi(str[2]);
				break;
			}
            break;
			*/
		case COMMAND_NAME::CHANGE_STATUS:
			
			switch (std::stoi(str[1])) {
			case GAME_STATUS::GAME_PLAY:
				game_status = GAME_STATUS::GAME_PLAY;
				break;
			case GAME_STATUS::GAME_PAUSE:
				game_status = GAME_STATUS::GAME_PAUSE;
				break;
			default:
				break;
			}
			//game_status:str[1]に変更
            break;
		case COMMAND_NAME::RETURN_BULLET:
			//player:str[1]がplayer:str[2]にbullet:str[3]を攻撃
			
			//bulletによっていくつスコアが上昇するかをscoreに格納
			int bullet_score=0;
			if(std::stoi(str[1]) == BULLET_KIND::BULLET_NOMAL)bullet_score=1;

            switch (std::stoi(str[1])){
			case 0:
				if(id != 0){ //他人の攻撃
					enemy0->score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						enemy1->score -= bullet_score;
						break;
					case 2:
						enemy2->score -= bullet_score;
						break;
					case 3:
						enemy3->score -= bullet_score;
						break;
					}
				}
				break;

			case 1:
				if(id != 1){ //他人の攻撃
					enemy1->score += bullet_score;
					switch(std::stoi(str[2])){
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					switch(std::stoi(str[2])){
					case 0:
						enemy0->score -= bullet_score;
						break;
					case 2:
						enemy2->score -= bullet_score;
						break;
					case 3:
						enemy3->score -= bullet_score;
						break;
					}
				}
				break;

			case 2:
				if(id != 2){ //他人の攻撃
					enemy2->score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 3:
						if(id != 3){//攻撃先が他人
							enemy3->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						enemy1->score -= bullet_score;
						break;
					case 0:
						enemy0->score -= bullet_score;
						break;
					case 3:
						enemy3->score -= bullet_score;
						break;
					}
				}
				break;
			case 3:
				if(id != 3){ //他人の攻撃
					enemy3->score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						if(id != 1){//攻撃先が他人
							enemy1->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 2:
						if(id != 2){//攻撃先が他人
							enemy2->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 0:
						if(id != 0){//攻撃先が他人
							enemy0->score -= bullet_score;
						}else{//攻撃先が自分
							score -= bullet_score;
						}
						break;
					}
				}else{ //自分が攻撃 攻撃先はすべて敵 
					score += bullet_score;
					switch(std::stoi(str[2])){
					case 1:
						enemy1->score -= bullet_score;
						break;
					case 2:
						enemy2->score -= bullet_score;
						break;
					case 0:
						enemy0->score -= bullet_score;
						break;
					}
				}
				break;
			}
            break;
		case COMMAND_NAME::FINISH_ITEM:
            
            break;
		case COMMAND_NAME::INFORM_ITEM:
           
            break;
		default:
            break;
        }
    }
    /* commandによる処理分岐ここまで */
    /* メッセージの処理ここまで */
}


void CMytank::detect_enemy(Mat image){

	enemy0->detect(image);
	enemy1->detect(image);
	enemy2->detect(image);
	enemy3->detect(image);
	
}
