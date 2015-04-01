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
#include "server.h"
//control.hをincludeしない！

#define BUFMAX 40
#define BASE_PORT (u_short)20000
#define PORT_NUM 1
#define Err(x) {fprintf(stderr,"-"); perror(x); exit(0);}

static int n, retval, nsockfd[PORT_NUM], maxfd;
static struct hostent *shost;
static struct sockaddr_in svaddr;
static char buf[BUFMAX], shostn[BUFMAX];
static fd_set mask;
static struct timeval tm;

GAME_STATUS game_status;//ゲームステータス
int left_time;//今のところ実装放置
CPlayer_param player_param[4];

int set_tcp_socket(int portnum, struct hostent *shost);

void recv_message(std::string msg, int n);
void init();


int main() {

    bool loop = true;

    tm.tv_sec = 0;
    tm.tv_usec = 1;

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

        FD_ZERO(&mask);
        for (int i = 0; i < PORT_NUM; i++) {
            FD_SET(nsockfd[i], &mask);
        }
        retval = select(maxfd, &mask, NULL, NULL, &tm);
        if (retval < 0) Err("select");
        for (int i = 0; i < PORT_NUM; i++) {
            if (FD_ISSET(nsockfd[i], &mask)) {

                n = read(nsockfd[i], buf, BUFMAX);
                if (n <= 0) {
                    printf("プレイヤー:%dが切断しました", i);
                    loop = false;
                    break;
                };

                recv_message(buf, i);
                bzero(buf, BUFMAX);
            }
        }

        sleep(1);
    }
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
    if (n == 4) {
        for (int i = 0; i < PORT_NUM; i++) {
            n = read(0, buf, BUFMAX);
            write(nsockfd[i], buf, n);
            if (n < 0) Err("socket disconnected");
            bzero(buf, BUFMAX);
        }
        return;
    }
    n = read(0, buf, BUFMAX);
    write(nsockfd[id], buf, n);
    if (n < 0) Err("socket disconnected");
    bzero(buf, BUFMAX);
}

void init(){
    game_status=GAME_STATUS::GAME_PLAY;
    left_time=300;
    for(int i=0;i<4;i++){
        player_param[i].exist=true;
        player_param[i].score=0;
        player_param[i].using_item=-1;
    }
}

