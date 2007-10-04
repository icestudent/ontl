/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
 *
 ****************************************************************************
 */

#ifndef NTL__KM_PROCESS
#define NTL__KM_PROCESS

#include "basedef.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "object.hxx"
#include "thread.hxx"
#include "system_information.hxx"
#include "../pe/image.hxx"


namespace ntl {
namespace km {


struct kprocess;
struct eprocess;

NTL__EXTERNAPI
kprocess * __stdcall
  IoGetCurrentProcess();


struct kprocess
{
  static 
  kprocess * get_current()
  {
#ifndef NTL_SUPPRESS_IMPORT
    return IoGetCurrentProcess();
#else
    return kthread::get_current()->ApcState.Process;    
#endif
  }

};


//struct eprocess;


///\note XP+ only, use IoGetCurrentProcess instead
NTL__EXTERNAPI
kprocess * __stdcall
  PsGetCurrentProcess();

///\note XP+ only
NTL__EXTERNAPI
char * __stdcall
  PsGetProcessImageFileName(kprocess *);


NTL__EXTERNAPI
ntstatus __stdcall
  PsLookupProcessByProcessId(
    legacy_handle ProcessId,
    kprocess **   Process
    );


static inline
kprocess * lookup_process(legacy_handle process_id)
{
  kprocess * process;
  return nt::success(PsLookupProcessByProcessId(process_id, &process))
    ? process : 0;
}


static __forceinline
void dereference_object(kprocess * process)
{
  ObfDereferenceObject(process);
}



NTL__EXTERNAPI
void __stdcall
  KeStackAttachProcess(
    kprocess *    Process,
    kapc_state *  ApcState
    );

NTL__EXTERNAPI
void __stdcall
  KeUnstackDetachProcess(
    kapc_state *  ApcState
    );


struct nt::peb;

typedef
nt::peb * __stdcall
  get_process_peb_t(kprocess * Process);

// XP+ only
NTL__EXTERNAPI
get_process_peb_t PsGetProcessPeb;


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_PROCESS
