//
// Created by Iwami kazuya on 2015/03/21.
//

#include "network.h"

int CNetwork::sofd;
int CNetwork::retval;
struct hostent *CNetwork::shost;
struct sockaddr_in CNetwork::svaddr;
char CNetwork::buf[BUFMAX];
fd_set CNetwork::mask;
struct timeval CNetwork::tm;
int CNetwork::id;
tstring CNetwork::ip_address;

int CNetwork::network_init(int id_, string hostname) {

	ip_address = _T(hostname);

	/* Windows 独自の設定 */
	WSADATA data;
	WSAStartup(MAKEWORD(2,0), &data); 

	if(0 > id_ || id_ >3)Err("idは0~3で設定して下さい。");

	id = id_;

	tm.tv_sec = 0;
	tm.tv_usec = 1;

	char host[256];
	sprintf_s(host, "%s", hostname.c_str());

	//shost = gethostbyname(host);
	//if (shost == NULL) Err("gethostbyname");

	memset((char *) &svaddr,0, sizeof(svaddr));
	svaddr.sin_family = AF_INET;
	svaddr.sin_port = htons(BASE_PORT + id);
	svaddr.sin_addr.s_addr = inet_addr(host);

	sofd = socket(AF_INET, SOCK_STREAM, 0);
	if (sofd < 0) Err("socket");


	connect(sofd, (struct sockaddr *) &svaddr, sizeof(svaddr));

	return 1;
}

string CNetwork::check_msg() {

	FD_ZERO(&mask);
	FD_SET(sofd, &mask);

	retval = select(sofd + 1, &mask, NULL, NULL, &tm);
	if (retval < 0) Err("select");
	if (FD_ISSET(sofd, &mask)) {
		memset(buf,0, BUFMAX);
		int n = recv(sofd, buf, BUFMAX,0);
		if(n<0)Err("socket read");
		return buf;
	}
	return "EMPTY";

}

void CNetwork::send_msg(string msg) {
	if(msg.length() > BUFMAX){
		Err("send_msg　msgがBUFMAXを超えています");
	}
	char str[BUFMAX];
	sprintf_s(str, "%s", msg.c_str());

	send(sofd, str, sizeof(str),0);
}

void CNetwork::release(){
	/* Windows 独自の設定 */
	closesocket(sofd);
	WSACleanup();
}

string encode(COMMAND_NAME command_name, int player_from, int player_to, int kind){
	std::ostringstream stream;
	stream << (int)command_name << ","  << player_from << "," << player_to << "," << kind;
	return stream.str();
}

vector<string> decode(string msg) {
	return split(msg, ",");
}

vector<string> split(string s, string delim) {
	vector<string> v;
	for (;;) {
	size_t found = s.find(delim);
	if (found == string::npos) {
		v.push_back(s);
		break;
	} else {
		v.push_back(s.substr(0, found));
		s = s.substr(found + 1);
	}
}

	return v;
}

