/**\file*********************************************************************
 *                                                                     \brief
 *
 *
 ****************************************************************************
 */
#ifndef NTL__FORMAT
#define NTL__FORMAT
#pragma once

#include "basedef.hxx"
#include <locale>

namespace ntl {
namespace fmt {

template<typename uint_t>
static inline
char dec_digit(uint_t v) { return static_cast<char>(v) + '0'; }

template<typename uint_t>
static inline
char hex_digit(uint_t v)
{
  static char h[] = "0123456789ABCDEF";
  return h[v & 0x0F];
}


template<typename int_t, typename char_t>
static inline
char_t * to_hex(int_t v, char_t * const p)
{
  p[0] = '0'; p[1] = 'x'; p[2 + 2 * sizeof(int_t)] = '\0';
  for ( size_t i = sizeof(int_t) * 2; i; --i )
  {
    p[2 + i - 1] = static_cast<char_t>(hex_digit(v));
    v >>= 4;
  }
  return &p[2 + 2 * sizeof(int_t)];
}

template<typename char_t>
static inline char_t* to_hex(char_t* dst, size_t dst_len, const void* src, size_t len)
{
  len = std::min(len, dst_len/2);
  const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
  for(size_t i = 0; i < len; i++) {
    const unsigned char c = *p++;
    *dst++ = hex_digit(c >> 4);
    *dst++ = hex_digit(c);
  }
  if(len*2 < dst_len)
    *dst = 0;
  return dst - len*2;
}

template<typename char_t>
static inline size_t from_hex(void* dst, size_t dst_len, const char_t* src, size_t len, const std::locale& locale)
{
	struct {
		byte operator()(char_t c) const
		{
			if(c >= '0' && c <= '9')
				return c - '0';

			if(c >= 'A' && c <= 'F')
				return (c - 'A' + 10);

			if(c >= 'a' && c <= 'f')
				return (c - 'a' + 10);

			return 0;
		}
	} decode;

	auto& ctype = std::use_facet<std::ctype<char_t>>(locale);

	const char_t* end = src + len;
	uint8_t* p = reinterpret_cast<uint8_t*>(dst);
	while(src < end) {
		const char_t c = src[0];
		if(ctype.is(ctype.space, c)) {
			src++;
			continue;
		}
		*p++ = (decode(src[0]) << 4) | decode(src[1]);
		src += 2;
	}
	return p - reinterpret_cast<uint8_t*>(dst);
}

template<typename char_t>
static inline size_t from_hex(void* dst, size_t dst_len, const char_t* src, size_t len)
{
  struct {
    byte operator()(char_t c) const
    {
      if(c >= '0' && c <= '9')
        return c - '0';

      if(c >= 'A' && c <= 'F')
        return (c - 'A' + 10);

      if(c >= 'a' && c <= 'f')
        return (c - 'a' + 10);

      return 0;
    }
  } decode;

  len = std::min(len/2, dst_len);
  uint8_t* p = reinterpret_cast<uint8_t*>(dst);
  for(size_t i = 0; i < len; i++, p++, src += 2) {
    *p = (decode(src[0]) << 4) | decode(src[1]);
  }
  return p - reinterpret_cast<uint8_t*>(dst);
}

template<typename char_t, size_t N>
static inline size_t from_hex(void* dst, size_t dst_len, const char_t (&src)[N])
{
  return from_hex(dst, dst_len, src, N);
}

template<typename char_t = char>
struct hex_str_cast
{
    hex_str_cast(int8_t v)    { to_hex(v, buf); }
    hex_str_cast(int16_t v)   { to_hex(v, buf); }
    hex_str_cast(int32_t v)   { to_hex(v, buf); }
    hex_str_cast(int64_t v)   { to_hex(v, buf); }
    hex_str_cast(uint8_t v)   { to_hex(v, buf); }
    hex_str_cast(uint16_t v)  { to_hex(v, buf); }
    hex_str_cast(uint32_t v)  { to_hex(v, buf); }
    hex_str_cast(uint64_t v)  { to_hex(v, buf); }
    operator const char_t * () { return buf; }

  private:
    char_t    buf[sizeof(uint64_t) * 2 + sizeof("0x")];
};//struct hex_str_cast


template<typename int_t, typename char_t>
static inline
char_t * to_dec(int_t v, char_t * const p)
{
  static const bool signed_type = static_cast<int_t>(-1) < 0;
  int i = 0;
  int j = signed_type && v < 0;
  if ( j )
  {
    p[i++] = '-';
    v = static_cast<int_t>(0-v);
  }
  do  p[i++] = static_cast<char_t>(dec_digit(v % 10));
  while ( v /= 10 );
  char_t * const end = &p[i];
  p[i--] = '\0';
  for ( ; j < i; ++j, --i )
  {
    char_t t = p[i];
    p[i] = p[j];
    p[j] = t;
  }
  return end;
}

template<typename char_t = char>
struct str_cast
{
    str_cast(int8_t v)    { to_dec(v, buf); }
    str_cast(int16_t v)   { to_dec(v, buf); }
    str_cast(int32_t v)   { to_dec(v, buf); }
    str_cast(int64_t v)   { to_dec(v, buf); }
    str_cast(uint8_t v)   { to_dec(v, buf); }
    str_cast(uint16_t v)  { to_dec(v, buf); }
    str_cast(uint32_t v)  { to_dec(v, buf); }
    str_cast(uint64_t v)  { to_dec(v, buf); }
    operator const char_t * () { return buf; }

  private:
    char_t    buf[sizeof(" 18446744073709551615")];
};//struct str_cast


}//namespace fmt

namespace format = fmt;

}//namespace ntl

#endif//#ifndef NTL__FORMAT
