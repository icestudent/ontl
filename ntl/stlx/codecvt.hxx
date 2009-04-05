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

  template<class Elem, unsigned long Maxcode = 0x10ffff, codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf8
    : public codecvt<Elem, char, mbstate_t>
  {
      // unspecified
  };

  template<class Elem, unsigned long Maxcode = 0x10ffff,
    codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf16
    : public codecvt<Elem, char, mbstate_t>
  {
      // unspecified
  };

  template<class Elem, unsigned long Maxcode = 0x10ffff,
    codecvt_mode Mode = (codecvt_mode)0>
  class codecvt_utf8_utf16
    : public codecvt<Elem, char, mbstate_t>
  {
      // unspecified
  };


  /** @} */
  /** @} lib_locale */

} // std
#endif // NTL__STLX_CODECVT
