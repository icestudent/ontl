/**\file*********************************************************************
 *                                                                     \brief
 *  Process and Thread Information
 *
 ****************************************************************************
 */
#ifndef NTL__NT_PROCESS_INFORMATION
#define NTL__NT_PROCESS_INFORMATION

#include "teb.hxx"

#include "../stlx/type_traits.hxx"

namespace ntl { namespace nt {
  struct peb;

  /**\defgroup process_information Process Information
   *@{*/

  /// Process Information Class
  enum process_information_class
  {
    // RO
    ProcessBasicInformation,
    ProcessQuotaLimits,
    // RO
    ProcessIoCounters,
    // RO
    ProcessVmCounters,
    // RO
    ProcessTimes,
    // WO
    ProcessBasePriority,
    // WO
    ProcessRaisePriority,
    ProcessDebugPort,
    // WO
    ProcessExceptionPort,
    // WO
    ProcessAccessToken,
    ProcessLdtInformation,
    // WO
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    // WO
    ProcessIoPortHandlers,
    // RO
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    // WO
    ProcessUserModeIOPL,
    // WO
    ProcessEnableAlignmentFaultFixup,
    // WO
    ProcessPriorityClass,
    // RO
    ProcessWx86Information,
    // RO
    ProcessHandleCount,
    // WO
    ProcessAffinityMask,
    ProcessPriorityBoost,
    ProcessDeviceMap,
    ProcessSessionInformation,
    // WO
    ProcessForegroundInformation,
    // RO
    ProcessWow64Information,
    // RO
    ProcessImageFileName,
    // RO
    ProcessLUIDDeviceMapsEnabled,
    ProcessBreakOnTermination,
    // RO
    ProcessDebugObjectHandle,
    ProcessDebugFlags,
    ProcessHandleTracing,
    // nt 6.0+
    ProcessIoPriority,
    ProcessExecuteFlags,
    // not used or vista+
    ProcessResourceManagement,
    // RO
    ProcessCookie,
    ProcessImageInformation,
    MaxProcessInfoClass
  };


typedef
ntstatus __stdcall
  query_process_information_t(
    legacy_handle             ProcessHandle,
    process_information_class ProcessInformationClass,
    void *                    ProcessInformation,
    uint32_t                  ProcessInformationLength,
    uint32_t *                ReturnLength
    );

typedef
ntstatus __stdcall
  set_process_information_t(
    legacy_handle             ProcessHandle,
    process_information_class ProcessInformationClass,
    const void *              ProcessInformation,
    uint32_t                  ProcessInformationLength
    );

NTL__EXTERNAPI query_process_information_t NtQueryInformationProcess;
NTL__EXTERNAPI set_process_information_t   NtSetInformationProcess;

//////////////////////////////////////////////////////////////////////////
namespace aux
{
  template<bool value = true>
  struct is_read_only:
    std::integral_constant<bool, value>
  {};

  template<bool value = false>
  struct is_write_only:
    std::integral_constant<bool, value>
  {};

  typedef is_read_only<true>  read_only;
  typedef is_write_only<true> write_only;


  /// default query policy
  template<class                        InformationClass,
          class                         QueryInformationT,
          bool                          write_only>
  struct query_information_policy
  {
    static __forceinline
      ntstatus
      _query(
      QueryInformationT             QueryInformation,
      legacy_handle   handle,
      void *          info,
      uint32_t        info_length
      )
    {
      return QueryInformation(handle, InformationClass::info_class_type, info, info_length, nullptr);
    }
  protected:
    InformationClass info;
  };

  /// write-only policy
  template<class                  InformationClass,
          class                   QueryInformationT>
  struct query_information_policy<InformationClass, QueryInformationT, true>
  {
    static __forceinline
      ntstatus
      _query(
      QueryInformationT,
      legacy_handle,
      void *,
      uint32_t
      )
    {
      return status::invalid_info_class;
    }
  protected:
    int info; // dummy
  };

  template <class                       InformationClass,
            class                       SetInformationT,
            bool                        read_only>
  struct set_information_policy
  {
    static const bool is_read_only = false;

      static __forceinline
      ntstatus
        _set(
          SetInformationT SetInformation,
          legacy_handle   handle,
          const void *    info,
          uint32_t        info_length
          )
      {
        return SetInformation(handle, InformationClass::info_class_type, info, info_length);
      }
  };

  template <class                       InformationClass,
            class                       SetInformationT>
  struct set_information_policy<InformationClass, SetInformationT, true>
  {
    static const bool is_read_only = true;

    static __forceinline
      ntstatus
      _set(
      SetInformationT,
      legacy_handle,
      const void *,
      uint32_t
      )
    {
      return status::invalid_info_class;
    }
  };

} // aux

template <class                       InformationClass,
          query_process_information_t QueryInformation,
          set_process_information_t   SetInformation>
struct process_information_base:
  aux::query_information_policy<InformationClass, query_process_information_t, std::is_base_of<aux::write_only, InformationClass>::value>,
  aux::set_information_policy<InformationClass, set_process_information_t, std::is_base_of<aux::read_only, InformationClass>::value>
{
    typedef InformationClass info_class;

    process_information_base(legacy_handle handle) __ntl_nothrow
    {
      static_assert((std::is_base_of<aux::write_only, InformationClass>::value == false), "Cannot query a write-only information class");
      status_ = _query(QueryInformation, handle, &info, sizeof(info));
    }

    process_information_base(
      legacy_handle       handle,
      const info_class &  new_info
      ) __ntl_nothrow
    {
      static_assert(is_read_only == false, "Cannot set a read-only information class");
      status_ = _set(SetInformation, handle, &new_info, sizeof(info_class));
    }

    info_class * data() { return nt::success(status_) ? &info : 0; }
    const info_class * data() const { return nt::success(status_) ? &info : 0; }
    info_class * operator->() { return data(); }
    const info_class * operator->() const { return data(); }

    operator bool() const { return nt::success(status_); }
    operator ntstatus() const { return status_; }

  protected:
    ntstatus    status_;
}; //class process_information_base


template<class ProcessInformationClass>
struct process_information
: public process_information_base <ProcessInformationClass, NtQueryInformationProcess, NtSetInformationProcess>
{
  typedef process_information_base <ProcessInformationClass, NtQueryInformationProcess, NtSetInformationProcess> base;
  process_information(legacy_handle process_handle = current_process()) __ntl_nothrow
  : base(process_handle)
  {/**/}

  process_information(
    legacy_handle             process_handle,
    const ProcessInformationClass &  info) __ntl_nothrow
  : base(process_handle, info)
  {/**/}
};


//////////////////////////////////////////////////////////////////////////

#ifndef _M_X64
# pragma pack(push, 4)
#else
# pragma pack(push, 8)
#endif

///\name  ProcessBasicInformation == 0
struct process_basic_information: aux::read_only
{
  static const process_information_class info_class_type = ProcessBasicInformation;

  ntstatus      ExitStatus;
  peb*          PebBaseAddress;
  uintptr_t     AffinityMask;
  km::kpriority BasePriority;
  legacy_handle UniqueProcessId;
  legacy_handle InheritedFromUniqueProcessId;
};

///\name  ProcessIoCounters == 2
struct process_io_counters: aux::read_only
{
  static const process_information_class info_class_type = ProcessIoCounters;

  uint64_t ReadOperationCount;
  uint64_t WriteOperationCount;
  uint64_t OtherOperationCount;
  uint64_t ReadTransferCount;
  uint64_t WriteTransferCount;
  uint64_t OtherTransferCount;
};

///\name  ProcessVmCounters == 3
struct process_vm_counters: aux::read_only
{
  static const process_information_class info_class_type = ProcessVmCounters;

  size_t PeakVirtualSize;
  size_t VirtualSize;
  uint32_t PageFaultCount;
  size_t PeakWorkingSetSize;
  size_t WorkingSetSize;
  size_t QuotaPeakPagedPoolUsage;
  size_t QuotaPagedPoolUsage;
  size_t QuotaPeakNonPagedPoolUsage;
  size_t QuotaNonPagedPoolUsage;
  size_t PagefileUsage;
  size_t PeakPagefileUsage;
};

///\name  ProcessVmCounters == 3
struct process_vm_counters_ex: aux::read_only
{
  static const process_information_class info_class_type = ProcessVmCounters;

  size_t PeakVirtualSize;
  size_t VirtualSize;
  uint32_t PageFaultCount;
  size_t PeakWorkingSetSize;
  size_t WorkingSetSize;
  size_t QuotaPeakPagedPoolUsage;
  size_t QuotaPagedPoolUsage;
  size_t QuotaPeakNonPagedPoolUsage;
  size_t QuotaNonPagedPoolUsage;
  size_t PagefileUsage;
  size_t PeakPagefileUsage;
  size_t PrivateUsage;
};

///\name  ProcessTimes == 4
struct kernel_user_times: aux::read_only
{
  static const process_information_class info_class_type = ProcessTimes;

  int64_t CreateTime;
  int64_t ExitTime;
  int64_t KernelTime;
  int64_t UserTime;
};

///\name  ProcessBasePriority == 5
struct process_base_priority_information: aux::write_only
{
  static const process_information_class info_class_type = ProcessBasePriority;

  km::kpriority BasePriority;

  process_base_priority_information(uint32_t priority)
    :BasePriority(priority)
  {}
};

///\name  ProcessIoCounters == 14
struct pooled_usage_and_limits: aux::read_only
{
  static const process_information_class info_class_type = ProcessPooledUsageAndLimits;

  size_t PeakPagedPoolUsage;
  size_t PagedPoolUsage;
  size_t PagedPoolLimit;
  size_t PeakNonPagedPoolUsage;
  size_t NonPagedPoolUsage;
  size_t NonPagedPoolLimit;
  size_t PeakPagefileUsage;
  size_t PagefileUsage;
  size_t PagefileLimit;
};

///\name  ProcessSessionInformation = 24
struct process_session_information
{
  static const process_information_class info_class_type = ProcessSessionInformation;

  uint32_t SessionId;

  // following ctors are just for test purposes, but may be they are useful?\n
  // for example: \code process_information<process_session_information> sinfo(current_process(), 0); \endcode
  process_session_information()
  {}
  process_session_information(uint32_t SessionId)
    :SessionId(SessionId)
  {}
};
///\}

/**@} process_information */

//////////////////////////////////////////////////////////////////////////
/**\addtogroup thread_information
 *@{*/
enum thread_info_class
{
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

typedef
ntstatus __stdcall
query_thread_information_t (
                            legacy_handle     ThreadHandle,
                            thread_info_class ThreadInformationClass,
                            void*             ThreadInformation,
                            uint32_t          ThreadInformationLength,
                            uint32_t*         ReturnLength
                            );

typedef
ntstatus __stdcall
set_thread_information_t (
                          legacy_handle     ThreadHandle,
                          thread_info_class ThreadInformationClass,
                          void*             ThreadInformation,
                          uint32_t          ThreadInformationLength
                          );

NTL__EXTERNAPI query_thread_information_t NtQueryInformationThread;
NTL__EXTERNAPI set_thread_information_t   NtSetInformationThread;

template <class                       InformationClass,
          query_thread_information_t QueryInformation,
          set_thread_information_t   SetInformation>
struct thread_information_base:
  aux::query_information_policy<InformationClass, query_thread_information_t, std::is_base_of<aux::write_only, InformationClass>::value>,
  aux::set_information_policy<InformationClass, query_thread_information_t, std::is_base_of<aux::read_only, InformationClass>::value>
{
  typedef InformationClass info_class;

  thread_information_base(legacy_handle handle) __ntl_nothrow
  {
    static_assert((std::is_base_of<aux::write_only, InformationClass>::value == false), "Cannot query a write-only information class");
    status_ = _query(QueryInformation, handle, &info, sizeof(info));
  }

  thread_information_base(
    legacy_handle       handle,
    const info_class &  new_info
    ) __ntl_nothrow
  {
    static_assert(is_read_only == false, "Cannot set a read-only information class");
    status_ = _set(SetInformation, handle, &new_info, sizeof(info_class));
  }

  info_class * data() { return nt::success(status_) ? &info : 0; }
  const info_class * data() const { return nt::success(status_) ? &info : 0; }
  info_class * operator->() { return data(); }
  const info_class * operator->() const { return data(); }

  operator bool() const { return nt::success(status_); }
  operator ntstatus() const { return status_; }

protected:
  ntstatus    status_;
}; //class thread_information_base

template<class ThreadInformationClass>
struct thread_information
  :public thread_information_base <ThreadInformationClass, NtQueryInformationThread, NtSetInformationThread>
{
  typedef thread_information_base <ThreadInformationClass, NtQueryInformationThread, NtSetInformationThread> base;

  thread_information(legacy_handle thread_handle = current_thread()) __ntl_nothrow
    : base(thread_handle)
  {/**/}

  thread_information(
    legacy_handle thread_handle,
    const ThreadInformationClass&  info) __ntl_nothrow
    : base(thread_handle, info)
  {/**/}
};

template<>
struct thread_information<kernel_user_times>
{
  typedef kernel_user_times info_class;

  thread_information(legacy_handle thread_handle = current_thread()) __ntl_nothrow
  {
    status_ = NtQueryInformationThread(thread_handle, ThreadTimes, &info, sizeof(info), nullptr);
  }

  info_class * data() { return nt::success(status_) ? &info : 0; }
  const info_class * data() const { return nt::success(status_) ? &info : 0; }
  info_class * operator->() { return data(); }
  const info_class * operator->() const { return data(); }

  operator bool() const { return nt::success(status_); }
  operator ntstatus() const { return status_; }
private:
  kernel_user_times info;
  ntstatus    status_;
};

/************************************************************************/
/* Thread informations                                                  */
/************************************************************************/
struct thread_basic_information
{
  static const thread_info_class info_class_type = ThreadBasicInformation;

  ntstatus ExitStatus;
  teb* TebBaseAddress;
  client_id ClientId;
  uintptr_t AffinityMask;
  km::kpriority Priority;
  km::kpriority BasePriority;
};

/** @} thread_information */

#pragma pack(pop)
}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_PROCESS_INFORMATION
