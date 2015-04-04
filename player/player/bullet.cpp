
#include "bullet.h"

CBullet::CBullet(int x_, int y_, int z_, BULLET_KIND kind_){
	x = x_;
	y = y_;
	z = z_;
	kind = kind_;
	draw_timer = 0;
}

bool CBullet::draw(){
	// ����m�F�p �����o��
	DrawFormatString(0, 20, GetColor(255,255,255), "bullet:%d", draw_timer);

	//�`��J�n�Ɠ����ɉ�������
	if(draw_timer==0) PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;

	switch(kind){
	case BULLET_KIND::BULLET_NOMAL :
		DrawGraph(x,y, bullet[draw_timer], true);
		break;
	case BULLET_KIND::BULLET_3D :
		DrawSphere3D(VGet(x, y, z), 80.0f, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), true);
		z += 30;
		break;
	}

	draw_timer++;

	if(draw_timer != 30){
		return true;
	}else return false;
}
