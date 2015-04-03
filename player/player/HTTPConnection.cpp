/**
 * HTTPConnection.cpp : HTTP通信のサンプルを改良
 * 中身はちゃんと検証してません...(2015/3/31現在)
 * 使用例
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

//なんかこれが無いと起動しない
#pragma comment( lib, "Wininet.lib" )

#define URLBUFFER_SIZE		(4096)
#define	READBUFFER_SIZE		(4096)

//エラー時の終了処理
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

//操作専用にカスタマイズ
bool HttpRequest(tstring strUrl, bool isMethodGet, tstring speed, tstring& rstrResult){
	tstring strUserAgent =  _T("MyApp");

	// アウトプットの初期化
	rstrResult = tstring();

	// インプットのチェック
	if (strUrl.length() == 0){
		assert(!"URLが不正");
		return false;
	}

	// 変数
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

	// URL解析
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
		&urlcomponents)){	// URLの解析に失敗
		assert(!"URL解析に失敗");
		return false;
	}
	strServer = urlcomponents.lpszHostName;
	strObject = urlcomponents.lpszUrlPath;
	nPort = urlcomponents.nPort;

	// HTTPかHTTPSかそれ以外か
	DWORD dwFlags = 0;
	if (INTERNET_SCHEME_HTTP == urlcomponents.nScheme){
		// HTTP
		dwFlags = INTERNET_FLAG_RELOAD				// 要求されたファイル、オブジェクト、またはフォルダ一覧を、キャッシュからではなく、元のサーバーから強制的にダウンロードします。
			| INTERNET_FLAG_DONT_CACHE			// 返されたエンティティをキャシュへ追加しません。
			| INTERNET_FLAG_NO_AUTO_REDIRECT;	// HTTP だけで使用され、リダイレクトが HttpSendRequest で処理されないことを指定します。
	}else if (INTERNET_SCHEME_HTTPS == urlcomponents.nScheme){	
		// HTTPS
		dwFlags = INTERNET_FLAG_RELOAD				// 要求されたファイル、オブジェクト、またはフォルダ一覧を、キャッシュからではなく、元のサーバーから強制的にダウンロードします。
			| INTERNET_FLAG_DONT_CACHE			// 返されたエンティティをキャシュへ追加しません。
			| INTERNET_FLAG_NO_AUTO_REDIRECT	// HTTP だけで使用され、リダイレクトが HttpSendRequest で処理されないことを指定します。
			| INTERNET_FLAG_SECURE						// 安全なトランザクションを使用します。これにより、SSL/PCT を使うように変換され、HTTP 要求だけで有効です。 
			| INTERNET_FLAG_IGNORE_CERT_DATE_INVALID	// INTERNET_FLAG_IGNORE_CERT_DATE_INVALID、INTERNET_FLAG_IGNORE_CERT_CN_INVALID
			| INTERNET_FLAG_IGNORE_CERT_CN_INVALID;		// は、証明書に関する警告を無視するフラグ
	}else{
		assert(!"HTTPでもHTTPSでもない");
		return false;
	}

	// GETかPOSTかでURLの結合方法が異なる
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
			// パラメータを、送信するオプションデータに変換する
			pszOptional = NhT2M(speed.c_str());	// char文字列に変換
		}
	}

	// WinInetの初期化
	hInternetOpen = InternetOpen(strUserAgent.c_str(),
		INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	if (NULL == hInternetOpen){
		assert(!"WinInetの初期化に失敗");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	//タイムアウトの設定
	DWORD ms = 5000;
	InternetSetOption(hInternetOpen, INTERNET_OPTION_CONNECT_TIMEOUT, &ms, sizeof(ms));
	InternetSetOption(hInternetOpen, INTERNET_OPTION_RECEIVE_TIMEOUT, &ms, sizeof(ms));

	// HTTP接続
	hInternetConnect = InternetConnect(hInternetOpen,
		strServer.c_str(),
		nPort,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0);
	if (NULL == hInternetConnect){
		assert(!"HTTP接続に失敗");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP接続を開く
	hInternetRequest = HttpOpenRequest(hInternetConnect,
		strVerb.c_str(),
		strObject.c_str(),
		NULL,
		NULL,
		NULL,
		dwFlags,
		NULL);
	if (NULL == hInternetRequest){
		assert(!"HTTP接続を開くに失敗");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP要求送信
	if (!HttpSendRequest(hInternetRequest,
		strHeaders.c_str(),
		(DWORD)strHeaders.length(),
		(LPVOID)((char*)pszOptional),
		pszOptional ? (DWORD)(strlen(pszOptional) * sizeof(char)) : 0)){
		assert(!"HTTP要求送信に失敗");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTP要求に対応するステータスコードの取得
	DWORD dwStatusCode;
	DWORD dwLength = sizeof(DWORD);
	if (!HttpQueryInfo(hInternetRequest,
		HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
		&dwStatusCode,
		&dwLength,
		0)){
		assert(!"HTTP要求に対応するステータスコードの取得に失敗");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}
	if (HTTP_STATUS_OK != dwStatusCode){
		assert(!"ステータスコードがOKでない");
		closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
		return false;
	}

	// HTTPファイル読み込み
	char szReadBuffer[READBUFFER_SIZE + 1];
	while (1){
		DWORD dwRead = 0;
		if (!InternetReadFile(hInternetRequest, szReadBuffer, READBUFFER_SIZE, &dwRead)){
			assert(!"HTTPファイル読み込みに失敗");
			closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
			return false;
		}
		if (0 == dwRead){
			break;
		}
		szReadBuffer[dwRead] = '\0';	// 終端文字「\0」の付加
		size_t length = dwRead + 1;
		LPWSTR	pszWideChar = (LPWSTR)malloc(length * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8,	// CODE PAGE: UTF-8
			0,
			szReadBuffer,
			-1,
			pszWideChar,
			(int)length);	// UTF-8文字列をANSI文字列に変換
		TCHAR* pszTchar = NhW2T(pszWideChar);	// WideChar文字列をTCHAR文字列に変換
		ssRead << pszTchar;	// ストリーム文字列に流し込む
		free(pszTchar);
		free(pszWideChar);
	}

	// ストリーム文字列を、出力文字列に変換
	rstrResult = ssRead.str().c_str();

	closeInternet(hInternetOpen, hInternetConnect, hInternetRequest, pszOptional);
	return true;
}