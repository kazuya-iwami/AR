
#pragma once
#include <map>
#include <list>
#include "DxLib.h"

using namespace std;

class CDrawable {
protected:
	int x,y;
	int draw_timer;//-1で常時描画
	static map<string,int> graph;//ここにloadしたイメージに対応したidを連想配列として保存

public:
	virtual bool draw();//falseが返ってきたら描画対象から外す
	static void load();
};

list<CDrawable*> drawlist;