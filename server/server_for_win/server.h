//
// Created by Iwami kazuya on 2015/03/22.
//

#ifndef _AR_SOCKET_SERVER_H_
#define _AR_SOCKET_SERVER_H_
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <WinSock.h>


enum BULLET_KIND{
	BULLET_NOMAL,
	BULLET_3D
};

enum ITEM_KIND{
	ITEM_NONE,
	STAR,
	THUNDER
};

enum VIABILITY_STATUS{//生存状況
	ALIVE,
	DEAD
};

enum GAME_STATUS {
	GAME_UNCONNECTED,
	GAME_WAIT,
	GAME_PLAY,
	GAME_PAUSE,
	GAME_FINISH
};

enum COMMAND_NAME {
	FINISH,
	USE_ITEM,
	INFORM_ITEM,
	SHOOT_BULLET,
	RETURN_BULLET,
	CHANGE_STATUS,
	UPDATE_LOCATIONS,
	DISCONNECT,
	INFORM_DIE,
	INFORM_REVIVE,
	UPDATE_DENKYU,
	RETURN_DENKYU
};

class Hsv {
public:
	int minH, maxH;
    int minS, maxS;
    int minV, maxV;
};

class Data{
public:
	Hsv player[4];
	Hsv corner;
	void open();
	void save();
};

extern Data hsv_data;

class CPlayer_param{
public:
	CPlayer_param();
	void init();

	int score;//スコア
	ITEM_KIND using_item;//使用していなかったら-1
	bool exist;//接続が切れたらfalse
	bool finish_flag;
	VIABILITY_STATUS viability;//生存状況を保持しておく
};

void send_message(std::string msg, int id);

std::string encode(COMMAND_NAME command_name, int player_from, int player_to, int kind);
void set_denkyu(int denkyu_id,bool flag);

#endif //_AR_SOCKET_SERVER_H_
