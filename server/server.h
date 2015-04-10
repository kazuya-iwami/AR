//
// Created by Iwami kazuya on 2015/03/22.
//

#ifndef _AR_SOCKET_SERVER_H_
#define _AR_SOCKET_SERVER_H_
#include <iostream>


enum BULLET_KIND{
	BULLET_NOMAL,
	BULLET_3D
};

enum ITEM_KIND{
	ITEM_NONE,
	STAR,
	THUNDER
};

enum GAME_STATUS {
	GAME_UNCONNECTED,
	GAME_WAIT,
	GAME_PLAY,
	GAME_PAUSE,
	GAME_FINISH
};

enum COMMAND_NAME{
	FINISH,
	USE_ITEM,
	INFORM_ITEM,
	SHOOT_BULLET,
	RETURN_BULLET,
	CHANGE_STATUS,
	UPDATE_LOCATIONS,
	DISCONNECT
};

class CPlayer_param{
public:
	CPlayer_param();
	void init();

	int score;//スコア
	ITEM_KIND using_item;//使用していなかったら-1
	bool exist;//接続が切れたらfalse
	bool finish_flag;
};

void send_message(std::string msg, int id);

std::string encode(COMMAND_NAME command_name, int player_from, int player_to, int kind);

#endif //_AR_SOCKET_SERVER_H_
