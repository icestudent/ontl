/**\file*********************************************************************
 *                                                                     \brief
 *
 *
 ****************************************************************************
 */


#ifndef NTL__FORMAT
#define NTL__FORMAT

#include "base.hxx"

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
  return h[v & 0xf];
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


template<typename char_t = char>
struct hex_str_cast
{
    hex_str_cast(int8 v)    { to_hex(v, buf); }
    hex_str_cast(int16 v)   { to_hex(v, buf); }
    hex_str_cast(int32 v)   { to_hex(v, buf); }
    hex_str_cast(int64 v)   { to_hex(v, buf); }
    hex_str_cast(uint8 v)   { to_hex(v, buf); }
    hex_str_cast(uint16 v)  { to_hex(v, buf); }
    hex_str_cast(uint32 v)  { to_hex(v, buf); }
    hex_str_cast(uint64 v)  { to_hex(v, buf); }
    operator const char_t * () { return buf; }

  private:
    char_t    buf[sizeof(uint64) * 2 + sizeof("0x")];
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
    str_cast(int8 v)    { to_dec(v, buf); }
    str_cast(int16 v)   { to_dec(v, buf); }
    str_cast(int32 v)   { to_dec(v, buf); }
    str_cast(int64 v)   { to_dec(v, buf); }
    str_cast(uint8 v)   { to_dec(v, buf); }
    str_cast(uint16 v)  { to_dec(v, buf); }
    str_cast(uint32 v)  { to_dec(v, buf); }
    str_cast(uint64 v)  { to_dec(v, buf); }
    operator const char_t * () { return buf; }

  private:
    char_t    buf[sizeof(" 18446744073709551615")];
};//struct str_cast


}//namespace fmt

namespace format = fmt;

}//namespace ntl

#endif//#ifndef NTL__FORMAT
