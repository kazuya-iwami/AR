
#pragma once
#include "object.h"
#include "image_processing.h"

//細かいクラスはここにかき集めている

class CSystem_timer : public CObject {

public:
	CSystem_timer(int x_,int y_);
	bool draw();
};

class CEnemy : public CImage_processer,public CObject {

public:
	bool draw();
};