//
// Created by Iwami kazuya on 2015/03/21.
//

#pragma once


#include <stdio.h>
#include <WinSock.h>
#include <iostream>


using namespace std;

#define BUFMAX 40
#define BASE_PORT (u_short)20000

#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

class CNetwork {
	int id;//ユーザーのID：０〜３でそれぞれユーザーごとに異なる
	
	//TCP接続用
	int sofd, retval;
	struct hostent *shost;
	struct sockaddr_in svaddr;
	char buf[BUFMAX];
	fd_set mask;
	struct timeval tm;

public:
	int init(int id_, string hostname);//初期化兼接続開始　成功したら1返す
	string check_msg();//ループの度にこれを呼ぶ　エラーの場合は"EMPTY"返す
	void send_msg(string msg); //メッセージをサーバーへ送信する
	void release();//アプリ終了時に呼ぶ
};


