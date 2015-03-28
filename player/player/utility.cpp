#include "utility.h"

bool CSystem_timer::draw(){


	return true;//í‚É•`‰æ
}

CSystem_timer::CSystem_timer(int x_,int y_){
	x=x_;
	y=y_;
}

bool CEnemy::draw(){
	x=ip_x;
	y=ip_y;

	if(visible){//‹ŠE‚É“ü‚Á‚Ä‚¢‚é‚È‚ç
		DrawGraph(x*1000/320 - 100,y*750/240 - 130,figure_id["F_DETECT"],true);//‰æ–Êˆø‚«‰„‚Î‚µ‚Ä‚é•ª‚Ì•â³
	}


	return true;
}