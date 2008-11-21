/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SSTREAM
#define NTL__STLX_SSTREAM

#include "ios.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library [input.output] **********
 *@{*/

/**\addtogroup  lib_string_streams ***** [27.7] String-based streams ********
 *@{*/
  template <class charT, class traits, class Allocator>
  class basic_stringbuf;

  typedef basic_stringbuf<char> stringbuf;
  typedef basic_stringbuf<wchar_t> wstringbuf;

  template <class charT, class traits, class Allocator>
  class basic_istringstream;

  typedef basic_istringstream<char> istringstream;
  typedef basic_istringstream<wchar_t> wistringstream;

  template <class charT, class traits, class Allocator>
  class basic_ostringstream;

  typedef basic_ostringstream<char> ostringstream;
  typedef basic_ostringstream<wchar_t> wostringstream;

  template <class charT, class traits, class Allocator>
  class basic_stringstream;

  typedef basic_stringstream<char> stringstream;
  typedef basic_stringstream<wchar_t> wstringstream;


  /// 27.7.1 Class template basic_stringbuf [stringbuf]

  template <class charT, class traits, class Allocator>
  class basic_stringbuf:
    public basic_streambuf<charT,traits>
  {
  public:
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    // 27.7.1.1 Constructors:
    explicit basic_stringbuf(ios_base::openmode which = ios_base::in | ios_base::out)
      :mode_(which)
    {}

    explicit basic_stringbuf(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::in | ios_base::out)
      :str_(str), mode_(which)
    {}

#ifdef NTL__CXX
    basic_stringbuf(basic_stringbuf&& rhs);
#else
    basic_stringbuf(basic_stringbuf& rhs);
#endif


    // 27.7.1.2 Assign and swap:
#ifdef NTL__CXX
    basic_stringbuf& operator=(basic_stringbuf&& rhs);
#else
    basic_stringbuf& operator=(basic_stringbuf& rhs);
#endif

#ifdef NTL__CXX
    void swap(basic_stringbuf&& rhs);
#else
    void swap(basic_stringbuf& rhs);
#endif

    // 27.7.1.3 Get and set:
    basic_string<charT,traits,Allocator> str() const { return str_; }

    void str(const basic_string<charT,traits,Allocator>& s) { str_ = s; }

  protected:
    // 27.7.1.4 Overridden virtual functions:
    virtual int_type underflow();
    virtual int_type pbackfail(int_type c = traits::eof());
    virtual int_type overflow (int_type c = traits::eof());
    virtual basic_streambuf<charT,traits>* setbuf(charT*, streamsize);
    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);
  private:
    string str_;
    ios_base::openmode mode_;
  };


  /// 27.7.1 Class template basic_stringstream [stringstream]
  template <class charT, class traits, class Allocator>
  class basic_stringstream:
    public basic_iostream<charT,traits>
  {
    typedef basic_iostream<charT, traits> base_type;
  public:
    // types:
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    // constructors/destructors
    explicit basic_stringstream(ios_base::openmode which = ios_base::out|ios_base::in)
      : sb_(which), base_type(&sb_)
    {}

    explicit basic_stringstream(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::out|ios_base::in)
      :sb_(str, which), base_type(&sb_)
    {}

#ifdef NTL__CXX
    basic_stringstream(basic_stringstream&& rhs);
#else
    basic_stringstream(basic_stringstream& rhs);
#endif

    // 27.7.5.1 Assign/swap:
#ifdef NTL__CXX
    basic_stringstream& operator=(basic_stringstream&& rhs);
#else
    basic_stringstream& operator=(basic_stringstream& rhs);
#endif

#ifdef NTL__CXX
    void swap(basic_stringstream&& rhs);
#else
    void swap(basic_stringstream& rhs);
#endif

    // Members:
    basic_stringbuf<charT,traits,Allocator>* rdbuf() const { return &sb_; }

    basic_string<charT,traits,Allocator> str() const { return sb_.str(); }

    void str(const basic_string<charT,traits,Allocator>& str) { sb_.str(str); }
  private:
     basic_stringbuf<charT, traits> sb_;
  };



  template <class charT, class traits, class Allocator>
  void swap(basic_stringbuf<charT, traits, Allocator>& x, basic_stringbuf<charT, traits, Allocator>& y)
  {
    x.swap(y);
  }

#ifdef NTL__CXX
  template <class charT, class traits, class Allocator>
  void swap(basic_stringbuf<charT, traits, Allocator>&& x,
    basic_stringbuf<charT, traits, Allocator>& y);

  template <class charT, class traits, class Allocator>
  void swap(basic_stringbuf<charT, traits, Allocator>& x,
    basic_stringbuf<charT, traits, Allocator>&& y);
#endif

  template <class charT, class traits, class Allocator>
  void swap(basic_stringstream<charT, traits, Allocator>& x, basic_stringstream<charT, traits, Allocator>& y)
  {
    x.swap(y);
  }

#ifdef NTL__CXX
  template <class charT, class traits, class Allocator>
  void swap(basic_stringstream<charT, traits, Allocator>&& x,
    basic_stringstream<charT, traits, Allocator>& y);
  template <class charT, class traits, class Allocator>
  void swap(basic_stringstream<charT, traits, Allocator>& x,
    basic_stringstream<charT, traits, Allocator>&& y);
#endif

  /**@} lib_string_streams */
  /**@} lib_input_output */
} // std

#endif // NTL__STLX_SSTREAM
