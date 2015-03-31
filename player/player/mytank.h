
#pragma once

#include "object.h"
#include "item.h"
#include "HTTPConnection.h"

enum OPERATION_STATUS {
	REGULAR,
	STOP
};

//���@�N���X

class CMytank : public CObject {
	CItem *item;
	int score;//���Đ�-�팂�Đ�
	int num_bullet; //�c�e
	OPERATION_STATUS ope_status; //����p�X�e�[�^�X
	int ope_timer; //����p�^�C�}�[
	int vel_R,vel_L; //���E�̃^�C���̑��x
	int focus_x,focus_y;//�Ə��̈ʒu
	tstring ipAddress; //���̃v���O�����ő��삷���Ԃ�IP�A�h���X

	
public:
	CMytank();
	void move(tstring direction);//�ړ�
	void set_ope_status(OPERATION_STATUS status);
	void set_vel(int vr,int vl);
	void get_msg(string str);//�T�[�o�[����̃��b�Z�[�W�p
	bool draw();
	void gen_bullet();//���������̍�邱�ƂɂȂ�Ǝv���
	void set_ip_address(tstring address);
};
