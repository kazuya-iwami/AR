
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

using namespace std;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

	auto network = make_shared<CNetwork>();
	auto mytank = make_shared<CMytank>();

	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;


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
	CFps fps; 

	//�g�p����摜�̓ǂݍ���
	CObject::load();//���ׂẲ摜�͂��̒��œǂݍ���

	//mytank��`��ɓo�^
	drawlist.push_back(mytank);

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
		
		std::list<std::shared_ptr<CObject>>::iterator it;
		for(it=drawlist.begin(); it!=drawlist.end();){  // �Ō�́uit++�v������
			if( !(*it)->draw() ){
				// �I�u�W�F�N�g�����X�g����͂���
				it = drawlist.erase( it );
				continue;
			}
		it++;   // �����ŃC���N�������g
		}

		//�T�[�o�[����msg�̎�M
		mytank->get_msg(network->check_msg());

		//�ړ�����
		mytank->move();

		//�L�[��Ԏ擾
		for(int i;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		if(  key_buf[ KEY_INPUT_UP ] == 1 ){
			mytank->set_vel(1,1);
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
		
		//�e�X�g�p�@B���������^�C�~���O��Bullet����
		if(  key_buf[ KEY_INPUT_B ] == 1 && key_prev_buf[ KEY_INPUT_B] == 0){
			auto mytank = make_shared<CBullet>();
			drawlist.push_back(mytank);
		}
		//�e�X�g�p�@E���������^�C�~���O��Explosion����
		if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
			auto mytank = make_shared<CExplosion>();
			drawlist.push_back(mytank);
		}

		fps.Update();//���ꂪ�Ă΂�鑬�x�𑪒�
		if(  key_buf[ KEY_INPUT_F ] == 1 ){
			fps.Draw();
		}
		fps.Wait();

		// ����ʂ̓��e��\��ʂɔ��f������
		DxLib::ScreenFlip() ;

		// Windows �V�X�e�����炭�������������
		if( ProcessMessage() == -1 ) break ;

		// �d�r�b�L�[�������ꂽ�烋�[�v���甲����
		if( key_buf[ KEY_INPUT_ESCAPE ] == 1 ) break ;
	}
	// �c�w���C�u�����g�p�̏I������
	DxLib_End() ;

	
	// �\�t�g�̏I��
	return 0 ;
}
