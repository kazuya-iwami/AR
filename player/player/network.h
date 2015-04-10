//
// Created by Iwami kazuya on 2015/03/21.
//

#pragma once


#include <stdio.h>
#include <WinSock.h>
#include <iostream>
#include "HTTPConnection.h"
#include <sstream>

enum COMMAND_NAME {
	USE_ITEM,
	INFORM_ITEM,
	SHOOT_BULLET,
	RETURN_BULLET,
	CHANGE_STATUS,
	UPDATE_LOCATIONS,
	DISCONNECT
};

using namespace std;

#define BUFMAX 40
#define BASE_PORT (u_short)20000

#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

vector<string> decode(string msg);
vector<string> split(string s, string delim);
string encode(COMMAND_NAME command_name, int player_from, int player_to, int kind);

//ネットワーク通信用クラス
// check_msg()とsend_msg()をつかえれば中身見なくてもよい

class CNetwork {

	//TCP接続用
	static int sofd, retval;
	static struct hostent *shost;
	static struct sockaddr_in svaddr;
	static char buf[BUFMAX];
	static fd_set mask;
	static struct timeval tm;

protected:
	static int id;//ユーザーのID：０〜３でそれぞれユーザーごとに異なる
	static tstring ip_address;

public:

	static int network_init(int id_, string hostname);//初期化兼接続開始　成功したら1返す
	string check_msg();//ループの度にこれを呼ぶ　エラーの場合は"EMPTY"返す
	void send_msg(string msg); //メッセージをサーバーへ送信する
	void release();//アプリ終了時に呼ぶ
};



