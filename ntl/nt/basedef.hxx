/**\file*********************************************************************
*                                                                     \brief
*  NT Types support library
*
****************************************************************************
*/

#ifndef NTL__NT_BASEDEF
#define NTL__NT_BASEDEF

#include "status.hxx"
#include "../basedef.hxx"
#include "../stdlib.hxx"
#include "string.hxx"


namespace ntl {

  namespace km {
    typedef uintptr_t kaffinity_t;
  }

  namespace pe {
    class image;
  }

  namespace nt {

    /**\addtogroup  native_types_support *** NT Types support library ***********
    *@{*/

    struct list_entry
    {
      union { list_entry * Flink; list_entry * next; };
      union { list_entry * Blink; list_entry * prev; };

      void link(list_entry * prev, list_entry * next)
      {
        this->prev = prev; this->next = next;
        prev->next = this; next->prev = this;
      }

      void unlink()
      {
        next->prev = prev;
        prev->next = next;
      }
    };


    struct list_head : public list_entry
    {
      list_head()
      {
        next = this;
        prev = this;
      }

      bool empty() const { return next == this; }

      void insert(list_entry * position, list_entry * entry)
      {
        entry->link(position, position->next);
      }

      list_entry * erase(list_entry * position)
      {
        list_entry * const next = position->next;
        position->unlink();
        return next;
      }

      void insert_tail(list_entry * entry)
      {
        entry->link(prev, this);
      }

      list_entry * begin() { return next; }
      list_entry * end()   { return this; }
      const list_entry * begin() const { return next; }
      const list_entry * end()   const { return this; }
      const list_entry * cbegin()const { return next; }
      const list_entry * cend()  const { return this; }
    };


    struct single_list_entry
    {
      single_list_entry * Next;

      single_list_entry * next() const { return Next; }
      void next(single_list_entry * p)  { Next = p; }
    };

    typedef single_list_entry slist_entry;

    __align(8)
    struct slist_header : public slist_entry
    {
      uint16_t    Depth;
      uint16_t    Sequence;
    };


    /// Masks for the predefined standard access types
    enum access_mask
    {
      no_access = 0,
      delete_access             = 0x00010000L,
      read_control              = 0x00020000L,
      write_dac                 = 0x00040000L,
      write_owner               = 0x00080000L,
      synchronize               = 0x00100000L,
      standard_rights_required  = delete_access | read_control | write_dac | write_owner,
      standard_rights_read      = read_control,
      standard_rights_write     = read_control,
      standard_rights_execute   = read_control,
      standard_rights_all       = standard_rights_required | synchronize,
      specific_rights_all       = 0x0000FFFFL,
      access_system_security    = 0x01000000L,
      maximum_allowed           = 0x02000000L,
      generic_read              = 0x80000000L,
      generic_write             = 0x40000000L,
      generic_execute           = 0x20000000L,
      generic_all               = 0x10000000L
    };

    static inline
      access_mask  operator | (access_mask m, access_mask m2)
    {
      return bitwise_or(m, m2);
    }

    static inline
      access_mask  operator & (access_mask m, access_mask m2)
    {
      return bitwise_and(m, m2);
    }

    struct generic_mapping
    {
      access_mask generic_read;
      access_mask generic_write;
      access_mask generic_execute;
      access_mask generic_all;
    };


    struct io_status_block
    {
      union { ntstatus  Status; void * Pointer; };
      uintptr_t Information;
    };


    typedef
      void __stdcall
      io_apc_routine(
        const void *            ApcContext,
        const io_status_block * IoStatusBlock,
        uint32_t                Reserved
        );


    /**@} native_types_support */

    enum system_power_state
    {
      PowerSystemUnspecified,
      PowerSystemWorking,
      PowerSystemSleeping1,
      PowerSystemSleeping2,
      PowerSystemSleeping3,
      PowerSystemHibernate,
      PowerSystemShutdown,
      PowerSystemMaximum
    };


    enum power_action
    {
      PowerActionNone,
      PowerActionReserved,
      PowerActionSleep,
      PowerActionHibernate,
      PowerActionShutdown,
      PowerActionShutdownReset,
      PowerActionShutdownOff,
      PowerActionWarmEject
    };

    enum device_power_state
    {
      PowerDeviceUnspecified,
      PowerDeviceD0,
      PowerDeviceD1,
      PowerDeviceD2,
      PowerDeviceD3,
      PowerDeviceMaximum
    };

    enum DllMainReason 
    {
      DllProcessDetach,
      DllProcessAttach,
      DllThreadAttach,
      DllThreadDetach
    };

    struct ldr_data_table_entry
    {
      enum flags {
        static_link                = 0x00000002,
        image_dll                  = 0x00000004,
        load_in_progress           = 0x00001000,
        unload_in_progress         = 0x00002000,
        entry_processed            = 0x00004000,
        entry_inserted             = 0x00008000,
        current_load               = 0x00010000,
        failed_builtin_load        = 0x00020000,
        dont_call_for_threads      = 0x00040000,
        process_attach_called      = 0x00080000,
        debug_symbols_loaded       = 0x00100000,
        image_not_at_base          = 0x00200000,
        wx86_ignore_machinetype    = 0x00400000
      };

      /* 0x00 */ list_head            InLoadOrderLinks;
      /* 0x08 */ list_head            InMemoryOrderLinks;
      /* 0x10 */ list_head            InInitializationOrderLinks;
      /* 0x18 */ pe::image *          DllBase;
      /* 0x1c */ void *               EntryPoint;
      /* 0x20 */ uint32_t             SizeOfImage;
      /* 0x24 */ const_unicode_string FullDllName;
      /* 0x2c */ const_unicode_string BaseDllName;
      /* 0x34 */ uint32_t             Flags;
      /* 0x38 */ uint16_t             LoadCount;
      /* 0x3a */ uint16_t             TlsIndex;
      union {
        /* 0x3c */ list_entry         HashLinks;
        struct {
          /* 0x3c */ void *           SectionPointer;
          /* 0x40 */ uint32_t         CheckSum;
        } ext;
      };
      union {
        /* 0x44 */ uint32_t           TimeDateStamp;
        /* 0x44 */ void *             LoadedImports;
      };
      /* 0x48 */ void *               EntryPointActivationContext;
      /* 0x4c */ void *               PatchInformation;

#if 0
      // nt 6.0+
      /*<thisrel this+0x50>*/ /*|0x8|*/ list_entry ForwarderLinks;
      /*<thisrel this+0x58>*/ /*|0x8|*/ list_entry ServiceTagLinks;
      /*<thisrel this+0x60>*/ /*|0x8|*/ list_entry StaticLinks;
      // // <size 0x68>
#endif


      struct find_dll
      {
        find_dll(list_head * head) : head(head) {}

        const pe::image * operator()(const char name[]) const
        {
          if (head)
            for (list_entry * it = head->begin(); it != head->end(); it = it->next)
            {
              const ldr_data_table_entry * const entry =
                reinterpret_cast<ldr_data_table_entry *>(it);
              if(!entry->BaseDllName.size())
                continue;
              for (uint16_t i = 0; i != entry->BaseDllName.size(); ++i) {
                if ((entry->BaseDllName[i] ^ name[i]) & 0x5F)
                  goto other_name;
              }
              return entry->DllBase;
other_name:;
            }
            return 0;
        }

    private:
      list_head * head;

      };

    };
    STATIC_ASSERT(sizeof(ldr_data_table_entry) == 0x50 || sizeof(ldr_data_table_entry) == 0x98);


NTL__EXTERNAPI
ntstatus __stdcall
  NtDelayExecution(
    bool            Alertable,
    const int64_t * DelayInterval
    );


template<times TimeResolution>
static inline
ntstatus sleep(
  uint32_t        time_resolution,
  bool            alertable = false)
{
  const int64_t interval = int64_t(-1) * TimeResolution * time_resolution;
  return NtDelayExecution(alertable, &interval);
}

/// default milliseconds
static inline
ntstatus sleep(
  uint32_t        ms,
  bool            alertable = false)
{
  const int64_t interval = int64_t(-1) * milliseconds * ms;
  return NtDelayExecution(alertable, &interval);
}


/// shared user data
static const size_t processor_feature_max = 64;
static const size_t max_wow64_shared_entries = 16;

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
    const kuser_shared_data* kusd = reinterpret_cast<const kuser_shared_data*>
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
  bool ProcessorFeatures[processor_feature_max];
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
  uint32_t Wow64SharedInformation[max_wow64_shared_entries];
  uint16_t UserModeGlobalLogger[8];
  uint32_t HeapTracingPid[2];
  uint32_t CritSecTracingPid[2];
  uint32_t ImageFileExecutionOptions;
  union {
    uint64_t    AffinityPad;
    km::kaffinity_t ActiveProcessorAffinity;
  };
  volatile uint64_t InterruptTimeBias;
};

#ifdef NTL__SUPPRESS_IMPORTS
__forceinline
  ntstatus NtQuerySystemTime(int64_t* SystemTime)
  {
#ifdef _M_IX86
    static const kuser_shared_data& usd = kuser_shared_data::instance();
    do{
      *(reinterpret_cast<uint32_t*>(SystemTime)+1) = usd.SystemTime.High1Time;
      *reinterpret_cast<uint32_t*>(SystemTime) = usd.SystemTime.LowPart;
    }while(*(reinterpret_cast<uint32_t*>(SystemTime)+1) != usd.SystemTime.High2Time);
#else
    *SystemTime = *reinterpret_cast<const volatile int64_t*>(&kuser_shared_data::instance().SystemTime);
#endif
    return status::success;
  }
#else
NTL__EXTERNAPI ntstatus __stdcall NtQuerySystemTime(int64_t* SystemTime);
#endif

NTL__EXTERNAPI
void __stdcall
  RtlTimeToTimeFields(int64_t* Time, time_fields* TimeFields);

NTL__EXTERNAPI
ntstatus __stdcall
  RtlSystemTimeToLocalTime(int64_t* SystemTime, int64_t* LocalTime);

/// rtl compression

namespace compression
{
  enum engines
  {
    engine_standart,
    engine_maximum   = 0x0100
  };

  enum formats
  {
    format_none,
    format_default,
    format_lznt1
  };

  //inline uint16_t operator|(formats f, engines e)
  //{
  //  return static_cast<uint16_t>( uint16_t(f) | uint16_t(e) );
  //}

  static const uint16_t default_format = engine_maximum|format_lznt1;

} // compression

NTL__EXTERNAPI
ntstatus __stdcall
  RtlGetCompressionWorkSpaceSize (
    uint16_t CompressionFormatAndEngine,
    uint32_t* CompressBufferWorkSpaceSize,
    uint32_t* CompressFragmentWorkSpaceSize
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlCompressBuffer (
    uint16_t CompressionFormatAndEngine,
    const void* UncompressedBuffer,
    uint32_t UncompressedBufferSize,
    void* CompressedBuffer,
    uint32_t CompressedBufferSize,
    uint32_t UncompressedChunkSize,
    uint32_t* FinalCompressedSize,
    void* WorkSpace
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlDecompressFragment (
    uint16_t CompressionFormat,
    void* UncompressedFragment,
    uint32_t UncompressedFragmentSize,
    const void* CompressedBuffer,
    uint32_t CompressedBufferSize,
    uint32_t FragmentOffset,
    uint32_t* FinalUncompressedSize,
    void* WorkSpace
    );

NTL__EXTERNAPI
ntstatus __stdcall
  RtlDecompressBuffer (
    uint16_t CompressionFormat,
    void* UncompressedBuffer,
    uint32_t UncompressedBufferSize,
    const void* CompressedBuffer,
    uint32_t CompressedBufferSize,
    uint32_t* FinalUncompressedSize
    );


  }//namespace nt
}//namespace ntl

#endif//#ifndef NTL__NT_BASEDEF
