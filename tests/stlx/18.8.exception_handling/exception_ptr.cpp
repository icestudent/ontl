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

// current_exception() under various conditions.

#include <ntl-tests-common.hxx>

#include <exception>


STLX_CLASS_TESTGROUP(std::exception_ptr);

template<> template<> void tut::to::test<0>()
{
  std::exception_ptr ptr;
  quick_ensure(ptr == 0);
#if _MSC_VER > 1400
  quick_ensure(ptr == nullptr);
#endif
}

template<> template<> void tut::to::test<01>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  exception_ptr ep = current_exception();
  VERIFY( ep == 0 );
}

template<> template<> void tut::to::test<02>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  try {
    throw 0;
  } catch(...) {
    exception_ptr ep = current_exception();
    VERIFY( ep != 0 );
  }
}

template<> template<> void tut::to::test<03>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  try {
    throw exception();
  } catch(std::exception&) {
    exception_ptr ep = current_exception();
    VERIFY( ep != 0 );
  }
}

template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  //tut::skip();
  try {
    throw 0;
  } catch(...) {
    exception_ptr ep1 = current_exception();
    try {
      throw 0;
    } catch(...) {
      exception_ptr ep2 = current_exception();
      VERIFY( ep1 != ep2 );
    }
    exception_ptr ep3 = current_exception();

    // Not guaranteed by standard, and not true for oNTL.
    //VERIFY( ep1 == ep3 );

    // oNTL specific
    VERIFY( *ep1 == *ep3 );
  }
}
