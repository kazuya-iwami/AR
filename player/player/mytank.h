
#pragma once

#include "object.h"
#include "item.h"

enum OPERATION_STATUS {
	NOMAL
};


class CMytank : public CObject {
	CItem *item;
	int score;//���Đ�-�팂�Đ�
	int num_bullet; //�c�e
	OPERATION_STATUS ope_status; //����p�X�e�[�^�X
	int ope_timer; //����p�^�C�}�[
	int vel_R,vel_L; //���E�̃^�C���̑��x
	int focus_x,focus_y;
public:
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int x_,int y_);
	void get_msg(string str);
	bool draw();
};
