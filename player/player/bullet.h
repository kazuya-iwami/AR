
#pragma once

#include "object.h"

//弾クラス

enum BULLET_KIND{
	BULLET_NOMAL
};

class CBullet : public CObject{
	BULLET_KIND kind;
	

public:
	/*
		コンストラクタ（初期化）
		引数：
			int x_	弾エフェクトの横軸座標
			int y_	弾エフェクトの縦軸座標
			BULLET_KIND kind_	弾エフェクトの種類 
		戻り値：
			なし
	*/
	CBullet(int x_,int y_,BULLET_KIND kind_);

	/*
		弾エフェクトの描画を行う関数
		引数：
			なし
		戻り値：
			true エフェクト描画中
			false エフェクト終了時
	*/
	bool draw();
};