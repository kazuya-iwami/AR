//
// Created by Iwami kazuya on 2015/03/22.
//

#include "control.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>

//メッセージを受け取った際に呼ばれる関数
//実装の加減でこの関数のみファイル分けしてます

string decode(char const *str, string *target);
string explode(int n,char const *y,char const *str,string *target=NULL);

string decode(char const *msg, string *target) {
	return explode(1, ",", msg, target);
}

double diff;

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

	/* メッセージが送られてきた際の処理 */
	std::string str[4];
	decode(msg.c_str(), str);
	/* commandによる処理分岐 */
	// メッセージがカンマ区切りで第四引数までもっていれば、commandとみなす
	if ("" != str[3]) {
	int command_name = atoi(str[0].c_str());
	int player_from = atoi(str[1].c_str());
	int player_to = atoi(str[2].c_str());
	int kind = atoi(str[3].c_str());
		switch (command_name) {
			case COMMAND_NAME::USE_ITEM:
				switch (kind) {
					case ITEM_KIND::STAR:
						// スターの処理
						cout << "[USE_ITEM]:player" << player_from << " used STAR" << std::endl;
						player_param[player_from].using_item = ITEM_KIND::STAR;
						item_start_time[player_from] = time(NULL);
						send_message(encode(COMMAND_NAME::INFORM_ITEM,player_from,player_to,kind),4);
						break;
					case ITEM_KIND::THUNDER:
						// サンダーの処理
						cout << "[USE_ITEM]:player" << player_from << " used THUNDER" << std::endl;
						player_param[player_from].using_item = ITEM_KIND::THUNDER;
						item_start_time[player_from] = time(NULL);
						send_message(encode(COMMAND_NAME::INFORM_ITEM,player_from,player_to,kind),4);
						break;
					default:
						std::cout << "ITEM_KIND ERROR" << std::endl;
						break;
				}
				break;
			case COMMAND_NAME::SHOOT_BULLET:
				std::cout << "[SHOOT_BULLET]:player" << player_to << " was shooted by player" << player_from <<
				std::endl;

				if(kind == BULLET_KIND::BULLET_NOMAL
				   && player_param[player_to].using_item != ITEM_KIND::STAR) {
					send_message(encode(COMMAND_NAME::RETURN_BULLET,player_from,player_to,kind),4);
				}

				break;
			case COMMAND_NAME::FINISH:
				player_param[id].finish_flag = true;
				cout << "プレイヤー:" << id <<"からFINISHを受け取りました" << endl;
				break;
			default:
				std::cout << "COMMAND_NAME ERROR" << std::endl;
				break;
		}
	}
	/* commandによる処理分岐ここまで */
	/* メッセージの処理ここまで */
	cout << " recv_message from：" << id <<" msg:"<< " " << msg << endl;
}

void check_item_valid() {
	item_end_time = time(NULL);
	/* 差分を求める */
	for (int i = 0; i < 4; i++) {
		if (ITEM_KIND::ITEM_NONE == player_param[i].using_item) { continue; }
		diff = difftime(item_end_time, item_start_time[i]);
		switch (player_param[i].using_item) {
			case ITEM_KIND::STAR:
				// アイテム使用から8秒以上たっていたら
				if (8 <= diff) {
					std::cout << "player" << i << "のSTARの使用が終了しました。" << std::endl;
					player_param[i].using_item = ITEM_KIND::ITEM_NONE ;
				}
				break;
			case ITEM_KIND::THUNDER:
				// アイテム使用から4秒以上たっていたら
				if (4 <= diff) {
					std::cout << "player" << i << "のTHUNDERの使用が終了しました。" << std::endl;
					player_param[i].using_item = ITEM_KIND::ITEM_NONE ;
				}
				break;
			default:
				std::cout << "ITEM_KIND ERROR" << std::endl;
				break;
		}
	}
}
