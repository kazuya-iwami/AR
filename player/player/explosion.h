#pragma once

#include "object.h"

enum EXPLOSION_KIND {
	NOMAL
};

class CExplosion : public CObject {
	EXPLOSION_KIND kind;
	CExplosion(int x_,int y_,EXPLOSION_KIND kind_);
public:
	bool draw();
};