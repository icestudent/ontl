/**\file*********************************************************************
 *                                                                     \brief
 *  27.6.3 Standard manipulators [std.manip]
 *  1 The header &ltiomanip&gt defines several functions that support extractors
 *    and inserters that alter information maintained by class ios_base and
 *    its derived classes.
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_IOMANIP
#define NTL__STLX_IOMANIP
#pragma once

#include "ios.hxx"
#include "istream.hxx"
#include "ostream.hxx"

namespace std {

namespace __ {
struct resetiosflags
{
  resetiosflags(ios_base::fmtflags mask) : mask(mask) {/**/}
  operator ios_base::fmtflags() const { return mask; }
  ios_base::fmtflags const mask;
  private: void operator=(const resetiosflags &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::resetiosflags & mask)
{
  in.setf(ios_base::fmtflags(0), mask);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::resetiosflags & mask)
{
  out.setf(ios_base::fmtflags(0), mask);
  return out;
}

__forceinline
__::resetiosflags resetiosflags(ios_base::fmtflags mask)
{
  return __::resetiosflags(mask);
}


namespace __ {
struct setiosflags
{
  setiosflags(ios_base::fmtflags mask) : mask(mask) {/**/}
  operator ios_base::fmtflags() const { return mask; }
  ios_base::fmtflags const mask;
  private: void operator=(const setiosflags &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::setiosflags & mask)
{
  in.setf(mask);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::setiosflags & mask)
{
  out.setf(mask);
  return out;
}

__forceinline
__::setiosflags setiosflags(ios_base::fmtflags mask)
{
  return __::setiosflags(mask);
}


namespace __ {
struct setbase
{
  setbase(int base) : base(base) {/**/}
  operator int() const { return base; }
  int const base;
  private: void operator=(const setbase &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::setbase & base)
{
  in.setf(base == 8 ? ios_base::oct :
          base == 10 ? ios_base::dec :
          base == 16 ? ios_base::hex :
          ios_base::fmtflags(0),
          ios_base::basefield);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::setbase & base)
{
  out.setf(base == 8 ? ios_base::oct :
          base == 10 ? ios_base::dec :
          base == 16 ? ios_base::hex :
          ios_base::fmtflags(0),
          ios_base::basefield);
  return out;
}

__forceinline
__::setbase setbase(int base)
{
  return __::setbase(base);
}

namespace __ {
template<typename charT> struct setfill
{
  setfill(charT fill) : fill(fill) {/**/}
  operator charT() const { return fill; }
  charT const fill;
  private: void operator=(const setfill &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::setfill<charT> & fill)
{
  in.fill(fill);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::setfill<charT> & fill)
{
  out.fill(fill);
  return out;
}

template<typename charT>
__::setfill<charT>
  setfill(charT c);

namespace __ {
struct setprecision
{
  setprecision(int n) : n(n) {/**/}
  operator int() const { return n; }
  int const n;
  private: void operator=(const setprecision &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::setprecision & n)
{
  in.precision(n);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::setprecision & n)
{
  out.precision(n);
  return out;
}

__forceinline
__::setprecision setprecision(int n)
{
  return __::setprecision(n);
}


namespace __ {
struct setw
{
  setw(int n) : n(n) {/**/}
  operator int() const { return n; }
  int const n;
  private: void operator=(const setw &);
};
}

template<typename charT, typename traits>
__forceinline
basic_istream<charT, traits>&
  operator>>(basic_istream<charT, traits>& in, const __::setw & n)
{
  in.width(n);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const __::setw & n)
{
  out.width(n);
  return out;
}

__forceinline
__::setw setw(int n)
{
  return __::setw(n);
}


}//namespace std

#endif//#ifndef NTL__STLX_IOMANIP
