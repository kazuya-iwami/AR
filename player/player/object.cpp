﻿#include "object.h"

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

//文字用
int word[160];

//燃える
int fire[5];
//up_effect
int up_effect[10];

//紙ふぶき
int kamifubuki[24];

//thunder
int thunder[10];

void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_ICON1"] =  LoadGraph("image/icon1.png");
	figure_id["F_ICON2"] =  LoadGraph("image/icon2.png");
	figure_id["F_ICON3"] =  LoadGraph("image/icon3.png");
	figure_id["F_ICON4"] =  LoadGraph("image/icon4.png");
	figure_id["F_X"]=LoadGraph("image/x.png");
	
	figure_id["F_CURSUR"]=LoadGraph("image/cursur.png");
	figure_id["F_CURSUR_ON"]=LoadGraph("image/cursur_on.png");
	figure_id["F_BULLETNOKORI"]=LoadGraph("image/bullet_nokori.png");
	figure_id["F_BULLETUSED"]=LoadGraph("image/bullet_used.png");
	figure_id["F_REDBACK"]=LoadGraph("image/redback.png");
	figure_id["F_FINISH"]=LoadGraph("image/finish.png");
	figure_id["F_HP"]=LoadGraph("image/hp_gauge.png");

	figure_id["F_MAPFRAME"] = LoadGraph("image/map_frame.png");

	figure_id["F_TIMER_FRAME"] = LoadGraph("image/timer_frame.png");

	figure_id["F_COUNTBASE"]=LoadGraph("image/countbase.png");
	figure_id["F_COUNTSIDE"]=LoadGraph("image/countside.png");

	figure_id["F_WAIT_SAMPLE"]=LoadGraph("image/waiting_sample_image.png");

	figure_id["F_MAN"]=LoadGraph("image/man.png");


	//上の効果音版
	sound_id["S_BOMB"]=LoadSoundMem("sound/bomb1.mp3");
	sound_id["S_KEIKOKU"]=LoadSoundMem("sound/keikoku.mp3");
	sound_id["S_GET"]=LoadSoundMem("sound/get.mp3");
	sound_id["S_LOCK"]=LoadSoundMem("sound/lock.mp3");
	//BGM
	sound_id["S_GAME_BGM"]=LoadSoundMem("sound/GameBGM.mp3");

	// explosion
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/explosion_sample.png", 45, 9, 5, 600, 600, explosion);
	LoadDivGraph("image/explosion1.png", 140, 14, 10, 600, 600, explosion1);

	// bullet
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/bullet_start.png", 45, 9, 5, 256, 256, bullet_start);

	//number
	//分割して読み込み
	LoadDivGraph("image/number.png",10,5,2,55,100,number);

	//fire
	LoadDivGraph("image/gamenfire.png",5,1,5,640,480,fire);
	//up_effect
	LoadDivGraph("image/up_effect.png",10,10,1,120,120,up_effect);
	//THUNDER
	LoadDivGraph("image/thunder.png",10,2,5,320,240,thunder);

	//文字読み込み
	LoadDivGraph("image/font.png",128,16,8,47,54,word);

	//紙ふぶき
	LoadDivGraph("image/kamifubuki.png",24,2,12,960,480,kamifubuki);

	//EEICの電球
	figure_id["F_LIGHT0"]=LoadGraph("image/light0.png");
	figure_id["F_ELIGHT1"]=LoadGraph("image/Elight1.png");
	figure_id["F_ILIGHT1"]=LoadGraph("image/Ilight1.png");
	figure_id["F_CLIGHT1"]=LoadGraph("image/Clight1.png");

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

//size:文字サイズ0~1のfloat space:文字間の間隔 int ,str:文字列 string
void CObject::DrawOriginalString(int x,int y,float size,int space,string str){
	for(int i = 0; i < (int)str.length(); i++){
		DrawExtendGraph(x+space*i,y,x+space*i+(int)(49*size),y+(int)(51*size),word[(int)str[i]],true);
	}
}
