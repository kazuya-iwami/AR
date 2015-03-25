
#pragma once

#include "object.h"

enum BULLET_KIND{
	NOMAL
};

class CBullet : public CObject{
	BULLET_KIND kind;
	

public:
	CBullet(int x_,int y_,BULLET_KIND kind_);
	bool draw();
};