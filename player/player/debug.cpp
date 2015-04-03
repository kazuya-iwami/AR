#include "debug.h"

bool CPopup::debug_flag;

bool CPopup::draw(){
	if(debug_flag == false)return false;//debugƒtƒ‰ƒO‚ª—§‚Á‚Ä‚¢‚È‚¢Žž‚Í•`‰æ‚µ‚È‚¢

	draw_timer++;

	if(draw_timer != 100){
		DrawFormatString(x, y, GetColor(255,255,255),str.c_str());
		return true;
	}
	return false;
}
CPopup::CPopup(int x_,int y_,string str_){
	
	x = x_;
	y = y_;
	draw_timer = 0;
	str = str_;
}