/**\file*********************************************************************
*                                                                     \brief
*  Shared resource support
*
****************************************************************************
*/

#ifndef NTL__NT_RESOURCE
#define NTL__NT_RESOURCE

#include "basedef.hxx"
#include "handle.hxx"

namespace ntl {
  namespace nt {

    namespace rtl
    {
      // critical section
      struct critical_section_debug
      {
        enum type { CritSect, Resource };
        uint16_t    Type;
        uint16_t    CreatorBackTraceIndex;
        struct critical_section* CriticalSection;
        list_entry  ProcessLocksList;
        uint32_t    LockCount;
        uint32_t    ContentionCount;
        uint32_t    Spare[2];
      };

      struct critical_section
      {
        critical_section_debug* DebugInfo;
        int32_t       LockCount;
        int32_t       RecursionCount;
        legacy_handle OwningThread;
        legacy_handle LockSemaphore;
        uint32_t      reserved;
      };

      struct resource_debug
      {
        uint32_t      reserved[5];
        uint32_t      ContentionCount;
        uint32_t      Spare[2];
      };

      // shared resource
      struct resource
      {
        critical_section  CriticalSection;
        legacy_handle     SharedSemaphore;
        uint32_t          NumberOfWaitingShared;
        legacy_handle     ExclusiveSemaphore;
        uint32_t          NumberOfWaitingExclusive;

        int32_t           NumberOfActive;
        legacy_handle     ExclusiveOwnerThread;

        enum flags { None, LongTerm };
        flags             Flags;
        resource_debug*   DebugInfo;
      };

    } // rtl


    typedef ntstatus __stdcall critical_section_control_t(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      critical_section_control_t
        RtlInitializeCriticalSection,
        RtlDeleteCriticalSection,
        RtlEnterCriticalSection, 
        RtlLeaveCriticalSection;

    NTL__EXTERNAPI
      ntstatus __stdcall RtlInitializeCriticalSectionAndSpinCount(
        rtl::critical_section* CriticalSection,
        uint32_t               SpinCount
        );

    NTL__EXTERNAPI
      uint32_t __stdcall RtlTryEnterCriticalSection(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlIsCriticalSectionLocked(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlIsCriticalSectionLockedByThread(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlGetCriticalSectionRecursionCount(rtl::critical_section* CriticalSection);

    NTL__EXTERNAPI
      uint32_t __stdcall RtlSetCriticalSectionSpinCount(rtl::critical_section* CriticalSection, uint32_t SpinCount);

    NTL__EXTERNAPI
      void __stdcall RtlEnableEarlyCriticalSectionEventCreation();

    NTL__EXTERNAPI
      void __stdcall RtlCheckForOrphanedCriticalSections(legacy_handle Thread);

    NTL__EXTERNAPI
      void __stdcall RtlpWaitForCriticalSection(rtl::critical_section* CriticalSection);
    
    NTL__EXTERNAPI
      void __stdcall RtlpUnWaitCriticalSection(rtl::critical_section* CriticalSection);      

    NTL__EXTERNAPI
      void __stdcall RtlpNotOwnerCriticalSection(rtl::critical_section* CriticalSection);      

  } //namespace nt
} //namespace ntl

#endif //#ifndef NTL__NT_RESOURCE

