
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

void CMytank::gen_bullet(BULLET_KIND kind,shared_ptr<CEnemy> enemy1,shared_ptr<CEnemy> enemy2,shared_ptr<CEnemy> enemy3){

	if(enemy1->get_x() < focus_x && enemy1->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy1->get_y() < focus_y && enemy1->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,1,(int)BULLET_KIND::BULLET_NOMAL));
	}
	if(enemy2->get_x() < focus_x && enemy2->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy2->get_y() < focus_y && enemy2->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH){
		auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
		CObject::register_object(bullet);
		send_msg(encode(COMMAND_NAME::SHOOT_BULLET,id,2,(int)BULLET_KIND::BULLET_NOMAL));
	}
	if(enemy3->get_x() < focus_x && enemy3->get_x() + ENEMY_WIDTH < focus_x + FOCUS_WIDTH && enemy3->get_y() < focus_y && enemy3->get_y() + ENEMY_WIDTH < focus_y + FOCUS_WIDTH){
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

            break;
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
			//自分の攻撃が当たった場合はここでスコア上昇
            
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



