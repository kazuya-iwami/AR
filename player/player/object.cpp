#include "object.h"
#define explosion_frame 44

 map<string,int> CObject::figure_id;
 map<string,int> CObject::sound_id;

void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");

	// explosion
	/*for(int i = 0; i < explosion_frame; i++){
		figure_id["F_EXPLOSION" + std::to_string(i)] = LoadGraph(_T("image/explosion_sample/explosion"+ std::to_string(i) +".jpg"));
	}*/
	// TCHAR型の文字連結でこけたのでとりあえず愚直に書きます とりあえず複数ファイルで
	figure_id["F_EXPLOSION0"] = LoadGraph("image/explosion_sample/explosion0000.png");
	figure_id["F_EXPLOSION1"] = LoadGraph("image/explosion_sample/explosion0001.png");
	figure_id["F_EXPLOSION2"] = LoadGraph("image/explosion_sample/explosion0002.png");
	figure_id["F_EXPLOSION3"] = LoadGraph("image/explosion_sample/explosion0003.png");
	figure_id["F_EXPLOSION4"] = LoadGraph("image/explosion_sample/explosion0004.png");
	figure_id["F_EXPLOSION5"] = LoadGraph("image/explosion_sample/explosion0005.png");
	figure_id["F_EXPLOSION6"] = LoadGraph("image/explosion_sample/explosion0006.png");
	figure_id["F_EXPLOSION7"] = LoadGraph("image/explosion_sample/explosion0007.png");
	figure_id["F_EXPLOSION8"] = LoadGraph("image/explosion_sample/explosion0008.png");
	figure_id["F_EXPLOSION9"] = LoadGraph("image/explosion_sample/explosion0009.png");
	figure_id["F_EXPLOSION10"] = LoadGraph("image/explosion_sample/explosion0010.png");
	figure_id["F_EXPLOSION11"] = LoadGraph("image/explosion_sample/explosion0011.png");
	figure_id["F_EXPLOSION12"] = LoadGraph("image/explosion_sample/explosion0012.png");
	figure_id["F_EXPLOSION13"] = LoadGraph("image/explosion_sample/explosion0013.png");
	figure_id["F_EXPLOSION14"] = LoadGraph("image/explosion_sample/explosion0014.png");
	figure_id["F_EXPLOSION15"] = LoadGraph("image/explosion_sample/explosion0015.png");
	figure_id["F_EXPLOSION16"] = LoadGraph("image/explosion_sample/explosion0016.png");
	figure_id["F_EXPLOSION17"] = LoadGraph("image/explosion_sample/explosion0017.png");
	figure_id["F_EXPLOSION18"] = LoadGraph("image/explosion_sample/explosion0018.png");
	figure_id["F_EXPLOSION19"] = LoadGraph("image/explosion_sample/explosion0019.png");
	figure_id["F_EXPLOSION20"] = LoadGraph("image/explosion_sample/explosion0020.png");
	figure_id["F_EXPLOSION21"] = LoadGraph("image/explosion_sample/explosion0021.png");
	figure_id["F_EXPLOSION22"] = LoadGraph("image/explosion_sample/explosion0022.png");
	figure_id["F_EXPLOSION23"] = LoadGraph("image/explosion_sample/explosion0023.png");
	figure_id["F_EXPLOSION24"] = LoadGraph("image/explosion_sample/explosion0024.png");
	figure_id["F_EXPLOSION25"] = LoadGraph("image/explosion_sample/explosion0025.png");
	figure_id["F_EXPLOSION26"] = LoadGraph("image/explosion_sample/explosion0026.png");
	figure_id["F_EXPLOSION27"] = LoadGraph("image/explosion_sample/explosion0027.png");
	figure_id["F_EXPLOSION28"] = LoadGraph("image/explosion_sample/explosion0028.png");
	figure_id["F_EXPLOSION29"] = LoadGraph("image/explosion_sample/explosion0029.png");
	figure_id["F_EXPLOSION30"] = LoadGraph("image/explosion_sample/explosion0030.png");
	figure_id["F_EXPLOSION31"] = LoadGraph("image/explosion_sample/explosion0031.png");
	figure_id["F_EXPLOSION32"] = LoadGraph("image/explosion_sample/explosion0032.png");
	figure_id["F_EXPLOSION33"] = LoadGraph("image/explosion_sample/explosion0033.png");
	figure_id["F_EXPLOSION34"] = LoadGraph("image/explosion_sample/explosion0034.png");
	figure_id["F_EXPLOSION35"] = LoadGraph("image/explosion_sample/explosion0035.png");
	figure_id["F_EXPLOSION36"] = LoadGraph("image/explosion_sample/explosion0036.png");
	figure_id["F_EXPLOSION37"] = LoadGraph("image/explosion_sample/explosion0037.png");
	figure_id["F_EXPLOSION38"] = LoadGraph("image/explosion_sample/explosion0038.png");
	figure_id["F_EXPLOSION39"] = LoadGraph("image/explosion_sample/explosion0039.png");
	figure_id["F_EXPLOSION40"] = LoadGraph("image/explosion_sample/explosion0040.png");
	figure_id["F_EXPLOSION41"] = LoadGraph("image/explosion_sample/explosion0041.png");
	figure_id["F_EXPLOSION42"] = LoadGraph("image/explosion_sample/explosion0042.png");
	figure_id["F_EXPLOSION43"] = LoadGraph("image/explosion_sample/explosion0043.png");

	//上の効果音版
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
}

bool CObject::draw(){

	//仮想関数なので中身なし
	return true;

}
