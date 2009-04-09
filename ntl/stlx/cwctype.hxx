/**\file*********************************************************************
 *                                                                     \brief
 *  21.5 Null-terminated sequence utilities [strings]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CWCTYPE
#define NTL__STLX_CWCTYPE

#include "cwchar.hxx"

typedef unsigned short wctype_t;
typedef wchar_t  wctrans_t;

namespace std
{
/**\addtogroup  lib_strings ************ 21 Strings library [strings] ****************
 *@{*/
/**\addtogroup  lib_c_strings ********** 21.5 Null-terminated sequence utilities [c.strings]
 *@{*/
/**\addtogroup  lib_c_strings_cwctype Wide character classification and mapping utilities
 *@{*/

  ///\name Wide character classification functions
  int iswalnum(wint_t wc);
  int iswalpha(wint_t wc);
  int iswblank(wint_t wc);
  int iswcntrl(wint_t wc);
  int iswdigit(wint_t wc);
  int iswgraph(wint_t wc);
  int iswlower(wint_t wc);
  int iswprint(wint_t wc);
  int iswpunct(wint_t wc);
  int iswspace(wint_t wc);
  int iswupper(wint_t wc);
  int iswxdigit(wint_t wc);

  ///\name Extensible wide character classification functions
  int iswctype(wint_t wc, wctype_t desc);
  wctype_t wctype(const char *property);

  ///\name Wide character case mapping utilities
  wint_t towlower(wint_t wc);
  wint_t towupper(wint_t wc);

  ///\name Extensible wide character case mapping functions
  wint_t towctrans(wint_t wc, wctrans_t desc);
  wctrans_t wctrans(const char *property);
  ///\}

  /**@} lib_c_strings_cwctype */
  /**@} lib_c_strings */
  /**@} lib_strings */
}
#endif // NTL__STLX_CWCTYPE
