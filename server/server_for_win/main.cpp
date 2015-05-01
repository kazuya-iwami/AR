#include "DxLib.h"
#include "server.h"
#include <windows.h>

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
time_t item_start_time[4], item_end_time;
time_t dead_start_time[4], dead_end_time;


// WinMain関数
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow )
{

	
	// ウインドウ関連の処理はＤＸライブラリでは一切行わない
	SetNotWinFlag( TRUE );

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ) return -1;

	// 描画先画面を裏画面にする
	SetDrawScreen( DX_SCREEN_BACK ) ;


	//consoleを使用可に
	AllocConsole();
	freopen ( "CONOUT$", "w", stdout ); 
	

	//winsocket初期化

	WSADATA wsaData;

	WSAStartup(MAKEWORD(2,0), &wsaData);


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

	//キーボード用
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	while( 1 )
	{

		//キー状態取得
		//書き方は以下の通り
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		// 画面に描かれているものをすべて消す
		ClearDrawScreen() ;

		// 上下左右のキー入力に対応して x, y の座標値を変更する
		if( key_buf[ KEY_INPUT_RETURN] == 1 && key_prev_buf[ KEY_INPUT_RETURN ] == 0 ){
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


		// 裏画面の内容を表画面に反映させる
		ScreenFlip() ;

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

				int n = (int)recv(nsockfd[i], buf, BUFMAX,0);
				if (n <= 0) {
					std::cout << "プレイヤー:"<< i <<"が切断しました" <<std::endl;
					//loop = false;
					player_param[i].exist = false;
					send_message(encode(COMMAND_NAME::DISCONNECT,i,0,0),4);
					break;
				};

				recv_message(buf, i);
				memset(buf, 0, sizeof(buf));
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



		// 待たないと処理が早すぎるのでここで２０ミリ秒待つ
		WaitTimer( 20 ) ;

		// Windows システムからくる情報を処理する
		if( ProcessMessage() == -1 ) break ;

		// ＥＳＣキーが押されたらループから抜ける
		if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;
	}

	for (int i = 0; i < PORT_NUM; i++) {
		 closesocket(nsockfd[i]);
	}

	//console閉じる
	FreeConsole();

	//winsocket使用終了
	WSACleanup();

	// ＤＸライブラリ使用の終了処理
	DxLib_End() ;

	// ソフトの終了
	return 0 ;
}

int set_tcp_socket(int portnum, struct hostent *shost) {
	int sockfd, nsockfd;
	struct sockaddr_in client;
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
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(portnum);


	/*if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}*/

	/* bind socket */
	if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
		perror("bind error");
		closesocket(sockfd);
		return -1;
	}

	listen(sockfd, 1);

	int len = sizeof(client);
	nsockfd = accept(sockfd,(struct sockaddr *)&client, &len);
	if (nsockfd < 0) Err("accept");
	closesocket(sockfd);
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
			int n = (int)send(nsockfd[i], msg.c_str(), msg.length(),0);
			if (n < 0) Err("socket disconnected1");
			memset(buf, 0, sizeof(buf));
		}
		return;
	}
	if(!player_param[id].exist) {
		std::cout << "抜けたプレイヤーへメッセージを送ろうとしています" << std::endl;
	}
	int n = (int)send(nsockfd[id], msg.c_str(), msg.length(),0);
	if (n < 0) Err("socket disconnected2");
	memset(buf, 0, sizeof(buf));
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

