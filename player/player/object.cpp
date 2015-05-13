#include "object.h"

map<string,int> CObject::figure_id;
map<string,int> CObject::sound_id;
std::list<std::shared_ptr<CObject>> CObject::drawlist;
int CObject::shake_x;
int CObject::shake_y;
int title[14];

//爆発エフェクト用配列
int explosion[45];

//弾エフェクト用配列
int bullet[30];
int bullet_start[45];

//数字用配列
int number[10];

//煙用配列
int fog[10];

//文字用
int word[160];
int digit[18];

//光
int flash[16];
int flash2[10];

//up_effect
int up_effect[10];

//紙ふぶき
int kamifubuki[24];


void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_ICON1"] =  LoadGraph("image/icon1.png");
	figure_id["F_ICON2"] =  LoadGraph("image/icon2.png");
	figure_id["F_ICON3"] =  LoadGraph("image/icon3.png");
	figure_id["F_ICON4"] =  LoadGraph("image/icon4.png");
	figure_id["F_X"]=LoadGraph("image/x.png");
	

	figure_id["F_CURSUR_OUT"]=LoadGraph("image/cursur/cursur_outside.png");
	figure_id["F_CURSUR_IN"]=LoadGraph("image/cursur/cursur_inside.png");
	figure_id["F_CURSUR_TRI"]=LoadGraph("image/cursur/cursur_triangle.png");
	figure_id["F_CURSUR_ON_OUT"]=LoadGraph("image/cursur/cursur_on_outside.png");
	figure_id["F_CURSUR_ON_IN"]=LoadGraph("image/cursur/cursur_on_inside.png");
	figure_id["F_CURSUR_ON_TRI"]=LoadGraph("image/cursur/cursur_on_triangle.png");

	figure_id["F_RELOADING"]=LoadGraph("image/reloading.png");
	figure_id["F_RELOADING_GAUGE"]=LoadGraph("image/reloading_gauge.png");
	figure_id["F_RELOADING2"]=LoadGraph("image/reloading2.png");
	figure_id["F_RELOADING_GAUGE2"]=LoadGraph("image/reloading2_gauge.png");

	figure_id["F_BULLETNOKORI"]=LoadGraph("image/bullet_nokori.png");
	figure_id["F_BULLETUSED"]=LoadGraph("image/bullet_used.png");

	figure_id["F_REDBACK"]=LoadGraph("image/redback.png");
	figure_id["F_BLACK"]=LoadGraph("image/back_black.png");

	figure_id["F_FINISH"]=LoadGraph("image/finish.png");

	figure_id["F_HPBAR"]=LoadGraph("image/hp_bar.png");
	figure_id["F_HPFRAME2"]=LoadGraph("image/hp_frame2.png");
	figure_id["F_LIFE"]=LoadGraph("image/life.png");

	figure_id["F_MAPFRAME"] = LoadGraph("image/map_frame.png");

	figure_id["F_TIMER_FRAME"] = LoadGraph("image/timer_frame.png");

	figure_id["F_COUNTBASE"]=LoadGraph("image/countbase.png");
	figure_id["F_COUNTSIDE"]=LoadGraph("image/countside.png");

	figure_id["F_BACKGROUND_WAIT"]=LoadGraph("image/background_wait.png");

	//タイトルのあつまり
	figure_id["F_TITLE"]=LoadGraph("image/title/title.png");
	title[0]=LoadGraph("image/title/titleR.png");
	title[1]=LoadGraph("image/title/titleE.png");
	title[2]=LoadGraph("image/title/titleA.png");
	title[3]=LoadGraph("image/title/titleL.png");
	title[4]=LoadGraph("image/title/titleT.png");
	title[5]=LoadGraph("image/title/titleA_2.png");
	title[6]=LoadGraph("image/title/titleN.png");
	title[7]=LoadGraph("image/title/titleK.png");
	title[8]=LoadGraph("image/title/titleB.png");
	title[9]=LoadGraph("image/title/titleA_3.png");
	title[10]=LoadGraph("image/title/titleT_2.png");
	title[11]=LoadGraph("image/title/titleT_3.png");
	title[12]=LoadGraph("image/title/titleL_2.png");
	title[13]=LoadGraph("image/title/titleE_2.png");

	figure_id["F_WAITBLACK"]=LoadGraph("image/background_wait_black.png");
	figure_id["F_CONNECT"]=LoadGraph("image/connect.png");
	figure_id["F_DOTGRAY"]=LoadGraph("image/dotgray.png");
	figure_id["F_CONNECTED"]=LoadGraph("image/connected.png");
	figure_id["F_CONNECTED_JA"]=LoadGraph("image/connected_ja.png");
	figure_id["F_GRAYBACK"]=LoadGraph("image/grayback.png");
	figure_id["F_WHITEBACK"]=LoadGraph("image/whiteback.png");
	figure_id["F_BACK"]=LoadGraph("image/back.png");

	figure_id["F_SCORE"]=LoadGraph("image/back_card.png");

	//マップで使うアイコン
	figure_id["F_ICONRED"]=LoadGraph("image/iconred.png");
	figure_id["F_ICONBLUE"]=LoadGraph("image/iconblue.png");
	figure_id["F_ICONGREEN"]=LoadGraph("image/icongreen.png");
	figure_id["F_ICONYELLOW"]=LoadGraph("image/iconyellow.png");
	figure_id["F_ICONDEAD"]=LoadGraph("image/icondead.png");

	//上の効果音版
	sound_id["S_BOMB"]=LoadSoundMem("sound/bomb1.mp3");
	sound_id["S_EXPLOSION"]=LoadSoundMem("sound/explosion.wav");
	sound_id["S_KEIKOKU"]=LoadSoundMem("sound/keikoku.mp3");
	sound_id["S_GET"]=LoadSoundMem("sound/get.mp3");
	sound_id["S_LOCK"]=LoadSoundMem("sound/lock.mp3");
	sound_id["S_ATTACKED"]=LoadSoundMem("sound/attacked.mp3");
	//BGM
	sound_id["S_GAME_BGM"]=LoadSoundMem("sound/GameBGM.mp3");

	// explosion
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/explosion_sample.png", 45, 9, 5, 600, 600, explosion);

	// bullet
	// 画像ファイル分割を行うので、別にint型配列を用意して画像をロード
	LoadDivGraph("image/bullet_start.png", 45, 9, 5, 256, 256, bullet_start);

	//number
	//分割して読み込み
	LoadDivGraph("image/number.png",10,5,2,55,100,number);

	//光
	LoadDivGraph("image/flash2.png",10,10,1,240,240,flash2);	
	//LoadDivGraph("image/flash.png",16,2,8,1350,730,flash);
	
	//up_effect
	LoadDivGraph("image/up_effect.png",10,10,1,120,120,up_effect);

	//文字読み込み
	LoadDivGraph("image/font.png",128,16,8,47,54,word);
	LoadDivGraph("image/digit.png",18,6,3,67,88,digit);

	//紙ふぶき
	LoadDivGraph("image/kamifubuki.png",24,2,12,960,480,kamifubuki);



	//EEICの電球
	figure_id["F_LIGHT_TOP"]=LoadGraph("image/top_light.png");
	figure_id["F_LIGHT_E"]=LoadGraph("image/denkyuE.png");
	figure_id["F_LIGHT_I"]=LoadGraph("image/denkyuI.png");
	figure_id["F_LIGHT_C"]=LoadGraph("image/denkyuC.png");
	figure_id["F_LIGHT_0"]=LoadGraph("image/denkyu0.png");	
	figure_id["F_MINILIGHT_TOP"]=LoadGraph("image/minitop_light.png");
	figure_id["F_MINILIGHT_E"]=LoadGraph("image/minidenkyuE.png");
	figure_id["F_MINILIGHT_I"]=LoadGraph("image/minidenkyuI.png");
	figure_id["F_MINILIGHT_C"]=LoadGraph("image/minidenkyuC.png");
	figure_id["F_MINILIGHT_0"]=LoadGraph("image/minidenkyu0.png");

	//3次元用、図と同様に扱えるっぽい?
	figure_id["X_TANK"]=MV1LoadModel("image/tank.pmx");

	//movie
	/* 動画はmp4不可。ogvは必ず再生できる。 */
	figure_id["M_LINKSTART"]=LoadGraph("movie/LinkStart.ogv");
	//figure_id["M_LINKSTART"]=LoadGraph("movie/LinkStart.avi");
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


void CObject::DrawDigitNum(int x, int y, float size, int space, string s){
	for(int i = 0; i < (int)s.length(); i++){
		switch (s[i]){
		case '1':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[0], true);
			break;
		case '2':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[1], true);
			break;
		case '3':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[2], true);
			break;
		case '4':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[3], true);
			break;
		case '5':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[4], true);
			break;
		case '6':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[6], true);
			break;
		case '7':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[7], true);
			break;
		case '8':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[8], true);
			break;
		case '9':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[9], true);
			break;
		case '0':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[10], true);
			break;
		case ':':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[5], true);
			break;
		case '.':
			DrawExtendGraph(x+space*i,y,x+space*i+(int)(66*size),y+(int)(80*size), digit[17], true);
			break;
		default:
			exit(1);
			break;
		}
	}
}
