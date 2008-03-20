/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
 *
 ****************************************************************************
 */

#ifndef NTL__KM_THREAD
#define NTL__KM_THREAD

#include "basedef.hxx"
#include "object.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "../nt/teb.hxx"
#include "../device_traits.hxx"
#include "../nt/context.hxx"

namespace ntl {
namespace km {

using nt::client_id;
using nt::context;

struct ktrap_frame;
struct kqueue;
struct kthread;
struct kgate;

NTL__EXTERNAPI
kthread * __stdcall
  KeGetCurrentThread();

///\note XP+ only, use KeGetCurrentThread instead
NTL__EXTERNAPI
kthread * __stdcall
  PsGetCurrentThread();

NTL__EXTERNAPI
kthread * __stdcall
  PsGetCurrentThreadId();


NTL__EXTERNAPI
ntstatus __stdcall
  PsLookupThreadByThreadId(
    legacy_handle ThreadId,
    kthread * *   Thread
    );

static inline
kthread * lookup_tread(legacy_handle threads_id)
{
  kthread * thread;
  return nt::success(PsLookupThreadByThreadId(threads_id, &thread))
    ? thread : 0;
}

static __forceinline
void dereference_object(kthread * thread)
{
  ObfDereferenceObject(thread);
}


typedef
nt::tib * __stdcall
  get_thread_teb_t(const kthread * Thread);
  
NTL__EXTERNAPI
get_thread_teb_t PsGetThreadTeb;


struct kwait_block
{
  list_entry    WaitListEntry;
  kthread *     Thread;
  void *        Object;
  kwait_block * NextWaitBlock;
  uint16_t      WaitKey;
  uint16_t      WaitType;
};

typedef uintptr_t kaffinity_t;
struct kaffinity
{
  kaffinity() {}
  kaffinity(kaffinity_t affinity) : affinity(affinity) { }
  kaffinity_t affinity;
};

#define LOW_PRIORITY 0              // Lowest thread priority level
#define LOW_REALTIME_PRIORITY 16    // Lowest realtime priority level
#define HIGH_PRIORITY 31            // Highest thread priority level
#define MAXIMUM_PRIORITY 32         // Number of thread priority levels

#if defined(_M_IX86)
struct kpcr
{
  union {
    nt::tib NtTib;
    struct {
      ntl::nt::exception::registration *Used_ExceptionList;
      void*     Used_StackBase;
      void*     Spare2;
      void*     TssCopy;
      uint32_t  ContextSwitches;
      kaffinity SetMemberCopy;
      void*     Used_Self;
    };
  };

  struct kpcr*  SelfPcr;
  struct kpcrb* Pcrb;
  kirql Irql;

  uint32_t    IRR;
  uint32_t    IrrActive;
  uint32_t    IDR;
  void*       KdVersionBlock;

  struct kidtentry*  IDT;
  struct kgdtentry*  GDT;
  struct ktss*       TSS;
  uint16_t    MajorVersion;
  uint16_t    MinorVersion;
  kaffinity   SetMember;
  uint32_t    StallScaleFactor;
  uint8_t     SpareUnused;
  uint8_t     Number;

  uint8_t     Spare0;
  uint8_t     SecondLevelCacheAssociativity;
  uint32_t    VdmAlert;
  uint32_t    KernelReserved[14];
  uint32_t    SecondLevelCacheSize;
  uint32_t    HalReserved[16];
};
#elif defined(_M_X64)

#define MAXIMUM_PROCESSORS 64


struct kdescriptor 
{
  uint16_t Pad[3];
  uint16_t Limit;
  void* Base;
};

struct kdescriptor32
{
  uint16_t Pad[3];
  uint16_t Limit;
  uint32_t Base;
};

struct kspecial_registers {
  uint64_t Cr0;
  uint64_t Cr2;
  uint64_t Cr3;
  uint64_t Cr4;
  uint64_t KernelDr0;
  uint64_t KernelDr1;
  uint64_t KernelDr2;
  uint64_t KernelDr3;
  uint64_t KernelDr6;
  uint64_t KernelDr7;
  kdescriptor Gdtr;
  kdescriptor Idtr;
  uint16_t Tr;
  uint16_t Ldtr;
  uint32_t MxCsr;
  uint64_t DebugControl;
  uint64_t LastBranchToRip;
  uint64_t LastBranchFromRip;
  uint64_t LastExceptionToRip;
  uint64_t LastExceptionFromRip;
  uint64_t Cr8;
  uint64_t MsrGsBase;
  uint64_t MsrGsSwap;
  uint64_t MsrStar;
  uint64_t MsrLStar;
  uint64_t MsrCStar;
  uint64_t MsrSyscallMask;
};

struct kprocessor_state
{
  kspecial_registers SpecialRegisters;
  context ContextFrame;
};

struct processor_idle_times {
  uint64_t  StartTime;
  uint64_t  EndTime;
  uint32_t  IdleHandlerReserved[4];
};

struct processor_perf_state {
  uint8_t                       PercentFrequency;   // max == POWER_PERF_SCALE
  uint8_t                       MinCapacity;        // battery capacity %
  uint16_t                      Power;              // in milliwatts
  uint8_t                       IncreaseLevel;      // goto higher state
  uint8_t                       DecreaseLevel;      // goto lower state
  uint16_t                      Flags;
  uint32_t                       IncreaseTime;       // in tick counts
  uint32_t                       DecreaseTime;       // in tick counts
  uint32_t                       IncreaseCount;      // goto higher state
  uint32_t                       DecreaseCount;      // goto lower state
  uint64_t                       PerformanceTime;    // Tick count
};

struct processor_power_state {
  void (__fastcall* IdleFunction)(processor_power_state* PState);
  uint32_t                    Idle0KernelTimeLimit;
  uint32_t                    Idle0LastTime;

  void*                       IdleHandlers;
  void*                       IdleState;
  uint32_t                    IdleHandlersCount;

  uint64_t                    LastCheck;
  processor_idle_times        IdleTimes;

  uint32_t                    IdleTime1;
  uint32_t                    PromotionCheck;
  uint32_t                    IdleTime2;

  uint8_t                       CurrentThrottle;    // current throttle setting
  uint8_t                       ThermalThrottleLimit;   // max available throttle setting
  uint8_t                       CurrentThrottleIndex;
  uint8_t                       ThermalThrottleIndex;

  uint32_t                       LastKernelUserTime;
  uint32_t                       PerfIdleTime;

  // temp for debugging
  uint64_t                   DebugDelta;
  uint32_t                       DebugCount;

  uint32_t                       LastSysTime;
  uint64_t                   TotalIdleStateTime[3];
  uint32_t                       TotalIdleTransitions[3];
  uint64_t                   PreviousC3StateTime;
  uint8_t                       KneeThrottleIndex;
  uint8_t                       ThrottleLimitIndex;
  uint8_t                       PerfStatesCount;
  uint8_t                       ProcessorMinThrottle;
  uint8_t                       ProcessorMaxThrottle;
  uint8_t                       LastBusyPercentage;
  uint8_t                       LastC3Percentage;
  uint8_t                       LastAdjustedBusyPercentage;
  uint32_t                    PromotionCount;
  uint32_t                    DemotionCount;
  uint32_t                    ErrorCount;
  uint32_t                    RetryCount;
  uint32_t                    Flags;
  int64_t               PerfCounterFrequency;
  uint32_t                    PerfTickCount;
  ktimer                      PerfTimer;
  kdpc                        PerfDpc;
  processor_perf_state*       PerfStates;
  ntstatus (__fastcall* PerfSetThrottle)(uint8_t Throttle);
  uint32_t                       LastC3KernelUserTime;
  uint32_t                       Spare1[1];
};


//
// Interprocessor interrupt worker routine function prototype.
//

typedef void* kipi_context;

//
// Define request packet structure.
//

struct krequest_packet {
  void* CurrentPacket[3];
  void (__stdcall* WorkerRoutine)(kipi_context, void*, void*, void*);
};

//
// Define request mailbox structure.
//

struct request_mailbox {
  int64_t RequestSummary;
  union {
    krequest_packet RequestPacket;
    void* Virtual[7];
  };
};

enum processor_cache_type {
  CacheUnified,
  CacheInstruction,
  CacheData,
  CacheTrace
};

struct cache_descriptor {
  uint8_t  Level;
  uint8_t  Associativity;
  uint16_t LineSize;
  uint32_t  Size;
  processor_cache_type Type;
};



struct kprcb
{
  uint32_t MxCsr;
  uint8_t Number;
  uint8_t NestingLevel;
  bool InterruptRequest;
  bool IdleHalt;
  struct kthread *CurrentThread;
  struct kthread *NextThread;
  struct kthread *IdleThread;
  uint64_t UserRsp;
  uint64_t RspBase;
  kspin_lock PrcbLock;
  kaffinity SetMember;
  kprocessor_state ProcessorState;
  int8_t CpuType;
  int8_t CpuID;
  uint16_t CpuStep;
  uint32_t MHz;
  uint64_t HalReserved[8];
  uint16_t MinorVersion;
  uint16_t MajorVersion;
  uint8_t BuildType;
  uint8_t CpuVendor;
  uint8_t InitialApicId;
  uint8_t LogicalProcessorsPerPhysicalProcessor;
  uint32_t ApicMask;
  uint8_t CFlushSize;
  uint8_t PrcbPad0x[3];
  void* AcpiReserved;
  uint64_t PrcbPad00[4];

  //
  // End of the architecturally defined section of the PRCB.
  //
  // end_nthal end_ntosp
  //
  // Numbered queued spin locks - 128-byte aligned.
  //

  kspin_lock_queue LockQueue[LockQueueMaximumLock];

  //
  // Nonpaged per processor lookaside lists - 128-byte aligned.
  //

  pp_lookaside_list PPLookasideList[16];

  //
  // Nonpaged per processor small pool lookaside lists - 128-byte aligned.
  //

  pp_lookaside_list PPNPagedLookasideList[pool_small_lists];

  //
  // Paged per processor small pool lookaside lists.
  //

  pp_lookaside_list PPPagedLookasideList[pool_small_lists];

  //
  // MP interprocessor request packet barrier - 128-byte aligned.
  //
  // This cache line shares per processor data with the packet barrier which
  // is used to signal the completion of an IPI request.
  //
  // The packet barrier variable is written by the initiating processor when
  // an IPI request is distributed to more than one target processor (sharing
  // other data in the cache line increases the probability that the write will
  // hit in the cache).
  //
  // The initiating processor waits (at elevated IRQL - generally SYNCH level)
  // for the last finishing processor to clear packet barrier which will cause
  // the packet barrier cache line to transfer to the last finishing processor
  // then back to respective processor. 
  //
  // N.B. This results in minimal sharing of the cache line (no more than would
  // have occurred if the packet barrier was in a cache line all by itself)) and
  // increases the probability of a cache hit when packet barrier is initialized.
  //

  volatile kaffinity PacketBarrier;
  single_list_entry DeferredReadyListHead;

  //
  // Memory management counters.
  //

  volatile int32_t MmPageFaultCount;
  volatile int32_t MmCopyOnWriteCount;
  volatile int32_t MmTransitionCount;
  volatile int32_t MmCacheTransitionCount;
  volatile int32_t MmDemandZeroCount;
  volatile int32_t MmPageReadCount;
  volatile int32_t MmPageReadIoCount;
  volatile int32_t MmCacheReadCount;
  volatile int32_t MmCacheIoCount;
  volatile int32_t MmDirtyPagesWriteCount;
  volatile int32_t MmDirtyWriteIoCount;
  volatile int32_t MmMappedPagesWriteCount;
  volatile int32_t MmMappedWriteIoCount;

  //
  // I/O IRP float.
  //

  int32_t LookasideIrpFloat;

  //
  // Number of system calls.
  //

  uint32_t KeSystemCalls;

  //
  // I/O system counters.
  //

  volatile int32_t IoReadOperationCount;
  volatile int32_t IoWriteOperationCount;
  volatile int32_t IoOtherOperationCount;
  int64_t IoReadTransferCount;
  int64_t IoWriteTransferCount;
  int64_t IoOtherTransferCount;

  //
  // Context switch count.
  //

  uint32_t KeContextSwitches;
  uint8_t PrcbPad2[12];

  //
  // MP interprocessor request packet and summary - 128-byte aligned.
  //

  volatile kaffinity TargetSet;
  volatile uint32_t IpiFrozen;
  uint8_t PrcbPad3[116];

  //
  // Interprocessor request summary - 128-byte aligned.
  //

  request_mailbox RequestMailbox[MAXIMUM_PROCESSORS];

  //
  // Interprocessor sender summary;
  //

  volatile kaffinity SenderSummary;
  uint8_t PrcbPad4[120];

  //
  // DPC listhead, counts, and batching parameters - 128-byte aligned.
  //

  kdpc_data DpcData[2];
  void* DpcStack;
  void* SavedRsp;
  int32_t MaximumDpcQueueDepth;
  uint32_t DpcRequestRate;
  uint32_t MinimumDpcRate;
  volatile bool DpcInterruptRequested;
  volatile bool DpcThreadRequested;

  //
  // N.B. the following two fields must be on a word boundary.
  //

  volatile bool DpcRoutineActive;
  volatile bool DpcThreadActive;
  union {
    volatile uint64_t TimerHand;
    volatile uint64_t TimerRequest;
  };

  int32_t TickOffset;
  int32_t MasterOffset;
  uint32_t DpcLastCount;
  bool ThreadDpcEnable;
  volatile bool QuantumEnd;
  uint8_t PrcbPad50;
  volatile bool IdleSchedule;
  int32_t DpcSetEventRequest;
  int32_t PrcbPad40;

  //
  // DPC thread and generic call DPC - 128-byte aligned
  //

  void* DpcThread;
  kevent DpcEvent;
  kdpc CallDpc;
  uint64_t PrcbPad7[4];

  //
  // Per-processor ready summary and ready queues - 128-byte aligned.
  //
  // N.B. Ready summary is in the first cache line as the queue for priority
  //      zero is never used.
  //

  list_entry WaitListHead;
  uint32_t ReadySummary;
  uint32_t QueueIndex;
  list_entry DispatcherReadyListHead[MAXIMUM_PRIORITY];

  //
  // Miscellaneous counters.
  //

  uint32_t InterruptCount;
  uint32_t KernelTime;
  uint32_t UserTime;
  uint32_t DpcTime;
  uint32_t InterruptTime;
  uint32_t AdjustDpcThreshold;
  bool SkipTick;
  kirql DebuggerSavedIRQL;
  uint8_t PollSlot;
  uint8_t PrcbPad8[13];
  struct _KNODE * ParentNode;
  kaffinity MultiThreadProcessorSet;
  struct _KPRCB * MultiThreadSetMaster;
  int32_t Sleeping;
  uint32_t PrcbPad90[1];
  uint32_t DebugDpcTime;
  uint32_t PageColor;
  uint32_t NodeColor;
  uint32_t NodeShiftedColor;
  uint32_t SecondaryColorMask;
  uint8_t PrcbPad9[12];

  //
  // Performance counters - 128-byte aligned.
  //
  // Cache manager performance counters.
  //

  uint32_t CcFastReadNoWait;
  uint32_t CcFastReadWait;
  uint32_t CcFastReadNotPossible;
  uint32_t CcCopyReadNoWait;
  uint32_t CcCopyReadWait;
  uint32_t CcCopyReadNoWaitMiss;

  //
  // Kernel performance counters.
  //

  uint32_t KeAlignmentFixupCount;
  uint32_t KeDcacheFlushCount;
  uint32_t KeExceptionDispatchCount;
  uint32_t KeFirstLevelTbFills;
  uint32_t KeFloatingEmulationCount;
  uint32_t KeIcacheFlushCount;
  uint32_t KeSecondLevelTbFills;

  //
  // Processor information.
  //

  uint8_t VendorString[13];
  uint8_t PrcbPad10[2];
  uint32_t FeatureBits;
  int64_t UpdateSignature;

  //
  // Processors power state
  //

  processor_power_state PowerState;

  //
  // Logical Processor Cache Information  
  //

  cache_descriptor Cache[5];
  uint32_t CacheCount;
};

struct kpcr
{
  template<typename type>
  static inline type get(type kpcr::* member, Int2Type<sizeof(uint8_t)>)
  {
    return (type)
#if defined(_M_IX86)
      ntl::intrinsic::__readfsbyte
#elif defined(_M_X64)
      ntl::intrinsic::__readgsbyte
#endif
      ((uint32_t)offsetof(kpcr,*member));
  }  
  template<typename type>
  static inline type get(type kpcr::* member, Int2Type<sizeof(uint16_t)>)
  {
    return (type)
#if defined(_M_IX86)
      ntl::intrinsic::__readfsword
#elif defined(_M_X64)
      ntl::intrinsic::__readgsword
#endif
      ((uint32_t)offsetof(kpcr,*member));
  }  
  template<typename type>
  static inline type get(type kpcr::* member, Int2Type<sizeof(uint32_t)>)
  {
    return (type)
#if defined(_M_IX86)
      ntl::intrinsic::__readfsdword
#elif defined(_M_X64)
      ntl::intrinsic::__readgsdword
#endif
      ((uint32_t)offsetof(kpcr,*member));
  }  
  template<typename type>
  static inline type get(type kpcr::* member, Int2Type<sizeof(uint64_t)>)
  {
    return (type)
#if defined(_M_IX86)
      ntl::intrinsic::__readfsqword
#elif defined(_M_X64)
      ntl::intrinsic::__readgsqword
#endif
      ((uint32_t)offsetof(kpcr,*member));
  }  
  template<typename type>
  static inline type get(type kpcr::* member)
  {
    return get( member, Int2Type<sizeof(type)>() );
  }

  static __forceinline
    kpcr& instance() { return *static_cast<kpcr*>(get(&kpcr::Self)); }



  union {
    nt::tib NtTib;
    struct {
      struct kgdtentry* GdtBase;
      struct ktss*      TssBase;
      void*             PerfGlobalGroupMask;
      kpcr*             Self;
      kprcb*            CurrentPcrb;
      kspin_lock_queue  LockArray;
      void*             Used_Self;
    };
  };
  struct kidtentry*   IdtBase;
  uint64_t Unused[2];
  kirql    Irql;
  uint8_t  SecondLevelCacheAssociativity;
  uint8_t  ObsoleteNumber;
  uint8_t  Fill0;
  uint32_t Unused0[3];
  uint16_t MajorVersion;
  uint16_t MinorVersion;
  uint32_t StallScaleFactor;
  void*    Unused1[3];
  uint32_t KernelReserved[15];
  uint32_t SecondLevelCacheSize;
  uint32_t HalReserved[16];
  uint32_t Unused2;
  void*    KdVersionBlock;
  void*    Unused3;
  uint32_t PcrAlign1[24];
  kprcb Prcb;
};
#endif

//
// Is the current processor executing a DPC (either a threaded DPC or a
// legacy DPC).
//
#if defined(_M_IX86)

NTL__EXTERNAPI
bool __stdcall
KeIsExecutingDpc();

#elif defined(_M_X64)

__forceinline
bool KeIsExecutingDpc()
{
  const kpcr& pcr = kpcr::instance();
  return pcr.Prcb.DpcRoutineActive || pcr.Prcb.DpcThreadActive;
}

#endif


/// Common KTHREAD region
struct kthread32
{
  /* 0x00 */  dispatcher_header Header;
  /* 0x10 */  list_entry        MutantListHead;
  /* 0x18 */  void *            InitialStack;
  /* 0x1c */  void *            StackLimit;
  /* 0x20 */  nt::tib *         Teb;
  /* 0x24 */  void *            TlsArray;
  /* 0x28 */  void *            KernelStack;
  /* 0x2c */  bool              DebugActive;
  /* 0x2d */  uint8_t           State;
  /* 0x2e */  bool              Alerted[2/*UserMode+1*/];
  /* 0x30 */  uint8_t           Iopl;
  /* 0x31 */  uint8_t           NpxState;
  /* 0x32 */  int8_t              Saturation;
  /* 0x33 */  int8_t            Priority;
  /* 0x34 */  kapc_state        ApcState;
  /* 0x4c */  uint32_t          ContextSwitches;

  static 
  kthread * get_current()
  {
#ifndef NTL_SUPPRESS_IMPORT
    return KeGetCurrentThread();
#else
    return *reinterpret_cast<kthread**>(/*__readfsdword(0x124)*/0xFFDFF124);    
#endif
  }

};
//STATIC_ASSERT(sizeof(kthread) == 0x50);



/// 2K
struct kthread50 : kthread32
{
  /* 0x54 */  intptr_t          WaitStatus;
  /* 0x58 */  kirql             WaitIrql;
  /* 0x59 */  kprocessor_mode   WaitMode;
  /* 0x5a */  bool              WaitNext;
  /* 0x5b */  uint8_t     WaitReason;
  /* 0x5c */  kwait_block *     WaitBlockList;
  /* 0x60 */  list_entry        WaitListEntry;
  /* 0x60 */  single_list_entry SwapListEntry;
  /* 0x68 */  uint32_t          WaitTime;
  /* 0x6c */  int8_t            BasePriority;
  /* 0x6d */  uint8_t           DecrementCount;
  /* 0x6e */  int8_t            PriorityDecrement;
  /* 0x6f */  int8_t            Quantum;
  /* 0x70 */  kwait_block       WaitBlock[4];
  /* 0xd0 */  void *            LegoData;
  /* 0xd4 */  uint32_t     KernelApcDisable;
  /* 0xd8 */  kaffinity         UserAffinity;
  /* 0xdc */  bool              SystemAffinityActive;
  /* 0xdd */  int8_t            PowerState;
  /* 0xde */  kirql             NpxIrql;
  /* 0xdf */  int8_t            InitialNode;
  /* 0xe0 */  void *            ServiceTable;
  /* 0xe4 */  kqueue *          Queue;
  /* 0xe8 */  kspin_lock        ApcQueueLock;
  /* 0xf0 */  ktimer            Timer;
  /* 0x118 */ list_entry        QueueListEntry;
  /* 0x124 */ kaffinity         Affinity;
  /* 0x128 */ bool              Preempted;
  /* 0x129 */ bool              ProcessReadyQueue;
  /* 0x12a */ bool              KernelStackResident;
  /* 0x12b */ int8_t            NextProcessor;
  /* 0x12c */ void *            CallbackStack;
  /* 0x130 */ void *            Win32Thread;
  /* 0x134 */ ktrap_frame *     TrapFrame;
  /* 0x138 */ kapc_state *      ApcStatePointer[2];
  /* 0x140 */ kprocessor_mode   PreviousMode;
  /* 0x141 */ bool              EnableStackSwap;
  /* 0x142 */ bool              LargeStack;
  /* 0x143 */ uint8_t     ResourceIndex;
  /* 0x144 */ uint32_t          KernelTime;
  /* 0x148 */ uint32_t          UserTime;
  /* 0x14c */ kapc_state        SavedApcState;
  /* 0x164 */ bool              Alertable;
  /* 0x165 */ uint8_t           ApcStateIndex;
  /* 0x166 */ bool              ApcQueueable;
  /* 0x167 */ bool              AutoAlignment;
  /* 0x168 */ void *            StackBase;
  /* 0x16c */ kapc              SuspendApc;
  /* 0x19c */ ksemaphore        SuspendSemaphore;
  /* 0x1b0 */ list_entry        ThreadListEntry;
  /* 0x1b8 */ int8_t              FreezeCount;
  /* 0x1b9 */ int8_t              SuspendCount;
  /* 0x1ba */ uint8_t           IdealProcessor;
  /* 0x1bb */ bool              DisableBoost;
}; // struct ktread50
//STATIC_ASSERT(sizeof(kthread50) == 0x1b8);


/// XP SP2
struct kthread51 : kthread32
{
  /* 0x50 */  uint8_t     IdleSwapBlock;  /// new to XP
  /* 0x51 */  uint8_t     Spare0[3];      /// new to XP
  /* 0x54 */  intptr_t          WaitStatus;
  /* 0x58 */  kirql             WaitIrql;
  /* 0x59 */  kprocessor_mode   WaitMode;
  /* 0x5a */  bool              WaitNext;
  /* 0x5b */  uint8_t     WaitReason;
  /* 0x5c */  kwait_block *     WaitBlockList;
  /* 0x60 */  list_entry        WaitListEntry;
  /* 0x60 */  single_list_entry SwapListEntry;
  /* 0x68 */  uint32_t          WaitTime;
  /* 0x6c */  int8_t            BasePriority;
  /* 0x6d */  uint8_t           DecrementCount;
  /* 0x6e */  int8_t            PriorityDecrement;
  /* 0x6f */  int8_t            Quantum;
  /* 0x70 */  kwait_block       WaitBlock[4];
  /* 0xd0 */  void *            LegoData;
  /* 0xd4 */  uint32_t     KernelApcDisable;
  /* 0xd8 */  kaffinity         UserAffinity;
  /* 0xdc */  bool              SystemAffinityActive;
  /* 0xdd */  int8_t            PowerState;
  /* 0xde */  kirql             NpxIrql;
  /* 0xdf */  int8_t            InitialNode;
  /* 0xe0 */  void *            ServiceTable;
  /* 0xe4 */  kqueue *          Queue;
  /* 0xe8 */  kspin_lock        ApcQueueLock;
  /* 0xf0 */  ktimer            Timer;
  /* 0x118 */ list_entry        QueueListEntry;
  /* 0x120 */ kaffinity         SoftAffinity; /// new to XP
  /* 0x124 */ kaffinity         Affinity;
  /* 0x128 */ bool              Preempted;
  /* 0x129 */ bool              ProcessReadyQueue;
  /* 0x12a */ bool              KernelStackResident;
  /* 0x12b */ int8_t            NextProcessor;
  /* 0x12c */ void *            CallbackStack;
  /* 0x130 */ void *            Win32Thread;
  /* 0x134 */ ktrap_frame *     TrapFrame;
  /* 0x138 */ kapc_state *      ApcStatePointer[2];
  /* 0x140 */ kprocessor_mode   PreviousMode;
  /* 0x141 */ bool              EnableStackSwap;
  /* 0x142 */ bool              LargeStack;
  /* 0x143 */ uint8_t     ResourceIndex;
  /* 0x144 */ uint32_t          KernelTime;
  /* 0x148 */ uint32_t          UserTime;
  /* 0x14c */ kapc_state        SavedApcState;
  /* 0x164 */ bool              Alertable;
  /* 0x165 */ uint8_t           ApcStateIndex;
  /* 0x166 */ bool              ApcQueueable;
  /* 0x167 */ bool              AutoAlignment;
  /* 0x168 */ void *            StackBase;
  /* 0x16c */ kapc              SuspendApc;
  /* 0x19c */ ksemaphore        SuspendSemaphore;
  /* 0x1b0 */ list_entry        ThreadListEntry;
  /* 0x1b8 */ int8_t              FreezeCount;
  /* 0x1b9 */ int8_t              SuspendCount;
  /* 0x1ba */ uint8_t           IdealProcessor;
  /* 0x1bb */ bool              DisableBoost;
}; // struct ktread51
//STATIC_ASSERT(sizeof(kthread51) == 0x1c0);

/// Common KTHREAD region (x64)
struct kthread64 {
	/*<thisrel this+0x0>*/ /*|0x18|*/ dispatcher_header Header;
	/*<thisrel this+0x18>*/ /*|0x10|*/ list_entry MutantListHead;
	/*<thisrel this+0x28>*/ /*|0x8|*/ void* InitialStack;
	/*<thisrel this+0x30>*/ /*|0x8|*/ void* StackLimit;
	/*<thisrel this+0x38>*/ /*|0x8|*/ void* KernelStack;
	/*<thisrel this+0x40>*/ /*|0x8|*/ uint64_t ThreadLock;
	union {
	/*<thisrel this+0x48>*/ /*|0x30|*/ kapc_state ApcState;
		struct {
	/*<thisrel this+0x48>*/ /*|0x2b|*/ uint8_t ApcStateFill[43];
	/*<thisrel this+0x73>*/ /*|0x1|*/ bool ApcQueueable;
	/*<thisrel this+0x74>*/ /*|0x1|*/ uint8_t NextProcessor;
	/*<thisrel this+0x75>*/ /*|0x1|*/ uint8_t DeferredProcessor;
	/*<thisrel this+0x76>*/ /*|0x1|*/ uint8_t AdjustReason;
	/*<thisrel this+0x77>*/ /*|0x1|*/ int8_t AdjustIncrement;
		};
	};
	/*<thisrel this+0x78>*/ /*|0x8|*/ uint64_t ApcQueueLock;
	/*<thisrel this+0x80>*/ /*|0x8|*/ int64_t WaitStatus;
	union {
	/*<thisrel this+0x88>*/ /*|0x8|*/ kwait_block* WaitBlockList;
	/*<thisrel this+0x88>*/ /*|0x8|*/ kgate* GateObject;
	};
	/*<thisrel this+0x90>*/ /*|0x1|*/ bool Alertable;
	/*<thisrel this+0x91>*/ /*|0x1|*/ bool WaitNext;
	/*<thisrel this+0x92>*/ /*|0x1|*/ uint8_t WaitReason;
	/*<thisrel this+0x93>*/ /*|0x1|*/ int8_t Priority;
	/*<thisrel this+0x94>*/ /*|0x1|*/ uint8_t EnableStackSwap;
	/*<thisrel this+0x95>*/ /*|0x1|*/ uint8_t SwapBusy;
	/*<thisrel this+0x96>*/ /*|0x2|*/ bool Alerted[2];
	union {
	/*<thisrel this+0x98>*/ /*|0x10|*/ list_entry WaitListEntry;
	/*<thisrel this+0x98>*/ /*|0x8|*/ single_list_entry SwapListEntry;
	};
	/*<thisrel this+0xa8>*/ /*|0x8|*/ kqueue* Queue;
	/*<thisrel this+0xb0>*/ /*|0x8|*/ void* Teb;
	union {
	/*<thisrel this+0xb8>*/ /*|0x40|*/ ktimer Timer;
		struct {
	/*<thisrel this+0xb8>*/ /*|0x3c|*/ uint8_t TimerFill[60];
			union {
				struct {
	/*<bitfield this+0xf4>*/ /*|0x4|*/ uint32_t AutoAlignment:1;
	/*<bitfield this+0xf4>*/ /*|0x4|*/ uint32_t DisableBoost:1;
	/*<bitfield this+0xf4>*/ /*|0x4|*/ uint32_t GuiThread:1;
	/*<bitfield this+0xf4>*/ /*|0x4|*/ uint32_t ReservedFlags:0x1d;
				};
	/*<thisrel this+0xf4>*/ /*|0x4|*/ int32_t ThreadFlags;
			};
		};
	};
	union {
	/*<thisrel this+0xf8>*/ /*|0xc0|*/ kwait_block WaitBlock[4];
		struct {
	/*<thisrel this+0xf8>*/ /*|0x2b|*/ uint8_t WaitBlockFill0[43];
	/*<thisrel this+0x123>*/ /*|0x1|*/ uint8_t SystemAffinityActive;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0x5b|*/ uint8_t WaitBlockFill1[91];
	/*<thisrel this+0x153>*/ /*|0x1|*/ int8_t PreviousMode;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0x8b|*/ uint8_t WaitBlockFill2[139];
	/*<thisrel this+0x183>*/ /*|0x1|*/ uint8_t ResourceIndex;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0xbb|*/ uint8_t WaitBlockFill3[187];
	/*<thisrel this+0x1b3>*/ /*|0x1|*/ uint8_t LargeStack;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0x2c|*/ uint8_t WaitBlockFill4[44];
	/*<thisrel this+0x124>*/ /*|0x4|*/ uint32_t ContextSwitches;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0x5c|*/ uint8_t WaitBlockFill5[92];
	/*<thisrel this+0x154>*/ /*|0x1|*/ uint8_t State;
	/*<thisrel this+0x155>*/ /*|0x1|*/ uint8_t NpxState;
	/*<thisrel this+0x156>*/ /*|0x1|*/ uint8_t WaitIrql;
	/*<thisrel this+0x157>*/ /*|0x1|*/ int8_t WaitMode;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0x8c|*/ uint8_t WaitBlockFill6[140];
	/*<thisrel this+0x184>*/ /*|0x4|*/ uint32_t WaitTime;
		};
		struct {
	/*<thisrel this+0xf8>*/ /*|0xbc|*/ uint8_t WaitBlockFill7[188];
			union {
				struct {
	/*<thisrel this+0x1b4>*/ /*|0x2|*/ int16_t KernelApcDisable;
	/*<thisrel this+0x1b6>*/ /*|0x2|*/ int16_t SpecialApcDisable;
				};
	/*<thisrel this+0x1b4>*/ /*|0x4|*/ uint32_t CombinedApcDisable;
			};
		};
	};
	/*<thisrel this+0x1b8>*/ /*|0x10|*/ list_entry QueueListEntry;
	/*<thisrel this+0x1c8>*/ /*|0x8|*/ ktrap_frame* TrapFrame;
	/*<thisrel this+0x1d0>*/ /*|0x8|*/ void* CallbackStack;
	// void* ServiceTable
	// uint32_t KernelLimit
	/*<thisrel this+0x1d8>*/ /*|0x1|*/ uint8_t ApcStateIndex;
	/*<thisrel this+0x1d9>*/ /*|0x1|*/ uint8_t IdealProcessor;
	/*<thisrel this+0x1da>*/ /*|0x1|*/ bool Preempted;
	/*<thisrel this+0x1db>*/ /*|0x1|*/ bool ProcessReadyQueue;
	/*<thisrel this+0x1dc>*/ /*|0x1|*/ bool KernelStackResident;
	/*<thisrel this+0x1dd>*/ /*|0x1|*/ int8_t BasePriority;
	/*<thisrel this+0x1de>*/ /*|0x1|*/ int8_t PriorityDecrement;
	/*<thisrel this+0x1df>*/ /*|0x1|*/ int8_t Saturation;
	/*<thisrel this+0x1e0>*/ /*|0x8|*/ uint64_t UserAffinity;
	/*<thisrel this+0x1e8>*/ /*|0x8|*/ kprocess* Process;
	/*<thisrel this+0x1f0>*/ /*|0x8|*/ uint64_t Affinity;
	/*<thisrel this+0x1f8>*/ /*|0x10|*/ kapc_state* ApcStatePointer[2];
	union {
	/*<thisrel this+0x208>*/ /*|0x30|*/ kapc_state SavedApcState;
		struct {
	/*<thisrel this+0x208>*/ /*|0x2b|*/ uint8_t SavedApcStateFill[43];
	/*<thisrel this+0x233>*/ /*|0x1|*/ int8_t FreezeCount;
	/*<thisrel this+0x234>*/ /*|0x1|*/ int8_t SuspendCount;
	/*<thisrel this+0x235>*/ /*|0x1|*/ uint8_t UserIdealProcessor;
	/*<thisrel this+0x236>*/ /*|0x1|*/ uint8_t CalloutActive;
	/*<thisrel this+0x237>*/ /*|0x1|*/ uint8_t CodePatchInProgress;
		};
	};
	/*<thisrel this+0x238>*/ /*|0x8|*/ void* Win32Thread;
	/*<thisrel this+0x240>*/ /*|0x8|*/ void* StackBase;
	union {
	/*<thisrel this+0x248>*/ /*|0x58|*/ kapc SuspendApc;
		struct {
	/*<thisrel this+0x248>*/ /*|0x1|*/ uint8_t SuspendApcFill0[1];
	/*<thisrel this+0x249>*/ /*|0x1|*/ int8_t Quantum;
		};
		struct {
	/*<thisrel this+0x248>*/ /*|0x3|*/ uint8_t SuspendApcFill1[3];
	/*<thisrel this+0x24b>*/ /*|0x1|*/ uint8_t QuantumReset;
		};
		struct {
	/*<thisrel this+0x248>*/ /*|0x4|*/ uint8_t SuspendApcFill2[4];
	/*<thisrel this+0x24c>*/ /*|0x4|*/ uint32_t KernelTime;
		};
		struct {
	/*<thisrel this+0x248>*/ /*|0x40|*/ uint8_t SuspendApcFill3[64];
	/*<thisrel this+0x288>*/ /*|0x8|*/ void* TlsArray;
		};
		struct {
	/*<thisrel this+0x248>*/ /*|0x48|*/ uint8_t SuspendApcFill4[72];
	/*<thisrel this+0x290>*/ /*|0x8|*/ void* LegoData;
		};
		struct {
	/*<thisrel this+0x248>*/ /*|0x53|*/ uint8_t SuspendApcFill5[83];
	/*<thisrel this+0x29b>*/ /*|0x1|*/ uint8_t PowerState;
	/*<thisrel this+0x29c>*/ /*|0x4|*/ uint32_t UserTime;
		};
	};
	union {
	/*<thisrel this+0x2a0>*/ /*|0x20|*/ ksemaphore SuspendSemaphore;
		struct {
	/*<thisrel this+0x2a0>*/ /*|0x1c|*/ uint8_t SuspendSemaphorefill[28];
	/*<thisrel this+0x2bc>*/ /*|0x4|*/ uint32_t SListFaultCount;
		};
	};
	/*<thisrel this+0x2c0>*/ /*|0x10|*/ list_entry ThreadListEntry;
	/*<thisrel this+0x2d0>*/ /*|0x8|*/ void* SListFaultAddress;
	/*<thisrel this+0x2d8>*/ /*|0x8|*/ int64_t ReadOperationCount;
	/*<thisrel this+0x2e0>*/ /*|0x8|*/ int64_t WriteOperationCount;
	/*<thisrel this+0x2e8>*/ /*|0x8|*/ int64_t OtherOperationCount;
	/*<thisrel this+0x2f0>*/ /*|0x8|*/ int64_t ReadTransferCount;
	/*<thisrel this+0x2f8>*/ /*|0x8|*/ int64_t WriteTransferCount;
	/*<thisrel this+0x300>*/ /*|0x8|*/ int64_t OtherTransferCount;
};

typedef
void __stdcall
  kstart_routine_t(
    void * StartContext
    );


class system_tread;

}//namspace km


template<>
struct device_traits<km::system_tread> : private device_traits<>
{
  enum access_mask
  {
    terminate                 = 0x0001,
    suspend_resume            = 0x0002,
    alert                     = 0x0004,
    get_context               = 0x0008,
    set_context               = 0x0010,
    set_information           = 0x0020,
    set_limited_information   = 0x0400,
    query_limited_information = 0x0800,
  #if 0//(NTDDI_VERSION >= NTDDI_LONGHORN)
    all_access                = standard_rights_required | synchronize | 0xFFFF,
  #else                                   
    all_access                = standard_rights_required | synchronize | 0x3FF,
  #endif
  };
};


namespace km {

NTL__EXTERNAPI
ntstatus __stdcall
  PsCreateSystemThread(
    handle *            ThreadHandle,
    uint32_t            DesiredAccess,
    object_attributes * ObjectAttributes,
    legacy_handle       ProcessHandle,
    client_id *         ClientId,
    kstart_routine_t *  StartRoutine,
    void *              StartContext
    );

NTL__EXTERNAPI
ntstatus __stdcall
  PsTerminateSystemThread(
    ntstatus  ExitStatus
    );


class system_tread : public handle, public device_traits<system_tread>
{
  ////////////////////////////////////////////////////////////////////////////
  public:

    typedef kstart_routine_t start_routine_t;

    explicit
    system_tread(
        start_routine_t *   start_routine,
        void *              start_context   = 0,
        object_attributes * oa              = 0,
        access_mask         desired_access  = all_access,
        legacy_handle       process_handle  = 0,
        client_id *         client          = 0)
    {
      create(this, start_routine, start_context, oa, desired_access,
              process_handle, client);
    }
      
    static
    ntstatus
      create(
        handle *            thread_handle,
        start_routine_t *   start_routine,
        void *              start_context   = 0,
        object_attributes * oa              = 0,
        access_mask         desired_access  = all_access,
        legacy_handle       process_handle  = 0,
        client_id *         client          = 0)
    {
      return PsCreateSystemThread(thread_handle, desired_access, oa, process_handle,
                                  client, start_routine, start_context);
    }

    static
    void exit(ntstatus st)
    {
      PsTerminateSystemThread(st);
    }

};//


}//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_THREAD
