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

#if STLX__CONFORMING_LOCALE
#define _NTL_LOC_VIRTUAL virtual
#else
#define _NTL_LOC_VIRTUAL /**/virtual/**/
#endif

#include "string.hxx"
#include "cwctype.hxx"
#include "stdexcept.hxx"

#include "../nt/string.hxx"

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4820)// X bytes padding added... (class ctype<char>)
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
      none = 0,   collate = 0x01,   ctype = 0x02,
      monetary = 0x04,  numeric = 0x08,   time = 0x10, messages = 0x20,
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
      public:
        id();
      private:
        void operator=(const id&) __deleted;
        id(const id&) __deleted;
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
template <class charT> bool isspace(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::space, c);
}

/** Returns true if character is printable */
template <class charT> bool isprint(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::print, c);
}

/** Returns true if character is control */
template <class charT> bool iscntrl(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::cntrl, c);
}

/** Returns true if character is an upper */
template <class charT> bool isupper(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::upper, c);
}

/** Returns true if character is a lower */
template <class charT> bool islower(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::lower, c);
}

/** Returns true if character is a letter */
template <class charT> bool isalpha(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::alpha, c);
}

/** Returns true if character is a digit */
template <class charT> bool isdigit(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::digit, c);
}

/** Returns true if character is punctuation character */
template <class charT> bool ispunct(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::punct, c);
}

/** Returns true if character is hexadecimal digit */
template <class charT> bool isxdigit(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::xdigit, c);
}

/** Returns true if character is alphanumeric */
template <class charT> bool isalnum(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::alnum, c);
}

/** Returns true if character is a graphic character, excluding space */
template <class charT> bool isgraph(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).is(ctype_base::graph, c);
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
template<class Codecvt, class Elem = wchar_t>
class wstring_convert
{
public:
  typedef basic_string<char>                          byte_string;
  typedef basic_string<Elem>                          wide_string;
  typedef typename Codecvt::state_type                state_type;
  typedef typename wide_string::traits_type::int_type int_type;

  wstring_convert(Codecvt *pcvt = new Codecvt)
    :cvptr(pcvt), cvtstate()
  {}
  wstring_convert(Codecvt *pcvt, state_type state)
    :cvptr(pcvt), cvtstate(state)
  {}
  wstring_convert(const byte_string& byte_err, const wide_string& wide_err = wide_string())
    :cvtptr(new Codecvt), cvtstate(), serr(byte_err), werr(wide_err)
  {}
  ~wstring_convert()
  {
    delete cvptr;
  }

  /** convert the single-element sequence byte to a wide %string */
  wide_string from_bytes(char byte) __ntl_throws(range_error);
  /** convert the null-terminated sequence beginning at \e ptr to a wide %string. */
  wide_string from_bytes(const char *ptr) __ntl_throws(range_error);
  /** convert the sequence stored in \e str to a wide %string */
  wide_string from_bytes(const byte_string& str) __ntl_throws(range_error);
  /** convert the sequence defined by the range <tt>[first,last)</tt> to a wide %string */
  wide_string from_bytes(const char *first, const char *last) __ntl_throws(range_error);

  /** convert the single-element sequence \e wchar to a byte %string */
  byte_string to_bytes(Elem wchar);
  /** convert the null-terminated sequence beginning at \e wptr to a byte %string */
  byte_string to_bytes(const Elem *wptr);
  /** convert the sequence stored in \e wstr to a byte %string */
  byte_string to_bytes(const wide_string& wstr);
  /** convert the sequence defined by the range <tt>[first,last)</tt> to a byte %string */
  byte_string to_bytes(const Elem *first, const Elem *last);

  size_t converted() const { return cvtcount; }
  state_type state() const { return cvtstate; }

private:
  // a byte string to display on errors
  byte_string serr;
  // a wide string to display on errors
  wide_string werr;
  // a pointer to the allocated conversion object
  Codecvt *cvtptr;
  // a conversion state object
  state_type cvtstate;
  // a conversion count
  size_t cvtcount;
};


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
class wbuffer_convert
  :public basic_streambuf<Elem, Tr>
{
public:
  typedef typename Tr::state_type state_type;

  wbuffer_convert(std::streambuf *bytebuf = 0, Codecvt *pcvt = new Codecvt, state_type state = state_type())
    :buf(bytebuf), cvtptr(pcvt), cvtstate(state)
  {}

  ~wbuffer_convert()
  {
    delete cvtptr;
  }

  std::streambuf *rdbuf() const { return buf; }
  std::streambuf *rdbuf(streambuf *bytebuf)
  {
    std::swap(buf, bytebuf);
    return bytebuf;
  }

  state_type state() const { return cvtstate; }
private:
  // a pointer to its underlying byte stream buffer
  streambuf *buf;
  // a pointer to the allocated conversion object
  Codecvt *cvtptr;
  // a conversion state object
  state_type cvtstate;
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
static const int16_t __ct_xdigit = 0x0080;
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
      graph = alnum|punct;
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

    explicit ctype(size_t refs = 0);

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
    const charT* narrow(const charT* low, const charT*, char dfault, char* to) const
    {
      return do_narrow(low, high, dfault, to);
    }

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ///\name 22.2.1.1.2 ctype virtual functions [locale.ctype.virtuals]

    ~ctype(); //virtual

    virtual bool do_is(mask m, charT c) const;
    virtual const charT* do_is(const charT* low, const charT* high, mask* vec) const;
    virtual const charT* do_scan_is(mask m, const charT* low, const charT* high) const;
    virtual const charT* do_scan_not(mask m, const charT* low, const charT* high) const;
    virtual charT do_toupper(charT) const;
    virtual const charT* do_toupper(charT* low, const charT* high) const;
    virtual charT do_tolower(charT) const;
    virtual const charT* do_tolower(charT* low, const charT* high) const;
    virtual charT do_widen(char) const;
    virtual const char* do_widen(const char* low, const char* high, charT* dest) const;
    virtual char do_narrow(charT, char dfault) const;
    virtual const charT* do_narrow(const charT* low, const charT* high, char dfault, char* dest) const;
    ///\}
};// class ctype

/// 22.2.1.2 Class template ctype_byname [locale.ctype.byname]
template <class charT>
class ctype_byname : public ctype<charT>
{
  public:
    typedef ctype<charT>::mask mask;
    explicit ctype_byname(const char*, size_t refs = 0);
  protected:
    ~ctype_byname(); // virtual
    virtual bool do_is(mask m, charT c) const;
    virtual const charT* do_is(const charT* low, const charT* high, mask* vec) const;
    virtual const charT* do_scan_is(mask m, const charT* low, const charT* high) const;
    virtual const charT* do_scan_not(mask m, const charT* low, const charT* high) const;
    virtual charT do_toupper(charT) const;
    virtual const charT* do_toupper(charT* low, const charT* high) const;
    virtual charT do_tolower(charT) const;
    virtual const charT* do_tolower(charT* low, const charT* high) const;
    virtual charT do_widen(char) const;
    virtual const char* do_widen(const char* low, const char* high, charT* dest) const;
    virtual char do_narrow(charT, char dfault) const;
    virtual const charT* do_narrow(const charT* low, const charT* high, char dfault, char* dest) const;
};

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


/// 22.2.1.4 Class ctype_byname<char> [lib.locale.ctype.byname.special]
template <> class ctype_byname<char>
: public ctype<char>
{
  public:
    explicit ctype_byname(const char*, size_t refs = 0);
  protected:
    virtual ~ctype_byname(); // virtual
    virtual char do_toupper(char c) const;
    virtual const char* do_toupper(char* low, const char* high) const;
    virtual char do_tolower(char c) const;
    virtual const char* do_tolower(char* low, const char* high) const;
    virtual char do_widen(char c) const;
    virtual const char* do_widen(const char* low, const char* high, char* to) const;
    virtual char do_narrow(char c, char dfault) const;
    virtual const char* do_narrow(const char* low, const char* high, char dfault, char* to) const;
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


// 22.2.1.5 Class template codecvt [lib.locale.codecvt]

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

    /** Returns -1 if the encoding of the \a externT sequence is state-dependent; else the constant number of
      \a externT characters needed to produce an internal character; or 0 if this number is not a constant */
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

    _NTL_LOC_VIRTUAL result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    _NTL_LOC_VIRTUAL result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const;

    _NTL_LOC_VIRTUAL result do_unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    _NTL_LOC_VIRTUAL int do_encoding() const __ntl_nothrow;

    _NTL_LOC_VIRTUAL bool do_always_noconv() const __ntl_nothrow;

    _NTL_LOC_VIRTUAL int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const;

    _NTL_LOC_VIRTUAL int do_max_length() const __ntl_nothrow;

};// codecvt

/// 22.2.1.6 Class template codecvt_byname [lib.locale.codecvt.byname]
template <class internT, class externT, class stateT>
class codecvt_byname : public codecvt<internT, externT, stateT>
{
  public:
    explicit codecvt_byname(const char*, size_t refs = 0);

  protected:

    using codecvt<internT, externT, stateT>::result;

    ~codecvt_byname(); // virtual

    _NTL_LOC_VIRTUAL result do_out(stateT& state,
                          const internT* from,
                          const internT* from_end,
                          const internT*& from_next,
                          externT* to,
                          externT* to_limit,
                          externT*& to_next) const;

    _NTL_LOC_VIRTUAL result do_in(stateT& state, const externT* from, const externT* from_end,
      const externT*& from_next, internT* to, internT* to_limit, internT*& to_next) const;

    _NTL_LOC_VIRTUAL int do_encoding() const __ntl_nothrow;

    _NTL_LOC_VIRTUAL bool do_always_noconv() const __ntl_nothrow;

    _NTL_LOC_VIRTUAL int do_length(stateT&, const externT* from, const externT* end, size_t max) const;

    _NTL_LOC_VIRTUAL result do_unshift(stateT& state, externT* to, externT* to_limit, externT*& to_next) const;

    _NTL_LOC_VIRTUAL int do_max_length() const __ntl_nothrow;

};

///\name codecvt specializations

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
  _NTL_LOC_VIRTUAL result do_out(state_type& /*state*/, const intern_type* from, const intern_type* from_end,
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

  _NTL_LOC_VIRTUAL result do_in(state_type& /*state*/, const extern_type* from, const extern_type* from_end,
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

  _NTL_LOC_VIRTUAL result do_unshift(state_type& /*state*/, extern_type* to, extern_type* /*to_limit*/, extern_type*& to_next) const
  {
    to_next = to;
    return noconv;
  }

  _NTL_LOC_VIRTUAL int do_encoding() const __ntl_nothrow
  {
    return 1;
  }

  _NTL_LOC_VIRTUAL bool do_always_noconv() const __ntl_nothrow
  {
    return false;
  }

  _NTL_LOC_VIRTUAL int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const
  {
    return static_cast<int>(min(max,static_cast<size_t>(end-from)));
  }

  _NTL_LOC_VIRTUAL int do_max_length() const __ntl_nothrow
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
    ///   in which each element vec[i] represents the number of digits246)
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

    explicit num_get(size_t refs = 0);
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, bool& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, long& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned short& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned int& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, unsigned long& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, float& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, double& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, long double& v) const;
    iter_type get(iter_type in, iter_type end, ios_base&, ios_base::iostate& err, void*& v) const;
    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ~num_get(); //virtual

    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, bool& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, long& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned short& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned int& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, unsigned long& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, float& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, double& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, long double& v) const;
    virtual iter_type do_get(iter_type, iter_type, ios_base&, ios_base::iostate& err, void*& v) const;
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
    /// 1 Returns: do_put(out, str, fill, val).

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
    virtual iter_type
      do_put(iter_type out, ios_base& str, char_type fill, bool val) const
    {
      if ( !(str.flags() & ios_base::boolalpha) )
        return do_put(out, str, fill, static_cast<unsigned long>(val));
      const numpunct<charT>& np = use_facet<numpunct<charT> >(str.getloc());
      const numpunct<charT>::string_type s = val ? np.truename() : np.falsename();
      return copy(s.begin(), s.end(), out);
    }

    virtual iter_type do_put(iter_type out, ios_base& str, char_type fill, long v) const
    {
      return put_int(out, str, fill, v, true);
    }
    virtual iter_type do_put(iter_type out, ios_base& str, char_type fill, unsigned long v) const
    {
      return put_int(out, str, fill, v, false);
    }
    virtual iter_type do_put(iter_type out, ios_base&, char_type, double) const
    {
      return out;
    }
    virtual iter_type do_put(iter_type out, ios_base&, char_type, long double) const
    {
      return out;
    }
    virtual iter_type do_put(iter_type out, ios_base& str, char_type fill, const void* v) const
    {
      return put_int(out,str,fill,reinterpret_cast<unsigned long long>(v), false, sizeof(void*) > sizeof(long), true);
    }

    ///\}
  private:
    iter_type put_int(iter_type out, ios_base& str, char_type fill, unsigned long long v, bool is_signed, bool is_long = false, bool is_pointer = false) const
    {
      const ios_base::fmtflags flags = str.flags();
      const ios_base::fmtflags adjust = flags & ios_base::adjustfield;
      const ios_base::fmtflags basefield = flags & ios_base::basefield;
      const bool               uppercase = (flags & ios_base::uppercase) != 0;
      //const ios_base::fmtflags floatfield = flags & ios_base::floatfield;
      const bool               showpos = (flags & ios_base::showpos) != 0;
      const bool               showbase = (flags & ios_base::showbase) != 0;

      const streamsize width = str.width();

      // [dec, oct, hex]
      static const char bases[] = {'u', 'o', 'x'};

      // %[flags] [width] [.precision] [{h | l | ll | I | I32 | I64}]type
      char fmtbuf[16];
      char* fmt = fmtbuf;
      *fmt++ = '%';

      if(showpos)
        *fmt++ = '+';
      if(showbase)
        *fmt++ = '#';
      if(adjust == ios_base::internal){
        *fmt++ = '0';
        if(width)
          *fmt++ = '*';
      }

      if(is_pointer){
        *fmt = 'p';
      }else{
        if(is_long)
          *fmt++ = 'I', *fmt++ = '6', *fmt++ = '4';

        *fmt = bases[(basefield >> 5) - 1];

        if(basefield == ios_base::dec && is_signed)
          *fmt = 'd';
        else if(basefield == ios_base::hex && uppercase)
          *fmt = 'X';
      }
      *++fmt = 0;

      char buf[32]; // may not enough if large width
      streamsize l = adjust == ios_base::internal && width
        ? _snprintf(buf, _countof(buf), fmtbuf, width, is_long ? v : static_cast<unsigned long>(v))
      : _snprintf(buf, _countof(buf), fmtbuf, is_long ? v : static_cast<unsigned long>(v));

      // adjust
      if(width && width > l){
        streamsize pad = width - l;
        if(is_pointer)
          pad -= 2;
        if(adjust != ios_base::left && adjust != ios_base::internal) // before
          out = __::fill_n(out, pad, fill);

        if(is_pointer)
          out = copy_n("0x",2,out);
        out = copy_n(buf,l,out);

        if(adjust == ios_base::left)
          out = __::fill_n(out, pad, fill);
      }else{
        if(is_pointer)
          out = copy_n("0x",2,out);
        out = copy_n(buf,l,out);
      }
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

  virtual int do_compare(const charT* low1, const charT* high1, const charT* low2, const charT* high2) const;
  virtual string_type do_transform(const charT* low, const charT* high) const;
  virtual long do_hash (const charT* low, const charT* high) const;
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

namespace __
{
  extern const unsigned char wchar_masks[];
  extern const unsigned wchar_masks_size;

  struct facets
  {
    // has_facet
    template<class Facet>
    struct has_facet: false_type {};

    template<class charT, class OutputIterator>
    struct has_facet<num_put<charT, OutputIterator> >: true_type{};

    template<class charT>
    struct has_facet<numpunct<charT> >: true_type{};

    template<>
    struct has_facet<ctype<char> >: true_type{};
    template<>
    struct has_facet<ctype<wchar_t> >: true_type{};

    //////////////////////////////////////////////////////////////////////////
    // use_facet
    template<class charT, class OutputIterator>
    static const num_put<charT, OutputIterator>& get_facet(const locale&, type2type<num_put<charT, OutputIterator> >/*, typename enable_if<is_same<charT,char>::value>::type* =0*/)
    {
      //static const num_put<charT, OutputIterator> facet;
      //return facet;
      return *static_storage<num_put<charT,OutputIterator> >::get_object();
    }

    template<class charT>
    static const numpunct<charT>& get_facet(const locale&, type2type<numpunct<charT> >)
    {
      //static const  facet;
      //return facet;
      return *static_storage<numpunct<charT> >::get_object();
    }

    __forceinline
    static const ctype<char>& get_facet(const locale&, type2type<ctype<char> >)
    {
      // static ctype<char> is constructed with the default table which is not to be freed,
      // so the destructor call may be not queued up to the atexit function.
      // This is why it is implemented through placement new over a raw storage.
      //static void * f[sizeof(ctype<char>)/sizeof(void*)];
      //ctype<char> * const p = reinterpret_cast<ctype<char>*>(f);
      // The first class member is VTable ptr or the tab ptr member,
      // both are non-null after initialization is done.
      //if ( !f[0] ) new (p) ctype<char>;
      //return *p;
      return *static_storage<ctype<char> >::get_object();
    }

    __forceinline
    static const ctype<wchar_t>& get_facet(const locale&, type2type<ctype<wchar_t> >)
    {
      //static void * f[sizeof(ctype<wchar_t>)/sizeof(void*)];
      //ctype<wchar_t> * const p = reinterpret_cast<ctype<wchar_t>*>(f);
      static ctype_base::mask* table = 0;
      if(!table){
        // get table
        table = new ctype_base::mask[ctype<wchar_t>::table_size];
        using namespace ntl::nt;

        uint32_t dsize;
        ntstatus st = RtlDecompressBuffer(compression::default_format, table, ctype<wchar_t>::table_size, wchar_masks, wchar_masks_size, &dsize);
        if(!success(st)){
          delete[] table;
          table = nullptr;
        }
      }
      //if ( !f[0] ) new (p) ctype<wchar_t>(table);
      //return *p;
      return *static_storage<ctype<wchar_t> >::get_object(table);
    }

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

#endif // STLX__CONFORMING_LOCALE

/** @} lib_locale */
}//namespace std

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif//#ifndef NTL__STLX_LOCALE
