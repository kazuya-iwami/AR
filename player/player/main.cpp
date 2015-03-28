
#include <stdio.h>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"DxLib.h"
#include <ctype.h>
#include<iostream>
#include "network.h"
#include <memory>
#include "FPSCounter.h"
#include "object.h"
#include "mytank.h"
#include "main.h"
#include "bullet.h"
#include "explosion.h"
#include "utility.h"

using namespace std;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

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


	// メインループ
	while(1){

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

		//すべての描画をここで受け持つ。
		//drawlistに登録されてるオブジェクトのdraw()をすべて実行
		//drawの戻り値がfalseだとリストから除く(アニメーション描画終了後falseを返す)

		std::list<std::shared_ptr<CObject>>::iterator it;
		for(it=drawlist.begin(); it!=drawlist.end();){  // 最後の「it++」を消す
			if( !(*it)->draw() ){
				// オブジェクトをリストからはずす
				it = drawlist.erase( it );
				continue;
			}
		it++;   // ここでインクリメント
		}
		
		//サーバーからmsgの受信
		//1サイクル1回呼ぶ
		mytank->get_msg(network->check_msg());

		//移動処理はこの中に書く
		mytank->move();

		//キー状態取得
		//書き方は以下の通り
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		if(  key_buf[ KEY_INPUT_UP ] == 1 ){
			mytank->set_vel(1,1);//自機の速度設定（引数適当。大杉君自由にclassいじって）
		}
		if(  key_buf[ KEY_INPUT_DOWN ] == 1 ){
			mytank->set_vel(-1,-1);
		}
		if(  key_buf[ KEY_INPUT_LEFT ] == 1 ){
			mytank->set_vel(-1,1);
		}
		if(  key_buf[ KEY_INPUT_RIGHT ] == 1 ){
			mytank->set_vel(1,-1);
		}
		
		//テスト用　Bを押したタイミングでBullet生成
		if(  key_buf[ KEY_INPUT_B ] == 1 && key_prev_buf[ KEY_INPUT_B] == 0){
			auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);
			drawlist.push_back(bullet);
		}
		//テスト用　Eを押したタイミングでExplosion生成
		if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
			auto explosion = make_shared<CExplosion>(200,200,EXPLOSION_KIND::EXPLOSION_NOMAL);
			drawlist.push_back(explosion);
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
