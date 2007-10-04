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

namespace ntl {
namespace nt {

struct rtl_user_process_parameters;
struct rtl_critacal_section;
struct rtl_heap;


struct peb
{
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

  typedef void __stdcall lock_routine_t(rtl_critacal_section * PebLock);  
  typedef void __stdcall kernel_callback_t(void *);

  struct free_block
  {
    free_block * Next;
    uint32_t     Size;
  };

  /* 0x00 */  bool                    InheritedAddressSpace;
  /* 0x01 */  bool                    ReadImageFileExecOptions;
  /* 0x02 */  bool                    BeingDebugged;
  /* 0x03 */  bool                    SpareBool;
  /* 0x04 */  legacy_handle           Mutant;
  /* 0x08 */  pe::image *             ImageBaseAddress;
  /* 0x0c */  ldr_data *              Ldr;
  /* 0x10 */  rtl_user_process_parameters* ProcessParameters;
  /* 0x14 */  void *                  SubSystemData;
  /* 0x18 */  rtl_heap *              ProcessHeap;
  /* 0x1c */  rtl_critacal_section *  FastPebLock;
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
  /* 0x9c */  uintptr_t               GdiDCAttributeList;
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
//STATIC_ASSERT(sizeof(peb) == 0x210);


}//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_PEB
