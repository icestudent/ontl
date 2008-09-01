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

/**\addtogroup  lib_input_output ******* [27] Input/output library **********
 *@{*/

/**\addtogroup  lib_iostream_format **** [27.6] Formatting and manipulators *
 *@{*/

/**\addtogroup  lib_output_streams ***** [27.6.2] Output streams ************
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
    basic_ostream(basic_ostream&& rhs);
#endif

    ///\name  basic_ostream destructor
    /// 3 Effects: Destroys an object of class basic_ostream.
    /// 4 Remarks: Does not perform any operations on rdbuf().
    virtual ~basic_ostream() {}
    ///@}

#ifdef NTL__CXX
    ///\name 27.6.2.3 Class basic_ostream assign and swap [ostream.assign]
    basic_ostream& operator=((basic_ostream&& rhs);
    void swap(basic_ostream&& rhs);
#endif

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
      return this->fail() ? pos_type(-1) : this->rdbuf()->pubseekoff(0, cur, out);
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
        this->rdbuf()->pubseekoff(off, dir, ios_base::out).;
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

    basic_ostream<charT, traits>& operator<<(bool n)
    {
      sentry good( *this );
      ios_base::iostate state = ios_base::badbit;
      if ( good )
        __ntl_try
        {
          typedef ostreambuf_iterator<charT, traits> iterator;
          typedef num_put<charT, iterator> numput;
          if ( !use_facet<numput>(this->getloc()).put(iterator(*this), *this,
                                                    this->fill(), n).failed() )
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

    basic_ostream<charT, traits>& operator<<(short n);

    basic_ostream<charT, traits>& operator<<(unsigned short n);

    basic_ostream<charT, traits>& operator<<(int n);

    basic_ostream<charT, traits>& operator<<(unsigned int n);

    basic_ostream<charT, traits>& operator<<(long n);

    basic_ostream<charT, traits>& operator<<(unsigned long n);

    basic_ostream<charT, traits>& operator<<(float f);

    basic_ostream<charT, traits>& operator<<(double f);

    basic_ostream<charT, traits>& operator<<(long double f);

    basic_ostream<charT, traits>& operator<<(const void* p);

    ///\name  basic_ostream::operator<< [27.6.2.5.3 lib.ostream.inserters]

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
    basic_ostream<charT, traits>& write(const char_type* s, streamsize n);

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
    basic_ostream(const basic_ostream&); // not defined
    basic_ostream& operator=(const basic_ostream&); // not defined

};// class basic_ostream

///\name  Character inserter function templates [27.6.2.5.4 lib.ostream.inserters.character]

template<class charT, class traits>
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>&, charT);

template<class charT, class traits>
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>&, char);

///\name  specialization
template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, char);

///\name  signed and unsigned

template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, signed char);

template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, unsigned char);

template<class charT, class traits>
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>&, const charT*);

template<class charT, class traits>
basic_ostream<charT, traits>&
  operator<<(basic_ostream<charT, traits>&, const char*);

///\name  partial specializationss
template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, const char*);

///\name  signed and unsigned

template<class traits>
basic_ostream<char, traits>&
  operator<<(basic_ostream<char, traits>&, const signed char*);

template<class traits>
basic_ostream<char, traits>&
operator<<(basic_ostream<char, traits>&, const unsigned char*);

///\name  Standard basic_ostream manipulators [27.6.2.7 lib.ostream.manip]

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

///@}
/**@} lib_output_streams */
/**@} lib_iostream_format */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_OSTREAM
