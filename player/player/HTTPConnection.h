/**
 * HTTPConnection.h
 * �g�p��
 *		tstring ipAddress = _T("192.168.10.125");
 *		tstring strUrl = _T("http://") + ipAddress+_T("/move/");
 *		bool isMethodGet = true;  //false�̎���POST���\�b�h�Ɖ���
 * 	tstring parameter = _T("forward);
 * 	tstring strResult;
 * 	if( HttpRequest(strUrl, isMethodGet, parameter, strResult) ){
 * 		//success
 * 	}else {
 *			//fail
 * 	}
 */
#include "tstring.h"
//HTTP�ʐM�p�֐�
bool HttpRequest(tstring strUrl, bool isMethodGet, tstring parameter, tstring& rstrResult);