/**\file*********************************************************************
 *                                                                     \brief
 *  22.3 Standard code conversion facets [locale.stdcvt]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CODECVT
#define NTL__STLX_CODECVT

#include "locale.hxx"

namespace std
{
  /** \addtogroup lib_locale        22 Localization library [localization]
   *@{*/
  /** \addtogroup lib_locale_stdcvt 22.3 Standard code conversion facets [locale.stdcvt]
   *@{*/


  /** codecvt mode */
  enum codecvt_mode
  {
    /** the facet shall consume an initial header sequence */
    consume_header = 4,
    /** the facet shall generate an initial header sequence */
    generate_header = 2,
    /** the facet shall generate a multibyte sequence in little-endian order, 
      as opposed to the default big-endian order */
    little_endian = 1
  };


  /**
   *	@brief Class template codecvt_utf8
   *  @details The facet shall convert between UTF-8 multibyte sequences and UCS2 or UCS4 (depending on the
   *  size of \e Elem) within the program.
   **/
  template<class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf8
    : public codecvt<Elem, char, mbstate_t>
  {
  protected:
    virtual result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const;

    virtual result do_unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const;

    virtual int do_max_length() const __ntl_nothrow;
  };

  
  /**
   *	@brief Class template codecvt_utf16
   *  @details The facet shall convert between UTF-16 multibyte sequences and UCS2 or UCS4 (depending on the
   *  size of \e Elem) within the program
   **/
  template<class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf16
    : public codecvt<Elem, char, mbstate_t>
  {
  protected:
    virtual result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const;

    virtual result do_unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const;

    virtual int do_max_length() const __ntl_nothrow;
  };


  /**
   *	@brief Class template codecvt_utf8_utf16
   *  @details The facet shall convert between UTF-8 multibyte sequences and UTF-16 (one or two 16-bit codes)
   *  within the program.
   **/
  template<class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf8_utf16
    : public codecvt<Elem, char, mbstate_t>
  {
  protected:
    virtual result do_out(state_type& state, const intern_type* from, const intern_type* from_end,
      const intern_type*& from_next, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual result do_in(state_type& state, const extern_type* from, const extern_type* from_end,
      const extern_type*& from_next, intern_type* to, intern_type* to_limit, intern_type*& to_next) const;

    virtual result do_unshift(state_type& state, extern_type* to, extern_type* to_limit, extern_type*& to_next) const;

    virtual int do_encoding() const __ntl_nothrow;

    virtual bool do_always_noconv() const __ntl_nothrow;

    virtual int do_length(state_type&, const extern_type* from, const extern_type* end, size_t max) const;

    virtual int do_max_length() const __ntl_nothrow;
  };


  /** @} */
  /** @} lib_locale */

} // std
#endif // NTL__STLX_CODECVT
