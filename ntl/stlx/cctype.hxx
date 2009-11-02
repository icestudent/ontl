/**\file*********************************************************************
 *                                                                     \brief
 *  21.5 Null-terminated sequence utilities [strings]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_CCTYPE
#define NTL__STLX_CCTYPE
#pragma once

namespace std
{
/**\addtogroup  lib_strings ************ 21 Strings library [strings]
 *@{*/
/**\addtogroup  lib_c_strings ********** 21.5 Null-terminated sequence utilities [c.strings]
 *@{*/
/**\addtogroup  lib_c_strings_cctype Character handling
 *@{*/

  ///\name Character classification functions
  int isalnum(int c);
  int isalpha(int c);
  int isblank(int c);
  int iscntrl(int c);
  int isdigit(int c);
  int isgraph(int c);
  int islower(int c);
  int isprint(int c);
  int ispunct(int c);
  int isspace(int c);
  int isupper(int c);
  int isxdigit(int c);

  ///\name Character case mapping functions
  int tolower(int c);
  int toupper(int c);
  ///\}

  /**@} lib_c_strings_cctype */
  /**@} lib_c_strings */
  /**@} lib_strings */
}
#endif //NTL__STLX_CCTYPE
