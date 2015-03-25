#pragma once

#include "object.h"

enum EXPLOSION_KIND {
	EXPLOSION_NOMAL
};

class CExplosion : public CObject {
	EXPLOSION_KIND kind;
	
public:
	CExplosion(int x_,int y_,EXPLOSION_KIND kind_);
	bool draw();
};