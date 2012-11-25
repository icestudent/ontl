/**\file*********************************************************************
 *                                                                     \brief
 *  18.9 Initializer lists [support.initlist]
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

  /**\addtogroup  lib_support_initlist ***** 18.9 Initializer lists [support.initlist]
   *@{*/

  /// Initializer lists [18.9 support.initlist]
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

    size_t   size()  const { return end_ - begin_;}
    const E* begin() const { return begin_;       }
    const E* end()   const { return end_;         }

    /** constructs an empty initializer_list object */
    initializer_list()
      :begin_(), end_()
    {}

    /** constructs from pointer plus a length */
    initializer_list(iterator first, size_type count)
      :begin_(first), end_(first+count)
    {}
    /** constructs from pair of pointers */
    initializer_list(const_iterator begin, const_iterator end)
      :begin_(begin), end_(end)
    {}

  private:
    const_iterator begin_, end_;
  };

  ///\name 18.9.3 Initializer list range access [support.initlist.range]
  template<class E> inline const E* begin(initializer_list<E> il) { return il.begin(); }
  template<class E> inline const E* end  (initializer_list<E> il) { return il.end();   }
  ///\}

  /** @} lib_support_initlist */
  /** @} lib_language_support */
} // namespace std

#endif // NTL__STLX_INITLIST
