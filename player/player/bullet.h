
#pragma once

#include "object.h"

//弾クラス

enum BULLET_KIND{
	BULLET_NOMAL,
	BULLET_3D
};

class CBullet : public CObject{
	BULLET_KIND kind;
	int z; //bulletのみが持つ奥行きの座標
	

public:
	/*
		コンストラクタ（初期化）
		引数：
			int x_	弾エフェクトのx座標
			int y_	弾エフェクトのy座標
			int z_	弾エフェクトのz座標
			BULLET_KIND kind_	弾エフェクトの種類 
		戻り値：
			なし
	*/
	CBullet(int x_, int y_, int z_, BULLET_KIND kind_);

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