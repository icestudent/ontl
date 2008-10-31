/**\file*********************************************************************
 *                                                                     \brief
 *  Initializer lists [18.8 support.initlist]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_INITLIST
#define NTL__STLX_INITLIST

#ifndef NTL__STLX_CSTDDEF
#include "cstddef.hxx" // for size_t
#endif

namespace std {

  /// Initializer lists [18.8 support.initlist]
  template<class E> 
  class initializer_list
  {
  public:
    initializer_list();
    size_t size() const;
    const E* begin() const;
    const E* end() const;
  };
}

#endif // NTL__STLX_INITLIST
