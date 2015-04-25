#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//グローバル変数

extern bool init_flag;

#define SERVER_IP_ADDRESS "192.168.0.7"	//サーバーのIPアドレス
#define RASPI_IP_ADDRESS "pi@rpi04.local"

string encode(COMMAND_NAME, int, int, int);
