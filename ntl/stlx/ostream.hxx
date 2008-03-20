/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.6.2 Output streams [output.streams]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_OSTREAM
#define NTL__STLX_OSTREAM

#include "ios.hxx"
#include "locale.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library **********
 *@{*/

/**\addtogroup  lib_iostream_format **** [27.6] Formatting and manipulators *
 *@{*/

/**\addtogroup  lib_output_streams ***** [27.6.2] Output streams ************
 *@{*/

/// 27.6.2.1 Class template basic_ostream [ostream]
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

    ///\name  basic_ostream constructors [27.6.2.2 lib.ostream.cons]
    explicit basic_ostream(basic_streambuf<char_type, traits>* sb)
    {
      basic_ios<charT, traits>::init(sb);
    }

    ///\name  basic_ostream destructor
    virtual ~basic_ostream() {}
    ///@}

    /// Class basic_ostream::sentry [27.6.2.3 lib.ostream::sentry]
    class sentry
    {
      public:

        explicit sentry(basic_ostream<charT, traits>& os) : os(os)
        {
          if ( os.good() )
          {
            if ( os.tie() ) os.tie()->flush();
            ok_ = os.good();
          }
        }

        ~sentry()
        {
          if ( (os.flags() & ios_base::unitbuf) /*&& !uncaught_exception()*/ )
            os.flush();
        }

        operator bool() const { return ok_; }

      private:

        basic_ostream<charT, traits>& os;
        bool ok_;
        

        sentry(const sentry&); // not defined
        sentry& operator=(const sentry&); // not defined
    };

    ///\name  basic_ostream seek members [27.6.2.4 lib.ostream.seeks]

    pos_type tellp()
    { 
      return fail() ? pos_type(-1) : rdbuf()->pubseekoff(0, cur, out);
    }

    basic_ostream<charT, traits>& seekp(pos_type pos)
    {
      if ( ! fail() && rdbuf()->pubseekpos(pos) == -1 )
        setstate(failbit);
      return *this;
    }

    basic_ostream<charT, traits>& seekp(off_type off, ios_base::seekdir dir)
    {
      if ( ! fail() ) rdbuf()->pubseekoff(off, dir);
      return *this;
    }

    //  Formatted output functions [27.6.2.5 lib.ostream.formatted]

    ///\name  Arithmetic Inserters [27.6.2.5.2 lib.ostream.inserters.arithmetic]

    basic_ostream<charT, traits>& operator<<(bool n);

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

    ///\name  Unformatted output functions [27.6.2.6 lib.ostream.unformatted]

    basic_ostream<charT, traits>& put(char_type c)
    {
      sentry good( *this );
      if ( true == good )
      {
        rdbuf()->sputc(c);
      }
    }

    basic_ostream<charT, traits>& write(const char_type* s, streamsize n);
    basic_ostream<charT, traits>& flush(){ return *this; }

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
