﻿
#include <stdio.h>
#include <ctype.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <memory>
#include <iostream>
#include "DxLib.h"
#include "network.h"
#include "FPSCounter.h"
#include "object.h"
#include "mytank.h"
#include "main.h"
#include "bullet.h"
#include "explosion.h"
#include "utility.h"
#include "tstring.h"
#include "debug.h"
#include <mutex>
#include <thread>
#include <fstream>

using namespace std;

#define GAME_TIME 180 //プレー時間　300秒



#define USE_CAMERA_FLAG 2


//0:画像 1:カメラ 2:ラズパイ


int hsv[4][4];

std::string SERVER_IP_ADDRESS;// "157.82.7.4"	//サーバーのIPアドレス
std::string RASPI_IP_ADDRESS;// = "pi@rpi04.local";//ラズパイのＩＰアドレス

int PLAYER_NM ;	//自分のプレイヤー番号



bool list_cmp(std::shared_ptr<CObject>& v1,std::shared_ptr<CObject>& v2 );
int configuration();

cv::VideoCapture vcap;

int camera_image_handle;
std::mutex draw_mtx;
std::mutex mytank_mtx;
bool thread_flag;
int camera_image_size;

bool exist_flag[4];

bool init_flag;//初期化関数init()用のフラグ

Data hsv_data;//hsv情報用

shared_ptr<CMytank> mytank;
shared_ptr<CSystem_timer> system_timer;

void image_get_process(); //別スレッドで映像の受信、処理を行う
void init();


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){

	//各ヘッダファイルを見るとclass構成がわかるよ

	//色認識およびIP初期化
	int config_result = configuration();
	if(config_result == 1){
		MessageBox(NULL,"hsvが間違ってます(´・ω・`)","error",MB_OK | MB_APPLMODAL);
		exit(1);
	}

	if(config_result == 2){
		MessageBox(NULL,"ipが間違ってます(´・ω・`)","error",MB_OK | MB_APPLMODAL);
		exit(1);
	}

	if(config_result == 3){
		MessageBox(NULL,"playが間違ってます(´・ω・`)","error",MB_OK | MB_APPLMODAL);
		exit(1);
	}

	//network初期化

	CNetwork::network_init(PLAYER_NM, SERVER_IP_ADDRESS); //自分のプレイヤー番号0~3とIPアドレス書くと接続試みる


	//クラスのインスタンスはスマートポインタ(std::shared_ptr)で生成します。
	//スマートポインタの詳細はググって
	//普通のポインタと違って適切なタイミングで解放してくれる便利グッズ
	//深く考えずに以下の書き方コピれば動く。
	//何か間違ってたら教えて
	//グローバル変数はmain.hに書いてる
	//他のファイルで用いる時はexternして（externでググるべし）

	
	//auto redback=make_shared<CEffect>();

	//キーボード用
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	CPopup::debug_flag = true; // デバッグフラグがtrueならポップアップを表示


	if(USE_CAMERA_FLAG == 1){ //カメラ使用

		/*ラズパイが現在10fpsで画像配信してるので別スレッド（非同期）で画像取得することにする予定
		CG描画は30fpsの予定*/

		if(!vcap.open(0)){//デフォルトのカメラを取得はこちら
			std::cout << "Error opening video stream or file" << std::endl;
			MessageBox(NULL,"カメラ取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
			return -1;
		}
		vcap.set(CV_CAP_PROP_FPS,10);
		vcap.set(CV_CAP_PROP_FRAME_WIDTH,320);
		vcap.set(CV_CAP_PROP_FRAME_HEIGHT,240);

	}else if(USE_CAMERA_FLAG == 2){
		//ラズパイからの映像取得用
		const std::string videoStreamAddress =std::string("http://")+RASPI_IP_ADDRESS+":8080/?action=stream.mjpeg";

		if(!vcap.open(videoStreamAddress)) { //ラズパイからの取得はこちら
			std::cout << "Error opening video stream or file" << std::endl;
			MessageBox(NULL,"カメラ取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
			return -1;
		}
	}



	// ウインドウモードで起動
	SetMainWindowText( "Real Tank Battle -４機の戦車が集いし時、現実と仮想空間が交差する-" ) ;
	ChangeWindowMode( TRUE ) ;//falseならフルスクリーン
	SetGraphMode(1350,730,32);
	SetWindowInitPosition(0,0);

	//文字サイズ
	SetFontSize( 60 ) ;							 //サイズを20に変更
	SetFontThickness( 8 ) ;						 //太さを8に変更
	ChangeFont("07ロゴたいぷゴシック7");			  //種類をMS明朝に変更
	//ChangeFontType( DX_FONTTYPE_ANTIALIASING_EDGE );//アンチエイリアス＆エッジ付きフォントに変更

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// 描画先を裏画面にする
	SetDrawScreen( DX_SCREEN_BACK ) ;


	//FPS測定器初期化 
	//これによって1秒に最大30回しかループが回らないようにする
	//fps固定しないと一サイクルに一定数動くようなプログラムがうまく動か
	CFps fps; 

	//使用する画像の読み込み
	SetUseASyncLoadFlag(TRUE);
	CObject::load();//すべての画像はこの中で読み込む
	SetUseASyncLoadFlag(FALSE);

	while(GetASyncLoadNum() > 0){ //全て読み込むまで次の動作行わない
		WaitTimer(10);
	}

	//exist_flag初期化 init()の前に行う
	for(int i=0;i<4;i++){
		exist_flag[i] = true;
	}

	init(); //ゲームの初期化

	mytank->set_game_status(GAME_STATUS::GAME_WAIT);//初回のみWAITから始める

	//描画リストの要素をすべて削除 waitでは描画しない
	CObject::drawlist.clear();

	auto wait=make_shared<CWait>();
	CObject::register_object(wait,DRAW_LAYER::IMFOMATION_LAYER);
	mytank->get_mode(&(wait->mode));

	shared_ptr<CMovie> iwami;

	thread_flag = true;
	std::thread th(image_get_process); //映像取得、処理用スレッド開始

	

	// メインループ
	while(1){
		// 画面に描かれているものを一回全部消す
		draw_mtx.lock();
		if(!(mytank->get_game_status() == GAME_STATUS::GAME_PAUSE)) ClearDrawScreen();
		draw_mtx.unlock();

		//キー状態取得
		//書き方は以下の通り
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		//初期化用のinit()を呼ぶ
		if(init_flag){

			init();
			init_flag=false;
		}


		if(mytank->get_game_status() == GAME_STATUS::GAME_WAIT){
			
			if( wait->gameflag==1){
				key_prev_buf[ KEY_INPUT_RETURN] = 1; //他の条件に引っかからないよう細工
				
				wait->gameflag=0;			
				
				mytank->start();
			}/*
			else if(  key_buf[ KEY_INPUT_SPACE ] == 1 && key_prev_buf[ KEY_INPUT_SPACE] == 0 && wait->mode > 0){
				if(wait->mode <5) wait->mode += 1;
			}
			else if(  key_buf[ KEY_INPUT_M ] == 1 && key_prev_buf[ KEY_INPUT_M] == 0){
				auto iwami_=make_shared<CMovie>("M_LINKSTART");
				iwami =iwami_;
				CObject::register_object(iwami,DRAW_LAYER::MOVIE_LAYER);
				draw_mtx.lock();
				iwami->init();
				draw_mtx.unlock();
			}
			else if(  key_buf[ KEY_INPUT_P ] == 1 && key_prev_buf[ KEY_INPUT_P] == 0 && wait->mode > 0){
				wait->mode=0;
			}
			*/
			if(key_buf[KEY_INPUT_S]==1){
				wait->spin=wait->spin+2;
			}
			if(wait->movieflag==1){
				wait->movieflag=0;
				auto iwami_=make_shared<CMovie>("M_LINKSTART");
				iwami =iwami_;
				CObject::register_object(iwami,DRAW_LAYER::MOVIE_LAYER);
				draw_mtx.lock();
				iwami->init();
				draw_mtx.unlock();
			}

		} else if(mytank->get_game_status() == GAME_STATUS::GAME_PLAY){

			//照準と敵が重なっているかチェック
			mytank->check_focus();

			//振動処理
			mytank->shake();

			//移動処理　この中に書く
			//キー状態取得の後に移動します(2015/3/_31 大杉追記)
			//mytank->move();
			tstring speed;
				//速度設定の分岐条件に関しては大杉は知らないです...
				if(true){
					//通常
					speed = _T("full");
				}else {
					//サーバから減速命令が送られてきた場合
					speed = _T("half");
				}

			//カウントダウン終了後キーボード操作許す
			if(system_timer->get_countdown_finish_flag()){

				mytank->show_focus();//一度だけ呼べばいいのだけども…

				


				mytank->check_dead();
				mytank->check_stun();
		
				/* 弾丸補充中は動作しない処理ここから */
				if (!mytank->is_reloading && !mytank->is_stunned && mytank->viability_status==ALIVE) {//攻撃を受け硬直してる場合、死んでる場合も操作を許さない

					//各キーを押し続けるとその動作をする。
					if(  key_buf[ KEY_INPUT_UP ] == 1 && key_prev_buf[ KEY_INPUT_UP] == 0 ){
					//mytank->set_vel(1,1);//自機の速度設定 (pwm制御の場合か)
					mytank->move(_T("forward"), speed);
					}
					if(  key_buf[ KEY_INPUT_DOWN ] == 1 && key_prev_buf[ KEY_INPUT_DOWN] == 0 ){
					//mytank->set_vel(-1,-1);
					mytank->move(_T("backward"), speed);
					}
					if(  key_buf[ KEY_INPUT_LEFT ] == 1 && key_prev_buf[ KEY_INPUT_LEFT] == 0 ){
					//mytank->set_vel(-1,1);
					mytank->move(_T("left"), speed);
					}
					if(  key_buf[ KEY_INPUT_RIGHT ] == 1 && key_prev_buf[ KEY_INPUT_RIGHT] == 0 ){
					//mytank->set_vel(1,-1);
					mytank->move(_T("right"), speed);
					}
					//各キーを離したらstop
					if( (key_buf[ KEY_INPUT_UP ] == 0 && key_prev_buf[ KEY_INPUT_UP] == 1) || 
					(key_buf[ KEY_INPUT_DOWN ] == 0 && key_prev_buf[ KEY_INPUT_DOWN] == 1) ||
					(key_buf[ KEY_INPUT_LEFT ] == 0 && key_prev_buf[ KEY_INPUT_LEFT] == 1) ||
					(key_buf[ KEY_INPUT_RIGHT ] == 0 && key_prev_buf[ KEY_INPUT_RIGHT] == 1) ){
					mytank->move(_T("stop"), speed);
					}
					
					//bullet生成
					if( ( key_buf[ KEY_INPUT_SPACE ] == 1 && key_prev_buf[ KEY_INPUT_SPACE] == 0) ||
						 mytank->endless_bullet_timer % 5 == 2){
						mytank->gen_bullet(BULLET_KIND::BULLET_NOMAL);
					}
	
					
					//テスト用　Dを押すとカーソルが右に
					if(  key_buf[ KEY_INPUT_D ] == 1){
						mytank->focus_to_right();
					}
	
					//テスト用　Aを押すとカーソルが左に
					if(  key_buf[ KEY_INPUT_A ] == 1){
						mytank->focus_to_left();
					}
					
					//テスト用　Wを押すとカーソルが上に
					if(  key_buf[ KEY_INPUT_W ] == 1 ){
						mytank->focus_to_up();
					}
	
					//テスト用　Sを押すとカーソルが下に
					if(  key_buf[ KEY_INPUT_S ] == 1){ 
						mytank->focus_to_down();
					}

					/*
					//テスト用　とりあえずX押したら画面が振動するよ
					if(key_buf[KEY_INPUT_X]==1 && key_prev_buf[KEY_INPUT_X]==0){
						mytank->shake_start(SHAKE_STATUS::BIG_SHAKE);
					}*/

				}
				/* 弾丸補充中動作しない処理ここまで */

				/* 弾丸補充処理ここから */
				if(  key_buf[ KEY_INPUT_2 ] == 1){
					mytank->reloading();
				}
				if ( key_buf[ KEY_INPUT_2 ] == 0 ) { // 2を離していれば、弾丸補充開始前の状態にする
					mytank->is_reloading = false;
				}
				/* 弾丸補充処理ここまで */

				/*
				//テスト用　Ｌを押すと攻撃を受けるよ
				if (key_buf[KEY_INPUT_L]==1 && key_prev_buf[KEY_INPUT_L]==0) {
					mytank->attacked(1);
				}

				//テスト用　H自分のポイント＋１
				if (key_buf[KEY_INPUT_H]==1 && key_prev_buf[KEY_INPUT_H]==0) {
					mytank->score++;
					auto plus=make_shared<CPlus>(&(mytank->score_info->score_info_enemy[PLAYER_NM].info_y));
					CObject::register_object(plus,DRAW_LAYER::IMFOMATION_LAYER);
				}
				//テスト用　J自分のポイント＋１
				if (key_buf[KEY_INPUT_J]==1 && key_prev_buf[KEY_INPUT_J]==0) {
					mytank->enemy1->score++;
				}
				//テスト用　L自分のポイント＋１
				if (key_buf[KEY_INPUT_L]==1 && key_prev_buf[KEY_INPUT_L]==0) {
					mytank->enemy2->score++;
				}
				*/
			}

			/*
			//ENTERでGAME_STAUTS変更
			if(  key_buf[ KEY_INPUT_RETURN ] == 1 && key_prev_buf[ KEY_INPUT_RETURN] == 0){
				mytank->set_game_status(GAME_STATUS::GAME_PAUSE);
			}
			*/
			
			//時間切れるとGAME_STATUS変更
			if(system_timer->get_finish_flag() /*|| key_buf[ KEY_INPUT_Q ] == 1*/){
				mytank->finish();
				
			}

		} else if(mytank->get_game_status() == GAME_STATUS::GAME_PAUSE){

			draw_mtx.lock();
			DrawFormatString(50 + LEFT_WINDOW_WIDTH, 300, GetColor(255,255,255), "PAUSE... ENTERで戻る");
			draw_mtx.unlock();
			/*
			//ENTERでGAME_STAUTS変更
			if(  key_buf[ KEY_INPUT_RETURN ] == 1 && key_prev_buf[ KEY_INPUT_RETURN] == 0){
				mytank->set_game_status(GAME_STATUS::GAME_PLAY);
			}
			*/

		} else if(mytank->get_game_status() == GAME_STATUS::GAME_FINISH){
			//リロードの棒が延びないように設定
			mytank->is_reloading = false;

			if( mytank->wait_start_flag ){
				mytank->wait_start_flag = false;
				mytank->set_game_status(GAME_STATUS::GAME_WAIT);

				if(mytank->get_id() != 0)exist_flag[0] = mytank->enemy0->exist;
				if(mytank->get_id() != 1)exist_flag[1] = mytank->enemy1->exist;
				if(mytank->get_id() != 2)exist_flag[2] = mytank->enemy2->exist;
				if(mytank->get_id() != 3)exist_flag[3] = mytank->enemy3->exist;

				
				//描画リストの要素をすべて削除
				CObject::drawlist.clear();
				
				
				auto wait_ = make_shared<CWait>();
				wait = wait_;
				CObject::register_object(wait,DRAW_LAYER::IMFOMATION_LAYER);
				mytank->get_mode(&(wait->mode));
				
			}


		}

		if(mytank->get_game_status() == GAME_STATUS::GAME_PLAY || mytank->get_game_status() == GAME_STATUS::GAME_FINISH
			||mytank->get_game_status() == GAME_STATUS::GAME_WAIT){
		    //描画
			/*	
			すべての描画をここで受け持つ。
			drawlistに登録されてるオブジェクトのdraw()をすべて実行
			drawの戻り値がfalseだとリストから除く(アニメーション描画終了後falseを返す)
			*/

			std::list<std::shared_ptr<CObject>>::iterator it;
			CObject::drawlist.sort(list_cmp);//レイヤーの順にソート

			draw_mtx.lock();
			for(it=CObject::drawlist.begin(); it!=CObject::drawlist.end();){  // 最後の「it++」を消す
				if( !(*it)->draw() ){ //アニメーション終了時
					// オブジェクトをリストからはずす
					it = CObject::drawlist.erase( it );
					continue;
				}
				it++;   // インクリメント
			}
			draw_mtx.unlock();
		}

		//サーバーからmsgの受信
		/* 1サイクル1回呼ぶ */
		mytank->get_msg();

		fps.Update();//1サイクルごとの速度を測定
		if(  key_buf[ KEY_INPUT_F ] == 1 ){
			draw_mtx.lock();
			fps.Draw();
			draw_mtx.unlock();
		}
		fps.Wait();//早すぎたらちょっと待つ

		// 裏画面の内容を表画面に反映させる
		draw_mtx.lock();
		DxLib::ScreenFlip();
		draw_mtx.unlock();

		// Windows システムからくる情報を処理する
		if( ProcessMessage() == -1 ){
			thread_flag = false; //スレッド終了連絡
			th.join();//スレッドが終了するのを待つ
			break ;
		}

		

		// ＥＳＣキーが押されたらループから抜ける
		if( key_buf[ KEY_INPUT_ESCAPE ] == 1 ){
			thread_flag = false; //スレッド終了連絡
			th.join();//スレッドが終了するのを待つ
			break ;
		}
	}
	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;

	// ソフトの終了
	return 0 ;
}


void image_get_process() {

	while(thread_flag){

		//静止画をopenCVで取得
		Mat image;

		if(USE_CAMERA_FLAG == 0){

			// DXライブラリで静止画取得 
			camera_image_size = (int)FileRead_size("out_.jpeg");
			vector<char> buf(camera_image_size,0);
			int FileHandle = FileRead_open("out_.jpeg");

			FileRead_read(&buf[0],buf.size(),FileHandle );
			FileRead_close(FileHandle);

			draw_mtx.lock();//排他的処理
			camera_image_handle = CreateGraphFromMem(&buf[0] , buf.size());
			draw_mtx.unlock();

		}else if(USE_CAMERA_FLAG == 1 || USE_CAMERA_FLAG == 2){
			if(!vcap.read(image)) {
				std::cout << "No frame" << std::endl;
				MessageBox(NULL,"画像フレーム取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
				cv::waitKey();
				return;
			}

			mytank_mtx.lock();
			mytank->detect_enemy(image);
			mytank_mtx.unlock();

			cv::imwrite("out.jpeg",image);

			// DXライブラリで静止画取得 
			camera_image_size = (int)FileRead_size("out.jpeg");
			vector<char> buf(camera_image_size,0);
			int FileHandle = FileRead_open("out.jpeg");

			FileRead_read(&buf[0],buf.size(),FileHandle );
			FileRead_close(FileHandle);

			draw_mtx.lock();//排他的処理
			camera_image_handle = CreateGraphFromMem(&buf[0], buf.size());
			draw_mtx.unlock();

		}
	}
	
}

//ゲームの初期化
void init(){
	

	//描画リストの要素をすべて削除
	CObject::drawlist.clear();

	//スマートポインタ生成
	auto mytank_ = make_shared<CMytank>();
	mytank_mtx.lock();
	mytank = mytank_;
	mytank_mtx.unlock();

	auto system_timer_ = make_shared<CSystem_timer>(10,10,GAME_TIME);
	system_timer = system_timer_;

	if(mytank->get_id() != 0)mytank->enemy0->exist = exist_flag[0];
	if(mytank->get_id() != 1)mytank->enemy1->exist = exist_flag[1];
	if(mytank->get_id() != 2)mytank->enemy2->exist = exist_flag[2];
	if(mytank->get_id() != 3)mytank->enemy3->exist = exist_flag[3];

	//色々描画リストに登録
	//ここ大事。object.h見て
	//後ここ参考　http://marupeke296.com/DXCLS_WhoIsDrawer.html

	CObject::register_object(mytank,DRAW_LAYER::MYTANK_LAYER);

	CObject::register_object(system_timer,DRAW_LAYER::IMFOMATION_LAYER);

	

}

//draw_listのソートに使用
bool list_cmp(std::shared_ptr<CObject>& v1,std::shared_ptr<CObject>& v2 ){
	if(v1->layer_id < v2->layer_id){
		return true;
	}else return false;

}

//色認識、IP初期化
int configuration(){
	int ip[4];
	//int FileHandle=FileRead_open("data/hsv.csv");
	//if(FileHandle==0){
	//	return 1;
	//}
	//for(int i=0;i<4;i++){
	//	FileRead_scanf(FileHandle,"%d,%d,%d,%d",&hsv[i][0],&hsv[i][1],&hsv[i][2],&hsv[i][3]);
	//	if(hsv[i][0]<0 || hsv[i][1]<0 || hsv[i][2]<0 || hsv[i][3]<0){
	//		FileRead_close(FileHandle);
	//		return 1;
	//	}
	//}

	std::ifstream hsv_file( "data/hsv.csv" );
	std::string str;

	hsv_file >> str;
	int result = sscanf_s(str.c_str(),"1p:%d,%d,%d,%d,%d,%d,2p:%d,%d,%d,%d,%d,%d,3p:%d,%d,%d,%d,%d,%d,4p:%d,%d,%d,%d,%d,%d,corner:%d,%d,%d,%d,%d,%d",
		&(hsv_data.player[0].minH),&(hsv_data.player[0].maxH),&(hsv_data.player[0].minS),&(hsv_data.player[0].maxS),&(hsv_data.player[0].minV),&(hsv_data.player[0].maxV),
		&(hsv_data.player[1].minH),&(hsv_data.player[1].maxH),&(hsv_data.player[1].minS),&(hsv_data.player[1].maxS),&(hsv_data.player[1].minV),&(hsv_data.player[1].maxV),
		&(hsv_data.player[2].minH),&(hsv_data.player[2].maxH),&(hsv_data.player[2].minS),&(hsv_data.player[2].maxS),&(hsv_data.player[2].minV),&(hsv_data.player[2].maxV),
		&(hsv_data.player[3].minH),&(hsv_data.player[3].maxH),&(hsv_data.player[3].minS),&(hsv_data.player[3].maxS),&(hsv_data.player[3].minV),&(hsv_data.player[3].maxV),
		&(hsv_data.corner.minH),&(hsv_data.corner.maxH),&(hsv_data.corner.minS),&(hsv_data.corner.maxS),&(hsv_data.corner.minV),&(hsv_data.corner.maxV));
	if(result != 30){ //30個の入力どれかに失敗した場合
			//cout << "hsv.csv load failed" <<endl;
			
			return 1;
				
	}

	ifstream ip_file("data/ip.csv");
	string ip_data;
	if(ip_file.fail()){
		return 2;
	}
	ip_file >> ip_data;
	if(sscanf_s(ip_data.c_str(),"%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3]) != 4) return 2;

	if(ip[0]<0 || ip[1]<0 || ip[2]<0 ||ip[3]<0){
		return 2;
	}
	SERVER_IP_ADDRESS = to_string(ip[0]) + "." + to_string(ip[1]) + "." + to_string(ip[2]) + "." + to_string(ip[3]);

	ifstream playnum_file("data/playernum.csv");
	if(playnum_file.fail()){
		return 3;
	}
	string row;
	int tmp[5];
	playnum_file >> row;

	if(sscanf_s(row.c_str(),"rpi:%d,%d,%d,%d,id:%d", &tmp[0],&tmp[1],&tmp[2],&tmp[3],&tmp[4]) != 5) return 3;
	PLAYER_NM = tmp[4];
	RASPI_IP_ADDRESS =  to_string(tmp[0]) + "." + to_string(tmp[1]) + "." + to_string(tmp[2]) + "." + to_string(tmp[3]);

	if(PLAYER_NM<0){
		return 3;
	}

	//sprintf_s(RASPI_IP_ADDRESS,"pi@rpi0%d.local",PLAYER_NM);
	//RASPI_IP_ADDRESS = "pi@rpi0" + to_string(PLAYER_NM) + ".local";
	return 0;
}