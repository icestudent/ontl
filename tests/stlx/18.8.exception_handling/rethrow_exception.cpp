// { dg-options "-std=gnu++0x" }
// { dg-require-atomic-builtins "" }

// 2008-05-25  Sebastian Redl  <sebastian.redl@getdesigned.at>

// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// rethrow_exception() and preservation of data

#include <ntl-tests-common.hxx>

#include <exception>

STLX_DEFAULT_TESTGROUP_NAME("std::rethrow_exception");

template<> template<> void tut::to::test<01>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  try {
    rethrow_exception(copy_exception(0));
  } catch(int i){
    quick_ensure(i == 0);
  } catch(...) {
    tut::fail(extmsg("wrong exception type"));
  }
}

template<> template<> void tut::to::test<02>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  try {
    rethrow_exception(copy_exception(runtime_error("test")));
  } catch(exception &e) {
    VERIFY( typeid(e) == typeid(runtime_error) );
    VERIFY( strcmp(e.what(), "test") == 0 );
  }
}

template<> template<> void tut::to::test<03>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  exception_ptr ep;
  try {
    throw 0;
  } catch(...) {
    ep = current_exception();
  }
  try {
    rethrow_exception(ep);
  } catch(int i){
    quick_ensure(i == 0);
  } catch(...) {
    tut::fail(extmsg("wrong exception type"));
  }
}

template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;
#if defined(__ICL) && defined(_M_IX86)
  tut::skip("isn't works with icc");
#endif

  // Weave the exceptions in an attempt to confuse the machinery.
  try {
    throw 0;
  } catch(...) {
    exception_ptr ep1 = current_exception();
    try {
      throw 1;
    } catch(...) {
      exception_ptr ep2 = current_exception();
      try {
        rethrow_exception(ep1);
      } catch(...) {
        try {
          rethrow_exception(ep2);
        } catch(...) {
          try {
            rethrow_exception(ep1);
          } catch(int i){
            quick_ensure(i == 0);
          } catch(...) {
            tut::fail(extmsg("wrong exception type"));
          }
          try {
            rethrow_exception(ep2);
          } catch(int i){
            quick_ensure(i == 1);
          } catch(...) {
            tut::fail(extmsg("wrong exception type"));
          }
        }
      }
    }
  }
}

