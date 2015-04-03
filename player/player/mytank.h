
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

enum GAME_STATUS{
    GAME_PLAY,
    GAME_PAUSE
};


//自機クラス

class CMytank : public CObject,public CNetwork {
	ITEM_KIND item_kind;
	int score;//撃墜数-被撃墜数
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R,vel_L; //左右のタイヤの速度
	int focus_x,focus_y;//照準の位置
	GAME_STATUS game_status;

	
public:
	shared_ptr<CEnemy> enemy0;
	shared_ptr<CEnemy> enemy1;
	shared_ptr<CEnemy> enemy2;
	shared_ptr<CEnemy> enemy3;

	CMytank();
	void move(tstring direction, tstring speed);//移動
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr,int vl);
	void get_msg();//サーバーからのメッセージ用
	bool draw();
	void gen_bullet(BULLET_KIND kind);//弾生成関数
	void use_item(); // item生成関数
	void detect_enemy(Mat image);
	void check_focus();
};
