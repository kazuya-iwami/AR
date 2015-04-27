#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//グローバル変数

extern bool init_flag;

#define SERVER_IP_ADDRESS "157.82.6.203"	//サーバーのIPアドレス
#define RASPI_IP_ADDRESS "pi@rpi04.local"//ラズパイのＩＰアドレス

#define PLAYER_NM 0	//自分のプレイヤー番号

string encode(COMMAND_NAME, int, int, int);
