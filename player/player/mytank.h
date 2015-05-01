
#pragma once

#include "object.h"
#include "item.h"
#include "HTTPConnection.h"
#include "network.h"
#include "bullet.h"
#include "utility.h"
#include "explosion.h"
#include <ctime>
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
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R, vel_L; //左右のタイヤの速度
	GAME_STATUS game_status;
	bool focus_flag;//カウントダウンタイマー終了後trueになりカーソル表示
	int map_x;
	int map_y;
	bool attackable;
	int charge_start_time, charge_end_time; // 弾丸補充開始,終了時間
	int stun_start_time, stun_end_time; // 敵の攻撃を受けた後の硬直開始時間,終了時間

public:
	int score;//撃墜数-被撃墜数

	int shaketimer;
	int shakeflag;
	int focus_x,focus_y;//照準の位置
	//int shake_x,shake_y;//被弾時の画面振動を司るパラメータ
	bool preflag;
	bool is_reloading;
	bool is_stunned; // 敵の攻撃を受けた際等の硬直

	int HP;//ヒットポイント-なくなるといったん死ぬ
	VIABILITY_STATUS viability_status;//生存状態

	shared_ptr<CEnemy> enemy0;
	shared_ptr<CEnemy> enemy1;
	shared_ptr<CEnemy> enemy2;
	shared_ptr<CEnemy> enemy3;
	
	shared_ptr<CBullet_image> bullet_image;//残弾描画
	shared_ptr<CMap> map;
	shared_ptr<CScore_Info> score_info;
	shared_ptr<CEeic> eeic;

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
	void show_focus();
	void focus_to_up();
	void focus_to_down();

	void bullet_charge(int charge);
	void start();
	void finish();
	int get_num_bullet();
	void lose_HP();//テスト用
	void check_dead();//生死状態の判定
	void check_stun();//敵に攻撃された影響が残ってるかの判定
	void reloading();
	void revive();
};
