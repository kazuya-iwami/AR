#include "object.h"

map<string,int> CObject::figure_id;
map<string,int> CObject::sound_id;
std::list<std::shared_ptr<CObject>> CObject::drawlist;

//爆発エフェクト用配列
int explosion[45];
int explosion1[140];

//弾エフェクト用配列
int bullet[30];

//数字用配列
int number[10];

void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");
	figure_id["F_FRAME"]=LoadGraph("image/frame.png");
	figure_id["F_CURSUR"]=LoadGraph("image/cursur.png");


	//上の効果音版
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");


	// explosion
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/explosion_sample.png", 45, 9, 5, 600, 600, explosion);
	LoadDivGraph("image/explosion1.png", 140, 14, 10, 600, 600, explosion1);

	// bullet
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/bullet.png", 30, 10, 3, 256, 256, bullet);

	//number
	//爆発と同じく分割して読み込み。
	LoadDivGraph("image/number.png",10,5,2,55,100,number);
}

bool CObject::draw(){

	//仮想関数なので中身なし
	return true;

}

void CObject::register_object(std::shared_ptr<CObject> obj){

	drawlist.push_back(obj);

}
