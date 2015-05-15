#include "DxLib.h"
#include "server.h"
#include <windows.h>
#include "minimap.h"
#include <fstream>

#define BUFMAX 40
#define BASE_PORT (u_short)20000
#define PORT_NUM 3

#define SERIAL_PORT  "\\\\.\\COM3" //�V���A���|�[�g��  "\\\\.\\COM3"
#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

Data hsv_data;

//minimap�p
Object player[4];
Field field;
int loop_counter=0;

//arduino�p
HANDLE arduino;
bool Ret;
int denkyu_flag =0;//������ 000



//server�p

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
void check_denkyu_valid();
void init();
int kbhit(void);

void serial_init();
void set_denkyu(int denkyu_id,bool flag);


time_t item_start_time[4], item_end_time;
time_t dead_start_time[4], dead_end_time;
time_t denkyu_start_time,denkyu_end_time;

//�L�[�{�[�h�p
char key_buf [ 256 ] ;
char key_prev_buf [ 256 ] ;


// WinMain�֐�
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow )
{

	// �E�C���h�E���[�h�ŋN��
	SetMainWindowText( "server" ) ;
	ChangeWindowMode( TRUE ) ;//false�Ȃ�t���X�N���[��
	SetGraphMode(500,300,32);//��ʃT�C�Y1000�~750�ɐݒ�

	// �c�w���C�u��������������
	if( DxLib_Init() == -1 ) return -1;

	SetDrawScreen( DX_SCREEN_BACK );

	//console���g�p��
	AllocConsole();
	freopen ( "CONOUT$", "w", stdout ); 


	//winsocket������

	WSADATA wsaData;

	WSAStartup(MAKEWORD(2,0), &wsaData);

	//�V���A���ʐM������
	serial_init();

	//�ɂ݂܂���
	/*
	Mat image;

    VideoCapture cap(0); // �f�t�H���g�J�������I�[�v��
    if(!cap.isOpened()){  // �����������ǂ������`�F�b�N
		return -1;
	}
	if(!cap.read(image)) {
		std::cout << "No frame" << std::endl;
		MessageBox(NULL,"�摜�t���[���擾���s(�L�E�ցE`)","error",MB_OK | MB_APPLMODAL);
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
	if(result != 30){ //30�̓��͂ǂꂩ�Ɏ��s�����ꍇ
			//cout << "hsv.csv load failed" <<endl;

		MessageBox(NULL,"hsv.csv�擾���s(�L�E�ցE`)","error",MB_OK | MB_APPLMODAL);
		cv::waitKey();
		return -1;
				
	}

	//field.setCorners(image);//�����Ńt�B�[���h�̃R�[�i�[���o

	//field.init(hsv_data.corner.minH,hsv_data.corner.maxH,hsv_data.corner.minS,hsv_data.corner.maxS,hsv_data.corner.minV,hsv_data.corner.maxV);
	//player[0].init(hsv_data.player[0].minH,hsv_data.player[0].maxH,hsv_data.player[0].minS,hsv_data.player[0].maxS,hsv_data.player[0].minV,hsv_data.player[0].maxV);
	//player[1].init(hsv_data.player[1].minH,hsv_data.player[1].maxH,hsv_data.player[1].minS,hsv_data.player[1].maxS,hsv_data.player[1].minV,hsv_data.player[1].maxV);
	//player[2].init(hsv_data.player[2].minH,hsv_data.player[2].maxH,hsv_data.player[2].minS,hsv_data.player[2].maxS,hsv_data.player[2].minV,hsv_data.player[2].maxV);
	//player[3].init(hsv_data.player[3].minH,hsv_data.player[3].maxH,hsv_data.player[3].minS,hsv_data.player[3].maxS,hsv_data.player[3].minV,hsv_data.player[3].maxV);

	//�ȉ����C���R�[�h

	//�f�[�^�̃��[�h

	int bgm_id=LoadSoundMem("../../player/player/sound/GameBGM.mp3");
	//int result_sound=LoadSoundMem("../../player/player/sound/result.mp3");
	int wait_sound=LoadSoundMem("../../player/player/sound/wait.mp3");

	int bgm_timer = 0;


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

		if(bgm_timer == 0){
			//wait_bgm 
		    // ���[�v�ʒu���Z�b�g����
			SetLoopPosSoundMem( 0, wait_sound ) ;
			PlaySoundMem( wait_sound , DX_PLAYTYPE_LOOP );
		}

		//�~�j�}�b�v����
		/*
		cap >> image; // �J��������V�����t���[�����擾
        image = imread("image2.png"); 
		if(!cap.read(image)) {
			std::cout << "No frame" << std::endl;
			MessageBox(NULL,"�摜�t���[���擾���s(�L�E�ցE`)","error",MB_OK | MB_APPLMODAL);
			cv::waitKey();
			return -1;
		}
		
		//�����Ńt�B�[���h�̃R�[�i�[�Č��o
		if( key_buf[ KEY_INPUT_F] == 1 && key_prev_buf[ KEY_INPUT_F ] == 0 ){
			field.setCorners(image);
		}

		//���K�����ꂽplayer�̈ʒu�擾
		for(int i = 0;i<4;i++){
			player[i].detect(image,&field);
		}

		//minimap�𑗐M
		if(game_status == GAME_STATUS::GAME_PLAY){
			loop_counter++;
			if(loop_counter >100000)loop_counter=0;

			//�X�V���x�𐧌�
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

		// Enter���͏���
		if( key_buf[ KEY_INPUT_RETURN] == 1 && key_prev_buf[ KEY_INPUT_RETURN ] == 0 ){
			if(game_status == GAME_STATUS::GAME_WAIT) {
				//�Q�[���J�n����
				send_message(encode(COMMAND_NAME::CHANGE_STATUS,GAME_STATUS::GAME_PLAY,0,0),4);
				game_status = GAME_STATUS::GAME_PLAY;
				std::cout << "�Q�[���X�^�[�g" << std::endl;
				for (int i = 0;i < PORT_NUM;i++){
					player_param[i].init();
				}
				//BGM�Đ�����
				StopSoundMem( wait_sound );
				bgm_timer = 0;

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


		//check_item_valid();
		check_dead_valid();
		check_denkyu_valid();

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
			//game_bgm�Đ�
			if(bgm_timer == 500) PlaySoundMem( bgm_id , DX_PLAYTYPE_BACK );

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
				//result_bgm
				/*
			    // ���[�v�ʒu���Z�b�g����
				SetLoopPosSoundMem( 0, result_sound ) ;
				PlaySoundMem( result_sound , DX_PLAYTYPE_LOOP );
				init();
				*/
			}

		}

		// ����ʂ̓��e��\��ʂɔ��f������
		ScreenFlip() ;


		// �҂��Ȃ��Ə�������������̂ł����łQ�O�~���b�҂�
		WaitTimer( 20 ) ;

		// Windows �V�X�e�����炭�������������
		if( ProcessMessage() == -1 ) break ;

		// �d�r�b�L�[�������ꂽ�烋�[�v���甲����
		if( key_buf[ KEY_INPUT_ESCAPE] == 1 && key_prev_buf[ KEY_INPUT_ESCAPE ] == 0 ){
			break;
		}
		//if( CheckHitKey( KEY_INPUT_ESCAPE ) == 1 ) break ;

		bgm_timer++;
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
	set_denkyu(0,true);
	set_denkyu(1,false);
	set_denkyu(2,false);
	denkyu_timer_flag=0;
	denkyu_flag =0;//������ 000
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
	//1.�|�[�g���I�[�v��
	arduino = CreateFile(SERIAL_PORT,GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

	if(arduino == INVALID_HANDLE_VALUE){
		printf("PORT COULD NOT OPEN\n");
	}
	//2.����M�o�b�t�@������
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
	//3.��{�ʐM�����̐ݒ�
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
		denkyu_flag |= tmp_flag; //����r�b�g���I���ɂ���
	}else denkyu_flag &= ~tmp_flag;  //����r�b�g���I�t�ɂ���


	BYTE data = denkyu_flag;
	
	//4.���M
	DWORD dwSendSize;
	DWORD dwErrorMask;

	Ret = WriteFile(arduino,&data,sizeof(data),&dwSendSize,NULL);
	if(!Ret){
		printf("SEND FAILED\n");
		CloseHandle(arduino);

	}
	printf("set_denkyu() FINISH\n");

}