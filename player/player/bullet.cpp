
#include "bullet.h"

CBullet::CBullet(int x_,int y_,BULLET_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer = 0;
}

bool CBullet::draw(){
	

	if(draw_timer==0)PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;

	switch(kind){
	case BULLET_KIND::BULLET_NOMAL :
		break;
	}

	draw_timer++;

	if(draw_timer != 300){
		return true;
	}else return false;
}
