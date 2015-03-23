
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

	//if(!vcap.open(1)){//�f�t�H���g�̃J�������擾�͂�����
	//if(!vcap.open(videoStreamAddress)) { �@//���Y�p�C����̎擾�͂�����
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// �E�C���h�E���[�h�ŋN��
	SetMainWindowText( "���A���}���I�J�[�g" ) ;
	ChangeWindowMode( TRUE ) ;
	SetGraphMode(1000,750,32);

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// �`���𗠉�ʂɂ���
	SetDrawScreen( DX_SCREEN_BACK ) ;

	//network������
	network->init(0,"192.168.11.2");
	network->send_msg("HELLO");

	//FPS����평���� �T���v����10
	 CFPSCounter FPS(10); 

	//�g�p����摜�̓ǂݍ���
	//int image2[10];
	//LoadDivGraph("suji.png",10,5,2,110,188,image2);

	// ���C�����[�v
	while(1){

		// ��ʂɕ`����Ă�����̂����S������
		ClearDrawScreen() ;

		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}
		// �t���[�����擾
		//cv::imwrite("out.jpeg",image);
		// �t���[���̓��e����ʂɕ`��B���ɍs���قǏ�ɕ\��
		LoadGraphScreen( 0 , 0 , "image/test.jpg" , FALSE ) ;
		//DrawGraph(50,50,image2[i%10],0);

		// ����ʂ̓��e��\��ʂɔ��f������
		ScreenFlip() ;
		FPS.GetFPS();
		if( CheckHitKey( KEY_INPUT_F ) == 1 ){
		
			char str[40];
			sprintf_s(str, "fps:%lf\n", FPS.GetFPS());
			OutputDebugString(str); //FPS�擾
		}

		// Windows �V�X�e�����炭�������������
		if( ProcessMessage() == -1 ) break ;

		// �d�r�b�L�[�������ꂽ�烋�[�v���甲����
		if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;
	}
	// �c�w���C�u�����g�p�̏I������
	DxLib_End() ;

	
	// �\�t�g�̏I��
	return 0 ;
}
