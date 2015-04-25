
#pragma once
#include "object.h"
#include "image_processing.h"
#include "explosion.h"

//細かいクラスはここにかき集めている

//残り時間描画クラス
class CSystem_timer : public CObject {
	int countdown_timer;
	int system_timer;
	bool finish_flag;
	bool countdown_finish_flag;
public:
	int handle;
	CSystem_timer(int x_,int y_,int game_time);
	bool draw();
	bool get_finish_flag(){return finish_flag;}
	bool get_countdown_finish_flag(){return countdown_finish_flag;}
};

//敵の位置情報管理クラス
class CEnemy : public CImage_processer,public CObject {

public:
	int enemy_id;
	int score;
	int HP;
	bool exist;
	bool lockon;
	static int just_before_shooted; // 直前にプレイヤーが撃ったenemyのidが入る
	bool countdown_finish_flag;
	int map_x;
	int map_y;

	CEnemy(int enemy_id_);
	int get_x(){return x;}
	int get_y(){return y;}
	void countdown_finish(){countdown_finish_flag = true;}
	
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


//燃やすクラス
class CFire:public CObject{
public:
	CFire();
	bool draw();
};


//up_effect
class CUp_effect:public CObject{
public:
	CUp_effect();
	bool draw();
};

class CFinish:public CObject{
public:
	std::vector<pair<int,int> > result;
	CFinish(int result_[4]);
	bool draw();
};

//thunder
class CThunder:public CObject{
public:
	CThunder();
	bool draw();
};


class CMap:public CObject {
public:
	bool draw();
};


class CWait:public CObject{
public:
	bool draw();
	CWait();
};

//結果発表で紙ふぶき飛ばしたいよね
class CKamifubuki:public CObject{
public:
	 CKamifubuki();
	 bool draw();
};

