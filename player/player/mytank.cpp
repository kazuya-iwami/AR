
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

void CMytank::move(tstring direction){
	//2015/3/31時点では正常運転のみ実装
	//通信失敗の時の処理やプレイヤー状態変更の場合は考慮しない。
	//とりあえず、プロジェクトに関係ない別のURLでは正常に通信を終了した。
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

void CMytank::get_msg(){
	string msg = check_msg();
}