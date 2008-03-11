/**\file*********************************************************************
 *                                                                     \brief
 *  Asynchronous Procedure Call
 *
 ****************************************************************************
 */


#ifndef NTL__KM_APC
#define NTL__KM_APC

#include "basedef.hxx"
#include "../nt/apc.hxx"

namespace ntl {
namespace km {

using nt::knormal_routine_t;
struct kprocess;

/// KAPC_STATE
struct kapc_state
{
  list_entry  ApcListHead[2];
  kprocess *  Process;
  bool        KernelApcInProgress;
  bool        KernelApcPending;
  bool        UserApcPending;
};


struct kapc;

typedef
void __stdcall
  kkernel_routine_t(
    const kapc *  Apc,
    knormal_routine_t * NormalRoutine,
    void *              NormalContext,
    void *              SystemArgument1,
    void *              SystemArgument2
    );

typedef
void __stdcall
  krundown_routine_t(
    const kapc *  Apc
    );


NTL__EXTERNAPI
bool __stdcall
  KeInsertQueueApc(
    kapc *    Apc,
    void *    SystemArgument1,
    void *    SystemArgument2,
    kpriority Increment
    );


struct kthread;

struct kapc
{
  enum environment
  {
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment
  };
  uint8_t             Type;
  uint8_t             SpareByte0;
  uint8_t             Size;
  uint8_t             SpareByte1;
  uint32_t            SpareLong0;
  kthread *           Thread;
  list_entry          ApcListEntry;
  kkernel_routine_t * KernelRoutine;
  krundown_routine_t* RundownRoutine;
  knormal_routine_t * NormalRoutine;
  void *              NormalContext;
  void *              SystemArgument1;
  void *              SystemArgument2;
  uint8_t             ApcStateIndex;
  kprocessor_mode     ApcMode;
  bool                Inserted;

  void* operator new      (std::size_t size) throw()
  {
    return ::operator new(size);
  }

  void  operator delete   (void* ptr) throw()
  {
    ::delete(ptr);
  }
};


struct apc : kapc
{
  inline
  apc(
    kthread *           Thread,
    kapc::environment   Environment,
    kkernel_routine_t * KernelRoutine,
    krundown_routine_t* RundownRoutine  = 0,
    knormal_routine_t * NormalRoutine   = 0,
    kprocessor_mode     ApcMode         = KernelMode,
    void *              NormalContext   = 0
    );

  inline
  apc(
    kthread *           Thread,
    knormal_routine_t * NormalRoutine,
    void *              NormalContext   = 0
    );

  bool queue(
    void *    system_argument1  = 0,
    void *    system_argument2  = 0,
    kpriority increment         = 0)
  {
    return KeInsertQueueApc(this, system_argument1, system_argument2, increment);
  }

  static 
  void __stdcall
    default_kernel_routine(const kapc * Apc, knormal_routine_t *, void *, void *, void *)
  {
    delete(Apc);
  }

};
STATIC_ASSERT(sizeof(kapc) == 0x30 || sizeof(kapc) == 0x58);


NTL__EXTERNAPI
void __stdcall
  KeInitializeApc(
    kapc *              Apc,
    kthread *           Thread,
    kapc::environment   Environment,
    kkernel_routine_t * KernelRoutine,
    krundown_routine_t* RundownRoutine  __optional,
    knormal_routine_t * NormalRoutine   __optional,
    kprocessor_mode     ApcMode         __optional,
    void *              NormalContext   __optional
    );

apc::apc(
    kthread *           Thread,
    kapc::environment   Environment,
    kkernel_routine_t * KernelRoutine,
    krundown_routine_t* RundownRoutine,
    knormal_routine_t * NormalRoutine,
    kprocessor_mode     ApcMode,
    void *              NormalContext
    )
{
  KeInitializeApc(this, Thread, Environment, KernelRoutine, RundownRoutine,
                  NormalRoutine, ApcMode, NormalContext);
}

apc::apc(
    kthread *           Thread,
    knormal_routine_t * NormalRoutine,
    void *              NormalContext
    )
{
  KeInitializeApc(this, Thread, OriginalApcEnvironment, default_kernel_routine, 0,
                  NormalRoutine, UserMode, NormalContext);
}


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_APC
