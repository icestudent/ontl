#ifdef NTL__SUBSYSTEM_KM
# include "../km/basedef.hxx"
#else
# include "../nt/basedef.hxx"
#endif
#include "../stlx/system_error.hxx"
#include "../stlx/cstdlib.hxx"

// NT system error support

# include "../nt/system_error.hxx"

#ifdef NTL__CXX_RV
#include "../stlx/future.hxx"
#endif

namespace std
{
}
