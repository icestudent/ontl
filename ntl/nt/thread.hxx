/**\file*********************************************************************
 *                                                                     \brief
 *  User Threads
 *
 ****************************************************************************
 */
#ifndef NTL__NT_THREAD
#define NTL__NT_THREAD
#pragma once

#include "basedef.hxx"
#include "../device_traits.hxx"
#include "object.hxx"
#include "apc.hxx"
#include "csr.hxx"
#include "context.hxx"

#include "../pe/image.hxx"

#include "resource.hxx"
#include "time.hxx"

namespace ntl {
  namespace nt {

    __class_enum(thread_state)
    {
        Initialized,
        Ready,
        Running,
        Standby,
        Terminated,
        Wait,
        Transition,
        Unknown
    };};

struct initial_tib;

/** Thread start prototype */
typedef
uint32_t __stdcall thread_start_routine_t(void* Parameter);

typedef
ntstatus __stdcall
  control_thread_t(
    legacy_handle       ThreadHandle,
    uint32_t*           PreviousSuspendCount
    );


NTL_EXTERNAPI
control_thread_t NtResumeThread;

NTL_EXTERNAPI
control_thread_t NtAlertResumeThread;

NTL_EXTERNAPI
control_thread_t NtSuspendThread;


class user_thread;
  }//namspace nt


  template<>
  struct device_traits<nt::user_thread>: private device_traits<>
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
      all_access                = (int)standard_rights_required | (int)synchronize | 0x3FF,
#endif
    };

    friend access_mask operator | (access_mask m, access_mask m2)
    {
      return bitwise_or(m, m2);
    }

    friend access_mask operator | (access_mask m, nt::access_mask m2)
    {
      return m | static_cast<access_mask>(m2);
    }

    friend access_mask operator | (nt::access_mask m, access_mask m2)
    {
      return m2 | m;
    }
  };

namespace nt{

NTL_EXTERNAPI
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

NTL_EXTERNAPI
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

NTL_EXTERNAPI
ntstatus __stdcall
  NtOpenThread(
    handle*             ThreadHandle,
    uint32_t            DesiredAccess,
    object_attributes*  ObjectAttributes,
    const client_id*    ClientId
    );

NTL_EXTERNAPI
ntstatus __stdcall
  NtTerminateThread(
    legacy_handle       ThreadHandle,
    ntstatus            ExitStatus
    );

NTL_EXTERNAPI
ntstatus __stdcall
  NtAlertThread(
    legacy_handle       ThreadHandle
    );

NTL_EXTERNAPI
ntstatus __stdcall
  NtGetContexThread(
    legacy_handle       ThreadHandle,
    context*            ThreadContext
    );

NTL_EXTERNAPI
ntstatus __stdcall
  NtSetContexThread(
    legacy_handle       ThreadHandle,
    context*            ThreadContext
    );


NTL_EXTERNAPI
ntstatus __stdcall
  NtQueueApcThread(
    legacy_handle       ThreadHandle,
    knormal_routine_t*  ApcRoutine,
    const void *        ApcContext,
    const void *        Argument1,
    const void *        Argument2
    );

NTL_EXTERNAPI
//__declspec(noreturn)
ntstatus __stdcall
  NtTerminateProcess(
    legacy_handle       ProcessHandle,
    ntstatus            ExitStatus
    );



NTL_EXTERNAPI
ntstatus __stdcall
  RtlAcquirePebLock();

NTL_EXTERNAPI
ntstatus __stdcall
  RtlReleasePebLock();

NTL_EXTERNAPI
void __stdcall
  RtlRaiseStatus(ntstatus Status);


NTL_EXTERNAPI
ntstatus __stdcall
  LdrShutdownProcess();

NTL_EXTERNAPI
ntstatus __stdcall
  LdrShutdownThread();

NTL_EXTERNAPI
ntstatus __stdcall
  LdrUnloadDll(legacy_handle DllHandle);

#ifndef NTL_SUPPRESS_IMPORT

NTL_EXTERNAPI
__declspec(noreturn)
void __stdcall
  RtlExitUserThread(ntstatus Status);

#endif

NTL_EXTERNAPI
__declspec(noreturn)
void __stdcall
  FreeLibraryAndExitThread(
    legacy_handle DllHandle,
    uint32_t      ExitCode
    );


/************************************************************************/
/* user_thread                                                          */
/************************************************************************/
class user_thread:
  public handle,
  public device_traits<user_thread>,
  public last_status_t
{
  ////////////////////////////////////////////////////////////////////////////
public:

  typedef thread_start_routine_t start_routine_t;

  /** Create thread */
  explicit
    user_thread(
    start_routine_t *   start_routine,
    void *              start_context   = 0,
    bool                create_suspended = false,
    client_id *         client          = 0,
    size_t              maximum_stack_size = 0,
    size_t              commited_stack_size = 0,
    security_descriptor* thread_security = 0)
  {
    last_status_ = create(this, current_process(), start_routine, start_context, maximum_stack_size, commited_stack_size,
      create_suspended, client, thread_security);
  }

  /** Open thread */
  user_thread(
    access_mask      DesiredAccess,
    const client_id& ClientId,
    bool             InheritHandle = false)
  {
    last_status_ = open(this, DesiredAccess, InheritHandle, &ClientId);
  }

  static ntstatus
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
#ifdef NTL__CSRSS_NOTIFY_THREAD
    client_id cid;
    if ( !client ) client = &cid;
#endif
    ntstatus s = RtlCreateUserThread(process_handle, thread_security, create_suspended, 0,
      maximum_stack_size, commited_stack_size, start_routine, start_context, thread_handle, client);
#ifdef NTL__CSRSS_NOTIFY_THREAD
    if ( success(s) )
    {
      // WoW needs 64 bit message from 32 bit code, try brute force as a quick hack
      union msg_t {
        struct { base_api_msg_64 msg64; uint64_t createthread_msg[3]; } msg64;
        base_api_msg msg;
      };
      msg_t msg = {};
      msg.msg.u.CreateThread.ThreadHandle = thread_handle->get();
      msg.msg.u.CreateThread.ClientId = *client;
      //This is somehow success on 2003x64
      //s = static_cast<ntstatus>(msg.msg.ReturnValue);
      s = CsrClientCallServer((csr_api_msg*)&msg, nullptr, MAKE_API_NUMBER(1, BasepCreateThread), (uint32_t)sizeof(base_createthread_msg));
#ifndef _M_X64//we done this already above
      if ( !success(s) )
      {
        msg.msg64.createthread_msg[0] = reinterpret_cast<uint64_t>(thread_handle->get());
        msg.msg64.createthread_msg[1] = reinterpret_cast<uint64_t>(client->UniqueProcess);
        msg.msg64.createthread_msg[2] = reinterpret_cast<uint64_t>(client->UniqueThread);
        //s = static_cast<ntstatus>(msg.msg64.msg64.ReturnValue);
        s = CsrClientCallServer((csr_api_msg*)&msg, nullptr, MAKE_API_NUMBER(1, BasepCreateThread), (uint32_t)sizeof(msg.msg64.createthread_msg));
      }
    }
#endif//_M_X64
#endif//NTL__CSRSS_NOTIFY_THREAD
    return s;
  }

  static ntstatus
  open(
    handle*         ThreadHandle,
    access_mask     DesiredAccess,
    bool            InheritHandle,
    const client_id*ClientId)
  {
    object_attributes oa(InheritHandle ? object_attributes::inherit : object_attributes::none);
    return NtOpenThread(ThreadHandle, DesiredAccess, &oa, ClientId);
  }

  ntstatus control(bool suspend) const volatile
  {
    return last_status_ = (suspend ? NtSuspendThread : NtResumeThread)(get(), NULL);
  }

  ntstatus suspend(uint32_t* PreviousSuspendCount = 0) const volatile
  {
    return last_status_ = NtSuspendThread(get(), PreviousSuspendCount);
  }

  ntstatus resume(bool alert = false, uint32_t* PreviousSuspendCount = 0) const volatile
  {
    return last_status_ = (alert ? NtAlertResumeThread : NtResumeThread)(get(), PreviousSuspendCount);
  }

  template <class Clock, class Duration>
  ntstatus wait_until(const std::chrono::time_point<Clock, Duration>& abs_time, bool alertable = true) const volatile
  {
    return last_status_ = NtWaitForSingleObject(get(), alertable, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count());
  }

  template <class Rep, class Period>
  ntstatus wait_for(const std::chrono::duration<Rep, Period>& rel_time, bool alertable = true) const volatile
  {
    return last_status_ = NtWaitForSingleObject(get(), alertable, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
  }

  ntstatus wait(bool alertable = true) const volatile
  {
    return last_status_ = NtWaitForSingleObject(get(), alertable, system_time::infinite());
  }

  static legacy_handle get_current() { return current_thread(); }

  static ntstatus exitstatus(legacy_handle thread_handle)
  {
    thread_information<thread_basic_information> info(thread_handle);
    return info ? info->ExitStatus : info;
  }

  ntstatus exitstatus () const
  {
    return exitstatus(get());
  }

  bool alive() const
  {
    return exitstatus() == status::still_active;
  }

  legacy_handle get_id() const
  {
    thread_information<thread_basic_information> info(get());
    return info ? info->ClientId.UniqueThread : 0;
  }

  static __declspec(noreturn)
  void exit(ntstatus Status)
  {
#ifndef NTL_SUPPRESS_IMPORT
    RtlExitUserThread(Status);
#else
    LdrShutdownThread();
    // TODO: set the 'free stack on termination'
    NtTerminateThread(get_current(), Status);
#endif
  }

  static __declspec(noreturn)
  void exitfree(ntstatus Status)
  {
    FreeLibraryAndExitThread((legacy_handle) pe::image::this_module(), Status);
  }

  ntstatus term(ntstatus Status)
  {
    return NtTerminateThread(get(), Status);
  }

  static __declspec(noreturn)
  void exit_process(ntstatus Status)
  {
    RtlAcquirePebLock();
    __try{
      NtTerminateProcess(NULL, Status);
      LdrShutdownProcess();
      base_api_msg msg;
      msg.u.ExitProcess.uExitCode = Status;
      CsrClientCallServer((csr_api_msg*) &msg, NULL, MAKE_API_NUMBER(1, BasepExitProcess), (uint32_t)sizeof(base_exitprocess_msg));
      NtTerminateProcess(current_process(), Status);
    }
    __finally{
      RtlReleasePebLock();
    }
  }

#ifdef NTL_CXX_RV
  user_thread(user_thread&& rhs)
  {
    reset(rhs.release());
  }

  user_thread& operator=(user_thread&& rhs)
  {
    reset(rhs.release());
    return *this;
  }

private:
  user_thread(const user_thread&) __deleted;
  user_thread& operator=(const user_thread&) __deleted;
public:
#else
  user_thread(user_thread& rhs)
  {
    reset(rhs.release());
  }

  user_thread& operator=(user_thread& rhs)
  {
    reset(rhs.release());
    return *this;
  }

  void swap(user_thread& rhs)
  {
    handle::swap(rhs);
  }
#endif

}; // class user_thread

  template <class Clock, class Duration>
  static inline ntstatus sleep_until(const std::chrono::time_point<Clock, Duration>& abs_time, bool alertable = false)
  {
    return NtDelayExecution(alertable, std::chrono::duration_cast<system_duration>(abs_time.time_since_epoch()).count());
  }

  template <class Rep, class Period>
  static inline ntstatus sleep_for(const std::chrono::duration<Rep, Period>& rel_time, bool alertable = false)
  {
    return NtDelayExecution(alertable, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
  }

 } // namespace nt

}
#endif // NTL__NT_THREAD
