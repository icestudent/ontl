/**\file*********************************************************************
 *                                                                     \brief
 *  27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_SSTREAM
#define NTL__STLX_SSTREAM

#include "ios.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

/**\addtogroup  lib_string_streams ***** 27.7 String-based streams
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


  /**
   *	@brief 27.7.1 Class template basic_stringbuf [stringbuf]
   *  @details The class basic_stringbuf is derived from basic_streambuf to associate 
   *  possibly the input sequence and possibly the output sequence with a sequence of arbitrary characters. 
   *  The sequence can be initialized from, or made available as, an object of class basic_string.
   **/
  template <class charT, class traits, class Allocator>
  class basic_stringbuf:
    public basic_streambuf<charT,traits>
  {
    typedef typename add_rvalue_reference<basic_stringbuf>::type rvalue;
  public:
    ///\name Types
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    ///\name 27.7.1.1 Constructors:
    explicit basic_stringbuf(ios_base::openmode which = ios_base::in | ios_base::out)
      :mode_(which)
    {}

    explicit basic_stringbuf(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::in | ios_base::out)
      :str_(str), mode_(which)
    {
      set_ptrs();
    }

  #ifdef NTL__CXX_RV
    basic_stringbuf(basic_stringbuf&& rhs)
      :mode_(ios_base::in|ios_base::out)
    {
      swap(rhs);
    }
  #endif

    ///\name 27.7.1.2 Assign and swap:
  #ifdef NTL__CXX_RV
    basic_stringbuf& operator=(basic_stringbuf&& rhs)
    {
      swap(rhs); return *this;
    }
  #endif

    void swap(basic_stringbuf& rhs)
    {
      basic_streambuf.swap(rhs);
      str_.swap(rhs.str_); // a bit quickly
      std::swap(mode_, rhs.mode_);
    }

    ///\name 27.7.1.3 Get and set:
    basic_string<charT,traits,Allocator> str() const 
    { 
      const char_type * const beg = mode & ios_base::out ? this->pbase()
        : mode & ios_base::in ? this->eback() : 0;
      const char_type * const end = mode & ios_base::out ? this->pptr()
        : mode & ios_base::in ? this->egptr() : 0;
      return basic_string<charT,traits,Allocator>(beg, end);
    }

    void str(const basic_string<charT,traits,Allocator>& s) 
    { 
      str_ = s;
      set_ptrs();
    }

  protected:
    ///\name 27.7.1.4 Overridden virtual functions:
    virtual int_type underflow()
    {
      return this->gptr() < this->egptr()
        ? traits::to_int_type(*this->gptr()) : traits::eof();
    }
    virtual int_type pbackfail(int_type c = traits::eof())
    {
      // backup sequence
      if ( this->eback() < this->gptr() )
      {
        if ( !traits::eq_int_type(c, traits::eof()) )
        {
          if ( !traits::eq(traits::to_char_type(c), this->gptr()[-1])
            && mode_ & ios_base::out )
            this->gptr()[-1] = traits::to_char_type(c);
        }
        this->gbump(-1);
        return traits::not_eof(c);
      }
      return traits::eof();
    }

    virtual int_type overflow (int_type c = traits::eof());
    virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
    virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);

    ///\}

  private:
    /// set stream poiners according to mode. \see str.
    void set_ptrs()
    {
      if ( mode & ios_base::out )
      {
        if(s.empty())
          s.resize(16); // 16 characters by default
        this->setp(s.begin(), s.end());
      }
      if ( mode & ios_base::in )
        this->setg(s.begin(), s.begin(), s.end());      
    }
  private:
    string str_;
    ios_base::openmode mode_;
  };


  /**
   *	@brief 27.7.2 Class template basic_istringstream [istringstream]
   *  @details The class basic_istringstream<charT, traits, Allocator> supports reading objects of class 
   *  basic_string<charT, traits, Allocator>. It uses a basic_stringbuf<charT, traits, Allocator> object
   *  to control the associated storage.
   **/
  template <class charT, class traits, class Allocator>
  class basic_istringstream:
    public basic_istream<charT,traits>
  {
    typedef typename add_rvalue_reference<basic_istringstream>::type rvalue;
  public:
    ///\name Types
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    ///\name 27.7.2.1 Constructors:
    explicit basic_istringstream(ios_base::openmode which = ios_base::in)
      :basic_istream<charT,traits>(&sb), sb(which | ios_base::in)
    {}

    explicit basic_istringstream(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::in)
      :basic_istream<charT,traits>(&sb), sb(which | ios_base::in)
    {}

  #ifdef NTL__CXX_RV
    basic_istringstream(basic_istringstream&& rhs)
      :basic_istream<charT,traits>(&sb), sb(ios_base::in)
    {
      swap(rhs);
      basic_istream::set_rdbuf(&sb);
    }
  #endif

    ///\name 27.7.2.2 Assign and swap:
  #ifdef NTL__CXX_RV
    basic_istringstream& operator=(basic_istringstream&& rhs)
    {
      swap(rhs); return *this;
    }
  #endif
    
    void swap(basic_istringstream& rhs)
    {
      basic_istream::swap(rhs);
      sb.swap(rhs.sb);
    }

    ///\name 27.7.2.3 Members:
    basic_stringbuf<charT,traits,Allocator>* rdbuf() const
    {
      return const_cast<basic_stringbuf<charT,traits,Allocator>*>(&sb);
    }

    basic_string<charT,traits,Allocator> str() const { return sb.str(); }
    void str(const basic_string<charT,traits,Allocator>& s) { sb.str(s) }

    ///\}
  private:
    basic_stringbuf<charT,traits,Allocator> sb;
  };



  /**
   *	@brief 27.7.3 Class basic_ostringstream [ostringstream]
   *  @details The class basic_ostringstream<charT, traits, Allocator> supports writing objects of class 
   *  basic_string<charT, traits, Allocator>. It uses a basic_stringbuf object to control the associated storage.
   **/
  template <class charT, class traits, class Allocator>
  class basic_ostringstream:
    public basic_ostream<charT,traits>
  {
    typedef typename add_rvalue_reference<basic_ostringstream>::type rvalue;
  public:
    ///\name Types
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    ///\name 27.7.3.1 Constructors/destructor:
    explicit basic_ostringstream(ios_base::openmode which = ios_base::out)
      :basic_ostream<charT,traits>(&sb), sb(which | ios_base::out)
    {}

    explicit basic_ostringstream(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::out)
      : basic_ostream<charT,traits>(&sb), sb(str, which | ios_base::out)
    {}

  #ifdef NTL__CXX_RV
    basic_ostringstream(basic_ostringstream&& rhs)
      :basic_ostream<charT,traits>(&sb), sb(ios_base::out)
    {
      swap(rhs);
      basic_ostream::set_rdbuf(&sb);
    }
  #endif

    ///\name 27.7.3.2 Assign/swap:
  #ifdef NTL__CXX_RV
    basic_ostringstream& operator=(basic_ostringstream&& rhs)
    {
      swap(rhs); return *this;
    }
  #endif
    void swap(basic_ostringstream& rhs)
    {
      basic_ostream::swap(rhs);
      sb.swap(rhs.sb);
    }

    ///\name 27.7.3.3 Members:
    basic_stringbuf<charT,traits,Allocator>* rdbuf() const
    {
      return const_cast<basic_stringbuf<charT,traits,Allocator>*>(&sb);
    }
    basic_string<charT,traits,Allocator> str() const { return sb.str(); }
    void str(const basic_string<charT,traits,Allocator>& s) { sb.str(s); }
    ///\}
  private:
    basic_stringbuf<charT,traits,Allocator> sb;
  };



  /**
   *	@brief 27.7.1 Class template basic_stringstream [stringstream]
   *  @details The class template basic_stringstream<charT, traits> supports reading and writing from objects of
   *  class basic_string<charT, traits, Allocator>. It uses a basic_stringbuf<charT, traits, Allocator>
   *  object to control the associated sequence.
   **/
  template <class charT, class traits, class Allocator>
  class basic_stringstream:
    public basic_iostream<charT,traits>
  {
    typedef typename add_rvalue_reference<basic_stringstream>::type rvalue;
    typedef basic_iostream<charT, traits> base_type;
  public:
    ///\name Types
    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;
    typedef Allocator allocator_type;

    ///\name constructors/destructors
    explicit basic_stringstream(ios_base::openmode which = ios_base::out|ios_base::in)
      : sb_(which), base_type(&sb_)
    {}

    explicit basic_stringstream(const basic_string<charT,traits,Allocator>& str, ios_base::openmode which = ios_base::out|ios_base::in)
      :sb_(str, which), base_type(&sb_)
    {}

  #ifdef NTL__CXX_RV
    basic_stringstream(basic_stringstream&& rhs)
      :sb_(ios_base::out|ios_base::in), base_type(&sb_)
    {
      swap(rhs);
      basic_iostream::set_rdbuf(&sb_);
    }
  #endif

    ///\name 27.7.5.1 Assign/swap:
  #ifdef NTL__CXX_RV
    basic_stringstream& operator=(basic_stringstream&& rhs);
  #endif

    void swap(basic_stringstream& rhs);

    ///\name Members:
    basic_stringbuf<charT,traits,Allocator>* rdbuf() const { return &sb_; }
    basic_string<charT,traits,Allocator> str() const { return sb_.str(); }
    void str(const basic_string<charT,traits,Allocator>& str) { sb_.str(str); }
    ///\}
  private:
     basic_stringbuf<charT, traits> sb_;
  };


  ///\name Swap functions
  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringbuf<charT, traits, Allocator>& x, basic_stringbuf<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_istringstream<charT, traits, Allocator>& x, basic_istringstream<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_ostringstream<charT, traits, Allocator>& x, basic_ostringstream<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringstream<charT, traits, Allocator>& x, basic_stringstream<charT, traits, Allocator>& y) { x.swap(y); }

#ifdef NTL__CXX_RV && 0 // disabled in N2857+
  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringbuf<charT, traits, Allocator>&& x, basic_stringbuf<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringbuf<charT, traits, Allocator>& x, basic_stringbuf<charT, traits, Allocator>&& y) { x.swap(y); }

  template <class charT, class traits, class Allocator>
  inline void swap(basic_istringstream<charT, traits, Allocator>&& x, basic_istringstream<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_istringstream<charT, traits, Allocator>& x, basic_istringstream<charT, traits, Allocator>&& y) { x.swap(y); }

  template <class charT, class traits, class Allocator>
  inline void swap(basic_ostringstream<charT, traits, Allocator>&& x, basic_ostringstream<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_ostringstream<charT, traits, Allocator>& x, basic_ostringstream<charT, traits, Allocator>&& y) { x.swap(y); }

  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringstream<charT, traits, Allocator>&& x, basic_stringstream<charT, traits, Allocator>& y) { x.swap(y); }
  template <class charT, class traits, class Allocator>
  inline void swap(basic_stringstream<charT, traits, Allocator>& x, basic_stringstream<charT, traits, Allocator>&& y) { x.swap(y); }
#endif
  ///\}
  /**@} lib_string_streams */
  /**@} lib_input_output */
} // std

#endif // NTL__STLX_SSTREAM
