/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
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

struct ex_rundown_ref
{
	union
	{
		/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Count;
		/*<thisrel this+0x0>*/ /*|0x4|*/ void* Ptr;
	};
};

struct ex_fast_ref
{
	union {
	/*<thisrel this+0x0>*/ /*|0x4|*/ void* Object;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t RefCnt:3;
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Value;
	};
};// <size 0x4>

struct ex_push_lock
{
	union {
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Waiting:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Exclusive:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Shared:0x1E;
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Value;
	/*<thisrel this+0x0>*/ /*|0x4|*/ void* Ptr;
	};
};// <size 0x4>

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

struct hardware_pte
{
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Valid:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Write:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Owner:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t WriteThrough:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t CacheDisable:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Accessed:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Dirty:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t LargePage:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Global:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t CopyOnWrite:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Prototype:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t reserved:1;
	/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t PageFrameNumber:0x14;
};
// <size 0x4>

struct mmwsle
{
	union {
		/*<thisrel this+0x0>*/ /*|0x4|*/ void* VirtualAddress;
		/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Long;
		/*<thisrel this+0x0>*/ /*|0x4|*/ struct mmwslentry
		{
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Valid:1;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t LockedInWs:1;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t LockedInMemory:1;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Protection:5;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Hashed:1;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Direct:1;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Age:2;
			/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t VirtualPageNumber:0x14;
		}e1;
	};
};

struct mmwsle_hash
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ void* Key;
	/*<thisrel this+0x4>*/ /*|0x4|*/ uint32_t Index;
};

struct mmwsl
{
	/*<thisrel this+0x0>*/ /*|0x4|*/ uint32_t Quota;
	/*<thisrel this+0x4>*/ /*|0x4|*/ uint32_t FirstFree;
	/*<thisrel this+0x8>*/ /*|0x4|*/ uint32_t FirstDynamic;
	/*<thisrel this+0xc>*/ /*|0x4|*/ uint32_t LastEntry;
	/*<thisrel this+0x10>*/ /*|0x4|*/ uint32_t NextSlot;
	/*<thisrel this+0x14>*/ /*|0x4|*/ mmwsle* Wsle;
	/*<thisrel this+0x18>*/ /*|0x4|*/ uint32_t LastInitializedWsle;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ uint32_t NonDirectCount;
	/*<thisrel this+0x20>*/ /*|0x4|*/ mmwsle_hash* HashTable;
	/*<thisrel this+0x24>*/ /*|0x4|*/ uint32_t HashTableSize;
	/*<thisrel this+0x28>*/ /*|0x4|*/ uint32_t NumberOfCommittedPageTables;
	/*<thisrel this+0x2c>*/ /*|0x4|*/ void* HashTableStart;
	/*<thisrel this+0x30>*/ /*|0x4|*/ void* HighestPermittedHashAddress;
	/*<thisrel this+0x34>*/ /*|0x4|*/ uint32_t NumberOfImageWaiters;
	/*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t VadBitMapHint;
	/*<thisrel this+0x3c>*/ /*|0x600|*/ uint16_t UsedPageTableEntries[768];
	/*<thisrel this+0x63c>*/ /*|0x60|*/ uint32_t CommittedPageTables[24];
};

struct mmsupport
{
	/*<thisrel this+0x0>*/ /*|0x8|*/ int64_t LastTrimTime;
	/*<thisrel this+0x8>*/ /*|0x4|*/ struct mmsupport_flags
	{
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t SessionSpace:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t BeingTrimmed:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t SessionLeader:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t TrimHard:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t WorkingSetHard:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t AddressSpaceBeingDeleted:1;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t Available:0x0A;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t AllowWorkingSetAdjustment:8;
		/*<bitfield this+0x0>*/ /*|0x4|*/ uint32_t MemoryPriority:8;
	}Flags;
	/*<thisrel this+0xc>*/ /*|0x4|*/ uint32_t PageFaultCount;
	/*<thisrel this+0x10>*/ /*|0x4|*/ uint32_t PeakWorkingSetSize;
	/*<thisrel this+0x14>*/ /*|0x4|*/ uint32_t WorkingSetSize;
	/*<thisrel this+0x18>*/ /*|0x4|*/ uint32_t MinimumWorkingSetSize;
	/*<thisrel this+0x1c>*/ /*|0x4|*/ uint32_t MaximumWorkingSetSize;
	/*<thisrel this+0x20>*/ /*|0x4|*/ mmwsl* VmWorkingSetList;
	/*<thisrel this+0x24>*/ /*|0x8|*/ list_entry WorkingSetExpansionLinks;
	/*<thisrel this+0x2c>*/ /*|0x4|*/ uint32_t Claim;
	/*<thisrel this+0x30>*/ /*|0x4|*/ uint32_t NextEstimationSlot;
	/*<thisrel this+0x34>*/ /*|0x4|*/ uint32_t NextAgingSlot;
	/*<thisrel this+0x38>*/ /*|0x4|*/ uint32_t EstimatedAvailable;
	/*<thisrel this+0x3c>*/ /*|0x4|*/ uint32_t GrowthSinceLastEstimate;
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
	union {
		/*<thisrel this+0x6b>*/ /*|0x1|*/ kexecute_options Flags;
		/*<thisrel this+0x6b>*/ /*|0x1|*/ uint8_t ExecuteOptions;
	};
}; // sizeof = 0x6C

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
	/*<thisrel this+0x168>*/ /*|0x8|*/ uint64_t Filler;
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
	/*<thisrel this+0x1f8>*/ /*|0x40|*/ mmsupport Vm;
	/*<thisrel this+0x238>*/ /*|0x4|*/ uint32_t LastFaultCount;
	/*<thisrel this+0x23c>*/ /*|0x4|*/ uint32_t ModifiedPageCount;
	/*<thisrel this+0x240>*/ /*|0x4|*/ uint32_t NumberOfVads;
	/*<thisrel this+0x244>*/ /*|0x4|*/ uint32_t JobStatus;
	/*<thisrel this+0x248>*/ /*|0x4|*/ uint32_t Flags;
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
	/*<thisrel this+0x252>*/ /*|0x2|*/ uint16_t SubSystemVersion;
	/*<thisrel this+0x254>*/ /*|0x1|*/ uint8_t PriorityClass;
	/*<thisrel this+0x255>*/ /*|0x1|*/ uint8_t WorkingSetAcquiredUnsafe;
	/*<thisrel this+0x258>*/ /*|0x4|*/ uint32_t Cookie;
}; // <size 0x260>



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
char * __stdcall
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
  get_process_peb_t(kprocess * Process);

// XP+ only
NTL__EXTERNAPI
get_process_peb_t PsGetProcessPeb;


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_PROCESS
