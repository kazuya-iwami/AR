#include "drawable.h"

 map<string,int> CDrawable::figure_id;

void CDrawable::load(){
	figure_id["TEST"] =  LoadGraph("image/test.jpg");

}

bool CDrawable::draw(){

	//���z�֐��Ȃ̂Œ��g�Ȃ�
	return true;

}
