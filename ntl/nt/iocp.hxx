/**\file*********************************************************************
 *                                                                     \brief
 *  I/O Completion objects
 *
 ****************************************************************************
 */
#ifndef NTL__NT_IOCP
#define NTL__NT_IOCP

#include "handle.hxx"
#include "../stlx/chrono.hxx"

namespace ntl { namespace nt {

  enum io_completion_information_class;

  NTL__EXTERNAPI
    ntstatus __stdcall
    ZwCreateIoCompletion (
      handle* IoCompletionHandle,
      uint32_t DesiredAccess,
      const object_attributes* ObjectAttributes,
      uint32_t Count
      );

  NTL__EXTERNAPI
    ntstatus __stdcall
    ZwOpenIoCompletion (
      handle* IoCompletionHandle,
      uint32_t DesiredAccess,
      const object_attributes* ObjectAttributes
      );

  NTL__EXTERNAPI
    ntstatus __stdcall
    ZwQueryIoCompletion (
      legacy_handle IoCompletionHandle,
      io_completion_information_class IoCompletionInformationClass,
      void* IoCompletionInformation,
      uint32_t IoCompletionInformationLength,
      uint32_t* ReturnLength
      );
  
  NTL__EXTERNAPI
    ntstatus __stdcall
    ZwSetIoCompletion (
      legacy_handle IoCompletionHandle,
      const void* KeyContext,
      const void* ApcContext,
      ntstatus IoStatus,
      uintptr_t IoStatusInformation
      );

  NTL__EXTERNAPI
    ntstatus __stdcall
    ZwRemoveIoCompletion (
      legacy_handle IoCompletionHandle,
      const void** KeyContext,
      const void** ApcContext,
      io_status_block* IoStatusBlock,
      const int64_t* Timeout
      );
  
  enum io_completion_information_class {
    IoCompletionBasicInformation
  };

  struct io_completion_basic_information
  {
    int32_t Depth;
  };

  /** Contains information used in asynchronous (or %overlapped) input and output (I/O). */
  #pragma warning(disable:4201) // nameless union
  struct overlapped
  {
    uintptr_t Internal1, Internal2;
    union {
      uint64_t    Offset;
      void*       Pointer;
    };
    legacy_handle Event;
  };
  #pragma warning(default:4201)

  class io_completion_port;
} // nt

template<>
struct device_traits<nt::io_completion_port>:
  private device_traits<>
{
  enum access_mask {
    query_state = 1,
    modify_state,
    all_access = standard_rights_required|synchronize|query_state|modify_state
  };
  friend access_mask operator | (access_mask m, access_mask m2)     { return bitwise_or(m, m2); }
  friend access_mask operator | (access_mask m, nt::access_mask m2) { return m | static_cast<access_mask>(m2); }
  friend access_mask operator | (nt::access_mask m, access_mask m2) { return m2 | m; }
};

namespace nt
{
  /**
   *	@brief I/O Completion Port wrapper
   *  @details I/O completion ports are the mechanism by which an application uses a pool of threads that was created when the application was started to process asynchronous I/O requests.
   *  These threads are created for the sole purpose of processing I/O requests.
   *  Applications that process many concurrent asynchronous I/O requests can do so more quickly and efficiently by using I/O completion ports than by creating threads at the time of the I/O request.
   **/
  class io_completion_port:
    public handle,
    public device_traits<io_completion_port>
  {
  public:
    /** Creates an i/o completion port */
    explicit io_completion_port(uint32_t MaxThreadCount = 0, access_mask DesiredAccess = all_access)
    {
      ZwCreateIoCompletion(this, DesiredAccess, nullptr, MaxThreadCount);
    }

    /** Creates an i/o completion port */
    explicit io_completion_port(const object_attributes& ObjectAttributes, uint32_t MaxThreadCount = 0, access_mask DesiredAccess = all_access)
    {
      ZwCreateIoCompletion(this, DesiredAccess, &ObjectAttributes, MaxThreadCount);
    }

    /** Opens a completion port */
    explicit io_completion_port(access_mask DesiredAccess)
    {
      ZwOpenIoCompletion(this, DesiredAccess, nullptr);
    }

    /** Opens an completion port */
    explicit io_completion_port(const object_attributes& ObjectAttributes, access_mask DesiredAccess = modify_state)
    {
      ZwOpenIoCompletion(this, DesiredAccess, &ObjectAttributes);
    }

    /** Returns a depth of completion port. \c -1 indicates an error. */
    int32_t get_depth() const volatile
    {
      // NOTE: is KeReadStateQueue returns a depth? And what is depth?
      int32_t depth = -1;
      ZwQueryIoCompletion(get(), IoCompletionBasicInformation, &depth, sizeof(int32_t), nullptr);
      return depth;
    }

    /** Posts a context data with the default i/o status values to the io completion queue */
    ntstatus set_completion(const void* KeyContext, const void* ApcContext = 0) volatile
    {
      return ZwSetIoCompletion(get(), KeyContext, ApcContext, status::success, 0);
    }

    /** Posts i/o status to the io completion queue */
    ntstatus set_completion(const void* KeyContext, ntstatus IoStatus, uintptr_t IoStatusInformation = 0, const void* ApcContext = 0) volatile
    {
      return ZwSetIoCompletion(get(), KeyContext, ApcContext, IoStatus, IoStatusInformation);
    }

    /** Posts i/o status to the io completion queue */
    ntstatus set_completion(const void* KeyContext, const io_status_block& Iosb, const void* ApcContext = 0) volatile
    {
      return ZwSetIoCompletion(get(), KeyContext, ApcContext, Iosb.Status, Iosb.Information);
    }

    /** Removes a context data from the io completion queue (i/o status ignored). If queue is empty, then the calling thread waits for data. */
    ntstatus remove_completion(const void*& KeyContext, const void** ApcContext = 0) volatile
    {
      io_status_block iosb;
      return ZwRemoveIoCompletion(get(), &KeyContext, ApcContext, &iosb, nullptr);
    }

    /** Removes an i/o status data from the io completion queue. If queue is empty, then the calling thread waits for data. */
    ntstatus remove_completion(io_status_block& Iosb, const void*& KeyContext, const void** ApcContext = 0) volatile
    {
      return ZwRemoveIoCompletion(get(), &KeyContext, ApcContext, &Iosb, nullptr);
    }

    /** Removes a context data from the io completion queue (i/o status ignored). If queue is empty, then the calling thread waits for data during specified time. */
    template <class Rep, class Period>
    ntstatus remove_completion(const std::chrono::duration<Rep, Period>& rel_time, const void*& KeyContext, const void** ApcContext = 0) volatile
    {
      io_status_block iosb;
      return ZwRemoveIoCompletion(get(), &KeyContext, &ApcContext, &iosb, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
    }

    /** Removes an i/o status data from the io completion queue. If queue is empty, then the calling thread waits for data during specified time. */
    template <class Rep, class Period>
    ntstatus remove_completion(const std::chrono::duration<Rep, Period>& rel_time, io_status_block& Iosb, const void*& KeyContext, const void** ApcContext = 0) volatile
    {
      return ZwRemoveIoCompletion(get(), &KeyContext, &ApcContext, &Iosb, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
    }

    /// I/O Completion data type
    struct entry: io_status_block
    {
      const void *Key, *Apc;

      entry()
      {
        Key = Apc = 0;
        Pointer = 0; Information = 0;
      }

      explicit entry(const void* Key, const void* Apc = 0)
        :Key(Key), Apc(Apc)
      {
        Pointer = 0; Information = 0;
      }

      explicit entry(const void* Key, const io_status_block& Iosb, const void* Apc = 0)
        :Key(Key), Apc(Apc)
      {
        Pointer = Iosb.Pointer; Information = Iosb.Information;
      }
    };

    /** Enqueues data to the completion queue */
    ntstatus push_completion(const entry& data) volatile
    {
      return ZwSetIoCompletion(get(), data.Key, data.Apc, data.Status, data.Information);
    }

    /** Dequeues data from the completion queue. If queue is empty, calling thread waits for data. */
    ntstatus pop_completion(entry& data) volatile
    {
      return ZwRemoveIoCompletion(get(), &data.Key, &data.Apc, &data, nullptr);
    }

    /** Dequeues data from the completion queue. If queue is empty, calling thread waits for data during specified time. */
    template <class Rep, class Period>
    ntstatus pop_completion(entry& data, const std::chrono::duration<Rep, Period>& wait_time) volatile
    {
      return ZwRemoveIoCompletion(get(), &data.Key, &data.Apc, &data, -1i64*std::chrono::duration_cast<system_duration>(rel_time).count());
    }
  };

}}
#endif // NTL__NT_IOCP
