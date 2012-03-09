/**\file*********************************************************************
  *                                                                     \brief
  *  I/O request packet
  *
  ****************************************************************************
  */
#ifndef NTL__KM_IRP
#define NTL__KM_IRP
#pragma once

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

    namespace priority_boost
    {
#if 1
      enum Increment: int8_t {
        event_increment = 1,
        io_no_increment = 0,
        io_cd_rom_increment = 1,
        io_disk_increment = 1,
        io_keyboard_increment = 6,
        io_mailslot_increment = 2,
        io_mouse_increment = 6,
        io_named_pipe_increment = 2,
        io_network_increment = 2,
        io_parallel_increment = 1,
        io_serial_increment = 2,
        io_sound_increment = 8,
        io_video_increment = 1,
        semaphore_increment = 1
      };
#else
      typedef int8_t Increment;
      static const Increment event_increment = 1;
      static const Increment io_no_increment = 0;
      static const Increment io_cd_rom_increment = 1;
      static const Increment io_disk_increment = 1;
      static const Increment io_keyboard_increment = 6;
      static const Increment io_mailslot_increment = 2;
      static const Increment io_mouse_increment = 6;
      static const Increment io_named_pipe_increment = 2;
      static const Increment io_network_increment = 2;
      static const Increment io_parallel_increment = 1;
      static const Increment io_serial_increment = 2;
      static const Increment io_sound_increment = 8;
      static const Increment io_video_increment = 1;
      static const Increment semaphore_increment = 1;
#endif
    }

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

    NTL_EXTERNAPI
    void __fastcall
      IofCompleteRequest(
        irp* Irp,
        int8_t PriorityBoost
        );

    NTL_EXTERNAPI
    void __stdcall
      IoSetCompletionRoutine(
        irp* Irp,
        io_completion_routine_t* CompletionRoutine,
        void* Context,
        bool InvokeOnSuccess,
        bool InvokeOnError,
        bool InvokeOnCancel
        );


    NTL_EXTERNAPI
    irp * __stdcall
      IoAllocateIrp(
        uint32_t  StackSize,  ///\note original type is char
        bool      ChargeQuota
        );

    NTL_EXTERNAPI
    void __stdcall IoFreeIrp(irp* Irp);

    NTL_EXTERNAPI void __stdcall IoReuseIrp(irp* Irp, ntstatus Iostatus);


#if defined(_M_IX86)
#   pragma pack(push, 4)
#elif defined(_M_X64)
#   pragma pack(push, 8)
#endif

    struct io_stack_location
    {
      enum control_flags: uint8_t
      {
        pending_returned  = 0x01,
        error_returned    = 0x02,
        invoke_on_cancel  = 0x20,
        invoke_on_success = 0x40,
        invoke_on_error   = 0x80,

        // lock
        fail_immediately             = 0x01,
        exclusive_lock               = 0x02,

        // create
        force_access_check           = 0x01,
        open_paging_file             = 0x02,
        open_target_directory        = 0x04,
        stop_on_symlink              = 0x08,
        case_sensitive               = 0x80,

        // read/write
        key_specified                = 0x01,
        override_verify_volume       = 0x02,
        write_through                = 0x04,
        ft_sequential_write          = 0x08,
        force_direct_write           = 0x10,
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
      enum major_function: uint8_t
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

      struct flags
      {
        enum type
        {
          nocache                     = 0x00000001,
          paging_io                   = 0x00000002,
          mount_completion            = 0x00000002,
          synchronous_api             = 0x00000004,
          associated_irp              = 0x00000008,
          buffered_io                 = 0x00000010,
          deallocate_buffer           = 0x00000020,
          input_operation             = 0x00000040,
          synchronous_paging_io       = 0x00000040,
          create_operation            = 0x00000080,
          read_operation              = 0x00000100,
          write_operation             = 0x00000200,
          close_operation             = 0x00000400,
          defer_io_completion         = 0x00000800,
          ob_query_name               = 0x00001000,
          hold_device_queue           = 0x00002000,

          // alternate allocation
          quota_charged               = 0x01,
          allocated_must_succeed      = 0x02,
          allocated_fixed_size        = 0x04,
          lookaside_allocation        = 0x08
        };
        friend type operator | (type m1, type m2)
        {
          return bitwise_or(m1, m2);
        }
      };

      uint16_t          Type;
      uint16_t          Size;
      mdl *             MdlAddress;
      flags::type       Flags;
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

      ntstatus complete_request(ntstatus Status, priority_boost::Increment PriorityBoost = priority_boost::io_no_increment)
      {
        IoStatus.Status = Status;
        IofCompleteRequest(this, PriorityBoost);
        return Status;
      }

      void complete_request(priority_boost::Increment PriorityBoost = priority_boost::io_no_increment)
      {
        IofCompleteRequest(this, PriorityBoost);
      }

      void reuse(ntstatus IoStatus = status::success)
      {
        IoReuseIrp(this, IoStatus);
      }

      void * operator new(std::size_t, uint8_t StackSize, bool ChargeQuota = false) __ntl_nothrow
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

    typedef irp::flags::type irp_flags;

    inline irp * irp::allocate(uint8_t StackSize, bool ChargeQuota)
    {
      return IoAllocateIrp(StackSize, ChargeQuota);
    }

    inline void irp::free(irp* p)
    {
      IoFreeIrp(p);
    }

  }//namspace km
}//namespace ntl


#endif//#ifndef NTL__KM_IRP
