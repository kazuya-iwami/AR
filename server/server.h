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

void send_message(std::string msg, int id);

#endif //_AR_SOCKET_SERVER_H_
