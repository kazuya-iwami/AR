
#pragma once
#include "object.h"

//�A�C�e���N���X
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