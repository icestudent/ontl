/**\file*********************************************************************
*                                                                     \brief
*  I/O request packet
*
****************************************************************************
*/

#ifndef NTL__KM_IRP
#define NTL__KM_IRP

#include "basedef.hxx"
#include "apc.hxx"
#include "object.hxx"
#include "file_information.hxx"

namespace ntl {
  namespace km {

    struct driver_object;
    struct device_object;
    struct file_object;
    struct ethread;
    struct irp;


    typedef
      ntstatus __stdcall
      io_completion_routine_t(
        device_object * DeviceObject,
        irp *           Irp,
        void *          Context
        );

    typedef
      void __stdcall
      driver_cancel_t(
        device_object * DeviceObject,
        irp *           Irp
        );

    typedef
      ntstatus __stdcall
      driver_dispatch_t (
        device_object *DeviceObject,
        irp *Irp
        );


    typedef
      void __stdcall
      driver_startio_t (
        device_object * DeviceObject,
        irp *Irp
        );

    typedef
      void __stdcall
      driver_unload_t (
        driver_object *DriverObject
        );

    typedef
      ntstatus __stdcall
      driver_add_device_t (
        driver_object *DriverObject,
        device_object *PhysicalDeviceObject
        );

#if defined(_M_IX86)
#   pragma pack(push, 4)
#elif defined(_M_X64)
#   pragma pack(push, 8)
#endif

    struct io_stack_location
    {
      enum control_flags
      {
        pending_returned  = 0x01,
        error_returned    = 0x02,
        invoke_on_cancel  = 0x20,
        invoke_on_success = 0x40,
        invoke_on_error   = 0x80
      };

      friend
        control_flags operator | (control_flags m, control_flags m2)
      {
        return bitwise_or(m, m2);
      }

      uint8_t                   MajorFunction;
      uint8_t                   MinorFunction;
      uint8_t                   Flags;
      uint8_t                   Control;

      union
      {
#if defined(_M_IX86)
        uint8_t _[0x10];
#elif defined(_M_X64)
        __declspec(align(8))
          uint8_t _[0x20];
#endif

        struct
        {
          struct IO_SECURITY_CONTEXT *  SecurityContext;
          uint32_t  Options     : 24;
          uint32_t  Disposition : 8;
          uint16_t  FileAttributes;
          uint16_t  ShareAccess;
          uint32_t  EaLength;
        } Create;

        struct
        {
          uint32_t  Length;
          uint32_t  Key;
          int64_t   ByteOffset;
        } Read;

        struct
        {
          uint32_t  Length;
          uint32_t  Key;
          int64_t   ByteOffset;
        } Write;

        struct
        {
          uint32_t                Length;
          file_information_class  FileInformationClass;
        } QueryFile;

        struct
        {
          uint32_t                Length;
          file_information_class  FileInformationClass;
          file_object *           FileObject;
          union {
            struct _ {
              bool  ReplaceIfExists;
              bool  AdvanceOnly;
            };
            uint32_t ClusterCount;
            legacy_handle DeleteHandle;
          };
        } SetFile;

#if 0
        struct
        {
          uint32_t                Length;
          fs_information_class    FsInformationClass;
        } QueryVolume;
#endif

        struct
        {
          uint32_t  OutputBufferLength;
          uint32_t  InputBufferLength;  // POINTER_ALIGNMENT
          uint32_t  IoControlCode;  // POINTER_ALIGNMENT
          void *    Type3InputBuffer;
        } DeviceIoControl;

        struct
        {
          void *  Argument1;
          void *  Argument2;
          void *  Argument3;
          void *  Argument4;
        } Others;
      }                         Parameters;

      device_object *           DeviceObject;
      file_object *             FileObject;
      io_completion_routine_t * CompletionRoutine;
      void *                    Context;
    };
#pragma pack(pop)
    //STATIC_ASSERT(sizeof(io_stack_location) == 0x24 || sizeof(io_stack_location) == 0x44);

    const io_stack_location::control_flags sl_invoke_on_cancel = io_stack_location::invoke_on_cancel;
    const io_stack_location::control_flags sl_invoke_on_success = io_stack_location::invoke_on_success;
    const io_stack_location::control_flags sl_invoke_on_error = io_stack_location::invoke_on_error;


    struct irp
    {
      enum major_function
      {
        mj_create,                    ///< 0x00
        mj_create_named_pipe,         ///< 0x01
        mj_close,                     ///< 0x02
        mj_read,                      ///< 0x03
        mj_write,                     ///< 0x04
        mj_query_information,         ///< 0x05
        mj_set_information,           ///< 0x06
        mj_query_ea,                  ///< 0x07
        mj_set_ea,                    ///< 0x08
        mj_flush_buffers,             ///< 0x09
        mj_query_volume_information,  ///< 0x0A
        mj_set_volume_information,    ///< 0x0B
        mj_directory_control,         ///< 0x0C
        mj_file_system_control,       ///< 0x0D
        mj_device_control,            ///< 0x0E
        mj_internal_device_control,   ///< 0x0F
        mj_shutdown,                  ///< 0x10
        mj_lock_control,              ///< 0x11
        mj_cleanup,                   ///< 0x12
        mj_create_mailslot,           ///< 0x13
        mj_query_security,            ///< 0x14
        mj_set_security,              ///< 0x15
        mj_power,                     ///< 0x16
        mj_system_control,            ///< 0x17
        mj_device_change,             ///< 0x18
        mj_query_quota,               ///< 0x19
        mj_set_quota,                 ///< 0x1A
        mj_pnp,                       ///< 0x1B
        //    mj_pnp_power = mj_pnp,
        mj_maximum_function = mj_pnp
      };
      STATIC_ASSERT(mj_maximum_function == 0x1b);

      uint16_t          Type;
      uint16_t          Size;
      mdl *             MdlAddress;
      uint32_t          Flags;
      union
      {
        irp *   MasterIrp;
        long    IrpCount;
        void *  SystemBuffer;
      }                 AssociatedIrp;
      list_entry        ThreadListEntry;
      io_status_block   IoStatus;
      kprocessor_mode   RequestorMode;
      bool              PendingReturned;
      int8_t            StackCount;
      int8_t            CurrentLocation;
      bool              Cancel;
      kirql             CancelIrql;
      uint8_t           ApcEnvironment;
      uint8_t           AllocationFlags;
      io_status_block * UserIosb;
      kevent *          UserEvent;
      union
      {
        struct
        {
          union { io_apc_routine * UserApcRoutine; void * IssuingProcess; };
          void *  UserApcContext;
        }       AsynchronousParameters;
        int64_t AllocationSize;
      }         Overlay;
      driver_cancel_t* CancelRoutine;
      void *            UserBuffer;
      union
      {
        struct
        {
          union
          {
            kdevice_queue_entry DeviceQueueEntry;
            void *              DriverContext[4];
          } ;
          ethread * Thread;
          char *    AuxiliaryBuffer;
          list_entry ListEntry;
          union
          {
            io_stack_location * CurrentStackLocation;
            uint32_t  PacketType;
          };
          file_object *   OriginalFileObject;
        } Overlay;
        kapc    Apc;
        void *  CompletionKey;
      } Tail;

      io_stack_location * get_current_stack_location() const
      {
        return Tail.Overlay.CurrentStackLocation;
      }

      io_stack_location * get_next_stack_location() const
      {
        return Tail.Overlay.CurrentStackLocation - 1;
      }

      void skip_current_stack_location()
      {
        ++CurrentLocation;
        ++Tail.Overlay.CurrentStackLocation;
      }

      void set_next_stack_location()
      {
        --CurrentLocation;
        --Tail.Overlay.CurrentStackLocation;
      }

      void set_completion_routine(
        io_completion_routine_t * completion_routine = 0,
        void *                    context         = 0,
        io_stack_location::control_flags  control = io_stack_location::control_flags()
        )
      {
        io_stack_location & stack = *get_next_stack_location();
        stack.CompletionRoutine = completion_routine;
        stack.Context = completion_routine ? context : 0;
        stack.Control = completion_routine ? static_cast<uint8_t>(control) : 0;
      }

      void * operator new(std::size_t, uint8_t StackSize, bool ChargeQuota = false) throw()
      {
        return allocate(StackSize, ChargeQuota);
      }

      void operator delete(void* p)
      {
        if ( p ) free(reinterpret_cast<irp *>(p));
      }

    private:
      static inline irp * allocate(uint8_t StackSize, bool ChargeQuota = false);
      static inline void free(irp * pirp);

    }; // struct irp


    NTL__EXTERNAPI
      irp * __stdcall
      IoAllocateIrp(
      uint32_t  StackSize,  ///\note original type is char
      bool      ChargeQuota
      );

    NTL__EXTERNAPI
      void __stdcall
      IoFreeIrp(
      irp * Irp
      );

    irp * 
      irp::allocate(uint8_t StackSize, bool ChargeQuota)
    {
      return IoAllocateIrp(StackSize, ChargeQuota);
    }

    void irp::free(irp * p)
    {
      IoFreeIrp(p);
    }

  }//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_IRP
