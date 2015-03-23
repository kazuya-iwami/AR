
#include <stdio.h>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"DxLib.h"
#include <ctype.h>
#include<iostream>
#include "network.h"
#include <memory>
#include "FPSCounter.h"

using namespace std;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

	unique_ptr<CNetwork> network(new CNetwork);

	// const std::string videoStreamAddress ="http://192.168.10.221:8080/?action=stream.mjpeg";

	//if(!vcap.open(1)){//デフォルトのカメラを取得はこちら
	//if(!vcap.open(videoStreamAddress)) { 　//ラズパイからの取得はこちら
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// ウインドウモードで起動
	SetMainWindowText( "リアルマリオカート" ) ;
	ChangeWindowMode( TRUE ) ;
	SetGraphMode(1000,750,32);

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// 描画先を裏画面にする
	SetDrawScreen( DX_SCREEN_BACK ) ;

	//network初期化
	network->init(0,"192.168.11.2");
	network->send_msg("HELLO");

	//FPS測定器初期化 サンプル数10
	 CFPSCounter FPS(10); 

	//使用する画像の読み込み
	//int image2[10];
	//LoadDivGraph("suji.png",10,5,2,110,188,image2);

	// メインループ
	while(1){

		// 画面に描かれているものを一回全部消す
		ClearDrawScreen() ;

		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}
		// フレームを取得
		//cv::imwrite("out.jpeg",image);
		// フレームの内容を画面に描画。下に行くほど上に表示
		LoadGraphScreen( 0 , 0 , "image/test.jpg" , FALSE ) ;
		//DrawGraph(50,50,image2[i%10],0);

		// 裏画面の内容を表画面に反映させる
		ScreenFlip() ;
		FPS.GetFPS();
		if( CheckHitKey( KEY_INPUT_F ) == 1 ){
		
			char str[40];
			sprintf_s(str, "fps:%lf\n", FPS.GetFPS());
			OutputDebugString(str); //FPS取得
		}

		// Windows システムからくる情報を処理する
		if( ProcessMessage() == -1 ) break ;

		// ＥＳＣキーが押されたらループから抜ける
		if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;
	}
	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;

	
	// ソフトの終了
	return 0 ;
}
