/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Processes
 *
 ****************************************************************************
 */

#ifndef NTL__KM_PROCESS
#define NTL__KM_PROCESS

#include "basedef.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "object.hxx"
#include "thread.hxx"
#include "system_information.hxx"
#include "../pe/image.hxx"
#include "mm.hxx"

namespace ntl {
namespace km {

struct nt::peb;

struct kprocess;
struct eprocess;
struct object_name_information;

NTL__EXTERNAPI
kprocess * __stdcall
  IoGetCurrentProcess();


struct kprocess
{
  static
  kprocess * get_current()
  {
#ifndef NTL_SUPPRESS_IMPORT
    return IoGetCurrentProcess();
#else
    return kthread::get_current()->ApcState.Process;
#endif
  }

};


struct kidtentry
{
	/*<thisrel this+0x0>*/ /*|0x2|*/ uint16_t Offset;
	/*<thisrel this+0x2>*/ /*|0x2|*/ uint16_t Selector;
	/*<thisrel this+0x4>*/ /*|0x2|*/ uint16_t Access;
	/*<thisrel this+0x6>*/ /*|0x2|*/ uint16_t ExtendedOffset;
}; // sizeof = 0x08

struct kgdtentry
{
	/*<thisrel this+0x0>*/ /*|0x2|*/ uint16_t LimitLow;
	/*<thisrel this+0x2>*/ /*|0x2|*/ uint16_t BaseLow;
	/*<thisrel this+0x4>*/ /*|0x4|*/
	union {
		struct {
			uint8_t   BaseMid;
			uint8_t   Flags1;
			uint8_t   Flags2;
			uint8_t   BaseHi;
		} Bytes;
		struct {
			uint32_t   BaseMid: 8;
			uint32_t   Type: 5;
			uint32_t   Dpl: 2;
			uint32_t   Pres: 1;
			uint32_t   LimitHi: 4;
			uint32_t   Sys: 1;
			uint32_t   Reserved_0: 1;
			uint32_t   Default_Big: 1;
			uint32_t   Granularity: 1;
			uint32_t   BaseHi: 8;
		} Bits;
	} HighWord;
}; // sizeof = 0x08

struct kexecute_options
{
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t ExecuteDisable:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t ExecuteEnable:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t DisableThunkEmulation:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t Permanent:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t ExecuteDispatchEnable:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t ImageDispatchEnable:1;
	/*<bitfield this+0x0>*/ /*|0x1|*/ uint8_t Spare:2;
}; // sizeof = 0x01


struct eprocess_quota_entry
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Usage;
	/*<thisrel this+0x4>*/ /*|0x4|*/ uint32_t Limit;
	/*<thisrel this+0x8>*/ /*|0x4|*/ uint32_t Peak;
	/*<thisrel this+0xc>*/ /*|0x4|*/ uint32_t Return;
};

struct eprocess_quota_block
{
	/*<thisrel this+0x0>*/ /*|0x30|*/ eprocess_quota_entry QuotaEntry[3];
	/*<thisrel this+0x30>*/ /*|0x8|*/ list_entry QuotaList;
	/*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t ReferenceCount;
	/*<thisrel this+0x3c>*/ /*|0x4|*/ uint32_t ProcessCount;
};

struct handle_trace_db_entry
{
	/*<thisrel this+0x0>*/ /*|0x10|*/ client_id ClientId;
	/*<thisrel this+0x10>*/ /*|0x8|*/ void* Handle;
	/*<thisrel this+0x18>*/ /*|0x4|*/ uint32_t Type;
	/*<thisrel this+0x20>*/ /*|0x80|*/ void* StackTrace[16];
};

struct handle_trace_debug_info
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t CurrentStackIndex;
	/*<thisrel this+0x4>*/ /*|0x50000|*/ handle_trace_db_entry TraceDb[4096];
};

struct handle_table
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t TableCode;
	/*<thisrel this+0x4>*/ /*|0x4|*/ eprocess* QuotaProcess;
	/*<thisrel this+0x8>*/ /*|0x4|*/ void* UniqueProcessId;
	/*<thisrel this+0xc>*/ /*|0x10|*/ ex_push_lock HandleTableLock[4];
	/*<thisrel this+0x1c>*/ /*|0x8|*/ list_entry HandleTableList;
	/*<thisrel this+0x24>*/ /*|0x4|*/ ex_push_lock HandleContentionEvent;
	/*<thisrel this+0x28>*/ /*|0x4|*/ handle_trace_debug_info* DebugInfo;
	/*<thisrel this+0x2c>*/ /*|0x4|*/ int32_t ExtraInfoPages;
	/*<thisrel this+0x30>*/ /*|0x4|*/ uint32_t FirstFree;
	/*<thisrel this+0x34>*/ /*|0x4|*/ uint32_t LastFree;
	/*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t NextHandleNeedingPool;
	/*<thisrel this+0x3c>*/ /*|0x4|*/ int32_t HandleCount;
	/*<thisrel this+0x40>*/ /*|0x4|*/ uint32_t Flags;
	/*<bitfield this+0x40>*/ /*|0x1|*/ int8_t StrictFIFO:1;
};

struct sid_and_attributes
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ void* Sid;
	/*<thisrel this+0x4>*/ /*|0x4|*/ uint32_t Attributes;
};

struct luid
{
	uint32_t LowPart;
	int32_t HighPart;
};

struct luid_and_attributes
{
	/*<thisrel this+0x0>*/ /*|0x8|*/ luid Luid;
	/*<thisrel this+0x8>*/ /*|0x4|*/ uint32_t Attributes;
};

struct ps_job_token_filter
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t CapturedSidCount;
	/*<thisrel this+0x4>*/ /*|0x4|*/ sid_and_attributes* CapturedSids;
	/*<thisrel this+0x8>*/ /*|0x4|*/ uint32_t CapturedSidsLength;
	/*<thisrel this+0xc>*/ /*|0x4|*/ uint32_t CapturedGroupCount;
	/*<thisrel this+0x10>*/ /*|0x4|*/ sid_and_attributes* CapturedGroups;
	/*<thisrel this+0x14>*/ /*|0x4|*/ uint32_t CapturedGroupsLength;
	/*<thisrel this+0x18>*/ /*|0x4|*/ uint32_t CapturedPrivilegeCount;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ luid_and_attributes* CapturedPrivileges;
	/*<thisrel this+0x20>*/ /*|0x4|*/ uint32_t CapturedPrivilegesLength;
};

struct io_counters
{
	/*<thisrel this+0x0>*/ /*|0x8|*/ uint64_t ReadOperationCount;
	/*<thisrel this+0x8>*/ /*|0x8|*/ uint64_t WriteOperationCount;
	/*<thisrel this+0x10>*/ /*|0x8|*/ uint64_t OtherOperationCount;
	/*<thisrel this+0x18>*/ /*|0x8|*/ uint64_t ReadTransferCount;
	/*<thisrel this+0x20>*/ /*|0x8|*/ uint64_t WriteTransferCount;
	/*<thisrel this+0x28>*/ /*|0x8|*/ uint64_t OtherTransferCount;
};

struct process_ws_watch_information
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ void* FaultingPc;
	/*<thisrel this+0x4>*/ /*|0x4|*/ void* FaultingVa;
};

struct pagefault_history
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t CurrentIndex;
	/*<thisrel this+0x4>*/ /*|0x4|*/ uint32_t MaxIndex;
	/*<thisrel this+0x8>*/ /*|0x4|*/ uint32_t SpinLock;
	/*<thisrel this+0xc>*/ /*|0x4|*/ void* Reserved;
	/*<thisrel this+0x10>*/ /*|0x8|*/ process_ws_watch_information WatchInfo[1];
};

struct se_audit_process_creation_info
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ object_name_information* ImageFileName;
};

struct ejob
{
	/*<thisrel this+0x0>*/ /*|0x10|*/ kevent Event;
	/*<thisrel this+0x10>*/ /*|0x8|*/ list_entry JobLinks;
	/*<thisrel this+0x18>*/ /*|0x8|*/ list_entry ProcessListHead;
	/*<thisrel this+0x20>*/ /*|0x38|*/ eresource JobLock;
	/*<thisrel this+0x58>*/ /*|0x8|*/ int64_t TotalUserTime;
	/*<thisrel this+0x60>*/ /*|0x8|*/ int64_t TotalKernelTime;
	/*<thisrel this+0x68>*/ /*|0x8|*/ int64_t ThisPeriodTotalUserTime;
	/*<thisrel this+0x70>*/ /*|0x8|*/ int64_t ThisPeriodTotalKernelTime;
	/*<thisrel this+0x78>*/ /*|0x4|*/ uint32_t TotalPageFaultCount;
	/*<thisrel this+0x7c>*/ /*|0x4|*/ uint32_t TotalProcesses;
	/*<thisrel this+0x80>*/ /*|0x4|*/ uint32_t ActiveProcesses;
	/*<thisrel this+0x84>*/ /*|0x4|*/ uint32_t TotalTerminatedProcesses;
	/*<thisrel this+0x88>*/ /*|0x8|*/ int64_t PerProcessUserTimeLimit;
	/*<thisrel this+0x90>*/ /*|0x8|*/ int64_t PerJobUserTimeLimit;
	/*<thisrel this+0x98>*/ /*|0x4|*/ uint32_t LimitFlags;
	/*<thisrel this+0x9c>*/ /*|0x4|*/ uint32_t MinimumWorkingSetSize;
	/*<thisrel this+0xa0>*/ /*|0x4|*/ uint32_t MaximumWorkingSetSize;
	/*<thisrel this+0xa4>*/ /*|0x4|*/ uint32_t ActiveProcessLimit;
	/*<thisrel this+0xa8>*/ /*|0x4|*/ uint32_t Affinity;
	/*<thisrel this+0xac>*/ /*|0x1|*/ int8_t PriorityClass;
	/*<thisrel this+0xb0>*/ /*|0x4|*/ uint32_t UIRestrictionsClass;
	/*<thisrel this+0xb4>*/ /*|0x4|*/ uint32_t SecurityLimitFlags;
	/*<thisrel this+0xb8>*/ /*|0x4|*/ void* Token;
	/*<thisrel this+0xbc>*/ /*|0x4|*/ ps_job_token_filter* Filter;
	/*<thisrel this+0xc0>*/ /*|0x4|*/ uint32_t EndOfJobTimeAction;
	/*<thisrel this+0xc4>*/ /*|0x4|*/ void* CompletionPort;
	/*<thisrel this+0xc8>*/ /*|0x4|*/ void* CompletionKey;
	/*<thisrel this+0xcc>*/ /*|0x4|*/ uint32_t SessionId;
	/*<thisrel this+0xd0>*/ /*|0x4|*/ uint32_t SchedulingClass;
	/*<thisrel this+0xd8>*/ /*|0x8|*/ uint64_t ReadOperationCount;
	/*<thisrel this+0xe0>*/ /*|0x8|*/ uint64_t WriteOperationCount;
	/*<thisrel this+0xe8>*/ /*|0x8|*/ uint64_t OtherOperationCount;
	/*<thisrel this+0xf0>*/ /*|0x8|*/ uint64_t ReadTransferCount;
	/*<thisrel this+0xf8>*/ /*|0x8|*/ uint64_t WriteTransferCount;
	/*<thisrel this+0x100>*/ /*|0x8|*/ uint64_t OtherTransferCount;
	/*<thisrel this+0x108>*/ /*|0x30|*/ io_counters IoInfo;
	/*<thisrel this+0x138>*/ /*|0x4|*/ uint32_t ProcessMemoryLimit;
	/*<thisrel this+0x13c>*/ /*|0x4|*/ uint32_t JobMemoryLimit;
	/*<thisrel this+0x140>*/ /*|0x4|*/ uint32_t PeakProcessMemoryUsed;
	/*<thisrel this+0x144>*/ /*|0x4|*/ uint32_t PeakJobMemoryUsed;
	/*<thisrel this+0x148>*/ /*|0x4|*/ uint32_t CurrentJobMemoryUsed;
	/*<thisrel this+0x14c>*/ /*|0x20|*/ fast_mutex MemoryLimitsLock;
	/*<thisrel this+0x16c>*/ /*|0x8|*/ list_entry JobSetLinks;
	/*<thisrel this+0x174>*/ /*|0x4|*/ uint32_t MemberLevel;
	/*<thisrel this+0x178>*/ /*|0x4|*/ uint32_t JobFlags;
};

struct alpc_process_context
{
  ex_push_lock  Lock;
  list_entry    ViewListHead;
  uint32_t      PagedPoolQuotaCache;
};

#if defined(_M_IX86)

// 32bit structures
struct kprocess50;  // 2k
struct kprocess51;  // xp, 2k3 sp0
struct kprocess52;  // 2k3 sp1+
struct kprocess60;  // vista
struct kprocess61;  // 2k8

struct eprocess50;  // 2k
struct eprocess51;  // xp
struct eprocess52;  // 2k3 sp0
struct eprocess521; // 2k3 sp1
struct eprocess60;  // vista
struct kprocess61;  // 2k8

struct kprocess50
{
  dispatcher_header Header;
  list_entry ProfileListHead;
  uintptr_t DirectoryTableBase[2];
  kgdtentry LdtDescriptor;
  kidtentry Int21Descriptor;
  uint16_t IopmOffset;
  uint8_t Iopl;
  bool VdmFlag;
  kaffinity ActiveProcessors;
  uint32_t KernelTime;
  uint32_t UserTime;
  list_entry ReadyListHead;
  list_entry SwapListEntry;
  list_entry ThreadListHead;
  kspin_lock ProcessLock;
  kaffinity Affinity;
  uint16_t StackCount;
  int8_t BasePriority;
  int8_t ThreadQuantum;
  bool AutoAlignment;
  uint8_t State;
  uint8_t ThreadSeed;
  bool DisableBoost;
  uint8_t PowerState;
  bool DisableQuantum;
  uint8_t Spare[2];
}; // <size 0x6c>

struct kprocess51
{
	/*<thisrel this+0x0>*/ /*|0x10|*/ dispatcher_header Header;
	/*<thisrel this+0x10>*/ /*|0x8|*/ list_entry ProfileListHead;
	/*<thisrel this+0x18>*/ /*|0x8|*/ uint32_t DirectoryTableBase[2];
	/*<thisrel this+0x20>*/ /*|0x8|*/ kgdtentry LdtDescriptor;
	/*<thisrel this+0x28>*/ /*|0x8|*/ kidtentry Int21Descriptor;
	/*<thisrel this+0x30>*/ /*|0x2|*/ uint16_t IopmOffset;
	/*<thisrel this+0x32>*/ /*|0x1|*/ uint8_t Iopl;
	/*<thisrel this+0x33>*/ /*|0x1|*/ uint8_t Unused;
	/*<thisrel this+0x34>*/ /*|0x4|*/ uint32_t ActiveProcessors;
	/*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t KernelTime;
	/*<thisrel this+0x3c>*/ /*|0x4|*/ uint32_t UserTime;
	/*<thisrel this+0x40>*/ /*|0x8|*/ list_entry ReadyListHead;
	/*<thisrel this+0x48>*/ /*|0x4|*/ single_list_entry SwapListEntry;
	/*<thisrel this+0x4c>*/ /*|0x4|*/ void* VdmTrapcHandler;
	/*<thisrel this+0x50>*/ /*|0x8|*/ list_entry ThreadListHead;
	/*<thisrel this+0x58>*/ /*|0x4|*/ uint32_t ProcessLock;
	/*<thisrel this+0x5c>*/ /*|0x4|*/ uint32_t Affinity;
	/*<thisrel this+0x60>*/ /*|0x2|*/ uint16_t StackCount;
	/*<thisrel this+0x62>*/ /*|0x1|*/ int8_t BasePriority;
	/*<thisrel this+0x63>*/ /*|0x1|*/ int8_t ThreadQuantum;
	/*<thisrel this+0x64>*/ /*|0x1|*/ uint8_t AutoAlignment;
	/*<thisrel this+0x65>*/ /*|0x1|*/ uint8_t State;
	/*<thisrel this+0x66>*/ /*|0x1|*/ uint8_t ThreadSeed;
	/*<thisrel this+0x67>*/ /*|0x1|*/ uint8_t DisableBoost;
	/*<thisrel this+0x68>*/ /*|0x1|*/ uint8_t PowerState;
	/*<thisrel this+0x69>*/ /*|0x1|*/ uint8_t DisableQuantum;
	/*<thisrel this+0x6a>*/ /*|0x1|*/ uint8_t IdealNode;
	/*<thisrel this+0x6b>*/ /*|0x1|*/ kexecute_options Flags;
}; // sizeof = 0x6C

struct kprocess52
{
#if 0
  /*<thisrel this+0x0>*/ /*|0x10|*/ struct _DISPATCHER_HEADER Header;
  /*<thisrel this+0x10>*/ /*|0x8|*/ struct _LIST_ENTRY ProfileListHead;
  /*<thisrel this+0x18>*/ /*|0x8|*/ unsigned long DirectoryTableBase[2];
  /*<thisrel this+0x20>*/ /*|0x8|*/ struct _KGDTENTRY LdtDescriptor;
  /*<thisrel this+0x28>*/ /*|0x8|*/ struct _KIDTENTRY Int21Descriptor;
  /*<thisrel this+0x30>*/ /*|0x2|*/ unsigned short IopmOffset;
  /*<thisrel this+0x32>*/ /*|0x1|*/ unsigned char Iopl;
  /*<thisrel this+0x33>*/ /*|0x1|*/ unsigned char Unused;
  /*<thisrel this+0x34>*/ /*|0x4|*/ unsigned long ActiveProcessors;
  /*<thisrel this+0x38>*/ /*|0x4|*/ unsigned long KernelTime;
  /*<thisrel this+0x3c>*/ /*|0x4|*/ unsigned long UserTime;
  /*<thisrel this+0x40>*/ /*|0x8|*/ struct _LIST_ENTRY ReadyListHead;
  /*<thisrel this+0x48>*/ /*|0x4|*/ struct _SINGLE_LIST_ENTRY SwapListEntry;
  /*<thisrel this+0x4c>*/ /*|0x4|*/ void* VdmTrapcHandler;
  /*<thisrel this+0x50>*/ /*|0x8|*/ struct _LIST_ENTRY ThreadListHead;
  /*<thisrel this+0x58>*/ /*|0x4|*/ unsigned long ProcessLock;
  /*<thisrel this+0x5c>*/ /*|0x4|*/ unsigned long Affinity;
  /*<bitfield this+0x60>*/ /*|0x4|*/ long AutoAlignment:0:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ long DisableBoost:1:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ long DisableQuantum:2:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ long ReservedFlags:3:1d;
  /*<thisrel this+0x60>*/ /*|0x4|*/ long ProcessFlags;
  /*<thisrel this+0x64>*/ /*|0x1|*/ char BasePriority;
  /*<thisrel this+0x65>*/ /*|0x1|*/ char QuantumReset;
  /*<thisrel this+0x66>*/ /*|0x1|*/ unsigned char State;
  /*<thisrel this+0x67>*/ /*|0x1|*/ unsigned char ThreadSeed;
  /*<thisrel this+0x68>*/ /*|0x1|*/ unsigned char PowerState;
  /*<thisrel this+0x69>*/ /*|0x1|*/ unsigned char IdealNode;
  /*<thisrel this+0x6a>*/ /*|0x1|*/ unsigned char Visited;
  /*<thisrel this+0x6b>*/ /*|0x1|*/ struct _KEXECUTE_OPTIONS Flags;
  /*<thisrel this+0x6b>*/ /*|0x1|*/ unsigned char ExecuteOptions;
  /*<thisrel this+0x6c>*/ /*|0x4|*/ unsigned long StackCount;
  /*<thisrel this+0x70>*/ /*|0x8|*/ struct _LIST_ENTRY ProcessListEntry;
#endif
}; // <size 0x78>


struct kprocess60
{
  /*<thisrel this+0x0>*/ /*|0x10|*/ dispatcher_header Header;
  /*<thisrel this+0x10>*/ /*|0x8|*/ list_entry ProfileListHead;
  /*<thisrel this+0x18>*/ /*|0x4|*/ uint32_t DirectoryTableBase;
  /*<thisrel this+0x1c>*/ /*|0x4|*/ uint32_t Unused0;
  /*<thisrel this+0x20>*/ /*|0x8|*/ kgdtentry LdtDescriptor;
  /*<thisrel this+0x28>*/ /*|0x8|*/ kidtentry Int21Descriptor;
  /*<thisrel this+0x30>*/ /*|0x2|*/ uint16_t IopmOffset;
  /*<thisrel this+0x32>*/ /*|0x1|*/ uint8_t Unused1;
  /*<thisrel this+0x33>*/ /*|0x1|*/ uint8_t Unused2;
  /*<thisrel this+0x34>*/ /*|0x4|*/ uint32_t ActiveProcessors;
  /*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t KernelTime;
  /*<thisrel this+0x3c>*/ /*|0x4|*/ uint32_t UserTime;
  /*<thisrel this+0x40>*/ /*|0x8|*/ list_entry ReadyListHead;
  /*<thisrel this+0x48>*/ /*|0x4|*/ single_list_entry SwapListEntry;
  /*<thisrel this+0x4c>*/ /*|0x4|*/ void* VdmTrapcHandler;
  /*<thisrel this+0x50>*/ /*|0x8|*/ list_entry ThreadListHead;
  /*<thisrel this+0x58>*/ /*|0x4|*/ uint32_t ProcessLock;
  /*<thisrel this+0x5c>*/ /*|0x4|*/ uint32_t Affinity;
  /*<bitfield this+0x60>*/ /*|0x4|*/ int32_t AutoAlignment:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ int32_t DisableBoost:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ int32_t DisableQuantum:1;
  /*<bitfield this+0x60>*/ /*|0x4|*/ int32_t ReservedFlags:0x1D;
  /*<thisrel this+0x64>*/ /*|0x1|*/ int8_t BasePriority;
  /*<thisrel this+0x65>*/ /*|0x1|*/ int8_t QuantumReset;
  /*<thisrel this+0x66>*/ /*|0x1|*/ uint8_t State;
  /*<thisrel this+0x67>*/ /*|0x1|*/ uint8_t ThreadSeed;
  /*<thisrel this+0x68>*/ /*|0x1|*/ uint8_t PowerState;
  /*<thisrel this+0x69>*/ /*|0x1|*/ uint8_t IdealNode;
  /*<thisrel this+0x6a>*/ /*|0x1|*/ uint8_t Visited;
  /*<thisrel this+0x6b>*/ /*|0x1|*/ kexecute_options Flags;
  /*<thisrel this+0x6c>*/ /*|0x4|*/ uint32_t StackCount;
  /*<thisrel this+0x70>*/ /*|0x8|*/ list_entry ProcessListEntry;
  /*<thisrel this+0x78>*/ /*|0x8|*/ uint64_t CycleTime;
}; // <size 0x80>

struct kprocess61: kprocess60 {};

struct eprocess50
{
  kprocess50 Pcb;
  ntstatus ExitStatus;
  kevent LockEvent;
  uint32_t LockCount;
  int64_t CreateTime;
  int64_t ExitTime;
  kthread50* LockOwner;
  legacy_handle UniqueProcessId;
  list_entry ActiveProcessLinks;
  size_t QuotaPeakPoolUsage[2];
  size_t QuotaPoolUsage[2];
  size_t PagefileUsage;
  size_t CommitCharge;
  size_t PeakPagefileUsage;
  size_t PeakVirtualSize;
  size_t VirtualSize;
  mmsupport50 Vm;
  list_entry SessionProcessLinks;
  void* DebugPort;
  void* ExceptionPort;
  struct handle_table* ObjectTable;
  struct access_token* Token;
  fast_mutex WorkingSetLock;
  pfn_number_t WorkingSetPage;
  bool ProcessOutswapEnabled;
  bool ProcessOutswapped;
  uint8_t AddressSpaceInitialized;
  bool AddressSpaceDeleted;
  fast_mutex AddressCreationLock;
  kspin_lock HyperSpaceLock;
  ethread* ForkInProgress;
  uint16_t VmOperation;
  uint8_t ForkWasSuccessful;
  uint8_t MmAgressiveWsTrimMask;
  kevent* VmOperationEvent;
  void* PaeTop;
  uint32_t LastFaultCount;
  uint32_t ModifiedPageCount;
  void* VadRoot;
  void* VadHint;
  void* CloneRoot;
  pfn_number_t NumberOfPrivatePages;
  pfn_number_t NumberOfLockedPages;
  uint16_t NextPageColor;
  bool ExitProcessCalled;
  bool CreateProcessReported;
  legacy_handle SectionHandle;
  nt::peb* Peb;
  void* SectionBaseAddress;
  eprocess_quota_block* QuotaBlock;
  ntstatus LastThreadExitStatus;
  struct pagefault_history* WorkingSetWatch;
  legacy_handle Win32WindowStation;
  legacy_handle InheritedFromUniqueProcessId;
  access_mask GrantedAccess;
  uint32_t DefaultHardErrorProcessing;
  void* LdtInformation;
  void* VadFreeHint;
  void* VdmObjects;
  void* DeviceMap;
  uint32_t SessionId;
  list_entry PhysicalVadList;
  hardware_pte PageDirectoryPte;
  uint32_t reserved;
  uint32_t PaePageDirectoryPage;
  uint8_t ImageFileName[ 16 ];
  uint32_t VmTrimFaultValue;
  bool SetTimerResolution;
  uint8_t PriorityClass;
  uint8_t SubSystemMinorVersion;
  uint8_t SubSystemMajorVersion;
  void* Win32Process;
  ejob* Job;
  uint32_t JobStatus;
  list_entry JobLinks;
  void* LockedPagesList;
  void* SecurityPort;
  struct wow64_process* Wow64Process;
  int64_t ReadOperationCount;
  int64_t WriteOperationCount;
  int64_t OtherOperationCount;
  int64_t ReadTransferCount;
  int64_t WriteTransferCount;
  int64_t OtherTransferCount;
  size_t CommitChargeLimit;
  size_t CommitChargePeak;
  list_entry ThreadListHead;
  struct rtl_bitmap* VadPhysicalPagesBitMap;
  uintptr_t VadPhysicalPages;
  kspin_lock AweLock;
}; // <size 0x288>

struct eprocess51
{
	/*<thisrel this+0x0>*/ /*|0x6c|*/ kprocess51 Pcb;
	/*<thisrel this+0x6c>*/ /*|0x4|*/ ex_push_lock ProcessLock;
	/*<thisrel this+0x70>*/ /*|0x8|*/ int64_t CreateTime;
	/*<thisrel this+0x78>*/ /*|0x8|*/ int64_t ExitTime;
	/*<thisrel this+0x80>*/ /*|0x4|*/ ex_rundown_ref RundownProtect;
	/*<thisrel this+0x84>*/ /*|0x4|*/ void* UniqueProcessId;
	/*<thisrel this+0x88>*/ /*|0x8|*/ list_entry ActiveProcessLinks;
	/*<thisrel this+0x90>*/ /*|0xc|*/ uint32_t QuotaUsage[3];
	/*<thisrel this+0x9c>*/ /*|0xc|*/ uint32_t QuotaPeak[3];
	/*<thisrel this+0xa8>*/ /*|0x4|*/ uint32_t CommitCharge;
	/*<thisrel this+0xac>*/ /*|0x4|*/ uint32_t PeakVirtualSize;
	/*<thisrel this+0xb0>*/ /*|0x4|*/ uint32_t VirtualSize;
	/*<thisrel this+0xb4>*/ /*|0x8|*/ list_entry SessionProcessLinks;
	/*<thisrel this+0xbc>*/ /*|0x4|*/ void* DebugPort;
	/*<thisrel this+0xc0>*/ /*|0x4|*/ void* ExceptionPort;
	/*<thisrel this+0xc4>*/ /*|0x4|*/ handle_table* ObjectTable;
	/*<thisrel this+0xc8>*/ /*|0x4|*/ ex_fast_ref Token;
	/*<thisrel this+0xcc>*/ /*|0x20|*/ fast_mutex WorkingSetLock;
	/*<thisrel this+0xec>*/ /*|0x4|*/ uint32_t WorkingSetPage;
	/*<thisrel this+0xf0>*/ /*|0x20|*/ fast_mutex AddressCreationLock;
	/*<thisrel this+0x110>*/ /*|0x4|*/ uint32_t HyperSpaceLock;
	/*<thisrel this+0x114>*/ /*|0x4|*/ ethread* ForkInProgress;
	/*<thisrel this+0x118>*/ /*|0x4|*/ uint32_t HardwareTrigger;
	/*<thisrel this+0x11c>*/ /*|0x4|*/ void* VadRoot;
	/*<thisrel this+0x120>*/ /*|0x4|*/ void* VadHint;
	/*<thisrel this+0x124>*/ /*|0x4|*/ void* CloneRoot;
	/*<thisrel this+0x128>*/ /*|0x4|*/ uint32_t NumberOfPrivatePages;
	/*<thisrel this+0x12c>*/ /*|0x4|*/ uint32_t NumberOfLockedPages;
	/*<thisrel this+0x130>*/ /*|0x4|*/ void* Win32Process;
	/*<thisrel this+0x134>*/ /*|0x4|*/ ejob* Job;
	/*<thisrel this+0x138>*/ /*|0x4|*/ void* SectionObject;
	/*<thisrel this+0x13c>*/ /*|0x4|*/ void* SectionBaseAddress;
	/*<thisrel this+0x140>*/ /*|0x4|*/ eprocess_quota_block* QuotaBlock;
	/*<thisrel this+0x144>*/ /*|0x4|*/ pagefault_history* WorkingSetWatch;
	/*<thisrel this+0x148>*/ /*|0x4|*/ void* Win32WindowStation;
	/*<thisrel this+0x14c>*/ /*|0x4|*/ void* InheritedFromUniqueProcessId;
	/*<thisrel this+0x150>*/ /*|0x4|*/ void* LdtInformation;
	/*<thisrel this+0x154>*/ /*|0x4|*/ void* VadFreeHint;
	/*<thisrel this+0x158>*/ /*|0x4|*/ void* VdmObjects;
	/*<thisrel this+0x15c>*/ /*|0x4|*/ void* DeviceMap;
	/*<thisrel this+0x160>*/ /*|0x8|*/ list_entry PhysicalVadList;
  union {
    /*<thisrel this+0x168>*/ /*|0x4|*/ hardware_pte PageDirectoryPte;
    uint64_t Filler;
  };
	/*<thisrel this+0x170>*/ /*|0x4|*/ void* Session;
	/*<thisrel this+0x174>*/ /*|0x10|*/ uint8_t ImageFileName[16];
	/*<thisrel this+0x184>*/ /*|0x8|*/ list_entry JobLinks;
	/*<thisrel this+0x18c>*/ /*|0x4|*/ void* LockedPagesList;
	/*<thisrel this+0x190>*/ /*|0x8|*/ list_entry ThreadListHead;
	/*<thisrel this+0x198>*/ /*|0x4|*/ void* SecurityPort;
	/*<thisrel this+0x19c>*/ /*|0x4|*/ void* PaeTop;
	/*<thisrel this+0x1a0>*/ /*|0x4|*/ uint32_t ActiveThreads;
	/*<thisrel this+0x1a4>*/ /*|0x4|*/ uint32_t GrantedAccess;
	/*<thisrel this+0x1a8>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
	/*<thisrel this+0x1ac>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
	/*<thisrel this+0x1b0>*/ /*|0x4|*/ nt::peb* Peb;
	/*<thisrel this+0x1b4>*/ /*|0x4|*/ ex_fast_ref PrefetchTrace;
	/*<thisrel this+0x1b8>*/ /*|0x8|*/ int64_t ReadOperationCount;
	/*<thisrel this+0x1c0>*/ /*|0x8|*/ int64_t WriteOperationCount;
	/*<thisrel this+0x1c8>*/ /*|0x8|*/ int64_t OtherOperationCount;
	/*<thisrel this+0x1d0>*/ /*|0x8|*/ int64_t ReadTransferCount;
	/*<thisrel this+0x1d8>*/ /*|0x8|*/ int64_t WriteTransferCount;
	/*<thisrel this+0x1e0>*/ /*|0x8|*/ int64_t OtherTransferCount;
	/*<thisrel this+0x1e8>*/ /*|0x4|*/ uint32_t CommitChargeLimit;
	/*<thisrel this+0x1ec>*/ /*|0x4|*/ uint32_t CommitChargePeak;
	/*<thisrel this+0x1f0>*/ /*|0x4|*/ void* AweInfo;
	/*<thisrel this+0x1f4>*/ /*|0x4|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
	/*<thisrel this+0x1f8>*/ /*|0x40|*/ mmsupport51 Vm;
	/*<thisrel this+0x238>*/ /*|0x4|*/ uint32_t LastFaultCount;
	/*<thisrel this+0x23c>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
	/*<thisrel this+0x240>*/ /*|0x4|*/ uint32_t NumberOfVads;
	/*<thisrel this+0x244>*/ /*|0x4|*/ uint32_t JobStatus;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t CreateReported:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmDeleted:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Outswapped:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ForkFailed:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t HasPhysicalVad:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SessionCreationUnderway:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t WriteWatch:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessInSession:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmTopDown:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Unused3:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Unused4:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Unused:5;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Unused1:1;
	/*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Unused2:1;
	/*<thisrel this+0x24c>*/ /*|0x4|*/ int32_t ExitStatus;
	/*<thisrel this+0x250>*/ /*|0x2|*/ uint16_t NextPageColor;
	/*<thisrel this+0x252>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
	/*<thisrel this+0x253>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
	/*<thisrel this+0x254>*/ /*|0x1|*/ uint8_t PriorityClass;
	/*<thisrel this+0x255>*/ /*|0x1|*/ uint8_t WorkingSetAcquiredUnsafe;
	/*<thisrel this+0x258>*/ /*|0x4|*/ uint32_t Cookie;
}; // <size 0x260>

struct eprocess52
{
#if 0
  /*<thisrel this+0x0>*/ /*|0x6c|*/ kprocess51 Pcb;
  /*<thisrel this+0x6c>*/ /*|0x4|*/ ex_push_lock ProcessLock;
  /*<thisrel this+0x70>*/ /*|0x8|*/ int64_t CreateTime;
  /*<thisrel this+0x78>*/ /*|0x8|*/ int64_t ExitTime;
  /*<thisrel this+0x80>*/ /*|0x4|*/ ex_rundown_ref RundownProtect;
  /*<thisrel this+0x84>*/ /*|0x4|*/ void* UniqueProcessId;
  /*<thisrel this+0x88>*/ /*|0x8|*/ list_entry ActiveProcessLinks;
  /*<thisrel this+0x90>*/ /*|0xc|*/ uint32_t QuotaUsage[3];
  /*<thisrel this+0x9c>*/ /*|0xc|*/ uint32_t QuotaPeak[3];
  /*<thisrel this+0xa8>*/ /*|0x4|*/ uint32_t CommitCharge;
  /*<thisrel this+0xac>*/ /*|0x4|*/ uint32_t PeakVirtualSize;
  /*<thisrel this+0xb0>*/ /*|0x4|*/ uint32_t VirtualSize;
  /*<thisrel this+0xb4>*/ /*|0x8|*/ list_entry SessionProcessLinks;
  /*<thisrel this+0xbc>*/ /*|0x4|*/ void* DebugPort;
  /*<thisrel this+0xc0>*/ /*|0x4|*/ void* ExceptionPort;
  /*<thisrel this+0xc4>*/ /*|0x4|*/ handle_table* ObjectTable;
  /*<thisrel this+0xc8>*/ /*|0x4|*/ ex_fast_ref Token;
  /*<thisrel this+0xcc>*/ /*|0x4|*/ uint32_t WorkingSetPage;
  /*<thisrel this+0xd0>*/ /*|0x20|*/ kguarded_mutex AddressCreationLock;
  /*<thisrel this+0xf0>*/ /*|0x4|*/ uint32_t HyperSpaceLock;
  /*<thisrel this+0xf4>*/ /*|0x4|*/ struct ethread* ForkInProgress;
  /*<thisrel this+0xf8>*/ /*|0x4|*/ uint32_t HardwareTrigger;
  /*<thisrel this+0xfc>*/ /*|0x4|*/ struct mm_avl_table* PhysicalVadRoot;
  /*<thisrel this+0x100>*/ /*|0x4|*/ void* CloneRoot;
  /*<thisrel this+0x104>*/ /*|0x4|*/ uint32_t NumberOfPrivatePages;
  /*<thisrel this+0x108>*/ /*|0x4|*/ uint32_t NumberOfLockedPages;
  /*<thisrel this+0x10c>*/ /*|0x4|*/ void* Win32Process;
  /*<thisrel this+0x110>*/ /*|0x4|*/ ejob* Job;
  /*<thisrel this+0x114>*/ /*|0x4|*/ void* SectionObject;
  /*<thisrel this+0x118>*/ /*|0x4|*/ void* SectionBaseAddress;
  /*<thisrel this+0x11c>*/ /*|0x4|*/ eprocess_quota_block* QuotaBlock;
  /*<thisrel this+0x120>*/ /*|0x4|*/ pagefault_history* WorkingSetWatch;
  /*<thisrel this+0x124>*/ /*|0x4|*/ void* Win32WindowStation;
  /*<thisrel this+0x128>*/ /*|0x4|*/ void* InheritedFromUniqueProcessId;
  /*<thisrel this+0x12c>*/ /*|0x4|*/ void* LdtInformation;
  /*<thisrel this+0x130>*/ /*|0x4|*/ void* VadFreeHint;
  /*<thisrel this+0x134>*/ /*|0x4|*/ void* VdmObjects;
  /*<thisrel this+0x138>*/ /*|0x4|*/ void* DeviceMap;
  /*<thisrel this+0x13c>*/ /*|0xc|*/ void* Spare0[3];
  /*<thisrel this+0x148>*/ /*|0x4|*/ hardware_pte PageDirectoryPte;
  /*<thisrel this+0x150>*/ /*|0x4|*/ void* Session;
  /*<thisrel this+0x154>*/ /*|0x10|*/ uint8_t ImageFileName[16];
  /*<thisrel this+0x164>*/ /*|0x8|*/ list_entry JobLinks;
  /*<thisrel this+0x16c>*/ /*|0x4|*/ void* LockedPagesList;
  /*<thisrel this+0x170>*/ /*|0x8|*/ list_entry ThreadListHead;
  /*<thisrel this+0x178>*/ /*|0x4|*/ void* SecurityPort;
  /*<thisrel this+0x17c>*/ /*|0x4|*/ void* PaeTop;
  /*<thisrel this+0x180>*/ /*|0x4|*/ uint32_t ActiveThreads;
  /*<thisrel this+0x184>*/ /*|0x4|*/ uint32_t GrantedAccess;
  /*<thisrel this+0x188>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
  /*<thisrel this+0x18c>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
  /*<thisrel this+0x190>*/ /*|0x4|*/ nt::peb* Peb;
  /*<thisrel this+0x194>*/ /*|0x4|*/ ex_fast_ref PrefetchTrace;
  /*<thisrel this+0x198>*/ /*|0x8|*/ int64_t ReadOperationCount;
  /*<thisrel this+0x1a0>*/ /*|0x8|*/ int64_t WriteOperationCount;
  /*<thisrel this+0x1a8>*/ /*|0x8|*/ int64_t OtherOperationCount;
  /*<thisrel this+0x1b0>*/ /*|0x8|*/ int64_t ReadTransferCount;
  /*<thisrel this+0x1b8>*/ /*|0x8|*/ int64_t WriteTransferCount;
  /*<thisrel this+0x1c0>*/ /*|0x8|*/ int64_t OtherTransferCount;
  /*<thisrel this+0x1c8>*/ /*|0x4|*/ uint32_t CommitChargeLimit;
  /*<thisrel this+0x1cc>*/ /*|0x4|*/ uint32_t CommitChargePeak;
  /*<thisrel this+0x1d0>*/ /*|0x4|*/ void* AweInfo;
  /*<thisrel this+0x1d4>*/ /*|0x4|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
  /*<thisrel this+0x1d8>*/ /*|0x60|*/ mmsupport52 Vm;
  /*<thisrel this+0x238>*/ /*|0x8|*/ list_entry MmProcessLinks;
  /*<thisrel this+0x240>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
  /*<thisrel this+0x244>*/ /*|0x4|*/ uint32_t JobStatus;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t CreateReported:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmDeleted:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Outswapped:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ForkFailed:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Wow64VaSpace4Gb:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SessionCreationUnderway:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t WriteWatch:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessInSession:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmTopDown:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ImageNotifyDone:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t PdeUpdateNeeded:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t SmapAllowed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t CreateFailed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t DefaultIoPriority:3;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t Spare1:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t Spare2:1;
  /*<thisrel this+0x24c>*/ /*|0x4|*/ int32_t ExitStatus;
  /*<thisrel this+0x250>*/ /*|0x2|*/ uint16_t NextPageColor;
  /*<thisrel this+0x252>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
  /*<thisrel this+0x253>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
  /*<thisrel this+0x254>*/ /*|0x1|*/ uint8_t PriorityClass;
  /*<thisrel this+0x258>*/ /*|0x20|*/ mm_avl_table VadRoot;
#endif
}; // <size 0x278>

struct eprocess521
{
  /*<thisrel this+0x0>*/ /*|0x6c|*/ kprocess52 Pcb;
  /*<thisrel this+0x6c>*/ /*|0x4|*/ ex_push_lock ProcessLock;
  /*<thisrel this+0x70>*/ /*|0x8|*/ int64_t CreateTime;
  /*<thisrel this+0x78>*/ /*|0x8|*/ int64_t ExitTime;
  /*<thisrel this+0x80>*/ /*|0x4|*/ ex_rundown_ref RundownProtect;
  /*<thisrel this+0x84>*/ /*|0x4|*/ void* UniqueProcessId;
  /*<thisrel this+0x88>*/ /*|0x8|*/ list_entry ActiveProcessLinks;
  /*<thisrel this+0x90>*/ /*|0xc|*/ uint32_t QuotaUsage[3];
  /*<thisrel this+0x9c>*/ /*|0xc|*/ uint32_t QuotaPeak[3];
  /*<thisrel this+0xa8>*/ /*|0x4|*/ uint32_t CommitCharge;
  /*<thisrel this+0xac>*/ /*|0x4|*/ uint32_t PeakVirtualSize;
  /*<thisrel this+0xb0>*/ /*|0x4|*/ uint32_t VirtualSize;
  /*<thisrel this+0xb4>*/ /*|0x8|*/ list_entry SessionProcessLinks;
  /*<thisrel this+0xbc>*/ /*|0x4|*/ void* DebugPort;
  /*<thisrel this+0xc0>*/ /*|0x4|*/ void* ExceptionPort;
  /*<thisrel this+0xc4>*/ /*|0x4|*/ handle_table* ObjectTable;
  /*<thisrel this+0xc8>*/ /*|0x4|*/ ex_fast_ref Token;
  /*<thisrel this+0xcc>*/ /*|0x4|*/ uint32_t WorkingSetPage;
  /*<thisrel this+0xd0>*/ /*|0x20|*/ kguarded_mutex AddressCreationLock;
  /*<thisrel this+0xf0>*/ /*|0x4|*/ uint32_t HyperSpaceLock;
  /*<thisrel this+0xf4>*/ /*|0x4|*/ struct ethread* ForkInProgress;
  /*<thisrel this+0xf8>*/ /*|0x4|*/ uint32_t HardwareTrigger;
  /*<thisrel this+0xfc>*/ /*|0x4|*/ struct mm_avl_table* PhysicalVadRoot;
  /*<thisrel this+0x100>*/ /*|0x4|*/ void* CloneRoot;
  /*<thisrel this+0x104>*/ /*|0x4|*/ uint32_t NumberOfPrivatePages;
  /*<thisrel this+0x108>*/ /*|0x4|*/ uint32_t NumberOfLockedPages;
  /*<thisrel this+0x10c>*/ /*|0x4|*/ void* Win32Process;
  /*<thisrel this+0x110>*/ /*|0x4|*/ ejob* Job;
  /*<thisrel this+0x114>*/ /*|0x4|*/ void* SectionObject;
  /*<thisrel this+0x118>*/ /*|0x4|*/ void* SectionBaseAddress;
  /*<thisrel this+0x11c>*/ /*|0x4|*/ eprocess_quota_block* QuotaBlock;
  /*<thisrel this+0x120>*/ /*|0x4|*/ pagefault_history* WorkingSetWatch;
  /*<thisrel this+0x124>*/ /*|0x4|*/ void* Win32WindowStation;
  /*<thisrel this+0x128>*/ /*|0x4|*/ void* InheritedFromUniqueProcessId;
  /*<thisrel this+0x12c>*/ /*|0x4|*/ void* LdtInformation;
  /*<thisrel this+0x130>*/ /*|0x4|*/ void* VadFreeHint;
  /*<thisrel this+0x134>*/ /*|0x4|*/ void* VdmObjects;
  /*<thisrel this+0x138>*/ /*|0x4|*/ void* DeviceMap;
  /*<thisrel this+0x13c>*/ /*|0xc|*/ void* Spare0[3];
  /*<thisrel this+0x148>*/ /*|0x4|*/ hardware_pte PageDirectoryPte;
  /*<thisrel this+0x150>*/ /*|0x4|*/ void* Session;
  /*<thisrel this+0x154>*/ /*|0x10|*/ uint8_t ImageFileName[16];
  /*<thisrel this+0x164>*/ /*|0x8|*/ list_entry JobLinks;
  /*<thisrel this+0x16c>*/ /*|0x4|*/ void* LockedPagesList;
  /*<thisrel this+0x170>*/ /*|0x8|*/ list_entry ThreadListHead;
  /*<thisrel this+0x178>*/ /*|0x4|*/ void* SecurityPort;
  /*<thisrel this+0x17c>*/ /*|0x4|*/ void* PaeTop;
  /*<thisrel this+0x180>*/ /*|0x4|*/ uint32_t ActiveThreads;
  /*<thisrel this+0x184>*/ /*|0x4|*/ uint32_t GrantedAccess;
  /*<thisrel this+0x188>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
  /*<thisrel this+0x18c>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
  /*<thisrel this+0x190>*/ /*|0x4|*/ nt::peb* Peb;
  /*<thisrel this+0x194>*/ /*|0x4|*/ ex_fast_ref PrefetchTrace;
  /*<thisrel this+0x198>*/ /*|0x8|*/ int64_t ReadOperationCount;
  /*<thisrel this+0x1a0>*/ /*|0x8|*/ int64_t WriteOperationCount;
  /*<thisrel this+0x1a8>*/ /*|0x8|*/ int64_t OtherOperationCount;
  /*<thisrel this+0x1b0>*/ /*|0x8|*/ int64_t ReadTransferCount;
  /*<thisrel this+0x1b8>*/ /*|0x8|*/ int64_t WriteTransferCount;
  /*<thisrel this+0x1c0>*/ /*|0x8|*/ int64_t OtherTransferCount;
  /*<thisrel this+0x1c8>*/ /*|0x4|*/ uint32_t CommitChargeLimit;
  /*<thisrel this+0x1cc>*/ /*|0x4|*/ uint32_t CommitChargePeak;
  /*<thisrel this+0x1d0>*/ /*|0x4|*/ void* AweInfo;
  /*<thisrel this+0x1d4>*/ /*|0x4|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
  /*<thisrel this+0x1d8>*/ /*|0x60|*/ mmsupport52 Vm;
  /*<thisrel this+0x238>*/ /*|0x8|*/ list_entry MmProcessLinks;
  /*<thisrel this+0x240>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
  /*<thisrel this+0x244>*/ /*|0x4|*/ uint32_t JobStatus;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t CreateReported:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmDeleted:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Outswapped:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ForkFailed:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t Wow64VaSpace4Gb:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t SessionCreationUnderway:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t WriteWatch:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ProcessInSession:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VmTopDown:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t ImageNotifyDone:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t PdeUpdateNeeded:1;
  /*<bitfield this+0x248>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t SmapAllowed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t CreateFailed:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t DefaultIoPriority:3;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t Spare1:1;
  /*<bitfield this+0x240>*/ /*|0x4|*/ uint32_t Spare2:1;
  /*<thisrel this+0x24c>*/ /*|0x4|*/ int32_t ExitStatus;
  /*<thisrel this+0x250>*/ /*|0x2|*/ uint16_t NextPageColor;
  /*<thisrel this+0x252>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
  /*<thisrel this+0x253>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
  /*<thisrel this+0x254>*/ /*|0x1|*/ uint8_t PriorityClass;
  /*<thisrel this+0x258>*/ /*|0x20|*/ mm_avl_table VadRoot;
}; // <size 0x278>

struct eprocess60
{
  /*<thisrel this+0x0>*/ /*|0x80|*/ kprocess61 Pcb;
  /*<thisrel this+0x80>*/ /*|0x4|*/ ex_push_lock ProcessLock;
  /*<thisrel this+0x88>*/ /*|0x8|*/ int64_t CreateTime;
  /*<thisrel this+0x90>*/ /*|0x8|*/ int64_t ExitTime;
  /*<thisrel this+0x98>*/ /*|0x4|*/ ex_rundown_ref RundownProtect;
  /*<thisrel this+0x9c>*/ /*|0x4|*/ void* UniqueProcessId;
  /*<thisrel this+0xa0>*/ /*|0x8|*/ list_entry ActiveProcessLinks;
  /*<thisrel this+0xa8>*/ /*|0xc|*/ uint32_t QuotaUsage[3];
  /*<thisrel this+0xb4>*/ /*|0xc|*/ uint32_t QuotaPeak[3];
  /*<thisrel this+0xc0>*/ /*|0x4|*/ uint32_t CommitCharge;
  /*<thisrel this+0xc4>*/ /*|0x4|*/ uint32_t PeakVirtualSize;
  /*<thisrel this+0xc8>*/ /*|0x4|*/ uint32_t VirtualSize;
  /*<thisrel this+0xcc>*/ /*|0x8|*/ list_entry SessionProcessLinks;
  /*<thisrel this+0xd4>*/ /*|0x4|*/ void* DebugPort;
  /*<thisrel this+0xd8>*/ /*|0x4|*/ void* ExceptionPortData;
  /*<bitfield this+0xd8>*/ /*|0x4|*/ uint32_t ExceptionPortState:3;
  /*<thisrel this+0xdc>*/ /*|0x4|*/ struct handle_table* ObjectTable;
  /*<thisrel this+0xe0>*/ /*|0x4|*/ ex_fast_ref Token;
  /*<thisrel this+0xe4>*/ /*|0x4|*/ uint32_t WorkingSetPage;
  /*<thisrel this+0xe8>*/ /*|0x4|*/ ex_push_lock AddressCreationLock;
  /*<thisrel this+0xec>*/ /*|0x4|*/ ethread* RotateInProgress;
  /*<thisrel this+0xf0>*/ /*|0x4|*/ ethread* ForkInProgress;
  /*<thisrel this+0xf4>*/ /*|0x4|*/ uint32_t HardwareTrigger;
  /*<thisrel this+0xf8>*/ /*|0x4|*/ mm_avl_table* PhysicalVadRoot;
  /*<thisrel this+0xfc>*/ /*|0x4|*/ void* CloneRoot;
  /*<thisrel this+0x100>*/ /*|0x4|*/ uint32_t NumberOfPrivatePages;
  /*<thisrel this+0x104>*/ /*|0x4|*/ uint32_t NumberOfLockedPages;
  /*<thisrel this+0x108>*/ /*|0x4|*/ void* Win32Process;
  /*<thisrel this+0x10c>*/ /*|0x4|*/ ejob* Job;
  /*<thisrel this+0x110>*/ /*|0x4|*/ void* SectionObject;
  /*<thisrel this+0x114>*/ /*|0x4|*/ void* SectionBaseAddress;
  /*<thisrel this+0x118>*/ /*|0x4|*/ eprocess_quota_block* QuotaBlock;
  /*<thisrel this+0x11c>*/ /*|0x4|*/ pagefault_history* WorkingSetWatch;
  /*<thisrel this+0x120>*/ /*|0x4|*/ void* Win32WindowStation;
  /*<thisrel this+0x124>*/ /*|0x4|*/ void* InheritedFromUniqueProcessId;
  /*<thisrel this+0x128>*/ /*|0x4|*/ void* LdtInformation;
  /*<thisrel this+0x12c>*/ /*|0x4|*/ void* Spare;
  /*<thisrel this+0x130>*/ /*|0x4|*/ void* VdmObjects;
  /*<thisrel this+0x134>*/ /*|0x4|*/ void* DeviceMap;
  /*<thisrel this+0x138>*/ /*|0x4|*/ void* EtwDataSource;
  /*<thisrel this+0x13c>*/ /*|0x4|*/ void* FreeTebHint;
  /*<thisrel this+0x140>*/ /*|0x4|*/ hardware_pte PageDirectoryPte;
  /*<thisrel this+0x148>*/ /*|0x4|*/ void* Session;
  /*<thisrel this+0x14c>*/ /*|0x10|*/ uint8_t ImageFileName[16];
  /*<thisrel this+0x15c>*/ /*|0x8|*/ list_entry JobLinks;
  /*<thisrel this+0x164>*/ /*|0x4|*/ void* LockedPagesList;
  /*<thisrel this+0x168>*/ /*|0x8|*/ list_entry ThreadListHead;
  /*<thisrel this+0x170>*/ /*|0x4|*/ void* SecurityPort;
  /*<thisrel this+0x174>*/ /*|0x4|*/ void* PaeTop;
  /*<thisrel this+0x178>*/ /*|0x4|*/ uint32_t ActiveThreads;
  /*<thisrel this+0x17c>*/ /*|0x4|*/ uint32_t ImagePathHash;
  /*<thisrel this+0x180>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
  /*<thisrel this+0x184>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
  /*<thisrel this+0x188>*/ /*|0x4|*/ nt::peb* Peb;
  /*<thisrel this+0x18c>*/ /*|0x4|*/ ex_fast_ref PrefetchTrace;
  /*<thisrel this+0x190>*/ /*|0x8|*/ int64_t ReadOperationCount;
  /*<thisrel this+0x198>*/ /*|0x8|*/ int64_t WriteOperationCount;
  /*<thisrel this+0x1a0>*/ /*|0x8|*/ int64_t OtherOperationCount;
  /*<thisrel this+0x1a8>*/ /*|0x8|*/ int64_t ReadTransferCount;
  /*<thisrel this+0x1b0>*/ /*|0x8|*/ int64_t WriteTransferCount;
  /*<thisrel this+0x1b8>*/ /*|0x8|*/ int64_t OtherTransferCount;
  /*<thisrel this+0x1c0>*/ /*|0x4|*/ uint32_t CommitChargeLimit;
  /*<thisrel this+0x1c4>*/ /*|0x4|*/ uint32_t CommitChargePeak;
  /*<thisrel this+0x1c8>*/ /*|0x4|*/ void* AweInfo;
  /*<thisrel this+0x1cc>*/ /*|0x4|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
  /*<thisrel this+0x1d0>*/ /*|0x48|*/ mmsupport60 Vm;
  /*<thisrel this+0x218>*/ /*|0x8|*/ list_entry MmProcessLinks;
  /*<thisrel this+0x220>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t JobNotReallyActive:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t AccountingFolded:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t NewProcessReported:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t ExitProcessReported:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t ReportCommitChanges:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t LastReportMemory:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t ReportPhysicalPageChanges:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t HandleTableRundown:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t NeedsHandleRundown:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t RefTraceEnabled:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t NumaAware:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t ProtectedProcess:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t DefaultPagePriority:3;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t PrimaryTokenFrozen:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t ProcessVerifierTarget:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t StackRandomizationDisabled:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t AffinityPermanent:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t AffinityUpdateEnable:1;
  /*<bitfield this+0x224>*/ /*|0x4|*/ uint32_t CrossSessionCreate:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t CreateReported:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t VmDeleted:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t Outswapped:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ForkFailed:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t Wow64VaSpace4Gb:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t DeprioritizeViews:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t WriteWatch:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ProcessInSession:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t VmTopDown:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ImageNotifyDone:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t PdeUpdateNeeded:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t SmapAllowed:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ProcessInserted:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t DefaultIoPriority:3;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t ProcessSelfDelete:1;
  /*<bitfield this+0x228>*/ /*|0x4|*/ uint32_t SpareProcessFlags:1;
  /*<thisrel this+0x22c>*/ /*|0x4|*/ int32_t ExitStatus;
  /*<thisrel this+0x230>*/ /*|0x2|*/ uint16_t Spare7;
  /*<thisrel this+0x232>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
  /*<thisrel this+0x233>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
  /*<thisrel this+0x234>*/ /*|0x1|*/ uint8_t PriorityClass;
  /*<thisrel this+0x238>*/ /*|0x20|*/ mm_avl_table VadRoot;
  /*<thisrel this+0x258>*/ /*|0x4|*/ uint32_t Cookie;
  /*<thisrel this+0x25c>*/ /*|0x10|*/ alpc_process_context AlpcContext;
}; // <size 0x270>

struct eprocess61: eprocess60 {};

#elif defined(_M_X64)

// 64bit structures
struct kprocess52;  // xp64/2k3
struct kprocess60;  // vista
struct kprocess61;  // 2k8

struct eprocess52;  // xp64/2k3
struct eprocess60;  // vista
struct eprocess61;  // 2k8

struct wow64_process {
  void* Wow64;
};

struct kprocess52 {
  // non-static data --------------------------------
  /*<thisrel this+0x0>*/ /*|0x18|*/ dispatcher_header Header;
  /*<thisrel this+0x18>*/ /*|0x10|*/ list_entry ProfileListHead;
  /*<thisrel this+0x28>*/ /*|0x10|*/ uint64_t DirectoryTableBase[2];
  /*<thisrel this+0x38>*/ /*|0x2|*/ uint16_t IopmOffset;
  /*<thisrel this+0x40>*/ /*|0x8|*/ uint64_t ActiveProcessors;
  /*<thisrel this+0x48>*/ /*|0x4|*/ uint32_t KernelTime;
  /*<thisrel this+0x4c>*/ /*|0x4|*/ uint32_t UserTime;
  /*<thisrel this+0x50>*/ /*|0x10|*/ list_entry ReadyListHead;
  /*<thisrel this+0x60>*/ /*|0x8|*/ single_list_entry SwapListEntry;
  /*<thisrel this+0x68>*/ /*|0x8|*/ void* Reserved1;
  /*<thisrel this+0x70>*/ /*|0x10|*/ list_entry ThreadListHead;
  /*<thisrel this+0x80>*/ /*|0x8|*/ uint64_t ProcessLock;
  /*<thisrel this+0x88>*/ /*|0x8|*/ uint64_t Affinity;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t AutoAlignment:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t DisableBoost:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t DisableQuantum:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t ReservedFlags:29;
  /*<thisrel this+0x94>*/ /*|0x1|*/ int8_t BasePriority;
  /*<thisrel this+0x95>*/ /*|0x1|*/ int8_t QuantumReset;
  /*<thisrel this+0x96>*/ /*|0x1|*/ uint8_t State;
  /*<thisrel this+0x97>*/ /*|0x1|*/ uint8_t ThreadSeed;
  /*<thisrel this+0x98>*/ /*|0x1|*/ uint8_t PowerState;
  /*<thisrel this+0x99>*/ /*|0x1|*/ uint8_t IdealNode;
  /*<thisrel this+0x9a>*/ /*|0x1|*/ uint8_t Visited;
  /*<thisrel this+0x9b>*/ /*|0x1|*/ kexecute_options Flags;
  /*<thisrel this+0x9b>*/ /*|0x1|*/ uint8_t ExecuteOptions;
  /*<thisrel this+0xa0>*/ /*|0x8|*/ uint64_t StackCount;
  /*<thisrel this+0xa8>*/ /*|0x10|*/ list_entry ProcessListEntry;
}; // <size 0xb8>

struct kprocess60
{
  /*<thisrel this+0x0>*/ /*|0x18|*/ dispatcher_header Header;
  /*<thisrel this+0x18>*/ /*|0x10|*/ list_entry ProfileListHead;
  /*<thisrel this+0x28>*/ /*|0x8|*/ uint64_t DirectoryTableBase;
  /*<thisrel this+0x30>*/ /*|0x8|*/ uint64_t Unused0;
  /*<thisrel this+0x38>*/ /*|0x2|*/ uint16_t IopmOffset;
  /*<thisrel this+0x40>*/ /*|0x8|*/ uint64_t ActiveProcessors;
  /*<thisrel this+0x48>*/ /*|0x4|*/ uint32_t KernelTime;
  /*<thisrel this+0x4c>*/ /*|0x4|*/ uint32_t UserTime;
  /*<thisrel this+0x50>*/ /*|0x10|*/ list_entry ReadyListHead;
  /*<thisrel this+0x60>*/ /*|0x8|*/ single_list_entry SwapListEntry;
  /*<thisrel this+0x68>*/ /*|0x8|*/ void* InstrumentationCallback;
  /*<thisrel this+0x70>*/ /*|0x10|*/ list_entry ThreadListHead;
  /*<thisrel this+0x80>*/ /*|0x8|*/ uint64_t ProcessLock;
  /*<thisrel this+0x88>*/ /*|0x8|*/ uint64_t Affinity;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t AutoAlignment:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t DisableBoost:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t DisableQuantum:1;
  /*<bitfield this+0x90>*/ /*|0x4|*/ int32_t ReservedFlags:0x1D;
  /*<thisrel this+0x94>*/ /*|0x1|*/ int8_t BasePriority;
  /*<thisrel this+0x95>*/ /*|0x1|*/ int8_t QuantumReset;
  /*<thisrel this+0x96>*/ /*|0x1|*/ uint8_t State;
  /*<thisrel this+0x97>*/ /*|0x1|*/ uint8_t ThreadSeed;
  /*<thisrel this+0x98>*/ /*|0x1|*/ uint8_t PowerState;
  /*<thisrel this+0x99>*/ /*|0x1|*/ uint8_t IdealNode;
  /*<thisrel this+0x9a>*/ /*|0x1|*/ uint8_t Visited;
  /*<thisrel this+0x9b>*/ /*|0x1|*/ kexecute_options Flags;
  /*<thisrel this+0xa0>*/ /*|0x8|*/ uint64_t StackCount;
  /*<thisrel this+0xa8>*/ /*|0x10|*/ list_entry ProcessListEntry;
  /*<thisrel this+0xb8>*/ /*|0x8|*/ uint64_t CycleTime;
}; // <size 0xc0>

struct kprocess61: kprocess60 {};


struct eprocess52 {
  /*<thisrel this+0x0>*/ /*|0xb8|*/ kprocess52 Pcb;
  /*<thisrel this+0xb8>*/ /*|0x8|*/ ex_push_lock ProcessLock;
  /*<thisrel this+0xc0>*/ /*|0x8|*/ int64_t CreateTime;
  /*<thisrel this+0xc8>*/ /*|0x8|*/ int64_t ExitTime;
  /*<thisrel this+0xd0>*/ /*|0x8|*/ ex_rundown_ref RundownProtect;
  /*<thisrel this+0xd8>*/ /*|0x8|*/ legacy_handle UniqueProcessId;
  /*<thisrel this+0xe0>*/ /*|0x10|*/ list_entry ActiveProcessLinks;
  /*<thisrel this+0xf0>*/ /*|0x18|*/ uint64_t QuotaUsage[3];
  /*<thisrel this+0x108>*/ /*|0x18|*/ uint64_t QuotaPeak[3];
  /*<thisrel this+0x120>*/ /*|0x8|*/ uint64_t CommitCharge;
  /*<thisrel this+0x128>*/ /*|0x8|*/ uint64_t PeakVirtualSize;
  /*<thisrel this+0x130>*/ /*|0x8|*/ uint64_t VirtualSize;
  /*<thisrel this+0x138>*/ /*|0x10|*/ list_entry SessionProcessLinks;
  /*<thisrel this+0x148>*/ /*|0x8|*/ legacy_handle DebugPort;
  /*<thisrel this+0x150>*/ /*|0x8|*/ legacy_handle ExceptionPort;
  /*<thisrel this+0x158>*/ /*|0x8|*/ handle_table* ObjectTable;
  /*<thisrel this+0x160>*/ /*|0x8|*/ ex_fast_ref Token;
  /*<thisrel this+0x168>*/ /*|0x8|*/ uint64_t WorkingSetPage;
  /*<thisrel this+0x170>*/ /*|0x38|*/ kguarded_mutex AddressCreationLock;
  /*<thisrel this+0x1a8>*/ /*|0x8|*/ uint64_t HyperSpaceLock;
  /*<thisrel this+0x1b0>*/ /*|0x8|*/ ethread* ForkInProgress;
  /*<thisrel this+0x1b8>*/ /*|0x8|*/ uint64_t HardwareTrigger;
  /*<thisrel this+0x1c0>*/ /*|0x8|*/ mm_avl_table* PhysicalVadRoot;
  /*<thisrel this+0x1c8>*/ /*|0x8|*/ void* CloneRoot;
  /*<thisrel this+0x1d0>*/ /*|0x8|*/ uint64_t NumberOfPrivatePages;
  /*<thisrel this+0x1d8>*/ /*|0x8|*/ uint64_t NumberOfLockedPages;
  /*<thisrel this+0x1e0>*/ /*|0x8|*/ void* Win32Process;
  /*<thisrel this+0x1e8>*/ /*|0x8|*/ ejob* Job;
  /*<thisrel this+0x1f0>*/ /*|0x8|*/ legacy_handle SectionObject;
  /*<thisrel this+0x1f8>*/ /*|0x8|*/ void* SectionBaseAddress;
  /*<thisrel this+0x200>*/ /*|0x8|*/ eprocess_quota_block* QuotaBlock;
  /*<thisrel this+0x208>*/ /*|0x8|*/ pagefault_history* WorkingSetWatch;
  /*<thisrel this+0x210>*/ /*|0x8|*/ legacy_handle Win32WindowStation;
  /*<thisrel this+0x218>*/ /*|0x8|*/ legacy_handle InheritedFromUniqueProcessId;
  /*<thisrel this+0x220>*/ /*|0x8|*/ void* LdtInformation;
  /*<thisrel this+0x228>*/ /*|0x8|*/ void* VadFreeHint;
  /*<thisrel this+0x230>*/ /*|0x8|*/ void* VdmObjects;
  /*<thisrel this+0x238>*/ /*|0x8|*/ void* DeviceMap;
  /*<thisrel this+0x240>*/ /*|0x18|*/ void* Spare0[3];
  /*<thisrel this+0x258>*/ /*|0x8|*/ hardware_pte PageDirectoryPte;
  /*<thisrel this+0x260>*/ /*|0x8|*/ void* Session;
  /*<thisrel this+0x268>*/ /*|0x10|*/ uint8_t ImageFileName[16];
  /*<thisrel this+0x278>*/ /*|0x10|*/ list_entry JobLinks;
  /*<thisrel this+0x288>*/ /*|0x8|*/ void* LockedPagesList;
  /*<thisrel this+0x290>*/ /*|0x10|*/ list_entry ThreadListHead;
  /*<thisrel this+0x2a0>*/ /*|0x8|*/ void* SecurityPort;
  /*<thisrel this+0x2a8>*/ /*|0x8|*/ struct wow64_process* Wow64Process;
  /*<thisrel this+0x2b0>*/ /*|0x4|*/ uint32_t ActiveThreads;
  /*<thisrel this+0x2b4>*/ /*|0x4|*/ uint32_t GrantedAccess;
  /*<thisrel this+0x2b8>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
  /*<thisrel this+0x2bc>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
  /*<thisrel this+0x2c0>*/ /*|0x8|*/ struct nt::peb* Peb;
  /*<thisrel this+0x2c8>*/ /*|0x8|*/ ex_fast_ref PrefetchTrace;
  /*<thisrel this+0x2d0>*/ /*|0x8|*/ int64_t ReadOperationCount;
  /*<thisrel this+0x2d8>*/ /*|0x8|*/ int64_t WriteOperationCount;
  /*<thisrel this+0x2e0>*/ /*|0x8|*/ int64_t OtherOperationCount;
  /*<thisrel this+0x2e8>*/ /*|0x8|*/ int64_t ReadTransferCount;
  /*<thisrel this+0x2f0>*/ /*|0x8|*/ int64_t WriteTransferCount;
  /*<thisrel this+0x2f8>*/ /*|0x8|*/ int64_t OtherTransferCount;
  /*<thisrel this+0x300>*/ /*|0x8|*/ uint64_t CommitChargeLimit;
  /*<thisrel this+0x308>*/ /*|0x8|*/ uint64_t CommitChargePeak;
  /*<thisrel this+0x310>*/ /*|0x8|*/ void* AweInfo;
  /*<thisrel this+0x318>*/ /*|0x8|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
  /*<thisrel this+0x320>*/ /*|0x58|*/ mmsupport52 Vm;
  /*<thisrel this+0x378>*/ /*|0x8|*/ uint32_t Spares[2];
  /*<thisrel this+0x380>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
  /*<thisrel this+0x384>*/ /*|0x4|*/ uint32_t JobStatus;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t CreateReported:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t VmDeleted:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t Outswapped:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t ForkFailed:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t Wow64VaSpace4Gb:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t SessionCreationUnderway:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t WriteWatch:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t ProcessInSession0:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t VmTopDown:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t ImageNotifyDone:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t PdeUpdateNeeded:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t SmapAllowed:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t CreateFailed:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t DefaultIoPriority:3;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t Spare1:1;
  /*<bitfield this+0x388>*/ /*|0x4|*/ uint32_t Spare2:1;
  /*<thisrel this+0x38c>*/ /*|0x4|*/ int32_t ExitStatus;
  /*<thisrel this+0x390>*/ /*|0x2|*/ uint16_t NextPageColor;
  /*<thisrel this+0x392>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
  /*<thisrel this+0x393>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
  /*<thisrel this+0x394>*/ /*|0x1|*/ uint8_t PriorityClass;
  /*<thisrel this+0x398>*/ /*|0x40|*/ mm_avl_table VadRoot;
  /*<thisrel this+0x3d8>*/ /*|0x4|*/ uint32_t Cookie;
};
// <size 0x3e0>

struct eprocess60
{
  /*<thisrel this+0x0>*/ /*|0xc0|*/ kprocess61 Pcb;
  /*<thisrel this+0xc0>*/ /*|0x8|*/ ex_push_lock ProcessLock;
  /*<thisrel this+0xc8>*/ /*|0x8|*/ int64_t CreateTime;
  /*<thisrel this+0xd0>*/ /*|0x8|*/ int64_t ExitTime;
  /*<thisrel this+0xd8>*/ /*|0x8|*/ ex_rundown_ref RundownProtect;
  /*<thisrel this+0xe0>*/ /*|0x8|*/ void* UniqueProcessId;
  /*<thisrel this+0xe8>*/ /*|0x10|*/ list_entry ActiveProcessLinks;
  /*<thisrel this+0xf8>*/ /*|0x18|*/ uint64_t QuotaUsage[3];
  /*<thisrel this+0x110>*/ /*|0x18|*/ uint64_t QuotaPeak[3];
  /*<thisrel this+0x128>*/ /*|0x8|*/ uint64_t CommitCharge;
  /*<thisrel this+0x130>*/ /*|0x8|*/ uint64_t PeakVirtualSize;
  /*<thisrel this+0x138>*/ /*|0x8|*/ uint64_t VirtualSize;
  /*<thisrel this+0x140>*/ /*|0x10|*/ list_entry SessionProcessLinks;
  /*<thisrel this+0x150>*/ /*|0x8|*/ void* DebugPort;
  union {
    union {
    /*<thisrel this+0x158>*/ /*|0x8|*/ void* ExceptionPortData;
    /*<thisrel this+0x158>*/ /*|0x8|*/ uint64_t ExceptionPortValue;
    } u;
    /*<bitfield this+0x158>*/ /*|0x8|*/ uint64_t ExceptionPortState:3;
  } ExceptionPort;
  /*<thisrel this+0x160>*/ /*|0x8|*/ struct handle_table* ObjectTable;
  /*<thisrel this+0x168>*/ /*|0x8|*/ ex_fast_ref Token;
  /*<thisrel this+0x170>*/ /*|0x8|*/ uint64_t WorkingSetPage;
  /*<thisrel this+0x178>*/ /*|0x8|*/ ex_push_lock AddressCreationLock;
  /*<thisrel this+0x180>*/ /*|0x8|*/ ethread* RotateInProgress;
  /*<thisrel this+0x188>*/ /*|0x8|*/ ethread* ForkInProgress;
  /*<thisrel this+0x190>*/ /*|0x8|*/ uint64_t HardwareTrigger;
  /*<thisrel this+0x198>*/ /*|0x8|*/ mm_avl_table* PhysicalVadRoot;
  /*<thisrel this+0x1a0>*/ /*|0x8|*/ void* CloneRoot;
  /*<thisrel this+0x1a8>*/ /*|0x8|*/ uint64_t NumberOfPrivatePages;
  /*<thisrel this+0x1b0>*/ /*|0x8|*/ uint64_t NumberOfLockedPages;
  /*<thisrel this+0x1b8>*/ /*|0x8|*/ void* Win32Process;
  /*<thisrel this+0x1c0>*/ /*|0x8|*/ ejob* Job;
  /*<thisrel this+0x1c8>*/ /*|0x8|*/ void* SectionObject;
  /*<thisrel this+0x1d0>*/ /*|0x8|*/ void* SectionBaseAddress;
  /*<thisrel this+0x1d8>*/ /*|0x8|*/ eprocess_quota_block* QuotaBlock;
  /*<thisrel this+0x1e0>*/ /*|0x8|*/ pagefault_history* WorkingSetWatch;
  /*<thisrel this+0x1e8>*/ /*|0x8|*/ void* Win32WindowStation;
  /*<thisrel this+0x1f0>*/ /*|0x8|*/ void* InheritedFromUniqueProcessId;
  /*<thisrel this+0x1f8>*/ /*|0x8|*/ void* LdtInformation;
  /*<thisrel this+0x200>*/ /*|0x8|*/ void* VadFreeHint;
  /*<thisrel this+0x208>*/ /*|0x8|*/ void* VdmObjects;
  /*<thisrel this+0x210>*/ /*|0x8|*/ void* DeviceMap;
  /*<thisrel this+0x218>*/ /*|0x8|*/ void* EtwDataSource;
  /*<thisrel this+0x220>*/ /*|0x8|*/ void* FreeTebHint;
  /*<thisrel this+0x228>*/ /*|0x8|*/ hardware_pte PageDirectoryPte;
  /*<thisrel this+0x230>*/ /*|0x8|*/ void* Session;
  /*<thisrel this+0x238>*/ /*|0x10|*/ uint8_t ImageFileName[16];
  /*<thisrel this+0x248>*/ /*|0x10|*/ list_entry JobLinks;
  /*<thisrel this+0x258>*/ /*|0x8|*/ void* LockedPagesList;
  /*<thisrel this+0x260>*/ /*|0x10|*/ list_entry ThreadListHead;
  /*<thisrel this+0x270>*/ /*|0x8|*/ void* SecurityPort;
  /*<thisrel this+0x278>*/ /*|0x8|*/ wow64_process* Wow64Process;
  /*<thisrel this+0x280>*/ /*|0x4|*/ uint32_t ActiveThreads;
  /*<thisrel this+0x284>*/ /*|0x4|*/ uint32_t ImagePathHash;
  /*<thisrel this+0x288>*/ /*|0x4|*/ uint32_t DefaultHardErrorProcessing;
  /*<thisrel this+0x28c>*/ /*|0x4|*/ int32_t LastThreadExitStatus;
  /*<thisrel this+0x290>*/ /*|0x8|*/ nt::peb* Peb;
  /*<thisrel this+0x298>*/ /*|0x8|*/ ex_fast_ref PrefetchTrace;
  /*<thisrel this+0x2a0>*/ /*|0x8|*/ int64_t ReadOperationCount;
  /*<thisrel this+0x2a8>*/ /*|0x8|*/ int64_t WriteOperationCount;
  /*<thisrel this+0x2b0>*/ /*|0x8|*/ int64_t OtherOperationCount;
  /*<thisrel this+0x2b8>*/ /*|0x8|*/ int64_t ReadTransferCount;
  /*<thisrel this+0x2c0>*/ /*|0x8|*/ int64_t WriteTransferCount;
  /*<thisrel this+0x2c8>*/ /*|0x8|*/ int64_t OtherTransferCount;
  /*<thisrel this+0x2d0>*/ /*|0x8|*/ uint64_t CommitChargeLimit;
  /*<thisrel this+0x2d8>*/ /*|0x8|*/ uint64_t CommitChargePeak;
  /*<thisrel this+0x2e0>*/ /*|0x8|*/ void* AweInfo;
  /*<thisrel this+0x2e8>*/ /*|0x8|*/ se_audit_process_creation_info SeAuditProcessCreationInfo;
  /*<thisrel this+0x2f0>*/ /*|0x68|*/ mmsupport60 Vm;
  /*<thisrel this+0x358>*/ /*|0x10|*/ list_entry MmProcessLinks;
  /*<thisrel this+0x368>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t JobNotReallyActive:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t AccountingFolded:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t NewProcessReported:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t ExitProcessReported:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t ReportCommitChanges:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t LastReportMemory:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t ReportPhysicalPageChanges:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t HandleTableRundown:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t NeedsHandleRundown:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t RefTraceEnabled:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t NumaAware:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t ProtectedProcess:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t DefaultPagePriority:3;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t PrimaryTokenFrozen:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t ProcessVerifierTarget:1;
  /*<bitfield this+0x36c>*/ /*|0x4|*/ uint32_t StackRandomizationDisabled:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t CreateReported:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t NoDebugInherit:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ProcessExiting:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ProcessDelete:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t Wow64SplitPages:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t VmDeleted:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t OutswapEnabled:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t Outswapped:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ForkFailed:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t Wow64VaSpace4Gb:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t AddressSpaceInitialized:2;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t SetTimerResolution:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t BreakOnTermination:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t DeprioritizeViews:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t WriteWatch:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ProcessInSession:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t OverrideAddressSpace:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t HasAddressSpace:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t LaunchPrefetched:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t InjectInpageErrors:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t VmTopDown:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ImageNotifyDone:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t PdeUpdateNeeded:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t VdmAllowed:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t SmapAllowed:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t ProcessInserted:1;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t DefaultIoPriority:3;
  /*<bitfield this+0x370>*/ /*|0x4|*/ uint32_t SparePsFlags1:2;
  /*<thisrel this+0x374>*/ /*|0x4|*/ int32_t ExitStatus;
  /*<thisrel this+0x378>*/ /*|0x2|*/ uint16_t Spare7;
  /*<thisrel this+0x37a>*/ /*|0x1|*/ uint8_t SubSystemMinorVersion;
  /*<thisrel this+0x37b>*/ /*|0x1|*/ uint8_t SubSystemMajorVersion;
  /*<thisrel this+0x37c>*/ /*|0x1|*/ uint8_t PriorityClass;
  /*<thisrel this+0x380>*/ /*|0x40|*/ mm_avl_table VadRoot;
  /*<thisrel this+0x3c0>*/ /*|0x4|*/ uint32_t Cookie;
  /*<thisrel this+0x3c8>*/ /*|0x20|*/ alpc_process_context AlpcContext;
}; // <size 0x3e8>

struct eprocess61: eprocess60 {};

#endif




///\note XP+ only, use IoGetCurrentProcess instead
NTL__EXTERNAPI
kprocess * __stdcall
  PsGetCurrentProcess();

NTL__EXTERNAPI
legacy_handle __stdcall
  PsGetCurrentProcessId();

///\note XP+ only
NTL__EXTERNAPI
legacy_handle __stdcall
  PsGetCurrentProcessSessionId();


///\note XP+ only
NTL__EXTERNAPI
int8_t * __stdcall
  PsGetProcessImageFileName(kprocess *);


NTL__EXTERNAPI
ntstatus __stdcall
  PsLookupProcessByProcessId(
    legacy_handle ProcessId,
    kprocess **   Process
    );


static inline
kprocess * lookup_process(legacy_handle process_id)
{
  kprocess * process;
  return nt::success(PsLookupProcessByProcessId(process_id, &process))
    ? process : 0;
}


static __forceinline
void dereference_object(kprocess * process)
{
  ObfDereferenceObject(process);
}



NTL__EXTERNAPI
void __stdcall
  KeStackAttachProcess(
    kprocess *    Process,
    kapc_state *  ApcState
    );

NTL__EXTERNAPI
void __stdcall
  KeUnstackDetachProcess(
    kapc_state *  ApcState
    );



typedef
nt::peb * __stdcall
  get_process_peb_t(kprocess const* Process);

// XP+ only
NTL__EXTERNAPI
get_process_peb_t PsGetProcessPeb;




typedef	void
(__stdcall* PCreateProcessNotifyRoutine)(
  legacy_handle ParentId,
  legacy_handle ProcessId,
  bool Create
  );

typedef void
(__stdcall* PCreateThreadNotifyRoutine)(
                                        legacy_handle ProcessId,
                                        legacy_handle ThreadId,
                                        bool Create
                                        );


NTL__EXTERNAPI
ntstatus __stdcall
PsSetCreateProcessNotifyRoutine(
                                PCreateProcessNotifyRoutine NotifyRoutine,
                                bool Remove
                                );

NTL__EXTERNAPI
ntstatus __stdcall
PsSetCreateThreadNotifyRoutine(
                               PCreateThreadNotifyRoutine NotifyRoutine
                               );

#if (NTDDI_VERSION >= NTDDI_WINXP)
NTL__EXTERNAPI
ntstatus __stdcall
PsRemoveCreateThreadNotifyRoutine (
                                   PCreateThreadNotifyRoutine NotifyRoutine
                                   );
#endif


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_PROCESS
