/**\file*********************************************************************
 *                                                                     \brief
 *  C Library General utilities (ISO9899 7.20) + platform specific extensions
 *
 ****************************************************************************
 */
#ifndef NTL__STDLIB
#define NTL__STDLIB
#pragma once

#include "stdint.h"
#include "stlx/cstddef.hxx"

namespace ntl {


template<typename T, typename T2>
static __forceinline
T brute_cast(T2 pf)
{
  static_assert(sizeof(T) == sizeof(T2), "types are incompatible");
  return *reinterpret_cast<const T*>(&pf);
}


#ifdef _MSC_VER

namespace intrinsic {

extern "C" uint8_t _rotl8(uint8_t value, uint8_t shift);
extern "C" uint16_t _rotl16(uint16_t value, uint8_t shift);
extern "C" uint32_t _lrotl(uint32_t value, uint8_t shift);
extern "C" uint64_t _rotl64(uint64_t value, uint8_t shift);

extern "C" uint8_t _rotr8(uint8_t value, uint8_t shift);
extern "C" uint16_t _rotr16(uint16_t value, uint8_t shift);
extern "C" uint32_t _lrotr(uint32_t value, uint8_t shift);
extern "C" uint64_t _rotr64(uint64_t value, uint8_t shift);

extern "C" uint16_t _byteswap_ushort (uint16_t value);
extern "C" uint32_t _byteswap_ulong (uint32_t value);
extern "C" uint64_t _byteswap_uint64(uint64_t value);
#ifndef __ICL
#pragma intrinsic(_rotr8, _rotr16, _lrotr, _rotr64)
#pragma intrinsic(_rotl8, _rotl16, _lrotl, _rotl64)
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#endif

}//namespace intrinsic


///\name  Rotations

static inline
uint8_t
  rotl(uint8_t value, uint8_t shift)
{
  return intrinsic::_rotl8(value, shift);
}

static inline
uint16_t
  rotl(uint16_t value, uint8_t shift)
{
  return intrinsic::_rotl16(value, shift);
}

static inline
uint32_t
  rotl(uint32_t value, uint8_t shift)
{
  return intrinsic::_lrotl(value, shift);
}

static inline
uint64_t
  rotl(uint64_t value, uint8_t shift)
{
  return intrinsic::_rotl64(value, shift);
}

static inline
uint8_t
  rotr(uint8_t value, uint8_t shift)
{
  return intrinsic::_rotr8(value, shift);
}

static inline
uint16_t
  rotr(uint16_t value, uint8_t shift)
{
  return intrinsic::_rotr16(value, shift);
}

static inline
uint32_t
  rotr(uint32_t value, uint8_t shift)
{
  return intrinsic::_lrotr(value, shift);
}

static inline
uint64_t
  rotr(uint64_t value, uint8_t shift)
{
  return intrinsic::_rotr64(value, shift);
}


///\name  Bytes swap

static inline
uint16_t
  bswap(uint16_t value)
{
  return intrinsic::_byteswap_ushort(value);
}

static inline
uint32_t
  bswap(uint32_t value)
{
  return intrinsic::_byteswap_ulong(value);
}

static inline
uint64_t
  bswap(uint64_t value)
{
  return intrinsic::_byteswap_uint64(value);
}


///\name  Endian conversions

/// host <-> big-endian
template<typename type>
static inline
type
  big_endian(const type value)
{
  return bswap(value);
}

/// host <-> little-endian
template<typename type>
static inline
type
  little_endian(const type value)
{
  return value;
}

#else

#endif  //_MSC_VER


///\name  Bitwise operations

template<typename type>
static inline
type
  bitwise_or(const type m, const type m2)
{
  return static_cast<type>(static_cast<unsigned>(m)
                         | static_cast<unsigned>(m2));
}

template<typename type>
static inline
type
  bitwise_and(const type m, const type m2)
{
  return static_cast<type>(static_cast<unsigned>(m)
                         & static_cast<unsigned>(m2));
}

template<typename type>
static inline
type
  bitwise_xor(const type m, const type m2)
{
  return static_cast<type>(static_cast<unsigned>(m)
                         ^ static_cast<unsigned>(m2));
}


///\name   POD object memory operations (platform-specific)

/// zeroes object's memory
template<typename type>
static inline
void zero_mem(type & object)
{
  char * const p = reinterpret_cast<char*>(&object);
  for ( unsigned i = 0; i != sizeof(type); ++i )
    p[i] = 0; //-V108
}

/// compares object's memory
template<typename type>
static inline
bool binary_equal(const type & o, const type & o2)
{
  const char * p  = reinterpret_cast<const char*>(&o);
  const char * p2 = reinterpret_cast<const char*>(&o2);
  for ( const char * const end = p + sizeof(type); p != end; ++p, ++p2 )
    if ( *p != *p2 ) return false;
  return true;
}


template<typename T, typename T2>
static inline
T align_up(T what, T2 alignment)
{
  return ((what + alignment - 1) / alignment) * alignment;
}

template<typename T, typename T2>
static inline
T align_down(T what, T2 alignment)
{
  return (what / alignment) * alignment;
}

template<typename T, typename U>
inline T padd(T p, U offset)
{
  return (T)( uintptr_t(p) + uintptr_t(offset) );
}

template<typename R, typename T, typename U>
inline R padd(T p, U offset)
{
  return (R)( uintptr_t(p) + uintptr_t(offset) );
}

///@}


}//namespace ntl

#endif//#ifndef NTL__STDLIB
