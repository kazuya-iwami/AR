
#pragma once
#include "object.h"
#include "image_processing.h"
#include "explosion.h"
#include <string>
#include "network.h"
#include"main.h"

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


enum VIABILITY_STATUS { //生存状態
	ALIVE,
	DEAD
};

//敵の位置情報管理クラス
class CEnemy : public CImage_processer,public CObject {

public:
	int enemy_id;
	int score;
	int HP;
	VIABILITY_STATUS viability_status;//生存状態
	bool exist;
	bool lockon;
	static int just_before_shooted; // 直前にプレイヤーが撃ったenemyのidが入る
	bool countdown_finish_flag; // 試合開始5秒間のカウントダウンが終了したか否か
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
	int* endless_timer;

public:
	int num_bullet;//残弾数
	const int max_bullet_num;//最大弾数
	CBullet_image(int x_,int y_,int max_bullet_num_,int *endless_timer_);
	bool draw();
	void update_num_bullet(int num_bullet_);
};


//画面効果クラス
class CEffect:public CObject{
public:
	int shaketiemr;
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
	std::vector<pair<int,int> > result_score;
	CFinish(std::vector<pair<int,int> > resul_score_);
	bool draw();
};

class CResult: public CObject{
public:
	int result_movie_handle;
	std::vector<pair<int,int> > result_score;
	CResult(vector<pair<int,int> > result_score_);
	bool draw();
};


class CMap:public CObject {
	int player_x[4];
	int player_y[4];
public:
	void update_pos(int p0_x,int p0_y,int p1_x, int p1_y,int p2_x,int p2_y,int p3_x,int p3_y);
	bool draw();
	CMap();
};


class CWait:public CObject{
public:
	bool draw();
	CWait();
	int mode; //ゲームスタートまでの映像再生状態　1:待機　0:動画再生からのGAME_PLAYへ
	int flag;//draw_timer モドキ
	int spin;
	int movieflag;
	int gameflag; // 1:動画再生の終了案内
	int waitflag;//待機画面中のモード管理
	int beforeflag;//前の終了時のflag
};


class CMovie:public CObject{
	std::string name;
public:
	CMovie(std::string name_);
	bool draw();
	void init();
};

class CScore_info_enemy {
public:
	CScore_info_enemy();
	int score;
	int info_y;
	int rank;

};

class CScore_Info:public CObject {
private:
	
	vector<pair<int, int> > rank_info;
	int mytank_id;

public:
	CScore_Info(int mytank_id);
	CScore_info_enemy score_info_enemy[4];
	void update_score(int score0,int score1, int score2,int score3);
	vector<pair<int, int> > get_rank_info(){ return rank_info; };
	bool draw();// 描画とともに動かす
};


//電球一つ一つのクラス　攻撃判定用
class CDenkyu :public CImage_processer,public CNetwork,public CObject{
public:
	bool hit;//既に攻撃されていたらtrue
	int denkyu_id;
	bool lockon;

	void attaacked();//攻撃が当たった場合の処理
	bool draw();
	int get_x(){return x;};
	int get_y(){return y;};

};
//EEICクラス
class CEeic:public CObject{
public:
	CDenkyu denkyu[3];
	CEeic();
	bool draw();
	void detect(Mat image);

};

class CBack:public CObject{
public:
	CBack();
	bool draw();
};


class CPlus:public CObject{
public:
	CPlus(int* score_y_);
	int* score_y;
	bool draw();
};

class CMarker : public CObject,public MarkerDetector,public CNetwork{
public:
	bool denkyu_hit;//既に攻撃されていたらtrue
	bool lockon;

	bool draw();
	int get_x(){return x;};
	int get_y(){return y;};

};


class CSumi :public CObject{
	double scale,rota;
	bool chain_flag;
public:
	CSumi(int x_,int y_, double scale_,double rota_,bool chain);
	bool draw();

};