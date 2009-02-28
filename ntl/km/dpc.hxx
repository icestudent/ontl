/**\file*********************************************************************
 *                                                                     \brief
 *  Deferred Procedure Calls
 *
 ****************************************************************************
 */


#ifndef NTL__KM_DPC
#define NTL__KM_DPC

#include "basedef.hxx"

namespace ntl {
namespace km {

  struct kdpc_data
  {
    list_entry DpcListHead;
    kspin_lock DpcLock;
    volatile int32_t DpcQueueDepth;
    uint32_t DpcCount;
  };

  namespace __ 
  {
    struct kdpc_types
    {
      enum type
      {
        Normal,
        Threaded
      };

      enum DpcImportance {
        LowImportance,
        MediumImportance,
        HighImportance
      };

      typedef void (__stdcall deferred_routine_t)(
        const kdpc* Dpc,
        void* DeferredContext,
        const void* SystemArgument1,
        const void* SystemArgument2);
    };

    struct kdpc_head32
    {
      uint8_t Type;
      uint8_t Number;
      uint8_t Importance;
    };

    struct kdpc_head64
    {
      // all x64 and only 2k3sp1 x86
      uint8_t Type;
      uint8_t Importance;
      uint8_t Number;
      uint8_t Expedite;
    };
  }

  struct kdpc32: __::kdpc_types, __::kdpc_head32
  {
    list_entry DpcListEntry;

    deferred_routine_t* DeferredRoutine;
    void* DeferredContext;
    void* SystemArgument1;
    void* SystemArgument2;

    kdpc_data* DpcData;
  };

  struct kdpc64: __::kdpc_types, __::kdpc_head64
  {
    list_entry DpcListEntry;

    deferred_routine_t* DeferredRoutine;
    void* DeferredContext;
    void* SystemArgument1;
    void* SystemArgument2;

    kdpc_data* DpcData;
  };

#ifndef _M_X64
  STATIC_ASSERT(sizeof(kdpc) == 0x20);
#else
  STATIC_ASSERT(sizeof(kdpc) == 0x40);
#endif

  /**
  	KeAcquireInStackQueuedSpinLockAtDpcLevel
    KeAcquireInStackQueuedSpinLockForDpc
    KeAcquireSpinLockAtDpcLevel
    KeAcquireSpinLockForDpc
    KeAcquireSpinLockRaiseToDpc
    KeFlushQueuedDpcs
    KeGenericCallDpc
    KeInitializeDpc
    KeInitializeThreadedDpc
    KeInsertQueueDpc
    KeRaiseIrqlToDpcLevel
    KeReleaseInStackQueuedSpinLockForDpc
    KeReleaseInStackQueuedSpinLockFromDpcLevel
    KeReleaseSpinLockForDpc
    KeReleaseSpinLockFromDpcLevel
    KeRemoveQueueDpc
    KeSetImportanceDpc
    KeSetTargetProcessorDpc
    KeSignalCallDpcDone
    KeSignalCallDpcSynchronize
    KeTryToAcquireSpinLockAtDpcLevel
   **/

  NTL__EXTERNAPI
  void __stdcall
  KeInitializeDpc(
      kdpc* Dpc,
      kdpc::deferred_routine_t* DeferredRoutine,
      void* DeferredContext
      );

  NTL__EXTERNAPI
  void __stdcall
  KeInitializeThreadedDpc(
      kdpc* Dpc,
      kdpc::deferred_routine_t* DeferredRoutine,
      void* DeferredContext
      );

  NTL__EXTERNAPI
  bool __stdcall
  KeInsertQueueDpc (
    kdpc* Dpc,
    void* SystemArgument1,
    void* SystemArgument2
    );

  NTL__EXTERNAPI
  bool __stdcall
  KeRemoveQueueDpc (
    kdpc* Dpc
    );

  NTL__EXTERNAPI
  void __stdcall
  KeSetImportanceDpc (
    kdpc* Dpc,
    kdpc::DpcImportance Importance
    );

  NTL__EXTERNAPI
  void __stdcall
    KeSetTargetProcessorDpc (
    kdpc*  Dpc,
    int8_t Number
    );

  NTL__EXTERNAPI
  void __stdcall KeFlushQueuedDpcs();

  NTL__EXTERNAPI
  void __stdcall
  KeGenericCallDpc (
    kdpc::deferred_routine_t* Routine,
    void* Context
    );

  NTL__EXTERNAPI
  void __stdcall KeSignalCallDpcDone(void* SystemArgument1);

  NTL__EXTERNAPI
  uint32_t __stdcall KeSignalCallDpcSynchronize(void* SystemArgument2);


  struct kdpc_instance: kdpc
  {
    inline explicit kdpc_instance(kdpc::deferred_routine_t* DeferredRoutine, void* DeferredContext = nullptr)
    {
      KeInitializeDpc(this, DeferredRoutine, DeferredContext);
    }

    inline explicit kdpc_instance(kdpc::deferred_routine_t* DeferredRoutine, void* DeferredContext, kdpc::DpcImportance Importance, int8_t TargetProcessor = 0)
    {
      KeInitializeDpc(this, DeferredRoutine, DeferredContext);
      KeSetImportanceDpc(this, Importance);
      KeSetTargetProcessorDpc(this, TargetProcessor);
    }

    inline bool insert_queue(void* SystemArgument1, void* SystemArgument2)
    {
      return KeInsertQueueDpc(this, SystemArgument1, SystemArgument2);
    }

    inline bool remove_queue()
    {
      return KeRemoveQueueDpc(this);
    }

    static inline void flush_queued()
    {
      KeFlushQueuedDpcs();
    }

    inline void importance(kdpc::DpcImportance Importance)
    {
      KeSetImportanceDpc(this, Importance);
    }

    inline void target_processor(int8_t number)
    {
      KeSetTargetProcessorDpc(this, number);
    }

    static inline void signal_done(void* SystemArgument1)
    {
      KeSignalCallDpcDone(SystemArgument1);
    }

    static inline uint32_t signal_synchronize(void* SystemArgument2)
    {
      KeSignalCallDpcSynchronize(SystemArgument2);
    }

  private:
    kdpc_instance(const kdpc_instance&);
    kdpc_instance& operator=(const kdpc_instance&);
  };



  } // km
} // ntl

#endif // NTL__KM_DPC