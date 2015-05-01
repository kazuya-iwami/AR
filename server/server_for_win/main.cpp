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

GAME_STATUS game_status;//�Q�[���X�e�[�^�X
int left_time;//���̂Ƃ���������u
CPlayer_param player_param[4];

int set_tcp_socket(int portnum, struct hostent *shost);

void recv_message(std::string msg, int n);
void check_item_valid();
void check_dead_valid();
void init();
int kbhit(void);
time_t item_start_time[4], item_end_time;
time_t dead_start_time[4], dead_end_time;


// WinMain�֐�
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine, int nCmdShow )
{

	
	// �E�C���h�E�֘A�̏����͂c�w���C�u�����ł͈�؍s��Ȃ�
	SetNotWinFlag( TRUE );

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ) return -1;

	// �`����ʂ𗠉�ʂɂ���
	SetDrawScreen( DX_SCREEN_BACK ) ;


	//console���g�p��
	AllocConsole();
	freopen ( "CONOUT$", "w", stdout ); 
	

	//winsocket������

	WSADATA wsaData;

	WSAStartup(MAKEWORD(2,0), &wsaData);


	//�ȉ����C���R�[�h

	//�f�[�^�̃��[�h

	int bgm_id=LoadSoundMem("../../player/player/sound/GameBGM.mp3");

	//�����̃R�[�h�𓥏P

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
		std::cout << "�v���C���[:" << i << "���ڑ����܂���" << std::endl;
	}

	std::cout << "�ڑ�����"<<std::endl;

	init();

	//�L�[�{�[�h�p
	char key_buf [ 256 ] ;
	char key_prev_buf [ 256 ] ;

	while( 1 )
	{

		//�L�[��Ԏ擾
		//�������͈ȉ��̒ʂ�
		for(int i=0;i<256;i++){
			key_prev_buf[i] = key_buf[i];
		}

		GetHitKeyStateAll( key_buf ) ;

		// ��ʂɕ`����Ă�����̂����ׂď���
		ClearDrawScreen() ;

		// �㉺���E�̃L�[���͂ɑΉ����� x, y �̍��W�l��ύX����
		if( key_buf[ KEY_INPUT_RETURN] == 1 && key_prev_buf[ KEY_INPUT_RETURN ] == 0 ){
			if(game_status == GAME_STATUS::GAME_WAIT) {
				//�Q�[���J�n����
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY,0,0),4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "�Q�[���X�^�[�g" << std::endl;
				//BGM�Đ�
				PlaySoundMem( bgm_id , DX_PLAYTYPE_BACK );

			}else if(game_status == GAME_STATUS::GAME_PLAY){
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PAUSE,0,0),4);
				game_status = GAME_STATUS::GAME_PAUSE;
				std::cout << "�ꎞ��~" << std::endl;

			}else if(game_status == GAME_STATUS::GAME_PAUSE) {
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY, 0, 0), 4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "�v���[�ĊJ" << std::endl;
			}
		}


		// ����ʂ̓��e��\��ʂɔ��f������
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
					std::cout << "�v���C���[:"<< i <<"���ؒf���܂���" <<std::endl;
					//loop = false;
					player_param[i].exist = false;
					send_message(encode(COMMAND_NAME::DISCONNECT,i,0,0),4);
					break;
				};

				recv_message(buf, i);
				memset(buf, 0, sizeof(buf));
			}
		}

		//finish��Ԃ��m�F
		if(game_status == GAME_STATUS::GAME_PLAY){
			bool all_finish_flag=true;
			bool disconnect_flag=false;
			for (int i = 0;i < PORT_NUM;i++){
				if(!player_param[i].exist)disconnect_flag =true;
				if(!player_param[i].finish_flag)all_finish_flag=false;
			}

			if(all_finish_flag){ //�����Ă���l���݂�finish��ԂȂ�Q�[���I��
//				if(disconnect_flag){ //�Q�[���I�����N�����ؒf���Ă�����T�[�o�[���Ƃ�
//					std::cout << "�ؒf�����v���C���[�����݂��邽�߃Q�[�����I�����܂�" << std::endl;
//					exit(1);
//				}
				StopSoundMem(bgm_id);//BGM��~
				init();
			}

		}



		// �҂��Ȃ��Ə�������������̂ł����łQ�O�~���b�҂�
		WaitTimer( 20 ) ;

		// Windows �V�X�e�����炭�������������
		if( ProcessMessage() == -1 ) break ;

		// �d�r�b�L�[�������ꂽ�烋�[�v���甲����
		if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;
	}

	for (int i = 0; i < PORT_NUM; i++) {
		 closesocket(nsockfd[i]);
	}

	//console����
	FreeConsole();

	//winsocket�g�p�I��
	WSACleanup();

	// �c�w���C�u�����g�p�̏I������
	DxLib_End() ;

	// �\�t�g�̏I��
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

//���b�Z�[�W�𑗂�Ƃ��͂����p����@n=4�őS��
void send_message(std::string msg, int id=4) {
	if (id == 4) {
		for (int i = 0; i < PORT_NUM; i++) {
			if(!player_param[i].exist) {
				std::cout << "�ؒf�����v���C���[�փ��b�Z�[�W�𑗂낤�Ƃ��Ă��܂�" << std::endl;
				continue;
			}
			int n = (int)send(nsockfd[i], msg.c_str(), msg.length(),0);
			if (n < 0) Err("socket disconnected1");
			memset(buf, 0, sizeof(buf));
		}
		return;
	}
	if(!player_param[id].exist) {
		std::cout << "�������v���C���[�փ��b�Z�[�W�𑗂낤�Ƃ��Ă��܂�" << std::endl;
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

