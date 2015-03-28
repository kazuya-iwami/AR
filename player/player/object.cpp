#include "object.h"

 map<string,int> CObject::figure_id;
 map<string,int> CObject::sound_id;

void CObject::load(){
	//���̂悤�ɏ�����test.jpg���������ɓǂݍ��ށB
	//�����draw()����figure_id["F_TEST"]��p����Ƃ��̉摜�ɑΉ�����id���擾�ł���B
	//F_TEST�̕����͑��Ɣ��Ȃ��悤�Ɏ��R�Ɍ��߂Ă��������B������F_���珉�߂āB
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");

	//��̌��ʉ���
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
}

bool CObject::draw(){

	//���z�֐��Ȃ̂Œ��g�Ȃ�
	return true;

}
