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

#include "ios.hxx"
#include "istream.hxx"
#include "ostream.hxx"

namespace std {

namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::resetiosflags & mask)
{
  in.setf(ios_base::fmtflags(0), mask);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const impl::resetiosflags & mask)
{
  out.setf(ios_base::fmtflags(0), mask);
  return out;
}

__forceinline
impl::resetiosflags resetiosflags(ios_base::fmtflags mask)
{
  return impl::resetiosflags(mask);
}


namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::setiosflags & mask)
{
  in.setf(mask);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const impl::setiosflags & mask)
{
  out.setf(mask);
  return out;
}

__forceinline
impl::setiosflags setiosflags(ios_base::fmtflags mask)
{
  return impl::setiosflags(mask);
}


namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::setbase & base)
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
  operator<<(basic_ostream<charT, traits>& out, const impl::setbase & base)
{
  out.setf(base == 8 ? ios_base::oct :
          base == 10 ? ios_base::dec :
          base == 16 ? ios_base::hex :
          ios_base::fmtflags(0),
          ios_base::basefield);
  return out;
}

__forceinline
impl::setbase setbase(int base)
{
  return impl::setbase(base);
}

namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::setfill<charT> & fill)
{
  in.fill(fill);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const impl::setfill<charT> & fill)
{
  out.fill(fill);
  return out;
}

template<typename charT>
impl::setfill<charT>
  setfill(charT c);

namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::setprecision & n)
{
  in.precision(n);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const impl::setprecision & n)
{
  out.precision(n);
  return out;
}

__forceinline
impl::setprecision setprecision(int n)
{
  return impl::setprecision(n);
}


namespace impl {
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
  operator>>(basic_istream<charT, traits>& in, const impl::setw & n)
{
  in.width(n);
  return in;
}

template<typename charT, typename traits>
__forceinline
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>& out, const impl::setw & n)
{
  out.width(n);
  return out;
}

__forceinline
impl::setw setw(int n)
{
  return impl::setw(n);
}


}//namespace std

#endif//#ifndef NTL__STLX_IOMANIP
