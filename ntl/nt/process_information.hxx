/**\file*********************************************************************
 *                                                                     \brief
 *  Process Information
 *
 ****************************************************************************
 */

#ifndef NTL__NT_PROCESS_INFORMATION
#define NTL__NT_PROCESS_INFORMATION

#include "basedef.hxx"
#include "handle.hxx"

#include "../stlx/type_traits.hxx"

namespace ntl { namespace nt {
  struct peb;

  /**\addtogroup  process_information ********* Process Information *************
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
          query_process_information_t   QueryInformation,
          bool                          write_only>
  struct query_process_information_policy
  {
    static __forceinline
      ntstatus
      _query(
      legacy_handle   process_handle,
      void *          info,
      uint32_t        info_length
      )
    {
      return QueryInformation(process_handle, InformationClass::info_class_type, info, info_length, std::nullptr);
    }
  protected:
    InformationClass info;
  };

  /// write-only policy
  template<class                        InformationClass,
          query_process_information_t   QueryInformation>
  struct query_process_information_policy<InformationClass, QueryInformation, true>
  {
    static __forceinline
      ntstatus
      _query(
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
            set_process_information_t   SetInformation,
            bool                        read_only>
  struct set_process_information_policy
  {
    static const bool is_read_only = false;

      static __forceinline
      ntstatus
        _set(
          legacy_handle   process_handle,
          const void *    info,
          uint32_t        info_length
          )
      {
        return SetInformation(process_handle, InformationClass::info_class_type, info, info_length);
      }
  };

  template <class                       InformationClass,
            set_process_information_t   SetInformation>
  struct set_process_information_policy<InformationClass, SetInformation, true>
  {
    static const bool is_read_only = true;

    static __forceinline
      ntstatus
      _set(
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
  aux::query_process_information_policy<InformationClass, QueryInformation, std::is_base_of<aux::write_only, InformationClass>::value>,
  aux::set_process_information_policy<InformationClass, SetInformation, std::is_base_of<aux::read_only, InformationClass>::value>
{
    typedef InformationClass info_class;

    process_information_base(legacy_handle process_handle) __ntl_nothrow
    {
      static_assert((std::is_base_of<aux::write_only, InformationClass>::value == false), "Cannot query a write-only information class");
      status_ = _query(process_handle, &info, sizeof(info));
    }

    process_information_base(
      legacy_handle       process_handle,
      const info_class &  new_info
      ) __ntl_nothrow
    {
      //static_assert(is_read_only == false, "Cannot set a read-only information class");
      status_ = _set(process_handle, &new_info, sizeof(info_class));
    }

    info_class * data() { return nt::success(status_) ? &info : 0; }
    const info_class * data() const { return nt::success(status_) ? &info : 0; }
    info_class * operator->() { return data(); }
    const info_class * operator->() const { return data(); }

    operator bool() const { return nt::success(status_); }

    operator ntstatus() const { return status_; }



  ///////////////////////////////////////////////////////////////////////////
  private:

    ntstatus    status_;
}; //class process_information_base


template<class InformationClass>
struct process_information
: public process_information_base <InformationClass,
                                   NtQueryInformationProcess,
                                   NtSetInformationProcess>
{
  process_information(legacy_handle process_handle = current_process()) __ntl_nothrow
  : process_information_base<InformationClass, NtQueryInformationProcess, NtSetInformationProcess>(process_handle)
  {/**/}

  process_information(
    legacy_handle             process_handle,
    const InformationClass &  info) __ntl_nothrow
  : process_information_base<InformationClass, NtQueryInformationProcess, NtSetInformationProcess>(process_handle, info)
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


#pragma pack(pop)
  /**@} process_information */

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_PROCESS_INFORMATION
