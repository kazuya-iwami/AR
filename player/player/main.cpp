
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

using namespace std;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
	cv::VideoCapture vcap;
	cv::Mat image;

	//�e�w�b�_�t�@�C���������class�\�����킩���

	//network������
	CNetwork::network_init(0,"192.168.10.126"); //�����̃v���C���[�ԍ�0~3��IP�A�h���X�����Ɛڑ����݂�

	//�N���X�̃C���X�^���X�̓X�}�[�g�|�C���^(std::shared_ptr)�Ő������܂��B
	//�X�}�[�g�|�C���^�̏ڍׂ̓O�O����
	//���ʂ̃|�C���^�ƈ���ēK�؂ȃ^�C�~���O�ŉ�����Ă����֗��O�b�Y
	//�[���l�����Ɉȉ��̏������R�s��Γ����B
	//�����Ԉ���Ă��狳����
	//�O���[�o���ϐ���main.h�ɏ����Ă�
	//���̃t�@�C���ŗp���鎞��extern���āiextern�ŃO�O��ׂ��j

	auto mytank = make_shared<CMytank>();
	auto system_timer = make_shared<CSystem_timer>(10,10);
	auto mycursur=make_shared<CCursur>();

	//�L�[�{�[�h�p
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	CPopup::debug_flag = true; // �f�o�b�O�t���O��true�Ȃ�|�b�v�A�b�v��\��

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

	//�����T�C�Y
	SetFontSize( 40 ) ;                             //�T�C�Y��20�ɕύX
    SetFontThickness( 1 ) ;                         //������1�ɕύX
    ChangeFont( "HGS�n�p�p�߯�ߑ�" ) ;              //HGS�n�p�p�߯�ߑ̂ɕύX
    ChangeFontType( DX_FONTTYPE_ANTIALIASING_EDGE );//�A���`�G�C���A�X���G�b�W�t���t�H���g�ɕύX

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ){
		return -1;
	}

	// �`���𗠉�ʂɂ���
	SetDrawScreen( DX_SCREEN_BACK ) ;

	
	//FPS����평���� 
	//����ɂ����1�b�ɍő�30�񂵂����[�v�����Ȃ��悤�ɂ���
	//fps�Œ肵�Ȃ��ƈ�T�C�N���Ɉ�萔�����悤�ȃv���O���������܂�����
	CFps fps; 

	//�g�p����摜�̓ǂݍ���
	CObject::load();//���ׂẲ摜�͂��̒��œǂݍ��ށ@


	//�F�X�`�惊�X�g�ɓo�^
	//�����厖�Bobject.h����
	//�ケ���Q�l�@http://marupeke296.com/DXCLS_WhoIsDrawer.html

	CObject::register_object(mytank);
	CObject::register_object(system_timer);
	CObject::register_object(mycursur);

	float bullet_z = 0.0;
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

		mytank->detect_enemy(image);

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
		for(it=CObject::drawlist.begin(); it!=CObject::drawlist.end();){  // �Ō�́uit++�v������
			if( !(*it)->draw() ){ //�A�j���[�V�����I����
				// �I�u�W�F�N�g�����X�g����͂���
				it = CObject::drawlist.erase( it );
				continue;
			}
			it++;   // �C���N�������g
		}
		
		//�T�[�o�[����msg�̎�M
		/* 1�T�C�N��1��Ă� */
		mytank->get_msg();

		//�ړ������@���̒��ɏ���
		//�L�[��Ԏ擾�̌�Ɉړ����܂�(2015/3/31 �吙�ǋL)
		//mytank->move();

		//�L�[��Ԏ擾
		//�������͈ȉ��̒ʂ�
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		tstring speed;
		//���x�ݒ�̕�������Ɋւ��Ă͑吙�͒m��Ȃ��ł�...
		if(true){
			//�ʏ�
			speed = _T("full");
		}else {
			//�T�[�o���猸�����߂������Ă����ꍇ
			speed = _T("half");
		}

		//�e�L�[������������Ƃ��̓��������B
		if(  key_buf[ KEY_INPUT_UP ] == 1 && key_prev_buf[ KEY_INPUT_UP] == 0 ){
			//mytank->set_vel(1,1);//���@�̑��x�ݒ� (pwm����̏ꍇ��)
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
		//�e�L�[�𗣂�����stop
		if( (key_buf[ KEY_INPUT_UP ] == 0 && key_prev_buf[ KEY_INPUT_UP] == 1) || 
			(key_buf[ KEY_INPUT_DOWN ] == 0 && key_prev_buf[ KEY_INPUT_DOWN] == 1) ||
			(key_buf[ KEY_INPUT_LEFT ] == 0 && key_prev_buf[ KEY_INPUT_LEFT] == 1) ||
			(key_buf[ KEY_INPUT_RIGHT ] == 0 && key_prev_buf[ KEY_INPUT_RIGHT] == 1) ){
			mytank->move(_T("stop"), speed);
		}

		//�e�X�g�p�@B���������^�C�~���O��Bullet����
		if(  key_buf[ KEY_INPUT_SPACE ] == 1 && key_prev_buf[ KEY_INPUT_SPACE] == 0){
			mytank->gen_bullet(BULLET_KIND::BULLET_NOMAL);
		}
		//�e�X�g�p�@3���������^�C�~���O��3D��(Bullet)����
		if(  key_buf[ KEY_INPUT_3 ] == 1 && key_prev_buf[ KEY_INPUT_3] == 0){
			auto bullet = make_shared<CBullet>(0, 0, 0, BULLET_KIND::BULLET_3D);
			CObject::register_object(bullet);
		}


		//�e�X�g�p�@E���������^�C�~���O��Explosion����
		if(  key_buf[ KEY_INPUT_E ] == 1 && key_prev_buf[ KEY_INPUT_E] == 0){
			auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_NOMAL);
			CObject::register_object(explosion);
		}
		//�e�X�g�p�@1���������^�C�~���O��Explosion����
		if(  key_buf[ KEY_INPUT_1 ] == 1 && key_prev_buf[ KEY_INPUT_1] == 0){
			auto explosion = make_shared<CExplosion>(530 , 50, EXPLOSION_KIND::EXPLOSION_1);
			CObject::register_object(explosion);
		}
		//�e�X�g�p�@I���������^�C�~���O��Item����
		if(  key_buf[ KEY_INPUT_I ] == 1 && key_prev_buf[ KEY_INPUT_I] == 0){
			mytank->use_item();
		}

		//�e�X�g�p�@0���������^�C�~���O�ŃX�R�A�ǉ�
		
		if(  key_buf[ KEY_INPUT_0 ] == 1 && key_prev_buf[ KEY_INPUT_0] == 0){
			score++;
		}
		
		//�e�X�g�p�@D�������ƃJ�[�\�����E��
		if(  key_buf[ KEY_INPUT_D ] == 1){
			cursur_x=cursur_x+4;
		}

		//�e�X�g�p�@W�������ƃJ�[�\�������
		if(  key_buf[ KEY_INPUT_W ] == 1 ){
			cursur_y=cursur_y-4;
		}

		//�e�X�g�p�@A�������ƃJ�[�\��������
		if(  key_buf[ KEY_INPUT_A ] == 1){
			cursur_x=cursur_x-4;
		}

		//�e�X�g�p�@S�������ƃJ�[�\��������
		if(  key_buf[ KEY_INPUT_S ] == 1){ 
			cursur_y=cursur_y+4;
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

