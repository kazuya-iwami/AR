
#pragma once

#include "object.h"

//’eƒNƒ‰ƒX

enum BULLET_KIND{
	BULLET_NOMAL
};

class CBullet : public CObject{
	BULLET_KIND kind;
	

public:
	CBullet(int x_,int y_,BULLET_KIND kind_);
	bool draw();
};