
#include <stdio.h>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include"DxLib.h"
#include <ctype.h>
#include<iostream>
#include "network.h"


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

    Network n1,n2,n3,n4;

	// const std::string videoStreamAddress ="http://192.168.10.221:8080/?action=stream.mjpeg";

	//if(!vcap.open(1)){//デフォルトのカメラを取得はこちら
	//if(!vcap.open(videoStreamAddress)) { 　//ラズパイからの取得はこちら
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// ウインドウモードで起動
	ChangeWindowMode( TRUE ) ;
	SetGraphMode(1000,750,32);

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ){
		return -1;
	}

	//network初期化
	n1.init(0,"192.168.11.2");
	n2.init(1,"192.168.11.2");
	n3.init(2,"192.168.11.2");
	n4.init(3,"192.168.11.2");

	n1.send_msg("HELLO WORLD");
	n2.send_msg("HELLO!!");


	//使用する画像の読み込み
	//int image2[10];
	//LoadDivGraph("suji.png",10,5,2,110,188,image2);

	// メインループ
	int i=0;
	while(CheckHitKeyAll() == 0 ){
		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}
		// フレームを取得
		//cv::imwrite("out.jpeg",image);
		// フレームの内容を画面に描画。下に行くほど上に表示
		DrawGraph(0,0,LoadGraph("image/test.jpg"),0);
		//DrawGraph(50,50,image2[i%10],0);
		i++;
	}
	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;
	// ソフトの終了
	return 0 ;
}
