
#pragma once
#include <iostream>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "DxLib.h"

using namespace std;

// 爆発エフェクト用
extern int explosion[45];
extern int explosion1[140];
extern int bullet[30];
extern int bullet_start[45];

//数字用

extern int number[10];
extern int fog[10];

//文字用
extern int word[160];

//燃える
extern int fire[5];
//up_effect
extern int up_effect[10];
//rain
//extern int rain[6];

#define LEFT_WINDOW_WIDTH 150

enum DRAW_LAYER{ //描画レイヤー 下に行くほど前面
	EXPLOSION_LAYER,
	BULLET_LAYER,
	ITEM_LAYER,
	ENEMY_LAYER,
	MESSAGE_LAYER,
	MYTANK_LAYER,
	IMFOMATION_LAYER
};

//描画、音声を取りまとめるクラス
class CObject {
protected:
	int x,y; //描画位置
	int draw_timer; //アニメーション用
	
	static map<string,int> figure_id;//ここにloadしたイメージに対応したidを連想配列として保存   F_から書き始める
	static map<string,int> sound_id;//ここにloadしたイメージに対応したidを連想配列として保存	S_から書き始める

	void set_layer_id(DRAW_LAYER layer_id_);
	
public:
	static std::list<std::shared_ptr<CObject>> drawlist;

	static int shake_x;
	static int shake_y;
	int layer_id; //描画する際に重なる順番　０が一番後ろの面に描画される
	/*
		画像の描画を行う関数
		falseが返ってきたら描画対象から外す
		仮想関数なので、子クラス側で各々の実際の処理を書く
	*/
	virtual bool draw();

	/*
		画像の登録を行う関数
		引数、戻り値　特になし
		任意の描画画像はここに登録させる必要がある
	*/
	static void load();//ロード
	static void register_object(std::shared_ptr<CObject> obj,DRAW_LAYER layer_id);//drawlistに登録

	void DrawOriginalString(int x,int y,float size,int space,string str);
};

/* 描画方法

	まずobject.cppで画像、音声load（初期化）

	描画を登録する際は下の２行使う
	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//スマートポインタ生成　コンストラクタは各ヘッダファイル見て
	CObject::register_object(bullet);//描画リストに登録

	draw()の中では
	PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;音再生　drawtimer==0の時のみ鳴らすものに注意 ２つめの引数はこれ
	DrawGraph(0,0,figure_id["F_TEST"],false);　描画　初めの２引数はx,y

	//DXライブラリについては下見て
	DXライブラリ詳細
	http://homepage2.nifty.com/natupaji/DxLib/dxlecture_main.html
	DXライブラリ　エフェクトについて（pdf）
	http://sherbet.transjiggen.com/ccs/ccs_wiki2/index.php?plugin=attach&pcmd=open&file=DXLib_Effect_h002.pdf&refer=%A5%CF%A5%EB%A5%C8



*/

