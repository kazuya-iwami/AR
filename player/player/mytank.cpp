
#include "mytank.h"

bool CMytank::draw(){

	//例　DrawGraph(0,0,figure_id["TEST"],false);
	//ここで 照準、所有アイテム、残機描画
	//それぞれ関数わけしても
	//DrawGraph(0,0,figure_id["F_TEST"],false);
	return true;
};

CMytank::CMytank(){
	//初期化
	score = 0;
	num_bullet = 10;
	ope_status = OPERATION_STATUS::REGULAR;
	ope_timer = 0;
	vel_R = 0;
	vel_L = 0;
	focus_x = 200;
	focus_y = 200;

};

void CMytank::move(){

}

void CMytank::set_vel(int vr,int vl){
	vel_R=vr;
	vel_L=vl;
}

void CMytank::get_msg(string msg){

}