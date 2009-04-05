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
#include "stdexcept.hxx"

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
        facet(const facet&); // not defined
        void operator=(const facet&); // not defined
    };

    /// 22.1.1.1.3 Class locale::id [locale.id]
    class id
    {
      public:
        id();
      private:
        void operator=(const id&); // not defined
        id(const id&); // not defined
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
    static const locale& classic();
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

template <class charT> bool isspace(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::space, c);
}

template <class charT> bool isprint(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::print, c);
}

template <class charT> bool iscntrl(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::cntrl, c);
}

template <class charT> bool isupper(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::upper, c);
}

template <class charT> bool islower(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::lower, c);
}

template <class charT> bool isalpha(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::alpha, c);
}

template <class charT> bool isdigit(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::digit, c);
}

template <class charT> bool ispunct(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::punct, c);
}

template <class charT> bool isxdigit(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::xdigit, c);
}

template <class charT> bool isalnum(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::alnum, c);
}

template <class charT> bool isgraph(charT c, const locale& loc)
{
  use_facet<ctype<charT> >(loc).is(ctype_base::graph, c);
}
///\}

///\name 22.1.3.2 Character conversions [conversions]

template <class charT>
charT
  toupper(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).toupper(c);
}

template <class charT>
charT
  tolower(charT c, const locale& loc)
{
  return use_facet<ctype<charT> >(loc).tolower(c);
}
///\}

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
class ctype_base
{
  public:
    typedef int16_t mask;
    static const mask
      lower = __ct_lower,
      upper = __ct_upper,
      alpha = lower|upper|__ct_alpha,
      cntrl = __ct_cntrl,
      digit = __ct_digit,
      xdigit = __ct_xdigit,
      space = __ct_space|__ct_blank,
      punct = __ct_punct,
      print = alpha|digit|xdigit|punct|__ct_blank,// __ct_space?
      alnum = alpha|digit,
      graph = alnum|punct;
};

/**
 *	@brief 22.2.1.1 Class template ctype [locale.ctype]
 *  @details Class ctype encapsulates the C library \c <cctype> features. istream members are required to use ctype<>
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

    bool is(mask m, charT c) const { return do_is(m, c); }

    const charT* is(const charT* low, const charT* high, mask* vec) const
    {
      return do_is(low, high, vec);
    }

    const charT* scan_is(mask m, const charT* low, const charT* high) const
    {
      return do_scan_is(m, low, high);
    }

    const charT* scan_not(mask m, const charT* low, const charT* high) const
    {
      return do_scan_not(m, low, high);
    }

    charT toupper(charT c) const { return do_toupper(c); }

    const charT* toupper(charT* low, const charT* high) const
    {
      return do_toupper(low, high);
    }

    charT tolower(charT c) const { return do_tolower(c); }

    const charT* tolower(charT* low, const charT* high) const
    {
      return do_tolower(low, high);
    }

    charT widen(char c) const { return do_widen(c); }

    const char* widen(const char* low, const char* high, charT* to) const
    {
      return do_widen(low, high, to);
    }

    char narrow(charT c, char dfault) const { return do_narrow(c,dfault); }

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
 *	@brief 22.2.1.3 ctype specializations [facet.ctype.special]
 *  @details A specialization ctype<char> is provided so that the member functions on type \c char can be implemented inline.
 *  The implementation-defined value of member \c table_size is at least 256.
 **/
template <> class ctype<char>
: public locale::facet, public ctype_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    template <class Facet> friend const Facet& use_facet(const locale&);

    typedef char char_type;

    explicit ctype(const mask* tab = 0, bool del = false, size_t refs = 0)
    : locale::facet(refs), tab(tab ? tab : classic_table()), del(del) {}

    bool is(mask m, char c) const
    {
      return 0 != (table()[static_cast<unsigned char>(c)] & m);
    }

    const char* is(const char* low, const char* high, mask* vec) const
    {
      while ( low != high )
        *vec++ = table()[static_cast<unsigned char>(*low++)];
      return high;
    }

    const char* scan_is(mask m, const char* low, const char* high) const
    {
      while ( low != high && !(table()[static_cast<unsigned char>(*low)] & m) )
          ++low;
      return low;
    }

    const char* scan_not(mask m, const char* low, const char* high) const
    {
      while ( low != high && (table()[static_cast<unsigned char>(*low)] & m) )
          ++low;
      return low;
    }

    char toupper(char c) const { return do_toupper(c); }

    const char* toupper(char* low, const char* high) const { return do_toupper(low, high); }

    char tolower(char c) const { return do_tolower(c); }

    const char* tolower(char* low, const char* high) const {return do_tolower(low, high); }

    char widen(char c) const { return do_widen(c); }

    const char* widen(const char* low, const char* high, char* to) const { return do_widen(low, high, to); }

    char narrow(char c, char dfault) const { return do_narrow(c, dfault); }

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
      // tab pointer is allways initialyzed by the constructor
      if ( /*tab &&*/ del )
        delete[] tab;
    }

    const mask* table() const __ntl_nothrow { return tab; }

    static const mask* classic_table() __ntl_nothrow
    {
      return 0;
    }

    ///\name 22.2.1.3.4 ctype<char> virtual functions [facet.ctype.char.virtuals]

    _NTL_LOC_VIRTUAL char do_toupper(char c) const
    {
      return is(c, lower) ? c + 'A'-'a' : c;
    }

    _NTL_LOC_VIRTUAL const char* do_toupper(char* low, const char* high) const
    {
      while ( low != high )
        *low++ = do_toupper(*low);
      return high;
    }

    _NTL_LOC_VIRTUAL char do_tolower(char c) const
    {
      return is(c, upper) ? c + 'A'-'a' : c;
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
      while ( low != high )
        *to++ = *low++;
      return high;
    }

    _NTL_LOC_VIRTUAL char do_narrow(char c, char /*dfault*/) const { return c; }

    _NTL_LOC_VIRTUAL const char* do_narrow(const char * low,
                       const char * high,
                       char         /*dfault*/,
                       char *       to) const
    {
      while ( low != high )
        *to++ = *low++;
      return high;
    }

    ///\}

  private:

    ctype(const ctype&); // not defined
    void operator=(const ctype&); // not defined

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
    ctype_byname(const ctype_byname&); // not defined
    void operator=(const ctype_byname&); // not defined
};

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
class codecvt : public locale::facet, public codecvt_base
{
  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef internT intern_type;
    typedef externT extern_type;
    typedef stateT  state_type;

    explicit codecvt(size_t refs = 0);

    /**
     *	Translates characters in the source range [from,from_end), placing the results in sequential
     *  positions starting at destination \a to. Converts no more than (<tt>from_end-from</tt>) source elements, and
     *  stores no more than (<tt>to_limit-to<tt>) destination elements.
     **/
    result out(      stateT &   state,
               const internT *  from,
               const internT *  from_end,
               const internT *& from_next,
                     externT *  to,
                     externT *  to_limit,
                     externT *& to_next) const;

    /**
     *	Places characters starting at \a to that should be appended to terminate a sequence when the
     *  current \c stateT is given by \a state.
     *  Stores no more than (<tt>to_limit-to</tt>) destination elements, and
     *  leaves the \a to_next pointer pointing one beyond the last element successfully stored.
     **/
    result unshift(stateT &   state,
                   externT *  to,
                   externT *  to_limit,
                   externT *& to_next) const;

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
                    internT *& to_next) const;

    /** Returns -1 if the encoding of the \a externT sequence is state-dependent; else the constant number of
      \a externT characters needed to produce an internal character; or 0 if this number is not a constant */
    int encoding() const __ntl_nothrow;

    /** Returns \c true if \c do_in() and \c do_out() return \c noconv for all valid argument values. */
    bool always_noconv() const __ntl_nothrow;

    /** The effect on the \a state argument is "as if" it called 
        <tt>do_in(state, from, from_end, from, to, to+max, to)</tt> for \a to pointing to a buffer of at least \a max elements. */
    int length(stateT &, const externT* from, const externT* end, size_t max) const;

    /** Returns the maximum value that <tt>do_length(state, from, from_end, 1)</tt> 
      can return for any valid range <tt>[from, from_end)</tt> and \c stateT value \a state. */
    int max_length() const __ntl_nothrow;

    static locale::id id;

  ///////////////////////////////////////////////////////////////////////////
  protected:

    ~codecvt(); //virtual

    virtual result do_out(stateT& state, const internT* from, const internT* from_end,
      const internT*& from_next, externT* to, externT* to_limit, externT*& to_next) const;

    virtual result do_in(stateT& state, const externT* from, const externT* from_end,
      const externT*& from_next, internT* to, internT* to_limit, internT*& to_next) const;

    virtual result do_unshift(stateT& state, externT* to, externT* to_limit, externT*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(stateT&, const externT* from, const externT* end, size_t max) const;

    virtual int do_max_length() const __ntl_nothrow;

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

    iter_type put(iter_type s, ios_base& f, char_type fill, long long v) const;

    iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long v) const
    {
      return do_put(s, f, fill, v);
    }

    iter_type put(iter_type s, ios_base& f, char_type fill, unsigned long long v) const;

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
    virtual iter_type do_put(iter_type out, ios_base&, char_type fill, double v) const
    {
      return out;
    }
    virtual iter_type do_put(iter_type out, ios_base&, char_type fill, long double v) const
    {
      return out;
    }
    virtual iter_type do_put(iter_type out, ios_base&, char_type fill, const void* v) const
    {
      return out;
    }

    ///\}
  private:
    iter_type put_int(iter_type out, ios_base& str, char_type fill, unsigned long long v, bool is_signed, bool is_long = false) const
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
        if(width){
          _itoa(width, fmt, 10);
          fmt += strlen(fmt);
        }
      }

      if(is_long)
        *fmt++ = 'I', *fmt++ = '6', *fmt++ = '4';

      *fmt = bases[(basefield >> 5) - 1];

      if(basefield == ios_base::dec && is_signed)
        *fmt = 'd';
      else if(basefield == ios_base::hex && uppercase)
        *fmt = 'X';
      *++fmt = 0;

      char buf[32]; // may not enough if large width
      streamsize l = _snprintf(buf, _countof(buf), fmtbuf, is_long ? v : static_cast<unsigned long>(v));

      // adjust
      if(width && width > l){
        const streamsize pad = width - l;
        if(adjust != ios_base::left && adjust != ios_base::internal) // before
          out = __::fill_n(out, pad, fill);

        out = copy_n(buf,l,out);

        if(adjust == ios_base::left)
          out = __::fill_n(out, pad, fill);
      }else
        out = copy_n(buf,l,out);
      return out;
    }

};

//template <>
//class num_put<char>;// : public locale::facet

/** @} lib_locale_numeric */



/** \defgroup lib_locale_collate 22.2.4 The collate category [category.collate]
 *@{*/

// 22.2.4, collation:

template <class charT> class collate;
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

    //////////////////////////////////////////////////////////////////////////
    // use_facet
    template<class charT, class OutputIterator>
    static const num_put<charT, OutputIterator>& get_facet(const locale&, type2type<num_put<charT, OutputIterator> >/*, typename enable_if<is_same<charT,char>::value>::type* =0*/)
    {
      static const num_put<charT, OutputIterator> facet;
      return facet;
    }

    template<class charT>
    static const numpunct<charT>& get_facet(const locale&, type2type<numpunct<charT> >)
    {
      static const numpunct<charT> facet;
      return facet;
    }

    __forceinline
    static const ctype<char>& get_facet(const locale&, type2type<ctype<char> >)
    {
      // static ctype<char> is constructed with the default table which is not to be freed,
      // so the destructor call may be not queued up to the atexit function.
      // This is why it is implemented through placement new over a raw storage.
      static void * f[sizeof(ctype<char>)/sizeof(void*)];
      ctype<char> * const p = reinterpret_cast<ctype<char>*>(f);
      // The first class member is VTable ptr or the tab ptr member,
      // both are non-null after initialization is done.
      if ( !f[0] ) new (p) ctype<char>;
      return *p;
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
