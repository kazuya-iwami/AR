
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

using namespace std;

#define FOCUS_SPEED 8
#define GAME_TIME 20 //プレー時間　20秒
#define FINISH_TIME 5 //結果発表の時間 5秒

#define USE_CAMERA_FLAG 1   //0:画像 1:カメラ 2:ラズパイ


bool list_cmp(std::shared_ptr<CObject>& v1,std::shared_ptr<CObject>& v2 );


cv::VideoCapture vcap;

int camera_image_handle;//スレッド処理用
std::mutex draw_mtx;
std::mutex mytank_mtx;
bool thread_flag;
int camera_image_size;

shared_ptr<CMytank> mytank;
shared_ptr<CSystem_timer> system_timer;

void image_get_process();//別スレッドで映像の受信、処理を行う
void init();


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){

	//各ヘッダファイルを見るとclass構成がわかるよ

	//network初期化
	CNetwork::network_init(0,"172.16.100.21"); //自分のプレイヤー番号0~3とIPアドレス書くと接続試みる

	//クラスのインスタンスはスマートポインタ(std::shared_ptr)で生成します。
	//スマートポインタの詳細はググって
	//普通のポインタと違って適切なタイミングで解放してくれる便利グッズ
	//深く考えずに以下の書き方コピれば動く。
	//何か間違ってたら教えて
	//グローバル変数はmain.hに書いてる
	//他のファイルで用いる時はexternして（externでググるべし）

	
	//auto redback=make_shared<CEffect>();

	int finish_timer=0; //結果発表画面用タイマー

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
		const std::string videoStreamAddress ="http://192.168.10.137:8080/?action=stream.mjpeg";

		if(!vcap.open(videoStreamAddress)) { //ラズパイからの取得はこちら
			std::cout << "Error opening video stream or file" << std::endl;
			MessageBox(NULL,"カメラ取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
			return -1;
		}
	}



	// ウインドウモードで起動
	SetMainWindowText( "Real Tank Battle -４機の戦車が集いし時、現実と仮想空間が交差する-" ) ;
	ChangeWindowMode( TRUE ) ;//falseならフルスクリーン
	SetGraphMode(1350,730,32);//画面サイズ1000×750に設定
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

	init(); //ゲームの初期化
	

	while(GetASyncLoadNum() > 0){ //全て読み込むまでスレッド立ち上げない
		WaitTimer(10);
	}

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

		//サーバーからmsgの受信
		/* 1サイクル1回呼ぶ */
		mytank->get_msg();

		if(mytank->get_game_status() == GAME_STATUS::GAME_WAIT){

			draw_mtx.lock();
			DrawFormatString(50 + LEFT_WINDOW_WIDTH, 300, GetColor(255,255,255), "Waiting... ENTERでスタート");
			draw_mtx.unlock();

			if(  key_buf[ KEY_INPUT_RETURN ] == 1 && key_prev_buf[ KEY_INPUT_RETURN] == 0){
				mytank->set_game_status(GAME_STATUS::GAME_PLAY);
				key_prev_buf[ KEY_INPUT_RETURN] = 1; //他の条件に引っかからないよう細工
			}

			
		}else if(mytank->get_game_status() == GAME_STATUS::GAME_PLAY){


			// 読みこんだグラフィックを拡大描画
			draw_mtx.lock(); //排他的処理
			DrawExtendGraph( mytank->shake_x + LEFT_WINDOW_WIDTH , mytank->shake_y,
				1000+mytank->shake_x + LEFT_WINDOW_WIDTH  , 750+mytank->shake_y , camera_image_handle, false ) ;
			draw_mtx.unlock();

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


			//照準と敵が重なっているかチェック
			mytank->check_focus();

			//振動処理
			mytank->shake();

			//移動処理　この中に書く
			//キー状態取得の後に移動します(2015/3/31 大杉追記)
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

			if(  key_buf[ KEY_INPUT_SPACE ] == 1 && key_prev_buf[ KEY_INPUT_SPACE] == 0){
				mytank->gen_bullet(BULLET_KIND::BULLET_NOMAL);
			}
			//テスト用　3を押したタイミングで3D球(Bullet)生成
			if(  key_buf[ KEY_INPUT_3 ] == 1 && key_prev_buf[ KEY_INPUT_3] == 0){
				auto bullet = make_shared<CBullet>(0, 0, 0, BULLET_KIND::BULLET_3D);
				CObject::register_object(bullet,DRAW_LAYER::BULLET_LAYER);
			}


			//テスト用　Eを押したタイミングでExplosion生成
			if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
				auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_NOMAL);
				CObject::register_object(explosion,DRAW_LAYER::EXPLOSION_LAYER);
			}
			//テスト用　1を押したタイミングでExplosion生成
			if(  key_buf[ KEY_INPUT_1 ] == 1 && key_prev_buf[ KEY_INPUT_1] == 0){
				auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_1);
				CObject::register_object(explosion,DRAW_LAYER::EXPLOSION_LAYER);
			}
			//テスト用　Iを押したタイミングでItem生成
			if(  key_buf[ KEY_INPUT_I ] == 1 && key_prev_buf[ KEY_INPUT_I] == 0){
				mytank->use_item();
			}


			//テスト用　Dを押すとカーソルが右に
			if(  key_buf[ KEY_INPUT_D ] == 1){
				mytank->focus_x += FOCUS_SPEED;
			}

			//テスト用　Wを押すとカーソルが上に
			if(  key_buf[ KEY_INPUT_W ] == 1 ){
				mytank->focus_y -=  FOCUS_SPEED;
			}

			//テスト用　Aを押すとカーソルが左に
			if(  key_buf[ KEY_INPUT_A ] == 1){
				mytank->focus_x -=  FOCUS_SPEED;
			}

			//テスト用　Sを押すとカーソルが下に
			if(  key_buf[ KEY_INPUT_S ] == 1){ 
				mytank->focus_y +=  FOCUS_SPEED;
			}

			//テスト用　とりあえずX押したら画面が振動するよ
			if(key_buf[KEY_INPUT_X]==1 && key_prev_buf[KEY_INPUT_X]==0){

				mytank->shake_start(SHAKE_STATUS::BIG_SHAKE);
			}

			//ENTERでGAME_STAUTS変更
			if(  key_buf[ KEY_INPUT_RETURN ] == 1 && key_prev_buf[ KEY_INPUT_RETURN] == 0){
				mytank->set_game_status(GAME_STATUS::GAME_PAUSE);
				finish_timer = FINISH_TIME*30;
			}

			//時間切れるとGAME_STATUS変更
			if(system_timer->get_finish_flag()){
				mytank->set_game_status(GAME_STATUS::GAME_FINISH);
				finish_timer = FINISH_TIME*30;
				//時間切れの処理
			}

		}else if(mytank->get_game_status() == GAME_STATUS::GAME_PAUSE){

			draw_mtx.lock();
			DrawFormatString(50 + LEFT_WINDOW_WIDTH, 300, GetColor(255,255,255), "PAUSE... ENTERで戻る");
			draw_mtx.unlock();

			//ENTERでGAME_STAUTS変更
			if(  key_buf[ KEY_INPUT_RETURN ] == 1 && key_prev_buf[ KEY_INPUT_RETURN] == 0){
				mytank->set_game_status(GAME_STATUS::GAME_PLAY);
			}

		}else if(mytank->get_game_status() == GAME_STATUS::GAME_FINISH){

			draw_mtx.lock();
			DrawFormatString(50 + LEFT_WINDOW_WIDTH, 300, GetColor(255,255,255), "FINISH!!!　5秒");
			draw_mtx.unlock();

			if(finish_timer == 0){
				mytank->set_game_status(GAME_STATUS::GAME_WAIT);
				init();
			}else finish_timer--;


		}

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

void image_get_process(){

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
			camera_image_size= FileRead_size("out.jpeg");
			vector<char> buf(camera_image_size,0);
			int FileHandle = FileRead_open("out.jpeg");

			FileRead_read(&buf[0],buf.size(),FileHandle );
			FileRead_close(FileHandle);

			draw_mtx.lock();//排他的処理
			camera_image_handle = CreateGraphFromMem(&buf[0] , buf.size());
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
