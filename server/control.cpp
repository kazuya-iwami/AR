//
// Created by Iwami kazuya on 2015/03/22.
//

#include "control.h"

//メッセージを受け取った際に呼ばれる関数
//実装の加減でこの関数のみファイル分けしてます

string decode(char const *str, string *target);
string explode(int n,char const *y,char const *str,string *target=NULL);

string decode(char const *msg, string *target) {
    return explode(1, ",", msg, target);
}

string explode(int n,char const *y,char const *str,string *target){
    bool option;
    if(target==NULL) option=false;
    else option=true;
    n--;
    string r_str[1000];
    int d=0,g=0,t=strlen(y),a=0;
    for(int i=0;i<strlen(str);i++){
        if(y[a]==str[i]){
            a++;
            if(a==t){
                for(int q=d;q<i-t+1;q++) r_str[g]+=str[q];
                d=i+1;
                i++;
                g++;
                a=0;
            }
        }else{
            i-=a;
            a=0;
        }
    }
    for(int i=d;i<strlen(str);i++) r_str[g]+=str[i];
    if(option){
        for(int i=0;i<=g;i++){
            *target=r_str[i];
            target++;
        }
    }
    return r_str[n];
}


void recv_message(string msg, int id) {
    cout << " プレイヤー：" << id << " " << msg << endl;

    /* メッセージが送られてきた際の処理 */
    string str[4];
    decode(msg.c_str(), str);
    /* commandによる処理分岐 */
    // メッセージがカンマ区切りで第四引数までもっていれば、commandとみなす
    if ("" != str[3]) {
        switch (std::stoi(str[0])) {
            case COMMAND_NAME::USE_ITEM:
                std::cout << "[USE_ITEM]:player" << str[1] << " used item" << str[3] << std::endl;
                break;
            case COMMAND_NAME::SHOOT_BULLET:
                std::cout << "[SHOOT_BULLET]:player" << str[2] << " was shooted by player" << str[1] <<
                        std::endl;
                break;
            case COMMAND_NAME::FINISH_ITEM:
                std::cout << "[FINISH_ITEM]:player" << str[1] << " make full use of item" << str[3] <<
                        std::endl;
                break;
            default:
                printf("error");
                break;
        }
    }
    /* commandによる処理分岐ここまで */
    /* メッセージの処理ここまで */

}

void encode_message(){}