#include "object.h"

 map<string,int> CObject::figure_id;
 map<string,int> CObject::sound_id;

void CObject::load(){
	//このように書けばtest.jpgをメモリに読み込む。
	//今後はdraw()内でfigure_id["F_TEST"]を用いるとこの画像に対応したidを取得できる。
	//F_TESTの部分は他と被らないように自由に決めてください。ただしF_から初めて。
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");

	//上の効果音版
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
}

bool CObject::draw(){

	//仮想関数なので中身なし
	return true;

}
