/**\file*********************************************************************
*                                                                     \brief
*  NT Virtual Memory support
*
****************************************************************************
*/


#ifndef NTL__NT_VIRTUALMEM
#define NTL__NT_VIRTUALMEM

#include "handle.hxx"

namespace ntl {
  namespace nt {

    __class_enum (section_inherit)
    {
      ViewShare = 1,
      ViewUnmap = 2
    };};

    __class_enum (allocation_attributes)
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
    __ntl_bitmask_type(type, friend)};

    __class_enum (page_protection)
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
    __ntl_bitmask_type(type, friend)};

    __class_enum  (map_type)
    {
      map_process = 1,
      map_system  = 2
    };};

    enum memory_information_class
    {
      MemoryBasicInformation
    };

    struct memory_basic_information
    {
      void*                       BaseAddress;
      void*                       AllocationBase;
      page_protection::type       AllocationProtect;
      size_t                      RegionSize;
      allocation_attributes::type State;
      page_protection::type       Protect;
      allocation_attributes::type Type;
    };




    NTL__EXTERNAPI
      ntstatus __stdcall
      NtAllocateVirtualMemory(
        legacy_handle ProcessHandle,
        void**        BaseAddress,
        uint32_t      ZeroBits,
        size_t*       RegionSize,
        uint32_t      AllocationType,
        uint32_t      Protect
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtFreeVirtualMemory(
        legacy_handle ProcessHandle,
        void*         BaseAddress,
        size_t*       RegionSize,
        uint32_t      FreeType
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtFlushVirtualMemory(
        legacy_handle ProcessHandle,
        void**        BaseAddress,
        size_t*       RegionSize,
        io_status_block* IoStatus
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtLockVirtualMemory(
        legacy_handle ProcessHandle,
        void**        BaseAddress,
        size_t*       LockSize,
        map_type      LockOption
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtProtectVirtualMemory(
        legacy_handle ProcessHandle,
        void**        BaseAddress,
        size_t*       ProtectSize,
        uint32_t      NewAccessProtection,
        uint32_t*     OldAccessProtection
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtQueryVirtualMemory(
        legacy_handle ProcessHandle,
        void*         BaseAddress,
        memory_information_class MemoryInformationClass,
        void*         Buffer,
        uint32_t      Length,
        uint32_t*     ResultLength
        );

    NTL__EXTERNAPI
      ntstatus __stdcall
      NtUnlockVirtualMemory(
        legacy_handle ProcessHandle,
        void**        BaseAddress,
        size_t*       LockSize,
        map_type      LockType
        );

  }
}

#endif // NTL__NT_VIRTUALMEM
