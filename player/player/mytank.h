﻿
#pragma once

#include "object.h"
#include "item.h"
#include "HTTPConnection.h"
#include "network.h"
#include "bullet.h"
#include "utility.h"
#include <memory>

enum OPERATION_STATUS {
	REGULAR,
	STOP
};

enum GAME_STATUS {
	GAME_UNCONNECTED,
	GAME_WAIT,
	GAME_PLAY,
	GAME_PAUSE,
	GAME_FINISH
};

enum SHAKE_STATUS{
	NO_SHAKE,
	BIG_SHAKE,
	SMALL_SHAKE
};

//自機クラス

class CMytank : public CObject, public CNetwork {
	ITEM_KIND item_kind;
	int score;//撃墜数-被撃墜数
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R, vel_L; //左右のタイヤの速度
	GAME_STATUS game_status;

public:

	int shaketimer;
	int shakeflag;
	int focus_x,focus_y;//照準の位置
	//int shake_x,shake_y;//被弾時の画面振動を司るパラメータ
	bool preflag;

	shared_ptr<CEnemy> enemy0;
	shared_ptr<CEnemy> enemy1;
	shared_ptr<CEnemy> enemy2;
	shared_ptr<CEnemy> enemy3;
	
	shared_ptr<CBullet_image> bullet_image;//残弾描画
	shared_ptr<CMap> map;

	CMytank();
	void move(tstring direction, tstring speed);//移動
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr, int vl);
	void get_msg();//サーバーからのメッセージ用
	bool draw();
	void gen_bullet(BULLET_KIND kind);//弾生成関数
	void use_item(); // item生成関数
	void detect_enemy(Mat image);
	void check_focus();
	void attacked(int score_);
	void shake_start(SHAKE_STATUS shake_status);
	void shake();
	GAME_STATUS get_game_status(){return game_status;}
	void set_game_status(GAME_STATUS game_status_);
	int get_id(){return id;}

	void bullet_charge(int charge);
	void start();
	void finish();
};
