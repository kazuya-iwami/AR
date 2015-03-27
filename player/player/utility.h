
#pragma once
#include "object.h"
#include "image_processing.h"

class CSystem_timer : public CObject {

public:
	CSystem_timer(int x_,int y_);
	bool draw();
};

class CEnemy : public CImage_processer,public CObject {

public:
	bool draw();
};