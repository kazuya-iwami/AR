#include "item.h"

CItem::CItem(int x_,int y_,ITEM_KIND kind_){
	x=x_;
	y=y_;
	kind = kind_;
	draw_timer=0;
}

bool CItem::draw(){
	draw_timer++;


	return true;
}