
#include "mytank.h"

bool CMytank::draw(){

	//��@DrawGraph(0,0,figure_id["TEST"],false);
	//������ �Ə��A���L�A�C�e���A�c�@�`��
	//���ꂼ��֐��킯���Ă�
	//DrawGraph(0,0,figure_id["F_TEST"],false);
	return true;
};

CMytank::CMytank(){
	//������
	score = 0;
	num_bullet = 10;
	ope_status = OPERATION_STATUS::REGULAR;
	ope_timer = 0;
	vel_R = 0;
	vel_L = 0;
	focus_x = 200;
	focus_y = 200;

};

void CMytank::move(tstring direction){
	//2015/3/31���_�ł͐���^�]�̂ݎ���
	//�ʐM���s�̎��̏�����v���C���[��ԕύX�̏ꍇ�͍l�����Ȃ��B
	//�Ƃ肠�����A�v���W�F�N�g�Ɋ֌W�Ȃ��ʂ�URL�ł͐���ɒʐM���I�������B
	tstring strUrl = _T("http://") + ipAddress+_T("/move/");
	bool isMethodGet = true;
	tstring strResult;
	HttpRequest(strUrl, isMethodGet, direction, strResult);
}

void CMytank::	set_ip_address(tstring address){
	ipAddress = address;
}

void CMytank::set_vel(int vr,int vl){
	vel_R=vr;
	vel_L=vl;
}

void CMytank::get_msg(string msg){

}