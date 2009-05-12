/**\file*********************************************************************
 *                                                                     \brief
 *  Diagnostics library [19 lib.diagnostics]
 *  Exception classes [19.1 lib.std.exceptions]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_STDEXCEPT_FWD
#define NTL__STLX_STDEXCEPT_FWD

#ifndef NTL__STLX_EXCPTDEF
#include "excptdef.hxx"
#endif

namespace std
{
  class logic_error;
    class domain_error;
    class invalid_argument;
    class length_error;
    class out_of_range;

  class runtime_error;
    class range_error;
    class overflow_error;
    class underflow_error;

  void __throw_out_of_range(const char* msg = "out of range");
}

#if STLX__USE_EXCEPTIONS
# include "stdexcept.hxx"
#endif

#endif // NTL__STLX_STDEXCEPT_FWD
