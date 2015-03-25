
#pragma once
#include <map>
#include <list>
#include "DxLib.h"
#include <string>

using namespace std;

class CObject {
protected:
	int x,y;
	int draw_timer;//-1で常時描画
	static map<string,int> figure_id;//ここにloadしたイメージに対応したidを連想配列として保存   F_から書き始める
	static map<string,int> sound_id;//ここにloadしたイメージに対応したidを連想配列として保存    S_から書き始める

public:
	virtual bool draw();//falseが返ってきたら描画対象から外す
	static void load();
};

/* 描画方法

	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//スマートポインタ生成
	drawlist.push_back(bullet);//描画リストに登録

*/

