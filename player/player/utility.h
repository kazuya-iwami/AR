
#pragma once
#include "object.h"
#include "image_processing.h"
#include "explosion.h"

//細かいクラスはここにかき集めている

//残り時間描画クラス
class CSystem_timer : public CObject {

	int system_timer;
	bool finish_flag;

public:
	int handle;
	CSystem_timer(int x_,int y_,int game_time);
	bool draw();
	bool get_finish_flag(){return finish_flag;}
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
	int num_bullet;//残弾数
	const int max_bullet_num;//最大弾数

	CBullet_image(int x_,int y_,int max_bullet_num_);
	bool draw();
	void update_num_bullet(int num_bullet_);
};


//画面効果クラス
class CEffect:public CObject{
public:
	int shaketiemr;
	bool draw();
};
