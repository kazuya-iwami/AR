
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

using namespace std;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

	//コンソールの表示
	AllocConsole();

	//各ヘッダファイルを見るとclass構成がわかるよ

	//クラスのインスタンスはスマートポインタ(std::shared_ptr)で生成します。
	//スマートポインタの詳細はググって
	//普通のポインタと違って適切なタイミングで解放してくれる便利グッズ
	//深く考えずに以下の書き方コピれば動く。
	//何か間違ってたら教えて
	//グローバル変数はmain.hに書いてる
	//他のファイルで用いる時はexternして（externでググるべし）

	auto mytank = make_shared<CMytank>();
	auto system_timer = make_shared<CSystem_timer>(10,10);
	auto mycursur=make_shared<CCursur>();

	//キーボード用
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	//ラズパイからの映像取得用
	const std::string videoStreamAddress ="http://192.168.10.137:8080/?action=stream.mjpeg";
	
	//ラズパイが現在10fpsで画像配信してるので別スレッド（非同期）で画像取得することにする予定
	//CG描画は30fpsの予定

	//if(!vcap.open(1)){//デフォルトのカメラを取得はこちら
	//if(!vcap.open(videoStreamAddress)) { //ラズパイからの取得はこちら
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// ウインドウモードで起動
	SetMainWindowText( "リアルマリオカート" ) ;
	ChangeWindowMode( TRUE ) ;//falseならフルスクリーン
	SetGraphMode(1000,750,32);//画面サイズ1000×750に設定

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// 描画先を裏画面にする
	SetDrawScreen( DX_SCREEN_BACK ) ;

	//network初期化
	network->init(0,"192.168.43.152"); //自分のプレイヤー番号0~3とIPアドレス書くと接続試みる
	network->send_msg("HELLO"); //サーバーにメッセージ送る際はこう書く

	//FPS測定器初期化 
	//これによって1秒に最大30回しかループが回らないようにする
	//fps固定しないと一サイクルに一定数動くようなプログラムがうまく動かん
	CFps fps; 

	//使用する画像の読み込み
	CObject::load();//すべての画像はこの中で読み込む　

	//自機のIPアドレスの設定
	mytank->set_ip_address(_T("192.168.10.125"));

	//敵クラス初期化
	enemy1->init(80,180,100,200,100,200);//スマートポインタって配列に対応してないようなんですが何事
	enemy2->init(0,30,100,200,100,200);//ここでHSV色領域（ググって）のminとmaxを指定するとその色の重心が取得できる
	enemy3->init(30,80,100,200,100,200);
	

	//色々描画リストに登録
	//ここ大事。object.h見て
	//後ここ参考　http://marupeke296.com/DXCLS_WhoIsDrawer.html

	drawlist.push_back(mytank);
	drawlist.push_back(system_timer);
	drawlist.push_back(enemy1);
	drawlist.push_back(enemy2);
	drawlist.push_back(enemy3);
	drawlist.push_back(mycursur);

	float bullet_z = 0.0;
	// メインループ
	while(1){
		printf("ok\n");
		// 画面に描かれているものを一回全部消す
		ClearDrawScreen() ;

		//静止画をopenCVで取得
		image = imread("out.jpeg");
		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}

		//敵の位置データ更新
		enemy1->detect(image);
		enemy2->detect(image);
		enemy3->detect(image);

		

		//cv::imwrite("out.jpeg",image);

		// DXライブラリで静止画取得 
		int GHandle = LoadGraph( "out.jpeg" ) ;

		// 読みこんだグラフィックを拡大描画
		DrawExtendGraph( 0 , 0 , 1000  , 750 , GHandle , TRUE ) ;


		//描画
		/*	
			すべての描画をここで受け持つ。
			drawlistに登録されてるオブジェクトのdraw()をすべて実行
			drawの戻り値がfalseだとリストから除く(アニメーション描画終了後falseを返す)
		*/
		std::list<std::shared_ptr<CObject>>::iterator it;
		for(it=drawlist.begin(); it!=drawlist.end();){  // 最後の「it++」を消す
			if( !(*it)->draw() ){ //アニメーション終了時
				// オブジェクトをリストからはずす
				it = drawlist.erase( it );
				continue;
			}
			it++;   // インクリメント
		}
		
		//サーバーからmsgの受信
		/* 1サイクル1回呼ぶ */
		mytank->get_msg(network->check_msg());

		//移動処理　この中に書く
		//キー状態取得の後に移動します(2015/3/31 大杉追記)
		//mytank->move();

		//キー状態取得
		//書き方は以下の通り
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		if(  key_buf[ KEY_INPUT_UP ] == 1 ){
			//mytank->set_vel(1,1);//自機の速度設定 (pwm制御の場合か)
			mytank->move(_T("forward"));
			printf("forward\n");
		}
		if(  key_buf[ KEY_INPUT_DOWN ] == 1 ){
			//mytank->set_vel(-1,-1);
			mytank->move(_T("backward"));
			printf("backward\n");
		}
		if(  key_buf[ KEY_INPUT_LEFT ] == 1 ){
			//mytank->set_vel(-1,1);
			mytank->move(_T("left"));
			printf("left\n");
		}
		if(  key_buf[ KEY_INPUT_RIGHT ] == 1 ){
			//mytank->set_vel(1,-1);
			mytank->move(_T("right"));
			printf("right\n");
		}
		
		//テスト用　Bを押したタイミングでBullet生成
		if(  key_buf[ KEY_INPUT_B ] == 1 && key_prev_buf[ KEY_INPUT_B] == 0){
			auto bullet = make_shared<CBullet>(530 , 50, 0, BULLET_KIND::BULLET_NOMAL);
			drawlist.push_back(bullet);
		}
		//テスト用　3を押したタイミングで3D球(Bullet)生成
		if(  key_buf[ KEY_INPUT_3 ] == 1 && key_prev_buf[ KEY_INPUT_3] == 0){
			auto bullet = make_shared<CBullet>(0, 0, 0, BULLET_KIND::BULLET_3D);
			drawlist.push_back(bullet);
		}


		//テスト用　Eを押したタイミングでExplosion生成
		if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
			auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_NOMAL);
			drawlist.push_back(explosion);
		}
		//テスト用　1を押したタイミングでExplosion生成
		if(  key_buf[ KEY_INPUT_1 ] == 1 && key_prev_buf[ KEY_INPUT_1] == 0){
			auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_1);
			drawlist.push_back(explosion);
		}

		//テスト用　0を押したタイミングでスコア追加
		if(  key_buf[ KEY_INPUT_0 ] == 1 && key_prev_buf[ KEY_INPUT_0] == 0){
			score++;
		}
		
		//テスト用　Dを押すとカーソルが右に
		if(  key_buf[ KEY_INPUT_D ] == 1){
			cursur_x=cursur_x+4;
		}

		//テスト用　Wを押すとカーソルが上に
		if(  key_buf[ KEY_INPUT_W ] == 1 ){
			cursur_y=cursur_y-4;
		}

		//テスト用　Aを押すとカーソルが左に
		if(  key_buf[ KEY_INPUT_A ] == 1){
			cursur_x=cursur_x-4;
		}

		//テスト用　Sを押すとカーソルが下に
		if(  key_buf[ KEY_INPUT_S ] == 1){ 
			cursur_y=cursur_y+4;
		}


		fps.Update();//1サイクルごとの速度を測定
		if(  key_buf[ KEY_INPUT_F ] == 1 ){
			fps.Draw();
		}
		fps.Wait();//早すぎたらちょっと待つ

		// 裏画面の内容を表画面に反映させる
		DxLib::ScreenFlip() ;

		// Windows システムからくる情報を処理する
		if( ProcessMessage() == -1 ) break ;

		// ＥＳＣキーが押されたらループから抜ける
		if( key_buf[ KEY_INPUT_ESCAPE ] == 1 ) break ;
	}
	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;

	
	// ソフトの終了
	return 0 ;
}
