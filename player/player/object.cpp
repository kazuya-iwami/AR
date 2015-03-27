#include "object.h"

 map<string,int> CObject::figure_id;
 map<string,int> CObject::sound_id;

void CObject::load(){
	figure_id["F_TEST"] =  LoadGraph("image/test.jpg");
	sound_id["S_TEST"] = LoadSoundMem("sound/test.mp3");
}

bool CObject::draw(){

	//‰¼‘zŠÖ”‚È‚Ì‚Å’†g‚È‚µ
	return true;

}
