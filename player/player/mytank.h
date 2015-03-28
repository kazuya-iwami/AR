
#pragma once

#include "object.h"
#include "item.h"

enum OPERATION_STATUS {
	REGULAR,
	STOP
};


class CMytank : public CObject {
	CItem *item;
	int score;//撃墜数-被撃墜数
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R,vel_L; //左右のタイヤの速度
	int focus_x,focus_y;//照準の位置

	
public:
	CMytank();
	void move();//移動
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr,int vl);
	void get_msg(string str);//サーバーからのメッセージ用
	bool draw();
	void gen_bullet();//こういうの作ることになると思われ
};
