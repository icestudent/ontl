/**\file*********************************************************************
 *                                                                     \brief
 *  I/O Manager
 *
 ****************************************************************************
 */

#ifndef NTL__KM_IO
#define NTL__KM_IO

#include "basedef.hxx"
#include "device_object.hxx"

namespace ntl {
namespace km {

  struct kinterrupt;
  struct io_connect_interrupt_parameters;

  typedef void(kinterrupt_routine_t)();
  typedef bool (__stdcall kservice_routine_t) (kinterrupt* Interrupt, void* ServiceContext);

  namespace interrupt_mode
  {
    enum mode {
      LevelSensitive,
      Latched
    };
  }
  typedef interrupt_mode::mode kinterrupt_mode;

  // imports
  NTL__EXTERNAPI
  ntstatus __stdcall
  IoConnectInterrupt(
    kinterrupt** InterruptObject,
    kservice_routine_t* ServiceRoutine,
    void* ServiceContext,
    kspin_lock* SpinLock,
    uint32_t Vector,
    kirql_t  Irql,
    kirql_t  SynchronizeIrql,
    kinterrupt_mode InterruptMode,
    bool ShareVector,
    kaffinity ProcessorEnableMask,
    bool FloatingSave
    );

  NTL__EXTERNAPI void __stdcall IoDisconnectInterrupt(kinterrupt* InterruptObject);

  NTL__EXTERNAPI ntstatus __stdcall IoConnectInterruptEx(io_connect_interrupt_parameters* Parameters);

  NTL__EXTERNAPI void __stdcall IoDisconnectInterruptEx(io_connect_interrupt_parameters* Parameters);


  // types
  struct kinterrupt
  {
    int16_t Type;
    int16_t Size;
    list_entry InterruptListEntry;
    kservice_routine_t* ServiceRoutine;
    void* ServiceContext;
    kspin_lock SpinLock;
    uint32_t TickCount;
    kspin_lock* ActualLock;
    kinterrupt_routine_t* DispatchAddress;
    uint32_t Vector;
    kirql Irql;
    kirql SynchronizeIrql;
    bool FloatingSave;
    bool Connected;
    int8_t Number;
    bool ShareVector;
    kinterrupt_mode Mode;
    uint32_t ServiceCount;
    uint32_t DispatchCount;

  #ifdef _M_X64
    ktrap_frame* TrapFrame;
    void* Reserved;

    static const size_t dispatch_length = 4;
  #else
    static const size_t dispatch_length = 106;
  #endif
    uint32_t DispatchCode[dispatch_length];
  };

#if 0
  struct io_connect_interrupt_parameters
  {
    ULONG  Version;    // Max Version == 3
    union {

      // LINE_BASED Version
      struct {
        device_object*  PhysicalDeviceObject;
        kinterrupt*  *InterruptObject;
        kservice_routine_t*  ServiceRoutine;
        void*  ServiceContext;
        lspin_lock*  SpinLock;
        kirql_t  SynchronizeIrql;
        bool  FloatingSave;
      } LineBased;

      // MESSAGED_BASED Version
      struct {
        device_object*              PhysicalDeviceObject;
        union {
          void*                       *Generic;
          PIO_INTERRUPT_MESSAGE_INFO  *InterruptMessageTable;
          kinterrupt*                 *InterruptObject;
        } ConnectionContext;
        PKMESSAGE_SERVICE_ROUTINE   MessageServiceRoutine;
        void*                       ServiceContext;
        lspin_lock*                 SpinLock;
        kirql_t                       SynchronizeIrql;
        bool                     FloatingSave;
        kservice_routine_t*           FallBackServiceRoutine OPTIONAL;
      } MessageBased;

      // FULLY_SPECIFIED Version
      struct {
        device_object*  PhysicalDeviceObject;
        kinterrupt*  *InterruptObject;
        kservice_routine_t*  ServiceRoutine;
        void*  ServiceContext;
        lspin_lock*  SpinLock;
        kirql_t  SynchronizeIrql;
        bool  FloatingSave;
        bool  ShareVector;
        uint32_t  Vector;
        kirql_t  Irql;
        kinterrupt_mode  InterruptMode;
        kaffinity  ProcessorEnableMask;
      } FullySpecified;
    };
  };
#endif

 } // km
}// ntl
#endif // NTL__KM_IO
