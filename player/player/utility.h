
#pragma once
#include "object.h"
#include "image_processing.h"
#include "explosion.h"

//細かいクラスはここにかき集めている

//残り時間描画クラス
class CSystem_timer : public CObject {

public:
	int handle;
	CSystem_timer(int x_,int y_);
	bool draw();
};

//敵の位置情報管理クラス
class CEnemy : public CImage_processer,public CObject {

public:
	int enemy_id;
	int score;
	bool exist;
	bool lockon;
	int map_x;
	int map_y;

	CEnemy(int enemy_id_);
	int get_x(){return x;}
	int get_y(){return y;}
	bool draw();
	void disconnect();
	void attacked(int score_);
};


class CBullet_image : public CObject {

public:
	CBullet_image(int x_,int y_);
    bool draw();
};


//画面効果クラス
class CEffect:public CObject{
public:
	int shaketiemr;
	bool draw();
};