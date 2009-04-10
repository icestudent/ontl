/**\file*********************************************************************
 *                                                                     \brief
 *  N2691 27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_OSTREAM
#define NTL__STLX_OSTREAM

#include "ios.hxx"
#include "iterator.hxx"
#include "locale.hxx"
#include "string.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

/**\addtogroup  lib_iostream_format **** 27.6 Formatting and manipulators
 *@{*/

/**\addtogroup  lib_output_streams ***** 27.6.2 Output streams
 *@{*/

/// 27.6.2.1 Class template basic_ostream [ostream].
/// 1 The class basic_ostream defines a number of member function signatures
///   that assist in formatting and writing output to output sequences
///   controlled by a stream buffer.
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ostream
: virtual public basic_ios<charT, traits>
{
    typedef basic_ios<charT, traits>  this_base;

  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name Types (inherited from basic_ios (27.4.4)):
    using this_base::char_type;
    using this_base::int_type;
    using this_base::pos_type;
    using this_base::off_type;
    using this_base::traits_type;

    ///\name  27.6.2.2 basic_ostream constructors [ostream.cons]
    /// 1 Effects: Constructs an object of class basic_ostream, assigning
    ///   initial values to the base class by calling
    ///   basic_ios<charT,traits>::init(sb) (27.4.4.1).
    /// 2 Postcondition: rdbuf() == sb.
    explicit basic_ostream(basic_streambuf<char_type, traits>* sb)
    {
      basic_ios<charT, traits>::init(sb);
    }

#ifdef NTL__CXX
    /// 5 Effects: Move constructs from the rvalue rhs. This is accomplished
    ///   by default constructing the base class and calling
    ///   basic_ios<charT, traits>::move(rhs) to initialize the base class.
    basic_ostream(basic_ostream&& rhs)
    {
      basic_ios::move(rhs);
    }
#endif

    ///\name  basic_ostream destructor
    /// 3 Effects: Destroys an object of class basic_ostream.
    /// 4 Remarks: Does not perform any operations on rdbuf().
    virtual ~basic_ostream() {}
    ///@}

#ifdef NTL__CXX_RV
    ///\name 27.6.2.3 Class basic_ostream assign and swap [ostream.assign]
    basic_ostream& operator=(basic_ostream&& rhs)
    {
      swap(rhs);
      return *this;
    }
    void swap(basic_ostream&& x)
#else
    void swap(basic_ostream& x)
#endif
    {
      if(this != &x)
        basic_ios::swap(x);
    }

    ///\name 27.6.2.4 Class basic_ostream::sentry [ostream::sentry].
    /// 1 The class sentry defines a class that is responsible for doing
    ///   exception safe prefix and suffix operations.
    class sentry
    {
      public:

        /// 2 If os.good() is nonzero, prepares for formatted or unformatted output.
        ///   If os.tie() is not a null pointer, calls os.tie()->flush().
        ///   (The call os.tie()->flush() does not necessarily occur if
        ///    the function can determine that no synchronization is necessary)
        /// 3 If, after any preparation is completed, os.good() is true,
        ///   ok == true otherwise, ok == false. During preparation,
        ///   the constructor may call setstate(failbit)
        ///   (which may throw ios_base::failure (27.4.4.3))
        __forceinline
        explicit sentry(basic_ostream<charT, traits>& os) : os(os), ok(false)
        {
          if ( os.good() )
          {
            if ( os.tie() ) os.tie()->flush();
            ok = true;
          }
        }

        __forceinline
        ~sentry()
        {
          //if ( (os.flags() & ios_base::unitbuf) && !uncaught_exception() )
          if ( (os.flags() & ios_base::unitbuf) && !os.rdstate() & ios_base::badbit )
            os.flush();
        }

        operator bool() const { return ok; }

      private:

        basic_ostream<charT, traits>& os;
        bool ok;


        sentry(const sentry&); // not defined
        sentry& operator=(const sentry&); // not defined
    };

    ///\name  27.6.2.5 basic_ostream seek members [ostream.seeks]

    /// 1 Returns: if fail() != false, returns pos_type(-1) to indicate failure.
    ///   Otherwise, returns rdbuf()->pubseekoff(0, cur, out).
    pos_type tellp()
    {
      return this->fail() ? this_base::pos_type(-1) : this->rdbuf()->pubseekoff(0, cur, out);
    }

    /// 2 Effects: If fail() != true, executes rdbuf()->pubseekpos(pos, ios_base::out).
    ///   In case of failure, the function calls setstate(failbit)
    ///   (which may throw ios_base::failure).
    /// 3 Returns: *this.
    basic_ostream<charT, traits>& seekp(pos_type pos)
    {
      if ( ! this->fail() && this->rdbuf()->pubseekpos(pos, ios_base::out) == -1 )
        this->setstate(ios_base::failbit);
      return *this;
    }

    /// 4 Effects: If fail() != true, executes rdbuf()->pubseekoff(off, dir, ios_base::out)
    /// 5 Returns: *this.
    basic_ostream<charT, traits>& seekp(off_type off, ios_base::seekdir dir)
    {
      if ( ! this->fail() )
        this->rdbuf()->pubseekoff(off, dir, ios_base::out);
      return *this;
    }

    /// 27.6.2.6 Formatted output functions [ostream.formatted].
    /// 27.6.2.6.1 Common requirements [ostream.formatted.reqmts]
    /// 1 Each formatted output function begins execution by constructing an object
    ///   of class sentry. If this object returns true when converted to a value
    ///   of type bool, the function endeavors to generate the requested output.
    ///   If the generation fails, then the formatted output function does
    ///   setstate(ios_base::failbit), which might throw an exception.
    ///   If an exception is thrown during output, then ios::badbit is turned on
    ///   (without causing an ios::failure to be thrown) in *this’s error state.
    ///   If (exceptions()&badbit) != 0 then the exception is rethrown.
    ///   Whether or not an exception is thrown, the sentry object is destroyed
    ///   before leaving the formatted output function. If no exception is thrown,
    ///   the result of the formattted output function is *this.
    /// 2 The descriptions of the individual formatted output operations describe
    ///   how they perform output and do not mention the sentry object.

    ///\name  27.6.2.6.2 Arithmetic Inserters [ostream.inserters.arithmetic]
    /// 1 Effects: The classes num_get<> and num_put<> handle locale-dependent
    ///   numeric formatting and parsing. These inserter functions use the imbued
    ///   locale value to perform numeric formatting.

  protected:
    template<typename ValueT>
    basic_ostream<charT, traits>& put_impl(const ValueT& value/*, typename enable_if<is_integral<ValueT>::value>::type* =0*/)
    {
      sentry good( *this );
      ios_base::iostate state = ios_base::badbit;
      if ( good )
        __ntl_try
      {
        typedef ostreambuf_iterator<charT, traits> iterator;
        typedef num_put<charT, iterator> facet_t;
        if ( !use_facet<facet_t>(this->getloc()).put(iterator(*this), *this, this->fill(), value).failed() )
          state = ios_base::goodbit;
      }
      __ntl_catch(...)
      {
        ///\todo shall no throw
        //this->setstate(ios_base::badbit);
      }
      this->setstate(state);
      return *this;
    }

  public:
    basic_ostream<charT, traits>& operator<<(bool n)
    {
      return put_impl(n);
    }
    basic_ostream<charT, traits>& operator<<(short n)
    {
      return put_impl(static_cast<long>(n));
    }
    basic_ostream<charT, traits>& operator<<(unsigned short n)
    {
      return put_impl(static_cast<unsigned long>(n));
    }
    basic_ostream<charT, traits>& operator<<(int n)
    {
      return put_impl(static_cast<long>(n));
    }
    basic_ostream<charT, traits>& operator<<(unsigned int n)
    {
      return put_impl(static_cast<unsigned long>(n));
    }
    basic_ostream<charT, traits>& operator<<(long n)
    {
      return put_impl(n);
    }
    basic_ostream<charT, traits>& operator<<(unsigned long n)
    {
      return put_impl(n);
    }
    basic_ostream<charT, traits>& operator<<(float f)
    {
      return put_impl(static_cast<double>(f));
    }
    basic_ostream<charT, traits>& operator<<(double f)
    {
      return put_impl(f);
    }
    basic_ostream<charT, traits>& operator<<(long double f)
    {
      return put_impl(f);
    }
    basic_ostream<charT, traits>& operator<<(const void* p)
    {
      return put_impl(p);
    }

    ///\name  27.6.2.5.3 basic_ostream::operator<< [lib.ostream.inserters]

    basic_ostream<charT, traits>&
      operator<<(basic_ostream<charT, traits>& (*pf)(basic_ostream<charT, traits>&))
    {
      return pf(*this);
    }

    basic_ostream<charT, traits>&
      operator<<(basic_ios<charT, traits>& (*pf)(basic_ios<charT, traits>&))
    {
      pf(*this);
      return *this;
    }

    basic_ostream<charT, traits>&
      operator<<(ios_base& (*pf)(ios_base&))
    {
      pf(*this);
      return *this;
    }

    basic_ostream<charT, traits>&
      operator<<(basic_streambuf<char_type, traits>* sb);

    ///\name  27.6.2.7 Unformatted output functions [ostream.unformatted]
    /// 1 Each unformatted output function begins execution by constructing
    ///   an object of class sentry. If this object returns true, while
    ///   converting to a value of type bool, the function endeavors to generate
    ///   the requested output. If an exception is thrown during output,
    ///   then ios::badbit is turned on (without causing an ios::failure to be thrown)
    ///   in *this’s error state. If (exceptions() & badbit) != 0 then
    ///   the exception is rethrown. In any case, the unformatted output function
    ///   ends by destroying the sentry object, then, if no exception was thrown,
    ///   returning the value specified for the unformatted output function.

    /// 2 Effects: Behaves as an unformatted output function (as described above).
    ///   After constructing a sentry object, inserts the character c, if possible.
    /// 3 Otherwise, calls setstate(badbit) (which may throw ios_base::failure (27.4.4.3)).
    /// 4 Returns: *this.
    basic_ostream<charT, traits>& put(char_type c)
    {
      sentry good( *this );
      ios_base::iostate state = ios_base::badbit;
      if ( good )
        __ntl_try
        {
          if ( this->rdbuf()->sputc(c) != traits_type::eof() )
            state = ios_base::goodbit;;
        }
        __ntl_catch(...)
        {
        }
      this->setstate(state);
      return *this;
    }

    /// 5 Effects: Behaves as an unformatted output function.
    ///   After constructing a sentry object, obtains characters to insert from
    ///   successive locations of an array whose first element is designated
    ///   by s. Characters are inserted until either of the following occurs:
    /// — n characters are inserted;
    /// — inserting in the output sequence fails (in which case the function
    ///   calls setstate(badbit), which may throw ios_base::failure.
    basic_ostream<charT, traits>& write(const char_type* s, streamsize n)
    {
      sentry good( *this );
      ios_base::iostate state = ios_base::badbit;
      if (good)
        __ntl_try {
        if (this->rdbuf()->sputn(s, n) != traits_type::eof())
          state = ios_base::goodbit;
      }
      __ntl_catch(...)
      {
      }
      this->setstate(state);
      return *this;
    }

    /// 7 Effects: Behaves as an unformatted output function.
    ///   If rdbuf() is not a null pointer, constructs a sentry object.
    ///   If this object returns true when converted to a value of type bool the
    ///   function calls rdbuf()->pubsync(). If that function returns -1 calls
    ///   setstate(badbit) (which may throw ios_base::failure (27.4.4.3)).
    ///   Otherwise, if the sentry object returns false, does nothing.
    /// 8 Returns: *this.
    basic_ostream<charT, traits>& flush()
    {
      if ( this->rdbuf() )
      {
        ///\note no sentry as it'll result in recursive call
        if ( good() )
        {
          ///\note STLPort doesn't have this line (as one above, but that doesn't matter)
          if ( tie() ) tie()->flush();
          if ( this->rdbuf()->pubsync() == -1 )
            this->setstate(ios_base::badbit);
        }
      }
      return *this;
    }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:
    basic_ostream(const basic_ostream&) __deleted;
    basic_ostream& operator=(const basic_ostream&) __deleted;

};// class basic_ostream

namespace __
{
  template<class charT, class traits>
  struct stream_writer
  {
    typedef basic_ostream<charT, traits> stream_t;

    static inline void write_widden(stream_t& os, const charT* const data, size_t length)
    {
      if(os.rdbuf()->sputn(data,length) != static_cast<streamsize>(length))
        os.setstate(ios_base::failbit);
    }
    static inline void write_narrow(stream_t& os, const charT* const data, size_t length)
    {
      const typename traits::int_type eof = traits::eof();
      basic_streambuf<charT,traits>* buf = os.rdbuf();
      for(const char* p = reinterpret_cast<const char*>(data); 
        length && !traits::eq_int_type(buf->sputc(os.widen(*p++)), eof); 
        length--);
      if(length)
        os.setstate(ios_base::failbit);
    }
    static inline void fill(stream_t& os, size_t length)
    {
      const typename traits::int_type eof = traits::eof();
      const typename stream_t::char_type fillc = os.fill();
      for(basic_streambuf<charT,traits>* buf = os.rdbuf();
        length && !traits::eq_int_type(buf->sputc(fillc), eof);
        length--);
      if(length)
        os.setstate(ios_base::failbit);
    }
    static inline stream_t& formatted_write(stream_t& os, const void* const vdata, size_t size, bool do_wide = false)
    {
      stream_t::sentry good(os);
      ios_base::iostate state = ios_base::badbit;
      if (good)
        __ntl_try {
          const charT* const data = reinterpret_cast<const charT*>(vdata);
          if(os.width() && os.width() > static_cast<streamsize>(size)){
            // need padd
            const ios_base::fmtflags adjust = os.flags() & ios_base::adjustfield;

            if(adjust != ios_base::left && adjust != ios_base::internal) // before
              fill(os, os.width() - size);

            if(os.good())
              (do_wide ? write_narrow : write_widden)(os, data, size);

            if(os.good() && adjust == ios_base::left)
              fill(os, os.width() - size);
          }else{
            (do_wide ? write_narrow : write_widden)(os, data, size);
          }
          if(os.good())
            state = ios_base::goodbit;
      }
      __ntl_catch(...)
      {
      }
      os.setstate(state);
      return os;
    }
  };
}


///\name  27.6.2.5.4 Character inserter function templates [lib.ostream.inserters.character]

template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, charT c)
{
  return __::stream_writer<charT,traits>::formatted_write(os, &c, 1);
}

template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, char c)
{
  charT wc = os.widen(c);
  return __::stream_writer<charT,traits>::formatted_write(os, &wc, 1);
}

///\name  specialization
template<class traits>
inline basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, char c)
{
  return __::stream_writer<char,traits>::formatted_write(os, &c, 1);
}

#ifdef NTL__CXX_RV
template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>&& os, charT c)
{
  return __::stream_writer<charT,traits>::formatted_write(os, &c, 1);
}
template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>&& os, char c)
{
  charT wc = os.widen(c);
  return __::stream_writer<charT,traits>::formatted_write(os, &wc, 1);
}
template<class traits>
inline basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&& os, char c)
{
  return __::stream_writer<charT,traits>::formatted_write(os, &c, 1);
}
#endif

///\name  signed and unsigned

template<class traits>
basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, signed char);

template<class traits>
basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&, unsigned char);

template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const charT*& s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s) __::stream_writer<charT,traits>::formatted_write(os, s, traits::length(s));
  return os;
}

template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const char*& s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s) __::stream_writer<charT,traits>::formatted_write(os, s, char_traits<char>::length(s), true);
  return os;
}

///\name  partial specializationss
template<class traits>
inline basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, const char*& s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s) __::stream_writer<char,traits>::formatted_write(os, s, char_traits<char>::length(s));
  return os;
}

template<class charT, class traits, size_t length>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const charT (&s)[length])
{
  return __::stream_writer<charT,traits>::formatted_write(os, s, length-1);
}
template<class traits, size_t length>
inline basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>& os, const char (&s)[length])
{
  return __::stream_writer<char,traits>::formatted_write(os, s, length-1);
}
template<class charT, class traits, size_t length>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>& os, const char (&s)[length])
{
  return __::stream_writer<charT,traits>::formatted_write(os, s, length-1, true);
}

///\name  signed and unsigned

template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, const signed char*);

template<class traits>
basic_ostream<char, traits>&
operator<<(basic_ostream<char, traits>&, const unsigned char*);

#ifdef NTL__CXX_RV
template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>&& os, const charT* s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s) os.write(s, traits::length(s));
  return os;
}
template<class charT, class traits>
inline basic_ostream<charT, traits>& operator<<(basic_ostream<charT, traits>&& os, const char* s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s){
    size_t l = char_traits<char>::length(s);
    while(l-- && os)
      os.put(os.widen(*s++));
  }
  return os;
}
template<class traits>
inline basic_ostream<char, traits>& operator<<(basic_ostream<char, traits>&& os, const char* s)
{
  assert(s != 0 && "s shall not be a null pointer");
  if(s) os.write(s, char_traits<char>::length(s));
  return os;
}
#endif

///\name Swap
template <class charT, class traits>
inline void swap(basic_ostream<charT, traits>& x, basic_ostream<charT, traits>& y)  { x.swap(y); }
#ifdef NTL__CXX_RV
template <class charT, class traits>
inline void swap(basic_ostream<charT, traits>&& x, basic_ostream<charT, traits>& y) { x.swap(y); }
template <class charT, class traits>
inline void swap(basic_ostream<charT, traits>& x, basic_ostream<charT, traits>&& y) { x.swap(y); }
#endif


///\name  27.6.2.7 Standard basic_ostream manipulators [lib.ostream.manip]

template <class charT, class traits>
inline
basic_ostream<charT, traits>&
  endl(basic_ostream<charT, traits>& os)
{
  os.put(os.widen('\n') );
  os.flush();
  return os;
}

template <class charT, class traits>
inline
basic_ostream<charT, traits>&
  ends(basic_ostream<charT, traits>& os)
{
  os.put(charT());
  return os;
}

template <class charT, class traits>
inline
basic_ostream<charT, traits>&
  flush(basic_ostream<charT, traits>& os)
{
  os.flush();
  return os;
}

///\name  Inserters and extractors [21.3.7.9 string.io]
template<class charT, class traits, class Allocator>
basic_ostream<charT, traits>& operator<<(typename __::ostream<charT,traits>::type os, const basic_string<charT,traits,Allocator>& str)
{
  return __::stream_writer<charT,traits>::formatted_write(os, str.data(), str.size());
}


/**@} lib_output_streams */
/**@} lib_iostream_format */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_OSTREAM
