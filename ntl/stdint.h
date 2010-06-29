#ifndef NTL__STDINT
#define NTL__STDINT
#pragma once

#ifndef NTL__STLX_CSTDINT
#include "stlx/cstdint.hxx"
#endif

using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

using std::int_least8_t;
using std::uint_least8_t;
using std::int_least16_t;
using std::uint_least16_t;
using std::int_least32_t;
using std::uint_least32_t;
using std::int_least64_t;
using std::uint_least64_t;

using std::int_fast8_t;
using std::uint_fast8_t;
using std::int_fast16_t;
using std::uint_fast16_t;
using std::int_fast32_t;
using std::uint_fast32_t;
using std::int_fast64_t;
using std::uint_fast64_t;

#ifndef _INTPTR_T_DEFINED
using std::intptr_t;
#endif
#ifndef _UINTPTR_T_DEFINED
using std::uintptr_t;
#endif

using std::intmax_t;
using std::uintmax_t;

///\move this elsewhere
//typedef std::uint8_t byte;

#endif//#ifndef NTL__STDINT
