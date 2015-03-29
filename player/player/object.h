
#pragma once
#include <map>
#include <list>
#include <string>
#include "DxLib.h"

using namespace std;

// 爆発エフェクト用
extern int explosion[45];

//描画、音声を取りまとめるクラス
class CObject {
protected:
	int x,y; //描画位置
	int draw_timer; //アニメーション用
	static map<string,int> figure_id;//ここにloadしたイメージに対応したidを連想配列として保存   F_から書き始める
	static map<string,int> sound_id;//ここにloadしたイメージに対応したidを連想配列として保存    S_から書き始める

public:
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
};

/* 描画方法

	まずobject.cppで画像、音声load（初期化）

	描画を登録する際は下の２行使う
	auto bullet = make_shared<CBullet>(200,200,BULLET_KIND::BULLET_NOMAL);//スマートポインタ生成　コンストラクタは各ヘッダファイル見て
	drawlist.push_back(bullet);//描画リストに登録

	draw()の中では
	PlaySoundMem( sound_id["S_TEST"] , DX_PLAYTYPE_BACK ) ;音再生　drawtimer==0の時のみ鳴らすものに注意 ２つめの引数はこれ
	DrawGraph(0,0,figure_id["F_TEST"],false);　描画　初めの２引数はx,y

	//DXライブラリについては下見て
	DXライブラリ詳細
	http://homepage2.nifty.com/natupaji/DxLib/dxlecture_main.html
	DXライブラリ　エフェクトについて（pdf）
	http://sherbet.transjiggen.com/ccs/ccs_wiki2/index.php?plugin=attach&pcmd=open&file=DXLib_Effect_h002.pdf&refer=%A5%CF%A5%EB%A5%C8



*/

