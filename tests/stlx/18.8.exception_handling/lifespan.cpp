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

// Tests the life span of the exception object.

#include <exception>
#include <ntl-tests-common.hxx>

STLX_DEFAULT_TESTGROUP_NAME("std::exception_ptr#lifespan");

namespace 
{

  volatile bool may_destruct = false;

  class destructing
  {
    mutable bool copied;

  public:
    destructing() : copied(false) { }
    destructing(const destructing &o) : copied(false) { o.copied = true; }
    ~destructing()
    {
      ntl::nt::dbg::info.printf("tut::ensure(%d)\n", copied || may_destruct);
      assert(copied || may_destruct);
      VERIFY( copied || may_destruct );
    }
  };
}

template<> template<> void tut::to::test<01>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  may_destruct = false;

  // Test the destructing class.
  {
    destructing *d = new destructing;
    destructing d2(*d);
    delete d;
    may_destruct = true;
  }
  may_destruct = false;
}

template<> template<> void tut::to::test<02>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  may_destruct = false;

  try {
    throw destructing();
  } catch(...) {
    may_destruct = true;
  }
  may_destruct = false;
}

template<> template<> void tut::to::test<03>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  may_destruct = false;

  try {
    throw destructing();
  } catch(...) {
    {
      exception_ptr ep = current_exception();
      may_destruct = true;  // ontl isn't sharing current exception object, so current_exception()->copied == false
    }
  }
  may_destruct = false;
}

template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  may_destruct = false;

  {
    exception_ptr ep;
    try {
      throw destructing();
    } catch(...) {
      ep = current_exception();
    }
    may_destruct = true;
  }
  may_destruct = false;
}

void test05_helper()
{
  using namespace std;
  try {
    throw destructing();
  } catch(...) {
    exception_ptr ep = current_exception();
    rethrow_exception(ep);
  }
}

template<> template<> void tut::to::test<05>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

#if defined(__ICL) && defined(_M_IX86)
  tut::skip("isn't works with icc");
#endif
  may_destruct = false;

  try {
    test05_helper();
  } catch(...) {
    may_destruct = true;
  }
  may_destruct = false;
}

void test06_helper()
{
  using namespace std;
  try {
    throw destructing();
  } catch(...) {
    exception_ptr ep = current_exception();
    may_destruct = true; // because the current_exception() object was destructed before the thrown
    throw;
  }
}

template<> template<> void tut::to::test<06>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

#if defined(__ICL) && defined(_M_IX86)
  tut::skip("isn't works with icc");
#endif
  may_destruct = false;

  try {
    test06_helper();
  } catch(...) {
    may_destruct = false;
  }
  may_destruct = false;
}

std::exception_ptr gep;

template<> template<> void tut::to::test<99>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  may_destruct = false;

  try {
    throw destructing();
  } catch(...) {
    gep = current_exception();
  }
}
