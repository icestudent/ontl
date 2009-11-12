#pragma once


#ifndef __GNUC__
# define __attribute__(x)
#endif

#ifdef _MSC_VER
# pragma warning(disable:4101 4127 4189 4512)
#endif

#include <tut/tut.hpp>

#define __unwrap(x) x
#define __unwrap1(x) __unwrap(x)
#define __unwrap2(x) __unwrap1(x)

#define __quick_ensure2(cond,line) tut::ensure("'"#cond"' at '"__FILE__":"#line"'", cond)
#define __quick_ensure1(cond,line) __quick_ensure2(cond, line)
#define quick_ensure(cond) __quick_ensure1(cond, __LINE__)

#define __extmsg2(msg,line) "'"msg"' at '"__FILE__":"#line"'"
#define __extmsg1(msg,line) __extmsg2(msg,line)
#define extmsg(msg) __extmsg1(msg,__LINE__)

#define VERIFY(cond) quick_ensure(cond)

namespace
{
  // unique for each test file
  struct empty_type{};
}

// default test group
#define STLX_DEFAULT_TESTGROUP() \
  typedef test_group<empty_type> testgroup; \
  typedef testgroup::object to;
#define STLX_DEFAULT_TESTGROUP_NAME(name) \
  namespace tut { \
    typedef test_group<empty_type> testgroup; \
    typedef testgroup::object to; \
    testgroup tg(name); \
  }

// specify type to test, e.g. stlx::string or stlx::vector<int>
#define STLX_DEFINE_TESTGROUP(class) \
  typedef test_group< class > testgroup; \
  typedef testgroup::object to;
