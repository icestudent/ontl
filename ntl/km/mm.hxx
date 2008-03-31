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

    struct mmsection_flags {
      unsigned BeingDeleted : 1;
      unsigned BeingCreated : 1;
      unsigned BeingPurged : 1;
      unsigned NoModifiedWriting : 1;
      unsigned FailAllIo : 1;
      unsigned Image : 1;
      unsigned Based : 1;
      unsigned File : 1;
      unsigned Networked : 1;
      unsigned NoCache : 1;
      unsigned PhysicalMemory : 1;
      unsigned CopyOnWrite : 1;
      unsigned Reserve : 1;
      unsigned Commit : 1;
      unsigned FloppyMedia : 1;
      unsigned WasPurged : 1;
      unsigned UserReference : 1;
      unsigned GlobalMemory : 1;
      unsigned DeleteOnClose : 1;
      unsigned FilePointerNull : 1;
      unsigned DebugSymbolsLoaded : 1;
      unsigned SetMappedFileIoComplete : 1;
      unsigned CollidedFlush : 1;
      unsigned NoChange : 1;
      unsigned HadUserReference : 1;
      unsigned ImageMappedInSystemSpace : 1;
      unsigned filler0 : 1;
      unsigned Accessed : 1;
      unsigned GlobalOnlyPerSession : 1;
      unsigned filler : 3;
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
      unsigned ReadOnly : 1;
      unsigned ReadWrite : 1;
      unsigned CopyOnWrite : 1;
      unsigned GlobalMemory: 1;
      unsigned Protection : 5;
      unsigned LargePages : 1;
      unsigned StartingSector4132 : 10;
      unsigned SectorEndOffset : 12;
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