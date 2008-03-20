/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.4 Iostreams base classes [iostreams.base]
 *
 ****************************************************************************
 */

#include "locale.hxx" // locale.hxx includes this header

#ifndef NTL__STLX_IOS
#define NTL__STLX_IOS

#include "cstddef.hxx"
#include "iosfwd.hxx"
#include "stdexcept.hxx"
#include "string.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* [27] Input/output library **********
 *@{*/

///\name  27.4.1 Types [stream.types]
typedef ptrdiff_t streamoff;
typedef ssize_t   streamsize;
///@}

/// 27.4.2 Class ios_base [ios.base]
class ios_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name 27.4.2.1 Types [ios.types]

    /// 27.4.2.1.1 Class ios_base::failure [ios::failure]
    class failure : public runtime_error
    {
      public:
        explicit failure(const string& msg) : runtime_error(msg) {}
      private:
        failure operator=(const failure&);
    };

    /// 27.4.2.1.2 Type ios_base::fmtflags [ios::fmtflags]
    typedef uint16_t fmtflags;
    static const fmtflags unitbuf     = 1 << 0;
    static const fmtflags uppercase   = 1 << 1;
    static const fmtflags internal    = 1 << 2;
    static const fmtflags left        = 1 << 3;
    static const fmtflags right       = 1 << 4;
    static const fmtflags adjustfield = left | right | internal;
    static const fmtflags dec         = 1 << 5;
    static const fmtflags oct         = 1 << 6;
    static const fmtflags hex         = 1 << 7;
    static const fmtflags basefield   = dec | oct | hex;
    static const fmtflags showbase    = 1 << 8;
    static const fmtflags showpos     = 1 << 9;
    static const fmtflags boolalpha   = 1 << 10;
    static const fmtflags fixed       = 1 << 11;
    static const fmtflags scientific  = 1 << 12;
    static const fmtflags floatfield  = scientific | fixed;
    static const fmtflags showpoint   = 1 << 13;
    static const fmtflags skipws      = 1 << 14;

    /// 27.4.2.1.3 Type ios_base::iostate [ios::iostate]
    typedef uint8_t iostate;
    static const iostate goodbit  = 0;
    static const iostate badbit   = 1 << 0;
    static const iostate eofbit   = 1 << 1;
    static const iostate failbit  = 1 << 2;

    static
    const char * __get_iostate_c_str(iostate state)
    {
      static const char * const msg[8] =
      {
        "goodbit",        // 0
        "badbit",         // 1
        "eofbit",         // 2
        "badbit|eofbit",  // 1|2
        "failbit",        // 4
        "badbit|failbit", // 1|4
        "eofbit|failbit", // 2|4
        "badbit|eofbit|failbit", // 1|2|4
      };
      return msg[state];
    }

    /// 27.4.2.1.4 Type ios_base::openmode [ios::openmode]
    typedef uint8_t openmode;
    static const openmode app     = 1 << 0;
    static const openmode ate     = 1 << 1;
    static const openmode trunc   = 1 << 2;
    static const openmode in      = 1 << 3;
    static const openmode out     = 1 << 4;
    static const openmode binary  = 1 << 5;
 
    /// 27.4.2.1.5 Type ios_base::seekdir [ios::seekdir]
    enum seekdir { beg, cur, end };

    /// 27.4.2.1.6 Class ios_base::Init [ios::Init]
    class Init
    {
      public:

        Init()
        { 
          if ( ! init_cnt )
          {
            init_cnt = 1;
            // __ntl_construct_iostream_objects();
          }
          ++init_cnt;
        }

        ~Init()
        {
          if ( --init_cnt == 1 )
          {
            // __ntl_destroy_iostream_objects();
          }
        }

      private:
        static int init_cnt;
    };

    ///\name  27.4.2.2 ios_base state functions [fmtflags.state]

    fmtflags flags() const
    {
      return fmtfl;
    }
    
    fmtflags flags(fmtflags fmtfl)
    {
      const fmtflags old = this->fmtfl;
      this->fmtfl = fmtfl;
      return old;
    }

    fmtflags setf(fmtflags fmtfl)
    {
      const fmtflags old = this->fmtfl;
      this->fmtfl |= fmtfl;
      return old;
    }
    
    fmtflags setf(fmtflags fmtfl, fmtflags mask)
    {
      const fmtflags old = this->fmtfl;
      this->fmtfl = fmtflags(fmtfl & mask | old & ~mask);
      return old;
    }

    void unsetf(fmtflags mask)
    {
      fmtfl &= ~mask;
    }
    
    streamsize precision() const
    {
      return prec;
    }
    
    streamsize precision(streamsize prec)
    {
      const streamsize old = this->prec;
      this->prec = prec;
      return old;
    }
    
    streamsize width() const
    {
      return wide;
    }
      
    streamsize width(streamsize wide)
    {
      const streamsize old = this->wide;
      this->wide = wide;
      return old;
    }

    ///\name  27.4.2.3 ios_base locale functions [ios.base.locales]
    locale imbue(const locale& loc);

    ///\note empty locale is always used to avoid lots of stuff
    locale getloc() const
    {
      return locale();
    }

    ///\name  27.4.2.4 ios_base static members [ios.members.static]
    /// not implemented as is stdio
    static bool sync_with_stdio(bool sync = true);

    ///\name  27.4.2.5 ios_base storage functions [ios.base.storage]
    static int xalloc();
    long& iword(int index);
    void*& pword(int index);

    ///\name  27.4.2.6 ios_base callbacks [ios.base.callback]
    enum event { erase_event, imbue_event, copyfmt_event };
    typedef void (*event_callback)(event, ios_base&, int index);
    void register_callback(event_callback fn, int index);
    ///@}

    virtual ~ios_base()
    {
      // Calls each registered callback pair
    }

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ios_base() {/**/}

    void init()
    {
      this->prec = 6;
      this->wide = 0;
      this->fmtfl = skipws | dec;
      this->exceptmask = goodbit;
    }

  ///////////////////////////////////////////////////////////////////////////
  private:

    // members are sorted by size
    
    streamsize  prec;
    streamsize  wide;

    fmtflags  fmtfl;

    // these 2 moved here from basic_ios for size optimization
    template <class charT, class traits> friend class basic_ios;
    iostate state;
    iostate exceptmask;

    // static int index;
    // long* iarray;
    // void** parray;

#if STLX__CONFORMING_LOCALE
    locale loc;
#endif

  ///////////////////////////////////////////////////////////////////////////
  private:

    ios_base(const ios_base&);
    ios_base& operator=(const ios_base&);

};

/// 27.4.3 Class template fpos [fpos]
template <class stateT>
class fpos
{
  public:

    fpos(int i) : state_(i) {}

    ///\name 27.4.3.1 fpos Members [fpos.members]

    stateT state() const { return state_; }
    void state(stateT s) { state_ = s; }

    ///@}

  private:
     stateT state_;
};

/// 27.4.4 Class template basic_ios [ios]
template <class charT, class traits /*= char_traits<charT>*/ >
class basic_ios : public ios_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    // Types:

    typedef charT                     char_type;
    typedef typename traits::int_type int_type;
    typedef typename traits::pos_type pos_type;
    typedef typename traits::off_type off_type;
    typedef traits                    traits_type;

    ///\name  27.4.4.1 basic_ios constructors [basic.ios.cons]
    explicit basic_ios(basic_streambuf<charT, traits>* sb) { init(sb); }

    /// @note The destructor does not destroy rdbuf().
    virtual ~basic_ios() {/**/}

  ///////////////////////////////////////////////////////////////////////////
  protected:

    basic_ios() {}

    void init(basic_streambuf<charT, traits>* sb)
    {
      ios_base::init();
      this->sb = sb;
      this->tiestr = 0;
      this->fillc = widen(' ');
      this->clear();
    }

  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name  27.4.4.2 Member functions [basic.ios.members]

    basic_ostream<charT, traits>* tie() const
    {
      return tiestr;
    }
    
    basic_ostream<charT, traits>* tie(basic_ostream<charT, traits>* tiestr)
    {
      basic_ostream<charT, traits>* const old = this->tiestr;
      this->tiestr = tiestr;
      return old;
    }
    
    basic_streambuf<charT, traits>* rdbuf() const
    {
      return sb;
    }

    basic_streambuf<charT, traits>* rdbuf(basic_streambuf<charT, traits>* sb)
    {
      basic_streambuf<charT, traits>* const old = this->sb;
      this->sb = sb;
      return old;
    }
    
    locale imbue(const locale& loc)
    {
      const locale old = ios_base::imbue(loc);
      if ( rdbuf() ) rdbuf()->pubimbue(loc);
      return old;
    }
    
    char narrow(char_type c, char dfault) const
    {
      return use_facet<ctype<char_type> >(getloc()).narrow(c, dfault);
    }
    
    char_type widen(char c) const
    {
      return use_facet<ctype<char_type> >(getloc()).widen(c);
    }

    char_type fill() const
    {
      return fillc;
    }
    
    char_type fill(char_type ch)
    {
      const char_type old = fillc;
      fillc = ch;
      return old;
    }

    basic_ios& copyfmt(const basic_ios& rhs);

    ///\name 27.4.4.3 basic_ios flags functions [iostate.flags]
    
    operator void*() const { return fail() ? 0 : this; }
    bool operator!() const { return fail(); }

    iostate rdstate() const { return state; }

    __forceinline
    void clear(iostate state = goodbit) throw(failure)
    {
      if ( !rdbuf() ) state |= badbit;
      const iostate failre = static_cast<iostate>((this->state = state) & exceptmask);
      if ( failre ) __ntl_throw(failure(__get_iostate_c_str(failre)));
    }

    __forceinline
    void setstate(iostate state)
    {
      if ( state != goodbit ) clear(static_cast<iostate>(rdstate() | state));
    }

    bool good() const { return rdstate() == goodbit; }
    bool eof() const  { return rdstate() & eofbit != 0; }
    bool fail() const { return rdstate() & (failbit | badbit) != 0; }
    bool bad() const  { return rdstate() & badbit != 0; }

    iostate exceptions() const { return exceptmask; }

    void exceptions(iostate exm)
    {
      exceptmask = exm;
      clear(rdstate());
    }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:

    basic_ios(const basic_ios&); // not defined
    basic_ios& operator=(const basic_ios&); // not defined

    basic_streambuf<charT, traits>* sb;
    basic_ostream<charT, traits>*   tiestr;

    char_type fillc;

};

/**\addtogroup  lib_std_ios_manip ****** [27.4.5] ios_base manipulators *****
 *@{*/

///\name  27.4.5.1 fmtflags manipulators [fmtflags.manip]

inline
ios_base&
  boolalpha(ios_base& str)
{
  str.setf(ios_base::boolalpha);
  return str;
}

inline
ios_base&
  noboolalpha(ios_base& str)
{
  str.unsetf(ios_base::boolalpha);
  return str;
}

inline
ios_base&
  showbase(ios_base& str)
{
  str.setf(ios_base::showbase);
  return str;
}

inline
ios_base&
  noshowbase(ios_base& str)
{
  str.unsetf(ios_base::showbase);
  return str;
}

inline
ios_base&
  showpoint(ios_base& str)
{
  str.setf(ios_base::showpoint);
  return str;
}
  
inline
ios_base&
  noshowpoint(ios_base& str)
{
  str.unsetf(ios_base::showpoint);
  return str;
}

inline
ios_base&
  showpos(ios_base& str)
{
  str.setf(ios_base::showpos);
  return str;
}

inline
ios_base&
  noshowpos(ios_base& str)
{
  str.unsetf(ios_base::showpos);
  return str;
}

inline
ios_base&
  skipws(ios_base& str)
{
  str.setf(ios_base::skipws);
  return str;
}

inline
ios_base&
  noskipws(ios_base& str)
{
  str.unsetf(ios_base::skipws);
  return str;
}

inline
ios_base&
  uppercase(ios_base& str)
{
  str.setf(ios_base::uppercase);
  return str;
}

inline
ios_base&
  nouppercase(ios_base& str)
{
  str.unsetf(ios_base::uppercase);
  return str;
}

inline
ios_base& unitbuf(ios_base& str)
{
  str.setf(ios_base::unitbuf);
  return str;
}

inline
ios_base&
  nounitbuf(ios_base& str)
{
  str.unsetf(ios_base::unitbuf);
  return str;
}

///\name  27.4.5.2 adjustfield manipulators [adjustfield.manip]

inline
ios_base&
  internal(ios_base& str)
{
  str.setf(ios_base::internal, ios_base::adjustfield);
  return str;
}

inline
ios_base&
  left(ios_base& str)
{
  str.setf(ios_base::left, ios_base::adjustfield);
  return str;
}

inline
ios_base&
  right(ios_base& str)
{
  str.setf(ios_base::right, ios_base::adjustfield);
  return str;
}

///\name  27.4.5.3 basefield manipulators [basefield.manip]

inline
ios_base&
  dec(ios_base& str)
{
  str.setf(ios_base::dec, ios_base::basefield);
  return str;
}

inline
ios_base&
  hex(ios_base& str)
{
  str.setf(ios_base::hex, ios_base::basefield);
  return str;
}

inline
ios_base&
  oct(ios_base& str)
{
  str.setf(ios_base::oct, ios_base::basefield);
  return str;
}

///\name  27.4.5.4 floatfield manipulators [floatfield.manip]

inline
ios_base&
  fixed(ios_base& str)
{
  str.setf(ios_base::fixed, ios_base::floatfield);
  return str;
}

inline
ios_base&
  scientific(ios_base& str)
{
  str.setf(ios_base::scientific, ios_base::floatfield);
  return str;
}

///@}
/**@} lib_std_ios_manip */
/**@} lib_input_output */

}//namespace std

#endif//#ifndef NTL__STLX_IOS
