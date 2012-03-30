/**\file*********************************************************************
 *                                                                     \brief
 *  22 Localization library [localization]
 *
 ****************************************************************************
 */

#ifdef STLX__CONFORMING_LOCALE
  #if STLX__CONFORMING_LOCALE
  #error no conforming locales support yet
  #endif
#else
#define STLX__CONFORMING_LOCALE 0
#endif

#ifndef NTL__STLX_LOCALE
#define NTL__STLX_LOCALE
#pragma once

#if STLX__CONFORMING_LOCALE
#define _NTL_LOC_VIRTUAL virtual
#else
#define _NTL_LOC_VIRTUAL
#endif

#ifndef NTL__STLX_STDEXCEPT_FWD
#include "stdexcept_fwd.hxx"
#endif

#include "cwctype.hxx"

#include "../nt/string.hxx"
#include "cstdlib.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820)// X bytes padding added... (class ctype<char>)
#ifdef __ICL
#pragma warning(disable:444)
#pragma warning(disable:2259) // [remark] non-pointer conversion from
#endif
#endif

namespace std {
  /** \addtogroup lib_locale 22 Localization library [localization]
   *@{*/

  /** \addtogroup lib_locales 22.1 Locales [locales]
   *@{*/

  namespace __
  {
    struct facets;
  }

/// 22.1.1 Class locale [locale]
class locale
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    ///\name 22.1.1.1 locale types [locale.types]

    /// 22.1.1.1.1 Type locale::category [locale.category]
    typedef int category;
    static const category
      none = 0,         collate = 0x01,   ctype = 0x02,
      monetary = 0x04,  numeric = 0x08,   time = 0x10,    messages = 0x20,
      all = collate | ctype | monetary | numeric | time | messages;

    /// 22.1.1.1.2 Class locale::facet [locale.facet]
    class facet
    {
      protected:
        explicit facet(size_t /*refs*/ = 0) {}
        _NTL_LOC_VIRTUAL ~facet() {}
      private:
        facet(const facet&) __deleted;
        void operator=(const facet&) __deleted;
    };

    /// 22.1.1.1.3 Class locale::id [locale.id]
    class id
    {
      #if STLX__CONFORMING_LOCALE
      public:
        id();
      private:
        void operator=(const id&) __deleted;
        id(const id&) __deleted;
      #endif
    };

    ///\name 22.1.1.2 locale constructors and destructor [locale.cons]

    locale() __ntl_nothrow {/**/}
    locale(const locale& /*other*/) __ntl_nothrow {/**/}
    const locale& operator=(const locale& other) __ntl_nothrow;

    explicit locale(const char* std_name) __ntl_throws(runtime_error);
    locale(const locale& other, const char* std_name, category) __ntl_throws(runtime_error);

    template <class Facet> locale(const locale& other, Facet* f);
    locale(const locale& other, const locale& one, category);

    ~locale() __ntl_nothrow {} // non-virtual

    ///\name 22.1.1.3 locale members [locale.members]

    template <class Facet> locale combine(const locale& other) const;
    basic_string<char> name() const;

    ///\name 22.1.1.4 locale operators [locale.operators]

    bool operator==(const locale& other) const;
    bool operator!=(const locale& other) const;

    template <class charT, class Traits, class Allocator>
    bool operator()(const basic_string<charT, Traits, Allocator>& s1,
                    const basic_string<charT, Traits, Allocator>& s2) const
    {
      return use_facet<collate<charT> >(*this).compare
          (s1.data(), s1.data()+s1.size(), s2.data(), s2.data()+s2.size()) < 0;
    }

    ///\name 22.1.1.5 locale static members [locale.statics]

    static locale global(const locale&);
    static const locale& classic()
    {
      static const locale loc;
      return loc;
    }
    ///\}

};// class locale

///\name 22.1.2 locale globals [locale.global.templates]

/// 1 Requires: Facet is a facet class whose definition contains the public
///   static member id as defined in 22.1.1.1.2.
/// 2 Returns: a reference to the corresponding facet of loc, if present.
template <class Facet> const Facet& use_facet(const locale&);

/// 5 Returns: true if the facet requested is present in loc; otherwise false.
template <class Facet> bool has_facet(const locale&) __ntl_nothrow;

///\name 22.1.3 Convenience interfaces [locale.convenience]

///\name 22.1.3.1 Character classification [classification]

/** Returns true if character is a space */
template <class charT> inline bool isspace(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::space, c);
}

/** Returns true if character is printable */
template <class charT> inline bool isprint(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::print, c);
}

/** Returns true if character is control */
template <class charT> inline bool iscntrl(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::cntrl, c);
}

/** Returns true if character is an upper */
template <class charT> inline bool isupper(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::upper, c);
}

/** Returns true if character is a lower */
template <class charT> inline bool islower(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::lower, c);
}

/** Returns true if character is a letter */
template <class charT> inline bool isalpha(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::alpha, c);
}

/** Returns true if character is a digit */
template <class charT> inline bool isdigit(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::digit, c);
}

/** Returns true if character is punctuation character */
template <class charT> inline bool ispunct(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::punct, c);
}

/** Returns true if character is hexadecimal digit */
template <class charT> inline bool isxdigit(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::xdigit, c);
}

/** Returns true if character is alphanumeric */
template <class charT> inline bool isalnum(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::alnum, c);
}

/** Returns true if character is a graphic character, excluding space */
template <class charT> inline bool isgraph(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::graph, c);
}

/** Returns true if character is a blank character, excluding space */
template <class charT> inline bool isblank(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::blank, c);
}
///\}

/**\defgroup lib_conversions 22.1.3.2 Conversions [conversions]
 *@{*/

///\name 22.1.3.21 Character conversions [conversions.character]

/** Converts a character to upper case */
template <class charT>
inline charT toupper(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).toupper(c);
}

/** Converts a character to lower case */
template <class charT>
inline charT tolower(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).tolower(c);
}

/** Class codecvt base that defines a result enumeration */
class codecvt_base 
{
public:
  /** do_in/do_out result values */
  enum result { 
    /** completed the conversion */
    ok, 
    /** not all source characters converted 
      or space for more than <tt>to_limit-to</tt> destination elements was needed to terminate a sequence given the value of specified state (for unshift())*/
    partial, 
    /** encountered a character in [from,from_end) that it could not convert or an unspecified error has occurred (for unshift()) */
    error, 
    /** internT and externT are the same type, and input sequence is identical to converted sequence or no termination is needed for this \c state_type (for unshift()) */
    noconv 
  };
};

///\name 22.1.3.2.2 string conversions [conversions.string]

/**
 *	@brief Class template wstring_convert
 *
 *  Class template wstring_convert performs conversions between a wide %string and a byte %string. It lets you
 *  specify a code conversion facet (like class template codecvt) to perform the conversions, without affecting
 *  any streams or locales.
 *
 *  The class template describes an object that controls conversions between wide %string objects of class
 *  <tt>std::basic_string<Elem></tt> and byte %string objects of class <tt>std::basic_string<char></tt> (also known as
 *  std::string). The class template defines the types \c wide_string and \c byte_string as synonyms for these
 *  two types. Conversion between a sequence of \e Elem values (stored in a \c wide_string object) and multi-byte sequences 
 *  (stored in a \c byte_string object) is performed by an object of class <tt>Codecvt<Elem, char, std::mbstate_t></tt>, 
 *  which meets the requirements of the standard code-conversion facet <tt>std::codecvt<Elem, char, std::mbstate_t></tt>.
 *  @sa codecvt
 **/
template<class Codecvt, class Elem = wchar_t,
        class Wide_alloc = std::allocator<Elem>,
        class Byte_alloc = std::allocator<char> 
        >
class wstring_convert
{
  static const size_t max_wide_size = 8;
public:
  typedef basic_string<char, char_traits<char>, Byte_alloc> byte_string;
  typedef basic_string<Elem, char_traits<Elem>, Wide_alloc> wide_string;
  typedef typename Codecvt::state_type                      state_type;
  typedef typename wide_string::traits_type::int_type       int_type;

  wstring_convert(Codecvt *pcvt = new Codecvt)
    :cvt(pcvt), cvtstate(), count(0)
  {}
  wstring_convert(Codecvt *pcvt, state_type state)
    :cvt(pcvt), state(state), count(0)
  {}
  wstring_convert(const byte_string& byte_err, const wide_string& wide_err = wide_string())
    :cvt(new Codecvt), cvtstate(), serr(byte_err), werr(wide_err), count(0)
  {}
  ~wstring_convert()
  {
    delete cvt;
  }

  /** convert the single-element sequence byte to a wide %string */
  wide_string from_bytes(char one_byte) __ntl_throws(range_error)
  {
    assert(cvt);
    const char bytes[1] = {one_byte}, *bnext;
    Elem chars[1], *wnext;
    codecvt_base::result re = cvt->in(cvtstate, bytes, bytes+_countof(bytes), bnext, chars, chars+_countof(chars), wnext);
    count = wnext - chars;
    if(re == codecvt_base::error){
      if(werr.empty())
        __ntl_throw(range_error("conversion from bytes failed"));
      return werr;
    }
    assert(re == codecvt_base::ok);
    return wide_string(chars, count);
  }
  /** convert the null-terminated sequence beginning at \e ptr to a wide %string. */
  wide_string from_bytes(const char *ptr) __ntl_throws(range_error)
  {
    return from_bytes(ptr, ptr ? strlen(ptr) : 0);
  }
  /** convert the sequence stored in \e str to a wide %string */
  wide_string from_bytes(const byte_string& str) __ntl_throws(range_error)
  {
    return from_bytes(str.c_str(), str.length());
  }
  /** convert the sequence defined by the range <tt>[first,last)</tt> to a wide %string */
  wide_string from_bytes(const char *first, const char *last) __ntl_throws(range_error)
  {
    return from_bytes(first, last-first);
  }

  /** convert the single-element sequence \e wchar to a byte %string */
  byte_string to_bytes(Elem wchar) __ntl_throws(range_error)
  {
    char bytes[max_wide_size], *bnext;
    const Elem chars[1] = {wchar}, *wnext;
    assert(cvt);
    codecvt_base::result re = cvt->out(cvtstate, chars, chars+_countof(chars), wnext, bytes, bytes+_countof(bytes), bnext);
    count = bnext - bytes;
    if(re == codecvt_base::error){
      if(serr.empty())
        __ntl_throw(range_error("conversion to bytes failed"));
      return serr;
    }
    assert(re == codecvt_base::ok);
    return byte_string(bytes, count);
  }
  /** convert the null-terminated sequence beginning at \e wptr to a byte %string */
  byte_string to_bytes(const Elem *wptr) __ntl_throws(range_error)
  {
    return to_bytes(wptr, wptr ? wcslen(wptr) : 0);
  }
  /** convert the sequence stored in \e wstr to a byte %string */
  byte_string to_bytes(const wide_string& wstr) __ntl_throws(range_error)
  {
    return to_bytes(wstr.c_str(), wstr.length());
  }
  /** convert the sequence defined by the range <tt>[first,last)</tt> to a byte %string */
  byte_string to_bytes(const Elem *first, const Elem *last) __ntl_throws(range_error)
  {
    return to_bytes(first, last-first);
  }

  size_t converted() const { return count; }
  state_type state() const { return cvtstate; }

private:
  wide_string from_bytes(const char *ptr, size_t len) __ntl_throws(range_error)
  {
    wide_string ws(len, Elem(0)); // assume that wide string can't be large than multibyte
    if(len == 0)
      return ws;
    const char* bnext; Elem* wnext;
    assert(cvt);
    codecvt_base::result re = cvt->in(cvtstate, ptr, ptr+len, bnext, ws.begin(), ws.end(), wnext);
    count = wnext - ws.begin();
    if(re == codecvt_base::error){
      if(werr.empty())
        __ntl_throw(range_error("conversion from bytes failed"));
      return werr;
    }
    assert(re == codecvt_base::ok);
    ws.resize(count);
    return move(ws);
  }
  byte_string to_bytes(const Elem *wptr, size_t len) __ntl_throws(range_error)
  {
    byte_string bs(len*max_wide_size, '\0');
    if(len == 0)
      return bs;
    char* bnext; const Elem *wnext;
    assert(cvt);
    codecvt_base::result re = cvt->out(cvtstate, wptr, wptr+len, wnext, bs.begin(), bs.end(), bnext);
    count = bnext - bs.begin();
    if(re == codecvt_base::error){
      if(serr.empty())
        __ntl_throw(range_error("conversion to bytes failed"));
      return serr;
    }
    assert(re == codecvt_base::ok);
    bs.resize(count);
    return move(bs);
  }

private:
  // a byte string to display on errors
  byte_string serr;
  // a wide string to display on errors
  wide_string werr;
  // a pointer to the allocated conversion object
  Codecvt *cvt;
  // a conversion state object
  state_type cvtstate;
  // a conversion count
  size_t count;
};


///\}
/** @} lib_conversions */
/** @} lib_locales */

/** \addtogroup lib_locale_categories   22.2 Standard locale categories [locale.categories]
 *@{*/

// ensure GetStringTypeX API is usable
#if  defined C1_UPPER && defined C1_LOWER && defined C1_DIGIT \
  && defined C1_SPACE && defined C1_PUNCT && defined C1_CNTRL \
  && defined C1_BLANK && defined C1_XDIGIT \
  && defined C1_ALPHA && C1_DEFINED
static const int16_t __ct_upper = (int16_t)C1_UPPER;
static const int16_t __ct_lower = (int16_t)C1_LOWER;
static const int16_t __ct_digit = (int16_t)C1_DIGIT;
static const int16_t __ct_space = (int16_t)C1_SPACE;
static const int16_t __ct_punct = (int16_t)C1_PUNCT;
static const int16_t __ct_cntrl = (int16_t)C1_CNTRL;
static const int16_t __ct_blank = (int16_t)C1_BLANK;
static const int16_t __ct_xdigit = (int16_t)C1_XDIGIT;
static const int16_t __ct_alpha = (int16_t)C1_ALPHA;
#else
static const int16_t __ct_upper = 0x0001;
static const int16_t __ct_lower = 0x0002;
static const int16_t __ct_digit = 0x0004;
static const int16_t __ct_space = 0x0008;
static const int16_t __ct_punct = 0x0010;
static const int16_t __ct_cntrl = 0x0020;
static const int16_t __ct_blank = 0x0040;
static const int16_t __ct_xdigit= 0x0080;
static const int16_t __ct_alpha = 0x0100;
#endif

/** \defgroup lib_locale_ctype 22.2.1 The ctype category [category.ctype]
 *@{*/

/**
 *	Class ctype_base that declares characters class mask values
 **/
class ctype_base
{
  public:
    /// character class mask type
    typedef int16_t mask;
    static const mask
      /** is lower */
      lower = __ct_lower,
      /** is upper */
      upper = __ct_upper,
      /** is letter */
      alpha = lower|upper|__ct_alpha,
      /** is control character */
      cntrl = __ct_cntrl,
      /** is digit */
      digit = __ct_digit,
      /** is hexadecimal digit */
      xdigit = __ct_xdigit,
      /** is whitespace */
      space = __ct_space|__ct_blank,
      /** is punctuation */
      punct = __ct_punct,
      /** is printable, including space */
      print = alpha|digit|punct|__ct_blank,// __ct_space?
      /** is alphanumeric */
      alnum = alpha|digit,
      /** is graphic character, excluding space */
      graph = alnum|punct,
      /** is blank character */
      blank = __ct_blank;
};

/**
 *	@brief 22.2.1.1 Class template ctype [locale.ctype]
 *  @details Class ctype encapsulates the C library \c \<cctype\> features. istream members are required to use ctype<>
 *  for character classing during input parsing.
 **/
template <class charT>
class ctype : public locale::facet, public ctype_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT char_type;

    explicit ctype(size_t refs = 0)
      :facet(refs)
    {}

    ///\name 22.2.1.1.1 ctype members [locale.ctype.members]

    /** Classifies a character. */
    bool is(mask m, charT c) const { return do_is(m, c); }

    /** Classifies a character or sequence of characters. @return \a high */
    const charT* is(const charT* low, const charT* high, mask* vec) const
    {
      return do_is(low, high, vec);
    }

    /** Locates a character in a buffer that conforms to a classification \a m. */
    const charT* scan_is(mask m, const charT* low, const charT* high) const
    {
      return do_scan_is(m, low, high);
    }

    /** Locates a character in a buffer that fails to conform to a classification \a m. */
    const charT* scan_not(mask m, const charT* low, const charT* high) const
    {
      return do_scan_not(m, low, high);
    }

    /** Converts a character to upper case. */
    charT toupper(charT c) const { return do_toupper(c); }

    /** Replaces each character \c *p in the range <tt>[low,high)</tt> for which a corresponding upper-case character exists, with that character. */
    const charT* toupper(charT* low, const charT* high) const
    {
      return do_toupper(low, high);
    }

    /** Converts a character to lower case. */
    charT tolower(charT c) const { return do_tolower(c); }

    /** Replaces each character \c *p in the range <tt>[low,high)</tt> and for which a corresponding lower-case character exists, with that character. */
    const charT* tolower(charT* low, const charT* high) const
    {
      return do_tolower(low, high);
    }

    /** Applies the simplest reasonable transformation from a char value to the corresponding \c charT value. */
    charT widen(char c) const { return do_widen(c); }

    /** Applies the simplest reasonable transformation from a sequence of char values to the corresponding \c charT values. */
    const char* widen(const char* low, const char* high, charT* to) const
    {
      return do_widen(low, high, to);
    }

    /** Applies the simplest reasonable transformation from a \c charT value to the corresponding \c char value. */
    char narrow(charT c, char dfault) const { return do_narrow(c,dfault); }

    /** Applies the simplest reasonable transformation from a sequence of \c charT values to the corresponding \c char values. */
    const charT* narrow(const charT* low, const charT* high, char dfault, char* to) const
    {
      return do_narrow(low, high, dfault, to);
    }

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name 22.2.1.1.2 ctype virtual functions [locale.ctype.virtuals]

    ~ctype() //virtual
    {}

    _NTL_LOC_VIRTUAL bool do_is(mask m, charT c) const;
    _NTL_LOC_VIRTUAL const charT* do_is(const charT* low, const charT* high, mask* vec) const;
    _NTL_LOC_VIRTUAL const charT* do_scan_is(mask m, const charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL const charT* do_scan_not(mask m, const charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_toupper(charT) const;
    _NTL_LOC_VIRTUAL const charT* do_toupper(charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_tolower(charT) const;
    _NTL_LOC_VIRTUAL const charT* do_tolower(charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_widen(char) const;
    _NTL_LOC_VIRTUAL const char* do_widen(const char* low, const char* high, charT* dest) const;
    _NTL_LOC_VIRTUAL char do_narrow(charT, char dfault) const;
    _NTL_LOC_VIRTUAL const charT* do_narrow(const charT* low, const charT* high, char dfault, char* dest) const;
    ///\}
};// class ctype
#ifdef __ICL
#pragma warning(disable:444)
#endif

/// 22.2.1.2 Class template ctype_byname [locale.ctype.byname]
template <class charT>
class ctype_byname : public ctype<charT>
{
  public:
    typedef ctype<charT>::mask mask;
    explicit ctype_byname(const char*, size_t refs = 0);
  protected:
    ~ctype_byname(); // virtual
    _NTL_LOC_VIRTUAL bool do_is(mask m, charT c) const;
    _NTL_LOC_VIRTUAL const charT* do_is(const charT* low, const charT* high, mask* vec) const;
    _NTL_LOC_VIRTUAL const charT* do_scan_is(mask m, const charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL const charT* do_scan_not(mask m, const charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_toupper(charT) const;
    _NTL_LOC_VIRTUAL const charT* do_toupper(charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_tolower(charT) const;
    _NTL_LOC_VIRTUAL const charT* do_tolower(charT* low, const charT* high) const;
    _NTL_LOC_VIRTUAL charT do_widen(char) const;
    _NTL_LOC_VIRTUAL const char* do_widen(const char* low, const char* high, charT* dest) const;
    _NTL_LOC_VIRTUAL char do_narrow(charT, char dfault) const;
    _NTL_LOC_VIRTUAL const charT* do_narrow(const charT* low, const charT* high, char dfault, char* dest) const;
};

#ifdef __ICL
#pragma warning(disable:444)
#endif
/**
 *	@brief 22.2.1.3 ctype<char> specialization [facet.ctype.special]
 *  @details A specialization ctype<char> is provided so that the member functions on type \c char can be implemented inline.
 *  The implementation-defined value of member \c table_size is at least 256.
 **/
template <> class ctype<char>
: public locale::facet, public ctype_base
{
  template <class Facet> friend const Facet& use_facet(const locale&);
  ///////////////////////////////////////////////////////////////////////////
  public:
    typedef char char_type;

    explicit ctype(const mask* tab = 0, bool del = false, size_t refs = 0)
    : locale::facet(refs), tab(tab ? tab : classic_table()), del(del) {}

    /** Classifies a character. */
    bool is(mask m, char c) const
    {
      return (tab[static_cast<unsigned char>(c)] & m) != 0;
    }

    /** Classifies a character or sequence of characters. @return \a high */
    const char* is(const char* low, const char* high, mask* vec) const
    {
      while ( low != high )
        *vec++ = tab[static_cast<unsigned char>(*low++)];
      return high;
    }

    /** Locates a character in a buffer that conforms to a classification \a m. */
    const char* scan_is(mask m, const char* low, const char* high) const
    {
      while ( low != high && !(tab[static_cast<unsigned char>(*low)] & m) )
          ++low;
      return low;
    }

    /** Locates a character in a buffer that fails to conform to a classification \a m. */
    const char* scan_not(mask m, const char* low, const char* high) const
    {
      while ( low != high && (tab[static_cast<unsigned char>(*low)] & m) )
          ++low;
      return low;
    }

    /** Converts a character to upper case. */
    char toupper(char c) const { return do_toupper(c); }

    /** Replaces each character \c *p in the range <tt>[low,high)</tt> for which a corresponding upper-case character exists, with that character. */
    const char* toupper(char* low, const char* high) const { return do_toupper(low, high); }

    /** Converts a character to lower case. */
    char tolower(char c) const { return do_tolower(c); }

    /** Replaces each character \c *p in the range <tt>[low,high)</tt> and for which a corresponding lower-case character exists, with that character. */
    const char* tolower(char* low, const char* high) const {return do_tolower(low, high); }

    /** Applies the simplest reasonable transformation from a char value to the corresponding \c charT value. */
    char widen(char c) const { return do_widen(c); }

    /** Applies the simplest reasonable transformation from a sequence of char values to the corresponding \c charT values. */
    const char* widen(const char* low, const char* high, char* to) const { return do_widen(low, high, to); }

    /** Applies the simplest reasonable transformation from a \c charT value to the corresponding \c char value. */
    char narrow(char c, char dfault) const { return do_narrow(c, dfault); }

    /** Applies the simplest reasonable transformation from a sequence of \c charT values to the corresponding \c char values. */
    const char* narrow(const char * low, const char * high, char dfault, char * to) const
    {
      return do_narrow(low, high, dfault, to);
    }

    static locale::id id;

    static const size_t table_size = 1 << CHAR_BIT;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ~ctype()
    {
      // tab pointer is always initialized by the constructor
      if ( /*tab &&*/ del )
        delete[] tab;
    }

    const mask* table() const __ntl_nothrow { return tab; }

    /** Returns a pointer to the initial element of an array of size table_size which represents the classifications of characters in the "C" locale */
    static const mask* classic_table() __ntl_nothrow
    {
      static const mask ascii_mask[256] =
      {
        0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x068, 0x028, 0x028, 0x028, 0x028, 0x020, 0x020,
        0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
        0x048, 0x010, 0x010, 0x010, 0x000, 0x010, 0x010, 0x010, 0x010, 0x010, 0x010, 0x000, 0x010, 0x010, 0x010, 0x010,
        0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x084, 0x010, 0x010, 0x000, 0x000, 0x000, 0x010,
        0x010, 0x181, 0x181, 0x181, 0x181, 0x181, 0x181, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101,
        0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x010, 0x010, 0x010, 0x000, 0x010,
        0x000, 0x182, 0x182, 0x182, 0x182, 0x182, 0x182, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102,
        0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x010, 0x000, 0x010, 0x000, 0x020,
        0x020, 0x020, 0x020, 0x020, 0x020, 0x028, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
        0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020, 0x020,
        0x048, 0x010, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x102, 0x010, 0x000, 0x020, 0x000, 0x000,
        0x000, 0x000, 0x000, 0x000, 0x000, 0x102, 0x000, 0x010, 0x000, 0x000, 0x102, 0x010, 0x000, 0x000, 0x000, 0x010,
        0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101,
        0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x000, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x101, 0x102,
        0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102,
        0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x000, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102, 0x102
      };
      return ascii_mask;
    }

    ///\name 22.2.1.3.4 ctype<char> virtual functions [facet.ctype.char.virtuals]

    _NTL_LOC_VIRTUAL char do_toupper(char c) const
    {
      return is(lower, c) ? c + 'A'-'a' : c;
    }

    _NTL_LOC_VIRTUAL const char* do_toupper(char* low, const char* high) const
    {
      while ( low != high )
        *low++ = do_toupper(*low);
      return high;
    }

    _NTL_LOC_VIRTUAL char do_tolower(char c) const
    {
      return is(upper, c) ? c + 'a'-'A' : c;
    }

    _NTL_LOC_VIRTUAL const char* do_tolower(char* low, const char* high) const
    {
      while ( low != high )
        *low++ = do_tolower(*low);
      return high;
    }

    _NTL_LOC_VIRTUAL char do_widen(char c) const { return c; }

    _NTL_LOC_VIRTUAL const char* do_widen(const char* low, const char* high, char* to) const
    {
      //while ( low != high )
      //  *to++ = *low++;
      memcpy(to, low, high-low);
      return high;
    }

    _NTL_LOC_VIRTUAL char do_narrow(char c, char /*dfault*/) const { return c; }

    _NTL_LOC_VIRTUAL const char* do_narrow(const char * low,
                       const char * high,
                       char         /*dfault*/,
                       char *       to) const
    {
      //while ( low != high )
      //  *to++ = *low++;
      memcpy(to, low, high-low);
      return high;
    }

    ///\}

  private:

    ctype(const ctype&) __deleted;
    void operator=(const ctype&) __deleted;

    const mask * const  tab;
    const bool          del;

};// class ctype<char>
#ifdef __ICL
#pragma warning(default:444)
#endif

/// 22.2.1.4 Class ctype_byname<char> [lib.locale.ctype.byname.special]
template <> class ctype_byname<char>
: public ctype<char>
{
  public:
    explicit ctype_byname(const char*, size_t refs = 0);
  protected:
    _NTL_LOC_VIRTUAL ~ctype_byname(); // virtual
    _NTL_LOC_VIRTUAL char do_toupper(char c) const;
    _NTL_LOC_VIRTUAL const char* do_toupper(char* low, const char* high) const;
    _NTL_LOC_VIRTUAL char do_tolower(char c) const;
    _NTL_LOC_VIRTUAL const char* do_tolower(char* low, const char* high) const;
    _NTL_LOC_VIRTUAL char do_widen(char c) const;
    _NTL_LOC_VIRTUAL const char* do_widen(const char* low, const char* high, char* to) const;
    _NTL_LOC_VIRTUAL char do_narrow(char c, char dfault) const;
    _NTL_LOC_VIRTUAL const char* do_narrow(const char* low, const char* high, char dfault, char* to) const;
  private:
    ctype_byname(const ctype_byname&) __deleted;
    void operator=(const ctype_byname&) __deleted;
};

/**
 *	@brief 22.2.1.3 ctype<wchar_t> specialization [facet.ctype.special]
 *  @details A specialization ctype<wchar_t> is provided so that the member functions on type \c wchar_t can be implemented inline.
 *  The implementation-defined value of member \c table_size is 65536.
 **/
template <> class ctype<wchar_t>
: public locale::facet, public ctype_base
{
  template <class Facet> friend const Facet& use_facet(const locale&);
  ///////////////////////////////////////////////////////////////////////////
public:

  typedef wchar_t char_type;

  explicit ctype(const mask* table, size_t refs = 0)
    : locale::facet(refs), tab(table)
  {}

  /** Classifies a character. */
  bool is(mask m, char_type c) const
  {
    return (tab[c] & m) != 0;
  }

  /** Classifies a character or sequence of characters. @return \a high */
  const char_type* is(const char_type* low, const char_type* high, mask* vec) const
  {
    while ( low != high )
      *vec++ = tab[*low++];
    return high;
  }

  /** Locates a character in a buffer that conforms to a classification \a m. */
  const char_type* scan_is(mask m, const char_type* low, const char_type* high) const
  {
    while ( low != high && !(tab[*low] & m) )
      ++low;
    return low;
  }

  /** Locates a character in a buffer that fails to conform to a classification \a m. */
  const char_type* scan_not(mask m, const char_type* low, const char_type* high) const
  {
    while ( low != high && (tab[*low] & m) )
      ++low;
    return low;
  }

  /** Converts a character to upper case. */
  char_type toupper(char_type c) const { return do_toupper(c); }

  /** Replaces each character \c *p in the range <tt>[low,high)</tt> for which a corresponding upper-case character exists, with that character. */
  const char_type* toupper(char_type* low, const char_type* high) const { return do_toupper(low, high); }

  /** Converts a character to lower case. */
  char_type tolower(char_type c) const { return do_tolower(c); }

  /** Replaces each character \c *p in the range <tt>[low,high)</tt> and for which a corresponding lower-case character exists, with that character. */
  const char_type* tolower(char_type* low, const char_type* high) const {return do_tolower(low, high); }

  /** Applies the simplest reasonable transformation from a char value to the corresponding \c charT value. */
  char_type widen(char c) const { return do_widen(c); }

  /** Applies the simplest reasonable transformation from a sequence of char values to the corresponding \c charT values. */
  const char* widen(const char* low, const char* high, char_type* to) const { return do_widen(low, high, to); }

  /** Applies the simplest reasonable transformation from a \c charT value to the corresponding \c char value. */
  char narrow(char_type c, char dfault) const { return do_narrow(c, dfault); }

  /** Applies the simplest reasonable transformation from a sequence of \c charT values to the corresponding \c char values. */
  const char_type* narrow(const char_type * low, const char_type * high, char dfault, char * to) const
  {
    return do_narrow(low, high, dfault, to);
  }

  static locale::id id;

  static const size_t table_size = WCHAR_MAX+1;

  ///////////////////////////////////////////////////////////////////////////
protected:
  ~ctype()
  {
  }

  ///\name 22.2.1.3.4 ctype<wchar_t> virtual functions [facet.ctype.char.virtuals]

  _NTL_LOC_VIRTUAL char_type do_toupper(char_type c) const
  {
    return is(lower, c) ? (c < 128 ? c + 'A'-'a' : ntl::nt::RtlUpcaseUnicodeChar(c)) : c;
  }

  _NTL_LOC_VIRTUAL const char_type* do_toupper(char_type* low, const char_type* high) const
  {
    while ( low != high )
      *low++ = do_toupper(*low);
    return high;
  }

  _NTL_LOC_VIRTUAL char_type do_tolower(char_type c) const
  {
    return is(upper, c) ? (c < 128 ? c + 'a'-'A' : ntl::nt::RtlDowncaseUnicodeChar(c)) : c;
  }

  _NTL_LOC_VIRTUAL const char_type* do_tolower(char_type* low, const char_type* high) const
  {
    while ( low != high )
      *low++ = do_tolower(*low);
    return high;
  }

  _NTL_LOC_VIRTUAL char_type do_widen(char c) const
  {
    return static_cast<unsigned char>(c) < 128 ? c : ntl::nt::widen(c);
  }

  _NTL_LOC_VIRTUAL const char* do_widen(const char* low, const char* high, char_type* to) const
  {
    while ( low != high )
      *to++ = do_widen(*low++);
    return high;
  }

  _NTL_LOC_VIRTUAL char do_narrow(char_type c, char dfault) const
  {
    char nc;
    return c < 128 ? static_cast<char>(c) : ntl::nt::success(ntl::nt::RtlUnicodeToMultiByteN(&nc, 1, nullptr, &c, 1)) ? nc : dfault;
  }

  _NTL_LOC_VIRTUAL const char_type* do_narrow(const char_type* low, const char_type* high, char dfault, char* to) const
  {
    while ( low != high )
      *to++ = do_narrow(*low++, dfault);
    return high;
  }
  ///\}

private:
private:
  const mask* const tab;

  ctype(const ctype&) __deleted;
  void operator=(const ctype&) __deleted;

};// class ctype<wchar_t>


/// 22.2.1.5 Class template codecvt [lib.locale.codecvt]

/**
 *	@brief 22.2.1.4 Class template codecvt [locale.codecvt]
 *  
 *  The class codecvt<internT,externT,stateT> is for use when converting from one codeset to another, such
 *  as from wide characters to multibyte characters or between wide character encodings such as Unicode and EUC.
 *
 *  @tparam stateT The stateT argument selects the %pair of codesets being mapped between.
 **/
template <class internT, class externT, class stateT>
class codecvt:
  public locale::facet,
  public codecvt_base
{
  public:
    typedef internT intern_type;
    typedef externT extern_type;
    typedef stateT  state_type;

    explicit codecvt(size_t refs = 0)
      :facet(refs)
    {}

    /**
     *	Translates characters in the source range [from,from_end), placing the results in sequential
     *  positions starting at destination \a to. Converts no more than (<tt>from_end-from</tt>) source elements, and
     *  stores no more than (<tt>to_limit-to</tt>) destination elements.
     **/
    result out(      stateT &   state,
               const internT *  from,
               const internT *  from_end,
               const internT *& from_next,
                     externT *  to,
                     externT *  to_limit,
                     externT *& to_next) const
    {
      return do_out(state,from,from_end,from_next,to,to_limit,to_next);
    }

    /**
     *	Places characters starting at \a to that should be appended to terminate a sequence when the
     *  current \c stateT is given by \a state.
     *  Stores no more than (<tt>to_limit-to</tt>) destination elements, and
     *  leaves the \a to_next pointer pointing one beyond the last element successfully stored.
     **/
    result unshift(stateT &   state,
                   externT *  to,
                   externT *  to_limit,
                   externT *& to_next) const
    {
      return do_unshift(state,to,to_limit,to_next);
    }

    /**
     *	Translates characters in the source range [from,from_end), placing the results in sequential
     *  positions starting at destination \a to. Converts no more than (<tt>from_end-from</tt>) source elements, and
     *  stores no more than (<tt>to_limit-to</tt>) destination elements.
     **/
    result in(      stateT &   state,
              const externT *  from,
              const externT *  from_end,
              const externT *& from_next,
                    internT *  to,
                    internT *  to_limit,
                    internT *& to_next) const
    {
      return do_in(state,from,from_end,from_next,to,to_limit,to_next);
    }

    /** Returns -1 if the encoding of the \a externT sequence is state-dependent;
        else the constant number of \a externT characters needed to produce an internal character;
        or 0 if this number is not a constant */
    int encoding() const __ntl_nothrow { return do_encoding(); }

    /** Returns \c true if \c do_in() and \c do_out() return \c noconv for all valid argument values. */
    bool always_noconv() const __ntl_nothrow { return do_always_noconv(); }

    /** The effect on the \a state argument is "as if" it called 
        <tt>do_in(state, from, from_end, from, to, to+max, to)</tt> for \a to pointing to a buffer of at least \a max elements. */
    int length(stateT& state, const externT* from, const externT* end, size_t max) const { return do_length(state, from, end, max); }

    /** Returns the maximum value that <tt>do_length(state, from, from_end, 1)</tt> 
      can return for any valid range <tt>[from, from_end)</tt> and \c stateT value \a state. */
    int max_length() const __ntl_nothrow { return do_max_length(); }

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:
    ~codecvt()
    {}
#if 0
    virtual result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const;

    virtual result do_unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const;

    virtual int do_max_length() const __ntl_nothrow;
#endif

};// codecvt

/// 22.2.1.6 Class template codecvt_byname [lib.locale.codecvt.byname]
template <class internT, class externT, class stateT>
class codecvt_byname : public codecvt<internT, externT, stateT>
{
  public:
    explicit codecvt_byname(const char*, size_t refs = 0);

  protected:

    using typename codecvt<internT, externT, stateT>::result;

    ~codecvt_byname(); // virtual

    virtual result do_out(stateT& state,
                          const internT* from,
                          const internT* from_end,
                          const internT*& from_next,
                          externT* to,
                          externT* to_limit,
                          externT*& to_next) const;

    virtual result do_in(stateT& state, const externT* from, const externT* from_end,
      const externT*& from_next, internT* to, internT* to_limit, internT*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(stateT&, const externT* from, const externT* end, size_t max) const;

    virtual result do_unshift(stateT& state, externT* to, externT* to_limit, externT*& to_next) const;

    virtual int do_max_length() const __ntl_nothrow;

};

///\name codecvt specializations

template<class charT>
class codecvt<charT,charT,mbstate_t>:
  public locale::facet,
  public codecvt_base
{
protected:
  ~codecvt(){}
public:
  typedef charT     intern_type;
  typedef charT     extern_type;
  typedef mbstate_t state_type;

  codecvt(size_t refs = 0)
    :facet(refs)
  {}

  result out(state_type&, const intern_type* from, const intern_type* /*from_end*/, const intern_type*& from_next, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    from_next = from,
      to_next = to;
    return noconv;
  }

  result unshift(state_type&, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    to_next = to;
    return noconv;
  }

  result in(state_type&, const extern_type* from, const extern_type* /*from_end*/, const extern_type*& from_next, intern_type* to, intern_type* /*to_limit*/, intern_type*& to_next) const
  {
    from_next = from,
      to_next = to;
    return noconv;
  }

  /** @return the constant number of externT characters needed to produce an internal character */
  int encoding() const __ntl_nothrow { return 1; }

  bool always_noconv() const __ntl_nothrow { return true; }

  int length(state_type&, const extern_type* from, const extern_type* end, size_t max) const { return static_cast<int>(min(max, static_cast<size_t>(end - from))); }

  int max_length() const __ntl_nothrow { return 1; }

  static locale::id id;
};

/**
 *	@brief codecvt<char, char, mbstate_t> implements a degenerate conversion; it does not convert at all.
 *  @sa codecvt
 **/
template<>
class codecvt<char,char,mbstate_t>:
  public locale::facet,
  public codecvt_base
{
public:
  typedef char      intern_type;
  typedef char      extern_type;
  typedef mbstate_t state_type;

  explicit codecvt(size_t refs = 0)
    :facet(refs)
  {}


  result out(state_type&, const intern_type* from, const intern_type* /*from_end*/, const intern_type*& from_next, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    from_next = from,
      to_next = to;
    return noconv;
  }

  result unshift(state_type&, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    to_next = to;
    return noconv;
  }

  result in(state_type&, const extern_type* from, const extern_type* /*from_end*/, const extern_type*& from_next, intern_type* to, intern_type* /*to_limit*/, intern_type*& to_next) const
  {
    from_next = from,
      to_next = to;
    return noconv;
  }

  /** @return the constant number of externT characters needed to produce an internal character */
  int encoding() const __ntl_nothrow { return 1; }

  bool always_noconv() const __ntl_nothrow { return true; }

  int length(state_type&, const extern_type* from, const extern_type* end, size_t max) const { return static_cast<int>(min(max, static_cast<size_t>(end - from))); }

  int max_length() const __ntl_nothrow { return 1; }

  static locale::id id;
};


/** The specialization codecvt<char16_t, char, mbstate_t> converts between the UTF-16 and UTF-8 encodings schemes */
template<>
class codecvt<char16_t,char,mbstate_t>:
  public locale::facet,
  public codecvt_base
{
  public:
    typedef char16_t  intern_type;
    typedef char      extern_type;
    typedef mbstate_t state_type;

    explicit codecvt(size_t refs = 0)
      :facet(refs)
    {}

    result out(state_type& state, const intern_type* from, const intern_type* from_end, const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
    {
      return do_out(state,from,from_end,from_next,to,to_limit,to_next);
    }

    result unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
    {
      return do_unshift(state,to,to_limit,to_next);
    }

    result in(state_type& state, const extern_type* from, const extern_type* from_end, const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const
    {
      return do_in(state,from,from_end,from_next,to,to_limit,to_next);
    }

    int encoding() const __ntl_nothrow { return do_encoding(); }

    bool always_noconv() const __ntl_nothrow { return do_always_noconv(); }

    int length(state_type& state, const extern_type* from, const extern_type* end, size_t max) const
    {
      return do_length(state, from, end, max);
    }

    int max_length() const __ntl_nothrow { return do_max_length(); }

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:
    ~codecvt()
    {}

    virtual result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
    {
      // wcstombs
      result r = ok;
      while(from < from_end && to < to_limit){
        size_t c = c16rtomb(to, *from, &state);
        if(c == static_cast<size_t>(-1)){
          r = error;
          break;
        }else if(c == 0){
          if(*from == 0)
            *to = 0;
          c = 1;
        }
        from++;
        to += c;
      }
      from_next = from,
        to_next = to;
      if(r == ok && from < from_end)
        r = partial;
      return r;
    }

    virtual result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const
    {
      // mbstowcs
      result r = ok;
      while(from < from_end && to < to_limit){
        size_t c = mbrtoc16(to, from, 1, &state);
        if(c == static_cast<size_t>(-1)){
          r = error;
          break;
        }else if(c == 0){
          if(*from == 0)
            *to = 0;
          c = 1;
        }
        from += c;
        to++;
      }
      from_next = from,
        to_next = to;
      if(r == ok && from < from_end)
        r = partial;
      return r;
    }

    virtual result do_unshift(state_type& /*state*/, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
    {
      to_next = to;
      return noconv;
    }

    virtual int do_encoding() const __ntl_nothrow { return 2; }

    virtual bool do_always_noconv() const __ntl_nothrow { return false; }

    virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const
    {
      return static_cast<int>(min(max,static_cast<size_t>(end-from)));
    }

    virtual int do_max_length() const __ntl_nothrow { return 1; }
};

template<>
class codecvt<char16_t,wchar_t,mbstate_t>:
  public codecvt<char16_t, char16_t,mbstate_t>
{
protected:
  ~codecvt(){}
public:
  codecvt(const codecvt&){}
  codecvt(size_t refs = 0)
    :codecvt<char16_t, char16_t,mbstate_t>(refs)
  {}

};

/** The specialization codecvt<char32_t, char, mbstate_t> converts between the UTF-32 and UTF-8 encodings schemes */
template<>
class codecvt<char32_t,char,mbstate_t>:
  public locale::facet,
  public codecvt_base
{

};


/**
 *	@brief codecvt<wchar_t,char,mbstate_t> converts between the native character sets for narrow and wide characters.
 *  @sa codecvt
 **/
template<>
class codecvt<wchar_t,char,mbstate_t>:
  public locale::facet,
  public codecvt_base
{
public:
  typedef wchar_t   intern_type;
  typedef char      extern_type;
  typedef mbstate_t state_type;

  explicit codecvt(size_t refs = 0)
    :facet(refs)
  {}

  /** conversion from wide string to the multibyte string */
  result out(state_type& state, const intern_type* from, const intern_type* from_end, const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
  {
    return do_out(state,from,from_end,from_next,to,to_limit,to_next);
  }

  result unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
  {
    return do_unshift(state,to,to_limit,to_next);
  }

  /** conversion from multibyte string to the wide string */
  result in(state_type& state, const extern_type* from, const extern_type* from_end, const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const
  {
    return do_in(state,from,from_end,from_next,to,to_limit,to_next);
  }

  /** @return the constant number of externT characters needed to produce an internal character */
  int encoding() const __ntl_nothrow { return do_encoding(); }

  bool always_noconv() const __ntl_nothrow { return do_always_noconv(); }

  /** calculates length of the multibyte string */
  int length(state_type& state, const extern_type* from, const extern_type* end, size_t max) const
  {
    return do_length(state, from, end, max);
  }

  int max_length() const __ntl_nothrow { return do_max_length(); }

  static locale::id id;

protected:
  virtual result do_out(state_type& /*state*/, const intern_type* from, const intern_type* from_end,
    const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const
  {
    // wcstombs
    result r = ok;
    while(from < from_end && to < to_limit){
      size_t c = wcstombs(to, from, 1);
      if(c == static_cast<size_t>(-1)){
        r = error;
        break;
      }else if(c == 0){
        if(*from == 0)
          *to = 0;
        c = 1;
      }
      from++;
      to += c;
    }
    from_next = from,
      to_next = to;
    if(r == ok && from < from_end)
      r = partial;
    return r;
  }

  virtual result do_in(state_type& /*state*/, const extern_type* from, const extern_type* from_end,
    const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const
  {
    // mbstowcs
    result r = ok;
    while(from < from_end && to < to_limit){
      size_t c = mbstowcs(to, from, 1);
      if(c == static_cast<size_t>(-1)){
        r = error;
        break;
      }else if(c == 0){
        if(*from == 0)
          *to = 0;
        c = 1;
      }
      from += c;
      to++;
    }
    from_next = from,
      to_next = to;
    if(r == ok && from < from_end)
      r = partial;
    return r;
  }

  virtual result do_unshift(state_type& /*state*/, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    to_next = to;
    return noconv;
  }

  virtual int do_encoding() const __ntl_nothrow
  {
    return 2;
  }

  virtual bool do_always_noconv() const __ntl_nothrow
  {
    return false;
  }

  virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const
  {
    return static_cast<int>(min(max,static_cast<size_t>(end-from)));
  }

  virtual int do_max_length() const __ntl_nothrow
  {
    return 1;
  }
};

///\}

/** @} lib_locale_ctype */
/** @} lib_locale_categories */
/** @} lib_locale */

}//namespace std

#include "ios.hxx"

namespace std {

/** \addtogroup lib_locale 22 Localization library [localization]
 *@{*/

  ///\name 22.1.3.2.3 Buffer conversions [conversions.buffer]

/**
 *	@brief Class template wbuffer_convert
 *
 *  Class template wbuffer_convert looks like a wide stream buffer, but performs all its I/O through an
 *  underlying byte stream buffer that you specify when you construct it. Like class template wstring_convert,
 *  it lets you specify a code conversion facet to perform the conversions, without affecting any streams or locales.
 *
 *  The class template describes a stream buffer that controls the transmission of elements of type \e Elem, whose
 *  character traits are described by the class \e Tr, to and from a byte stream buffer of type \c streambuf.
 *  Conversion between a sequence of \e Elem values and multibyte sequences is performed by an object of
 *  class <tt>Codecvt<Elem, char, std::mbstate_t></tt>, which shall meet the requirements of the standard code-conversion
 *  facet <tt>std::codecvt<Elem, char, std::mbstate_t></tt>.
 *  @sa codecvt
 **/
template<class Codecvt, class Elem = wchar_t, class Tr = char_traits<Elem> >
class wbuffer_convert:
  public basic_streambuf<Elem, Tr>
{
  typedef Tr traits;
  typedef Elem                      char_type;
  typedef typename traits::int_type int_type;
  typedef typename traits::pos_type pos_type;
  typedef typename traits::off_type off_type;
  typedef traits                    traits_type;
public:
  typedef typename Codecvt::state_type state_type;

  wbuffer_convert(std::streambuf *bytebuf = 0, Codecvt *pcvt = new Codecvt, state_type state = state_type())
    :sb(bytebuf), cvtptr(pcvt), cvtstate(state)
  {
    reset();
  }

  ~wbuffer_convert()
  {
    __ntl_try { sync(); }
    __ntl_catch(...){}
    if(buf.second)
      delete[] buf.first;
    delete cvtptr;
  }

  std::streambuf *rdbuf() const { return sb; }
  std::streambuf *rdbuf(streambuf *bytebuf)
  {
    std::swap(sb, bytebuf);
    reset();
    return bytebuf;
  }

  state_type state() const { return cvtstate; }

private:
  // 27.6.2.4 virtual functions:
  // 27.6.2.4.1 Locales:
  virtual void imbue(const locale& loc)
  {
    if(sb) sb->pubimbue(loc);
  }
  // 27.6.2.4.2 Buffer management and positioning:
  virtual basic_streambuf<char_type,traits>* setbuf(char_type* s, streamsize n)
  {
    if(sb && !s && !n){
      // disable buffering
      setp(0,0);
      sb->pubsetbuf(0,0);
    }
    return this;
  }

  //virtual pos_type seekoff(off_type off, ios_base::seekdir way, ios_base::openmode which = ios_base::in | ios_base::out);
  //virtual pos_type seekpos(pos_type sp, ios_base::openmode which = ios_base::in | ios_base::out);
  virtual int sync()
  {
    const streamsize pending = pptr()-pbase();
    if(pending){
      if(!write(pbase(), pending))
        return -1;
      reset();
      if(sb->pubsync() == -1)
        return -1;
    }
    return static_cast<int>(pending);
  }
  // 27.6.2.4.3 Get area:
  //virtual streamsize showmanyc();
  //virtual int_type underflow();
  //virtual int_type uflow();
  // 27.6.2.4.4 Putback:
  //virtual int_type pbackfail(int_type c = traits::eof());
  // 27.6.2.4.5 Put area:
  virtual int_type overflow (int_type c = traits::eof())
  {
    const int_type eof = traits_type::eof();
    const bool eofc = traits_type::eq_int_type(c, eof);
    if(!pbase() && eofc)
      return eof;

    bool ok = true;
    if(const streamsize pending = pptr()-pbase()) {
      // write buffer
      if(ok = write(pbase(), pending), ok)
        reset();
    }

    if(ok && !eofc){
      // write c
      const char_type cc = traits_type::to_char_type(c);
      ok = write(&cc, 1);
    }
    return eofc ? traits_type::not_eof(c) : c;
  }
private:
  void reset()
  {
    if(!sb){
      delete[] buf.first;
      buf.second = 0;
    }else if(!buf.second){
      buf.first = new char_type[buf.second = 1024*1]; // 1 KB
      if(!buf.first) buf.second = 0;
    }
    setp(buf.first, buf.first+buf.second); //-V104
  }
  bool write(const char_type* s, streamsize n)
  {
    return write(s, s+n); //-V104
  }
  bool write(const char_type* from, const char_type* to)
  {
    char sbuf[1024], *snext;
    const char_type  *wnext;
    streamsize n = to-from; //-V103
    do {
      const codecvt_base::result re = cvtptr->out(cvtstate, from, to, wnext, sbuf, sbuf+_countof(sbuf), snext);
      if(re == codecvt_base::error)
        break;
      assert(re != codecvt_base::noconv);

      const streamsize write_size = snext-sbuf; //-V103
      const streamsize scb = sb->sputn(sbuf, write_size);
      if(scb != write_size)
        break;

      n = to-wnext; //-V103
    }while(n > 0);
    return n == 0;
  }
private:
  // a pointer to its underlying byte stream buffer
  streambuf *sb;
  // a pointer to the allocated conversion object
  Codecvt *cvtptr;
  // a conversion state object
  state_type cvtstate;

  pair<char_type*, streamsize> buf;
};

/** \addtogroup lib_locale_categories 22.2 Standard locale categories [locale.categories]
 *@{*/

/** \defgroup lib_locale_numpunct 22.2.3 The numeric punctuation facet [facet.numpunct]
 *@{*/

/// 22.2.3.1 Class template numpunct [locale.numpunct]
/// 1 numpunct<> specifies numeric punctuation.
template <class charT>
class numpunct : public locale::facet
{
  public:

    typedef charT char_type;
    typedef basic_string<charT> string_type;

    explicit numpunct(size_t refs = 0)
      :facet(refs)
    {}

    ///\name 22.2.3.1.1 numpunct members [facet.numpunct.members]

    char_type decimal_point() const { return do_decimal_point(); }
    char_type thousands_sep() const { return do_thousands_sep(); }
    string grouping() const         { return do_grouping(); }
    string_type truename() const    { return do_truename(); }
    string_type falsename() const   { return do_falsename(); }

    static locale::id id;

  protected:
    ///\name 22.2.3.1.2 numpunct virtual functions [facet.numpunct.virtuals]
    friend struct __::facets;
    ~numpunct() // virtual
    {}

    /// Returns: A character for use as the decimal radix separator.
    /// The required specializations return ’.’ or L’.’.
    _NTL_LOC_VIRTUAL char_type do_decimal_point() const { return '.'; }

    /// 2 Returns: A character for use as the digit group separator.
    /// The required specializations return ’,’ or L’,’.
    _NTL_LOC_VIRTUAL char_type do_thousands_sep() const { return ','; }

    /// 3 Returns: A basic_string<char> vec used as a vector of integer values,
    ///   in which each element vec[i] represents the number of digits
    ///   in the group at position i, starting with position 0 as the rightmost
    ///   group. If vec.size() <= i, the number is the same as group (i-1);
    ///   if (i<0 || vec[i]<=0 || vec[i]==CHAR_MAX), the size of the digit group
    ///   is unlimited.
    /// 4 The required specializations return the empty string, indicating no grouping.
    _NTL_LOC_VIRTUAL string do_grouping() const { return ""; }

    /// 5 Returns: A string representing the name of the boolean value true.
    /// 6 In the base class implementation these names are "true" or L"true".
    _NTL_LOC_VIRTUAL string_type do_truename() const { return "true"; }

    /// 5 Returns: A string representing the name of the boolean value false.
    /// 6 In the base class implementation these names are "false" or L"false".
    _NTL_LOC_VIRTUAL string_type do_falsename() const { return "false"; }

    ///\}
};

template <class charT> class numpunct_byname;

template <>
class numpunct<wchar_t>:
  public locale::facet
{
public:
  typedef wchar_t                 char_type;
  typedef basic_string<char_type> string_type;

  explicit numpunct(size_t refs = 0)
    :facet(refs)
  {}

  ///\name 22.2.3.1.1 numpunct members [facet.numpunct.members]

  char_type decimal_point() const { return do_decimal_point(); }
  char_type thousands_sep() const { return do_thousands_sep(); }
  string grouping() const         { return do_grouping(); }
  string_type truename() const    { return do_truename(); }
  string_type falsename() const   { return do_falsename(); }

  static locale::id id;

protected:
  ///\name 22.2.3.1.2 numpunct virtual functions [facet.numpunct.virtuals]
  friend struct __::facets;
  ~numpunct() // virtual
  {}

  /// Returns: A character for use as the decimal radix separator.
  /// The required specializations return ’.’ or L’.’.
  _NTL_LOC_VIRTUAL char_type do_decimal_point() const { return L'.'; }

  /// 2 Returns: A character for use as the digit group separator.
  /// The required specializations return ’,’ or L’,’.
  _NTL_LOC_VIRTUAL char_type do_thousands_sep() const { return L','; }

  /// 3 Returns: A basic_string<char> vec used as a vector of integer values,
  ///   in which each element vec[i] represents the number of digits246)
  ///   in the group at position i, starting with position 0 as the rightmost
  ///   group. If vec.size() <= i, the number is the same as group (i-1);
  ///   if (i<0 || vec[i]<=0 || vec[i]==CHAR_MAX), the size of the digit group
  ///   is unlimited.
  /// 4 The required specializations return the empty string, indicating no grouping.
  _NTL_LOC_VIRTUAL string do_grouping() const { return ""; }

  /// 5 Returns: A string representing the name of the boolean value true.
  /// 6 In the base class implementation these names are "true" or L"true".
  _NTL_LOC_VIRTUAL string_type do_truename() const { return L"true"; }

  /// 5 Returns: A string representing the name of the boolean value false.
  /// 6 In the base class implementation these names are "false" or L"false".
  _NTL_LOC_VIRTUAL string_type do_falsename() const { return L"false"; }

  ///\}
};


/** @} lib_locale_numpunct */

/** \defgroup lib_locale_numeric 22.2.2 The numeric category [lib.category.numeric]
*@{*/

/// 22.2.2.1 Class template num_get [lib.locale.num.get]
template <class charT, class InputIterator = istreambuf_iterator<charT> >
class num_get : public locale::facet
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT         char_type;
    typedef InputIterator iter_type;

    explicit num_get(size_t refs = 0)
      :facet(refs)
    {}
    ///\name 22.4.2.1.1 num_get members [facet.num.get.members]
    /// 1 Returns: do_get(in, end, f, err, v).
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, bool& v) const              { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long& v) const              { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long long& v) const         { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned short& v) const    { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned int& v) const      { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned long& v) const     { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned long long& v) const{ return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, float& v) const             { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, double& v) const            { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long double& v) const       { return do_get(in,end,f,err,v); }
    iter_type get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, void*& v) const             { return do_get(in,end,f,err,v); }
    ///\}

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ~num_get() //virtual
    {}

    ///\name 22.4.2.1.2 num_get virtual functions [facet.num.get.virtuals]
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, bool& v) const
    {
      if(in == end){
        err |= ios_base::eofbit;
        return in;
      }

      if(!(str.flags() & ios_base::boolalpha)){

        // integral bool
        unsigned value = get_int<unsigned long, signed long>(in,end,str,err, numeric_limits<bool>::__max, numeric_limits<bool>::__min);
        v = value != 0;
        if(value > 1)
          err |= ios_base::failbit;
      }else{
        // textual bool
        v = false;
        err = ios_base::goodbit;
        const numpunct<char_type>& np = use_facet< numpunct<char_type> >(str.getloc());
        const numpunct<char_type>::string_type t = np.truename(), f = np.falsename();
        size_t i = 0, e1 = t.length(), e2 = f.length();
        unsigned nt = 0, nf = 0; // not true, not false
        do{
          const char_type c = *in;
          if(in == end)
            break;

          if(i == e1 && !nt){ // its a true
            v = true;
            break;
          }else if(i == e2 && !nf) // its a false
            break;

          if(!nt && !(i < e1 && t[i] == c)) // end of truename or not 'true'
            nt = true;
          if(!nf && !(i < e2 && f[i] == c)) // end of falsename or not 'false'
            nf = true;
        }while(++i, ++in, !(nt && nf));
        if(nt && nf)
          err |= ios_base::failbit;
      }
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long& v) const
    {
      unsigned long value = get_int<unsigned long, signed long>(in,end,f,err, numeric_limits<long>::__max, numeric_limits<long>::__min);
      v = static_cast<long>(value);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long long& v) const
    {
      unsigned long long value = get_int<unsigned long long, signed long long>(in,end,f,err, numeric_limits<long long>::__max, numeric_limits<long long>::__min);
      v = static_cast<long long>(value);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned short& v) const
    {
      unsigned value = get_int<unsigned long, signed long>(in,end,f,err, numeric_limits<unsigned short>::__max, numeric_limits<unsigned short>::__min);
      v = static_cast<unsigned short>(value);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned int& v) const
    {
      v = get_int<unsigned long, signed long>(in,end,f,err, numeric_limits<unsigned int>::__max, numeric_limits<unsigned int>::__min);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned long& v) const
    {
      v = get_int<unsigned long, signed long>(in,end,f,err, numeric_limits<unsigned long>::__max, numeric_limits<unsigned long>::__min);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, unsigned long long& v) const
    {
      v = get_int<unsigned long long, signed long long>(in,end,f,err, numeric_limits<unsigned long long>::__max, numeric_limits<unsigned long long>::__min);
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, float& v) const
    {
      v = static_cast<float>(get_float(in, end, f, err, numeric_limits<float>::max(), numeric_limits<float>::min()));
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, double& v) const
    {
      v = static_cast<double>(get_float(in, end, f, err, numeric_limits<double>::max(), numeric_limits<double>::min()));
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, long double& v) const
    {
      v = static_cast<long double>(get_float(in, end, f, err, numeric_limits<long double>::max(), numeric_limits<long double>::min()));
      return in;
    }
    _NTL_LOC_VIRTUAL iter_type do_get(iter_type in, iter_type end, ios_base& f, ios_base::iostate& err, void*& v) const
    {
      // turn on hexadecimal mode for parsing pointer value
      const ios_base::fmtflags prev_base = (f.flags() & ios_base::basefield);
      f.setf(ios_base::hex, ios_base::basefield);

      uintptr_t value = get_int<uintptr_t, intptr_t>(in,end,f,err, numeric_limits<uintptr_t>::__max, numeric_limits<uintptr_t>::__min);
      v =reinterpret_cast<void*>(value);
      f.setf(prev_base, ios_base::basefield);  // restore previous base flag
      return in;
    }
    ///\}
private:
  static long double get_float(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, long double max_val, long double min_val)
  {
    if(in == end){
      err |= ios_base::eofbit;
      return 0;
    }

    // initialization
    const numpunct<char_type>& np = use_facet< numpunct<char_type> >(str.getloc());
    const ctype<char_type>& check = use_facet< ctype<char_type> >(str.getloc());

    const ios_base::fmtflags flags = str .flags();
    const ios_base::fmtflags basefield = (flags & ios_base::basefield);

    const char_type thousands_sep = np.thousands_sep(), decimal_sep = np.decimal_point();
    const size_t group_width = np.grouping().length();

    const unsigned base = basefield == ios_base::oct ? 8 : basefield == ios_base::hex ? 16 : 10;
    ctype_base::mask base_mask = ctype_base::xdigit;

    bool dec_extracted = false;
    char valuebuf[128], *pval = valuebuf;
    size_t ic = 0;
    do {
      char_type ct = *in;
      if(in == end)
        break;
      char c = static_cast<char>(ct);
      bool discard = ct == thousands_sep && group_width > 0;
      if(discard){
        if(!dec_extracted)
          continue;
        // thousand sep isn't valid in decimal part
        ic = 0;
        break;
      }
      if(ct == decimal_sep) {
        c = '.', dec_extracted = true;
        base_mask = ctype_base::digit;
      } else if(ct != '-' && ct != '+') {
        // check is it a valid digit
        if(dec_extracted && (c == 'e' || c == 'E')){
          // ok
        }else {
          // check digit
          if(!check.is(base_mask, c))
            break;
          // check base
          unsigned digit;
          if(check.is(ctype_base::alpha, c))
            //digit = ct.toupper(c) - 'A' + 10;
            digit = (c & ~(1 << 5)) - 'A' + 10;
          else
            digit = c - '0';
          if(digit >= base)
            break;
        }
      }
      ++ic;
      *pval++ = c;
    } while(++in, true);

    // NOTE: may be bad/fail bit on format error?
    static const ios_base::iostate bad_format = ios_base::failbit;

    if(ic == 0)
      err |= bad_format;
    if(in == end)
      err |= ios_base::eofbit;

    // extract
    *pval = 0;
    long double value = 0;
    if(ic){
      value = std::strtod(*valuebuf == '-' ? valuebuf+1 : valuebuf, &pval);
      if(value > max_val)
        value = max_val;
      else if (value < min_val)
        value = min_val;
      else if(*valuebuf == '-')
        value = -value; 
    }
    return value;
  }

  template<typename storage_type, typename signed_storage_type> // this function will instantiated only two times: for 64bit and all other types
  static storage_type get_int(iter_type in, iter_type end, ios_base& str, ios_base::iostate& err, storage_type max_val, signed_storage_type min_val)
  {
    if(in == end){
      err |= ios_base::eofbit;
      return 0;
    }

    // initialization
    const numpunct<char_type>& np = use_facet< numpunct<char_type> >(str.getloc());
    const ctype<char_type>& ct = use_facet< ctype<char_type> >(str.getloc());

    const ios_base::fmtflags flags = str .flags();
    const ios_base::fmtflags basefield = (flags & ios_base::basefield);

    const char_type thousands_sep = np.thousands_sep();
    const size_t group_width = np.grouping().length();

    const unsigned base = basefield == ios_base::oct ? 8 : basefield == ios_base::hex ? 16 : 10;
    unsigned rem = static_cast<unsigned>(max_val % base);
    const ctype_base::mask base_mask = base == 16 ? ctype_base::xdigit : ctype_base::digit;

    // parse state
    bool minus = false, sign_extracted = false, prefix_extracted = false, overflow = false;

    storage_type value = 0, max_base_val = max_val / base;

    // NOTE: may be bad/fail bit on format error?
    static const ios_base::iostate bad_format = ios_base::failbit;

    prefix_extracted = base != 8 && base != 16; // 8 & 16 have special prefix

    size_t ic = 0; // number of digits extracted
    do{
      char_type c = *in;
      if(in == end)
        break;

      if(base != 16 && value != 0 && group_width > 0 && c == thousands_sep) // skip group separator in the middle of digits (only in octal or decimal base)
        continue;

      if(c == '-' || c == '+'){
        if(sign_extracted || ic > 0)  // break on second sign or in the middle of number ("+123-456" == 123, -456)
          break;
        sign_extracted = true;
        minus = c == '-';
        if(minus){
          max_base_val = static_cast<storage_type>(min_val);
          rem = static_cast<unsigned>(max_base_val % base);
        }
        continue;
      }

      // skip prefix
      if(!prefix_extracted && c == '0'){
        ++ic;
        continue;
      }

      if(base == 16 && (c == 'x' || c == 'X')){
        if(prefix_extracted || ic > 1)  // break on 'x' ("0x123x")
          break;
        ic--;
        continue;
      }


      prefix_extracted = true;

      // check is it a valid digit
      if(!ct.is(base_mask, c))
        break;

      // its a valid character, use it
      unsigned digit;
      if(ct.is(ctype_base::alpha, c))
        //digit = ct.toupper(c) - 'A' + 10;
        digit = (c & ~(1 << 5)) - 'A' + 10;
      else
        digit = c - '0';
      if(digit >= base)
        break;

      ++ic;
      if(value < max_base_val || (value == max_base_val && digit <= rem))
        value = value * base + digit;
      else{
        overflow = true;
        break;
      }
    }while(++in, true);

    if(overflow)
      value = minus ? static_cast<storage_type>(min_val) : max_val;
    else if(minus)
      value = static_cast<storage_type>( -static_cast<signed_storage_type>(value) );

    if(ic == 0)
      err |= bad_format;
    if(in == end)
      err |= ios_base::eofbit;
    return value;
  }
};

/// 22.2.2.2 Class template num_put [lib.locale.nm.put].
/// 1 The facet num_put is used to format numeric values to a character sequence
///   such as an ostream.
template <class charT, class OutputIterator = ostreambuf_iterator<charT> >
class num_put : public locale::facet
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef charT           char_type;
    typedef OutputIterator  iter_type;

    explicit num_put(size_t refs = 0)
      :facet(refs)
    {}

    ///\name 22.2.2.2.1 num_put members [facet.num.put.members]
    /// 1 Returns: do_put(out, f, fill, val).

    iter_type put(iter_type s, ios_base& f, char_type fill, bool v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, long v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, long long v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long long v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, double v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, long double v) const
    {
      return do_put(s, f, fill, v);
    }
    iter_type put(iter_type s, ios_base& f, char_type fill, const void* v) const
    {
      return do_put(s, f, fill, v);
    }
    ///\}

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:
    friend struct __::facets;
    ~num_put() //virtual
    {}

    ///\name 22.2.2.2.2 num_put virtual functions [facet.num.put.virtuals]

    /// 6 Returns: If (str.flags() & ios_base::boolalpha) == 0
    ///   returns do_put(out, str, fill, (int)val),
    ///   otherwise obtains a string s as if by
    ///   string_type s = val ? use_facet<ctype<charT> >(loc).truename()
    ///                   : use_facet<ctype<charT> >(loc).falsename();
    ///   and then inserts each character c of s into out via *out++ = c and returns out.
    _NTL_LOC_VIRTUAL iter_type
      do_put(iter_type out, ios_base& str, char_type fill, bool val) const
    {
      if ( !(str.flags() & ios_base::boolalpha) )
        return do_put(out, str, fill, static_cast<unsigned long>(val));
      const numpunct<charT>& np = use_facet<numpunct<charT> >(str.getloc());
      const numpunct<charT>::string_type s = val ? np.truename() : np.falsename();
      return copy(s.begin(), s.end(), out);
    }

    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, long v) const
    {
      return put_int(out, str, fill, v, true);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, unsigned long v) const
    {
      return put_int(out, str, fill, v, false);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, long long v) const
    {
      return put_int(out, str, fill, v, true, true);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, unsigned long long v) const
    {
      return put_int(out, str, fill, v, false, true);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, double v) const
    {
      return put_float(out, str, fill, v);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, long double v) const
    {
      return return put_float(out, str, fill, v);
    }
    _NTL_LOC_VIRTUAL iter_type do_put(iter_type out, ios_base& str, char_type fill, const void* v) const
    {
      return put_int(out,str,fill,reinterpret_cast<unsigned long long>(v), false, sizeof(void*) > sizeof(long), true);
    }
    ///\}
  private:
    static iter_type put_int(iter_type out, ios_base& str, char_type fill, unsigned long long v, bool signed_v, bool long_v = false, bool pointer_v = false)
    {
      const ios_base::fmtflags flags = str.flags();
      const ios_base::fmtflags adjust = flags & ios_base::adjustfield;
      const ios_base::fmtflags basefield = flags & ios_base::basefield;
      const bool               uppercase = (flags & ios_base::uppercase) != 0;
      //const ios_base::fmtflags floatfield = flags & ios_base::floatfield;
      const bool               showpos = (flags & ios_base::showpos) != 0;
      const bool               showbase = (flags & ios_base::showbase) != 0;


      // [dec, oct, hex] = [0, 2, 4]
      static const char bases[] = {'u', 'o', 'x'};

      // %[flags] [width] [.precision] [{h | l | ll | I | I32 | I64}]type
      char fmtbuf[16];
      char* fmt = fmtbuf;
      *fmt++ = '%';
      if(showpos)
        *fmt++ = '+';
      if(showbase && !pointer_v) // pointer have a custom prefix always
        *fmt++ = '#';

      if(pointer_v){
        *fmt = 'p';
      }else{
        if(long_v)
          *fmt++ = 'I', *fmt++ = '6', *fmt++ = '4';

        *fmt = bases[basefield >> 6]; // (base >> 5) / 2

        if(basefield == ios_base::dec && signed_v)
          *fmt = 'd';
        else if(basefield == ios_base::hex && uppercase)
          *fmt = 'X';
      }
      *++fmt = 0;

      char buf[128], *valuebuf = buf; // may not enough if large width
      streamsize l = _snprintf(buf, _countof(buf), fmtbuf, long_v ? v : static_cast<unsigned long>(v));

      if(basefield == ios_base::hex && showbase && v == 0)
        pointer_v = true;

      // adjust
      const streamsize width = str.width();
      if(width && width > l){
        streamsize pad = width - l;
        if(pointer_v)
          pad -= 2;

        // pad before
        if(adjust == ios_base::internal && l > 0){
          if(valuebuf[0] == '-' || valuebuf[0] == '+'){
            // copy only sign
            out = copy_n(valuebuf, 1, out);
            valuebuf++, l--;
          }else if(basefield == ios_base::hex && showbase){
            // copy '0x'
            out = copy_n(valuebuf, 2, out);
            valuebuf += 2, l -= 2;
          } else if(pointer_v){
            out = copy_n("0x",2,out);
            pointer_v = false;
          }
        }
        if(adjust != ios_base::left)
          out = __::fill_n(out, pad, fill);

        if(pointer_v)
          out = copy_n("0x",2,out);

        out = copy_n(valuebuf, l, out);

        // pad after
        if(adjust == ios_base::left)
          out = __::fill_n(out, pad, fill);
      }else{
        if(pointer_v)
          out = copy_n("0x",2,out);
        out = copy_n(valuebuf, l, out);
      }
      str.width(0);
      return out;
    }

    static iter_type put_float(iter_type out, ios_base& str, char_type fill, double v)
    {
      const ios_base::fmtflags flags = str.flags(),
        floatfield = flags & ios_base::floatfield,
        adjust = flags & ios_base::adjustfield;

      static const char type[] = {'g', 'f', 'e', 'a'};
      // %
      char fmtbuf[16], *fmt = fmtbuf;
      *fmt++ = '%';
      if(flags & ios_base::showpos)
        *fmt++ = '+';
      if(flags & ios_base::showpoint)
        *fmt++ = '#';

      // precision
      if(floatfield != ios_base::floatfield){
        *fmt++ = '.';
        size_t n;
        ntl::numeric::itoa(str.precision(), fmt, _countof(fmtbuf), 10, &n);
        fmt += n;
      }
      // type
      *fmt = type[floatfield >> 11];
      if(flags & ios_base::uppercase)
        *fmt -= 'a'-'A';
      fmt[1] = 0;

      char floatbuf[128];
      size_t len = ntl::msvcrt::snprintf(floatbuf, _countof(floatbuf), fmtbuf, v);

      charT value[128], *valuebuf = value;
      if(len > 0)
      {
        // group & sep
        const numpunct<charT>& punct = use_facet< numpunct<charT> >(str.getloc());
        const basic_string<charT> vec = punct.grouping();
        const charT ts = punct.thousands_sep(),
          ds = punct.decimal_point();

        int group = 0, groupsize = 0, groups = static_cast<int>(vec.size()), i = 0;
        size_t to = _countof(value) - 1;

        bool nogroup = vec.empty() || memchr(floatbuf, '.', len) != nullptr;
        if(!nogroup)
          groupsize = vec[0];
        
        for(ssize_t from = len-1; from >= 0; from--, to--, i++){
          const char c = floatbuf[from];
          if(nogroup){
            value[to] = c != '.' ? c : ds;

            if(c == '.'){
              // setup grouping
              nogroup = vec.empty();
              if(!nogroup)
                groupsize = vec[group];
              i = -1;
            }
            continue;
          }
          if(i == groupsize && groupsize > 0 && groupsize != CHAR_MAX 
            && c != '+' && c != '-'){ // prevent "-,123,456"
            // new group
            value[to--] = ts;
            if(group+1 < groups){
              groupsize = vec[++group];
            }
            i = 0;
          }
          value[to] = c;
        }
        to++;
        len = _countof(value) - to;
        valuebuf = &value[to];
      }

      // adjust
      const size_t width = str.width();
      const bool need_adjust = width && width > len;
      streamsize pad = width - len;
      if(need_adjust){
        // pad before
        if(adjust == ios_base::internal && len > 0 && (valuebuf[0] == '-' || valuebuf[0] == '+') ){
          // copy only sign
          out = copy_n(valuebuf, 1, out);
          valuebuf++, len--;
        }
        if(adjust != ios_base::left)
          out = __::fill_n(out, pad, fill);
      }

      out = copy_n(valuebuf, len, out);

      if(need_adjust && adjust == ios_base::left)    // after
        out = __::fill_n(out, pad, fill);
      str.width(0);
      return out;
    }

};

//template <>
//class num_put<char>;// : public locale::facet

/** @} lib_locale_numeric */



/** \defgroup lib_locale_collate 22.2.4 The collate category [category.collate]
 *@{*/

// 22.2.4, collation:

/**
 *	@brief    22.2.4.1 Class template collate [locale.collate]
 *  @details  The class collate<charT> provides features for use in the collation (comparison) and hashing of strings.
 *  A locale member function template, operator(), uses the collate facet to allow a locale to act directly
 *  as the predicate argument for standard algorithms (Clause 25) and containers operating on strings.
 **/
template <class charT>
class collate:
  public locale::facet 
{
public:
  typedef charT char_type;
  typedef basic_string<charT> string_type;

  explicit collate(size_t refs = 0)
    :facet(refs)
  {}

  /** Returns 1 if the first string is greater than the second, -1 if less, zero otherwise. */
  int compare(const charT* low1, const charT* high1, const charT* low2, const charT* high2) const;

  /** Returns a basic_string<charT> value that, compared lexicographically with the result of calling
    transform() on another string, yields the same result as calling do_compare() on the same two
    strings. */
  string_type transform(const charT* low, const charT* high) const;

  /** Returns an integer value equal to the result of calling hash() on any other string for which 
    do_compare() returns 0 (equal) when passed the two strings. */
  long hash(const charT* low, const charT* high) const;

  static locale::id id;
protected:
  ~collate()
  {}

  _NTL_LOC_VIRTUAL int do_compare(const charT* low1, const charT* high1, const charT* low2, const charT* high2) const;
  _NTL_LOC_VIRTUAL string_type do_transform(const charT* low, const charT* high) const;
  _NTL_LOC_VIRTUAL long do_hash (const charT* low, const charT* high) const;
};

template <class charT> class collate_byname;

/** @} lib_locale_collate */

/** \defgroup lib_locale_time 22.2.5 The time category [category.time]
*@{*/

// 22.2.5, date and time:

class time_base;
template <class charT, class InputIterator> class time_get;
template <class charT, class InputIterator> class time_get_byname;
template <class charT, class OutputIterator> class time_put;
template <class charT, class OutputIterator> class time_put_byname;

/** @} lib_locale_time */

/** \defgroup lib_locale_monetary 22.2.6 The monetary category [category.monetary]
*@{*/

// 22.2.6, money:

class money_base;
template <class charT, class InputIterator> class money_get;
template <class charT, class OutputIterator> class money_put;
template <class charT, bool Intl> class moneypunct;
template <class charT, bool Intl> class moneypunct_byname;

/** @} lib_locale_monetary */

/** \defgroup lib_locale_messages 22.2.7 The message retrieval category [category.messages]
*@{*/

// 22.2.7, message retrieval:

class messages_base;
template <class charT> class messages;
template <class charT> class messages_byname;

/** @} lib_locale_messages */
/** @} lib_locale_categories */

#if !STLX__CONFORMING_LOCALE

inline const locale& locale::operator=(const locale&) __ntl_nothrow { return *this; }
inline basic_string<char> locale::name() const { return "C"; }

// locale::id ctor
//inline locale::id::id(){}

namespace __
{
  extern "C" bool __wchar_masks_data(const unsigned char*& masks, unsigned& size);

  struct facets
  {
    // has_facet
    template<class Facet>
    struct has_facet: false_type {};

    template<class charT, class InputIterator>
    struct has_facet<num_get<charT, InputIterator > >: true_type{};
    template<class charT, class OutputIterator>
    struct has_facet<num_put<charT, OutputIterator> >: true_type{};

    template<class charT>
    struct has_facet<numpunct<charT> >: true_type{};

    template<>
    struct has_facet<ctype<char> >: true_type{};
    template<>
    struct has_facet<ctype<wchar_t> >: true_type{};

    template<>
    struct has_facet<codecvt<char,char,mbstate_t> >: true_type{};
    template<>
    struct has_facet<codecvt<wchar_t,wchar_t,mbstate_t> >: true_type{};
    template<>
    struct has_facet<codecvt<wchar_t,char,mbstate_t> >: true_type{};
    template<>
    struct has_facet<codecvt<char16_t,char,mbstate_t> >: true_type{};

    //////////////////////////////////////////////////////////////////////////
    // use_facet

    /** Default facet */
    template<class DefaultFacet>
    static const DefaultFacet& get_facet(const locale&, type2type<DefaultFacet>/*, typename enable_if<has_facet<DefaultFacet>::value>::type* =0*/)
    {
      return *static_storage<DefaultFacet>::get_object();
    }
#if 1
    static __forceinline const ctype<wchar_t>& get_facet(const locale&, type2type<ctype<wchar_t> >/*, enable_if<has_facet<ctype<wchar_t>>::value>::type* =0*/)
    {
      //static void * f[sizeof(ctype<wchar_t>)/sizeof(void*)];
      //ctype<wchar_t> * const p = reinterpret_cast<ctype<wchar_t>*>(f);
      static ctype_base::mask* table = 0;
      if(!table){
        // get table
        const unsigned char* wchar_masks;
        unsigned wchar_masks_size;
        if(__wchar_masks_data(wchar_masks, wchar_masks_size)){
          table = new ctype_base::mask[ctype<wchar_t>::table_size];
          using namespace ntl::nt;

          uint32_t dsize;
          ntstatus st = RtlDecompressBuffer(compression::default_format, table, ctype<wchar_t>::table_size, wchar_masks, wchar_masks_size, &dsize);
          if(!success(st)){
            delete[] table;
            table = nullptr;
          }
        }
      }
      //if ( !f[0] ) new (p) ctype<wchar_t>(table);
      //return *p;
      return *static_storage<ctype<wchar_t> >::get_object(table);
    }
#endif
  };
}

template <class Facet> 
const Facet& use_facet(const locale& loc)
{
  return __::facets::get_facet(loc, __::type2type<Facet>());
}

/// 5 Returns: true if the facet requested is present in loc; otherwise false.
template <class Facet>
bool has_facet(const locale&) __ntl_nothrow
{
  return __::facets::has_facet<Facet>::value;
}

// @note: locale id globals to suppress the C4661
__declspec(selectany)
std::locale::id std::ctype<char>::id;
__declspec(selectany)
std::locale::id std::ctype<wchar_t>::id;
template<>
__declspec(selectany)
std::locale::id std::num_get<char, istreambuf_iterator<char> >::id;
template<>
__declspec(selectany)
std::locale::id std::num_get<wchar_t, istreambuf_iterator<wchar_t> >::id;
template<>
__declspec(selectany)
std::locale::id std::num_put<char, ostreambuf_iterator<char> >::id;
template<>
__declspec(selectany)
std::locale::id std::num_put<wchar_t, ostreambuf_iterator<wchar_t> >::id;


#endif // STLX__CONFORMING_LOCALE

/** @} lib_locale */
}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_LOCALE
