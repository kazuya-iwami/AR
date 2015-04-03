#pragma once

#include "object.h"

class CPopup : public CObject{
	string str;
public:
	static bool debug_flag;
	CPopup(int x_,int y_,string str_);
	bool draw();

};