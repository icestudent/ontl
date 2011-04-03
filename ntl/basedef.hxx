/**\file*********************************************************************
 *                                                                     \brief
 *  Native Template Library types
 *
 ****************************************************************************
 */
#ifndef NTL__BASEDEF
#define NTL__BASEDEF
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "stlx/cstddef.hxx"
#endif
#ifndef NTL__STLX_CSTDINT
#include "stlx/cstdint.hxx"
#endif

// std::vector forward declaration
namespace std {
  template<class T> class allocator;
  template <class T, class Allocator> class vector;
} // std

/// Native Template Lybrary
namespace ntl {

/**\addtogroup  ntative_types ********** NT Native Types ********************
 *@{*/

///\name  Exact-width integer types
typedef std::uint8_t byte;

using std::int8_t;
using std::uint8_t;
using std::int16_t;
using std::uint16_t;
using std::int32_t;
using std::uint32_t;
using std::int64_t;
using std::uint64_t;

///\name  Integer types capable of holding object pointers
using std::intptr_t;
using std::uintptr_t;

using std::ptrdiff_t;

/// raw data representation
typedef std::vector<uint8_t, std::allocator<uint8_t> > raw_data;

namespace __unspecified
{
  struct noncopyable
  {
  protected:
    noncopyable(){}
    ~noncopyable(){}
  private:
    noncopyable(const noncopyable&) __deleted;
    const noncopyable& operator=(const noncopyable&) __deleted;
  };
}

/// Base class to deny copying of derived classes.
typedef __unspecified::noncopyable noncopyable;

/// Type helper for the template specialization for functions 
template<typename T>
struct type2type
{
  typedef T type;
};

/// Integral value helper for the template specialization for functions 
template<size_t v>
struct int2type
{
  enum { value = v };
};

/// class_enum by remark
#if 0
template<typename def, typename inner = def::type>
struct class_enum: def
{
  typedef typename def::type type;
  typedef typename inner inner;
  
  inline class_enum(type v)
    :v(static_cast<type>(value))
  {}
  inline operator inner() const { return value; }

  inner value;
};
#else
template<typename def>
struct class_enum: public def
{
  typedef typename def::type type;
  __forceinline
    class_enum(type v) : value(v) {}
  __forceinline
    operator type () const { return value; }
  type value;
};
#endif

#define __class_enum(name) struct name ## _def; typedef ntl::class_enum<name ## _def> name; struct name ## _def { enum type

__class_enum(times) 
{
  nanoseconds   = 1,
  microseconds  = 10   * nanoseconds,
  milliseconds  = 1000 * microseconds,
  seconds       = 1000 * milliseconds,
  minutes       = 60   * seconds
  //  hours         = int64_t(60)   * minutes,
  //  days          = int64_t(24)   * hours,
};};

///@}


}//namespace ntl

#endif//#ifndef NTL__BASEDEF
