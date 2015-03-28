
#pragma once
#include "object.h"

//アイテムクラス
enum ITEM_KIND{
	STER,
	THUNDER
};

class CItem : public CObject{
	ITEM_KIND kind;
	
public:
	CItem(int x_,int y_,ITEM_KIND kind_);
	bool draw();
};