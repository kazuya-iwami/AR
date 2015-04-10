
#pragma once
#include "object.h"

//アイテムクラス
enum ITEM_KIND{
	ITEM_NONE,
	STAR,
	THUNDER
};

class CItem : public CObject{
	ITEM_KIND kind;
	
public:
	/*
		コンストラクタ（初期化）
		引数：
			int x_	アイテム表示画面の横軸座標
			int y_	アイテム表示画面の縦軸座標
			ITEM_KIND kind_	アイテムの種類 
		戻り値：
			なし
	*/
	CItem(int x_,int y_,ITEM_KIND kind_);

	/*
		アイテム表示画面の描画を行う関数
		引数：
			なし
		戻り値：
			true エフェクト描画中
			false エフェクト終了時
	*/
	bool draw();
};
