#pragma once

#include "../cwchar.hxx"

namespace std
{
  wchar_t* NTL_CRTCALL wcstok(wchar_t* __restrict s1, const wchar_t* __restrict s2, wchar_t** __restrict ptr);
  wchar_t* NTL_CRTCALL wcstok(wchar_t* __restrict s1, const wchar_t* __restrict s2);
}

//using std::btowc;
//using std::mbrlen;
//using std::vfwscanf;
using std::wcscpy;
using std::wcsspn;
//using std::wcsxfrm;
//
//using std::fgetwc;
//using std::mbrtowc;
//using std::vswscanf;
//using std::wcscspn;
using std::wcsstr;
//using std::wctob;
//
//using std::fgetws;
//using std::mbsinit;
//using std::vswprintf;
//using std::wcsftime;
//using std::wcstod;
//using std::wmemchr;
//
//using std::fputwc;
//using std::mbsrtowcs;
//using std::vwprintf;
using std::wcslen;
//using std::wcstof;
using std::wmemcmp;
//
//using std::fputws;
//using std::putwc;
//using std::vwscanf;
using std::wcsncat;
using std::wcstok;
using std::wmemcpy;

#define wcstok_s(a,b,c) wcstok(a,b,c)
#define _wcsnset_s(a,b,c,d) errno_t(0)
#define _wcsset_s(a,b,c) errno_t(0)
#define _wcsnset(a,b,c) (wchar_t*)0
#define _wcsset(a,b) (wchar_t*)0


//
//using std::fwide;
//using std::putwchar;
//using std::wcrtomb;
using std::wcsncmp;
//using std::wcstol;
using std::wmemmove;
//
//using std::fwprintf;
//using std::swprintf;
using std::wcscat;
using std::wcsncpy;
//using std::wcstold;
using std::wmemset;
//
//using std::fwscanf;
//using std::swscanf;
using std::wcschr;
//using std::wcspbrk;
//using std::wcstoll;
//using std::wprintf;
//
//using std::getwc;
//using std::ungetwc;
using std::wcscmp;
using std::wcsrchr;
//using std::wcstoul;
//using std::wscanf;
//
//using std::getwchar;
//using std::vfwprintf;
//using std::wcscoll;
//using std::wcsrtombs;
//using std::wcstoull;
