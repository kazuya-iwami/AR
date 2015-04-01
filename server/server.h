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
    STER,
    THUNDER
};

enum GAME_STATUS{
    GAME_PLAY,
    GAME_PAUSE
};

enum COMMAND_NAME{
    USE_ITEM,
    SHOOT_BULLET,
    FINISH_ITEM,
};

class CPlayer_param{
public:
    int score;//スコア
    int using_item;//使用していなかったら-1
    bool exist;//接続が切れたらfalse
};

void send_message(std::string msg, int id);

#endif //_AR_SOCKET_SERVER_H_
