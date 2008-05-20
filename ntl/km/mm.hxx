/**\file*********************************************************************
*                                                                     \brief
*  NT Memory 
*
****************************************************************************
*/


#ifndef NTL__NT_MM
#define NTL__NT_MM

#include "basedef.hxx"
#include "../nt/virtualmem.hxx"

namespace ntl {
  namespace km {

    typedef uint32_t wsle_number_t;
    typedef uintptr_t pfn_number_t;

    using nt::memory_basic_information;
    using nt::NtAllocateVirtualMemory;
    using nt::NtFreeVirtualMemory;
    using nt::NtFlushVirtualMemory;
    using nt::NtLockVirtualMemory;
    using nt::NtProtectVirtualMemory;
    using nt::NtQueryVirtualMemory;
    using nt::NtUnlockVirtualMemory;

    enum section_inherit
    {
      ViewShare = 1,
      ViewUnmap = 2
    };

    enum allocation_attributes
    {
      mem_commit            = 0x1000,
      mem_reserve           = 0x2000,
      mem_decommit          = 0x4000,
      mem_release           = 0x8000,
      mem_free             = 0x10000,
      mem_private          = 0x20000,
      mem_mapped           = 0x40000,
      mem_reset            = 0x80000,
      mem_top_down        = 0x100000,
      mem_image          = 0x1000000,
      sec_reserve        = 0x4000000,
      mem_large_pages   = 0x20000000,
      mem_4mb_pages     = 0x80000000,
      sec_large_pages   = 0x80000000
    };

    enum page_protection
    {
      page_noaccess           = 0x01,
      page_readonly           = 0x02,
      page_readwrite          = 0x04,
      page_writecopy          = 0x08,
      page_execute            = 0x10,
      page_execute_read       = 0x20,
      page_execute_readwrite  = 0x40,
      page_execute_writecopy  = 0x80,
      page_guard             = 0x100,
      page_nocache           = 0x200,
      page_writecombine      = 0x400
    };

    enum map_type
    {
      map_process = 1,
      map_system  = 2
    };

    enum memory_information_class
    {
      MemoryBasicInformation
    };


    /// common declarations

    NTL__EXTERNAPI void *     MmHighestUserAddress;
    NTL__EXTERNAPI void *     MmSystemRangeStart;
    NTL__EXTERNAPI uintptr_t  MmUserProbeAddress;

    enum memory_caching_type {
      MmNonCached,
      MmCached,
      MmWriteCombined
    };

    static inline
      void * highest_user_address()
    {
      return MmHighestUserAddress;
    }

    static inline
      void * lowest_user_address()
    {
      return reinterpret_cast<void*>(0x10000);
    }

    static inline
      bool valid_user_address(const void * p)
    {
      return lowest_user_address() <= p && p < highest_user_address();
    }


    typedef uint64_t physical_address;

    struct physical_memory_range
    {
      physical_address  BaseAddress;
      int64_t           NumberOfBytes;
    };


    NTL__EXTERNAPI
      physical_address __stdcall
      MmGetPhysicalAddress(const void * BaseAddress);

    NTL__EXTERNAPI
      void* __stdcall
      MmGetVirtualForPhysical(const physical_address PhysicalAddress);

    NTL__EXTERNAPI
      void* __stdcall
      MmMapIoSpace(
        const physical_address PhysicalAddress,
        uint32_t NumberOfBytes,
        memory_caching_type CacheType
        );

    NTL__EXTERNAPI
      void __stdcall
      MmUnmapIoSpace(
        const void* BaseAddress,
        uint32_t NumberOfBytes
        );


    NTL__EXTERNAPI
      void* __stdcall
      MmAllocateNonCachedMemory(
        uint32_t NumberOfBytes
        );

    NTL__EXTERNAPI
      void __stdcall
      MmFreeNonCachedMemory(
        const void* BaseAddress,
        uint32_t NumberOfBytes
        );

    NTL__EXTERNAPI
      void* __stdcall
      MmAllocateContiguousMemory(
        uint32_t NumberOfBytes,
        physical_address HighestAcceptableAddress
        );

    NTL__EXTERNAPI
      void __stdcall
      MmFreeContiguousMemory(
        const void* BaseAddress
        );

    NTL__EXTERNAPI
      void* __stdcall
      MmAllocateContiguousMemorySpecifyCache(
        uint32_t NumberOfBytes,
        physical_address LowestAcceptableAddress,
        physical_address HighestAcceptableAddress,
        physical_address BoundaryAddressMultiple,
        memory_caching_type CacheType
        );

    NTL__EXTERNAPI
      void __stdcall
      MmFreeContiguousMemorySpecifyCache(
        const void* BaseAddress,
        uint32_t NumberOfBytes,
        memory_caching_type CacheType
        );


    struct mdl;
    struct irp;

    NTL__EXTERNAPI
      void * __stdcall
      MmMapLockedPages(
        const mdl *     MemoryDescriptorList,
        kprocessor_mode AccessMode
        );

    NTL__EXTERNAPI
      void * __stdcall
      MmUnmapLockedPages(
        void *      BaseAddress,
        const mdl * MemoryDescriptorList
        );

    NTL__EXTERNAPI
      mdl * __stdcall
      IoAllocateMdl(
        void *    VirtualAddress,
        uint32_t  Length,
        bool      SecondaryBuffer,
        bool      ChargeQuota,
        irp *     Irp             __optional
        );

    NTL__EXTERNAPI
      void  __stdcall
      IoFreeMdl(
        mdl * Mdl
        );

    NTL__EXTERNAPI
      void  __stdcall
      MmBuildMdlForNonPagedPool(
        mdl * MemoryDescriptorList
        );

    struct mdl
    {
      mdl *     Next;
      int16_t   Size;
      //int16_t   MdlFlags;
      uint16_t  mapped_to_system_va     : 1;
      uint16_t  pages_locked            : 1;
      uint16_t  source_is_nonpaged_pool : 1;
      uint16_t  allocated_fixed_size    : 1;
      uint16_t  partial                 : 1;
      uint16_t  partial_has_been_mapped : 1;
      uint16_t  io_page_read            : 1;
      uint16_t  write_operation         : 1;
      uint16_t  parent_mapped_system_va : 1;
      uint16_t  free_extra_ptes         : 1;
      uint16_t  describes_awe           : 1;
      uint16_t  io_space                : 1;
      uint16_t  network_header          : 1;
      uint16_t  mapping_can_fail        : 1;
      uint16_t  allocated_must_succeed  : 1;
      struct eprocess *  Process;
      void *    MappedSystemVa;
      void *    StartVa;
      uint32_t  ByteCount;
      uint32_t  ByteOffset;

      void * operator new(
        std::size_t,
        void *    virtual_address,
        uint32_t  length,
        bool      Secondary_buffer  = false,
        bool      charge_quota     = false,
        irp *     pirp             = 0
        ) throw()
      {
        return IoAllocateMdl(virtual_address, length, Secondary_buffer, charge_quota, pirp);
      }

      void operator delete(void * pmdl) throw()
      {
        if ( pmdl ) IoFreeMdl(reinterpret_cast<mdl *>(pmdl));
      }

      void build_for_non_paged_pool()
      {
        MmBuildMdlForNonPagedPool(this);
      }

      mdl * next() const
      {
        return Next;
      }

      const uint32_t * pfn_array() const
      {
        return reinterpret_cast<const uint32_t *>(&this[1]);
      }

      ///\note valid in in the context of the subject thread
      uintptr_t virtual_address() const
      {
        return uintptr_t(StartVa) + ByteOffset;
      }

      void * map_locked_pages(kprocessor_mode AccessMode = KernelMode) const
      {
        return MmMapLockedPages(this, AccessMode);
      }

      void unmap_locked_pages(void * base_address) const
      {
        MmUnmapLockedPages(base_address, this);
      }

      void * system_address() const
      {
        return mapped_to_system_va || source_is_nonpaged_pool
          ? MappedSystemVa : map_locked_pages();
      }

      uint32_t byte_count() const
      {
        return ByteCount;
      }

      uint32_t total_bytes() const
      {
        uint32_t size = 0;
        for ( const mdl * p = this; p; p = p->next() )
          size += p->ByteCount;
        return size;
      }
    };



    /// MM

    struct mmcolor_tables {
      uint32_t Flink;
      void* Blink;
    };

    struct mmprimary_color_tables {
      list_entry ListHead;
    };

    struct mmpte_software {
      uint32_t Valid : 1;
      uint32_t PageFileLow : 4;
      uint32_t Protection : 5;
      uint32_t Prototype : 1;
      uint32_t Transition : 1;
      uint32_t PageFileHigh : 20;
    } ;

    struct mmpte_transition {
      uint32_t Valid : 1;
      uint32_t Write : 1;
      uint32_t Owner : 1;
      uint32_t WriteThrough : 1;
      uint32_t CacheDisable : 1;
      uint32_t Protection : 5;
      uint32_t Prototype : 1;
      uint32_t Transition : 1;
      uint32_t PageFrameNumber : 20;
    } ;

    struct mmpte_prototype {
      uint32_t Valid : 1;
      uint32_t ProtoAddressLow : 7;
      uint32_t ReadOnly : 1;
      uint32_t WhichPool : 1;
      uint32_t Prototype : 1;
      uint32_t ProtoAddressHigh : 21;
    };

    struct mmpte_subsection {
      uint32_t Valid : 1;
      uint32_t SubsectionAddressLow : 4;
      uint32_t Protection : 5;
      uint32_t Prototype : 1;
      uint32_t SubsectionAddressHigh : 20;
      uint32_t WhichPool : 1;
    } ;

    struct mmpte_list {
      uint32_t Valid : 1;
      uint32_t OneEntry : 1;
      uint32_t filler10 : 10;
      uint32_t NextEntry : 20;
    } ;

    struct mmpte_hardware_up {
      uint32_t Valid : 1;
      uint32_t Write : 1;
      uint32_t Owner : 1;
      uint32_t WriteThrough : 1;
      uint32_t CacheDisable : 1;
      uint32_t Accessed : 1;
      uint32_t Dirty : 1;
      uint32_t LargePage : 1;
      uint32_t Global : 1;
      uint32_t CopyOnWrite : 1;
      uint32_t Prototype : 1;
      uint32_t reserved : 1;
      uint32_t PageFrameNumber : 20;
    };

    struct mmpte_hardware {
      uint32_t Valid : 1;
      uint32_t Writable : 1;
      uint32_t Owner : 1;
      uint32_t WriteThrough : 1;
      uint32_t CacheDisable : 1;
      uint32_t Accessed : 1;
      uint32_t Dirty : 1;
      uint32_t LargePage : 1;
      uint32_t Global : 1;
      uint32_t CopyOnWrite : 1;
      uint32_t Prototype : 1;
      uint32_t Write : 1;
      uint32_t PageFrameNumber : 20;
    } ;

    struct hardware_pte_x86 {
      uint32_t Valid : 1;
      uint32_t Write : 1;
      uint32_t Owner : 1;
      uint32_t WriteThrough : 1;
      uint32_t CacheDisable : 1;
      uint32_t Accessed : 1;
      uint32_t Dirty : 1;
      uint32_t LargePage : 1;
      uint32_t Global : 1;
      uint32_t CopyOnWrite : 1; // software field
      uint32_t Prototype : 1;   // software field
      uint32_t reserved : 1;  // software field
      uint32_t PageFrameNumber : 20;
    };

    struct hardware_pte_x86pae {
      union {
        struct {
          uint64_t Valid : 1;
          uint64_t Write : 1;
          uint64_t Owner : 1;
          uint64_t WriteThrough : 1;
          uint64_t CacheDisable : 1;
          uint64_t Accessed : 1;
          uint64_t Dirty : 1;
          uint64_t LargePage : 1;
          uint64_t Global : 1;
          uint64_t CopyOnWrite : 1; // software field
          uint64_t Prototype : 1;   // software field
          uint64_t reserved0 : 1;  // software field
          uint64_t PageFrameNumber : 24;
          uint64_t reserved1 : 28;  // software field
        };
        struct {
          uint32_t LowPart;
          uint32_t HighPart;
        };
      };
    };

    struct hardware_pte_x64 {
      uint64_t Valid : 1;
      uint64_t Write : 1;                // UP version
      uint64_t Owner : 1;
      uint64_t WriteThrough : 1;
      uint64_t CacheDisable : 1;
      uint64_t Accessed : 1;
      uint64_t Dirty : 1;
      uint64_t LargePage : 1;
      uint64_t Global : 1;
      uint64_t CopyOnWrite : 1;          // software field
      uint64_t Prototype : 1;            // software field
      uint64_t reserved0 : 1;            // software field
      uint64_t PageFrameNumber : 28;
      uint64_t reserved1 : 24 - (11+1);
      uint64_t SoftwareWsIndex : 11;
      uint64_t NoExecute : 1;
    };

#if defined (_M_IX86)
    typedef hardware_pte_x86 hardware_pte;
#elif defined(_M_X64)
    typedef hardware_pte_x64 hardware_pte;
#endif

    struct mmpte {
      union  {
        uint32_t Long;
        mmpte_hardware Hard;
        hardware_pte Flush;
        mmpte_prototype Proto;
        mmpte_software Soft;
        mmpte_transition Trans;
        mmpte_subsection Subsect;
        mmpte_list List;
      } u;
    } ;

    struct mmvad_flags {
#if defined(_M_IX86)
      uintptr_t CommitCharge : 19;
#elif defined(_M_X64)
      uintptr_t CommitCharge : 51;
#endif
      uintptr_t PhysicalMapping : 1;
      uintptr_t ImageMap : 1;
      uintptr_t UserPhysicalPages : 1;
      uintptr_t NoChange : 1;
      uintptr_t WriteWatch : 1;
      uintptr_t Protection : 5;
      uintptr_t LargePages : 1;
      uintptr_t MemCommit: 1;
      uintptr_t PrivateMemory : 1;
    };

    struct mmvad_flags2 {
      uint32_t FileOffset : 24;
      uint32_t SecNoChange : 1;
      uint32_t OneSecured : 1;
      uint32_t MultipleSecured : 1;
      uint32_t ReadOnly : 1;
      uint32_t StoredInVad : 1;
      uint32_t ExtendableFile : 1;
      uint32_t Inherit : 1;
      uint32_t CopyOnWrite : 1;
    };

    struct mmaddress_list {
      uintptr_t StartVpn;
      uintptr_t EndVpn;
    };

    struct mmsecure_entry {
      union {
        uintptr_t LongFlags2;
        mmvad_flags2 VadFlags2;
      } u2;
      uintptr_t StartVpn;
      uintptr_t EndVpn;
      list_entry List;
    };


    // mm

    struct mmsection_flags {
      uint32_t BeingDeleted : 1;
      uint32_t BeingCreated : 1;
      uint32_t BeingPurged : 1;
      uint32_t NoModifiedWriting : 1;
      uint32_t FailAllIo : 1;
      uint32_t Image : 1;
      uint32_t Based : 1;
      uint32_t File : 1;
      uint32_t Networked : 1;
      uint32_t NoCache : 1;
      uint32_t PhysicalMemory : 1;
      uint32_t CopyOnWrite : 1;
      uint32_t Reserve : 1;
      uint32_t Commit : 1;
      uint32_t FloppyMedia : 1;
      uint32_t WasPurged : 1;
      uint32_t UserReference : 1;
      uint32_t GlobalMemory : 1;
      uint32_t DeleteOnClose : 1;
      uint32_t FilePointerNull : 1;
      uint32_t DebugSymbolsLoaded : 1;
      uint32_t SetMappedFileIoComplete : 1;
      uint32_t CollidedFlush : 1;
      uint32_t NoChange : 1;
      uint32_t HadUserReference : 1;
      uint32_t ImageMappedInSystemSpace : 1;
      uint32_t filler0 : 1;
      uint32_t Accessed : 1;
      uint32_t GlobalOnlyPerSession : 1;
      uint32_t filler : 3;
    } ;

    struct mmsupport_flags {
      uint8_t SessionSpace : 1;
      uint8_t BeingTrimmed : 1;
      uint8_t SessionLeader : 1;
      uint8_t TrimHard : 1;
      uint8_t MaximumWorkingSetHard : 1;
      uint8_t ForceTrim : 1;
      uint8_t MinimumWorkingSetHard : 1;
      uint8_t Available0 : 1;
      uint8_t MemoryPriority : 8;
      uint16_t GrowWsleHash : 1;
      uint16_t AcquiredUnsafe : 1;
      uint16_t Available : 14;
    };

    struct mmsupport_flags50
    {
      uint32_t SessionSpace : 1;
      uint32_t BeingTrimmed : 1;
      uint32_t ProcessInSession : 1;
      uint32_t SessionLeader : 1;
      uint32_t TrimHard : 1;
      uint32_t WorkingSetHard : 1;
      uint32_t WriteWatch : 1;
      uint32_t Filler : 25;
    };


    struct mmsupport50
    {
      int64_t LastTrimTime;
      uint32_t LastTrimFaultCount;
      uint32_t PageFaultCount;
      uint32_t PeakWorkingSetSize;
      uint32_t WorkingSetSize;
      uint32_t MinimumWorkingSetSize;
      uint32_t MaximumWorkingSetSize;
      struct mmwsl *VmWorkingSetList;
      list_entry WorkingSetExpansionLinks;
      uint8_t AllowWorkingSetAdjustment;
      bool AddressSpaceBeingDeleted;
      uint8_t ForegroundSwitchCount;
      uint8_t MemoryPriority;
      mmsupport_flags50 Flags;
      uint32_t Claim;
      uint32_t NextEstimationSlot;
      uint32_t NextAgingSlot;
      uint32_t EstimatedAvailable;
      uint32_t GrowthSinceLastEstimate;
    };

    struct mmsupport51
    {
      int64_t LastTrimTime;

      mmsupport_flags Flags;
      uint32_t PageFaultCount;
      wsle_number_t PeakWorkingSetSize;
      wsle_number_t GrowthSinceLastEstimate;

      wsle_number_t MinimumWorkingSetSize;
      wsle_number_t MaximumWorkingSetSize;
      struct mmwsl *VmWorkingSetList;
      wsle_number_t Claim;

      wsle_number_t NextEstimationSlot;
      wsle_number_t NextAgingSlot;
      wsle_number_t EstimatedAvailable;
      wsle_number_t WorkingSetSize;

      ex_push_lock WorkingSetMutex;
    };

    struct mmsupport52
    {
      list_entry WorkingSetExpansionLinks;
      int64_t LastTrimTime;

      mmsupport_flags Flags;
      uint32_t PageFaultCount;
      wsle_number_t PeakWorkingSetSize;
      wsle_number_t GrowthSinceLastEstimate;

      wsle_number_t MinimumWorkingSetSize;
      wsle_number_t MaximumWorkingSetSize;
      struct mmwsl *VmWorkingSetList;
      wsle_number_t Claim;

      wsle_number_t NextEstimationSlot;
      wsle_number_t NextAgingSlot;
      wsle_number_t EstimatedAvailable;
      wsle_number_t WorkingSetSize;
#if defined(_M_IX86)
      kguarded_mutex WorkingSetMutex;
#elif defined(_M_X64)
      ex_push_lock WorkingSetMutex;
#endif
    };

    struct mmsupport60
    {
      /*<thisrel this+0x0>*/ /*|0x8|*/ list_entry WorkingSetExpansionLinks;
      /*<thisrel this+0x8>*/ /*|0x2|*/ uint16_t LastTrimStamp;
      /*<thisrel this+0xa>*/ /*|0x2|*/ uint16_t NextPageColor;
      /*<thisrel this+0xc>*/ /*|0x4|*/ mmsupport_flags Flags;
      /*<thisrel this+0x10>*/ /*|0x4|*/ int32_t PageFaultCount;
      /*<thisrel this+0x14>*/ /*|0x4|*/ int32_t PeakWorkingSetSize;
      /*<thisrel this+0x18>*/ /*|0x4|*/ int32_t ChargedWslePages;
      /*<thisrel this+0x1c>*/ /*|0x4|*/ int32_t MinimumWorkingSetSize;
      /*<thisrel this+0x20>*/ /*|0x4|*/ int32_t MaximumWorkingSetSize;
      /*<thisrel this+0x24>*/ /*|0x4|*/ struct mmwsl* VmWorkingSetList;
      /*<thisrel this+0x28>*/ /*|0x4|*/ int32_t Claim;
      /*<thisrel this+0x2c>*/ /*|0x4|*/ int32_t ActualWslePages;
      /*<thisrel this+0x30>*/ /*|0x4|*/ int32_t WorkingSetPrivateSize;
      /*<thisrel this+0x34>*/ /*|0x4|*/ int32_t WorkingSetSizeOverhead;
      /*<thisrel this+0x38>*/ /*|0x4|*/ int32_t WorkingSetSize;
      /*<thisrel this+0x3c>*/ /*|0x4|*/ kgate* ExitGate;
      /*<thisrel this+0x40>*/ /*|0x4|*/ ex_push_lock WorkingSetMutex;
      /*<thisrel this+0x44>*/ /*|0x4|*/ void* AccessLog;
    };

#if defined(_M_IX86)
#   define _MI_PAGING_LEVELS 2
#   define PDE_PER_PAGE 1024
#   define MM_USER_PAGE_TABLE_PAGES 768
#elif defined(_M_X64)
#   define _MI_PAGING_LEVELS 4
#   define PDE_PER_PAGE 512
#   define MM_USER_PXES (0x10)
#   define MM_USER_PAGE_TABLE_PAGES ((ULONG_PTR)PDE_PER_PAGE * PPE_PER_PAGE * MM_USER_PXES)
#   define MM_USER_PAGE_DIRECTORY_PAGES (PPE_PER_PAGE * MM_USER_PXES)
#   define MM_USER_PAGE_DIRECTORY_PARENT_PAGES (MM_USER_PXES)

#endif

    struct mmaddress_node 
    {
      union {
        intptr_t Balance : 2;
        mmaddress_node *Parent;
      };
      mmaddress_node *LeftChild;
      mmaddress_node *RightChild;
      uintptr_t StartingVpn;
      uintptr_t EndingVpn;
    };

    struct mm_avl_table {
      mmaddress_node  BalancedRoot;
      uintptr_t DepthOfTree: 5;
      uintptr_t Unused: 3;
#if defined (_M_X64)
      uintptr_t NumberGenericTableElements: 56;
#else
      uintptr_t NumberGenericTableElements: 24;
#endif
      void* NodeHint;
      void* NodeFreeHint;
    };

    struct mmwsle_hash {
      void*         Key;
      wsle_number_t Index;
    };

    struct mmwsl {
      wsle_number_t FirstFree;
      wsle_number_t FirstDynamic;
      wsle_number_t LastEntry;
      wsle_number_t NextSlot;
      struct mmwsle* Wsle;
      wsle_number_t LastInitializedWsle;
      wsle_number_t NonDirectCount;
      mmwsle_hash* HashTable;
      uint32_t HashTableSize;
      uint32_t NumberOfCommittedPageTables;
      void* HashTableStart;
      void* HighestPermittedHashAddress;
      uint32_t NumberOfImageWaiters;
      uint32_t VadBitMapHint;

#if defined _M_X64
      void* HighestUserAddress;
#endif

#if (_MI_PAGING_LEVELS >= 3)
      uint32_t MaximumUserPageTablePages;
#endif

#if (_MI_PAGING_LEVELS >= 4)
      uint32_t MaximumUserPageDirectoryPages;
      uint32_t* CommittedPageTables;

      uint32_t NumberOfCommittedPageDirectories;
      uint32_t* CommittedPageDirectories;

      uint32_t NumberOfCommittedPageDirectoryParents;
      uintptr_t CommittedPageDirectoryParents[(MM_USER_PAGE_DIRECTORY_PARENT_PAGES + sizeof(uintptr_t)*8-1)/(sizeof(uintptr_t)*8)];

#elif (_MI_PAGING_LEVELS >= 3)
      uint32_t* CommittedPageTables;

      uint32_t NumberOfCommittedPageDirectories;
      uintptr_t CommittedPageDirectories[(MM_USER_PAGE_DIRECTORY_PAGES + sizeof(uintptr_t)*8-1)/(sizeof(uintptr_t)*8)];

#else

      uint16_t UsedPageTableEntries[MM_USER_PAGE_TABLE_PAGES];
      uint32_t CommittedPageTables[MM_USER_PAGE_TABLE_PAGES/(sizeof(uint32_t)*8)];
#endif

    };

    struct section {
      mmaddress_node Address;
      struct segment *Segment;
      int64_t SizeOfSection;
      mmsection_flags Flags;
      uint32_t InitialPageProtection;
    } ;

    struct mmbanked_section {
      pfn_number_t BasePhysicalPage;
      mmpte* BasedPte;
      uint32_t BankSize;
      uint32_t BankShift;
      void (__stdcall *BankedRoutine)(uint32_t ReadBank, uint32_t WriteBank, void* Context) ;
      void* Context;
      mmpte* CurrentMappedPte;
      mmpte BankTemplate[1];
    };

    struct mmextend_info {
      uint64_t CommittedSize;
      uint32_t ReferenceCount;
    };

    struct mmvad {
      uintptr_t StartingVpn;
      uintptr_t EndingVpn;
      mmvad *Parent;
      mmvad *LeftChild;
      mmvad *RightChild;
      union {
        uintptr_t LongFlags;
        mmvad_flags VadFlags;
      } u;
      struct control_area* ControlArea;
      mmpte* FirstPrototypePte;
      mmpte* LastContiguousPte;
      mmvad_flags2 VadFlags2;
      union {
        list_entry List;
        mmaddress_list Secured;
      } u3;
      union {
        mmbanked_section* Banked;
        mmextend_info* ExtendedInfo;
      } u4;
    };

    struct mmvad_short {
      uintptr_t StartingVpn;
      uintptr_t EndingVpn;
      mmvad *Parent;
      mmvad *LeftChild;
      mmvad *RightChild;
      union {
        uintptr_t LongFlags;
        mmvad_flags VadFlags;
      } u;
    };

    struct rtl_bitmap {
      uint32_t SizeOfBitMap;
      uint32_t* Buffer;
    };

    struct mi_physical_view {
      list_entry ListEntry;
      mmvad* Vad;
      int8_t* StartVa;
      int8_t* EndVa;
      rtl_bitmap* BitMap;
    };

    // Page File structures.
 
    struct mmmod_writer_listhead {
      list_entry ListHead;
      kevent Event;
    } ;

    struct mmmod_writer_mdl_entry {
      list_entry Links;
      int64_t WriteOffset;
      union {
        io_status_block IoStatus;
        int64_t LastByte;
      } u;
      irp* Irp;
      uintptr_t LastPageToWrite;
      mmmod_writer_listhead* PagingListHead;
      list_entry* CurrentList;
      struct mmpaging_file *PagingFile;
      file_object* File;
      control_area* ControlArea;
      eresource* FileResource;
      mdl Mdl;
      pfn_number_t Page[1];
    } ;

    struct mmpaging_file {
      pfn_number_t Size;
      pfn_number_t MaximumSize;
      pfn_number_t MinimumSize;
      pfn_number_t FreeSpace;
      pfn_number_t CurrentUsage;
      pfn_number_t PeakUsage;
      pfn_number_t Hint;
      pfn_number_t HighestPage;
      mmmod_writer_mdl_entry* Entry[2];
      rtl_bitmap* Bitmap;
      file_object* File;
      unicode_string PageFileName;
      uint32_t PageFileNumber;
      bool Extended;
      bool HintSetToZero;
    };

    struct mminpage_support_list {
      list_entry ListHead;
      uint32_t Count;
    };

    struct mmevent_count_list {
      list_entry ListHead;
      uint32_t Count;
    };


    // System PTE structures.

    enum mmsystem_pte_pool_type {
      SystemPteSpace,
      NonPagedPoolExpansion,
      MaximumPtePoolTypes
    };

    struct mmfree_pool_entry {
      list_entry List;
      pfn_number_t Size;
      uint32_t Signature;
      mmfree_pool_entry *Owner;
    };


    struct mmlock_conflict {
      list_entry List;
      struct ethread* Thread;
    };


    // System view structures

    struct mmview {
      uintptr_t Entry;
      control_area *ControlArea;
    };

    struct mmsession {
      fast_mutex SystemSpaceViewLock;
      fast_mutex *SystemSpaceViewLockPointer;
      int8_t* SystemSpaceViewStart;
      mmview *SystemSpaceViewTable;
      uint32_t SystemSpaceHashSize;
      uint32_t SystemSpaceHashEntries;
      uint32_t SystemSpaceHashKey;
      rtl_bitmap *SystemSpaceBitMap;
    };

#if defined(_M_IX86)
    struct mmpte_flush_list {
      uint32_t Count;
      mmpte FlushPte[15];
      void* FlushVa[15];
    };
#endif

    struct lock_tracker {
      list_entry ListEntry;
      mdl *Mdl;
      void* StartVa;
      pfn_number_t Count;
      uint32_t Offset;
      uint32_t Length;
      pfn_number_t Page;
      void* CallingAddress;
      void* CallersCaller;
      list_entry GlobalListEntry;
      uint32_t Who;
      struct eprocess *Process;
    };

    struct lock_header {
      list_entry ListHead;
      pfn_number_t Count;
    };

    struct unloaded_drivers {
      unicode_string Name;
      void* StartAddress;
      void* EndAddress;
      int64_t CurrentTime;
    };

    enum section_check_type {
      CheckDataSection,
      CheckImageSection,
      CheckUserDataSection,
      CheckBothSection
    } ;

    struct section_image_information {
      void* TransferAddress;
      uint32_t ZeroBits;
      size_t MaximumStackSize;
      size_t CommittedStackSize;
      uint32_t SubSystemType;
      union {
        struct {
          uint16_t SubSystemMinorVersion;
          uint16_t SubSystemMajorVersion;
        };
        uint32_t SubSystemVersion;
      };
      uint32_t GpValue;
      uint16_t ImageCharacteristics;
      uint16_t DllCharacteristics;
      uint16_t Machine;
      bool ImageContainsCode;
      bool Spare1;
      uint32_t LoaderFlags;
      uint32_t ImageFileSize;
      uint32_t Reserved[ 1 ];
    };

    struct segment {
      control_area *ControlArea;
      void* SegmentBaseAddress;
      uint32_t TotalNumberOfPtes;
      uint32_t NonExtendedPtes;
      uint64_t SizeOfSegment;
      size_t ImageCommitment;
      section_image_information* ImageInformation;
      void* SystemImageBase;
      size_t NumberOfCommittedPages;
      mmpte SegmentPteTemplate;
      void* BasedAddress;
      mmextend_info *ExtendInfo;
      mmpte *PrototypePte;
      mmpte ThePtes[1];
    };

    struct event_counter {
      uint32_t RefCount;
      kevent Event;
      list_entry ListEntry;
    } ;


    struct control_area {
      segment *Segment;
      list_entry DereferenceList;
      uint32_t NumberOfSectionReferences;
      uint32_t NumberOfPfnReferences;
      uint32_t NumberOfMappedViews;
      uint16_t NumberOfSubsections;
      uint16_t FlushInProgressCount;
      uint32_t NumberOfUserReferences;
      mmsection_flags Flags;
      file_object *FilePointer;
      event_counter *WaitingForDeletion;
      uint16_t ModifiedWriteCount;
      uint16_t NumberOfSystemCacheViews;
      size_t PagedPoolUsage;
      size_t NonPagedPoolUsage;
    } ;

    struct large_control_area {
      segment *Segment;
      list_entry DereferenceList;
      uint32_t NumberOfSectionReferences;
      uint32_t NumberOfPfnReferences;
      uint32_t NumberOfMappedViews;
      uint16_t NumberOfSubsections;
      uint16_t FlushInProgressCount;
      uint32_t NumberOfUserReferences;
      mmsection_flags Flags;
      file_object *FilePointer;
      event_counter *WaitingForDeletion;
      uint16_t ModifiedWriteCount;
      uint16_t NumberOfSystemCacheViews;
      size_t PagedPoolUsage;
      size_t NonPagedPoolUsage;
      list_entry UserGlobalList;
      uint32_t SessionId;
      uint32_t Pad;
    } ;

    struct mmsubsection_flags {
      uint32_t ReadOnly : 1;
      uint32_t ReadWrite : 1;
      uint32_t CopyOnWrite : 1;
      uint32_t GlobalMemory: 1;
      uint32_t Protection : 5;
      uint32_t LargePages : 1;
      uint32_t StartingSector4132 : 10;
      uint32_t SectorEndOffset : 12;
    } ;

    struct subsection {
      control_area *ControlArea;
      mmsubsection_flags SubsectionFlags;
      uint32_t StartingSector;
      uint32_t NumberOfFullSectors;
      mmpte *SubsectionBase;
      uint32_t UnusedPtes;
      uint32_t PtesInSubsection;
      subsection *NextSubsection;
    } ;


  } // km
} // ntl

#endif // NTL__NT_MM