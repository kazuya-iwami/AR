
#include "bullet.h"

CBullet::CBullet(int x_,int y_,BULLET_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
}

bool CBullet::draw(){
	switch(kind){
	case BULLET_KIND::NOMAL :
		break;
	}

	return true;
}
