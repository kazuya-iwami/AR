/**
 * HTTPConnection.cpp : HTTP�ʐM�̃T���v��������
 * ���g�͂����ƌ��؂��Ă܂���...(2015/3/31����)
 * �g�p��
 *		tstring ipAddress = _T("192.168.10.125");
 *		tstring strUrl = _T("http://") + ipAddress+_T("/move/");
 *		bool isMethodGet = true;
 * 	tstring parameter = _T("forward);
 * 	tstring strResult;
 * 	if( HttpRequest(strUrl, isMethodGet, parameter, rstrResult) ){
 * 		//success
 * 	}else {
 *			//fail
 * 	}
 */
#include <windows.h>
#include <Wininet.h>

#include <assert.h>
#include <string>
#include <sstream>
#include <locale.h>
#include <conio.h>
#include <tchar.h>
#include "NhConversion.h"
#include "HTTPConnection.h"

//�Ȃ񂩂��ꂪ�����ƋN�����Ȃ�
#pragma comment( lib, "Wininet.lib" )

#define URLBUFFER_SIZE		(4096)
#define	READBUFFER_SIZE		(4096)

//�G���[���̏I������
void closeInternet(HINTERNET hInternetOpen,
							 HINTERNET hInternetConnect,
							 HINTERNET hInternetRequest,
							 char* pszOptional){
	if (pszOptional){
		free(pszOptional);
	}
	InternetCloseHandle(hInternetRequest);
	InternetCloseHandle(hInternetConnect);
	InternetCloseHandle(hInternetOpen);
}

//�����p�ɃJ�X�^�}�C�Y
bool HttpRequest(tstring strUrl, bool isMethodGet, tstring speed, tstring& rstrResult){
	tstring strUserAgent =  _T("MyApp");

	// �A�E�g�v�b�g�̏�����
	rstrResult = tstring();

	// �C���v�b�g�̃`�F�b�N
	if (strUrl.length() == 0){
		assert(!"URL���s��");
		return false;
	}

	// �ϐ�
	HINTERNET			hInternetOpen = NULL;
	HINTERNET			hInternetConnect = NULL;
	HINTERNET			hInternetRequest = NULL;
	char*				pszOptional = NULL;
	URL_COMPONENTS		urlcomponents;
	tstring				strServer;
	tstring				strObject;
	INTERNET_PORT			nPort;
	tstring				strVerb;
	tstring				strHeaders;
	tstringstream			ssRead;

	// URL���
	ZeroMemory(&urlcomponents, sizeof(URL_COMPONENTS));
	urlcomponents.dwStructSize = sizeof(URL_COMPONENTS);
	TCHAR szHostName[URLBUFFER_SIZE];
	TCHAR szUrlPath[URLBUFFER_SIZE];
	urlcomponents.lpszHostName = szHostName;
	urlcomponents.lpszUrlPath = szUrlPath;
	urlcomponents.dwHostNameLength = URLBUFFER_SIZE;
	urlcomponents.dwUrlPathLength = URLBUFFER_SIZE;
	if (!InternetCrackUrl(strUrl.c_str(),
		(DWORD)strUrl.length(),
		0,
		&urlcomponents)){	// URL�̉�͂Ɏ��s
		assert(!"URL��͂Ɏ��s");
		return false;
	}
	strServer = urlcomponents.lpszHostName;
	strObject = urlcomponents.lpszUrlPath;
	nPort = urlcomponents.nPort;

	// HTTP��HTTPS������ȊO��
	DWORD dwFlags = 0;
	if (INTERNET_SCHEME_HTTP == urlcomponents.nScheme){
		// HTTP
		dwFlags = INTERNET_FLAG_RELOAD				// �v�����ꂽ�t�@�C���A�I�u�W�F�N�g�A�܂��̓t�H���_�ꗗ���A�L���b�V������ł͂Ȃ��A���̃T�[�o�[���狭���I�Ƀ_�E�����[�h���܂��B
			| INTERNET_FLAG_DONT_CACHE			// �Ԃ��ꂽ�G���e�B�e�B���L���V���֒ǉ����܂���B
			| INTERNET_FLAG_NO_AUTO_REDIRECT;	// HTTP �����Ŏg�p����A���_�C���N�g�� HttpSendRequest �ŏ�������Ȃ����Ƃ��w�肵�܂��B
	}else if (INTERNET_SCHEME_HTTPS == urlcomponents.nScheme){	
		// HTTPS
		dwFlags = INTERNET_FLAG_RELOAD				// �v�����ꂽ�t�@�C���A�I�u�W�F�N�g�A�܂��̓t�H���_�ꗗ���A�L���b�V������ł͂Ȃ��A���̃T�[�o�[���狭���I�Ƀ_�E�����[�h���܂��B
			| INTERNET_FLAG_DONT_CACHE			// �Ԃ��ꂽ�G���e�B�e�B���L���V���֒ǉ����܂���B
			| INTERNET_FLAG_NO_AUTO_REDIRECT	// HTTP �����Ŏg�p����A���_�C���N�g�� HttpSendRequest �ŏ�������Ȃ����Ƃ��w�肵�܂��B
			| INTERNET_FLAG_SECURE						// ���S�ȃg�����U�N�V�������g�p���܂��B����ɂ��ASSL/PCT ���g���悤�ɕϊ�����AHTTP �v�������ŗL���ł��B 
			| INTERNET_FLAG_IGNORE_CERT_DATE_INVALID	// INTERNET_FLAG_IGNORE_CERT_DATE_INVALID�AINTERNET_FLAG_IGNORE_CERT_CN_INVALID
			| INTERNET_FLAG_IGNORE_CERT_CN_INVALID;		// �́A�ؖ����Ɋւ���x���𖳎�����t���O
	}else{
		assert(!"HTTP�ł�HTTPS�ł��Ȃ�");
		return false;
	}

	// GET��POST����URL�̌������@���قȂ�
	if (isMethodGet){
		// GET
		strVerb = _T("GET");
		strHeaders = _T("");
		if (0 != speed.length()){
			strObject += speed;
		}
	}else{
		// POST
		strVerb = _T("POST");
		strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
		if (0 != speed.length()){
			// �p�����[�^���A���M����I�v�V�����f�[�^�ɕϊ�����
			pszOptional = NhT2M(speed.c_str());	// char������ɕϊ�
		}
	}

	// WinInet�̏�����
	hInternetOpen = InternetOpen(strUserAgent.c_str(),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (NULL == hInternetOpen){
		assert(!"WinInet�̏������Ɏ��s");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	//�^�C���A�E�g�̐ݒ�
	DWORD ms = 5000;
	InternetSetOption(hInternetOpen, INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hInternetOpen, INTERNET_OPTION_RECEIVE_TIMEOUT, &ms, sizeof(ms));

	// HTTP�ڑ�
	hInternetConnect = InternetConnect(hInternetOpen,
		strServer.c_str(),
		nPort,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0);
	if (NULL == hInternetConnect){
		assert(!"HTTP�ڑ��Ɏ��s");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP�ڑ����J��
	hInternetRequest = HttpOpenRequest(hInternetConnect,
		strVerb.c_str(),
		strObject.c_str(),
		NULL,
		NULL,
		NULL,
		dwFlags,
		NULL);
	if (NULL == hInternetRequest){
		assert(!"HTTP�ڑ����J���Ɏ��s");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP�v�����M
	if (!HttpSendRequest(hInternetRequest,
		strHeaders.c_str(),
		(DWORD)strHeaders.length(),
		(LPVOID)((char*)pszOptional),
		pszOptional ? (DWORD)(strlen(pszOptional) * sizeof(char)) : 0)){
		assert(!"HTTP�v�����M�Ɏ��s");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP�v���ɑΉ�����X�e�[�^�X�R�[�h�̎擾
	DWORD dwStatusCode;
	DWORD dwLength = sizeof(DWORD);
	if (!HttpQueryInfo(hInternetRequest,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&dwStatusCode,
		&dwLength,
		0)){
		assert(!"HTTP�v���ɑΉ�����X�e�[�^�X�R�[�h�̎擾�Ɏ��s");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}
	if (HTTP_STATUS_OK != dwStatusCode){
		assert(!"�X�e�[�^�X�R�[�h��OK�łȂ�");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP�t�@�C���ǂݍ���
	char szReadBuffer[READBUFFER_SIZE + 1];
	while (1){
		DWORD dwRead = 0;
		if (!InternetReadFile(hInternetRequest, szReadBuffer, READBUFFER_SIZE, &dwRead)){
			assert(!"HTTP�t�@�C���ǂݍ��݂Ɏ��s");
			closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
			return false;
		}
		if (0 == dwRead){
			break;
		}
		szReadBuffer[dwRead] = '\0';	// �I�[�����u\0�v�̕t��
		size_t length = dwRead + 1;
		LPWSTR	pszWideChar = (LPWSTR)malloc(length * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8,	// CODE PAGE: UTF-8
			0,
			szReadBuffer,
			-1,
			pszWideChar,
			(int)length);	// UTF-8�������ANSI������ɕϊ�
		TCHAR* pszTchar = NhW2T(pszWideChar);	// WideChar�������TCHAR������ɕϊ�
		ssRead << pszTchar;	// �X�g���[��������ɗ�������
		free(pszTchar);
		free(pszWideChar);
	}

	// �X�g���[����������A�o�͕�����ɕϊ�
	rstrResult = ssRead.str().c_str();

	closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
	return true;
}