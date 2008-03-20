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

namespace ntl {

  namespace km {
    typedef long kpriority;
  }

namespace nt {

/**\addtogroup  system_information ********* System Information *************
 *@{*/


enum system_information_class
{
  SystemBasicInformation                = 0,
  SystemProcessorInformation            = 1,
  SystemPerformanceInformation          = 2,
  SystemTimeOfDayInformation            = 3,
  SystemPathInformation                 = 4,  ///< not implemented
  SystemProcessInformation              = 5,  ///< SystemProcessesAndThreadsInformation
  SystemCallCountInformation            = 6,
  SystemDeviceInformation               = 7,  ///< SystemConfigurationInformation
  SystemProcessorPerformanceInformation = 8,
  SystemFlagsInformation                = 9,  ///< NT GlobalFlag
  SystemCallTimeInformation             = 10,
  SystemModuleInformation               = 11,
  SystemLocksInformation                = 12,
  SystemStackTraceInformation           = 13,
  SystemPagedPoolInformation            = 14,
  SystemNonPagedPoolInformation         = 15,
  SystemHandleInformation               = 16,
  SystemObjectInformation               = 17,
  SystemPageFileInformation             = 18,
  SystemVdmInstemulInformation          = 19,
  SystemVdmBopInformation               = 20, 
  SystemFileCacheInformation            = 21,
  SystemPoolTagInformation              = 22,
  SystemInterruptInformation            = 23,
  SystemDpcBehaviourInformation         = 24,
  SystemFullMemoryInformation           = 25,
  SystemLoadGdiDriverInformation        = 26,
  SystemUnloadGdiDriverInformation      = 27,
  SystemTimeAdjustmentInformation       = 28,
  SystemSummaryMemoryInformation        = 29,
  SystemMirrorMemoryInformation         = 30,
  SystemPerformanceTraceInformation     = 31,
  SystemCrashDumpInformation            = 32,
  SystemExceptionInformation            = 33,
  SystemCrashDumpStateInformation       = 34,
  SystemKernelDebuggerInformation       = 35,
  SystemContextSwitchInformation        = 36,
  SystemRegistryQuotaInformation        = 37,
  SystemExtendServiceTableInformation   = 38, ///< SystemLoadAndCallImage
  SystemPrioritySeperation              = 39,
  SystemVerifierAddDriverInformation    = 40,
  SystemVerifierRemoveDriverInformation = 41,
  SystemProcessorIdleInformation        = 42,
  SystemLegacyDriverInformation         = 43,
  SystemCurrentTimeZoneInformation      = 44,
  SystemLookasideInformation            = 45,
  SystemTimeSlipNotification            = 46,
  SystemSessionCreate                   = 47,
  SystemSessionDetach                   = 48,
  SystemSessionInformation              = 49,
  SystemRangeStartInformation           = 50,
  SystemVerifierInformation             = 51,
  SystemVerifierThunkExtend             = 52,
  SystemSessionProcessInformation       = 53,
  SystemLoadGdiDriverInSystemSpace      = 54,
  SystemNumaProcessorMap                = 55,
  SystemPrefetcherInformation           = 56,
  SystemExtendedProcessInformation      = 57,
  SystemRecommendedSharedDataAlignment  = 58,
  SystemComPlusPackage                  = 59,
  SystemNumaAvailableMemory             = 60,
  SystemProcessorPowerInformation       = 61,
  SystemEmulationBasicInformation       = 62,
  SystemEmulationProcessorInformation   = 63,
  SystemExtendedHandleInformation       = 64,
  SystemLostDelayedWriteInformation     = 65,
  SystemBigPoolInformation              = 66,
  SystemSessionPoolTagInformation       = 67,
  SystemSessionMappedViewInformation    = 68,
  SystemHotpatchInformation             = 69,
  SystemObjectSecurityMode              = 70,
  SystemWatchdogTimerHandler            = 71,
  SystemWatchdogTimerInformation        = 72,
  SystemLogicalProcessorInformation     = 73,
  SystemVerifierTriageInformation       = 74,
  SystemSuperfetchInformation           = 75,
  SystemMemoryListInformation           = 76,
  SystemModuleInformationEx             = 77,
  SystemReserveInformation              = 78,
};


typedef
ntstatus __stdcall
  query_system_information_t(
    system_information_class  SystemInformationClass,
    void *                    SystemInformation,
    unsigned long             SystemInformationLength,
    unsigned long *           ReturnLength
    );

NTL__EXTERNAPI query_system_information_t NtQuerySystemInformation;


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

    system_information_base() throw() : ptr(0) 
    {
      unsigned long length = 0;
      for ( unsigned long i = sizeof(info_class);
            (ptr = new char[length = length ? length : i ]) != 0;
            i *= 2 )
      {
        const ntstatus s = _query(ptr, length, &length);
        if ( s == status::success )
          break;
        delete[] ptr;
        if ( s == status::info_length_mismatch )
          continue;
        ptr = std::nullptr;
        break;
      }
    }

    ~system_information_base() throw()
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
      _query(
        void *          system_nformation,
        unsigned long   system_information_length,
        unsigned long * return_length = std::nullptr
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
  uint32_t      ThreadState;
  uint32_t      WaitReason;
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

  const_iterator begin() const
  { 
    return this;
  }
  
  const_iterator end() const
  { 
    return 0;
  }

  const system_process_information * 
  find_process(const const_unicode_string & image_name)
  {
    for ( const_iterator it = begin(); it != end(); ++it )
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
