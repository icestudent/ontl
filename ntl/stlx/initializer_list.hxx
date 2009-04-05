/**\file*********************************************************************
 *                                                                     \brief
 *  18.8 Initializer lists [support.initlist]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_INITLIST
#define NTL__STLX_INITLIST

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx" // for size_t
#endif

namespace std
{
  /**\addtogroup  lib_language_support *** 18 Language support library [language.support] ******
   *@{*/

  /**\defgroup  lib_support_initlist ***** 18.8 Initializer lists [support.initlist]
   *@{*/

  /// Initializer lists [18.8 support.initlist]
  template<class E> 
  class initializer_list
  {
  public:
    initializer_list();
    size_t size() const;
    const E* begin() const { return nullptr; }
    const E* end() const { return nullptr; }
  };

  /** @} lib_support_initlist */
  /** @} lib_language_support */
} // namespace std

#endif // NTL__STLX_INITLIST
