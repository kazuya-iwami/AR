#pragma once

#include "object.h"

//爆発クラス

enum EXPLOSION_KIND {
	EXPLOSION_NOMAL
};

class CExplosion : public CObject {
	EXPLOSION_KIND kind;
	
public:
	/*
		コンストラクタ（初期化）
		引数：
			int x_	爆発エフェクトの横軸座標
			int y_	爆発エフェクトの縦軸座標
			EXPLOSION_KIND kind_	爆発の種類 
		戻り値：
			なし
	*/
	CExplosion(int x_,int y_,EXPLOSION_KIND kind_);

	/*
		爆発エフェクトの描画を行う関数
		引数：
			なし
		戻り値：
			true エフェクト描画中
			false エフェクト終了時
	*/
	bool draw();
};
