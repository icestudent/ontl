/**\file*********************************************************************
 *                                                                     \brief
 *  Win API Handles support
 *
 ****************************************************************************
 */


#ifndef NTL__WIN_HANDLE
#define NTL__WIN_HANDLE

#include "windef.hxx"
#include "../nt/handle.hxx"

namespace ntl {
namespace win {


using nt::legacy_handle;

static const legacy_handle null_handle;

static inline legacy_handle current_process()
{
  return nt::current_process();
}


using nt::duplicate_options;
using nt::duplicate_close_source;
using nt::duplicate_same_access;
using nt::duplicate_same_attributes;


NTL__EXTERNAPI
int __stdcall
  DuplicateHandle(
    legacy_handle   hSourceProcessHandle,
    legacy_handle   hSourceHandle,
    legacy_handle   hTargetProcessHandle,
    legacy_handle * lpTargetHandle,
    access_mask     dwDesiredAccess,
    bool            bInheritHandle,
    uint32_t        dwOptions
    );


static __forceinline
legacy_handle
  duplicate(
    legacy_handle     source_handle,
    legacy_handle     source_process  = current_process(),
    legacy_handle     target_process  = current_process(),
    access_mask       desired_access  = win::no_access,
    bool              inherit_handle  = false,
    duplicate_options options         = duplicate_same_access | duplicate_same_attributes
    )
{
  legacy_handle h = { 0 };
  // returns valid handle or 0
  DuplicateHandle(source_process, source_handle, target_process, &h,
                    desired_access, inherit_handle, options);
  return h;
}


NTL__EXTERNAPI
int __stdcall
  CloseHandle(legacy_handle hObject);

static __forceinline
int close(legacy_handle handle)
{
  return CloseHandle(handle);
}


namespace aux {

__forceinline
void close(legacy_handle handle)
{
  nt::close(handle);
}

__forceinline
legacy_handle duplicate(legacy_handle handle)
{
  return nt::duplicate(handle);
}

} //namespace aux

typedef basic_handle<legacy_handle, aux::close, aux::duplicate> handle;


}//namespace win
}//namespace ntl

#endif//#ifndef NTL__WIN_HANDLE
