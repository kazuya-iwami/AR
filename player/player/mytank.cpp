﻿#include "mytank.h"
#include <sstream>
#include "debug.h"
#include "item.h"

#define MARGIN1 150
#define MARGIN2 50

bool CMytank::draw() {

	//スコア表示
	DrawGraph(200, 650, number[score / 10], true);
	DrawGraph(260, 650, number[score % 10], true);

	//アイテム枠表示
	DrawGraph(0, 0, figure_id["F_FRAME"], true);

	//カーソル表示
	DrawGraph(focus_x, focus_y, figure_id["F_CURSUR"], true);

	return true;
};

CMytank::CMytank() {
	//初期化
	score = 0;
	num_bullet = 10; //残弾10こ
	ope_status = OPERATION_STATUS::REGULAR;
	ope_timer = 0;
	vel_R = 0;
	vel_L = 0;
	focus_x = 200;
	focus_y = 200;
	game_status = GAME_STATUS::GAME_PLAY;
	item_kind = ITEM_KIND::STAR; //スターを持たせる

	send_msg("HELLO");

	if (id != 0) {
		auto enemy0_ = make_shared<CEnemy>(0); //スマートポインタに配列が実装されていないため
		enemy0 = enemy0_;
		enemy0->init(80, 180, 100, 200, 100, 200);
		CObject::register_object(enemy1);
	}
	if (id != 1) {
		auto enemy1_ = make_shared<CEnemy>(1);
		enemy1 = enemy1_;
		enemy1->init(80, 180, 100, 200, 100, 200);
		CObject::register_object(enemy1);
	}
	if (id != 2) {
		auto enemy2_ = make_shared<CEnemy>(2);
		enemy2 = enemy2_;
		enemy2->init(0, 30, 100, 200, 100, 200);
		CObject::register_object(enemy2);
	}
	if (id != 3) {
		auto enemy3_ = make_shared<CEnemy>(3);
		enemy3 = enemy3_;
		enemy3->init(30, 80, 100, 200, 100, 200);
		CObject::register_object(enemy3);
	}
};

void CMytank::move(tstring direction, tstring speed) {
	tstring ip_address = _T("192.168.10.125");
	//2015/3/31時点では正常運転のみ実装
	//通信失敗の時の処理は考慮していない。
	//2015/4/4において、方向によってURLを作成したため追記。
	tstring strUrl = _T("http://") + ip_address + _T("/move/") + direction + _T("/") ;
	bool isMethodGet = true;
	tstring strResult;
	HttpRequest(strUrl, isMethodGet, speed, strResult);
}


void CMytank::set_vel(int vr, int vl) {
	vel_R = vr;
	vel_L = vl;
}

void CMytank::gen_bullet(BULLET_KIND item_data) {

	//残弾処理
	if (num_bullet == 0)return;
	num_bullet--;

	//描画
	auto bullet = make_shared<CBullet>(focus_x - 50 , focus_y - 50, 0, BULLET_KIND::BULLET_NOMAL);
	CObject::register_object(bullet);


	if (id != 0 && enemy0->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 0, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 1 && enemy1->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 1, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 2 && enemy2->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 2, (int)BULLET_KIND::BULLET_NOMAL));
	if (id != 3 && enemy3->lockon)send_msg(encode(COMMAND_NAME::SHOOT_BULLET, id, 3, (int)BULLET_KIND::BULLET_NOMAL));

}

void CMytank::check_focus() {

	if (id != 0) {
		if (enemy0->get_x() - MARGIN1 < focus_x && enemy0->get_x() + MARGIN2 > focus_x && enemy0->get_y() - MARGIN1 < focus_y && enemy0->get_y() + MARGIN2 > focus_y) {
			if (enemy0->exist) { //切断したプレーヤーへの攻撃禁止
				enemy0->lockon = true;
			}
		} else enemy0->lockon = false;
	}
	if (id != 1) {
		if (enemy1->get_x() - MARGIN1 < focus_x && enemy1->get_x() + MARGIN2 > focus_x && enemy1->get_y() - MARGIN1 < focus_y && enemy1->get_y() + MARGIN2 > focus_y) {
			if (enemy1->exist) { //切断したプレーヤーへの攻撃禁止
				enemy1->lockon = true;
			}
		} else enemy1->lockon = false;
	}
	if (id != 2) {
		if (enemy2->get_x() - MARGIN1 < focus_x && enemy2->get_x() + MARGIN2 > focus_x && enemy2->get_y() - MARGIN1 < focus_y && enemy2->get_y() + MARGIN2 > focus_y) {
			if (enemy2->exist) { //切断したプレーヤーへの攻撃禁止
				enemy2->lockon = true;
			}
		} else enemy2->lockon = false;
	}
	if (id != 3) {
		if (enemy3->get_x() - MARGIN1 < focus_x && enemy3->get_x() + MARGIN2 > focus_x && enemy3->get_y() - MARGIN1 < focus_y && enemy3->get_y() + MARGIN2 > focus_y) {
			if (enemy3->exist) { //切断したプレーヤーへの攻撃禁止
				enemy3->lockon = true;
			}
		} else enemy3->lockon = false;
	}

}

void CMytank::use_item() {
	if (item_kind != ITEM_KIND::ITEM_NONE) {
		send_msg(encode(COMMAND_NAME::USE_ITEM, id, 4, (int)item_kind));
		auto item = make_shared<CItem>(530 , 50, item_kind);
		CObject::register_object(item);
	}
}

void CMytank::get_msg() {
	string msg = check_msg();

	int bullet_score = 0; //bulletによっていくつスコアが上昇するかをscoreに格納

	/* メッセージが送られてきた際の処理 */
	std::string str[10];
	int data[10];
	decode(msg.c_str(), str);
	/* commandによる処理分岐 */
	// メッセージがカンマ区切りで第四引数までもっていれば、commandとみなす
	if ("" != str[3]) {
		int command_name = std::stoi(str[0]);
		for (int i = 0; i < 10; ++i) {
			data[i] = std::stoi(str[i]);
		}
		std::ostringstream stream;
		switch (command_name) {
		case COMMAND_NAME::CHANGE_STATUS:
			switch (data[1]) {
			case GAME_STATUS::GAME_PLAY:
				game_status = GAME_STATUS::GAME_PLAY;
				break;
			case GAME_STATUS::GAME_PAUSE:
				game_status = GAME_STATUS::GAME_PAUSE;
				break;
			default:
				break;
			}
			break;
		//game_status:data[1]に変更
		case COMMAND_NAME::RETURN_BULLET:
			if (data[3] == BULLET_KIND::BULLET_NOMAL) bullet_score = 1;

			switch (data[1]) {
			case 0:
				if (id != 0) { //他人の攻撃
				enemy0->score += bullet_score;
				switch (data[2]) {
					case 1:
						if (id != 1) {//攻撃先が他人
							enemy1->score -= bullet_score;
						} else {//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 2:
						if (id != 2) {//攻撃先が他人
							enemy2->score -= bullet_score;
						} else {//攻撃先が自分
							score -= bullet_score;
						}
						break;
					case 3:
						if (id != 3) { //攻撃先が他人
							enemy3->score -= bullet_score;
						} else { //攻撃先が自分
							score -= bullet_score;
						}
						break;
					}
				} else { //自分が攻撃 攻撃先はすべて敵
					score += bullet_score;
					switch (data[2]) {
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
					if (id != 1) { //他人の攻撃
					enemy1->score += bullet_score;
					switch (data[2]) {
						case 0:
							if (id != 0) {//攻撃先が他人
								enemy0->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 2:
							if (id != 2) { //攻撃先が他人
								enemy2->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 3:
							if (id != 3) { //攻撃先が他人
								enemy3->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						}
					} else { //自分が攻撃 攻撃先はすべて敵
						score += bullet_score;
						switch (data[2]) {
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
					if (id != 2) { //他人の攻撃
					enemy2->score += bullet_score;
					switch (data[2]) {
						case 1:
							if (id != 1) {//攻撃先が他人
								enemy1->score -= bullet_score;
							} else {//攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 0:
							if (id != 0) {//攻撃先が他人
								enemy0->score -= bullet_score;
							} else {//攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 3:
							if (id != 3) { //攻撃先が他人
								enemy3->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						}
					} else { //自分が攻撃 攻撃先はすべて敵
						score += bullet_score;
						switch (data[2]) {
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
					if (id != 3) { //他人の攻撃
					enemy3->score += bullet_score;
					switch (data[2]) {
						case 1:
							if (id != 1) { //攻撃先が他人
								enemy1->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 2:
							if (id != 2) { //攻撃先が他人
								enemy2->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						case 0:
							if (id != 0) { //攻撃先が他人
								enemy0->score -= bullet_score;
							} else { //攻撃先が自分
								score -= bullet_score;
							}
							break;
						}
					} else { //自分が攻撃 攻撃先はすべて敵
						score += bullet_score;
						switch (data[2]) {
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
		case COMMAND_NAME::DISCONNECT://敵が切断した場合
			switch (data[1]) { //自分のidを受け取ることはない前提
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
			switch (data[3]) { //アイテムの種類で場合分け
			case ITEM_KIND::ITEM_NONE:
				break;
			case ITEM_KIND::STAR:
				if (data[1] != id) { //アイテム使用者が自分でなければ
					switch (data[1]) {//アイテム被使用者にPopUP表示
					case 0:
						{
						auto popup = make_shared<CPopup>(enemy0->get_x(), enemy0->get_y(), "スター使った☆");
						CObject::register_object(popup);
						break;
						}
					case 1:
						{
						auto popup = make_shared<CPopup>(enemy1->get_x(), enemy1->get_y(), "スター使った☆");
						CObject::register_object(popup);
						break;
						}
					case 2:
						{
						auto popup = make_shared<CPopup>(enemy2->get_x(), enemy2->get_y(), "スター使った☆");
						CObject::register_object(popup);
						break;
						}
					case 3:
						{
						auto popup = make_shared<CPopup>(enemy3->get_x(), enemy3->get_y(), "スター使った☆");
						CObject::register_object(popup);
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
			enemy0->map_x = data[1];
			enemy0->map_y = data[2];
			enemy1->map_x = data[3];
			enemy1->map_y = data[4];
			enemy2->map_x = data[5];
			enemy2->map_y = data[6];
			enemy3->map_x = data[7];
			enemy3->map_y = data[8];
			break;
		default:
			break;
		}
	}
}
/* commandによる処理分岐ここまで */
/* メッセージの処理ここまで */



void CMytank::detect_enemy(Mat image) {

	if (id != 0)enemy0->detect(image);
	if (id != 1)enemy1->detect(image);
	if (id != 2)enemy2->detect(image);
	if (id != 3)enemy3->detect(image);

}
