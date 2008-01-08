/**\file*********************************************************************
 *                                                                     \brief
 *  NT Process Environment
 *
 ****************************************************************************
 */

#ifndef NTL__NT_PEB
#define NTL__NT_PEB

#include "basedef.hxx"
#include "string.hxx"
#include "../pe/image.hxx"
#include "teb.hxx"

namespace ntl {
namespace nt {

struct rtl_critical_section;
struct rtl_heap;

struct curdir
{
	/* 0x0 */ /*|0x8|*/ unicode_string DosPath;
	/* 0x8 */ /*|0x4|*/ legacy_handle Handle;
};

struct rtl_drive_letter_curdir
{
	/* 0x0 */ /*|0x2|*/ uint16_t Flags;
	/* 0x2 */ /*|0x2|*/ uint16_t Length;
	/* 0x4 */ /*|0x4|*/ uint32_t TimeStamp;
	/* 0x8 */ /*|0x8|*/ ansi_string DosPath;
};

static const unsigned rtl_max_drive_letters = 32;

struct rtl_user_process_parameters
{
	/* 0x0 */ /*|0x4|*/ uint32_t MaximumLength;
	/* 0x4 */ /*|0x4|*/ uint32_t Length;
	/* 0x8 */ /*|0x4|*/ uint32_t Flags;
	/* 0xc */ /*|0x4|*/ uint32_t DebugFlags;
	/* 0x10 */ /*|0x4|*/ legacy_handle ConsoleHandle;
	/* 0x14 */ /*|0x4|*/ uint32_t ConsoleFlags;
	/* 0x18 */ /*|0x4|*/ legacy_handle StandardInput;
	/* 0x1c */ /*|0x4|*/ legacy_handle StandardOutput;
	/* 0x20 */ /*|0x4|*/ legacy_handle StandardError;
	/* 0x24 */ /*|0xc|*/ curdir CurrentDirectory;
	/* 0x30 */ /*|0x8|*/ unicode_string DllPath;
	/* 0x38 */ /*|0x8|*/ unicode_string ImagePathName;
	/* 0x40 */ /*|0x8|*/ unicode_string CommandLine;
	/* 0x48 */ /*|0x4|*/ void* Environment;
	/* 0x4c */ /*|0x4|*/ uint32_t StartingX;
	/* 0x50 */ /*|0x4|*/ uint32_t StartingY;
	/* 0x54 */ /*|0x4|*/ uint32_t CountX;
	/* 0x58 */ /*|0x4|*/ uint32_t CountY;
	/* 0x5c */ /*|0x4|*/ uint32_t CountCharsX;
	/* 0x60 */ /*|0x4|*/ uint32_t CountCharsY;
	/* 0x64 */ /*|0x4|*/ uint32_t FillAttribute;
	/* 0x68 */ /*|0x4|*/ uint32_t WindowFlags;
	/* 0x6c */ /*|0x4|*/ uint32_t ShowWindowFlags;
	/* 0x70 */ /*|0x8|*/ unicode_string WindowTitle;
	/* 0x78 */ /*|0x8|*/ unicode_string DesktopInfo;
	/* 0x80 */ /*|0x8|*/ unicode_string ShellInfo;
	/* 0x88 */ /*|0x8|*/ unicode_string RuntimeData;
	/* 0x90 */ /*|0x200|*/ rtl_drive_letter_curdir CurrentDirectores[rtl_max_drive_letters];
};

struct peb
{
  static __forceinline
  peb& instance()
  {
    return *teb::get(&teb::ProcessEnvironmentBlock);
  }
  
  struct ldr_data
  {
    /* 0x00 */  uint32_t      Length;
    /* 0x04 */  bool          Initialized;
    /* 0x08 */  legacy_handle SsHandle;
    /* 0x0c */  list_head     InLoadOrderModuleList;
    /* 0x14 */  list_head     InMemoryOrderModuleList;
    /* 0x1c */  list_head     InInitializationOrderModuleList;
    /* 0x24 */  void *        EntryInProgress;
  };

  struct find_dll
  {
    find_dll(nt::peb * peb) : peb(peb) {}

    nt::peb * peb;

    const pe::image * operator()(const char name[]) const
    {
      if ( !peb ) return 0;
      nt::ldr_data_table_entry::find_dll find_dll(&peb->Ldr->InLoadOrderModuleList);
      return find_dll(name);
    }
  };

  typedef void __stdcall lock_routine_t(rtl_critical_section * PebLock);  
  typedef void __stdcall kernel_callback_t(void *);

  struct free_block
  {
    free_block * Next;
    uint32_t     Size;
  };

  /* 0x00 */  bool                    InheritedAddressSpace;
  /* 0x01 */  bool                    ReadImageFileExecOptions;
  /* 0x02 */  bool                    BeingDebugged;

#if defined(_M_IX86)

  /* 0x03 */  bool                    SpareBool;
  /* 0x04 */  legacy_handle           Mutant;
  /* 0x08 */  pe::image *             ImageBaseAddress;
  /* 0x0c */  ldr_data *              Ldr;
  /* 0x10 */  rtl_user_process_parameters* ProcessParameters;
  /* 0x14 */  void *                  SubSystemData;
  /* 0x18 */  rtl_heap *              ProcessHeap;

#elif defined(_M_X64)

  /*<thisrel this+0x3>*/ /*|0x1|*/ uint8_t BitField;
  /*<bitfield this+0x3>*/ /*|0x1|*/ uint8_t ImageUsesLargePages:1;
  /*<bitfield this+0x3>*/ /*|0x1|*/ uint8_t SpareBits:7;
  /*<thisrel this+0x8>*/ /*|0x8|*/ void* Mutant;
  /*<thisrel this+0x10>*/ /*|0x8|*/ pe::image* ImageBaseAddress;
  /*<thisrel this+0x18>*/ /*|0x8|*/ ldr_data* Ldr;
  /*<thisrel this+0x20>*/ /*|0x8|*/ rtl_user_process_parameters* ProcessParameters;
  /*<thisrel this+0x28>*/ /*|0x8|*/ void* SubSystemData;
  /*<thisrel this+0x30>*/ /*|0x8|*/ rtl_heap* ProcessHeap;
#endif

};

#if defined(_M_IX86)

struct peb51: peb
{
  /* 0x1c */  rtl_critical_section *  FastPebLock;
  /* 0x20 */  lock_routine_t *        FastPebLockRoutine;
  /* 0x24 */  lock_routine_t *        FastPebUnlockRoutine;
  /* 0x28 */  uint32_t                EnvironmentUpdateCount;
  /* 0x2c */  kernel_callback_t * *   KernelCallbackTable;
  /* 0x30 */  uintptr_t               SystemReserved[1];  // EventLogSection
  /* 0x34 */  uintptr_t               AtlThunkSListPtr32; // EventLog
  /* 0x38 */  free_block *            FreeList;
  /* 0x3c */  uint32_t                TlsExpansionCounter;
  /* 0x40 */  void *                  TlsBitmap;
  /* 0x44 */  uint32_t                TlsBitmapBits[2];
  /* 0x4c */  void *                  ReadOnlySharedMemoryBase;
  /* 0x50 */  void *                  ReadOnlySharedMemoryHeap;
  /* 0x54 */  void **                 ReadOnlyStaticServerData;
  /* 0x58 */  void *                  AnsiCodePageData;
  /* 0x5c */  void *                  OemCodePageData;
  /* 0x60 */  void *                  UnicodeCaseTableData;
  /* 0x64 */  uint32_t                NumberOfProcessors;
  /* 0x68 */  uint32_t                NtGlobalFlag;
  /* 0x70 */  int64_t                 CriticalSectionTimeout;
  /* 0x78 */  uint32_t                HeapSegmentReserve;
  /* 0x7c */  uint32_t                HeapSegmentCommit;
  /* 0x80 */  uint32_t                HeapDeCommitTotalFreeThreshold;
  /* 0x84 */  uint32_t                HeapDeCommitFreeBlockThreshold;
  /* 0x88 */  uint32_t                NumberOfHeaps;
  /* 0x8c */  uint32_t                MaximumNumberOfHeaps;
  /* 0x90 */  void **                 ProcessHeaps;
  /* 0x94 */  void *                  GdiSharedHandleTable;
  /* 0x98 */  void *                  ProcessStarterHelper;
  /* 0x9c */  uint32_t                GdiDCAttributeList;
  /* 0xa0 */  void *                  LoaderLock;
  /* 0xa4 */  uint32_t                OSMajorVersion;
  /* 0xa8 */  uint32_t                OSMinorVersion;
  /* 0xac */  uint16_t                OSBuildNumber;
  /* 0xae */  uint16_t                OSCSDVersion;
  /* 0xb0 */  uint32_t                OSPlatformId;
  /* 0xb4 */  uint32_t                ImageSubsystem;
  /* 0xb8 */  uint32_t                ImageSubsystemMajorVersion;
  /* 0xbc */  uint32_t                ImageSubsystemMinorVersion;
  /* 0xc0 */  uint32_t                ImageProcessAffinityMask;
  /* 0xc4 */  legacy_handle           GdiHandleBuffer[34];
  /* 0x14c */ void                  (*PostProcessInitRoutine)();
  /* 0x150 */ void *                  TlsExpansionBitmap;
  /* 0x154 */ uint32_t                TlsExpansionBitmapBits[32];
  /* 0x1d4 */ uint32_t                SessionId;
  /* 0x1d8 */ uint64_t                AppCompatFlags;
  /* 0x1e0 */ uint64_t                AppCompatFlagsUser;
  /* 0x1e8 */ void *                  pShimData;
  /* 0x1ec */ void *                  AppCompatInfo;
  /* 0x1f0 */ unicode_string          CSDVersion;
  /* 0x1f8 */ void *                  ActivationContextData;
  /* 0x1fc */ void *                  ProcessAssemblyStorageMap;
  /* 0x200 */ void *                  SystemDefaultActivationContextData;
  /* 0x204 */ void *                  SystemAssemblyStorageMap;
  /* 0x208 */ uint32_t                MinimumStackCommit;
};
// <size 0x210 

struct peb52: peb
{
  /* 0x1c */ /*|0x4|*/ rtl_critical_section* FastPebLock;
  /* 0x20 */ /*|0x4|*/ void* SparePtr1;
  /* 0x24 */ /*|0x4|*/ void* SparePtr2;
  /* 0x28 */ /*|0x4|*/ uint32_t EnvironmentUpdateCount;
  /* 0x2c */ /*|0x4|*/ kernel_callback_t** KernelCallbackTable;
  /* 0x30 */ /*|0x4|*/ uint32_t SystemReserved[1];
  /* 0x34 */ /*|0x4|*/ uint32_t ExecuteOptions:2;
  /* 0x34 */ /*|0x4|*/ uint32_t SpareBits:0x1E;
  /* 0x38 */ /*|0x4|*/ free_block* FreeList;
  /* 0x3c */ /*|0x4|*/ uint32_t TlsExpansionCounter;
  /* 0x40 */ /*|0x4|*/ void* TlsBitmap;
  /* 0x44 */ /*|0x8|*/ uint32_t TlsBitmapBits[2];
  /* 0x4c */ /*|0x4|*/ void* ReadOnlySharedMemoryBase;
  /* 0x50 */ /*|0x4|*/ void* ReadOnlySharedMemoryHeap;
  /* 0x54 */ /*|0x4|*/ void** ReadOnlyStaticServerData;
  /* 0x58 */ /*|0x4|*/ void* AnsiCodePageData;
  /* 0x5c */ /*|0x4|*/ void* OemCodePageData;
  /* 0x60 */ /*|0x4|*/ void* UnicodeCaseTableData;
  /* 0x64 */ /*|0x4|*/ uint32_t NumberOfProcessors;
  /* 0x68 */ /*|0x4|*/ uint32_t NtGlobalFlag;
  /* 0x70 */ /*|0x8|*/ int64_t CriticalSectionTimeout;
  /* 0x78 */ /*|0x4|*/ uint32_t HeapSegmentReserve;
  /* 0x7c */ /*|0x4|*/ uint32_t HeapSegmentCommit;
  /* 0x80 */ /*|0x4|*/ uint32_t HeapDeCommitTotalFreeThreshold;
  /* 0x84 */ /*|0x4|*/ uint32_t HeapDeCommitFreeBlockThreshold;
  /* 0x88 */ /*|0x4|*/ uint32_t NumberOfHeaps;
  /* 0x8c */ /*|0x4|*/ uint32_t MaximumNumberOfHeaps;
  /* 0x90 */ /*|0x4|*/ rtl_heap** ProcessHeaps;
  /* 0x94 */ /*|0x4|*/ void* GdiSharedHandleTable;
  /* 0x98 */ /*|0x4|*/ void* ProcessStarterHelper;
  /* 0x9c */ /*|0x4|*/ uint32_t GdiDCAttributeList;
  /* 0xa0 */ /*|0x4|*/ rtl_critical_section* LoaderLock;
  /* 0xa4 */ /*|0x4|*/ uint32_t OSMajorVersion;
  /* 0xa8 */ /*|0x4|*/ uint32_t OSMinorVersion;
  /* 0xac */ /*|0x2|*/ uint16_t OSBuildNumber;
  /* 0xae */ /*|0x2|*/ uint16_t OSCSDVersion;
  /* 0xb0 */ /*|0x4|*/ uint32_t OSPlatformId;
  /* 0xb4 */ /*|0x4|*/ uint32_t ImageSubsystem;
  /* 0xb8 */ /*|0x4|*/ uint32_t ImageSubsystemMajorVersion;
  /* 0xbc */ /*|0x4|*/ uint32_t ImageSubsystemMinorVersion;
  /* 0xc0 */ /*|0x4|*/ uint32_t ImageProcessAffinityMask;
  /* 0xc4 */ /*|0x88|*/ legacy_handle GdiHandleBuffer[34];
  /* 0x14c */ /*|0x4|*/ void  (*PostProcessInitRoutine)();
  /* 0x150 */ /*|0x4|*/ void* TlsExpansionBitmap;
  /* 0x154 */ /*|0x80|*/ uint32_t TlsExpansionBitmapBits[32];
  /* 0x1d4 */ /*|0x4|*/ uint32_t SessionId;
  /* 0x1d8 */ /*|0x8|*/ uint64_t AppCompatFlags;
  /* 0x1e0 */ /*|0x8|*/ uint64_t AppCompatFlagsUser;
  /* 0x1e8 */ /*|0x4|*/ void* pShimData;
  /* 0x1ec */ /*|0x4|*/ void* AppCompatInfo;
  /* 0x1f0 */ /*|0x8|*/ unicode_string CSDVersion;
  /* 0x1f8 */ /*|0x4|*/ struct activation_context_data* ActivationContextData;
  /* 0x1fc */ /*|0x4|*/ struct assembly_storage_map* ProcessAssemblyStorageMap;
  /* 0x200 */ /*|0x4|*/ struct activation_context_data* SystemDefaultActivationContextData;
  /* 0x204 */ /*|0x4|*/ struct assembly_storage_map* SystemAssemblyStorageMap;
  /* 0x208 */ /*|0x4|*/ uint32_t MinimumStackCommit;
  /* 0x20c */ /*|0x4|*/ void** FlsCallback;
  /* 0x210 */ /*|0x8|*/ list_entry FlsListHead;
  /* 0x218 */ /*|0x4|*/ void* FlsBitmap;
  /* 0x21c */ /*|0x10|*/ uint32_t FlsBitmapBits[4];
  /* 0x22c */ /*|0x4|*/ uint32_t FlsHighIndex;
};
// <size 0x230 


#elif defined(_M_X64)

struct peb52: peb
{
  /*<thisrel this+0x38>*/ /*|0x8|*/ rtl_critical_section* FastPebLock;
  /*<thisrel this+0x40>*/ /*|0x8|*/ void* AtlThunkSListPtr;
  /*<thisrel this+0x48>*/ /*|0x8|*/ void* SparePtr2;
  /*<thisrel this+0x50>*/ /*|0x4|*/ uint32_t EnvironmentUpdateCount;
  /*<thisrel this+0x58>*/ /*|0x8|*/ kernel_callback_t** KernelCallbackTable;
  /*<thisrel this+0x60>*/ /*|0x4|*/ uint32_t SystemReserved[1];
  /*<thisrel this+0x64>*/ /*|0x4|*/ uint32_t SpareUlong;
  /*<thisrel this+0x68>*/ /*|0x8|*/ free_block* FreeList;
  /*<thisrel this+0x70>*/ /*|0x4|*/ uint32_t TlsExpansionCounter;
  /*<thisrel this+0x78>*/ /*|0x8|*/ void* TlsBitmap;
  /*<thisrel this+0x80>*/ /*|0x8|*/ uint32_t TlsBitmapBits[2];
  /*<thisrel this+0x88>*/ /*|0x8|*/ void* ReadOnlySharedMemoryBase;
  /*<thisrel this+0x90>*/ /*|0x8|*/ void* ReadOnlySharedMemoryHeap;
  /*<thisrel this+0x98>*/ /*|0x8|*/ void** ReadOnlyStaticServerData;
  /*<thisrel this+0xa0>*/ /*|0x8|*/ void* AnsiCodePageData;
  /*<thisrel this+0xa8>*/ /*|0x8|*/ void* OemCodePageData;
  /*<thisrel this+0xb0>*/ /*|0x8|*/ void* UnicodeCaseTableData;
  /*<thisrel this+0xb8>*/ /*|0x4|*/ uint32_t NumberOfProcessors;
  /*<thisrel this+0xbc>*/ /*|0x4|*/ uint32_t NtGlobalFlag;
  /*<thisrel this+0xc0>*/ /*|0x8|*/ int64_t CriticalSectionTimeout;
  /*<thisrel this+0xc8>*/ /*|0x8|*/ uint64_t HeapSegmentReserve;
  /*<thisrel this+0xd0>*/ /*|0x8|*/ uint64_t HeapSegmentCommit;
  /*<thisrel this+0xd8>*/ /*|0x8|*/ uint64_t HeapDeCommitTotalFreeThreshold;
  /*<thisrel this+0xe0>*/ /*|0x8|*/ uint64_t HeapDeCommitFreeBlockThreshold;
  /*<thisrel this+0xe8>*/ /*|0x4|*/ uint32_t NumberOfHeaps;
  /*<thisrel this+0xec>*/ /*|0x4|*/ uint32_t MaximumNumberOfHeaps;
  /*<thisrel this+0xf0>*/ /*|0x8|*/ rtl_heap** ProcessHeaps;
  /*<thisrel this+0xf8>*/ /*|0x8|*/ void* GdiSharedHandleTable;
  /*<thisrel this+0x100>*/ /*|0x8|*/ void* ProcessStarterHelper;
  /*<thisrel this+0x108>*/ /*|0x4|*/ uint32_t GdiDCAttributeList;
  /*<thisrel this+0x110>*/ /*|0x8|*/ rtl_critical_section* LoaderLock;
  /*<thisrel this+0x118>*/ /*|0x4|*/ uint32_t OSMajorVersion;
  /*<thisrel this+0x11c>*/ /*|0x4|*/ uint32_t OSMinorVersion;
  /*<thisrel this+0x120>*/ /*|0x2|*/ uint16_t OSBuildNumber;
  /*<thisrel this+0x122>*/ /*|0x2|*/ uint16_t OSCSDVersion;
  /*<thisrel this+0x124>*/ /*|0x4|*/ uint32_t OSPlatformId;
  /*<thisrel this+0x128>*/ /*|0x4|*/ uint32_t ImageSubsystem;
  /*<thisrel this+0x12c>*/ /*|0x4|*/ uint32_t ImageSubsystemMajorVersion;
  /*<thisrel this+0x130>*/ /*|0x4|*/ uint32_t ImageSubsystemMinorVersion;
  /*<thisrel this+0x138>*/ /*|0x8|*/ uint64_t ImageProcessAffinityMask;
  /*<thisrel this+0x140>*/ /*|0xf0|*/ uint32_t GdiHandleBuffer[60];
  /*<thisrel this+0x230>*/ /*|0x8|*/ void  (*PostProcessInitRoutine)();
  /*<thisrel this+0x238>*/ /*|0x8|*/ void* TlsExpansionBitmap;
  /*<thisrel this+0x240>*/ /*|0x80|*/ uint32_t TlsExpansionBitmapBits[32];
  /*<thisrel this+0x2c0>*/ /*|0x4|*/ uint32_t SessionId;
  /*<thisrel this+0x2c8>*/ /*|0x8|*/ uint64_t AppCompatFlags;
  /*<thisrel this+0x2d0>*/ /*|0x8|*/ uint64_t AppCompatFlagsUser;
  /*<thisrel this+0x2d8>*/ /*|0x8|*/ void* pShimData;
  /*<thisrel this+0x2e0>*/ /*|0x8|*/ void* AppCompatInfo;
  /*<thisrel this+0x2e8>*/ /*|0x10|*/ unicode_string CSDVersion;
  /*<thisrel this+0x2f8>*/ /*|0x8|*/ struct activation_context_data* ActivationContextData;
  /*<thisrel this+0x300>*/ /*|0x8|*/ struct assembly_storage_map* ProcessAssemblyStorageMap;
  /*<thisrel this+0x308>*/ /*|0x8|*/ struct activation_context_data* SystemDefaultActivationContextData;
  /*<thisrel this+0x310>*/ /*|0x8|*/ struct assembly_storage_map* SystemAssemblyStorageMap;
  /*<thisrel this+0x318>*/ /*|0x8|*/ uint64_t MinimumStackCommit;
  /*<thisrel this+0x320>*/ /*|0x8|*/ void** FlsCallback;
  /*<thisrel this+0x328>*/ /*|0x10|*/ list_entry FlsListHead;
  /*<thisrel this+0x338>*/ /*|0x8|*/ void* FlsBitmap;
  /*<thisrel this+0x340>*/ /*|0x10|*/ uint32_t FlsBitmapBits[4];
  /*<thisrel this+0x350>*/ /*|0x4|*/ uint32_t FlsHighIndex; 
};
// <size 0x358>

#endif


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_PEB
