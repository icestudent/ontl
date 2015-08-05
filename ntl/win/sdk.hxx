/**\file*********************************************************************
 *                                                                     \brief
 *  Windows SDK support
 *
 ****************************************************************************
 */
#ifndef NTL__WIN_SDK
#define NTL__WIN_SDK
#pragma once

#define NTL_NO_SAL
#define __optional
#define __STDC_WANT_SECURE_LIB__ 0
#define _SECURE_COMPILER_COM 0

#include <wchar.h>
#include <Windows.h>

#ifdef small
#undef small
#endif

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <stdio.h>
#include <locale>


inline int std::iswdigit(wint_t wc)
{
  return std::isdigit(static_cast<char>(wc), std::locale::classic());
}

inline int std::iswspace(wint_t wc)
{
  return std::isspace(static_cast<char>(wc), std::locale::classic());
}

#endif//#ifndef NTL__WIN_SDK
