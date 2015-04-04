#include "utility.h"

//�Q�[���̐������ԁB�K���ɒ���
int timer=3359;

bool CSystem_timer::draw(){
	//�c�莞�ԕ\��
	DrawGraph(760,20,number[timer/30/100],true);
	DrawGraph(820,20,number[(timer/30%100)/10],true);
	DrawGraph(880,20,number[timer/30%10],true);

	timer--;

	return true;//��ɕ`��
}

CSystem_timer::CSystem_timer(int x_,int y_){
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x*1000/320;
	y=ip_y*750/240;

	if(visible){//���E�ɓ����Ă���Ȃ�
		if(!lockon){
			SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
		}else DrawFormatString(x-50 ,y-80, GetColor(255,255,255), "TRUE");
		DrawGraph(x - 100,y - 130,figure_id["F_DETECT"],true);//��ʈ������΂��Ă镪�̕␳
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
		DrawFormatString(x - 50 ,y - 100, GetColor(255,255,255), "%dP", enemy_id);
	}


	return true;
}


CEnemy::CEnemy(int enemy_id_){
	score = 0;
	exist=true;
	enemy_id = enemy_id_;
	lockon = false;
}


void CEnemy::disconnect(){
	exist = false;
}