/**\file*********************************************************************
 *                                                                     \brief
 *  N2315 27.4 Iostreams base classes [iostreams.base]
 *
 ****************************************************************************
 */

#include "locale.hxx" // locale.hxx includes this header

#ifndef NTL__STLX_IOS
#define NTL__STLX_IOS

#include "cstdio.hxx"
#include "iosfwd.hxx"
#include "system_error.hxx"

namespace std {

/**\addtogroup  lib_input_output ******* 27 Input/output library [input.output]
 *@{*/

///\name  27.4.1 Types [stream.types]
typedef ptrdiff_t streamoff;
typedef ssize_t   streamsize;
///@}

///\name 27.5.5.5 Error reporting [error.reporting] (N2857)
#ifdef NTL__CXX_ENUM

enum class io_errc {
  stream = 1
};

template<>
struct is_error_code_enum<io_errc>: true_type{};

#else

__class_enum(io_errc)
{
  stream = 1
};};

template<>
struct is_error_code_enum<io_errc::type>: true_type{};

#endif

inline error_code make_error_code(io_errc e)
{
  return error_code(static_cast<int>(e), iostream_category());
}

inline error_condition make_error_condition(io_errc e)
{
  return error_condition(static_cast<int>(e), iostream_category());
}
///\}

/// 27.4.2 Class ios_base [ios.base]
class ios_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name 27.4.2.1 Types [ios.types]

    /**
     *	@brief 27.5.2.1.1 Class ios_base::failure [ios::failure] (N2857)
     *  @details The class failure defines the base class for the types of all objects thrown as exceptions, 
     *  by functions in the iostreams library, to report errors detected during stream buffer operations.
     **/
    class failure:
      public system_error
    {
    public:
      explicit failure(const string& msg, const error_code& ec = io_errc::stream)
        :system_error(ec, msg)
      {}
      explicit failure(const char* msg, const error_code& ec = io_errc::stream)
        :system_error(ec, msg)
      {}
      //virtual const char* what() const throw();
    };

    /// 27.4.2.1.2 Type ios_base::fmtflags [ios::fmtflags]
    typedef uint16_t fmtflags;
    /// flushes output after each output operation
    static const fmtflags unitbuf     = 1 << 0;
    /// replaces certain lowercase letters with their uppercase equivalents in generated output
    static const fmtflags uppercase   = 1 << 1;
    /// adds fill characters at a designated internal point in certain generated
    /// output, or identical to right if no such point is designated
    static const fmtflags internal    = 1 << 2;
    /// adds fill characters on the right (final positions) of certain generated output
    static const fmtflags left        = 1 << 3;
    /// adds fill characters on the left (initial positions) of certain generated output
    static const fmtflags right       = 1 << 4;
    static const fmtflags adjustfield = left | right | internal;
    /// converts integer input or generates integer output in decimal base
    static const fmtflags dec         = 1 << 5;
    /// converts integer input or generates integer output in octal base
    static const fmtflags oct         = 1 << 6;
    /// converts integer input or generates integer output in hexadecimal base
    static const fmtflags hex         = 1 << 7;
    static const fmtflags basefield   = dec | oct | hex;
    /// generates a prefix indicating the numeric base of generated integer output
    static const fmtflags showbase    = 1 << 8;
    /// generates a + sign in non-negative generated numeric output
    static const fmtflags showpos     = 1 << 9;
    /// insert and extract bool type in alphabetic format
    static const fmtflags boolalpha   = 1 << 10;
    /// generate floating-point output in fixed-point notation
    static const fmtflags fixed       = 1 << 11;
    /// generates floating-point output in scientific notation
    static const fmtflags scientific  = 1 << 12;
    static const fmtflags floatfield  = scientific | fixed;
    /// generates a decimal-point character unconditionally in generated floating-point output
    static const fmtflags showpoint   = 1 << 13;
    /// skips leading whitespace before certain input operations
    static const fmtflags skipws      = 1 << 14;

    /// 27.4.2.1.3 Type ios_base::iostate [ios::iostate]
    typedef uint8_t iostate;
    /** indicates a good state */
    static const iostate goodbit  = 0;
    /** indicates a loss of integrity in an input or output sequence (such as an irrecoverable read error from a file) */
    static const iostate badbit   = 1 << 0;
    /** indicates that an input operation reached the end of an input sequence */
    static const iostate eofbit   = 1 << 1;
    /** indicates that an input operation failed to read the expected characters, or
    that an output operation failed to generate the desired characters */
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
    /** seek to end before each write */
    static const openmode app     = 1 << 0;
    /** open and seek to end immediately after opening */
    static const openmode ate     = 1 << 1;
    /** truncate an existing stream when opening */
    static const openmode trunc   = 1 << 2;
    /** open for input */
    static const openmode in      = 1 << 3;
    /** open for output */
    static const openmode out     = 1 << 4;
    /** perform input and output in binary mode (as opposed to text mode) */
    static const openmode binary  = 1 << 5;

    /// 27.4.2.1.5 Type ios_base::seekdir [ios::seekdir]
    enum seekdir
    { 
      /** request a seek (for subsequent input or output) relative to the beginning of the stream */
      beg,
      /** request a seek relative to the current end of the sequence */
      end,
      /** request a seek relative to the current position within the sequence */
      cur
    };

    /// 27.4.2.1.6 Class ios_base::Init [ios::Init]
    class Init
    {
      public:
        Init();
        ~Init();
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
    /// not implemented as stdio is
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
    ios_base(const ios_base&) __deleted;
    ios_base& operator=(const ios_base&) __deleted;
};

/// 27.4.3 Class template fpos [fpos]
template <class stateT>
class fpos
{
  public:

    fpos(streamoff off) : offset(off), state_(0) {}

    ///\name 27.4.3.1 fpos Members [fpos.members]

    stateT state() const { return state_; }
    void state(stateT s) { state_ = s; }

    ///@}

    operator streamoff() const { return offset; }

  private:
    //file_t    filepos;
    streamoff offset;
    stateT    state_;
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

    basic_ios& copyfmt(const basic_ios& x)
    {
      if(this != &x){
        prec = x.prec;
        wide = x.wide;
        fmtfl = x.fmtfl;
        fillc = x.fillc;
        tiestr = x.tiestr;

      #if STLX__CONFORMING_LOCALE
        loc = x.loc;
      #endif

        /*
        If any newly stored pointer values in *this point at objects stored outside the object rhs, and
        those objects are destroyed when rhs is destroyed, the newly stored pointer values are altered to
        point at newly constructed copies of the objects.
        */

        exceptions(x.exceptmask);
      }
      return *this;
    }

  #ifdef NTL__CXX_RV
    void move(basic_ios&& x)
    {
      swap(x);
    }
    void swap(basic_ios&& x)
  #else
    void swap(basic_ios& x)
  #endif
    {
      if(this == &x)
        return;
      // ios_base
      std::swap(prec, x.prec);
      std::swap(wide, x.wide);
      std::swap(fmtfl, x.fmtfl);
      std::swap(state, x.state);
      std::swap(exceptmask, x.exceptmask);
    #if STLX__CONFORMING_LOCALE
      std::swap(loc, x.loc);
    #endif

      // basic_ios
      std::swap(tiestr,x.tiestr);
      std::swap(fillc,x.fillc);
    }

    ///\name 27.4.4.3 basic_ios flags functions [iostate.flags]

    operator __::explicit_bool_type() const { return __::explicit_bool(!fail()); }
    bool operator!() const { return fail(); }

    iostate rdstate() const { return state; }

    __forceinline
    void clear(iostate state = goodbit)// throw(failure)
    {
      if ( !rdbuf() ) state |= badbit;
      const iostate fail = static_cast<iostate>((this->state = state) & exceptmask);
      if ( fail ) __ntl_throw (failure(__get_iostate_c_str(fail)));
    }

    __forceinline
    void setstate(iostate state)
    {
      if ( state != goodbit ) clear(static_cast<iostate>(rdstate() | state));
    }

    bool good() const { return (rdstate() == goodbit); }
    bool eof() const  { return (rdstate() & eofbit) != 0; }
    bool fail() const { return (rdstate() & (failbit | badbit)) != 0; }
    bool bad() const  { return (rdstate() & badbit) != 0; }

    iostate exceptions() const { return exceptmask; }

    void exceptions(iostate exm)
    {
      exceptmask = exm;
      clear(rdstate());
    }

    ///@}

  ///////////////////////////////////////////////////////////////////////////
  private:

    basic_ios(const basic_ios&) __deleted;
    basic_ios& operator=(const basic_ios&) __deleted;

    basic_streambuf<charT, traits>* sb;
    basic_ostream<charT, traits>*   tiestr;

    char_type fillc;
};

/**\addtogroup  lib_std_ios_manip ****** 27.4.5 ios_base manipulators *****
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
