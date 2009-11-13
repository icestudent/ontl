#pragma once

#include <tut/tut.hpp>
#include <nt/debug.hxx>
#include <nt/dbgstream.hxx>
namespace dbg = ntl::nt::dbg;


//////////////////////////////////////////////////////////////////////////
// helper macroses

#define __unwrap(x) x
#define __unwrap1(x) __unwrap(x)
#define __unwrap2(x) __unwrap1(x)

#define __quick_ensure2(cond,line) tut::ensure("'"#cond"' at '"__FILE__":"#line"'", cond)
#define __quick_ensure1(cond,line) __quick_ensure2(cond, line)

#define __extmsg2(msg,line) "'"msg"' at '"__FILE__":"#line"'"
#define __extmsg1(msg,line) __extmsg2(msg,line)





// add additional information for the literal string message
#define extmsg(msg) __extmsg1(msg,__LINE__)

// assert replacement
#define VERIFY(cond) quick_ensure(cond)
#define quick_ensure(cond) __quick_ensure1(cond, __LINE__)


//////////////////////////////////////////////////////////////////////////
// common test settings
namespace
{
  // unique for each test file
  struct default_test_type{};
}

// default test group
#define STLX_DEFAULT_TESTGROUP() \
  namespace { \
    typedef test_group<default_test_type> testgroup; \
    typedef testgroup::object to; \
  }

#define STLX_DEFAULT_TESTGROUP_NAME(name) \
  namespace tut { \
    typedef test_group<default_test_type> testgroup; \
    typedef testgroup::object to; \
  } namespace { \
    tut::testgroup tg(name); \
  }

// specify type to test, e.g. stlx::string or stlx::vector<int>

#define STLX_CLASS_TESTGROUP_NAME(class, name) \
  namespace tut { \
    typedef test_group<class> testgroup; \
    typedef testgroup::object to; \
  } namespace { \
    tut::testgroup tg(name); \
  }

#define STLX_CLASS_TESTGROUP(class) STLX_CLASS_TESTGROUP_NAME(class, #class)

#define STLX_DEFINE_TESTGROUP(class) \
  namespace tut { \
    typedef test_group< class > testgroup; \
    typedef testgroup::object to; \
  }


//////////////////////////////////////////////////////////////////////////
// compiler-specific settings

#ifdef _MSC_VER
# pragma warning(disable:4101 4127 4189) // unreferenced local, conditional expression is constant, unreferenced initialized local, 
#endif
#ifndef __GNUC__
# define __attribute__(x)
#endif
