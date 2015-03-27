
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

	auto network = make_shared<CNetwork>();
	auto mytank = make_shared<CMytank>();
	auto system_timer = make_shared<CSystem_timer>(10,10);

	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;


 const std::string videoStreamAddress ="http://192.168.10.137:8080/?action=stream.mjpeg";

	//if(!vcap.open(1)){//�f�t�H���g�̃J�������擾�͂�����
	if(!vcap.open(videoStreamAddress)) { //���Y�p�C����̎擾�͂�����
		std::cout << "Error opening video stream or file" << std::endl;
		return -1;
	}
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

	//�G�N���X������
	enemy1->init(80,180,100,200,100,200);
	enemy2->init(0,30,100,200,100,200);
	enemy3->init(30,80,100,200,100,200);
	

	//�F�X�`��ɓo�^
	drawlist.push_back(mytank);
	drawlist.push_back(system_timer);
	drawlist.push_back(enemy1);
	drawlist.push_back(enemy2);
	drawlist.push_back(enemy3);

	// ���C�����[�v
	while(1){

		// ��ʂɕ`����Ă�����̂����S������
		ClearDrawScreen() ;

		if(!vcap.read(image)) {
			std::cout << "No frame" << std::endl;
			cv::waitKey();
			return -1;
		}
		//�t���[�����擾
		enemy1->detect(image);
		enemy2->detect(image);
		enemy3->detect(image);

		

		cv::imwrite("out.jpeg",image);

		  // test1.bmp�̓ǂݍ���
		int GHandle = LoadGraph( "out.jpeg" ) ;

		// �ǂ݂��񂾃O���t�B�b�N���g��`��
		DrawExtendGraph( 0 , 0 , 1000  , 750 , GHandle , TRUE ) ;

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
		for(int i=0;i<256;i++){
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
			auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);
			drawlist.push_back(bullet);
		}
		//�e�X�g�p�@E���������^�C�~���O��Explosion����
		if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
			auto explosion = make_shared<CExplosion>(200,200,EXPLOSION_KIND::EXPLOSION_NOMAL);
			drawlist.push_back(explosion);
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
