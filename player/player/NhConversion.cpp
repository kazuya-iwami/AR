/**
 * NhConversion.cpp
 * HTTP�ʐM�̂��߂̃T���v���R�[�h�ɂ������t�@�C��
 * ���g�͂����ƌ��؂��Ă܂���...(2015/3/31����)
 */
#include <windows.h>	// WideCharToMultiByte, LPSTR��
#include <assert.h>

#include <crtdbg.h>
#ifdef _DEBUG
#define   new                   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define   malloc(s)             _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   calloc(c, s)          _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   realloc(p, s)         _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _recalloc(p, c, s)    _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _expand(p, s)         _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

LPSTR  NhM2M(LPCSTR  pszMultiByte)
{
	assert(pszMultiByte);
	if (NULL == pszMultiByte)
	{
		return _strdup("");
	}
	return _strdup(pszMultiByte);
}

LPSTR  NhW2M(LPCWSTR pszWideChar)
{
	assert(pszWideChar);
	if (NULL == pszWideChar)
	{
		return _strdup("");
	}
	size_t length = wcslen(pszWideChar) + 1;
	LPSTR	pszMultiByte = (LPSTR)malloc(length * sizeof(WCHAR));
	assert(pszMultiByte);
	WideCharToMultiByte(CP_ACP,	// CODE PAGE: ANSI code page
		0,
		pszWideChar,
		-1,
		pszMultiByte,
		(int)length * sizeof(WCHAR),	// �S�p�����́AWCHAR�ł͂P��������CHAR�ł͂Q�����˕��������������WCHAR�̕������ł͕s�\��
		NULL,
		NULL);
	return pszMultiByte;
}

LPWSTR NhM2W(LPCSTR  pszMultiByte)
{
	assert(pszMultiByte);
	if (NULL == pszMultiByte)
	{
		return _wcsdup(L"");
	}
	size_t length = strlen(pszMultiByte) + 1;
	LPWSTR	pszWideChar = (LPWSTR)malloc(length * sizeof(WCHAR));
	assert(pszWideChar);
	MultiByteToWideChar(CP_ACP,	// CODE PAGE: ANSI code page
		0,
		pszMultiByte,
		-1,
		pszWideChar,
		(int)length);	// �S�p�����́ACHAR�ł͂Q��������WCHAR�ł͂P�����˕������������CHAR�̕������ŏ\��
	return pszWideChar;
}


LPWSTR NhW2W(LPCWSTR pszWideChar)
{
	assert(pszWideChar);
	if (NULL == pszWideChar)
	{
		return _wcsdup(L"");
	}
	return _wcsdup(pszWideChar);
}

int NhM2M(LPCSTR  pszMB_src, LPSTR  pszMB_dest, int iCountMB_dest)
{
	return strncpy_s(pszMB_dest, iCountMB_dest, pszMB_src, _TRUNCATE);
}

int NhW2M(LPCWSTR pszWC_src, LPSTR  pszMB_dest, int iCountMB_dest)
{
	return WideCharToMultiByte(CP_ACP,	// CODE PAGE: ANSI code page
		0,
		pszWC_src,
		(int)wcslen(pszWC_src) + 1,
		pszMB_dest,
		iCountMB_dest,
		NULL,
		NULL);
}

int NhM2W(LPCSTR  pszMB_src, LPWSTR pszWC_dest, int iCountWC_dest)
{
	return MultiByteToWideChar(CP_ACP,	// CODE PAGE: ANSI code page
		0,
		pszMB_src,
		(int)strlen(pszMB_src) + 1,
		pszWC_dest,
		iCountWC_dest);
}

int NhW2W(LPCWSTR pszWC_src, LPWSTR pszWC_dest, int iCountWC_dest)
{
	return wcsncpy_s(pszWC_dest, iCountWC_dest, pszWC_src, _TRUNCATE);
}
