/**\file*********************************************************************
 *                                                                     \brief
 *  System Information
 *
 ****************************************************************************
 */

#ifndef NTL__NT_SYSTEM_INFORMATION
#define NTL__NT_SYSTEM_INFORMATION

#include "basedef.hxx"
#include "string.hxx"
#include "../iterator"

// for SystemProcessInformation
#include "teb.hxx"
// for SystemModuleInformation
#include "../pe/image.hxx"

#include "thread.hxx"

namespace ntl {


namespace nt {

/**\addtogroup  system_information ********* System Information *************
 *@{*/


enum system_information_class
{
  SystemBasicInformation,
  SystemProcessorInformation,
  SystemPerformanceInformation,
  SystemTimeOfDayInformation,
  SystemPathInformation,  ///< not implemented
  SystemProcessInformation,  ///< SystemProcessesAndThreadsInformation
  SystemCallCountInformation,
  SystemDeviceInformation,  ///< SystemConfigurationInformation
  SystemProcessorPerformanceInformation,
  SystemFlagsInformation,  ///< NT GlobalFlag
  SystemCallTimeInformation,
  SystemModuleInformation,
  SystemLocksInformation,
  SystemStackTraceInformation,
  SystemPagedPoolInformation,
  SystemNonPagedPoolInformation,
  SystemHandleInformation,
  SystemObjectInformation,
  SystemPageFileInformation,
  SystemVdmInstemulInformation,
  SystemVdmBopInformation,
  SystemFileCacheInformation,
  SystemPoolTagInformation,
  SystemInterruptInformation,
  SystemDpcBehaviourInformation,
  SystemFullMemoryInformation,
  SystemLoadGdiDriverInformation,
  SystemUnloadGdiDriverInformation,
  SystemTimeAdjustmentInformation,
  SystemSummaryMemoryInformation,
  SystemMirrorMemoryInformation,
  SystemPerformanceTraceInformation,
  SystemCrashDumpInformation,
  SystemExceptionInformation,
  SystemCrashDumpStateInformation,
  SystemKernelDebuggerInformation,
  SystemContextSwitchInformation,
  SystemRegistryQuotaInformation,
  SystemExtendServiceTableInformation, ///< SystemLoadAndCallImage
  SystemPrioritySeperation,
  SystemVerifierAddDriverInformation,
  SystemVerifierRemoveDriverInformation,
  SystemProcessorIdleInformation,
  SystemLegacyDriverInformation,
  SystemCurrentTimeZoneInformation,
  SystemLookasideInformation,
  SystemTimeSlipNotification,
  SystemSessionCreate,
  SystemSessionDetach,
  SystemSessionInformation,
  SystemRangeStartInformation,
  SystemVerifierInformation,
  SystemVerifierThunkExtend,
  SystemSessionProcessInformation,
  SystemLoadGdiDriverInSystemSpace,
  SystemNumaProcessorMap,
  SystemPrefetcherInformation,
  SystemExtendedProcessInformation,
  SystemRecommendedSharedDataAlignment,
  SystemComPlusPackage,
  SystemNumaAvailableMemory,
  SystemProcessorPowerInformation,
  SystemEmulationBasicInformation,
  SystemEmulationProcessorInformation,
  SystemExtendedHandleInformation,
  SystemLostDelayedWriteInformation,
  SystemBigPoolInformation,
  SystemSessionPoolTagInformation,
  SystemSessionMappedViewInformation,
  SystemHotpatchInformation,
  SystemObjectSecurityMode,
  SystemWatchdogTimerHandler,
  SystemWatchdogTimerInformation,
  SystemLogicalProcessorInformation,
  SystemWow64SharedInformation,
  SystemRegisterFirmwareTableInformationHandler,
  SystemFirmwareTableInformation,
  SystemModuleInformationEx,
  SystemVerifierTriageInformation,
  SystemSuperfetchInformation,
  SystemMemoryListInformation,
  SystemFileCacheInformationEx,
  MaxSystemInfoClass
};

STATIC_ASSERT(SystemFileCacheInformationEx == 81);

typedef
ntstatus __stdcall
  query_system_information_t(
    system_information_class  SystemInformationClass,
    void *                    SystemInformation,
    uint32_t                  SystemInformationLength,
    uint32_t *                ReturnLength
    );

typedef
ntstatus __stdcall
  set_system_information_t(
    system_information_class  SystemInformationClass,
    const void *              SystemInformation,
    uint32_t                  SystemInformationLength
    );

NTL__EXTERNAPI query_system_information_t NtQuerySystemInformation;
NTL__EXTERNAPI set_system_information_t NtSetSystemInformation;


///\note  most of SystemInformationClasses have either big or variable size,
///       so we stick to dinamic allocation in this generic implementation.
template <class                     InformationClass,
          query_system_information_t  QueryInformation>//,
          //set_system_information_t    SetInformation>
class system_information_base
{
    system_information_base(const system_information_base&);
    const system_information_base& operator=(const system_information_base&);

  ///////////////////////////////////////////////////////////////////////////
  public:

    typedef InformationClass info_class;

    system_information_base() __ntl_nothrow : ptr(0)
    {
      unsigned long length = 0;
      for ( unsigned long i = sizeof(info_class);
            (ptr = new char[length = length ? length : i ]) != 0;
            i *= 2 )
      {
        const ntstatus s = query(ptr, length, &length);
        if ( s == status::success )
          break;
        delete[] ptr;
        if ( s == status::info_length_mismatch )
          continue;
        ptr = nullptr;
        break;
      }
    }

    ~system_information_base() __ntl_nothrow
    {
      delete[] ptr;
    }

    info_class * operator->() { return data(); }
    const info_class * operator->() const { return data(); }

    info_class * data() { return reinterpret_cast<info_class*>(ptr); }
    const info_class * data() const { return reinterpret_cast<const info_class*>(ptr); }

    operator const void *() const { return ptr; }

    static __forceinline
    ntstatus
      query(
        void *          system_nformation,
        unsigned long   system_information_length,
        unsigned long * return_length = nullptr
        )
    {
      return QueryInformation(info_class::info_class_type,
                      system_nformation, system_information_length, return_length);
    }

  ///////////////////////////////////////////////////////////////////////////
  private:
    char * ptr;
};//class system_information_base

template<class InformationClass>
struct system_information
: public system_information_base<InformationClass,
                                NtQuerySystemInformation>
{};

//////////////////////////////////////////////////////////////////////////

///\name  SystemBasicInformation (0)
struct system_basic_information
{
  static const system_information_class info_class_type = SystemBasicInformation;

  uint32_t Reserved;
  uint32_t TimerResolution;
  uint32_t PageSize;
  uint32_t NumberOfPhysicalPages;
  uint32_t LowestPhysicalPageNumber;
  uint32_t HighestPhysicalPageNumber;
  uint32_t AllocationGranularity;
  uintptr_t MinimumUserModeAddress;
  uintptr_t MaximumUserModeAddress;
  uintptr_t ActiveProcessorsAffinityMask;
  int8_t    NumberOfProcessors;
};

///\name  SystemProcessInformation (5)

struct system_thread_information
{
  uint64_t      KernelTime;
  uint64_t      UserTime;
  uint64_t      CreateTime;
  uint32_t      WaitTime;
  void *        StartAddress;
  client_id     ClientId;
  km::kpriority Priority;
  km::kpriority BasePriority;
  uint32_t      ContextSwitches;
  thread_state::type  ThreadState;
  kwait_reason::type  WaitReason;
};

struct system_process_information
{
  static const system_information_class info_class_type = SystemProcessInformation;

  uint32_t              NextEntryOffset;
  uint32_t              NumberOfThreads;
  uint64_t              SpareLi[3];
  int64_t               CreateTime;
  int64_t               UserTime;
  int64_t               KernelTime;
  const_unicode_string  ImageName;
  km::kpriority         BasePriority;
  legacy_handle         UniqueProcessId;
  legacy_handle         InheritedFromUniqueProcessId;
  int32_t               HandleCount;
  uint32_t              SessionId;
  uint32_t              PageDirectoryBase;
  // VM counters
  size_t                PeakVirtualSize;
  size_t                VirtualSize;
  uint32_t              PageFaultCount;
  size_t                PeakWorkingSetSize;
  size_t                WorkingSetSize;
  size_t                QuotaPeakPagedPoolUsage;
  size_t                QuotaPagedPoolUsage;
  size_t                QuotaPeakNonPagedPoolUsage;
  size_t                QuotaNonPagedPoolUsage;
  size_t                PagefileUsage;
  size_t                PeakPagefileUsage;
  size_t                PrivatePageCount;
  // IO counters (win2k+)
  uint64_t              ReadOperationCount;
  uint64_t              WriteOperationCount;
  uint64_t              OtherOperationCount;
  uint64_t              ReadTransferCount;
  uint64_t              WriteTransferCount;
  uint64_t              OtherTransferCount;

  size_t size() const { return NumberOfThreads; }

  typedef const system_thread_information *     const_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

#if 0
  const_iterator begin() const
  {
    return end() - NumberOfThreads;
  }

  const_iterator end() const
  {
    return reinterpret_cast<system_thread_information*>(
      uintptr_t(this) + NextEntryOffset
      - (ImageName.size() ? ImageName.max_size()*sizeof(const_unicode_string::value_type) : 0));
  }
#else
  const_iterator begin() const
  {
    return reinterpret_cast<system_thread_information*>(uintptr_t(this) + sizeof(system_process_information));
  }

  const_iterator end() const
  {
    return begin() + NumberOfThreads;
  }
#endif
  const_reverse_iterator  rbegin() const { return const_reverse_iterator(end()); }
  const_reverse_iterator  rend() const  { return const_reverse_iterator(begin()); }
  const_iterator cbegin() const { return begin(); }
  const_iterator cend()   const { return end(); }
  const_reverse_iterator  crbegin() const { return rbegin(); }
  const_reverse_iterator  crend()   const { return rend(); }

};
//STATIC_ASSERT(sizeof(system_process_information) == 0xB8);

struct system_processes: public system_process_information
{
  static const system_information_class info_class_type = SystemProcessInformation;

  struct const_iterator
  : public std::iterator< std::forward_iterator_tag,
                          const system_process_information>
  {
      reference operator* () const { return *p; }
      pointer   operator->() const { return p; }

      iterator & operator++()
      {
        p = p->NextEntryOffset ?
          reinterpret_cast<pointer>(uintptr_t(p) + p->NextEntryOffset) : 0;
        return *this;
      }

    friend
      bool operator==(const const_iterator & x, const const_iterator & y)
      {
        return x.p == y.p;
      }

    friend
      bool operator!=(const const_iterator & x, const const_iterator & y)
      {
        return !(x == y);
      }

    ///////////////////////////////////////////////////////////////////////////
    private:

      const system_process_information * p;

      const_iterator(const system_process_information * p)
      : p(reinterpret_cast<pointer>(p)) {/**/}

      friend struct system_processes;
  };

  const_iterator begin() const {  return this; }
  const_iterator end() const { return 0; }
  const_iterator cbegin() const {  return this; }
  const_iterator cend() const { return 0; }

  const system_process_information *
  find_process(const const_unicode_string & image_name) const
  {
    for ( const_iterator it = cbegin(); it != cend(); ++it )
      if ( image_name == it->ImageName )
        return &*it;
    return 0;
  }

};// struct system_processes


///\name  SystemModuleInformation

struct rtl_process_module_information// RTL_PROCESS_MODULE_INFORMATION
{
  static const uint32_t full_path_name_len = 256;
  uint32_t    Section;
  void *      MappedBase;
  pe::image * ImageBase;
  uint32_t    ImageSize;
  uint32_t    Flags;
  uint16_t    LoadOrderIndex;
  uint16_t    InitOrderIndex;
  uint16_t    LoadCount;
  uint16_t    OffsetToFileName;
  char        FullPathName[full_path_name_len];

  pe::image * image() const
  {
    return pe::image::bind(ImageBase);
  }

  const char * file_name() const
  {
    return &FullPathName[OffsetToFileName];
  }

};

struct system_modules_information //RTL_PROCESS_MODULES
{
  static const system_information_class info_class_type = SystemModuleInformation;

  uint32_t                        NumberOfModules;
  rtl_process_module_information  Modules[1];

  size_t size() const { return NumberOfModules; }

  typedef rtl_process_module_information *        iterator;
  typedef const rtl_process_module_information *  const_iterator;
  typedef std::reverse_iterator<iterator>         reverse_iterator;
  typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

  iterator                begin()       { return &Modules[0]; }
  const_iterator          begin() const { return &Modules[0]; }
  iterator                end()         { return &Modules[NumberOfModules]; }
  const_iterator          end()   const { return &Modules[NumberOfModules]; }
  reverse_iterator        rbegin()      { return reverse_iterator(end()); }
  const_reverse_iterator  rbegin() const { return const_reverse_iterator(end()); }
  reverse_iterator        rend()        { return reverse_iterator(begin()); }
  const_reverse_iterator  rend() const  { return const_reverse_iterator(begin()); }
  const_iterator          cbegin() const { return begin(); }
  const_iterator          cend()   const { return end(); }
  const_reverse_iterator  crbegin()const { return rbegin(); }
  const_reverse_iterator  crend()  const { return rend(); }

  /// @note returns Modules[0] when file_name == nullptr
  __forceinline
  const rtl_process_module_information *
    find_module(const char file_name[]) const
  {
    for ( uint32_t i = 0; i != NumberOfModules; ++i )
      if ( ! file_name
        || ! _stricmp(file_name, Modules[i].file_name()) )
        return &Modules[i];
    return 0;
  }

  __forceinline
  const rtl_process_module_information *
    find_module(const std::string & file_name) const
  {
    for ( uint32_t i = 0; i != NumberOfModules; ++i )
      if ( file_name == Modules[i].file_name() )
        return &Modules[i];
    return 0;
  }

  static const char * module0()
  {
    return 0;
  }
}; // system_modules_information

///}

/**@} system_information */

}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_SYSTEM_INFORMATION
