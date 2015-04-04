/**
 * HTTPConnection.h
 * 使用例
 *		tstring ipAddress = _T("192.168.10.125");
 *		tstring strUrl = _T("http://") + ipAddress+_T("/move/");
 *		bool isMethodGet = true;  //falseの時はPOSTメソッドと解釈
 * 	tstring parameter = _T("forward);
 * 	tstring strResult;
 * 	if( HttpRequest(strUrl, isMethodGet, parameter, strResult) ){
 * 		//success
 * 	}else {
 *			//fail
 * 	}
 */
#include "tstring.h"
//HTTP通信用関数
bool HttpRequest(tstring strUrl, bool isMethodGet, tstring speed, tstring& rstrResult);