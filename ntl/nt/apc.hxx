/**\file*********************************************************************
*                                                                     \brief
*  Asynchronous Procedure Call
*
****************************************************************************
*/


#ifndef NTL__NT_APC
#define NTL__NT_APC

#include "basedef.hxx"

namespace ntl {
  namespace nt {

typedef
void __stdcall
  knormal_routine_t(
    const void *  NormalContext,
    const void *  SystemArgument1,
    const void *  SystemArgument2
    );

  }
}

#endif // NTL__NT_APC