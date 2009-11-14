// 1999-06-10 bkoz

// Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2002,
// 2003, 2004, 2005, 2006, 2007, 2009 Free Software Foundation, Inc.
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

// 21.3.5.6 basic_string::replace

#pragma warning(disable:4244) // for int ar[] = { 'H', 'e', 'l', 'l', 'o' };
#include <ntl-tests-common.hxx>

#include <string>
#include <algorithm> // for std::find

STLX_DEFAULT_TESTGROUP_NAME("std::string#replace");

template<> template<> void tut::to::test<01>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::const_reference cref;
  typedef std::string::reference ref;

  const char str_lit01[] = "ventura, california";
  const std::string str01(str_lit01);
  std::string str02("del mar, california");
  std::string str03(" and ");
  std::string str05;

  // string& replace(size_type pos, size_type n, const string& string)
  // string& replace(size_type pos1, size_type n1, const string& string,
  //                 size_type pos2, size_type n2)
  // string& replace(size_type pos, size_type n1, const char* s, size_type n2)
  // string& replace(size_type pos, size_type n1, const char* s)
  // string& replace(size_type pos, size_type n1, size_type n2, char c)
  // string& replace(iterator it1, iterator it2, const string& str)
  // string& replace(iterator it1, iterator it2, const chat* s, size_type n)
  // string& replace(iterator it1, iterator it2, const chat* s)
  // string& replace(iterator it1, iterator it2, size_type n, char c)
  // template<typename InputIter>
  //   string& replace(iterator it1, iterator it2, InputIter j1, InputIter j2)

  // with mods, from tstring.cc, from jason merrill, et. al.
  std::string X = "Hello";
  std::string x = X;

  char ch = x[0];
  VERIFY( ch == 'H' );

  std::string z = x.substr(2, 3);
  VERIFY( z == "llo" );

  x.replace(2, 2, "r");
  VERIFY( x == "Hero" );

  x = X;
  x.replace(0, 1, "j");
  VERIFY( x == "jello" );

  int ar[] = { 'H', 'e', 'l', 'l', 'o' };
  x.replace(std::find(x.begin(), x.end(), 'l'), 
    std::find(x.rbegin(), x.rend(), 'l').base(), ar, 
    ar + sizeof(ar) / sizeof(ar[0]));
  VERIFY( x == "jeHelloo" );

}

template<> template<> void tut::to::test<02>()
{
  bool test __attribute__((unused)) = true;
  const char* strlit = "../the long pier/Hanalei Bay/Kauai/Hawaii";
  std::string aux = strlit;
  aux.replace(aux.begin()+5, aux.begin()+20,
    aux.begin()+10, aux.begin()+15);
  VERIFY(aux == "../thg piealei Bay/Kauai/Hawaii");

  aux = strlit;
  aux.replace(aux.begin() + 10, aux.begin() + 15,
    aux.begin() + 5, aux.begin() + 20);
  VERIFY(aux == "../the lone long pier/Hanr/Hanalei Bay/Kauai/Hawaii");
}

// Some more miscellaneous tests
template<> template<> void tut::to::test<03>()
{
  bool test __attribute__((unused)) = true;
  const char* title01 = "nine types of ambiguity";
  const char* title02 = "ultra";
  std::string str01 = title01;
  std::string str02 = title02;

  str01.replace(0, 4, str02);
  VERIFY(str01 == "ultra types of ambiguity");

  str01.replace(15, 9, str02, 2, 2);
  VERIFY(str01 == "ultra types of tr");

  str01 = title01;
  str02.replace(0, 0, str01, 0, std::string::npos);
  VERIFY(str02 == "nine types of ambiguityultra");

  str02.replace(11, 2, title02, 5);
  VERIFY(str02 == "nine types ultra ambiguityultra");

  str02.replace(11, 5, title01, 2);
  VERIFY(str02 == "nine types ni ambiguityultra");

  str01.replace(str01.size(), 0, title02);
  VERIFY(str01 == "nine types of ambiguityultra");

  str01 = title01;
  str02 = title02;
  str01.replace(str01.begin(), str01.end(), str02);
  VERIFY(str01 == "ultra");

  str01.replace(str01.begin(), str01.begin(), title01, 4);
  VERIFY(str01 == "nineultra");

  str01.replace(str01.end(), str01.end(), title01 + 5, 5);
  VERIFY(str01 == "nineultratypes");

  str01.replace(str01.begin(), str01.end(), title02);
  VERIFY(str01 == "ultra");
}


// Some more tests for 
// template<typename InputIter>
//   string& replace(iterator it1, iterator it2, InputIter j1, InputIter j2)
template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;
  std::string str01 = "geogaddi";
  std::string str02;

  typedef std::string::iterator iterator;
  typedef std::string::const_iterator const_iterator;

  iterator it1 = str01.begin();
  iterator it2 = str01.end();
  str02.replace(str02.begin(), str02.end(), it1, it2);
  VERIFY(str02 == "geogaddi");

  str02 = "boards";
  const_iterator c_it1 = str01.begin();
  const_iterator c_it2 = str01.end();
  str02.replace(str02.begin(), str02.end(), c_it1, c_it2);
  VERIFY(str02 == "geogaddi");

  str02 = "boards";
  const char* c_ptr1 = str01.c_str();
  const char* c_ptr2 = str01.c_str() + 8;
  str02.replace(str02.begin(), str02.end(), c_ptr1, c_ptr2);
  VERIFY(str02 == "geogaddi");

  str02 = "boards";
  char* ptr1 = &*str01.begin();
  char* ptr2 = ptr1 + str01.length();
  str02.replace(str02.begin(), str02.end(), ptr1, ptr2);
  VERIFY(str02 == "geogaddi");
}


// We wrongly used __n1 instead of __foldn1 in the length_error
// check at the beginning of replace(__pos, __n1, __s, __n2)
template<> template<> void tut::to::test<05>()
{
  bool test __attribute__((unused)) = true;
  std::string str01 = "londinium";
  std::string str02 = "cydonia";

  str01.replace(0, 20, str02.c_str(), 3);
  VERIFY(str01 == "cyd");
}


template<> template<> void tut::to::test<06>()
{
  bool test __attribute__((unused)) = true;

  std::string str01("Valle Del Salto");
  str01.replace(0, 5, str01.data() + 10, 5);
  VERIFY( str01 == "Salto Del Salto" );

  std::string str02("Colle di Val d'Elsa");
  str02.replace(0, 9, str02.data() + 10, 0);
  VERIFY( str02 == "Val d'Elsa" );

  std::string str03("Novi Ligure");
  str03.replace(11, 0, str03.data() + 4, 7);
  VERIFY( str03 == "Novi Ligure Ligure");

  std::string str04("Trebisacce");
  str04.replace(3, 4, str04.data(), 0);
  VERIFY( str04 == "Trecce" );

  std::string str05("Altopiano della Sila");
  str05.replace(1, 18, str05.data() + 19, 1);
  VERIFY( str05 == "Aaa" );
}
