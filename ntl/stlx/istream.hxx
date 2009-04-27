/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.1 Input streams [input.streams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_ISTREAM
#define NTL__STLX_ISTREAM

#include "ios.hxx"
#include "ostream.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

/**\addtogroup  lib_iostream_format **** 27.6 Formatting and manipulators
 *@{*/

/**\addtogroup  lib_input_streams ****** 27.6.1 Input streams
 *@{*/

/// 27.6.1.1 Class template basic_istream [istream]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_istream : virtual public basic_ios<charT, traits>
{
    typedef basic_ios<charT, traits>  this_base;

  ///////////////////////////////////////////////////////////////////////////
  public:

    // Types (inherited from basic_ios (27.4.4)):
    typedef typename this_base::char_type   char_type;
    typedef typename this_base::int_type    int_type;
    typedef typename this_base::pos_type    pos_type;
    typedef typename this_base::off_type    off_type;
    typedef typename this_base::traits_type traits_type;

    // 27.6.1.1.1 basic_istream constructors [istream.cons]
    explicit basic_istream(basic_streambuf<charT, traits>* sb)
    : ccount(0)
    {
      basic_ios::init(sb);
    }

    //basic_istream(basic_istream&& rhs);

    virtual ~basic_istream() {/**/}

    // 27.6.1.1.2 Class basic_istream assign and swap [istream.assign]

    //basic_istream& operator=(basic_istream&& rhs);
    //void swap(basic_istream&& rhs);

    // 27.6.1.1.3 Class basic_istream::sentry [istream::sentry]
    class sentry
    {
        typedef traits traits_type;

      public:

        // constructor for formated input
        explicit sentry(basic_istream& is);// bool noskipws = false

        // constructor for unformated input
        explicit sentry(basic_istream& is, bool noskipws)
        //: is(is)
        {
          (noskipws);
          assert(noskipws == true);
          if ( is.good() )
          {
            if ( is.tie() ) is.tie()->flush();
          }
          ok_ = is.good();
        }

        ~sentry() {/**/}

        operator bool() const { return ok_; }

      private:

        //basic_istream<charT, traits>& is;
        bool ok_;

        sentry(const sentry&); // not defined
        sentry& operator=(const sentry&); // not defined

    };

    // 27.6.1.2 Formatted input functions [istream.formatted]

    ///\name 27.6.1.2.2 Arithmetic Extractors [istream.formatted.arithmetic]
    basic_istream<charT,traits>& operator>>(bool& n );
    basic_istream<charT,traits>& operator>>(short& n );
    basic_istream<charT,traits>& operator>>(unsigned short& n );
    basic_istream<charT,traits>& operator>>(int& n );
    basic_istream<charT,traits>& operator>>(unsigned int& n );
    basic_istream<charT,traits>& operator>>(long& n );
    basic_istream<charT,traits>& operator>>(unsigned long& n );
    basic_istream<charT,traits>& operator>>(long long& n );
    basic_istream<charT,traits>& operator>>(unsigned long long& n );
    basic_istream<charT,traits>& operator>>(float& f );
    basic_istream<charT,traits>& operator>>(double& f );
    basic_istream<charT,traits>& operator>>(long double& f );
    basic_istream<charT,traits>& operator>>(void*& p );

    ///\name 27.6.1.2.3 basic_istream::operator>> [istream::extractors]
    basic_istream<charT, traits>&
      operator>>(basic_istream<charT,traits>&(*pf)(basic_istream<charT,traits>&))
    {
      return pf(*this);
    }

    basic_istream<charT, traits>&
      operator>>(basic_ios<charT,traits>& (*pf)(basic_ios<charT,traits>&))
    {
      return pf(*this);
    }

    basic_istream<charT, traits>&
      operator>>(ios_base& (*pf)(ios_base&))
    {
      return pf(*this);
    }

    basic_istream<charT, traits>&
      operator>>(basic_streambuf<char_type, traits>* sb);

    ///\todo non-member operator>>

    ///\name 27.6.1.3 Unformatted input functions [istream.unformatted]

    streamsize gcount() const { return ccount; }

    int_type get()
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      int_type c = traits_type::eof();
      if ( ok )
        __ntl_try
        {
          c = this->rdbuf()->sbumpc();
          if ( traits_type::eq_int_type(traits_type::eof(), c) )
            state |= ios_base::eofbit | ios_base::failbit;
          else ++ccount;
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
//          if ( this->exceptions() & ios_base::badbit ) __ntl_rethrow;
        }
      this->setstate(state);
      return c;
    }

    basic_istream<charT,traits>& get(char_type& c)
    {
      const int_type ic = get();
      if ( !traits_type::eq_int_type(traits_type::eof(), ic) )
        c = traits_type::to_char_type(ic);
      return *this;
    }

    basic_istream<charT,traits>& get(char_type* s, streamsize n)
    {
      return get(s, n, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& get(char_type* s, streamsize n, char_type delim)
    {
      ccount = 0;
      if ( n > 0 ) *s = char_type();
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      if ( ok )
        __ntl_try
        {
          while ( 0 < --n )
          {
            s[1] = char_type();
            const int_type c = this->rdbuf()->sbumpc();
            if ( traits_type::eq_int_type(traits_type::eof(), c) )
            {
              state |= ios_base::eofbit;
              break;
            }
            if ( traits_type::eq(delim, traits_type::to_char_type(c)) )
              break;
            *s++ = traits_type::to_char_type(c);
            ++ccount;
          }
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
      this->setstate(state | ccount ? state : ios_base::failbit);
      return *this;
    }

    basic_istream<charT,traits>& get(basic_streambuf<char_type,traits>& sb)
    {
      return get(sb, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& get(basic_streambuf<char_type,traits>& dest, char_type delim)
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      if ( ok )
        __ntl_try
        {
          for ( const int_type c = this->rdbuf()->sgetc(); ; c = this->rdbuf()->snextc() )
          {
            if ( traits_type::eq_int_type(traits_type::eof(), c) )
            {
              state |= ios_base::eofbit;
              break;
            }
            if ( traits_type::eq(delim, traits_type::to_char_type(c)) )
              break;
            if ( traits_type::eq_int_type(traits_type::eof(), dest.sputc(traits_type::to_char_type(c))) )
              break;
            ++ccount;
          }
        }
        __ntl_catch(...)
        {
          break;
        }
      this->setstate(state | ccount ? state : ios_base::failbit);
      return *this;
    }

    basic_istream<charT,traits>& getline(char_type* s, streamsize n)
    {
      return getline(s, this_base::widen('\n'));
    }

    basic_istream<charT,traits>& getline(char_type* s, streamsize n, char_type delim)
    ;

    basic_istream<charT,traits>& ignore(streamsize n = 1, int_type delim = traits::eof());

    int_type peek()
    {
      ccount = 0;
      const sentry ok(*this, true);
      ios_base::iostate state = ios_base::goodbit;
      int_type c = traits_type::eof();
      if ( ok )
        __ntl_try
        {
          c = this->rdbuf()->sbumpc();
          if ( traits_type::eq_int_type(traits_type::eof(), c) )
            state |= ios_base::eofbit;
        }
        __ntl_catch(...)
        {
          state |= ios_base::badbit;
        }
      this->setstate(state);
      return c;
    }

    basic_istream<charT,traits>& read(char_type* s, streamsize n);
    streamsize readsome(char_type* s, streamsize n);
    basic_istream<charT,traits>& putback(char_type c);
    basic_istream<charT,traits>& unget();
    int sync();
    pos_type tellg();
    basic_istream<charT,traits>& seekg(pos_type);
    basic_istream<charT,traits>& seekg(off_type, ios_base::seekdir);

  ///////////////////////////////////////////////////////////////////////////
  private:

    streamsize ccount;

    basic_istream(const basic_istream&);
    basic_istream& operator=(const basic_istream&);

};


/// 27.6.1.4 Standard basic_istream manipulators [istream.manip]
/// 1 Effects: Behaves as an unformatted input function (as described in 27.6.1.3/1),
///   except that it does not count the number of characters extracted and
///   does not affect the value returned by subsequent calls to is.gcount().
///   After constructing a sentry object extracts characters as long as
///   the next available character c is whitespace or until there are no more
///   characters in the sequence. Whitespace characters are distinguished with
///   the same criterion as used by sentry::sentry (27.6.1.1.3).
///   If ws stops extracting characters because there are no more available it
///   sets eofbit, but not failbit.
/// 2 Returns: is.
template <class charT, class traits>
basic_istream<charT,traits>&
  ws(basic_istream<charT,traits>& is);


/// 27.6.1.5 Class template basic_iostream [iostreamclass].
/// 1 The class basic_iostream inherits a number of functions that allow
///   reading input and writing output to sequences controlled by a stream buffer.
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_iostream
: public basic_istream<charT, traits>, public basic_ostream<charT, traits>
{
  public:

    ///\name Types

    typedef charT char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits traits_type;

    ///\name 27.6.1.5.1 basic_iostream constructors [iostream.cons]

    /// 1 Effects: Constructs an object of class basic_iostream, assigning
    ///   initial values to the base classes by calling
    ///   basic_istream<charT,traits>(sb) (27.6.1.1)
    ///   and basic_ostream<charT,traits>(sb) (27.6.2.1)
    /// 2 Postcondition: rdbuf()==sb and gcount()==0.
    explicit basic_iostream(basic_streambuf<charT,traits>* sb)
    : basic_istream<charT, traits>(sb), basic_ostream<charT, traits>(sb)
    {;}

#ifdef NTL__CXX_RV
    /// 3 Effects: Move constructs from the rvalue rhs by constructing
    ///   the basic_istream base class with move(rhs).
    basic_iostream(basic_iostream&& rhs);
#endif

    ///\name 27.6.1.5.2 basic_iostream destructor [iostream.dest]
    /// 1 Effects: Destroys an object of class basic_iostream.
    /// 2 Remarks: Does not perform any operations on rdbuf().
    virtual ~basic_iostream() {;}

#ifdef NTL__CXX_RV
    ///\name 27.6.1.5.3 basic_iostream assign and swap [iostream.assign]

    /// 1 Effects: swap(rhs).
    basic_iostream& operator=(basic_iostream&& rhs);

    /// 2 Effects: Calls basic_istream<charT, traits>::swap(rhs).
    void swap(basic_iostream&& rhs);
#endif
    ///\}
};

/// 3 Effects: x.swap(y).
template <class charT, class traits>
void swap(basic_iostream<charT, traits>& x, basic_iostream<charT, traits>& y)  { x.swap(y); }

#if defined NTL__CXX_RV && 0 // disabled in N2857
template <class charT, class traits>
void swap(basic_iostream<charT, traits>&& x, basic_iostream<charT, traits>& y) { x.swap(y); }
template <class charT, class traits>
void swap(basic_iostream<charT, traits>& x, basic_iostream<charT, traits>&& y) { x.swap(y); }

/// 27.7.1.6 Rvalue stream extraction [istream.rvalue]
template <class charT, class traits, class T>
inline basic_istream<charT, traits>& operator>>(basic_istream<charT, traits>&& is, T& x)
{
  return is >> x;
}
#endif
/**@} lib_input_streams */
/**@} lib_iostream_format */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_ISTREAM
