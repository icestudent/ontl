#pragma once

#include "../cstdio.hxx"

using std::snprintf;
using std::vsnprintf;

int NTL_CRTCALL printf(const char* format, ...);
NTL_EXTERNAPI int __cdecl sscanf(const char* string, const char* format, ...);
