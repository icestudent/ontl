#ifndef NTL__STLX_MOVE
#define NTL__STLX_MOVE
#pragma once

#ifndef NTL__CXX_RV

/// move emulation support
template<typename U>
struct _rvalue
{
  U& x;

  _rvalue(const U& x)
    :x(const_cast<U&>(x))
  {}

  operator U&() const { return x; }
  U* operator&() const { return &x; }
private:
  void operator=(_rvalue&) __deleted;
};

#endif // rv


#ifdef NTL__CXX_RV
# define __rvalue(T) T&&
# define __lvalue(rhs) (&rhs)
#else
# define __rvalue(T) _rvalue<T>
# define __lvalue(rhs) (&rhs.x)
#endif


#endif // NTL__STLX_MOVE
