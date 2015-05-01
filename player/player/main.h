#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//グローバル変数

extern bool init_flag;
extern int hsv[4][4];

extern std::string SERVER_IP_ADDRESS;// "157.82.7.4"	//サーバーのIPアドレス
extern std::string RASPI_IP_ADDRESS ;//"pi@rpi04.local"//ラズパイのＩＰアドレス

extern int PLAYER_NM ;	//自分のプレイヤー番号

string encode(COMMAND_NAME, int, int, int);
