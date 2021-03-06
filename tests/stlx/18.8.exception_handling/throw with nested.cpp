// { dg-options "-std=gnu++0x" }
// { dg-require-atomic-builtins "" }

// Copyright (C) 2009 Free Software Foundation, Inc.
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

#include <exception>
#include <ntl-tests-common.hxx>

STLX_DEFAULT_TESTGROUP_NAME("std::throw_with_nested");

namespace 
{
  struct derived : std::nested_exception { };

  struct not_derived { virtual ~not_derived(){} };
}

template<> template<> void tut::to::test<03>() 
{

  bool test = false;
  try {
    throw 1;
  }
  catch(...){
    try {
      throw;
    }
    catch(int i){
      test = true;
    }
    catch(...){
      test = false;
    }
  }
  return;

}

template<> template<> void tut::to::test<01>() 
{
  //tut::skip();
  bool test __attribute__((unused)) = false;

  try {
    try
    {
      std::throw_with_nested(derived());
    }
    //catch (const derived&){
    //  test = true;
    //}
    catch (const std::nested_exception& e)
    {
      const std::type_info& ti = std::current_exception()->target_type();
      VERIFY( e.nested_ptr() == 0 );
      try
      {
        throw;
      }
      catch (const derived&)
      {
        test = true;
      }
      catch(...){
        const std::type_info& ti = std::current_exception()->target_type();
      }
    }
  }catch(...){
    const std::type_info* ti = &std::current_exception()->target_type();
    test = false;
  }
  VERIFY( test );
}

template<> template<> void tut::to::test<02>() 
{
  //tut::skip();
  bool test __attribute__((unused)) = false;

  try
  {
    std::throw_with_nested(not_derived());
  }
  catch (const std::nested_exception& e)
  {
    VERIFY( e.nested_ptr() == 0 );
    try
    {
      throw;
    }
    catch (const not_derived&)
    {
      test = true;
    }
  }
  VERIFY( test );
}
