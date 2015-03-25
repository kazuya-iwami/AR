#pragma once

#include "object.h"

enum EXPLOSION_KIND {
	NOMAL
};

class CExplosion : public CObject {
	EXPLOSION_KIND kind;
public:
	bool draw();
};