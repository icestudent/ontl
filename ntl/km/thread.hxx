/**\file*********************************************************************
 *                                                                     \brief
 *  Kernel Threads
 *
 ****************************************************************************
 */

#ifndef NTL__KM_THREAD
#define NTL__KM_THREAD

#include "basedef.hxx"
#include "apc.hxx"
#include "handle.hxx"
#include "../nt/teb.hxx"
#include "../device_traits.hxx"

namespace ntl {
namespace km {

using nt::client_id;

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


struct kaffinity
{
  kaffinity() {}
  kaffinity(uintptr_t affinity) : affinity(affinity) { }
  uintptr_t affinity;
};


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
