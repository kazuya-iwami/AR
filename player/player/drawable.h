
#pragma once

class CDrawable {
	int x,y;
	int draw_timer;//-1‚Åí•`‰æ

public:
	virtual bool draw();//false‚ª•Ô‚Á‚Ä‚«‚½‚ç•`‰æ‘ÎÛ‚©‚çŠO‚·
};