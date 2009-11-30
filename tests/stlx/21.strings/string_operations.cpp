// Copyright (C) 2005, 2009 Free Software Foundation, Inc.
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
// with this library; see the file COPYING3.  If not see121
// <http://www.gnu.org/licenses/>.
#include <ntl-tests-common.hxx>

#include <string>
#include <stdexcept>


STLX_DEFAULT_TESTGROUP_NAME("std::string#operations");

template<> template<> void tut::to::test<01>()
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::const_reference cref;
  typedef std::string::reference ref;
  csize_type csz01;

  const char str_lit01[] = "point bolivar, texas";
  const std::string str01(str_lit01);
  const std::string str02("corpus, ");
  const std::string str03;
  std::string str05;


  // string& append(const string&)
  str05 = str02;
  str05.append(str05); 
  VERIFY( str05 == "corpus, corpus, " );
  str05.append(str01);
  VERIFY( str05 == "corpus, corpus, point bolivar, texas" );
  str05.append(str03);
  VERIFY( str05 == "corpus, corpus, point bolivar, texas" );
  std::string str06;
  str06.append(str05);
  VERIFY( str06 == str05 );


  // string& append(const string&, size_type pos, size_type n)
  str05.erase();
  str06.erase();
  csz01 = str03.size();
  try {
    str06.append(str03, csz01 + 1, 0);
    VERIFY( false ); 
  }
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  csz01 = str01.size();
  try {
    str06.append(str01, csz01 + 1, 0);
    VERIFY( false ); 
  }
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  str05 = str02;
  str05.append(str01, 0, std::string::npos);
  VERIFY( str05 == "corpus, point bolivar, texas" );
  VERIFY( str05 != str02 );

  str06 = str02;
  str06.append(str01, 15, std::string::npos);
  VERIFY( str06 == "corpus, texas" );
  VERIFY( str02 != str06 );


  // string& append(const char* s)
  str05.erase();
  str06.erase();
  str05.append("");
  VERIFY( str05 == str03 );

  str05.append(str_lit01);
  VERIFY( str05 == str01 );

  str06 = str02;
  str06.append("corpus, ");
  VERIFY( str06 == "corpus, corpus, " );


  // string& append(const char* s, size_type n)
  str05.erase();
  str06.erase();
  str05.append("", 0);
  VERIFY( str05.size() == 0 );
  VERIFY( str05 == str03 );

  str05.append(str_lit01, sizeof(str_lit01) - 1);
  VERIFY( str05 == str01 );

  str06 = str02;
  str06.append("corpus, ", 6);
  VERIFY( str06 == "corpus, corpus" );

  str06 = str02;
  str06.append("corpus, ", 12); // UB
  VERIFY( str06 != "corpus, corpus, " );


  // string& append(size_type n, char c)
  str05.erase();
  str06.erase();
  str05.append(0, 'a');
  VERIFY( str05 == str03 );
  str06.append(8, '.');
  VERIFY( str06 == "........" );


  // template<typename InputIter>
  //  string& append(InputIter first, InputIter last)
  str05.erase();
  str06.erase();
  str05.append(str03.begin(), str03.end());
  VERIFY( str05 == str03 );

  str06 = str02;
  str06.append(str01.begin(), str01.begin() + str01.find('r')); 
  VERIFY( str06 == "corpus, point boliva" );
  VERIFY( str06 != str01 );
  VERIFY( str06 != str02 );

  str05 = str01;
  str05.append(str05.begin(), str05.begin() + str05.find('r')); 
  VERIFY( str05 ==  "point bolivar, texaspoint boliva" );
  VERIFY( str05 != str01 );
}

template<> template<> void tut::to::test<02>()
{
  bool test __attribute__((unused)) = true;

  using namespace std;

  string one; 
  string two;
  string three;
  const char * source = "Written in your eyes";

  one.append(source);
  VERIFY( one == "Written in your eyes" );

  two.append(source, 20);
  VERIFY( two == "Written in your eyes" );

  three.append(source, 7);
  VERIFY( three == "Written" );

  three.clear();
  three.append(source + 8, 2);
  VERIFY( three == "in" );

  one.append(one.c_str(), 20);
  VERIFY( one == "Written in your eyesWritten in your eyes" );

  two.append(two.c_str() + 16, 4);
  VERIFY( two == "Written in your eyeseyes" );

  two.append(two.c_str(), 3);
  VERIFY( two == "Written in your eyeseyesWri" );
}

template<> template<> void tut::to::test<03>()
{
  bool test __attribute__((unused)) = true;

  using namespace std;

  const char * source = "Kesto";

  for (unsigned i = 0; i < 10; ++i)
  {
    string one(source);
    string two(source);
    for (unsigned j = 0; j < 18; ++j)
    {
      VERIFY( one == two );
      one.append(one);
      one += 'x';
      two.append(two.c_str(), two.size());
      two += 'x';
    }
  }
}

template<> template<> void tut::to::test<04>()
{
  bool test __attribute__((unused)) = true;

  using namespace std;

  const char* strlit = "../the long pier/Hanalei Bay/Kauai/Hawaii";
  string aux = strlit;
  string::size_type i = aux.rfind("/");
  if (i != string::npos)
    aux.assign(aux, i + 1, string::npos);
  VERIFY(aux == "Hawaii");

  aux = strlit;
  i = aux.rfind("r/");
  if (i != string::npos)
    aux.assign(aux, i + 1, string::npos);
  VERIFY(aux.c_str()[9] == 'B');
  VERIFY(aux == "/Hanalei Bay/Kauai/Hawaii");

  aux.assign(10, '\0');
  VERIFY(aux.length() == 10);
}

template<> template<> void tut::to::test<05>()
{
  bool test __attribute__((unused)) = true;

  using namespace std;

  string one = "Selling England by the pound";
  string two = one;
  string three = "Brilliant trees";

  one.assign(one, 8, 100);
  VERIFY( one == "England by the pound" );

  one.assign(one, 8, 0);
  VERIFY( one == "" );

  one.assign(two, 8, 7);
  VERIFY( one == "England" );

  one.assign(three, 10, 100);
  VERIFY( one == "trees" );

  three.assign(one, 0, 3);
  VERIFY( three == "tre" );
}

template<> template<> void tut::to::test<06>()
{
  bool test __attribute__((unused)) = true;

  using namespace std;

  string one; 
  string two;
  const char * source = "Selling England by the pound";

  one.assign(source);
  VERIFY( one == "Selling England by the pound" );

  one.assign(source, 28);
  VERIFY( one == "Selling England by the pound" );

  two.assign(source, 7);
  VERIFY( two == "Selling" );

  one.assign(one.c_str() + 8, 20);
  VERIFY( one == "England by the pound" );

  one.assign(one.c_str() + 8, 6);
  VERIFY( one == "by the" );
}

template<> template<> void tut::to::test<07>(void)
{
  bool test __attribute__((unused)) = true;

  std::string empty;

  // data() for size == 0 is non-NULL.
  VERIFY( empty.size() == 0 );
  const std::string::value_type* p = empty.data();
  VERIFY( p != NULL );

}

template<> template<> void tut::to::test<8>(void)
{
  bool test __attribute__((unused)) = true;

  std::string str1;
  std::string str2;

  // Should get this:
  // 1:8-chars_8-chars_
  // 2:8-chars_8-chars_
  str1 = std::string("8-chars_") + "8-chars_";
  str1.c_str();
  // printf("1:%s\n", str1.c_str());
  str2 = str1 + "7-chars";
  // printf("2:%s\n", str1.c_str()); //str1 is gone
  str1.c_str();
}

template<> template<> void tut::to::test<9>(void)
{
  bool test __attribute__((unused)) = true;
  std::string 	str_0("costa rica");
  std::string 	str_1("costa marbella");
  std::string 	str_2("cost");
  std::string	str_3("costa ricans");
  std::string        str_4;
  
  str_4 = str_0;
  //comparisons between string objects
  VERIFY( !(str_0 == str_1) );
  VERIFY( !(str_0 == str_2) );
  VERIFY( !(str_0 == str_3) );
  VERIFY( !(str_1 == str_0) );
  VERIFY( !(str_2 == str_0) );
  VERIFY( !(str_3 == str_0) );
  VERIFY( str_4 == str_0 );
  VERIFY( str_0 == str_4 );

  VERIFY( str_0 != str_1 );
  VERIFY( str_0 != str_2 );
  VERIFY( str_0 != str_3 );
  VERIFY( str_1 != str_0 );
  VERIFY( str_2 != str_0 );
  VERIFY( str_3 != str_0 );
  VERIFY( !(str_0 != str_4) );
  VERIFY( !(str_4 != str_0) );
   
  VERIFY( str_0 > str_1 ); //true cuz r>m
  VERIFY( str_0 > str_2 );
  VERIFY( !(str_0 > str_3) );
  VERIFY( !(str_1 > str_0) ); //false cuz m<r
  VERIFY( !(str_2 > str_0) );
  VERIFY( str_3 > str_0 );
  VERIFY( !(str_0 > str_4) );
  VERIFY( !(str_4 > str_0) );

  VERIFY( !(str_0 < str_1) ); //false cuz r>m
  VERIFY( !(str_0 < str_2) );
  VERIFY( str_0 < str_3 );
  VERIFY( str_1 < str_0 ); //true cuz m<r
  VERIFY( str_2 < str_0 );
  VERIFY( !(str_3 < str_0) );
  VERIFY( !(str_0 < str_4) );
  VERIFY( !(str_4 < str_0) );

  VERIFY( str_0 >= str_1 ); //true cuz r>m
  VERIFY( str_0 >= str_2 );
  VERIFY( !(str_0 >= str_3) );
  VERIFY( !(str_1 >= str_0) );//false cuz m<r
  VERIFY( !(str_2 >= str_0) );
  VERIFY( str_3 >= str_0 );
  VERIFY( str_0 >= str_4 );
  VERIFY( str_4 >= str_0 );

  VERIFY( !(str_0 <= str_1) );//false cuz r>m
  VERIFY( !(str_0 <= str_2) );
  VERIFY( str_0 <= str_3 );
  VERIFY( str_1 <= str_0 );//true cuz m<r
  VERIFY( str_2 <= str_0 );
  VERIFY( !(str_3 <= str_0) );
  VERIFY( str_0 <= str_4 );
  VERIFY( str_4 <= str_0 );

  //comparisons between string object and string literal
  VERIFY( !(str_0 == "costa marbella") );
  VERIFY( !(str_0 == "cost") );
  VERIFY( !(str_0 == "costa ricans") );
  VERIFY( !("costa marbella" == str_0) );
  VERIFY( !("cost" == str_0) );
  VERIFY( !("costa ricans" == str_0) );
  VERIFY( "costa rica" == str_0 );
  VERIFY( str_0 == "costa rica" );

  VERIFY( str_0 != "costa marbella" );
  VERIFY( str_0 != "cost" );
  VERIFY( str_0 != "costa ricans" );
  VERIFY( "costa marbella" != str_0 );
  VERIFY( "cost" != str_0 );
  VERIFY( "costa ricans" != str_0 );
  VERIFY( !("costa rica" != str_0) );
  VERIFY( !(str_0 != "costa rica") );

  VERIFY( str_0 > "costa marbella" ); //true cuz r>m
  VERIFY( str_0 > "cost" );
  VERIFY( !(str_0 > "costa ricans") );
  VERIFY( !("costa marbella" > str_0) );//false cuz m<r
  VERIFY( !("cost" > str_0) );
  VERIFY( "costa ricans" > str_0 );
  VERIFY( !("costa rica" > str_0) );
  VERIFY( !(str_0 > "costa rica") );

  VERIFY( !(str_0 < "costa marbella") );//false cuz r>m
  VERIFY( !(str_0 < "cost") );
  VERIFY( str_0 < "costa ricans" );
  VERIFY( "costa marbella" < str_0 );//true cuz m<r
  VERIFY( "cost" < str_0 );
  VERIFY( !("costa ricans" < str_0) );
  VERIFY( !("costa rica" < str_0) );
  VERIFY( !(str_0 < "costa rica") );

  VERIFY( str_0 >= "costa marbella" );//true cuz r>m
  VERIFY( str_0 >= "cost" );
  VERIFY( !(str_0 >= "costa ricans") );
  VERIFY( !("costa marbella" >= str_0) );//false cuz m<r
  VERIFY( !("cost" >= str_0) );
  VERIFY( "costa ricans" >= str_0 );
  VERIFY( "costa rica" >= str_0 );
  VERIFY( str_0 >= "costa rica" );

  VERIFY( !(str_0 <= "costa marbella") );//false cuz r>m
  VERIFY( !(str_0 <= "cost") );
  VERIFY( str_0 <= "costa ricans" );
  VERIFY( "costa marbella" <= str_0 );//true cuz m<r
  VERIFY( "cost" <= str_0 );
  VERIFY( !("costa ricans" <= str_0) );
  VERIFY( "costa rica" <= str_0 );
  VERIFY( str_0 <= "costa rica" );

  // 21.3.7.1 operator+
/*
template<class charT, class traits, class Allocator>
  basic_string<charT,traits,Allocator>
    operator+(const basic_string<charT,traits,Allocator>& lhs,
              const basic_string<charT,traits,Allocator>& rhs);

template<class charT, class traits, class Allocator>
  basic_string<charT,traits,Allocator>
    operator+(const charT* lhs,
              const basic_string<charT,traits,Allocator>& rhs);

template<class charT, class traits, class Allocator>
  basic_string<charT,traits,Allocator>
    operator+(const basic_string<charT,traits,Allocator>& lhs,
              const charT* rhs);

template<class charT, class traits, class Allocator>
  basic_string<charT,traits,Allocator>
    operator+(charT lhs, const basic_string<charT,traits,Allocator>& rhs);

template<class charT, class traits, class Allocator>
  basic_string<charT,traits,Allocator>
    operator+(const basic_string<charT,traits,Allocator>& lhs, charT rhs);
*/

  str_4 = str_0 + "ns";
  VERIFY( str_4 == str_3 );

  const std::string str_5(" marbella");
  str_4 = "costa" + str_5;
  VERIFY( str_4 == str_1 );

  std::string str_6("ns");
  str_4 = str_0 + str_6;
  VERIFY( str_4 == str_3 );

  str_4 = str_0 + 'n';
  str_4 = str_4 + 's';
  VERIFY( str_4 == str_3 );

  str_4 = 'a' + str_6;
  str_4 = 'c' + str_4;
  str_4 = 'i' + str_4;
  str_4 = 'r' + str_4;
  str_4 = ' ' + str_4;
  str_4 = 'a' + str_4;
  str_4 = 't' + str_4;
  str_4 = 's' + str_4;
  str_4 = 'o' + str_4;
  str_4 = 'c' + str_4;
  VERIFY( str_4 == str_3 );
}

template<> template<> void tut::to::test<10>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::const_reference cref;
  typedef std::string::reference ref;
  csize_type csz01;

  const char str_lit01[] = "rockaway, pacifica";
  const std::string str01(str_lit01);
  std::string str02;

  // basic_string<charT, _Traits, _Alloc>
  //  substr(size_type pos = 0, size_type n = npos) const;
  csz01 = str01.size();
  str02 = str01.substr(0, 1);
  VERIFY( str02 == "r" );
  str02 = str01.substr(10);
  VERIFY( str02 == "pacifica" );

  try {
    str02 = str01.substr(csz01 + 1);
    VERIFY( false ); 
  }
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  try {
    str02 = str01.substr(csz01);
    VERIFY( str02.size() == 0 );
  }
  catch(std::out_of_range& fail) {
    VERIFY( false );
  }
  catch(...) {
    VERIFY( false );
  }

}

template<> template<> void tut::to::test<11>(void)
{
  std::string s("xarg");
  s.erase(1,1);
  tut::ensure_equals(s, "xrg");
  s.erase(1);
  tut::ensure_equals(s, "x");

  s = "xarg";
  s.erase();
  tut::ensure_equals(s, "");

  s = "xarg";
  s.erase(0,1);
  tut::ensure_equals(s, "arg");

  s.erase(s.length()-1);
  tut::ensure_equals(s, "ar");
}