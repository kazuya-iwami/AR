
#pragma once

#include "object.h"
#include "item.h"
#include "HTTPConnection.h"
#include "network.h"

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
	CItem *item;
	int score;//撃墜数-被撃墜数
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R,vel_L; //左右のタイヤの速度
	int focus_x,focus_y;//照準の位置
	tstring ipAddress; //このプログラムで操作する戦車のIPアドレス
	GAME_STATUS game_status;

	
public:
	CMytank();
	void move(tstring direction);//移動
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr,int vl);
	void get_msg();//サーバーからのメッセージ用
	bool draw();
	void gen_bullet();//こういうの作ることになると思われ
	void set_ip_address(tstring address);
};
