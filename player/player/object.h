
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

	まずobject.cppで画像、音声load（初期化）

	描画を登録する際は下の２行
	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//スマートポインタ生成　コンストラクタは各ヘッダファイル見て
	drawlist.push_back(bullet);//描画リストに登録

	draw()の中では
	PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;音再生　drawtimer==0の時のみ鳴らすものに注意 ２つめの引数はこれ
	DrawGraph(0,0,figure_id["F_TEST"],false);　描画　初めの２引数はx,y


*/

