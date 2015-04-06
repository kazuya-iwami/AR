
#include "bullet.h"

CBullet::CBullet(int x_, int y_, int z_, BULLET_KIND kind_){
	x = x_;
	y = y_;
	z = z_;
	kind = kind_;
	draw_timer = 0;
}

bool CBullet::draw(){
	// 動作確認用 文字出力
	//DrawFormatString(0, 20, GetColor(255,255,255), "bullet:%d", draw_timer);

	//描画開始と同時に音声発生
	if(draw_timer==0) PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;

	switch(kind){
	case BULLET_KIND::BULLET_NOMAL :
		DrawGraph(x-128,y-128, bullet[draw_timer], true);
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
