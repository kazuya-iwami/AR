#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include "server.h"
#include "server.h"
//#include "control.h"
#include <termios.h>
#include <fcntl.h>

//control.hをincludeしない！

#define BUFMAX 40
#define BASE_PORT (u_short)20000
#define PORT_NUM 1
#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

static int retval, nsockfd[PORT_NUM], maxfd;
static struct hostent *shost;
static char buf[BUFMAX], shostn[BUFMAX];
static fd_set mask;
static struct timeval tm;

GAME_STATUS game_status;//ゲームステータス
int left_time;//今のところ実装放置
CPlayer_param player_param[4];

int set_tcp_socket(int portnum, struct hostent *shost);

void recv_message(std::string msg, int n);
void check_item_valid();
void check_dead_valid();
void init();
int kbhit(void);
clock_t item_start_time[4], item_end_time;
clock_t dead_start_time[4], dead_end_time;

int main() {

	bool loop = true;

	tm.tv_sec = 0;
	tm.tv_usec = 1;

	game_status = GAME_STATUS::GAME_UNCONNECTED;

	if (gethostname(shostn, sizeof(shostn)) < 0) Err("gethostname");

	printf("hostname is %s", shostn);
	printf("\n");


	shost = gethostbyname(shostn);
	if (shost == NULL) Err("gethostbyname");

	maxfd = 0;
	for (int i = 0; i < PORT_NUM; i++) {
		nsockfd[i] = set_tcp_socket(BASE_PORT + i, shost);
		if (maxfd < nsockfd[i] + 1)maxfd = nsockfd[i] + 1;
		std::cout << "プレイヤー:" << i << "が接続しました" << std::endl;
	}

	std::cout << "接続完了"<<std::endl;

	init();

	while (loop) {

		check_item_valid();
		check_dead_valid();

		FD_ZERO(&mask);
		for (int i = 0; i < PORT_NUM; i++) {
			FD_SET(nsockfd[i], &mask);
		}
		retval = select(maxfd, &mask, NULL, NULL, &tm);
		if (retval < 0) Err("select");
		for (int i = 0; i < PORT_NUM; i++) {
			if(!player_param[i].exist)continue;
			if (FD_ISSET(nsockfd[i], &mask)) {

				int n = (int)read(nsockfd[i], buf, BUFMAX);
				if (n <= 0) {
					std::cout << "プレイヤー:"<< i <<"が切断しました" <<std::endl;
					//loop = false;
					player_param[i].exist = false;
					send_message(encode(COMMAND_NAME::DISCONNECT,i,0,0),4);
					break;
				};

				recv_message(buf, i);
				bzero(buf, BUFMAX);
			}
		}

		//キー入力受け付け
		if(kbhit()) {/*キーが押されていたら、変数cにキー取得*/
			char c = getchar();

			if(game_status == GAME_STATUS::GAME_WAIT) {
				//ゲーム開始命令
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY,0,0),4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "ゲームスタート" << std::endl;
			}else if(game_status == GAME_STATUS::GAME_PLAY){
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PAUSE,0,0),4);
				game_status = GAME_STATUS::GAME_PAUSE;
				std::cout << "一時停止" << std::endl;
			}else if(game_status == GAME_STATUS::GAME_PAUSE) {
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY, 0, 0), 4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "プレー再開" << std::endl;
			}

		}

		//finish状態か確認
		if(game_status == GAME_STATUS::GAME_PLAY){
			bool all_finish_flag=true;
			bool disconnect_flag=false;
			for (int i = 0;i < PORT_NUM;i++){
				if(!player_param[i].exist)disconnect_flag =true;
				if(!player_param[i].finish_flag)all_finish_flag=false;
			}

			if(all_finish_flag){ //生きている人がみなfinish状態ならゲーム終了
//				if(disconnect_flag){ //ゲーム終了時誰かが切断していたらサーバー落とす
//					std::cout << "切断したプレイヤーが存在するためゲームを終了します" << std::endl;
//					exit(1);
//				}
				init();
			}

		}

	}
	//現在無限ループだね

	for (int i = 0; i < PORT_NUM; i++) {
		close(nsockfd[i]);
	}
}


int set_tcp_socket(int portnum, struct hostent *shost) {
	int sockfd, nsockfd;
	struct sockaddr_in server;
	int yes = 1;

	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket error");
		return -1;
	}

	/* set socket address information */
	memset(&server, 0, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	bcopy((char *) shost->h_addr, (char *) &server.sin_addr, shost->h_length);
	server.sin_port = htons(portnum);


	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	/* bind socket */
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("bind error");
		close(sockfd);
		return -1;
	}

	listen(sockfd, 1);


	nsockfd = accept(sockfd, NULL, NULL);
	if (nsockfd < 0) Err("accept");
	close(sockfd);
	return nsockfd;
}

//メッセージを送るときはこれを用いる　n=4で全体
void send_message(std::string msg, int id=4) {
	if (id == 4) {
		for (int i = 0; i < PORT_NUM; i++) {
			if(!player_param[i].exist) {
				std::cout << "切断したプレイヤーへメッセージを送ろうとしています" << std::endl;
				continue;
			}
			int n = (int)write(nsockfd[i], msg.c_str(), msg.length());
			if (n < 0) Err("socket disconnected1");
			bzero(buf, BUFMAX);
		}
		return;
	}
	if(!player_param[id].exist) {
		std::cout << "抜けたプレイヤーへメッセージを送ろうとしています" << std::endl;
	}
	int n = (int)write(nsockfd[id], msg.c_str(), msg.length());
	if (n < 0) Err("socket disconnected2");
	bzero(buf, BUFMAX);
}

void init(){
	game_status=GAME_STATUS::GAME_WAIT;
	item_end_time = 0;
	for(int i=0;i<4;i++){
		item_start_time[i] = 0;
		player_param[i].init();
	}
}

std::string encode(COMMAND_NAME command_name, int player_from, int player_to, int kind){

	std::ostringstream stream;
	stream << (int)command_name << ","  << player_from << "," << player_to << "," << kind;
	return stream.str();
}

CPlayer_param::CPlayer_param() {
	exist = true;
	score = 0;
	using_item = ITEM_KIND::ITEM_NONE ;
	finish_flag = false;
	viability = VIABILITY_STATUS::ALIVE;
}
void CPlayer_param::init() {
	score = 0;
	using_item = ITEM_KIND::ITEM_NONE ;
	finish_flag = false;
	viability = VIABILITY_STATUS::ALIVE;
}

//キー操作用
//http://tricky-code.net/mine/c/mc06linuxkbhit.php
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF) {
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
}