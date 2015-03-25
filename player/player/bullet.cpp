
#include "bullet.h"

CBullet::CBullet(int x_,int y_,BULLET_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer = 0;
}

bool CBullet::draw(){
	draw_timer++;

	switch(kind){
	case BULLET_KIND::NOMAL :
		break;
	}

	if(draw_timer != 300){
		return true;
	}else return false;
}
