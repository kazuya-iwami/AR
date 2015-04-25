//
// Created by Iwami kazuya on 2015/03/22.
//

#ifndef _AR_SOCKET_MAIN_H_
#define _AR_SOCKET_MAIN_H_

#include <iostream>
#include <time.h>
#include "server.h"

extern GAME_STATUS game_status;//ゲームステータス
extern int left_time;//今のところ実装放置
extern CPlayer_param player_param[4];
extern clock_t item_start_time[4], item_end_time;
extern clock_t dead_start_time[4], dead_end_time;

using namespace std;


void recv_message(string msg, int id);

#endif //_AR_SOCKET_MAIN_H_
