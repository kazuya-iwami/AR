
#pragma once

class CDrawable {
	int x,y;
	int draw_timer;//-1で常時描画

public:
	virtual bool draw();//falseが返ってきたら描画対象から外す
};