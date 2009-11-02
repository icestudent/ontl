/**\file*********************************************************************
 *                                                                     \brief
 *  18.8 Initializer lists [support.initlist]
 *
 ****************************************************************************
 */
#ifndef NTL__STLX_INITLIST
#define NTL__STLX_INITLIST
#pragma once

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx" // for size_t
#endif

namespace std
{
  /**\addtogroup  lib_language_support *** 18 Language support library [language.support]
   *@{*/

  /**\defgroup  lib_support_initlist ***** 18.8 Initializer lists [support.initlist]
   *@{*/

  /// Initializer lists [18.8 support.initlist]
  template<class E> 
  class initializer_list
  {
  public:
    typedef size_t    size_type;
    typedef       E   value_type;
    typedef const E&  reference;
    typedef const E&  const_reference;

    typedef const E*  iterator;
    typedef const E*  const_iterator;

#ifndef NTL__CXX_IL
    size_t   size()  const { return 0; }
    const E* begin() const { return nullptr; }
    const E* end()   const { return nullptr; }
#else
    size_t   size()  const;
    const E* begin() const;
    const E* end()   const;
    initializer_list();
#endif
  };

  ///\name 18.9.3 Initializer list range access [support.initlist.range]
  template<class E> inline const E* begin(initializer_list<E> il) { return il.begin(); }
  template<class E> inline const E* end  (initializer_list<E> il) { return il.end();   }
  ///\}

  /** @} lib_support_initlist */
  /** @} lib_language_support */
} // namespace std

#endif // NTL__STLX_INITLIST
