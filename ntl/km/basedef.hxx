/**\file*********************************************************************
 *                                                                     \brief
 *  NT Kernel Types support library
 *
 ****************************************************************************
 */

#ifndef NTL__KM_BASEDEF
#define NTL__KM_BASEDEF

#include "../nt/basedef.hxx"
#include "../stdlib.hxx"
#include "../nt/exception.hxx"

namespace ntl {
namespace km {

#ifndef SYSTEM_CACHE_ALIGNMENT_SIZE
#define SYSTEM_CACHE_ALIGNMENT_SIZE 64
#endif

using nt::slist_header;
using nt::single_list_entry;
using nt::slist_entry;
using nt::list_entry;
using nt::list_head;
using nt::status;
using nt::exception;

using nt::access_mask;
using nt::synchronize;
using nt::generic_read;

using nt::io_status_block;
using nt::io_apc_routine;

using nt::device_power_state;
using nt::system_power_state;

struct kthread;

//enum mode { KernelMode, UserMode };
struct kprocessor_mode { uint8_t mode; };

static const kprocessor_mode KernelMode = { 0 };
static const kprocessor_mode UserMode = { 1 };

typedef long kpriority;

#if 1

/// interrupt request level
typedef uint8_t kirql_t;

NTL__EXTERNAPI
kirql_t __stdcall
  KeGetCurrentIrql();

NTL__EXTERNAPI
kirql_t __fastcall
  KfLowerIrql(kirql_t NewIrql);

NTL__EXTERNAPI
kirql_t __fastcall
  KfRaiseIrql(kirql_t NewIrql);

/** Interrupt request level RAII wrapper */
struct kirql
{ 
  /// level definitions
  enum level {
		passive_level = 0,             // passive release level
		low_level = 0,                 // lowest interrupt level
		apc_level = 1,                 // apc interrupt level
		dispatch_level = 2,            // dispatcher level

#ifdef _X86_
		profile_level = 27,            // timer used for profiling.
		clock1_level = 28,             // interval clock 1 level - not used on x86
		clock2_level = 28,             // interval clock 2 level
		ipi_level,                     // interprocessor interrupt level
		power_level,                   // power failure level
		high_level,                    // highest interrupt level
#elif _M_AMD64
    clock_level = 13,              // interval clock level
    ipi_level,                     // interprocessor interrupt level
    power_level,                   // power failure level
    profile_level,                 // timer used for profiling
    high_level,                    // highest interrupt level
#endif

#ifdef NT_UP
    // UP
    synch_level = dispatch_level  // synchronization level
#else
    // MP
    synch_level = ipi_level-2
#endif
	};

protected:
	kirql_t _;
	kirql(kirql_t irql)
	{
		_ = irql;
	}

public:
	static kirql get_current()
	{
		return kirql(KeGetCurrentIrql());
	}
  kirql()
  {
    _ = KeGetCurrentIrql();
  }
  void raise(const level NewIrql)
  {
    _ = KfRaiseIrql((kirql_t)NewIrql);
  }
  void raisetodpc()
  {
    _ = KfRaiseIrql(dispatch_level);
  }
  void raisetosynch()
  {
    _ = KfRaiseIrql(synch_level);
  }
  void lower()
  {
    KfLowerIrql(_);
  }


	friend static bool operator == (const kirql irql, level l)
	{
		return irql._ == (kirql_t)l;
	}
	friend static bool operator < (const kirql irql, level l)
	{
		return irql._ < (kirql_t)l;
	}
	friend static bool operator > (const kirql irql, level l)
	{
		return irql._ > (kirql_t)l;
	}
};

#else
typedef uint8_t kirql;
#endif

struct kspin_lock
{ 
    kspin_lock() : _() {} // no need for KeInitializeSpinLock call

    __forceinline kirql acquire();
    __forceinline void release(const kirql);

  private:
    /*volatile*/ uintptr_t _;
};


NTL__EXTERNAPI
bool __stdcall
  PsGetVersion(
    uint32_t *        MajorVersion,
    uint32_t *        MinorVersion,
    uint32_t *        BuildNumber,
    void/*unicode_string*/ *  CSDVersion
    );

static inline
bool get_version(uint32_t & major_version, uint32_t & minor_version)
{
  return PsGetVersion(&major_version, &minor_version, 0, 0);
}

NTL__EXTERNAPI
ntstatus __stdcall
  ZwYieldExecution();

static inline
ntstatus yield_execution()
{
  return ZwYieldExecution();
}


NTL__EXTERNAPI
ntstatus __stdcall
  KeDelayExecutionThread(
    kprocessor_mode WaitMode,
    bool            Alertable,
    const int64_t * Interval
    );

enum times
{
  nanoseconds   = 1,
  microseconds  = 10   * nanoseconds,
  milliseconds  = 1000 * microseconds,
  seconds       = 1000 * milliseconds,
  minutes       = 60   * seconds, 
//  hours         = int64_t(60)   * minutes,
//  days          = int64_t(24)   * hours,
};


template<times TimeResolution>
static inline
ntstatus sleep(
  uint32_t        time_resolution,
  bool            alertable = false,
  kprocessor_mode wait_mode = KernelMode)
{
  const int64_t interval = int64_t(-1) * TimeResolution * time_resolution;
  return KeDelayExecutionThread(wait_mode, alertable, &interval);
}

/// default milliseconds
static inline
ntstatus sleep(
  uint32_t        ms,
  bool            alertable = false,
  kprocessor_mode wait_mode = KernelMode)
{
  const int64_t interval = int64_t(-1) * milliseconds * ms;
  return KeDelayExecutionThread(wait_mode, alertable, &interval);
}


NTL__EXTERNAPI
kirql __fastcall
  KfAcquireSpinLock(kspin_lock * SpinLock);

NTL__EXTERNAPI
void __fastcall
  KfReleaseSpinLock(
    kspin_lock *  SpinLock,
    const kirql   NewIrql
    );


kirql kspin_lock::acquire()
{
  return KfAcquireSpinLock(this);
}

void kspin_lock::release(const kirql new_irql)
{
  KfReleaseSpinLock(this, new_irql);
}


struct kspin_lock_at_dpc : public kspin_lock
{
    __forceinline void acquire();
    __forceinline void release();
};

NTL__EXTERNAPI
void __fastcall
  KefAcquireSpinLockAtDpcLevel(kspin_lock * SpinLock);

NTL__EXTERNAPI
void __fastcall
  KefReleaseSpinLockFromDpcLevel(kspin_lock * SpinLock);


void kspin_lock_at_dpc::acquire()
{
  KefAcquireSpinLockAtDpcLevel(this);
}

void kspin_lock_at_dpc::release()
{
  KefReleaseSpinLockFromDpcLevel(this);
}


typedef ntl::atomic_exec<kspin_lock, kirql> synchronize_access;
typedef ntl::atomic_exec<kspin_lock_at_dpc, void> synchronize_access_at_dpc_level;


struct dispatcher_header
{
  uint8_t     Type;
  uint8_t     Absolute;
  uint8_t     Size;
  uint8_t     Inserted;
  long        SignalState;
  list_entry  WaitListHead;
};


struct kevent { dispatcher_header Header; };

enum event_type { NotificationEvent, SynchronizationEvent };

NTL__EXTERNAPI
void __stdcall
  KeInitializeEvent(
    kevent *    Event,
    event_type  Type,
    bool        State
    );

NTL__EXTERNAPI
long __stdcall
  KeSetEvent(
    kevent *  Event,
    kpriority Increment,
    bool      Wait
    );

template <event_type Type>
struct event : kevent
{
  event(bool Signaled = false)
  {
    KeInitializeEvent(this, Type, Signaled);
  }

  long set(
    kpriority Increment = 0,
    bool      Wait      = false)
  {
    return KeSetEvent(this, Increment, Wait);
  }
};

typedef event<NotificationEvent> notification_event;
typedef event<SynchronizationEvent> synchronization_event;


enum kwait_reason
{
  Executive,
  FreePage,
  PageIn,
  PoolAllocation,
  DelayExecution,
  Suspended,
  UserRequest,
  WrExecutive,
  WrFreePage,
  WrPageIn,
  WrPoolAllocation,
  WrDelayExecution,
  WrSuspended,
  WrUserRequest,
  WrEventPair,
  WrQueue,
  WrLpcReceive,
  WrLpcReply,
  WrVirtualMemory,
  WrPageOut,
  WrRendezvous,
  Spare2,
  Spare3,
  Spare4,
  Spare5,
  Spare6,
  WrKernel,
  WrResource,
  WrPushLock,
  WrMutex,
  WrQuantumEnd,
  WrDispatchInt,
  WrPreempted,
  WrYieldExecution,
  WrFastMutex,
  WrGuardedMutex,
  WrRundown,
  MaximumWaitReason
}; // enum kwait_reason

NTL__EXTERNAPI
ntstatus __stdcall
  KeWaitForSingleObject(
    void *          Object,
    kwait_reason    WaitReason,
    kprocessor_mode WaitMode,
    bool            Alertable,
    int64_t *       Timeout  __optional
    );

static inline
ntstatus
  wait_for_single_object(
    void *          Object,
    kwait_reason    WaitReason  = Executive,
    kprocessor_mode WaitMode    = KernelMode,
    bool            Alertable   = false,
    int64_t *       Timeout     = 0
    )
{
  return KeWaitForSingleObject(Object, WaitReason, WaitMode, Alertable, Timeout);
}


struct kdpc;

typedef
void __stdcall
  kdeferred_routine_t(
    const kdpc *  Dpc,
    const void *  DeferredContext,
    const void *  SystemArgument1,
    const void *  SystemArgument2
    );

struct kdpc
{
  uint8_t               Type;
  uint8_t               Importance;
  uint8_t               Number;
  uint8_t               Expedite;
  list_entry            DpcListEntry;
  kdeferred_routine_t * DeferredRoutine;
  void *                DeferredContext;
  void *                SystemArgument1;
  void *                SystemArgument2;
  void *                DpcData;
};


NTL__EXTERNAPI void *     MmHighestUserAddress;
NTL__EXTERNAPI void *     MmSystemRangeStart;
NTL__EXTERNAPI uintptr_t  MmUserProbeAddress;

static inline
void * highest_user_address()
{
  return MmHighestUserAddress;
}

static inline
void * lowest_user_address()
{
  return (void*)0x10000;
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


struct mdl;
struct irp;

NTL__EXTERNAPI
void * __stdcall
  MmMapLockedPages(
    const mdl *     MemoryDescriptorList,
    kprocessor_mode AccessMode
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
    struct _EPROCESS *  Process;
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
      return
        IoAllocateMdl(virtual_address, length, Secondary_buffer, charge_quota, pirp);
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


struct ksemaphore
{
  dispatcher_header Header;
  long  Limit;
};


struct ktimer
{
  dispatcher_header Header;
  uint64_t          DueTime;
  list_entry        TimerListEntry;
  kdpc *            Dpc;
  long              Period;
};


struct work_queue_item 
{
  list_entry  List;
  void     (* WorkerRoutine)(void*);
  void      * Parameter;
};


//
//  executive resource data structures.
//

typedef uintptr_t eresource_thread_t;

struct owner_entry {
	eresource_thread_t OwnerThread;
	union {
		int32_t OwnerCount;
		uint32_t TableSize;
	};

};

struct eresource {
	list_entry SystemResourcesList;
	owner_entry* OwnerTable;

	int16_t ActiveCount;
	enum Flags {
		ResourceNeverExclusive		= 0x10,
		ResourceReleaseByOtherThread= 0x20,
		ResourceOwnedExclusive		= 0x80
	};
	uint16_t Flag;

	volatile ksemaphore* SharedWaiters;
	volatile kevent* ExclusiveWaiters;

	owner_entry OwnerEntry;
	uint32_t ActiveEntries;
	uint32_t ContentionCount;
	uint32_t NumberOfSharedWaiters;
	uint32_t NumberOfExclusiveWaiters;
#if defined(_M_X64)
	void* Reserved2;
#endif
	union {
		void* Address;
		uintptr_t CreatorBackTraceIndex;
	};

	kspin_lock SpinLock;
};


#define RESOURCE_HASH_TABLE_SIZE 64

struct resource_hash_entry {
	list_entry ListEntry;
	void* Address;
	uint32_t ContentionCount;
	uint32_t Number;
};

struct resource_performance_data {
	uint32_t ActiveResourceCount;
	uint32_t TotalResourceCount;
	uint32_t ExclusiveAcquire;
	uint32_t SharedFirstLevel;
	uint32_t SharedSecondLevel;
	uint32_t StarveFirstLevel;
	uint32_t StarveSecondLevel;
	uint32_t WaitForExclusive;
	uint32_t OwnerTableExpands;
	uint32_t MaximumTableExpand;
	list_entry HashTable[RESOURCE_HASH_TABLE_SIZE];
};


}//namespace ntl
}//namespace nt

#endif//#ifndef NTL__KM_BASEDEF
