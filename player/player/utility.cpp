#include "utility.h"

//�Q�[���̐������ԁB�K���ɒ���
int timer=3359;

extern int score;
extern int cursur_x;
extern int cursur_y;

bool CSystem_timer::draw(){
	//�c�莞�ԕ\��
	DrawGraph(760,20,number[timer/30/100],true);
	DrawGraph(820,20,number[(timer/30%100)/10],true);
	DrawGraph(880,20,number[timer/30%10],true);

	timer--;

	//�X�R�A�\��
	DrawGraph(200,650,number[score/10],true);
	DrawGraph(260,650,number[score%10],true);

	//�A�C�e���g�\��
	DrawGraph(0,0,figure_id["F_FRAME"],true);

	return true;//��ɕ`��
}

CSystem_timer::CSystem_timer(int x_,int y_){
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x;
	y=ip_y;

	if(visible){//���E�ɓ����Ă���Ȃ�
		DrawGraph(x*1000/320 - 100,y*750/240 - 130,figure_id["F_DETECT"],true);//��ʈ������΂��Ă镪�̕␳
	}


	return true;
}

bool CCursur::draw(){
	DrawGraph(cursur_x,cursur_y,figure_id["F_CURSUR"],true);
	return true;
}