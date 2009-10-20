/**\file*********************************************************************
 *                                                                     \brief
 *  Forward declarations [27.2 lib.iostream.forward]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_IOSFWD
#define NTL__STLX_IOSFWD

#include "cstdint.hxx" // for uint_least16_t

//#include "cwchar.hxx" // for mbstate_t

// 1 Default template arguments are described as appearing both in <iosfwd> and
//   in the synopsis of other headers but it is well-formed to include
//   both <iosfwd> and one or more of the other headers.

typedef int mbstate_t; // fwd

namespace std {

#ifndef NTL__CXX_CHARS_TYPES
  typedef uint_least16_t char16_t;
  typedef uint_least32_t char32_t;
#endif

/**\defgroup  lib_input_output ********* 27 Input/output library [input.output]
 *
 *    Components that C++ programs may use to perform input/output operations
 *@{
 */

template<class charT> struct char_traits;
template<>            struct char_traits<char>;
template<>            struct char_traits<char16_t>;
template<>            struct char_traits<char32_t>;
template<>            struct char_traits<wchar_t>;

template<class T>     class allocator;
class locale;

/// 2 The class template specialization basic_ios<charT,traits> serves as
///   a virtual base class for the class templates
///   basic_istream, basic_ostream, and class templates derived from them.
///   basic_iostream is a class template derived from both
///   basic_istream<charT,traits> and basic_ostream<charT,traits>.
template <class charT, class traits = char_traits<charT> >
class basic_ios;

/// 3 The class template specialization basic_streambuf<charT,traits> serves as
///   a base class for template classes basic_stringbuf and basic_filebuf.
template <class charT, class traits = char_traits<charT> >
class basic_streambuf;

/// 4 The class template specialization basic_istream<charT,traits> serves as
///   a base class for template classes basic_istringstream and basic_ifstream
template <class charT, class traits = char_traits<charT> >
class basic_istream;

/// 5 The class template specialization basic_ostream<charT,traits> serves as
///   a base class for template classes basic_ostringstream and basic_ofstream
template <class charT, class traits = char_traits<charT> >
class basic_ostream;

/// 6 The class template specialization basic_iostream<charT,traits> serves as
///   a base class for template classes basic_stringstream and basic_fstream.
template <class charT, class traits = char_traits<charT> >
class basic_iostream;

template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_stringbuf;

template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_istringstream;

template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_ostringstream;

template <class charT,
          class traits    = char_traits<charT>,
          class Allocator = allocator<charT> >
class basic_stringstream;

template <class charT, class traits = char_traits<charT> >
class basic_filebuf;

template <class charT, class traits = char_traits<charT> >
class basic_ifstream;

template <class charT, class traits = char_traits<charT> >
class basic_ofstream;

template <class charT, class traits = char_traits<charT> >
class basic_fstream;

template <class charT, class traits = char_traits<charT> >
class istreambuf_iterator;

template <class charT, class traits = char_traits<charT> >
class ostreambuf_iterator;

///\name  Typedefs define instances of class templates specialized for char or wchar_t types. (7)
typedef basic_ios<char>               ios;
typedef basic_ios<wchar_t>            wios;

typedef basic_streambuf<char>         streambuf;
typedef basic_streambuf<wchar_t>      wstreambuf;
typedef basic_istream<char>           istream;
typedef basic_istream<wchar_t>        wistream;
typedef basic_ostream<char>           ostream;
typedef basic_ostream<wchar_t>        wostream;
typedef basic_iostream<char>          iostream;
typedef basic_iostream<wchar_t>       wiostream;

typedef basic_stringbuf<char>         stringbuf;
typedef basic_stringbuf<wchar_t>      wstringbuf;
typedef basic_istringstream<char>     istringstream;
typedef basic_istringstream<wchar_t>  wistringstream;
typedef basic_ostringstream<char>     ostringstream;
typedef basic_ostringstream<wchar_t>  wostringstream;
typedef basic_stringstream<char>      stringstream;
typedef basic_stringstream<wchar_t>   wstringstream;

typedef basic_filebuf<char>           filebuf;
typedef basic_filebuf<wchar_t>        wfilebuf;
typedef basic_ifstream<char>          ifstream;
typedef basic_ifstream<wchar_t>       wifstream;
typedef basic_ofstream<char>          ofstream;
typedef basic_ofstream<wchar_t>       wofstream;
typedef basic_fstream<char>           fstream;
typedef basic_fstream<wchar_t>        wfstream;

/// 8 Specializations of the class template fpos are used for specifying
///   file position information.
template <class state> class fpos;

///\name  These types are used for positioning streams specialized on char, char16_t, char32_t and wchar_t respectively. (9)
typedef fpos<mbstate_t/*char_traits<char>::state_type*/>     streampos;
typedef fpos<mbstate_t/*char_traits<char16_t>::state_type*/> u16streampos;
typedef fpos<mbstate_t/*char_traits<char32_t>::state_type*/> u32streampos;
typedef fpos<mbstate_t/*char_traits<wchar_t>::state_type*/>  wstreampos;

///@}

/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_IOSFWD
