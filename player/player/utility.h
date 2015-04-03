
#pragma once
#include "object.h"
#include "image_processing.h"

//細かいクラスはここにかき集めている

//残り時間描画クラス
class CSystem_timer : public CObject {

public:
	CSystem_timer(int x_,int y_);
	bool draw();
};

//敵の位置情報管理クラス
class CEnemy : public CImage_processer,public CObject {

public:
	int score;
	bool exist;

	CEnemy();
	int get_x(){return x;}
	int get_y(){return y;}
	bool draw();
	void disconnect();
};

//カーソルクラス
class CCursur:public CObject{
public:
	bool draw();
};

