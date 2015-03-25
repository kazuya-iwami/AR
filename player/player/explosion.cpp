#include "explosion.h"

CExplosion::CExplosion(int x_,int y_,EXPLOSION_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer=0;
}

bool CExplosion::draw(){
	draw_timer++;

	switch(kind){
	case EXPLOSION_KIND::NOMAL :
		break;
	}

	if(draw_timer != 300){
		return true;
	}else return false;
}
