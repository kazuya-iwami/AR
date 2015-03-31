/**
 * NhConversion.h
 * HTTP通信のためのサンプルコードにあったファイル
 * 中身はちゃんと検証してません...(2015/3/31現在)
 */
#pragma once

LPSTR  NhM2M(LPCSTR  pszMultiByte);
LPSTR  NhW2M(LPCWSTR pszWideChar);
LPWSTR NhM2W(LPCSTR  pszMultiByte);
LPWSTR NhW2W(LPCWSTR pszWideChar);

int NhM2M(LPCSTR  pszMB_src, LPSTR  pszMB_dest, int iCountMB_dest);
int NhW2M(LPCWSTR pszWC_src, LPSTR  pszMB_dest, int iCountMB_dest);
int NhM2W(LPCSTR  pszMB_src, LPWSTR pszWC_dest, int iCountWC_dest);
int NhW2W(LPCWSTR pszWC_src, LPWSTR pszWC_dest, int iCountWC_dest);

#if defined(UNICODE)
#define NhT2W	NhW2W
#define NhT2M	NhW2M
#define NhW2T	NhW2W
#define NhM2T	NhM2W
#else
#define NhT2W	NhM2W
#define NhT2M	NhM2M
#define NhW2T	NhW2M
#define NhA2T	NhA2M
#endif

