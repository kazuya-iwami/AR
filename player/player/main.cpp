
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

	//if(!vcap.open(1)){//�f�t�H���g�̃J�������擾�͂�����
	//if(!vcap.open(videoStreamAddress)) { �@//���Y�p�C����̎擾�͂�����
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// �E�C���h�E���[�h�ŋN��
	ChangeWindowMode( TRUE ) ;
	SetGraphMode(1000,750,32);

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ){
		return -1;
	}

	//network������
	n1.init(0,"192.168.11.2");
	n2.init(1,"192.168.11.2");
	n3.init(2,"192.168.11.2");
	n4.init(3,"192.168.11.2");

	n1.send_msg("HELLO WORLD");
	n2.send_msg("HELLO!!");


	//�g�p����摜�̓ǂݍ���
	//int image2[10];
	//LoadDivGraph("suji.png",10,5,2,110,188,image2);

	// ���C�����[�v
	int i=0;
	while(CheckHitKeyAll() == 0 ){
		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}
		// �t���[�����擾
		//cv::imwrite("out.jpeg",image);
		// �t���[���̓��e����ʂɕ`��B���ɍs���قǏ�ɕ\��
		DrawGraph(0,0,LoadGraph("image/test.jpg"),0);
		//DrawGraph(50,50,image2[i%10],0);
		i++;
	}
	// �c�w���C�u�����g�p�̏I������
	DxLib_End() ;
	// �\�t�g�̏I��
	return 0 ;
}
