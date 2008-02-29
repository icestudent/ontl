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

  namespace intrinsic {

#if defined(_M_IX86)

#elif defined(_M_X64)
    extern "C" uint64_t __cdecl __readcr8();
    extern "C" void __cdecl __writecr8(uint64_t);

#	pragma intrinsic(__readcr8)
#	pragma intrinsic(__writecr8)

#endif

  }//namespace intrinsic


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

using nt::ansi_string;
using nt::unicode_string;
using nt::const_unicode_string;
using nt::const_ansi_string;

struct kthread;

//enum mode { KernelMode, UserMode };
struct kprocessor_mode { uint8_t mode; };

static const kprocessor_mode KernelMode = { 0 };
static const kprocessor_mode UserMode = { 1 };

typedef long kpriority;
typedef uintptr_t kaffinity_t;

#if 1

/// interrupt request level
typedef uint8_t kirql_t;

#if defined(_M_IX86)
NTL__EXTERNAPI
kirql_t __stdcall
  KeGetCurrentIrql();

NTL__EXTERNAPI
kirql_t __fastcall
  KfLowerIrql(kirql_t NewIrql);

NTL__EXTERNAPI
kirql_t __fastcall
  KfRaiseIrql(kirql_t NewIrql);

#elif defined(_M_X64)

__forceinline
kirql_t KeGetCurrentIrql()
{
  return (kirql_t)ntl::intrinsic::__readcr8();
}

__forceinline
kirql_t KfRaiseIrql(kirql_t NewIrql)
{
  kirql_t OldIrql = KeGetCurrentIrql();
  ntl::intrinsic::__writecr8(NewIrql);
  return OldIrql;
}

__forceinline
void KfLowerIrql(kirql_t NewIrql)
{
  ntl::intrinsic::__writecr8(NewIrql);
}

#endif

#ifdef _DEBUG
NTL__EXTERNAPI
void __stdcall
DbgBreakPointWithStatus(
                        ntstatus Status
                        );
#endif

/** Interrupt request level RAII wrapper */
struct kirql
{ 
  /// level definitions
  enum level {
		passive_level = 0,             // passive release level
		low_level = 0,                 // lowest interrupt level
		apc_level = 1,                 // apc interrupt level
		dispatch_level = 2,            // dispatcher level

#if defined(_M_IX86)
		profile_level = 27,            // timer used for profiling.
		clock1_level = 28,             // interval clock 1 level - not used on x86
		clock2_level = 28,             // interval clock 2 level
		ipi_level,                     // interprocessor interrupt level
		power_level,                   // power failure level
		high_level,                    // highest interrupt level
#elif defined(_M_X64)
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

public:
  kirql()
  {
    _ = KeGetCurrentIrql();
  }
  kirql(kirql_t irql)
  {
    _ = irql;
  }

	static kirql get_current()
	{
		return kirql(KeGetCurrentIrql());
	}

  void raise(const level NewIrql)
  {
#ifdef _DEBUG
    if(KeGetCurrentIrql() > static_cast<kirql_t>(NewIrql))
      // lower irql is impossible
      // IRQL_NOT_GREATER_OR_EQUAL
      DbgBreakPointWithStatus((ntstatus)0x00000009);
#endif
    _ = KfRaiseIrql(static_cast<kirql_t>(NewIrql));
  }
  void raisetodpc()
  {
#ifdef _DEBUG
    if(KeGetCurrentIrql() > static_cast<kirql_t>(dispatch_level))
      // lower irql is impossible
      // IRQL_NOT_GREATER_OR_EQUAL
      DbgBreakPointWithStatus((ntstatus)0x00000009);
#endif
    _ = KfRaiseIrql(dispatch_level);
  }
  void raisetosynch()
  {
#ifdef _DEBUG
    if(KeGetCurrentIrql() > static_cast<kirql_t>(synch_level))
      // lower irql is impossible
      // IRQL_NOT_GREATER_OR_EQUAL
      DbgBreakPointWithStatus((ntstatus)0x00000009);
#endif
    _ = KfRaiseIrql(synch_level);
  }
  void lower()
  {
    KfLowerIrql(_);
  }

  operator kirql_t() const
  {
    return _;
  }

	friend bool operator == (const kirql irql, level l)
	{
		return irql._ == static_cast<kirql_t>(l);
	}
	friend bool operator < (const kirql irql, level l)
	{
		return irql._ < static_cast<kirql_t>(l);
	}
	friend bool operator > (const kirql irql, level l)
	{
		return irql._ > static_cast<kirql_t>(l);
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

typedef uint64_t kspin_lock_queue_number_t;
enum kspin_lock_queue_number 
{
  LockQueueDispatcherLock,
  LockQueueExpansionLock,
  LockQueuePfnLock,
  LockQueueSystemSpaceLock,
  LockQueueVacbLock,
  LockQueueMasterLock,
  LockQueueNonPagedPoolLock,
  LockQueueIoCancelLock,
  LockQueueWorkQueueLock,
  LockQueueIoVpbLock,
  LockQueueIoDatabaseLock,
  LockQueueIoCompletionLock,
  LockQueueNtfsStructLock,
  LockQueueAfdWorkQueueLock,
  LockQueueBcbLock,
  LockQueueMmNonPagedPoolLock,
  LockQueueUnusedSpare16,
  LockQueueTimerTableLock,
  LockQueueMaximumLock = LockQueueTimerTableLock + (1 << (8-4))
};

struct kspin_lock_queue
{
  kspin_lock_queue* volatile Next;
  kspin_lock* volatile Lock;
};

#define MAXIMUM_LEADBYTES   12

struct cptableinfo {
  uint16_t CodePage;                    // code page number
  uint16_t MaximumCharacterSize;        // max length (bytes) of a char
  uint16_t DefaultChar;                 // default character (MB)
  uint16_t UniDefaultChar;              // default character (Unicode)
  uint16_t TransDefaultChar;            // translation of default char (Unicode)
  uint16_t TransUniDefaultChar;         // translation of Unic default char (MB)
  uint16_t DBCSCodePage;                // Non 0 for DBCS code pages
  uint8_t  LeadByte[MAXIMUM_LEADBYTES]; // lead byte ranges
  uint16_t*MultiByteTable;             // pointer to MB translation table
  void*    WideCharTable;              // pointer to WC translation table
  uint16_t*DBCSRanges;                 // pointer to DBCS ranges
  uint16_t*DBCSOffsets;                // pointer to DBCS offsets
};

struct nlstableinfo {
  cptableinfo OemTableInfo;
  cptableinfo AnsiTableInfo;
  uint16_t*   UpperCaseTable;             // 844 format upcase table
  uint16_t*   LowerCaseTable;             // 844 format lower case table
};

extern "C" {
  extern uint32_t* NtBuildNumber;
  extern uint32_t* InitSafeBootMode;
#if 0
  extern unicode_string* NtSystemRoot;
  extern const uint32_t* NtMajorVersion;
  extern const uint32_t* NtMinorVersion;
  extern uint32_t* CmNtCSDVersion;
  extern uint32_t* CmNtCSDReleaseType;
  extern uint32_t* CmNtSpBuildNumber;
  extern unicode_string* CmVersionString;
  extern unicode_string* CmCSDVersionString;

  extern const char NtBuildLab[];

  extern nlstableinfo* InitTableInfo;
  extern uint32_t* InitNlsTableSize;
  extern void** InitNlsTableBase;
  extern uint32_t* InitAnsiCodePageDataOffset;
  extern uint32_t* InitOemCodePageDataOffset;
  extern uint32_t* InitUnicodeCaseTableDataOffset;
  extern void** InitNlsSectionPointer;
  extern bool* InitSafeModeOptionPresent;

  extern bool* InitIsWinPEMode;
  extern uint32_t* InitWinPEModeType;
#endif
}

NTL__EXTERNAPI
bool __stdcall
  PsGetVersion(
    uint32_t *        MajorVersion,
    uint32_t *        MinorVersion,
    uint32_t *        BuildNumber,
    unicode_string *  CSDVersion
    );

static inline
bool get_version(uint32_t& major_version, uint32_t& minor_version)
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
kirql_t __fastcall
  KfAcquireSpinLock(kspin_lock * SpinLock);

NTL__EXTERNAPI
void __fastcall
  KfReleaseSpinLock(
    kspin_lock *  SpinLock,
    const kirql_t   NewIrql
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

struct kgate { dispatcher_header Header; };

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

#if 0

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

#else

struct kdpc_data 
{
  list_entry DpcListHead;
  kspin_lock DpcLock;
#if defined(_M_IX86)
  volatile int32_t DpcQueueDepth;
#elif defined(_M_X64)
  volatile int32_t DpcQueueDepth;
#endif
  uint32_t DpcCount;
};

struct kdpc 
{
  enum DpcType {
    Normal,
    Threaded
  };
  enum DpcImportance {
    LowImportance,
    MediumImportance,
    HighImportance
  };

  uint8_t Type;
#if defined(_M_IX86)
  uint8_t Number;
  uint8_t Importance;
#elif defined(_M_X64)
  uint8_t Importance;
  uint8_t Number;
  uint8_t Expedite;
#endif
  list_entry DpcListEntry;

  void (__stdcall *DeferredRoutine)(
    const kdpc* Dpc, 
    const void* DeferredContext, 
    const void* SystemArgument1, 
    const void* SystemArgument2);

  void* DeferredContext;
  void* SystemArgument1;
  void* SystemArgument2;

  kdpc_data* DpcData;
};

#if defined(_M_IX86)
STATIC_ASSERT(sizeof(kdpc) == 0x20);
#elif  _M_X64
STATIC_ASSERT(sizeof(kdpc) == 0x40);
#endif

#endif


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
// fast mutexes.
//
struct fast_mutex;

NTL__EXTERNAPI
void __fastcall
ExAcquireFastMutexUnsafe (fast_mutex* FastMutex);

NTL__EXTERNAPI
void __fastcall
ExReleaseFastMutexUnsafe (fast_mutex* FastMutex);


#if defined(_NTHAL_) && defined(_M_IX86)

NTL__EXTERNAPI
void __fastcall
ExiAcquireFastMutex (fast_mutex* FastMutex);

NTL__EXTERNAPI
void __fastcall
ExiReleaseFastMutex (fast_mutex* FastMutex);

NTL__EXTERNAPI
void __fastcall
ExiTryToAcquireFastMutex (fast_mutex* FastMutex);

#else

NTL__EXTERNAPI
void __fastcall
ExAcquireFastMutex (fast_mutex* FastMutex);

NTL__EXTERNAPI
void __fastcall
ExReleaseFastMutex (fast_mutex* FastMutex);

NTL__EXTERNAPI
bool __fastcall
ExTryToAcquireFastMutex (fast_mutex* FastMutex);

#endif // _NTHAL_ && _M_IX86


struct kthread;
struct fast_mutex {

  enum {
    FM_LOCK_BIT_V        = 0x0, // Lock bit as a bit number
    FM_LOCK_BIT          = 0x1, // Actual lock bit, 1 = Unlocked, 0 = Locked
    FM_LOCK_WAITER_WOKEN = 0x2, // A single waiter has been woken to acquire this lock
    FM_LOCK_WAITER_INC   = 0x4 // Increment value to change the waiters count
  };
  volatile uint32_t Count;
  kthread* Owner;
  uint32_t Contention;
  synchronization_event Gate;
  uint32_t OldIrql;

  fast_mutex()
    :Count(FM_LOCK_BIT), Owner(NULL),
    Contention(0)
  {
  }

  void acquire()
  {
    ExAcquireFastMutex(this);
  }
  bool try_acquire()
  {
    return ExTryToAcquireFastMutex(this);
  }
  void release()
  {
    ExReleaseFastMutex(this);
  }
  void acquire_unsafe()
  {
    ExAcquireFastMutexUnsafe(this);
  }
  void release_unsafe()
  {
    ExReleaseFastMutexUnsafe(this);
  }
};


//
// Mutex object
//
struct kmutant;

NTL__EXTERNAPI
void __stdcall
KeInitializeMutex (
                   kmutant* Mutex,
                   uint32_t Level
                   );
NTL__EXTERNAPI
int32_t __stdcall
KeReadStateMutex (
                  kmutant* Mutex
                  );
NTL__EXTERNAPI
int32_t __stdcall
KeReleaseMutex (
                kmutant* Mutex,
                bool Wait
                );

struct kmutant 
{
  dispatcher_header   Header;
  list_entry          MutantListEntry;
  kthread            *OwnerThread;
  bool                Abandoned;
  bool                ApcDisable;

  kmutant()
  {
    KeInitializeMutex(this, 0);
  }

  int32_t release(bool wait = false)
  {
    return KeReleaseMutex(this, wait);
  }

  int32_t read_state()
  {
    return KeReadStateMutex(this);
  }
};


// guarded mutex

struct kguarded_mutex 
{
  enum lock
  {
    bit_v = 0x00,
    bit   = 0x01,
    waiter_woken  = 0x02,
    waiter_inc    = 0x04
  };

  int32_t   Count;
  kthread*  Owner;
  uint32_t  Contention;
  kgate     Gate;
  int16_t   KernelApcDisable;
  int16_t   SpecialApcDisable;
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


#define PROCESSOR_FEATURE_MAX 64
#define MAX_WOW64_SHARED_ENTRIES 16

    enum AlternativeArchitectureType 
    {
      StandardDesign,                 // None == 0 == standard design
      NEC98x86,                       // NEC PC98xx series on X86
      EndAlternatives                 // past end of known alternatives
    };

    enum nt_product_type 
    {
      NtProductWinNt = 1,
      NtProductLanManNt,
      NtProductServer
    };

    // time
    struct ksystem_time
    {
      uint32_t LowPart;
      int32_t High1Time;
      int32_t High2Time;
    };

    struct time_fields
    {
      int16_t Year;        // range [1601...]
      int16_t Month;       // range [1..12]
      int16_t Day;         // range [1..31]
      int16_t Hour;        // range [0..23]
      int16_t Minute;      // range [0..59]
      int16_t Second;      // range [0..59]
      int16_t Milliseconds;// range [0..999]
      int16_t Weekday;     // range [0..6] == [Sunday..Saturday]
    };


    // user shared data
    struct kuser_shared_data 
    {
      static const kuser_shared_data& instance()
      {
        const kuser_shared_data* const kusd = reinterpret_cast<const kuser_shared_data* const>
#if defined(_M_IX86)
          (0xffdf0000);
#elif defined(_M_X64)
          (0xFFFFF78000000000UI64);
#endif
        return *kusd;
      }


      uint32_t TickCountLowDeprecated;
      uint32_t TickCountMultiplier;
      volatile ksystem_time InterruptTime;
      volatile ksystem_time SystemTime;
      volatile ksystem_time TimeZoneBias;
      uint16_t ImageNumberLow;
      uint16_t ImageNumberHigh;
      wchar_t NtSystemRoot[260];
      uint32_t MaxStackTraceDepth;
      uint32_t CryptoExponent;
      uint32_t TimeZoneId;
      uint32_t LargePageMinimum;
      uint32_t Reserved2[7];
      nt_product_type NtProductType;
      bool ProductTypeIsValid;
      uint32_t NtMajorVersion;
      uint32_t NtMinorVersion;
      bool ProcessorFeatures[PROCESSOR_FEATURE_MAX];
      uint32_t Reserved1;
      uint32_t Reserved3;
      volatile uint32_t TimeSlip;
      AlternativeArchitectureType AlternativeArchitecture;
      int64_t SystemExpirationDate;
      uint32_t SuiteMask;
      bool KdDebuggerEnabled;
      uint8_t NXSupportPolicy;
      volatile uint32_t ActiveConsoleId;
      volatile uint32_t DismountCount;
      uint32_t ComPlusPackage;
      uint32_t LastSystemRITEventTickCount;
      uint32_t NumberOfPhysicalPages;
      bool SafeBootMode;
      uint32_t TraceLogging;
      uint64_t TestRetInstruction;
      uint32_t SystemCall;
      uint32_t SystemCallReturn;
      uint64_t SystemCallPad[3];
      union {
        volatile ksystem_time TickCount;
        volatile uint64_t TickCountQuad;
      };
      uint32_t Cookie;
      // xp64+
      uint32_t Wow64SharedInformation[MAX_WOW64_SHARED_ENTRIES];
      uint16_t UserModeGlobalLogger[8];
      uint32_t HeapTracingPid[2];
      uint32_t CritSecTracingPid[2];
      uint32_t ImageFileExecutionOptions;
      union {
        uint64_t    AffinityPad;
        kaffinity_t ActiveProcessorAffinity;
      };
      volatile uint64_t InterruptTimeBias;
    };



    __forceinline
      void KeQuerySystemTime(int64_t* CurrentTime)
    {
      *CurrentTime = *reinterpret_cast<const volatile int64_t*>(&kuser_shared_data::instance().SystemTime);
    }

    __forceinline
      void KeQueryTickCount(int64_t* TickCount)
    {
      *TickCount = kuser_shared_data::instance().TickCountQuad;
    }

    NTL__EXTERNAPI
      void __stdcall
      ExSystemTimeToLocalTime(int64_t* SystemTime, int64_t* LocalTime);

    NTL__EXTERNAPI
      void __stdcall
      RtlTimeToTimeFields(int64_t* Time, time_fields* TimeFields);


    struct pp_lookaside_list {
      struct _general_lookaside *P;
      struct _general_lookaside *L;
    };

#define POOL_SMALL_LISTS 32

    // mm

    struct mmsupport_flags {

      //
      // The next 8 bits are protected by the expansion lock.
      //

      uint8_t SessionSpace : 1;
      uint8_t BeingTrimmed : 1;
      uint8_t SessionLeader : 1;
      uint8_t TrimHard : 1;
      uint8_t MaximumWorkingSetHard : 1;
      uint8_t ForceTrim : 1;
      uint8_t MinimumWorkingSetHard : 1;
      uint8_t Available0 : 1;

      uint8_t MemoryPriority : 8;

      //
      // The next 16 bits are protected by the working set mutex.
      //

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

    typedef uint32_t wsle_number_t;
    typedef uint32_t pfn_number_t;

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
      wsle_number_t NextSlot;               // The next slot to trim
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
      void* HighestUserAddress;           // Maintained for wow64 processes only
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

      //
      // This must be at the end.
      // Not used in system cache or session working set lists.
      //

      uint16_t UsedPageTableEntries[MM_USER_PAGE_TABLE_PAGES];

      uint32_t CommittedPageTables[MM_USER_PAGE_TABLE_PAGES/(sizeof(uint32_t)*8)];
#endif

    };


    //
    // Define executive resource function prototypes.
    //
    NTL__EXTERNAPI
      ntstatus __stdcall
      ExInitializeResourceLite (
      __out eresource* Resource
      );

    NTL__EXTERNAPI
      ntstatus __stdcall
      ExReinitializeResourceLite (
      __inout eresource* Resource
      );

    NTL__EXTERNAPI
      bool
      ExAcquireResourceSharedLite (
      __inout eresource* Resource,
      __in bool Wait
      );

#if (NTDDI_VERSION >= NTDDI_LONGHORN || NTDDI_VERSION >= NTDDI_WS03SP1)
    NTL__EXTERNAPI
      void*
      ExEnterCriticalRegionAndAcquireResourceShared (
      __inout eresource* Resource
      );
#endif

    NTL__EXTERNAPI
      bool
      ExAcquireResourceExclusiveLite (
      __inout eresource* Resource,
      __in bool Wait
      );

#if (NTDDI_VERSION >= NTDDI_LONGHORN || NTDDI_VERSION >= NTDDI_WS03SP1)
    NTL__EXTERNAPI
      void*
      ExEnterCriticalRegionAndAcquireResourceExclusive (
      __inout eresource* Resource
      );
#endif

    NTL__EXTERNAPI
      bool
      ExAcquireSharedStarveExclusive(
      __inout eresource* Resource,
      __in bool Wait
      );

    NTL__EXTERNAPI
      bool
      ExAcquireSharedWaitForExclusive(
      __inout eresource* Resource,
      __in bool Wait
      );

#if (NTDDI_VERSION >= NTDDI_LONGHORN || NTDDI_VERSION >= NTDDI_WS03SP1)
    NTL__EXTERNAPI
      void*
      ExEnterCriticalRegionAndAcquireSharedWaitForExclusive (
      __inout eresource* Resource
      );
#endif

    //
    //  void
    //  ExReleaseResource(
    //      IN eresource* Resource
    //      );
    //

#if PRAGMA_DEPRECATED_DDK
#pragma deprecated(ExReleaseResource)       // Use ExReleaseResourceLite
#endif
#define ExReleaseResource(R) (ExReleaseResourceLite(R))

    NTL__EXTERNAPI
      void
      __fastcall
      ExReleaseResourceLite(
      __inout eresource* Resource
      );

#if (NTDDI_VERSION >= NTDDI_LONGHORN || NTDDI_VERSION >= NTDDI_WS03SP1)
    NTL__EXTERNAPI
      void
      __fastcall
      ExReleaseResourceAndLeaveCriticalRegion(
      __inout eresource* Resource
      );
#endif

    NTL__EXTERNAPI
      void
      ExReleaseResourceForThreadLite(
      __inout eresource* Resource,
      __in eresource_thread_t ResourceThreadId
      );

    NTL__EXTERNAPI
      void
      ExSetResourceOwnerPointer(
      __inout eresource* Resource,
      __in void* OwnerPointer
      );

    NTL__EXTERNAPI
      void
      ExConvertExclusiveToSharedLite(
      __inout eresource* Resource
      );

    NTL__EXTERNAPI
      ntstatus __stdcall
      ExDeleteResourceLite (
      __inout eresource* Resource
      );

    NTL__EXTERNAPI
      uint32_t
      ExGetExclusiveWaiterCount (
      __in eresource* Resource
      );

    NTL__EXTERNAPI
      uint32_t
      ExGetSharedWaiterCount (
      __in eresource* Resource
      );

    NTL__EXTERNAPI
      bool
      ExIsResourceAcquiredExclusiveLite (
      __in eresource* Resource
      );

    NTL__EXTERNAPI
      uint32_t
      ExIsResourceAcquiredSharedLite (
      __in eresource* Resource
      );

#define ExIsResourceAcquiredLite ExIsResourceAcquiredSharedLite

}//namespace km
}//namespace ntl

#endif//#ifndef NTL__KM_BASEDEF
