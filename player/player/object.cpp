#include "object.h"

 map<string,int> CObject::figure_id;

void CObject::load(){
	figure_id["TEST"] =  LoadGraph("image/test.jpg");

}

bool CObject::draw(){

	//���z�֐��Ȃ̂Œ��g�Ȃ�
	return true;

}
