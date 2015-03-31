
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

	//�R���\�[���̕\��
	AllocConsole();

	//�e�w�b�_�t�@�C���������class�\�����킩���

	//�N���X�̃C���X�^���X�̓X�}�[�g�|�C���^(std::shared_ptr)�Ő������܂��B
	//�X�}�[�g�|�C���^�̏ڍׂ̓O�O����
	//���ʂ̃|�C���^�ƈ���ēK�؂ȃ^�C�~���O�ŉ�����Ă����֗��O�b�Y
	//�[���l�����Ɉȉ��̏������R�s��Γ����B
	//�����Ԉ���Ă��狳����
	//�O���[�o���ϐ���main.h�ɏ����Ă�
	//���̃t�@�C���ŗp���鎞��extern���āiextern�ŃO�O��ׂ��j

	auto mytank = make_shared<CMytank>();
	auto system_timer = make_shared<CSystem_timer>(10,10);

	//�L�[�{�[�h�p
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	//���Y�p�C����̉f���擾�p
	const std::string videoStreamAddress ="http://192.168.10.137:8080/?action=stream.mjpeg";
	
	//���Y�p�C������10fps�ŉ摜�z�M���Ă�̂ŕʃX���b�h�i�񓯊��j�ŉ摜�擾���邱�Ƃɂ���\��
	//CG�`���30fps�̗\��

	//if(!vcap.open(1)){//�f�t�H���g�̃J�������擾�͂�����
	//if(!vcap.open(videoStreamAddress)) { //���Y�p�C����̎擾�͂�����
	//	std::cout << "Error opening video stream or file" << std::endl;
	//	return -1;
	//}
	// �E�C���h�E���[�h�ŋN��
	SetMainWindowText( "���A���}���I�J�[�g" ) ;
	ChangeWindowMode( TRUE ) ;//false�Ȃ�t���X�N���[��
	SetGraphMode(1000,750,32);//��ʃT�C�Y1000�~750�ɐݒ�

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// �`���𗠉�ʂɂ���
	SetDrawScreen( DX_SCREEN_BACK ) ;

	//network������
	network->init(0,"192.168.43.152"); //�����̃v���C���[�ԍ�0~3��IP�A�h���X�����Ɛڑ����݂�
	network->send_msg("HELLO"); //�T�[�o�[�Ƀ��b�Z�[�W����ۂ͂�������

	//FPS����평���� 
	//����ɂ����1�b�ɍő�30�񂵂����[�v�����Ȃ��悤�ɂ���
	//fps�Œ肵�Ȃ��ƈ�T�C�N���Ɉ�萔�����悤�ȃv���O���������܂�������
	CFps fps; 

	//�g�p����摜�̓ǂݍ���
	CObject::load();//���ׂẲ摜�͂��̒��œǂݍ��ށ@

	//���@��IP�A�h���X�̐ݒ�
	mytank->set_ip_address(_T("192.168.10.125"));

	//�G�N���X������
	enemy1->init(80,180,100,200,100,200);//�X�}�[�g�|�C���^���Ĕz��ɑΉ����ĂȂ��悤�Ȃ�ł�������
	enemy2->init(0,30,100,200,100,200);//������HSV�F�̈�i�O�O���āj��min��max���w�肷��Ƃ��̐F�̏d�S���擾�ł���
	enemy3->init(30,80,100,200,100,200);
	

	//�F�X�`�惊�X�g�ɓo�^
	//�����厖�Bobject.h����
	//�ケ���Q�l�@http://marupeke296.com/DXCLS_WhoIsDrawer.html

	drawlist.push_back(mytank);
	drawlist.push_back(system_timer);
	drawlist.push_back(enemy1);
	drawlist.push_back(enemy2);
	drawlist.push_back(enemy3);


	// ���C�����[�v
	while(1){
		printf("ok\n");
		// ��ʂɕ`����Ă�����̂����S������
		ClearDrawScreen() ;

		//�Î~���openCV�Ŏ擾
		image = imread("out.jpeg");
		//if(!vcap.read(image)) {
		//	std::cout << "No frame" << std::endl;
		//	cv::waitKey();
		//	return -1;
		//}

		//�G�̈ʒu�f�[�^�X�V
		enemy1->detect(image);
		enemy2->detect(image);
		enemy3->detect(image);

		

		//cv::imwrite("out.jpeg",image);

		// DX���C�u�����ŐÎ~��擾 
		int GHandle = LoadGraph( "out.jpeg" ) ;

		// �ǂ݂��񂾃O���t�B�b�N���g��`��
		DrawExtendGraph( 0 , 0 , 1000  , 750 , GHandle , TRUE ) ;


		//�`��
		/*	
			���ׂĂ̕`��������Ŏ󂯎��B
			drawlist�ɓo�^����Ă�I�u�W�F�N�g��draw()�����ׂĎ��s
			draw�̖߂�l��false���ƃ��X�g���珜��(�A�j���[�V�����`��I����false��Ԃ�)
		*/
		std::list<std::shared_ptr<CObject>>::iterator it;
		for(it=drawlist.begin(); it!=drawlist.end();){  // �Ō�́uit++�v������
			if( !(*it)->draw() ){ //�A�j���[�V�����I����
				// �I�u�W�F�N�g�����X�g����͂���
				it = drawlist.erase( it );
				continue;
			}
			it++;   // �C���N�������g
		}
		
		//�T�[�o�[����msg�̎�M
		/* 1�T�C�N��1��Ă� */
		mytank->get_msg(network->check_msg());

		//�ړ������@���̒��ɏ���
		//�L�[��Ԏ擾�̌�Ɉړ����܂�(2015/3/31 �吙�ǋL)
		//mytank->move();

		//�L�[��Ԏ擾
		//�������͈ȉ��̒ʂ�
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		if(  key_buf[ KEY_INPUT_UP ] == 1 ){
			//mytank->set_vel(1,1);//���@�̑��x�ݒ� (pwm����̏ꍇ��)
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
		//�e�X�g�p�@1���������^�C�~���O��Explosion����
		if(  key_buf[ KEY_INPUT_1 ] == 1 && key_prev_buf[ KEY_INPUT_1] == 0){
			auto explosion = make_shared<CExplosion>(200,200,EXPLOSION_KIND::EXPLOSION_1);
			drawlist.push_back(explosion);
		}

		fps.Update();//1�T�C�N�����Ƃ̑��x�𑪒�
		if(  key_buf[ KEY_INPUT_F ] == 1 ){
			fps.Draw();
		}
		fps.Wait();//���������炿����Ƒ҂�

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
