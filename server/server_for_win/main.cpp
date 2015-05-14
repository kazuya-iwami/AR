#include "DxLib.h"
#include "server.h"
#include <windows.h>
#include "minimap.h"
#include <fstream>

#define BUFMAX 40
#define BASE_PORT (u_short)20000
#define PORT_NUM 1
#define SERIAL_PORT  "\\\\.\\COM4" //シリアルポート名  "\\\\.\\COM3"
#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

Data hsv_data;

//minimap用
Object player[4];
Field field;
int loop_counter=0;

//arduino用
HANDLE arduino;
bool Ret;
int denkyu_flag =0;//初期化 000

//server用

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

void serial_init();
void set_denkyu(int denkyu_id,bool flag);


time_t item_start_time[4], item_end_time;
time_t dead_start_time[4], dead_end_time;

//キーボード用
char key_buf [ 256 ] ;
char key_prev_buf [ 256 ] ;


// WinMain関数
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow )
{

	// ウインドウモードで起動
	SetMainWindowText( "server" ) ;
	ChangeWindowMode( TRUE ) ;//falseならフルスクリーン
	SetGraphMode(500,300,32);//画面サイズ1000×750に設定

	// ＤＸライブラリ初期化処理
	if( DxLib_Init() == -1 ) return -1;

	SetDrawScreen( DX_SCREEN_BACK );

	//consoleを使用可に
	AllocConsole();
	freopen ( "CONOUT$", "w", stdout ); 


	//winsocket初期化

	WSADATA wsaData;

	WSAStartup(MAKEWORD(2,0), &wsaData);

	//シリアル通信初期化
	serial_init();

	//にみまっぷ
	/*
	Mat image;

    VideoCapture cap(0); // デフォルトカメラをオープン
    if(!cap.isOpened()){  // 成功したかどうかをチェック
		return -1;
	}
	if(!cap.read(image)) {
		std::cout << "No frame" << std::endl;
		MessageBox(NULL,"画像フレーム取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
		cv::waitKey();
		return -1;
	}
	*/

	std::ifstream ifs( "../../player/player/data/hsv.csv" );
	std::string str;

	ifs >> str;
	int result = sscanf_s(str.c_str(),"1p:%d,%d,%d,%d,%d,%d,2p:%d,%d,%d,%d,%d,%d,3p:%d,%d,%d,%d,%d,%d,4p:%d,%d,%d,%d,%d,%d,corner:%d,%d,%d,%d,%d,%d",
		&(hsv_data.player[0].minH),&(hsv_data.player[0].maxH),&(hsv_data.player[0].minS),&(hsv_data.player[0].maxS),&(hsv_data.player[0].minV),&(hsv_data.player[0].maxV),
		&(hsv_data.player[1].minH),&(hsv_data.player[1].maxH),&(hsv_data.player[1].minS),&(hsv_data.player[1].maxS),&(hsv_data.player[1].minV),&(hsv_data.player[1].maxV),
		&(hsv_data.player[2].minH),&(hsv_data.player[2].maxH),&(hsv_data.player[2].minS),&(hsv_data.player[2].maxS),&(hsv_data.player[2].minV),&(hsv_data.player[2].maxV),
		&(hsv_data.player[3].minH),&(hsv_data.player[3].maxH),&(hsv_data.player[3].minS),&(hsv_data.player[3].maxS),&(hsv_data.player[3].minV),&(hsv_data.player[3].maxV),
		&(hsv_data.corner.minH),&(hsv_data.corner.maxH),&(hsv_data.corner.minS),&(hsv_data.corner.maxS),&(hsv_data.corner.minV),&(hsv_data.corner.maxV));
	if(result != 30){ //30個の入力どれかに失敗した場合
			//cout << "hsv.csv load failed" <<endl;

		MessageBox(NULL,"hsv.csv取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
		cv::waitKey();
		return -1;
				
	}

	//field.setCorners(image);//ここでフィールドのコーナー検出

	//field.init(hsv_data.corner.minH,hsv_data.corner.maxH,hsv_data.corner.minS,hsv_data.corner.maxS,hsv_data.corner.minV,hsv_data.corner.maxV);
	//player[0].init(hsv_data.player[0].minH,hsv_data.player[0].maxH,hsv_data.player[0].minS,hsv_data.player[0].maxS,hsv_data.player[0].minV,hsv_data.player[0].maxV);
	//player[1].init(hsv_data.player[1].minH,hsv_data.player[1].maxH,hsv_data.player[1].minS,hsv_data.player[1].maxS,hsv_data.player[1].minV,hsv_data.player[1].maxV);
	//player[2].init(hsv_data.player[2].minH,hsv_data.player[2].maxH,hsv_data.player[2].minS,hsv_data.player[2].maxS,hsv_data.player[2].minV,hsv_data.player[2].maxV);
	//player[3].init(hsv_data.player[3].minH,hsv_data.player[3].maxH,hsv_data.player[3].minS,hsv_data.player[3].maxS,hsv_data.player[3].minV,hsv_data.player[3].maxV);

	//以下メインコード

	//データのロード

	int bgm_id=LoadSoundMem("../../player/player/sound/GameBGM.mp3");

	//既存のコードを踏襲

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

		//ミニマップ処理
		/*
		cap >> image; // カメラから新しいフレームを取得
        image = imread("image2.png"); 
		if(!cap.read(image)) {
			std::cout << "No frame" << std::endl;
			MessageBox(NULL,"画像フレーム取得失敗(´・ω・`)","error",MB_OK | MB_APPLMODAL);
			cv::waitKey();
			return -1;
		}
		
		//ここでフィールドのコーナー再検出
		if( key_buf[ KEY_INPUT_F] == 1 && key_prev_buf[ KEY_INPUT_F ] == 0 ){
			field.setCorners(image);
		}

		//正規化されたplayerの位置取得
		for(int i = 0;i<4;i++){
			player[i].detect(image,&field);
		}

		//minimapを送信
		if(game_status == GAME_STATUS::GAME_PLAY){
			loop_counter++;
			if(loop_counter >100000)loop_counter=0;

			//更新速度を制限
			if(loop_counter%30 == 1){
				std::ostringstream stream;
				stream << (int)COMMAND_NAME::UPDATE_LOCATIONS << ","  << player[0].normalized_x << "," <<  player[0].normalized_y << "," 
					<< player[1].normalized_x << "," <<  player[1].normalized_y << "," 
					<< player[2].normalized_x << "," <<  player[2].normalized_y << ","
					<< player[3].normalized_x << "," <<  player[3].normalized_y;
				send_message(stream.str(),4);
			}
		}
		*/

		// Enter入力処理
		if( key_buf[ KEY_INPUT_RETURN] == 1 && key_prev_buf[ KEY_INPUT_RETURN ] == 0 ){
			if(game_status == GAME_STATUS::GAME_WAIT) {
				//ゲーム開始命令
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY,0,0),4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "ゲームスタート" << std::endl;
				for (int i = 0;i < PORT_NUM;i++){
					player_param[i].init();
				}
				//BGM再生
				PlaySoundMem( bgm_id , DX_PLAYTYPE_BACK );

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


		//check_item_valid();
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
				StopSoundMem(bgm_id);//BGM停止
				init();
			}

		}

		// 裏画面の内容を表画面に反映させる
		ScreenFlip() ;


		// 待たないと処理が早すぎるのでここで２０ミリ秒待つ
		WaitTimer( 20 ) ;

		// Windows システムからくる情報を処理する
		if( ProcessMessage() == -1 ) break ;

		// ＥＳＣキーが押されたらループから抜ける
		if( key_buf[ KEY_INPUT_ESCAPE] == 1 && key_prev_buf[ KEY_INPUT_ESCAPE ] == 0 ){
			break;
		}
		//if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;
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
	set_denkyu(0,false);
	set_denkyu(1,false);
	set_denkyu(2,false);
	game_status=GAME_STATUS::GAME_WAIT;
	item_end_time = 0;
	for(int i=0;i<4;i++){
		//item_start_time[i] = 0;
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
	//using_item = ITEM_KIND::ITEM_NONE ;
	finish_flag = false;
	viability = VIABILITY_STATUS::ALIVE;
}
void CPlayer_param::init() {
	score = 0;
	//using_item = ITEM_KIND::ITEM_NONE ;
	finish_flag = false;
	viability = VIABILITY_STATUS::ALIVE;
}

void serial_init(){
	//1.ポートをオープン
	arduino = CreateFile(SERIAL_PORT,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(arduino == INVALID_HANDLE_VALUE){
		printf("PORT COULD NOT OPEN\n");
	}
	//2.送受信バッファ初期化
	Ret = SetupComm(arduino,1024,1024);
	if(!Ret){
		printf("SET UP FAILED\n");
		CloseHandle(arduino);

	}
	Ret = PurgeComm(arduino,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	if(!Ret){
		printf("CLEAR FAILED\n");
		CloseHandle(arduino);
	}
	//3.基本通信条件の設定
	DCB dcb;
	GetCommState(arduino,&dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 9600;
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity =NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	Ret = SetCommState(arduino,&dcb);
	if(!Ret){
		printf("SetCommState FAILED\n");
		CloseHandle(arduino);
	}
}
void set_denkyu(int denkyu_id,bool flag){

	int tmp_flag = 0x00;
	if(denkyu_id == 1)tmp_flag=0x01; //001
	if(denkyu_id == 2)tmp_flag=0x02; //010
	if(denkyu_id == 3)tmp_flag=0x04; //100

	if(flag){
		denkyu_flag |= tmp_flag; //あるビットをオンにする
	}else denkyu_flag &= ~tmp_flag;  //あるビットをオフにする


	BYTE data = denkyu_flag;
	
	//4.送信
	DWORD dwSendSize;
	DWORD dwErrorMask;

	Ret = WriteFile(arduino,&data,sizeof(data),&dwSendSize,NULL);
	if(!Ret){
		printf("SEND FAILED\n");
		CloseHandle(arduino);

	}
	printf("set_denkyu() FINISH\n");

}