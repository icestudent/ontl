// 1999-06-08 bkoz

// Copyright (C) 1999, 2003, 2009 Free Software Foundation, Inc.
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

// 21.3.4 basic_string element access

#include <string>
#include <stdexcept>
#include <ntl-tests-common.hxx>

STLX_DEFAULT_TESTGROUP_NAME("std::string#access");

template<> template<> void tut::to::test<01>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::const_reference cref;
  typedef std::string::reference ref;
  csize_type csz01, csz02;

  const std::string str01("tamarindo, costa rica");
  std::string str02("41st street beach, capitola, california");
  std::string str03;

  // const_reference operator[] (size_type pos) const;
  csz01 = str01.size();
  cref cref1 = str01[csz01 - 1];
  VERIFY( cref1 == 'a' );
  cref cref2 = str01[csz01];
  VERIFY( cref2 == char() );

  // reference operator[] (size_type pos);
  csz02 = str02.size();
  ref ref1 = str02[csz02 - 1];
  VERIFY( ref1 == 'a' );
  ref ref2 = str02[1];
  VERIFY( ref2 == '1' );

  // const_reference at(size_type pos) const;
  csz01 = str01.size();
  cref cref3 = str01.at(csz01 - 1);
  VERIFY( cref3 == 'a' );
  try {
    str01.at(csz01);
    VERIFY( false ); // Should not get here, as exception thrown.
  }
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  // reference at(size_type pos);
  csz01 = str02.size();
  ref ref3 = str02.at(csz02 - 1);
  VERIFY( ref3 == 'a' );
  try {
    str02.at(csz02);
    VERIFY( false ); // Should not get here, as exception thrown.
  }
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

}

// Do a quick sanity check on known problems with element access and
// ref-counted strings. These should all pass, regardless of the
// underlying string implementation, of course.
template<> template<> void tut::to::test<02>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::iterator siterator;
  typedef std::string::reverse_iterator sriterator;
  csize_type csz01, csz02;
  siterator it1;
  sriterator rit1;  

  std::string str01("montara beach, half moon bay");
  const std::string str02("ocean beach, san francisco");
  std::string str03;

  // 21.3 p 5

  // References, pointers, and iterators referring to the elements of
  // a basic_string may be invalidated by the following uses of that
  // basic_string object:

  // ...

  // Susequent to any of the above uses except the forms of insert()
  // and erase() which return iterators, the first call to non-const
  // member functions operator[](), at(), begin(), rbegin(), end(), or
  // rend()

  str03 = str01;
  it1 = str01.begin();
  *it1 = 'x';
  VERIFY( str01[0] == 'x' );
  VERIFY( str03[0] == 'm' );

  str03 = str01; 
  csz01 = str01.size();
  rit1 = str01.rbegin(); // NB: Pointing at one-past the end, so ...
  *rit1 = 'z'; 		 // ... but it's taken care of here 
  VERIFY( str01[csz01 - 1] == 'z' );
  VERIFY( str03[csz01 - 1] == 'y' );

  str03 = str01;
  csz01 = str01.size();
  std::string::reference r1 = str01.at(csz01 - 2);
  VERIFY( str03 == str01 );
  r1 = 'd';
  VERIFY( str01[csz01 - 2] == 'd' );
  VERIFY( str03[csz01 - 2] == 'a' );

  str03 = str01; 
  csz01 = str01.size();
  std::string::reference r2 = str01[csz01 - 3];
  VERIFY( str03 == str01 );
  r2 = 'w'; 
  VERIFY( str01[csz01 - 3] == 'w' );
  VERIFY( str03[csz01 - 3] == 'b' );

  str03 = str01;
  csz02 = str01.size();
  it1 = str01.end();
  VERIFY( str03 == str01 );
  --it1;
  *it1 = 'q'; 
  VERIFY( str01[csz02 - 1] == 'q' );
  VERIFY( str03[csz02 - 1] == 'z' );

  str03 = str01;
  rit1 = str01.rend();
  VERIFY( str03 == str01 );
  --rit1; 	
  *rit1 = 'p'; 
  VERIFY( str01[0] == 'p' );
  VERIFY( str03[0] == 'x' );

  // need to also test for const begin/const end

}

// Do another sanity check, this time for member functions that return
// iterators, namely insert and erase.
template<> template<> void tut::to::test<03>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::iterator siterator;
  typedef std::string::reverse_iterator sriterator;
  siterator it1;
  sriterator rit1;  

  const std::string str01("its beach, santa cruz");

  std::string str02 = str01;
  std::string str05 = str02; // optional, so that begin below causes a mutate
  std::string::iterator p = str02.insert(str02.begin(), ' ');
  std::string str03 = str02;
  VERIFY( str03 == str02 );
  *p = '!';
  VERIFY( *str03.c_str() == ' ' );
  str03[0] = '@';
  VERIFY( str02[0] == '!' );
  VERIFY( *p == '!' );
  VERIFY( str02 != str05 );
  VERIFY( str02 != str03 );

  std::string str10 = str01;
  std::string::iterator p2 = str10.insert(str10.begin(), 'a');
  std::string str11 = str10;
  *p2 = 'e';
  VERIFY( str11 != str10 );

  std::string str06 = str01;
  std::string str07 = str06; // optional, so that begin below causes a mutate
  p = str06.erase(str06.begin());
  std::string str08 = str06;
  VERIFY( str08 == str06 );
  *p = '!';
  VERIFY( *str08.c_str() == 't' );
  str08[0] = '@';
  VERIFY( str06[0] == '!' );
  VERIFY( *p == '!' );
  VERIFY( str06 != str07 );
  VERIFY( str06 != str08 );

  std::string str12 = str01;
  p2 = str12.erase(str12.begin(), str12.begin() + str12.size() - 1);
  std::string str13 = str12;
  *p2 = 'e';
  VERIFY( str12 != str13 );

}

template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;
  using namespace std;

  for (int i = 0; i < 2000; ++i)
  {
    string str_01;

    for (int j = 0; j < i; ++j)
      str_01 += 'a';

    str_01.reserve(i + 10);

    const string str_02(str_01);
    VERIFY( str_02[i] == '\0' );
  }
}

// as per 21.3.4
template<> template<> void tut::to::test<05>()
{
  bool test __attribute__((unused)) = true;

  {
    std::string empty;
    char c = empty[0];
    VERIFY( c == char() );
  }

  {
    const std::string empty;
    char c = empty[0];
    VERIFY( c == char() );
  }
}