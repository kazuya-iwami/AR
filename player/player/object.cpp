#include "object.h"

 map<string,int> CObject::figure_id;
 map<string,int> CObject::sound_id;

void CObject::load(){
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	figure_id["F_DETECT"] =  LoadGraph("image/detect.png");

	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
}

bool CObject::draw(){

	//仮想関数なので中身なし
	return true;

}
