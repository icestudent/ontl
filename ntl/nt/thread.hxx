/**\file*********************************************************************
*                                                                     \brief
*  User Threads
*
****************************************************************************
*/

#ifndef NTL__NT_THREAD
#define NTL__NT_THREAD

#include "basedef.hxx"
#include "../device_traits.hxx"
#include "object.hxx"
#include "teb.hxx"
#include "apc.hxx"
#include "csr.hxx"
#include "context.hxx"

namespace ntl {
  namespace nt {

struct initial_tib;

typedef
uint32_t __stdcall 
thread_start_routine_t(
   void* Parameter
   );

typedef 
ntstatus __stdcall
  control_thread_t(
    legacy_handle       ThreadHandle,
    uint32_t*           PreviousSuspendCount
    );


NTL__EXTERNAPI
control_thread_t NtResumeThread;

NTL__EXTERNAPI
control_thread_t NtAlertResumeThread;

NTL__EXTERNAPI
control_thread_t NtSuspendThread;


enum thread_info_class {
  ThreadBasicInformation,
  ThreadTimes,
  ThreadPriority,
  ThreadBasePriority,
  ThreadAffinityMask,
  ThreadImpersonationToken,
  ThreadDescriptorTableEntry,
  ThreadEnableAlignmentFaultFixup,
  ThreadEventPair,
  ThreadQuerySetWin32StartAddress,
  ThreadZeroTlsCell,
  ThreadPerformanceCount,
  ThreadAmILastThread,
  ThreadIdealProcessor,
  ThreadPriorityBoost,
  ThreadSetTlsArrayAddress,
  MaxThreadInfoClass
};


NTL__EXTERNAPI
ntstatus __stdcall
  NtQueryInformationThread (
    legacy_handle     ThreadHandle,
    thread_info_class ThreadInformationClass,
    void*             ThreadInformation,
    uint32_t          ThreadInformationLength,
    uint32_t*         ReturnLength
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtSetInformationThread (
    legacy_handle     ThreadHandle,
    thread_info_class ThreadInformationClass,
    void*             ThreadInformation,
    uint32_t          ThreadInformationLength
    );

class user_tread;
  }//namspace nt


  template<>
  struct device_traits<nt::user_tread>: private device_traits<>
  {
    enum access_mask
    {
      terminate                 = 0x0001,
      suspend_resume            = 0x0002,
      alert                     = 0x0004,
      get_context               = 0x0008,
      set_context               = 0x0010,
      set_information           = 0x0020,
      set_limited_information   = 0x0400,
      query_limited_information = 0x0800,
#if 0//(NTDDI_VERSION >= NTDDI_LONGHORN)
      all_access                = standard_rights_required | synchronize | 0xFFFF,
#else                                   
      all_access                = standard_rights_required | synchronize | 0x3FF,
#endif
    };
  };

namespace nt{

NTL__EXTERNAPI
ntstatus __stdcall
  NtCreateThread(
    handle*         ThreadHandle,
    uint32_t        DesiredAccess,
    object_attributes* ObjectAttributes,
    legacy_handle   ProcessHandle,
    client_id*      ClientId,
    context*        ThreadContext,
    initial_tib*    UserStack,
    bool            CreateSuspended
  );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlCreateUserThread(
    legacy_handle           Process,
    security_descriptor*    ThreadSecurityDescriptor,
    bool                    CreateSuspended,
    uint32_t                ZeroBits,
    size_t                  MaximumStackSize,
    size_t                  CommittedStackSize,
    thread_start_routine_t* StartAddress,
    void*                   Parameter,
    handle*                 Thread,
    client_id*              ClientId
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtOpenThread(
    handle*             ThreadHandle,
    uint32_t            DesiredAccess,
    object_attributes*  ObjectAttributes,
    client_id*          ClientId
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtTerminateThread(
    legacy_handle       ThreadHandle,
    ntstatus            ExitStatus
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtAlertThread(
    legacy_handle       ThreadHandle
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtGetContexThread(
    legacy_handle       ThreadHandle,
    context*            ThreadContext
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtSetContexThread(
    legacy_handle       ThreadHandle,
    context*            ThreadContext
    );


NTL__EXTERNAPI
ntstatus __stdcall
  NtQueueApcThread(
    legacy_handle       ThreadHandle,
    knormal_routine_t*  ApcRoutine,
    const void *        ApcContext,
    const void *        Argument1,
    const void *        Argument2
    );

NTL__EXTERNAPI
ntstatus __stdcall
  NtTerminateProcess(
    legacy_handle       ProcessHandle,
    ntstatus            ExitStatus
    );



NTL__EXTERNAPI
ntstatus __stdcall
  RtlAcquirePebLock();

NTL__EXTERNAPI
ntstatus __stdcall
  RtlReleasePebLock();

NTL__EXTERNAPI
void __stdcall
  RtlExitUserThread(ntstatus Status);

NTL__EXTERNAPI
void __stdcall
  RtlRaiseStatus(ntstatus Status);


NTL__EXTERNAPI
ntstatus __stdcall
  LdrShutdownProcess();

NTL__EXTERNAPI
ntstatus __stdcall
  LdrShutdownThread();


/************************************************************************/
/* user_thread                                                          */
/************************************************************************/
class user_tread:
  public handle, 
  public device_traits<user_tread>
{
  ////////////////////////////////////////////////////////////////////////////
public:

  typedef thread_start_routine_t start_routine_t;

  explicit
    user_tread(
    start_routine_t *   start_routine,
    void *              start_context   = 0,
    size_t              maximum_stack_size = 0,
    size_t              commited_stack_size = 0,
    bool                create_suspended = false,
    client_id *         client          = 0,
    security_descriptor* thread_security = 0) 
  {
    create(this, current_process(), start_routine, start_context, maximum_stack_size, commited_stack_size,
      create_suspended, client, thread_security);
  }

  user_tread(
    access_mask     DesiredAccess,
    client_id*      ClientId,
    bool            InheritHandle = false)
 {
    open(this, DesiredAccess, InheritHandle, ClientId);
  }

  static
    ntstatus
    create(
    handle *            thread_handle,
    legacy_handle       process_handle,
    start_routine_t *   start_routine,
    void *              start_context   = 0,
    size_t              maximum_stack_size = 0,
    size_t              commited_stack_size = 0,
    bool                create_suspended = false,
    client_id *         client          = 0,
    security_descriptor* thread_security = 0)
  {
    return RtlCreateUserThread(process_handle, thread_security, create_suspended, 0, 
      maximum_stack_size, commited_stack_size, start_routine, start_context, thread_handle, client);
  }

  static ntstatus
    open(
    handle*  ThreadHandle,
    access_mask     DesiredAccess,
    bool            InheritHandle,
    client_id*      ClientId)
  {
    object_attributes oa(InheritHandle ? object_attributes::inherit : object_attributes::none);
    return NtOpenThread(ThreadHandle, DesiredAccess, &oa, ClientId);
  }

  ntstatus control(bool suspend) 
  {
    return (suspend ? NtSuspendThread : NtResumeThread)(get(), NULL);
  }

  ntstatus suspend(uint32_t* PreviousSuspendCount = 0) const
  {
    return NtSuspendThread(get(), PreviousSuspendCount);
  }

  ntstatus resume(bool alert = false, uint32_t* PreviousSuspendCount = 0) const
  {
    return (alert ? NtAlertResumeThread : NtResumeThread)(get(), PreviousSuspendCount);
  }

  static void exit(ntstatus st)
  {
    RtlExitUserThread(st);
  }

  void exit_process(ntstatus st)
  {
    RtlAcquirePebLock();
    __try{
      NtTerminateProcess(NULL, st);
      LdrShutdownProcess();
      base_api_msg msg;
      msg.u.ExitProcess.uExitCode = st;
      CsrClientCallServer((csr_api_msg*) &msg, NULL, MAKE_API_NUMBER(1, BasepExitProcess), (uint32_t)sizeof(base_exitprocess_msg));
      NtTerminateProcess(current_process(), st);
    }
    __finally{
      RtlReleasePebLock();
    }
  }
};

  }
}
#endif // NTL__NT_THREAD
