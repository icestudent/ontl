// 1999-06-03 bkoz

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

// 21.3.5.4 basic_string::insert
#include <ntl-tests-common.hxx>

#include <string>
#include <stdexcept>

STLX_DEFAULT_TESTGROUP_NAME("std::string#insert");

#include <istream>
#include <fstream>

template<> template<> void tut::to::test<01>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::iterator citerator;
  csize_type csz01, csz02;

  const std::string str01("rodeo beach, marin");
  const std::string str02("baker beach, san francisco");
  std::string str03;

  // string& insert(size_type p1, const string& str, size_type p2, size_type n)
  // requires:
  //   1) p1 <= size()
  //   2) p2 <= str.size()
  //   3) rlen = min(n, str.size() - p2)
  // throws:
  //   1) out_of_range if p1 > size() || p2 > str.size()
  //   2) length_error if size() >= npos - rlen
  // effects:
  // replaces *this with new string of length size() + rlen such that
  // nstr[0]  to nstr[p1] == thisstr[0] to thisstr[p1]
  // nstr[p1 + 1] to nstr[p1 + rlen] == str[p2] to str[p2 + rlen]
  // nstr[p1 + 1 + rlen] to nstr[...] == thisstr[p1 + 1] to thisstr[...]  
  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  try {
    str03.insert(csz01 + 1, str02, 0, 5);
    VERIFY( false );
  }		 
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  try {
    str03.insert(0, str02, csz02 + 1, 5);
    VERIFY( false );
  }		 
  catch(std::out_of_range& fail) {
    VERIFY( true );
  }
  catch(...) {
    VERIFY( false );
  }

  csz01 = str01.max_size();
  try {
    std::string str04(csz01, 'b'); 
    str03 = str04; 
    csz02 = str02.size();
    try {
      str03.insert(0, str02, 0, 5);
      //VERIFY( false );
    }		 
    catch(std::length_error& fail) {
      VERIFY( true );
    }
    catch(...) {
      VERIFY( false );
    }
  }
  catch(std::bad_alloc& failure){
    VERIFY( true ); 
  }
  catch(std::exception& failure){
    VERIFY( false );
  }

  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  str03.insert(13, str02, 0, 12); 
  VERIFY( str03 == "rodeo beach, baker beach,marin" );

  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  str03.insert(0, str02, 0, 12); 
  VERIFY( str03 == "baker beach,rodeo beach, marin" );

  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  str03.insert(csz01, str02, 0, csz02); 
  VERIFY( str03 == "rodeo beach, marinbaker beach, san francisco" );

  // string& insert(size_type __p, const string& string);
  // insert(p1, str, 0, npos)
  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  str03.insert(csz01, str02); 
  VERIFY( str03 == "rodeo beach, marinbaker beach, san francisco" );

  str03 = str01; 
  csz01 = str03.size();
  csz02 = str02.size();
  str03.insert(0, str02); 
  VERIFY( str03 == "baker beach, san franciscorodeo beach, marin" );

  // string& insert(size_type __p, const char* s, size_type n);
  // insert(p1, string(s,n))
  str03 = str02; 
  csz01 = str03.size();
  str03.insert(0, "-break at the bridge", 20); 
  VERIFY( str03 == "-break at the bridgebaker beach, san francisco" );

  // string& insert(size_type __p, const char* s);
  // insert(p1, string(s))
  str03 = str02; 
  str03.insert(0, "-break at the bridge"); 
  VERIFY( str03 == "-break at the bridgebaker beach, san francisco" );

  // string& insert(size_type __p, size_type n, char c)
  // insert(p1, string(n,c))
  str03 = str02; 
  csz01 = str03.size();
  str03.insert(csz01, 5, 'z'); 
  VERIFY( str03 == "baker beach, san franciscozzzzz" );

  // iterator insert(iterator p, char c)
  // inserts a copy of c before the character referred to by p
  str03 = str02; 
  citerator cit01 = str03.begin();
  str03.insert(cit01, 'u'); 
  VERIFY( str03 == "ubaker beach, san francisco" );

  // iterator insert(iterator p, size_type n,  char c)
  // inserts n copies of c before the character referred to by p
  str03 = str02; 
  cit01 = str03.begin();
  str03.insert(cit01, 5, 'u'); 
  VERIFY( str03 == "uuuuubaker beach, san francisco" );

  // template<inputit>
  //   void 
  //   insert(iterator p, inputit first, inputit, last)
  // ISO-14882: defect #7 part 1 clarifies this member function to be:
  // insert(p - begin(), string(first,last))
  str03 = str02; 
  csz01 = str03.size();
  str03.insert(str03.begin(), str01.begin(), str01.end()); 
  VERIFY( str03 == "rodeo beach, marinbaker beach, san francisco" );

  str03 = str02; 
  csz01 = str03.size();
  str03.insert(str03.end(), str01.begin(), str01.end()); 
  VERIFY( str03 == "baker beach, san franciscorodeo beach, marin" );
}

// More
//   string& insert(size_type __p, const char* s, size_type n);
//   string& insert(size_type __p, const char* s);
// but now s points inside the _Rep
template<> template<> void tut::to::test<02>(void)
{
  bool test __attribute__((unused)) = true;

  std::string str01 ("0011");
  str01.insert(0, str01.begin()+str01.size()-2,2);
  str01.c_str();
  assert(str01 == "110011");
  const char* title = "Everything was beautiful, and nothing hurt";
  // Increasing size: str01 is reallocated every time.
  str01 = title;
  str01.insert(0, str01.c_str() + str01.size() - 4, 4);
  VERIFY( str01 == "hurtEverything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(0, str01.c_str(), 5);
  VERIFY( str01 == "EveryEverything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(10, str01.c_str() + 4, 6);
  VERIFY( str01 == "Everythingything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(15, str01.c_str(), 10);
  VERIFY( str01 == "Everything was Everythingbeautiful, and nothing hurt" );
  str01 = title;
  str01.insert(15, str01.c_str() + 11, 13);
  VERIFY( str01 == "Everything was was beautifulbeautiful, and nothing hurt" );
  str01 = title;
  str01.insert(0, str01.c_str());
  VERIFY( str01 == "Everything was beautiful, and nothing hurt"
    "Everything was beautiful, and nothing hurt");
  // Again: no reallocations.
  str01 = title;
  str01.insert(0, str01.c_str() + str01.size() - 4, 4);
  VERIFY( str01 == "hurtEverything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(0, str01.c_str(), 5);
  VERIFY( str01 == "EveryEverything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(10, str01.c_str() + 4, 6);
  VERIFY( str01 == "Everythingything was beautiful, and nothing hurt" );
  str01 = title;
  str01.insert(15, str01.c_str(), 10);
  VERIFY( str01 == "Everything was Everythingbeautiful, and nothing hurt" );
  str01 = title;
  str01.insert(15, str01.c_str() + 11, 13);
  VERIFY( str01 == "Everything was was beautifulbeautiful, and nothing hurt" );
  str01 = title;
  str01.insert(0, str01.c_str());
  VERIFY( str01 == "Everything was beautiful, and nothing hurt"
    "Everything was beautiful, and nothing hurt");

}

template<> template<> void tut::to::test<03>(void)
{
  bool test __attribute__((unused)) = true;
  typedef std::string::size_type csize_type;
  typedef std::string::const_reference cref;
  typedef std::string::reference ref;

  const std::string str01("sailing grand traverse bay\n"
    "\t\t\t    from Elk Rapids to the point reminds me of miles");
  const std::string str02("sailing");
  const std::string str03("grand");
  const std::string str04("traverse");
  const std::string str05;
  std::string str10;

  // istream& operator>>(istream&, string&)
  std::istringstream istrs01(str01);
  istrs01 >> str10;
  VERIFY( str10 == str02 );
  try 
  {
    std::istringstream::int_type i01 = istrs01.peek(); //a-boo
    VERIFY( std::istringstream::traits_type::to_char_type(i01) == ' ' );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  istrs01.clear();
  istrs01 >> str10; 
  VERIFY( str10 == str03 ); 
  istrs01.clear();
  istrs01 >> str10; 
  VERIFY( str10 == str04 ); // sentry picks out the white spaces. . 

  std::istringstream istrs02(str05); // empty
  istrs02 >> str10;
  VERIFY( str10 == str04 );

  // istream& getline(istream&, string&, char)
  // istream& getline(istream&, string&)
  try 
  {
    istrs01.clear();
    getline(istrs01, str10);
    VERIFY( !istrs01.fail() );
    VERIFY( !istrs01.eof() );
    VERIFY( istrs01.good() );
    VERIFY( str10 == " bay" );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  try 
  {
    istrs01.clear();
    //char buf[128];
    //istrs01.getline(buf, _countof(buf), '\t');
    getline(istrs01, str10,'\t');
    VERIFY( !istrs01.fail() );
    VERIFY( !istrs01.eof() );
    VERIFY( istrs01.good() );
    VERIFY( str10 == str05 );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  try 
  {
    istrs01.clear();
    getline(istrs01, str10,'\t');
    VERIFY( !istrs01.fail() );
    VERIFY( !istrs01.eof() );
    VERIFY( istrs01.good() );
    VERIFY( str10 == str05 );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  try 
  {
    istrs01.clear();
    getline(istrs01, str10, '.');
    VERIFY( !istrs01.fail() );
    VERIFY( istrs01.eof() );
    VERIFY( !istrs01.good() );
    VERIFY( str10 == "\t    from Elk Rapids to the point reminds me of miles" );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  try 
  {
    getline(istrs02, str10);
    VERIFY( istrs02.fail() );
    VERIFY( istrs02.eof() );
    VERIFY( str10 =="\t    from Elk Rapids to the point reminds me of miles" );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false ); // shouldn't throw
  }

  // ostream& operator<<(ostream&, const basic_string&)
  std::ostringstream ostrs01;
  try 
  {
    ostrs01 << str01;
    VERIFY( ostrs01.str() == str01 );
  }
  catch(std::exception& fail) 
  {
    VERIFY( false );
  }

  std::string hello_world;
  std::cout << hello_world;

}


// testing basic_stringbuf::xsputn via stress testing with large strings
// based on a bug report libstdc++ 9
void test04(std::size_t size)
{
  bool test __attribute__((unused)) = true;
  std::string str(size, 's');
  std::size_t expected_size = 2 * (size + 1);
  std::ostringstream oss(str);

  // sanity checks
  VERIFY( str.size() == size );
  VERIFY( oss.good() );

  // stress test
  oss << str << std::endl;
  if (!oss.good()) 
    test = false;

  oss << str << std::endl;
  if (!oss.good()) 
    test = false;

  VERIFY( str.size() == size );
  VERIFY( oss.good() );
  std::string str_tmp = oss.str();
  VERIFY( str_tmp.size() == expected_size );
}

template<> template<> void tut::to::test<04>()
{ 
  test04(1); // expected_size == 4
  test04(1000); // expected_size == 2002
  test04(10000); // expected_size == 20002

}

// testing basic_filebuf::xsputn via stress testing with large strings
// based on a bug report libstdc++ 9
// mode == out
void test05(std::size_t size)
{
  bool test __attribute__((unused)) = true;
  const char filename[] = "inserters_extractors-1.txt";
  const char fillc = 'f';
  std::ofstream ofs(filename, std::ofstream::out|std::ofstream::trunc);
  std::string str(size, fillc);

  // sanity checks
  VERIFY( str.size() == size );
  VERIFY( ofs.good() );

  // stress test
  ofs << str << std::endl;
  if (!ofs.good()) 
    test = false;

  ofs << str << std::endl;
  if (!ofs.good()) 
    test = false;

  VERIFY( str.size() == size );
  VERIFY( ofs.good() );

  ofs.close();

  // sanity check on the written file
  std::ifstream ifs(filename);
  std::size_t count = 0;
  char c;
  while (count <= (2 * size) + 4)
  {
    ifs >> c;
    if (ifs.good() && c == fillc)
    {
      ++count;
      c = '0';
    }
    else 
      break;
  }

  VERIFY( count == 2 * size );
}

template<> template<> void tut::to::test<05>()
{ 
  test05(1); 
  test05(1000); 
  test05(10000);

}

// istringstream/stringbuf extractor properly size buffer based on
// actual, not allocated contents (string.size() vs. string.capacity()).
// http://gcc.gnu.org/ml/libstdc++/1999-q4/msg00049.html
template<> template<> void tut::to::test<06>(void)
{
  bool test __attribute__((unused)) = true;

  typedef std::string::size_type size_type;
  std::string str01("@silent");
  size_type i01 = str01.size();
  size_type i02 = str01.capacity();
  str01.erase(0, 1);
  size_type i03 = str01.size();
  size_type i04 = str01.capacity();
  VERIFY( i01 - 1 == i03 );
  VERIFY( i02 >= i04 );

  std::istringstream is(str01);
  std::string str02;
  is >> str02 >> std::ws;
  size_type i05 = str02.size();
  size_type i06 = str02.capacity();
  VERIFY( i05 == i03 );
  VERIFY( i06 <= i04 );
}

// http://gcc.gnu.org/ml/libstdc++/2000-q1/msg00085.html
// istream::operator>>(string)
// sets failbit
// NB: this is a defect in the standard.
template<> template<> void tut::to::test<07>(void)
{
  bool test __attribute__((unused)) = true;
  const std::string name("z6.cc");
  std::istringstream iss (name);
  int i = 0;
  std::string s;
  while (iss >> s) 
    ++i;

  VERIFY( i < 3 );
  VERIFY( /*static_cast<bool>*/!!(iss.rdstate() & std::ios_base::failbit) );
}

// libstdc++/1019
template<> template<> void tut::to::test<8>()
{
  using namespace std;

  bool test __attribute__((unused)) = true;
  istringstream istrm("enero:2001");
  int 		year;
  char 		sep;
  string 	month;

  istrm >> setw(5) >> month >> sep >> year;
  VERIFY( month.size() == 5 );
  VERIFY( sep == ':' );
  VERIFY( year == 2001 );
}

// libstdc++/2830
template<> template<> void tut::to::test<9>()
{
  bool test __attribute__((unused)) = true;
  std::string blanks( 3, '\0');
  std::string foo = "peace";
  foo += blanks;
  foo += "& love";

  std::ostringstream oss1;
  oss1 << foo;
  VERIFY( oss1.str() == foo );

  std::ostringstream oss2;
  oss2.width(20);
  oss2 << foo;
  VERIFY( oss2.str() != foo );
  VERIFY( oss2.str().size() == 20 );
}

using namespace std;

string prepare(string::size_type len, unsigned nchunks, char delim)
{
  string ret;
  for (unsigned i = 0; i < nchunks; ++i)
  {
    for (string::size_type j = 0; j < len; ++j)
      ret.push_back(static_cast<char>('a' + rand() % 26));
    len *= 2;
    ret.push_back(delim);
  }
  return ret;
}

void check(istream& stream, const string& str, unsigned nchunks, char delim)
{
  bool test __attribute__((unused)) = true;

  string chunk;
  string::size_type index = 0, index_new = 0;
  unsigned n = 0;

  while (getline(stream, chunk, delim))
  {
    index_new = str.find(delim, index);
    bool ok1 = str.compare(index, index_new - index, chunk) == 0;
    string tmp(str, index, index_new-index);
    bool ok2 = tmp.compare(chunk) == 0;
    VERIFY( !str.compare(index, index_new - index, chunk) );
    index = index_new + 1;
    ++n;
  }
  VERIFY( stream.eof() );
  VERIFY( n == nchunks );
}

// istream& getline(istream&, string&, char)
template<> template<> void tut::to::test<10>()
{
  const char filename[] = "inserters_extractors-2.txt";

  const char delim = '|';
  const unsigned nchunks = 10;
  const string data = prepare(777, nchunks, delim);

  ofstream ofstrm;
  ofstrm.open(filename, ofstream::out|ofstream::trunc);
  ofstrm.write(data.data(), data.size());
  ofstrm.close();

  ifstream ifstrm;
  ifstrm.open(filename);
  check(ifstrm, data, nchunks, delim);
  ifstrm.close();
}

string prepare(string::size_type len, unsigned nchunks)
{
  string ret;
  for (unsigned i = 0; i < nchunks; ++i)
  {
    for (string::size_type j = 0; j < len; ++j)
      ret.push_back(static_cast<char>('a' + rand() % 26));
    len *= 2;
    ret.push_back(' ');
  }
  return ret;
}

void check(istream& stream, const string& str, unsigned nchunks)
{
  bool test __attribute__((unused)) = true;

  string chunk;
  string::size_type index = 0, index_new = 0;
  unsigned n = 0;

  while (stream >> chunk)
  {
    index_new = str.find(' ', index);
    VERIFY( !str.compare(index, index_new - index, chunk) );
    index = index_new + 1;
    ++n;
  }
  VERIFY( stream.eof() );
  VERIFY( n == nchunks );  
}

// istream& operator>>(istream&, string&)
template<> template<> void tut::to::test<11>()
{
  const char filename[] = "inserters_extractors-3.txt";

  const unsigned nchunks = 10;
  const string data = prepare(666, nchunks);

  ofstream ofstrm;
  ofstrm.open(filename, ofstream::out|ofstream::trunc);
  ofstrm.write(data.data(), data.size());
  ofstrm.close();

  ifstream ifstrm;
  ifstrm.open(filename);
  check(ifstrm, data, nchunks);
  ifstrm.close();
}

#ifndef WIDTH
#define WIDTH 20000000
#endif

// libstdc++/28277
template<> template<> void tut::to::test<12>()
{
  using namespace std;
  bool test __attribute__((unused)) = true;

  ostringstream oss_01;
  const string str_01(50, 'a');

  oss_01.width(WIDTH);
  const streamsize width = oss_01.width();

  oss_01 << str_01;

  VERIFY( oss_01.good() );
  VERIFY( oss_01.str().size() == string::size_type(width) );
}

