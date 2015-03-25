
#pragma once

#include "object.h"

enum OPERATION_STATUS {
	NOMAL
};


class CMytank : public CObject {
	CItem *item;
	int score;//撃墜数-被撃墜数
	int num_bullet; //残弾
	OPERATION_STATUS ope_status; //操作用ステータス
	int ope_timer; //操作用タイマー
	int vel_R,vel_L; //左右のタイヤの速度
	int focus_x,focus_y;
public:
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int x_,int y_);
	void get_msg(string str);
	bool draw();
};
