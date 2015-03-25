#include "explosion.h"

CExplosion::CExplosion(int x_,int y_,EXPLOSION_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
}

bool CExplosion::draw(){
	switch(kind){
	case EXPLOSION_KIND::NOMAL :
		break;
	}

	return true;
}
