#include "object.h"

map<string,int> CObject::figure_id;
map<string,int> CObject::sound_id;
std::list<std::shared_ptr<CObject>> CObject::drawlist;
int CObject::shake_x;
int CObject::shake_y;

//爆発エフェクト用配列
int explosion[45];
int explosion1[140];

//弾エフェクト用配列
int bullet[30];
int bullet_start[45];

//数字用配列
int number[10];

//煙用配列
int fog[10];

//燃える
int fire[5];
//up_effect
int up_effect[10];
//rain
//int rain[6];

void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");
	figure_id["F_FRAME"]=LoadGraph("image/frame.png");
	figure_id["F_CURSUR"]=LoadGraph("image/cursur.png");
	figure_id["F_BULLETNOKORI"]=LoadGraph("image/bullet_nokori.png");
	figure_id["F_BULLETUSED"]=LoadGraph("image/bullet_used.png");
	figure_id["F_REDBACK"]=LoadGraph("image/redback.png");
	figure_id["F_FINISH"]=LoadGraph("image/finish.png");
	figure_id["F_COUNTBASE"]=LoadGraph("image/countbase.png");
	figure_id["F_COUNTSIDE"]=LoadGraph("image/countside.png");

	//上の効果音版
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
	sound_id["S_BOMB"]=LoadSoundMem("sound/bomb1.mp3");
	sound_id["S_KEIKOKU"]=LoadSoundMem("sound/keikoku.mp3");
	sound_id["S_GET"]=LoadSoundMem("sound/get.mp3");
	//BGM
	sound_id["S_GAME_BGM"]=LoadSoundMem("sound/GameBGM.mp3");

	// explosion
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/explosion_sample.png", 45, 9, 5, 600, 600, explosion);
	LoadDivGraph("image/explosion1.png", 140, 14, 10, 600, 600, explosion1);

	// bullet
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/bullet.png", 30, 10, 3, 256, 256, bullet);
	LoadDivGraph("image/bullet_start.png", 45, 9, 5, 256, 256, bullet_start);

	//number
	//爆発と同じく分割して読み込み。
	LoadDivGraph("image/number.png",10,5,2,55,100,number);

	//fire
	LoadDivGraph("image/gamenfire.png",5,1,5,640,480,fire);
	//up_effect
	LoadDivGraph("image/up_effect.png",10,10,1,120,120,up_effect);
	//rain
	//LoadDivGraph("image/rain.png",6,1,6,640,480,rain);
}

bool CObject::draw(){

	//仮想関数なので中身なし
	return true;

}

void CObject::register_object(std::shared_ptr<CObject> obj,DRAW_LAYER layer_id_){

	obj->set_layer_id(layer_id_);
	drawlist.push_back(obj);

}

void CObject::set_layer_id(DRAW_LAYER layer_id_){

	layer_id = layer_id_;
}
